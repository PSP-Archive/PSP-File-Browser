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

int printError(int err) {
	if(err == ERROR_NO_AFTER_IMG){
		guStart();
		intraFontSetStyle(jpn0, 0.7f, WHITE, DARKGRAY, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, "次の画像はありません");

		sceGuFinish();
		sceGuSync(0,0);
		sceDisplayWaitVblankStart();
		flipScreen();
		sceKernelDelayThread(500000);
		return 0;
	}
	else if(err == ERROR_NO_BEFORE_IMG){
		guStart();
		intraFontSetStyle(jpn0, 0.7f, WHITE, DARKGRAY, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, "前の画像はありません");
		
		sceGuFinish();
		sceGuSync(0,0);
		sceDisplayWaitVblankStart();
		flipScreen();
		sceKernelDelayThread(500000);
		return 0;
	}
	else if(err == ERROR_UNSUPPORTED){
		while(running){
			ScreenPaste();
			guStart();
			DrawAlphaFillBoxScreen(0xAA, BLACK, (SCREEN_WIDTH-200)/2, (SCREEN_HEIGHT-100)/2, 200, 100);
			DrawAlphaBoxScreen(0xAA, WHITE, (SCREEN_WIDTH-200)/2, (SCREEN_HEIGHT-100)/2, 200, 100);
			intraFontSetStyle(jpn0, 0.7f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
			intraFontPrintJP(SCREEN_WIDTH/2, (SCREEN_HEIGHT-100)/2+40, "画像が読み込めませんでした\n\n\n\n×戻る");
			sceGuFinish();
			sceGuSync(0,0);

			sceDisplayWaitVblankStart();
			flipScreen();
			sceCtrlReadBufferPositive(&pad, 1);
			if(pad.Buttons & PSP_CTRL_CROSS){
				break;
			}
		}
	}
	else if(err == ERROR_NO_AFTER_MP3){
		guStart();
		intraFontSetStyle(jpn0, 0.7f, WHITE, DARKGRAY, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, "次の音楽はありません");

		sceGuFinish();
		sceGuSync(0,0);
		sceDisplayWaitVblankStart();
		flipScreen();
		sceKernelDelayThread(500000);
		return 0;
	}
	else if(err == ERROR_NO_BEFORE_MP3){
		guStart();
		intraFontSetStyle(jpn0, 0.7f, WHITE, DARKGRAY, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, "前の音楽はありません");
		
		sceGuFinish();
		sceGuSync(0,0);
		sceDisplayWaitVblankStart();
		flipScreen();
		sceKernelDelayThread(500000);
		return 0;
	}
	return 0;
}