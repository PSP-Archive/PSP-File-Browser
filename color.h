#ifndef PB_COLOR_H
#define PB_COLOR_H

#define	SetAlpha(color,alpha)	((color & 0xffffff) | ((alpha & 0xff) << 24))
#define	GetAlpha(color)		((color & 0xff000000) >> 24)


#define	BLACK			0xff000000          //  F
#define	WHITE			0xffFFFFFF          //  F

#define	RED			0xff0000FF          //  ÔF
#define	GREEN			0xff00FF00          //  ÎF
#define	BLUE			0xffFF0000          //  ÂF

#define	DARKBLUE		0xff800000          //  ®F
#define	LIGHTBLUE		0xffFA0000          //  F
#define	BROWN			0xff404080          //  F
#define	PINK			0xffFF80FF          //  F
#define	DARKGREEN		0xff008000          //  [ÎF
#define	YELLOGREEN		0xff00FF80          //  ©ÎF
#define	PURPLE			0xffFF0080          //  F
#define	ORANGE			0xff4080FF          //  òF
#define	LEMON			0xff80FFFF          //  F
#define	YELLOW			0xff00FFFF          //  ©F
#define	SKYBLUE			0xffFFFF00          //  óF
#define	PEARLORANGE		0xff75BAFF          //  §F
#define	GRAY			0xff808080          //  DF
#define	SILVER			0xffC0C0C0          //  âF
#define	GOLD			0xff20C0E6          //  àF


#define	Invalid			LIBM_NO_DRAW


//    32rbgJ[Ì16iR[hð RGB©çì¬·évZ®
//#define  RGB(r,g,b) (((long)b<<16)|((long)g<<8)|((long)r)

#endif
