/*******************************************************************************

 *******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "..\Includes\Wub_lib.h"

#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\Display.h"

#include "..\Includes\V5IsoFunc.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\Trans.h"   


#include "..\Includes\CTOSInput.h"


#include "..\debug\debug.h"
#include "..\Accum\Accum.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\accum\accum.h"
#include "..\Includes\POSSetting.h"
#include "..\Debug\Debug.h"
#include "..\filemodule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\CTOSInput.h"
#include "..\Ctls\PosWave.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"


#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"
#include "../Ctls/POSCtls.h"
#include "..\Includes\POSIpp.h"

#define d_NOT_RECORD            102

#define d_GETPIN_TIMEOUT        6000
#define d_DUPLICATE_INVOICE     0x0080

// patrick add code 20131208
extern BOOL fECRTxnFlg;

BOOL fIsShowingQR = FALSE;
extern char szECRTipAmount[6+1];



int inCTOS_TUPGetTxnBaseAmount(void) {
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4 + 1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    ULONG ulAmount = 0L;
    int len;
    USHORT usLen;
    int ret;
    BYTE byFirstKey = 0;
	int inCSTinCurrencyIndex = 0;


vdDebug_LogPrintf("inCTOS_TUPGetTxnBaseAmount byTransType[%d]CDTid[%d]HDTid[%d] IITid[%d] strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]fGetMPUTrans[%d]fGetMPUCard[%d]", 
	srTransRec.byTransType, srTransRec.CDTid, srTransRec.HDTid, srTransRec.IITid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, fGetMPUTrans(), fGetMPUCard());


    if (VS_FALSE == fGetMPUTrans()) {
        //        inHDTRead(1);
        inHDTRead(srTransRec.HDTid);
        inCSTRead(strHDT.inCurrencyIdx);
    }

    if (chGetIdleEventSC_MSR() == 1) {
        if (fGetMPUTrans() == TRUE || fGetMPUCard() == TRUE) {
            if (srTransRec.HDTid == 17)
                inCSTRead(1);
        }
    }

    if (srTransRec.fIsInstallment == TRUE) {
        inCSTRead(1);
    }

    #ifdef TOPUP_RELOAD
    if(srTransRec.byTransType == TOPUP || srTransRec.byTransType == RELOAD)
    {
		inCSTRead(20);
    }
    #endif

#ifdef CBPAY_DV
	if(srTransRec.HDTid == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
	{
		inCSTRead(1);
	}
#endif

//http://118.201.48.214:8080/issues/75.41 #1 - "1)onus MPU-UPI currency issue is okey with latest patch file. 
//But we are getting another issue is Pure MPU cards 9503 should be MMK on USD currency setup terminal. Please kindly compare it on S1F2 latest version."
//Request # 4) MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus should be always MMK
#ifdef CBB_FIN_ROUTING

		vdDebug_LogPrintf(" inCTOS_TUPGetTxnBaseAmount srTransRec.CDTid =[%d]", srTransRec.CDTid); 			
		
		// on cancel, terminal should display original default currency. #2
		inCSTinCurrencyIndex = strCST.inCurrencyIndex;
		inCSTRead(strCST.inCurrencyIndex);	
#endif

vdDebug_LogPrintf("inCTOS_TUPGetTxnBaseAmount XXXX strCST.inCurrencyIndex [%d]", strCST.inCurrencyIndex);


#ifdef APP_AUTO_TEST
    if (inCTOS_GetAutoTestCnt() > 1) {
        vdDebug_LogPrintf("Auto Test ---> Amount : 20.00");
        memcpy(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x20\x00", 6);
        return d_OK;
    }
#endif

    if (fECRTxnFlg == 1) {
        if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) == 0) {
            vdSetErrorMessage("OUT OF RANGE");
            return (ST_ERROR);
        }
    }
	
    if (1 == inCTOSS_GetWaveTransType())
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;

    if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
        return d_OK;

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vduiLightOn();

    vdDispTransTitle(srTransRec.byTransType);

    if (CARD_ENTRY_MANUAL == srTransRec.byEntryMode) {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr + 3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 6, bDisplayStr);
    } else {
        if (1 != inCTOSS_GetWaveTransType())
            inCTOS_DisplayCardTitle(4, 5);
    }

    //    if(chGetFirstIdleKey() == 0x00)
    //        CTOS_KBDHit(&key);//clear key buffer

    //    if(strTCT.fEnableAmountIdle != TRUE){
    //        memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    //    CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    //    setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    //    }
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);

    memset(baAmount, 0x00, sizeof (baAmount));
    byFirstKey = 0x00;


    vdDebug_LogPrintf("strTCT.fEnableAmountIdle[%d]", strTCT.fEnableAmountIdle);

    if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
        if ((chGetFirstIdleKey() >= d_KBD_1) && (chGetFirstIdleKey() <= d_KBD_9)) {
            CTOS_KBDInKey(&key);
            byFirstKey = chGetFirstIdleKey();
        }
    }

    //        baAmount[0] = '5';
    //    ulAmount = 5;
    //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    vdDebug_LogPrintf("szCurSymbol=[%s]", strCST.szCurSymbol);
    if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0)
        //		key = InputAmountEx(1, 8, szDisplayBuf, 0, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
        key = InputAmountEx(1, 8, szDisplayBuf, 0, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    else
        //                key = InputAmountEx(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
        key = InputAmountEx(1, 8, szDisplayBuf, 2, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);



    if (d_OK == key) {
        vdDebug_LogPrintf("11baAmount=[%s]", baAmount);


        if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
            if (strlen(baAmount) > 10) {
                vdDispErrMsg("EXCEED INPUT AMOUNT");
                return d_NO;
            }
            len = strlen(baAmount);
            strcpy(&baAmount[len], "00");
        }
        vdDebug_LogPrintf("22baAmount=[%s]", baAmount);
        memset(szTemp, 0x00, sizeof (szTemp));
        //format amount 10+2 
        sprintf(szTemp, "%012.0f", atof(baAmount));
        wub_str_2_hex(szTemp, srTransRec.szBaseAmount, 12);
    }
    if (0xFF == key) {
        vdSetErrorMessage("TIME OUT");
        return d_NO;
    }

	// on cancel, terminal should display original default currency. #1
	#ifdef CBB_FIN_ROUTING
	if(key == d_USER_CANCEL){
		if(inCSTinCurrencyIndex == 2)
			inCSTRead(2);
	}
	#endif	

    vdDebug_LogPrintf("inCTOS_GetTxnBaseAmount end[%d][%d][%d]", srTransRec.HDTid, key, strCST.inCurrencyIndex);
	
    return key;
}


int inCTOS_CAVGetTxnBaseAmount(void) {
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4 + 1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    ULONG ulAmount = 0L;
    int len;
    USHORT usLen;
    int ret;
    BYTE byFirstKey = 0;
	int inCSTinCurrencyIndex = 0;


vdDebug_LogPrintf("inCTOS_CAVGetTxnBaseAmount byTransType[%d]CDTid[%d]HDTid[%d] IITid[%d] strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]fGetMPUTrans[%d]fGetMPUCard[%d]", 
	srTransRec.byTransType, srTransRec.CDTid, srTransRec.HDTid, srTransRec.IITid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, fGetMPUTrans(), fGetMPUCard());


    if (VS_FALSE == fGetMPUTrans()) {
        //        inHDTRead(1);
        inHDTRead(srTransRec.HDTid);
        inCSTRead(strHDT.inCurrencyIdx);
    }

    if (chGetIdleEventSC_MSR() == 1) {
        if (fGetMPUTrans() == TRUE || fGetMPUCard() == TRUE) {
            if (srTransRec.HDTid == 17)
                inCSTRead(1);
        }
    }

    if (srTransRec.fIsInstallment == TRUE) {
        inCSTRead(1);
    }

    #ifdef TOPUP_RELOAD
    if(srTransRec.byTransType == TOPUP || srTransRec.byTransType == RELOAD)
    {
		inCSTRead(20);
    }
    #endif

#ifdef CBPAY_DV
	if(srTransRec.HDTid == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
	{
		inCSTRead(1);
	}
#endif

//http://118.201.48.214:8080/issues/75.41 #1 - "1)onus MPU-UPI currency issue is okey with latest patch file. 
//But we are getting another issue is Pure MPU cards 9503 should be MMK on USD currency setup terminal. Please kindly compare it on S1F2 latest version."
//Request # 4) MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus should be always MMK
#ifdef CBB_FIN_ROUTING

		vdDebug_LogPrintf(" inCTOS_GetTxnBaseAmount srTransRec.CDTid =[%d]", srTransRec.CDTid); 			
		
		// on cancel, terminal should display original default currency. #2
		inCSTinCurrencyIndex = strCST.inCurrencyIndex;
		inCSTRead(strCST.inCurrencyIndex);	
#endif

vdDebug_LogPrintf("inCTOS_GetTxnBaseAmount XXXX strCST.inCurrencyIndex [%d]", strCST.inCurrencyIndex);


#ifdef APP_AUTO_TEST
    if (inCTOS_GetAutoTestCnt() > 1) {
        vdDebug_LogPrintf("Auto Test ---> Amount : 20.00");
        memcpy(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x20\x00", 6);
        return d_OK;
    }
#endif

    if (fECRTxnFlg == 1) {
        if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) == 0) {
            vdSetErrorMessage("OUT OF RANGE");
            return (ST_ERROR);
        }
    }
	
    if (1 == inCTOSS_GetWaveTransType())
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;

    if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
        return d_OK;

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vduiLightOn();

    vdDispTransTitle(srTransRec.byTransType);

    if (CARD_ENTRY_MANUAL == srTransRec.byEntryMode) {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr + 3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 6, bDisplayStr);
    } else {
        if (1 != inCTOSS_GetWaveTransType())
            inCTOS_DisplayCardTitle(4, 5);
    }

    //    if(chGetFirstIdleKey() == 0x00)
    //        CTOS_KBDHit(&key);//clear key buffer

    //    if(strTCT.fEnableAmountIdle != TRUE){
    //        memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    //    CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    //    setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    //    }
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);

    memset(baAmount, 0x00, sizeof (baAmount));
    byFirstKey = 0x00;


    vdDebug_LogPrintf("strTCT.fEnableAmountIdle[%d]", strTCT.fEnableAmountIdle);

    if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
        if ((chGetFirstIdleKey() >= d_KBD_1) && (chGetFirstIdleKey() <= d_KBD_9)) {
            CTOS_KBDInKey(&key);
            byFirstKey = chGetFirstIdleKey();
        }
    }

    //        baAmount[0] = '5';
    //    ulAmount = 5;
    //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    vdDebug_LogPrintf("szCurSymbol=[%s]", strCST.szCurSymbol);
    if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0)
        //		key = InputAmountEx(1, 8, szDisplayBuf, 0, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
        key = InputAmountEx(1, 8, szDisplayBuf, 0, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    else
        //                key = InputAmountEx(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
        key = InputAmountEx(1, 8, szDisplayBuf, 2, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);



    if (d_OK == key) {
        vdDebug_LogPrintf("11baAmount=[%s]", baAmount);


        if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
            if (strlen(baAmount) > 10) {
                vdDispErrMsg("EXCEED INPUT AMOUNT");
                return d_NO;
            }
            len = strlen(baAmount);
            strcpy(&baAmount[len], "00");
        }
        vdDebug_LogPrintf("22baAmount=[%s]", baAmount);
        memset(szTemp, 0x00, sizeof (szTemp));
        //format amount 10+2 
        sprintf(szTemp, "%012.0f", atof(baAmount));
        wub_str_2_hex(szTemp, srTransRec.szBaseAmount, 12);
    }
    if (0xFF == key) {
        vdSetErrorMessage("TIME OUT");
        return d_NO;
    }

	// on cancel, terminal should display original default currency. #1
	#ifdef CBB_FIN_ROUTING
	if(key == d_USER_CANCEL){
		if(inCSTinCurrencyIndex == 2)
			inCSTRead(2);
	}
	#endif	

    vdDebug_LogPrintf("inCTOS_GetTxnBaseAmount end[%d][%d][%d]", srTransRec.HDTid, key, strCST.inCurrencyIndex);
	
    return key;
}

int inCTOS_GetTxnBaseAmount(void) {
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4 + 1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    ULONG ulAmount = 0L;
    int len;
    USHORT usLen;
    int ret;
    BYTE byFirstKey = 0;
	int inCSTinCurrencyIndex = 0;


#ifdef MPUPIOnUsFlag

    vdDebug_LogPrintf("inCTOS_GetTxnBaseAmount byTransType[%d]CDTid[%d]HDTid[%d] IITid[%d] strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]fGetMPUTrans[%d]fGetMPUCard[%d]fGetMPUPIOnUs[%d]", 
		srTransRec.byTransType, srTransRec.CDTid, srTransRec.HDTid, srTransRec.IITid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, fGetMPUTrans(), fGetMPUCard(), fGetMPUPIOnUs());
#else
vdDebug_LogPrintf("inCTOS_GetTxnBaseAmount byTransType[%d]CDTid[%d]HDTid[%d] IITid[%d] strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]fGetMPUTrans[%d]fGetMPUCard[%d]", 
	srTransRec.byTransType, srTransRec.CDTid, srTransRec.HDTid, srTransRec.IITid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, fGetMPUTrans(), fGetMPUCard());

#endif

    if (VS_FALSE == fGetMPUTrans()) {
        //        inHDTRead(1);
        inHDTRead(srTransRec.HDTid);
        inCSTRead(strHDT.inCurrencyIdx);
    }

    if (chGetIdleEventSC_MSR() == 1) {
        if (fGetMPUTrans() == TRUE || fGetMPUCard() == TRUE) {
            if (srTransRec.HDTid == 17)
                inCSTRead(1);
        }
    }

    if (srTransRec.fIsInstallment == TRUE) {
        inCSTRead(1);
    }

    #ifdef TOPUP_RELOAD
    if(srTransRec.byTransType == TOPUP || srTransRec.byTransType == RELOAD)
    {
		inCSTRead(20);
    }
    #endif

#ifdef CBPAY_DV
	if(srTransRec.HDTid == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
	{
		inCSTRead(1);
	}
#endif

//http://118.201.48.214:8080/issues/75.41 #1 - "1)onus MPU-UPI currency issue is okey with latest patch file. 
//But we are getting another issue is Pure MPU cards 9503 should be MMK on USD currency setup terminal. Please kindly compare it on S1F2 latest version."
//Request # 4) MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus should be always MMK
#ifdef CBB_FIN_ROUTING
		#ifdef MPUPIOnUsFlag
		vdDebug_LogPrintf(" inCTOS_GetTxnBaseAmount srTransRec.CDTid =[%d] fGetMPUPIOnUs[%d]", srTransRec.CDTid, fGetMPUPIOnUs()); 	
		#else
		vdDebug_LogPrintf(" inCTOS_GetTxnBaseAmount srTransRec.CDTid =[%d]", srTransRec.CDTid); 			
		#endif
		
		// on cancel, terminal should display original default currency. #2
		inCSTinCurrencyIndex = strCST.inCurrencyIndex;


		//fix for case http://118.201.48.214:8080/issues/75.53 #1
		//1) We are getting currency issue with MPU-UPI offus card on USD setup terminal . MPU onus/offus, MPU-JCB onus/off, MPU-UPI offus should be always MMK.
				//fin-mmk				mpu-bpc 				installment
		if(srTransRec.HDTid == 23 || srTransRec.HDTid == 17){
			if(srTransRec.CDTid == 58 || srTransRec.CDTid == 62 || srTransRec.CDTid == 63 || srTransRec.CDTid == 64 || srTransRec.CDTid == 71)	// ADDRESSED NOTE #4
				inCSTRead( strCST.inCurrencyIndex);	
			else	
				inCSTRead(1);	
		}
		else			
			inCSTRead(strCST.inCurrencyIndex);	
#endif

vdDebug_LogPrintf("inCTOS_GetTxnBaseAmount XXXX strCST.inCurrencyIndex [%d]", strCST.inCurrencyIndex);


#ifdef APP_AUTO_TEST
    if (inCTOS_GetAutoTestCnt() > 1) {
        vdDebug_LogPrintf("Auto Test ---> Amount : 20.00");
        memcpy(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x20\x00", 6);
        return d_OK;
    }
#endif

    if (fECRTxnFlg == 1) {
        if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) == 0) {
            vdSetErrorMessage("OUT OF RANGE");
            return (ST_ERROR);
        }
    }
	
    if (1 == inCTOSS_GetWaveTransType())
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;

    if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
        return d_OK;

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vduiLightOn();

    vdDispTransTitle(srTransRec.byTransType);

    if (CARD_ENTRY_MANUAL == srTransRec.byEntryMode) {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr + 3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 6, bDisplayStr);
    } else {
        if (1 != inCTOSS_GetWaveTransType())
            inCTOS_DisplayCardTitle(4, 5);
    }

    //    if(chGetFirstIdleKey() == 0x00)
    //        CTOS_KBDHit(&key);//clear key buffer

    //    if(strTCT.fEnableAmountIdle != TRUE){
    //        memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    //    CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    //    setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    //    }
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);

    memset(baAmount, 0x00, sizeof (baAmount));
    byFirstKey = 0x00;


    vdDebug_LogPrintf("strTCT.fEnableAmountIdle[%d]", strTCT.fEnableAmountIdle);

    if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
        if ((chGetFirstIdleKey() >= d_KBD_1) && (chGetFirstIdleKey() <= d_KBD_9)) {
            CTOS_KBDInKey(&key);
            byFirstKey = chGetFirstIdleKey();
        }
    }

    //        baAmount[0] = '5';
    //    ulAmount = 5;
    //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    vdDebug_LogPrintf("szCurSymbol=[%s]", strCST.szCurSymbol);
    if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0)
        //		key = InputAmountEx(1, 8, szDisplayBuf, 0, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
        key = InputAmountEx(1, 8, szDisplayBuf, 0, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
    else
        //                key = InputAmountEx(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
        key = InputAmountEx(1, 8, szDisplayBuf, 2, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);



    if (d_OK == key) {
        vdDebug_LogPrintf("11baAmount=[%s]", baAmount);


        if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
            if (strlen(baAmount) > 10) {
                vdDispErrMsg("EXCEED INPUT AMOUNT");
                return d_NO;
            }
            len = strlen(baAmount);
            strcpy(&baAmount[len], "00");
        }
        vdDebug_LogPrintf("22baAmount=[%s]", baAmount);
        memset(szTemp, 0x00, sizeof (szTemp));
        //format amount 10+2 
        sprintf(szTemp, "%012.0f", atof(baAmount));
        wub_str_2_hex(szTemp, srTransRec.szBaseAmount, 12);
    }
    if (0xFF == key) {
        vdSetErrorMessage("TIME OUT");
        return d_NO;
    }

	// on cancel, terminal should display original default currency. #1
	#ifdef CBB_FIN_ROUTING
	if(key == d_USER_CANCEL){
		if(inCSTinCurrencyIndex == 2)
			inCSTRead(2);
	}
	#endif	

    vdDebug_LogPrintf("inCTOS_GetTxnBaseAmount end[%d][%d][%d]", srTransRec.HDTid, key, strCST.inCurrencyIndex);
	
    return key;
}

int inCTOS_GetTxnTipAmount(void) {
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE bBuf[4 + 1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    ULONG ulAmount = 0L;
    int len;

    DebugAddSTR("inCTOS_GetTxnTipAmount", "Processing...", 20);

    if (d_OK != inCTOS_CheckTipAllowd())
        return d_OK;

    if (1 == inCTOSS_GetWaveTransType())
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;

    memset(baBaseAmount, 0x00, sizeof (baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);

    do {
        CTOS_LCDTClearDisplay();
        //vduiClearBelow(2);
        vdDispTransTitle(srTransRec.byTransType);

        if (CARD_ENTRY_MANUAL == srTransRec.byEntryMode) {
            setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
            memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
            strcpy(bDisplayStr, srTransRec.szPAN);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 3, bDisplayStr);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

            memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
            memset(bBuf, 0x00, sizeof (bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
            memcpy(bDisplayStr, bBuf, 2);
            bDisplayStr[2] = '/';
            memset(bBuf, 0x00, sizeof (bBuf));
            wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
            memcpy(bDisplayStr + 3, bBuf, 2);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 6, bDisplayStr);
        } else {
            if (1 != inCTOSS_GetWaveTransType())
                inCTOS_DisplayCardTitle(4, 5);
        }

        CTOS_KBDHit(&key); //clear key buffer

        CTOS_LCDTPrintXY(1, 7, "TIP:");

        memset(baAmount, 0x00, sizeof (baAmount));
        //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        vdDebug_LogPrintf("szCurSymbol=[%s]", strCST.szCurSymbol);
        if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0)
            key = InputAmountEx(1, 8, szDisplayBuf, 0, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        else
            key = InputAmountEx(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if (d_OK == key) {

            if (0 == strlen(baAmount)) //by pass TIP
                return d_OK;

            vdDebug_LogPrintf("11baAmount=[%s]", baAmount);
            if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
                len = strlen(baAmount);
                strcpy(&baAmount[len], "00");
            }
            vdDebug_LogPrintf("22baAmount=[%s]", baAmount);
            memset(szTemp, 0x00, sizeof (szTemp));
            //format amount 10+2
            sprintf(szTemp, "%012.0f", atof(baAmount));
            if (strcmp(szTemp, baBaseAmount) > 0) {
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
                continue;
            }

            wub_str_2_hex(szTemp, srTransRec.szTipAmount, 12);

            break;
        } else if (d_USER_CANCEL == key) {
            clearLine(8);
            vdSetErrorMessage("USER CANCEL");
            clearLine(8);
            return d_NO;
        } else if (0xFF == key) {
            clearLine(8);
            vdSetErrorMessage("TIME OUT");
            clearLine(8);
            return d_NO;
        }


    } while (1);

    return key;
}

int inCTOS_GetTipAfjustAmount(void) {
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE baTipAmount[20];
    ULONG ulAmount = 0L;
    BYTE szAmtTmp2[16 + 1];
    int len;

    DebugAddSTR("inCTOS_GetTipAfjustAmount", "Processing...       ", 20);
    memset(baBaseAmount, 0x00, sizeof (baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(baTipAmount, 0x00, sizeof (baTipAmount));
    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    wub_hex_2_str(srTransRec.szTipAmount, baTipAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4);


    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    //format amount 10+2
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", baTipAmount, szAmtTmp2);
    //sprintf(szAmtTmp2, "%lu.%02lu", atol(baTipAmount)/100, atol(baTipAmount)%100);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ORG TIP");
    setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (strlen(szAmtTmp2) + 1)*2, 6, szAmtTmp2);

    CTOS_LCDTPrintXY(1, 7, "TIP");

    do {
        CTOS_KBDHit(&key); //clear key buffer

        memset(baAmount, 0x00, sizeof (baAmount));
        //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        vdDebug_LogPrintf("szCurSymbol=[%s]", strCST.szCurSymbol);
        if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0)
            key = InputAmountEx(1, 8, szDisplayBuf, 0, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        else
            key = InputAmountEx(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if (strlen(baAmount) > 0) {
            vdDebug_LogPrintf("11baAmount=[%s]", baAmount);
            if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
                len = strlen(baAmount);
                strcpy(&baAmount[len], "00");
            }
            vdDebug_LogPrintf("22baAmount=[%s]", baAmount);

            memset(szTemp, 0x00, sizeof (szTemp));
            //format amount 10+2
            sprintf(szTemp, "%012.0f", atof(baAmount));
            if (strcmp(szTemp, baBaseAmount) > 0) {
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
                continue;
            }

            wub_str_2_hex(szTemp, srTransRec.szTipAmount, 12);

            break;
        } else if (d_USER_CANCEL == key) {
            vdSetErrorMessage("USER CANCEL");
            return d_NO;
        } else if (0xFF == key) {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }

    } while (1);

    return key;
}

int inCTOS_GetSaleAdjustAmount(void) {
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE baTipAmount[20];
    ULONG ulAmount = 0L;
    BYTE szAmtTmp2[16 + 1];
    int len;

    DebugAddSTR("inCTOS_GetSaleAdjustAmount", "Processing...       ", 20);
    memset(baBaseAmount, 0x00, sizeof (baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4);


    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    //format amount 10+2
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", baBaseAmount, szAmtTmp2);
    //sprintf(szAmtTmp2, "%lu.%02lu", atol(baTipAmount)/100, atol(baTipAmount)%100);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ORG AMT");
    setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (strlen(szAmtTmp2) + 1)*2, 6, szAmtTmp2);

    CTOS_LCDTPrintXY(1, 7, "NEW AMT");

    do {
        CTOS_KBDHit(&key); //clear key buffer

        memset(baAmount, 0x00, sizeof (baAmount));
        //key = InputAmount(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        vdDebug_LogPrintf("szCurSymbol=[%s]", strCST.szCurSymbol);
        if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0)
            key = InputAmountEx(1, 8, szDisplayBuf, 0, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);
        else
            key = InputAmountEx(1, 8, szDisplayBuf, 2, 0x00, baAmount, &ulAmount, d_INPUT_TIMEOUT, 1);

        if (strlen(baAmount) > 0) {
            vdDebug_LogPrintf("11baAmount=[%s]", baAmount);
            if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
                len = strlen(baAmount);
                strcpy(&baAmount[len], "00");
            }
            vdDebug_LogPrintf("22baAmount=[%s]", baAmount);

            memset(szTemp, 0x00, sizeof (szTemp));
            //format amount 10+2
            sprintf(szTemp, "%012.0f", atof(baAmount));

            wub_str_2_hex(szTemp, srTransRec.szBaseAmount, 12);

            break;
        } else if (d_USER_CANCEL == key) {
            vdSetErrorMessage("USER CANCEL");
            return d_NO;
        } else if (0xFF == key) {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }

    } while (1);

    return key;
}

int inCTOS_UpdateTxnTotalAmount(void) {
    BYTE szBaseAmount[20];
    BYTE szTipAmount[20];
    BYTE szTotalAmount[20];
    BYTE EMVtagVal[64];
    BYTE szStr[64];
    BYTE byDataTmp1[32];
    BYTE byDataTmp2[32];
    BYTE bPackSendBuf[256];
    USHORT usPackSendLen = 0;
    USHORT ushEMVtagLen;
    ULONG lnTmp;

    memset(szBaseAmount, 0x00, sizeof (szBaseAmount));
    memset(szTipAmount, 0x00, sizeof (szTipAmount));
    memset(szTotalAmount, 0x00, sizeof (szTotalAmount));

    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);

    //format amount 10+2
    sprintf(szTotalAmount, "%012.0f", atof(szBaseAmount) + atof(szTipAmount));
    wub_str_2_hex(szTotalAmount, srTransRec.szTotalAmount, 12);

    DebugAddSTR(szTotalAmount, szTipAmount, 12);
    DebugAddSTR("Total amount", "Tip amount", 12);

    DebugAddHEX("Tip amount", srTransRec.szTipAmount, 6);
    DebugAddHEX("Total amount", srTransRec.szTotalAmount, 6);
    //for improve transaction speed    
#if 0  
    //for trans speed 20150317
    //    if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
    if (((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
            (1 == inCTOSS_GetWaveTransType())))
        /*		
        (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
        (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
        (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
        (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC)))
        /* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
    {
        memset(byDataTmp1, 0x00, sizeof (byDataTmp1));
        memset(byDataTmp2, 0x00, sizeof (byDataTmp2));
        wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
        lnTmp = atol(byDataTmp1);
        wub_long_2_array(lnTmp, byDataTmp2);

        memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
        bPackSendBuf[usPackSendLen++] = 0x04;
        memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
        usPackSendLen += 4;

        memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szTotalAmount, 6);

        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x02", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F02, 6);
        usPackSendLen += 6;

        if (atol(szTipAmount) > 0) {
            memcpy(srTransRec.stEMVinfo.T9F03, srTransRec.szTipAmount, 6);
        } else {
            memset(szTipAmount, 0x00, sizeof (szTipAmount));
            memcpy(srTransRec.stEMVinfo.T9F03, szTipAmount, 6);
        }

        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x03", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F03, 6);
        usPackSendLen += 6;

        usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);
    }
#endif    
    return d_OK;
}


#ifdef DISCOUNT_FEATURE
int inCTOS_UpdateTxnTotalAmountDisc(void)
{
    BYTE szBaseAmount[20];
    BYTE szFixedAmount[20];
    BYTE szTotalAmount[20];
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
    BYTE  byDataTmp1[32];
    BYTE  byDataTmp2[32];
    BYTE  bPackSendBuf[256];
    USHORT usPackSendLen = 0;
    USHORT ushEMVtagLen;
    ULONG lnTmp;


	vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountDisc [%s]", srTransRec.szFixedAmount);
	
    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szFixedAmount, 0x00, sizeof(szFixedAmount));
    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));

    wub_hex_2_str(srTransRec.szFixedAmount, szFixedAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);


	vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountDisc2 [%s][%s]", szFixedAmount, szBaseAmount);

	if(atol(szFixedAmount) > atol(szBaseAmount))
	{
		CTOS_LCDTClearDisplay();		
		CTOS_LCDTPrintXY(1, 6, "FIXED AMOUNT IS");					
		CTOS_LCDTPrintXY(1, 7, "GREATER THAN TOTAL");										
		CTOS_LCDTPrintXY(1, 8, "AMOUNT");										
		CTOS_Beep();
		CTOS_Beep();
		CTOS_Delay(2000);
		return(ST_ERROR); 
	}
	else
	{
		//format amount 10+2
    	sprintf(szTotalAmount, "%012.0f", atof(szBaseAmount) - atof(szFixedAmount));
    	wub_str_2_hex(szTotalAmount,srTransRec.szTotalAmount,12);    // For display
    	wub_str_2_hex(szTotalAmount,srTransRec.szBaseAmount,12);	 //	for printing and accum
		wub_str_2_hex(szBaseAmount,srTransRec.szOrigAmountDisc,12);  // save for printing - ORIGINAL AMOUNT
		srTransRec.fIsDiscounted = TRUE; // for printing.
		srTransRec.fIsDiscountedFixAmt = TRUE;  // for print condition
		
		
	}
	
    DebugAddSTR(szTotalAmount,szFixedAmount,12); 
    DebugAddSTR("Total amount","szFixedAmount amount",12); 
    
    DebugAddHEX("szFixedAmount amount",srTransRec.szFixedAmount,6); 
    DebugAddHEX("Total amount",srTransRec.szTotalAmount,6); 
    DebugAddHEX("Base amount",srTransRec.szBaseAmount,6); 

    return d_OK;
}

int inCTOS_UpdateTxnTotalAmountDiscPerc(void)
{
    BYTE szBaseAmount[20];
    BYTE szPercentage[20];
    BYTE szTotalAmount[12+1];
	BYTE szTotalAmountFinal[12+1];
    BYTE szTotalAmountDeducted[20];	
    BYTE   EMVtagVal[64];
    BYTE   szStr[12 + 1];
    BYTE  byDataTmp1[32];
    BYTE  byDataTmp2[32];
    BYTE  bPackSendBuf[256];
    USHORT usPackSendLen = 0;
    USHORT ushEMVtagLen;
    ULONG lnTmp;
	int inPecentage = 0;
    BYTE szTotalAmt[12 + 1];
    BYTE szTempBuf[12 + 1];


	vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountPerc");
	
    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szPercentage, 0x00, sizeof(szPercentage));
    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
    memset(szTotalAmountFinal, 0x00, sizeof(szTotalAmountFinal));
	
    memset(szTotalAmountDeducted, 0x00, sizeof(szTotalAmountDeducted));
    memset(szStr, 0x00, sizeof(szStr));


    wub_hex_2_str(srTransRec.szPercentage, szPercentage, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);


	vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountPerc2 [%d][%d]", atoi(szPercentage), atoi(szBaseAmount));
#if 1
	//if(atoi(szPercentage) > atoi(szBaseAmount))
	if(atoi(szPercentage) > 9999)
	{
		CTOS_LCDTClearDisplay();		
		CTOS_LCDTPrintXY(1, 6, "DISC PERCENTAGE IS");					
		CTOS_LCDTPrintXY(1, 7, "MORE THAN 100%");										
		//CTOS_LCDTPrintXY(1, 7, "GREATER THAN TOTAL");										
		//CTOS_LCDTPrintXY(1, 8, "AMOUNT");										
		CTOS_Beep();
		CTOS_Beep();
		CTOS_Delay(3000);
		return(ST_ERROR); 
	}
	else
	{
		//Compute Percentage
		sprintf(szTotalAmount, "%012.0f", atof(szPercentage) / 100 * atof(szBaseAmount));

		// Get final amount with deducted percentage
    	sprintf(szTotalAmountDeducted, "%012.0f", atof(szTotalAmount) / 100);
    	sprintf(szTotalAmountFinal, "%012.0f", atof(szBaseAmount) - atof(szTotalAmountDeducted));
		

		vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountDisc4 [%s][%s][%s][%s]", 
			szTotalAmount, szTotalAmountDeducted, szTotalAmountFinal, strCST.szCurSymbol);

		
		// round off
		if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0){
			szTotalAmountFinal[10] = '0';
			szTotalAmountFinal[11] = '0';
		}


		vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountDisc5 [%02x][%02x]", szTotalAmountFinal[10],szTotalAmountFinal[11]);


		wub_str_2_hex(szTotalAmountFinal,srTransRec.szTotalAmount,12);	  // For display
		wub_str_2_hex(szTotalAmountFinal,srTransRec.szBaseAmount,12);	 // original working...for printing and accum, is also the "Tot Disc AMT:"	
		wub_str_2_hex(szBaseAmount,srTransRec.szOrigAmountDisc,12);  	// save for printing - ORIGINAL AMOUNT
		srTransRec.fIsDiscounted = TRUE; 			// for printing.
		srTransRec.fIsDiscountedPercAmt = TRUE; 	// for printing.		
	}
#else
		//Compute Percentage
		sprintf(szTotalAmount, "%012.0f", atof(szPercentage) / 100 * atof(szBaseAmount));

		// Get final amount with deducted percentage
    	sprintf(szTotalAmountDeducted, "%012.0f", atof(szTotalAmount) / 100);
    	sprintf(szTotalAmountFinal, "%012.0f", atof(szBaseAmount) - atof(szTotalAmountDeducted));
		

		vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountDisc4 [%s][%s][%s][%s]", 
			szTotalAmount, szTotalAmountDeducted, szTotalAmountFinal, strCST.szCurSymbol);

		
		// round off
		if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0){
			szTotalAmountFinal[10] = '0';
			szTotalAmountFinal[11] = '0';
		}


		vdDebug_LogPrintf("inCTOS_UpdateTxnTotalAmountDisc5 [%02x][%02x]", szTotalAmountFinal[10],szTotalAmountFinal[11]);


		wub_str_2_hex(szTotalAmountFinal,srTransRec.szTotalAmount,12);	  // For display
		wub_str_2_hex(szTotalAmountFinal,srTransRec.szBaseAmount,12);	 // original working...for printing and accum, is also the "Tot Disc AMT:"	
		wub_str_2_hex(szBaseAmount,srTransRec.szOrigAmountDisc,12);  	// save for printing - ORIGINAL AMOUNT
		srTransRec.fIsDiscounted = TRUE; 			// for printing.
		srTransRec.fIsDiscountedPercAmt = TRUE; 	// for printing.		
#endif	

    DebugAddHEX("Percentage",srTransRec.szPercentage,6); 
    DebugAddHEX("Total amount",srTransRec.szTotalAmount,6); 
    DebugAddHEX("Base amount",srTransRec.szBaseAmount,6); 

    return d_OK;
}


#endif
int inCTOS_GetOffApproveNO(void) {
    USHORT usX = 1, usY = 6;
    BYTE bShowAttr = 0x02;
    USHORT szAuthCodeLen = 6;
    BYTE baPIN[6 + 1];
    BYTE szAuthCode[6 + 1];
    BYTE bRet;
	int inRet;

    memset(szAuthCode, 0x00, sizeof (szAuthCode));

	vdDebug_LogPrintf("inCTOS_GetOffApproveNO");

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Enter Approval Code: ");

    while (TRUE) {
		clearLine(8);
        bRet = InputStringAlphaEx2(1, 8, 0x06, 0x02, szAuthCode, &szAuthCodeLen, 0, d_INPUT_TIMEOUT);

        if (bRet == d_KBD_CANCEL) {
            CTOS_LCDTClearDisplay();
            //vduiClearBelow(2);
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        //if(strlen(szAuthCode) >= 2)
        if(strlen(szAuthCode) == 6)
        {		
            memcpy(srTransRec.szAuthCode, szAuthCode, 6);
            //break;
            			//http://118.201.48.214:8080/issues/75.41 #3
            			//3)We can't test Same MID/ Different TID case and got "No Record found" with another terminal for Preauth functions. Please kindly help to check and fix it.
				#if 0 // should continue without checking approval code  - 08222022
			//#ifdef CB_MPU_NH_MIGRATION //for NEW HOST implementation. to get RRn for PreAuth Comp 
				inRet = inCTOS_BatchSearchByAuthCode();
				if (d_OK != inRet)
				{
				
					vdDebug_LogPrintf("inCTOS_GetOffApproveNO-1");
				
					vdDisplayErrorMsg(1, 8, "NO RECORD FOUND");
					//return inRet;
				}
				else
					break;
			#else
				break;
			#endif			
        }
        else
        {
        
			vdDebug_LogPrintf("inCTOS_GetOffApproveNO-2");

	            memset(szAuthCode, 0x00, sizeof(szAuthCode));
	            szAuthCodeLen = 6;
		      //vdDisplayErrorMsg(1, 8, "NO RECORD FOUND");
		      vdDisplayErrorMsg(1, 8, "INVLD INPUT");
			
        }
    }

	vdDebug_LogPrintf("inCTOS_GetOffApproveNO-end");

    return ST_SUCCESS;
}

int inCTOS_GetInvoice(void) {

    BYTE key;
    USHORT usX = 1, usY = 6;
    BYTE baString[100 + 1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02;
    USHORT usInvoiceLen = 6;
    BYTE baPIN[6 + 1];
    BYTE szInvNoAsc[6 + 1];
    BYTE szInvNoBcd[3];
    BYTE bRet;
    int iLens = 6;
    int inResult;
    char szBcd[INVOICE_BCD_SIZE + 1];

    TRANS_DATA_TABLE srTransRecTemp;

    DebugAddSTR("inCTOS_GetInvoice", "Processing...", 20);

    if (!strTCT.fManualEntryInv) {
        memset(szBcd, 0x00, sizeof (szBcd));
        memcpy(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);
        inBcdAddOne(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);

        if ((inResult = inTCTSave(1)) != ST_SUCCESS) {
            vdSetErrorMessage("LOAD TCT ERR");
            return ST_ERROR;
        }

        memcpy(srTransRec.szInvoiceNo, strTCT.szInvoiceNo, 3);
    } else {
        memset(szInvNoAsc, 0x00, sizeof (szInvNoAsc));
        memset(szInvNoBcd, 0x00, sizeof (szInvNoBcd));

        memset((char*) &srTransRecTemp, 0x00, sizeof (TRANS_DATA_TABLE));
        memcpy(&srTransRecTemp, &srTransRec, sizeof (TRANS_DATA_TABLE));

        while (TRUE) {
            if (strTCT.fECR) // tct
            {
                //If ECR send INV
                if (memcmp(srTransRec.szInvoiceNo, "\x00\x00\x00", 3) != 0) {
                    inResult = inCTOSS_BatchCheckDuplicateInvoice();
                    vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice[%d]", inResult);

                    memcpy(&srTransRec, &srTransRecTemp, sizeof (TRANS_DATA_TABLE));
                    if (d_OK == inResult) {
                        CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "DUPLICATE INVOICE");

                        return (d_NO);
                    } else {
                        return (d_OK);
                    }
                }
            }

            CTOS_LCDTClearDisplay();
            //vduiClearBelow(2);
            vdDispTransTitle(srTransRecTemp.byTransType);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "Invoice NO: ");

            bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
            if (bRet == d_KBD_CANCEL) {
                CTOS_LCDTClearDisplay();
                vdSetErrorMessage("USER CANCEL");
                memcpy(&srTransRec, &srTransRecTemp, sizeof (TRANS_DATA_TABLE));
                return (d_EDM_USER_CANCEL);
            }


            if (atoi(szInvNoAsc) != 0) {
                inAscii2Bcd(szInvNoAsc, szInvNoBcd, INVOICE_BCD_SIZE);

                memcpy(srTransRec.szInvoiceNo, szInvNoBcd, 3);
                inResult = inCTOSS_BatchCheckDuplicateInvoice();
                vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice[%d]", inResult);
                if (d_OK == inResult) {
                    CTOS_LCDTClearDisplay();
                    vdDisplayErrorMsg(1, 8, "DUPLICATE INVOICE");
                    memset(szInvNoAsc, 0x00, sizeof (szInvNoAsc));
                    usInvoiceLen = 6;

                    memset(srTransRec.szInvoiceNo, 0x00, sizeof (srTransRec.szInvoiceNo));
                    continue;
                } else {
                    break;
                }
            }
        }

        memcpy(&srTransRec, &srTransRecTemp, sizeof (TRANS_DATA_TABLE));
        memcpy(srTransRec.szInvoiceNo, szInvNoBcd, 3);
    }

    return (d_OK);
}

VS_BOOL fAmountLessThanFloorLimit(void) {
    //long lnTotalAmount = 0;
    BYTE szTotalAmt[12 + 1];

    BYTE    szStr[45];
    char szTemp[AMT_ASC_SIZE+1];
    char szTemp1[AMT_ASC_SIZE+1];

	int inTemp;

    /* If the transaction amount is less than the floor limit,
        Set the transaction type to OFFLINE */

	vdDebug_LogPrintf("fAmountLessThanFloorLimit strHDT.inCurrencyIdx [%d]>> strCST.inCurrencyIndex [%d]>>strCST.szCurSymbol[%s]", 
	strHDT.inCurrencyIdx, strCST.inCurrencyIndex, strCST.szCurSymbol);

	//4) Currency is incorrect in Customer Copy and Bank Copy (merchant copy is correct) for MPU-JCB cards which should be always MMK.
	// no need, already assigned in print.c line 2575
	//inCSTRead(strHDT.inCurrencyIdx);

