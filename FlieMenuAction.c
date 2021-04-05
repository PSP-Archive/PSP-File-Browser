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
#include "getDir.h"

char dirname[1024], *tmp;
char dirfile[1024], *tmp;
char file[1024];
char sub_file[1024];
char filename[1024];
char filepath[1024], *tmp;
char message[1024];
float scroll = 69;
char utf8[1024];

int FileMenuAction(SceIoDirent *dir, Image* Background, int mode){
	int sub_entry = 0;

	strcpy(dirname, "ms0:/");
	dir = getDir(dirname, &sub_entry, NULL);
	sceIoChdir(dirname);
	if(sub_entry < 20)sub_page_max = 0;
	else{
		page_max = sub_entry/20-1;
		if(sub_entry%20 != 0)page_max++;
	}
	sub_page_count = 0;
	sub_fname = 0;
	flag = 1;

	while(running){
		clearScreen(BLACK);

		if(Background!=NULL)blitImageToScreen(0,0,480,272,Background,0,0);

		guStart();

		pspTime time;
		sceRtcGetCurrentClockLocalTime(&time);
		char timeBuffer[22];
		sprintf(timeBuffer, "%d/%02d/%02d %02d:%02d %d%%", time.year, time.month, time.day, time.hour, time.minutes, scePowerGetBatteryLifePercent());

		float y = 42;

		DrawAlphaFillBoxScreen(0xAA, BLACK, 60, 252, 360, 20);
		HorizontalAlphaLine(0xAA, WHITE, 60, 252, 360);
		VerticalAlphaLine(0xAA, WHITE, 60, 252, 20);
		VerticalAlphaLine(0xAA, WHITE, 421, 252, 20);
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
		if(mode == 1) intraFontPrintJP(240, 267, "コピー場所を選択してください。 □：コピー");
		if(mode == 2) intraFontPrintJP(240, 267, "貼\り付け場所を選択してください。 □：貼\り付け");

		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
		intraFontPrintfJP(5, 15, "%d/%dページ", page_count + 1, page_max + 1);
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_RIGHT);
		intraFontPrintfJP(475, 15, "%s", timeBuffer);
		intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
		intraFontPrintJP(5, 30, dirname);

		DrawAlphaFillBoxScreen(0x88, WHITE, 0, 12 * (sub_fname + 1 - 20 * sub_page_count) + 22, 480, 10);

		if(sub_entry<20){
			for(i = 0; i<sub_entry; i++){
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
			for(i = 0; i<(sub_page_count<sub_page_max ? 20 : sub_entry-20*sub_page_count); i++){
				if(dir[i + 20 * sub_page_count].d_stat.st_attr & FIO_SO_IFDIR){
					intraFontSetStyle(jpn0, 0.5f, WHITE, DARKGRAY, 0);
					intraFontPrintfJP(5, y, "%s/", dir[i + 12 * sub_page_count].d_name);
				}
				else intraFontSetStyle(jpn0, 0.5f, WHITE, BLACK, 0);
				intraFontPrintJP(5, y, dir[i + 12 * sub_page_count].d_name);
				y+=12;
			}
		}

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();

		sceCtrlReadBufferPositive(&pad, 1);
		if(MScmIsMediumInserted()){
			if(pad.Buttons != 0){
				if(pad.Buttons & PSP_CTRL_DOWN){
					if(!flag && sub_fname < sub_entry -1){
						if((sub_fname+1)%20 != 0)sub_fname++;
						else if(sub_page_count < sub_page_max){
							sub_page_count++;
							sub_fname++;
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_UP){
					if(!flag && sub_fname > 0){
						if(sub_fname%20 != 0)sub_fname--;
						else if(sub_page_count > 0){
							sub_page_count--;
							sub_fname--;
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_CROSS){
					if(!flag){
						if(strchr(dirname, '/') != strrchr(dirname, '/')){
							tmp = strrchr(dirname, '/');
							sub_length = strlen(dirname);
							dirname[sub_length-1] = '\0';
							tmp = strrchr(dirname, '/');
							tmp[1] = '\0';
							dir = getDir(dirname, &sub_entry, dir);
							if(sub_entry < 20)sub_page_max = 0;
							else{
								sub_page_max = sub_entry/20-1;
								if(sub_entry%20 != 0)sub_page_max++;
							}
							sub_page_count = 0;
							sceIoChdir(dirname);
							sub_fname = 0;
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_CIRCLE){
					if(!flag && sub_entry > 0){
						tmp = strrchr(dir[sub_fname].d_name, '.');
						if(dir[sub_fname].d_stat.st_attr & FIO_SO_IFDIR){
							sprintf(dirname, "%s%s/", dirname, dir[sub_fname].d_name);
							dir = getDir(dirname, &sub_entry, dir);
							sceIoChdir(dirname);
							if(sub_entry < 20)sub_page_max = 0;
							else{
								sub_page_max = sub_entry/20-1;
								if(sub_entry%20 != 0)sub_page_max++;
							}
							sub_page_count = 0;
							sub_fname = 0;
						}
					}
					flag = 1;
				}
				if(pad.Buttons & PSP_CTRL_SQUARE){
					int select = 1;
					if(!flag && select == 1){
						flag = 1;
						while(running){
							if(state){
								break;
							}

							clearScreen(BLACK);

							if(Background!=NULL)blitImageToScreen(0,0,480,272,Background,0,0);

							guStart();
							DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
							DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);
							DrawAlphaFillBoxScreen(0xAA, GRAY, (select*40+select*20+110+272/8), (480/8+30), 50,12);

							sprintf(sub_file, "%s", dir[sub_fname].d_name);

							if(dir[fname].d_stat.st_attr & FIO_SO_IFREG && dir[sub_fname].d_stat.st_attr & FIO_SO_IFREG){
								sprintf(dirfile, "%s%s", dirname, sub_file);
							}
							else if(dir[fname].d_stat.st_attr & FIO_SO_IFREG && dir[sub_fname].d_stat.st_attr & FIO_SO_IFDIR){
								sprintf(dirfile, "%s%s/%s", dirname, sub_file, file);
							}
							else if(dir[fname].d_stat.st_attr & FIO_SO_IFDIR && dir[sub_fname].d_stat.st_attr & FIO_SO_IFREG){
								sprintf(dirfile, "%s", dirname);
							}
							else if(dir[fname].d_stat.st_attr & FIO_SO_IFDIR && dir[sub_fname].d_stat.st_attr & FIO_SO_IFDIR){
								sprintf(dirfile, "%s%s/", dirname, sub_file);
							}

							if(mode == 1) sprintf(message, "%sを%sにコピーしますか？", filepath, dirfile);
							if(mode == 2) sprintf(message, "%sを%sに貼\り付けしますか？", filepath, dirfile);
							Sjis2UTF8(utf8, message);
							intraFontSetStyle(jpn0, 0.6f, WHITE, DARKGRAY, INTRAFONT_SCROLL_LEFT);
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
										Copy(filepath,dirfile);
										break;
									}
									else{
										state = 1;
										break;
									}
								}
								else flag = 0;
							}
						}
						state = 0;
						break;
					}
				}
			}
			else if(170<=pad.Ly&&pad.Ly<=255){
				if(sub_fname < sub_entry -1){
					if((sub_fname+1)%20 != 0)sub_fname++;
					else if(sub_page_count < sub_page_max){
						sub_page_count++;
						sub_fname++;
					}
				}
			}
			else if(0<=pad.Ly&&pad.Ly<=84){
				if(sub_fname > 0){
					if(sub_fname%20 != 0)sub_fname--;
					else if(sub_page_count > 0){
						sub_page_count--;
						sub_fname--;
					}
				}			
			}
			else flag = 0;
		}
	}
	return 0;
}