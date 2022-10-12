/*******************************************************************************

 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>


#include "..\Includes\POSTypedef.h"
#include "..\Debug\Debug.h"


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

int inCTOS_RefundFlowProcess(void) {
    int inRet = d_NO;


    vdCTOS_SetTransType(REFUND);

    //display title
    vdDispTransTitle(REFUND);


	//vdDebug_LogPrintf("inCTOS_RefundFlowProcess");		

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

    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;

// for new MPU host application upgrade - based "POS function and  Regulation" doc
#if 0
    inRet = inCTOS_GetCVV2();
    if (d_OK != inRet)
        return inRet;
#endif

//vdDebug_LogPrintf("inCTOS_RefundFlowProcess srTransRec.HDTid [%d]", srTransRec.HDTid);		

vdDebug_LogPrintf("inCTOS_RefundFlowProcess HDTID IS  [%d]", srTransRec.HDTid);	


#if 1 // for new MPU host application upgrade	
	if( srTransRec.HDTid == 17 ||
		srTransRec.HDTid == 18 ||
		srTransRec.HDTid == 19)
	{

	    inRet = inCTOS_GeneralGetRRN();
		if(d_OK != inRet)
			return inRet;

		#if 0
	    vdCTOS_SetTransType(REFUND);
    	vdDispTransTitle(REFUND);	
		#endif

	#if 0			
	    inRet = inCTOS_RefundMultiAPBatchSearchByRRN(d_IPC_CMD_REFUND);
	    if(d_OK != inRet)
	        return inRet;
	#endif
	
	#if 1
		inRet = inMPU_GetAuthDate();
    	if (d_OK != inRet)
        	return inRet;

		inRet = inMPU_GetAuthTime();
    	if (d_OK != inRet)
        	return inRet;
		
	#endif
		
	}
	else
	{

	//Applicable for MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus 
	//for CDTid 50, 52, 59, 60, 20-27, 29-30 and 92
	//http://118.201.48.214:8080/issues/75#change-4195 #67	
	//As per the host side request, EDC to prompt Merchant to key in two additional key data during refund, the Retrieval Ref Number (DE37) and Approval Code (DE38).
	
	
		#ifdef CBB_CR_REFUND
		vdDebug_LogPrintf("inCTOS_RefundFlowProcess HDTID IS 22 OR 23 srTransRec.CDTid [%d]::strCDT.fAddRefRRNAPP[%d]", srTransRec.CDTid, strCDT.fAddRefRRNAPP);	
		
		/*if(srTransRec.CDTid == 20 || srTransRec.CDTid == 21 || srTransRec.CDTid == 22 || srTransRec.CDTid == 23 || srTransRec.CDTid == 24 || srTransRec.CDTid == 25 || srTransRec.CDTid == 26 ||
			srTransRec.CDTid == 27 || srTransRec.CDTid == 29 || srTransRec.CDTid == 30 || srTransRec.CDTid == 50 || srTransRec.CDTid == 52 || srTransRec.CDTid ==  59 || srTransRec.CDTid ==  60)*/
		if(strCDT.fAddRefRRNAPP == TRUE)	
		{
			inRet = inCTOS_GeneralGetRRN();
			if(d_OK != inRet)
				return inRet;


			inRet = inCTOS_GetOffApproveNO();
			if(d_OK != inRet)
			   return inRet;
		}
		#endif
	}
#endif	

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

	#ifdef MPU_CARD_TC_UPLOAD_ENABLE
        inRet = inCTOS_EMVTCUpload();
        if (d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
	#else
    if (srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19) {
        inRet = inCTOS_EMVTCUpload();
        if (d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
    }
	
	#endif
	
    return d_OK;
}

int inCTOS_REFUND(void) {
    int inRet = d_NO;

    //CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_RefundFlowProcess();

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
