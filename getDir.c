#include <math.h>
#include <pspgu.h>
#include <stdio.h>
#include <string.h>
#include <pspgum.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <malloc.h>
#include <psprtc.h>
#include <pspctrl.h>
#include <psputils.h>
#include <psppower.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspaudiolib.h>
#include <pspmscm.h>
#include <pspmoduleinfo.h>
#include <psputility.h>
#include <psputility_sysparam.h>
#include <pspreg.h>
#include "graphics.h"
#include "intraFont.h"
#include "mp3player.h"
#include "libio.h"
#include "main.h"
#include "converter.h"
#include "color.h"

SceIoDirent* getDir(char *DirName, int *count, SceIoDirent *DList) {
	static SceIoDirent	dir;
	int 	dfd,ret,DListMax;

	free(DList);
	dfd = sceIoDopen(DirName);
	DListMax = 1;
	if (dfd>=0){
		do{
			memset( &dir, 0, sizeof(dir) );
			ret = sceIoDread( dfd, &dir );
			if (strlen(dir.d_name)) DListMax++;					// エントリー数
		}while (ret>0);
		sceIoDclose(dfd);
	}
	DList = (SceIoDirent*) malloc(sizeof(SceIoDirent) * DListMax);	// メモリ確保
	if (!DList) return (NULL);
	DList[DListMax-1].d_name[0] = '\0';

	(*count) = 0;
	if (DListMax==1) return(0);

	dfd = sceIoDopen(DirName);
	do{
		DList[*count].d_name[0] = '\0';
		memset( &dir, 0, sizeof(dir) );							// メモリ初期化
		ret = sceIoDread( dfd, &dir );
		if (strlen(dir.d_name)){
			if (dir.d_stat.st_attr & FIO_SO_IFDIR){				// ディレクトリ
				if (strcmp(dir.d_name,".")!=0 && strcmp(dir.d_name,"..")!=0 ){
					strcpy( DList[*count].d_name, dir.d_name );
					DList[(*count)++].d_stat = dir.d_stat;
				}
			} else {											// ファイル
				strcpy( DList[*count].d_name, dir.d_name );
				DList[*count].d_stat = dir.d_stat;
				DList[(*count)++].d_stat.st_attr &= FIO_SO_IFREG;
			}
		}
	}while (*count<DListMax && ret>0);
	sceIoDclose(dfd);
	return (DList);
}