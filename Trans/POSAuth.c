/*******************************************************************************

 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>


#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\comm\V5comm.h"
#include "..\print\print.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"

#include "../Database/DatabaseFunc.h"
#include "..\erm\Poserm.h"
#include "..\Accum\accum.h"
#include "..\Includes\POSSetting.h"
#include "..\debug\debug.h"

#include "..\Includes\POSAuth.h"

int inPreAuthCompType = NORMAL_PREAUTHCOMP;


/*must with YYMMSS format, */
int day_diff(char *pszStartYYMMDD, char *pszEndYYMMDD)
{
	int y2, m2, d2;
	int y1, m1, d1;

	int year_start;
	int month_start;
	int day_start;

	int year_end;
	int month_end;
	int day_end;

	BYTE szTemp[2+1];

	/*format start day*/
	memset(szTemp, 0x00, sizeof(szTemp));
	memcpy(szTemp, pszStartYYMMDD, 2);
	year_start =  atoi(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	memcpy(szTemp, &pszStartYYMMDD[2], 2);
	month_start =  atoi(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	memcpy(szTemp, &pszStartYYMMDD[4], 2);
	day_start =  atoi(szTemp);

	/*format end day*/
	memset(szTemp, 0x00, sizeof(szTemp));
	memcpy(szTemp, pszEndYYMMDD, 2);
	year_end =  atoi(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	memcpy(szTemp, &pszEndYYMMDD[2], 2);
	month_end =  atoi(szTemp);

	memset(szTemp, 0x00, sizeof(szTemp));
	memcpy(szTemp, &pszEndYYMMDD[4], 2);
	day_end =  atoi(szTemp);
 
	m1 = (month_start + 9) % 12;
	y1 = year_start - m1/10;
	d1 = 365*y1 + y1/4 - y1/100 + y1/400 + (m1*306 + 5)/10 + (day_start - 1);

	m2 = (month_end + 9) % 12;
	y2 = year_end - m2/10;
	d2 = 365*y2 + y2/4 - y2/100 + y2/400 + (m2*306 + 5)/10 + (day_end - 1);
 
	return (d2 - d1);
}


int inCTOS_CheckPreAuthCompTrans(void)
{
	int inRet = d_OK;
	TRANS_DATA_TABLE srTempTransRec;

	unsigned char key;

	BYTE szPreAuthPAN[20+1];
	BYTE szPreAuthCompPAN[20+1];

	int inPAPANLen = 0;
	int inPACPANLen = 0;

	BYTE 	szAuthCode[AUTH_CODE_DIGITS + 1];

	BYTE    szTotalAmt[12+1];
    BYTE    szTempBuf[12+1];
    BYTE    szTempBuf1[12+1]; 
    BYTE    szDisplayBuf[30];
    BYTE    szStr[45];

	memset(&srTempTransRec, 0x00, sizeof(srTempTransRec));
	memcpy(&srTempTransRec, &srTransRec, sizeof(srTransRec));

	vdDebug_LogPrintf("=====inCTOS_CheckPreAuthCompTrans=====");
	vdDebug_LogPrintf("Current srTransRec.szPAN[%s]", srTransRec.szPAN);
	
	/*Clear Current Trans rec*/
	memset(&srTransRec, 0x00, sizeof(srTransRec));
	
	while (1)
	{
		inRet = inCTOS_GetOffApproveNO();
		if (d_EDM_USER_CANCEL == inRet)
        	return inRet;

		inRet = inCTOS_BatchSearchByAuthCode();
		if (d_OK != inRet)
		{
			vdDisplayErrorMsg(1, 8, "NO RECORD FOUND");
        	//return inRet;
		}
		else
		{
			break;
		}
	}

    memcpy(srTempTransRec.szPreAuthCode, srTransRec.szAuthCode, 6);
    vdDebug_LogPrintf("Load from Batch srTransRec.szPAN[%s]", srTransRec.szPAN);
	vdDebug_LogPrintf("Load from Batch srTransRec.szAuthCode[%s]", srTransRec.szAuthCode);

	/*check PAN last 4 digits*/
	memset(szPreAuthPAN, 0x00, sizeof(szPreAuthPAN));
	memset(szPreAuthCompPAN, 0x00, sizeof(szPreAuthCompPAN));
	
	strcpy(szPreAuthPAN, srTransRec.szPAN); // Pre auth data from batch
	strcpy(szPreAuthCompPAN, srTempTransRec.szPAN); // Pre Auth Comp data from get card
	
	inPAPANLen = strlen(szPreAuthPAN);
	inPACPANLen = strlen(szPreAuthCompPAN);
	
	vdDebug_LogPrintf("szPreAuthPAN[%s]inPAPANLen[%d]", szPreAuthPAN, inPAPANLen);
	vdDebug_LogPrintf("szPreAuthCompPAN[%s]inPACPANLen[%d]", szPreAuthCompPAN, inPACPANLen);
	
	vdDebug_LogPrintf("szPreAuthPAN[inPAPANLen-4][%s]", &szPreAuthPAN[inPAPANLen-4]);


	/*Display*/
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(PREAUTH_COMP);

	CTOS_LCDTPrintXY(1, 2, "PRE AUTH/CARD VERFIY:");

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	sprintf(szDisplayBuf, "%s%s", "LAST 4 DIGITS: ", &szPreAuthPAN[inPAPANLen-4]);
	CTOS_LCDTPrintXY(1, 3,  szDisplayBuf);


	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
    memset(szStr, 0x00, sizeof(szStr));
	
	//format amount 10+2
    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
	{
        memset(szTempBuf, 0x00, sizeof(szTempBuf));
        sprintf(szTempBuf, "%012.0f", atof(szTotalAmt)/100);
        //vdDebug_LogPrintf("szTempBuf [%s]", szTempBuf);
        memset(szTotalAmt, 0x00, sizeof(szTotalAmt));
        strcpy(szTotalAmt, szTempBuf);
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmt, szStr);
    } 
	else
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	sprintf(szDisplayBuf, "%s%s%s", "AMOUNT: ", strCST.szCurSymbol, szStr);
    CTOS_LCDTPrintXY(1, 4, szDisplayBuf);

	if (0 != memcmp(&szPreAuthPAN[inPAPANLen-4], &szPreAuthCompPAN[inPACPANLen-4], 4))
	{
		vdDisplayErrorMsg(1, 7, "PAN NOT MATCH");
		vdDisplayErrorMsg(1, 8, "PLS USE CORRECT CARD");
		return d_NO;
	}

    vduiDisplayStringCenter(7,"CONFIRM & CONTINUE?");
    vduiDisplayStringCenter(8,"NO[X]   YES[OK] ");
    
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
		{
			memset(szAuthCode, 0x00, sizeof(szAuthCode));
			strcpy(szAuthCode, srTransRec.szAuthCode);

			memset(&srTransRec, 0x00, sizeof(srTransRec));
			memcpy(&srTransRec, &srTempTransRec, sizeof(srTempTransRec));
			strcpy(srTransRec.szAuthCode, szAuthCode);
            return d_OK;
		}
        else if (key==d_KBD_CANCEL)
            return d_NO;
        else
            vduiWarningSound();
    }
	
	
	return inRet;
}

int inCTOS_PreAuthFlowProcess(void) 
{
    int inRet = d_NO;


    vdCTOS_SetTransType(PRE_AUTH);

    vdDispTransTitle(PRE_AUTH);

	vdDebug_LogPrintf("inCTOS_PreAuthFlowProcess. POSAuth.c");

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    //    inRet = inCTOS_GetCardFields();
    //    if (d_OK != inRet)
    //        return inRet;

#if 1 //orig code 09052022 - to get bin details 1st for currency index.
    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;

    vdSetMPUCard(VS_FALSE);
    inRet = inCTOS_WaveGetCardFields();
    if (d_OK != inRet)
        return inRet;
	#else

    vdSetMPUCard(VS_FALSE);
    inRet = inCTOS_WaveGetCardFields();
    if (d_OK != inRet)
        return inRet;

	    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;
	
	#endif

    inRet = inCTOS_CheckAllowCardVer();
    if (d_OK != inRet)
        return inRet;

	#if 0
	// Just added to check preauth is allowed - 06142021
	inRet = inCTOS_CheckAllowPreAuth();
    if (d_OK != inRet)
        return inRet;
	#endif
	

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_PreAuthFlowProcess >> strCST.inCurrencyIndex = [%d] strHDT.inCurrencyIdx = [%d]", strCST.inCurrencyIndex, strHDT.inCurrencyIdx);

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH);
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

    inRet = inCTOS_CheckIssuerEnable();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if (d_OK != inRet)
        return inRet;

#if 0
    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;
#endif
//    inRet = inCTOS_GetCVV2();
//    if (d_OK != inRet)
//        return inRet;
    
    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;

//    inRet = inCTOS_EMVProcessing();
//    if (d_OK != inRet)
//        return inRet;
    
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }
    
    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
        inRet = inMPU_GetOnlinePIN();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if (d_OK != inRet)
        return inRet;
	
	//albert - 20180226 - send ECR response before printing the receipt - 
    inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;

    inRet = ushCTOS_ePadSignature();
    if (d_OK != inRet)
        return inRet;

    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_PreAuthFlowProcess AAAAA >> strCST.inCurrencyIndex = [%d] strHDT.inCurrencyIdx = [%d]", strCST.inCurrencyIndex, strHDT.inCurrencyIdx);

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_EMVTCUpload();
    if (d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
    return d_OK;
}

