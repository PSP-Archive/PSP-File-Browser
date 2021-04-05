#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <string.h>
#include <psputility.h>
#include <psputility_osk.h>
#include "converter.h"

static unsigned int __attribute__((aligned(16))) list[262144];

#define BUF_WIDTH	(512)
#define SCR_WIDTH	(480)
#define SCR_HEIGHT	(272)
#define PIXEL_SIZE	(4)
#define FRAME_SIZE	(BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE	(BUF_WIDTH SCR_HEIGHT * 2)

#define TEXT_LENGTH	(128)

void OSKSetup(void) {
	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_FLAT);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

int OSKInput(char *title,char *intext, char *outtext) {
	int done = 1;
	unsigned char*	p;
	unsigned short OSKDesc[TEXT_LENGTH], OSKIn[TEXT_LENGTH], OSKOut[TEXT_LENGTH];

	outtext[0] = '\0';
	OSKOut[0] = 0;

	p = (unsigned char*) title;
	SJIS2UCS(OSKDesc, p, TEXT_LENGTH);
	p = (unsigned char*) intext;
	SJIS2UCS(OSKIn, p, TEXT_LENGTH);

	SceUtilityOskData data;
	
	memset(&data, 0, sizeof(SceUtilityOskData));
	data.language = PSP_UTILITY_OSK_LANGUAGE_DEFAULT; // Use system default for text input
	data.lines = 1;
	data.unk_24 = 1;
	data.inputtype = PSP_UTILITY_OSK_INPUTTYPE_ALL; // Allow all input types
	data.desc = OSKDesc;
	data.intext = OSKIn;
	data.outtextlength = TEXT_LENGTH;
	data.outtextlimit = 32; // Limit input to 32 characters
	data.outtext = OSKOut;
	
	SceUtilityOskParams params;
	memset(&params, 0, sizeof(params));
	params.base.size = sizeof(params);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &params.base.language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &params.base.buttonSwap);
	params.base.graphicsThread = 17;
	params.base.accessThread = 19;
	params.base.fontThread = 18;
	params.base.soundThread = 16;
	params.datacount = 1;
	params.data = &data;

	sceUtilityOskInitStart(&params);
	while(done) {
		sceGuStart(GU_DIRECT,list);
		sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
		sceGuClearColor(0xCCCCCC);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

		sceGuFinish();
		sceGuSync(0,0);

		switch(sceUtilityOskGetStatus()) {
			case PSP_UTILITY_DIALOG_INIT:
				break;

			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityOskUpdate(1);
				break;

			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityOskShutdownStart();
				break;
			
			case PSP_UTILITY_DIALOG_FINISHED:
				done = 0;
				break;
				
			case PSP_UTILITY_DIALOG_NONE:
				break;

			default :
				break;
		}
		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}

	UCS2SJIS((unsigned char*)outtext, data.outtext, TEXT_LENGTH);

	return 1;
}
