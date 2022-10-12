/**************************************************************************

 sample massage
 1. double length 3DES (Encryption Method field first byte = '2')
 2. Unique key per terminal (Encryption Method field second byte = '0')
 3. ANSI X9.9 MAC (Encryption Method field third byte = '0')
 4. DEK plain value: 8FD9130B0955F425BF5D6543BE55B54C
 5. MAK plain value: F143024742731D754F5067E919B6535F

 0200 request
 60 01 00 05 00 02 00 20 00 05 80 00 80 04 83 00 00 00 00 22 00 19 00 33
 30 30 30 30 30 34 32 00 12 30 30 30 30 30 30 30 30 30 30 30 31 00 92 48
 54 4C 45 30 31 30 30 31 38 30 30 30 30 30 30 31 32 30 30 30 30 32 30 30
 30 30 32 30 35 32 00 00 00 00 00 FC E7 A7 5F EE E4 0F AB E3 BB 65 C2 A7
 90 85 CD 5B EC 87 23 33 33 4C DE 1B EA ED B1 AC C4 16 54 45 83 E3 BC 3F
 AC 71 CB 34 4F D1 60 3F B1 18 E3 2A C5 46 29 D1 2F BD 5B 00 21 00 19 39
 39 31 32 33 34 35 36 37 30 30 30 30 30 30 30 30 30 31 91 D0 B1 F3 00 00
 00 00

 0210 response
 60 05 00 01 00 02 10 30 38 01 00 0A 80 00 81 00 00 00 00 00 00 00 96 20
 00 02 66 16 29 33 02 01 00 19 31 32 33 34 35 36 37 38 39 30 31 33 30 30
 33 30 30 30 30 30 34 32 00 44 48 54 4C 45 30 31 30 30 31 38 30 30 30 30
 30 30 31 32 30 30 30 30 32 30 30 30 30 32 30 30 38 00 00 00 00 00 33 7E
 99 C7 3E FD 17 A8 AD 76 04 A6 00 00 00 00



 --- encrypt data
 FCE7A75FEEE40FABE3BB65C2A79085CD5BEC872333334CDE1BEAEDB1ACC416544583E3BC3FAC71CB344FD1603FB118E32AC54629D12FBD5B
 --- decrypt data
 04060000000096201303000266
 4314
 37
 5951265724916967D03121011888812312345F520F31353830303030303030303030343200000000
 ---------------------
 04 = F.04
 06 = 6 byte
 000000009620
 13 = F.11
 03 = 3 byte
 000266
 43 = F.35
 14 = 20 byte
 375951265724916967D03121011888812312345F
 52 = F.42
 0F = 15 byte
 313538303030303030303030303432
 00000000 = pad


 00 44 48 54 4C 45 30 31
 30 30 31 31 31 30 30 30
 32 30 31 32 30 30 30 30
 35 32 30 30 30 34 30 30
 38 00 00 00 00 00 EF 7A
 4A 71 AD 8A 7F 84 79 EF
 27 84 00 00 00 00

 -------------------
 pure request

 6001000500
 0200
 3020058020C00403
 000000
 000000009620
 000266
 0022001900
 375951265724916967D03121011888812312345F
 3330303030303432
 313538303030303030303030303432
 0012303030303030303030303031
 0021001939393132333435363730303030303030303031
 91D0B1F300000000



 60 80 00 02 37 02 30 20
 38 01 00 06 80 00 10 80
 30 00 00 00 01 14 48 33
 02 20 02 37 33 33 32 35
 31 33 30 30 31 31 31 31
 31 31 31 31 00 16 41 50
 50 52 4F 56 45 20 20 20
 20 20 20 20 20 20
 ************************************************************************/
#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include "htlesrc.h"

#include "..\Includes\EFTSec.h"
#include "../Includes/V5IsoFunc.h"
#include "../Includes/MultiApLib.h"
#include "../Includes/POSTrans.h"
#include "../Includes/wub_lib.h"
#include "../Includes/Showbmp.h"
#include "../Includes/POSTypedef.h"
#include "../Includes/CTOSInput.h"
#include "../Comm/V5Comm.h"
#include "../Erm/PosErm.h"
#include "../UI/Display.h"

#include "../Aptrans/MultiAptrans.h"
#include "../FileModule/myFileFunc.h"
#include "../Des/dessrc.h"
#include "../print/Print.h"
#include "../Database/DatabaseFunc.h"
#include "..\Debug\Debug.h"
#include "..\Includes\CardUtil.h"
#include "..\Includes\POSSetting.h"


#include <openssl\rsa.h>
#include <openssl\sha.h>
#include <openssl\pem.h>
#include <openssl\Err.h>

// Hold Line for Load TWK after Settle
int fHoldLine = 0;
int fContinueLine = 0;

char szPublicKeyFileName[120]; /*[FILENAME_SIZE +1];*/
unsigned char uchSSKbuf[16]; /*Session Key*/
unsigned char szHTLEfield62[HTLE_FIELD62_SIZE + 1];
char szTEId[8 + 1] = "12002002";
char szTEPIN[8 + 1] = "12121212";

BYTE  IPEK[16] = {0x47,0x5B,0x2B,0xB4,0x5A,0xED,0x91,0xB0,0xA2,0xE3,0xBA,0xF1,0xAE,0x95,0xB4,0xDE};
BYTE  InitiallyLoadedKeySerialNumber[10] = {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
typedef struct AID_SUPPORTED {
#define		EMV_MAX_AID_SIZE			 		33      // ASCII rep. of 16 bytes binary

    char szAidSuported[EMV_MAX_AID_SIZE];
} AID_SUPPORTED;

TMK_RES_DATA TMKresponse;
TMK_RSA_RES_DATA TMKRSAresponse;

HTLE_CARD_DATA CardData;
TWK_RES_DATA TWKresponse;
TWK_RSA_RES_DATA TWKRSAresponse;

unsigned char TMFK_keys[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
};

char *szGetTE_ID(void) {
	return (char*)szTEId;
}

char *szGetTE_PIN(void) {
	return (char*)szTEPIN;
}
		
char *szGetAqcID(void) {
    return (char*)strEFT[0].szAcquirerID;
}

char *szGetPOSVendorID(void) {
    return (char*)strEFT[0].szVendorID;
}

void vdSetPublicKeyFileName(void) {
    char szAqcID[3 + 1];
    char szVendorId[8 + 1];
    int inHostNum;

    memset(szAqcID, 0, sizeof (szAqcID));
    memset(szVendorId, 0, sizeof (szVendorId));
    memset(szPublicKeyFileName, 0x00, sizeof(szPublicKeyFileName));
    
    strcpy(szAqcID, szGetAqcID());
    strcpy(szVendorId, szGetPOSVendorID());
    sprintf(szPublicKeyFileName, "%s%s%s%s", PUBLIC_PATH, PUBLICKEY_FILENAME, szAqcID, szVendorId);
    strcat(szPublicKeyFileName, PUBLICKEY_FILE_EXTN);

	return;
}

int CTOSS_inSMPKCS1put(unsigned char* output, int len_out, unsigned char* input, int len_in)
{
    if(output == NULL || input == NULL)
        return(SM_RET_PARAM);
    
    memset(output, 0xFF, len_out);
    output[0]=0x00;
    output[1]=0x02;
    output[len_out-len_in-1]=0x00;
    memcpy(&output[len_out-len_in], input, len_in);

    return(SM_RET_OK);

}

BYTE ith(BYTE c)
{
	if(c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if(c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}
	else if(c >= 'a' && c <= 'f')
	{
		return c - 'a' + 10;
	}
	
	return 0;
}

void Pack(BYTE* pData, USHORT usLen, BYTE* pResult)
{
	USHORT i;
	BYTE v;
	
	for(i = 0; i < usLen; i += 2)
	{
		v = ith(pData[i]);
		v <<= 4;
		v |= ith(pData[i+1]);
		*pResult++ = v;
	}
}

int inGetPublickey(void) {
    char strZssk[128 + 1];
    char szHTLEmod[256 + 1];
    char szHTLEexp[32 + 1];
    unsigned char szZssk[128 + 1];
    unsigned char b8zero[8 + 1];
    unsigned char sz3DESe[256 + 1];
    unsigned char bKCV[8 + 1];
    unsigned char szSendBuff[256 + 1], szReceiveBuff[256 + 1];
    unsigned int inReturn;
    uchar ciphertext[128 + 1];
    unsigned char eCardSerial[8 + 1];
	BYTE Modulus[256] = {'\0'};
	BYTE Exponent[256] = {'\0'};
	int i,inNumOfHDTRecord;

    memset(uchSSKbuf, 0x00, sizeof (uchSSKbuf));
    memset(szHTLEmod, 0x00, sizeof (szHTLEmod));
    memset(szHTLEexp, 0x00, sizeof (szHTLEexp));
    memset(szZssk, 0x00, sizeof (szZssk));
    memset(strZssk, 0x00, sizeof (strZssk));
    memset(b8zero, 0x00, sizeof (b8zero));
    memset(sz3DESe, 0x00, sizeof (sz3DESe));
    memset(bKCV, 0x00, sizeof (bKCV));
    memset(szSendBuff, 0x00, sizeof (szSendBuff));
    memset(szReceiveBuff, 0x00, sizeof (szReceiveBuff));
    memset(eCardSerial, 0x00, sizeof (eCardSerial));

	inHDTRead(strEFT[0].inHDTid);

	if ((strcmp(strEFT[0].szEFTVersion, "01")!=0)||(strcmp(strEFT[0].szEFTVersion, "04")!=0))
		return VS_ERR;
	
    vdSetPublicKeyFileName();
    if (Read_FilePublicKey(szPublicKeyFileName, (unsigned char *) szHTLEmod, (unsigned char *) szHTLEexp) != VS_SUCCESS) {
        vdHTLEDisplayMSG(0x6A80);
        return VS_ERR;
    }

//    inGenSSK(uchSSKbuf);
	CTOS_RNG(uchSSKbuf);																						
	CTOS_RNG(&uchSSKbuf[8]);																						

	vdPCIDebug_HexPrintf("inGenSSK", uchSSKbuf , 16);

	CTOS_KMS2Init();
//	CTOSS_inSMPKCS1put(szZssk, 128, uchSSKbuf, 16);	
	memset(szZssk, 0x00, sizeof(szZssk));
	memcpy(&szZssk[112],uchSSKbuf,sizeof(uchSSKbuf)); //szZssk is 112 zero(0x00) plus 16 bytes session key ::128 byte

	Pack((BYTE*)szHTLEmod, 256, Modulus);
	Pack((BYTE*)szHTLEexp, 6, Exponent);
	CTOS_RSA(Modulus, 128, Exponent, 3, szZssk, ciphertext);	

	vdPCIDebug_HexPrintf("Modulus", Modulus , 128);
	vdPCIDebug_HexPrintf("Exponent", Exponent , 3);
	vdPCIDebug_HexPrintf("szZssk", szZssk , 128);
	vdPCIDebug_HexPrintf("ciphertext", ciphertext , 128);
	
    memcpy(szSendBuff, ciphertext, 128);
    in3DES_Encrypt(b8zero, uchSSKbuf, sz3DESe);
	vdPCIDebug_HexPrintf("sz3DESe", sz3DESe , 4);
    szSendBuff[128] = sz3DESe[0];
    szSendBuff[129] = sz3DESe[1];
    szSendBuff[130] = sz3DESe[2];
    szSendBuff[131] = sz3DESe[3];
    szSendBuff[132] = 0x00;
    szSendBuff[133] = 0x00;
    szSendBuff[134] = 0x00;
    szSendBuff[135] = 0x00;

    inReturn = inReadRecordJavaCard(CLA_NORM, INS_STR, P1_SESS, 0x00, 136/*is 136 Byte*/, szSendBuff, 0x08, szReceiveBuff);
    if (inReturn == 0x9000) {
        // use SSK to encrypt(3DES-CBC) card serail no
        memset(CardData.szCardSerial, 0x00, 8);
        memcpy(eCardSerial, szReceiveBuff, 8);
        memcpy(CardData.szCardSerial, szReceiveBuff, 8);
    } else {
        vdHTLEDisplayMSG(inReturn);
    }

    return (VS_SUCCESS);
}

int inGenSSK(unsigned char * uchSSK) {
	int i;
	unsigned char temp[16+1];
	unsigned int len;
	int inLen = 0;
	
	i=0;
	do
	{
	    memset(temp,0,sizeof(temp));
		CTOS_RNG(temp);
        
		if(len-i >= 8)
			memcpy(&uchSSK[i], temp, 8);
		else
			memcpy(&uchSSK[i], temp, len-i);

		i+=8;
	} while(len >  i);

    inLen = strlen((char*) uchSSK);
    return (inLen);
}

void read_clock(char *szDateTime) {
    CTOS_RTC SetRTC;
    char szDate[4 + 1];
    char szTime[6 + 1];
    char szBuf[2 + 1];

    CTOS_RTCGet(&SetRTC);

    memset(szDate, 0, sizeof (szDate));
    memset(szTime, 0, sizeof (szTime));

    wub_hex_2_str(srTransRec.szDate, szDate, 2);
    wub_hex_2_str(srTransRec.szTime, szTime, 3);
    sprintf(szDateTime, "%s%s%s11", "2014", srTransRec.szDate, srTransRec.szTime);
}

int inVXEMVAPCardPresent(void) {
    BYTE bStatus  = 0;
	int inResult = 0;

    CTOS_SCStatus(d_SC_USER, &bStatus);
    if (bStatus & d_MK_SC_PRESENT)
		inResult = 1;

	return inResult;
}

static short openUsercardSlot(void) {
    BYTE baATR[d_BUFF_SIZE], bATRLen, CardType;
    BYTE key;
    BYTE bStatus;

    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 2, "Plz Ins Card");

    do {
        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL) return;
        //Check the ICC status //
        CTOS_SCStatus(d_SC_USER, &bStatus);
    } while (!(bStatus & d_MK_SC_PRESENT)); //Break until the ICC Card is inserted //

    bATRLen = sizeof (baATR);
    //Power on the ICC and retrun the ATR contents metting the EMV2000 specification //
    if (CTOS_SCResetEMV(d_SC_USER, d_SC_5V, baATR, &bATRLen, &CardType) == d_OK) CTOS_LCDTPrintXY(13, 3, "OK");
    else CTOS_LCDTPrintXY(13, 3, "Fail");

    CTOS_Delay(1000);

    bATRLen = sizeof (baATR);
    //Power on the ICC and retrun the ATR content metting the ISO-7816 specification //
    if (CTOS_SCResetISO(d_SC_USER, d_SC_5V, baATR, &bATRLen, &CardType) == d_OK) {
        CTOS_LCDTPrintXY(13, 4, "OK");
        return 1;
    } else {
        CTOS_LCDTPrintXY(13, 4, "Fail");
        return 0;
    }
}