int inCTOS_PREAUTH(void) {
    int inRet = d_NO;

	
	if(FALSE == strTCT.fEnablePreAuthMenu)
	{
		vduiClearBelow(2);
		vduiWarningSound();
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
		WaitKey(3);
		return d_OK;
	}


    //CTOS_LCDTClearDisplay();
    inPreAuthCompType=0;

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreAuthFlowProcess();

    inCTOS_inDisconnect();

    if(strTCT.fUploadReceiptIdle != TRUE){
		#ifdef NEW_MPOS_ISSUE
		//eReceipt Issue raised for MPOS.
		if(usCTOSS_Erm_GetReceiptTotal() > 0)	
			inCTOSS_UploadReceipt();
		#else
		
			inCTOSS_UploadReceipt();
		#endif
    }

    if (isCheckTerminalMP200() == d_OK) {
        //CTOS_KBDBufFlush();
        if (d_OK != inRet)
            vdCTOSS_DisplayStatus(inRet);
        //WaitKey(5);
    }

    vdCTOS_TransEndReset();

    return inRet;
}

static int inCTOS_CheckPreAuthVOID(void) {

	
    vdDebug_LogPrintf("inCTOS_CheckPreAuthVOID");


    if (srTransRec.byVoided == TRUE) {
        vdSetErrorMessage("ALREADY VOIDED");
        return d_NO;
    }

    if (srTransRec.byTransType != PRE_AUTH) {
        vdDisplayErrorMsg(1, 8, "VOID NOT ALLOWED");
        return d_NO;
    }

    //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - START
    if (srTransRec.byTransType == SALE_TIP) {
        szOriginTipTrType = srTransRec.byOrgTransType;
        srTransRec.byOrgTransType = srTransRec.byTransType;
    } else //to find the original transaction of the sale tip to be voided - Meena 26/12/12 - END
        srTransRec.byOrgTransType = srTransRec.byTransType;

    srTransRec.byTransType = VOID_PREAUTH;

    return d_OK;
}

