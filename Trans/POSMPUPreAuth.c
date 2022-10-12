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

BYTE szAuthDate[2];
BYTE szRefundTime[6+1];



int inCTOSS_UPIBatchSearchByRRN(void)
{
	int inResult = d_NO;
//        BYTE byOriginalTrans = 0;
//        
//        byOriginalTrans = srTransRec.byTransType;

	TRANS_DATA_TABLE srTransParaTmp;

	memset(&srTransParaTmp,0x00,sizeof(TRANS_DATA_TABLE));
	vdDebug_LogPrintf("inCTOSS_UPIBatchSearchByRRN,szRRN=[%s]", srTransRec.szRRN);
	inResult = inDatabase_BatchSearchByRRN(&srTransParaTmp, srTransRec.szRRN);
	vdDebug_LogPrintf("inCTOSS_UPIBatchSearchByRRN,inResult=[%d]", inResult);
//	if(inResult != d_OK)
//    {
//		return d_OK;
//    }
    
    memcpy(srTransRec.szOrgDate, srTransParaTmp.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransParaTmp.szTime, 3);

	srTransRec.ulOrgTraceNum = srTransParaTmp.ulTraceNum;
	srTransRec.byOrgTransType = srTransParaTmp.byTransType;
//        srTransRec.byTransType = byOriginalTrans;
        srTransRec.ulSavedIndex = srTransParaTmp.ulSavedIndex;
	vdPCIDebug_HexPrintf("szOrgDate",srTransRec.szOrgDate,2);
	vdPCIDebug_HexPrintf("szOrgTime",srTransRec.szOrgTime,3);
	vdDebug_LogPrintf("inCTOSS_UPIBatchSearchByRRN ulTraceNum=[%d],byOrgTransType=[%d]",srTransRec.ulOrgTraceNum,srTransRec.byOrgTransType);
	
	return d_OK;
}


static int inCTOS_MPU_PreAuthFlowProcess(void)
{
    int inRet = d_NO;


    vdCTOS_SetTransType(MPU_PREAUTH);
    vdDispTransTitle(MPU_PREAUTH);

	
    vdDebug_LogPrintf("inCTOS_MPU_PreAuthFlowProcess", inRet);

    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnTipAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)

//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
    
//    inRet = inCTOS_GetMPUCardFields();
//    if(d_OK != inRet && READ_APPLET_ERR != inRet)
//        return inRet;
//
//	if (READ_APPLET_ERR == inRet)
//	{
//		vdSetMPUCard(VS_FALSE);
//		inRet = inCTOS_WaveGetCardFields();
//    	if(d_OK != inRet)
//        	return inRet;
//		
//	}
    
    vdSetMPUCard(VS_FALSE);
    inRet = inCTOS_WaveGetCardFields();
    vdDebug_LogPrintf("inRet = %d", inRet);
    if (d_OK != inRet)
        return inRet;
    
    if(strIIT.fUPICard == FALSE && strIIT.fMPUCard == FALSE) {
        vdCTOS_SetTransType(PRE_AUTH);
        vdDispTransTitle(PRE_AUTH);
    }

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

	//if (fGetMPUTrans() == TRUE && strHDT.inHostIndex == 17)
    if (fGetMPUTrans() == TRUE )
    {
        inRet = inCTOS_CheckMPUSignOn();	
        
        if (d_OK != inRet)	
        {
            inRet=inCTOS_AutoMPU_SIGNON();
            if (d_OK != inRet)
                return inRet;
        }
        
        //inCTOS_MPU_SIGNON();
        if (d_OK== inRet)
        {	
            vdCTOS_SetTransType(SALE);
            vdDispTransTitle(SALE);
        }
    }

    inRet = inCTOS_CheckIssuerEnable();
    if(d_OK != inRet)
        return inRet;

	#if 0
	// Request to check Preauth capability - 06102021
	inRet = inCTOS_CheckAllowPreAuth();
    if(d_OK != inRet)
        return inRet;
	#endif

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

//    inRet = inCTOS_GetTxnBaseAmount();
//    if(d_OK != inRet)
//        return inRet;
//
//    inRet = inCTOS_UpdateTxnTotalAmount();
//    if(d_OK != inRet)
//        return inRet;
    
    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
//    inRet = inCTOS_EMVProcessing();
//    if(d_OK != inRet)
//        return inRet;
    
//    inRet = inMPU_GetOnlinePIN();
//    if(d_OK != inRet)
//        return inRet;
    
    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }
    
    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE) {
        inRet = inMPU_GetOnlinePIN();
        if (d_OK != inRet)
            return inRet;
    }

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

	#if 0
    inRet = inCTOS_EMVTCUpload();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
	#endif
	
    return d_OK;
}

