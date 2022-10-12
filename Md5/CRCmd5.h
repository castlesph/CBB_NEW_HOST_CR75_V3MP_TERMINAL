

#ifndef ___CRCMD5_H___
#define	___CRCMD5_H___

#ifdef	__cplusplus
extern "C" {
#endif


typedef unsigned       int uint4;
typedef unsigned short int uint2;
typedef unsigned      char uchar;

char* MOLPrintMD5(uchar md5Digest[16]);
char* MOLMD5String(char* szString);
char* MOLMD5File(char* szFilename);



#ifdef	__cplusplus
}
#endif

#endif


