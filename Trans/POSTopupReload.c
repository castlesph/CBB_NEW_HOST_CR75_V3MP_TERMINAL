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
#include "..\Accum\Accum.h"
#include "..\Comm\V5Comm.h"
#include "..\print\Print.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\erm\Poserm.h"
#include "..\Includes\POSSetting.h"

#include "..\Includes\CTOSInput.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Debug\Debug.h"

extern char DE52[32+1];

int inCTOS_TopupFlowProcess(void) {
    int inRet = d_NO;

#ifdef CBB_TUP_ROUTING
	int inTopUpReload = get_env_int("TUPFINFLAG");
#endif

    vdCTOS_SetTransType(TOPUP_RELOAD_TRANS);

    //display title
    vdDispTransTitle(TOPUP_RELOAD_TRANS);

    vdDebug_LogPrintf("inCTOS_TopupFlowProcess inTopUpReload [%d]", inTopUpReload);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

//    inRet = inCTOS_GetCardFields();
//    if (d_OK != inRet)
//        return inRet;

#ifdef CBB_TUP_ROUTING
	if(inTopUpReload == 1)
	{
	    inRet = inCTOS_TUPGetTxnBaseAmount();
	    if (d_OK != inRet)
	        return inRet;
	}
	else
	{
	    inRet = inCTOS_GetTxnBaseAmount();
	    if (d_OK != inRet)
	        return inRet;
	}
#else	
		inRet = inCTOS_GetTxnBaseAmount();
		if (d_OK != inRet)
			return inRet;

#endif

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;

    vdSetMPUCard(VS_FALSE);
        inRet = inCTOS_WaveGetCardFields();
        if (d_OK != inRet)
            return inRet;
		
    if(strCDT.inBTR == TOPUP || strCDT.inBTR == RELOAD)
    {
		vdCTOS_SetTransType(strCDT.inBTR);
		//display title
		vdDispTransTitle(strCDT.inBTR);
    }
	else
	{
	}
	
	strCDT.HDTid=20; /*forced Balance Enquiry/Topup/Reload host*/
	vdSetMPUCard(VS_FALSE);
	vdSetMPUTrans(VS_FALSE);

#ifdef CBB_TUP_ROUTING
	if(inTopUpReload == 1)
	{
	    inRet = inCTOS_TUPSelectHost();
	    if (d_OK != inRet)
	        return inRet;
	}
	else
	{
	    inRet = inCTOS_SelectHost();
	    if (d_OK != inRet)
	        return inRet;
	}
#else
	inRet = inCTOS_SelectHost();
	if (d_OK != inRet)
		return inRet;
#endif


    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_REFUND);
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

    inRet = inCTOS_GetCVV2();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;

//    inRet = inCTOS_EMVProcessing();
//    if (d_OK != inRet)
//        return inRet;

    //inRet = inCTOSS_GetOnlinePIN();
    //if(d_OK != inRet)
    //    return inRet;
    
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }
	
#if 0
    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
        inRet = inMPU_GetOnlinePIN();
        if (d_OK != inRet)
            return inRet;
    }
#endif

    if(get_env_int("LOGON") != 1)
    {
	    inRet = inCTOS_AutoTopupReload_LOGON();
        if (d_OK != inRet)
            return inRet;

        if (d_OK== inRet)
        {	
            vdCTOS_SetTransType(TOPUP);
            vdDispTransTitle(TOPUP);
        }    
    }

    inRet = inCTOS_GetTopupReloadPIN();
    if (d_OK != inRet)
        return inRet;

	vdCTOS_SetDateTrans();
	
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

    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;

    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if (d_OK != inRet)
        //return inRet;

    //    inRet = inCTOS_EMVTCUpload();
    //    if (d_OK != inRet)
    //        return inRet;
    //    else
    //        vdSetErrorMessage("");
    
    if (srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19) {
        inRet = inCTOS_EMVTCUpload();
        if (d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
    }
    return d_OK;
}

