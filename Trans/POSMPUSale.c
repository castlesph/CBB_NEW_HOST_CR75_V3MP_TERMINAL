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

int MPU_ReversalFlag = 0;
int JCB_ReversalFlag = 0;
int CUP_ReversalFlag = 0;

int inCTOS_MPU_SaleFlowProcess(void) {
    int inRet = d_NO;

	vdDebug_LogPrintf("*** inCTOS_MPU_SaleFlowProcess  START ***");

    vdCTOS_SetTransType(SALE);
    vdDispTransTitle(SALE);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;	

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    //    if((chGetIdleEventSC_MSR() == 1)) {
    if (((strTCT.fEnableAmountIdle == TRUE) && (fCheckButtonFromIdle() != TRUE) && (fCheckCardFromIdle() == TRUE)) ||
            (strTCT.fEnableAmountIdle == FALSE && (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_MSR || fCheckButtonFromIdle() == TRUE || fCheckCardFromIdle() == TRUE))) {

		vdDebug_LogPrintf("*** inCTOS_MPU_SaleFlowProcess  AAAAA ***");

		vdSetMPUCard(VS_FALSE);
        inRet = inCTOS_WaveGetCardFields();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnTipAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;
    
//    inRet = inTransSelectCurrency();
//    if(d_OK != inRet)
//        return inRet;
    
//        if((chGetIdleEventSC_MSR() == 0)){
    if ((srTransRec.byEntryMode != CARD_ENTRY_ICC && srTransRec.byEntryMode != CARD_ENTRY_MSR && srTransRec.byEntryMode != CARD_ENTRY_MANUAL)) {
        vdSetMPUCard(VS_FALSE);
        inRet = inCTOS_WaveGetCardFields();
        if (d_OK != inRet)
            return inRet;
    }
        			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess A szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;
			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess B szTraceNo1 = %02x%02x%02x%d", 
				strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2], strCST.inCurrencyIndex);

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
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

	vdDebug_LogPrintf("XXXXXX fGetMPUTrans [%d][%d][%d][%d][%d]XXXXXX", 
		fGetMPUTrans(), fGetMPUCard(), strHDT.inHostIndex, srTransRec.IITid, inGetMPUSignOnStatus());
	
	
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
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;
			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess C szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetInvoice();
    if (d_OK != inRet)
        return inRet;
			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess D szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);


    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;    

    //    if (VS_FALSE == fGetMPUCard()) {

	//thandar_update for VEPS 
    #if 0
	
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }
   #endif

   #if 0
   vdSetMPUCard(VS_FALSE);
   vdSetMPUTrans(VS_FALSE);   
   #endif
   
    vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess E szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

	inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
        			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess F szTraceNo1 =[%d][%d] [%02x][%02x][%02x][%d]", 
						fGetMPUCard(), fGetMPUTrans(),strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2], strCST.inCurrencyIndex);

	// for None/ Off US MPU cards
    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
        inRet = inMPU_GetOnlinePIN();
        if (d_OK != inRet)
            return inRet;
    }	
			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess G szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;
			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess H szTraceNo1 =[ %02x][%02x][%02x][%d]", 
				strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2], strCST.inCurrencyIndex);

    inRet = inCTOS_SaveBatchTxn();
    if (d_OK != inRet)
        return inRet;
			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess I szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

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
		
    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if (d_OK != inRet)
        //return inRet;

	vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess okik strHDT.inCurrencyIdx[%d] inCurrencyIndex [%d]", strHDT.inCurrencyIdx, strCST.inCurrencyIndex);

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess  J szTraceNo1 = [%02x%02x%02x]strHDT.inCurrencyIdx[%d] strCST.inCurrencyIndex[%d]", 
		strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2], strHDT.inCurrencyIdx, strCST.inCurrencyIndex);

