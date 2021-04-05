#include <stdio.h>
#include <string.h>
#include "cptbl.h"
#include "converter.h"

#define IS_SJIS_ZENKAKU(x) ( (0x80 < x && x < 0xa0) || (0xdf < x && x < 0xf0) )

int sjis2ucs(int *uni, int sjis)
{
	static int cache = 0;
	int i, temp = sjis & 0xFF;
	if (cache)
	{
		temp = (temp << 8) | cache;
		cache = 0x00;
	}
	else if (IS_SJIS_ZENKAKU(temp))
	{
		cache = temp;
		*uni = -1;
		return 1;
	}
	for (i = 0; i < TOTAL_TBL; i++)
		if (map[i][0] == temp)
			break;
	if (i == TOTAL_TBL)
	{
		*uni = 0;
		return -1;
	}
	*uni = map[i][1];
	return 0;
}

int ucs2sjis(int *sjis, int uni)
{
	int i;
	for (i = 0; i < TOTAL_TBL; i++)
		if (map[i][1] == uni)
			break;
	if (i == TOTAL_TBL)
	{
		*sjis = 0;
		return -1;
	}
	*sjis = map[i][0];
	return 0;
}

void SJIS2UCS(unsigned short *dst, const unsigned char *src, unsigned int num)
{
	int sjis, len = 0;
	
    while (*src && (unsigned int)len < num)
    {
    	sjis2ucs(&sjis, *src++);
        if (sjis == -1)
        {
            continue;
        }
        *dst++ = (unsigned short)sjis;
        len++;
    }
    *dst = '\0';
}

void UCS2SJIS(unsigned char *dst, const unsigned short *src, unsigned int num)
{
	unsigned int sjis, len = 0;
	
    while (*src)
    {
    	if (ucs2sjis((int*)&sjis, (int)(*src++))) {
			sjis = 0xA181;										// �ϊ��ł��Ȃ�������'��'
		}
        if (sjis < 0x100)
        {
            *dst++ = sjis;
            len++;
        }
        else
        {
            if (len >= num - 2)
            {
                break;
            }
            *dst++ = sjis & 0xFF;
            *dst++ = (sjis>>8) & 0xFF;
            len += 2;
        }
        if (len >= num - 1)
        {
            break;
        }
    }
    *dst = '\0';
}

