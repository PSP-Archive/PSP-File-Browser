
#ifndef __DRAW_H__
#define __DRAW_H__

int drawBox(Color color, int x0, int y0, int width, int height);
inline u32 AlphaBlend( u8 alpha, u32 src, u32 dst );
extern void DrawAlphaPixelScreen(u8 alpha, Color color, int x, int y);
void HorizontalAlphaLine(u8 alpha, Color color, int x, int y, int width);
void VerticalAlphaLine(u8 alpha, Color color, int x, int y, int height);
void DrawAlphaLineScreen(u8 alpha, Color color, int x0, int y0, int x1, int y1);
void DrawAlphaBoxScreen(u8 alpha, Color color, int x0, int y0, int width, int height);
void DrawAlphaFillBoxScreen(u8 alpha, Color color, int x0, int y0, int width, int height);
void ScreenCopy(int screen);
void ScreenPaste(void);

#endif