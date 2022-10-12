#ifndef _RAS_H
#define _RAS_H

#ifndef _STD_TYPES
#define _STD_TYPES

//#define __arm

#define uchar   unsigned char
#ifdef __arm
#define uint    unsigned short
#define ulong   unsigned  long
#else
#define uint    unsigned int
#define ulong   unsigned long
#endif

void RSA_Test(void);
void vdGenerateRandomKey(int inLen, BYTE *ptrResult);

int inRSA_EncryptPublicKey(unsigned char *randomkey, unsigned char *result);
void vdGenerateKCV(unsigned *key, unsigned *kcv, unsigned char fString);
#endif

#endif /* rsasrc.h */
