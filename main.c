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
#include "osk.h"

#include "systemctrl.h"
#include "kubridge.h"

#include "draw.h"
#include "intraFontJP.h"
#include "TextViewer.h"
#include "SystemViewer.h"
#include "CpuClockChanger.h"
#include "Error.h"
#include "Readme.h"
#include "FileMenuAction.h"
#include "getDir.h"
#include "Property.h"

#include "Splash.c"

PSP_MODULE_INFO("PSPFileBrowser", 0, 1, 00);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

SceCtrlData pad;
SceOff size;
char dirname[1024], *tmp;
char dirfile[1024], *tmp;
char file[1024];
char sub_file[1024];
char filename[1024];
char filepath[1024], *tmp;
char utf8[1024];
int i,		//ループカウンタ
	flag,	//ボタンフラグ
	fname,	//ファイル番号
	status,	//スレッド・メモリースティック状態
	length,	//ファイルなどの長さ
	page,	//表示中のページ
	state,		//メニュー状態判断
	sub_state,	//予備の状態判断
	sx,		//画像表示x座標
	sy,		//画像表示y座標
	page_count,	//現在のページ
	page_max,	//ページ数
	sub_fname,	//ファイル番号
	sub_length,	//ファイルなどの長さ
	sub_page,	//表示中のページ
	sub_page_count,	//現在のページ
	sub_page_max;	//ページ数
			//sub_xxxはコピー・切り取り時のファイルリスト時に使用
DiskInfo MS;

int MSCallback(int arg1, int arg2, void *common) {
	status = 1;
	return 0;
}