#if 1
    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
	{
    	memset(szTemp, 0x00, sizeof(szTemp));
        wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
        sprintf(szTotalAmt, "%012.0f", atof(szTemp)/100);
    	memset(szStr, 0x00, sizeof(szStr));
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmt, szStr);
    } 
	else 
	{
        memset(szTemp1, 0x00, sizeof(szTemp1));
       wub_hex_2_str(srTransRec.szTotalAmount, szTemp1, AMT_BCD_SIZE);
        memset(szStr, 0x00, sizeof(szStr));
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szStr);
    }
	
#else
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
    lnTotalAmount = atol(szTotalAmt);
#endif

	vdDebug_LogPrintf("fAmountLessThanFloorLimit >>>>> szTotalAmt:[%s] szTemp:[%s] szTemp1:[%s]:szTemp1:[%ld] InFloorLimitAmount:[%d] szStr:[%s]szStr:[%ld]", 
	szTotalAmt, szTemp, szTemp1,atol(szTemp1), strCDT.InFloorLimitAmount, szStr, atol(szStr)); 

	//must have same logic with UPI CVM checking - as per CB 10-28-2021
	if(strCDT.InFloorLimitAmount == 0){
		vdDebug_LogPrintf("fAmountLessThanFloorLimit-1");
		//return (d_OK);	- 2) Sign line is not present for any amount when InFloorLimit = 0 and JCBSIGNLINE = 1 for JCB and MPU-JCB cards. - Reported 06212022
		return (d_NO);
	}

	/*
	5. With fix for http://118.201.48.214:8080/issues/75.72 #2
	> The expected flow for all entry modes is ->
	when JCBSIGNLINE = 1 and InFloorLimit is USD 50, then sign line should be present starting from USD 50.01,
	*/
    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
		inTemp = atoi(szTemp);		
		if(inTemp > strCDT.InFloorLimitAmount){
			
			vdDebug_LogPrintf("fAmountLessThanFloorLimit return MMK with SIGN LINE");
			return d_NO;
		}
    	}
	else
	{	
		if(atol(szTemp1)> strCDT.InFloorLimitAmount){
			vdDebug_LogPrintf("fAmountLessThanFloorLimit return USD with SIGN LINE");
			return d_NO;
		}
	}

	#if 0 //orig
	if (strCDT.InFloorLimitAmount >= inTemp)
	{
		
		vdDebug_LogPrintf("fAmountLessThanFloorLimit-2");
		return (d_OK); //return with NO signature required
	}
	
	return (d_NO); //Return with signature line
	#endif


	vdDebug_LogPrintf("fAmountLessThanFloorLimit-3.END");
	return d_OK;

}

