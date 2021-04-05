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

int ChangeCPUClock() {
	int freq;
	int select = 1;
	int flag = 1;
	while(running){
		if(state){
			state = 0;
			break;
		}

		ScreenPaste();

		guStart();

		DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
		DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
		DrawAlphaFillBoxScreen(0xAA, GRAY, (480/8+10), (select*15+20+272/8), 340,12);

		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(240, (15+272/8), "PSP File Browser - CPU Clock Changer");
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
		intraFontPrintfJP((480/8+10), (1*15+15+272/8), "Œ»Ý‚ÌÝ’èF%d/%d",scePowerGetCpuClockFrequency(),scePowerGetBusClockFrequency());
		intraFontPrintJP((480/8+10), (2*15+15+272/8), "20/10");
		intraFontPrintJP((480/8+10), (3*15+15+272/8), "75/37");
		intraFontPrintJP((480/8+10), (4*15+15+272/8), "100/50");
		intraFontPrintJP((480/8+10), (5*15+15+272/8), "133/66");
		intraFontPrintJP((480/8+10), (6*15+15+272/8), "222/111");
		intraFontPrintJP((480/8+10), (7*15+15+272/8), "266/133");
		intraFontPrintJP((480/8+10), (8*15+15+272/8), "300/150");
		intraFontPrintJP((480/8+10), (9*15+15+272/8), "333/166");
		intraFontPrintJP((480/8+10), (10*15+15+272/8), "–ß‚é");

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
					freq = 20;
					scePowerSetClockFrequency(freq,freq,10);
				}
				if(!flag && select == 2){
					freq = 75;
					scePowerSetClockFrequency(freq,freq,(freq/2));
				}
				if(!flag && select == 3){
					freq = 100;
					scePowerSetClockFrequency(freq,freq,(freq/2));
				}
				if(!flag && select == 4){
					freq = 133;
					scePowerSetClockFrequency(freq,freq,(freq/2));
				}
				if(!flag && select == 5){
					freq = 222;
					scePowerSetClockFrequency(freq,freq,(freq/2));
				}
				if(!flag && select == 6){
					freq = 266;
					scePowerSetClockFrequency(freq,freq,(freq/2));
				}
				if(!flag && select == 7){
					freq = 300;
					scePowerSetClockFrequency(freq,freq,(freq/2));
				}
				if(!flag && select == 8){
					freq = 333;
					scePowerSetClockFrequency(freq,freq,(freq/2));
				}
				if(!flag && select == 9){
					flag = 1;
					break;
				}
				flag = 1;
			}
			if(pad.Buttons & PSP_CTRL_CROSS){
				if(!flag){
					select = 9;
				}
				flag = 1;
			}
		}
		else flag = 0;
	}
	return 0;
}