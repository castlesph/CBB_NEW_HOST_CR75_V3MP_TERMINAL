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
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\erm\Poserm.h"
#include "..\Includes\POSSetting.h"

int inCTOS_VoidFlowProcess(void)
{
    int inRet = d_NO;


    vdCTOS_SetTransType(VOID);
    
    //display title
    vdDispTransTitle(VOID);
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GeneralGetInvoice();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_VOID_SALE);
        if(d_OK != inRet)
            return inRet;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetVoid();
            if(d_OK != inRet)
                return inRet;
        }       
        inRet = inCTOS_BatchSearch();
        if(d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CheckVOID();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_LoadCDTandIIT();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_VoidSelectHost();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_ConfirmInvAmt();
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
    else
        vdSetErrorMessage("");
        
    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();
        
    return d_OK;
}

int inCTOS_VOID(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_VoidFlowProcess();

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
