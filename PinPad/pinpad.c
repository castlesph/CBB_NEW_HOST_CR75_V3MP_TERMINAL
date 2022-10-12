#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

#include "../Database/DatabaseFunc.h"
#include "pinpad.h"
#include "../Includes/wub_lib.h"
#include "../Includes/CTOSInput.h"
#include "../Includes/EFTSec.h"
#include "../Includes/POSTypedef.h"
#include "..\Debug\Debug.h"
#include "../Ui/Display.h"
#include "../External/External.h"
#include "..\Includes\showbmp.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Includes\POSTrans.h"

int ginPinByPass = 0;

#define PIN_POSITION_X	8
#define PIN_POSITION_Y	7

BYTE g_byPINDigits = 0;

char card_holder_pin[16+1];

int inInitializePinPad(void)
{
		CTOS_KMS2Init();
		TEST_Write3DES_Plaintext();
		return d_OK;
}
//For testing and development only, hardcode the key
void TEST_Write3DES_Plaintext(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	// patrick test key 20150706	
	KeySet = 0x0002;
	KeyIndex = 0x0002;
	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);

	#if 0 //AAA
	strcpy(srTransRec.szPAN,"1234567890123456");
	srTransRec.HDTid = 6;
	srTransRec.byTransType = SALE;
	strcpy(strCST.szCurSymbol,"SGD");
	strcpy(srTransRec.szTotalAmount,"1");
	inGetIPPPin();
	memset(szDataIn, 'A', 100);
	memset(szInitialVector, 0, 8);
	memset(szMAC, 0, 8);
	inIPPGetMAC(szDataIn, 8, szInitialVector, szMAC);
	#endif
}
	
void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine)
{
	char szDisplayBuf[50+1];
	char szDisplayBuf1[50+1];
	BYTE baAmount[20+1];
        char szTempAmt[AMT_ASC_SIZE+1];

	DebugAddSTR("inCTOS_DisplayCurrencyAmount","Processing...",20);
	
	memset(baAmount, 0x00, sizeof(baAmount));
	wub_hex_2_str(szAmount, baAmount, 6);
        
	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	memset(szDisplayBuf1, 0x00, sizeof(szDisplayBuf1));
	// patrick add code 20141216
	//sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);	
	//format amount 10+2
        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
            memset(szTempAmt, 0x00, sizeof(szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(baAmount)/100);
            memset(baAmount, 0x00, sizeof(baAmount));
            strcpy(baAmount, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", baAmount, szDisplayBuf1);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", baAmount, szDisplayBuf1);
        }
        
	sprintf(szDisplayBuf,"%s %s", strCST.szCurSymbol,szDisplayBuf1);
	vdDebug_LogPrintf("AAA - inCTOS_DisplayCurrencyAmount szDisplayBuf:%s",szDisplayBuf);
	setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szDisplayBuf);
}

void OnGetPINDigit(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
	
   DebugAddINT("OnGetPINDigit", NoDigits);
   g_byPINDigits = NoDigits;

}

void OnGetPINCancel(void)
{
   DebugAddINT("OnGetPINCancel", 1);

}

void OnGetPINBackspace(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
   DebugAddINT("OnGetPINBackspace", NoDigits);
}

void vdDisplayErrorMessage(USHORT errorID)
{
	char szErr[50+1];

	memset(szErr,0x00,sizeof(szErr));
	switch(errorID)
	{
		case d_KMS2_GET_PIN_TIMEOUT:
			strcpy(szErr,"TIME OUT");
			break;
		case d_KMS2_GET_PIN_ABORT:
			strcpy(szErr,"GET PIN ABORT");
			break;
		default:
			strcpy(szErr,"GET PIN ABORT");
			break;
	}
	vdDisplayErrorMsg(1, 8, szErr);
}

#if 0
int inGetIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

	BYTE str[40],key;
	USHORT ret;
	int inRet;

	vdDebug_LogPrintf("inGetIPPPin start");
	
	DebugAddSTR("inGetIPPPin","Processing...",20);
//	inDCTRead(srTransRec.HDTid);
	inDCTRead(6);

// patrick test key 20150706 start
//	strDCT.usKeySet = 0x0002;
//	strDCT.usKeyIndex = 0x0002;
//	strDCT.inMinPINDigit = 6;
//	strDCT.inMaxPINDigit = 6;
//	memcpy(strDCT.szPINKey, "\x8B\x3E\xD0\xE8\xC2\x04\x1E\xAE\xEE\x7A\x7D\x0D\x63\x99\x92\x38",  16); 
//	strcpy(srTransRec.szPAN, "6279022411700004061");
// patrick test key 20150706 end

	/*check for keys if injected -- sidumili*/
    inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);
    if (inRet != d_OK)
        return(inRet);
    /*check for keys if injected -- sidumili*/
	
    vdDebug_LogPrintf("inDCTRead [%ld] [%ld] [%ld]", srTransRec.HDTid, strDCT.usKeyIndex, strDCT.usKeySet);

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
	stPinGetPara.Version = 0x02;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	stPinGetPara.Protection.pSK = strDCT.szPINKey;
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	stPinGetPara.Control.Timeout = 60;
	stPinGetPara.Control.NULLPIN = TRUE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

    CTOS_KBDBufFlush();//cleare key buffer

	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
	if(ret != d_OK)
	{
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
			memset(srTransRec.szPINBlock,0x00,sizeof(srTransRec.szPINBlock));
			vdDebug_LogPrintf("PIN BY PASSED");
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            vdDisplayErrorMessage(ret);
            return ret;
        }
    }

	vdPCIDebug_HexPrintf("Pin Block",srTransRec.szPINBlock,8);
    vdDebug_LogPrintf(" pin enter");
	
	return d_OK;
}
#endif

int inGetIPPPin(void)
{
    CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

    BYTE str[40],key;
    USHORT ret;
	char szPINKey[16+1];
	BYTE   EMVtagVal[64];
    USHORT EMVtagLen;

	CTOS_KMS2Init();


	vdDebug_LogPrintf("inGetIPPPin....");

	#ifdef PIN_CHANGE_ENABLE
	if(srTransRec.byTransType == CHANGE_PIN)
		return d_OK;

	#endif

    DebugAddSTR("inGetIPPPin","Processing...",20);
    vdDebug_LogPrintf("*** HDTid 1 = %d",srTransRec.HDTid);
    inDCTRead(srTransRec.HDTid);

	strDCT.inMinPINDigit = 4;
	strDCT.inMaxPINDigit = 12;
//	

#if 0
	strDCT.usKeySet = MPU_PIK_KEYSET;
	strDCT.usKeyIndex = MPU_PIK_KEYIDX; // CipherKeyIndex/TPK index 0014

#endif

	vdDebug_LogPrintf("strDCT.usKeySet[%04X]", strDCT.usKeySet);
    vdDebug_LogPrintf("strDCT.usKeyIndex[%04X]", strDCT.usKeyIndex);
	
    CTOS_LCDTClearDisplay();
	if (strTCT.byPinPadMode != 1)
	{
		#ifdef QUICKPASS
        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        {
        	#if 1 // for testing, 6244 1902 (UPI) should support pin entry if hostid MPU (17, 18 or 19) 09072020
			if(srTransRec.HDTid == 7)
			{
            	EMVtagLen = 3;
            	memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
            	if ((EMVtagVal[0] != 0x02) && (EMVtagVal[0] != 0x42))
            	{
                	return d_OK;
            	}
			}
			#endif
            
            if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.HDTid == 7) /*Host 7 - UPI Finexus*/
                return d_OK;
        }
		#endif
		
        vduiClearBelow(2);
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
        vdDebug_LogPrintf("*** HDTid 2 = %d",srTransRec.HDTid);
        vdDebug_LogPrintf("strDCT.szDisplayLine1 [%s]", strDCT.szDisplayLine1);
        vdDebug_LogPrintf("strDCT.szDisplayLine2 [%s]", strDCT.szDisplayLine2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	}
	else
	{
		displaybmpEx(0, 0, "Enterpin.bmp");
	}
	
	if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3))
	{
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "Please key in PINPAD");
		
		memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
		stOLPinParam.HDTid = srTransRec.HDTid;
		stOLPinParam.ushKeySet = strDCT.usKeySet;
		stOLPinParam.ushKeyIdx = strDCT.usKeyIndex;
		strcpy(stOLPinParam.szPAN,srTransRec.szPAN);
		memcpy(stOLPinParam.szPINKey,strDCT.szPINKey,16);
                DebugAddHEX("PIN KEY", stOLPinParam.szPINKey, 16);
		ret = inCTOSS_EXTGetIPPPin();
		
		memset(card_holder_pin,0,sizeof(card_holder_pin));
		if (ret == d_OK)
		{
			if (stOLPinParam.inPINEntered == 2)
			{
				memcpy(srTransRec.szPINBlock,stOLPinParam.szPINBlock,8);
				wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
			}
			ret = stOLPinParam.ushKeySet;
			
		}
		else
		{
			if (ret != d_NO)
				ret = stOLPinParam.ushKeySet;
		}
		
		vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
		return ret;
	}
    
    memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
    stPinGetPara.Version = 0x02;
	#if 0
    stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	if (strDCT.inPIN3des == 0)//for des pin
	{
		memset(szPINKey,0x00,sizeof(szPINKey));
		memcpy(szPINKey,strDCT.szPINKey,8);
		memcpy(szPINKey+8,strDCT.szPINKey,8);
		stPinGetPara.Protection.pSK = szPINKey;
	}
	else
    	stPinGetPara.Protection.pSK = strDCT.szPINKey;
	#endif

	stPinGetPara.Protection.SK_Length = 0x00;
	stPinGetPara.Protection.pSK = NULL;
    
    stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
    stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
    stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
    vdDebug_LogPrintf("**Min[%d]Max[%d]**",strDCT.inMinPINDigit,strDCT.inMaxPINDigit);

    stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
    stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
    
//    stPinGetPara.Protection.CipherKeyIndex = CUP_TMK_KEY_INDEX;
//    stPinGetPara.Protection.CipherKeySet = CUP_TMK_KEY_SET;
    
    stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
    stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
    stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
    stPinGetPara.Control.Timeout = 0;
    
    if(fGetMPUCard() == TRUE){
        stPinGetPara.Control.NULLPIN = FALSE;
    } else {
        stPinGetPara.Control.NULLPIN = TRUE;
    }
    
    stPinGetPara.PINOutput.EncryptedBlockLength = 8;
    stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
    stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
    stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
    stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

    //CTOS_KBDBufFlush();//cleare key buffer -commented to resolve 04-02-2020 issue below?
	//To fix pin entry display on MPOS
	//04-02-2020
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
    
    vdDebug_LogPrintf("**Online PIN[%d]*PINBlock[%s]*",ret,stPinGetPara.PINOutput.pEncryptedBlock);
    
    DebugAddHEX("szPINBlock=",stPinGetPara.PINOutput.pEncryptedBlock,8);
    
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
            memset(card_holder_pin,0,sizeof(card_holder_pin));
			srTransRec.byPINEntryCapability = 2;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
            return ret;
        }
    }

	srTransRec.byPINEntryCapability = 1;
		
    if(stPinGetPara.PINOutput.EncryptedBlockLength != 8)
    {
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
    }
    else
    {
        wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
        vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
    }
    return d_OK;
}

int inCheckStringMAC(BYTE *szDataIn, int inLengthIn, BYTE *szDataOut, int *inOutLen){
    int i = 0, j = 0;
    BYTE dat;
    BYTE *szParseData;
    
    szParseData = malloc(inLengthIn+1);
    memset(szParseData, 0x00, sizeof(szParseData));
    
    for(i = 0; i < inLengthIn; i++){
        dat = szDataIn[i];
        
        if((dat >= 0x30 && dat <= 0x39) || (dat >= 0x41 && dat <= 0x46) || (dat == 0x20) || (dat == 0x2C) || (dat == 0x2E)){
            szParseData[j] = dat;
            j++;
        }       
    }
    
    memcpy(szDataOut, szParseData, j);
    
    *inOutLen = j;
    
    free(szParseData);
    
    return d_OK;
}

//int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC)
//{
//	CTOS_KMS2MAC_PARA para;
//	USHORT ret;
//	BYTE key,str[40];
//	
//	CTOS_LCDTClearDisplay();
//	DebugAddSTR("inGetIPPMAC","Processing...       ",20);    
//	
//	memset(&para, 0x00, sizeof(CTOS_KMS2MAC_PARA));
//	para.Version = 0x01;
//	para.Protection.CipherKeySet = strDCT.usKeySet;
//	para.Protection.CipherKeyIndex = strDCT.usKeyIndex;
//	para.Protection.CipherMethod = KMS2_MACMETHOD_CBC;
//	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
//	para.Protection.pSK = strDCT.szMACKey;
//	para.ICV.Length = 8;
//	para.ICV.pData = szInitialVector;
//	para.Input.Length = inLengthIn;
//	para.Input.pData = szDataIn;
//	para.Output.pData = szMAC;
//	
//	ret = CTOS_KMS2MAC(&para);
//	if(ret != d_OK)
//		return ret;
//	return d_OK;
//}

int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC)
{
	CTOS_KMS2MAC_PARA para;
	USHORT ret;
	BYTE key,str[40];
	
	CTOS_LCDTClearDisplay();
	DebugAddSTR("inGetIPPMAC","Processing...       ",20);    
	
	memset(&para, 0x00, sizeof(CTOS_KMS2MAC_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = MPU_MAK_KEYSET;
	para.Protection.CipherKeyIndex = MPU_MAK_KEYIDX;
	para.Protection.CipherMethod = KMS2_MACMETHOD_CBC;
//	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
//	para.Protection.pSK = strDCT.szMACKey;

	para.Protection.SK_Length = 0x00;
	para.Protection.pSK = NULL;

	para.ICV.Length = 8;
	para.ICV.pData = szInitialVector;
	para.Input.Length = inLengthIn;
	para.Input.pData = szDataIn;
	para.Output.pData = szMAC;
	
	ret = CTOS_KMS2MAC(&para);
	vdDebug_LogPrintf("inIPPGetMAC ret=%d", ret);	
	if(ret != d_OK)
		return ret;
	return d_OK;
}

//int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC)
//{
//	BYTE szInitialVector[8];
//
//	inIPPGetMAC(szDataIn, inLengthIn,  szInitialVector, szMAC);
//}

int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC)
{
	BYTE szInitialVector[8];
        
        memset(szInitialVector, 0x00, sizeof(szInitialVector));

	inIPPGetMAC(szDataIn, inLengthIn,  szInitialVector, szMAC);
}

//int inMPU_CalcMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC){
//    BYTE szInitialVector[8+1];
//    int inTotalData = 0;
//    int inCheckDataPart = 0, inDataCnt = 0;
//    BYTE *szDataMac;
//    int i = 0, j = 0;
//    BYTE szResult[8+1];
//    BYTE szDataPart[8+1];
//    
//    memset(szInitialVector, 0x00, sizeof(szInitialVector));
//    
//    inTotalData = inLengthIn;
//    
//    if((inTotalData % 8) != 0){
//        while(1){
//            
//            if((inTotalData % 8) != 0){
//                inTotalData++;
//            }
//            else
//                break;
//        }
//    }
//    
//    szDataMac = malloc(inTotalData+1);
//    
//    memset(szDataMac, 0x00, sizeof(szDataMac));
//    
//    memcpy(szDataMac, szDataIn, inLengthIn);
//    
//    inFmtPad(szDataMac, inTotalData, '0');
//    
//    inCheckDataPart = inTotalData / 8;
//    
//    for(i=0; i < inCheckDataPart; i++){
//        memset(szDataPart, 0x00, sizeof(szDataPart));
//        memcpy(szDataPart, szDataIn+(i*8), 8);
//        for(j=0; j < 8; j++){
//            szResult[j] = szInitialVector[j] ^ szDataPart[j]; 
//        }
//        
//        
//        
//        
//    }
//}
	

/*   
int inOldGetIPPPin1(void)
{
	CTOS_KMS2PINGET_PARA para;
	BYTE str[17],key;
	USHORT ret;
	
	DebugAddSTR("inGetIPPPin","Processing...",20);
	inDCTRead(srTransRec.HDTid);
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);

	memset(&para, 0x00, sizeof(CTOS_KMS2PINGET_PARA));
	para.Version = 0x01;
	para.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	para.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	para.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
	para.Protection.CipherKeySet = strDCT.usKeySet;
	para.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	para.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	para.Protection.pSK = strDCT.szPINKey;
	para.AdditionalData.InLength = strlen(srTransRec.szPAN);
	para.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
	para.PINOutput.EncryptedBlockLength = 8;
	para.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	para.Control.Timeout = 0;
	para.Control.AsteriskPositionX = 8;
	para.Control.AsteriskPositionY = 7;
	para.Control.NULLPIN = FALSE;
	para.Control.piTestCancel = NULL;
	
	ret = CTOS_KMS2PINGet(&para);
	if(ret != d_OK)
	{
		sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return ret;
	}
	return d_OK;
}
*/

void vdCTOS_PinEntryPleaseWaitDisplay(void){

/*************************************************************/
CTOS_LCDTClearDisplay();
vdDispTransTitle(srTransRec.byTransType);
setLCDPrint27(8,DISPLAY_POSITION_LEFT, "Please Wait...");

CTOS_KBDBufFlush(); // sidumili: clear buffer of keyboard
/*************************************************************/	

}

int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex)
{
    USHORT rtn;
    
    vdDebug_LogPrintf("SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);
    
    rtn = CTOS_KMS2KeyCheck(ushKeySet, ushKeyIndex);
    if (rtn != d_OK)
        vdDisplayErrorMsg(1, 8, "PLEASE INJECT KEY");
    
    return rtn;
}

int GetPIN_With_3DESDUKPT(void)
{
	CTOS_KMS2PINGET_PARA stPinGetPara;
	USHORT ret;
	BYTE str[17];
	BYTE key;
	BYTE PINBlock[16];
	BYTE *pCipherKey;
	BYTE CipherKeyLength;
	BYTE DecipherPINBlock[16];
	BYTE ExpectPINBlock[16];
	BYTE ksn[20];
	BYTE ksn_Len =  10;
	int inRet;
	
	if  ((srTransRec.byTransType == SALE ) 
		|| (srTransRec.byTransType == PRE_AUTH ))
//		|| (srTransRec.byTransType == REFUND ))
	{
	}
	else
	{
		return d_OK;
	}
		
	DebugAddSTR("GetPIN_With_3DESDUKPT","Processing...",20);
	CTOS_KMS2Init();
	ginPinByPass = 0;

//	inDCTRead(srTransRec.HDTid);
	inDCTRead(6);

	vdDebug_LogPrintf("inDCTRead IN [%ld] [%ld] [%ld]", srTransRec.HDTid, strDCT.usKeyIndex, strDCT.usKeySet);

// patrick test key 20150706 start
	strDCT.usKeySet = 0xC000;
	strDCT.usKeyIndex = 0x0004;
	strDCT.inMinPINDigit = 4;
	strDCT.inMaxPINDigit = 12;

    inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);
    if (inRet != d_OK)
        return(inRet);

	CTOS_LCDTClearDisplay();
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);

	if (strTCT.byPinPadMode != 1)
	{
	    vdDispTransTitle(srTransRec.byTransType);
//	    memset(card_holder_pin,0,sizeof(card_holder_pin));
	    
	    inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
		vdDebug_LogPrintf("szDisplayLine1 [%s] szDisplayLine2[%s]", strDCT.szDisplayLine1, strDCT.szDisplayLine2);
		strcpy(strDCT.szDisplayLine1, "PLEASE ENTER");
		strcpy(strDCT.szDisplayLine2, "ONLINE PIN");
	    setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	    setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	}
	else
	{
		displaybmpEx(0, 0, "Enterpin.bmp");
	}
	CTOS_Delay(1000);

	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA));
	stPinGetPara.Version = 0x01;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
	
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.Protection.SK_Length = 0;
	
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
	stPinGetPara.DUKPT_PARA.IsUseCurrentKey = FALSE;

    ret = CTOS_KMS2DUKPTGetKSN(stPinGetPara.Protection.CipherKeySet, stPinGetPara.Protection.CipherKeyIndex, ksn, &ksn_Len);
    vdDebug_LogPrintf("CTOS_KMS2DUKPTGetKSN[%d]", ret);
    DebugAddHEX("NOT RESET KSN YET", ksn, 10);

	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;//PINBlock; -- fix for issue DE 52 has no value
	stPinGetPara.Control.Timeout = 20;
	stPinGetPara.Control.AsteriskPositionX = 2;
	stPinGetPara.Control.AsteriskPositionY = 7;
	stPinGetPara.Control.NULLPIN = TRUE;
	stPinGetPara.Control.piTestCancel = NULL;

    ksn_Len = 10;
    CTOS_KBDBufFlush();//cleare key buffer	

    memcpy(srTransRec.szKSN, ksn, 10);

	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);

    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
			memset(srTransRec.szKSN, 0x00, sizeof(srTransRec.szKSN));
			ginPinByPass = 1;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
            return ret;
        }
    }

	inCTOSS_ResetDUKPTKSN(ksn);	
    DebugAddHEX("ResetDUKPTKSN Current KSN", ksn, 10);
	
    if(stPinGetPara.PINOutput.EncryptedBlockLength != 8)
    {        
//        vdDebug_LogPrintf("card_holder_pin[%s]","123");
    }
    else
    {
//        wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
//        vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
    }
    return d_OK;
}