//for testing, to send tc upload on JCB
// Add to send de55 for JCB CTLS transactions #2
#ifdef MPU_CARD_TC_UPLOAD_ENABLE
        inRet = inCTOS_EMVTCUpload();
        if (d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
#else
		if (srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19) 
		{
			inRet = inCTOS_EMVTCUpload();
			if (d_OK != inRet)
				return inRet;
			else
				vdSetErrorMessage("");
		}
#endif
			vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess end szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

    return d_OK;
}

int inCTOS_MPU_SALE(void) {
    int inRet = d_NO;
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_MPU_SaleFlowProcess();
	vdDebug_LogPrintf("inCTOS_MPU_SaleFlowProcess AFTER >>> inRet[%d]", inRet);
	
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
        if (d_OK != inRet)
            vdCTOSS_DisplayStatus(inRet);
    }

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_ReversalProcess(void) {
    int inRet = d_NO;

    CHAR szFileName[d_BUFF_SIZE];

    vdDebug_LogPrintf("inCTOS_ReversalProcess");

    vdCTOS_SetTransType(AUTO_REVERSAL);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    /*MPU*/
    inRet = inCTOS_SelectMPUHost();
    if (d_OK != inRet)
        goto QQWALLET;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        goto QQWALLET;

    memset(szFileName, 0, sizeof (szFileName));
    sprintf(szFileName, "%s%02d%02d.rev"
            , strHDT.szHostLabel
            , strHDT.inHostIndex
            , srTransRec.MITid);
    DebugAddSTR("inCTOS_ReversalProcess", szFileName, 12);
    if ((inRet = inMyFile_CheckFileExist(szFileName)) < 0) {
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        goto QQWALLET;
    }

    CTOS_LCDTClearDisplay();
    vdCTOS_SetTransType(AUTO_REVERSAL);
    vdDispTransTitle(AUTO_REVERSAL);

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        goto QQWALLET;

    inRet = inBuildAndSendIsoData();
    vdDebug_LogPrintf("MPU_ReversalFlag =[%d],inRet=[%d], inHostIndex[%d]", MPU_ReversalFlag, inRet, strHDT.inHostIndex);
    if (inRet != d_OK) {
        /*MPU_ReversalFlag++;
        if (MPU_ReversalFlag >= 5) {
            inMyFile_ReversalDelete();
            MPU_ReversalFlag = 0;
        }*/
        if(inExceedMaxTimes_ReversalDelete() != ST_SUCCESS)
        	vdDisplayErrorMsg(1, 8, "Delete REV Fail...");

    } else
        inMyFile_ReversalDelete();


QQWALLET:
    /*JCB*/
    inRet = inCTOS_SelectJCBHost();
    if (d_OK != inRet)
        goto WECHAT;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        goto WECHAT;

    memset(szFileName, 0, sizeof (szFileName));
    sprintf(szFileName, "%s%02d%02d.rev"
            , strHDT.szHostLabel
            , strHDT.inHostIndex
            , srTransRec.MITid);
    DebugAddSTR("inCTOS_ReversalProcess", szFileName, 12);
    if ((inRet = inMyFile_CheckFileExist(szFileName)) < 0) {
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        goto WECHAT;
    }

    CTOS_LCDTClearDisplay();
    vdCTOS_SetTransType(AUTO_REVERSAL);
    vdDispTransTitle(AUTO_REVERSAL);

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        goto WECHAT;

    inRet = inBuildAndSendIsoData();
    vdDebug_LogPrintf("JCB_ReversalFlag =[%d],inRet=[%d]", JCB_ReversalFlag, inRet);
    if (inRet != d_OK) {
        /*JCB_ReversalFlag++;
        if (JCB_ReversalFlag >= 5) {
            inMyFile_ReversalDelete();
            JCB_ReversalFlag = 0;
        }*/
			if(inExceedMaxTimes_ReversalDelete() != ST_SUCCESS)
				vdDisplayErrorMsg(1, 8, "Delete REV Fail...");
        
    } else
        inMyFile_ReversalDelete();

WECHAT:
    /*UPI*/
    inRet = inCTOS_SelectCUPHost();
    if (d_OK != inRet)
        goto REV_END; //return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        goto REV_END; //return inRet;

    memset(szFileName, 0, sizeof (szFileName));
    sprintf(szFileName, "%s%02d%02d.rev"
            , strHDT.szHostLabel
            , strHDT.inHostIndex
            , srTransRec.MITid);
    DebugAddSTR("inCTOS_ReversalProcess", szFileName, 12);
    if ((inRet = inMyFile_CheckFileExist(szFileName)) < 0) {
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        goto REV_END; //return d_OK;
    }

    CTOS_LCDTClearDisplay();
    vdCTOS_SetTransType(AUTO_REVERSAL);
    vdDispTransTitle(AUTO_REVERSAL);

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        goto REV_END; //return inRet;

    inRet = inBuildAndSendIsoData();
    vdDebug_LogPrintf("CUP_ReversalFlag =[%d],inRet=[%d]", CUP_ReversalFlag, inRet);
    if (inRet != d_OK) {
        /*CUP_ReversalFlag++;
        if (CUP_ReversalFlag >= 5) {
            inMyFile_ReversalDelete();
            CUP_ReversalFlag = 0;
        }*/
			if(inExceedMaxTimes_ReversalDelete() != ST_SUCCESS)
				vdDisplayErrorMsg(1, 8, "Delete REV Fail...");
      
    } else
        inMyFile_ReversalDelete();

    CTOS_LCDTClearDisplay();
    if (d_OK != inRet)
        goto REV_END; //return inRet;


REV_END:
    vdSetErrorMessage(""); //no need to display error message

    return d_OK;
}