/**********************
������̃G���R�[�h�ϊ�
Shift JIS����EUC-JP��
**********************/
void SjisToEuc(char* dst, char* src)
{
    unsigned char hi, lo;

    while (*src)
    {
        // ���p�J�i
        if ((unsigned char)*src > 0xA0 && (unsigned char)*src < 0xE0)
        {
            *dst++ = 0x8E;
            *dst++ = *src++;
        }
        // 2�o�C�g����
        else if ((unsigned char)*src > 0x7F)
        {
            hi = *src++;
            lo = *src++;
            hi -= (hi <= 0x9f) ? 0x71 : 0xb1;
            hi <<= 1;
            hi++;
            if (lo > 0x7f)
                lo--;
            if (lo >= 0x9e) {
                lo -= 0x7d;
                hi++;
            }
            else {
                lo -= 0x1f;
            }
            *dst++ = hi | 0x80;
            *dst++ = lo | 0x80;
        }
        // ASCII����
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

/**********************
������̃G���R�[�h�ϊ�
EUC-JP����Shift JIS��
**********************/
void EucToSjis(char* dst, char* src)
{
    unsigned char hi, lo;

    while (*src)
    {
    	// X212(���݂��Ȃ�����"�H"�֕ϊ�
    	if ((unsigned char)*src == 0x8F)
    	{
    		src += 3;
    		*dst++ = 0x81;
    		*dst++ = 0x48;
    	}
        // ���p�J�i
        else if ((unsigned char)*src == 0x8E)
        {
            src++;
            *dst++ = *src++;
        }
        // 2�o�C�g����
        else if ((unsigned char)*src > 0x7F)
        {
            hi = *src++ & 0x7F;
            lo = *src++ & 0x7F;
            hi -= 0x21;
            if (hi & 1)
            {
                lo += 0x7E;
            }
            else
            {
                lo += 0x1F;
                if (lo>= 0x7F)
                {
                    lo++;
                }
            }
            hi >>= 1;
            hi += (hi <= 0x1E) ? 0x81 : 0xC1;
            *dst++ = hi;
            *dst++ = lo;
        }
        // ASCII����
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

int UCS2UTF8(unsigned char *dst, const unsigned short *src)
{
	int len = 0;
	while (*src)
	{
		if (*src <= 0x7F)
		{
			if (dst != NULL)
				*dst++ = *src;
			len++;
		}
		else if (*src >= 0x80 && *src <= 0x07FF)
		{
			if (dst != NULL)
			{
				*dst++ = ((*src >> 6) & 0x1F) | 0xC0;
				*dst++ = (*src & 0x3F) | 0x80;
			}
			len += 2;
		}
		else
		{
			if (dst != NULL)
			{
				*dst++ = (*src >> 12) | 0xE0;
				*dst++ = ((*src >> 6) & 0x3F) | 0x80;
				*dst++ = (*src & 0x3F) | 0x80;
			}
			len += 3;
		}
		src++;
	}
	if (dst != NULL)
		*dst = '\0';
	return len;
}

/*********************
src�������URL�G���R�[�h����dst������Ɋi�[�@
dst�̗e�ʂɒ���(src��3�{�����OK)
*********************/
void UrlEncode(char* dst, char* src)
{
    char buf[4];

    while (*src)
    {
        if ((*src >= 0x30 && *src <= 0x39) || // '0'-'9'
            (*src >= 0x41 && *src <= 0x59) || // 'A'-'Z'
            (*src >= 0x61 && *src <= 0x79) || // 'a'-'z'
            *src == '-' || *src == '.' || *src == '_')
        {
            *dst++ = *src++;
        }
        else if (*src == ' ')
        {
            *dst++ = '+';
            src++;
        }
        else
        {
            sprintf(buf, "%02X", *(unsigned char*)src++);
            *dst++ = '%';
            *dst++ = buf[0];
            *dst++ = buf[1];
        }
    }
    *dst = '\0';
}

//==============================================================
// UTF8���V�t�gJIS�֕����R�[�h�ϊ�
//--------------------------------------------------------------

void UTF82Sjis(char *dst,char *text)
{
	int		sjis, uni;

	while (*text) {
		if ((unsigned char)*text <= 0x7F){						// 1�o�C�g�R�[�h�iASCII�j
			*dst++ = *text++;
		} else {
			if ((unsigned char)*text <= 0xDFU){					// 2�o�C�g�R�[�h
				uni = ((text[0] & 0x001fu) << 6) | (text[1] & 0x003fu);
				text += 2;
			} else if ((unsigned char)*text <= 0xEFU){			// 3�o�C�g�R�[�h
				uni = ((text[0] & 0x001fu) << 12) | ((text[1] & 0x003fu) << 6) | (text[2] & 0x003fu);
				text += 3;
			} else {
				text++;
				uni = -1;
			}
			if (ucs2sjis(&sjis, uni) == -1){
				sjis = 0x81A1;									// �ϊ��ł��Ȃ�������'��'
			}
			*dst++ = sjis & 0xFF;
			*dst++ = (sjis>>8) & 0xFF;
		}
	}
	*dst = '\0';
}

//==============================================================
// �V�t�gJIS��UTF8�֕����R�[�h�ϊ�
//--------------------------------------------------------------

void Sjis2UTF8(char *dst,char *text)
{
	unsigned short	ucs[512];

	SJIS2UCS( ucs, text, 512 );
	UCS2UTF8( dst, ucs );
}

//==============================================================
// Base64�ϊ�
//--------------------------------------------------------------

void Base64(char *dst,char *text)
{
	char *table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char *p;
	int pos, dat, bit;

	p = (unsigned char*)text;
	pos = 0;
	dat = 0;
	bit = 0;
	for (; *p; p++) {
		dat = dat << 8 | *p;
		for (bit += 8; bit >= 6; bit -= 6) {
			dst[pos++] = table[(dat >> (bit - 6)) & 0x3f];
		}
	}
	if (bit > 0) {
		dat <<= 6 - bit;
		dst[pos++] = table[dat & 0x3f];
	}
	for (; pos % 4;) {
		dst[pos++] = '=';
	}
	dst[pos] = '\0';
}