USHORT ushCBB_WriteZMK(USHORT ushKeySet, USHORT ushKeyIndex, BYTE *baZMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];


	KeySet = 0xC000;
	KeyIndex = 0x0010;
	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	ret = CTOS_KMS2KeyWrite(&para);

	return ret;
}

USHORT ushCBB_WriteTMK(USHORT ushKeySet, USHORT ushKeyIndex, BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];


	KeySet = 0xC000;
	KeyIndex = 0x0011;
	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	ret = CTOS_KMS2KeyWrite(&para);

	return ret;
}

USHORT ushCBB_WriteTPK(USHORT ushKeySet, USHORT ushKeyIndex, BYTE *baTPK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];


	KeySet = 0xC000;
	KeyIndex = 0x0012;
	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_PIN;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	ret = CTOS_KMS2KeyWrite(&para);

	return ret;
}

/*Write TMK with Plain Text*/
USHORT ushCBB_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];


	//KeySet = 0xC000;
	//KeyIndex = 0x0011;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = CBB_TMK_KEYSET;
	para.Info.KeyIndex = CBB_TMK_KEYIDX;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_INTERMEDIATE | KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushCBB_WriteTMKPlain CTOS_KMS2KeyWrite ret[%d]", ret);
	
	return ret;
}




/*TPK Encrypted by TMK, then TPK will saved as plain in KMS*/
// FROM inUnPackIsoFunc57 - for MPU HOST encyption
USHORT ushCBB_WriteEncTPKUSD(BYTE *baTPK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];	


	vdDebug_LogPrintf("ushCBB_WriteEncTPKUSD");

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
//	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	//memset(szKCV, 0x00, sizeof(szKCV));
	//memcpy(szKCV, "\x09\x24\x22\x90\x0B\xAC\x46\x31", 8);
	
	
     //thandar_add hardcode
	//memcpy(baTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//memcpy(baTPK, "\x4B\xE2\x7C\xE1\x9D\xB4\x93\xD0\x33\x3B\x67\xB8\xF5\xFB\x38\xBE", 16);

    
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = USD_FIN_MAC_KEYSET;	//0xC003
	para.Info.KeyIndex = USD_FIN_MAC_KEYIDX; //0022 - DCT table
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baTPK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	para.Protection.CipherKeySet = USD_FIN_TMK_KEYSET;		//0xC003
	para.Protection.CipherKeyIndex = USD_FIN_TMK_KEYIDX; //0012
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
		
	/*CBB TPK don't have KCV to check*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = 3;
	para.Verification.pKeyCheckValue = szKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);	

	CTOS_Delay(3000);
	vdCTOSS_GetKEYInfo(USD_FIN_MAC_KEYSET,USD_FIN_MAC_KEYIDX);//thandar
	
	vdDebug_LogPrintf("ushCBB_WriteEncTPK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}


/*TPK Encrypted by TMK, then TPK will saved as plain in KMS*/
// FROM inUnPackIsoFunc57 - for MPU HOST encyption
USHORT ushCBB_WriteEncTPKMMK(BYTE *baTPK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];	


	vdDebug_LogPrintf("ushCBB_WriteEncTPKMMK");

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
//	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	//memset(szKCV, 0x00, sizeof(szKCV));
	//memcpy(szKCV, "\x09\x24\x22\x90\x0B\xAC\x46\x31", 8);
	
	
     //thandar_add hardcode
	//memcpy(baTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//memcpy(baTPK, "\x4B\xE2\x7C\xE1\x9D\xB4\x93\xD0\x33\x3B\x67\xB8\xF5\xFB\x38\xBE", 16);

    
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MMK_FIN_MAC_KEYSET;	//0xC003
	para.Info.KeyIndex = MMK_FIN_MAC_KEYIDX; //0021
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baTPK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	para.Protection.CipherKeySet = MMK_FIN_TMK_KEYSET;		//0xC003
	para.Protection.CipherKeyIndex = MMK_FIN_TMK_KEYIDX; //0011
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
		
	/*CBB TPK don't have KCV to check*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = 3;
	para.Verification.pKeyCheckValue = szKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);	

	CTOS_Delay(3000);
	vdCTOSS_GetKEYInfo(MMK_FIN_MAC_KEYSET,MMK_FIN_MAC_KEYIDX);//thandar
	
	vdDebug_LogPrintf("ushCBB_WriteEncTPK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}

/*TPK Encrypted by TMK, then TPK will saved as plain in KMS*/
// FROM inUnPackIsoFunc57 - for MPU HOST encyption
USHORT ushCBB_WriteEncTPK(BYTE *baTPK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];	


	vdDebug_LogPrintf("ushCBB_WriteEncTPK");

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
//	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	//memset(szKCV, 0x00, sizeof(szKCV));
	//memcpy(szKCV, "\x09\x24\x22\x90\x0B\xAC\x46\x31", 8);
	
	
     //thandar_add hardcode
	//memcpy(baTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//memcpy(baTPK, "\x4B\xE2\x7C\xE1\x9D\xB4\x93\xD0\x33\x3B\x67\xB8\xF5\xFB\x38\xBE", 16);

    
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = CBB_TPK_KEYSET;	//0xC000
	para.Info.KeyIndex = CBB_TPK_KEYIDX; //005
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baTPK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	para.Protection.CipherKeySet = CBB_TMK_KEYSET;		//0xC000
	para.Protection.CipherKeyIndex = CBB_TMK_KEYIDX; //004
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
		
	/*CBB TPK don't have KCV to check*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = 3;
	para.Verification.pKeyCheckValue = szKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);	

	CTOS_Delay(3000);
	vdCTOSS_GetKEYInfo(CBB_TPK_KEYSET,CBB_TPK_KEYIDX);//thandar
	
	vdDebug_LogPrintf("ushCBB_WriteEncTPK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}



/*TAK Encrypted by TMK*/
USHORT ushCBB_WriteEncTAK(BYTE *baTAK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = CBB_TAK_KEYSET;
	para.Info.KeyIndex = CBB_TAK_KEYIDX;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Value.pKeyData = baTAK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	para.Protection.CipherKeySet = CBB_TMK_KEYSET;
	para.Protection.CipherKeyIndex = CBB_TMK_KEYIDX;
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	//para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	//para.Verification.KeyCheckValueLength = 3;
	//para.Verification.pKeyCheckValue = szKCV;
	
	ret = CTOS_KMS2KeyWrite(&para);

	
	vdDebug_LogPrintf("ushCBB_WriteEncTAK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}


USHORT ushCBB_WriteTAK(USHORT ushKeySet, USHORT ushKeyIndex, BYTE *baTAK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];


	KeySet = CBB_TAK_KEYSET;
	KeyIndex = CBB_TAK_KEYIDX;
	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	ret = CTOS_KMS2KeyWrite(&para);

	return ret;
}




USHORT ushMPU_WriteTMK(USHORT ushKeySet, USHORT ushKeyIndex, BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];


	KeySet = ushKeySet;
	KeyIndex = ushKeyIndex;
	memcpy(KeyData, baTMK, byKeyLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = ushKeySet;
	para.Info.KeyIndex = ushKeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushMPU_WriteTMK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}


/*PIK Encrypted by TMK, then PIK will saved as plain in KMS*/
USHORT ushMPU_WriteEncPIK(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushMPU_WriteEncPIK=====");
	DebugAddHEX("baPIK", baPIK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MPU_PIK_KEYSET;	//0xC000
	para.Info.KeyIndex = MPU_PIK_KEYIDX; // 0015 - DCT TABLE EQUIVALENT TO - int 21 value
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baPIK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	//para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_CBC;
    //para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Protection.CipherKeySet = MPU_TMK_KEYSET;	//0xC000
	para.Protection.CipherKeyIndex = MPU_TMK_KEYIDX; //0014 -  MPU HOST
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU PIK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

	
	vdDebug_LogPrintf("ushMPU_WriteEncPIK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}



/*MAK Encrypted by TMK, then MAK will saved as plain in KMS*/
USHORT ushMPU_WriteEncMAK(BYTE *baMAK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushMPU_WriteEncMAK=====");
	DebugAddHEX("baMAK", baMAK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MPU_MAK_KEYSET;		//0xC000
	para.Info.KeyIndex = MPU_MAK_KEYIDX;	//0016
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
//  para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC;
	para.Value.pKeyData = baMAK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	//para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_CBC;
    //para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Protection.CipherKeySet = MPU_TMK_KEYSET;  // 0xC000 - reference for key injection (vdCTOS_MPUInjectKey function)
	para.Protection.CipherKeyIndex = MPU_TMK_KEYIDX; //0014
    memset(szIV, 0x00, sizeof(szIV));
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU MAK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ MAK ERROR!");
	}
	
	vdDebug_LogPrintf("ushMPU_WriteEncMAK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}


void vdHardCodeMPU_UATKey(void)
{
	BYTE baTMK[16];
	BYTE byKeyLen = 16;

	USHORT ushKeySet = MPU_TMK_KEYSET;
	USHORT ushKeyIndex = MPU_TMK_KEYIDX;
	USHORT ushRet = 0;

	vdDebug_LogPrintf("vdHardCodeMPU_UATKey....");
	
	memset(baTMK, 0x00, sizeof(baTMK));
	memcpy(baTMK, "\x12\x34\x56\x78\x90\xAB\xCD\xEF\x12\x34\x56\x78\x90\xAB\xCD\xEF", byKeyLen);
        memset(szTMK, 0x00,sizeof(szTMK));
        memcpy(szTMK, baTMK, 16);

	ushRet = ushMPU_WriteTMK(ushKeySet, ushKeyIndex, baTMK, byKeyLen);

	
	return;
}

#ifdef PIN_CHANGE_ENABLE


void vdHardCodeVISAMC_FinexusUATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	//BYTE szTPK[16];

	CTOS_KMS2Init();

	KeySet = CBB_COMM_KEYSET; //c000
	KeyIndex = CBB_TAK_KEYIDX; //0006 - refer to CAash Advance menu/ transaction.  lookup at inUnPackIsoFunc57

	vdDebug_LogPrintf("vdHardCodeCUP_FinexusUATKey2");
	

	//0E E6 D5 2F 25 B0 DA 29 67 E9 19 DC A7 80 7A 0D - VISA/ MC UAT KEYS
	memcpy(KeyData, "\x0E\xE6\xD5\x2F\x25\xB0\xDA\x29\x67\xE9\x19\xDC\xA7\x80\x7A\x0D", 16);
	
	#if 0
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);
	#endif

	ushCAVPCVISAMCCBB_WriteTMKPlain(KeyData, IPP_TDES_KEY_SIZE);

	vdCTOSS_GetKEYInfo(CBB_COMM_KEYSET,CBB_TAK_KEYIDX);//thandar

	//memset(szTPK, 0x00, sizeof (szTPK));
	//memcpy(szTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//ushCBB_WriteEncTPK(szTPK, 16);		
	  
}




/*Write TMK with Plain Text*/
USHORT ushUPICBB_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	vdDebug_LogPrintf("ushUPICBB_WriteTMKPlain");


	//KeySet = 0xC000;
	//KeyIndex = 0x0011;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MPU_TMK_KEYSET;	//c000
	para.Info.KeyIndex = CBB_TMK_KEYIDX; //0004
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_INTERMEDIATE | KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushUPICBB_WriteTMKPlain end ret[%d]", ret);
	if(ret == d_OK)
		vdDisplayErrorMsgResp2("SUCCESSFUL UPI","","KEY INJECTION");
	
	return ret;
}


void vdHardCodeCUP_FinexusUATKey2(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	//BYTE szTPK[16];

	CTOS_KMS2Init();

	KeySet = CBB_COMM_KEYSET; //c000
	KeyIndex = CBB_TMK_KEYIDX;//0004;

	vdDebug_LogPrintf("vdHardCodeCUP_FinexusUATKey2");
	

	#if 0
	if(get_env_int("#UKEY1") == 1)	{
		
		vdDebug_LogPrintf("vdHardCodeCUP_FinexusUATKey-1");
		memcpy(KeyData, "\xFE\x08\xE6\x3E\xE6\x45\x3B\xEA\x6E\x61\xB5\x20\x75\x6E\xBC\xE6", 16);
		}

    if(get_env_int("#UKEY2") == 1){
		
		vdDebug_LogPrintf("vdHardCodeCUP_FinexusUATKey-2");
		memcpy(KeyData, "\x0E\xE6\xD5\x2F\x25\xB0\xDA\x29\x67\xE9\x19\xDC\xA7\x80\x7A\x0D", 16);
    	}
	#else	
		//FE 08 E6 3E E6 45 3B EA 6E 61 B5 20 75 6E BC E6 UPI 
		memcpy(KeyData, "\xFE\x08\xE6\x3E\xE6\x45\x3B\xEA\x6E\x61\xB5\x20\x75\x6E\xBC\xE6", 16);
	#endif
	
	#if 0
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);
	#endif

	ushUPICBB_WriteTMKPlain(KeyData, IPP_TDES_KEY_SIZE);

	vdCTOSS_GetKEYInfo(CBB_COMM_KEYSET,CBB_TMK_KEYIDX);//0004

	//memset(szTPK, 0x00, sizeof (szTPK));
	//memcpy(szTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//ushCBB_WriteEncTPK(szTPK, 16);		
	  
}


/*TPK Encrypted by TMK, then TPK will saved as plain in KMS*/
USHORT ushCAVPCCBB_WriteEncTPK(BYTE *baTPK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];	

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
//	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	//memset(szKCV, 0x00, sizeof(szKCV));
	//memcpy(szKCV, "\x09\x24\x22\x90\x0B\xAC\x46\x31", 8);
	
	
     //thandar_add hardcode
	//memcpy(baTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//memcpy(baTPK, "\x4B\xE2\x7C\xE1\x9D\xB4\x93\xD0\x33\x3B\x67\xB8\xF5\xFB\x38\xBE", 16);

	//12 C1 75 82 D4 C2 C6 23 62 CD 73 0F 69 40 73 56 - de 57
	//memcpy(baTPK, "\x12\xC1\x75\x82\xD4\xC2\xC6\x23\x62\xCD\x73\x0F\x69\x40\x73\x56", 16);

	vdDebug_LogPrintf("ushCAVPCCBB_WriteEncTPK START");
    
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MPU_PIK_KEYSET;
	para.Info.KeyIndex = MPU_PIK_KEYIDX;		//0015 index = uskeyindex(dct) decimal 21 to match DCT table based on ex: HDTID[17]
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baTPK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	para.Protection.CipherKeySet = MPU_TMK_KEYSET;		
	para.Protection.CipherKeyIndex = MPU_TMK_KEYIDX;	//0014 index to save keys
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
		
	/*CBB TPK don't have KCV to check*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = 3;
	para.Verification.pKeyCheckValue = szKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);	

	CTOS_Delay(3000);
	vdCTOSS_GetKEYInfo(MPU_PIK_KEYSET,MPU_PIK_KEYIDX);//thandar
	
	vdDebug_LogPrintf("ushCAVPCCBB_WriteEncTPK END ret[%d]", ret);

	return ret;
}






/*Write TMK with Plain Text*/
USHORT ushMMK_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	vdDebug_LogPrintf("ushMMK_WriteTMKPlain");


	//KeySet = 0xC000;
	//KeyIndex = 0x0011;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MMK_FIN_TMK_KEYSET;	//c003
	para.Info.KeyIndex = MMK_FIN_TMK_KEYIDX; //0011
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_INTERMEDIATE | KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushMMK_WriteTMKPlain end ret[%d]", ret);

	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}
	
	return ret;
}


/*Write TMK with Plain Text*/
USHORT ushUSD_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	vdDebug_LogPrintf("ushUSD_WriteTMKPlain");


	//KeySet = 0xC000;
	//KeyIndex = 0x0011;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = USD_FIN_TMK_KEYSET;	//c003
	para.Info.KeyIndex = USD_FIN_TMK_KEYIDX; //0012
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_INTERMEDIATE | KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushUSD_WriteTMKPlain end ret[%d]", ret);
	
	return ret;
}


/*Write TMK with Plain Text*/
USHORT ushCAVPCVISAMCCBB_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	vdDebug_LogPrintf("ushCAVPCVISAMCCBB_WriteTMKPlain");


	//KeySet = 0xC000;
	//KeyIndex = 0x0011;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = CBB_COMM_KEYSET;	//c000
	para.Info.KeyIndex = CBB_TAK_KEYIDX; //0006
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_INTERMEDIATE | KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushCAVPCCBB_WriteTMKPlain end ret[%d]", ret);
	
	return ret;
}

/*Write TMK with Plain Text*/
USHORT ushCAVPCCBB_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	vdDebug_LogPrintf("ushCAVPCCBB_WriteTMKPlain");


	//KeySet = 0xC000;
	//KeyIndex = 0x0011;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MPU_TMK_KEYSET;	//c000
	para.Info.KeyIndex = MPU_TMK_KEYIDX; //0014
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_INTERMEDIATE | KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushCAVPCCBB_WriteTMKPlain end ret[%d]", ret);
	
	return ret;
}

void vdCAVPCHardCodeCUP_UATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	CTOS_KMS2Init();

	vdDebug_LogPrintf("vdCAVPCHardCodeCUP_UATKey");
	// patrick test key 20150706	
	KeySet = MPU_TMK_KEYSET;	//c000
	KeyIndex = MPU_TMK_KEYIDX;  // 0014 - index to save keys (TMK and TPK)


	//memcpy(KeyData, "\xAE\x64\xF7\x51\xBC\xCB\xA4\x26\xCD\x75\x8A\xD9\xC7\xCE\x98\x1F", 16); - orig


	//22222222222222226666666666666666 - TMK UAT keys (comp1 + comp2) result
	//FE 08 E6 3E E6 45 3B EA 6E 61 B5 20 75 6E BC E6  - UAT keys
	
	//memcpy(KeyData, "\x22\x22\x22\x22\x22\x22\x22\x22\x66\x66\x66\x66\x66\x66\x66\x66", 16);
	memcpy(KeyData, "\xFE\x08\xE6\x3E\xE6\x45\x3B\xEA\x6E\x61\xB5\x20\x75\x6E\xBC\xE6", 16);
	
	#if 0
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);
	#endif

	ushCAVPCCBB_WriteTMKPlain(KeyData, IPP_TDES_KEY_SIZE);
	
}
#endif

