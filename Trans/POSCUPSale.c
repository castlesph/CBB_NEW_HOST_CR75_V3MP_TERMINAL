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

int inCTOS_CUPSaleFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];

    vdCTOS_SetTransType(CUP_SALE);
    
    //display title
    vdDispTransTitle(CUP_SALE);
    
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
    if(d_OK != inRet)
        return inRet;
 
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;

	//inRet = inCTOSS_GetOnlinePIN();
    //if(d_OK != inRet)
    //    return inRet;
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

int inCTOS_CUP_SALE(void)
{
    int inRet = d_NO;
	BYTE bret = d_NO;
	
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

//	vdCTOSS_GetAmt();
	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_CUPSaleFlowProcess();

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