int inHTLEInitCardData(void) {
    char szMessage[29 + 1];
    //unsigned long ulTimerCount;
    int inResult = VS_ERR;
    BYTE bKey = 0x00;

    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 4, "INSERT CARD");

    do {
		CTOS_KBDHit(&bKey);
		
        if (inVXEMVAPCardPresent() == 1) {
            CTOS_LCDTPrintXY(1, 4, "WAITING..   ");
            inResult = SendCardCommandAID();
        } else if (bKey == d_KBD_CANCEL) {
            inResult = VS_ESCAPE;
            return VS_ESCAPE;
        }
    } while (inResult == VS_ERR);

    if (inResult == ERR_SELECT_AID) {
        return (VS_ERR);
    }

    return (VS_SUCCESS);
}

int inVXEMVAPCardInit(void) {
    char data_buf[100];
    BYTE bKey = 0x00;
    BYTE baATR[d_BUFF_SIZE], bATRLen, CardType;

    bATRLen = sizeof (baATR);
    //Power on the ICC and retrun the ATR contents metting the EMV2000 specification //
    if (CTOS_SCResetEMV(d_SC_USER, d_SC_5V, baATR, &bATRLen, &CardType) == d_OK){
		return 1;
    } else {
//        return 0;
    }

//    bATRLen = sizeof (baATR);
    //Power on the ICC and retrun the ATR content metting the ISO-7816 specification //
//    if (CTOS_SCCommonReset(d_SC_USER, 0x11, TRUE, TRUE, TRUE, TRUE, d_SC_3V, baATR, &bATRLen, &CardType) == d_OK) {
//		CTOS_LCDTPrintXY(1, 4, "RESET OK");
//		return 1;
//    } else {
//     	CTOS_LCDTPrintXY(1, 4, "RESET Fail");
//        return 0;
//    }

    CTOS_Delay(1000);

    bATRLen = sizeof (baATR);
    //Power on the ICC and retrun the ATR content metting the ISO-7816 specification //
    if (CTOS_SCResetISO(d_SC_USER, d_SC_5V, baATR, &bATRLen, &CardType) == d_OK) {
		CTOS_LCDTPrintXY(1, 4, "ISO7816 OK");
		return 1;
    } else {
     	CTOS_LCDTPrintXY(1, 4, "ISO7816 Fail");
        return 0;
    }
}

int SendCardCommandAID(void) {
    unsigned char lc = 0;
    int loop = 0;
    int inResult = 0;
    unsigned short lReturn = 0;
    unsigned char szSendBuff[16 + 1], szReceiveBuff[256 + 1];
    char szTempBuff[16 + 1];
    int inHostNum;

    AID_SUPPORTED stAid[11];

    lc = 0;
    memset(szSendBuff, 0x00, 16);
    memset(szTempBuff, 0x00, 16);

    for (loop = 0; loop <= 10; loop++) {
        memset(stAid[loop].szAidSuported, 0x00, sizeof (stAid[loop].szAidSuported));
    }
    strcpy(stAid[1].szAidSuported, "A000000018FF00000000000000000002");
    strcpy(stAid[2].szAidSuported, "A0000000031020");
    strcpy(stAid[3].szAidSuported, "A0000000999090");
    strcpy(stAid[4].szAidSuported, "A0000000032010");
    strcpy(stAid[5].szAidSuported, "");
    strcpy(stAid[6].szAidSuported, "");
    strcpy(stAid[7].szAidSuported, "");
    strcpy(stAid[8].szAidSuported, "");
    strcpy(stAid[9].szAidSuported, "");
    strcpy(stAid[10].szAidSuported, "");

    inResult = inVXEMVAPCardInit();

	if (inResult!=1)
        return ERR_SELECT_AID;
		
    for (loop = 1; loop <= 10; loop++) {
        if (strcmp(stAid[loop].szAidSuported, "") != 0) {
            lc = strlen(stAid[loop].szAidSuported) / 2;
            wub_str_2_hex((char *) stAid[loop].szAidSuported, (char*) szSendBuff, strlen(stAid[loop].szAidSuported));
            lReturn = inReadRecordJavaCard(CLA_SELECT, INS_SELECT, P1_SELECT, 0x00, lc, szSendBuff, 0x00, szReceiveBuff);
            if (lReturn == 0x9000) {
                break;
            }
        }
    }

    if (lReturn != 0x9000) {
        vdHTLEDisplayMSG(lReturn);
        return ERR_SELECT_AID;
    }

    return VS_SUCCESS;
}

unsigned int inReadRecordJavaCard(unsigned char bCLA, unsigned char bINS, unsigned char bP1, unsigned char bP2, int inSize, unsigned char *cSendBuffer, unsigned char bLOUT, unsigned char *cRecvBuffer) {
    BYTE usRetVal;
    unsigned long ulSlotNo = d_SC_USER;
    unsigned short pusRespLen = 0;
    char strRx[512+1];
    unsigned char bCBuffer[256 + 1], pucRespMsg[512 + 1]; // , bData[256];
    unsigned char chRet[2];
    unsigned int inRetval;
    char szTemp[256];

    memset(strRx, 0x00, sizeof (strRx));
    memset(bCBuffer, 0x00, sizeof (bCBuffer));

    bCBuffer[0] = bCLA; // not secure
    bCBuffer[1] = bINS; // ins
    bCBuffer[2] = bP1; // P1	variable
    bCBuffer[3] = bP2; // P2
    bCBuffer[4] = inSize;

    if (inSize > 0) {
        memcpy(&bCBuffer[5], cSendBuffer, inSize);
		bCBuffer[5 + inSize] = bLOUT;
    }
	else
		bCBuffer[4] = bLOUT;

    memset(szTemp, 0, sizeof (szTemp));

    if (inSize > 0) 
    	wub_hex_2_str((char*) bCBuffer, szTemp, inSize + 5);

    memset(pucRespMsg, 0, sizeof (pucRespMsg));
    pusRespLen = 256;
	
    if (inSize == 0)
        usRetVal = CTOS_SCSendAPDU(d_SC_USER, bCBuffer, (5 + inSize), pucRespMsg, &pusRespLen);
    else
        usRetVal = CTOS_SCSendAPDU(d_SC_USER, bCBuffer, (5 + inSize + 1), pucRespMsg, &pusRespLen);

	vdDebug_LogPrintf("CTOS_SCSendAPDU usRetVal[%d] pusRespLen[%d]", usRetVal, pusRespLen);
    if (inSize == 0)
		vdPCIDebug_HexPrintf("CTOS_SCSendAPDU", bCBuffer , (5 + inSize));
	else
		vdPCIDebug_HexPrintf("CTOS_SCSendAPDU", bCBuffer , (5 + inSize + 1));

	vdPCIDebug_HexPrintf("CTOS_SCSendAPDUReceive", pucRespMsg , pusRespLen);

    if (usRetVal != d_OK)
        return (unsigned int) VS_ERR;

    memcpy(chRet, &pucRespMsg[pusRespLen - 2], 2);

    if (pusRespLen > 2) {
        memcpy(cRecvBuffer, pucRespMsg, pusRespLen - 2);
        wub_hex_2_str((char*) cRecvBuffer, strRx, pusRespLen - 2);
    }

    inRetval = (unsigned int) chRet[0] << 8;
    inRetval |= (unsigned int) chRet[1] & 0xFF;

    return (inRetval);
}

int Read_PublicKeyFile(char *inputfile, unsigned char *HTLE_MOD, unsigned char *HTLE_EXP) {
    int filesize;
    int inResult;
    FILE *hHandle;
    char szTempBuff[640];
    char szKeyTemp1Buff[300];
    char szKeyTemp2Buff[300];
    char szKeyTemp3Buff[300];
    int ibuff = 0;
    int linenum1 = 0, linenum2 = 0, linenum3 = 0;
    char szEXP[6], szMOD[256];
    int i;

    filesize = (int) lnGetFileSize((char*) inputfile);
    hHandle = fopen((char*) inputfile, "r+");
    if (hHandle < 0) {
        fclose(hHandle);
        return (VS_ERR);
    }
    memset(szTempBuff, 0x00, sizeof (szTempBuff));
    memset(szKeyTemp1Buff, 0x00, sizeof (szKeyTemp1Buff));
    memset(szKeyTemp2Buff, 0x00, sizeof (szKeyTemp2Buff));
    memset(szKeyTemp3Buff, 0x00, sizeof (szKeyTemp3Buff));
    memset(szEXP, 0x00, sizeof (szEXP));
    memset(szMOD, 0x00, sizeof (szMOD));

    inResult = fread(szTempBuff, filesize, 1, hHandle);
    while (szTempBuff[ibuff] != 0x00) {
        if (szTempBuff[ibuff] == '\n' && linenum2 != 0 && linenum3 == 0)
            linenum3 = ibuff + 1;
        if (szTempBuff[ibuff] == '\n' && linenum3 == 0)
            linenum2 = ibuff + 1;
        ibuff++;
    }
    ibuff = 0;
    while (szTempBuff[linenum1] != '\n') {
        szKeyTemp1Buff[ibuff] = szTempBuff[linenum1];
        linenum1++;
        ibuff++;
    }

    if (linenum2 != 0 && linenum3 != 0) {
        ibuff = 0;
        while (szTempBuff[linenum2] != '\n') {
            szKeyTemp2Buff[ibuff] = szTempBuff[linenum2];
            linenum2++;
            ibuff++;
        }
        ibuff = 0;
        while (szTempBuff[linenum3] != '\n') {
            szKeyTemp3Buff[ibuff] = szTempBuff[linenum3];
            ibuff++;
            linenum3++;
        }
    }

    ibuff = 0;
    for (i = 43; i < 49; i++) {
        szEXP[ibuff] = szTempBuff[i];
        ibuff++;
    }
    memcpy(HTLE_EXP, szKeyTemp2Buff, 32);
    memcpy(HTLE_MOD, szKeyTemp3Buff, 256);

    if (inResult == 0L) {
        fclose(hHandle);
        return (VS_ERR);
    }
    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }
    return (VS_SUCCESS);
}

