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

#include "systemctrl.h"
#include "kubridge.h"

#include "draw.h"
#include "intraFontJP.h"
#include "Error.h"
#include "Readme.h"
#include "getDir.h"
#include "parseDate.h"

char utf8[1024];

int Property(char *dirname,int *fname, SceIoDirent *dir) {
	char filemode[30] = "";			//ファイル属性
	char ctime[30] = "";			//作成日時
	char atime[30] = "";			//アクセス日時
	char mtime[30] = "";			//更新日時
	char filename[1024] = "";		//ファイルネーム
	char filepath[1024] = "";		//フルパス
	double filesize;			//サイズ

	if(dir[*fname].d_stat.st_attr & FIO_SO_IWOTH)strcpy(filemode,"隠し");
	else if(dir[*fname].d_stat.st_attr & FIO_SO_IFREG)strcpy(filemode,"通常");
	if(FIO_SO_ISDIR(dir[*fname].d_stat.st_attr))sprintf(filemode,"%sフォルダ",filemode);
	else sprintf(filemode,"%sファイル",filemode);
	filesize = (double)dir[*fname].d_stat.st_size;
	parseDate(ctime, dir[*fname].d_stat.st_ctime);
	parseDate(atime, dir[*fname].d_stat.st_atime);
	parseDate(mtime, dir[*fname].d_stat.st_mtime);

	float scroll = 120;
	float scroll2 = 70;
	Sjis2UTF8(utf8, dir[*fname].d_name);
	sprintf(filename, "%s", utf8);
	Sjis2UTF8(utf8, dir[*fname].d_name);
	sprintf(filepath, "%s%s", dirname, utf8);

	flag = 1;
	while(running){
		ScreenPaste();

		guStart();
		DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
		DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

		intraFontSetStyle(jpn0, 0.8f, WHITE, DARKGRAY, INTRAFONT_SCROLL_LEFT);
		scroll2 = intraFontPrintColumn(jpn0, scroll2, 65, 340, filename);
		intraFontSetStyle(jpn0, 0.8f,WHITE,DARKGRAY,INTRAFONT_ALIGN_CENTER);
		intraFontSetStyle(jpn0, 0.6f,WHITE,DARKGRAY,0);
		intraFontPrintJP((480/8+10), (1*15+40+272/8), "フルパス:");
		intraFontSetStyle(jpn0, 0.6f, WHITE, DARKGRAY, INTRAFONT_SCROLL_LEFT);
		scroll = intraFontPrintColumn(jpn0, scroll, (1*15+40+272/8), 290, filepath);
		intraFontSetStyle(jpn0, 0.6f,WHITE,DARKGRAY,0);
		intraFontPrintfJP( (480/8+10), (2*15+40+272/8), "属性: %s", filemode);
		if(dir[*fname].d_stat.st_attr & FIO_SO_IFREG){
			if(filesize >= 1024 * 1024 * 1024){
				intraFontPrintfJP((480/8+10), (3*15+40+272/8), "サイズ: %.2f GB", filesize/1024/1024/1024);
			}
			else if(filesize >= 1024 * 1024){
				intraFontPrintfJP((480/8+10), (3*15+40+272/8), "サイズ: %.2f MB", filesize/1024/1024);
			}
			else if(filesize >= 1024){
				intraFontPrintfJP((480/8+10), (3*15+40+272/8), "サイズ: %.2f KB", filesize/1024);
			}
			else{
				intraFontPrintfJP((480/8+10), (3*15+40+272/8), "サイズ: %d バイト", (SceOff)filesize);
			}
		}
		intraFontPrintfJP((480/8+10), (4*15+40+272/8), "作成日時: %s", ctime);
		intraFontPrintfJP((480/8+10), (5*15+40+272/8), "アクセス日時: %s", atime);
		intraFontPrintfJP((480/8+10), (6*15+40+272/8), "更新日時: %s", mtime);

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
	}
	return 0;
}