int inCTOS_TOPUP(void) {
    int inRet = d_NO;

    //CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_TopupFlowProcess();

	//vdClearISO_DataElement();

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

int inCTOS_ReloadFlowProcess(void) {
    int inRet = d_NO;


    vdCTOS_SetTransType(RELOAD);

    //display title
    vdDispTransTitle(RELOAD);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

//    inRet = inCTOS_GetCardFields();
//    if (d_OK != inRet)
//        return inRet;
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

	strCDT.HDTid=20; /*forced Balance Enquiry/Topup/Reload host*/
	vdSetMPUCard(VS_FALSE);
	vdSetMPUTrans(VS_FALSE);

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_REFUND);
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

    inRet = inCTOS_GetCVV2();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;

//    inRet = inCTOS_EMVProcessing();
//    if (d_OK != inRet)
//        return inRet;

    //inRet = inCTOSS_GetOnlinePIN();
    //if(d_OK != inRet)
    //    return inRet;
    
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }
	
#if 0
    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
        inRet = inMPU_GetOnlinePIN();
        if (d_OK != inRet)
            return inRet;
    }
#endif
    if(get_env_int("LOGON") != 1)
    {
	    inRet = inCTOS_AutoTopupReload_LOGON();
        if (d_OK != inRet)
            return inRet;

        if (d_OK== inRet)
        {	
            vdCTOS_SetTransType(RELOAD);
            vdDispTransTitle(RELOAD);
        }    
    }

    inRet = inCTOS_GetTopupReloadPIN();
    if (d_OK != inRet)
        return inRet;

    vdCTOS_SetDateTrans();
	
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

    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;

    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if (d_OK != inRet)
        //return inRet;

    //    inRet = inCTOS_EMVTCUpload();
    //    if (d_OK != inRet)
    //        return inRet;
    //    else
    //        vdSetErrorMessage("");
    
    if (srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19) {
        inRet = inCTOS_EMVTCUpload();
        if (d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
    }
    return d_OK;
}