int in3DES_Encrypt(unsigned char *cInputBuffer, unsigned char *sessionKEY, unsigned char * uchOut3DESe) {
    /* uchar DES3_init[8] =
     {
     0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74
     };

     uchar DES3_keys[16] =
     {
     0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
     0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
     };*/
    des3_context ctx3;

    des3_set_2keys(&ctx3, sessionKEY);
    des3_encrypt(&ctx3, cInputBuffer, uchOut3DESe);
    return (VS_SUCCESS);
}

int in3DES_CBC_Encrypt(unsigned char *cInputBuffer, int inSize, unsigned char *sessionKEY, unsigned char * uchOut3DESe) {
    des3_context ctx3;
    uchar iv[8];

    memset(iv, 0, 8);
    des3_set_2keys(&ctx3, sessionKEY);
    des3_cbc_encrypt(&ctx3, iv, cInputBuffer, uchOut3DESe, inSize);

    return (VS_SUCCESS);
}

int in3DES_CBC_Decrypt(unsigned char *cInputBuffer, int inSize, unsigned char *sessionKEY, unsigned char * uchOut3DESd) {
    des3_context ctx3;
    uchar iv[8];

    memset(iv, 0x00, 8);
    des3_set_2keys(&ctx3, sessionKEY);
    des3_cbc_decrypt(&ctx3, iv, cInputBuffer, uchOut3DESd, inSize);

    return (VS_SUCCESS);
}

int inEnterTEpin(void) {
    int inResult;
    char szInputStr[8];
    int inTryPIN = 3;
    unsigned int inReturn;
    unsigned char szSendBuff[8], szReceiveBuff[256];
    int i;
    BYTE bKey = 0x00;
    USHORT shMinLen = 1;
    USHORT shMaxLen = 8;
    BYTE Bret;

    memset(szInputStr, 0x00, sizeof (szInputStr));

    CTOS_LCDTClearDisplay();
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Input PIN");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, " 	 ");

    do {
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        CTOS_KBDHit(&bKey);
        memset(szInputStr, 0x00, sizeof (szInputStr));
        shMinLen = 8;
        shMaxLen = 8;
        Bret = InputString(1, 8, 0x01, 0x02, szInputStr, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

        if (inResult < VS_SUCCESS) {
            return (VS_ESCAPE);
        }
        if (strcmp(szInputStr, "") == 0)
            continue;

        memset(szSendBuff, 0x00, sizeof (szSendBuff));
        memset(szReceiveBuff, 0x00, sizeof (szReceiveBuff));

        for (i = 0; i < 8; i++) {
            if ((szInputStr[i] == 0x00) || (szInputStr[i] == 0x08))
                szInputStr[i] = 0xFF;
        }

        for (i = 0; i < sizeof (szInputStr); i++) {
        }
        for (i = 0; i < sizeof (uchSSKbuf); i++) {
        }

        in3DES_Encrypt((unsigned char*) szInputStr, uchSSKbuf, szSendBuff);
        inReturn = inReadRecordJavaCard(CLA_SECURE, INS_PIN, P1_TE_PIN, 0x00, 8/*is 8 Byte*/, szSendBuff, 0x00, szReceiveBuff);

        if (inReturn == 0x9000) {
            return VS_SUCCESS;
        } else if (inReturn == 0x63C1) {
            inTryPIN--;
        } else if (inReturn == 0x63C2) {
            inTryPIN--;
        } else if (inReturn == 0x63C3) {
            inTryPIN = 0;
        } else {
            vdHTLEDisplayMSG(inReturn);
            break;
        }

    } while (inTryPIN != 0);

    if (inTryPIN == 0) {
        return VS_ERR;
    }
    return VS_ERR;
}

int inTMKSessionKeyGen(void) {
    unsigned int inReturn;
    char szAqcID[3 + 1];
    char szVendorId[8 + 1];
    unsigned char uchDATA[16];
    unsigned char szSendBuff[16], szReceiveBuff[256];
    int i;
    int inHostNum;
    unsigned char eCardResponse[24];
    unsigned char dCardResponse[24];

    memset(szAqcID, 0, sizeof (szAqcID));
    memset(szVendorId, 0, sizeof (szVendorId));
    memset(uchDATA, 0x00, sizeof (uchDATA));
    memset(szSendBuff, 0x00, sizeof (szSendBuff));
    memset(szReceiveBuff, 0x00, sizeof (szReceiveBuff));
    memset(eCardResponse, 0x00, sizeof (eCardResponse));
    memset(dCardResponse, 0x00, sizeof (dCardResponse));
    inHostNum = srTransRec.HDTid;	
    memcpy(szVendorId, szGetPOSVendorID(), 8);
    memcpy(szAqcID, szGetAqcID(), 3);
    sprintf((char*) uchDATA, "%s%s", szAqcID, szVendorId);

    in3DES_CBC_Encrypt(uchDATA, 16/*sizeof(uchDATA)*/, uchSSKbuf, szSendBuff);
    inReturn = inReadRecordJavaCard(CLA_SECURE, INS_TMK, 0x00, 0x00, 16/*is 16 Byte*/, szSendBuff, 0x18, szReceiveBuff);
    if (inReturn == 0x9000) {
        memset(CardData.szKEKid, 0x00, 4);
        memset(CardData.eRANsim, 0x00, 16);
        memset(CardData.szKCVsim, 0x00, 4);
        memcpy(eCardResponse, szReceiveBuff, 24);
        in3DES_CBC_Decrypt(eCardResponse, 24/*sizeof(eCardResponse)*/, uchSSKbuf, dCardResponse);
        memcpy(CardData.szKEKid, dCardResponse, 4);
        memcpy(CardData.eRANsim, dCardResponse + 4, 16);
        memcpy(CardData.szKCVsim, dCardResponse + 20, 4);
    } else {
        vdHTLEDisplayMSG(inReturn);
		return (VS_ERR);
    }

    return (VS_SUCCESS);
}

int Read_FilePublicKey(char * inputfile, unsigned char * HTLE_MOD, unsigned char * HTLE_EXP) {
    int filesize;
    int inResult;
    FILE *hHandle;
    int ibuff = 0;
    char szTempBuff[320];
    char szFig[40], szEXP[6], szMOD[256];
    int i = 0;
    int j = 0;
    int inSizefile;

    filesize = (int) lnGetFileSize(inputfile);
    hHandle = fopen(inputfile, "r+");
    if (hHandle < 0) {
        fclose(hHandle);
        return (VS_ERR);
    }
    memset(szTempBuff, 0x00, sizeof (szTempBuff));
    memset(szFig, 0x00, sizeof (szFig));
    memset(szEXP, 0x00, sizeof (szEXP));
    memset(szMOD, 0x00, sizeof (szMOD));
    inResult = fread(szTempBuff, filesize, 1, hHandle);
    inSizefile = sizeof (szTempBuff);

    ibuff = 0;
    for (i = 0; i < inSizefile; i++) {
        if (szTempBuff[i] != 0x0D) {
            szFig[ibuff] = szTempBuff[i];
            ibuff++;
        } else {
            j = i + 3;
            break;
        }
    }

    ibuff = 0;
    for (i = j; i < inSizefile; i++) {
        if (szTempBuff[i] != 0x0D) {
            szEXP[ibuff] = szTempBuff[i];
            ibuff++;
        } else {
            j = i + 3;
            break;
        }
    }

    ibuff = 0;
    for (i = j; i < inSizefile; i++) {
        if (szTempBuff[i] != 0x0D) {
            szMOD[ibuff] = szTempBuff[i];
            ibuff++;
        } else
            break;
    }

    memcpy(HTLE_EXP, szEXP, 6);
    memcpy(HTLE_MOD, szMOD, 256);

    if (inResult == 0L) {
        fclose(hHandle);
        return (VS_ERR);
    }
    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }
    return (VS_SUCCESS);
}

int inHTLEField62Data(unsigned char *pbtBuff, unsigned short *pusLen) {
    char szBuf[20];
    char btTmp[80];
    int inSize = 0;
    char szTLEindc[4 + 1];
    char szVersion[2 + 1];
    char szAqcID[3 + 1];
    char szTID[8 + 1];
    char szVendorId[8 + 1];
    char szKEKid[4 + 1];
    char szDownType[100 + 1];
    char eRANSIM[16 + 1];
    char szKCVSIM[8 + 1];
    char szCardNo[8 + 1];

    memset(btTmp, 0, sizeof (btTmp));
    memset(szTLEindc, 0, sizeof (szTLEindc));
    memset(szVersion, 0, sizeof (szVersion));
    memset(szAqcID, 0, sizeof (szAqcID));
    memset(szTID, 0, sizeof (szTID));
    memset(szVendorId, 0, sizeof (szVendorId));
    memset(szKEKid, 0, sizeof (szKEKid));
    memset(szDownType, 0, sizeof (szDownType));
    memset(eRANSIM, 0, sizeof (eRANSIM));
    memset(szKCVSIM, 0, sizeof (szKCVSIM));
    memset(szCardNo, 0, sizeof (szCardNo));

    memcpy(szTLEindc, "HTLE", 4);
    memcpy(szVersion, "01", 2);
    memcpy(szAqcID, szGetAqcID(), 3);
    memcpy(szTID, srTransRec.szTID, 8);
    memcpy(szVendorId, szGetPOSVendorID(), 8);
    memcpy(szKEKid, CardData.szKEKid, 4);
    sprintf(szBuf, "%d", 0);
    memcpy(szDownType, szBuf, 1); // use KEK-SAM
    memcpy(eRANSIM, CardData.eRANsim, 16);
    wub_hex_2_str((char*) CardData.szKCVsim, szKCVSIM, 4);
    memcpy(szCardNo, CardData.szCardSerial, 8);
    memcpy(&btTmp[0], szTLEindc, 4);
    memcpy(&btTmp[0 + 4], szVersion, 2);
    memcpy(&btTmp[0 + 4 + 2], szAqcID, 3);
    memcpy(&btTmp[0 + 4 + 2 + 3], szTID, 8);
    memcpy(&btTmp[0 + 4 + 2 + 3 + 8], szVendorId, 8);
    memcpy(&btTmp[0 + 4 + 2 + 3 + 8 + 8], szKEKid, 4);
    memcpy(&btTmp[0 + 4 + 2 + 3 + 8 + 8 + 4], szDownType, 1);
    memcpy(&btTmp[0 + 4 + 2 + 3 + 8 + 8 + 4 + 1], eRANSIM, 16);
    memcpy(&btTmp[0 + 4 + 2 + 3 + 8 + 8 + 4 + 1 + 16], szKCVSIM, 8);
    memcpy(&btTmp[0 + 4 + 2 + 3 + 8 + 8 + 4 + 1 + 16 + 8], szCardNo, 8);
    memcpy(&pbtBuff[0], btTmp, 4 + 2 + 3 + 8 + 8 + 4 + 1 + 16 + 8 + 8);
    inSize = 4 + 2 + 3 + 8 + 8 + 4 + 1 + 16 + 8 + 8;
    *pusLen = (unsigned short) inSize;

    return VS_SUCCESS;
}

