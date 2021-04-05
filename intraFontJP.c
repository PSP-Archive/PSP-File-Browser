#include <stdio.h>
#include <string.h>
#include <pspgum.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include "graphics.h"
#include "intraFont.h"
#include "converter.h"

char utf8[1024];
u32 value;

intraFont* jpn0;	//日本語フォント

float intraFontPrintJP(int x, int y, char *text) {
	Sjis2UTF8(utf8, text);
	intraFontPrint(jpn0, x, y, utf8);
	return 0;
}

float intraFontPrintfJP(int x, int y, char *text, ...) {
	char buffer[1024];
	va_list ap;

	va_start(ap, text);
	vsnprintf(buffer, 1024, text, ap);
	va_end(ap);
	buffer[1023] = 0;

	Sjis2UTF8(utf8, buffer);

	intraFontPrint(jpn0, x, y, utf8);

	return 0;
}