int inCTOS_MPU_Reversal(void) {
    int inRet = d_NO;

    vdDebug_LogPrintf("inCTOS_MPU_Reversal");

    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_ReversalProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndResetEx();

    return inRet;
}

int inCTOS_MPU_SALE_PARKING(void) 
{
    int inRet = d_NO;
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_MPU_SaleParkingFlowProcess();
    vdSetMPUTrans(FALSE);

    inCTOS_inDisconnect();

	vdDebug_LogPrintf("strTCT.fUploadReceiptonSettle: %d", strTCT.fUploadReceiptonSettle);
	
    if(strTCT.fUploadReceiptonSettle != TRUE)
        inCTOSS_UploadReceipt();
	
    if (isCheckTerminalMP200() == d_OK) 
    {
        if (d_OK != inRet)
            vdCTOSS_DisplayStatus(inRet);
    }

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_MPU_SaleParkingFlowProcess(void) {
    int inRet = d_NO;
	char szParkFeeAmt[12+1];
	
    int inParkingFee, inParkFeeAmt;
	
	vdDebug_LogPrintf("*** inCTOS_MPU_SaleFlowProcess  START ***");

    vdCTOS_SetTransType(SALE);
    vdDispTransTitle(SALE);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;	

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    //    if((chGetIdleEventSC_MSR() == 1)) {
    if (((strTCT.fEnableAmountIdle == TRUE) && (fCheckButtonFromIdle() != TRUE) && (fCheckCardFromIdle() == TRUE)) ||
            (strTCT.fEnableAmountIdle == FALSE && (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_MSR || fCheckButtonFromIdle() == TRUE || fCheckCardFromIdle() == TRUE))) {
        vdSetMPUCard(VS_FALSE);
        inRet = inCTOS_WaveGetCardFields();
        if (d_OK != inRet)
            return inRet;
    }

    #ifdef PARKING_FEE
    inParkingFee = get_env_int("PARKFEE");
    if(inParkingFee == TRUE)
    {
		#if 1
		inParkFeeAmt = get_env_int("PARKFEEAMT");
		memset(szParkFeeAmt, 0, sizeof(szParkFeeAmt));
		sprintf(szParkFeeAmt, "%012ld", inParkFeeAmt*100);

        memset(srTransRec.szBaseAmount, 0, sizeof(srTransRec.szBaseAmount));
		wub_str_2_hex(szParkFeeAmt, srTransRec.szBaseAmount, 12);
		#else
        memcpy(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x05\x00", 6); 
        #endif
    }
	#else
    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;
    #endif

    inRet = inCTOS_GetTxnTipAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;
    
//    inRet = inTransSelectCurrency();
//    if(d_OK != inRet)
//        return inRet;
    
//        if((chGetIdleEventSC_MSR() == 0)){
    if ((srTransRec.byEntryMode != CARD_ENTRY_ICC && srTransRec.byEntryMode != CARD_ENTRY_MSR && srTransRec.byEntryMode != CARD_ENTRY_MANUAL)) {
        vdSetMPUCard(VS_FALSE);
        //inRet = inCTOS_WaveGetCardFields();
        inRet = inCTOS_WaveGetCardFieldsCTLS();
        if (d_OK != inRet)
            return inRet;
    }
    
    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
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
	
	#if 1
	//if (fGetMPUTrans() == TRUE && strHDT.inHostIndex == 17)
	if (fGetMPUTrans() == TRUE )
	//{
			inRet = inCTOS_CheckMPUSignOn();	
	
   			if (d_OK != inRet)		
			 inRet=inCTOS_AutoMPU_SIGNON();
			
				//inCTOS_MPU_SIGNON();
              if (d_OK== inRet)
				{	vdCTOS_SetTransType(SALE_OFFLINE);
    				vdDispTransTitle(SALE_OFFLINE);
               	}
	
	//}
    #endif
	

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


    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;    

    //    if (VS_FALSE == fGetMPUCard()) {

	//thandar_update for VEPS 
    #if 0
	
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }
   #endif
   
	 inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    
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
        
#if 0
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
#else
    if (isCheckTerminalMP200() == d_OK) 
    {
        vdCTOSS_DisplayStatusNoKey(d_OK);
    }
#endif

    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if (d_OK != inRet)
        //return inRet;

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;

    if (srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19) {
        inRet = inCTOS_EMVTCUpload();
        if (d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
    }


    return d_OK;
}

// for Discount function
#ifdef DISCOUNT_FEATURE
int inCTOS_FIXED_AMOUNT(void) {
    int inRet = d_NO;

	if(FALSE == strTCT.fEnableDiscountMenu)
	{
		vduiClearBelow(2);
		vduiWarningSound();
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
		WaitKey(3);
		return d_OK;
	}
	
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_FIXED_AMOUNT_SaleFlowProcess();
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
        if (d_OK != inRet)
            vdCTOSS_DisplayStatus(inRet);
    }

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_FIXED_AMOUNT_SaleFlowProcess(void) {
    int inRet = d_NO;

	vdDebug_LogPrintf("*** inCTOS_FIXED_AMOUNT_SaleFlowProcess  START ***");

    vdCTOS_SetTransType(SALE);
    vdDispTransTitle(SALE);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;	

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    if (((strTCT.fEnableAmountIdle == TRUE) && (fCheckButtonFromIdle() != TRUE) && (fCheckCardFromIdle() == TRUE)) ||
            (strTCT.fEnableAmountIdle == FALSE && (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_MSR || fCheckButtonFromIdle() == TRUE || fCheckCardFromIdle() == TRUE))) {
        vdSetMPUCard(VS_FALSE);
        inRet = inCTOS_WaveGetCardFields();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

#if 1
// for Discount function
    inRet = inCTOS_GetFixedAmount();
    if (d_OK != inRet)
        return inRet;

	inRet = inCTOS_UpdateTxnTotalAmountDisc();
	if (d_OK != inRet)
		return inRet;
		
#endif


/*
   Original code
    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;
*/

    if ((srTransRec.byEntryMode != CARD_ENTRY_ICC && srTransRec.byEntryMode != CARD_ENTRY_MSR && srTransRec.byEntryMode != CARD_ENTRY_MANUAL)) {
        vdSetMPUCard(VS_FALSE);
        //inRet = inCTOS_WaveGetCardFields();
        inRet = inCTOS_WaveGetCardFieldsDisc();
        if (d_OK != inRet)
            return inRet;
    }
    
    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
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
	
	vdDebug_LogPrintf("*** inCTOS_FIXED_AMOUNT_SaleFlowProcess [%d][%d][%d][%d]", strHDT.inHostIndex, srTransRec.IITid, fGetMPUTrans(), fGetMPUCard());

	if (fGetMPUTrans() == TRUE )	 
	{	
		inRet = inCTOS_CheckMPUSignOn();					
		if (d_OK != inRet)			
		{			 
			inRet=inCTOS_AutoMPU_SIGNON(); 		   

			if (d_OK != inRet)				 
			return inRet;		  
		} 			   
		if (d_OK== inRet)		  
		{ 			
			vdCTOS_SetTransType(SALE);			  
			vdDispTransTitle(SALE);		 
		}	  
	}    
	

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


    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;    

	 inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    
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

// for Discount function

int inCTOS_PERCENTAGE(void) {
    int inRet = d_NO;
	
	if(FALSE == strTCT.fEnableDiscountMenu)
	{
		vduiClearBelow(2);
		vduiWarningSound();
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
		WaitKey(3);
		return d_OK;
	}
	
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_PERCENTAGE_SaleFlowProcess();
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
        if (d_OK != inRet)
            vdCTOSS_DisplayStatus(inRet);
    }

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_PERCENTAGE_SaleFlowProcess(void) {
    int inRet = d_NO;

	vdDebug_LogPrintf("*** inCTOS_FIXED_AMOUNT_SaleFlowProcess  START ***");

    vdCTOS_SetTransType(SALE);
    vdDispTransTitle(SALE);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;	

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    if (((strTCT.fEnableAmountIdle == TRUE) && (fCheckButtonFromIdle() != TRUE) && (fCheckCardFromIdle() == TRUE)) ||
            (strTCT.fEnableAmountIdle == FALSE && (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_MSR || fCheckButtonFromIdle() == TRUE || fCheckCardFromIdle() == TRUE))) {
        vdSetMPUCard(VS_FALSE);
        inRet = inCTOS_WaveGetCardFields();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

#if 1
// for Discount function
    inRet = inCTOS_GetPercentage();
    if (d_OK != inRet)
        return inRet;

	inRet = inCTOS_UpdateTxnTotalAmountDiscPerc();
	if (d_OK != inRet)
		return inRet;
		
#endif


/*
   Original code
    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;
*/

    if ((srTransRec.byEntryMode != CARD_ENTRY_ICC && srTransRec.byEntryMode != CARD_ENTRY_MSR && srTransRec.byEntryMode != CARD_ENTRY_MANUAL)) {
        vdSetMPUCard(VS_FALSE);
        //inRet = inCTOS_WaveGetCardFields();
        inRet = inCTOS_WaveGetCardFieldsDiscPerc();
        if (d_OK != inRet)
            return inRet;
    }
    
    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_ONLINES_SALE);
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
	
	vdDebug_LogPrintf("*** inCTOS_FIXED_AMOUNT_SaleFlowProcess [%d][%d][%d][%d]", strHDT.inHostIndex, srTransRec.IITid, fGetMPUTrans(), fGetMPUCard());

	if (fGetMPUTrans() == TRUE )	 
	{	
		inRet = inCTOS_CheckMPUSignOn();					
		if (d_OK != inRet)			
		{			 
			inRet=inCTOS_AutoMPU_SIGNON(); 		   

			if (d_OK != inRet)				 
			return inRet;		  
		} 			   
		if (d_OK== inRet)		  
		{ 			
			vdCTOS_SetTransType(SALE);			  
			vdDispTransTitle(SALE);		 
		}	  
	}    
	

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


    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_GetCVV2();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;    

	 inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    
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

#endif

