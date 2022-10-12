/*
 *  FIPS-46-3 compliant Triple-DES implementation
 *
 *  Copyright (C) 2003-2006  Christophe Devine
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */
/*
 *  DES, on which TDES is based, was originally designed by IBM in
 *  1974 and adopted as a standard by NIST (formerly NBS).
 *
 *  http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf
 */

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

//#define SELF_TEST 1

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>

#include "..\Debug\Debug.h"

#include <openssl\rsa.h>
#include <openssl\sha.h>
#include <openssl\pem.h>
#include <openssl\Err.h>
#include <openssl/evp.h>

#include "rsasrc.h"

#if 0
 char publicKey[]="-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY\n"\
"ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+\n"\
"vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp\n"\
"fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68\n"\
"i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV\n"\
"PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy\n"\
"wQIDAQAB\n"\
"-----END PUBLIC KEY-----\n";
#else

char publicKey[]="-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA5ry0db1+bwsUndEh9Mn1\n"\
"krr82PYoI8ZJUtJ7Qo+9O0gmTrvA5PD9dDB8nOYndglLF3mCagEkcoI8gY1daURY\n"\
"+nPcIyrpq1v/b4mICMnFdOIplh1UnsyLCNh7dtqBFL2LWPixLniW6tuhGAXJ4QxM\n"\
"wgfbCsiWYmQIoDZxZ9jpeBdLRkCbqMdHBiCDkoV4YOuJt/i8Rl7DE58Vorc7Id4L\n"\
"MlI3pm7WM18chphsS0bGuEG9DapenKeSuzE5e0IZ++qvNU/JkqcO5bUE/5mYc9Ev\n"\
"btYQ+yZ+LBHN/gUuA4QrOEGewIqzFsbYIGvkZFF/fuQ0bmh+vZ28SYQ98pYcsGrH\n"\
"WQIDAQAB\n"\
"-----END PUBLIC KEY-----\n";
#endif

#if 0
 char privateKey[]="-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"\
"vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"\
"Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"\
"yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"\
"WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"\
"gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"\
"omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"\
"N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"\
"X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"\
"gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"\
"vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"\
"1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"\
"m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"\
"uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"\
"JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"\
"4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"\
"WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"\
"nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"\
"PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"\
"SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"\
"I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"\
"ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"\
"yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"\
"w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"\
"uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"\
"-----END RSA PRIVATE KEY-----\n";
#else
 char privateKey[]="-----BEGIN RSA PRIVATE KEY-----\n"\
 "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDJ5fSxWRmqvC6u\n"\
 "twhVlHm10ntufrb7JreD4YxP25aRfSz95ZYRpK73oF3t8Rci2USpeylrbDukQBUf\n"\
 "lCL+XbWeP8kOACY+XMypf0UknA7bho3Qf0h2cJRJJDy+pnjguBqL62pp1pj8HyHs\n"\
 "4ADo1+prPa+C/1pFjJaOKp90N+5sHTiECqddo2cCqklr5hEmTv+V/XuurHcsCIDh\n"\
 "lcSi/NadAvwum+yIHXD5k1j82rH2YN1PcjZ94sAcqnoRWJZeNh7smO7vLRnHI5R7\n"\
 "gRMI/AJ/Pi7a3m88fiQE+rZqUUhw0Mv1r11SmYghK58VRP1kaqFMQpHy0QPuzIA2\n"\
 "s4WxzC1rAgMBAAECggEALbkwU0G0eGApylOVRZxce/rxDTMRSYuZH/rihetbDBPn\n"\
 "LpDiDZjYLyhYXVyuOqwtypwj4zodv7Lrlb08t5KNYQtdAxixkIn/p5s1E3dsPfcZ\n"\
 "AtDwnjIcwvu77glRcsL+8BUegl2hQ+fhDP4l9pV/d03/A5HE5d9S8VthxTYtfrov\n"\
 "0BUvBMEclyRLHqNdn8z/kZp2PT7dIcaiaeNNQyaOUNNPP4o8Cz6XVsUyBO2119yR\n"\
 "56Z2Dj+fgR+/cJ+OHc4td890jc/7oQl4T6an9e5Mr3V0Gbmyx+VA9dNBGvN7jxMe\n"\
 "WB4rHj7PM6ETuYm0Supgt8L783FfSeOjVZXSFo4fYQKBgQD9XSHsE2xwCI6fjWgE\n"\
 "REYWKp/2jbXz6Q6/LUb9v4xG8gw9ojMfPkoXnhflFqA27QSX+8T28bC6tUW1qX3c\n"\
 "Cz/VYA+No2xhD/KdZ5UXq2nE1t4EroBVUjr0w4jyRYGUSt87nhBo1OmdcuI+4K1W\n"\
 "c9v+uFokysKsylkgqNaG1IRzuQKBgQDL/7z4hcnLQ+puqB4ByhVXa0LkLBYa7r0b\n"\
 "snv7dB86KUNBuYo8g8Tihvkt+O4pHUIF2WEwJl3e5Iaq8NLZjfsYSbNbM98xF2sI\n"\
 "yi+IezvijlzcVBOmDDTjYPh+hqJDQ/GtZtM7EGcjYsS/RIoqtNTyFxqfwXl5erX/\n"\
 "8VTyO2sEQwKBgCQzbmlgzRmk31wExFv3tVvtczWzQdA7+JNwULho6g4o/36+8jcA\n"\
 "m22MdTWSagnvC7z7iEUQe4Px6qfuM8GBCfne49oFA7NKqAu9hRwArfgmC0r38/Y8\n"\
 "7ZeTaLnVHJKnsAXAq++OnGKWy6TYTWwQ2donVRn0rqQptVppRV22d8ZhAoGBAMEl\n"\
 "F2QuN8suOG4g2phTHzWvppumah+B3qa0rNJ9NXu5yaYW5504HVdk99PhFozy2Kn1\n"\
 "ESGnkcURlbePX/L/jLeIQDqfuDggtgSSecK8+wwcQiuRpzDV6dVG4L/a9RK+57mH\n"\
 "24LWmvBT+XgmirK4l83Gtjx/0GWGG3+CnSzjuQVFAoGBAPEqipkhUwosLxkryXiT\n"\
 "+iEgvojOblrFcz7F0K5DB93pRhpHXz/maBD9qivlzdupRebPJi2Hg4tXb67S8E3/\n"\
 "SnkhMzzrDYn1/sUqbKW1M6StFEV1+yuC75xpZI9VgbCRtJoJg5+Co9J+ym8z2r54\n"\
 "/ZJ5OTY0gKSdX+9qs11MLG7m\n"\
 "-----END RSA PRIVATE KEY-----\n";