#ifdef OK_DOLLAR_FEATURE
void vdHardCodedOKD_UATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	unsigned char KeyData[16];
	unsigned char uszIULPMACKey[32+1] = {0};	
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

#ifdef OK_DOLLAR_FALLBACK_URL	
    BYTE szKeyComp1Str[32 + 1];
    USHORT usKeyStrLen = 16;
    BYTE bRet;
#endif


	//BYTE szTPK[16];
    CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER MASTER KEY");	

	CTOS_KMS2Init();

	KeySet = CBB_AES_KEYSET;//CBB_TAK_KEYSET; //	//C001
	KeyIndex = CBB_AES_KEYIDX;//Orig is 0004 conflict with IPP keys index

	vdDebug_LogPrintf("vdHardCodedOK$_UATKey [KeyIndex[%04X] KeyIndex[%04X]", KeySet, KeyIndex);

	#ifdef OK_DOLLAR_FALLBACK_URL
    memset(szKeyComp1Str, 0x00, sizeof (szKeyComp1Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x06, 0x02, szKeyComp1Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;

	memset(uszIULPMACKey,0x00,sizeof(uszIULPMACKey));
	strcpy((char *)uszIULPMACKey, (char *)szKeyComp1Str); 
	vdDebug_LogPrintf("AES256 uszKeyData [%s]", uszIULPMACKey);	
		
	#else // hard coded script	
	memset(uszIULPMACKey,0x00,sizeof(uszIULPMACKey));
										  //UAT sample MKEY //PROD KEYS 620d491e8a51a397
	strcpy((char *)uszIULPMACKey, (char *)"4ab9129abd5e2127"); //- original 
	vdDebug_LogPrintf("AES256 uszKeyData [%s]", uszIULPMACKey);	
	#endif
	
	vdWriteAES128KEY(KeySet, KeyIndex, uszIULPMACKey);
	
	//vdWriteAES256_PlainText(KeySet, KeyIndex, KeyData);	
	//memcpy(KeyData, "\x4a\xb9\x12\x9a\xbd\x5e\x21\x27", 8);
    //DebugAddHEX("AES256 Key result", KeyData, 8);

	vdCTOSS_GetKEYInfo(KeySet,KeyIndex);

	//memset(szTPK, 0x00, sizeof (szTPK));
	//memcpy(szTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//ushCBB_WriteEncTPK(szTPK, 16);		
	  
}

#endif


#if 0
void vdHardCodedOKD_UATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	unsigned char KeyData[16];
	unsigned char uszIULPMACKey[32+1] = {0};	
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	//BYTE szTPK[16];

	CTOS_KMS2Init();

	KeySet = CBB_AES_KEYSET;//CBB_TAK_KEYSET; //	//C000
	KeyIndex = CBB_AES_KEYIDX;//CBB_TAK_KEYIDX; // //0008

	vdDebug_LogPrintf("vdHardCodedOK$_UATKey [KeyIndex[%04X] KeyIndex[%04X]", KeySet, KeyIndex);
	


	memset(uszIULPMACKey,0x00,sizeof(uszIULPMACKey));
	strcpy((char *)uszIULPMACKey, (char *)"4ab9129abd5e2127"); 
	vdDebug_LogPrintf("AES256 uszKeyData [%s]", uszIULPMACKey);	
	
	vdWriteAES128KEY(KeySet, KeyIndex, uszIULPMACKey);
	
	//vdWriteAES256_PlainText(KeySet, KeyIndex, KeyData);	
	//memcpy(KeyData, "\x4a\xb9\x12\x9a\xbd\x5e\x21\x27", 8);
    //DebugAddHEX("AES256 Key result", KeyData, 8);

	vdCTOSS_GetKEYInfo(KeySet,KeyIndex);

	//memset(szTPK, 0x00, sizeof (szTPK));
	//memcpy(szTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//ushCBB_WriteEncTPK(szTPK, 16);		
	  
}

#endif

void vdHardCodeCUP_UATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	CTOS_KMS2Init();

	// patrick test key 20150706	
	KeySet = CBB_TMK_KEYSET;	//c000
	KeyIndex = CBB_TMK_KEYIDX;  // 0004 - index to save keys (TMK and TPK)


	//memcpy(KeyData, "\xAE\x64\xF7\x51\xBC\xCB\xA4\x26\xCD\x75\x8A\xD9\xC7\xCE\x98\x1F", 16); - orig


	//22222222222222226666666666666666 - TMK UAT keys (comp1 + comp2) result
	memcpy(KeyData, "\x22\x22\x22\x22\x22\x22\x22\x22\x66\x66\x66\x66\x66\x66\x66\x66", 16);
	
	#if 0
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);
	#endif

	ushCBB_WriteTMKPlain(KeyData, IPP_TDES_KEY_SIZE);
	
}

void vdHardCodeCUP_FinexusUATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	//BYTE szTPK[16];

	CTOS_KMS2Init();

	KeySet = CBB_TMK_KEYSET;
	KeyIndex = CBB_TMK_KEYIDX;

	vdDebug_LogPrintf("vdHardCodeCUP_FinexusUATKey");

	if(get_env_int("#UKEY1") == 1)	{
		
		vdDebug_LogPrintf("vdHardCodeCUP_FinexusUATKey-1");
		memcpy(KeyData, "\xFE\x08\xE6\x3E\xE6\x45\x3B\xEA\x6E\x61\xB5\x20\x75\x6E\xBC\xE6", 16);
		}

    if(get_env_int("#UKEY2") == 1){
		
		vdDebug_LogPrintf("vdHardCodeCUP_FinexusUATKey-2");
		memcpy(KeyData, "\x0E\xE6\xD5\x2F\x25\xB0\xDA\x29\x67\xE9\x19\xDC\xA7\x80\x7A\x0D", 16);
    	}
		
	#if 0
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);
	#endif

	ushCBB_WriteTMKPlain(KeyData, IPP_TDES_KEY_SIZE);

	vdCTOSS_GetKEYInfo(CBB_TMK_KEYSET,CBB_TMK_KEYIDX);//thandar

	//memset(szTPK, 0x00, sizeof (szTPK));
	//memcpy(szTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//ushCBB_WriteEncTPK(szTPK, 16);		
	  
}

void vdTestCUPPINBlock(void)
{
	strcpy(srTransRec.szPAN, "6222620610004116588");
	inGetIPPPin();
}


void CC_XorOperation(BYTE *pb1, BYTE *pb2, USHORT usLen, BYTE *baOutBuf)
{
    USHORT i;
    
    for(i = 0 ; i < usLen ; i++)
    {
        baOutBuf[i] = pb1[i] ^ pb2[i];
    }
}


USHORT CC_DES_ECB_Cipher(BYTE *pbKey, BYTE bKLen, BYTE *pbData, BYTE bDLen, BYTE *baOutBuf)
{
    USHORT usRtn;

    usRtn = CTOS_DES(d_ENCRYPTION, pbKey, bKLen, pbData, bDLen, baOutBuf);
    if(usRtn != d_OK)
    {
        return usRtn;
    }
    
    return d_OK;
}


//@@IBR ADD 06102016
static int CC_KMS_DES_ECB(int inKeySet, int inKeyIndex, BYTE *inPlainData, int inPlainLen, BYTE *outEncData)
{
	USHORT ret;
	CTOS_KMS2DATAENCRYPT_PARA para;
	BYTE str[17];

	BYTE	szOutBuf[1024];
	
	memset(&para, 0x00, sizeof(CTOS_KMS2DATAENCRYPT_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = inKeySet;
	para.Protection.CipherKeyIndex = inKeyIndex;
	para.Protection.CipherMethod = KMS2_DATAENCRYPTCIPHERMETHOD_ECB;
	para.Protection.SK_Length = 0;
	para.Input.Length = inPlainLen;
	para.Input.pData = inPlainData;
	para.Output.pData = szOutBuf;
	
	ret = CTOS_KMS2DataEncrypt(&para);
	if(ret != d_OK)
	{
 		sprintf(str, "CC_KMS_DES_ECB ret = 0x%04X", ret);
// 		CTOS_LCDTPrintXY(1, 8, str);
 		return ret;
	}

	memcpy(outEncData, szOutBuf, inPlainLen);

	return d_OK;
}


//USHORT ushMPU_WriteEncMAC(BYTE *baMAC, BYTE byKeyLen, BYTE *outEncData)
//{
//	USHORT ret;
//	CTOS_KMS2DATAENCRYPT_PARA para;
//	BYTE str[17];
//
//	BYTE	szOutBuf[1024];
//	
//	memset(&para, 0x00, sizeof(CTOS_KMS2DATAENCRYPT_PARA));
//	para.Version = 0x01;
//	para.Protection.CipherKeySet = MPU_MAK_KEYSET;
//
//	para.Protection.CipherKeyIndex = MPU_MAK_KEYIDX;
//	para.Protection.CipherMethod = KMS2_DATAENCRYPTCIPHERMETHOD_CBC;
//	para.Protection.SK_Length = 0;
//	para.Input.Length = byKeyLen;
//	para.Input.pData = baMAC;
//	para.Output.pData = szOutBuf;
//	
//	ret = CTOS_KMS2DataEncrypt(&para);
//	if(ret != d_OK)
//	{
// 		sprintf(str, "ushMPU_WriteEncMAC ret = 0x%04X", ret);
//// 		CTOS_LCDTPrintXY(1, 8, str);
// 		return ret;
//	}
//
//	memcpy(outEncData, szOutBuf, byKeyLen);
//
//	return d_OK;
//}

#if 0
void vdCTOS_MPUInjectKey(void) {
    char szKey1[16 + 1], szKey2[16 + 1];
    USHORT usKeyLen = 16;
    BYTE bRet;
    BYTE szKeyHex[16 + 1], szKeyHex1[8 + 1], szKeyHex2[8 + 1];
    BYTE szKeyKCV[8 + 1];
    BYTE baTMK[16+1];
    char szTemp[16+1], szDisp[6+1];
    USHORT ushKeySet = MPU_TMK_KEYSET;
    USHORT ushKeyIndex = MPU_TMK_KEYIDX;
    USHORT ushRet = 0;
    int i = 0;
    BYTE key;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #1");
    
    memset(szKey1, 0x00, sizeof (szKey1));
    memset(szKey2, 0x00, sizeof (szKey2));

	usKeyLen = 16;
    while (1) {
        vduiClearBelow(2);

        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
        bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKey1, &usKeyLen, 0, d_INPUT_TIMEOUT);
        if (bRet == d_KBD_CANCEL)
            return;

		vduiClearBelow(2);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
        bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKey1, &usKeyLen, 0, d_INPUT_TIMEOUT);
        if (bRet == d_KBD_CANCEL)
            return;

        if (strlen(szKey1) != 16) {
            setLCDPrint(4, DISPLAY_POSITION_CENTER, "1st KEY LESS THAN 16");
            CTOS_Delay(1500);
            continue;
        } else {
            CTOS_LCDTClearDisplay();
            vdDispTitleString("ENTER COMPONENT #2");
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter 8 Bytes KEY");
            bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKey2, &usKeyLen, 0, d_INPUT_TIMEOUT);
            if (bRet == d_KBD_CANCEL)
                return;

            if (strlen(szKey2) != 16) {
                setLCDPrint(4, DISPLAY_POSITION_CENTER, "2nd KEY LESS THAN 16");
                CTOS_Delay(1500);
                continue;
            } else
                break;
        }
    }
    
    memset(szKeyHex, 0x00, sizeof(szKeyHex));
    memset(szKeyHex1, 0x00, sizeof(szKeyHex1));
    memset(szKeyHex2, 0x00, sizeof(szKeyHex2));

	vdDebug_LogPrintf("szKey1[%s]", szKey1);
	vdDebug_LogPrintf("szKey2[%s]", szKey2);
    
    wub_str_2_hex(szKey1, szKeyHex1, 16);
    wub_str_2_hex(szKey2, szKeyHex2, 16);

	#if 0
    for (i = 0; i < 8; i++) {
        szKeyHex[i] = szKeyHex1[i] ^ szKeyHex2[i];
    }
	#endif
	memcpy(szKeyHex, szKeyHex1, 8);
	memcpy(&szKeyHex[8], szKeyHex2, 8);
	usKeyLen = 16;

	DebugAddHEX("szKeyHex", szKeyHex, 16);

	ushRet = ushMPU_WriteTMK(ushKeySet, ushKeyIndex, szKeyHex, usKeyLen);
    if (ushRet != d_OK)
        return;

    memset(szKeyKCV, 0x00, sizeof (szKeyKCV));
    ushRet = CC_KMS_DES_ECB(ushKeySet, ushKeyIndex, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, szKeyKCV);

    memset(szTemp, 0x00, sizeof (szTemp));
    wub_hex_2_str(szKeyKCV, szTemp, 8);
    memset(szDisp, 0x00, sizeof (szDisp));
    memcpy(szDisp, szTemp, 6);
    CTOS_LCDTClearDisplay();
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Final KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS OK TO CONFIRM");

    CTOS_KBDGet(&key);
    
    memset(baTMK, 0x00, sizeof (baTMK));
    memcpy(baTMK, szKeyHex, 8);
    memcpy(baTMK + 8, szKeyHex, 8);
    
    memset(szTMK, 0x00, sizeof(szTMK));
    memcpy(szTMK, baTMK, 16);
    
    ushRet = ushMPU_WriteTMK(ushKeySet, ushKeyIndex, szKeyHex, usKeyLen);
    if (ushRet != d_OK)
        return;

    vdDebug_LogPrintf("vdCTOS_MPUInjectKey = %d", ushRet);
    return;
}
#endif