int inCTOS_PreAuthVoidFlowProcess(void) {
    int inRet = d_NO;

    vdDebug_LogPrintf("inCTOS_PreAuthVoidFlowProcess");

    vdCTOS_SetTransType(VOID_PREAUTH);

    //display title
    vdDispTransTitle(VOID_PREAUTH);

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;

#ifdef CB_MPU_NH_MIGRATION
	inRet = inCTOS_WaveGetCardFields();
	if(d_OK != inRet)
		return inRet;
#endif	

    inRet = inCTOS_GeneralGetInvoice();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_VOID_SALE);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetVoid();
            if (d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_BatchSearch();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckPreAuthVOID();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_LoadCDTandIIT();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_VoidSelectHost();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_ConfirmInvAmt();
    if (d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if (d_OK != inRet)
        return inRet;

	//albert - 20180226 - send ECR response before printing the receipt - 
    inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;

    inRet = ushCTOS_ePadSignature();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();

    return d_OK;
}

int inCTOS_PREAUTH_VOID(void) {
    int inRet = d_NO;

    //CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_PreAuthVoidFlowProcess();

    inCTOS_inDisconnect();

    if(strTCT.fUploadReceiptIdle != TRUE)
	{
			#ifdef NEW_MPOS_ISSUE
				//eReceipt Issue raised for MPOS.
				if(usCTOSS_Erm_GetReceiptTotal() > 0)	
					inCTOSS_UploadReceipt();
			#else
				
					inCTOSS_UploadReceipt();
			#endif
	}

    vdCTOS_TransEndReset();

    return inRet;
}

/*
 * basically this preauth completion is the offline sale, but the difference is
 * this transaction is going online, not just like the offline sale
 */
static int inCTOS_PreAuthCompletionFlowProcess(void)
{
    int inRet = d_NO;
	int inGetNEWPACFlow, inGetRRNFlag = 0;

	vdDebug_LogPrintf("inCTOS_PreAuthCompletionFlowProcess");

    vdCTOS_SetTransType(PREAUTH_COMP);
    
    //display title
    vdDispTransTitle(PREAUTH_COMP);

	inGetNEWPACFlow = get_env_int("#PREAUTHRRNDT"); // enable/ disable original flow.

    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

#if 1 //orig code 09052022 - to get bin details 1st for currency index.
//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
	
    vdSetMPUCard(VS_FALSE);
    inRet = inCTOS_WaveGetCardFields();
    if (d_OK != inRet)
        return inRet;
#else
vdSetMPUCard(VS_FALSE);
inRet = inCTOS_WaveGetCardFields();
if (d_OK != inRet)
	return inRet;

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
	

#endif

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH_COMP);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;
            
            inRet = inCTOS_MultiAPReloadTable();
            if(d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_MultiAPCheckAllowd();
        if(d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckIssuerEnable();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

#if 0
	/*with CBB req, we need check */
	inRet = inCTOS_CheckPreAuthCompTrans();
	if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
#endif

//    inRet = inCTOS_GetCVV2();
//    if(d_OK != inRet)
//        return inRet;
    
    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }

#ifdef CB_MPU_NH_MIGRATION
    vdDebug_LogPrintf("XXXXXXX - byTransType[%d] inGetNEWPACFlow[%d]",srTransRec.byTransType, inGetNEWPACFlow);	

	//http://118.201.48.210:8080/redmine/issues/1525.99 and 100 #1
	//should be fixed to prompt RRN
	
	// Enable inGetNEWPACFlow for MPU host, as for new routing Terminal should follow the existing flow of VISA/MASTER/UPI when processing MPU and JCB card transactions for Precomp
    if (inGetNEWPACFlow == 1)
	{
		// new adjustment for MPU new host application - 05/06/19 - based "POS function and  Regulation" doc
		if( srTransRec.HDTid == 17 ||
			srTransRec.HDTid == 18 ||
			srTransRec.HDTid == 19)
		{
			inRet = inCTOS_GeneralGetRRN();
			if(d_OK != inRet)
				return inRet;

			#if 0
			// to fix incorrect transtype after checking for RRN
	    	vdCTOS_SetTransType(PREAUTH_COMP);
	        vdDispTransTitle(PREAUTH_COMP);		
			#endif
		}
	}

	#if 0
	//http://118.201.48.210:8080/redmine/issues/1525.99 and 100 #1.2	
    if (inGetNEWPACFlow != 1)
	{
    	inRet = inCTOS_GetOffApproveNO();
    	if(d_OK != inRet)
        	return inRet;
	}	
	
    vdDebug_LogPrintf("XXXXXXX - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	
	DebugAddHEX("srTransRec.szDate", srTransRec.szOrgDate, 2);	
	DebugAddHEX("srTransRec.szOrgTime", srTransRec.szOrgTime, 3);	
	/*for CUP card, use host 19, go to MPU host. PreAuth Comp Need get Auth Date*/
	
	//http://118.201.48.210:8080/redmine/issues/1525.99 and 100 #2
    if (inGetNEWPACFlow != 1)
	{
		if( srTransRec.HDTid == 17 ||
			srTransRec.HDTid == 18 ||
			srTransRec.HDTid == 19)
		{
			inRet = inMPU_GetAuthDate();
	    	if (d_OK != inRet)
	        return inRet;
			vdDebug_LogPrintf("XXXXXXX2 - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	

			// new adjustment for MPU new host application - 05/06/19
			inRet = inMPU_GetAuthTime();
			if (d_OK != inRet)
		    	return inRet;
		}
	}
	#endif

    vdDebug_LogPrintf("XXXXXXX4 - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	
#endif	

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
//    inRet = inCTOS_EMVProcessing();
//    if(d_OK != inRet)
//        return inRet;   
    
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }

#if 1 // for testing - 01022019 - for Bug #326 CBB CUP PreAuth Completion Fatal Error Issued by Zin during his MPU onsite testing.
	vdDebug_LogPrintf("check fGetMPUCard: fGetMPUTrans [%d][%d]", fGetMPUCard(), fGetMPUTrans());
	// Enable inGetNEWPACFlow for MPU host, as for new routing Terminal should follow the existing flow of VISA/MASTER/UPI when processing MPU and JCB card transactions for Precomp
    if (inGetNEWPACFlow == 1)
	{
		if( srTransRec.HDTid == 17 ||
			srTransRec.HDTid == 18 ||
			srTransRec.HDTid == 19)
		{
		    inRet = inCTOSS_GetOnlinePIN();
		    if (d_OK != inRet)
		        return inRet;

		    if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
		        inRet = inMPU_GetOnlinePIN();
		        if (d_OK != inRet)
		            return inRet;
		    }
		}
	}
#endif	

	//http://118.201.48.210:8080/redmine/issues/1525.99 and 100 #3
	//function placement should be here
	inRet = inCTOS_GetOffApproveNO();
	if(d_OK != inRet)
	   return inRet;


    vdDebug_LogPrintf("YYYYYYYY - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

	//albert - 20180226 - send ECR response before printing the receipt - 
    inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;

	inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;
	
	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);
	}

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
        return inRet;
	
    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
        
    return d_OK;
}

int inCTOS_PREAUTH_COMPLETION(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("inCTOS_PREAUTH_COMPLETION");

    if(FALSE == strTCT.fEnablePreAuthMenu)
	{
		vduiClearBelow(2);
		vduiWarningSound();
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
		WaitKey(3);
		return d_OK;
	}
		
    
    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_PreAuthCompletionFlowProcess();

    inCTOS_inDisconnect();

    if(strTCT.fUploadReceiptIdle != TRUE)
	{
		#ifdef NEW_MPOS_ISSUE
				//eReceipt Issue raised for MPOS.
				if(usCTOSS_Erm_GetReceiptTotal() > 0)	
					inCTOSS_UploadReceipt();
		#else
				
					inCTOSS_UploadReceipt();
		#endif
	}

	if (isCheckTerminalMP200() == d_OK)
	{
		//CTOS_KBDBufFlush();
		if (d_OK != inRet)
		vdCTOSS_DisplayStatus(inRet);
		//WaitKey(5);
	}

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_PreauthCompVoidFlowProcess(void) {
    int inRet = d_NO;


    vdCTOS_SetTransType(VOID_PREAUTH_COMP);

    //display title
    vdDispTransTitle(VOID_PREAUTH_COMP);

	vdDebug_LogPrintf("inCTOS_PreauthCompVoidFlowProcess");

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;


    inRet = inCTOS_GeneralGetInvoice();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_VOID_SALE);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetVoid();
            if (d_OK != inRet)
                return inRet;
        }
        
        inRet = inCTOS_BatchSearch();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckVOID_PREAUTH_COMP();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_LoadCDTandIIT();
    if (d_OK != inRet)
        return inRet;


    inRet = inCTOS_VoidSelectHost();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_ConfirmInvAmt();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_PreauthCompVoidFlowProcess1 fGetMPUTrans [%d]", fGetMPUTrans());

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_PreauthCompVoidFlowProcess2 fGetMPUTrans [%d]", fGetMPUTrans());

    inRet = inCTOS_SaveBatchTxn();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if (d_OK != inRet)
        return inRet;

	//albert - 20180226 - send ECR response before printing the receipt - 
    inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;

    inRet = ushCTOS_ePadSignature();
    if (d_OK != inRet)
        return inRet;

    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();

    return d_OK;
}

