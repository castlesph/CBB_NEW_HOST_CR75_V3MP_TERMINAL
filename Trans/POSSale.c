/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

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
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Database\DatabaseFunc.h"
#include "..\erm\Poserm.h"
#include "..\Includes\POSSetting.h"

int inCTOS_SaleFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

	vdDebug_LogPrintf("inCTOS_SaleFlowProcess");

    vdCTOS_SetTransType(SALE);
    
    //display title
    vdDispTransTitle(SALE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetCardFields();
    //inRet = inCTOS_WaveGetCardFields();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
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

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_EMVProcessing();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;
    
    if (VS_TRUE == fGetMPUCard()) {
		inRet = inMPU_UPI_GetOnlinePIN();
    	if(d_OK != inRet)
        	return inRet;
	}

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
    if (d_OK != inRet)
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

    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if (d_OK != inRet)
        //return inRet;
    // patrick add code 20141205 end

    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return d_OK;
}

int inCTOS_SALE(void)
{
    int inRet = d_NO;
	BYTE bret = d_NO;
	
    vdCTOS_TxnsBeginInit();
    
    vdDebug_LogPrintf("SALE NORMAL");

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_SaleFlowProcess();

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


int inCTOS_CashAdvFlowProcess(void)
{
    int inRet = d_NO;
#ifdef CBB_CAV_ROUTING	
	int inGetCAVFinFlag = get_env_int("CAVFINFLAG");
#endif

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

    vdCTOS_SetTransType(CASH_ADVANCE);
    
    //display title
    vdDispTransTitle(CASH_ADVANCE);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

#ifdef CBB_CAV_ROUTING	

	 vdDebug_LogPrintf("inCTOS_CashAdvFlowProcess  inGetCAVFinFlag[%d]", inGetCAVFinFlag);
	 //CONNECT CAV transaction to OLD MPU
	 if(inGetCAVFinFlag == 1)
	 {
		 inRet = inCTOS_CAVGetTxnBaseAmount();
			 if(d_OK != inRet)
				 return inRet;
	 }
	else// connect to FInexus host
	{	 
		 inRet = inCTOS_GetTxnBaseAmount();
	    	if(d_OK != inRet)
	        	return inRet;
	}
#else
		inRet = inCTOS_GetTxnBaseAmount();
		   if(d_OK != inRet)
			   return inRet;
#endif

    inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
    
//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
    
    inRet = inCTOS_WaveGetCardFields();
    if(d_OK != inRet)
        return inRet;

#ifdef CBB_CAV_ROUTING	
	//CONNECT CAV transaction to OLD MPU
	if(inGetCAVFinFlag == 1)
	{
		inRet = inCTOS_CAVSelectHost();
		if(d_OK != inRet)
			return inRet;
	}
	else	// connect to FInexus host
	{
	
		inRet = inCTOS_SelectHost();
	    if(d_OK != inRet)
	        return inRet;	
	}
#else
		inRet = inCTOS_SelectHost();
		if(d_OK != inRet)
			return inRet;
#endif

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
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
    //if(d_OK != inRet)
        //return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

   

    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_ConfirmAmt();
    if(d_OK != inRet)
        return inRet;
 
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;

	//inRet = inCTOSS_GetOnlinePIN();
    //if(d_OK != inRet)
    //    return inRet;
	vdDebug_LogPrintf("inCTOS_CashAdvFlowProcess check fGetMPUCard[%d]", fGetMPUCard());
	// remove condition to resolve redmine case #2181 #1
    //if (VS_TRUE == fGetMPUCard() || VS_TRUE == fGetMPUTrans()) {
		inRet = inCAV_MPU_UPI_GetOnlinePIN();//inMPU_UPI_GetOnlinePIN();

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

    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;
	// patrick add code 20141205 end

    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return d_OK;
}


int inCTOS_CashAdvance(void)
{
    int inRet = d_NO;
	BYTE bret = d_NO;
	
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_CashAdvFlowProcess();

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