int inCTOS_CustComputeAndDispTotal(void) {
    CTOS_RTC SetRTC;
    BYTE szTotalAmt[12 + 1];
    BYTE szTempBuf[12 + 1];
    BYTE szTempBuf1[12 + 1];
    BYTE szDisplayBuf[30];
    BYTE szStr[45];

    int inResult, tranId, hostId;

    vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal START");

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);

    //inCSTRead(strCST.inCurrencyIndex); // for testing currency display issue raised by CB dated August 21 #2

    CTOS_LCDTPrintXY(1, 5, "TOTAL AMOUNT :");



    memset(szTotalAmt, 0x00, sizeof (szTotalAmt)); //thandar_added

    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
    memset(szStr, 0x00, sizeof (szStr));

    //format amount 10+2
    if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
        if (strlen(szTotalAmt) - 2 > 10) {
            vdDispErrMsg("EXCEED INPUT AMOUNT");
            return d_NO;
        }

        memset(szTempBuf, 0x00, sizeof (szTempBuf));
        sprintf(szTempBuf, "%012.0f", atof(szTotalAmt) / 100);
		
        vdDebug_LogPrintf("szTempBuf [%s]", szTempBuf);
		
        memset(szTotalAmt, 0x00, sizeof (szTotalAmt));
        strcpy(szTotalAmt, szTempBuf);
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmt, szStr);
    } else
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);

    //sprintf(szStr,"%10.0f.%02.0f",(atof(szTotalAmt)/100), (atof(szTotalAmt)%100));
    //sprintf(szStr, "%lu.%02lu", atol(szTotalAmt)/100, atol(szTotalAmt)%100);
    setLCDPrint(6, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szStr)+1)*2,  6, szStr);
    CTOS_LCDTPrintAligned(6, szStr, d_LCD_ALIGNRIGHT);

    if (inCTOSS_CheckExceedMaxAMT(szTotalAmt) == d_NO) {
        vdDisplayErrorMsg(1, 8, "EXCEED MAX AMOUT");
        return d_NO;
    }

    //nyan
    //
    #if 0
    tranId = srTransRec.byTransType;
    if ((strIIT.inIssuerNumber == 6) && (tranId == PRE_AUTH || tranId == PRE_COMP || tranId == PREAUTH_COMP)) { //only pre auth and pre auth complete
        //hostId = srTransRec.b
        vdDebug_LogPrintf("PANDISPLAY: Entry=[%d],byTransType=[%d]", srTransRec.byEntryMode, srTransRec.byTransType);
        inCTOS_DisplayCardInfo(2, 3);

        //TO WAIT USER CONFIRM
        inResult = struiGetchWithTimeOut();
        if (inResult == d_KBD_CANCEL) {
            vdDebug_LogPrintf("struiGetchWithTimeOut() with user cancel!");
            return d_NO;
        }
    }
    #endif


	#if 1
    vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal:szTotalAmt=[%ld],fVEPS=[%d],  strTCT.fQPS =[%d]",atol(szTotalAmt), strTCT.fVEPS, strTCT.fQPS);

    vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal:strTCT.szVEPSUSDLimit=[%s]:[%ld]",strTCT.szVEPSUSDLimit,atol(strTCT.szVEPSUSDLimit));	
    vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal:strTCT.szQPSUSDLimit=[%s]:[%ld]",strTCT.szQPSUSDLimit,atol(strTCT.szQPSUSDLimit));

    //vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal:strTCT.szVEPSLimit=[%s]:[%ld]",strTCT.szVEPSLimit,atol(strTCT.szVEPSLimit));
    //vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal:strTCT.szQPSLimit=[%s]:%ld]",strTCT.szQPSLimit,atol(strTCT.szQPSLimit));
	#endif

    vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal Entry=[%d],byTransType=[%d], inCurrencyIndex[%d], srTransRec.HDTid[%d]", 
		srTransRec.byEntryMode, srTransRec.byTransType, strCST.inCurrencyIndex, srTransRec.HDTid);

   #if 1
   //USD CURRENCY
   //1) http://118.201.48.214:8080/issues/75.72. #1Visa/MC sign line issue The current flow is whatever fVEPS, fQPS values are 0 or 1, sign line is present for any amount.   
    //if ((strncmp(strCST.szCurSymbol, "USD", 3) == 0) && (srTransRec.HDTid == 21)) {
    if ((strncmp(strCST.szCurSymbol, "USD", 3) == 0) && (srTransRec.HDTid == 21 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)) {
        vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal USD currency");	

		if(('4' == srTransRec.szPAN[0]) || ('5' == srTransRec.szPAN[0]))
			vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal USD ('4' || '5' == srTransRec.szPAN[0])");			


        //for VISA USD currency
        if (strTCT.fVEPS == 1 && atol(strTCT.szVEPSUSDLimit) >= atol(szTotalAmt)
                && ('4' == srTransRec.szPAN[0]) && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND)) {

            if (srTransRec.byEntryMode == CARD_ENTRY_FALLBACK ||
                    srTransRec.byEntryMode == CARD_ENTRY_MANUAL) {
                vdDebug_LogPrintf("VEPS MMK not support for manual, fallback");
            } else {

                strcpy(srTransRec.stIPPinfo.szInstallmentTerms, "VPU");

                vdDebug_LogPrintf(" inCTOS_CustComputeAndDispTotal szInstallmentTerms=[%s]", srTransRec.stIPPinfo.szInstallmentTerms);
               
                // Return with NO SIGNATURE REQUIRED
                return d_OK;
            }
        }

        //for Master USD currency

        if (strTCT.fQPS == 1 && atol(strTCT.szQPSUSDLimit) >= atol(szTotalAmt)
                && ('5' == srTransRec.szPAN[0]) && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND)) {

            if (srTransRec.byEntryMode == CARD_ENTRY_FALLBACK ||
                    srTransRec.byEntryMode == CARD_ENTRY_MANUAL) {
                vdDebug_LogPrintf("QPS MMK not support for manual, fallback");
            } else {

                strcpy(srTransRec.stIPPinfo.szInstallmentTerms, "QPU");

                vdDebug_LogPrintf(" inCTOS_CustComputeAndDispTotal szInstallmentTerms=[%s]", srTransRec.stIPPinfo.szInstallmentTerms);
               
                // Return with NO SIGNATURE REQUIRED
                return d_OK;
            }
        }


    }

	//MMK CURRENCY
	//1) http://118.201.48.214:8080/issues/75.72. #1Visa/MC sign line issue The current flow is whatever fVEPS, fQPS values are 0 or 1, sign line is present for any amount.	
	//if ((strncmp(strCST.szCurSymbol, "MMK", 3) == 0) && (srTransRec.HDTid == 21)) {
    if ((strncmp(strCST.szCurSymbol, "MMK", 3) == 0) && (srTransRec.HDTid == 21 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)) {
        		vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal MMK currency szPAN[%x]", srTransRec.szPAN[0]);	

		if(('4' == srTransRec.szPAN[0]) || ('5' == srTransRec.szPAN[0]))
			vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal MMK ('4' || '5' == srTransRec.szPAN[0])");	


        //for VISA MMK currency
        if (strTCT.fVEPS == 1 && atol(strTCT.szVEPSLimit) >= atol(szTotalAmt)
                && ('4' == srTransRec.szPAN[0]) && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND)) {

            if (srTransRec.byEntryMode == CARD_ENTRY_FALLBACK ||
                    srTransRec.byEntryMode == CARD_ENTRY_MANUAL) {
                vdDebug_LogPrintf("VEPS MMK not support for manual, fallback");
            } else {

                strcpy(srTransRec.stIPPinfo.szInstallmentTerms, "VPM");

                vdDebug_LogPrintf(" inCTOS_CustComputeAndDispTotal szInstallmentTerms=[%s]", srTransRec.stIPPinfo.szInstallmentTerms);

                // Return with NO SIGNATURE REQUIRED
                return d_OK;
            }
        }

        //for Master MMK currency
        if ( strTCT.fQPS == 1 && atol(strTCT.szQPSLimit) >= atol(szTotalAmt)
                && ('5' == srTransRec.szPAN[0]) && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND)) {

            if (srTransRec.byEntryMode == CARD_ENTRY_FALLBACK ||
                    srTransRec.byEntryMode == CARD_ENTRY_MANUAL) {
                vdDebug_LogPrintf("QPS MMK not support for manual, fallback");
            } else {

                strcpy(srTransRec.stIPPinfo.szInstallmentTerms, "QPM");

                vdDebug_LogPrintf(" inCTOS_CustComputeAndDispTotal szInstallmentTerms=[%s]", srTransRec.stIPPinfo.szInstallmentTerms);

                // Return with NO SIGNATURE REQUIRED				
                return d_OK;
            }
        }

    }
	#endif

	vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal AFTER CHECKING!!!");

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        return (d_OK);
    }

    if ((srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
            (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
            (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))
        /* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
    {
        return (d_OK);
    }

    if (srTransRec.byTransType != SALE) {
        return (d_OK);
    }

	// no need to check as offline will not be supported
	#if 0
    if (fAmountLessThanFloorLimit() == d_OK) {
        srTransRec.shTransResult = TRANS_AUTHORIZED;
        srTransRec.byOffline = CN_TRUE;

        //Read the date and the time //
        CTOS_RTCGet(&SetRTC);
        /* Set Month & Day*/
        memset(szTempBuf, 0x00, sizeof (szTempBuf));
        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        sprintf(szTempBuf, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szTempBuf, srTransRec.szDate, 4);
        sprintf(szTempBuf1, "%02d", SetRTC.bYear);
        memcpy(srTransRec.szYear, szTempBuf1, 2);
        memset(szTempBuf, 0, sizeof (szTempBuf));
        sprintf(szTempBuf, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
        wub_str_2_hex(szTempBuf, srTransRec.szTime, 6);
    }
	#endif
	vdDebug_LogPrintf("inCTOS_CustComputeAndDispTotal END");

    return (d_OK);
}

int inCTOS_DisplayResponse(void) {
    int inMsgid = atoi(srTransRec.szRespCode);
    int inHostIndex = srTransRec.HDTid;
    int inResult = 0;
    char szResponseCode[60];

    extern BYTE szErrRespMsg[64 + 1];

    memset(szErrRespMsg, 0x00, sizeof (szErrRespMsg));

    memset(szResponseCode, 0x00, sizeof (szResponseCode));
    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);

    if (inMsgid == 61) {
        strcpy(szResponseCode, "EXCEED LIMIT");
        if (srTransRec.byPackType == CASH_ADVANCE) {
            memset(szResponseCode, 0x00, sizeof (szResponseCode));
            strcpy(szResponseCode, "Exceeds withdraw amt");
        }
        vdDisplayErrorMsg(1, 8, szResponseCode);
    } else {
        //start_thandar_UPI (Finexus) host and  Visa/Master host are routing to Finexus host

        if (inHostIndex == 7)
            inHostIndex = 1;
        //end_thandar_

        inMSGResponseCodeRead(szResponseCode, inMsgid, inHostIndex);
        if (0 == strlen(szResponseCode)) {
            inMSGResponseCodeRead(szResponseCode, inMsgid, 1);
            if (0 != strlen(szResponseCode))
                vdDisplayErrorMsg(1, 8, szResponseCode);
        } else {
            vdDisplayErrorMsg(1, 8, szResponseCode);
        }
    }

    strcpy(szErrRespMsg, szResponseCode);

    vdDebug_LogPrintf("inMsgid[%d]inHostIndex[%d]szResponseCode[%s]", inMsgid, inHostIndex, szResponseCode);
    return (d_OK);
}

int inCTOS_GeneralGetInvoice(void) {
    BYTE key;
    USHORT usX = 1, usY = 6;
    BYTE baString[100 + 1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02;
    USHORT usInvoiceLen = 6;
    BYTE baPIN[6 + 1];
    BYTE szInvNoAsc[6 + 1];
    BYTE szInvNoBcd[3];
    BYTE bRet;
    int iLens = 6;
    int inResult;
    char szBcd[INVOICE_BCD_SIZE + 1];

    if (inMultiAP_CheckSubAPStatus() == d_OK) {
        return d_OK;
    }

    //1010
    if (strTCT.fECR) // tct
    {
        // patrick fix code 20141208 ==if idle want void transaction and ecr flag on. this checking is illegal.  
        //       if (memcmp(srTransRec.szInvoiceNo, "\x00\x00\x00", 3) != 0)
        //      return ST_SUCCESS;
        if (fECRTxnFlg == 1) {
            return ST_SUCCESS;
        }


    }
    //1010


    memset(szInvNoAsc, 0x00, sizeof (szInvNoAsc));
    memset(szInvNoBcd, 0x00, sizeof (szInvNoBcd));

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "INVOICE NO: "); // CB_MPU_NH_MIGRATION
    //setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRACE NO: "); -- original MPU change request

    while (TRUE) {
        usInvoiceLen = 6;
        vdDebug_LogPrintf("11bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]", bRet, atoi(szInvNoAsc), usInvoiceLen);
        bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usInvoiceLen, 1, d_GETPIN_TIMEOUT);
        vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usInvoiceLen[%d]", bRet, atoi(szInvNoAsc), usInvoiceLen);
        if (bRet == d_KBD_CANCEL) {
            CTOS_LCDTClearDisplay();
            vdSetErrorMessage("USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }


        if (atoi(szInvNoAsc) != 0) {
            inAscii2Bcd(szInvNoAsc, szInvNoBcd, INVOICE_BCD_SIZE);
            DebugAddSTR("INV NUM:", szInvNoAsc, 12);
            memcpy(srTransRec.szInvoiceNo, szInvNoBcd, 3);
            break;
        }
    }

    return ST_SUCCESS;
}

int inCTOS_GeneralGetRRN(void) {
    BYTE key;
    USHORT usX =1, usY = 8;
    BYTE baString[100+1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02;
    USHORT usRRNLen = 12;
    BYTE szRRNAsc[RRN_BYTES + 1];
    BYTE bRet;
    int iLens = 6;
    int inResult;

    if (inMultiAP_CheckSubAPStatus() == d_OK) {
        return d_OK;
    }

    //1010
    if (strTCT.fECR) // tct
    {
        // patrick fix code 20141208 ==if idle want void transaction and ecr flag on. this checking is illegal.  
        //       if (memcmp(srTransRec.szInvoiceNo, "\x00\x00\x00", 3) != 0)
        //      return ST_SUCCESS;
        if (fECRTxnFlg == 1) {
            return ST_SUCCESS;
        }


    }
    //1010


    memset(szRRNAsc, 0x00, sizeof (szRRNAsc));

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
	
	#ifdef CB_MPU_NH_MIGRATION
    setLCDPrint(4, DISPLAY_POSITION_LEFT, "Enter Original");
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Reference Number:");
	#else
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "REF. NUM: ");
	#endif
	
    while (TRUE) {
		clearLine(8);
        usRRNLen = 12;
        bRet = InputString(usX, usY, 0x00, bShowAttr, szRRNAsc, &usRRNLen, 1, d_GETPIN_TIMEOUT);
        vdDebug_LogPrintf("bRet[%d] szRRNAsc=[%s] usRRNLen[%d]", bRet, szRRNAsc, usRRNLen);
        if (bRet == d_KBD_CANCEL) {
            CTOS_LCDTClearDisplay();
            vdSetErrorMessage("USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        if(strlen(szRRNAsc) == 12)
        {
            //memcpy(srTransRec.szRRN, szRRNAsc, 12);
            //break;

			#if 0 // Check for existing record in batch - for NEW HOST implementation. to get RRn for PreAuth Comp 			
				inResult = inDatabase_BatchSearchByRRN(&srTransRec, szRRNAsc);

				vdDebug_LogPrintf("inCTOS_GeneralGetRRN inResult [%d] RRN [%s]", inResult, srTransRec.szRRN);
				
				if (d_OK != inResult)
				{
					vdDebug_LogPrintf("inCTOS_GeneralGetRRN CCCCC");
				
					vdDisplayErrorMsg(1, 8, "NO RECORD FOUND");
				}
				else
				{
					memcpy(srTransRec.szRRN, szRRNAsc, 12);
					break;
				}
			#else
            			memcpy(srTransRec.szRRN, szRRNAsc, 12);
				break;
			#endif
			
        }       	
		else // CB_MPU_NH_MIGRATION
		{
			vdDebug_LogPrintf("inCTOS_GeneralGetRRN DDDDD");		
			//vdDisplayErrorMsg(1, 8, "NO RECORD FOUND");
			vdDisplayErrorMsg(1, 8, "INVLD INPUT");
		}
    }

    return ST_SUCCESS;
}

int inCTOS_BatchSearch(void) {
    int inResult = d_NO;

	vdDebug_LogPrintf("inCTOS_BatchSearch START");

    inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);

    DebugAddSTR("inCTOS_BatchSearch", "Processing...", 20);

    DebugAddINT("inCTOS_BatchSearch", inResult);

    if (inResult != d_OK) {
        if (inMultiAP_CheckSubAPStatus() != d_OK)
			vdDebug_LogPrintf("inCTOS_BatchSearch HERE");
			
            vdSetErrorMessage("NO RECORD FOUND");
        return d_NOT_RECORD;
    }

    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 3);

    return inResult;
}

int inCTOS_BatchSearchByRRN(void) {
    int inResult = d_NO;

    inResult = inDatabase_BatchSearchByRRN(&srTransRec, srTransRec.szRRN);

    DebugAddSTR("inCTOS_BatchSearchByRRN", "Processing...", 20);

    DebugAddINT("inCTOS_BatchSearchByRRN", inResult);

    if (inResult != d_OK) {
        if (inMultiAP_CheckSubAPStatus() != d_OK)
            vdSetErrorMessage("NO RECORD FOUND");
        return d_NOT_RECORD;
    }

    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 3);

    return inResult;
}

int inCTOS_BatchSearchByAuthCode(void) {
    int inResult = d_NO;


	vdDebug_LogPrintf("inCTOS_BatchSearchByAuthCode srTransRec.HDTid [%d]", srTransRec.HDTid);

	//http://118.201.48.210:8080/redmine/issues/1525.110
	if(srTransRec.HDTid == 21 || srTransRec.HDTid == 7){
		vdDebug_LogPrintf("inCTOS_BatchSearchByAuthCode - here");
		return d_OK;
	}

	
    inResult = inDatabase_BatchSearchByAuthCode(&srTransRec, srTransRec.szAuthCode);

    DebugAddSTR("inCTOS_BatchSearchByAuthCode", "Processing...", 20);

    DebugAddINT("inCTOS_BatchSearchByAuthCode", inResult);
    vdDebug_LogPrintf("inCTOS_BatchSearchByAuthCode srTransRec.szRRN[%s]", srTransRec.szRRN);
	//vdDebug_LogPrintf("inCTOS_BatchSearchByAuthCode srTransRec.szTotalAmount[%s]", srTransRec.szTotalAmount);

    if (inResult != d_OK) {
        if (inMultiAP_CheckSubAPStatus() != d_OK)
            vdSetErrorMessage("NO RECORD FOUND");
        return d_NOT_RECORD;
    }

	DebugAddHEX("D A T E: ", srTransRec.szDate, 2);	
	DebugAddHEX("T I M E: ", srTransRec.szTime, 3);	
	
    
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 3);

	vdDebug_LogPrintf("inCTOS_BatchSearchByAuthCode end inResult [%d]", inResult);

    return inResult;
}

int inCTOS_LoadCDTandIIT(void) {

	
    if (inCDTRead(srTransRec.CDTid) != d_OK) {
        vdDebug_LogPrintf("inCTOS_LoadCDTandIIT LOAD CDT ERROR [%d]", srTransRec.CDTid, strCST.inCurrencyIndex);
        vdSetErrorMessage("LOAD CDT ERROR");
        return (d_NO);
    }
    inGetIssuerRecord(strCDT.IITid);

	vdDebug_LogPrintf("inCTOS_LoadCDTandIIT inCurrencyIndex:[%d] srTransRec.inCardType[%d]", strCST.inCurrencyIndex, srTransRec.inCardType);

    //    if(strCDT.inType == 1){
    //        vdSetMPUTrans(TRUE);
    //    } else {
    //        vdSetMPUTrans(FALSE);
    //    }
    if (srTransRec.inCardType == 1) {
        vdSetMPUTrans(TRUE);
    } else {
        vdSetMPUTrans(FALSE);
    }

    if ((strCDT.inType == 1) && (srTransRec.fIsInstallment == TRUE) && (srTransRec.byTransType == VOID)) {
        vdSetMPUCard(TRUE);
    }

    return ST_SUCCESS;
}

int inCTOS_CheckTipadjust() {
    if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_TIP)) {
        if (srTransRec.byTransType == SALE)
            srTransRec.byOrgTransType = srTransRec.byTransType;

        memcpy(srTransRec.szOrgAmount, srTransRec.szTotalAmount, 6);
    } else {
        vdSetErrorMessage("TIP NOT ALLWD");
        return d_NO;
    }

    srTransRec.byTransType = SALE_TIP;

    return d_OK;
}

int inCTOS_CheckVOID() {

    vdDebug_LogPrintf("void = %d", srTransRec.byVoided);

    if (srTransRec.byVoided == TRUE) {
        vdSetErrorMessage("ALREADY VOIDED");
        return d_NO;
    }
	
    #ifdef TOPUP_RELOAD
        if (srTransRec.byTransType == TOPUP || srTransRec.byTransType == RELOAD) {
            vdSetErrorMessage("TRANS NOT ALLOWED");
        return d_NO;
    }
    #endif
	
    //    if(srTransRec.byTransType == PRE_AUTH)
    //    {
    //        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED"); 
    //        return d_NO;
    //    }

    //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - START
    if (srTransRec.byTransType == SALE_TIP) {
        szOriginTipTrType = srTransRec.byOrgTransType;
        srTransRec.byOrgTransType = srTransRec.byTransType;
    } else //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - END
        srTransRec.byOrgTransType = srTransRec.byTransType;

    srTransRec.byTransType = VOID;

    return d_OK;
}

int inCTOS_CheckVOID_IPP(void) {
    if (srTransRec.byVoided == TRUE) {
        vdSetErrorMessage("ALREADY VOIDED");
        return d_NO;
    }

    //    if(srTransRec.byTransType == PRE_AUTH)
    //    {
    //        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED"); 
    //        return d_NO;
    //    }

    if (srTransRec.fIsInstallment != TRUE) {
        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED");
        return d_NO;
    }

    //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - START
    if (srTransRec.byTransType == SALE_TIP) {
        szOriginTipTrType = srTransRec.byOrgTransType;
        srTransRec.byOrgTransType = srTransRec.byTransType;
    } else //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - END
        srTransRec.byOrgTransType = srTransRec.byTransType;

    srTransRec.byTransType = VOID;

    return d_OK;
}

int inCTOS_CheckVOID_MPUPreAuth(void) {
    if (srTransRec.byVoided == TRUE) {
        vdSetErrorMessage("ALREADY VOIDED");
        return d_NO;
    }

    //    if((srTransRec.byTransType != MPU_PREAUTH) && (srTransRec.byTransType != MPU_PREAUTH_COMP))
    //    {
    //        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED"); 
    //        return d_NO;
    //    }

    //    //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - START
    //    if(srTransRec.byTransType == SALE_TIP)
    //    {
    //        szOriginTipTrType = srTransRec.byOrgTransType;
    //        srTransRec.byOrgTransType = srTransRec.byTransType;
    //    }
    //    else //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - END
    //    srTransRec.byOrgTransType = srTransRec.byTransType;

    //    if (srTransRec.byOrgTransType == MPU_PREAUTH)
    //        srTransRec.byTransType = MPU_VOID_PREAUTH;
    //    else
    //        srTransRec.byTransType = MPU_VOID_PREAUTH_COMP;
    //    srTransRec.byTransType = *byTransType;

    //    if (srTransRec.byTransType == MPU_VOID_PREAUTH && srTransRec.byOrgTransType != MPU_PREAUTH) {
    //        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED");
    //        return d_NO;
    //    }
    //    
    //    if(srTransRec.byTransType == MPU_VOID_PREAUTH_COMP && srTransRec.byOrgTransType != MPU_PREAUTH_COMP){
    //        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED");
    //        return d_NO;
    //    }

    return d_OK;
}

