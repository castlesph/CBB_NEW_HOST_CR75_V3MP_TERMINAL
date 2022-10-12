#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include <ctos_clapi.h>
#include <Vwdleapi.h>
#include "..\Includes\aes.h"
#include "..\Includes\DESFire.h"
#include <ctos_clapi.h>

#define BLOCKN_VALUE    (1)
#define get_unaligned_4_le(__p) ((__p)[0] | (__p)[1] << 8 | (__p)[2] << 16 | (__p)[3] << 24)
#define IncreaseOK 1
#define DecreaseOK 2
#define BalanceOK 3

// define const baPrinterBufferLogo --> "CASTLES" Text & Graphic Logo
const BYTE baPrinterBufferLogo[]={ //Width=361, Height=46
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x18,0x18,0x8C,0xC4,0xC6,
	0x66,0x66,0x22,0x22,0x22,0x22,0x22,0x62,0x66,0x46,0xC4,0x84,0x8C,0x18,0x10,0x30,
	0x60,0xC0,0xC0,0xE0,0x38,0x0E,0x07,0x1E,0x38,0xE0,0x80,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0xC0,0xF8,0x0E,0x03,0x81,0xF0,0x1C,0x0E,0x03,0xC1,0xF0,0xF8,
	0x7C,0x3C,0x1C,0x1E,0x1E,0x1E,0xFE,0xFC,0xFC,0xFC,0xF8,0xF0,0xC1,0x83,0xCE,0x7C,
	0x1C,0x07,0x81,0xE0,0x70,0x1C,0x0E,0x1C,0x70,0xC0,0x83,0x07,0x1C,0x70,0xE0,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x0F,0x7F,0xC0,0x00,0x03,0x3F,0xF0,0x80,0x00,0x0F,0x3F,0x7F,
	0xFF,0xFC,0xFC,0xFC,0xFC,0xFE,0xF3,0xFB,0xFF,0xFF,0x7F,0x3F,0x0F,0x03,0x81,0xE0,
	0x38,0x0E,0x03,0xC0,0xF0,0x1C,0x1C,0x38,0xE0,0x81,0x03,0x0E,0x38,0x70,0xC0,0x03,
	0x0E,0x1C,0x70,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,
	0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x08,0x18,0x38,0x30,0x00,0x00,0x00,0x00,
	0x80,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x80,0xC0,
	0xC0,0xC0,0xC0,0x80,0x00,0x00,0xC0,0xFC,0xFC,0xFC,0xC0,0xC0,0x00,0xFC,0xFC,0xFC,
	0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,
	0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,
	0x0C,0xFC,0xFC,0xFC,0x0C,0x0C,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,
	0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,
	0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,
	0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,
	0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,
	0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,
	0xC0,0xC0,0xC0,0x80,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x40,0xC0,0xC0,0x80,0x00,0x00,
	0x00,0x00,0x80,0xC0,0xC0,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,
	0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x38,0x30,0x00,0x00,0x00,0x00,
	0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0xC0,0xC0,0xFC,0xFC,0xFC,0xC0,0x00,0x00,
	0x00,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x0E,0x18,0x30,0x61,0x63,0xC6,0x8C,0x88,
	0x98,0x98,0x11,0x11,0x11,0x11,0x11,0x11,0x90,0x98,0x88,0x8C,0xC6,0x63,0x61,0x30,
	0x18,0x0C,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x1C,0x70,0xE0,0x81,0x07,
	0x0E,0x38,0xE0,0xC1,0x07,0x0E,0x38,0x70,0xC0,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,
	0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x00,0x3F,
	0xFF,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0xCF,0x8F,
	0x9E,0x9C,0xFC,0xF8,0x20,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,
	0x00,0x00,0x3E,0x7F,0xFF,0xCD,0x8C,0x8C,0x8C,0x8C,0xCF,0xCF,0x0E,0x08,0x00,0x03,
	0xC7,0x8F,0x8E,0x9C,0xFC,0xF9,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3E,0x7F,0xFF,0xCD,0xCC,0x8C,0x8C,0x8C,
	0xCF,0xCF,0x0F,0x0C,0x00,0x00,0x1C,0x7F,0xFF,0xE3,0xC1,0x80,0x80,0x80,0xC1,0xC1,
	0x41,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x01,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x01,0xFF,0xFF,0xFF,0x00,0x00,0x08,0x3E,0x7F,0xFF,
	0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x3E,
	0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xE3,0x7F,0x3F,0x08,0x00,0x0C,0x7F,0xFF,0xE3,
	0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0x0F,0x3F,0xFC,0xF0,
	0xF8,0x7E,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,
	0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x08,0x3E,
	0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0x80,0xC0,0xC0,0x00,
	0x00,0x00,0x00,0x00,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x1C,
	0x7F,0xFF,0xE1,0xC0,0x80,0x80,0xC0,0xE1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xC0,0xC0,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
	0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,
	0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,
	0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0E,0x0E,
	0x0D,0x09,0x09,0x08,0x0C,0x0F,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x0C,0x0F,0x0F,
	0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

ULONG ulAPRtn;
BYTE  baATQA[2], baSAK, bCSN_Len, baCSN[20];
char  baInput[20];
int   iLen;
//BYTE baTemp[384*520]; //The width of paper is 384 dot.

void PrinterMsg(int Msg, int Value, int Balance)
{
    ULONG ulValue;
    STR sBuf[80], TransDate[11], TransTime[9];
    USHORT iXPos, iYPos, j;
    
    CTOS_RTC stRTC;
    CTOS_FONT_ATTRIB FONT_ATTRIB;
    BYTE baTemp[384*520]; //The width of paper is 384 dot.
    iXPos = 10;
    iYPos = 0;
    j = 30;
    
    if(Msg == BalanceOK) return;
    
    CTOS_PrinterLogo((BYTE *)baPrinterBufferLogo, 0 , 361, 6);  //Print the "CASTLES" Text & Graphic Logo //
    
    CTOS_LanguagePrinterFontSize(d_FONT_12x24, 0, d_FONT_TYPE1);

    //Set the font attribut //
    FONT_ATTRIB.FontSize = d_FONT_12x24; //Font Size = 12x24 //
    FONT_ATTRIB.X_Zoom = 1; //The width magnifies X_Zoom diameters //
    FONT_ATTRIB.Y_Zoom = 1; //The height magnifies Y_Zoom diameters //
    FONT_ATTRIB.X_Space = 0;
    memset(baTemp, 0x00, sizeof(baTemp));
    
    //Get Time
    CTOS_RTCGet(&stRTC);
    sprintf(TransDate, "%04d/%02d/%02d", 2000+(int)stRTC.bYear, (int)stRTC.bMonth, (int)stRTC.bDay);
    sprintf(TransTime, "%02d:%02d:%02d", (int)stRTC.bHour, (int)stRTC.bMinute, (int)stRTC.bSecond);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"CASTLES Co., Ltd. ");
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos+80, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"Consumer Receipt");
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos+80, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"Store : Castles_000001");
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos+40, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"============================ ");
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"Ref SN     : 00000001 ");
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"Date       : %s",TransDate);
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"Time       : %s",TransTime);
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"Staff ID   : 89131771 ");
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    sprintf(sBuf,"Terminal ID: 0001");
    CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
    
    memset(sBuf, 0x00, sizeof(sBuf));
    if(Msg == IncreaseOK)
    {
        sprintf(sBuf,"Deposit     : %d", Value);
        CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
        
        memset(sBuf, 0x00, sizeof(sBuf));
        sprintf(sBuf,"Balance    : %d", Balance);
        CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
        CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), "Response   : Deposit OK", &FONT_ATTRIB);
    }
    
    if(Msg == DecreaseOK)
    {
        sprintf(sBuf,"Credit     : %d", Value);
        CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
        
        memset(sBuf, 0x00, sizeof(sBuf));
        sprintf(sBuf,"Balance    : %d", Balance);
        CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
        CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), "Response   : Credit OK", &FONT_ATTRIB);
    }
    
    if(Msg == BalanceOK)
    {
        sprintf(sBuf,"Balance    : %d", Balance);
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), sBuf, &FONT_ATTRIB);
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, iXPos, (iYPos+=j), "Response   : Balance OK", &FONT_ATTRIB);
    }
    
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 65);
}