int inCTOS_MPU_PREAUTH(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;

	vdSetMPUTrans(TRUE);
    inRet = inCTOS_MPU_PreAuthFlowProcess();
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

static int inCTOS_MPU_PreAuthCompFlowProcess(void){
    int inRet = d_NO;

    vdDebug_LogPrintf("inCTOS_MPU_PreAuthCompFlowProcess", inRet);
    
    vdCTOS_SetTransType(MPU_PREAUTH_COMP);
    vdDispTransTitle(MPU_PREAUTH_COMP);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
    
//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
    
    vdSetMPUCard(VS_FALSE);
    inRet = inCTOS_WaveGetCardFields();
    vdDebug_LogPrintf("inRet = %d", inRet);
    if (d_OK != inRet)
        return inRet;
    
    if(strIIT.fMPUCard == FALSE && strIIT.fUPICard == FALSE) {
        vdCTOS_SetTransType(PREAUTH_COMP);
        vdDispTransTitle(PREAUTH_COMP);
    }
    
//    inRet = inCTOS_GetMPUCardFields();
//    if(d_OK != inRet && READ_APPLET_ERR != inRet)
//        return inRet;
//
//	if (READ_APPLET_ERR == inRet)
//	{
//		vdSetMPUCard(VS_FALSE);
//		inRet = inCTOS_WaveGetCardFields();
//    	if(d_OK != inRet)
//        	return inRet;
//		
//	}
    
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

//    inRet = inCTOS_GetTxnBaseAmount();
//    if(d_OK != inRet)
//        return inRet;
//
//    inRet = inCTOS_UpdateTxnTotalAmount();
//    if(d_OK != inRet)
//        return inRet;

    #if 0
    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;
#endif

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }


    inRet = inCTOS_GetAuthCode();
    if (d_OK != inRet)
        return inRet;

    inRet = inMPU_GetAuthDate();
    if (d_OK != inRet)
        return inRet;
    
    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE) {
        inRet = inMPU_GetOnlinePIN();
        if (d_OK != inRet)
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

if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);
	}

	inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;

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

    return d_OK;
}

int inCTOS_MPU_PREAUTH_COMP(void){
    
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;

	vdSetMPUTrans(TRUE);
    inRet = inCTOS_MPU_PreAuthCompFlowProcess();
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

#if 0
static int inCTOS_MPUPreAuthCompAdvFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(MPU_PREAUTH_COMP_ADV);
    vdDispTransTitle(MPU_PREAUTH_COMP_ADV);
    
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
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_OFFLINE_SALE);
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

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
	
	#if 0
    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;
	#endif

    inRet = inCTOS_GetOffApproveNO();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_EMVProcessing();
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

	inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;
	
    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
        
    return d_OK;
}
#endif

static int inCTOS_MPUPreAuthCompAdvFlowProcess(void){
    int inRet = d_NO;
    
    vdCTOS_SetTransType(MPU_PREAUTH_COMP_ADV);
    vdDispTransTitle(MPU_PREAUTH_COMP_ADV);
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
    
//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
    
    inRet = inCTOS_GetMPUCardFields();
    if(d_OK != inRet && READ_APPLET_ERR != inRet)
        return inRet;

	if (READ_APPLET_ERR == inRet)
	{
		vdSetMPUCard(VS_FALSE);
		inRet = inCTOS_WaveGetCardFields();
    	if(d_OK != inRet)
        	return inRet;
		
	}
    
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

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;

    #if 0
    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;
	#endif

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_EMVProcessing();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetAuthCode();
	if(d_OK != inRet)
        return inRet;

	inRet = inMPU_GetAuthDate();
	if(d_OK != inRet)
        return inRet;
    
    inRet = inMPU_GetOnlinePIN();
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

if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);
	}

	inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;

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
    vdSetErrorMessage("");
	
    return d_OK;
}