int inCTOS_PREAUTHCOMP_VOID(void) {
    int inRet = d_NO;
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_PreauthCompVoidFlowProcess();
    vdSetMPUTrans(FALSE);

    inCTOS_inDisconnect();

    if(strTCT.fUploadReceiptIdle != TRUE)
	{
		#ifdef NEW_MPOS_ISSUE
				//eReceipt Issue raised for MPOS.
				if(usCTOSS_Erm_GetReceiptTotal() > 0)	
					inCTOSS_UploadReceipt();
		#else
				
					inCTOSS_UploadReceipt();
		#endif
	}

    if (isCheckTerminalMP200() == d_OK) {
        //CTOS_KBDBufFlush();
        if (d_OK != inRet)
            vdCTOSS_DisplayStatus(inRet);
        //WaitKey(5);
    }

    vdCTOS_TransEndReset();

    return inRet;
}

int inUpdatePreAuthTxnTable(void)
{
	int inRet = 0;
	
	//if table not exist , then create
	inRet = inDB_CheckTableExist(PRE_AUTH_TXN_TAB);

	if (inRet < 0)
	{
		inRet = inDB_CreateTable(PRE_AUTH_TXN_TAB, sizeof(PRE_AUTH_TXN_REC));
	}

	/*just append rec*/
	//inDB_AppendTableRec()


	return d_OK;
}