int inTMKDecryptReq(void) 
{
    unsigned int inReturn;
    unsigned char szSendBuff[24], szReceiveBuff[256];
    unsigned char uchDATA[24];
    unsigned char eCardResponse[16];
    unsigned char szTemp[20];

    memset(uchDATA, 0x00, sizeof (uchDATA));
    memset(szSendBuff, 0x00, sizeof (szSendBuff));
    memset(szReceiveBuff, 0x00, sizeof (szReceiveBuff));
    memset(eCardResponse, 0x00, sizeof (eCardResponse));
    memcpy(uchDATA, TMKresponse.resTMKe, 16);
    wub_str_2_hex((char*) TMKresponse.resKCVTMK, (char*) szTemp, 8);
    memcpy(uchDATA + 16, szTemp, 4);
    memset(uchDATA + 20, 0, 4);
    in3DES_CBC_Encrypt(uchDATA, 24/*sizeof(uchDATA)*/, uchSSKbuf, szSendBuff);
    inReturn = inReadRecordJavaCard(CLA_SECURE, INS_TMKD, 0x00, 0x00, 24/*is 24 Byte*/, szSendBuff, 16/*Len out is 16*/, szReceiveBuff);
    if (inReturn == 0x9000) {
        memset(CardData.szTMK, 0x00, 16);
        memcpy(eCardResponse, szReceiveBuff, 16);
        in3DES_CBC_Decrypt(eCardResponse, 16/*sizeof(eCardResponse)*/, uchSSKbuf, CardData.szTMK);
    } else {
        memcpy(CardData.szTMK, uchSSKbuf, 16);
        vdHTLEDisplayMSG(inReturn);
        return VS_ERR;
    }

    /***END Session***/
    memset(szSendBuff, 0x00, sizeof (szSendBuff));
    memset(szReceiveBuff, 0x00, sizeof (szReceiveBuff));
    inReturn = inReadRecordJavaCard(CLA_NORM, INS_END, 0x00, 0x00, 0/*is 24 Byte*/, szSendBuff, 0x00, szReceiveBuff);
    if (inReturn == 0x9000) {
    } else {
        vdHTLEDisplayMSG(inReturn);
    }
    return VS_SUCCESS;
}

void vdHTLEDisplayMSG(unsigned int usCondition) {
    unsigned int usMsgGroup;
    unsigned int usMsgId;
    char msg[50];
    char *szErrMsg_A[] = {
        "KEK not found",
        "KEK is not active",
        "KEK-SAM no mismatch",
        "KEK-SAM is blocked",
        "Terminal record not active",
        "KCV does not match",
        "MAC not match",
        "Unexpected Unencrypted",
        "Unexpected KMS txn",
        "Unexpected EFTsec txn"
    };
    char *szErrMsg_B[] = {
        "Key F.62 invalid",
        "invalid TLE F.57",
        "invalid Versoin F.57",
        "Acquirer BDK not found",
        "Database error",
        "Decrypt failed",
        "Host switching error",
        "F.57 format error",
        "",
        "Invalid encryption"
    };
    char *szErrMsg_C[] = {
        "Invalid dynamic key",
        "Invalid MAC ",
        "Decode F.57 length",
        "Decode F.57 content",
        "",
        "Host Invalid resp"
    };
    char *szErrMsg_K[] = {
        "Unexpected error",
        "Force TMK download"
    };
    //		char *szErrMsg_Z[] = {
    //					"Public Key File err"
    //			};

    memset(msg, 0, sizeof (msg));
    switch (usCondition) {
        case 0x63C0:
            strcpy(msg, "FAIL,NO TRY LEFT");
            break;
        case 0x6283:
            strcpy(msg, "APPL BLOCKED");
            break;
        case 0x6700:
            strcpy(msg, "INCORRECT LEN");
            break;
        case 0x6999:
            strcpy(msg, "SELECT APPL FAIL");
            break;
        case 0x6A81:
            strcpy(msg, "FUNC NOT SUPPORT");
            break;
//        case 0x6A82:
//            strcpy(msg, "FILE NOT FOUND");
//            break;
        case 0x6A86:
            strcpy(msg, "ERROR P1 P2");
            break;
        case 0x6300:
            strcpy(msg, "WAITING..");
            break;
        case 0x6581:
            strcpy(msg, "AUTH FAIL");
            break;
        case 0x6985:
            strcpy(msg, "MEMORY FAIL");
            break;
        case 0x6B00:
            strcpy(msg, "CON NOT SATIS");
            break;
        case 0x6E00:
            strcpy(msg, "CLA ERROR");
            break;
        case 0x9013:
            strcpy(msg, "Decryp/Encryp Err");
            break;
        case 0x9014:
            strcpy(msg, "KCV ERROR");
            break;
        case 0x9016:
            strcpy(msg, "KEY GEN ERR");
            break;
        case 0x6982:
            strcpy(msg, "SEC NOT SATIS");
            break;
        case 0x6A80:
            strcpy(msg, "INVALID DATA");
            break;
        case 0x9011:
            strcpy(msg, "CARD NOT INIT");
            break;
        case 0x9012:
            strcpy(msg, "KEK NOT INIT");
            break;
        case 0x9015:
            strcpy(msg, "NO SESS ESTAB");
            break;
        case 0x9017:
            strcpy(msg, "TMK missing");
            break;

            /*
             case E_INVALID_LENGTH:
             case 0x6400:
             case 0x6985:
             case 0x6300:
             case 0x63C0:
             case 0x63C1:
             case 0x63C2:
             case 0x63C3:
             case 0x6A83:
             case 0x6A88:
             case 0x6700:
             case 0x6E00:
             case 0x6D00:
             vdGetMessageFromFile(EMV_FAILURE_MSG21, msg);
             break;
             */
            // Err form HTLE Host

        default:

            usMsgGroup = (usCondition & 0xFF00) >> 8;
            usMsgId = usCondition & 0x000F;
            if ((usMsgGroup == 'A') && (usMsgId <= 9)) {
                strcpy(msg, szErrMsg_A[usMsgId]);
            } else
                if ((usMsgGroup == 'B') && (usMsgId <= 9)) {
                strcpy(msg, szErrMsg_B[usMsgId]);
            } else
                if ((usMsgGroup == 'C') && (usMsgId <= 5)) {
                strcpy(msg, szErrMsg_C[usMsgId]);
            } else
                if ((usMsgGroup == 'K') && (usMsgId <= 2)) {
                strcpy(msg, szErrMsg_K[usMsgId]);
            } 
//                else
//                strcpy(msg, "CARD ERROR!");
            break;
    }

    if (strlen(msg) > 0) {
        CTOS_LCDTPrintXY(1, 8, msg);
        CTOS_Beep();
        CTOS_Beep();
        CTOS_Beep();
        //CTOS_Delay(2000);
        WaitKey(2);
    }
}

void vdCTOSS_EFT_InjectKey(IN int inKeySet, IN int inKeyIndex)
{
    USHORT ret;
    BYTE msg[16];
    BYTE basekey[16];
    BYTE ksn[10];
    CTOS_KMS2KEYWRITE_PARA params;
    int i;
    BYTE testksn[10];
    BYTE testksnlen = 10;

//    CTOS_KMS2Erase();
    
    memset(&params, 0, sizeof(params));

	DebugAddHEX("IPEK", IPEK, 16);
	DebugAddHEX("KSN", InitiallyLoadedKeySerialNumber, 10);

	if (strTCT.fPrintISOMessage == VS_TRUE)
		inPrintISOPacket(VS_TRUE , IPEK, 16);	
	if (strTCT.fPrintISOMessage == VS_TRUE)
		inPrintISOPacket(VS_TRUE , InitiallyLoadedKeySerialNumber, 10);	

    memcpy(basekey, IPEK, 16);
    memcpy(ksn, InitiallyLoadedKeySerialNumber, 10);        
    params.Version = 0x01;
    params.Info.KeySet = inKeySet;
    params.Info.KeyIndex = inKeyIndex;
    params.Info.KeyType = KMS2_KEYTYPE_3DES_DUKPT;
    params.Info.KeyVersion = 0x01;
    params.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;        
    params.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;        
    params.Value.KeyLength = 16;
    params.Value.pKeyData = basekey;
    params.DUKPT_DATA.KSNLength = 10;
    params.DUKPT_DATA.pKSN = ksn;        

    ret = CTOS_KMS2KeyWrite(&params);    

    vdDebug_LogPrintf("vdCTOSS_EFT_InjectKey[%d]keyset[%d]keyindex[%d]", ret, inKeySet, inKeyIndex);

}
int inSaveTMK(void) {
    char szTMKFileName[FILENAME_SIZE + 1];
    FILE *hHandle;
    int inResult;
    char szTWKFileName[FILENAME_SIZE + 1];
    TMK_FILE srTMK_FILE;
	int inRetVal = 0;
	char szAqcID[3 + 1];
	char szVendorId[8 + 1];
	char szDownType[100 + 1];
	int inRet = -1;
	
	memset(szDownType, 0x00 ,sizeof(szDownType));
	inRet = get_env("#DOWNTYPE", szDownType, sizeof(szDownType));

	if (inRet == d_OK)
	{
		if (szDownType[0] == '5')
		{
			vdCTOSS_EFT_InjectKey(strEFT[0].inKeySet, strEFT[0].inKeyIndex);
			return (VS_SUCCESS);
		}
	}
	
    //  save TMK data to TMK File
    memset((void*) &srTMK_FILE, 0, sizeof (srTMK_FILE));
    // put TMK-id
    memcpy(srTMK_FILE.szTMKid, TMKresponse.resTMKid, sizeof (srTMK_FILE.szTMKid));
    // put TMK-data
    in3DES_CBC_Encrypt(CardData.szTMK, 16, TMFK_keys, srTMK_FILE.szTMKey);

    // save TMK data to File
    memset(szTMKFileName, 0x00, sizeof (szTMKFileName));
	memset(szAqcID, 0, sizeof (szAqcID));
	memset(szVendorId, 0, sizeof (szVendorId));

	strcpy(szAqcID, szGetAqcID());
	strcpy(szVendorId, szGetPOSVendorID());
	sprintf(szTMKFileName, "%sTMK%s%s", PUBLIC_PATH, szAqcID, szVendorId);

    strcat(szTMKFileName, TMK_FILE_EXTN);
	if((inRetVal = lnGetFileSize(szTMKFileName)) >= 0)
	    remove(szTMKFileName);

    hHandle = fopen(szTMKFileName, "w+");
    if (hHandle < 0) {
        fclose(hHandle);
        return (VS_ERR);
    }

    inResult = fwrite((void*) &srTMK_FILE, sizeof (srTMK_FILE), 1, hHandle);
    if (inResult == 0L) /* End of file reached, invalid index  */ {
        fclose(hHandle);
        return (VS_ERR);
    }

    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }
    // Save TWK-id

    memset(TWKresponse.resTWKid, '0', sizeof (TWKresponse.resTWKid));
    memset(szTWKFileName, 0x00, sizeof (szTWKFileName));
    sprintf(szTWKFileName, "%sTWK%s%s", PUBLIC_PATH, szAqcID, szVendorId);
    strcat(szTWKFileName, TWK_FILE_EXTN);
	if((inRetVal = lnGetFileSize(szTWKFileName)) >= 0)
	    remove(szTWKFileName);

    hHandle = fopen(szTWKFileName, "w+");
    if (hHandle < 0) {
        fclose(hHandle);
        return (VS_ERR);
    }
    inResult = fwrite((void*) TWKresponse.resTWKid, sizeof (TWKresponse.resTWKid), 1, hHandle);
    if (inResult == 0L) /* End of file reached, invalid index  */ {
        fclose(hHandle);
        return (VS_ERR);
    }

    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }

    /*SAVE   TWK Header = TWKid*/
    return (VS_SUCCESS);
}