int inCTOS_CheckVOID_PREAUTH_COMP(void) {

    vdDebug_LogPrintf("inCTOS_CheckVOID_PREAUTH_COMP void = %d", srTransRec.byVoided);

    if (srTransRec.byVoided == TRUE) {
        vdSetErrorMessage("ALREADY VOIDED");
        return d_NO;
    }

    if (srTransRec.byTransType != PREAUTH_COMP) {
        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED");
        return d_NO;
    }

    srTransRec.byOrgTransType = srTransRec.byTransType;

    srTransRec.byTransType = VOID_PREAUTH_COMP;

    return d_OK;
}

int inCTOS_VoidSelectHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;

    inHostIndex = (short) srTransRec.HDTid;

    DebugAddSTR("inCTOS_SelectHost", "Processing...", 20);

#ifdef MPUPIOnUsFlag
	vdDebug_LogPrintf("inCTOS_VoidSelectHost >>[%d] inHostIndex >> inHostIndex[%d] >> fGetMPUPIOnUs[%d]", strHDT.inCurrencyIdx, inHostIndex,  fGetMPUPIOnUs());
#else
vdDebug_LogPrintf("inCTOS_VoidSelectHost >>[%d] inHostIndex >> inHostIndex[%d]", strHDT.inCurrencyIdx, inHostIndex);
#endif

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {

        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;


		vdDebug_LogPrintf("inCTOS_VoidSelectHost2 >inCurrencyIndex>inHostIndex>CDTid>[%d][%d][%d]", inCurrencyIdx, inHostIndex, srTransRec.CDTid);

        //        if(VS_TRUE == fGetMPUTrans()) //@@IBR ADD 20170208
        //            inCurrencyIdx = 1;
#if 0
		//FIN-MMK
        if (inHostIndex == 17 || inHostIndex == 23)
            inCurrencyIdx = 1;

	//FIN-USD
	if(inHostIndex == 22)
		inCurrencyIdx = 2;
#else

	//http://118.201.48.214:8080/issues/75.63 #1
	//1) MPU-UPI OnUs Void > currency changed to MMK (original trnx is performed with USD)
			//09202022
	#ifdef CBB_FIN_ROUTING
			//fin-mmk				mpu-bpc 				installment
	if(inHostIndex == 23 || inHostIndex == 17 || srTransRec.fIsInstallment == TRUE)
	{
		if(srTransRec.CDTid == 58 || srTransRec.CDTid == 62 || srTransRec.CDTid == 63 || srTransRec.CDTid == 64 || srTransRec.CDTid == 71)	// ADDRESSED NOTE #4
			inCurrencyIdx = strHDT.inCurrencyIdx;
		else	
			inCurrencyIdx = 1;
	}
	else			
		 inCurrencyIdx = strHDT.inCurrencyIdx;

	#endif

#endif	

        if (srTransRec.fIsInstallment == TRUE)
            inHostIndex = 6;

        //        srTransRec.fIsInstallment = FALSE;

        if (inCSTRead(inCurrencyIdx) != d_OK) {

            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

        if (inMMTReadRecord(inHostIndex, srTransRec.MITid) != d_OK) {
            vdSetErrorMessage("LOAD MMT ERR");
            return (d_NO);
        }

        if (inCPTRead(inHostIndex) != d_OK) {
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

        inCTOS_PreConnect();

        return (d_OK);
    }


}

int inCTOS_SelectCurrAppHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;
    char szAPName[25];
    int inAPPID;

    memset(szAPName, 0x00, sizeof (szAPName));
    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
    vdDebug_LogPrintf("szAPName[%s]....", szAPName);


    //inHostIndex = (short) strCDT.HDTid;
    inHostIndex = inHDTReadByApname(szAPName);

    vdDebug_LogPrintf("inCTOS_SelectCurrAppHost =[%d]", inHostIndex);

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTRead(inCurrencyIdx) != d_OK) {
            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

        if (inCPTRead(inHostIndex) != d_OK) {
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

        return (d_OK);
    }
}

int inCTOS_ChkBatchEmpty() {
    int inResult;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    memset(&srAccumRec, 0, sizeof (srAccumRec));
    memset(&strFile, 0, sizeof (strFile));
    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    memset(&strFile, 0, sizeof (strFile));
    vdCTOS_GetAccumName(&strFile, &srAccumRec);

    //vdDebug_LogPrintf("strFile.szFileName=[%s]-----", strFile.szFileName);//thandar

    if ((inResult = inMyFile_CheckFileExist(strFile.szFileName)) <= 0) {

       vdDebug_LogPrintf("inCTOS_ChkBatchEmpty inResult=[%d]-----",inResult);//thandar

        if (CN_TRUE == strMMT[0].fMustSettFlag) {
            strMMT[0].fMustSettFlag = CN_FALSE;
            inMMTSave(strMMT[0].MMTid);
        }

        CTOS_LCDTClearDisplay();
        //if (srTransRec.byTransType == SETTLE)  // sidumili: Issue#:000109 [Display hostname during settle all]
        vdSetErrorMessage("BATCH EMPTY");

        return (d_NO);
    }

    if (strTCT.fMustAutoSettle == TRUE)
        return d_OK;

    return (d_OK);
}

int inCTOS_ConfirmInvAmt(void) {
    BYTE szAmtTmp1[16 + 1];
    BYTE szAmtTmp2[16 + 1];
    char szDisplayBuf[30];
    BYTE key;
    USHORT result;
    char szTemp[MAX_CHAR_PER_LINE + 1];

    BYTE bBuf[4 + 1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
	
    int inCSTinCurrencyIndex = 0;

    CTOS_LCDTClearDisplay();
    vduiLightOn();

    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(2, 3);

	vdDebug_LogPrintf("inCTOS_ConfirmInvAmt inCurrencyIndex[%d]inHostIndex[%d]inCurrencyIdx[%d]HDTid[%d]IITid[%d]",
		strCST.inCurrencyIndex, strHDT.inHostIndex, strHDT.inCurrencyIdx, srTransRec.HDTid, srTransRec.IITid);

    //if (strHDT.inHostIndex != CBPAY_HOST_INDEX) //expiry date not applicable for CB PAY
    if(srTransRec.HDTid != CBPAY_HOST_INDEX && srTransRec.HDTid != OK_DOLLAR_HOST_INDEX)
	{
		vdDebug_LogPrintf("inCTOS_ConfirmInvAmt HERE");
	
		if (srTransRec.byTransType == VOID) {
	        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
	        memset(bBuf, 0x00, sizeof (bBuf));
	        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
	        memcpy(bDisplayStr, bBuf, 2);
	        bDisplayStr[2] = '/';
	        memset(bBuf, 0x00, sizeof (bBuf));
	        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
	        memcpy(bDisplayStr + 3, bBuf, 2);
	        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 4, bDisplayStr);
	    }
    }
//old code
#if 0
    //if (strHDT.inHostIndex == 17) -- orig    	//IPP SHOULD ALWAYS BE MMK //http://118.201.48.210:8080/redmine/issues/1525.115
    if(srTransRec.HDTid == 17 || srTransRec.HDTid == 6)
        inCSTRead(1);

	//Request # 4) MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus should be always MMK
	#ifdef CBB_FIN_ROUTING
	vdDebug_LogPrintf("inCTOS_ConfirmInvAmt CBB_FIN_ROUTING");
	
		if(srTransRec.IITid == 7 || srTransRec.IITid == 8 || srTransRec.IITid == 9)
			inCSTRead(1);		
	#endif
#else
#ifdef CBB_FIN_ROUTING
		#ifdef MPUPIOnUsFlag
		vdDebug_LogPrintf(" inCTOS_GetTxnBaseAmount srTransRec.CDTid =[%d] fGetMPUPIOnUs[%d]", srTransRec.CDTid, fGetMPUPIOnUs()); 	
		#else
		vdDebug_LogPrintf(" inCTOS_GetTxnBaseAmount srTransRec.CDTid =[%d]", srTransRec.CDTid);

		#endif

		// on cancel, terminal should display original default currency. #2
		inCSTinCurrencyIndex = strCST.inCurrencyIndex;


		//fix for case http://118.201.48.214:8080/issues/75.53 #1
		//1) We are getting currency issue with MPU-UPI offus card on USD setup terminal . MPU onus/offus, MPU-JCB onus/off, MPU-UPI offus should be always MMK.
				//fin-mmk				mpu-bpc 				installment
		//http://118.201.48.214:8080/issues/75.63 #1
		//1) MPU-UPI OnUs Void, Preauth Void, Preauth Comp Void -> currency changed to MMK (original trnx is performed with USD)
		//The expected flow is currency should be the same as original trnx currency.
		//Please refer to attached logs.
		
		if(srTransRec.HDTid == 23 || srTransRec.HDTid == 17){
			if(srTransRec.CDTid == 58 || srTransRec.CDTid == 62 || srTransRec.CDTid == 63 || srTransRec.CDTid == 64 || srTransRec.CDTid == 71)	// ADDRESSED NOTE #4
				inCSTRead( strCST.inCurrencyIndex);	
			else	
				inCSTRead(1);	
		}
		else			
			inCSTRead(strCST.inCurrencyIndex);	
#endif

#endif
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtTmp1, 0x00, sizeof (szAmtTmp1));
    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    //    wub_hex_2_str(srTransRec.szTotalAmount, szAmtTmp1, 6);
    wub_hex_2_str(srTransRec.szTotalAmount, szTemp, 6);



    if (strCST.inCurrencyIndex == 1) {
        sprintf(szAmtTmp1, "%012.0f", atof(szTemp) / 100);
        //format amount 10+2
        //	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szAmtTmp1, szAmtTmp2);
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szAmtTmp1, szAmtTmp2);
    } else {
        sprintf(szAmtTmp1, "%012.0f", atof(szTemp));
        //format amount 10+2
        //	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szAmtTmp1, szAmtTmp2);
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szAmtTmp1, szAmtTmp2);
    }
    //sprintf(szAmtTmp2,"%10.0f.%02.0f",(atof(szAmtTmp1)/100), (atof(szAmtTmp1)%100));
    //sprintf(szAmtTmp2, "%lu.%02lu", atol(szAmtTmp1)/100, atol(szAmtTmp1)%100);

    //        if(srTransRec.byTransType == VOID){
    //        memset(szTemp, 0x00, sizeof(szTemp));
    //        strcpy(szTemp, "-");
    //        strcat(szTemp, szAmtTmp2);
    //        
    //        memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
    //        strcpy(szAmtTmp2, szTemp);
    //    }

    //gcitra
    //CTOS_LCDTPrintXY(1, 5, "AMOUNT");
    //setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtTmp2)+1)*2,  6, szAmtTmp2);
    CTOS_LCDTPrintXY(1, 5, "AMOUNT");
    setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (strlen(szAmtTmp2) + 1)*2, 6, szAmtTmp2);

    //gcitra


    if (srTransRec.byTransType == SALE_TIP)
        vduiDisplayStringCenter(7, "CONFIRM TIP ADJUST");
    //    if(srTransRec.byTransType == VOID)
    if (srTransRec.byTransType == VOID || srTransRec.byTransType == MPU_VOID_PREAUTH || srTransRec.byTransType == MPU_VOID_PREAUTH_COMP)
        vduiDisplayStringCenter(7, "CONFIRM VOID");

    vduiDisplayStringCenter(8, "NO[X] YES[OK]");
    CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
    vduiWarningSound();

    CTOS_KBDBufFlush(); //cleare key buffer

    while (1) {

        CTOS_KBDHit(&key);
        if (key == d_KBD_ENTER) {
			vduiClearBelow(7);
            result = d_OK;
            break;
        } else if ((key == d_KBD_CANCEL)) {
            result = d_NO;
            vdSetErrorMessage("USER CANCEL");
            break;
        }
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            result = d_NO;
            vdSetErrorMessage("TIME OUT");
            break;
        }
    }

	
	// on cancel, terminal should display original default currency. #1
#ifdef CBB_FIN_ROUTING
	if(key == d_USER_CANCEL){
		if(inCSTinCurrencyIndex == 2)
			inCSTRead(2);
	}
#endif	

    return result;
}

int inCTOS_ConfirmAmt(void) {
    BYTE szAmtTmp1[16 + 1];
    BYTE szAmtTmp2[16 + 1];
    char szDisplayBuf[30];
    BYTE key;
    USHORT result;

#if 0
    CTOS_LCDTClearDisplay();
    vduiLightOn();

    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtTmp1, 0x00, sizeof (szAmtTmp1));
    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    wub_hex_2_str(srTransRec.szTotalAmount, szAmtTmp1, 6);
    //format amount 10+2
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szAmtTmp1, szAmtTmp2);
    //sprintf(szAmtTmp2,"%10.0f.%02.0f",(atof(szAmtTmp1)/100), (atof(szAmtTmp1)%100));
    //sprintf(szAmtTmp2, "%lu.%02lu", atol(szAmtTmp1)/100, atol(szAmtTmp1)%100);

    //gcitra
    //CTOS_LCDTPrintXY(1, 5, "AMOUNT");
    //setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-(strlen(szAmtTmp2)+1)*2,  6, szAmtTmp2);
    CTOS_LCDTPrintXY(1, 4, "AMOUNT");
    setLCDPrint(5, DISPLAY_POSITION_LEFT, szDisplayBuf);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (strlen(szAmtTmp2) + 1)*2, 5, szAmtTmp2);

    //gcitra

#endif

    vduiDisplayStringCenter(7, "CONFIRM AMOUNT");

    vduiDisplayStringCenter(8, "NO[X] YES[OK]");
    CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
    vduiWarningSound();

    CTOS_KBDBufFlush(); //cleare key buffer

    while (1) {

        CTOS_KBDHit(&key);
        if (key == d_KBD_ENTER) {
            result = d_OK;
            break;
        } else if ((key == d_KBD_CANCEL)) {
            result = d_NO;
            vdSetErrorMessage("USER CANCEL");
            break;
        }
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            result = d_NO;
            vdSetErrorMessage("TIME OUT");
            break;
        }
    }

    return result;
}

int inCTOS_IPP_SettlementSelectAndLoadHost(void) {
    int key;
    char szBcd[INVOICE_BCD_SIZE + 1];

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    inCTOS_IPP_HostSetting();

    memset(szBcd, 0x00, sizeof (szBcd));
    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
    inHDTSave(strHDT.inHostIndex);

    if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0) {
        vdSetErrorMessage("NOT ALLOWED");
        return (d_NO);
    }

    vdSetSettleIPPFlag(TRUE);

    return d_OK;
}

int inCTOS_SettlementSelectAndLoadHost(void) {
    int key;
    char szBcd[INVOICE_BCD_SIZE + 1];


    vdDebug_LogPrintf("inCTOS_SettlementSelectAndLoadHost");

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    key = inCTOS_SelectHostSetting();
    if (key == -1) {
        return key;
    }

    memset(szBcd, 0x00, sizeof (szBcd));
    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
    inHDTSave(strHDT.inHostIndex);

    if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0) {
        vdSetErrorMessage("NOT ALLOWED");
        return (d_NO);
    }

    vdDebug_LogPrintf("strHDT.inHostIndex[%d]", strHDT.inHostIndex);
    //vdDebug_PrintOnPaper("strHDT.inHostIndex[%d]", strHDT.inHostIndex);

    if (strHDT.inHostIndex == 6) {
        vdSetSettleIPPFlag(TRUE);
    } else {
        vdSetSettleIPPFlag(FALSE);
    }

    return d_OK;
}

int inCTOS_SettlementClearBathAndAccum(void) {
    int inResult;
    BYTE szBcd[INVOICE_BCD_SIZE + 1];
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    vdDebug_LogPrintf("inCTOS_SettlementClearBathAndAccum");

    memset(szBcd, 0x00, sizeof (szBcd));
    memcpy(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);
    inBcdAddOne(szBcd, strMMT[0].szBatchNo, INVOICE_BCD_SIZE);
    strMMT[0].fMustSettFlag = CN_FALSE;
    inMMTSave(strMMT[0].MMTid);


    inDatabase_BatchDelete();

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    inCTOS_ReadAccumTotal(&srAccumRec);
    strcpy(srAccumRec.szTID, srTransRec.szTID);
    strcpy(srAccumRec.szMID, srTransRec.szMID);
    memcpy(srAccumRec.szYear, srTransRec.szYear, DATE_BCD_SIZE);
    memcpy(srAccumRec.szDate, srTransRec.szDate, DATE_BCD_SIZE);
    memcpy(srAccumRec.szTime, srTransRec.szTime, TIME_BCD_SIZE);
    memcpy(srAccumRec.szBatchNo, srTransRec.szBatchNo, BATCH_NO_BCD_SIZE);
    inCTOS_SaveAccumTotal(&srAccumRec);

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    memset(&strFile, 0, sizeof (strFile));
    vdCTOS_GetAccumName(&strFile, &srAccumRec);
    vdDebug_LogPrintf("[strFile.szFileName[%s]", strFile.szFileName);
    vdCTOS_SetBackupAccumFile(strFile.szFileName);
    if ((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK) {
        vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
    }
    //create the accum file
    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    inCTOS_ReadAccumTotal(&srAccumRec);

	
    inMyFile_ReversalDelete();

    inMyFile_AdviceDelete();

    inMyFile_TCUploadDelete();

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

    return d_OK;
}

int inCTOS_DisplayBatchTotal(void) {
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount;
    DOUBLE ulSaleTotalAmount;
    USHORT usRefundCount;
    DOUBLE ulRefundTotalAmount;
    DOUBLE ulTotalAmount;
    ACCUM_REC srAccumRec;


    CTOS_LCDTClearDisplay();
    vdDispTransTitle(BATCH_TOTAL);

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    if ((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR) {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;
    }

    /*CardBiz no need display this*/
    if (get_env_int("#CUSTNSR") == 1)
        return d_OK;

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    if (strTCT.inFontFNTMode != 1 && (strTCT.inThemesType == 2 || strTCT.inThemesType == 3))
        strcpy(szDisplayBuf, "Type   Cnt    Amt");
    else
        strcpy(szDisplayBuf, "Type   Cnt  Amt(N LINE)");
    CTOS_LCDTPrintXY(1, 2, szDisplayBuf);


    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));
    //sprintf(szBuf, "%ld", ulSaleTotalAmount);
    //format amount 10+2 
    if (ulSaleTotalAmount > 0) {
        sprintf(szBuf, "%.0f", ulSaleTotalAmount);
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
    } else
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
    sprintf(szDisplayBuf, "SALE  %04d", usSaleCount);
    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
    //sprintf(szDisplayBuf, "%s  %16s", strCST.szCurSymbol, szAmtBuf);
    //CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
    vdCTOSS_DisplayAmount(1, 4, strCST.szCurSymbol, szAmtBuf);


    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));
    //format amount 10+2 
    if (ulRefundTotalAmount > 0) {
        sprintf(szBuf, "%.0f", ulRefundTotalAmount);
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
    } else
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);

    sprintf(szDisplayBuf, "RFUD  %04d", usRefundCount);
    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
    vdCTOSS_DisplayAmount(1, 6, strCST.szCurSymbol, szAmtBuf);
    //sprintf(szDisplayBuf, "%s  %16s", strCST.szCurSymbol, szAmtBuf);
    //CTOS_LCDTPrintXY(1, 6, szDisplayBuf);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));
    //format amount 10+2 
    ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount;
    sprintf(szBuf, "%.0f", (ulSaleTotalAmount > ulRefundTotalAmount) ? (ulSaleTotalAmount - ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount));
    if (ulTotalAmount == 0)
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
    else
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
    memset(szBuf, 0x00, sizeof (szBuf));
    if (ulSaleTotalAmount < ulRefundTotalAmount)
        sprintf(szBuf, "-%s", szAmtBuf);
    else
        sprintf(szBuf, "%s", szAmtBuf);
    sprintf(szDisplayBuf, "TOTL  %04d", (usSaleCount + usRefundCount));
    CTOS_LCDTPrintXY(1, 7, szDisplayBuf);
    vdCTOSS_DisplayAmount(1, 8, strCST.szCurSymbol, szBuf);
    //sprintf(szDisplayBuf, "%s%17s", strCST.szCurSymbol, szBuf);
    //CTOS_LCDTPrintXY(1, 8, szDisplayBuf);

    WaitKey(60);

    return d_OK;
}

int inCTOS_ConfirmYesNo(BYTE *pszMsg) {
    int inResult;
    BYTE key;

    CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
    vduiWarningSound();

    CTOS_KBDBufFlush(); //cleare key buffer

    vduiDisplayStringCenter(6, pszMsg);
    vduiDisplayStringCenter(8, "NO[X] YES[OK]");
    CTOS_KBDBufFlush();
    while (1) {
        CTOS_KBDHit(&key);
        if (key == d_KBD_ENTER) {
            inResult = d_OK;
            break;
        }
        if ((key == d_KBD_CANCEL)) {
            inResult = d_NO;
            break;
        }

        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            inResult = d_NO;
            vdSetErrorMessage("TIME OUT");
            break;
        }
    }

    return inResult;
}

int inCTOS_DisplayBatchTotalEx2(void) {
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount;
    DOUBLE ulSaleTotalAmount;
    USHORT usRefundCount;
    DOUBLE ulRefundTotalAmount;
    DOUBLE ulTotalAmount;
    ACCUM_REC srAccumRec;
    USHORT usCashAdvanceCount;
    DOUBLE ulCashAdvTotalAmount;
    USHORT usCUPSaleCount;
    DOUBLE ulCUPSaleTotalAmount;
    int inECRTransactionStatus;
	
    USHORT usCBPaySaleCount;
    DOUBLE ulCBPaySaleTotalAmount;

	
//#ifdef OK_DOLLAR_FEATURE
#if 1
    USHORT usOKDSaleCount;
    DOUBLE ulOKDSaleTotalAmount;
#endif

    BYTE key;
    char szTemp[20 + 1];

    //        char szTemp[20+1];

    vdDebug_LogPrintf("inCTOS_DisplayBatchTotalEx2");

    if (get_env_int("NERASGERM") == 1)
        return d_OK;

    inECRTransactionStatus = get_env_int("ECRTRANS");

    CTOS_LCDTClearDisplay();

    if (strTCT.fMustAutoSettle == TRUE)
        return d_OK;

    vdDispTransTitle(srTransRec.byTransType);

    CTOS_LCDTSelectFontSize(d_FONT_12x24);

    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "BATCH TOTAL : %s", strHDT.szHostLabel);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, szTemp);

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    if ((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR) {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;
    }

    /*CardBiz no need display this*/
    if (get_env_int("#CUSTNSR") == 1)
        return d_OK;

    vdDebug_LogPrintf("fGetMPUTrans[%d] strHDT.inCurrencyIdx[%d]srTransRec.HDTid[%d]", fGetMPUTrans(), strHDT.inCurrencyIdx, srTransRec.HDTid);
    //vdDebug_PrintOnPaper("fGetMPUTrans[%d]", fGetMPUTrans());

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;
    inCSTRead(strHDT.inCurrencyIdx);

	
    if (VS_TRUE == fGetMPUTrans()) {
        inTranCardType = 1;
    }

    /*if it is IPP host*/
    if (srTransRec.HDTid == 6) {
        inTranCardType = 0;
    }

    if (srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23
		#ifdef TOPUP_RELOAD
		|| srTransRec.HDTid == 20
		#endif
		) 
		{
        inTranCardType = 1;

        if (srTransRec.HDTid == 17)
            inCSTRead(1);
    }
																								////http://118.201.48.210:8080/redmine/issues/1525.115
	if(strHDT.inHostIndex == CBPAY_HOST_INDEX || strHDT.inHostIndex == OK_DOLLAR_HOST_INDEX || srTransRec.HDTid == 6 || srTransRec.HDTid == 23)
		inCSTRead(1);

	//FIN-USD
	if(srTransRec.HDTid ==  22)
		inCSTRead(2);
		
		

    vdDebug_LogPrintf("inTranCardType[%d]", inTranCardType);

    if (TRUE == fGetCashAdvAppFlag()) {
        inTranCardType = 0;
    }

    vdDebug_LogPrintf("inTranCardType[%d]", inTranCardType);

    vdDebug_LogPrintf("srTransRec.HDTid[%d] inTranCardType[%d]", srTransRec.HDTid, inTranCardType);

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("[inCTOS_DisplayBatchTotalEx] ulSaleTotalAmount[%f]", ulSaleTotalAmount);

    //@@IBR ADD 05102016
    usCashAdvanceCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount);
    ulCashAdvTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount);
    //@@IBR FINISH ADD 05102016

    usCUPSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPSaleCount);
    ulCUPSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPSaleTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

	//CBPay
    usCBPaySaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCBPaySaleCount);
    ulCBPaySaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount);
	

	#ifdef OK_DOLLAR_FEATURE
    usOKDSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOKDSaleCount);
    ulOKDSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount);	
	#endif
	
    vdDebug_LogPrintf("strTCT.inFontFNTMode[%d]", strTCT.inFontFNTMode);
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    if (strTCT.inFontFNTMode != 1 && (strTCT.inThemesType == 2 || strTCT.inThemesType == 3))
        strcpy(szDisplayBuf, "Type   Cnt      Amt");
    else
        strcpy(szDisplayBuf, "Type   Cnt  Amt(N LINE)");
    //    CTOS_LCDTPrintXY(1, 2, szDisplayBuf);


    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));