/*every time terminal boot up will check date , read table compare date*/
int inCleanUpPreAuthTxnTable(void)
{
	int inTot = 0;
	int inIdx = 0;
	int inGap = 0;
	PRE_AUTH_TXN_REC stTmpRec;
	
	//get total
	inTot = inDB_GetTableTotalRecNum(PRE_AUTH_TXN_TAB);

	//load rec one by one check the date
	//for (inIdx = 0; inIdx<inTot; inIdx++)
	{
		//memset(&stTmpRec, 0x00, sizeof(stTmpRec));
	//inDB_ReadTableRecBySeq(PRE_AUTH_TXN_TAB, inIdx, stTmpRec, sizeof(PRE_AUTH_TXN_REC))
	}
	
	//Compre date get Gap.
	//inGap = day_diff(CurrDate, PreAutDate)

	//if inGap >= set "#PACGAP" delete that rec.
	
	return d_OK;
}

// replace function inCTOS_CheckPreAuthCompTrans
int inCTOS_CheckPreAuthTab(void)
{
	// for loop load rec and compare the date.
	
	return d_OK;
}

#ifdef ECR_PREAUTH_AND_COMP
static int inCTOS_PreAuthCompletionFlowProcess_ECR(void)
{
    int inRet = d_NO;
    char szMIDFrECR[MERCHANT_ID_BYTES+1];
	//char szMIDFrTable[MERCHANT_ID_BYTES+1];
	
	vdDebug_LogPrintf("inCTOS_PreAuthCompletionFlowProcess");

    vdCTOS_SetTransType(TRANS_ENQUIRY);
    
    //display title
    vdDispTransTitle(TRANS_ENQUIRY);

	vduiClearBelow(2);
	
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
	
    vdSetMPUCard(VS_FALSE);
	
   #if 0
   inRet = inCTOS_WaveGetCardFields();
    if (d_OK != inRet)
        return inRet;
   #endif

   #if 0
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;
   #endif
   
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH_COMP);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return inRet;
            
            inRet = inCTOS_MultiAPReloadTable();
            if(d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_MultiAPCheckAllowd();
        if(d_OK != inRet)
            return inRet;
    }

	vdDebug_LogPrintf("*srTransRec.szRRN: %s\n", srTransRec.szRRN);
	vdDebug_LogPrintf("*srTransRec.szAuthCode: %s\n", srTransRec.szAuthCode);
	vdDebug_LogPrintf("*srTransRec.szMID: %s\n", srTransRec.szMID);
	vdDebug_LogPrintf("*srTransRec.HDTid: %d\n", srTransRec.HDTid);

	if(srTransRec.HDTid != 7 && srTransRec.HDTid != 21)
	{
        vdSetErrorMessage("TRANS NOT ALLOWED");
        return ST_ERROR;
	}
	
	memset(szMIDFrECR, 0, sizeof(szMIDFrECR));
	memcpy(szMIDFrECR, srTransRec.szMID, 15);
	
	srTransRec.MITid=1;
		
    inRet = inCTOS_VoidSelectHost();
    if (d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

    if(memcmp(szMIDFrECR, srTransRec.szMID, 15) != 0)
    {
		vdSetErrorMessage("INVALID MERCHANT");
		return ST_ERROR;
    }
		
    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    //memset(szMIDFrTable, 0, sizeof(szMIDFrTable));
	//memcpy(szMIDFrTable, srTransRec.szMID, 15);
	memset(srTransRec.szMID, 0, sizeof(srTransRec.szMID));
	memcpy(srTransRec.szMID, szMIDFrECR, 15);
	
#if 1
    inRet = inProcessTransEnquiry();
    if(d_OK != inRet)
        return inRet;
#endif

    //get PAN from host
 
    inRet = inCTOS_CheckIssuerEnable();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

#if 0
	/*with CBB req, we need check */
	inRet = inCTOS_CheckPreAuthCompTrans();
	if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
#endif

//    inRet = inCTOS_GetCVV2();
//    if(d_OK != inRet)
//        return inRet;

#if 0    
    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }
#else
    memset(srTransRec.szCVV2, 0x00, sizeof(srTransRec.szCVV2));

#endif

	#if 0
    inRet = inCTOS_GetOffApproveNO();
    if(d_OK != inRet)
        return inRet;
	#endif
	
	/*for CUP card, use host 19, go to MPU host. PreAuth Comp Need get Auth Date*/
	if( srTransRec.HDTid == 17 ||
		srTransRec.HDTid == 18 ||
		srTransRec.HDTid == 19)
	{
		inRet = inMPU_GetAuthDate();
    	if (d_OK != inRet)
        return inRet;
	}

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
//    inRet = inCTOS_EMVProcessing();
//    if(d_OK != inRet)
//        return inRet;   
    
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }

#if 1 // for testing - 01022019 - for Bug #326 CBB CUP PreAuth Completion Fatal Error Issued by Zin during his MPU onsite testing.
	vdDebug_LogPrintf("check fGetMPUCard: fGetMPUTrans [%d][%d]", fGetMPUCard(), fGetMPUTrans());
	if( srTransRec.HDTid == 17 ||
		srTransRec.HDTid == 18 ||
		srTransRec.HDTid == 19)
	{
	    inRet = inCTOSS_GetOnlinePIN();
	    if (d_OK != inRet)
	        return inRet;

	    if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
	        inRet = inMPU_GetOnlinePIN();
	        if (d_OK != inRet)
	            return inRet;
	    }
	}
