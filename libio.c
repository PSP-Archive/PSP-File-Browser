#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <pspsdk.h>
#include <pspiofilemgr.h>

#include "libio.h"



int GetDirFileCount(const char *dirname)
{
	unsigned int count = 0;

	int dfd = sceIoDopen(dirname);
	if(dfd <= 0){ sceIoDclose(dfd);  return -1;}

	SceIoDirent dir;
	memset(&dir, 0, sizeof(dir));

	while( sceIoDread(dfd, &dir) > 0 )
	{
		if( (dir.d_stat.st_attr & FIO_SO_IFDIR) && 
			( strcmp( dir.d_name, ".") == 0 || strcmp( dir.d_name, "..") == 0 ) )
			continue;
		count++;
	}

	sceIoDclose(dfd);
	return count;
}



int Remove(const char *filename)
{
	SceIoStat state;
	if( sceIoGetstat(filename, &state) < 0 )
		return -1;

	if( state.st_attr & FIO_SO_IFDIR )
		return Remove_Directory(filename);
	else
		return sceIoRemove(filename);
}

/*
 * 例：
 * Remove_Directory("ms0:/PSP/GAME");
 */
int Remove_Directory(const char *path)
{
	if(!path) return -1;

	int dfd = sceIoDopen(path);
	if(dfd < 0) {
		sceIoDclose(dfd);
		return -1;
	}
	
	int type = CheckPathType(path);

	int file_count = GetDirFileCount(path);
	if(file_count < 0)
	{
		sceIoDclose(dfd);
		return -1;
	}

	SceIoDirent dir;
	memset(&dir, 0, sizeof(dir));

	while( sceIoDread(dfd, &dir) > 0 )
	{
		if( (dir.d_stat.st_attr & FIO_SO_IFDIR) && 
			( strcmp( dir.d_name, ".") == 0 || strcmp( dir.d_name, "..") == 0 ) )
			continue;

		char* buf = NULL;
		if( type == PATH_LAST_IS_STR )
		{
			buf = (char*)malloc(sizeof(char) * (strlen(path) + strlen(dir.d_name) + 2));
			if(!buf){
				sceIoDclose(dfd);
				return -1;
			}
			strcpy(buf, path);
			strcat(buf, "/");
			strcat(buf, dir.d_name);
		}
		else if( type == PATH_LAST_IS_DIR )
		{
			buf = (char*)malloc(sizeof(char) * (strlen(path) + strlen(dir.d_name) + 1));
			if(!buf){
				sceIoDclose(dfd);
				return -1;
			}
			strcpy(buf, path);
			strcat(buf, dir.d_name);
		}

		if(dir.d_stat.st_attr & FIO_SO_IFDIR)
			Remove_Directory(buf);
		else
			sceIoRemove(buf);

		free(buf);
	}
	
	if( type == PATH_LAST_IS_DIR )
	{
		char str[ (strlen(path) +1) ];
		strcpy(str, path);
		char *ptr = strrchr(str,'/');
		*ptr = '\0';
		sceIoRmdir(str);
	}
	else
	{
		sceIoRmdir(path);
	}

	sceIoDclose(dfd);
	return 0;
}

/*
 * 例：
 * Copy_File("ms0:/PSP/GAME/UPDATE/EBOOT.PBP","ms0:/EBOOT.PBP");
 */
int Copy_File(const char *in, const char *out)
{
	if( strcmp(in,out) == 0 || in == NULL || out == NULL )
		return -1;

	SceUID fd_out = sceIoOpen( out, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC , 0777 );
	if (fd_out < 0){
		sceIoClose(fd_out);
		return -1;
	}
	SceUID fd_in = sceIoOpen( in , PSP_O_RDONLY, 0777);
	if (fd_in < 0){
		sceIoClose(fd_in);
		sceIoClose(fd_out);
		return -1;
	}
	
	unsigned char buffer[COPY_BUFFERSIZE];

	while(1)
	{
		int read_byte = sceIoRead(fd_in, buffer, COPY_BUFFERSIZE);
		if( read_byte < 0 ) {
			sceIoClose(fd_in);
			sceIoClose(fd_in);
			return -1;
		} else if(read_byte == 0 ) {
			break;
		} else if(read_byte == COPY_BUFFERSIZE ) {
			sceIoWrite(fd_out, buffer, COPY_BUFFERSIZE );
		} else {
			sceIoWrite(fd_out, buffer, read_byte);
			break;
		}
	}

	sceIoClose(fd_out);
	sceIoClose(fd_in);
	return 0;
}


/*
 * 例：
 * Copy_Directory("ms0:/PSP/GAME/UPDATE/","ms0:/");
 */