//CASH ADVANCE
    if (TRUE == fGetCashAdvAppFlag()) 
	{ //@@IBR ADD 05102016
        if (ulCashAdvTotalAmount > 0) {
            sprintf(szBuf, "%.0f", ulCashAdvTotalAmount);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        } else
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);

        if (strTCT.fECR) {
            put_env_int("#STMTCASHADVCNT", usRefundCount);
            put_env("#STMTCASHADVAMT", szAmtBuf, 40);
        }

        //        sprintf(szDisplayBuf, "CASH ADV  %04d", usCashAdvanceCount);
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
        sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "CASH ADV:", 21 - (strlen("CASH ADV:")), szTemp);
        CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
        //        vdCTOSS_DisplayAmount(1, 4, strCST.szCurSymbol, szAmtBuf);
    } 
	else 
	{
        //        if(memcmp(srTransRec.szHostLabel, "CUP", 3) == 0){
        //            if (ulCUPSaleTotalAmount > 0) {
        //                sprintf(szBuf, "%.0f", ulCUPSaleTotalAmount);
        //                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        //            } else
        //                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        //            
        //            if (strTCT.fECR) {
        //                put_env_int("#STMTSALECUPCNT",usCUPSaleCount);
        //                put_env("#STMTSALECUPAMT", szAmtBuf, 40);
        //            }
        //            
        //        } else {


		#ifdef CASH_ADV_NEW_FEATURE

		#if 0
        if (ulCashAdvTotalAmount > 0) {
            sprintf(szBuf, "%.0f", ulCashAdvTotalAmount);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        } else
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
		#endif
		
        if (ulCashAdvTotalAmount > 0) 
		{
            sprintf(szBuf, "%.0f", ulCashAdvTotalAmount);

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTemp, 0x00, sizeof (szTemp));
                sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
                memset(szBuf, 0x00, sizeof (szBuf));
                strcpy(szBuf, szTemp);
            }

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
            }
			
			memset(szTemp, 0x00, sizeof (szTemp));
			
			sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
			//sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "CASH ADV:", 21 - (strlen("CASH ADV:")), szTemp);
			sprintf(szDisplayBuf, "%s %s", "CASH ADV:", szTemp);		
			CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
			
        }
		/*else
		{
			if (strcmp(strCST.szCurSymbol, "MMK") == 0)
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
			else
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);			
		}*/
			
        if (strTCT.fECR) {
            put_env_int("#STMTCASHADVCNT", usCashAdvanceCount);
            put_env("#STMTCASHADVAMT", szAmtBuf, 40);
        }

        /*memset(szTemp, 0x00, sizeof (szTemp));
		
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
        //sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "CASH ADV:", 21 - (strlen("CASH ADV:")), szTemp);
		sprintf(szDisplayBuf, "%s %s", "CASH ADV:", szTemp);        
        CTOS_LCDTPrintXY(1, 3, szDisplayBuf);*/
		
		#endif


//SALE
		memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
		memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
		memset(szBuf, 0x00, sizeof (szBuf));

        if (ulSaleTotalAmount > 0) 
		{
            sprintf(szBuf, "%.0f", ulSaleTotalAmount);

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTemp, 0x00, sizeof (szTemp));
                sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
                memset(szBuf, 0x00, sizeof (szBuf));
                strcpy(szBuf, szTemp);
            }

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
            }

			memset(szTemp, 0x00, sizeof (szTemp));
			sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf); 	
			
			if(srTransRec.HDTid != CBPAY_HOST_INDEX && srTransRec.HDTid != OK_DOLLAR_HOST_INDEX){	//don't display for CBPAY	
				sprintf(szDisplayBuf, "%s %s", "SALES:", szTemp);
				CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
			}
			
        }
        /*else
		{                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
            else
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        }*/


        if (strTCT.fECR) {
            vdDebug_LogPrintf("usSaleCount=[%d]", usSaleCount);
            put_env_int("#STMTSALECNT", usSaleCount);
            put_env("#STMTSALEAMT", szAmtBuf, 40);
            vdDebug_LogPrintf("#STMTSALECNT=[%d]", get_env_int("#STMTSALECNT"));
        }

        /*memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);		

		if(srTransRec.HDTid != CBPAY_HOST_INDEX)	//don't display for CBPAY
			sprintf(szDisplayBuf, "%s %s", "SALES:", szTemp);
        CTOS_LCDTPrintXY(1, 4, szDisplayBuf);*/

    }

	
    //sprintf(szBuf, "%ld", ulSaleTotalAmount);
    //format amount 10+2 
    //    if (ulSaleTotalAmount > 0)
    //    {
    //		sprintf(szBuf, "%.0f", ulSaleTotalAmount);
    //	    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    //    }
    //	else
    //		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0",szAmtBuf);
    //    sprintf(szDisplayBuf, "SALE  %04d", usSaleCount);
    //    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
    //	//sprintf(szDisplayBuf, "%s  %16s", strCST.szCurSymbol, szAmtBuf);
    //	//CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
    //	vdCTOSS_DisplayAmount(1,4,strCST.szCurSymbol,szAmtBuf);

//REFUND
	    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
	    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
	    memset(szBuf, 0x00, sizeof (szBuf));
	    //format amount 10+2 
	    if (ulRefundTotalAmount > 0) 
		{
	        sprintf(szBuf, "%.0f", ulRefundTotalAmount);

	        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
	            memset(szTemp, 0x00, sizeof (szTemp));
	            sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
	            memset(szBuf, 0x00, sizeof (szBuf));
	            strcpy(szBuf, szTemp);
	        }

	        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
	        } else {
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
	        }
			
			memset(szTemp, 0x00, sizeof (szTemp));
			sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
			
			if (fGetCashAdvAppFlag() == TRUE) 
				sprintf(szDisplayBuf, "%-*s%*s", 20/2, "RFNDS:", 20-(strlen("RFNDS:")), szTemp);
			else
				sprintf(szDisplayBuf, "%s %s", "RFNDS:", szTemp);
			
			if(srTransRec.HDTid != CBPAY_HOST_INDEX && srTransRec.HDTid != OK_DOLLAR_HOST_INDEX)	//don't display for CBPAY				
				CTOS_LCDTPrintXY(1, 6, szDisplayBuf);
			
	    } 

		/*else {

	        if (strcmp(strCST.szCurSymbol, "MMK") == 0)
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
	        else
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
	    }*/

	    if (strTCT.fECR) {
	        put_env_int("#STMTREFUNDCNT", usRefundCount);
	        put_env("#STMTREFUNDAMT", szAmtBuf, 40);
	    }

	    /*memset(szTemp, 0x00, sizeof (szTemp));
	    sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
		
		if (fGetCashAdvAppFlag() == TRUE) 
	        sprintf(szDisplayBuf, "%-*s%*s", 20/2, "RFNDS:", 20-(strlen("RFNDS:")), szTemp);
		else
	    	sprintf(szDisplayBuf, "%s %s", "RFNDS:", szTemp);
	    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);*/
	


//CBPAY		
	memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
	memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
	memset(szBuf, 0x00, sizeof (szBuf));

	if (fGetCashAdvAppFlag() == FALSE) 
	{
		if (ulCBPaySaleTotalAmount > 0) 
		{
			sprintf(szBuf, "%.0f", ulCBPaySaleTotalAmount);
	
			if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
				memset(szTemp, 0x00, sizeof (szTemp));
				sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
				memset(szBuf, 0x00, sizeof (szBuf));
				strcpy(szBuf, szTemp);
			}
	
			if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
			} else {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
			}
			memset(szTemp, 0x00, sizeof (szTemp));
			sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
			sprintf(szDisplayBuf, "%s %s", "SALES:", szTemp);
			CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
			
		} 
		/*else 
		{
	
			if (strcmp(strCST.szCurSymbol, "MMK") == 0)
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
			else
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
		}*/
	
		/*if (strTCT.fECR) {
			put_env_int("#STMTREFUNDCNT", usRefundCount);
			put_env("#STMTREFUNDAMT", szAmtBuf, 40);
		}*/
	
	}		

#ifdef OK_DOLLAR_FEATURE		
	memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
	memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
	memset(szBuf, 0x00, sizeof (szBuf));
				
	if (ulOKDSaleTotalAmount > 0) 
	{
		sprintf(szBuf, "%.0f", ulOKDSaleTotalAmount);
		
		if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
			memset(szTemp, 0x00, sizeof (szTemp));
			sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
			memset(szBuf, 0x00, sizeof (szBuf));
			strcpy(szBuf, szTemp);
		}
			
		if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
		} 
		else 
		{
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
		}
		memset(szTemp, 0x00, sizeof (szTemp));
		sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
		sprintf(szDisplayBuf, "%s %s", "SALES:", szTemp);
		CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
		
	} 
	/*else 
	{
		if (strcmp(strCST.szCurSymbol, "MMK") == 0)
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
		else
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
	}*/
			
	
				
#endif	


	    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
	    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
	    memset(szBuf, 0x00, sizeof (szBuf));
	    //format amount 10+2 

	    if (TRUE == fGetCashAdvAppFlag()) 
		{ //@@IBR ADD 05102016
	        ulTotalAmount = ulCashAdvTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount;

			sprintf(szBuf, "%.0f", (ulCashAdvTotalAmount > ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) ? (ulCashAdvTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) : (ulRefundTotalAmount - ulCashAdvTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount));

			if (ulTotalAmount == 0)
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
	        else
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
	            
	    } 
		else 
		{
	        if (memcmp(srTransRec.szHostLabel, "UPI", 3) == 0) 
			{
	            ulTotalAmount = ulCUPSaleTotalAmount - ulRefundTotalAmount;
	            sprintf(szBuf, "%.0f", (ulCUPSaleTotalAmount > ulRefundTotalAmount) ? (ulCUPSaleTotalAmount - ulRefundTotalAmount) : (ulRefundTotalAmount - ulCUPSaleTotalAmount));
	            if (ulTotalAmount == 0)
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
	            else
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
	        } 
			else 
			{
				#ifdef CASH_ADV_NEW_FEATURE
		            ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount;

					sprintf(szBuf, "%.0f", (ulSaleTotalAmount > ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount) ? (ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount));

					if (ulTotalAmount == 0)
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
		            else
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
					
	            #else
		            ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount;

					sprintf(szBuf, "%.0f", (ulSaleTotalAmount > ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) ? (ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount));

					if (ulTotalAmount == 0)
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
		            else
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
				
	            #endif
	        }

	    }

	    memset(szBuf, 0x00, sizeof (szBuf));

	    if (TRUE == fGetCashAdvAppFlag()) { //@@IBR ADD 05102016
	        if (ulCashAdvTotalAmount < ulRefundTotalAmount)
	            sprintf(szBuf, "-%s", szAmtBuf);
	        else
	            sprintf(szBuf, "%s", szAmtBuf);
	    } else {
	        if (memcmp(srTransRec.szHostLabel, "UPI", 3) == 0) {
	            if (ulCUPSaleTotalAmount < ulRefundTotalAmount)
	                sprintf(szBuf, "-%s", szAmtBuf);
	            else
	                sprintf(szBuf, "%s", szAmtBuf);
	        } else {
	            if (ulSaleTotalAmount < ulRefundTotalAmount)
	                sprintf(szBuf, "-%s", szAmtBuf);
	            else
	                sprintf(szBuf, "%s", szAmtBuf);
	        }

	    }
	

    if (TRUE == fGetCashAdvAppFlag()) { //@@IBR ADD 05102016
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szBuf);
        memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
        sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "TOTAL:", 20 - (strlen("TOTAL:")), szTemp);
    } 
	else 
	{
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szBuf, 0x00, sizeof (szBuf));
        memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
        memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));

        if (memcmp(srTransRec.szHostLabel, "UPI", 3) == 0) {
            ulTotalAmount = 0;
            ulTotalAmount = ulCUPSaleTotalAmount - ulRefundTotalAmount;
            if (ulTotalAmount > 0) {
                sprintf(szBuf, "%.0f", ulTotalAmount);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
            } else
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        } 
		else 
		{
            ulTotalAmount = 0;
			#ifdef CASH_ADV_NEW_FEATURE			
            ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount;
			#else
            ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount;			
			#endif

			
            if (ulTotalAmount > 0) {
                sprintf(szBuf, "%.0f", ulTotalAmount);

                if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                    memset(szTemp, 0x00, sizeof (szTemp));
                    sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
                    memset(szBuf, 0x00, sizeof (szBuf));
                    strcpy(szBuf, szTemp);
                }

                if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
                } else {
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
                }
            } else {

                if (strcmp(strCST.szCurSymbol, "MMK") == 0)
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
                else
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
            }
        }


        vdDebug_LogPrintf("szAmtBuf [%s]", szAmtBuf);

        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
        sprintf(szDisplayBuf, "%s %s", "TOTAL:", szTemp);
        vdDebug_LogPrintf("szDisplayBuf [%s]", szDisplayBuf);
    }

    CTOS_LCDTPrintXY(1, 7, szDisplayBuf);

    setLCDPrint(8, DISPLAY_POSITION_LEFT, "Press any key to exit");
    CTOS_KBDBufFlush(); //cleare key buffer
    while (1) {
        CTOS_KBDHit(&key);
        if (key == d_KBD_ENTER) {
            inResult = d_OK;
            break;
        }

        if ((key == d_KBD_CANCEL)) {
            inResult = d_NO;
            // vdSetErrorMessage("USER CANCEL");
            break;
        }
    }

    CTOS_LCDTSelectFontSize(d_FONT_16x30);

    return inResult;
}

int inCTOS_DisplayBatchTotalEx(void) {
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount;
    DOUBLE ulSaleTotalAmount;
    USHORT usRefundCount;
    DOUBLE ulRefundTotalAmount;
    DOUBLE ulTotalAmount;
    ACCUM_REC srAccumRec;
    USHORT usCashAdvanceCount;
    DOUBLE ulCashAdvTotalAmount;
    USHORT usCUPSaleCount;
    DOUBLE ulCUPSaleTotalAmount;
    int inECRTransactionStatus;
    USHORT usCBPaySaleCount;
    DOUBLE ulCBPaySaleTotalAmount;

//#ifdef OK_DOLLAR_FEATURE
#if 1
    USHORT usOKDSaleCount;
    DOUBLE ulOKDSaleTotalAmount;
#endif

    BYTE key;
    char szTemp[20 + 1];

    extern BOOL fIsSettleAll;

    //        char szTemp[20+1];

	vdDebug_LogPrintf("inCTOS_DisplayBatchTotalEx START fIPPGetSettleFlag[%d] fIsSettleAll[%d]", fIPPGetSettleFlag(), fIsSettleAll);

    if (get_env_int("NERASGERM") == 1)
        return d_OK;

    inECRTransactionStatus = get_env_int("ECRTRANS");

    CTOS_LCDTClearDisplay();

    if (strTCT.fMustAutoSettle == TRUE)
        return d_OK;

    if (1 == fIsSettleAll){
		vdSetSettleIPPFlag(FALSE); // termp, for testing. allways followed IPP setlement outline if settle was made on IDLE settlement menu.
        return d_OK;
    }

    vdDispTransTitle(srTransRec.byTransType);

    CTOS_LCDTSelectFontSize(d_FONT_12x24);

    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "SETTLE : %s", strHDT.szHostLabel);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, szTemp);

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    if ((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR) {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;
    }

    /*CardBiz no need display this*/
    if (get_env_int("#CUSTNSR") == 1)
        return d_OK;

    vdDebug_LogPrintf("inCTOS_DisplayBatchTotalEx.fGetMPUTrans[%d]", fGetMPUTrans());
    //vdDebug_PrintOnPaper("fGetMPUTrans[%d]", fGetMPUTrans());

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;
    inCSTRead(strHDT.inCurrencyIdx);
    if (VS_TRUE == fGetMPUTrans()) {
        inTranCardType = 1;
    }

    /*if it is IPP host*/
    if (srTransRec.HDTid == 6) {
        inTranCardType = 0;
        inCSTRead(1); // IPP only for MMK
    }

    if (srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23 || srTransRec.HDTid == 20)
    {
        inTranCardType = 1;

        if (srTransRec.HDTid == 17 || srTransRec.HDTid == 23)
            inCSTRead(1);
	else if(srTransRec.HDTid == 22)		
		inCSTRead(2);
    }

#ifdef CBPAY_DV
	if(strHDT.inHostIndex == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
		inCSTRead(1);		
#endif

    vdDebug_LogPrintf("inTranCardType[%d]", inTranCardType);

    if (TRUE == fGetCashAdvAppFlag()) {
        inTranCardType = 0;
    }

    vdDebug_LogPrintf("inTranCardType[%d]", inTranCardType);

    vdDebug_LogPrintf("srTransRec.HDTid[%d] inTranCardType[%d]", srTransRec.HDTid, inTranCardType);

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
	if(srTransRec.HDTid == 20)
	{
		ulSaleTotalAmount += (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTopupTotalAmount + srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRepaymentTotalAmount);
	}
	
    vdDebug_LogPrintf("[inCTOS_DisplayBatchTotalEx] ulSaleTotalAmount[%f]usSaleCount[%f]", ulSaleTotalAmount, usSaleCount);

    //@@IBR ADD 05102016
    usCashAdvanceCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount);
    ulCashAdvTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount);
    //@@IBR FINISH ADD 05102016

    vdDebug_LogPrintf("[inCTOS_DisplayBatchTotalEx] byTransType[%d], byOrgTransType[%d], usCashAdvanceCount[%d], ulCashAdvTotalAmount[%f]", 
    srTransRec.byTransType, srTransRec.byOrgTransType, usCashAdvanceCount, ulCashAdvTotalAmount);

    usCUPSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPSaleCount);
    ulCUPSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPSaleTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

    vdDebug_LogPrintf("[inCTOS_DisplayBatchTotalEx] byTransType[%d], byOrgTransType[%d], usRefundCount[%d], ulRefundTotalAmount[%f]", 
    srTransRec.byTransType, srTransRec.byOrgTransType, usRefundCount, ulRefundTotalAmount);

	//CBPay
    usCBPaySaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCBPaySaleCount);
    ulCBPaySaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount);

#ifdef OK_DOLLAR_FEATURE
		usOKDSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOKDSaleCount);
		ulOKDSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount);
#endif

    vdDebug_LogPrintf("strTCT.inFontFNTMode[%d]", strTCT.inFontFNTMode);
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    if (strTCT.inFontFNTMode != 1 && (strTCT.inThemesType == 2 || strTCT.inThemesType == 3))
        strcpy(szDisplayBuf, "Type   Cnt      Amt");
    else
        strcpy(szDisplayBuf, "Type   Cnt  Amt(N LINE)");
    //    CTOS_LCDTPrintXY(1, 2, szDisplayBuf);


    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));

    if (TRUE == fGetCashAdvAppFlag()) 
	{ //@@IBR ADD 05102016
        if (ulCashAdvTotalAmount > 0) {
            sprintf(szBuf, "%.0f", ulCashAdvTotalAmount);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        } else
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);

        if (strTCT.fECR) {
            put_env_int("#STMTCASHADVCNT", usRefundCount);
            put_env("#STMTCASHADVAMT", szAmtBuf, 40);
        }

        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
        sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "CASH ADV:", 21 - (strlen("CASH ADV:")), szTemp);
        CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
    } 
	else 
	{
        //        if(memcmp(srTransRec.szHostLabel, "CUP", 3) == 0){
        //            if (ulCUPSaleTotalAmount > 0) {
        //                sprintf(szBuf, "%.0f", ulCUPSaleTotalAmount);
        //                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        //            } else
        //                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        //            
        //            if (strTCT.fECR) {
        //                put_env_int("#STMTSALECUPCNT",usCUPSaleCount);
        //                put_env("#STMTSALECUPAMT", szAmtBuf, 40);
        //            }
        //            
        //        } else {

//CASH ADVANCE
		#ifdef CASH_ADV_NEW_FEATURE

		#if 0
        if (ulCashAdvTotalAmount > 0) {
            sprintf(szBuf, "%.0f", ulCashAdvTotalAmount);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        } else
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
		#endif
		
        if (ulCashAdvTotalAmount > 0) 
		{
            sprintf(szBuf, "%.0f", ulCashAdvTotalAmount);

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTemp, 0x00, sizeof (szTemp));
                sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
                memset(szBuf, 0x00, sizeof (szBuf));
                strcpy(szBuf, szTemp);
            }

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
            }

	        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
	        //sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "CASH ADV:", 21 - (strlen("CASH ADV:")), szTemp);
	        sprintf(szDisplayBuf, "%s %s", "CASH ADV:", szTemp);		
	        CTOS_LCDTPrintXY(1, 4, szDisplayBuf);			
        }
		/*else
		{
			if (strcmp(strCST.szCurSymbol, "MMK") == 0)
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
			else
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);			
		}*/
			
        if (strTCT.fECR) {
            put_env_int("#STMTCASHADVCNT", usCashAdvanceCount);
            put_env("#STMTCASHADVAMT", szAmtBuf, 40);
        }

        /*memset(szTemp, 0x00, sizeof (szTemp));
		
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
        //sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "CASH ADV:", 21 - (strlen("CASH ADV:")), szTemp);
        sprintf(szDisplayBuf, "%s %s", "CASH ADV:", szTemp);		
        CTOS_LCDTPrintXY(1, 3, szDisplayBuf);*/
		
		#endif