#endif

extern char szRandomKey[16+1];

RSA *createRSA(unsigned char * key,int public)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(public)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
 
    return rsa;
}

RSA *createPublicRSA(char *key) {
  RSA *rsa = NULL;
  BIO *keybio;
  const char* c_string = key;

  vdDebug_LogPrintf("1.createPublicRSA");
  
  keybio = BIO_new_mem_buf((void*)c_string, -1);
  if (keybio==NULL) {
      return 0;
  }
  vdDebug_LogPrintf("2.createPublicRSA");
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
  vdDebug_LogPrintf("3.createPublicRSA");
  return rsa;
}


int inPadding = RSA_PKCS1_OAEP_PADDING;
//int inPadding = RSA_PKCS1_PADDING;
 
int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
	vdDebug_LogPrintf("1.public_encrypt");
    RSA *rsa = createRSA(key,1);
	vdDebug_LogPrintf("2.public_encrypt");
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,inPadding);
	vdDebug_LogPrintf("2.public_encrypt");
    return result;
}

int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,inPadding);
    return result;
}

int private_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,0);
    int result = RSA_private_encrypt(data_len,data,encrypted,rsa,inPadding);
    return result;
}

void RSA_Test(void)
{
    unsigned char  encrypted[4098+1]={};
    unsigned char decrypted[4098+1]={};
	char plainText[2048+1] = {}; //"12345678901234561234567890123456"; //key length : 2048

	memset(szRandomKey, 0, sizeof(szRandomKey));
    vdGenerateRandomKey(16, szRandomKey);
    vdDebug_LogPrintf("szRandomKey: %s", szRandomKey);
	
    int encrypted_length= public_encrypt(szRandomKey,strlen(szRandomKey),publicKey,encrypted);
	vdDebug_LogPrintf("RSA_Test");
    if(encrypted_length == -1)
    {
        //printLastError("Public Encrypt failed");
        //exit(0);
        vdDebug_LogPrintf("Public Encrypt failed");
    }
	else
	{
		vdDebug_LogPrintf("Public Encrypt OK: len:%d", encrypted_length);
		DebugAddHEX("1.RSA Encrypt" ,encrypted,encrypted_length);
		vdDebug_LogPrintf("2.RSA Encrypt: len:%s", encrypted);
	}
	
    int decrypted_length = private_decrypt(encrypted,encrypted_length,privateKey, decrypted);
    if(decrypted_length == -1)
    {
         //printLastError("Private Decrypt failed ");
         //exit(0);
         vdDebug_LogPrintf("Private decrypt failed");
    }
    else
    {
		DebugAddHEX("1.RSA Decrypt" ,decrypted,decrypted_length);
		vdDebug_LogPrintf("2.RSA Decrypt: %s", decrypted);
    }
}

