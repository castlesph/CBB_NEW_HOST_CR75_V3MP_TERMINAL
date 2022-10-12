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
#include "..\Accum\accum.h"
#include "../Database/DatabaseFunc.h"
#include "..\erm\Poserm.h"
#include "..\Includes\POSSetting.h"

int inCTOS_CUPPreAuthFlowProcess(void)
{
    int inRet = d_NO;


    vdCTOS_SetTransType(CUP_PRE_AUTH);
    
    vdDispTransTitle(CUP_PRE_AUTH);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH);
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

    //inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;  

	inRet = inCUP_GetOnlinePIN();
	if(d_OK != inRet)
		return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
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
    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");      
    return d_OK;
}

int inCTOS_CUP_PreAuth(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_CUPPreAuthFlowProcess();

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


int inCTOS_CUPVoidPreAuthFlowProcess(void)
{
    int inRet = d_NO;


    vdCTOS_SetTransType(CUP_VOID_PREAUTH);
    
    vdDispTransTitle(CUP_VOID_PREAUTH);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH);
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

    //inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetTransOrgDate();
    if(d_OK != inRet)
        return inRet;
	
	inRet = inCTOS_GetAuthCode();
	if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetTransRRN();
	if(d_OK != inRet)
        return inRet;

	inRet = inCUP_GetOnlinePIN();
	if(d_OK != inRet)
		return inRet;
	
    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
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
    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");      
    return d_OK;
}


int inCTOS_CUP_VoidPreAuth(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_CUPVoidPreAuthFlowProcess();

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



int inCTOS_CUPPreAuthCompFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(CUP_PREAUTH_COMP);
    
    vdDispTransTitle(CUP_PREAUTH_COMP);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCardFields();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_PRE_AUTH);
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

    //inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetAuthCode();
	if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetTransRRN();
	if(d_OK != inRet)
        return inRet;

	inRet = inCUP_GetOnlinePIN();
	if(d_OK != inRet)
		return inRet;

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
    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");      
    return d_OK;
}


int inCTOS_CUP_PreAuthComp(void)
{
    int inRet = d_NO;

    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_CUPPreAuthCompFlowProcess();

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


