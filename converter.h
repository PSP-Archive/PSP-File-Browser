

#ifndef __CHAR_CONV_H__
#define __CHAR_CONV_H__

int sjis2ucs(int *uni, int sjis);
int ucs2sjis(int *sjis, int uni);
void SJIS2UCS(unsigned short *dst, const unsigned char *src, unsigned int num);
void UCS2SJIS(unsigned char *dst, const unsigned short *src, unsigned int num);
void SjisToEuc(char* dst, char* src);
void EucToSjis(char* dst, char* src);
int UCS2UTF8(unsigned char *dst, const unsigned short *src);
void UrlEncode(char* dst, char* src);
void UTF82Sjis(char *dst,char *text);
void Sjis2UTF8(char *dst,char *text);
void Base64(char *dst,char *text);

#endif
