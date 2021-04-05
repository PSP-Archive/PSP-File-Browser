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
			intraFontPrintJP((480/8+10), (1*15+15+272/8), "�ڎ��F");
			intraFontPrintJP((480/8+10), (2*15+15+272/8), "1.�ڎ�");
			intraFontPrintJP((480/8+10), (3*15+15+272/8), "2.����ɂ���");
			intraFontPrintJP((480/8+10), (4*15+15+272/8), "3.PSP File Browser�ɂ���");
			intraFontPrintJP((480/8+10), (5*15+15+272/8), "4.Special Thanks");
		}
		if(rpage == 2) {
			intraFontPrintJP((480/8+10), ( 1*15+15+272/8), "����ɂ��āF");
			intraFontPrintJP((480/8+10), ( 2*15+15+272/8), "Main Program");
			intraFontPrintJP((480/8+10), ( 3*15+15+272/8), "���{�^��");
			intraFontPrintJP((480/8+60), ( 3*15+15+272/8), "�F1��̃t�H���_�E�t�@�C����I��");
			intraFontPrintJP((480/8+10), ( 4*15+15+272/8), "���{�^��");
			intraFontPrintJP((480/8+60), ( 4*15+15+272/8), "�F1���̃t�H���_�E�t�@�C����I��");
			intraFontPrintJP((480/8+10), ( 5*15+15+272/8), "���{�^��");
			intraFontPrintJP((480/8+60), ( 5*15+15+272/8), "�F�I�����Ă���t�H���_���J��");
			intraFontPrintJP((480/8+10), ( 6*15+15+272/8), "�~�{�^��");
			intraFontPrintJP((480/8+60), ( 6*15+15+272/8), "�F�J���Ă���t�@�C�������");
			intraFontPrintJP((480/8+10), ( 7*15+15+272/8), "���{�^��");
			intraFontPrintJP((480/8+60), ( 7*15+15+272/8), "�F���C�����j���[");
			intraFontPrintJP((480/8+10), ( 8*15+15+272/8), "���{�^��");
			intraFontPrintJP((480/8+60), ( 8*15+15+272/8), "�F�t�@�C�����j���[");
			intraFontPrintJP((480/8+10), ( 9*15+15+272/8), "R�{�^��");
			intraFontPrintJP((480/8+60), ( 9*15+15+272/8), "�F���y�[�W");
			intraFontPrintJP((480/8+10), (10*15+15+272/8), "L�{�^��");
			intraFontPrintJP((480/8+60), (10*15+15+272/8), "�F�O�y�[�W");
		}
		if(rpage == 3) {
			intraFontPrintJP((480/8+10), (1*15+15+272/8), "PSP File Browser�ɂ��āF");
			intraFontPrintJP((480/8+10), (2*15+15+272/8), "�o�[�W����");
			intraFontPrintJP((480/8+70), (2*15+15+272/8), "�F1.00");
			intraFontPrintJP((480/8+10), (3*15+15+272/8), "�����");
			intraFontPrintJP((480/8+70), (3*15+15+272/8), "�FLIGHT");
			intraFontPrintJP((480/8+10), (4*15+15+272/8), "�u���O");
			intraFontPrintJP((480/8+70), (4*15+15+272/8), "�Fhttp://netnews100.blog112.fc2.com/");
		}
		if(rpage == 4) {
			intraFontPrintJP((480/8+10), ( 1*15+15+272/8), "Special Thanks�F");
			intraFontPrintJP((480/8+10), ( 2*15+15+272/8), "Multimedia Player v1.0");
			intraFontPrintJP((480/8+150), ( 2*15+15+272/8), "�FChitoku ��");
			intraFontPrintJP((480/8+10), ( 3*15+15+272/8), "PSPident 0.4");
			intraFontPrintJP((480/8+150), ( 3*15+15+272/8), "�F~jas0nuk ��");
			intraFontPrintJP((480/8+10), ( 4*15+15+272/8), "intraFont 0.31");
			intraFontPrintJP((480/8+150), ( 4*15+15+272/8), "�FBenHur ��");
			intraFontPrintJP((480/8+10), ( 5*15+15+272/8), "Libccc 0.31");
			intraFontPrintJP((480/8+150), ( 5*15+15+272/8), "�FBenHur ��");
			intraFontPrintJP((480/8+10), ( 6*15+15+272/8), "libio");
			intraFontPrintJP((480/8+150), ( 6*15+15+272/8), "�FDadrfy ��");
			intraFontPrintJP((480/8+10), ( 7*15+15+272/8), "cptbl");
			intraFontPrintJP((480/8+150), ( 7*15+15+272/8), "�FSTEAR ��");
			intraFontPrintJP((480/8+10), ( 8*15+15+272/8), "charConv");
			intraFontPrintJP((480/8+150), ( 8*15+15+272/8), "�FSTEAR ��");
			intraFontPrintJP((480/8+10), ( 9*15+15+272/8), "6.39 PRO");
			intraFontPrintJP((480/8+150), ( 9*15+15+272/8), "�FTeam PRO");
			intraFontPrintJP((480/8+10), (10*15+15+272/8), "Advice�F�L�R �L�@ �� ABCanG �� Zooner �� Dadrfy �� �䂤��");
			intraFontPrintJP((480/8+10), (11*15+15+272/8), "Tester�FOVER�� Team D.C.A Team PSCF");			
		}
		if(rpage != 1) intraFontPrintJP(120, 232, "���O��");
		intraFontPrintJP(220, 232, "�~�߂�");
		if(rpage != 4) intraFontPrintJP(320, 232, "���ց�");

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