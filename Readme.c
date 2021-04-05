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

int Readme(int *fname, int *entry, SceIoDirent *dir) {
	int rpage = 1;
	while(running) {
		ScreenPaste();

		guStart();

		DrawAlphaFillBoxScreen(0xAA, BLACK, (480/8), (272/8), 360, 204);
		DrawAlphaBoxScreen(0xAA, WHITE, (480/8), (272/8), 360, 204);

		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_CENTER);
		intraFontPrintJP(240, (15+272/8), "PSP File Browser - Readme");
		intraFontSetStyle(jpn0, 0.6f, WHITE, BLACK, INTRAFONT_ALIGN_LEFT);
		if(rpage == 1) {
			intraFontPrintJP((480/8+10), (1*15+15+272/8), "目次：");
			intraFontPrintJP((480/8+10), (2*15+15+272/8), "1.目次");
			intraFontPrintJP((480/8+10), (3*15+15+272/8), "2.操作について");
			intraFontPrintJP((480/8+10), (4*15+15+272/8), "3.PSP File Browserについて");
			intraFontPrintJP((480/8+10), (5*15+15+272/8), "4.Special Thanks");
		}
		if(rpage == 2) {
			intraFontPrintJP((480/8+10), ( 1*15+15+272/8), "操作について：");
			intraFontPrintJP((480/8+10), ( 2*15+15+272/8), "Main Program");
			intraFontPrintJP((480/8+10), ( 3*15+15+272/8), "↑ボタン");
			intraFontPrintJP((480/8+60), ( 3*15+15+272/8), "：1つ上のフォルダ・ファイルを選択");
			intraFontPrintJP((480/8+10), ( 4*15+15+272/8), "↓ボタン");
			intraFontPrintJP((480/8+60), ( 4*15+15+272/8), "：1つ下のフォルダ・ファイルを選択");
			intraFontPrintJP((480/8+10), ( 5*15+15+272/8), "○ボタン");
			intraFontPrintJP((480/8+60), ( 5*15+15+272/8), "：選択しているフォルダを開く");
			intraFontPrintJP((480/8+10), ( 6*15+15+272/8), "×ボタン");
			intraFontPrintJP((480/8+60), ( 6*15+15+272/8), "：開いているファイルを閉じる");
			intraFontPrintJP((480/8+10), ( 7*15+15+272/8), "△ボタン");
			intraFontPrintJP((480/8+60), ( 7*15+15+272/8), "：メインメニュー");
			intraFontPrintJP((480/8+10), ( 8*15+15+272/8), "□ボタン");
			intraFontPrintJP((480/8+60), ( 8*15+15+272/8), "：ファイルメニュー");
			intraFontPrintJP((480/8+10), ( 9*15+15+272/8), "Rボタン");
			intraFontPrintJP((480/8+60), ( 9*15+15+272/8), "：次ページ");
			intraFontPrintJP((480/8+10), (10*15+15+272/8), "Lボタン");
			intraFontPrintJP((480/8+60), (10*15+15+272/8), "：前ページ");
		}
		if(rpage == 3) {
			intraFontPrintJP((480/8+10), (1*15+15+272/8), "PSP File Browserについて：");
			intraFontPrintJP((480/8+10), (2*15+15+272/8), "バージョン");
			intraFontPrintJP((480/8+70), (2*15+15+272/8), "：1.00");
			intraFontPrintJP((480/8+10), (3*15+15+272/8), "製作者");
			intraFontPrintJP((480/8+70), (3*15+15+272/8), "：LIGHT");
			intraFontPrintJP((480/8+10), (4*15+15+272/8), "ブログ");
			intraFontPrintJP((480/8+70), (4*15+15+272/8), "：http://netnews100.blog112.fc2.com/");
		}
		if(rpage == 4) {
			intraFontPrintJP((480/8+10), ( 1*15+15+272/8), "Special Thanks：");
			intraFontPrintJP((480/8+10), ( 2*15+15+272/8), "Multimedia Player v1.0");
			intraFontPrintJP((480/8+150), ( 2*15+15+272/8), "：Chitoku 氏");
			intraFontPrintJP((480/8+10), ( 3*15+15+272/8), "PSPident 0.4");
			intraFontPrintJP((480/8+150), ( 3*15+15+272/8), "：~jas0nuk 氏");
			intraFontPrintJP((480/8+10), ( 4*15+15+272/8), "intraFont 0.31");
			intraFontPrintJP((480/8+150), ( 4*15+15+272/8), "：BenHur 氏");
			intraFontPrintJP((480/8+10), ( 5*15+15+272/8), "Libccc 0.31");
			intraFontPrintJP((480/8+150), ( 5*15+15+272/8), "：BenHur 氏");
			intraFontPrintJP((480/8+10), ( 6*15+15+272/8), "libio");
			intraFontPrintJP((480/8+150), ( 6*15+15+272/8), "：Dadrfy 氏");
			intraFontPrintJP((480/8+10), ( 7*15+15+272/8), "cptbl");
			intraFontPrintJP((480/8+150), ( 7*15+15+272/8), "：STEAR 氏");
			intraFontPrintJP((480/8+10), ( 8*15+15+272/8), "charConv");
			intraFontPrintJP((480/8+150), ( 8*15+15+272/8), "：STEAR 氏");
			intraFontPrintJP((480/8+10), ( 9*15+15+272/8), "6.39 PRO");
			intraFontPrintJP((480/8+150), ( 9*15+15+272/8), "：Team PRO");
			intraFontPrintJP((480/8+10), (10*15+15+272/8), "Advice：猫山 猫宗 氏 ABCanG 氏 Zooner 氏 Dadrfy 氏 ゆう氏");
			intraFontPrintJP((480/8+10), (11*15+15+272/8), "Tester：OVER氏 Team D.C.A Team PSCF");			
		}
		if(rpage != 1) intraFontPrintJP(120, 232, "←前へ");
		intraFontPrintJP(220, 232, "×戻る");
		if(rpage != 4) intraFontPrintJP(320, 232, "次へ→");

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		flipScreen();
		sceCtrlReadBufferPositive(&pad, 1);
		if(pad.Buttons != 0){
			if(pad.Buttons & PSP_CTRL_LEFT){
				if(!flag && rpage > 1) {
					rpage--;
				}
				flag = 1;
			}
			if(pad.Buttons & PSP_CTRL_RIGHT){
				if(!flag && rpage < 4) {
					rpage++;
				}
				flag = 1;
			}
			if(pad.Buttons & PSP_CTRL_CROSS){
				if(!flag) {
					break;
				}
				flag = 1;
			}
		}
		else flag = 0;
	}
	return 0;
}