int inLoadTMK(TMK_FILE *srTMK_FILE) 
{
    char szTMKFileName[FILENAME_SIZE + 1];
    FILE *hHandle;
    int inResult;
    unsigned char szBuf[50];
	char szAqcID[3 + 1];
	char szVendorId[8 + 1];

    // save TMK data to File
    memset(szTMKFileName, 0x00, sizeof (szTMKFileName));

	strcpy(szAqcID, szGetAqcID());
	strcpy(szVendorId, szGetPOSVendorID());	
    sprintf(szTMKFileName, "%sTMK%s%s", PUBLIC_PATH, szAqcID, szVendorId);
    strcat(szTMKFileName, TMK_FILE_EXTN);
	vdDebug_LogPrintf("szTMKFileName[%s]", szTMKFileName);	

    hHandle = fopen(szTMKFileName, "r+");
    if (hHandle < 0) {
//        fclose(hHandle);
        return (VS_ERR);
    }

    memset((void*) srTMK_FILE, 0, sizeof (TMK_FILE));
    inResult = fread((void*) srTMK_FILE, sizeof (TMK_FILE), 1, hHandle);

    if (inResult == 0L) /* End of file reached, invalid index  */ {
        fclose(hHandle);
        return (VS_ERR);
    }

    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }

    // decrytpt TMK-key
    // put TMK-data
    in3DES_CBC_Decrypt(srTMK_FILE->szTMKey, 16, TMFK_keys, szBuf);
    memcpy(srTMK_FILE->szTMKey, szBuf, 16);
	vdPCIDebug_HexPrintf("szTMKey", szBuf, 16);
	
    return VS_SUCCESS;
}

int inSaveTWK(void) 
{
    FILE *hHandle;
    int inResult;
    char szTWKFileName[FILENAME_SIZE + 1];
    TWK_FILE srTWK_FILE;
	int inRetVal = 0;
	char szAqcID[3 + 1];
	char szVendorId[8 + 1];

    memset((void*) &srTWK_FILE, 0x00, sizeof (srTWK_FILE));
    memset(szTWKFileName, 0x00, sizeof (szTWKFileName));
	strcpy(szAqcID, szGetAqcID());
	strcpy(szVendorId, szGetPOSVendorID());	
    sprintf(szTWKFileName, "%sTWK%s%s", PUBLIC_PATH, szAqcID, szVendorId);

    strcat(szTWKFileName, TWK_FILE_EXTN);
	if((inRetVal = lnGetFileSize(szTWKFileName)) >= 0)
	    remove(szTWKFileName);

    hHandle = fopen(szTWKFileName, "w+");
    if (hHandle < 0) {
        fclose(hHandle);
        return (VS_ERR);
    }

    // put data to struct
    memcpy(srTWK_FILE.szTWKid, TWKresponse.resTWKid, sizeof (srTWK_FILE.szTWKid));
    memcpy(srTWK_FILE.szTWKeDEK, TWKresponse.resTWKeDEK, sizeof (srTWK_FILE.szTWKeDEK));
    memcpy(srTWK_FILE.szTWKeMAK, TWKresponse.resTWKeMAK, sizeof (srTWK_FILE.szTWKeMAK));

    // write to file
    inResult = fwrite((void*) &srTWK_FILE, sizeof (TWK_FILE), 1, hHandle);
    if (inResult == 0L) /* End of file reached, invalid index  */ {
        fclose(hHandle);
        return (VS_ERR);
    }

    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }

    return (VS_SUCCESS);
}

int inSaveTWKRSA(void) 
{
    FILE *hHandle;
    int inResult;
    char szTWKFileName[FILENAME_SIZE + 1];
    TWK_FILE srTWK_FILE;
	int inRetVal = 0;
	char szAqcID[3 + 1];
	char szVendorId[8 + 1];

    memset((void*) &srTWK_FILE, 0x00, sizeof (srTWK_FILE));
    memset(szTWKFileName, 0x00, sizeof (szTWKFileName));
	strcpy(szAqcID, szGetAqcID());
	strcpy(szVendorId, szGetPOSVendorID());	
    sprintf(szTWKFileName, "%sTWK%s%s", PUBLIC_PATH, szAqcID, szVendorId);

    strcat(szTWKFileName, TWK_FILE_EXTN);
	if((inRetVal = lnGetFileSize(szTWKFileName)) >= 0)
	    remove(szTWKFileName);

    hHandle = fopen(szTWKFileName, "w+");
    if (hHandle < 0) {
        fclose(hHandle);
        return (VS_ERR);
    }

    // put data to struct
    memcpy(srTWK_FILE.szTWKid, TWKRSAresponse.resTWKid, sizeof (srTWK_FILE.szTWKid));
    memcpy(srTWK_FILE.szTWKeDEK, TWKRSAresponse.resTWKeDEK, sizeof (srTWK_FILE.szTWKeDEK));
    memcpy(srTWK_FILE.szTWKeMAK, TWKRSAresponse.resTWKeMAK, sizeof (srTWK_FILE.szTWKeMAK));

    // write to file
    inResult = fwrite((void*) &srTWK_FILE, sizeof (TWK_FILE), 1, hHandle);
    if (inResult == 0L) /* End of file reached, invalid index  */ {
        fclose(hHandle);
        return (VS_ERR);
    }

    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }

    return (VS_SUCCESS);
}

int inLoadTWK(TWK_FILE *srTWK_FILE)
{
    FILE *hHandle;
    int inResult;

    char szTWKFileName[FILENAME_SIZE + 1];
    TMK_FILE srTMK_FILE;
    unsigned char szBuf[50];
	char szAqcID[3 + 1];
	char szVendorId[8 + 1];

    memset(srTWK_FILE, 0, sizeof (TWK_FILE));
    memset(szTWKFileName, 0, sizeof (szTWKFileName));
	strcpy(szAqcID, szGetAqcID());
	strcpy(szVendorId, szGetPOSVendorID());	
    sprintf(szTWKFileName, "%sTWK%s%s", PUBLIC_PATH, szAqcID, szVendorId);

    strcat(szTWKFileName, TWK_FILE_EXTN);
	vdDebug_LogPrintf("szTWKFileName[%s]", szTWKFileName);	

    hHandle = fopen(szTWKFileName, "r+");
    if (hHandle < 0) {
//        fclose(hHandle);
        return (VS_ERR);
    }

    // READ file
    inResult = fread((void*) srTWK_FILE, sizeof (TWK_FILE), 1, hHandle);

    if (inResult == 0L) {
        fclose(hHandle);
        return (VS_ERR);
    }
    if (fclose(hHandle) < 0) {
        return (VS_ERR);
    }

    // LOAD master key
    if (inLoadTMK(&srTMK_FILE) != VS_SUCCESS)
        return (VS_ERR);

    // decrypt TWK
    in3DES_CBC_Decrypt(srTWK_FILE->szTWKeDEK, 16, srTMK_FILE.szTMKey, szBuf);
    memcpy(srTWK_FILE->szTWKeDEK, szBuf, 16);
	vdPCIDebug_HexPrintf("szTWKeDEK", szBuf, 16);

    in3DES_CBC_Decrypt(srTWK_FILE->szTWKeMAK, 16, srTMK_FILE.szTMKey, szBuf);
    memcpy(srTWK_FILE->szTWKeMAK, szBuf, 16);
	vdPCIDebug_HexPrintf("szTWKeMAK", szBuf, 16);

    return (VS_SUCCESS);
}

int inTWKField62Data(unsigned char *pbtBuff, unsigned short *pusLen) {
    int inHostNum;
    TMK_FILE srTMK_FILE;
    TWK_FILE srTWK_FILE;
    TWK_REQ_DATA TWKReqData;

    inHostNum = srTransRec.HDTid;

//    if (inLoadHLERec(inHostNum) != VS_SUCCESS) {
//        return (VS_ERROR);
//    }

    /******** OPEN TMK ********/
    memset((void*) &srTMK_FILE, 0, sizeof (srTMK_FILE));
    if (inLoadTMK(&srTMK_FILE) != VS_SUCCESS) {
        return (VS_ERR);
    }

    /******** OPEN TWK ********/
    memset((void*) &srTWK_FILE, 0, sizeof (srTWK_FILE));
    if (inLoadTWK(&srTWK_FILE) != VS_SUCCESS) {
        // reset TWK ID
        memset(srTWK_FILE.szTWKid, '0', sizeof (srTWK_FILE.szTWKid));
    }

    /***** END TWK ******/
    memset((void*) &TWKReqData, 0, sizeof (TWK_REQ_DATA));
    memcpy(TWKReqData.reqTLEind, "HTLE", sizeof (TWKReqData.reqTLEind));
    memcpy(TWKReqData.reqVer, "01", sizeof (TWKReqData.reqVer));
    //	vdLoadCPACParams(&srCPAC);
    memcpy(TWKReqData.reqAcqId, szGetAqcID(), sizeof (TWKReqData.reqAcqId));
	vdDebug_LogPrintf("szGetAqcID[%s]", szGetAqcID());		
    memcpy(TWKReqData.reqTermId, srTransRec.szTID, sizeof (TWKReqData.reqTermId));
	vdDebug_LogPrintf("szTID[%s]", srTransRec.szTID);		
    memcpy(TWKReqData.reqTMKId, srTMK_FILE.szTMKid, sizeof (TWKReqData.reqTMKId));
    memcpy(TWKReqData.reqTWKId, srTWK_FILE.szTWKid, sizeof (TWKReqData.reqTWKId));
    memcpy((char*) pbtBuff, (char*) &TWKReqData, sizeof (TWK_REQ_DATA));
    *pusLen = (unsigned short) sizeof (TWK_REQ_DATA);
    return VS_SUCCESS;
}

int inTWKRSAField62Data(unsigned char *pbtBuff, unsigned short *pusLen) {
    int inHostNum;
    TMK_FILE srTMK_FILE;
    TWK_FILE srTWK_FILE;
    TWK_RSA_REQ_DATA TWKRSAReqData;

    inHostNum = srTransRec.HDTid;

//    if (inLoadHLERec(inHostNum) != VS_SUCCESS) {
//        return (VS_ERROR);
//    }

    /******** OPEN TMK ********/
    memset((void*) &srTMK_FILE, 0, sizeof (srTMK_FILE));
    if (inLoadTMK(&srTMK_FILE) != VS_SUCCESS) {
        return (VS_ERR);
    }

    /******** OPEN TWK ********/
    memset((void*) &srTWK_FILE, 0, sizeof (srTWK_FILE));
    if (inLoadTWK(&srTWK_FILE) != VS_SUCCESS) {
        // reset TWK ID
        memset(srTWK_FILE.szTWKid, '0', sizeof (srTWK_FILE.szTWKid));
    }

    /***** END TWK ******/
    memset((void*) &TWKRSAReqData, 0, sizeof (TWK_REQ_DATA));
    memcpy(TWKRSAReqData.reqTLEind, "HTLE", sizeof (TWKRSAReqData.reqTLEind));
    memcpy(TWKRSAReqData.reqVer, "04", sizeof (TWKRSAReqData.reqVer));
    memcpy(TWKRSAReqData.reqDownType, "1", sizeof (TWKRSAReqData.reqDownType));
    memcpy(TWKRSAReqData.reqLTMKAcqId, szGetAqcID(), sizeof (TWKRSAReqData.reqLTMKAcqId));
	vdDebug_LogPrintf("szGetAqcID[%s]", szGetAqcID());		
    memcpy(TWKRSAReqData.reqAcqId, szGetAqcID(), sizeof (TWKRSAReqData.reqAcqId));
	vdDebug_LogPrintf("szGetAqcID[%s]", szGetAqcID());		
    memcpy(TWKRSAReqData.reqTermId, srTransRec.szTID, sizeof (TWKRSAReqData.reqTermId));
	vdDebug_LogPrintf("szTID[%s]", srTransRec.szTID);		
    memcpy(TWKRSAReqData.reqVendorId, szGetPOSVendorID(), sizeof (TWKRSAReqData.reqVendorId));
	vdDebug_LogPrintf("szGetPOSVendorID[%s]", szGetPOSVendorID());		
    memcpy(TWKRSAReqData.reqTMKId, srTMK_FILE.szTMKid, sizeof (TWKRSAReqData.reqTMKId));
    memcpy(TWKRSAReqData.reqTWKId, srTWK_FILE.szTWKid, sizeof (TWKRSAReqData.reqTWKId));
    memcpy((char*) pbtBuff, (char*) &TWKRSAReqData, sizeof (TWK_RSA_REQ_DATA));
    *pusLen = (unsigned short) sizeof (TWK_RSA_REQ_DATA);

	return VS_SUCCESS;
}