void Mifare_Auth(void)
{
    ulAPRtn = CTOS_MifareLOADKEY ("\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F");
    if(ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Load Fail");
    
    ulAPRtn = CTOS_MifareAUTHEx (0x60, BLOCKN_VALUE, baCSN, bCSN_Len);
    if(ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Auth Fail");
}

void ShowBalance(void)
{
    BYTE block[16];
    unsigned value;
    char buffer[32];
    
    CTOS_LCDTClearDisplay();
     
    while(1)
    {
        // detect card
        CTOS_LCDTPrintXY(1, 1, "Detect Card");
        ulAPRtn=CTOS_CLTypeAActiveFromIdle (d_CL_BR_106, baATQA , &baSAK,  baCSN, &bCSN_Len);
        if (ulAPRtn == d_OK)
            break;
    }
    
    Mifare_Auth();
    
    ulAPRtn = CTOS_MifareREADBLOCK (BLOCKN_VALUE, block);
    if (ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Read Fail");
    else
    {
        value = get_unaligned_4_le(block);	
        sprintf(buffer, "Balance=%d", value);
        CTOS_LCDTPrintXY(1, 4, buffer);
        PrinterMsg(BalanceOK, (unsigned)1, value);
    }
}

void EzInitCard(void)
{
    CTOS_LCDTClearDisplay();
     
    while(1)
    {
        // detect card
        CTOS_LCDTPrintXY(1, 1, "Detect Card");
        ulAPRtn=CTOS_CLTypeAActiveFromIdle (0, baATQA , &baSAK,  baCSN, &bCSN_Len);
        if (ulAPRtn == d_OK)
            break;
    }
    
    Mifare_Auth();
    
    ulAPRtn = CTOS_MifareWRITEBLOCK (BLOCKN_VALUE ,"\x00\x00\x00\x00\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\xFF\x00\xFF");
    if(ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Write Fail");
    else
        CTOS_LCDTPrintXY(1, 1,"Init card OK");
}

int EzModifyValue(int action, unsigned long amount)
{
    BYTE block[16];
    unsigned value;
    char buffer[32];
    char cValue[4];
    
    CTOS_LCDTClearDisplay();
    
    cValue[0] = (amount & 0xFF);
    cValue[1] = ((amount >> 8) & 0xFF);
    cValue[2] = ((amount >> 16) & 0xFF);
    cValue[3] = ((amount >> 24) & 0xFF);
     
    while(1)
    {
        // detect card
        CTOS_LCDTPrintXY(1, 1, "Detect Card");
        ulAPRtn = CTOS_CLTypeAActiveFromIdle (0, baATQA , &baSAK,  baCSN, &bCSN_Len);
        if (ulAPRtn == d_OK)
            break;
    }
    
    Mifare_Auth();
    
    ulAPRtn = CTOS_MifareREADBLOCK (BLOCKN_VALUE, block);
    if (ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Read Fail");
    else
    {
        value = get_unaligned_4_le(block);	
        //sprintf(buffer, "Balance=%d", value);
        //CTOS_LCDTPrintXY(1, 4, buffer);
    }
    
    if (action == 0)
    {
        // increase value
        
        ulAPRtn = CTOS_MifareINCREASE (BLOCKN_VALUE, cValue);
    }
    else
    {
        if(value < amount)
        {
            sprintf(buffer, "Balance=%d", value);
            CTOS_LCDTPrintXY(1, 3, "Insufficient");
            CTOS_LCDTPrintXY(1, 4, "credit!");
            CTOS_LCDTPrintXY(1, 5, buffer);
            return 0;
        }
        else
        {
            // decrease value
            ulAPRtn = CTOS_MifareDECREASE (BLOCKN_VALUE, cValue);
        }
    }
    
    if (ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Inc/Dec Fail");
    
    ulAPRtn = CTOS_MifareTRANSFER (BLOCKN_VALUE);
    if (ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Trans Fail");
    
    ulAPRtn = CTOS_MifareREADBLOCK (BLOCKN_VALUE, block);
    if (ulAPRtn != d_OK)
        CTOS_LCDTPrintXY(1, 2, "Read Fail");
    else
    {
        value = get_unaligned_4_le(block);	
        sprintf(buffer, "Balance=%d", value);
        CTOS_LCDTPrintXY(1, 4, buffer);
    }
    
    if(action == 0)
        PrinterMsg(IncreaseOK, amount, value);
    else if(action == 1)
        PrinterMsg(DecreaseOK, amount, value);
}

BYTE InputValue(void) 
{
    int	iX, iY;
    BYTE bKey;
    
    CTOS_LCDTClearDisplay();
    
    memset(baInput, '0', 8);
    CTOS_LCDTPrintXY(1, 3, "INPUT AMOUNT\n");
    CTOS_LCDTPrintXY(1, 4, "( 0...9999999 )");
    iLen = 0;
    iX = 1;
    iY = 6;
    
    while(1) 
    {
        CTOS_KBDGet(&bKey);
        switch(bKey)
        {
            case d_KBD_ENTER:	//Enter
                if (iLen < 1)
                {
                    CTOS_Delay(100);
                    CTOS_Beep();
                    CTOS_Delay(100);
                    CTOS_Beep();
                }
                else
                {
                    if(iLen==1 && baInput[0] =='0')
                    {
                        CTOS_Delay(100);
                        CTOS_Beep();
                        CTOS_Delay(100);
                        CTOS_Beep();
                        iX--;
                        CTOS_LCDTPutchXY(iX, iY, ' ');
                        iLen--;
                    }
                    else
                        return TRUE;
                }
                break;
            case d_KBD_CANCEL:	//Cancel
                iLen = 1;
                baInput[0] = '0';
                return FALSE;
            case d_KBD_CLEAR:	//Clear
                if (iLen == 0)
                {
                    CTOS_Delay(100);
                    CTOS_Beep();
                    CTOS_Delay(100);
                    CTOS_Beep();
                }
                else
                {
                    iX--;
                    CTOS_LCDTPutchXY(iX, iY, ' ');
                    iLen--;
                }
                break;
            default:
                if ((bKey>='0' && bKey<='9') && iLen<7)
                {
                    CTOS_LCDTPutchXY(iX, iY, bKey);
                    iX++;
                    baInput[iLen++]=bKey;
                }
                else
                {
                    CTOS_Delay(100);
                    CTOS_Beep();
                    CTOS_Delay(100);
                    CTOS_Beep();
                }
        }
    }
}

ULONG ASCII2Int(BYTE *baSBuf, int iL) 
{
    unsigned long k, iN;
    int i;
    iN=0;
    k=1;
    for(i=iL-1; i>=0; i--)
    {
        iN = (unsigned long)((baSBuf[i]-'0')*k)+iN;
        k *= 10;
    }
    return iN;
}

void MainMenu(void)
{
    CTOS_LCDTPrintXY(1, 1, "V5CL Mifare");
    CTOS_LCDTPrintXY(1, 2, "1. Init New Card");
    CTOS_LCDTPrintXY(1, 3, "2. Show Balance");
    CTOS_LCDTPrintXY(1, 4, "3. Store Value");
    CTOS_LCDTPrintXY(1, 5, "4. Deduct Value");
    CTOS_LCDTPrintXY(1, 8, "X. Exit");
}

int inTestMifare(void) 
{
    BYTE bKey;
        
    // Open COM
    ulAPRtn = CTOS_CLInitComm(d_COMM_DEFAULT_BAUDRATE);
    if(ulAPRtn != d_OK)
    {
        CTOS_LCDTPrintXY(1, 8, "InitCom Fail");
        CTOS_KBDGet(&bKey);
        exit(0);
    }             
    
    while(1)
    {
        CTOS_LCDTClearDisplay();
        MainMenu();
        CTOS_KBDGet(&bKey);
        switch(bKey)
        {
            case '1': 
                EzInitCard();
                break;
            case '2':
                ShowBalance();
                break;
            case '3':
                if(InputValue() == TRUE)
                    EzModifyValue(0, ASCII2Int(baInput, iLen));
                break;
            case '4':
                if(InputValue() == TRUE)
                    EzModifyValue(1, ASCII2Int(baInput, iLen));
                break;
            case d_KBD_CANCEL:                
                return 0;
        }
        CTOS_KBDGet(&bKey);
    }
    
    exit(0);
}

//#define d_RX_TIMEOUT 0xFF01
#define d_DATA_ERROR 0xFF02


//PICC Master Key
const BYTE baMasterKey[16] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
//const BYTE baMasterKey[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

/******************************************************************************
		Value Data File
******************************************************************************/
void DebitValue(void)
{
		USHORT uRtn;

		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireDebit(0x04, 1);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Debit NG!\n");
			return;
		}
		CTOS_LCDTPrint("Debit OK!\n");
		
		uRtn = CTOS_DesfireCommitTransaction();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("CommitTransaction NG!\n");
			return;
		}
		CTOS_LCDTPrint("CommitTransaction OK!\n");		
}

//***********************************************************
void CreditValue(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireCredit(0x04, 1);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Credit NG!\n");
			return;
		}
		CTOS_LCDTPrint("Credit OK!\n");
		
		uRtn = CTOS_DesfireCommitTransaction();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("CommitTransaction NG!\n");
			return;
		}
		CTOS_LCDTPrint("CommitTransaction OK!\n");
		
}
//***********************************************************
void GetValue(void)
{
		USHORT uRtn;
		ULONG ulValue;
		BYTE ShowData[64];

		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireGetValue(0x04, &ulValue);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("GetValue NG!\n");
			return;
		}
		CTOS_LCDTPrint("GetValue OK!\n");
		
		memset(ShowData, 0x00, sizeof(ShowData));
		sprintf(ShowData,"Balance : %ld\n",ulValue);
		CTOS_LCDTPrint(ShowData);
		
}

//***********************************************************
void CreateValueFile(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireCreateValueFile(0x04, 0x00, 0x0000, 10, 1000, 500, 0x00);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Create Value NG!\n");
			return;
		}
		CTOS_LCDTPrint("Create Value OK!\n");
		
}

/******************************************************************************
													Cyclic Data File
******************************************************************************/
void ClearCyclicRecord(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireClearRecordFile(0x03);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Clear NG!\n");
			return;
		}
		CTOS_LCDTPrint("Clear OK!\n");
		
		uRtn = CTOS_DesfireCommitTransaction();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("CommitTransaction NG!\n");
			return;
		}
		CTOS_LCDTPrint("CommitTransaction OK!\n");
		
}

