#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "graphics.h"
#include "color.h"

#define BLEND_FUNC( src_ch, dst_ch, alpha, maxbits ) ( ( ( ( src_ch ) + 1 - ( dst_ch ) ) * ( alpha ) >> ( maxbits ) ) + ( dst_ch ) )

#define VRAM		((long(*)[512])(((char*)0x4000000)+0x40000000))

typedef struct
{
	int	wx;
	int	wy;
	int	width;
	int	height;
	long	*data;
} DrawImage;

int drawBox(Color color, int x0, int y0, int width, int height) {
	drawLineScreen(x0, y0, x0 + width, y0, color);
	drawLineScreen(x0, y0, x0, y0 + height, color);
	drawLineScreen(x0, y0 + height, x0 + width, y0 + height, color);
	drawLineScreen(x0 + width, y0, x0 + width, y0 + height, color);
	return 0;
}

inline u32 AlphaBlend( u8 alpha, u32 src, u32 dst )
{
	if( ! alpha )
	{
		return dst;
	}
	else if( alpha == 0xFF )
	{
		return src;
	}
	else
	{
		u32 color = alpha << 24;
		color |= BLEND_FUNC( ( src       ) & 0xFF, ( dst       ) & 0xFF, alpha, 8 );
		color |= BLEND_FUNC( ( src >>  8 ) & 0xFF, ( dst >>  8 ) & 0xFF, alpha, 8 ) << 8;
		color |= BLEND_FUNC( ( src >> 16 ) & 0xFF, ( dst >> 16 ) & 0xFF, alpha, 8 ) << 16;
		return color;
	}
}

void DrawAlphaPixelScreen(u8 alpha, Color color, int x, int y)
{
	Color* vram = getVramDrawBuffer();
	vram[PSP_LINE_SIZE * y + x] = AlphaBlend(alpha, color, getPixelScreen(x, y) );
}

void HorizontalAlphaLine(u8 alpha, Color color, int x, int y, int width)
{
	int i;

	for(i = 0; i  < width; i++, x++) {
		DrawAlphaPixelScreen(alpha, color, x, y);
	}
}

void VerticalAlphaLine(u8 alpha, Color color, int x, int y, int height)
{
	int i;

	for(i = 0; i  < height; i++, y++) {
		DrawAlphaPixelScreen(alpha, color, x, y);
	}
}

static void DrawAlphaLine(int x0, int y0, int x1, int y1, u8 alpha, int color, Color* destination, int width)
{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	
	if (dy < 0) { dy = -dy;  stepy = -width; } else { stepy = width; }
	if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
	dy <<= 1;
	dx <<= 1;
	
	y0 *= width;
	y1 *= width;
	destination[x0+y0] = color;
	if (dx > dy) {
		int fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			destination[x0+y0] = AlphaBlend(alpha, color, getPixelScreen(x0, y0) );
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			destination[x0+y0] = AlphaBlend(alpha, color, getPixelScreen(x0, y0) );
		}
	}
}

void DrawAlphaLineScreen(u8 alpha, Color color, int x0, int y0, int x1, int y1)
{
	DrawAlphaLine(x0, y0, x1, y1, alpha, color, getVramDrawBuffer(), PSP_LINE_SIZE);
}

void DrawAlphaBoxScreen(u8 alpha, Color color, int x0, int y0, int width, int height)
{
	HorizontalAlphaLine(alpha, color, x0, y0, width);
	VerticalAlphaLine(alpha, color, x0, y0, height);
	HorizontalAlphaLine(alpha, color, x0, y0 + height, width + 1);
	VerticalAlphaLine(alpha, color, x0 + width, y0, height);
}

void DrawAlphaFillBoxScreen(u8 alpha, Color color, int x0, int y0, int width, int height)
{
	int i;

	if(width == 0 || height == 0) return;

	for(i = 0; i < height; i++, y0++) HorizontalAlphaLine(alpha, color, x0, y0,width);
}

void DrawAlphaFillBoxScreen2(u8 alpha, Color color, int x0, int y0, int width, int height)
{
	int skipX = PSP_LINE_SIZE - width;
	int x, y;
	Color* data = getVramDrawBuffer() + x0 + y0 * PSP_LINE_SIZE;
	for (y = 0; y < height; y++, data += skipX) {
		for (x = 0; x < width; x++, data++) *data = AlphaBlend(alpha, color, getPixelScreen(x+x0, y+y0) );
	}
}

void ScreenCopy(int screen)
{
	Color *dVram;

	if (screen) {
		dVram = getVramDrawBuffer();
	} else {
		dVram = getVramDisplayBuffer();
	}

	sceKernelDcacheWritebackInvalidateAll();
	guStart();
	sceGuCopyImage( GU_PSM_8888, 0, 0, 480, 272, 512, dVram, 0, 272*2.5, 512, VRAM );
	sceGuFinish();
	sceGuSync(0,0);
}

void ScreenPaste(void)
{
	Color *dVram = getVramDrawBuffer();

	guStart();
	sceGuCopyImage( GU_PSM_8888, 0, 272*2.5, 480, 272, 512, VRAM, 0, 0, 512, dVram );
	sceGuFinish();
	sceGuSync(0,0);
}