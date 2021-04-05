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

int viewtext(int *fname, int *entry, SceIoDirent *dir, Image* Background) {
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
		intraFontPrintJP(5, 15, "Text Viewer");
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_RIGHT);
		intraFontPrintfJP(475, 15, "%s", timeBuffer);
		intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);

		//�t�@�C���n���h��
		SceUID fd;
		//�t�@�C���ǂݍ���
		if ((fd = sceIoOpen(dir[*fname].d_name, PSP_O_RDONLY, 0777)) > 0) {
			int size;

			sceIoLseek(fd, 0, PSP_SEEK_SET);		// �ǂݏo���ʒu��擪��
			size = sceIoLseek(fd, 0, PSP_SEEK_END);		// �ǂݏo���ʒu�𖖔���
			sceIoLseek(fd, 0, PSP_SEEK_SET);		// �ǂݏo���ʒu��擪��

			char data[size];
			int nread;

			if (size > 0) {
				nread = sceIoRead(fd, data, size);
				data[nread] = '\0';
				intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
				intraFontPrintfJP(5, 30, "%s", data);
				sceIoClose(fd);
			}
			else {
				intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
				intraFontPrintJP(5, 30, "�t�@�C�����J���܂���ł����B");
			}
 		}
		else {
			intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
			intraFontPrintJP(5, 30, "�t�@�C�����J���܂���ł����B");
		}

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

	flag = 1;
	return 0;
}