//***********************************************************
void ReadCyclicRecord(void)
{
		USHORT uRtn;
		BYTE bReadData[8];
		int i;
		BYTE ShowData[64];
		ULONG ulDataLen;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireReadRecords(0x03, 0, 0, bReadData, &ulDataLen);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Read NG!\n");
			return;
		}
		CTOS_LCDTPrint("Read OK!\n");
		
		memset(ShowData, 0x00, sizeof(ShowData));
		sprintf(ShowData,"%02X %02X %02X %02X \n%02X %02X %02X %02X\n",
				bReadData[0],bReadData[1],bReadData[2],bReadData[3],
				bReadData[4],bReadData[5],bReadData[6],bReadData[7]);
				
		CTOS_LCDTPrint(ShowData);
		
}

//***********************************************************
void WriteCyclicRecord(void)
{
		USHORT uRtn;
		BYTE bWriteData[8];
		int i;
		
		for(i = 0 ; i < 8; i++)
			bWriteData[i] = i;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireWriteRecord(0x03, 0, 8, bWriteData);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Write NG!\n");
			return;
		}
		CTOS_LCDTPrint("Write OK!\n");
		
		uRtn = CTOS_DesfireCommitTransaction();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("CommitTransaction NG!\n");
			return;
		}
		CTOS_LCDTPrint("CommitTransaction OK!\n");
		
}

