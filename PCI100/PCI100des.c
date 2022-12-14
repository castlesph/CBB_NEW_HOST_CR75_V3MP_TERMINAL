// mac.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "PCI100des.h"
//#include <svc.h>



int	InitialTr[64] = {
		58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
		62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
		57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
		61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
	};

int	FinalTr[64] = {
		40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
		38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
		36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
		34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
	};

int	KeyTr1[56] = {
		57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18, 10,  2,
		59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36, 63, 55, 47, 39,
		31, 23, 15,  7, 62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37,
		29, 21, 13,  5, 28, 20, 12,  4
	};

int	KeyTr2[48] = {
		14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10, 23, 19, 12,  4,
		26,  8, 16,  7, 27, 20, 13,  2, 41, 52, 31, 37, 47, 55, 30, 40,
		51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
	};

int	Etr[48] = {
		32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,  8,  9, 10, 11,
		12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21,
		22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
	};

int	Ptr[32] = {
		16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
		 2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
	};

int	S[8][64] = {

	{   14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 },

	{   15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 },

	{   10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 },

	{    7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 },

	{    2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 },

	{   12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 },

	{    4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 },

	{   13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
	};

int	Rots[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1	};

/***************************************************************************
 SUPPOSE dsp CONTAINS THE ASSCII ARRAY "12345F" AND WE EXECUTE THIS FUNCTION
  THEN THE ARRAY  AT hex WILL CONTAIN 12H, 34H,  5FH
****************************************************************************/
void PCI100_DSP_2_HEX(byte *dsp, byte *hex, int count)
{
	int i, c;

	for(i = 0; i<count; i++)
	{
		c = dsp[i*2];
		if(c<'0' || c>'f')
			continue;
		if(dsp[i*2] >= 'a')
			dsp[i*2] -= ('a'-'A');
		hex[i] = ((dsp[i*2] <= 0x39) ? dsp[i*2]-0x30 : dsp[i*2]-0x41+10);
		hex[i] = hex[i]<<4;
		if(dsp[i*2+1] >= 'a')
			dsp[i*2+1] -= ('a'-'A');
		hex[i] += ((dsp[i*2+1] <= 0x39) ? dsp[i*2+1]-0x30 : dsp[i*2+1]-0x41+10);
	}
}

void PCI100_XOR(byte *inOut, byte *ICV, int count)
{
	int i, c;

	for(i = 0; i<count; i++)
	{
		inOut[i] = inOut[i]^ICV[i];
	}
}


void PCI100_HEX_2_DSP(char *hex, char *dsp, int count)
{
	int i;
	char ch;
	for(i = 0; i<count; i++)
	{
		ch = (hex[i]&0xf0)>>4;
		dsp[i*2] = (ch>9) ? ch+0x41-10 : ch+0x30;
		ch = hex[i]&0xf;
		dsp[i*2+1] = (ch>9)?ch+0x41-10:ch+0x30;
	}
}

void PCI100_HEX_2_BIT(byte *inText, int *outText, int count)
{
	register int i, j;

	for(i = 0; i<count; i++)
		for(j = 0; j<8; j++)
			outText[i*8+j] = (inText[i]>>(7-j)) & 0x01;
}

void PCI100_BIT_2_HEX(int *inText, byte *outText, int count)
{
	register int i, j;

	for(i = 0; i<count; i++)
	{
		outText[i] = 0;
		for(j = 0; j<8; j++)
			outText[i] += inText[i*8+j]<<(7-j);
	}
}

void transpose(int *data, int *tr, int n)
{
	register int i, k;
	int      temp[64];

	for(i = 0;  i<n;  i++)
	{
		k = tr[i]-1;
		temp[i] = data[k];
	}
	for(i = 0;  i<n;  i++)
		data[i] = temp[i];
}

void xchange(int *a)
{
	register int i, aa;

	for(i = 0; i<32; i++)
	{
		aa = a[i];
		a[i] = a[i+32];
		a[i+32] = aa;
	}
}

void rotateleft(int *key)
{
	register int i, aa;

	aa = key[0];
	for (i = 0; i<27; i++)
		key[i] = key[i+1];
	key[27] = aa;
	aa = key[28];
	for(i = 28; i<55; i++)
		key[i] = key[i+1];
	key[55] = aa;
}

void rotateright(int *key)
{
	register int i, aa;

	aa = key[55];
	for(i = 55; i>28; i--)
		key[i] = key[i-1];
	key[28] = aa;
	aa = key[27];
	for(i = 27; i>0; i--)
		key[i] = key[i-1];
	key[0] = aa;
}

void fm(int func, int i, int *key, int *a, int *x)
{
	int e[48], ikey[56], y[48];
	int j, k, r;

	for(j = 0; j<32; j++)
		e[j] = a[j];
	transpose(e, Etr, 48);

	for(j = 1; func == 1 &&j <= Rots[i]; j++)
		rotateleft(key);
	for(j = 1; func == -1 && i>0 && j <= Rots[16-i]; j++)
		rotateright(key);

	for(j = 0; j<56; j++)
		ikey[j] = key[j];
	transpose(ikey, KeyTr2, 48);

	for(j = 0; j<48; j++)
		y[j] = (e[j] != ikey[j]) ? 1 : 0;
	for(k = 0; k<8; k++)
	{
		r = 32*y[6*k]+16*y[6*k+5]+8*y[6*k+1]+4*y[6*k+2]+2*y[6*k+3]+y[6*k+4];
		x[4*k]  = ((S[k][r]%16) >= 8) ? 1 : 0;
		x[4*k+1] = ((S[k][r]%8) >= 4) ? 1 : 0;
		x[4*k+2] = ((S[k][r]%4) >= 2) ? 1 : 0;
		x[4*k+3] = (S[k][r]%2);
	}
	transpose(x, Ptr, 32);
}

void xdes(int *plaintext, int *key, int *ciphertext, int func)
{
	int i, j;
	int a[64], x[32];

	for(i = 0; i<64; i++)
		a[i] = plaintext[i];
	transpose(a, InitialTr, 64);
	if(func == -1) xchange(a);
	transpose(key, KeyTr1, 56);
	for(i = 0; i<16; i++)
	{
		if(func == 1) xchange(a);
		fm(func, i, key, a, x);
		for(j = 0; j<32; j++)
			a[j+32] = (a[j+32] != x[j]) ? 1 : 0;
		if(func == -1) xchange(a);
	}

	if(func == 1) xchange(a);
	transpose(a, FinalTr, 64);
	for(i = 0; i<64; i++)
		ciphertext[i] = a[i];
}

void dess(byte *dkey, byte *tt, byte *cipher, char f)
{
	int aaa[64], bbb[64], ccc[64], flag = 1;

	PCI100_HEX_2_BIT(dkey, aaa, 8);

	PCI100_HEX_2_BIT(tt, bbb, 8);

	if ((f == 'd') || (f == 'D'))
		flag = -1;
	xdes(bbb, aaa, ccc, flag);
	PCI100_BIT_2_HEX(ccc, cipher, 8);
}

void asc_des(char *dkey, char *tt, char *cipher, char f)
{
	char buf[20], buf1[20], buf2[20];
	PCI100_DSP_2_HEX(dkey, buf, 8);
	PCI100_DSP_2_HEX(tt, buf1, 8);
	dess((byte *)buf, (byte *)buf1, (byte *)buf2, f);
	PCI100_HEX_2_DSP(buf2, cipher, 8);
	cipher[16] = 0;
}

void TripleDes_16Key(byte *dkey, byte *tt, byte *cipher, char f)
{
	byte buffer[10];

	if(f == 'e' || f == 'E')
	{
		dess(dkey, tt, cipher, 'E');
		dess(dkey+8, cipher, buffer, 'D');
		dess(dkey, buffer, cipher, 'E');
	}
	else
	{
		dess(dkey, tt, cipher, 'D');
		dess(dkey+8, cipher, buffer, 'E');
		dess(dkey, buffer, cipher, 'D');
	}
}


void TripleDes_24Key(byte *dkey, byte *tt, byte *cipher, char f)
{
	byte buffer[10];

	if(f == 'e' || f == 'E')
	{
		dess(dkey, tt, cipher, 'E');
		dess(dkey+8, cipher, buffer, 'D');
		dess(dkey+16, buffer, cipher, 'E');
	}
	else
	{
		dess(dkey, tt, cipher, 'D');
		dess(dkey+8, cipher, buffer, 'E');
		dess(dkey+16, buffer, cipher, 'D');
	}
}


void TripleMac(byte *key, byte *command, int slen, byte *cipher)
{
	byte buffer1[9], buffer2[9];
	register int i, j;
	int len1, len2;

	memset(buffer1, 0, 8);
	len1 = (slen-1)/8+1;
	for(i = 0; i<len1; i++)
	{
		len2 = (i < len1-1) ? 8 : (slen%8);
		for(j = 0; j<len2; j++)
			buffer1[j] ^= command[i*8+j];

		dess(key, buffer1, buffer1, 'e');
	}
	dess(key+8, buffer1, buffer2, 'd');
	dess(key, buffer2, cipher, 'e');
}