//SALE
		memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
		memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
		memset(szBuf, 0x00, sizeof (szBuf));

		
        if (ulSaleTotalAmount > 0) 
		{
		
		vdDebug_LogPrintf("inCTOS_DisplayBatchTotalEx XXXXXX");
            sprintf(szBuf, "%.0f", ulSaleTotalAmount);


            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTemp, 0x00, sizeof (szTemp));
                sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
                memset(szBuf, 0x00, sizeof (szBuf));
                strcpy(szBuf, szTemp);
            }

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
            }

			memset(szTemp, 0x00, sizeof (szTemp));
	        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);        
	        sprintf(szDisplayBuf, "%s %s", "SALES:", szTemp);

			
			vdDebug_LogPrintf("inCTOS_DisplayBatchTotalEx szDisplayBuf [%s]", szDisplayBuf);

			if(srTransRec.HDTid != CBPAY_HOST_INDEX && srTransRec.HDTid != OK_DOLLAR_HOST_INDEX)	//don't display for CBPAY	
				CTOS_LCDTPrintXY(1, 5, szDisplayBuf);			
        }
        /*else
		{            
            if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
            else
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        }*/
		

        if (strTCT.fECR) {
            vdDebug_LogPrintf("usSaleCount=[%d]", usSaleCount);
            put_env_int("#STMTSALECNT", usSaleCount);
            put_env("#STMTSALEAMT", szAmtBuf, 40);
            vdDebug_LogPrintf("#STMTSALECNT=[%d]", get_env_int("#STMTSALECNT"));
        }

        /*memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);        
        sprintf(szDisplayBuf, "%s %s", "SALES:", szTemp);
		
		if(srTransRec.HDTid != CBPAY_HOST_INDEX)	//don't display for CBPAY	
			CTOS_LCDTPrintXY(1, 4, szDisplayBuf);*/
	
    }
    //sprintf(szBuf, "%ld", ulSaleTotalAmount);
    //format amount 10+2 
    //    if (ulSaleTotalAmount > 0)
    //    {
    //		sprintf(szBuf, "%.0f", ulSaleTotalAmount);
    //	    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);
    //    }
    //	else
    //		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0",szAmtBuf);
    //    sprintf(szDisplayBuf, "SALE  %04d", usSaleCount);
    //    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
    //	//sprintf(szDisplayBuf, "%s  %16s", strCST.szCurSymbol, szAmtBuf);
    //	//CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
    //	vdCTOSS_DisplayAmount(1,4,strCST.szCurSymbol,szAmtBuf);


//REFUND
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));
    //format amount 10+2 
    if (ulRefundTotalAmount > 0) 
	{
        sprintf(szBuf, "%.0f", ulRefundTotalAmount);

        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
            memset(szTemp, 0x00, sizeof (szTemp));
            sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
            memset(szBuf, 0x00, sizeof (szBuf));
            strcpy(szBuf, szTemp);
        }

        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        }

	    memset(szTemp, 0x00, sizeof (szTemp));
	    sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);

		
		if (fGetCashAdvAppFlag() == TRUE) 
	       sprintf(szDisplayBuf, "%-*s%*s", 20/2, "RFNDS:", 20-(strlen("RFNDS:")), szTemp);
		else
	    	sprintf(szDisplayBuf, "%s %s", "RFNDS:", szTemp);

		
		if(srTransRec.HDTid != CBPAY_HOST_INDEX && srTransRec.HDTid != OK_DOLLAR_HOST_INDEX)	//don't display for CBPAY	
	    CTOS_LCDTPrintXY(1, 6, szDisplayBuf);

    }
	/*else {

        if (strcmp(strCST.szCurSymbol, "MMK") == 0)
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
        else
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
    }*/

    if (strTCT.fECR) {
        put_env_int("#STMTREFUNDCNT", usRefundCount);
        put_env("#STMTREFUNDAMT", szAmtBuf, 40);
    }

    /*memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);

	
	if (fGetCashAdvAppFlag() == TRUE) 
       sprintf(szDisplayBuf, "%-*s%*s", 20/2, "RFNDS:", 20-(strlen("RFNDS:")), szTemp);
	else
    	sprintf(szDisplayBuf, "%s %s", "RFNDS:", szTemp);
	
    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);*/

//FOR CBPay
#if 1
	if (fGetCashAdvAppFlag() == FALSE) 
	{
		memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
		memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
		memset(szBuf, 0x00, sizeof (szBuf));
		
		if (ulCBPaySaleTotalAmount > 0) 
		{
			sprintf(szBuf, "%.0f", ulCBPaySaleTotalAmount);
	
			if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
				memset(szTemp, 0x00, sizeof (szTemp));
				sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
				memset(szBuf, 0x00, sizeof (szBuf));
				strcpy(szBuf, szTemp);
			}
	
			if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
			} else {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
			}
			memset(szTemp, 0x00, sizeof (szTemp));
			sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
			sprintf(szDisplayBuf, "%s %s", "AMOUNT:", szTemp);
			CTOS_LCDTPrintXY(1, 4, szDisplayBuf);			
		} 
		/*else 
		{
	
			if (strcmp(strCST.szCurSymbol, "MMK") == 0)
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
			else
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
		}*/
	
		/*if (strTCT.fECR) {
			put_env_int("#STMTREFUNDCNT", usRefundCount);
			put_env("#STMTREFUNDAMT", szAmtBuf, 40);
		}*/
	
	}
#endif	

#ifdef OK_DOLLAR_FEATURE
				memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
				memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
				memset(szBuf, 0x00, sizeof (szBuf));
				
				if (ulOKDSaleTotalAmount > 0) 
				{
					sprintf(szBuf, "%.0f", ulOKDSaleTotalAmount);
			
					if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
						memset(szTemp, 0x00, sizeof (szTemp));
						sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
						memset(szBuf, 0x00, sizeof (szBuf));
						strcpy(szBuf, szTemp);
					}
			
					if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
					} else {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
					}
					memset(szTemp, 0x00, sizeof (szTemp));
					sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
					sprintf(szDisplayBuf, "%s %s", "AMOUNT:", szTemp);
					CTOS_LCDTPrintXY(1, 4, szDisplayBuf);
					
				} 
				/*else 
				{
			
					if (strcmp(strCST.szCurSymbol, "MMK") == 0)
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
					else
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
				}*/
			
				/*if (strTCT.fECR) {
					put_env_int("#STMTREFUNDCNT", usRefundCount);
					put_env("#STMTREFUNDAMT", szAmtBuf, 40);
				}*/
			
				
#endif	


    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));
    //format amount 10+2 

    if (TRUE == fGetCashAdvAppFlag()) { //@@IBR ADD 05102016
        ulTotalAmount = ulCashAdvTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount;
        sprintf(szBuf, "%.0f", (ulCashAdvTotalAmount > ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) ? (ulCashAdvTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) : (ulRefundTotalAmount - ulCashAdvTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount));
        if (ulTotalAmount == 0)
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        else
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
    } 
	else 
    {
        if (memcmp(srTransRec.szHostLabel, "UPI", 3) == 0) {
            ulTotalAmount = ulCUPSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount;
            sprintf(szBuf, "%.0f", (ulCUPSaleTotalAmount > ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) ? (ulCUPSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) : (ulRefundTotalAmount - ulCUPSaleTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount));
            if (ulTotalAmount == 0)
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
            else
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
        } 
		else 
		{
			#ifdef CASH_ADV_NEW_FEATURE
				ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount;
			
				sprintf(szBuf, "%.0f", (ulSaleTotalAmount > ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount) ? (ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount));
			
				if (ulTotalAmount == 0)
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
				else
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
				
			#else
				ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount;
			
				sprintf(szBuf, "%.0f", (ulSaleTotalAmount > ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) ? (ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount));
			
				if (ulTotalAmount == 0)
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
				else
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
			
			#endif
				
        }




    }
    //	ulTotalAmount = ulSaleTotalAmount-ulRefundTotalAmount;
    //	sprintf(szBuf, "%.0f",  (ulSaleTotalAmount > ulRefundTotalAmount) ? (ulSaleTotalAmount-ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount));
    //	if (ulTotalAmount == 0)
    //		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0",szAmtBuf);
    //	else
    //		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf,szAmtBuf);// patrick add code 20141216		
    memset(szBuf, 0x00, sizeof (szBuf));

    if (TRUE == fGetCashAdvAppFlag()) { //@@IBR ADD 05102016
        if (ulCashAdvTotalAmount < ulRefundTotalAmount)
            sprintf(szBuf, "-%s", szAmtBuf);
        else
            sprintf(szBuf, "%s", szAmtBuf);
    } else {
        if (memcmp(srTransRec.szHostLabel, "UPI", 3) == 0) {
            if (ulCUPSaleTotalAmount < ulRefundTotalAmount)
                sprintf(szBuf, "-%s", szAmtBuf);
            else
                sprintf(szBuf, "%s", szAmtBuf);
        } else {
            if (ulSaleTotalAmount < ulRefundTotalAmount)
                sprintf(szBuf, "-%s", szAmtBuf);
            else
                sprintf(szBuf, "%s", szAmtBuf);
        }

    }
    //    if(ulSaleTotalAmount < ulRefundTotalAmount)
    //        sprintf(szBuf,"-%s",szAmtBuf);
    //	else
    //		sprintf(szBuf,"%s",szAmtBuf);

    if (TRUE == fGetCashAdvAppFlag()) { //@@IBR ADD 05102016
        //        sprintf(szDisplayBuf, "TOTL  %04d", (usCashAdvanceCount + usRefundCount));
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szBuf);
        memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
        sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "TOTAL:", 20 - (strlen("TOTAL:")), szTemp);
    } 
	else 
	{
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szBuf, 0x00, sizeof (szBuf));
        memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
        memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));

        if (memcmp(srTransRec.szHostLabel, "UPI", 3) == 0) {
            ulTotalAmount = 0;
            ulTotalAmount = ulCUPSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount;
            if (ulTotalAmount > 0) {
                sprintf(szBuf, "%.0f", ulTotalAmount);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
            } else
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        } 
		else 
		{
            ulTotalAmount = 0;

			
			#ifdef CASH_ADV_NEW_FEATURE
            ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulCashAdvTotalAmount + ulOKDSaleTotalAmount;
			#else
            ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount + ulCBPaySaleTotalAmount + ulOKDSaleTotalAmount;			
			#endif
			
            if (ulTotalAmount > 0) {
                sprintf(szBuf, "%.0f", ulTotalAmount);

                if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                    memset(szTemp, 0x00, sizeof (szTemp));
                    sprintf(szTemp, "%012.0f", atof(szBuf) / 100);
                    memset(szBuf, 0x00, sizeof (szBuf));
                    strcpy(szBuf, szTemp);
                }

                if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
                } else {
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
                }
                //                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
            } else {

                if (strcmp(strCST.szCurSymbol, "MMK") == 0)
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
                else
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
            }
            //                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        }


        vdDebug_LogPrintf("szAmtBuf [%s]", szAmtBuf);

        sprintf(szTemp, "%s %s", strCST.szCurSymbol, szAmtBuf);
        //        sprintf(szDisplayBuf, "%-*s%*s", 20/2, "TOTAL:", 20-(strlen("TOTAL:"))-1, szTemp);
        sprintf(szDisplayBuf, "%s %s", "TOTAL:", szTemp);
        vdDebug_LogPrintf("szDisplayBuf [%s]", szDisplayBuf);
        //        ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount;
        //        memset(szTemp, 0x00, sizeof(szTemp));
        //        memset(szBuf, 0x00, sizeof(szBuf));
        //        sprintf(szBuf, "%ul", ulTotalAmount);
        //        sprintf(szTemp, "%s %s",strCST.szCurSymbol, szBuf);
        //        sprintf(szDisplayBuf, "%-*s%*s", 20/2, "TOTL:", 20-(strlen("TOTL:")), szTemp);
        //        sprintf(szDisplayBuf, "TOTL  %04d", (usSaleCount + usRefundCount));
    }

    CTOS_LCDTPrintXY(1, 7, szDisplayBuf);
    //    sprintf(szDisplayBuf, "TOTL  %04d", (usSaleCount + usRefundCount));
    //    CTOS_LCDTPrintXY(1, 7, szDisplayBuf);
    //	vdCTOSS_DisplayAmount(1,8,strCST.szCurSymbol,szBuf);
    //sprintf(szDisplayBuf, "%s%17s", strCST.szCurSymbol, szBuf);
    //CTOS_LCDTPrintXY(1, 8, szDisplayBuf);

    //WaitKey(2);

    setLCDPrint(8, DISPLAY_POSITION_LEFT, "ENTER TO CONFIRM");
    CTOS_KBDBufFlush(); //cleare key buffer
    while (1) {
        CTOS_KBDHit(&key);
        if (key == d_KBD_ENTER) {
            inResult = d_OK;
            break;
        }

        if ((key == d_KBD_CANCEL)) {
            inResult = d_NO;
            vdSetErrorMessage("USER CANCEL");
            break;
        }
    }

    CTOS_LCDTSelectFontSize(d_FONT_16x30);

    return inResult;
}

int inCTOS_DisplayBatchRecordDetail(int inType) {
    BYTE szTotalAmount[12 + 1];
    BYTE szAmtOut[30 + 1];
    BYTE szINV[6 + 1];
    BYTE szDisplayBuf[40 + 1];
    BYTE szTitle[16 + 1];

    vdDebug_LogPrintf("inCTOS_DisplayBatchRecordDetail");
    vdDebug_LogPrintf("HDTid [%d] fGetMPUTrans [%d] IITid [%d]", srTransRec.HDTid, fGetMPUTrans(), srTransRec.IITid);


    CTOS_LCDTClearDisplay();

    vdDispTransTitle(inType);

    inIITRead(srTransRec.IITid);

    memset(szTitle, 0x00, sizeof (szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

	memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));

	#if 0
	//#ifdef CBB_FIN_ROUTING
	vdDebug_LogPrintf("inCTOS_DisplayBatchRecordDetail CBB_FIN_ROUTING");
	
		if(srTransRec.HDTid == 23)
			strcpy(strIIT.szIssuerLabel, "FIN-MMK");
		else if(srTransRec.HDTid == 22)
			strcpy(strIIT.szIssuerLabel, "FIN-USD");			
	#endif

	//http://118.201.48.210:8080/redmine/issues/1525.50.5
	if(srTransRec.IITid != 10 && srTransRec.IITid != 11)
	    sprintf(szDisplayBuf, "%s   %s", szTitle, strIIT.szIssuerLabel);
	else
	    sprintf(szDisplayBuf, "%s", szTitle);

	CTOS_LCDTPrintXY(1, 3, szDisplayBuf);

    memset(szDisplayBuf, 0, sizeof (szDisplayBuf));
    vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szDisplayBuf);
    CTOS_LCDTPrintXY(1, 4, szDisplayBuf);

    memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    vdDebug_LogPrintf("szTotalAmount[%s]", szTotalAmount);
    //format amount 10+2
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, szAmtOut);

	
    vdDebug_LogPrintf("szTotalAmount[%s]szCurSymbol[%s]", szDisplayBuf, strCST.szCurSymbol);


// for issue raised by Khaing 12/16/19
// fix for case #1346 122319																							//http://118.201.48.210:8080/redmine/issues/1525.115
	if(srTransRec.HDTid == CBPAY_HOST_INDEX || srTransRec.HDTid == 17 || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX || srTransRec.HDTid == 6 ||
		srTransRec.HDTid == 23){
		inCSTRead(1);
	}
	else
	{
       	 inHDTRead(strHDT.inHostIndex);
		 vdDebug_LogPrintf("inCTOS_DisplayBatchRecordDetail MMMM strHDT.inCurrencyIdx[%d]", strHDT.inCurrencyIdx);

		 if(srTransRec.HDTid == 22)
			strHDT.inCurrencyIdx = 2;
		 
		inCSTRead(strHDT.inCurrencyIdx);		
	}
	
    memset(szDisplayBuf, 0, sizeof (szDisplayBuf));
    //sprintf(szDisplayBuf, "%s  %16s", strCST.szCurSymbol, szAmtOut);
    //CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
    //format amount 10+2
    vdCTOSS_DisplayAmount(1, 5, strCST.szCurSymbol, szAmtOut);

    memset(szINV, 0x00, sizeof (szINV));
    wub_hex_2_str(srTransRec.szInvoiceNo, szINV, 3);
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "Invoice:  %ld", atol(szINV));
    CTOS_LCDTPrintXY(1, 6, szDisplayBuf);

    vdCTOS_LCDGShowUpDown(1, 1);

    return d_OK;
}

int inCTOSS_DeleteAdviceByINV(BYTE *szInvoiceNo) {
    int inResult, inUpDateAdviceIndex;
    TRANS_DATA_TABLE srAdvTransTable;
    STRUCT_ADVICE strAdvice;

    memset((char *) &srAdvTransTable, 0, sizeof (TRANS_DATA_TABLE));
    memset((char *) &strAdvice, 0, sizeof (strAdvice));

    memcpy((char *) &srAdvTransTable, (char *) &srTransRec, sizeof (TRANS_DATA_TABLE));

    inResult = ST_SUCCESS;
    inUpDateAdviceIndex = 0;
    while (1) {
        inResult = inMyFile_AdviceReadByIndex(inUpDateAdviceIndex, &strAdvice, &srAdvTransTable);

        vdDebug_LogPrintf("ADVICE inUpDateAdviceIndex[%d] inMyFile_AdviceRead Rult(%d)(%d)(%d)(%d)", inUpDateAdviceIndex, inResult, srAdvTransTable.ulTraceNum, srAdvTransTable.byPackType, strAdvice.byTransType);

        if (inResult == ST_ERROR || inResult == RC_FILE_READ_OUT_NO_DATA) {
            inResult = ST_SUCCESS;
            break;
        }

        vdDebug_LogPrintf("ulnInvoiceNo[%02X %02X %02X] strAdvice->szInvoiceNo [%02X %02X %02X]", szInvoiceNo[0], szInvoiceNo[1], szInvoiceNo[2], strAdvice.szInvoiceNo[0], strAdvice.szInvoiceNo[1], strAdvice.szInvoiceNo[2]);
        if (0 != memcmp(szInvoiceNo, strAdvice.szInvoiceNo, INVOICE_BCD_SIZE)) {
            inUpDateAdviceIndex++;
            continue;
        } else {
            srAdvTransTable.byUploaded = CN_TRUE;
            inResult = inMyFile_AdviceUpdate(inUpDateAdviceIndex);
            break;
        }

    }

    return ST_SUCCESS;
}

int inCTOSS_BatchCheckDuplicateInvoice(void) {
    int inRet = d_NO;

	vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice");

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
		vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice-1");
		
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_CHECK_DUP_INV);

        vdSetErrorMessage("");
        if (d_OK != inRet)
            return inRet;
    } else {
		vdDebug_LogPrintf("inCTOSS_BatchCheckDuplicateInvoice-2");
    
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetVoid();
            if (d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_BatchSearch();

        vdSetErrorMessage("");
        if (d_OK != inRet)
            return inRet;
    }

    return inRet;
}

void vdCTOSS_GetBatchTotal(DOUBLE *ulSaleTotalAmount, DOUBLE *ulRefundTotalAmount, DOUBLE *ulTotalAmount) {
    int inResult;
    int inTranCardType;
    ACCUM_REC srAccumRec;

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    if ((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR) {
        vdDebug_LogPrintf("inCTOSS_GetBatchTotal---Read Total Rec. error");
        return;
    }

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;

    *ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);

    *ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

    *ulTotalAmount = (*ulSaleTotalAmount)-(*ulRefundTotalAmount);

    return;
}

int inCTOSS_CheckExceedMaxAMT(char *szAmount) {
    DOUBLE ulSaleTotalAmount = 0;
    DOUBLE ulRefundTotalAmount = 0;
    DOUBLE ulTotalAmount = 0;

    DOUBLE ultmpTotalAmt = 0;

    DOUBLE LONG ulMAXAmt = (DOUBLE LONG) MAX_AMOUNT;

    ultmpTotalAmt = atof(szAmount);

    vdCTOSS_GetBatchTotal(&ulSaleTotalAmount, &ulRefundTotalAmount, &ulTotalAmount);
    vdDebug_LogPrintf("inCTOSS_CheckExceedMaxAMT ultmpTotalAmt=[%f],ulSaleTotalAmount=[%f],ulRefundTotalAmount=[%f],ulTotalAmount=[%f]", ultmpTotalAmt, ulSaleTotalAmount, ulRefundTotalAmount, ulTotalAmount);


    if (ultmpTotalAmt + ulTotalAmount >= ulMAXAmt)
        return d_NO;

    if (srTransRec.byTransType == SALE)
        if (ultmpTotalAmt + ulSaleTotalAmount >= ulMAXAmt)
            return d_NO;

    if (srTransRec.byTransType == REFUND)
        if (ultmpTotalAmt + ulRefundTotalAmount >= ulMAXAmt)
            return d_NO;

	vdDebug_LogPrintf("inCTOSS_CheckExceedMaxAMT END");

    return d_OK;
}