//***********************************************************
void CreateCyclicFile(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireCreateCyclicRecordFile(0x03, "", 0x00, 0x0000, 8, 2, FALSE);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Create Cyclic NG!\n");
			return;
		}
		CTOS_LCDTPrint("Create Cyclic OK!\n");
		
}


/******************************************************************************
													Linear Data File
******************************************************************************/
void ClearLinearRecord(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireClearRecordFile(0x02);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Clear NG!\n");
			return;
		}
		CTOS_LCDTPrint("Clear OK!\n");
		
		uRtn = CTOS_DesfireCommitTransaction();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("CommitTransaction NG!\n");
			return;
		}
		CTOS_LCDTPrint("CommitTransaction OK!\n");
		
}

//***********************************************************
void ReadLinearRecord(void)
{
		USHORT uRtn;
		BYTE bReadData[8];
		int i;
		BYTE ShowData[64];
		ULONG ulDataLen;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireReadRecords(0x02, 0, 0, bReadData, &ulDataLen);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Read NG!\n");
			return;
		}
		CTOS_LCDTPrint("Read OK!\n");
		
		memset(ShowData, 0x00, sizeof(ShowData));
		sprintf(ShowData,"%02X %02X %02X %02X \n%02X %02X %02X %02X\n",
				bReadData[0],bReadData[1],bReadData[2],bReadData[3],
				bReadData[4],bReadData[5],bReadData[6],bReadData[7]);
				
		CTOS_LCDTPrint(ShowData);
		
}