int MSCallbackThread(SceSize args, void *argp) {
	int cbid = sceKernelCreateCallback("MSCallback", MSCallback, NULL);
	MScmRegisterMSInsertEjectCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int exit_callback(int arg1, int arg2, void *common) {
	running = 0;
	return 0;
}

int CallbackThread(SceSize args, void *argp) {
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int SetupCallbacks(void) {
	int thid = sceKernelCreateThread("CallbackThread", CallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if (thid >= 0) sceKernelStartThread(thid, 0, 0);
	return thid;
}

int drawImage(int *fname, int *entry, SceIoDirent *dir, int mode) {
	flag = 1;
	sx = 0; sy = 0;
	static int menu = 1;
	Image *Img = NULL;
	tmp = strrchr(dir[*fname].d_name, '.');
	if(IS_PNG)Img = loadImage(dir[*fname].d_name);

	if(Img == NULL && mode == 1){
		while(running){
			if(*fname < *entry-1){
				(*fname)++;
				page_count = *fname / 12;
				tmp = strrchr(dir[*fname].d_name, '.');
				if(IS_PNG)Img = loadImage(dir[*fname].d_name);
				if(Img != NULL)break;
			}
			else{
				printError(ERROR_NO_AFTER_IMG);
				break;
			}
		}
	}
	else if(Img == NULL && mode == -1){
		while(running){
			if(*fname > 0){
				(*fname)--;
				page_count = *fname / 12;
				tmp = strrchr(dir[*fname].d_name, '.');
				if(IS_PNG)Img = loadImage(dir[*fname].d_name);
				if(Img != NULL)break;
			}
			else{
				printError(ERROR_NO_BEFORE_IMG);
				break;
			}
		}
	}
	if(Img != NULL){
		while(running){
			clearScreen(LITEGRAY);
			if(Img->imageWidth <= SCREEN_WIDTH && Img->imageHeight <= SCREEN_HEIGHT)
				blitImageToScreen(0, 0, Img->imageWidth, Img->imageHeight, Img, 0, 0);
			else if(Img->imageWidth <= SCREEN_WIDTH)
				blitImageToScreen(sx, sy, Img->imageWidth, SCREEN_HEIGHT, Img, 0, 0);
			else if(Img->imageHeight <= SCREEN_HEIGHT)
				blitImageToScreen(sx, sy, SCREEN_WIDTH, Img->imageHeight, Img, 0, 0);
			else
				blitImageToScreen(sx, sy, SCREEN_WIDTH, SCREEN_HEIGHT, Img, 0, 0);
			
			guStart();
			if(menu) {
				DrawAlphaFillBoxScreen(0x88, GRAY, 0, 0, 480, 16);
				intraFontSetStyle(jpn0, 0.6f, WHITE, DARKGRAY, INTRAFONT_ALIGN_LEFT);
				intraFontPrintfJP(10, 12, "%s", dir[*fname].d_name);
				intraFontSetStyle(jpn0, 0.6f, WHITE, DARKGRAY, INTRAFONT_ALIGN_CENTER);
				intraFontPrintJP(SCREEN_WIDTH/2, 268, "L 前　　×戻る　　△メニュー　　R 次");
			}
			
			sceGuFinish();
			sceGuSync(0,0);
			sceDisplayWaitVblankStart();
			flipScreen();

			sceCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons != 0){
				if(pad.Buttons & PSP_CTRL_CROSS){
					freeImage(Img);
					mode = 0;
					break;
				}
				if(pad.Buttons & PSP_CTRL_TRIANGLE){
					if(!flag)menu = !menu;
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_LTRIGGER){
					if(!flag){
						if(*fname > 0){
							freeImage(Img);
							mode = -1;
							break;
						}
						else printError(ERROR_NO_BEFORE_IMG);
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_RTRIGGER){
					if(!flag){
						if(*fname < *entry-1){
							freeImage(Img);
							mode = 1;
							break;
						}
						else printError(ERROR_NO_AFTER_IMG);
					}
					flag = 1;
				}
			}
			else flag = 0;
			if((85<=pad.Lx)&&(pad.Lx<=169)&&(0<=pad.Ly)&&(pad.Ly<=84)){
				if(sy > 0)sy-=8;
			}
			else if((85<=pad.Lx)&&(pad.Lx<=169)&&(170<=pad.Ly)&&(pad.Ly<=255)){
				if(sy < Img->imageHeight - SCREEN_HEIGHT)sy+=8;
			}
			else if((0<=pad.Lx)&&(pad.Lx<=84)&&(85<=pad.Ly)&&(pad.Ly<=169)){
				if(sx > 0)sx-=8;
			}
			else if((170<=pad.Lx)&&(pad.Lx<=255)&&(85<=pad.Ly)&&(pad.Ly<=169)){
				if(sx < Img->imageWidth - SCREEN_WIDTH)sx+=8;
			}

		}
	}
	else if(mode == 0){
		printError(ERROR_UNSUPPORTED);
		mode = 0;
	}
	else mode = 0;

	if(!running)mode = 0;
	if(mode == 1){
		(*fname)++;
		page_count = *fname / 12;
		drawImage(fname, entry, dir, 1);
		return 0;
	}
	else if(mode == -1){
		(*fname)--;
		page_count = *fname / 12;
		drawImage(fname, entry, dir, -1);
		return 0;
	}
	else return 0;
}

int playMP3(int *fname, int *entry, SceIoDirent *dir, int mode, Image* Background) {
	flag = 1;
	char Time[] = "00:00:00";
	static int menu = 1;
	int MP3 = 0;
	MP3_Init(2);
	tmp = strrchr(dir[*fname].d_name, '.');
	if(IS_MP3){
		MP3_Load(dir[*fname].d_name);
		MP3 = 1;
	}

	if(MP3 == 0 && mode == 1){
		while(running){
			if(*fname < *entry-1){
				(*fname)++;
				page_count = *fname / 12;
				tmp = strrchr(dir[*fname].d_name, '.');
				if(IS_MP3){
					MP3_Load(dir[*fname].d_name);
					break;
				}
			}
			else{
				printError(ERROR_NO_AFTER_MP3);
				break;
			}
		}
	}
	else if(MP3 == 0 && mode == -1){
		while(running){
			if(*fname > 0){
				(*fname)--;
				page_count = *fname / 12;
				tmp = strrchr(dir[*fname].d_name, '.');
				if(IS_MP3){
					MP3_Load(dir[*fname].d_name);
					break;
				}
			}
			else{
				printError(ERROR_NO_BEFORE_MP3);
				break;
			}
		}
	}
	if(MP3 != 0){
		MP3_Play();
		while(running){
			clearScreen(BLACK);

			if(Background!=NULL)blitImageToScreen(0,0,480,272,Background,0,0);

			guStart();
			MP3_GetTimeString(Time);
			if(menu) {
				DrawAlphaFillBoxScreen(0x88, GRAY, 0, 0, 480, 16);
				intraFontSetStyle(jpn0, 0.6f, WHITE, DARKGRAY, INTRAFONT_ALIGN_LEFT);
				intraFontPrintfJP(10, 12, "%s", dir[*fname].d_name);
				intraFontSetStyle(jpn0, 0.6f, WHITE, DARKGRAY, INTRAFONT_ALIGN_CENTER);
				intraFontPrintJP(SCREEN_WIDTH/2, 268, "L前　　START一時停止　　×戻る　　△メニュー　　R次");
			}
			DrawAlphaFillBoxScreen(0x88, GRAY, 30, 46, 180, 180);
			DrawAlphaBoxScreen(0x88, WHITE, 30, 46, 180, 180);
			intraFontSetStyle(jpn0, 0.6f, WHITE, DARKGRAY, INTRAFONT_ALIGN_CENTER);
			intraFontPrintJP(SCREEN_WIDTH/2+120, 200, Time);

			sceGuFinish();
			sceGuSync(0,0);
			sceDisplayWaitVblankStart();
			flipScreen();

			sceCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons != 0){
				if(pad.Buttons & PSP_CTRL_CROSS){
					MP3_End();
					mode = 0;
					break;
				}
				if(pad.Buttons & PSP_CTRL_TRIANGLE){
					if(!flag)menu = !menu;
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_START){
					if(!flag){
						MP3_Pause();
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_LTRIGGER){
					if(!flag){
						if(*fname > 0){
							MP3_End();
							mode = -1;
							break;
						}
						else printError(ERROR_NO_BEFORE_MP3);
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_RTRIGGER){
					if(!flag){
						if(*fname < *entry-1){
							MP3_End();
							mode = 1;
							break;
						}
						else printError(ERROR_NO_AFTER_MP3);
					}
					flag = 1;
				}
			}
			else flag = 0;
			if(MP3_EndOfStream() == 1){
				if(*fname < *entry-1){
					MP3_End();
					mode = 1;
					break;
				}
				else{
					MP3_End();
					mode = 0;
					break;
				}
			}
		}
	}
	else if(mode == 0){
		mode = 0;
	}
	else mode = 0;

	if(!running)mode = 0;
	if(mode == 1){
		(*fname)++;
		page_count = *fname / 12;
		playMP3(fname, entry, dir, 1, Background);
		return 0;
	}
	else if(mode == -1){
		(*fname)--;
		page_count = *fname / 12;
		playMP3(fname, entry, dir, -1, Background);
		return 0;
	}
	else return 0;
}

int getDiskInfo(DiskInfo *info) {
	if(sceIoDevctl("ms0:", 0x02425818, &info, 4, NULL, 0) >= 0){
		info->smax = (info->cluster_max  * info->sector_count) * info->sector_size;
		info->sfree = (info->cluster_free * info->sector_count) * info->sector_size;
		info->sused = info->smax - info->sfree;
		return 0;
	}
	else return -1;
}

int printDiskInfo() {
	memset(&MS, 0, sizeof(MS));
	if(getDiskInfo(&MS) >= 0){
		MS.smax = (MS.cluster_max  * MS.sector_count) * MS.sector_size;
		MS.sfree = (MS.cluster_free * MS.sector_count) * MS.sector_size;
		MS.sused = MS.smax - MS.sfree;
		while(running){
			ScreenPaste();

			guStart();
			DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
			DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

			intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
			intraFontPrintJP(240, (15+272/8), "PSP File Browser - Disk Viewer");
			intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);

			//グラフの描画
			DrawAlphaBoxScreen(0x88, WHITE, 180, 60, 202, 22);
			if(MS.smax*0.9 <= MS.sused) {
				DrawAlphaFillBoxScreen(0x88, LITERED, 181, 61, (double)(MS.sused)/(double)MS.smax*200, 10);
				DrawAlphaFillBoxScreen(0x88, RED, 181, 71, (double)(MS.sused)/(double)MS.smax*200, 11);
				DrawAlphaFillBoxScreen(0x88, HIGHLITEGRAY, 181 + (double)(MS.sused)/(double)MS.smax*200, 61, 202 - (double)(MS.sused)/(double)MS.smax*200, 10);
				DrawAlphaFillBoxScreen(0x88, LITEGRAY, 181 + (double)(MS.sused)/(double)MS.smax*200, 71, 202 - (double)(MS.sused)/(double)MS.smax*200, 11);
			}
			else {
				DrawAlphaFillBoxScreen(0x88, LITEBLUE, 181, 61, (double)(MS.sused)/(double)MS.smax*200, 10);
				DrawAlphaFillBoxScreen(0x88, BLUE, 181, 71, (double)(MS.sused)/(double)MS.smax*200, 11);
				DrawAlphaFillBoxScreen(0x88, HIGHLITEGRAY, 181 + (double)(MS.sused)/(double)MS.smax*200, 61, 202 - (double)(MS.sused)/(double)MS.smax*200, 10);
				DrawAlphaFillBoxScreen(0x88, LITEGRAY, 181 + (double)(MS.sused)/(double)MS.smax*200, 71, 202 - (double)(MS.sused)/(double)MS.smax*200, 11);
			}

			//簡易情報
			intraFontPrintJP((480/8+10), 80, "メモリースティック");
			intraFontPrintfJP((480/8+120), 100, "空き容量：%llu MB / %llu MB", MS.sfree/1024/1024, MS.smax/1024/1024);

			//詳細情報
			intraFontPrintJP((480/8+10), (1*15+75+272/8), "総クラスタ数");
			intraFontPrintfJP((480/8+150), (1*15+75+272/8), "：%d", MS.cluster_max);
			intraFontPrintJP((480/8+10), (2*15+75+272/8), "空きクラスタ数");
			intraFontPrintfJP((480/8+150), (2*15+75+272/8), "：%d", MS.cluster_free);
			intraFontPrintJP((480/8+10), (3*15+75+272/8), "クラスタあたりのセクタ数");
			intraFontPrintfJP((480/8+150), (3*15+75+272/8), "：%d", MS.sector_count);
			intraFontPrintJP((480/8+10), (4*15+75+272/8), "セクタあたりのバイト数");
			intraFontPrintfJP((480/8+150), (4*15+75+272/8), "：%d", MS.sector_size);
			intraFontPrintJP((480/8+10), (5*15+75+272/8), "クラスタあたりのバイト数");
			intraFontPrintfJP((480/8+150), (5*15+75+272/8), "：%d", MS.sector_size * MS.sector_count);

			intraFontSetStyle(jpn0, 0.6f,WHITE,DARKGRAY,INTRAFONT_ALIGN_CENTER);
			intraFontPrintJP(240, 232, "×戻る");

			sceGuFinish();
			sceGuSync(0,0);

			sceDisplayWaitVblankStart();
			flipScreen();
			sceCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_CROSS){
				if(!flag)break;
			}
			else flag = 0;
			if(status == 1)break;
		}
		return 0;
	}
	else return 0;
}

int loadFonts(SceSize args, void *argp) {
	char file[40];
	for (i = 0; i < 8; i++) {
		sprintf(file, "flash0:/font/ltn%d.pgf", i); 
		ltn[i] = intraFontLoad(file,0);
		intraFontSetStyle(ltn[i], 0.5f, DARKGRAY, WHITE, 0);
	}
	jpn0 = intraFontLoad("flash0:/font/jpn0.pgf",INTRAFONT_STRING_UTF8 | INTRAFONT_CACHE_ALL);
	intraFontSetStyle(jpn0, 0.5f, DARKGRAY, WHITE, 0);
	sceKernelExitThread(0);
	return 0;
}


int main(int argc, char *argv[]) {
	SetupCallbacks();
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	intraFontInit();

	int LoadThread = sceKernelCreateThread("Load", loadFonts, 0x32, 0xFA0, 0, 0);
	if(LoadThread >= 0) sceKernelStartThread(LoadThread, 0, 0);

	int written;
	SceUID fd;

	fd = sceIoOpen("ms0:/Splash.png", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC , 0777);

	written = sceIoWrite(fd, Splash_Data, Size_Splash_Data);

	sceIoClose(fd);

	Image* Splash = loadImage("ms0:/Splash.png");

	Image* Background = loadImage("Background.png");

	initGraphics();

	if(Splash!=NULL) {
		while(running) {
			clearScreen(BLACK);
			status = sceKernelGetThreadExitStatus(LoadThread);
			blitImageToScreen(0,0,480,272,Splash,0,0);
			sceGuFinish();
			sceGuSync(0,0);
			sceDisplayWaitVblankStart();
			flipScreen();
			if(status >= 0){
				sceKernelDelayThread(500000);	//0.5秒待機
				break;
			}
		}
	}

	else {
		while(running) {
			clearScreen(BLACK);
			guStart();
			status = sceKernelGetThreadExitStatus(LoadThread);
			sceGuFinish();
			sceGuSync(0,0);
			sceDisplayWaitVblankStart();
			flipScreen();
			if(status >= 0){
				sceKernelDelayThread(500000);	//0.5秒待機
				break;
			}
		}
	}

	if(Splash!=NULL) freeImage(Splash);

	sceIoRemove("ms0:/Splash.png");

	SceIoDirent *dir;
	int entry = 0;
	strcpy(dirname, "ms0:/");
	dir = getDir(dirname, &entry, NULL);
	if(entry < 20)page_max = 0;
	else{
		page_max = entry/20-1;
		if(entry%20 != 0)page_max++;
	}
	page_count = 0;
	sceIoChdir(dirname);
	pspAudioInit();
	MP3_Init(2);

	int thid = sceKernelCreateThread("MSCallbackThread", MSCallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if (thid >= 0) sceKernelStartThread(thid, 0, 0);
	status = 0;

	while(running){
		if(status == 1){
			strcpy(dirname, "ms0:/");
			dir = getDir(dirname, &entry, NULL);
			sceIoChdir(dirname);
			if(entry < 20)page_max = 0;
			else{
				page_max = entry/20-1;
				if(entry%20 != 0)page_max++;
			}
			page_count = 0;
			fname = 0;
			status = 0;
		}

		clearScreen(BLACK);

		if(Background!=NULL)blitImageToScreen(0,0,480,272,Background,0,0);

		guStart();

		pspTime time;
		sceRtcGetCurrentClockLocalTime(&time);
		char timeBuffer[22];
		sprintf(timeBuffer, "%d/%02d/%02d %02d:%02d %d%%", time.year, time.month, time.day, time.hour, time.minutes, scePowerGetBatteryLifePercent());

		float y = 42;
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(240, 15, "PSP File Browser");
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
		intraFontPrintfJP(5, 15, "%d/%dページ", page_count + 1, page_max + 1);
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_RIGHT);
		intraFontPrintfJP(475, 15, "%s", timeBuffer);
		intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
		intraFontPrintJP(5, 30, dirname);

		DrawAlphaFillBoxScreen(0x88,WHITE, 0, 12 * (fname + 1 - 20 * page_count) + 22, 480, 10);

		if(entry<20){
			for(i = 0; i<entry; i++){
				if(dir[i].d_stat.st_attr & FIO_SO_IFDIR){
						intraFontSetStyle(jpn0, 0.5f, WHITE, DARKGRAY, 0);
						intraFontPrintfJP(5, y, "%s/", dir[i].d_name);
				}
				else intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
				intraFontPrintJP(5, y, dir[i].d_name);
				y+=12;
			}
		}
		else{
			for(i = 0; i<(page_count<page_max ? 20 : entry-20*page_count); i++){
				if(dir[i + 20 * page_count].d_stat.st_attr & FIO_SO_IFDIR){
					intraFontSetStyle(jpn0, 0.5f, WHITE, DARKGRAY, 0);
					intraFontPrintfJP(5, y, "%s/", dir[i + 12 * page_count].d_name);
				}
				else intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
				intraFontPrintJP(5, y, dir[i + 12 * page_count].d_name);
				y+=12;
			}
		}

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();

		ScreenCopy(0);

		sceCtrlReadBufferPositive(&pad, 1);
		if(MScmIsMediumInserted()){
			if(pad.Buttons != 0){
				if(pad.Buttons & PSP_CTRL_DOWN){
					if(!flag && fname < entry -1){
						if((fname+1)%20 != 0)fname++;
						else if(page_count < page_max){
							page_count++;
							fname++;
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_UP){
					if(!flag && fname > 0){
						if(fname%20 != 0)fname--;
						else if(page_count > 0){
							page_count--;
							fname--;
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_CROSS){
					if(!flag){
						if(strchr(dirname, '/') != strrchr(dirname, '/')){
							tmp = strrchr(dirname, '/');
							length = strlen(dirname);
							dirname[length-1] = '\0';
							tmp = strrchr(dirname, '/');
							tmp[1] = '\0';
							dir = getDir(dirname, &entry, dir);
							if(entry < 20)page_max = 0;
							else{
								page_max = entry/20-1;
								if(entry%20 != 0)page_max++;
							}
							page_count = 0;
							sceIoChdir(dirname);
							fname = 0;
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_CIRCLE){
					if(!flag && entry > 0){
						tmp = strrchr(dir[fname].d_name, '.');
						if(dir[fname].d_stat.st_attr & FIO_SO_IFDIR){
							sprintf(dirname, "%s%s/", dirname, dir[fname].d_name);
							dir = getDir(dirname, &entry, dir);
							sceIoChdir(dirname);
							if(entry < 20)page_max = 0;
							else{
								page_max = entry/20-1;
								if(entry%20 != 0)page_max++;
							}
							page_count = 0;
							fname = 0;
						}
						else if(tmp != NULL){
							if(stricmp(tmp,".txt") == 0){
								viewtext(&fname, &entry, dir, Background);
							}
							else if(stricmp(tmp,".db") == 0){
								viewtext(&fname, &entry, dir, Background);
							}
							else if(stricmp(tmp,".ini") == 0){
								viewtext(&fname, &entry, dir, Background);
							}
							else if(stricmp(tmp,".mp3") == 0){
								playMP3(&fname, &entry, dir, 0, Background);
							}
							else if(stricmp(tmp,".png") == 0){
								chdir(dirname);
								drawImage(&fname, &entry, dir, 0);
							}
							else {
								int select = 1;
								int flag_ex = 1;
								char message[1024] = "";
								float scroll = 69;
								while(running){
									if(state){
										state = 0;
										break;
									}
									if(flag){
										break;
									}

									ScreenPaste();
									guStart();

									DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
									DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
									DrawAlphaFillBoxScreen(0xAA, GRAY, (480/8+10), (select*15+5+272/8), 340,12);

									sprintf(message, "%sを開くプログラムを選択してください。", dir[fname].d_name);
									Sjis2UTF8(utf8, message);
									intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_SCROLL_LEFT);
									scroll = intraFontPrintColumn(jpn0, scroll, (15+272/8), 340, utf8);
									intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
									intraFontPrintJP((480/8+10), (1*15+15+272/8), "Text Viewer");
									intraFontPrintJP((480/8+10), (2*15+15+272/8), "Image Viewer");
									intraFontPrintJP((480/8+10), (3*15+15+272/8), "Music Player");
									intraFontPrintJP((480/8+10), (4*15+15+272/8), "戻る");

									sceGuFinish();
									sceGuSync(0,0);

									sceDisplayWaitVblankStart();
									flipScreen();
									sceCtrlReadBufferPositive(&pad, 1);
									if(pad.Buttons != 0){
										if(pad.Buttons & PSP_CTRL_UP){
											if(!flag_ex && select > 1){
												select--;
											}
											flag_ex = 1;
										}
										if (pad.Buttons & PSP_CTRL_DOWN){
											if(!flag_ex && select < 4){
												select++;
											}
											flag_ex = 1;
										}
										if(pad.Buttons & PSP_CTRL_CIRCLE){
											if(!flag_ex && select == 1){
												viewtext(&fname, &entry, dir, Background);
											}
											if(!flag_ex && select == 2){
												chdir(dirname);
												drawImage(&fname, &entry, dir, 0);
											}
											if(!flag_ex && select == 3){
												playMP3(&fname, &entry, dir, 0, Background);
											}
											if(!flag_ex && select == 4){
												break;
											}
										}
										if(pad.Buttons & PSP_CTRL_CROSS){
											if(!flag){
												select = 4;
											}
											flag = 1;
										}
									}
									else flag_ex = 0;
								}
							}
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_SQUARE){
					int select = 1;
					while(running){
						if(state){
							state = 0;
							break;
						}

						ScreenPaste();

						guStart();

						DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
						DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
						DrawAlphaFillBoxScreen(0xAA, GRAY, (480/8+10), (select*15+5+272/8), 340,12);

						intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
						intraFontPrintJP(240, (15+272/8), "PSP File Browser - File Menu");
						intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
						intraFontPrintJP((480/8+10), (1*15+15+272/8), "コピー");
						intraFontPrintJP((480/8+10), (2*15+15+272/8), "切り取り・貼\り付け");
						intraFontPrintJP((480/8+10), (3*15+15+272/8), "削除");
						intraFontPrintJP((480/8+10), (4*15+15+272/8), "リネーム");
						intraFontPrintJP((480/8+10), (5*15+15+272/8), "フォルダ作成");
						intraFontPrintJP((480/8+10), (6*15+15+272/8), "ファイル作成");
						intraFontPrintJP((480/8+10), (7*15+15+272/8), "プロパティ");			
						intraFontPrintJP((480/8+10), (8*15+15+272/8), "戻る");
						intraFontPrintJP((480/8+10), (9*15+15+272/8), "終了");

						sceGuFinish();
						sceGuSync(0,0);

						sceDisplayWaitVblankStart();
						flipScreen();
						sceCtrlReadBufferPositive(&pad, 1);
						if(pad.Buttons != 0){
							if(pad.Buttons & PSP_CTRL_UP){
								if(!flag && select > 1){
									select--;
								}
								flag = 1;
							}
							if (pad.Buttons & PSP_CTRL_DOWN){
								if(!flag && select < 9){
									select++;
								}
								flag = 1;
							}
							if(pad.Buttons & PSP_CTRL_CIRCLE){
								if(!flag && select == 1){
									select = 1;
									flag = 1;
									sub_state = 1;
									char message[1024] = "";
									float scroll = 69;
									while(running){
										if(state){
											state = 0;
											break;
										}

										ScreenPaste();

										guStart();
										DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
										DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
										DrawAlphaFillBoxScreen(0xAA, GRAY, (select*40+select*20+110+272/8), (480/8+30), 50,12);

										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
										sprintf(filepath, "%s%s", dirname, dir[fname].d_name);

										sprintf(file, "%s", dir[fname].d_name);
										sprintf(message, "%sをコピーしますか？", filepath);
										Sjis2UTF8(utf8, message);
										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_SCROLL_LEFT);
										scroll = intraFontPrintColumn(jpn0, scroll, (30+272/10), 340, utf8);
										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
										intraFontPrintJP((1*60+5+272/8+120), (480/8+40), "はい");
										intraFontPrintJP((2*60+272/8+120), (480/8+40), "いいえ");

										sceGuFinish();
										sceGuSync(0,0);

										sceDisplayWaitVblankStart();
										flipScreen();
										sceCtrlReadBufferPositive(&pad, 1);
										if(pad.Buttons != 0){
											if(pad.Buttons & PSP_CTRL_LEFT){
												if(!flag && select > 1){
													select--;
												}
												flag = 1;
											}
											if (pad.Buttons & PSP_CTRL_RIGHT){
												if(!flag && select < 2){
													select++;
												}
												flag = 1;
											}
											if(pad.Buttons & PSP_CTRL_CIRCLE){
												if(!flag && select == 1){
													while(running){
														if(state){
															state = 0;
															break;
														}

														FileMenuAction(dir, Background, 1);

														if(select == 1){
															ScreenPaste();

															guStart();
															DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
															DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

															intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
															intraFontPrintJP(240, (30+272/10), "コピーが完了しました。");

															sceGuFinish();
															sceGuSync(0,0);

															sceDisplayWaitVblankStart();
															flipScreen();

															dir = getDir(dirname, &entry, dir);//ファイルリスト更新
															sceIoChdir(dirname);//↓　↓

															page_count = 0;//↓　↓
															fname = 0;//ファイルリスト更新

															sceKernelDelayThread(500000);
															state = 1;
															break;
														}
														if(!flag && select == 2){
															ScreenPaste();

															guStart();
															DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
															DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

															intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
															intraFontPrintJP(240, (30+272/10), "コピーを終了しています。");

															sceGuFinish();
															sceGuSync(0,0);

															sceDisplayWaitVblankStart();
															flipScreen();

															dir = getDir(dirname, &entry, dir);//ファイルリスト更新
															sceIoChdir(dirname);//↓　↓

															page_count = 0;//↓　↓
															fname = 0;//ファイルリスト更新

															sceKernelDelayThread(500000);

															state = 1;
															break;
														}
														flag = 1;
													}
												}
												if(!flag && select == 2){
													ScreenPaste();

													guStart();

													DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
													DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

													intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
													intraFontPrintJP(240, (30+272/10), "コピーを終了しています。");

													sceGuFinish();
													sceGuSync(0,0);

													sceDisplayWaitVblankStart();
													flipScreen();

													dir = getDir(dirname, &entry, dir);//ファイルリスト更新
													sceIoChdir(dirname);//↓　↓

													page_count = 0;//↓　↓
													fname = 0;//ファイルリスト更新

													sceKernelDelayThread(500000);
													state = 1;
													break;
												}
												flag = 1;
											}
										}
										else flag = 0;
									}
									break;
								}if(!flag && select == 2){
									select = 1;
									flag = 1;
									sub_state = 1;
									char message[1024] = "";
									float scroll = 69;
									while(running){
										if(state){
											state = 0;
											break;
										}

										ScreenPaste();

										guStart();
										DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
										DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
										DrawAlphaFillBoxScreen(0xAA, GRAY, (select*40+select*20+110+272/8), (480/8+30), 50,12);

										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
										sprintf(filepath, "%s%s", dirname, dir[fname].d_name);

										sprintf(file, "%s", dir[fname].d_name);
										sprintf(message, "%sを切り取り・貼\り付けしますか？", filepath);
										Sjis2UTF8(utf8, message);
										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_SCROLL_LEFT);
										scroll = intraFontPrintColumn(jpn0, scroll, (30+272/10), 340, utf8);
										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
										intraFontPrintJP((1*60+5+272/8+120), (480/8+40), "はい");
										intraFontPrintJP((2*60+272/8+120), (480/8+40), "いいえ");

										sceGuFinish();
										sceGuSync(0,0);

										sceDisplayWaitVblankStart();
										flipScreen();
										sceCtrlReadBufferPositive(&pad, 1);
										if(pad.Buttons != 0){
											if(pad.Buttons & PSP_CTRL_LEFT){
												if(!flag && select > 1){
													select--;
												}
												flag = 1;
											}
											if (pad.Buttons & PSP_CTRL_RIGHT){
												if(!flag && select < 2){
													select++;
												}
												flag = 1;
											}
											if(pad.Buttons & PSP_CTRL_CIRCLE){
												if(!flag && select == 1){
													while(running){
														if(state){
															state = 0;
															break;
														}

														FileMenuAction(dir, Background, 2);

														Remove(filepath);

														if(select == 1){
															ScreenPaste();

															guStart();
															DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
															DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

															intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
															intraFontPrintJP(240, (30+272/10), "切り取り・貼\り付けが完了しました。");

															sceGuFinish();
															sceGuSync(0,0);

															sceDisplayWaitVblankStart();
															flipScreen();

															dir = getDir(dirname, &entry, dir);//ファイルリスト更新
															sceIoChdir(dirname);//↓　↓

															page_count = 0;//↓　↓
															fname = 0;//ファイルリスト更新

															sceKernelDelayThread(500000);
															state = 1;
															break;
														}
														if(!flag && select == 2){
															ScreenPaste();

															guStart();
															DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
															DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

															intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
															intraFontPrintJP(240, (30+272/10), "切り取り・貼\り付けを終了しています。");

															sceGuFinish();
															sceGuSync(0,0);

															sceDisplayWaitVblankStart();
															flipScreen();

															dir = getDir(dirname, &entry, dir);//ファイルリスト更新
															sceIoChdir(dirname);//↓　↓

															page_count = 0;//↓　↓
															fname = 0;//ファイルリスト更新

															sceKernelDelayThread(500000);

															state = 1;
															break;
														}
														flag = 1;
													}
												}
												if(!flag && select == 2){
													ScreenPaste();

													guStart();

													DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
													DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

													intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
													intraFontPrintJP(240, (30+272/10), "切り取り・貼\り付けを終了しています。");

													sceGuFinish();
													sceGuSync(0,0);

													sceDisplayWaitVblankStart();
													flipScreen();

													dir = getDir(dirname, &entry, dir);//ファイルリスト更新
													sceIoChdir(dirname);//↓　↓

													page_count = 0;//↓　↓
													fname = 0;//ファイルリスト更新

													sceKernelDelayThread(500000);
													state = 1;
													break;
												}
												flag = 1;
											}
										}
										else flag = 0;
									}
									break;
								}
								if(!flag && select == 3){
									select = 1;
									flag = 1;
									char message[1024] = "";
									float scroll = 69;
									while(running){
										if(state){
											state = 0;
											break;
										}

										ScreenPaste();

										guStart();
										DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
										DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
										DrawAlphaFillBoxScreen(0xAA, GRAY, (select*40+select*20+110+272/8), (480/8+30), 50,12);

										sprintf(filepath, "%s%s/", dirname, dir[fname].d_name);
										length = strlen(filepath);
										filepath[length-1] = '\0';

										sprintf(message, "%sを削除しますか？", filepath);
										Sjis2UTF8(utf8, message);
										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_SCROLL_LEFT);
										scroll = intraFontPrintColumn(jpn0, scroll, (30+272/10), 340, utf8);
										intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
										intraFontPrintJP((1*60+5+272/8+120), (480/8+40), "はい");
										intraFontPrintJP((2*60+272/8+120), (480/8+40), "いいえ");

										sceGuFinish();
										sceGuSync(0,0);

										sceDisplayWaitVblankStart();
										flipScreen();
										sceCtrlReadBufferPositive(&pad, 1);
										if(pad.Buttons != 0){
											if(pad.Buttons & PSP_CTRL_LEFT){
												if(!flag && select > 1){
													select--;
												}
												flag = 1;
											}
											if(pad.Buttons & PSP_CTRL_RIGHT){
												if(!flag && select < 2){
													select++;
												}
												flag = 1;
											}
											if(pad.Buttons & PSP_CTRL_CIRCLE){
												if(!flag && select == 1){
													while(running){
														if(state){
															state = 0;
															break;
														}

														Remove(filepath);

														ScreenPaste();

														guStart();
														DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
														DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

														intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
														intraFontPrintJP(240, (30+272/10), "削除が完了しました。");

														sceGuFinish();
														sceGuSync(0,0);

														sceDisplayWaitVblankStart();
														flipScreen();

														dir = getDir(dirname, &entry, dir);	//ファイルリスト更新
														sceIoChdir(dirname);			//

														page_count = 0;				//↓		　↓
														fname = 0;				//ファイルリスト更新

														sceKernelDelayThread(500000);
														state = 1;
														break;
													}
												}
												if(!flag && select == 2){
													flag = 1;
													break;
												}
												flag = 1;
											}
										}
										else flag = 0;
									}
									break;
								}
								if(!flag && select == 4){
									if(!flag){
										select = 1;
										char message[1024] = "";
										char newfilename[128] = "";
										char oldfilename[128] = "";
										char newfilepath[1024] = "";
										char oldfilepath[1024] = "";
										float scroll = 69;

										sprintf(oldfilename, "%s", dir[fname].d_name);

										OSKSetup();
										OSKInput("Rename",oldfilename,newfilename);
										initGraphics();
										flipScreen();
		
										while(running){
											if(state){
												state = 0;
												break;
											}

											ScreenPaste();

											guStart();
											DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
											DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
											DrawAlphaFillBoxScreen(0xAA, GRAY, (select*40+select*20+110+272/8), (480/8+30), 50,12);

											sprintf(message, "%sを%sにリネームしますか？", oldfilename, newfilename);
											sprintf(newfilepath, "%s%s", dirname, newfilename);
											sprintf(oldfilepath, "%s%s", dirname, oldfilename);

											Sjis2UTF8(utf8, message);
											intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_SCROLL_LEFT);
											scroll = intraFontPrintColumn(jpn0, scroll, (30+272/10), 340, utf8);
											intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
											intraFontPrintJP((1*60+5+272/8+120), (480/8+40), "はい");
											intraFontPrintJP((2*60+272/8+120), (480/8+40), "いいえ");

											sceGuFinish();
											sceGuSync(0,0);

											sceDisplayWaitVblankStart();
											flipScreen();
											sceCtrlReadBufferPositive(&pad, 1);
											if(pad.Buttons != 0){
												if(pad.Buttons & PSP_CTRL_CIRCLE){
													if(!flag && select == 1){
														sceIoRename(oldfilepath, newfilepath);

														ScreenPaste();

														guStart();
														DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
														DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

														intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
														intraFontPrintJP(240, (30+272/10), "リネームしました。");

														sceGuFinish();
														sceGuSync(0,0);

														sceDisplayWaitVblankStart();
														flipScreen();

														dir = getDir(dirname, &entry, dir);	//ファイルリスト更新
														sceIoChdir(dirname);			//

														page_count = 0;				//↓		　↓
														fname = 0;				//ファイルリスト更新

														sceKernelDelayThread(500000);
														state = 1;
														break;
													}
													if(!flag && select == 2){
														flag = 1;
														break;
													}
												}
												else flag = 0;
											}
										}
									}
									flag = 1;
								}
								if(!flag && select == 5){
									if(!flag){
										select = 1;
										char message[1024] = "";
										char foldername[128] = "";
										char folderpath[1024] = "";
										float scroll = 69;

										OSKSetup();
										OSKInput("Folder Name","新しいフォルダ",foldername);
										initGraphics();
										flipScreen();
		
										while(running){
											if(state){
												state = 0;
												break;
											}

											ScreenPaste();

											guStart();
											DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
											DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
											DrawAlphaFillBoxScreen(0xAA, GRAY, (select*40+select*20+110+272/8), (480/8+30), 50,12);

											sprintf(message, "フォルダ%sを作成しますか？", foldername);
											sprintf(folderpath, "%s%s", dirname, foldername);

											Sjis2UTF8(utf8, message);
											intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_SCROLL_LEFT);
											scroll = intraFontPrintColumn(jpn0, scroll, (30+272/10), 340, utf8);
											intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
											intraFontPrintJP((1*60+5+272/8+120), (480/8+40), "はい");
											intraFontPrintJP((2*60+272/8+120), (480/8+40), "いいえ");

											sceGuFinish();
											sceGuSync(0,0);

											sceDisplayWaitVblankStart();
											flipScreen();
											sceCtrlReadBufferPositive(&pad, 1);
											if(pad.Buttons != 0){
												if(pad.Buttons & PSP_CTRL_CIRCLE){
													if(!flag && select == 1){
														sceIoMkdir(foldername, 0777);

														ScreenPaste();

														guStart();
														DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
														DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

														intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
														intraFontPrintJP(240, (30+272/10), "フォルダを作成しました。");

														sceGuFinish();
														sceGuSync(0,0);

														sceDisplayWaitVblankStart();
														flipScreen();

														dir = getDir(dirname, &entry, dir);	//ファイルリスト更新
														sceIoChdir(dirname);			//

														page_count = 0;				//↓		　↓
														fname = 0;				//ファイルリスト更新

														sceKernelDelayThread(500000);
														state = 1;
														break;
													}
													if(!flag && select == 2){
														flag = 1;
														break;
													}
												}
												else flag = 0;
											}
										}
									}
									flag = 1;
								}
								if(!flag && select == 6){
									if(!flag){
										select = 1;
										char message[1024] = "";
										char filename[128] = "";
										char filepath[1024] = "";
										float scroll = 69;

										OSKSetup();
										OSKInput("File Name","新しいファイル",filename);
										initGraphics();
										flipScreen();
		
										while(running){
											if(state){
												state = 0;
												break;
											}

											ScreenPaste();

											guStart();
											DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
											DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
											DrawAlphaFillBoxScreen(0xAA, GRAY, (select*40+select*20+110+272/8), (480/8+30), 50,12);

											sprintf(message, "ファイル%sを作成しますか？", filename);
											sprintf(filepath, "%s%s", dirname, filename);

											Sjis2UTF8(utf8, message);
											intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_SCROLL_LEFT);
											scroll = intraFontPrintColumn(jpn0, scroll, (30+272/10), 340, utf8);
											intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
											intraFontPrintJP((1*60+5+272/8+120), (480/8+40), "はい");
											intraFontPrintJP((2*60+272/8+120), (480/8+40), "いいえ");

											sceGuFinish();
											sceGuSync(0,0);

											sceDisplayWaitVblankStart();
											flipScreen();
											sceCtrlReadBufferPositive(&pad, 1);
											if(pad.Buttons != 0){
												if(pad.Buttons & PSP_CTRL_CIRCLE){
													if(!flag && select == 1){
														SceUID fd;
														fd = sceIoOpen(filepath, PSP_O_WRONLY | PSP_O_CREAT, 0777);
														sceIoClose(fd);

														ScreenPaste();

														guStart();
														DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
														DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

														intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
														intraFontPrintJP(240, (30+272/10), "ファイルを作成しました。");

														sceGuFinish();
														sceGuSync(0,0);

														sceDisplayWaitVblankStart();
														flipScreen();

														dir = getDir(dirname, &entry, dir);	//ファイルリスト更新
														sceIoChdir(dirname);			//

														page_count = 0;				//↓		　↓
														fname = 0;				//ファイルリスト更新

														sceKernelDelayThread(500000);
														state = 1;
														break;
													}
													if(!flag && select == 2){
														flag = 1;
														break;
													}
												}
												else flag = 0;
											}
										}
									}
									flag = 1;
								}
								if(!flag && select == 7){
									if(!flag){
										Property(dirname, &fname, dir);
									}
									flag = 1;
								}
								if(!flag && select == 8){
									flag = 1;
									break;
								}
								if(!flag && select == 9){
									for (i = 0; i < 8; i++) {
										intraFontUnload(ltn[i]);
									}
									intraFontUnload(jpn0);

									intraFontShutdown();
									sceKernelExitGame();
									return 0;
								}
								flag = 1;
							}
							if(pad.Buttons & PSP_CTRL_CROSS){
								if(!flag){
									select = 8;
								}
								flag = 1;
							}
						}
						else flag = 0;
					}
				}
				if(pad.Buttons & PSP_CTRL_TRIANGLE){
					int select = 1;
					while(running){
						if(state){
							state = 0;
							break;
						}

						ScreenPaste();

						guStart();

						DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
						DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
						DrawAlphaFillBoxScreen(0xAA, GRAY, (480/8+10), (select*15+5+272/8), 340,12);

						intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
						intraFontPrintJP(240, (15+272/8), "PSP File Browser - Main Menu");
						intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
						intraFontPrintJP((480/8+10), (1*15+15+272/8), "System Viewer");
						intraFontPrintJP((480/8+10), (2*15+15+272/8), "Disk Viewer");
						intraFontPrintJP((480/8+10), (3*15+15+272/8), "CPU Clock Changer");
						intraFontPrintJP((480/8+10), (4*15+15+272/8), "Readme");
						intraFontPrintJP((480/8+10), (5*15+15+272/8), "戻る");
						intraFontPrintJP((480/8+10), (6*15+15+272/8), "終了");

						sceGuFinish();
						sceGuSync(0,0);

						sceDisplayWaitVblankStart();
						flipScreen();
						sceCtrlReadBufferPositive(&pad, 1);
						if(pad.Buttons != 0){
							if(pad.Buttons & PSP_CTRL_UP){
								if(!flag && select > 1){
									select--;
								}
								flag = 1;
							}
							if (pad.Buttons & PSP_CTRL_DOWN){
								if(!flag && select < 6){
									select++;
								}
								flag = 1;
							}
							if(pad.Buttons & PSP_CTRL_CIRCLE){
								if(!flag && select == 1){
									viewsystem(&fname, &entry, dir, Background);
								}
								if(!flag && select == 2){
									printDiskInfo();
								}
								if(!flag && select == 3){
									ChangeCPUClock();
								}
								if(!flag && select == 4){
									Readme(&fname, &entry, dir);
								}
								if(!flag && select == 5){
									flag = 1;
									break;
								}
								if(!flag && select == 6){
									for (i = 0; i < 8; i++) {
										intraFontUnload(ltn[i]);
									}
									intraFontUnload(jpn0);

									intraFontShutdown();
									sceKernelExitGame();
									return 0;
								}
								flag = 1;
							}
							if(pad.Buttons & PSP_CTRL_CROSS){
								if(!flag){
									select = 5;
								}
								flag = 1;
							}
						}
						else flag = 0;
					}
				}
				if(pad.Buttons & PSP_CTRL_LTRIGGER){
					if(!flag && page_count > 0){
						page_count--;
						fname = 20 * page_count;
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_RTRIGGER){
					if(!flag && page_count < page_max){
						page_count++;
						fname = 20 * page_count;
					}
					flag = 1;
				}
			}
			else if(170<=pad.Ly&&pad.Ly<=255){
				if(fname < entry -1){
					if((fname+1)%20 != 0)fname++;
					else if(page_count < page_max){
						page_count++;
						fname++;
					}
				}
			}
			else if(0<=pad.Ly&&pad.Ly<=84){
				if(fname > 0){
					if(fname%20 != 0)fname--;
					else if(page_count > 0){
						page_count--;
						fname--;
					}
				}			
			}
			else flag = 0;
		}
		else{
			while(running){
				clearScreen(BLACK);
				guStart();

				intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
				intraFontPrintJP(240, 15, "Memory Stickを入れてください。");

				sceGuFinish();
				sceGuSync(0,0);

				sceDisplayWaitVblankStart();
				flipScreen();
				sceCtrlReadBufferPositive(&pad, 1);
				if(status == 1)break;
			}
		}
	}

	if(Background!=NULL) freeImage(Background);

	for (i = 0; i < 8; i++) {
		intraFontUnload(ltn[i]);
	}
	intraFontUnload(jpn0);

	intraFontShutdown();
	sceKernelExitGame();
	return 0;
}