int inCTOSS_TWKFlowProcess(void) {
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE EMVtagVal[64];
    BYTE szStr[64];

    vdCTOS_SetTransType(EFTSEC_TWK);

    //display title
    vdDispTransTitle(EFTSEC_TWK);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetData();
            if (d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadTable();
            if (d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_MultiAPCheckAllowd();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

    inRet = inSaveTWK();
    if (d_OK != inRet)
        return inRet;

    inRet = inPrintLoadKeysuccess();
    if (d_OK != inRet)
        return inRet;

    return d_OK;
}

int inCTOSS_TWKRSAFlowProcess(void) {
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE EMVtagVal[64];
    BYTE szStr[64];

    vdCTOS_SetTransType(EFTSEC_TWK_RSA);

    //display title
    vdDispTransTitle(EFTSEC_TWK_RSA);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetData();
            if (d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadTable();
            if (d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_MultiAPCheckAllowd();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        return inRet;

//    inRet = inCTOS_CheckTranAllowd();
//    if (d_OK != inRet)
//        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

//    inRet = inSaveTWKRSA();
//    if (d_OK != inRet)
//        return inRet;
    inRet = inSaveTWK();
    if (d_OK != inRet)
        return inRet;

    inRet = inPrintLoadKeysuccess();
    if (d_OK != inRet)
        return inRet;

    return d_OK;
}

int inCTOSS_TWK(void) {
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_TWKFlowProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inEnterTEIDTEpin(void) {
    int inResult;
    char szInputStr[8+1];
    int inTryPIN = 3;
    unsigned int inReturn;
    unsigned char szSendBuff[8], szReceiveBuff[256];
    int i;
    BYTE bKey = 0x00;
    USHORT shMinLen = 1;
    USHORT shMaxLen = 8;
    BYTE Bret;

	memset(szInputStr, 0x00, sizeof (szInputStr));

	CTOS_LCDTClearDisplay();
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Input TE ID");
	setLCDPrint(4, DISPLAY_POSITION_LEFT, " 	 ");

	do {
		clearLine(5);
		clearLine(6);
		clearLine(7);
		clearLine(8);
		CTOS_KBDHit(&bKey);
		memset(szInputStr, 0x00, sizeof (szInputStr));
		shMinLen = 8;
		shMaxLen = 8;
		Bret = InputString(1, 8, 0x00, 0x02, szInputStr, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

		if (inResult < VS_SUCCESS) {
			return (VS_ESCAPE);
		}
		if (strcmp(szInputStr, "") == 0)
			continue;

		strcpy(szTEId, szInputStr);
		memset(szSendBuff, 0x00, sizeof (szSendBuff));
		memset(szReceiveBuff, 0x00, sizeof (szReceiveBuff));

		//return VS_SUCCESS;
		break;
	} while (inTryPIN != 0);

	if (inTryPIN == 0) {
		return VS_ERR;
	}

    memset(szInputStr, 0x00, sizeof (szInputStr));

    CTOS_LCDTClearDisplay();
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Input TE PIN");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, " 	 ");

    do {
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        CTOS_KBDHit(&bKey);
        memset(szInputStr, 0x00, sizeof (szInputStr));
        shMinLen = 8;
        shMaxLen = 8;
        Bret = InputString(1, 8, 0x01, 0x02, szInputStr, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

        if (inResult < VS_SUCCESS) {
            return (VS_ESCAPE);
        }
        if (strcmp(szInputStr, "") == 0)
            continue;

		strcpy(szTEPIN, szInputStr);
        memset(szSendBuff, 0x00, sizeof (szSendBuff));
        memset(szReceiveBuff, 0x00, sizeof (szReceiveBuff));

		return VS_SUCCESS;
    } while (inTryPIN != 0);

    if (inTryPIN == 0) {
        return VS_ERR;
    }
	
    return VS_ERR;
}

int inCTOSS_TMKRSAFlowProcess(void) {
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE EMVtagVal[64];
    BYTE szStr[64];

    vdCTOS_SetTransType(EFTSEC_TMK_RSA);

    //display title
    vdDispTransTitle(EFTSEC_TMK_RSA);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        return inRet;

    inRet = inEnterTEIDTEpin();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

    inRet = inHTLEDecryptAndSaveOnlineKey();
    if (d_OK != inRet)
        return inRet;

    inRet = inSaveTMK();
    if (d_OK != inRet)
        return inRet;

    inRet = inPrintLoadKeysuccess();
    if (d_OK != inRet)
        return inRet;

    return d_OK;
}

int inCTOSS_TMKFlowProcess(void) {
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE EMVtagVal[64];
    BYTE szStr[64];

    vdCTOS_SetTransType(EFTSEC_TMK);

    //display title
    vdDispTransTitle(EFTSEC_TMK);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

    inRet = inTMKDecryptReq();
    if (d_OK != inRet)
        return inRet;

    inRet = inSaveTMK();
    if (d_OK != inRet)
        return inRet;

    inRet = inPrintLoadKeysuccess();
    if (d_OK != inRet)
        return inRet;

    return d_OK;
}

int inCTOSS_TMK(void) {
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_TMKFlowProcess();
    if (VS_SUCCESS == inRet)
	{
		inRet = inCTOSS_TWKFlowProcess();
	}
	
	srTransRec.byEntryMode = CARD_ENTRY_ICC;

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inAutoLoadTWK(void) {
    int inResult = 0;

    inResult = inCTOSS_TWK();
    return inResult;
}

int inPrintLoadKeysuccess(void) {
    if (printCheckPaper() == -1)
        return -1;

    vdDisplayAnimateBmp(0, 0, "Printer1(320240).bmp", "Printer2(320240).bmp", "Printer3(320240).bmp", NULL, NULL);

    if (srTransRec.byTransType == EFTSEC_TMK) {
        CTOS_PrinterPutString("\n**********CASTLES**********\n");
        CTOS_PrinterPutString("\n   LOAD MASTER KEY OK   \n");
    }

    if (srTransRec.byTransType == EFTSEC_TWK) {
        CTOS_PrinterPutString("\n**********CASTLES**********\n");
        CTOS_PrinterPutString("\n*****LOAD KEY SUCCESS***\n");
    }

    if (srTransRec.byTransType == EFTSEC_TMK_RSA) {
        CTOS_PrinterPutString("\n**********CASTLES**********\n");
        CTOS_PrinterPutString("\n   LOAD MASTER KEY OK(RSA)\n");
    }

    if (srTransRec.byTransType == EFTSEC_TWK_RSA) {
        CTOS_PrinterPutString("\n**********CASTLES**********\n");
        CTOS_PrinterPutString("\n*****LOAD KEY SUCCESS(RSA)***\n");
    }

    CTOS_PrinterFline(d_LINE_DOT * 5);

    return (VS_SUCCESS);
}

int inLoadTWKfromTMK(void) {
    inAutoLoadTWK();
    return (VS_SUCCESS);
}

void ShowTitle(char *caStr)
{
	CTOS_LCDTSetReverse(d_TRUE);
	CTOS_LCDTPrintXY(1, 3, caStr);
	CTOS_LCDTSetReverse(d_FALSE);
}

void vdCTOSS_EFTSECKeyCard(void) {
    int inRet = d_NO;

    inRet = inHTLESelectMutipleAcqVendorID();
    if (VS_SUCCESS != inRet)
        return;

    inRet = inHTLEInitCardData();
    if (VS_SUCCESS != inRet)
	{		
		vdRemoveCard();
		return;
	}		

    inRet = inGetPublickey();
    if (VS_SUCCESS != inRet)
	{		
		vdRemoveCard();
		return;
	}		

    inRet = inEnterTEpin();
    if (VS_SUCCESS != inRet)
	{		
		vdRemoveCard();
		return;
	}

    inRet = inTMKSessionKeyGen();
    if (VS_SUCCESS != inRet)
	{		
		vdRemoveCard();
		return;
	}

    inRet = inCTOSS_TMK();
    if (VS_SUCCESS != inRet)
	{    	
        vdRemoveCard();
        return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void vdRSA_GenerateKey(void )
{
	const int kBits = 2048;
	const int kExp = 65537;
	char szPubliPublicKeyFileName[FILENAME_SIZE + 1];
	char szPubliPrivateKeyFileName[FILENAME_SIZE + 1];
	FILE *hfilePub, *hfilePri;

	int keylen;
	char *pem_key;
	BIO *bio;

	RSA *rsa = RSA_generate_key(kBits, kExp, 0, 0);

	memset(szPubliPublicKeyFileName, 0x00, sizeof (szPubliPublicKeyFileName));
	sprintf(szPubliPublicKeyFileName, "%sRSAPub.pem", PUBLIC_PATH);

	hfilePub = fopen( szPubliPublicKeyFileName, "w+");	

	memset(szPubliPrivateKeyFileName, 0x00, sizeof (szPubliPrivateKeyFileName));
	sprintf(szPubliPrivateKeyFileName, "%sRSAPri.pem", PUBLIC_PATH);

	hfilePri = fopen( szPubliPrivateKeyFileName, "w+");
	if( hfilePub <= 0 || hfilePri <= 0 )
	{
//		return VS_ERR;
		return;            
	}
	
	bio = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
	keylen = BIO_pending(bio);
	pem_key = calloc(keylen+1, 1); /* Null-terminate */
	BIO_read(bio, pem_key, keylen);

	fwrite(  pem_key , keylen, 1, hfilePri);
	free(pem_key);
	BIO_free_all(bio);


	bio = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPublicKey(bio , rsa );
	keylen = BIO_pending(bio);
	pem_key = calloc(keylen+1, 1); /* Null-terminate */
	BIO_read(bio, pem_key, keylen);
	fwrite( pem_key , keylen, 1, hfilePub);

	BIO_free_all(bio);
	RSA_free(rsa);
	free(pem_key);
	fclose( hfilePub );
	fclose( hfilePri );

//	return 0;
        return;
}

void vdDisplayErrorString(char* szErrorString)
{
	CTOS_LCDTPrintXY(1, 8, szErrorString);
	CTOS_Beep();
	CTOS_Beep();
	CTOS_Beep();
	CTOS_Delay(1000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int inHTLESelectAcqVendorIDByHostId(int inHDTid)
{
#define ITEMS_PER_PAGE          4

	char szMMT[50];
	char szDisplay[50];
	int inNumOfRecords = 0;
	short shCount = 0;
	short shTotalPageNum;
	short shCurrentPageNum;
	short shLastPageItems = 0;
	short shPageItems = 0;
	short shLoop;
	short shFalshMenu = 1;
	BYTE isUP = FALSE, isDOWN = FALSE;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE  x = 1;
	BYTE key;
	char szHeaderString[50] = " ACQ ID+VENDOR ID";
	char szMitMenu[1024];
	int inLoop = 0;
	short shMinLen = 1;
	short shMaxLen = 20;
	BYTE Bret;
	unsigned char szOutput[30];

	unsigned char bstatus = 0; 

    memset(szMitMenu, 0x00, sizeof(szMitMenu));
    inEFTReadNumofRecordsByHostId(&inNumOfRecords, inHDTid);

	CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
	{
	    for (inLoop = 0; inLoop < inNumOfRecords; inLoop++)
	    {
            strcat((char *)szMitMenu, strEFT[inLoop].szAcquirerID);
            strcat((char *)szMitMenu, " ");
            strcat((char *)szMitMenu, strEFT[inLoop].szVendorID);

			if(inLoop + 1 != inNumOfRecords)
            	strcat((char *)szMitMenu, (char *)" \n");
	    }
	        
	    //key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);
	    key = 0x31;
		vdDebug_LogPrintf("key=[%d]",key);

		if (key == 0xFF) 
		{
		    CTOS_LCDTClearDisplay();
		    setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		    vduiWarningSound();
		    return -1;  
		}

    	if(key > 0)
	    {
	        if(d_KBD_CANCEL == key)
	            return -1;
			
			memcpy(&strEFT[0], &strEFT[key-1], sizeof(STRUCT_EFT));
	    }
	}
    else
		return FAIL;
	
    return SUCCESS;
}

int inHTLESelectMutipleAcqVendorID(void)
{
#define ITEMS_PER_PAGE          4

	char szMMT[50];
	char szDisplay[50];
	int inNumOfRecords = 0;
	short shCount = 0;
	short shTotalPageNum;
	short shCurrentPageNum;
	short shLastPageItems = 0;
	short shPageItems = 0;
	short shLoop;
	short shFalshMenu = 1;
	BYTE isUP = FALSE, isDOWN = FALSE;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE  x = 1;
	BYTE key;
	char szHeaderString[50] = " ACQ ID+VENDOR ID";
	char szMitMenu[1024];
	int inLoop = 0;
	short shMinLen = 1;
	short shMaxLen = 20;
	BYTE Bret;
	unsigned char szOutput[30];

	unsigned char bstatus = 0; 

    memset(szMitMenu, 0x00, sizeof(szMitMenu));
    inEFTReadNumofRecords(&inNumOfRecords);

	CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords >= 1)
	{
	    for (inLoop = 0; inLoop < inNumOfRecords; inLoop++)
	    {
            strcat((char *)szMitMenu, strEFT[inLoop].szAcquirerID);
            strcat((char *)szMitMenu, " ");
            strcat((char *)szMitMenu, strEFT[inLoop].szVendorID);

			if(inLoop + 1 != inNumOfRecords)
            	strcat((char *)szMitMenu, (char *)" \n");
	    }
	        
	    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);
		vdDebug_LogPrintf("key=[%d]",key);

		if (key == 0xFF) 
		{
		    CTOS_LCDTClearDisplay();
		    setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		    vduiWarningSound();
		    return -1;  
		}

    	if(key > 0)
	    {
	        if(d_KBD_CANCEL == key)
	            return -1;
			
			memcpy(&strEFT[0], &strEFT[key-1], sizeof(STRUCT_EFT));
			vdDebug_LogPrintf("strEFT[0].HDTid =[%d]", strEFT[0].inHDTid);
	    }
	}
    else
		return FAIL;
	
    return SUCCESS;
}

int inCTOSS_TMK_RSA(void) {
    int inRet = d_NO;
	char szDownType[100 + 1];

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_TMKRSAFlowProcess();
    if (VS_SUCCESS == inRet)
	{
		memset(szDownType, 0x00 ,sizeof(szDownType));
		inRet = get_env("#DOWNTYPE", szDownType, sizeof(szDownType));
		
		if (inRet == d_OK)
		{
			if (szDownType[0] == '5')
			{				
				srTransRec.byEntryMode = CARD_ENTRY_ICC;
				inCTOS_inDisconnect();	
				vdCTOS_TransEndReset();
				return d_OK;
			}
		}
		inCTOS_inDisconnect();
		inRet = inCTOSS_TWKRSAFlowProcess();
	}
	
	srTransRec.byEntryMode = CARD_ENTRY_ICC;

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

void vdTLELoadMasterKeyOnline(void)
{
	char szPubliPublicKeyFileName[FILENAME_SIZE + 1];
	char szPubliPrivateKeyFileName[FILENAME_SIZE + 1];
	FILE * hRSAFile;
	int inCode, inTransactionCode,TRTransactionCode;
	int inRetVal =  VS_ERR;
	long lnMessage;
	RSA * rsa = NULL;
	unsigned char* szTestBuff1 = malloc( 256 );
	unsigned char* szTestBuff2 = malloc( 256 );
    int inRet = d_NO;

    inRet = inHTLESelectMutipleAcqVendorID();
    if (VS_SUCCESS != inRet)
        return;

	memset(szPubliPublicKeyFileName, 0x00, sizeof (szPubliPublicKeyFileName));
	sprintf(szPubliPublicKeyFileName, "%sRSAPub.pem", PUBLIC_PATH);

	hRSAFile = fopen( szPubliPublicKeyFileName, "r" );
	vdDebug_LogPrintf("hRSAFile=[%d]",hRSAFile);
	if (hRSAFile <= 0)
	{
		//fclose(hRSAFile);
		
		vdRSA_GenerateKey();
		            
		hRSAFile = fopen( szPubliPublicKeyFileName, "r" );
		if (hRSAFile <= 0)
		{
			//fclose(hRSAFile);
			vdDisplayErrorString("Can't Create Key.");
//			return VS_ERR;
			return;
		}		
	}

	rsa = PEM_read_RSAPublicKey(hRSAFile,NULL,NULL,NULL);
	if( rsa == NULL )
	{
		vdDisplayErrorString("TLE ONLINE KEY ERR");
//		return VS_ERR;
		return;
	}
	memset( szTestBuff1 , 0 , 256 );
	inRetVal = RSA_public_encrypt( 5,(uchar *)"nant" , (uchar *)szTestBuff1 , rsa ,RSA_PKCS1_PADDING );
	if(  inRetVal < 0 )
	{
		vdDisplayErrorString("TLE ONLINE KEY ERR");
	}

	RSA_free( rsa ); 
	
	fclose(hRSAFile); 
	
	memset(szPubliPrivateKeyFileName, 0x00, sizeof (szPubliPrivateKeyFileName));
	sprintf(szPubliPrivateKeyFileName, "%sRSAPri.pem", PUBLIC_PATH);
	
	hRSAFile = fopen( szPubliPrivateKeyFileName, "r" );
	if (hRSAFile <= 0)
	{
		//fclose(hRSAFile);
		
		vdRSA_GenerateKey();
		            
		hRSAFile = fopen(szPubliPrivateKeyFileName, "r" );
		if (hRSAFile <= 0)
		{
			//fclose(hRSAFile);
			vdDisplayErrorString("Can't Create Key.");
//			return VS_ERR;
			return;
		}	
	}

	rsa = NULL;	
	rsa = PEM_read_RSAPrivateKey(hRSAFile,NULL,NULL,NULL);

	if( rsa == NULL )
	{
		vdDisplayErrorString("TLE ONLINE KEY ERR");
//		return VS_ERR;
		return;
	}

	memset( szTestBuff2 , 0 , 256 );

	inRetVal = RSA_private_decrypt( RSA_size(rsa) ,(uchar *)szTestBuff1, (uchar *)szTestBuff2,rsa,  RSA_PKCS1_PADDING );
	if(  inRetVal < 0 || strcmp(szTestBuff2,"nant") )
	{
		vdDisplayErrorString("TLE ONLINE KEY ERR");
	}

	fclose(hRSAFile);
	free( szTestBuff1 );
	free( szTestBuff2 );

	if (inRetVal >=0)
	{
		inCTOSS_TMK_RSA();
	}

	RSA_free(rsa);
//	return inRetVal;
	return;        
}

void vdHTLELoadField62DataRSA(unsigned char *pbtBuff, unsigned short *pusLen)
{
    char szBuf[20];
    char btTmp[512];
    int inSize = 0;
    char szTLEindc[4 + 1];
    char szVersion[2 + 1];
    char szAqcID[3 + 1];
    char szTID[8 + 1];
    char szVendorId[8 + 1];
    char szDownType[100 + 1];
    char szRequestType[1 + 1];
    char szTEID[8 + 1];
    unsigned char szTxnHash[SHA_DIGEST_LENGTH + 1];

	short byteCount = 2, i = 0;
	unsigned char szSHA1Hash[SHA_DIGEST_LENGTH+1];
	unsigned char szSHA1HEX[2*SHA_DIGEST_LENGTH+1];
	
	char szCoockedBuff[40+1];
	char szTrace[8+1]={0};
	int inRet = -1;	
	//SHA_CTX sha;
	SHA_CTX_OP sha;

	FILE *hRSAFile ;
	//char * ptrTmpMustFree;
	RSA * rsa;
	char szPubliPublicKeyFileName[FILENAME_SIZE + 1];
	unsigned char szRSAPub[259+1];

    memset(btTmp, 0, sizeof (btTmp));
    memset(szTLEindc, 0, sizeof (szTLEindc));
    memset(szVersion, 0, sizeof (szVersion));
    memset(szDownType, 0, sizeof (szDownType));
    memset(szRequestType, 0, sizeof (szRequestType));
    memset(szAqcID, 0, sizeof (szAqcID));
    memset(szTID, 0, sizeof (szTID));
    memset(szVendorId, 0, sizeof (szVendorId));
    memset(szTEID, 0, sizeof (szTEID));
    memset(szTxnHash, 0, sizeof (szTxnHash));

    memcpy(szTLEindc, "HTLE", 4);
    memcpy(szVersion, "04", 2);
	memset(szDownType, 0x00 ,sizeof(szDownType));
	inRet = get_env("#DOWNTYPE", szDownType, sizeof(szDownType));

	if (inRet != d_OK)
    memcpy(szDownType, "4", 1);
    memcpy(szRequestType, "1", 1);
    memcpy(szAqcID, szGetAqcID(), 3);
    memcpy(szTID, srTransRec.szTID, 8);
    memcpy(szVendorId, szGetPOSVendorID(), 8);
    memcpy(szTEID, szGetTE_ID(), 8);
	vdDebug_LogPrintf("szTEId=[%s], szGetTE_ID=[%s]", szTEId,  szGetTE_ID());

	memset(szSHA1Hash, 0 , SHA_DIGEST_LENGTH+1 );
	memset(szCoockedBuff, 0, sizeof(szCoockedBuff) );
	memcpy(szCoockedBuff, szGetTE_ID(), 8);
	memcpy(szCoockedBuff+8, szGetTE_PIN(), 8);

	vdDebug_LogPrintf("szGetTE_ID=[%s], szGetTE_PIN=[%s]", szGetTE_ID(),  szGetTE_PIN());
	
	memcpy(szCoockedBuff+16, "1234", 4);

	vdDebug_LogPrintf("szCoockedBuff[%s]", szCoockedBuff);
	SHA1_Init(&sha);
	SHA1_Update(&sha,szCoockedBuff,20);
	SHA1_Final((unsigned char *)szSHA1Hash,&sha);
	
	for( i = 0  ; i < SHA_DIGEST_LENGTH ; ++i )
	{
		sprintf(&szSHA1HEX[2*i],"%02X" , szSHA1Hash[i]);
	}
	vdDebug_LogPrintf("szSHA1HEX=[%s]", szSHA1HEX);
	
	memset(szCoockedBuff , 0, sizeof(szCoockedBuff) );
	memcpy(szCoockedBuff, szSHA1HEX , 8);
	memcpy(szCoockedBuff+8, srTransRec.szTID, 8);
	sprintf(szTrace,"%06ld", srTransRec.ulTraceNum);
	memcpy(szCoockedBuff+16, szTrace+strlen(szTrace)-4 , 4);
	vdDebug_LogPrintf("szCoockedBuff[%s]", szCoockedBuff);	
	SHA1_Init(&sha);
	SHA1_Update(&sha,szCoockedBuff,20);
	SHA1_Final((unsigned char *)szSHA1Hash,&sha);

	//CheckSum
	for( i = 0  ; i < 4 ; ++i )
	{
		sprintf((char *)&szTxnHash[(2*i)],"%02X" , szSHA1Hash[i]);
	}
	vdDebug_LogPrintf("szTxnHash=[%s]", szTxnHash);

	memset(szPubliPublicKeyFileName, 0x00, sizeof (szPubliPublicKeyFileName));
	sprintf(szPubliPublicKeyFileName, "%sRSAPub.pem", PUBLIC_PATH);
	hRSAFile = fopen(szPubliPublicKeyFileName, "r" );
	rsa = PEM_read_RSAPublicKey(hRSAFile,NULL,NULL,NULL);
	vdDebug_LogPrintf("44444");

	byteCount = 0;	
	BN_bn2bin(rsa->e, szRSAPub+byteCount);
	byteCount += BN_num_bytes(rsa->e);
		
	BN_bn2bin(rsa->n, szRSAPub+byteCount);
	byteCount += BN_num_bytes(rsa->n);	

	RSA_free( rsa );
	fclose( hRSAFile );
	vdDebug_LogPrintf("555555");

    memcpy(&btTmp[0], szTLEindc, 4);
    memcpy(&btTmp[0 + 4], szVersion, 2);
    memcpy(&btTmp[0 + 4 + 2], szDownType, 1);
    memcpy(&btTmp[0 + 4 + 2 + 1], szRequestType, 1);
    memcpy(&btTmp[0 + 4 + 2 + 1 + 1], szAqcID, 3);
    memcpy(&btTmp[0 + 4 + 2 + 1 + 1 + 3], szTID, 8);
    memcpy(&btTmp[0 + 4 + 2 + 1 + 1 + 3 + 8], szVendorId, 8);
    memcpy(&btTmp[0 + 4 + 2 + 1 + 1 + 3 + 8 + 8], szTEID, 8);
    memcpy(&btTmp[0 + 4 + 2 + 1 + 1 + 3 + 8 + 8 + 8], szTxnHash, 8);
	
	vdDebug_LogPrintf("szTEID=[%s], szTxnHash=[%s],", szTEID, szTxnHash);

    memcpy(&btTmp[0 + 4 + 2 + 1 + 1 + 3 + 8 + 8 + 8 + 8], szRSAPub, 259);

    memcpy(&pbtBuff[0], btTmp, 4 + 2 + 1 + 1 + 3 + 8 + 8 + 8 + 8 + 259);
    inSize = 4 + 2 + 1 + 1 + 3 + 8 + 8 + 8 + 8 + 259;
    *pusLen = (unsigned short) inSize;
	vdDebug_LogPrintf("6666666");

//    return VS_SUCCESS;
    return;        
}

/*
	R.nant
	inHTLEDecryptAndSaveOnlineKey

	this function encrypts and save TLE key from the server.

	--input encrypted FIELD62 key
	--output saved decrypted key
*/

int inHTLEDecryptAndSaveOnlineKey(void)
{
	short offset = 13;
	int inRetVal;
	FILE * hPriKey;
	unsigned char pszDUPKT[16];
	unsigned char szZeros[16],szTestOut[16]; 
	unsigned char *pszEncryptKey = TMKRSAresponse.resKMSRSAKey; // remark
	RSA *rsa;
	char szPubliPrivateKeyFileName[FILENAME_SIZE + 1];

	memset(szPubliPrivateKeyFileName, 0x00, sizeof (szPubliPrivateKeyFileName));
	sprintf(szPubliPrivateKeyFileName, "%sRSAPri.pem", PUBLIC_PATH);
	hPriKey = fopen(szPubliPrivateKeyFileName, "r" );

	if( hPriKey <= 0 )
		return VS_ERROR;

	rsa = PEM_read_RSAPrivateKey(hPriKey,NULL,NULL,NULL);
	if( rsa == NULL )
		return VS_ERROR;

	inRetVal = RSA_private_decrypt( RSA_size(rsa) ,pszEncryptKey, pszDUPKT,rsa,  RSA_PKCS1_PADDING );
	if(  inRetVal < 0 )
	{
		vdDisplayErrorString("TLE ONLINE KEY ERR");
		return VS_ERROR;
	}
//	LOG_PRINTF(("Decrypted TMK"));
//	LOG_PRINT_HEX(pszDUPKT , 16);

	pszEncryptKey += RSA_size(rsa); // point to KCV
//	LOG_PRINTF(("KCV"));
//	LOG_PRINT_HEX(pszEncryptKey , 6);
	if (strTCT.fPrintISOMessage == VS_TRUE)
		inPrintISOPacket(VS_TRUE , pszEncryptKey, 6);	
	vdDebug_LogPrintf("KCV=[%s]",pszEncryptKey);

	memset( szZeros , 0 , 16 );
	memset( szTestOut , 0, 16 );
	in3DES_CBC_Encrypt( szZeros ,16,pszDUPKT ,szTestOut);
	DebugAddHEX("szTestOut", szTestOut, 6); 			
//	LOG_PRINT_HEX(szTestOut , 16);
	pszEncryptKey += 6; // point to Key Identifier.

//	LOG_PRINTF(("key iden"));
	DebugAddHEX("IKSN", pszEncryptKey, 20); 			
//	LOG_PRINT_HEX( pszEncryptKey , 20 );

	memcpy ( CardData.szTMK , pszDUPKT,16);
	memcpy ( TMKresponse.resTMKid, pszEncryptKey ,4);// 4 bytes only for tmk
	memcpy ( IPEK , pszDUPKT,16);
	wub_str_2_hex(pszEncryptKey,InitiallyLoadedKeySerialNumber,20);

	RSA_free(rsa);
	fclose(hPriKey);

	return VS_SUCCESS;
}

void vdHTLESelectloadMethods(void)
{
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
	BYTE key;
    char szHeaderString[50] = "PLEASE SELECT";
    char szMitMenu[1024];
    int inLoop = 0;
	
	memset(szMitMenu,0x00,sizeof(szMitMenu));
	strcpy((char *)szMitMenu,"Online Download");
	strcat((char *)szMitMenu, (char *)" \n");
	strcat((char *)szMitMenu,"IC Load");
        
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

	if (key == 0xFF) 
	{
	    CTOS_LCDTClearDisplay();
	    setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
	    vduiWarningSound();
	    return ;  
	}

	if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return ;
        
		if (key == 1)
		{
			vdTLELoadMasterKeyOnline();
		}

		if (key == 2)
		{
			vdCTOSS_EFTSECKeyCard();
		}
    }
}

int inEFTSecTest(void)
{
	BYTE	szDataTmp[1024+1] = {0};
	BYTE	szErrResponse[1024+1] = {0};
	int inReqSiz = 0;
	unsigned int inResSiz = 0;
	
	strcpy(strEFT[0].szAcquirerID, "002");
	strcpy(strEFT[0].szVendorID, "00000001");
	strcpy(TMKresponse.resTMKid, "0007");
	
	wub_str_2_hex("B0C13E45EF0D6D58970D8CD9D3ECCE25", CardData.szTMK, strlen("B0C13E45EF0D6D58970D8CD9D3ECCE25"));
	
	inSaveTMK();

	strcpy(strEFT[0].szAcquirerID, "002");
	strcpy(strEFT[0].szVendorID, "00000001");

	memset((void*)&TWKresponse, 0, sizeof(TWK_RES_DATA));
	memset(szDataTmp, 0x00, sizeof(szDataTmp));
	wub_str_2_hex("48544C45303130303037840EB31D31D41D51D3D78F403CC60AC78097963ED0AD93BD4BE35A4B1D357DC54338324245384231463041343341304", szDataTmp, strlen("48544C45303130303037840EB31D31D41D51D3D78F403CC60AC78097963ED0AD93BD4BE35A4B1D357DC54338324245384231463041343341304"));
	memcpy((void*)&TWKresponse, szDataTmp, sizeof(TWK_RES_DATA));
	
	inSaveTWK();

 	inReqSiz = strlen("600800000002003020078020C0020400000000000000868600001800510000080000374524192002008427D20092010000001199000F393030303030303131303030303030303030303434343901105F2A0200965F34010082023C008407A0000000031010950500800000009A031606239C01009F02060000000086869F03060000000000009F100706020A03A0B8029F1A0200969F1E0831323334353637389F2608E5AFAA163FD4AEDD9F3303E0B0C89F360200A09F3704A7C17DDD0006303030303230")/2;
	wub_str_2_hex("600800000002003020078020C0020400000000000000868600001800510000080000374524192002008427D20092010000001199000F393030303030303131303030303030303030303434343901105F2A0200965F34010082023C008407A0000000031010950500800000009A031606239C01009F02060000000086869F03060000000000009F100706020A03A0B8029F1A0200969F1E0831323334353637389F2608E5AFAA163FD4AEDD9F3303E0B0C89F360200A09F3704A7C17DDD0006303030303230", szDataTmp, strlen("600800000002003020078020C0020400000000000000868600001800510000080000374524192002008427D20092010000001199000F393030303030303131303030303030303030303434343901105F2A0200965F34010082023C008407A0000000031010950500800000009A031606239C01009F02060000000086869F03060000000000009F100706020A03A0B8029F1A0200969F1E0831323334353637389F2608E5AFAA163FD4AEDD9F3303E0B0C89F360200A09F3704A7C17DDD0006303030303230"));

	inCTOSS_ProcessEFT01Send( 1, 0xC000, 0x0000, '1', "201", "4004000020090200", "002", "00000001", szDataTmp, &inReqSiz);
	DebugAddHEX("inCTOSS_ProcessEFT01Send", szDataTmp, inReqSiz);

 	inReqSiz = strlen("60000000220210303801000E8000810000000000000086860000180000000000080036313735313638373233383920202020202038393930303030303031015648544C453031303032393030303030303132303130303037303030313131340000000000E3AF0769D7967CB5C97A4F287AE2296003C6BDD8EE2DD875F5A47C40958156B94118509F641C50BEC5F178EBEB553C36938696D46AB898C3E618862C7A7C5A07F26C610F813D92887076FAF816C0F5326D7586DBA2D763B2589CC91920B670AB3AFD246A463994D36523EEC4933B3D15214E527927E59987C642761200000000")/2;
	wub_str_2_hex("60000000220210303801000E8000810000000000000086860000180000000000080036313735313638373233383920202020202038393930303030303031015648544C453031303032393030303030303132303130303037303030313131340000000000E3AF0769D7967CB5C97A4F287AE2296003C6BDD8EE2DD875F5A47C40958156B94118509F641C50BEC5F178EBEB553C36938696D46AB898C3E618862C7A7C5A07F26C610F813D92887076FAF816C0F5326D7586DBA2D763B2589CC91920B670AB3AFD246A463994D36523EEC4933B3D15214E527927E59987C642761200000000", szDataTmp, strlen("60000000220210303801000E8000810000000000000086860000180000000000080036313735313638373233383920202020202038393930303030303031015648544C453031303032393030303030303132303130303037303030313131340000000000E3AF0769D7967CB5C97A4F287AE2296003C6BDD8EE2DD875F5A47C40958156B94118509F641C50BEC5F178EBEB553C36938696D46AB898C3E618862C7A7C5A07F26C610F813D92887076FAF816C0F5326D7586DBA2D763B2589CC91920B670AB3AFD246A463994D36523EEC4933B3D15214E527927E59987C642761200000000"));

	inResSiz = inReqSiz;

	inCTOSS_ProcessEFT01Recv( 1, 0xC000, 0x0000, '1', "201", szDataTmp, &inResSiz, szErrResponse);

 	DebugAddHEX("inCTOSS_ProcessEFT01Recv", szDataTmp, inResSiz);

}