//***********************************************************
void WriteLinearRecord(void)
{
		USHORT uRtn;
		BYTE bWriteData[8];
		int i;
		
		for(i = 0 ; i < 8; i++)
			bWriteData[i] = i;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireWriteRecord(0x02, 0, 8, bWriteData);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Write NG!\n");
			return;
		}
		CTOS_LCDTPrint("Write OK!\n");
		
		uRtn = CTOS_DesfireCommitTransaction();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("CommitTransaction NG!\n");
			return;
		}
		CTOS_LCDTPrint("CommitTransaction OK!\n");
		
}

//***********************************************************
void CreateLinearFile(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireCreateLinearRecordFile(0x02, "", 0x00, 0x0000, 8, 1, FALSE);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Create Linear NG!\n");
			return;
		}
		CTOS_LCDTPrint("Create Linear OK!\n");
		
}

/******************************************************************************
													Backup Data File
******************************************************************************/
void ReadBackupData(void)
{
		USHORT uRtn;
		BYTE bReadData[8];
		int i;
		BYTE ShowData[64];
		ULONG ulDataLen;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireReadData(0x01, 0, 8, bReadData, &ulDataLen);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Read NG!\n");
			return;
		}
		CTOS_LCDTPrint("Read OK!\n");
		
		memset(ShowData, 0x00, sizeof(ShowData));
		sprintf(ShowData,"%02X %02X %02X %02X \n%02X %02X %02X %02X\n",
				bReadData[0],bReadData[1],bReadData[2],bReadData[3],
				bReadData[4],bReadData[5],bReadData[6],bReadData[7]);
				
		CTOS_LCDTPrint(ShowData);
		
}