void vdCTOS_MMKInjectKey(void)
{
    BYTE szKeyComp1Str[32 + 1];
	BYTE szKeyComp2Str[32 + 1];

	BYTE szKeyComp1Hex[16 + 1];
	BYTE szKeyComp2Hex[16 + 1];
	
	BYTE baKeyComp1KCV[8];
	BYTE baKeyComp2KCV[8];

	BYTE szMPU_TMK[16];
	BYTE szMPU_TMK_KCV[8];
	
    USHORT usKeyStrLen = 16;
    BYTE bRet;
	BYTE key;

    USHORT ushKeySet = MMK_FIN_TMK_KEYSET;	//0xC003
    USHORT ushKeyIndex = MMK_FIN_TMK_KEYIDX; // 0011

    USHORT ushRet = 0;

	BYTE szTempDisp[64];
    
    CTOS_LCDTClearDisplay();

	//EA57C11CAD5B51FE510D76BC38E66497	- latest TMK from FIN-MMK host
	//EA 57 C1 1C AD 5B 51 FE 
	//51 0D 76 BC 38 E6 64 97
	
    vdDispTitleString("ENTER COMPONENT #1");
    
    memset(szKeyComp1Str, 0x00, sizeof (szKeyComp1Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp1Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp1Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("vdCTOS_MMKInjectKey TMK1 szKeyComp1Str[%s]", szKeyComp1Str);

	memset(szKeyComp1Hex, 0x00, sizeof (szKeyComp1Hex));
    wub_str_2_hex(szKeyComp1Str, szKeyComp1Hex, 32);

	memset(baKeyComp1KCV, 0x00, sizeof (baKeyComp1KCV));
	CC_DES_ECB_Cipher(szKeyComp1Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp1KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp1KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp1 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

//#if 1
	CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #2");
    
    memset(szKeyComp2Str, 0x00, sizeof (szKeyComp2Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp2Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp2Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	//vdDebug_LogPrintf("szKeyComp2Str[%s]", szKeyComp2Str);
	vdDebug_LogPrintf("vdCTOS_MMKInjectKey TMK2 szKeyComp2Str[%s]", szKeyComp2Str);

	memset(szKeyComp2Hex, 0x00, sizeof (szKeyComp2Hex));
    wub_str_2_hex(szKeyComp2Str, szKeyComp2Hex, 32);

	memset(baKeyComp2KCV, 0x00, sizeof (baKeyComp2KCV));
	CC_DES_ECB_Cipher(szKeyComp2Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp2KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp2KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp2 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	#if 0
	CC_XorOperation(szKeyComp1Hex, szKeyComp2Hex, 16, szMPU_TMK);

	vdDebug_LogPrintf("vdCTOS_MMKInjectKey  szMPU_TMK[%d][%d][%s]", ushKeySet, ushKeyIndex, szMPU_TMK);

	ushRet = ushMPU_WriteTMK(ushKeySet, ushKeyIndex, szMPU_TMK, 16);
    if (ushRet != d_OK)
        return;

    memset(szMPU_TMK_KCV, 0x00, sizeof (szMPU_TMK_KCV));
    ushRet = CC_KMS_DES_ECB(ushKeySet, ushKeyIndex, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, szMPU_TMK_KCV);

    CTOS_LCDTClearDisplay();

	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(szMPU_TMK_KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;
	
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Final KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);
	#else

	/* working
	DebugAddHEX("szMPU_TMK", szKeyComp1Hex, 16);

	vdDebug_LogPrintf("vdCTOS_MMKInjectKey SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);

	ushRet = ushMMK_WriteTMKPlain(szKeyComp1Hex, IPP_TDES_KEY_SIZE);
	*/

	
	DebugAddHEX("szMPU_TMK", szKeyComp2Hex, 16);

	vdDebug_LogPrintf("vdCTOS_MMKInjectKey SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);

	ushRet = ushMMK_WriteTMKPlain(szKeyComp2Hex, IPP_TDES_KEY_SIZE);

	vdCTOSS_GetKEYInfo(MMK_FIN_TMK_KEYSET,MMK_FIN_TMK_KEYIDX);	
	#endif

    vdDebug_LogPrintf("vdCTOS_MPUInjectKey = %d", ushRet);
    return;
}


void vdCTOS_USDInjectKey(void)
{
    BYTE szKeyComp1Str[32 + 1];
	BYTE szKeyComp2Str[32 + 1];

	BYTE szKeyComp1Hex[16 + 1];
	BYTE szKeyComp2Hex[16 + 1];
	
	BYTE baKeyComp1KCV[8];
	BYTE baKeyComp2KCV[8];

	BYTE szMPU_TMK[16];
	BYTE szMPU_TMK_KCV[8];
	
    USHORT usKeyStrLen = 16;
    BYTE bRet;
	BYTE key;
	
    USHORT ushKeySet = USD_FIN_TMK_KEYSET;	//0xC003
    USHORT ushKeyIndex = USD_FIN_TMK_KEYIDX; // 0012

	USHORT ushRet = 0;

	BYTE szTempDisp[64];




	//854968167F4A101F37086EB3FBDF1670
	//85 49 68 16 7F 4A 10 1F 
	//37 08 6E B3 FB DF 16 70
	
    CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #1");
    
    memset(szKeyComp1Str, 0x00, sizeof (szKeyComp1Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp1Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp1Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp1Str[%s]", szKeyComp1Str);

	memset(szKeyComp1Hex, 0x00, sizeof (szKeyComp1Hex));
    wub_str_2_hex(szKeyComp1Str, szKeyComp1Hex, 32);

	memset(baKeyComp1KCV, 0x00, sizeof (baKeyComp1KCV));
	CC_DES_ECB_Cipher(szKeyComp1Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp1KCV);
	
	vduiClearBelow(2);

	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp1KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp1 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

//#if 0
	CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #2");
    
    memset(szKeyComp2Str, 0x00, sizeof (szKeyComp2Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp2Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp2Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp2Str[%s]", szKeyComp2Str);

	memset(szKeyComp2Hex, 0x00, sizeof (szKeyComp2Hex));
    wub_str_2_hex(szKeyComp2Str, szKeyComp2Hex, 32);

	memset(baKeyComp2KCV, 0x00, sizeof (baKeyComp2KCV));
	CC_DES_ECB_Cipher(szKeyComp2Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp2KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp2KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp2 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	#if 0
	CC_XorOperation(szKeyComp1Hex, szKeyComp2Hex, 16, szMPU_TMK);

	DebugAddHEX("szMPU_TMK", szMPU_TMK, 16);

	ushRet = ushMPU_WriteTMK(ushKeySet, ushKeyIndex, szMPU_TMK, 16);
    if (ushRet != d_OK)
        return;

    memset(szMPU_TMK_KCV, 0x00, sizeof (szMPU_TMK_KCV));
    ushRet = CC_KMS_DES_ECB(ushKeySet, ushKeyIndex, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, szMPU_TMK_KCV);

    CTOS_LCDTClearDisplay();

	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(szMPU_TMK_KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;
	
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Final KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);
	#else
	/* working
	DebugAddHEX("szMPU_TMK", szKeyComp1Hex, 16);
	vdDebug_LogPrintf("vdCTOS_USDInjectKey SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);
	
	ushRet = ushUSD_WriteTMKPlain(szKeyComp1Hex, IPP_TDES_KEY_SIZE);
	*/

	DebugAddHEX("szMPU_TMK", szKeyComp2Hex, 16);
	vdDebug_LogPrintf("vdCTOS_USDInjectKey SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);
	
	ushRet = ushUSD_WriteTMKPlain(szKeyComp2Hex, IPP_TDES_KEY_SIZE);
	
	vdCTOSS_GetKEYInfo(USD_FIN_TMK_KEYSET,USD_FIN_TMK_KEYIDX);	
	#endif

    vdDebug_LogPrintf("vdCTOS_MPUInjectKey = %d", ushRet);
    return;
}


void vdCTOS_MPUInjectKey(void)
{
    BYTE szKeyComp1Str[32 + 1];
	BYTE szKeyComp2Str[32 + 1];

	BYTE szKeyComp1Hex[16 + 1];
	BYTE szKeyComp2Hex[16 + 1];
	
	BYTE baKeyComp1KCV[8];
	BYTE baKeyComp2KCV[8];

	BYTE szMPU_TMK[16];
	BYTE szMPU_TMK_KCV[8];
	
    USHORT usKeyStrLen = 16;
    BYTE bRet;
	BYTE key;
	
    USHORT ushKeySet = MPU_TMK_KEYSET;	//0xC000
    USHORT ushKeyIndex = MPU_TMK_KEYIDX; // 0014
    USHORT ushRet = 0;

	BYTE szTempDisp[64];
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #1");
    
    memset(szKeyComp1Str, 0x00, sizeof (szKeyComp1Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp1Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp1Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp1Str[%s]", szKeyComp1Str);

	memset(szKeyComp1Hex, 0x00, sizeof (szKeyComp1Hex));
    wub_str_2_hex(szKeyComp1Str, szKeyComp1Hex, 32);

	memset(baKeyComp1KCV, 0x00, sizeof (baKeyComp1KCV));
	CC_DES_ECB_Cipher(szKeyComp1Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp1KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp1KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp1 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #2");
    
    memset(szKeyComp2Str, 0x00, sizeof (szKeyComp2Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp2Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp2Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp2Str[%s]", szKeyComp2Str);

	memset(szKeyComp2Hex, 0x00, sizeof (szKeyComp2Hex));
    wub_str_2_hex(szKeyComp2Str, szKeyComp2Hex, 32);

	memset(baKeyComp2KCV, 0x00, sizeof (baKeyComp2KCV));
	CC_DES_ECB_Cipher(szKeyComp2Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp2KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp2KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp2 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	CC_XorOperation(szKeyComp1Hex, szKeyComp2Hex, 16, szMPU_TMK);

	DebugAddHEX("szMPU_TMK", szMPU_TMK, 16);

	ushRet = ushMPU_WriteTMK(ushKeySet, ushKeyIndex, szMPU_TMK, 16);
    if (ushRet != d_OK)
        return;

    memset(szMPU_TMK_KCV, 0x00, sizeof (szMPU_TMK_KCV));
    ushRet = CC_KMS_DES_ECB(ushKeySet, ushKeyIndex, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, szMPU_TMK_KCV);

    CTOS_LCDTClearDisplay();

	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(szMPU_TMK_KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;
	
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Final KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

    vdDebug_LogPrintf("vdCTOS_MPUInjectKey = %d", ushRet);
    return;
}


/**
 * Encrypt data using 3des algorithm
 * @param [in] data
 * @param [in] key
 * @param [out] result
 */
static void Encrypt3Des(unsigned char *data, unsigned char *key, unsigned char *result)
{
    unsigned char tmpBuf1[9], tmpBuf2[9];

    CTOS_DES(d_ENCRYPTION, key, 8, data, 8, tmpBuf1);
    CTOS_DES(d_DECRYPTION, &key[8], 8, tmpBuf1, 8, tmpBuf2);
    CTOS_DES(d_ENCRYPTION, key, 8, tmpBuf2, 8, result);
}

/**
 * Encrypt data using 3des algorithm
 * @param [in] Mode CBC_MODE or ECB_MODE
 * @param [in] szMsg
 * @param [in] inMsgLen
 * @param [in] szKey
 * @param [out] DESResult
 * @param [in] szVektor
 */

static void Decrypt3Des(unsigned char *data, unsigned char *key, unsigned char *result)
{
    unsigned char tmpBuf1[9], tmpBuf2[9];
    CTOS_DES(d_DECRYPTION, key, 8, data, 8, tmpBuf1);
    CTOS_DES(d_ENCRYPTION, &key[8], 8, tmpBuf1, 8, tmpBuf2);
    CTOS_DES(d_DECRYPTION, key, 8, tmpBuf2, 8, result);
}

void vdEncrypt3DES(BYTE Mode, BYTE *szMsg, int inMsgLen, BYTE *szKey, BYTE *DESResult, BYTE* szVektor)
{
    USHORT usIndex, usCnt;
    BYTE szDesData[17];

    DebugAddHEX("Msg", szMsg, inMsgLen);

    for (usIndex = 0; usIndex < inMsgLen; usIndex = usIndex + 8) {
        if (Mode == CBC_MODE) {
            for (usCnt = 0; usCnt < 8; usCnt++) {
                if (usIndex == 0)
                    szDesData[usCnt] = szMsg[usIndex + usCnt] ^ szVektor[usCnt];
                else
                    szDesData[usCnt] = szMsg[usIndex + usCnt] ^ DESResult[(usIndex - 8) + usCnt];
            }
        }
        Encrypt3Des(szDesData, szKey, &DESResult[usIndex]);
    }
}

#if 0
/**
 * Decrypt data using 3des algorithm
 * @param [in] Mode CBC_MODE or ECB_MODE
 * @param [in] szMsg
 * @param [in] inMsgLen
 * @param [in] szKey
 * @param [out] DESResult
 * @param [in] szVektor
 */
void vdDecrypt3DES(BYTE Mode, BYTE *szMsg, int inMsgLen, BYTE *szKey, BYTE *DESResult, BYTE* szVektor)
{
    USHORT usIndex, usCnt;

    DebugAddHEX("Msg", szMsg, inMsgLen);
    for (usIndex = 0; usIndex < inMsgLen; usIndex = usIndex + 8) {
        Decrypt3Des(&szMsg[usIndex], szKey, &DESResult[usIndex]);
        if (Mode == CBC_MODE) {
            for (usCnt = 0; usCnt < 8; usCnt++) {
                if (usIndex == 0)
                    DESResult[usIndex + usCnt] = DESResult[usIndex + usCnt] ^ szVektor[usCnt];
                else
                    DESResult[usIndex + usCnt] = DESResult[usIndex + usCnt] ^ szMsg[(usIndex - 8) + usCnt];
            }
        }
    }
}

int inCalcMAC_CBC(BYTE *inPlainData, int inPlainLen, BYTE *byVector, BYTE *outEncData)
{
	USHORT ret;
	CTOS_KMS2DATAENCRYPT_PARA para;
	BYTE str[17];

	BYTE	szOutBuf[1024];
	
	memset(&para, 0x00, sizeof(CTOS_KMS2DATAENCRYPT_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = MPU_MAK_KEYSET;
	para.Protection.CipherKeyIndex = MPU_MAK_KEYIDX;
	para.Protection.CipherMethod = KMS2_DATAENCRYPTCIPHERMETHOD_CBC;
	para.Protection.SK_Length = 0;
	para.Input.Length = inPlainLen;
	para.Input.pData = inPlainData;
        para.Input.ICVLength = 8;
        para.Input.pICV = byVector;
	para.Output.pData = szOutBuf;
	
	ret = CTOS_KMS2DataEncrypt(&para);
	if(ret != d_OK)
	{
 		sprintf(str, "CC_KMS_DES_ECB ret = 0x%04X", ret);
// 		CTOS_LCDTPrintXY(1, 8, str);
 		return ret;
	}

	memcpy(outEncData, szOutBuf, inPlainLen);

	return d_OK;
}

//@@IBR FINISH ADD 06102016
#endif

extern BYTE baMPU_DE07[5];
extern BYTE baMPU_DE61[20];
extern BYTE baMPU_ReqMTI[2];

//void vdMPU_MACTest(void)
//{
//	BYTE baTMK[16];
//	BYTE baEncMAK[16];
//
//	BYTE baMACBuf[1024];
//	BYTE baMAC[8];
//
//	int inMACBufLen = 0;
//
//	memset(baTMK, 0x00, sizeof(baTMK));
//	memset(baEncMAK, 0x00, sizeof(baEncMAK));
//
//	memcpy(baTMK, "\x12\x34\x56\x78\x90\xAB\xCD\xEF\x12\x34\x56\x78\x90\xAB\xCD\xEF", 16);
//	memcpy(baEncMAK, "\x87\x73\xCA\x12\x3D\xD8\x5E\xF8\x59\xD4\xCE\x3F\x70\x58\xF5\x78", 16);
//
//
//	ushMPU_WriteTMK(MPU_TMK_KEYSET, MPU_TMK_KEYIDX, baTMK, 16);
//	ushMPU_WriteEncMAK(baEncMAK, 16, "\x91\x93\x6C\xF8", 4);
//	ushMPU_WriteEncPIK(baEncMAK, 16, "\x91\x93\x6C\xF8", 4);
//
//	memset(baMACBuf, 0x00, sizeof(baMACBuf));
//
//	//memcpy(srTransRec.szMassageType, "\x02\x00", 2);
//	memcpy(baMPU_ReqMTI, "\x02\x00", 2);
//	strcpy(srTransRec.szPAN, "9503080101260490");
//	memcpy(srTransRec.szTotalAmount, "\x00\x00\x00\x01\x00\x00", 6);
//	memcpy(baMPU_DE07, "\x11\x25\x16\x12\x51", 5);
//	srTransRec.ulTraceNum = 20;
//	strcpy(srTransRec.szTID, "10107025");
//	
//	inMACBufLen = inMPU_GenMACBuffer(&srTransRec, baMACBuf, 0);
//	vdDebug_LogPrintf("inMPU_GenMACBuffer inMACBufLen[%d]", inMACBufLen);
//	DebugAddHEX("baMACBuf", baMACBuf, inMACBufLen);
//	
//	//inMACBufLen = 63;
//	//memcpy(baMACBuf, "\x30\x32\x30\x30\x20\x31\x36\x39\x35\x30\x33\x30\x38\x30\x31\x30\x31\x32\x36\x30\x34\x39\x30\x20\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x20\x31\x31\x32\x35\x31\x36\x31\x32\x35\x31\x20\x30\x30\x30\x30\x32\x30\x20\x31\x30\x31\x30\x37\x30\x32\x35", inMACBufLen);
//	memset(baMAC, 0x00, sizeof(baMAC));
//	inCalculateMAC(baMACBuf, inMACBufLen, baMAC);
//
//	DebugAddHEX("baMAC", baMAC, 8);
//	
//}
//
//
//#if 1
//void vdMPU_MACTest2(void)
//{
//	BYTE baTMK[16];
//	BYTE baEncMAK[16];
//
//	BYTE baMACBuf[1024];
//	BYTE baMAC[8];
//
//	int inMACBufLen = 0;
//
//	memset(baTMK, 0x00, sizeof(baTMK));
//	memset(baEncMAK, 0x00, sizeof(baEncMAK));
//
//	memcpy(baTMK, "\x12\x34\x56\x78\x90\xAB\xCD\xEF\x12\x34\x56\x78\x90\xAB\xCD\xEF", 16);
//	memcpy(baEncMAK, "\x49\x58\xEB\x77\xDF\x77\x1B\xA7\x14\xD0\x1A\x2C\x57\xC7\x8B\xCC", 16);
//
//	
//
//	ushMPU_WriteTMK(MPU_TMK_KEYSET, MPU_TMK_KEYIDX, baTMK, 16);
//	ushMPU_WriteEncMAK(baEncMAK, 16, "\x87\x46\x82\x65", 4);
//	ushMPU_WriteEncPIK(baEncMAK, 16, "\x87\x46\x82\x65", 4);
//
//	memset(baMACBuf, 0x00, sizeof(baMACBuf));
//
//	//memcpy(srTransRec.szMassageType, "\x02\x00", 2);
//	memcpy(baMPU_ReqMTI, "\x02\x00", 2);
//	strcpy(srTransRec.szPAN, "9503080000005756");
//	memcpy(srTransRec.szTotalAmount, "\x00\x00\x00\x20\x00\x00", 6);
//	memcpy(baMPU_DE07, "\x12\x05\x13\x57\x42", 5);
//	srTransRec.ulTraceNum = 126;
//	strcpy(srTransRec.szTID, "10102848");
//	
//	inMACBufLen = inMPU_GenMACBuffer(&srTransRec, baMACBuf, 0);
//	vdDebug_LogPrintf("inMPU_GenMACBuffer inMACBufLen[%d]", inMACBufLen);
//	DebugAddHEX("baMACBuf", baMACBuf, inMACBufLen);
//	
//	//inMACBufLen = 63;
//	//memcpy(baMACBuf, "\x30\x32\x30\x30\x20\x31\x36\x39\x35\x30\x33\x30\x38\x30\x31\x30\x31\x32\x36\x30\x34\x39\x30\x20\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x20\x31\x31\x32\x35\x31\x36\x31\x32\x35\x31\x20\x30\x30\x30\x30\x32\x30\x20\x31\x30\x31\x30\x37\x30\x32\x35", inMACBufLen);
//	memset(baMAC, 0x00, sizeof(baMAC));
//	inCalculateMAC(baMACBuf, inMACBufLen, baMAC);
//
//	
//
//	DebugAddHEX("baMAC", baMAC, 8);
//	
//	
//}
//
//
//void vdMPU_MACTest3(void)
//{
//	BYTE baTMK[16];
//	BYTE baEncMAK[16];
//
//	BYTE baMACBuf[1024];
//	BYTE baMAC[8];
//
//	int inMACBufLen = 0;
//
//	memset(baTMK, 0x00, sizeof(baTMK));
//	memset(baEncMAK, 0x00, sizeof(baEncMAK));
//
//	memcpy(baTMK, "\x12\x34\x56\x78\x90\xAB\xCD\xEF\x12\x34\x56\x78\x90\xAB\xCD\xEF", 16);
//	memcpy(baEncMAK, "\x29\x3F\x85\xD2\x01\x91\x30\x0D\xB5\x5F\x14\x78\x1B\xDB\x26\xD3", 16);
//	
//
//	ushMPU_WriteTMK(MPU_TMK_KEYSET, MPU_TMK_KEYIDX, baTMK, 16);
//	ushMPU_WriteEncMAK(baEncMAK, 16, "\x97\x56\x64\x4A", 4);
//	ushMPU_WriteEncPIK(baEncMAK, 16, "\x97\x56\x64\x4A", 4);
//
//	memset(baMACBuf, 0x00, sizeof(baMACBuf));
//
//	//memcpy(srTransRec.szMassageType, "\x02\x00", 2);
//	memcpy(baMPU_ReqMTI, "\x02\x00", 2);
//	strcpy(srTransRec.szPAN, "9503080101260490");
//	memcpy(srTransRec.szTotalAmount, "\x00\x00\x00\x01\x00\x00", 6);
//	memcpy(baMPU_DE07, "\x12\x01\x14\x32\x20", 5);
//	srTransRec.ulTraceNum = 23;
//	strcpy(srTransRec.szTID, "10107025");
//	
//	inMACBufLen = inMPU_GenMACBuffer(&srTransRec, baMACBuf, 0);
//	vdDebug_LogPrintf("inMPU_GenMACBuffer inMACBufLen[%d]", inMACBufLen);
//	DebugAddHEX("baMACBuf", baMACBuf, inMACBufLen);
//	
//	//inMACBufLen = 63;
//	//memcpy(baMACBuf, "\x30\x32\x30\x30\x20\x31\x36\x39\x35\x30\x33\x30\x38\x30\x31\x30\x31\x32\x36\x30\x34\x39\x30\x20\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x20\x31\x31\x32\x35\x31\x36\x31\x32\x35\x31\x20\x30\x30\x30\x30\x32\x30\x20\x31\x30\x31\x30\x37\x30\x32\x35", inMACBufLen);
//	memset(baMAC, 0x00, sizeof(baMAC));
//	inCalculateMAC(baMACBuf, inMACBufLen, baMAC);
//
//	DebugAddHEX("baMAC", baMAC, 8);
//	
//	
//}
//
//void vdMPU_MACTest4(void)
//{
//	BYTE baTMK[16];
//	BYTE baEncMAK[16];
//
//	BYTE baMACBuf[1024];
//	BYTE baMAC[8];
//
//	int inMACBufLen = 0;
//
//	memset(baTMK, 0x00, sizeof(baTMK));
//	memset(baEncMAK, 0x00, sizeof(baEncMAK));
//
//	memcpy(baTMK, "\x12\x34\x56\x78\x90\xAB\xCD\xEF\x12\x34\x56\x78\x90\xAB\xCD\xEF", 16);
//	memcpy(baEncMAK, "\x87\x73\xCA\x12\x3D\xD8\x5E\xF8\x59\xD4\xCE\x3F\x70\x58\xF5\x78", 16);
//
//
//	ushMPU_WriteTMK(MPU_TMK_KEYSET, MPU_TMK_KEYIDX, baTMK, 16);
//	ushMPU_WriteEncMAK(baEncMAK, 16, "\x91\x93\x6C\xF8", 4);
//	ushMPU_WriteEncPIK(baEncMAK, 16, "\x91\x93\x6C\xF8", 4);
//
//	memset(baMACBuf, 0x00, sizeof(baMACBuf));
//
//	//memcpy(srTransRec.szMassageType, "\x02\x00", 2);
//	memcpy(baMPU_ReqMTI, "\x02\x00", 2);
//	strcpy(srTransRec.szPAN, "9503080000005756");
//	memcpy(srTransRec.szTotalAmount, "\x00\x00\x00\x01\x00\x00", 6);
//	memcpy(baMPU_DE07, "\x12\x05\x11\x06\x20", 5);
//	srTransRec.ulTraceNum = 14;
//	strcpy(srTransRec.szTID, "10102848");
//	
//	inMACBufLen = inMPU_GenMACBuffer(&srTransRec, baMACBuf, 0);
//	vdDebug_LogPrintf("inMPU_GenMACBuffer inMACBufLen[%d]", inMACBufLen);
//	DebugAddHEX("baMACBuf", baMACBuf, inMACBufLen);
//	
//	//inMACBufLen = 63;
//	//memcpy(baMACBuf, "\x30\x32\x30\x30\x20\x31\x36\x39\x35\x30\x33\x30\x38\x30\x31\x30\x31\x32\x36\x30\x34\x39\x30\x20\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x20\x31\x31\x32\x35\x31\x36\x31\x32\x35\x31\x20\x30\x30\x30\x30\x32\x30\x20\x31\x30\x31\x30\x37\x30\x32\x35", inMACBufLen);
//	memset(baMAC, 0x00, sizeof(baMAC));
//	inCalculateMAC(baMACBuf, inMACBufLen, baMAC);
//
//	DebugAddHEX("baMAC", baMAC, 8);
//	
//	
//}
//
///*test void*/
//void vdMPU_MACTest5(void)
//{
//	BYTE baTMK[16];
//	BYTE baEncMAK[16];
//
//	BYTE baMACBuf[1024];
//	BYTE baMAC[8];
//
//	int inMACBufLen = 0;
//
//	memset(baTMK, 0x00, sizeof(baTMK));
//	memset(baEncMAK, 0x00, sizeof(baEncMAK));
//
//	memcpy(baTMK, "\x12\x34\x56\x78\x90\xAB\xCD\xEF\x12\x34\x56\x78\x90\xAB\xCD\xEF", 16);
//	memcpy(baEncMAK, "\x49\x58\xEB\x77\xDF\x77\x1B\xA7\x14\xD0\x1A\x2C\x57\xC7\x8B\xCC", 16);
//
//	
//
//	ushMPU_WriteTMK(MPU_TMK_KEYSET, MPU_TMK_KEYIDX, baTMK, 16);
//	ushMPU_WriteEncMAK(baEncMAK, 16, "\x87\x46\x82\x65", 4);
//	ushMPU_WriteEncPIK(baEncMAK, 16, "\x87\x46\x82\x65", 4);
//
//	memset(baMACBuf, 0x00, sizeof(baMACBuf));
//
//	srTransRec.byTransType = VOID;
//	//memcpy(srTransRec.szMassageType, "\x02\x00", 2);
//	memcpy(baMPU_ReqMTI, "\x02\x00", 2);
//	strcpy(srTransRec.szPAN, "9503080000005756");
//	memcpy(srTransRec.szTotalAmount, "\x00\x00\x00\x20\x00\x00", 6);
//	memcpy(baMPU_DE07, "\x12\x05\x13\x57\x52", 5);
//	memcpy(baMPU_DE61, "\x00\x16\x00\x00\x01\x00\x01\x26\x12\x05", 10);
//	srTransRec.ulTraceNum = 127;
//	strcpy(srTransRec.szTID, "10102848");
//	
//	inMACBufLen = inMPU_GenMACBuffer(&srTransRec, baMACBuf, 0);
//	vdDebug_LogPrintf("inMPU_GenMACBuffer inMACBufLen[%d]", inMACBufLen);
//	DebugAddHEX("baMACBuf", baMACBuf, inMACBufLen);
//	
//	//inMACBufLen = 63;
//	//memcpy(baMACBuf, "\x30\x32\x30\x30\x20\x31\x36\x39\x35\x30\x33\x30\x38\x30\x31\x30\x31\x32\x36\x30\x34\x39\x30\x20\x30\x30\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x20\x31\x31\x32\x35\x31\x36\x31\x32\x35\x31\x20\x30\x30\x30\x30\x32\x30\x20\x31\x30\x31\x30\x37\x30\x32\x35", inMACBufLen);
//	memset(baMAC, 0x00, sizeof(baMAC));
//	inCalculateMAC(baMACBuf, inMACBufLen, baMAC);
//
//	
//
//	DebugAddHEX("baMAC", baMAC, 8);
//	
//	
//}
//
//
//#endif



/*TPK Encrypted by TMK, then TPK will saved as plain in KMS*/
USHORT ushIPP_WriteEncTPK(BYTE *baTPK, BYTE byKeyLen)
{
    
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE szIV[8];
	BYTE szKCV[8];

	
	memset(szKCV,0x00,sizeof(szKCV));
	memset(&para,0x00,sizeof(CTOS_KMS2KEYGETINFO_PARA));


	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
//	memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	//memset(szKCV, 0x00, sizeof(szKCV));
	//memcpy(szKCV, "\x09\x24\x22\x90\x0B\xAC\x46\x31", 8);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = IPP_TPK_KEYSET;
	para.Info.KeyIndex = IPP_TPK_KEYIDX;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
//        para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_KPK;
	para.Value.pKeyData = baTPK;
//        para.Value.pKeyData = KeyData;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	para.Protection.CipherKeySet = IPP_TMK_KEYSET;
	para.Protection.CipherKeyIndex = IPP_TMK_KEYIDX;
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*CBB TPK don't have KCV to check*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = 3;
	para.Verification.pKeyCheckValue = szKCV;
	#endif

	vdCTOSS_GetKEYInfo(IPP_TMK_KEYSET, IPP_TMK_KEYIDX);
	
	ret = CTOS_KMS2KeyWrite(&para);
        
	vdDebug_LogPrintf("ushIPP_WriteEncTPK CTOS_KMS2KeyWrite ret[%d]", ret);
        
        if(ret == d_KMS2_KEY_NOT_ALLOWED){
            vdCTOSS_GetKEYInfo(IPP_TPK_KEYSET, IPP_TPK_KEYIDX);
            
//            ret = CTOS_KMS2KeyDelete(IPP_TPK_KEYSET, IPP_TPK_KEYIDX);
//            ret = CTOS_KMS2KeyDeleteAll();
//            ret = CTOS_KMS2Erase();
            
            vdDebug_LogPrintf("CTOS_KMS2KeyDelete ret = %d", ret);
        }

        //start_thandar_add to check IPP KCV TPK
		CTOS_Delay(3000);
		vdCTOSS_GetKEYInfo(IPP_TPK_KEYSET,IPP_TPK_KEYIDX);//thandar
		vdDebug_LogPrintf("IPP_TPK_KEY check [%d]", ret);
		//end_thandar_add to check IPP KCV TPK

	return ret;
}

USHORT ushIPP_WriteTMK(USHORT ushKeySet, USHORT ushKeyIndex, BYTE *baTMK, BYTE byKeyLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];


	KeySet = ushKeySet;
	KeyIndex = ushKeyIndex;
	memcpy(KeyData, baTMK, byKeyLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = ushKeySet;
	para.Info.KeyIndex = ushKeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = baTMK;
	para.Value.KeyLength = byKeyLen;
	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("ushIPP_WriteTMK CTOS_KMS2KeyWrite ret[%d]", ret);

	vdDebug_LogPrintf("ushIPP_WriteTMK = [%04X] KeyIndex[%04X] KeyIndex[%04X] KeyData[%s]", ret, KeySet, KeyIndex, KeyData);
	
	if(ret != d_OK)
	{
		vdDebug_LogPrintf("ushIPP_WriteTMK RETURN KEYWRITE ERROR = 0x%04X", ret);

		sprintf(str, "KEYWRITE ERROR = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return ret;
	}
	

	return ret;
}

void vdCTOS_IPPInjectKey(void)
{
    BYTE szKeyComp1Str[32 + 1];
	BYTE szKeyComp2Str[32 + 1];

	BYTE szKeyComp1Hex[16 + 1];
	BYTE szKeyComp2Hex[16 + 1];
	
	BYTE baKeyComp1KCV[8];
	BYTE baKeyComp2KCV[8];

	BYTE szIPP_TMK[16];
	BYTE szIPP_TMK_KCV[8];
	
    USHORT usKeyStrLen = 16;
    BYTE bRet;
	BYTE key;
	
    USHORT ushKeySet = IPP_TMK_KEYSET; //c001
    USHORT ushKeyIndex = IPP_TMK_KEYIDX; //0004
    USHORT ushRet = 0;

	BYTE szTempDisp[64];
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #1");
    
    memset(szKeyComp1Str, 0x00, sizeof (szKeyComp1Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp1Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp1Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp1Str[%s]", szKeyComp1Str);

	memset(szKeyComp1Hex, 0x00, sizeof (szKeyComp1Hex));
    wub_str_2_hex(szKeyComp1Str, szKeyComp1Hex, 32);

	memset(baKeyComp1KCV, 0x00, sizeof (baKeyComp1KCV));
	CC_DES_ECB_Cipher(szKeyComp1Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp1KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp1KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp1 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #2");
    
    memset(szKeyComp2Str, 0x00, sizeof (szKeyComp2Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp2Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp2Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp2Str[%s]", szKeyComp2Str);

	memset(szKeyComp2Hex, 0x00, sizeof (szKeyComp2Hex));
    wub_str_2_hex(szKeyComp2Str, szKeyComp2Hex, 32);

	memset(baKeyComp2KCV, 0x00, sizeof (baKeyComp2KCV));
	CC_DES_ECB_Cipher(szKeyComp2Hex, 16, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, baKeyComp2KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp2KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp2 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	CC_XorOperation(szKeyComp1Hex, szKeyComp2Hex, 16, szIPP_TMK);

	DebugAddHEX("szIPP_TMK", szIPP_TMK, 16);

	ushRet = ushIPP_WriteTMK(ushKeySet, ushKeyIndex, szIPP_TMK, 16);

    vdDebug_LogPrintf("vdCTOS_IPPInjectKey = %d", ushRet);

	
    if (ushRet != d_OK)
        return;

    memset(szIPP_TMK_KCV, 0x00, sizeof (szIPP_TMK_KCV));
    ushRet = CC_KMS_DES_ECB(ushKeySet, ushKeyIndex, "\x00\x00\x00\x00\x00\x00\x00\x00", 8, szIPP_TMK_KCV);

    CTOS_LCDTClearDisplay();

	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(szIPP_TMK_KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;
        
        vdCTOSS_GetKEYInfo(IPP_TMK_KEYSET, IPP_TMK_KEYIDX);
	
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Final KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

    vdDebug_LogPrintf("vdCTOS_IPPInjectKey = %d", ushRet);

	if(ushRet == d_OK)
		vdDisplayErrorMsgResp2("SUCCESSFUL IPP","","KEY INJECTION");

    return;
}

void vdCTOSS_GetKEYInfo(USHORT ushKeySet, USHORT ushKeyIdx){
    unsigned char szKCV[17];
    CTOS_KMS2KEYGETINFO_PARA pKeyGetInfoPara;
    int ret;
	
	
	memset(szKCV,0x00,sizeof(szKCV));
	memset(&pKeyGetInfoPara,0x00,sizeof(CTOS_KMS2KEYGETINFO_PARA));

    vdDebug_LogPrintf("vdCTOSS_GetKEYInfo ushKeySet[%04X] ushKeyIdx[%04X]", ushKeySet, ushKeyIdx);

    pKeyGetInfoPara.Version = 0x01;	
    pKeyGetInfoPara.Input.KeySet = ushKeySet;	
    pKeyGetInfoPara.Input.KeyIndex = ushKeyIdx;	
    pKeyGetInfoPara.Input.CVLen = 4;	
	pKeyGetInfoPara.Output.pCV = szKCV;	

    ret = CTOS_KMS2KeyGetInfo(&pKeyGetInfoPara);
	
    vdDebug_LogPrintf("CTOS_KMS2KeyGetInfo ret=[%d]",ret);
	
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

    DebugAddHEX("vdCTOSS_GetKEYInfo KCV", pKeyGetInfoPara.Output.pCV, 4);
    //vdDebug_LogPrintf("CTOS_KMS2KeyGetInfo KeyAttribute=[%08x]",pKeyGetInfoPara.Output.KeyAttribute);

}

/*PIK Encrypted by TMK, then PIK will saved as plain in KMS*/
USHORT ushCAV_WriteEncPIK(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];
        int inHostIndex = 0;
        
        inHostIndex = inHDTCheckByHostName("MPU");
        inDCTRead(inHostIndex);

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushCAV_WriteEncPIK=====");
	vdDebug_LogPrintf("ushCAV_WriteEncPIK inHostIndex:[%d]", inHostIndex);
	vdDebug_LogPrintf("=====UPI INDEX SAVED=====");
	
	DebugAddHEX("baPIK", baPIK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MPU_PIK_KEYSET;	//0xC000
	para.Info.KeyIndex = MPU_PIK_KEYIDX; // 0015
        
//        para.Info.KeySet = strDCT.usKeySet;
//        para.Info.KeyIndex = strDCT.usKeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baPIK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	//para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_CBC;
    //para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Protection.CipherKeySet = MPU_TMK_KEYSET;		//0xC000
	para.Protection.CipherKeyIndex = MPU_TMK_KEYIDX; // 0014
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU PIK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

	
	vdDebug_LogPrintf("ushCAV_WriteEncPIK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}
void vdCTOS_UPI_Finexus_InjectKey(void)
{
    BYTE szKeyComp1Str[32 + 1];
	BYTE szKeyComp2Str[32 + 1];

	BYTE szKeyComp1Hex[16 + 1];
	BYTE szKeyComp2Hex[16 + 1];
	
	BYTE baKeyComp1KCV[8];
	BYTE baKeyComp2KCV[8];

	BYTE szUPI_TMK[16];
	BYTE szUPI_TMK_KCV[8];
	
    USHORT usKeyStrLen = 16;
    BYTE bRet;
	BYTE key;
	
    USHORT ushKeySet = CBB_TMK_KEYSET;
    USHORT ushKeyIndex = CBB_TMK_KEYIDX;
    USHORT ushRet = 0;

	BYTE szTempDisp[64];
	BYTE inPlainData[8+1];

	memset(szUPI_TMK, 0x00, sizeof (szUPI_TMK));
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #1");
    
    memset(szKeyComp1Str, 0x00, sizeof (szKeyComp1Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp1Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp1Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp1Str[%s]", szKeyComp1Str);

	memset(szKeyComp1Hex, 0x00, sizeof (szKeyComp1Hex));
    wub_str_2_hex(szKeyComp1Str, szKeyComp1Hex, 32);

	memset(baKeyComp1KCV, 0x00, sizeof (baKeyComp1KCV));
	memset(inPlainData, 0x00, sizeof (inPlainData));
	memcpy(inPlainData,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
	CC_DES_ECB_Cipher(szKeyComp1Hex, 16, inPlainData, 8, baKeyComp1KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp1KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp1 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	CTOS_LCDTClearDisplay();
    vdDispTitleString("ENTER COMPONENT #2");
    
    memset(szKeyComp2Str, 0x00, sizeof (szKeyComp2Str));

	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter First 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, szKeyComp2Str, &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	usKeyStrLen = 16;
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Enter Second 8 Bytes KEY");
	bRet = InputStringAlphaEx2(1, 4, 0x04, 0x02, &szKeyComp2Str[16], &usKeyStrLen, 16, d_INPUT_TIMEOUT);
	if (bRet == d_KBD_CANCEL)
		return;
	
	vdDebug_LogPrintf("szKeyComp2Str[%s]", szKeyComp2Str);

	memset(szKeyComp2Hex, 0x00, sizeof (szKeyComp2Hex));
    wub_str_2_hex(szKeyComp2Str, szKeyComp2Hex, 32);

	memset(baKeyComp2KCV, 0x00, sizeof (baKeyComp2KCV));
	memset(inPlainData, 0x00, sizeof (inPlainData));
	memcpy(inPlainData,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
	CC_DES_ECB_Cipher(szKeyComp2Hex, 16, inPlainData, 8, baKeyComp2KCV);
	
	vduiClearBelow(2);
	
	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(baKeyComp2KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;

	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Comp2 KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

	CC_XorOperation(szKeyComp1Hex, szKeyComp2Hex, 16, szUPI_TMK);

	DebugAddHEX("szUPI_TMK", szUPI_TMK, 16);

	ushRet = ushIPP_WriteTMK(ushKeySet, ushKeyIndex, szUPI_TMK, 16);
    if (ushRet != d_OK)
        return;

	memset(szUPI_TMK_KCV, 0x00, sizeof (szUPI_TMK_KCV));
	memset(inPlainData, 0x00, sizeof (inPlainData));
	memcpy(inPlainData,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
	
    ushRet = CC_KMS_DES_ECB(ushKeySet, ushKeyIndex, inPlainData, 8, szUPI_TMK_KCV);
    #if 1
	vdDebug_LogPrintf("ushRet = %d", ushRet);
	
    CTOS_LCDTClearDisplay();

	memset(szTempDisp, 0x00, sizeof(szTempDisp));
	wub_hex_2_str(szUPI_TMK_KCV, szTempDisp, 4);
	szTempDisp[8] = 0x00;
        
    vdCTOSS_GetKEYInfo(CBB_TMK_KEYSET, CBB_TMK_KEYIDX);
	
	setLCDPrint(3, DISPLAY_POSITION_LEFT, "Pls Verify Final KCV");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, szTempDisp);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY");

    CTOS_KBDGet(&key);

    vdDebug_LogPrintf("vdCTOS_UPIInjectKey = %d", ushRet);
	#endif
    return;

	
}


#ifdef CBB_FIN_ROUTING



/*PIK Encrypted by TMK, then PIK will saved as plain in KMS*/
USHORT ushUSD_WriteEncPIK(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushUSD_WriteEncPIK=====");
	DebugAddHEX("baPIK", baPIK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = USD_FIN_MAC_KEYSET;	//0xC003
	para.Info.KeyIndex = USD_FIN_MAC_KEYIDX; // 0016 - DCT TABLE EQUIVALENT TO - int 22 value
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baPIK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	para.Protection.CipherKeySet = USD_FIN_TMK_KEYSET;	//0xC003
	para.Protection.CipherKeyIndex = USD_FIN_TMK_KEYIDX; //0012 -  USD HOST
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU PIK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

	
	vdDebug_LogPrintf("ushUSD_WriteEncPIK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}



/*PIK Encrypted by TMK, then PIK will saved as plain in KMS*/
USHORT ushMMK_WriteEncPIK(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushMMK_WriteEncPIK=====");
	DebugAddHEX("baPIK", baPIK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MMK_FIN_MAC_KEYSET;	//0xC000
	para.Info.KeyIndex = MMK_FIN_MAC_KEYIDX; // 0015 - 21 in DCT TABLE EQUIVALENT 
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baPIK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;

	para.Protection.CipherKeySet = MMK_FIN_TMK_KEYSET;	//0xC003
	para.Protection.CipherKeyIndex = MMK_FIN_TMK_KEYIDX; //0011 -  MMK HOST
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU PIK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

	
	vdDebug_LogPrintf("ushMMK_WriteEncPIK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}




/*MAK Encrypted by TMK, then MAK will saved as plain in KMS*/
USHORT ushMMK_WriteEncMAK(BYTE *baMAK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushMMK_WriteEncMAK=====");
	DebugAddHEX("baMAK", baMAK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MMK_FIN_MAC_KEYSET;		//0xC000
	para.Info.KeyIndex = MMK_FIN_MAC_KEYIDX2;	//00016
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Value.pKeyData = baMAK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;

	para.Protection.CipherKeySet = MMK_FIN_TMK_KEYSET;  // 0xc003 reference for key injection (vdCTOS_MPUInjectKey function)
	para.Protection.CipherKeyIndex = MMK_FIN_TMK_KEYIDX; //0011
    memset(szIV, 0x00, sizeof(szIV));
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU MAK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ MAK ERROR!");
	}
	
	vdDebug_LogPrintf("ushMMK_WriteEncMAK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}





/*MAK Encrypted by TMK, then MAK will saved as plain in KMS*/
USHORT ushUSD_WriteEncMAK(BYTE *baMAK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushUSD_WriteEncMAK=====");
	DebugAddHEX("baMAK", baMAK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = USD_FIN_MAC_KEYSET;		//0xC000
	para.Info.KeyIndex = USD_FIN_MAC_KEYIDX2;	// 0016 - DCT TABLE EQUIVALENT TO - int 22 value
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Value.pKeyData = baMAK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;

	para.Protection.CipherKeySet = USD_FIN_TMK_KEYSET;  // reference for key injection (vdCTOS_MPUInjectKey function)
	para.Protection.CipherKeyIndex = USD_FIN_TMK_KEYIDX; //0012
    memset(szIV, 0x00, sizeof(szIV));
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU MAK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ MAK ERROR!");
	}
	
	vdDebug_LogPrintf("ushUSD_WriteEncMAK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}





void vdHardCodeUSD_FinexusUATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	//BYTE szTPK[16];

	CTOS_KMS2Init();

	//KeySet = USD_FIN_TMK_KEYSET; //c003
	//KeyIndex = USD_FIN_TMK_KEYIDX; //0012 - refer to CAash Advance menu/ transaction.  lookup at inUnPackIsoFunc57

	vdDebug_LogPrintf("vdHardCodeUSD_FinexusUATKey");
	

	//854968167F4A101F37086EB3FBDF1670
	memcpy(KeyData, "\x85\x49\x68\x16\x7F\x4A\x10\x1F\x37\x08\x6E\xB3\xFB\xDF\x16\x70", 16);

	ushUSD_WriteTMKPlain(KeyData, IPP_TDES_KEY_SIZE);

	vdCTOSS_GetKEYInfo(USD_FIN_TMK_KEYSET,USD_FIN_TMK_KEYIDX);

	//memset(szTPK, 0x00, sizeof (szTPK));
	//memcpy(szTPK, "\x22\xBF\xDF\xDC\x75\x56\x3C\x3A\x42\xBA\xC3\x7D\xE8\xCC\x0B\x53", 16);
	//ushCBB_WriteEncTPK(szTPK, 16);		
	  
}



void vdHardCodeMMK_FinexusUATKey(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];

	//BYTE szTPK[16];

	CTOS_KMS2Init();

	//KeySet = MMK_FIN_TMK_KEYSET; //c003
	//KeyIndex = MMK_FIN_TMK_KEYIDX; //0011 - refer to CAash Advance menu/ transaction.  lookup at inUnPackIsoFunc57

	vdDebug_LogPrintf("vdHardCodeMMK_FinexusUATKey");
	
	//EA 57 C1 1C AD 5B 51 FE 
	//51 0D 76 BC 38 E6 64 97 - UAT TMK KEYS
	// KCV - 36AF25
	// FINAL KCV - A5F05F18
	
	memcpy(KeyData, "\xEA\x57\xC1\x1C\xAD\x5B\x51\xFE\x51\x0D\x76\xBC\x38\xE6\x64\x97", 16);

	ushMMK_WriteTMKPlain(KeyData, IPP_TDES_KEY_SIZE);

	vdCTOSS_GetKEYInfo(MMK_FIN_TMK_KEYSET,MMK_FIN_TMK_KEYIDX);

	  
}



/*PIK Encrypted by TMK, then PIK will saved as plain in KMS*/
USHORT ushUSD_WriteEncPIK2(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];
        int inHostIndex = 0;
        
    //    inHostIndex = inHDTCheckByHostName("MPU");
    inDCTRead(22);

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushUSD_WriteEncPIK2=====");
	vdDebug_LogPrintf("ushUSD_WriteEncPIK2 inHostIndex:[%d]", inHostIndex);
	vdDebug_LogPrintf("=====FIN-USDINDEX SAVED=====");
	
	DebugAddHEX("baPIK", baPIK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = USD_FIN_MAC_KEYSET;//0xC003 
	para.Info.KeyIndex = USD_FIN_MAC_KEYIDX; //0016 - (22) in DCT table 
        

	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baPIK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;

	para.Protection.CipherKeySet = USD_FIN_TMK_KEYSET;		//0xC003
	para.Protection.CipherKeyIndex = USD_FIN_TMK_KEYIDX;	//0012
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU PIK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

	
	vdDebug_LogPrintf("ushUSD_WriteEncPIK2 CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}



/*PIK Encrypted by TMK, then PIK will saved as plain in KMS*/
USHORT ushMMK_WriteEncPIK2(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];
        int inHostIndex = 0;
        
    //    inHostIndex = inHDTCheckByHostName("MPU");
    inDCTRead(23);

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushMMK_WriteEncPIK2=====");
	vdDebug_LogPrintf("ushMMK_WriteEncPIK2 inHostIndex:[%d]", inHostIndex);
	vdDebug_LogPrintf("=====FIN-USDINDEX SAVED=====");
	
	DebugAddHEX("baPIK", baPIK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = MMK_FIN_MAC_KEYSET; // 0xC003
	para.Info.KeyIndex = MMK_FIN_MAC_KEYIDX; //0015 - (21) in DCT table
        

	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baPIK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;

	para.Protection.CipherKeySet = MMK_FIN_TMK_KEYSET;		//0xC003
	para.Protection.CipherKeyIndex = MMK_FIN_TMK_KEYIDX;	//0011
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU PIK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

	
	vdDebug_LogPrintf("ushMMK_WriteEncPIK2 CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}


#endif


#ifdef PIN_CHANGE_ENABLE

/*PIK Encrypted by TMK, then PIK will saved as plain in KMS*/
USHORT ushCAV_WriteEncPIK2(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szIV[8], szDataIn[100], szMAC[8];
	BYTE szKCV[8];
        int inHostIndex = 0;
        
    //    inHostIndex = inHDTCheckByHostName("MPU");
    inDCTRead(21);

	//KeySet = 0xC000;
	//KeyIndex = 0x0012;
	//memcpy(KeyData, "\x11\x11\x11\x11\x11\x11\x11\x11\x22\x22\x22\x22\x22\x22\x22\x22", 16);

	vdDebug_LogPrintf("=====ushCAV_WriteEncPIK=====");
	vdDebug_LogPrintf("ushCAV_WriteEncPIK2 inHostIndex:[%d]", inHostIndex);
	vdDebug_LogPrintf("=====MC/VISA INDEX SAVED=====");
	
	DebugAddHEX("baPIK", baPIK, 16);

	memset(szKCV, 0x00, sizeof(szKCV));
	memcpy(szKCV, baKCV, byKCVLen);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = CBB_COMM_KEYSET;		//0xC000
	para.Info.KeyIndex = CBB_TPK_KEYIDX;  //0005 - (5) in DCT table
        
//        para.Info.KeySet = strDCT.usKeySet;
//        para.Info.KeyIndex = strDCT.usKeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT;
	para.Value.pKeyData = baPIK;
	para.Value.KeyLength = byKeyLen;
	
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_ECB;
	//para.Protection.Mode = KMS2_KEYPROTECTIONMODE_KPK_CBC;
    //para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Protection.CipherKeySet = CBB_COMM_KEYSET;		//0xC000
	para.Protection.CipherKeyIndex = CBB_TAK_KEYIDX;	//0006
	para.Protection.AdditionalData.pData = szIV;
	para.Protection.AdditionalData.Length = 8;
	/*MPU PIK KCV*/
	#if 0
	para.Verification.Method = KMS2_KEYVERIFICATIONMETHOD_DEFAULT;
	para.Verification.KeyCheckValueLength = byKCVLen;
	para.Verification.pKeyCheckValue = baKCV;
	#endif
	
	ret = CTOS_KMS2KeyWrite(&para);
	if (d_OK != ret)
	{
		vdDisplayErrorMsg(1, 8, "INJ PIK ERROR!");
	}

	
	vdDebug_LogPrintf("ushCAV_WriteEncPIK CTOS_KMS2KeyWrite ret[%d]", ret);

	return ret;
}


int inCTOS_GetNewPIN(void){
BYTE key;	
int inRet = d_NO;
char szNewPIN[16] = {0};

vdDebug_LogPrintf("::inCTOS_GetNewPIN -->>START");

if (inMultiAP_CheckSubAPStatus() == d_OK)
return d_OK;

vduiLightOn();
CTOS_KBDHit(&key);//clear key buffer

//-->> NEW PIN
inRet = inCTOS_PinPadTypePinEntry(TRUE);
if(d_OK != inRet)
	return inRet;

memset(szNewPIN, 0x00, sizeof(szNewPIN));
memcpy((char *)szNewPIN, srTransRec.szNewPINBlock, PIN_SIZE);
vdDebug_LogPrintf(":: | szNewPIN[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", (unsigned char)szNewPIN[0], (unsigned char)szNewPIN[1], (unsigned char)szNewPIN[2],(unsigned char)szNewPIN[3], (unsigned char)szNewPIN[4], (unsigned char)szNewPIN[5], (unsigned char)szNewPIN[6], (unsigned char)szNewPIN[7]);

vdDebug_LogPrintf("::inCTOS_GetNewPIN -->>END");

return inRet;

}


int inCTOS_GetVerifyPIN(void){
BYTE key;	
int inRet = d_NO;
char szVerifyPIN[16] = {0};

	vdDebug_LogPrintf("::inCTOS_GetVerifyPIN -->>START");

	if (inMultiAP_CheckSubAPStatus() == d_OK)
	return d_OK;

	vduiLightOn();
	CTOS_KBDHit(&key);//clear key buffer

	//-->> VERIFY PIN
	inRet = inCTOS_PinPadTypePinEntry(FALSE);
	if(d_OK != inRet)
		return inRet;

	memset(szVerifyPIN, 0x00, sizeof(szVerifyPIN));
	memcpy((char *)szVerifyPIN, srTransRec.szPINBlock, PIN_SIZE);
	vdDebug_LogPrintf("Default PINBLOCK | szVerifyPIN[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", (unsigned char)szVerifyPIN[0], (unsigned char)szVerifyPIN[1], (unsigned char)szVerifyPIN[2],(unsigned char)szVerifyPIN[3], (unsigned char)szVerifyPIN[4], (unsigned char)szVerifyPIN[5], (unsigned char)szVerifyPIN[6], (unsigned char)szVerifyPIN[7]);

	vdDebug_LogPrintf("::inCTOS_GetVerifyPIN -->>END");

	return inRet;

}


int inCTOS_ValidatePINEntry(void)
{
	char szNewPIN[16] = {0};
	char szVerifyPIN[16] = {0};
	int inRet;

	memset(szNewPIN, 0x00, sizeof(szNewPIN));
	memcpy((char *)szNewPIN, srTransRec.szNewPINBlock, PIN_SIZE);

	memset(szVerifyPIN, 0x00, sizeof(szVerifyPIN));
	//memcpy((char *)szVerifyPIN, srTransRec.szPINBlock, PIN_SIZE);
	memcpy((char *)szVerifyPIN, srTransRec.szVerifyNewPINBlock, PIN_SIZE);

	vdDebug_LogPrintf("inCTOS_ValidatePINEntry: szNewPIN[%s]|szVerifyPIN[%s]", szNewPIN, szVerifyPIN);
	
	if (strcmp(szNewPIN, szVerifyPIN) != 0){
		CTOS_LCDTClearDisplay(); 
		vdDispTransTitle(srTransRec.byTransType);
		vdDisplayErrorMsg(1, 8, "PIN MISMATCH");
		return(d_NO);
	}

	return(d_OK);
	
}


void vdCTOS_PinEntryScreenDisplay(BOOL fPinSequence){
BYTE key;
int inRet = d_OK;
BYTE szDisplayLine1[IPP_DISPLAY_SIZE + 1] = {0};
BYTE szPinMsg[20 + 1] = {0};

CTOS_LCDTClearDisplay();
vdDispTransTitle(srTransRec.byTransType);

vdDebug_LogPrintf("-->>vdCTOS_PinEntryScreenDisplay");

memset(szPinMsg, 0x00, sizeof(szPinMsg));
switch (fPinSequence)
{
	case ENTER_DEFAULT_PIN:
		strcpy(szPinMsg, "ENTER DEFAULT PIN");
	break;
	case ENTER_NEW_PIN:
		strcpy(szPinMsg, "ENTER NEW PIN");
	break;
	case ENTER_VERIFY_PIN:
		strcpy(szPinMsg, "ENTER VERIFY NEW PIN");
	break;
	case ENTER_PIN:
		strcpy(szPinMsg, "ENTER PIN");
	break;
	default:
		strcpy(szPinMsg, "ENTER PIN");
	break;	
}


	strcpy(szDisplayLine1, strDCT.szDisplayLine1); // for PCi100


if ((strTCT.byTerminalType % 2) == 1)
{
    if (isCheckTerminalMP200() == d_OK){
    	setLCDPrint27(5, d_LCD_ALIGNLEFT, szDisplayLine1);
		setLCDPrint27(6, d_LCD_ALIGNLEFT, szPinMsg);

    }else{
		setLCDPrint(5, DISPLAY_POSITION_LEFT, szDisplayLine1);
		setLCDPrint(6, DISPLAY_POSITION_LEFT, szPinMsg);
    }
}
else
{
    if (isCheckTerminalMP200() == d_OK){
    	setLCDPrint27(5, DISPLAY_POSITION_LEFT, szDisplayLine1);
		setLCDPrint27(6, DISPLAY_POSITION_LEFT, szPinMsg);

    }else{
		CTOS_LCDTPrintAligned(5, szDisplayLine1, d_LCD_ALIGNLEFT);
		CTOS_LCDTPrintAligned(6, szPinMsg, d_LCD_ALIGNLEFT);
    }
}

}


int inCTOS_PinPadTypePinEntry(BOOL fNewPin){
int inRet = d_NO;

inTCTRead(1);

vdDebug_LogPrintf("-->>inCTOS_PinPadTypePinEntry byTransType[%d]", srTransRec.byTransType);
vdDebug_LogPrintf("-->>inCTOS_PinPadTypePinEntry byPinPadType[%d]", strTCT.byPinPadType);
vdDebug_LogPrintf("-->>inCTOS_PinPadTypePinEntry byPinPadPort[%d]", strTCT.byPinPadPort);
//vdDebug_LogPrintf("-->>inCTOS_PinPadTypePinEntry inPinPadBaudRate[%d]", strTCT.inPinPadBaudRate);
//vdDebug_LogPrintf("-->>inCTOS_PinPadTypePinEntry byEntryMode[%d]||byCtlsMode[%d]:||fSourcePINRequired[%d]", srTransRec.byEntryMode, strTCT.byCtlsMode, srTransRec.fSourcePINRequired);


		if (fNewPin == TRUE){
			inRet = inCTOS_inGetNewIPPPin();
			if (inRet != d_OK)
			return(d_NO);
		}
		else{
			inRet = inCTOS_inGetVerifyIPPPin();
			if (inRet != d_OK)
			return(d_NO);
		}

return(d_OK);
}
#if 1
int inGetCAVIPPPin(void)
{
    CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

    BYTE str[40],key;
    USHORT ret;
	char szPINKey[16+1];
	BYTE   EMVtagVal[64];
    USHORT EMVtagLen;

	CTOS_KMS2Init();


    vdDebug_LogPrintf("*** inGetCAVIPPPin byEntryMode:%d byTransType:%d ***", srTransRec.byEntryMode, srTransRec.byTransType);

	#ifdef PIN_CHANGE_ENABLE
	if(srTransRec.byTransType == CHANGE_PIN)
		return d_OK;

	#endif
		
	vdDebug_LogPrintf("inGetCAVIPPPin....");

    DebugAddSTR("inGetCAVIPPPin","Processing...",20);
    vdDebug_LogPrintf("*** inGetCAVIPPPin HDTid = %d",srTransRec.HDTid);

	
    inDCTRead(srTransRec.HDTid);
    //inDCTRead(17); // to get keyindex 21 - working

	strDCT.inMinPINDigit = 4;
	strDCT.inMaxPINDigit = 12;

#if 1
	if((srTransRec.HDTid == 7) || (srTransRec.HDTid == 17))// get TPK for UPI cards
	{	
		strDCT.usKeySet = MPU_PIK_KEYSET;
		strDCT.usKeyIndex = MPU_PIK_KEYIDX; // CipherKeyIndex/TPK index 0014
	}
	else
	{
		#ifdef CBB_FIN_ROUTING
			if(srTransRec.HDTid == 23){
				strDCT.usKeySet = MMK_FIN_MAC_KEYSET;
				strDCT.usKeyIndex = MMK_FIN_MAC_KEYIDX;
			}
			else if (srTransRec.HDTid == 22){
				strDCT.usKeySet = USD_FIN_MAC_KEYSET;
				strDCT.usKeyIndex = USD_FIN_MAC_KEYIDX;
			}
			else
			{	
				strDCT.usKeySet = CBB_TPK_KEYSET;
				strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
			}
		#else		
			strDCT.usKeySet = CBB_TPK_KEYSET;
			strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
		#endif
	}
#endif


	vdDebug_LogPrintf("strDCT.usKeySet[%04X]", strDCT.usKeySet);
    vdDebug_LogPrintf("strDCT.usKeyIndex[%04X]", strDCT.usKeyIndex);

	vdDebug_LogPrintf("inGetCAVIPPPin T9F34 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
	
    CTOS_LCDTClearDisplay();
	if (strTCT.byPinPadMode != 1)
	{
		//#ifdef QUICKPASS
		// fix RC[12] - did not prompt pin entry on quickpass txn.
		#if 0
        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        {
        	#if 1 // for testing, 6244 1902 (UPI) should support pin entry if hostid MPU (17, 18 or 19) 09072020
			if(srTransRec.HDTid == 7)
			{
            	EMVtagLen = 3;
            	memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
            	if ((EMVtagVal[0] != 0x02) && (EMVtagVal[0] != 0x42))
            	{
                	return d_OK;
            	}
			}
			#endif
            
            if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.HDTid == 7) /*Host 7 - UPI Finexus*/
                return d_OK;
        }
		#endif
		
        vduiClearBelow(2);
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
        vdDebug_LogPrintf("*** HDTid 2 = %d",srTransRec.HDTid);
        vdDebug_LogPrintf("strDCT.szDisplayLine1 [%s]", strDCT.szDisplayLine1);
        vdDebug_LogPrintf("strDCT.szDisplayLine2 [%s]", strDCT.szDisplayLine2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	}
	else
	{
		displaybmpEx(0, 0, "Enterpin.bmp");
	}
	
	if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3))
	{
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "Please key in PINPAD");
		
		memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
		stOLPinParam.HDTid = srTransRec.HDTid;
		stOLPinParam.ushKeySet = strDCT.usKeySet;
		stOLPinParam.ushKeyIdx = strDCT.usKeyIndex;
		strcpy(stOLPinParam.szPAN,srTransRec.szPAN);
		memcpy(stOLPinParam.szPINKey,strDCT.szPINKey,16);
                DebugAddHEX("PIN KEY", stOLPinParam.szPINKey, 16);
		ret = inCTOSS_EXTGetIPPPin();
		
		memset(card_holder_pin,0,sizeof(card_holder_pin));
		if (ret == d_OK)
		{
			if (stOLPinParam.inPINEntered == 2)
			{
				memcpy(srTransRec.szPINBlock,stOLPinParam.szPINBlock,8);
				wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
			}
			ret = stOLPinParam.ushKeySet;
			
		}
		else
		{
			if (ret != d_NO)
				ret = stOLPinParam.ushKeySet;
		}
		
		vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
		return ret;
	}
    
    memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
    stPinGetPara.Version = 0x02;
	#if 0
    stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	if (strDCT.inPIN3des == 0)//for des pin
	{
		memset(szPINKey,0x00,sizeof(szPINKey));
		memcpy(szPINKey,strDCT.szPINKey,8);
		memcpy(szPINKey+8,strDCT.szPINKey,8);
		stPinGetPara.Protection.pSK = szPINKey;
	}
	else
    	stPinGetPara.Protection.pSK = strDCT.szPINKey;
	#endif

	stPinGetPara.Protection.SK_Length = 0x00;
	stPinGetPara.Protection.pSK = NULL;
    
    stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
    stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
    stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
    vdDebug_LogPrintf("**Min[%d]Max[%d]**",strDCT.inMinPINDigit,strDCT.inMaxPINDigit);

    stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
    stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
    
//    stPinGetPara.Protection.CipherKeyIndex = CUP_TMK_KEY_INDEX;
//    stPinGetPara.Protection.CipherKeySet = CUP_TMK_KEY_SET;
    
    stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
    stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
    stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
    stPinGetPara.Control.Timeout = 0;
    
    if(fGetMPUCard() == TRUE){
        stPinGetPara.Control.NULLPIN = FALSE;
    } else {
        stPinGetPara.Control.NULLPIN = TRUE;
    }
    
    stPinGetPara.PINOutput.EncryptedBlockLength = 8;
    stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
    stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
    stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
    stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

    //CTOS_KBDBufFlush();//cleare key buffer -commented to resolve 04-02-2020 issue below?

//(1)All asked pin number card types(MPU-UPI co-brand card onus/offus,MPU onus/offus and Pure UPI onus/offus)are masking asterisk(*)
//at inputing PIN Number.So we can't see how much pin digits in terminal UI.
//04-02-2020
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
    
    vdDebug_LogPrintf("**inGetCAVIPPPin Online PIN[%d]*PINBlock[%s]*",ret,stPinGetPara.PINOutput.pEncryptedBlock);
    
    DebugAddHEX("inGetCAVIPPPin szPINBlock=",stPinGetPara.PINOutput.pEncryptedBlock,8);
    
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
            memset(card_holder_pin,0,sizeof(card_holder_pin));
			srTransRec.byPINEntryCapability = 2;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
            return ret;
        }
    }

	srTransRec.byPINEntryCapability = 1;
		
    if(stPinGetPara.PINOutput.EncryptedBlockLength != 8)
    {
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
    }
    else
    {
        wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
        vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
    }
    return d_OK;
}


int inCTOS_inGetDefaultIPPPin(void)
{
    CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

    BYTE str[40],key;
    USHORT ret;
	char szPINKey[16+1];
	BYTE   EMVtagVal[64];
    USHORT EMVtagLen;

	CTOS_KMS2Init();


    vdDebug_LogPrintf("*** inCTOS_inGetDefaultIPPPin byEntryMode:%d byTransType:%d ***", srTransRec.byEntryMode, srTransRec.byTransType);

	vdDebug_LogPrintf("inCTOS_inGetDefaultIPPPin....");

    DebugAddSTR("inCTOS_inGetDefaultIPPPin","Processing...",20);
    vdDebug_LogPrintf("*** inCTOS_inGetDefaultIPPPin HDTid = %d",srTransRec.HDTid);

	
    inDCTRead(srTransRec.HDTid);
	//inDCTRead(17); - original working


	vdCTOS_PinEntryScreenDisplay(ENTER_DEFAULT_PIN);

	strDCT.inMinPINDigit = 4;
	strDCT.inMaxPINDigit = 12;

#if 1
	if((srTransRec.HDTid == 7) || (srTransRec.HDTid == 17))// get TPK for UPI cards
	{	
		strDCT.usKeySet = MPU_PIK_KEYSET;
		strDCT.usKeyIndex = MPU_PIK_KEYIDX; // CipherKeyIndex/TPK index 0014
	}
	else
	{
		#ifdef CBB_FIN_ROUTING
			if(srTransRec.HDTid == 23){
				strDCT.usKeySet = MMK_FIN_MAC_KEYSET;
				strDCT.usKeyIndex = MMK_FIN_MAC_KEYIDX;
			}
			else if (srTransRec.HDTid == 22){
				strDCT.usKeySet = USD_FIN_MAC_KEYSET;
				strDCT.usKeyIndex = USD_FIN_MAC_KEYIDX;
			}
			else
			{	
				strDCT.usKeySet = CBB_TPK_KEYSET;
				strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
			}
		#else		
			strDCT.usKeySet = CBB_TPK_KEYSET;
			strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
		#endif

	}
#endif



	vdDebug_LogPrintf("strDCT.usKeySet[%04X]", strDCT.usKeySet);
    vdDebug_LogPrintf("strDCT.usKeyIndex[%04X]", strDCT.usKeyIndex);

	vdDebug_LogPrintf("inCTOS_inGetDefaultIPPPin T9F34 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
	
    //CTOS_LCDTClearDisplay();
	if (strTCT.byPinPadMode != 1)
	{
		//#ifdef QUICKPASS
		#if 0
        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        {
        	#if 1 // for testing, 6244 1902 (UPI) should support pin entry if hostid MPU (17, 18 or 19) 09072020
			if(srTransRec.HDTid == 7)
			{
            	EMVtagLen = 3;
            	memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
            	if ((EMVtagVal[0] != 0x02) && (EMVtagVal[0] != 0x42))
            	{
                	return d_OK;
            	}
			}
			#endif
            
            if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.HDTid == 7) /*Host 7 - UPI Finexus*/
                return d_OK;
        }	
        vduiClearBelow(2);
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));

        inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
        vdDebug_LogPrintf("*** HDTid 2 = %d",srTransRec.HDTid);
        vdDebug_LogPrintf("strDCT.szDisplayLine1 [%s]", strDCT.szDisplayLine1);
        vdDebug_LogPrintf("strDCT.szDisplayLine2 [%s]", strDCT.szDisplayLine2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
		#else
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));
		
		#endif
	}
	else
	{
		displaybmpEx(0, 0, "Enterpin.bmp");
	}
	
	if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3))
	{
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "Please key in PINPAD");
		
		memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
		stOLPinParam.HDTid = srTransRec.HDTid;
		stOLPinParam.ushKeySet = strDCT.usKeySet;
		stOLPinParam.ushKeyIdx = strDCT.usKeyIndex;
		strcpy(stOLPinParam.szPAN,srTransRec.szPAN);
		memcpy(stOLPinParam.szPINKey,strDCT.szPINKey,16);
                DebugAddHEX("PIN KEY", stOLPinParam.szPINKey, 16);
		ret = inCTOSS_EXTGetIPPPin();
		
		memset(card_holder_pin,0,sizeof(card_holder_pin));
		if (ret == d_OK)
		{
			if (stOLPinParam.inPINEntered == 2)
			{
				memcpy(srTransRec.szPINBlock,stOLPinParam.szPINBlock,8);
				wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
			}
			ret = stOLPinParam.ushKeySet;
			
		}
		else
		{
			if (ret != d_NO)
				ret = stOLPinParam.ushKeySet;
		}
		
		vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
		return ret;
	}
    
    memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
    stPinGetPara.Version = 0x02;
	#if 0
    stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	if (strDCT.inPIN3des == 0)//for des pin
	{
		memset(szPINKey,0x00,sizeof(szPINKey));
		memcpy(szPINKey,strDCT.szPINKey,8);
		memcpy(szPINKey+8,strDCT.szPINKey,8);
		stPinGetPara.Protection.pSK = szPINKey;
	}
	else
    	stPinGetPara.Protection.pSK = strDCT.szPINKey;
	#endif

	stPinGetPara.Protection.SK_Length = 0x00;
	stPinGetPara.Protection.pSK = NULL;
    
    stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
    stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
    stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
    vdDebug_LogPrintf("**Min[%d]Max[%d]**",strDCT.inMinPINDigit,strDCT.inMaxPINDigit);

    stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
    stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;

//    stPinGetPara.Protection.CipherKeyIndex = CUP_TMK_KEY_INDEX;
//    stPinGetPara.Protection.CipherKeySet = CUP_TMK_KEY_SET;
    
    stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
    stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
    stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
    stPinGetPara.Control.Timeout = 0;
    
    if(fGetMPUCard() == TRUE){
        stPinGetPara.Control.NULLPIN = FALSE;
    } else {
        stPinGetPara.Control.NULLPIN = TRUE;
    }
    
    stPinGetPara.PINOutput.EncryptedBlockLength = 8;
    stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
    stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
    stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
    stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

    //CTOS_KBDBufFlush();//cleare key buffer -commented to resolve 04-02-2020 issue below?

//(1)All asked pin number card types(MPU-UPI co-brand card onus/offus,MPU onus/offus and Pure UPI onus/offus)are masking asterisk(*)
//at inputing PIN Number.So we can't see how much pin digits in terminal UI.
//04-02-2020
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
    
    vdDebug_LogPrintf("**Online PIN[%d]*PINBlock[%s]*",ret,stPinGetPara.PINOutput.pEncryptedBlock);
    
    DebugAddHEX("szPINBlock=",stPinGetPara.PINOutput.pEncryptedBlock,8);
    
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
            memset(card_holder_pin,0,sizeof(card_holder_pin));
			srTransRec.byPINEntryCapability = 2;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
            return ret;
        }
    }

	//srTransRec.byPINEntryCapability = 1;
		
    if(stPinGetPara.PINOutput.EncryptedBlockLength != 8)
    {
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        vdDebug_LogPrintf("inCTOS_inGetDefaultIPPPin:card_holder_pin[%s]",card_holder_pin);
    }
    else
    {
        wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
        vdDebug_LogPrintf("inCTOS_inGetDefaultIPPPin:card_holder_pin[%s]",card_holder_pin);
    }
    return d_OK;
}

int inCTOS_inGetNewIPPPin(void)
{
    CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

    BYTE str[40],key;
    USHORT ret;
	char szPINKey[16+1];
	BYTE   EMVtagVal[64];
    USHORT EMVtagLen;

	CTOS_KMS2Init();


    vdDebug_LogPrintf("*** inCTOS_inGetNewIPPPin byEntryMode:%d byTransType:%d ***", srTransRec.byEntryMode, srTransRec.byTransType);

	vdDebug_LogPrintf("inCTOS_inGetNewIPPPin....");

    DebugAddSTR("inCTOS_inGetNewIPPPin","Processing...",20);
    vdDebug_LogPrintf("*** inCTOS_inGetNewIPPPin HDTid = %d",srTransRec.HDTid);

	
    inDCTRead(srTransRec.HDTid);
	//inDCTRead(17); -- original working

	vdCTOS_PinEntryScreenDisplay(ENTER_NEW_PIN);

	strDCT.inMinPINDigit = 4;
	strDCT.inMaxPINDigit = 12;

#if 1
	if((srTransRec.HDTid == 7) || (srTransRec.HDTid == 17))// get TPK for UPI cards
	{	
		strDCT.usKeySet = MPU_PIK_KEYSET;
		strDCT.usKeyIndex = MPU_PIK_KEYIDX; // CipherKeyIndex/TPK index 0014
	}
	else
	{
		strDCT.usKeySet = CBB_TPK_KEYSET;
		#ifdef CBB_FIN_ROUTING
			if(srTransRec.HDTid == 23){
				strDCT.usKeySet = MMK_FIN_MAC_KEYSET;
				strDCT.usKeyIndex = MMK_FIN_MAC_KEYIDX;
			}
			else if (srTransRec.HDTid == 22){
				strDCT.usKeySet = USD_FIN_MAC_KEYSET;
				strDCT.usKeyIndex = USD_FIN_MAC_KEYIDX;
			}
			else
			{	
				strDCT.usKeySet = CBB_TPK_KEYSET;
				strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
			}
		#else		
			strDCT.usKeySet = CBB_TPK_KEYSET;
			strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
		#endif

	}
#endif



	vdDebug_LogPrintf("strDCT.usKeySet[%04X]", strDCT.usKeySet);
    vdDebug_LogPrintf("strDCT.usKeyIndex[%04X]", strDCT.usKeyIndex);

	vdDebug_LogPrintf("inCTOS_inGetNewIPPPin T9F34 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
	
    //CTOS_LCDTClearDisplay();
	if (strTCT.byPinPadMode != 1)
	{
		//#ifdef QUICKPASS
		#if 0
        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        {
        	#if 1 // for testing, 6244 1902 (UPI) should support pin entry if hostid MPU (17, 18 or 19) 09072020
			if(srTransRec.HDTid == 7)
			{
            	EMVtagLen = 3;
            	memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
            	if ((EMVtagVal[0] != 0x02) && (EMVtagVal[0] != 0x42))
            	{
                	return d_OK;
            	}
			}
			#endif
            
            if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.HDTid == 7) /*Host 7 - UPI Finexus*/
                return d_OK;
        }
		
        vduiClearBelow(2);
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
        vdDebug_LogPrintf("*** HDTid 2 = %d",srTransRec.HDTid);
        vdDebug_LogPrintf("strDCT.szDisplayLine1 [%s]", strDCT.szDisplayLine1);
        vdDebug_LogPrintf("strDCT.szDisplayLine2 [%s]", strDCT.szDisplayLine2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
		#else
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));
		
		#endif
	}
	else
	{
		displaybmpEx(0, 0, "Enterpin.bmp");
	}
	
	if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3))
	{
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "Please key in PINPAD");
		
		memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
		stOLPinParam.HDTid = srTransRec.HDTid;
		stOLPinParam.ushKeySet = strDCT.usKeySet;
		stOLPinParam.ushKeyIdx = strDCT.usKeyIndex;
		strcpy(stOLPinParam.szPAN,srTransRec.szPAN);
		memcpy(stOLPinParam.szPINKey,strDCT.szPINKey,16);
                DebugAddHEX("PIN KEY", stOLPinParam.szPINKey, 16);
		ret = inCTOSS_EXTGetIPPPin();
		
		memset(card_holder_pin,0,sizeof(card_holder_pin));
		if (ret == d_OK)
		{
			if (stOLPinParam.inPINEntered == 2)
			{
				memcpy(srTransRec.szNewPINBlock,stOLPinParam.szNewPINBlock,8);
				wub_hex_2_str(srTransRec.szNewPINBlock,card_holder_pin,8);
			}
			ret = stOLPinParam.ushKeySet;
			
		}
		else
		{
			if (ret != d_NO)
				ret = stOLPinParam.ushKeySet;
		}
		
		vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
		return ret;
	}
    
    memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
    stPinGetPara.Version = 0x02;
	#if 0
    stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	if (strDCT.inPIN3des == 0)//for des pin
	{
		memset(szPINKey,0x00,sizeof(szPINKey));
		memcpy(szPINKey,strDCT.szPINKey,8);
		memcpy(szPINKey+8,strDCT.szPINKey,8);
		stPinGetPara.Protection.pSK = szPINKey;
	}
	else
    	stPinGetPara.Protection.pSK = strDCT.szPINKey;
	#endif

	stPinGetPara.Protection.SK_Length = 0x00;
	stPinGetPara.Protection.pSK = NULL;
    
    stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
    stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
    stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
    vdDebug_LogPrintf("**Min[%d]Max[%d]**",strDCT.inMinPINDigit,strDCT.inMaxPINDigit);

    stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
    stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
    
//    stPinGetPara.Protection.CipherKeyIndex = CUP_TMK_KEY_INDEX;
//    stPinGetPara.Protection.CipherKeySet = CUP_TMK_KEY_SET;
    
    stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
    stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
    stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
    stPinGetPara.Control.Timeout = 0;
    
    if(fGetMPUCard() == TRUE){
        stPinGetPara.Control.NULLPIN = FALSE;
    } else {
        stPinGetPara.Control.NULLPIN = TRUE;
    }
    
    stPinGetPara.PINOutput.EncryptedBlockLength = 8;
    stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szNewPINBlock;
    stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
    stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
    stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

    //CTOS_KBDBufFlush();//cleare key buffer -commented to resolve 04-02-2020 issue below?

//(1)All asked pin number card types(MPU-UPI co-brand card onus/offus,MPU onus/offus and Pure UPI onus/offus)are masking asterisk(*)
//at inputing PIN Number.So we can't see how much pin digits in terminal UI.
//04-02-2020
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
    
    vdDebug_LogPrintf("**Online PIN[%d]*PINBlock[%s]*",ret,stPinGetPara.PINOutput.pEncryptedBlock);
    
    DebugAddHEX("szPINBlock=",stPinGetPara.PINOutput.pEncryptedBlock,8);
    
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
            memset(card_holder_pin,0,sizeof(card_holder_pin));
			srTransRec.byPINEntryCapability = 2;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
            return ret;
        }
    }

	//srTransRec.byPINEntryCapability = 1;
		
    if(stPinGetPara.PINOutput.EncryptedBlockLength != 8)
    {
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        vdDebug_LogPrintf("inCTOS_inGetNewIPPPin:card_holder_pin[%s]",card_holder_pin);
    }
    else
    {
        wub_hex_2_str(srTransRec.szNewPINBlock,card_holder_pin,8);
        vdDebug_LogPrintf("inCTOS_inGetNewIPPPin:card_holder_pin[%s]",card_holder_pin);
    }
    return d_OK;
}


int inCTOS_inGetVerifyIPPPin(void)
{
    CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

    BYTE str[40],key;
    USHORT ret;
	char szPINKey[16+1];
	BYTE   EMVtagVal[64];
    USHORT EMVtagLen;

	CTOS_KMS2Init();


    vdDebug_LogPrintf("*** inCTOS_inGetVerifyIPPPin byEntryMode:%d byTransType:%d ***", srTransRec.byEntryMode, srTransRec.byTransType);

	vdDebug_LogPrintf("inCTOS_inGetVerifyIPPPin....");

    DebugAddSTR("inCTOS_inGetVerifyIPPPin","Processing...",20);
    vdDebug_LogPrintf("*** inCTOS_inGetVerifyIPPPin HDTid = %d",srTransRec.HDTid);

	
    inDCTRead(srTransRec.HDTid);
	//inDCTRead(17); original working

	vdCTOS_PinEntryScreenDisplay(ENTER_VERIFY_PIN);

	strDCT.inMinPINDigit = 4;
	strDCT.inMaxPINDigit = 12;

#if 1
		if((srTransRec.HDTid == 7) || (srTransRec.HDTid == 17))// get TPK for UPI cards
		{	
			strDCT.usKeySet = MPU_PIK_KEYSET;
			strDCT.usKeyIndex = MPU_PIK_KEYIDX; // CipherKeyIndex/TPK index 0014
		}
		else
		{
			#ifdef CBB_FIN_ROUTING
				if(srTransRec.HDTid == 23){
					strDCT.usKeySet = MMK_FIN_MAC_KEYSET;
					strDCT.usKeyIndex = MMK_FIN_MAC_KEYIDX;
				}
				else if (srTransRec.HDTid == 22){
					strDCT.usKeySet = USD_FIN_MAC_KEYSET;
					strDCT.usKeyIndex = USD_FIN_MAC_KEYIDX;
				}
				else
				{	
					strDCT.usKeySet = CBB_TPK_KEYSET;
					strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
				}
			#else		
				strDCT.usKeySet = CBB_TPK_KEYSET;
				strDCT.usKeyIndex = CBB_TPK_KEYIDX; // CipherKeyIndex/TPK index 0006
			#endif

		}
#endif


	vdDebug_LogPrintf("strDCT.usKeySet[%04X]", strDCT.usKeySet);
    vdDebug_LogPrintf("strDCT.usKeyIndex[%04X]", strDCT.usKeyIndex);

	vdDebug_LogPrintf("inCTOS_inGetVerifyIPPPin T9F34 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
	
    //CTOS_LCDTClearDisplay();
	if (strTCT.byPinPadMode != 1)
	{
		//#ifdef QUICKPASS
		#if 0
        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        {
        	#if 1 // for testing, 6244 1902 (UPI) should support pin entry if hostid MPU (17, 18 or 19) 09072020
			if(srTransRec.HDTid == 7)
			{
            	EMVtagLen = 3;
            	memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
            	if ((EMVtagVal[0] != 0x02) && (EMVtagVal[0] != 0x42))
            	{
                	return d_OK;
            	}
			}
			#endif
            
            if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.HDTid == 7) /*Host 7 - UPI Finexus*/
                return d_OK;
        }
		
        vduiClearBelow(2);
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
        vdDebug_LogPrintf("*** HDTid 2 = %d",srTransRec.HDTid);
        vdDebug_LogPrintf("strDCT.szDisplayLine1 [%s]", strDCT.szDisplayLine1);
        vdDebug_LogPrintf("strDCT.szDisplayLine2 [%s]", strDCT.szDisplayLine2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
		#else
		
        vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
        vdDispTransTitle(srTransRec.byTransType);
        memset(card_holder_pin,0,sizeof(card_holder_pin));
		#endif
	}
	else
	{
		displaybmpEx(0, 0, "Enterpin.bmp");
	}
	
	if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3))
	{
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "Please key in PINPAD");
		
		memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
		stOLPinParam.HDTid = srTransRec.HDTid;
		stOLPinParam.ushKeySet = strDCT.usKeySet;
		stOLPinParam.ushKeyIdx = strDCT.usKeyIndex;
		strcpy(stOLPinParam.szPAN,srTransRec.szPAN);
		memcpy(stOLPinParam.szPINKey,strDCT.szPINKey,16);
                DebugAddHEX("PIN KEY", stOLPinParam.szPINKey, 16);
		ret = inCTOSS_EXTGetIPPPin();
		
		memset(card_holder_pin,0,sizeof(card_holder_pin));
		if (ret == d_OK)
		{
			if (stOLPinParam.inPINEntered == 2)
			{
				memcpy(srTransRec.szVerifyNewPINBlock,stOLPinParam.szVerifyNewPINBlock,8);
				wub_hex_2_str(srTransRec.szVerifyNewPINBlock,card_holder_pin,8);
			}
			ret = stOLPinParam.ushKeySet;
			
		}
		else
		{
			if (ret != d_NO)
				ret = stOLPinParam.ushKeySet;
		}
		
		vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
		return ret;
	}
    
    memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
    stPinGetPara.Version = 0x02;
	#if 0
    stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	if (strDCT.inPIN3des == 0)//for des pin
	{
		memset(szPINKey,0x00,sizeof(szPINKey));
		memcpy(szPINKey,strDCT.szPINKey,8);
		memcpy(szPINKey+8,strDCT.szPINKey,8);
		stPinGetPara.Protection.pSK = szPINKey;
	}
	else
    	stPinGetPara.Protection.pSK = strDCT.szPINKey;
	#endif

	stPinGetPara.Protection.SK_Length = 0x00;
	stPinGetPara.Protection.pSK = NULL;
    
    stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
    stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
    stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
    vdDebug_LogPrintf("**Min[%d]Max[%d]**",strDCT.inMinPINDigit,strDCT.inMaxPINDigit);

    stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
    stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
    
//    stPinGetPara.Protection.CipherKeyIndex = CUP_TMK_KEY_INDEX;
//    stPinGetPara.Protection.CipherKeySet = CUP_TMK_KEY_SET;
    
    stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
    stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
    stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
    stPinGetPara.Control.Timeout = 0;
    
    if(fGetMPUCard() == TRUE){
        stPinGetPara.Control.NULLPIN = FALSE;
    } else {
        stPinGetPara.Control.NULLPIN = TRUE;
    }
    
    stPinGetPara.PINOutput.EncryptedBlockLength = 8;
    stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szVerifyNewPINBlock;
    stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
    stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
    stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

    //CTOS_KBDBufFlush();//cleare key buffer -commented to resolve 04-02-2020 issue below?

//(1)All asked pin number card types(MPU-UPI co-brand card onus/offus,MPU onus/offus and Pure UPI onus/offus)are masking asterisk(*)
//at inputing PIN Number.So we can't see how much pin digits in terminal UI.
//04-02-2020
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
    
    vdDebug_LogPrintf("**Online PIN[%d]*PINBlock[%s]*",ret,stPinGetPara.PINOutput.pEncryptedBlock);
    
    DebugAddHEX("szPINBlock=",stPinGetPara.PINOutput.pEncryptedBlock,8);
    
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
            memset(card_holder_pin,0,sizeof(card_holder_pin));
			srTransRec.byPINEntryCapability = 2;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
            return ret;
        }
    }

	srTransRec.byPINEntryCapability = 1;
		
    if(stPinGetPara.PINOutput.EncryptedBlockLength != 8)
    {
        memset(card_holder_pin,0,sizeof(card_holder_pin));
        
        vdDebug_LogPrintf("inCTOS_inGetVerifyIPPPin:card_holder_pin[%s]",card_holder_pin);
    }
    else
    {
        wub_hex_2_str(srTransRec.szVerifyNewPINBlock,card_holder_pin,8);
        vdDebug_LogPrintf("inCTOS_inGetVerifyIPPPin:card_holder_pin[%s]",card_holder_pin);
    }
    return d_OK;
}
#endif

#if 0
int inCBGetChangePIN_With_3DESDUKPT(void)
{
	CTOS_KMS2PINGET_PARA stPinGetPara;
	USHORT ret;
	BYTE str[17];
	BYTE key;
	BYTE PINBlock[16];
	BYTE *pCipherKey;
	BYTE CipherKeyLength;
	BYTE DecipherPINBlock[16];
	//BYTE ExpectPINBlock[16];
	BYTE ksn[20];
	BYTE ksn_Len =  10;
	int inRet;
    BYTE szNewPINBlock[8+1], szConfirmNewPINBlock[8+1];
	
	DebugAddSTR("GetPIN_With_3DESDUKPT","Processing...",20);
	CTOS_KMS2Init();
	ginPinByPass = 0;
	gGetPINStatus = GET_PIN_NO_REQ;

	//inDCTRead(srTransRec.HDTid);
	inDCTRead(7);

	vdDebug_LogPrintf("inDCTRead IN [%ld] [%ld] [%ld]", srTransRec.HDTid, strDCT.usKeyIndex, strDCT.usKeySet);

// patrick test key 20150706 start
	strDCT.usKeySet = 0xC000;
	strDCT.usKeyIndex = 0x0005;
	strDCT.inMinPINDigit = 4;
	strDCT.inMaxPINDigit = 12;

    inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);
    if (inRet != d_OK)
        return(inRet);

	//vdCTOSS_GetKEYInfo(strDCT.usKeySet, strDCT.usKeyIndex);

	CTOS_LCDTClearDisplay();
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);

    if (strTCT.byPinPadMode != 1)
    {
        vdDispTransTitle(srTransRec.byTransType);
        strcpy(strDCT.szDisplayLine1, "PLEASE ENTER");
        strcpy(strDCT.szDisplayLine2, "ONLINE PIN");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
    }
    else
    {
        displaybmpEx(0, 0, "Enterpin.bmp");
    }
    //CTOS_Delay(1000);

	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA));
	stPinGetPara.Version = 0x01;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
	
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.Protection.SK_Length = 0;
	
    vdDebug_LogPrintf("srTransRec.baChipData: %s", srTransRec.baChipData);
	//vdDebug_LogPrintf("szDRIVERPAN: %s", szDRIVERPAN);
	vdDebug_LogPrintf("srTransRec.szPAN: %s", srTransRec.szPAN);
	
	//vdDebug_LogPrintf("len szDRIVERPAN: %d", strlen(szDRIVERPAN));
	vdDebug_LogPrintf("len srTransRec.szPAN: %d", strlen(srTransRec.szPAN));
/*	
	if (srTransRec.baChipData[3] == '1')//dual card pin block always come from driver card
	{
		stPinGetPara.AdditionalData.InLength = strlen(szDRIVERPAN);
		stPinGetPara.AdditionalData.pInData = (BYTE*)szDRIVERPAN;
  
	}
	else//single card
*/	
	{	
		stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
		stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
	}

	vdDebug_LogPrintf("stPinGetPara.AdditionalData.InLength: %d", stPinGetPara.AdditionalData.InLength);
	vdDebug_LogPrintf("stPinGetPara.AdditionalData.pInData: %s", stPinGetPara.AdditionalData.pInData);
	
	stPinGetPara.DUKPT_PARA.IsUseCurrentKey = FALSE;

    ret = CTOS_KMS2DUKPTGetKSN(stPinGetPara.Protection.CipherKeySet, stPinGetPara.Protection.CipherKeyIndex, ksn, &ksn_Len);
    vdDebug_LogPrintf("CTOS_KMS2DUKPTGetKSN[%d]", ret);
    DebugAddHEX("NOT RESET KSN YET", ksn, 10);

	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;//PINBlock; -- fix for issue DE 52 has no value
	stPinGetPara.Control.Timeout = 20;
	stPinGetPara.Control.AsteriskPositionX = 2;
	stPinGetPara.Control.AsteriskPositionY = 7;
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.Control.piTestCancel = NULL;

    ksn_Len = 10;
    CTOS_KBDBufFlush();//cleare key buffer	

    memcpy(srTransRec.szKSN, ksn, 10);

	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);

    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara); /*enter CURRENT PIN*/
    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
			memset(srTransRec.szKSN, 0x00, sizeof(srTransRec.szKSN));
			ginPinByPass = 1;
			gGetPINStatus = GET_PIN_BYPASS;	
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
			gGetPINStatus = GET_PIN_FAILED;		
			vdDebug_LogPrintf("srTransRec.szPINBlock[%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]", srTransRec.szPINBlock[0], srTransRec.szPINBlock[1], srTransRec.szPINBlock[2], srTransRec.szPINBlock[3], srTransRec.szPINBlock[4], srTransRec.szPINBlock[5], srTransRec.szPINBlock[6], srTransRec.szPINBlock[7]);
            return ret;
        }
    }
	
    //DebugAddHEX("1.srTransRec.szKSN",srTransRec.szKSN,10);
	//DebugAddHEX("1.srTransRec.szPINBlock",srTransRec.szPINBlock,8);
/*************************************************************************************************************************/


	
	CTOS_LCDTClearDisplay();
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
	memset(szNewPINBlock, 0, sizeof(szNewPINBlock));
    stPinGetPara.PINOutput.pEncryptedBlock = szNewPINBlock;//PINBlock; -- fix for issue DE 52 has no value
    
    if (strTCT.byPinPadMode != 1)
    {
        vdDispTransTitle(srTransRec.byTransType);
        vdDebug_LogPrintf("szDisplayLine1 [%s] szDisplayLine2[%s]", strDCT.szDisplayLine1, strDCT.szDisplayLine2);
        strcpy(strDCT.szDisplayLine1, "PLEASE ENTER");
        strcpy(strDCT.szDisplayLine2, "NEW PIN");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
    }
    else
    {
        displaybmpEx(0, 0, "Enterpin.bmp");
    }
	//CTOS_Delay(5000);
	
	stPinGetPara.DUKPT_PARA.IsUseCurrentKey = TRUE;
    ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara); /*enter NEW PIN*/

	
	vdDebug_LogPrintf("inCBGetChangePIN_With_3DESDUKPT :ret %d", ret);

    if(ret != d_OK)
    {
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
            CTOS_Delay(300);
			memset(srTransRec.szKSN, 0x00, sizeof(srTransRec.szKSN));
			ginPinByPass = 1;
			gGetPINStatus = GET_PIN_BYPASS;	
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else
        {
            sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
            vdDisplayErrorMsg(1, 8, str);
			gGetPINStatus = GET_PIN_FAILED;		
			vdDebug_LogPrintf("srTransRec.szPINBlock[%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]", srTransRec.szPINBlock[0], srTransRec.szPINBlock[1], srTransRec.szPINBlock[2], srTransRec.szPINBlock[3], srTransRec.szPINBlock[4], srTransRec.szPINBlock[5], srTransRec.szPINBlock[6], srTransRec.szPINBlock[7]);
            return ret;
        }
    }

    //DebugAddHEX("2.srTransRec.szKSN",srTransRec.szKSN,10);
	//DebugAddHEX("2.srTransRec.szPINBlock",srTransRec.szPINBlock,8);
    //DebugAddHEX("szNewPINBlock",szNewPINBlock,8);
	
    while(1)
    {
		CTOS_LCDTClearDisplay();
		CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
		memset(szConfirmNewPINBlock, 0, sizeof(szConfirmNewPINBlock));
		stPinGetPara.PINOutput.pEncryptedBlock = szConfirmNewPINBlock;//PINBlock; -- fix for issue DE 52 has no value
        if (strTCT.byPinPadMode != 1)
        {
            vdDispTransTitle(srTransRec.byTransType);
            strcpy(strDCT.szDisplayLine1, "PLEASE ENTER");
            strcpy(strDCT.szDisplayLine2, "CONFIRM PIN");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
        }
        else
        {
            displaybmpEx(0, 0, "Enterpin.bmp");
        }
        //CTOS_Delay(1000);
        
        stPinGetPara.DUKPT_PARA.IsUseCurrentKey = TRUE;
        ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara); /*enter CONFIRM PIN*/
        if(ret != d_OK)
        {
            if(ret == d_KMS2_GET_PIN_NULL_PIN)
            {
                CTOS_LCDTPrintXY(1, 8, "PIN BY PASSED");
                CTOS_Delay(300);
                memset(srTransRec.szKSN, 0x00, sizeof(srTransRec.szKSN));
                ginPinByPass = 1;
                gGetPINStatus = GET_PIN_BYPASS; 
                return d_KMS2_GET_PIN_NULL_PIN;
            }
            else
            {
                sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
                vdDisplayErrorMsg(1, 8, str);
                gGetPINStatus = GET_PIN_FAILED; 	
                vdDebug_LogPrintf("srTransRec.szPINBlock[%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]", srTransRec.szPINBlock[0], srTransRec.szPINBlock[1], srTransRec.szPINBlock[2], srTransRec.szPINBlock[3], srTransRec.szPINBlock[4], srTransRec.szPINBlock[5], srTransRec.szPINBlock[6], srTransRec.szPINBlock[7]);
                return ret;
            }
        }
        else
        {
            if(memcmp(szNewPINBlock, szConfirmNewPINBlock, 8) == 0)
            {
				DebugAddHEX("szConfirmNewPINBlock",szConfirmNewPINBlock,8);
				memcpy(srTransRec.szNewPINBlock, szConfirmNewPINBlock, 8);
				break;
            }
			else
			{
				vdDisplayErrorMsg(1, 8, "PINCODE MISMATCH");
			}
        }
    }
	
    DebugAddHEX("Final.srTransRec.szKSN",srTransRec.szKSN,10);
	DebugAddHEX("Final.srTransRec.szPINBlock",srTransRec.szPINBlock,8);
    //DebugAddHEX("szConfirmNewPINBlock",szConfirmNewPINBlock,8);
    DebugAddHEX("Final.srTransRec.szNewPINBlock",srTransRec.szNewPINBlock,8);
	
	inCTOSS_ResetDUKPTKSN(ksn);	
    DebugAddHEX("ResetDUKPTKSN Current KSN", ksn, 10);
	
    if(stPinGetPara.PINOutput.EncryptedBlockLength != 8)
    {        
//        vdDebug_LogPrintf("card_holder_pin[%s]","123");
    }
    else
    {
//        wub_hex_2_str(srTransRec.szPINBlock,card_holder_pin,8);
//        vdDebug_LogPrintf("card_holder_pin[%s]",card_holder_pin);
    }

	gGetPINStatus = GET_PIN_ENTERED;
	
	vdDebug_LogPrintf("srTransRec.szPINBlock[%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]", srTransRec.szPINBlock[0], srTransRec.szPINBlock[1], srTransRec.szPINBlock[2], srTransRec.szPINBlock[3], srTransRec.szPINBlock[4], srTransRec.szPINBlock[5], srTransRec.szPINBlock[6], srTransRec.szPINBlock[7]);

	 vduiClearBelow(1);
	   
    return d_OK;
}
#endif

#if 0
int inCTOS_inGetDefaultIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

	BYTE str[40],key;
	USHORT ret;
	int inRecNo;
	char szDebug[40 + 1]={0};
	
	vdDebug_LogPrintf("::inCTOS_inGetDefaultIPPPin::DCTid[%d]", srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid);

	vdCTOS_PinEntryScreenDisplay(ENTER_DEFAULT_PIN);
	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
	stPinGetPara.Version = 0x02;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = 4;//strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = 6;//strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
//add start        
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
    stPinGetPara.Protection.pSK = strDCT.szPINKey;
//add end        
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	stPinGetPara.Control.Timeout = 0;
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock; //default, current pin block for de52 value
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE); // Fix for undisplay of * during pin -- sidumili
	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
	
	if(ret != d_OK){
		
		return d_NO;
	}

	vdDebug_LogPrintf("inCTOS_inGetDefaultIPPPin | szPINBlock[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", (unsigned char)srTransRec.szPINBlock[0], (unsigned char)srTransRec.szPINBlock[1], (unsigned char)srTransRec.szPINBlock[2], (unsigned char)srTransRec.szPINBlock[3], (unsigned char)srTransRec.szPINBlock[4], (unsigned char)srTransRec.szPINBlock[5], (unsigned char)srTransRec.szPINBlock[6], (unsigned char)srTransRec.szPINBlock[7]);

	return d_OK;
}

int inCTOS_inGetNewIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

	BYTE str[40],key;
	USHORT ret;
	int inRecNo;
	char szDebug[40 + 1]={0};

	//inDCTRead(srTransRec.HDTid);
	
	vdDebug_LogPrintf("::inCTOS_inGetNewIPPPin::DCTid[%d]", srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid);

	vdCTOS_PinEntryScreenDisplay(ENTER_NEW_PIN);
	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
	stPinGetPara.Version = 0x02;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = 4;//strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = 6;//strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
//add start        
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
    stPinGetPara.Protection.pSK = strDCT.szPINKey;
//add end        
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	stPinGetPara.Control.Timeout = 0;
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szNewPINBlock; // new pin block , for validation
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE); // Fix for undisplay of * during pin -- sidumili
	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
	
	if(ret != d_OK){
		
		return d_NO;
	}

	vdDebug_LogPrintf("inCTOS_inGetNewIPPPin | szNewPINBlock[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", (unsigned char)srTransRec.szNewPINBlock[0], (unsigned char)srTransRec.szNewPINBlock[1], (unsigned char)srTransRec.szNewPINBlock[2], (unsigned char)srTransRec.szNewPINBlock[3], (unsigned char)srTransRec.szNewPINBlock[4], (unsigned char)srTransRec.szNewPINBlock[5], (unsigned char)srTransRec.szNewPINBlock[6], (unsigned char)srTransRec.szNewPINBlock[7]);

	return d_OK;
}


int inCTOS_inGetVerifyIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_2 stPinGetPara;

	BYTE str[40],key;
	USHORT ret;
	int inRecNo;
	char szDebug[40 + 1]={0};
	
	//inDCTRead(srTransRec.HDTid);
	
	vdDebug_LogPrintf("::inCTOS_inGetVerifyIPPPin::DCTid[%d]", srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid);
	
	vdCTOS_PinEntryScreenDisplay(ENTER_VERIFY_PIN);
	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_2));
	stPinGetPara.Version = 0x02;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = 4;//strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = 6;//strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
//add start        
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
    stPinGetPara.Protection.pSK = strDCT.szPINKey;
//add end        
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	stPinGetPara.Control.Timeout = 0;
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	//stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szVerifyNewPINBlock; // verified pin block for de60 value
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE); // Fix for undisplay of * during pin -- sidumili
	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);
	
	if(ret != d_OK){
		
		return d_NO;
	}
	
	vdDebug_LogPrintf("inCTOS_inGetVerifyIPPPin | szVerifyNewPINBlock[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", (unsigned char)srTransRec.szPINBlock[0], (unsigned char)srTransRec.szPINBlock[1], (unsigned char)srTransRec.szPINBlock[2], (unsigned char)srTransRec.szPINBlock[3], (unsigned char)srTransRec.szPINBlock[4], (unsigned char)srTransRec.szPINBlock[5], (unsigned char)srTransRec.szPINBlock[6], (unsigned char)srTransRec.szPINBlock[7]);

	return d_OK;
}

#endif

#endif