int inCTOS_MPU_PREAUTH_COMP_ADVICE(void){
    
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;

	vdSetMPUTrans(TRUE);
    inRet = inCTOS_MPUPreAuthCompAdvFlowProcess();
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

int inCTOS_MPUVoidPreAuthFlowProcess(void)
{
    int inRet = d_NO;
    BYTE byTransType;

    vdCTOS_SetTransType(MPU_VOID_PREAUTH);
    
    //display title
    vdDispTransTitle(MPU_VOID_PREAUTH);
    byTransType = MPU_VOID_PREAUTH;

	#if 1
	inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    //inRet = inCTOS_GetTxnPassword();
    //if(d_OK != inRet)
        //return inRet;

//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
        
//        inRet = inCTOS_GetMPUCardFields();
//    if(d_OK != inRet && READ_APPLET_ERR != inRet)
//        return inRet;
//
//	if (READ_APPLET_ERR == inRet)
//	{
		vdSetMPUCard(VS_FALSE);
		inRet = inCTOS_WaveGetCardFields();
    	if(d_OK != inRet)
        	return inRet;
		
//	}

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
    //if(d_OK != inRet)
        //return inRet;

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
	#endif

	/*for MPU UPI void pre-auth, 
	can maunal key in date time and Auth Code, these two info only
	so may set DE37 as blank.*/
	memset(srTransRec.szRRN, 0x00, sizeof(srTransRec.szRRN));
	memset(srTransRec.szRRN, 0x20, sizeof(srTransRec.szRRN)-1);

	
	#if 0
	memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

    inRet = inCTOS_GetMPUCardFields();
    //if(d_OK != inRet)
        //return inRet;
	#endif

	#if 0
	if (READ_APPLET_ERR == inRet)
	{
		vdSetMPUCard(VS_FALSE);
		inRet = inCTOS_WaveGetCardFields();
    	if(d_OK != inRet)
        	return inRet;
		
	}
	#endif

	if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
		vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

	inRet = inMPU_GetOnlinePIN();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetAuthCode();
	if(d_OK != inRet)
        return inRet;

	inRet = inMPU_GetAuthDate();
	if(d_OK != inRet)
        return inRet;
	
	vdDebug_LogPrintf("HDTid=[%d]..........",srTransRec.HDTid);
	if (srTransRec.HDTid == MPU_CUP_HOST_IDX) 
	{
	inRet = inCTOS_GetTransRRN();
	if(d_OK != inRet)
        return inRet;

	inCTOSS_UPIBatchSearchByRRN();
        
        inRet = inCTOS_CheckVOID_MPUPreAuth();
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

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);
	}

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    
    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;
    //else
    vdSetErrorMessage("");
        
    return d_OK;
}

int inCTOS_MPU_VOID_PREAUTH(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(TRUE);
    inRet = inCTOS_MPUVoidPreAuthFlowProcess();
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

int inCTOS_MPUVoidPreAuthCompFlowProcess(void)
{
    int inRet = d_NO;
    BYTE byTransType;

    vdCTOS_SetTransType(MPU_VOID_PREAUTH_COMP);
    
    //display title
    vdDispTransTitle(MPU_VOID_PREAUTH_COMP);
    byTransType = 0;
    byTransType = MPU_VOID_PREAUTH_COMP;

	
	vdDebug_LogPrintf("=====inCTOS_MPUVoidPreAuthCompFlowProcess=====");
	#if 1
	inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;
    
    //inRet = inCTOS_GetTxnPassword();
    //if(d_OK != inRet)
        //return inRet;

//    inRet = inCTOS_GetCardFields();
//    if(d_OK != inRet)
//        return inRet;
        
        inRet = inCTOS_GetMPUCardFields();
    if(d_OK != inRet && READ_APPLET_ERR != inRet)
        return inRet;

	if (READ_APPLET_ERR == inRet)
	{
		vdSetMPUCard(VS_FALSE);
		inRet = inCTOS_WaveGetCardFields();
    	if(d_OK != inRet)
        	return inRet;
		
	}

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
    //if(d_OK != inRet)
        //return inRet;

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
	#endif

	/*for MPU UPI void pre-auth, 
	can maunal key in date time and Auth Code, these two info only
	so may set DE37 as blank.*/
	memset(srTransRec.szRRN, 0x00, sizeof(srTransRec.szRRN));
	memset(srTransRec.szRRN, 0x20, sizeof(srTransRec.szRRN)-1);

	
	#if 0
	memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

    inRet = inCTOS_GetMPUCardFields();
    //if(d_OK != inRet)
        //return inRet;
	#endif

	#if 0
	if (READ_APPLET_ERR == inRet)
	{
		vdSetMPUCard(VS_FALSE);
		inRet = inCTOS_WaveGetCardFields();
    	if(d_OK != inRet)
        	return inRet;
		
	}
	#endif

	if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
		vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

	inRet = inMPU_GetOnlinePIN();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetAuthCode();
	if(d_OK != inRet)
        return inRet;

	inRet = inMPU_GetAuthDate();
	if(d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("HDTid11=[%d]..........",srTransRec.HDTid);
	if (srTransRec.HDTid == MPU_CUP_HOST_IDX) 
	{
	inRet = inCTOS_GetTransRRN();
	if(d_OK != inRet)
        return inRet;

	inCTOSS_UPIBatchSearchByRRN();
        
        inRet = inCTOS_CheckVOID_MPUPreAuth();
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

	if (isCheckTerminalMP200() == d_OK)
	{
		vdCTOSS_DisplayStatus(d_OK);
	}

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    
    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if(d_OK != inRet)
        //return inRet;
    //else
    vdSetErrorMessage("");
        
    return d_OK;
}

int inCTOS_MPU_VOID_PREAUTH_COMP(void)
{
    int inRet = d_NO;
    
    //CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(TRUE);
    inRet = inCTOS_MPUVoidPreAuthCompFlowProcess();
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