//***********************************************************
void WriteBackupData(void)
{
		USHORT uRtn;
		BYTE bWriteData[8];
		int i;
		
		for(i = 0 ; i < 8; i++)
			bWriteData[i] = i;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireWriteData(0x01, 0, 8, bWriteData);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Write NG!\n");
			return;
		}
		CTOS_LCDTPrint("Write OK!\n");
		
		uRtn = CTOS_DesfireCommitTransaction();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("CommitTransaction NG!\n");
			return;
		}
		CTOS_LCDTPrint("CommitTransaction OK!\n");
		
}

//***********************************************************
void CreateBackupFile(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireCreateBackupDataFile(0x01, "", 0x0000, 0x00, 8, FALSE);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Create Backup NG!\n");
			return;
		}
		CTOS_LCDTPrint("Create Backup OK!\n");
		
}

/******************************************************************************
													Std Data File
******************************************************************************/
void ReadStdData(void)
{
		USHORT uRtn;
		BYTE bReadData[8];
		int i;
		BYTE ShowData[64];
		ULONG ulDataLen;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireReadData(0x00, 0, 8, bReadData, &ulDataLen);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Read NG!\n");
			return;
		}
		CTOS_LCDTPrint("Read OK!\n");
		
		memset(ShowData, 0x00, sizeof(ShowData));
		sprintf(ShowData,"%02X %02X %02X %02X \n%02X %02X %02X %02X\n",
				bReadData[0],bReadData[1],bReadData[2],bReadData[3],
				bReadData[4],bReadData[5],bReadData[6],bReadData[7]);
				
		CTOS_LCDTPrint(ShowData);
		
}