int inMPU_DisplayBatchTotalEx(void) {
    int inResult;
    int inTranCardType;
    BYTE szDisplayBuf[40];
    BYTE szAmtBuf[40];
    BYTE szBuf[40];
    USHORT usSaleCount;
    DOUBLE ulSaleTotalAmount;
    USHORT usVoidSaleCount;
    DOUBLE ulVoidSaleTotalAmount;
    USHORT usRefundCount;
    DOUBLE ulRefundTotalAmount;
    DOUBLE ulTotalAmount;
    ACCUM_REC srAccumRec;
    USHORT usCashAdvanceCount;
    DOUBLE ulCashAdvTotalAmount;
    USHORT usCUPSaleCount;
    DOUBLE ulCUPSaleTotalAmount;
    USHORT usCUPPreAuthCount;
    DOUBLE ulCUPPreAuthTotalAmount;
    USHORT usOfflineSaleCount;
    DOUBLE ulOfflineSaleTotalAmount;
    BYTE key;
    char szTemp[20 + 1];


    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

    CTOS_LCDTSelectFontSize(d_FONT_12x24);

	vdDebug_LogPrintf("inMPU_DisplayBatchTotalEx fGetMPUTrans[%d]strHDT.inCurrencyIdx[%d]strHDT.inHostIndex[%d]", 
		fGetMPUTrans(), strHDT.inCurrencyIdx, strHDT.inHostIndex);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "SETTLE: %s", strHDT.szHostLabel);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, szDisplayBuf);

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    if ((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR) {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;
    }

    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;

    inCSTRead(strHDT.inCurrencyIdx);
    if (VS_TRUE == fGetMPUTrans()) {
        inTranCardType = 1;
        if (strHDT.inHostIndex == 17 || srTransRec.HDTid == 23) { //marco 20170704
            inCSTRead(1);
        }
    }


	vdDebug_LogPrintf("inMPU_DisplayBatchTotalEx inTranCardType[%d]", inTranCardType);

    usSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    ulSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);

    usVoidSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount);
    ulVoidSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);

    usCUPPreAuthCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPPreAuthCount);
    ulCUPPreAuthTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount);

    usRefundCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
    ulRefundTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);

    usOfflineSaleCount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount);
    ulOfflineSaleTotalAmount = (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);

    vdDebug_LogPrintf("usSaleCount[%d] ulSaleTotalAmount[%f]", usSaleCount, ulSaleTotalAmount);
    vdDebug_LogPrintf("usVoidSaleCount[%d] ulVoidSaleTotalAmount[%f]", usSaleCount, ulVoidSaleTotalAmount);
    vdDebug_LogPrintf("usRefundCount[%d] ulRefundTotalAmount[%f]", usRefundCount, ulRefundTotalAmount);
    vdDebug_LogPrintf("usCUPSaleCount[%d] ulCUPSaleTotalAmount[%f]", usCUPSaleCount, ulCUPSaleTotalAmount);
    vdDebug_LogPrintf("usCUPPreAuthCount[%d] ulCUPPreAuthTotalAmount[%f]", usCUPPreAuthCount, ulCUPPreAuthTotalAmount);

    vdDebug_LogPrintf("strTCT.inFontFNTMode[%d]", strTCT.inFontFNTMode);
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    if (strTCT.inFontFNTMode != 1 && (strTCT.inThemesType == 2 || strTCT.inThemesType == 3))
        strcpy(szDisplayBuf, "Type   Cnt      Amt");
    else
        strcpy(szDisplayBuf, "Type   Cnt  Amt(N LINE)");
    //    CTOS_LCDTPrintXY(1, 2, szDisplayBuf);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));

    if (usCUPSaleCount > 0)
        ulSaleTotalAmount += ulCUPSaleTotalAmount;

    //    if ((ulSaleTotalAmount-ulVoidSaleTotalAmount) > 0) //@@IBR note: why this condition still substract the sale total amount even though the sale total amount already been calculated in accum.c?
    if ((ulSaleTotalAmount) > 0) {
        //        sprintf(szBuf, "%.0f", ulSaleTotalAmount-ulVoidSaleTotalAmount);

        if (strCST.inCurrencyIndex == 1) {
            sprintf(szBuf, "%.0f", ulSaleTotalAmount / 100);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
        } else {
            sprintf(szBuf, "%.0f", ulSaleTotalAmount);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        }
    } else {
        if (strCST.inCurrencyIndex == 1) {
            sprintf(szBuf, "%d", 0);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
        } else {
            sprintf(szBuf, "%d", 0);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        }
    }

    //    sprintf(szDisplayBuf, "SALES  %04d", usSaleCount-usVoidSaleCount);
    //    CTOS_LCDTPrintXY(1, 3, szDisplayBuf);
    //
    //    vdCTOSS_DisplayAmount(1, 4, strCST.szCurSymbol, szAmtBuf);
    memset(szBuf, 0x00, sizeof (szBuf));
    sprintf(szBuf, "%s %s", strCST.szCurSymbol, szAmtBuf);
    //    sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "SALES:", 20 - (strlen("SALES:")), szBuf);
    sprintf(szDisplayBuf, "%s %s", "SALES:", szBuf);

    if (strHDT.inHostIndex == 19) {
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szDisplayBuf);
    } else {
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szDisplayBuf);
    }

    if (strHDT.inHostIndex == 19) {
        memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
        memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
        memset(szBuf, 0x00, sizeof (szBuf));
        if ((ulCUPPreAuthTotalAmount) > 0) {
            sprintf(szBuf, "%.0f", ulCUPPreAuthTotalAmount);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        } else {
            sprintf(szBuf, "%d", 0);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        }
        memset(szBuf, 0x00, sizeof (szBuf));
        sprintf(szBuf, "%s %s", strCST.szCurSymbol, szAmtBuf);
        //        sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "PRE AUTH:", 20 - (strlen("PRE AUTH:")), szBuf);
        sprintf(szDisplayBuf, "%s %s", "PRE AUTH:", szBuf);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szDisplayBuf);
    }



    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));
    //format amount 10+2 
    if (ulRefundTotalAmount > 0) {
        if (strCST.inCurrencyIndex == 1) {
            sprintf(szBuf, "%.0f", ulRefundTotalAmount / 100);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
        } else {
            sprintf(szBuf, "%.0f", ulRefundTotalAmount);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        }

    } else {
        if (strCST.inCurrencyIndex == 1) {
            sprintf(szBuf, "%d", 0);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf);
        } else {
            sprintf(szBuf, "%d", 0);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf);
        }

    }

    //    sprintf(szDisplayBuf, "RFNDS  %04d", usRefundCount);
    //    CTOS_LCDTPrintXY(1, 5, szDisplayBuf);
    //
    //    vdCTOSS_DisplayAmount(1, 6, strCST.szCurSymbol, szAmtBuf);
    memset(szBuf, 0x00, sizeof (szBuf));
    sprintf(szBuf, "%s %s", strCST.szCurSymbol, szAmtBuf);
    //    sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "RFNDS:", 20 - (strlen("RFNDS:")), szBuf);
    sprintf(szDisplayBuf, "%s %s", "RFNDS:", szBuf);
    if (strHDT.inHostIndex == 19) {
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    } else {
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szDisplayBuf);
    }
    //    setLCDPrint(5, DISPLAY_POSITION_LEFT, szDisplayBuf);

    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    memset(szBuf, 0x00, sizeof (szBuf));
    //format amount 10+2 

    //    ulTotalAmount = ulSaleTotalAmount - ulRefundTotalAmount;
    ulTotalAmount = (ulSaleTotalAmount + ulCUPPreAuthTotalAmount) - ulRefundTotalAmount;
    //    sprintf(szBuf, "%.0f", (ulSaleTotalAmount > ulRefundTotalAmount) ? (ulSaleTotalAmount - ulRefundTotalAmount) : (ulRefundTotalAmount - ulSaleTotalAmount));
    //    sprintf(szBuf, "%.0f", ((ulSaleTotalAmount+ulCUPPreAuthTotalAmount) > ulRefundTotalAmount) ? ((ulSaleTotalAmount+ulCUPPreAuthTotalAmount) - ulRefundTotalAmount) : (ulRefundTotalAmount - (ulSaleTotalAmount+ulCUPPreAuthTotalAmount)));

    if (ulTotalAmount == 0) {
        if (strCST.inCurrencyIndex == 1) {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", "0", szAmtBuf);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szAmtBuf);
        }

    } else {
        if (strCST.inCurrencyIndex == 1) {
            sprintf(szBuf, "%.0f", (((ulSaleTotalAmount + ulCUPPreAuthTotalAmount) > ulRefundTotalAmount) ? ((ulSaleTotalAmount + ulCUPPreAuthTotalAmount) - ulRefundTotalAmount) : (ulRefundTotalAmount - (ulSaleTotalAmount + ulCUPPreAuthTotalAmount))) / 100); //marco 20170704
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szBuf, szAmtBuf); // patrick add code 20141216
        } else {
            sprintf(szBuf, "%.0f", (((ulSaleTotalAmount + ulCUPPreAuthTotalAmount) > ulRefundTotalAmount) ? ((ulSaleTotalAmount + ulCUPPreAuthTotalAmount) - ulRefundTotalAmount) : (ulRefundTotalAmount - (ulSaleTotalAmount + ulCUPPreAuthTotalAmount)))); //marco 20170704
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szBuf, szAmtBuf); // patrick add code 20141216		
        }

    }


    memset(szBuf, 0x00, sizeof (szBuf));

    if (ulSaleTotalAmount < ulRefundTotalAmount)
        sprintf(szBuf, "-%s", szAmtBuf);
    else
        sprintf(szBuf, "%s", szAmtBuf);

    //    sprintf(szDisplayBuf, "TOTL  %04d", (usSaleCount - usVoidSaleCount + usRefundCount));
    //    
    //    CTOS_LCDTPrintXY(1, 7, szDisplayBuf);
    memset(szAmtBuf, 0x00, sizeof (szAmtBuf));
    sprintf(szAmtBuf, "%s %s", strCST.szCurSymbol, szBuf);
    //    sprintf(szDisplayBuf, "%-*s%*s", 20 / 2, "TOTAL:", 20 - (strlen("TOTAL:")), szAmtBuf);
    sprintf(szDisplayBuf, "%s %s", "TOTAL:", szAmtBuf);
    if (strHDT.inHostIndex == 19) {
        setLCDPrint(7, DISPLAY_POSITION_LEFT, szDisplayBuf);
    } else {
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    }
    //    setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);

    if (strHDT.inHostIndex == 19) {
        setLCDPrint(9, DISPLAY_POSITION_LEFT, "ENTER TO CONFIRM");
    } else {
        setLCDPrint(8, DISPLAY_POSITION_LEFT, "ENTER TO CONFIRM");
    }

    //    setLCDPrint(8, DISPLAY_POSITION_LEFT, "ENTER TO CONFIRM");
    CTOS_KBDBufFlush(); //cleare key buffer
    while (1) {
        CTOS_KBDHit(&key);
        if (key == d_KBD_ENTER) {
            inResult = d_OK;
            break;
        }

        if ((key == d_KBD_CANCEL)) {
            inResult = d_NO;
            vdSetErrorMessage("USER CANCEL");
            break;
        }
    }

    CTOS_LCDTSelectFontSize(d_FONT_16x30);

    return inResult;
}

int inCTOS_EnableManualEntryMode(void) {
    int i = 0;

    CTOS_LCDTClearDisplay();
}

void vdCTOS_SetDateTrans(void) {
    char szTemp1[6 + 1];
    CTOS_RTC SetRTC;

    memset(&SetRTC, 0x00, sizeof (CTOS_RTC));
    CTOS_RTCGet(&SetRTC);
    if (memcmp(srTransRec.szDate, "\x00\x00", 2) == 0) {
        memset(szTemp1, 0x00, sizeof (szTemp1));
        sprintf(szTemp1, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);

        wub_str_2_hex(szTemp1, srTransRec.szDate, 4);
    }

    if (memcmp(srTransRec.szTime, "\x00\x00\x00", 3) == 0) {
        memset(szTemp1, 0x00, sizeof (szTemp1));
        sprintf(szTemp1, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
        wub_str_2_hex(szTemp1, srTransRec.szTime, 6);
    }

    if (memcmp(srTransRec.szYear, "\x00\x00", 2) == 0) {
        memset(szTemp1, 0x00, sizeof (szTemp1));
        sprintf(szTemp1, "%04d", SetRTC.bYear);
        wub_str_2_hex(szTemp1, srTransRec.szYear, 4);
    }

    return;
}

void vdCTOS_SetNextAutoSettle(void) {
    CTOS_RTC setRTC;
    int inDayNow = 0, inMonthNow = 0, inYearNow = 0;
    int inDay = 0, inHour = 0;
    char chNextTimeSettle[14 + 1];
    int inYearSet = 0, inMonthSet = 0, inDaySet = 0;

    memset(&setRTC, 0x00, sizeof (CTOS_RTC));
    CTOS_RTCGet(&setRTC);

    inTCTRead(1);

    //    if(strTCT.inAutoSettleIntervalHour == 0)
    //        return;

    inYearNow = setRTC.bYear + 2000;
    inMonthNow = setRTC.bMonth;
    inDayNow = setRTC.bDay;
    inHour = setRTC.bHour;

    //    inHour += strTCT.inAutoSettleIntervalHour;

    inDayNow += 1;

    //    if (inHour >= 24) {
    if (inDayNow >= 28) {
        //        inHour -= 24;
        //        inDayNow += 1;

        if (inMonthNow == 1 || inMonthNow == 3 || inMonthNow == 5 || inMonthNow == 7 || inMonthNow == 8 || inMonthNow == 10 || inMonthNow == 12) {
            if (inDay > 31) {
                inDayNow = 1;
                inMonthNow += 1;

                if (inMonthNow > 12) {
                    inMonthNow = 1;
                    inYearNow += 1;
                }

                inDaySet = inDayNow;
                inMonthSet = inMonthNow;
                inYearSet = inYearNow;
            } else {
                inDaySet = inDayNow;
                inMonthSet = inMonthNow;
                inYearSet = inYearNow;
            }
        } else if (inMonthNow == 2 && ((inYearNow % 4) == 0)) {
            if (inDayNow > 29) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        } else if (inMonthNow == 2 && ((inYearNow % 4) != 0)) {
            if (inDayNow > 28) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        } else if (inMonthNow == 4 || inMonthNow == 6 || inMonthNow == 9 || inMonthNow == 11) {
            if (inDayNow > 30) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        }

        inDaySet = inDayNow;
        inMonthSet = inMonthNow;
        inYearSet = inYearNow;
    } else {
        inDaySet = inDayNow;
        inMonthSet = inMonthNow;
        inYearSet = inYearNow;
    }

    memset(chNextTimeSettle, 0x00, sizeof (chNextTimeSettle));
    //    sprintf(chNextTimeSettle, "%04d%02d%02d%04d%02d", inYearSet, inMonthSet, inDaySet, strTCT.inAutoSettleIntervalHour, 0);
    vdDebug_LogPrintf("Year Month Date = %d %d %d", inYearSet, inMonthSet, inDaySet);
    sprintf(chNextTimeSettle, "%04d%02d%02d%s%02d", inYearSet, inMonthSet, inDaySet, strTCT.szAutoSettleIntervalHour, 0);
    //    sprintf(chNextTimeSettle, "%04d%02d%02d%02d%02d%02d", inYearSet, inMonthSet, inDaySet, inHour, setRTC.bMinute, setRTC.bSecond);
    memcpy(strTCT.chNextSettleTime, chNextTimeSettle, 14);
    inTCTSave(1);

}

void vdSetDateForAutoSettle(int inYearIn, int inMonthIn, int inDateIn, int inYearOut, int inMonthOut, int inDateOut) {

    int inDayNow = 0, inMonthNow = 0, inYearNow = 0;
    int inYearSet = 0, inMonthSet = 0, inDaySet = 0;

    inYearNow = inYearIn;
    inMonthNow = inMonthIn;
    inDayNow = inDateIn;

    inDayNow += 1;

    //    if (inHour >= 24) {
    if (inDayNow >= 28) {
        //        inHour -= 24;
        //        inDayNow += 1;

        if (inMonthNow == 1 || inMonthNow == 3 || inMonthNow == 5 || inMonthNow == 7 || inMonthNow == 8 || inMonthNow == 10 || inMonthNow == 12) {
            if (inDayNow > 31) {
                inDayNow = 1;
                inMonthNow += 1;

                if (inMonthNow > 12) {
                    inMonthNow = 1;
                    inYearNow += 1;
                }

                inDaySet = inDayNow;
                inMonthSet = inMonthNow;
                inYearSet = inYearNow;
            } else {
                inDaySet = inDayNow;
                inMonthSet = inMonthNow;
                inYearSet = inYearNow;
            }
        } else if (inMonthNow == 2 && ((inYearNow % 4) == 0)) {
            if (inDayNow > 29) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        } else if (inMonthNow == 2 && ((inYearNow % 4) != 0)) {
            if (inDayNow > 28) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        } else if (inMonthNow == 4 || inMonthNow == 6 || inMonthNow == 9 || inMonthNow == 11) {
            if (inDayNow > 30) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        }

        inDaySet = inDayNow;
        inMonthSet = inMonthNow;
        inYearSet = inYearNow;
    } else {
        inDaySet = inDayNow;
        inMonthSet = inMonthNow;
        inYearSet = inYearNow;
    }

    inYearOut = inYearSet;
    inMonthOut = inMonthSet;
    inDateOut = inDaySet;
}

void vdCheckTimeAutoSettle(void) {
    CTOS_RTC setRTC;
    int inDayNow = 0, inMonthNow = 0, inYearNow = 0;
    int inDay = 0, inHour = 0;
    char chTimeNow[14 + 1], chNextTimeSettle[14 + 1];
    DOUBLE dbTimeNext = 0, dbTimeNow = 0;
    int inYearSet = 0, inMonthSet = 0, inDaySet = 0;
    int inCheckFirstSet = 0;

    //    inTCTRead(1);

    //    if(strTCT.inAutoSettleIntervalHour == 0)
    //        return;

    vdDebug_LogPrintf("*** vdCheckTimeAutoSettle ***");

    memset(&setRTC, 0x00, sizeof (CTOS_RTC));
    CTOS_RTCGet(&setRTC);


    inYearNow = setRTC.bYear;
    inMonthNow = setRTC.bMonth;
    inDayNow = setRTC.bDay;
    inHour = setRTC.bHour;

    if (atoi(strTCT.chNextSettleTime) > 0)
        dbTimeNext = atof(strTCT.chNextSettleTime);


    vdDebug_LogPrintf("dbTimeNext = %d", dbTimeNext);

    //    inHour += strTCT.inAutoSettleIntervalHour;

    inDayNow += 1;

    //    if (inHour >= 24) {
    if (inDayNow >= 28) {
        //        inHour -= 24;
        //        inDayNow += 1;

        if (inMonthNow == 1 || inMonthNow == 3 || inMonthNow == 5 || inMonthNow == 7 || inMonthNow == 8 || inMonthNow == 10 || inMonthNow == 12) {
            if (inDayNow > 31) {
                inDayNow = 1;
                inMonthNow += 1;

                if (inMonthNow > 12) {
                    inMonthNow = 1;
                    inYearNow += 1;
                }

                inDaySet = inDayNow;
                inMonthSet = inMonthNow;
                inYearSet = inYearNow;
            } else {
                inDaySet = inDayNow;
                inMonthSet = inMonthNow;
                inYearSet = inYearNow;
            }
        } else if (inMonthNow == 2 && ((inYearNow % 4) == 0)) {
            if (inDayNow > 29) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        } else if (inMonthNow == 2 && ((inYearNow % 4) != 0)) {
            if (inDayNow > 28) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        } else if (inMonthNow == 4 || inMonthNow == 6 || inMonthNow == 9 || inMonthNow == 11) {
            if (inDayNow > 30) {
                inDayNow = 1;
                inMonthNow += 1;
            }

            inDaySet = inDayNow;
            inMonthSet = inMonthNow;
            inYearSet = inYearNow;
        }

        inDaySet = inDayNow;
        inMonthSet = inMonthNow;
        inYearSet = inYearNow;
    } else {
        inDaySet = inDayNow;
        inMonthSet = inMonthNow;
        inYearSet = inYearNow;
    }

    vdDebug_LogPrintf("3 Year Month Date = %d %d %d", inYearSet, inMonthSet, inDaySet);

    if (strlen(strTCT.chNextSettleTime) < 2) {
        vdDebug_LogPrintf("first time [%s]", strTCT.chNextSettleTime);
        inCheckFirstSet = atoi(strTCT.chNextSettleTime);
    }

    //    
    //    if (dbTimeNext <= 0) {
    if (inCheckFirstSet <= 0) {
        vdDebug_LogPrintf("set auto settle first time");
        memset(chNextTimeSettle, 0x00, sizeof (chNextTimeSettle));
        //        sprintf(chNextTimeSettle, "%04d%02d%02d%02d%02d%02d", inYearSet+2000, inMonthSet, inDaySet, inHour, setRTC.bMinute, setRTC.bSecond);
        //        vdSetDateForAutoSettle(inYearNow, inMonthNow, inDayNow, inYearSet, inMonthSet, inDaySet);
        //        sprintf(chNextTimeSettle, "%04d%02d%02d%s%02d", setRTC.bYear+2000, setRTC.bMonth, setRTC.bDay+1, strTCT.szAutoSettleIntervalHour, 0);
        vdDebug_LogPrintf("2 Year Month Date = %d %d %d", inYearSet, inMonthSet, inDaySet);
        sprintf(chNextTimeSettle, "%04d%02d%02d%s%02d", inYearSet + 2000, inMonthSet, inDaySet, strTCT.szAutoSettleIntervalHour, 0);
        memcpy(strTCT.chNextSettleTime, chNextTimeSettle, 14);
        strTCT.fMustAutoSettle = FALSE;
        inTCTSave(1);
        return;
    }


    memset(chTimeNow, 0x00, sizeof (chTimeNow));
    sprintf(chTimeNow, "%04d%02d%02d%02d%02d%02d", setRTC.bYear + 2000, setRTC.bMonth, setRTC.bDay, setRTC.bHour, setRTC.bMinute, setRTC.bSecond);
    dbTimeNow = atof(chTimeNow);

    if (dbTimeNow == dbTimeNext) {
        strTCT.fMustAutoSettle = TRUE;
    } else if (dbTimeNow > dbTimeNext) {
        strTCT.fMustAutoSettle = TRUE;
    } else if (dbTimeNow < dbTimeNext) {
        strTCT.fMustAutoSettle = FALSE;
    }

    //    if(strTCT.fMustAutoSettle == TRUE){
    //        memset(chNextTimeSettle, 0x00, sizeof (chNextTimeSettle));
    //        sprintf(chNextTimeSettle, "%04d%02d%02d%02d%02d%02d", inYearSet+2000, inMonthSet, inDaySet, inHour, setRTC.bMinute, setRTC.bSecond);
    //        memcpy(strTCT.chNextSettleTime, chNextTimeSettle, 14);
    //        inTCTSave(1);
    //    }

}

int inCTOS_CheckAllowCardVer(void) {

    vdDebug_LogPrintf("fCardVerEnable = %d", strIIT.fCardVerEnable);

    if (strIIT.fCardVerEnable == FALSE)
        return d_NO;

    return d_OK;
}

int inCTOS_CheckAllowPreAuth(void) {

    vdDebug_LogPrintf("inCTOS_CheckAllowPreAuth. fPreAuthEnable = %d", strIIT.fPreAuthEnable);

    if (strIIT.fPreAuthEnable == FALSE){
        vdSetErrorMessage("TRANS NOT ALLWD");		
        return d_NO;
    }

    return d_OK;
}

void vdCTOS_Pad_String(char* str, int padlen, char padval, int padtype) {
    int padno;

    if ((padno = padlen - strlen(str)) > 0) {
        if (padtype == POSITION_LEFT)
            memmove(str + padno, str, strlen(str) + 1);
        else
            str += strlen(str);
        memset(str, padval, padno);
        if (padtype == POSITION_RIGHT)
            *(str + padno) = '\0';
    } else if (padno < 0) {
        // Truncate string if too long!!
        memmove(str, str + abs(padno), padlen + 1);
    }
}

int inCTOSS_GetPOSTransRef(void) {
    USHORT usX = 1, usY = 6;
    BYTE baString[100 + 1];
    USHORT iStrLen = 7;
    BYTE bShowAttr = 0x02;
    USHORT usInputLen = 7;
    BYTE szInputAsc[7 + 1];
    BYTE szInputBcd[4];
    BYTE bRet;

    TRANS_DATA_TABLE srTransRecTemp;

    DebugAddSTR("inCTOSS_GetPOSTransRef", "Processing...", 20);

    if (inChkPOSTransRefReq() == TRUE) //reuse existing parameter to prevent change in Amex-Alipay
    {
        memset(szInputAsc, 0x00, sizeof (szInputAsc));
        memset(szInputBcd, 0x00, sizeof (szInputBcd));

        memset((char*) &srTransRecTemp, 0x00, sizeof (TRANS_DATA_TABLE));
        memcpy(&srTransRecTemp, &srTransRec, sizeof (TRANS_DATA_TABLE));

        while (TRUE) {
            vduiClearBelow(2);
            vdDispTransTitle(srTransRecTemp.byTransType);
            setLCDPrint(5, DISPLAY_POSITION_LEFT, "POS Trans Ref: ");

            if ((strTCT.byTerminalType % 2) == 0)
                bRet = InputString(usX, usY, 0x00, bShowAttr, szInputAsc, &usInputLen, iStrLen, d_GETPIN_TIMEOUT);
            else
                bRet = InputString(usX, usY, 0x00, bShowAttr, szInputAsc, &usInputLen, iStrLen, d_GETPIN_TIMEOUT);
            if (bRet == d_KBD_CANCEL) {
                CTOS_LCDTClearDisplay();
                vdSetErrorMessage("USER CANCEL");
                memcpy(&srTransRec, &srTransRecTemp, sizeof (TRANS_DATA_TABLE));
                return (d_EDM_USER_CANCEL);
            }


            if (atoi(szInputAsc) != 0) {
                //inAscii2Bcd(szInputAsc, szInputBcd, INVOICE_BCD_SIZE);

                //memcpy(srTransRec.szTrack3Data,szInputBcd,3);	//reuse existing parameter to prevent change in Amex-Alipay
                strcpy(srTransRec.szTrack3Data, szInputAsc);
                break;
            }
        }

        memcpy(&srTransRec, &srTransRecTemp, sizeof (TRANS_DATA_TABLE));
        //memcpy(srTransRec.szTrack3Data,szInputBcd,3);	//reuse existing parameter to prevent change in Amex-Alipay
        strcpy(srTransRec.szTrack3Data, szInputAsc);
    }

    return d_OK;
}

int inCTOS_Confirm_QRCode(void) {
    BYTE key;

    CTOS_LCDTClearDisplay();
    vdCTOSS_DisplayQRCodeOneLine("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    key = 0;
    CTOS_KBDGet(&key);

    if (key == d_KBD_CANCEL)
        return d_NO;

    return d_OK;
}

void vdConfirmQRCode(void) {
    BYTE key;

    vduiClearBelow(2);

    vdSetShowQRCode(TRUE);

    DebugAddHEX("srTransRec.byQRData", srTransRec.byQRData, srTransRec.inQRDataLen);

    vdCTOSS_DisplayQRCodeOneLine(srTransRec.byQRData);

    key = WaitKey(60);
    if (key != d_KBD_ENTER)
        return d_NO;

    return d_OK;
}

void vdSetShowQRCode(BOOL fSet) {

    fIsShowingQR = fSet;
}

BOOL fGetShowQRCode(void) {
    return fIsShowingQR;
}

int inConfirmQRCode() {
    BYTE key;
    int inResult2, inTransCode = 0;
    vduiClearBelow(2);

    vdSetShowQRCode(TRUE);

    DebugAddHEX("srTransRec.byQRData", srTransRec.byQRData, srTransRec.inQRDataLen);

    vdCTOSS_DisplayQRCodeOneLine(srTransRec.byQRData);

    vduiClearBelow(7);

    setLCDPrint(7, DISPLAY_POSITION_LEFT, "SCAN QR CODE DONE? ");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO[X]	   YES[OK]");
    key = WaitKeyNoTIMEOUT();
    if (key != d_KBD_ENTER) {
        inTransCode = ALIPAY_SALE;
        vduiClearBelow(7);
        vdDisplayErrorMsg(7, 8, "TRANSACTION REJECTED");
        vduiClearBelow(7);
        inResult2 = inSaveReversalFile(&srTransRec, inTransCode);
        if (inResult2 == ST_SUCCESS) {
            inSnedReversalToHost(&srTransRec, inTransCode);
            srTransRec.fSendAlipayAutoReversal = TRUE;
            memset(srTransRec.szRespCode, 0x00, 2);
        }
        return d_NO;
    }
    vduiClearBelow(7);

    return d_OK;
}


#if 0

int inCTOS_CheckPreCompAuth(void) {
    if (srTransRec.byTransType != PRE_AUTH) {
        vdSetErrorMessage("PRECOMP NOT ALLWD");
        return d_NO;
    }

    srTransRec.byTransType = PREAUTH_COMP;

    return d_OK;
}

int inCTOS_CheckApprovalCode(void) {
    USHORT usX = 1, usY = 6;
    BYTE bShowAttr = 0x02;
    USHORT szAuthCodeLen = 6;
    BYTE baPIN[6 + 1];
    BYTE szAuthCode[6 + 1];
    BYTE bRet;

    memset(szAuthCode, 0x00, sizeof (szAuthCode));

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ENTER APPROVAL CODE: ");

    while (TRUE) {
        bRet = InputStringAlphaEx2(1, 8, 0x06, 0x02, szAuthCode, &szAuthCodeLen, 0, d_INPUT_TIMEOUT);

        if (bRet == d_KBD_CANCEL) {
            CTOS_LCDTClearDisplay();
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        if (strlen(szAuthCode) >= 2) {
            vdDebug_LogPrintf("srTransRec.szAuthCode: %s", srTransRec.szAuthCode);
            vdDebug_LogPrintf("szAuthCode: %s", szAuthCode);
            if (memcmp(srTransRec.szAuthCode, szAuthCode, 6) == 0) {
                strcpy(srTransRec.szAuthCode, szAuthCode);
                break;
            } else {
                vdDisplayErrorMsg(1, 8, "INVALID APPROVAL CODE");
                CTOS_LCDTPrintXY(1, 8, "                                        ");
                memset(szAuthCode, 0x00, sizeof (szAuthCode));
                szAuthCodeLen = 6;
            }
        } else {
            memset(szAuthCode, 0x00, sizeof (szAuthCode));
            szAuthCodeLen = 6;
        }
    }

    return ST_SUCCESS;
}

int inCTOS_CheckLast4Digit(void) {
    USHORT usX = 1, usY = 6;
    BYTE bShowAttr = 0x02;
    USHORT inLen = 4;
    BYTE baPIN[6 + 1];
    BYTE szLast4Digit[6 + 1];
    BYTE bRet;

    memset(szLast4Digit, 0x00, sizeof (szLast4Digit));

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ENTER LAST 4 DIGIT: ");

    while (TRUE) {
        bRet = InputString(1, 8, 0x06, 0x02, szLast4Digit, &inLen, 0, d_INPUT_TIMEOUT);

        if (bRet == d_KBD_CANCEL) {
            CTOS_LCDTClearDisplay();
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        if (strlen(szLast4Digit) >= 4) {
            vdDebug_LogPrintf("szPAN: %s, byPanLen:%d", srTransRec.szPAN, srTransRec.byPanLen);
            vdDebug_LogPrintf("szLast4Digit: %s", szLast4Digit);
            if (memcmp(srTransRec.szPAN + (srTransRec.byPanLen - 4), szLast4Digit, 4) == 0)
                break;
            else {
                vdDisplayErrorMsg(1, 8, "INVALID 4 DIGIT");
                CTOS_LCDTPrintXY(1, 8, "										");
                memset(szLast4Digit, 0x00, sizeof (szLast4Digit));
                inLen = 4;
            }
        } else {
            memset(szLast4Digit, 0x00, sizeof (szLast4Digit));
            inLen = 4;
        }
    }

    return ST_SUCCESS;
}
#endif

int inRetryTransStatusEnquiry(void) {
    BYTE key;

    vduiClearBelow(2);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "ENQ STATUS AGAIN?");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO[X]   YES[OK]");

    key = WaitKey(180);
    if (key != d_KBD_ENTER)
        return d_NO;

    vduiClearBelow(2);

    return d_OK;
}

int inCTOS_BatchSearchByInvNo(void) {
    int inResult = d_NO;

    inResult = inDatabase_BatchSearchByInvNo(&srTransRec, srTransRec.szInvoiceNo);

    DebugAddSTR("inCTOS_BatchSearchByInvNo", "Processing...", 20);

    DebugAddINT("inCTOS_BatchSearchByInvNo", inResult);

    if (inResult != d_OK) {
        if (inMultiAP_CheckSubAPStatus() != d_OK)
            vdSetErrorMessage("NO RECORD FOUND");
        return d_NOT_RECORD;
    }

    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 3);

    return inResult;
}

int inCTOSS_CheckVEPQPS(int flag) {
    unsigned short tagLen;
    char outp[40];
    int inResult = 0;
    BYTE szTotalAmt[12 + 1];
    //int inRet = d_NO;

    inResult = inCTOSS_CheckFlagVEPSQPS();


    //inCTOSS_GetBatchFieldData(&srTransFlexiData, AMEX_NSR_FLAG, &fNSRflag, 1);
    vdDebug_LogPrintf("inCTOSS_CheckVEPQPS inResult=[%d]", inResult);
    vdDebug_LogPrintf("inCTOSS_CheckVEPQPS byEntryMode=[%d]", srTransRec.byEntryMode);


    if (inResult) {
        if (srTransRec.byEntryMode == CARD_ENTRY_FALLBACK ||
                srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
 {
            vdDebug_LogPrintf("NSR not support for manual,  fallback");
            return d_NO;
        } else {
            if (flag == 0) {

                if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
                    //overwrite E0B0C8 for visa & master card
                    ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB, 3, "\x00\x08\xC8");

                    ushCTOS_EMV_NewTxnDataSet(TAG_9F1B_TERM_FLOOR_LIMIT, 4, "\x00\x00\x00\x00");
                }

                if (srTransRec.byEntryMode == CARD_ENTRY_WAVE) {
                    if (inResult == 1 || inResult == 2) {
                        //Visa PayWave						  	
                        memcpy(srTransRec.stEMVinfo.T9F33, "\x00\x08\x88", 3);


                    }

                    if (inResult == 3 || inResult == 4) { //MasterCard PayPass						 
                        memcpy(srTransRec.stEMVinfo.T9F33, "\x00\x08\x08", 3);

                    }


                }

            }




            //return d_OK;
        }
    }


    return d_OK;


    //return inRet;
}

int inCTOSS_CheckFlagVEPSQPS(void) {

    int inRet = 0;

    vdDebug_LogPrintf("inCTOSS_CheckFlagVEPSQPS szInstallmentTerms=[%s]", srTransRec.stIPPinfo.szInstallmentTerms);

    if (strcmp(srTransRec.stIPPinfo.szInstallmentTerms, "VPU") == 0)
        inRet = 1;

    if (strcmp(srTransRec.stIPPinfo.szInstallmentTerms, "VPM") == 0)
        inRet = 2;

    else if (strcmp(srTransRec.stIPPinfo.szInstallmentTerms, "QPU") == 0)
        inRet = 3;

    else if (strcmp(srTransRec.stIPPinfo.szInstallmentTerms, "QPM") == 0)
        inRet = 4;

    vdDebug_LogPrintf("inCTOSS_CheckFlagVEPSQPS inRet=[%d]", inRet);

    return inRet;

}

#ifdef DISCOUNT_FEATURE
// for  Discount function
int inCTOS_GetFixedAmount(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;
	int len;
        USHORT usLen;
        int ret;
        BYTE byFirstKey = 0;


		
	vdDebug_LogPrintf("inCTOS_GetFixedAmount");
    
    if(VS_FALSE == fGetMPUTrans()){
        inHDTRead(srTransRec.HDTid);
        inCSTRead(strHDT.inCurrencyIdx);
    }
        
        if(chGetIdleEventSC_MSR() == 1){
            if(fGetMPUTrans() == TRUE || fGetMPUCard() == TRUE){
                if(srTransRec.HDTid == 17)
                    inCSTRead(1);
            }
                
        }
        
        if(srTransRec.fIsInstallment == TRUE){
            inCSTRead(1);
        }

   	#if 0
	if (fECRTxnFlg == 1)
	{
		if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) == 0){
			vdSetErrorMessage("OUT OF RANGE");
			return(ST_ERROR);  
		}
	}
	#endif
	
	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	//if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
	//	return d_OK;
	
    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);