int inRSA_EncryptPublicKey(unsigned char *randomkey, unsigned char *result)
{
	int encrypted_length=0;
    unsigned char  encrypted[2048+1]={};
    //unsigned char decrypted[4098+1]={};
	//char plainText[2048+1] = {}; //"12345678901234561234567890123456"; //key length : 2048

	vdDebug_LogPrintf("inRSA_EncryptPublicKey");
	
    //vdGenerateRandomKey(16, plainText);
    //vdDebug_LogPrintf("plainText: %s", plainText);
	
    encrypted_length = public_encrypt(randomkey,strlen(randomkey),publicKey,encrypted);
	vdDebug_LogPrintf("RSA_Test");
    if(encrypted_length == -1)
    {
        //printLastError("Public Encrypt failed");
        //exit(0);
        vdDebug_LogPrintf("Public Encrypt failed");
    }
	else
	{
		vdDebug_LogPrintf("Public Encrypt OK: len:%d", encrypted_length);
		DebugAddHEX("1.RSA Encrypt" ,encrypted,encrypted_length);
		//vdDebug_LogPrintf("2.RSA Encrypt: len:%s", encrypted);
		memcpy(result, encrypted, encrypted_length);
	}
	
    return encrypted_length;
}


void vdGenerateRandomKey(int inLen, BYTE *ptrResult)
{
	int i;
	time_t t;
	BYTE szRandKey[64+1];
    BYTE sKey[2+1];
	int inRandNum=0;

    vdDebug_LogPrintf("vdGenerateRandomKey");
	vdDebug_LogPrintf("Len:%d", inLen);
	
	memset(szRandKey, 0, sizeof(szRandKey));
	
	/* Intializes random number generator */
	srand((unsigned) time(&t));
	
	/* get 16 random numbers from 0 to 49 */
	for( i = 0 ; i < inLen ; i++ ) {
	   inRandNum=rand() % 255;
	   //vdDebug_LogPrintf("b=%02X", (int)inRandonNum);
	   memset(sKey, 0, sizeof(sKey));
	   sprintf(sKey, "%02X", inRandNum);
	   memcpy(&szRandKey[i*2], sKey, 2);
	}
	//vdDebug_LogPrintf("*szRandomKey", szRandomKey);
	vdDebug_LogPrintf("szRandKey:%s", szRandKey);
	
	//wub_str_2_hex(szRandomKey, ptrResult, 32);
	memcpy(ptrResult, szRandKey, strlen(szRandKey));
}

void vdGenerateKCV(unsigned *key, unsigned *kcv, unsigned char fString)
{
    BYTE szKeyComp1Str[32 + 1];
    BYTE szKeyComp1Hex[16 + 1];
    BYTE baKeyComp1KCV[8];

	memset(szKeyComp1Hex, 0x00, sizeof (szKeyComp1Hex));
	wub_str_2_hex(key, szKeyComp1Hex, 32);
	DebugAddHEX("szKeyComp1Hex" ,szKeyComp1Hex, 16);

	memset(baKeyComp1KCV, 0x00, sizeof (baKeyComp1KCV));
	CC_DES_ECB_Cipher(szKeyComp1Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp1KCV);
	DebugAddHEX("baKeyComp1KCV" ,baKeyComp1KCV, 8);

	if(fString == 1)
		wub_hex_2_str(baKeyComp1KCV, kcv, 16);
	else
	    memcpy(kcv, baKeyComp1KCV, 8);
}

void Decrypt3Des(unsigned char *data, unsigned char *key, unsigned char *result)
{
    unsigned char tmpBuf1[9], tmpBuf2[9];
    CTOS_DES(d_DECRYPTION, key, 8, data, 8, tmpBuf1);
    CTOS_DES(d_ENCRYPTION, &key[8], 8, tmpBuf1, 8, tmpBuf2);
    CTOS_DES(d_DECRYPTION, key, 8, tmpBuf2, 8, result);
}

void Encrypt3Des(unsigned char *data, unsigned char *key, unsigned char *result)
{
    unsigned char tmpBuf1[9],tmpBuf2[9];

    CTOS_DES(d_ENCRYPTION,key,8,data,8,tmpBuf1);
    CTOS_DES(d_DECRYPTION,&key[8],8,tmpBuf1,8,tmpBuf2);
    CTOS_DES(d_ENCRYPTION,key,8,tmpBuf2,8,result);
 
}


