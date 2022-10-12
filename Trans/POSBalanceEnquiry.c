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

#include "..\Debug\Debug.h"

int inCTOS_BalanceEnquiryFlowProcess(void) {
    int inRet = d_NO;
#ifdef CBB_TUP_ROUTING
	int inTopUpReload = get_env_int("TUPFINFLAG");
#endif

    vdCTOS_SetTransType(BALANCE_ENQUIRY);

    //display title
    vdDispTransTitle(BALANCE_ENQUIRY);


    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

//    inRet = inCTOS_GetCardFields();
//    if (d_OK != inRet)
//        return inRet;
    
    vdSetMPUCard(VS_FALSE);
    inRet = inCTOS_WaveGetCardFields();
    if (d_OK != inRet)
        return inRet;

	strCDT.HDTid=20; /*forced Balance Enquiry/Topup/Reload host*/
	vdSetMPUCard(VS_FALSE);
	vdSetMPUTrans(VS_FALSE);
	
#ifdef CBB_TUP_ROUTING
    vdDebug_LogPrintf("inCTOS_BalanceEnquiryFlowProcess inTopUpReload [%d]", inTopUpReload);

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
#endif

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;

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

    vdCTOS_SetDateTrans();

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

#if 0
    inRet = inCTOS_SaveBatchTxn();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if (d_OK != inRet)
        return inRet;
#endif

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

int inCTOS_BALANCE_ENQUIRY(void) {
    int inRet = d_NO;
    char temp[500+1];
	char temp2[500+1];
	char temp3[500+1];
	int inLen=0;
	int inPos=0;
	
    //CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();
	
	#if 0
    memset(temp, 0, sizeof(temp));
    strcpy(temp, "600022000002007034400100A18000164183580167060104020000000000000000000055100207200560120400013130313037363037434242414E4B20444556454C4F504D454E54202020202020202059474E2020202020202020204D4D00653030313030313130303230303337343430303230303230333032313031353030303030313030303031313330323130303030343030303131303130303430303031383430");

	inPos=inGetPosition(temp, "1002072005", 10);
	
	vdDebug_LogPrintf("temp:%s, inPos=%d", temp, inPos);
	
	inLen=strlen(temp);
    vdDebug_LogPrintf("temp:%s", temp);
	
	//DebugAddHEX("orig ascii", "CBBANK DEVELOPMENT        YGN         MM", 40);
	
	memset(temp2, 0, sizeof(temp2));
    wub_str_2_hex(temp, temp2, inLen);
	DebugAddHEX("hex",temp2, inLen/2);

    memset(temp3, 0, sizeof(temp3));	
    wub_hex_2_str(temp2, temp3, inLen/2);
	DebugAddHEX("ascii",temp3, inLen);

	vdDebug_LogPrintf("revert to original: %s", temp3);
#endif

    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_BalanceEnquiryFlowProcess();

    vdClearISO_DataElement();
	
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