#endif	

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

	//albert - 20180226 - send ECR response before printing the receipt - 
    inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;

	inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;
	
	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);
	}

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
        return inRet;
	
    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
        
    return d_OK;
}

int inCTOS_PREAUTH_COMPLETION_ECR(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("inCTOS_PREAUTH_COMPLETION_ECR");

    if(FALSE == strTCT.fEnablePreAuthMenu)
	{
		vduiClearBelow(2);
		vduiWarningSound();
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
		WaitKey(3);
		return d_OK;
	}
		
    
    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_PreAuthCompletionFlowProcess_ECR();

    inCTOS_inDisconnect();

    if(strTCT.fUploadReceiptIdle != TRUE)
	{
		#ifdef NEW_MPOS_ISSUE
				//eReceipt Issue raised for MPOS.
				if(usCTOSS_Erm_GetReceiptTotal() > 0)	
					inCTOSS_UploadReceipt();
		#else
				
					inCTOSS_UploadReceipt();
		#endif
	}

	if (isCheckTerminalMP200() == d_OK)
	{
		//CTOS_KBDBufFlush();
		if (d_OK != inRet)
		vdCTOSS_DisplayStatus(inRet);
		//WaitKey(5);
	}

    vdCTOS_TransEndReset();

    return inRet;
}

int inProcessTransEnquiry(void)
{
    int inRet = d_NO;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

	vduiClearBelow(2);
    vdCTOS_SetTransType(PREAUTH_COMP);
    vdDispTransTitle(PREAUTH_COMP);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
	
    if (d_OK != inCTOS_LoadCDTIndex()) 
    {
        CTOS_KBDBufFlush();
        return USER_ABORT;
    }

    vdCTOS_SetTransEntryMode(CARD_ENTRY_MANUAL);
	
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

	srTransRec.byECRPreauthComp=TRUE;
	
    return ST_SUCCESS;  
}

#endif