//***********************************************************
void WriteStdData(void)
{
		USHORT uRtn;
		BYTE bWriteData[8];
		int i;
		
		for(i = 0 ; i < 8; i++)
			bWriteData[i] = i;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireWriteData(0x00, 0, 8, bWriteData);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Write NG!\n");
			return;
		}
		CTOS_LCDTPrint("Write OK!\n");
		
}

//***********************************************************
void CreateStdFile(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x0A\x0B\x0C");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireCreateStdDataFile(0x00, "", 0x0000, 0x00, 8, FALSE);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Create Std NG!\n");
			return;
		}
		CTOS_LCDTPrint("Create Std OK!\n");
		
}

//***********************************************************
void CreateAP(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x00\x00\x00");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireCreateApplication("\x0A\x0B\x0C", 0x0F, 0x04, "", "", 0, FALSE);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Create AP NG!\n");
			return;
		}
		CTOS_LCDTPrint("Create AP OK!\n");
		
}

//***********************************************************
void FormatPICC(void)
{
		USHORT uRtn;
		
		uRtn = CTOS_DesfireSelectApplication("\x00\x00\x00");
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Select AP NG!\n");
                        return;
		}
		CTOS_LCDTPrint("Select AP OK!\n");
		
		uRtn = CTOS_DesfireAuthenticate(0x00, (BYTE *)baMasterKey, 16);
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Auth NG!\n");
			return;
		}
		CTOS_LCDTPrint("Auth OK!\n");
		
		uRtn = CTOS_DesfireFormatPICC();
		if(uRtn != 0x00)
		{
			CTOS_LCDTPrint("Format NG!\n");
			return;
		}
		CTOS_LCDTPrint("Format OK!\n");
}