int Copy_Directory(const char* in, const char *out)
{
	if( strcmp(in,out) == 0 || in == NULL || out == NULL )
		return -1;

	int in_type, out_type;
	in_type = CheckPathType(in);
	out_type = CheckPathType(out);

	//出力フォルダ名を取得
	char folder[256];
	if( in_type == PATH_LAST_IS_STR )
	{
		char *ptr = strrchr(in,'/');
		strcpy(folder, ++ptr);
	}
	else if( in_type == PATH_LAST_IS_DIR )
	{
		char *str = (char*)malloc( sizeof(char) * (strlen(in) + 1) );
		strcpy(str, in);
		char *ptr = strrchr(str,'/');
		*ptr = '\0';
		ptr = strrchr(str,'/');
		strcpy(folder, ++ptr);
	}
	
	//出力先のパス+出力フォルダを合わせたパスを取得
	char *filename = NULL;
	if( out_type == PATH_LAST_IS_STR )
	{
		filename = (char*)malloc( sizeof(char) * (strlen(out) + strlen(folder) + 2) );
		strcpy(filename, out);
		strcat(filename, "/");
		strcat(filename, folder);
	}
	else if( out_type == PATH_LAST_IS_DIR )
	{
		filename = (char*)malloc( sizeof(char) * (strlen(out) + strlen(folder) + 1) );
		strcpy(filename, out);
		strcat(filename, folder);
	}

	//同じフォルダが存在していたら終了
	SceUID dfd = sceIoDopen(filename);
	if(dfd > 0){
		sceIoDclose(dfd);
		free(filename);
		return -2;
	}

	//フォルダ名のファイルを作成
	if( sceIoMkdir(filename, 0777) < 0 ){
		sceIoDclose(dfd);
		free(filename);
		return -1;
	}

	//コピー元のディレクトリオープン
	dfd = sceIoDopen(in);
	if(dfd < 0) {
		sceIoDclose(dfd);
		free(filename);
		return -1;
	}

	//コピー元のフォルダ中のファイル名を調べてファイルをコピーする
	SceIoDirent dir;
	memset(&dir, 0, sizeof(dir));
	while( sceIoDread(dfd, &dir) > 0 )
	{
		if( (dir.d_stat.st_attr & FIO_SO_IFDIR) && 
			( strcmp( dir.d_name, ".") == 0 || strcmp( dir.d_name, "..") == 0 ) )
			continue;

		//コピー元のファイル（フォルダ）名取得
		char *in_buf = NULL;
		if( in_type == PATH_LAST_IS_STR )
		{
			in_buf = (char*)malloc(sizeof(char) * (strlen(in) + strlen(dir.d_name) + 2));
			if(!in_buf){
				free(filename);
				sceIoDclose(dfd);
				return -1;
			}
			strcpy(in_buf, in);
			strcat(in_buf, "/");
			strcat(in_buf, dir.d_name);
		}
		else if( in_type == PATH_LAST_IS_DIR )
		{
			in_buf = (char*)malloc(sizeof(char) * (strlen(in) + strlen(dir.d_name) + 1));
			if(!in_buf){
				free(filename);
				sceIoDclose(dfd);
				return -1;
			}
			strcpy(in_buf, in);
			strcat(in_buf, dir.d_name);
		}
		//コピー先のファイル（フォルダ）名取得
		char *out_buf = (char*)malloc(sizeof(char) * (strlen(filename) + strlen(dir.d_name) + 2));
		if(!out_buf){
			free(in_buf);
			free(filename);
			sceIoDclose(dfd);
			return -1;
		}
		strcpy(out_buf, filename);
		strcat(out_buf, "/");
		strcat(out_buf, dir.d_name);

		//フォルダ
		if(dir.d_stat.st_attr & FIO_SO_IFDIR)
		{
			char *p = strrchr(out_buf,'/');
			*p = '\0';
			Copy_Directory(in_buf, out_buf);
		}
		//ファイル
		else
		{
			Copy_File(in_buf, out_buf);
		}

		free(in_buf);
		free(out_buf);
	}
	sceIoDclose(dfd);
	free(filename);

	return 0;
}

int Copy(const char *in, const char *out)
{
	SceIoStat state;
	if( sceIoGetstat(in, &state) < 0 )
		return -1;

	if( state.st_attr & FIO_SO_IFDIR )
		return Copy_Directory(in, out);
	else
		return Copy_File(in, out);
}

int CheckPathType(const char* path)
{
	int len = strlen(path);
	if( path[len-1] == '/' )
		return 1;
	
	return 0;
}

int FlashWriteEnable(void)
{
	u32 res = sceIoUnassign("flash0:");
	if(res<0) return -1;
	res = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
	if(res<0) return -1;

	return 0;
}

int FlashProtect(void)
{
	u32 res = sceIoUnassign("flash0:");
	if(res<0) return -1;
	res = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDONLY, NULL, 0);
	if(res<0) return -1;

	return 0;
}

/*


typedef struct
{
	unsigned long clusters_max;
	unsigned long clusters_free;
	int sector_max;
	int sector_size;
	int sector_count;
}Ms_Disk_Info;


int GetMsInfo(unsigned int *max, unsigned int *free, unsigned int *used)
{
	Ms_Disk_Info info;
	memset(&info,0,sizeof(info));

	if( sceIoDevctl("ms0:", 0x02425818, &info, 4, NULL, 0) < 0)
		return -1;

	unsigned int smax  = (info.clusters_max  * info.sector_count * info.sector_size);
	unsigned int sfree = (info.clusters_free * info.sector_count * info.sector_size);
	unsigned int sused = (smax - sfree);

	*max = smax;
	*free = sfree;
	*used = sused;

	return 0;
}
*/
#define LOG_DEFAULT_NAME		"ms0:/log.txt"
//DXPから拝借しました
int Log(const char *filename, const char *format,...)
{
	va_list args;
	va_start(args,format);
	if(!format) return -1;
	FILE *fp = NULL;
	if(!filename)
		fp = fopen(LOG_DEFAULT_NAME,"a");
	else
		fp = fopen(filename,"a");
	if(!fp) return -1;
	vfprintf(fp,format,args);
	//fprintf(fp,"\r\n");
	fclose(fp);
	va_end(args);
	return 0;
}