#if 0
    if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
    {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr+3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);
    }
    else
    {
    	if (1 != inCTOSS_GetWaveTransType())
        	inCTOS_DisplayCardTitle(4, 5);
    }
#endif    

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
    CTOS_LCDTPrintXY(1, 7, "FIXED AMOUNT:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);

    memset(baAmount, 0x00, sizeof(baAmount));
    byFirstKey = 0x00;

	
	vdDebug_LogPrintf("strTCT.fEnableAmountIdle[%d]", strTCT.fEnableAmountIdle);
    
    if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
        if ((chGetFirstIdleKey() >= d_KBD_1) && (chGetFirstIdleKey() <= d_KBD_9)) {
            CTOS_KBDInKey(&key);
            byFirstKey = chGetFirstIdleKey();
        }
    }
    
	vdDebug_LogPrintf("szCurSymbol=[%s]",strCST.szCurSymbol);
	if (strncmp(strCST.szCurSymbol,"MMK", 3) == 0)
            key = InputAmountEx(1, 8, szDisplayBuf, 0, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
	else
            key = InputAmountEx(1, 8, szDisplayBuf, 2, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
        
        

    if(d_OK == key)
    {
    	vdDebug_LogPrintf("11baAmount=[%s]",baAmount);
        
        
		if (strncmp(strCST.szCurSymbol,"MMK", 3) == 0)
		{
                    if(strlen(baAmount) > 10){
                        vdDispErrMsg("EXCEED INPUT AMOUNT");
                        return d_NO;
                    }
			len = strlen(baAmount);
			strcpy(&baAmount[len],"00");
		}
		vdDebug_LogPrintf("22baAmount=[%s]",baAmount);
        memset(szTemp, 0x00, sizeof(szTemp));
        //format amount 10+2 
		sprintf(szTemp, "%012.0f", atof(baAmount));

    	vdDebug_LogPrintf("33baAmount=[%s]",szTemp);
		
        wub_str_2_hex(szTemp,srTransRec.szFixedAmount,12);
    }
    if(0xFF == key)
    {
        vdSetErrorMessage("TIME OUT");
        return d_NO;
    }
    return key;
}


// for  Discount function
int inCTOS_GetPercentage(void)
{
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];
    ULONG ulAmount = 0L;
	int len;
        USHORT usLen;
        int ret;
        BYTE byFirstKey = 0;


		
	vdDebug_LogPrintf("inCTOS_GetPercentage");
    
    if(VS_FALSE == fGetMPUTrans()){
        inHDTRead(srTransRec.HDTid);
        inCSTRead(strHDT.inCurrencyIdx);
    }
        
        if(chGetIdleEventSC_MSR() == 1){
            if(fGetMPUTrans() == TRUE || fGetMPUCard() == TRUE){
                if(srTransRec.HDTid == 17)
                    inCSTRead(1);
            }
                
        }
        
        if(srTransRec.fIsInstallment == TRUE){
            inCSTRead(1);
        }

	
	if (1 == inCTOSS_GetWaveTransType())
	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

	
    CTOS_LCDTClearDisplay();
    vduiLightOn();
    
    vdDispTransTitle(srTransRec.byTransType);

    memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    //sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
	
    CTOS_LCDTPrintXY(1, 7, "DISC PERCENTAGE:");
    //setLCDPrint(8, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);

    memset(baAmount, 0x00, sizeof(baAmount));
    byFirstKey = 0x00;

	
	vdDebug_LogPrintf("strTCT.fEnableAmountIdle[%d]", strTCT.fEnableAmountIdle);
    
    if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
        if ((chGetFirstIdleKey() >= d_KBD_1) && (chGetFirstIdleKey() <= d_KBD_9)) {
            CTOS_KBDInKey(&key);
            byFirstKey = chGetFirstIdleKey();
        }
    }

#if 0    
	vdDebug_LogPrintf("szCurSymbol=[%s]",strCST.szCurSymbol);
	if (strncmp(strCST.szCurSymbol,"MMK", 3) == 0)
            key = InputAmountEx(1, 8, szDisplayBuf, 0, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
	else
            key = InputAmountEx(1, 8, szDisplayBuf, 2, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
#else
	key = InputAmountEx(1, 8, szDisplayBuf, 0, byFirstKey, baAmount, &ulAmount, d_INPUT_TIMEOUT, 0);
#endif        
        

    if(d_OK == key)
    {
    	vdDebug_LogPrintf("11baAmount=[%s]",baAmount);
        
        #if 0
		if (strncmp(strCST.szCurSymbol,"MMK", 3) == 0)
		{
                    if(strlen(baAmount) > 10){
                        vdDispErrMsg("EXCEED INPUT AMOUNT");
                        return d_NO;
                    }
			len = strlen(baAmount);
			strcpy(&baAmount[len],"00");
		}
		#else
			len = strlen(baAmount);
			strcpy(&baAmount[len],"00");		
		#endif
		
		vdDebug_LogPrintf("22baAmount=[%s]",baAmount);
        memset(szTemp, 0x00, sizeof(szTemp));
        //format amount 10+2 
		sprintf(szTemp, "%012.0f", atof(baAmount));
		//sprintf(szTemp, "%d", atoi(baAmount));

    	vdDebug_LogPrintf("33baAmount=[%s]",szTemp);
		
        wub_str_2_hex(szTemp,srTransRec.szPercentage,12);
    }
    if(0xFF == key)
    {
        vdSetErrorMessage("TIME OUT");
        return d_NO;
    }
    return key;
}

#endif

int inCTOS_ConfirmTipAfjustAmount(void) {
    char szDisplayBuf[30];
    BYTE key;
    BYTE szTemp[20];
    BYTE baAmount[20];
    BYTE baBaseAmount[20];
    BYTE baTipAmount[20];
    ULONG ulAmount = 0L;
    BYTE szAmtTmp2[16 + 1];
    int len;

    vdDebug_LogPrintf("inCTOS_ConfirmTipAfjustAmount");
	
    DebugAddSTR("inCTOS_GetTipAfjustAmount", "Processing...       ", 20);
    memset(baBaseAmount, 0x00, sizeof (baBaseAmount));
    wub_hex_2_str(srTransRec.szBaseAmount, baBaseAmount, 6);

    memset(baTipAmount, 0x00, sizeof (baTipAmount));
    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    wub_hex_2_str(srTransRec.szTipAmount, baTipAmount, 6);

	vdDebug_LogPrintf("baTipAmount:%s",baTipAmount);
	
    memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
    sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);

    inCTOS_DisplayCardTitle(3, 4);


    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    //format amount 10+2
    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
    {
		sprintf(baTipAmount, "%012.0f", atof(baTipAmount) / 100);
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", baTipAmount, szAmtTmp2);
    }
	else
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", baTipAmount, szAmtTmp2);
    //sprintf(szAmtTmp2, "%lu.%02lu", atol(baTipAmount)/100, atol(baTipAmount)%100);
    //setLCDPrint(5, DISPLAY_POSITION_LEFT, "ORG TIP");
    //setLCDPrint(6, DISPLAY_POSITION_LEFT, szDisplayBuf);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (strlen(szAmtTmp2) + 1)*2, 6, szAmtTmp2);
    inDisplayLeftRight(6, szDisplayBuf, szAmtTmp2, 32);
	
    CTOS_LCDTPrintXY(1, 7, "TIP");
	
#if 0
    memset(baAmount, 0x00, sizeof (baAmount));
    if(fECRTxnFlg == 1) {
        if(memcmp(szECRTipAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0) 
        {
            memcpy(baAmount, szECRTipAmount, 6);  
        }
    }
#endif

#if 0
	DebugAddHEX("baAmount", baAmount, 6);
    do {
		CTOS_KBDHit(&key);
		
        if (strlen(baAmount) > 0) {
            vdDebug_LogPrintf("11baAmount=[%s]", baAmount);
            if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
                len = strlen(baAmount);
                strcpy(&baAmount[len], "00");
            }
            vdDebug_LogPrintf("22baAmount=[%s]", baAmount);

            memset(szTemp, 0x00, sizeof (szTemp));
            //format amount 10+2
            sprintf(szTemp, "%012.0f", atof(baAmount));
            if (strcmp(szTemp, baBaseAmount) > 0) {
                clearLine(8);
                vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                clearLine(8);
				memset(baAmount, 0x00, sizeof (baAmount));
                continue;
            }

            wub_str_2_hex(szTemp, srTransRec.szTipAmount, 12);

            break;
        } else if (d_USER_CANCEL == key) {
            vdSetErrorMessage("USER CANCEL");
            return d_NO;
        } else if (0xFF == key) {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }

    } while (1);
#else
    memset(baTipAmount, 0x00, sizeof (baTipAmount));
    memset(szAmtTmp2, 0x00, sizeof (szAmtTmp2));
    wub_hex_2_str(szECRTipAmount, baTipAmount, 6);
	
    vdDebug_LogPrintf("szECRTipAmount:%s", baTipAmount);

	if(strncmp(strCST.szCurSymbol, "MMK", 3) == 0)
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", baTipAmount, szAmtTmp2);
	else
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", baTipAmount, szAmtTmp2);

    //setLCDPrint(8, DISPLAY_POSITION_LEFT, szDisplayBuf);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (strlen(szAmtTmp2) + 1)*2, 6, szAmtTmp2);
    
    inDisplayLeftRight(8, szDisplayBuf, szAmtTmp2, 32);
	
    //vduiDisplayStringCenter(8, "NO[X] YES[OK]");
    CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
    vduiWarningSound();

    CTOS_KBDBufFlush(); //cleare key buffer

    memcpy(baAmount, baTipAmount, 12);
	
    while (1) 
    {
    
        CTOS_KBDHit(&key);
        if (key == d_KBD_ENTER) 
        {
            if (strlen(baAmount) > 0) {
                vdDebug_LogPrintf("11baAmount=[%s]", baAmount);
                if (strncmp(strCST.szCurSymbol, "MMK", 3) == 0) {
                    len = strlen(baAmount);
                    strcpy(&baAmount[len], "00");
                }
                vdDebug_LogPrintf("1.baAmount=[%s]", baAmount);
                
                memset(szTemp, 0x00, sizeof (szTemp));
                //format amount 10+2
                sprintf(szTemp, "%012.0f", atof(baAmount));
                if (strcmp(szTemp, baBaseAmount) > 0) {
                    clearLine(8);
                    vdDisplayErrorMsg(1, 8, "TOO MUCH TIP");
                    clearLine(8);
                    memset(baAmount, 0x00, sizeof (baAmount));
                    continue;
                }

				vdDebug_LogPrintf("2.baAmount=[%s]", baAmount);
                wub_str_2_hex(szTemp, srTransRec.szTipAmount, 12);
				key = d_OK;
                break;
            }
			
            vduiClearBelow(7);
            key = d_OK;
            break;
        } 
		else if ((key == d_KBD_CANCEL)) 
		{
            vdSetErrorMessage("USER CANCEL");
            return d_NO;
        }
		
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) 
        {
            vdSetErrorMessage("TIME OUT");
            return d_NO;
        }
    }

#endif
    return key;
}


