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

char *model[] = {
	"PSP 1000",
	"PSP 2000",
	"PSP 3000",
	"Unknown",
	"PSP Go"
};

int viewsystem(int *fname, int *entry, SceIoDirent *dir, Image* Background) {
	state = 1;
	while(running){
		clearScreen(BLACK);

		if(Background!=NULL)blitImageToScreen(0,0,480,272,Background,0,0);

		guStart();

		pspTime time;
		sceRtcGetCurrentClockLocalTime(&time);
		char timeBuffer[22];
		sprintf(timeBuffer, "%d/%02d/%02d %02d:%02d %d%%", time.year, time.month, time.day, time.hour, time.minutes, scePowerGetBatteryLifePercent());

		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(240, 15, "PSP File Browser");
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
		intraFontPrintJP(5, 15, "System Viewer");
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_RIGHT);
		intraFontPrintfJP(475, 15, "%s", timeBuffer);
		intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);

		intraFontPrintfJP(5, 0*15+30, "Model：%s", model[kuKernelGetModel()]);

		int version_number = sceKernelDevkitVersion();
		char *version = (char *)&version_number;

		intraFontPrintfJP(5, 1*15+40, "Version：%i.%i%i\n\n", version[3], version[2], version[1]);
		intraFontPrintfJP(5, 2*15+40, "Version：0x%08X\n\n", version_number);

		char nickname[20];
		sceUtilityGetSystemParamString(PSP_SYSTEMPARAM_ID_STRING_NICKNAME, nickname, 20); 

		intraFontPrintfJP(5, 3*15+50, "Nickname：%s\n\n", nickname);

		int language_number;
		char *language = "";
		sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &language_number);

		if(language_number == 0)	language = "日本語";
		else if(language_number ==  1)	language = "English";
		else if(language_number ==  2)	language = "French";
		else if(language_number ==  3)	language = "Spanish";
		else if(language_number ==  4)	language = "German";
		else if(language_number ==  5)	language = "Italian";
		else if(language_number ==  6)	language = "Dutch";
		else if(language_number ==  7)	language = "Portuguese";
		else if(language_number ==  8)	language = "Russian";
		else if(language_number ==  9)	language = "Pussian";
		else if(language_number == 10)	language = "Korean";
		else if(language_number == 11)	language = "Chinese Traditional";
		else if(language_number == 12)	language = "Chinese Simplified";

		intraFontPrintfJP(5, 4*15+50, "Language：%s\n\n", language);

		intraFontPrintfJP(5, 5*15+60, "BatteryLifeTime：%d分\n\n", scePowerGetBatteryLifeTime());
		intraFontPrintfJP(5, 6*15+60, "BatteryPercent：%d%%\n\n", scePowerGetBatteryLifePercent());

		intraFontPrintfJP(5, 7*15+70, "CPUClock：%d\n\n", scePowerGetCpuClockFrequency());
		intraFontPrintfJP(5, 8*15+70, "BUSClock：%d\n\n", scePowerGetBusClockFrequency());

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();

		sceCtrlReadBufferPositive(&pad, 1);

		if(pad.Buttons != 0){
			if(pad.Buttons & PSP_CTRL_CROSS){
				if(!flag){
					break;
				}
				flag = 1;
			}
		}
	}
	return 0;
}