int inCTOS_RELOAD(void) {
    int inRet = d_NO;

    //CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_ReloadFlowProcess();

    //vdClearISO_DataElement();
	
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

int inCTOS_Topup_Reload_LogOnFlowProcess(void)
{
    int inRet = d_NO;
#ifdef CBB_TUP_ROUTING
		int inTopUpReload = get_env_int("TUPFINFLAG");
#endif

    vdCTOS_SetTransType(TOPUP_RELOAD_LOGON);
    
    //display title
    vdDispTransTitle(TOPUP_RELOAD_LOGON);

vdDebug_LogPrintf("inCTOS_Topup_Reload_LogOnFlowProcess START inTopUpReload[%d]", inTopUpReload);

#if 0
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
#endif

	strCDT.HDTid=20; /*forced Balance Enquiry/Topup/Reload host*/
	vdSetMPUCard(VS_FALSE);
	vdSetMPUTrans(VS_FALSE);

#ifdef CBB_TUP_ROUTING	
		if(inTopUpReload == 1)
		{
			inRet = inCTOS_TUPSelectHost();
			if (d_OK != inRet)
				return inRet;
		}
		else
		{
			inRet = inCTOS_SelectHost();
			if (d_OK != inRet)
				return inRet;
		}
#else	
			inRet = inCTOS_SelectHost();
			if (d_OK != inRet)
				return inRet;
#endif



    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
    {
        put_env_int("LOGON", 0);
        return inRet;
    }

	vdSetMPUSignOnStatus(0);

    put_env_int("LOGON", 1);

	/*Display OK*/
	vduiDisplayStringCenter(8, "LOGON SUCCESS");

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return ST_SUCCESS;
}

int inCTOS_TOPUP_RELOAD_LOGON(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(FALSE);
    inRet = inCTOS_Topup_Reload_LogOnFlowProcess();
	vdSetMPUTrans(FALSE);

    //vdClearISO_DataElement();
	
	//inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_GetTopupReloadPIN(void) 
{
    USHORT usX = 1, usY = 6;
    BYTE bShowAttr = 0x02;
    USHORT szRRNLen = 12;
    BYTE szPINCode[32+1];
	BYTE szPINCodePadded[32+1]; //padded with F on the right
    BYTE bRet;
 
    char szOutput[32+1];
	USHORT shMinLen=4, shMaxLen=12;
	
    inDCTRead(20);
    memset(szPINCode, 0x00, sizeof (szPINCode));

    vduiClearBelow(2);
    //vdDebug_LogPrintf("strTCT.byPinPadMode = %d", strTCT.byPinPadMode);
    vdDispTransTitle(srTransRec.byTransType);
    //memset(card_holder_pin,0,sizeof(card_holder_pin));
    
    inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
    //vdDebug_LogPrintf("*** HDTid 2 = %d",srTransRec.HDTid);
    //vdDebug_LogPrintf("strDCT.szDisplayLine1 [%s]", strDCT.szDisplayLine1);
    //vdDebug_LogPrintf("strDCT.szDisplayLine2 [%s]", strDCT.szDisplayLine2);
    setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);

    while (TRUE) 
    {
        memset(szPINCode, 0, sizeof(szPINCode));
		
		bRet = InputString(usX, usY, 0x01, 0x02, szPINCode, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
		
        if (bRet == d_KBD_CANCEL) {
            vduiClearBelow(2);
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (ST_ERROR);
        }

        if (strlen(szPINCode) >= 4) {
            //strcpy(srTransRec.szRRN, szPINCode);
            memset(szPINCodePadded, 'F', sizeof(szPINCodePadded));
            memcpy(szPINCodePadded, szPINCode, strlen(szPINCode));
			szPINCodePadded[32]=0x00;
			vdDebug_LogPrintf("szPINCodePadded:%s", szPINCodePadded);

			memset(szPINCode, 0x00, sizeof(szPINCode));
            wub_str_2_hex(szPINCodePadded, szPINCode, 32);
			DebugAddHEX("szPINCode", szPINCode, 16);
			DebugAddHEX("strDCT.szPINKey", strDCT.szPINKey, 16);

            memset(szOutput, 0, sizeof(szOutput));
			Encrypt3Des(szPINCode, strDCT.szPINKey, szOutput);
            Encrypt3Des(szPINCode+8, strDCT.szPINKey, szOutput+8);

			wub_hex_2_str(szOutput, DE52, 16);
			
			vdDebug_LogPrintf("DE52:%s", DE52);
            break;
        } else {
            memset(szPINCode, 0x00, sizeof (szPINCode));
            shMaxLen=6;
        }
    }

    return ST_SUCCESS;
}

int inCTOS_AutoTopupReload_LOGON(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(TOPUP_RELOAD_LOGON);
    
    //display title
    vdDispTransTitle(TOPUP_RELOAD_LOGON);

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
    {
        put_env_int("LOGON", 0);
        return inRet;
    }

	vdSetMPUSignOnStatus(0);

    put_env_int("LOGON", 1);

	/*Display OK*/
	vduiDisplayStringCenter(8, "LOGON SUCCESS");

    //vdClearISO_DataElement();
	//vdCTOS_TransEndReset();
 
    return ST_SUCCESS;  
}

#ifdef PIN_CHANGE_ENABLE
int inCTOS_ChangePIN(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_ChangePINFlowProcess();

    vdCTOS_TransEndReset();

    return inRet;
}


int inCTOS_ChangePINFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(CHANGE_PIN);
    
    //display title
    vdDispTransTitle(CHANGE_PIN);

	vdDebug_LogPrintf("inCTOS_ChangePINFlowProcess");

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
/*
	strCDT.HDTid = 7;
	
    inRet = inCTOS_SelectHost();
	if(d_OK != inRet)
        return inRet;
    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;
	*/


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_GetCardFieldsNoFleet();
    
    //inRet = inCTOS_GetCardFields();
    inRet = inCTOS_WaveGetCardFields();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inRet)
		return inRet;

#if 0

		//inMPU_GetOnlinePIN();
		inRet = inCBGetChangePIN_With_3DESDUKPT();
		if(d_OK != inRet)
			return inRet;
#else
		inRet = inCTOS_inGetDefaultIPPPin();
		  if(d_OK != inRet)
			return inRet;
	
#if 1
		inRet = inCTOS_GetNewPIN();
		if(d_OK != inRet)
			return inRet;

		inRet = inCTOS_EMVProcessing();
		   if (d_OK != inRet)
			   return inRet;
	
		inRet = inCTOS_GetVerifyPIN();
		if(d_OK != inRet)
			return inRet;

		inRet = inCTOS_ValidatePINEntry();
		if(d_OK != inRet)
		return inRet;
		
#endif
#endif

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    vduiClearBelow(1);
    vdDisplayErrorMsg2(1, 7, "PIN CODE", "SUCCESSFULY CHANGED");
	
    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return ST_SUCCESS;
}


#endif