//***********************************************************
USHORT ActiveCard(void)
{
    USHORT uRtn;
    BYTE baATQA[16];
    BYTE bSAK[16];
    BYTE baCSN[16];
    BYTE bCSNLen;
    BYTE baATS[16];
    USHORT bATSLen;
    
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrint("Draw Contactless Card\n");
    do{
        uRtn = CTOS_CLTypeAActiveFromIdle(0,baATQA,bSAK,baCSN,&bCSNLen);
    }while(uRtn != 0);
    
    uRtn =  CTOS_CLRATS(0,baATS,&bATSLen); 
    if(uRtn != d_OK)  
    {
        CTOS_LCDTPrint("RATS NG\n");
        return uRtn;
    }    
    CTOS_LCDTPrint("RATS OK\n");
    CTOS_LCDTClearDisplay();
    return uRtn;
}
//***********************************************************
void ValueMenu(void)
{
    BYTE bKey;
      
    while(1)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1,1,"  Value   Menu  ");
        CTOS_LCDTPrintXY(1,2,"1.Create Value");
        CTOS_LCDTPrintXY(1,3,"2.Get Value");
        CTOS_LCDTPrintXY(1,4,"3.Credit");
        CTOS_LCDTPrintXY(1,5,"4.Debit");
        CTOS_KBDGet(&bKey);
        
        switch(bKey)
        {
            case '1':
                ActiveCard();
            	CreateValueFile();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '2':
                ActiveCard();
            	GetValue();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '3':
                ActiveCard();
            	CreditValue();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '4':
                ActiveCard();
            	DebitValue();
                CTOS_KBDGet(&bKey);
            	break;
                
            case d_KBD_CANCEL:
                return;
        }
    }
}
//***********************************************************
void CyclicMenu(void)
{
    BYTE bKey;
      
    while(1)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1,1,"  Cyclic  Menu  ");
        CTOS_LCDTPrintXY(1,2,"1.Create Linear");
        CTOS_LCDTPrintXY(1,3,"2.Write Record");
        CTOS_LCDTPrintXY(1,4,"3.Read Record");
        CTOS_LCDTPrintXY(1,5,"4.Clear Record");
        CTOS_KBDGet(&bKey);
        switch(bKey)
        {
            case '1':
                ActiveCard();
            	CreateCyclicFile();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '2':
                ActiveCard();
                WriteCyclicRecord();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '3':
                ActiveCard();
            	ReadCyclicRecord();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '4':
                ActiveCard();
            	ClearCyclicRecord();
                CTOS_KBDGet(&bKey);
            	break;
                
            case d_KBD_CANCEL:
                return;
        }
    }
}
//***********************************************************
void LinearMenu(void)
{
    BYTE bKey;
    
      
    while(1)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1,1,"  Linear  Menu  ");
        CTOS_LCDTPrintXY(1,2,"1.Create Linear");
        CTOS_LCDTPrintXY(1,3,"2.Write Record");
        CTOS_LCDTPrintXY(1,4,"3.Read Record");
        CTOS_LCDTPrintXY(1,5,"4.Clear Record");
        CTOS_KBDGet(&bKey);
        
        switch(bKey)
        {
            case '1':
                ActiveCard();
            	CreateLinearFile();
                CTOS_KBDGet(&bKey);
            	break;
                
            case '2':
                ActiveCard();
            	WriteLinearRecord();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '3':
                ActiveCard();
            	ReadLinearRecord();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '4':
                ActiveCard();
            	ClearLinearRecord();
                CTOS_KBDGet(&bKey);
            	break;
                
            case d_KBD_CANCEL:
                return;
        }
    }
}
//***********************************************************
void BackupMenu(void)
{
    BYTE bKey;
    
      
    while(1)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1,1,"  Backup  Menu  ");
        CTOS_LCDTPrintXY(1,2,"1.Create Backup");
        CTOS_LCDTPrintXY(1,3,"2.Write Data");
        CTOS_LCDTPrintXY(1,4,"3.Read Data");
        CTOS_KBDGet(&bKey);
        
        switch(bKey)
        {
            case '1':
                ActiveCard();
            	CreateStdFile();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '2':
                ActiveCard();
            	WriteStdData();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '3':
                ActiveCard();
            	ReadStdData();
                CTOS_KBDGet(&bKey);
            	break;
                
            case d_KBD_CANCEL:
                return;
        }
    }
}
//***********************************************************
void StdMenu(void)
{
    BYTE bKey;
    
      
    while(1)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1,1,"    Std Menu    ");
        CTOS_LCDTPrintXY(1,2,"1.Create Std");
        CTOS_LCDTPrintXY(1,3,"2.Write Data");
        CTOS_LCDTPrintXY(1,4,"3.Read Data");
        CTOS_KBDGet(&bKey);
        
        switch(bKey)
        {
            case '1':
                ActiveCard();
            	CreateBackupFile();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '2':
                ActiveCard();
            	WriteBackupData();
                CTOS_KBDGet(&bKey);
            	break;
            	
            case '3':
                ActiveCard();
            	ReadBackupData();
                CTOS_KBDGet(&bKey);
            	break;
                
            case d_KBD_CANCEL:
                return;
        }
        
    }
}
//***********************************************************
int DESFireDemo(void)
{
    USHORT uRtn;
    BYTE bKey;
    
    CTOS_CLInit();
    
    while(1)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1,1,"  DesFire Test  ");
        CTOS_LCDTPrintXY(1,2,"1.FormatPICC");
        CTOS_LCDTPrintXY(1,3,"2.CreateAP");
        CTOS_LCDTPrintXY(1,4,"3.StdFile");
        CTOS_LCDTPrintXY(1,5,"4.BackupFile");
        CTOS_LCDTPrintXY(1,6,"5.LinearFile");
        CTOS_LCDTPrintXY(1,7,"6.CyclicFile");
        CTOS_LCDTPrintXY(1,8,"7.ValueFile");
        CTOS_KBDGet(&bKey);
        
        switch(bKey)
        {
            case '1':
                ActiveCard();
                FormatPICC();
                CTOS_KBDGet(&bKey);
                break;
            
            case '2':
                ActiveCard();
                CreateAP();
                CTOS_KBDGet(&bKey);
                break;
                
            case '3':
                StdMenu();
                break;
                
            case '4':
                BackupMenu();
                break;
                
            case '5':
                LinearMenu();
                break;
                
            case '6':
                CyclicMenu();
                break;
                
            case '7':
                ValueMenu();
                break;
                
            case d_KBD_CANCEL:
                return 0;
                break;
        }
        
    }
    return 0;
} 

/** 
** The main entry of the terminal application 
**/
int inDESFireDemo(void) 
{
	BYTE key;
        
	// TODO: Add your program here //
	DESFireDemo();
}

