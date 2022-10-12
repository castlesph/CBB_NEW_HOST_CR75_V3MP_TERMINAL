#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <vwdleapi.h>
#include <sqlite3.h>

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
#include "..\Includes\Wub_lib.h"
#include "..\Database\DatabaseFunc.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\CardUtil.h"
#include "..\Includes\POSSetting.h"
#include "..\PCI100\COMMS.h"
#include "..\erm\Poserm.h"

//#include "POSCtls.h"
//#include "POSWave.h"

#if 0
int inWaveTransType = 0;

void vdCTOSS_SetWaveTransType(int type)
{
	inWaveTransType = type;
}

int inCTOSS_GetWaveTransType(void)
{
	return inWaveTransType;
}

int inCTOS_MultiAPReloadWaveData(void)
{
	if (srTransRec.usChipDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baChipData, srTransRec.usChipDataLen);
	}
		
	if (srTransRec.usAdditionalDataLen > 0)
	{			
		vdCTOSS_WaveGetEMVData(srTransRec.baAdditionalData, srTransRec.usAdditionalDataLen);
	}
	
	return (d_OK);
}
#endif

int inCTOS_AlipayFlowProcess(void)
{
    int inRet = d_NO;

    USHORT ushEMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
	int inEVENT_ID = 0;

	BOOL fRetryTranStatEnq;
	
	//memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));
    inFLGRead(1);
	vdDebug_LogPrintf("inCTOS_AlipayFlowProcess");

    vdDebug_LogPrintf("strFLG.fRetryTranStatEnq:%d", strFLG.fRetryTranStatEnq);
	
    fRetryTranStatEnq=strFLG.fRetryTranStatEnq;
    
    inRet = inCTOSS_CheckMemoryStatus();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    if (fGetECRTransactionFlg() == TRUE) {
        if (inMultiAP_CheckMainAPStatus() == d_OK) {
            if (srTransRec.byTransType == REFUND)
                inEVENT_ID = d_IPC_CMD_WAVE_REFUND;
            else
                inEVENT_ID = d_IPC_CMD_WAVE_SALE;
            inRet = inCTOS_MultiAPSaveData(inEVENT_ID);
            if (d_OK != inRet)
                return inRet;
        } else {
            if (inMultiAP_CheckSubAPStatus() == d_OK) {
                inRet = inCTOS_MultiAPGetData();
                if (d_OK != inRet)
                    return inRet;

                inRet = inCTOS_MultiAPReloadWaveData();
                if (d_OK != inRet)
                    return inRet;
            }
        }
    }

    inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnTipAmount();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
    

    //inRet = inCTOS_WaveGetCardFields();
    //if(d_OK != inRet)
    //    return inRet;
    
    inRet = inCTOSS_GetPOSTransRef();
    if(d_OK != inRet)
        return inRet;

//	inRet = inCTOSS_ScanBarcode();
//	if(d_OK != inRet)
//	 return inRet;
    
//    inRet = inCTOS_SelectALipayHost();
//    if(d_OK != inRet)
//        return inRet;

	//vdCTOSS_DisplayQRCodeOneLine("\x30\x34\x30\x31\x68\x74\x74\x70\x73\x3A\x2F\x2F\x71\x72\x2E\x61\x6C\x69\x70\x61\x79\x2E\x63\x6F\x6D\x2F\x62\x61\x78\x30\x33\x35\x36\x34\x7A\x37\x6D\x62\x6C\x6C\x72\x71\x6A\x73\x72\x70\x30\x30\x63\x66");

    inRet = inCTOS_SelectHostAlipay();
    if(d_OK != inRet)
        return inRet;


	//inRet = inCTOS_SelectALipayHostEx();
	//if(d_OK != inRet)
		//return inRet;

#if 0
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
    	if (srTransRec.byTransType == REFUND)
			inEVENT_ID = d_IPC_CMD_WAVE_REFUND;
		else
			inEVENT_ID = d_IPC_CMD_WAVE_SALE;
        inRet = inCTOS_MultiAPSaveData(inEVENT_ID);
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

			inRet = inCTOS_MultiAPReloadWaveData();
            if(d_OK != inRet)
                return inRet;
        }
        inRet = inCTOS_MultiAPCheckAllowd();
        if(d_OK != inRet)
            return inRet;
    }
#endif

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;



		

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_CheckIssuerEnable();
    //if(d_OK != inRet)
    //    return inRet;
//
//    inRet = inCTOS_CheckTranAllowd();
//    if(d_OK != inRet)
//        return inRet;
	//Emmy moved here 12092018
		
		inRet = inCTOS_CheckMustSettle();
	
		vdDebug_LogPrintf("inCTOS_AlipayFlowProcess inCTOS_CheckMustSettle inRet[%d] ",inRet);
	
		if(d_OK != inRet)
		{
			/*for alipay display settle Visa/Master host*/
//			vdSetErrorMessage("MUST SETTLE MC/Vs");
			vdDisplayErrorMsg(1, 8,"MUST SETTLE MC/VS");
			return inRet;
		}



    inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_GetCVV2();
    //if(d_OK != inRet)
    //    return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inCTOS_QRInquiry_SaleFlowProcess();
    if(d_OK != inRet)
        return inRet;
    
    inRet=inConfirmQRCode();
	if(d_OK != inRet)
	 return inRet;
	
  	//inRet = inCTOSS_ScanBarcode();
	//if(d_OK != inRet)
	// return inRet;
	
    //inRet = inCTOS_EMVProcessing();
    //if(d_OK != inRet)
    //    return inRet;   
    
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
	
	#if 0
	inRet = ushCTOS_ePadSignature();
    if(d_OK != inRet)
        return inRet;
	#endif
	
    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }	

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
        return inRet;
	
    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;

    //inRet = inCTOS_EMVTCUpload();
    //if(d_OK != inRet)
   //     return inRet;
   // else
   vdSetErrorMessage("");

    return d_OK;
}


int inCTOS_ALIPAY_SALE(void)
{
    int inRet = d_NO;

    vdDebug_LogPrintf("inCTOS_ALIPAY_SALE [%d]", strTCT.fEnableAlipayMenu);

	if(FALSE == strTCT.fEnableAlipayMenu)
	{
		vduiClearBelow(2);
		vduiWarningSound();
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
		WaitKey(3);
		return d_OK;
	}

    vdCTOS_TxnsBeginInit();

	vdCTOS_SetTransType(ALIPAY_SALE);

	vdCTOSS_GetAmt();

    vdDispTransTitle(ALIPAY_SALE);
	
    inRet = inCTOS_AlipayFlowProcess();

    inCTOS_inDisconnect();

	inCTOS_ALIPAY_Reversal();

    if(strTCT.fUploadReceiptIdle != TRUE){
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

int inCTOS_QRInquiry_SaleFlowProcess(void)
{
    int inRet = d_NO;
    TRANS_DATA_TABLE srtmpTransRec;

	srTransRec.fAlipay=TRUE;
	
    memset(&srtmpTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    memcpy(&srtmpTransRec, &srTransRec, sizeof(TRANS_DATA_TABLE));
    
    vdCTOS_SetTransType(QR_INQUIRY);
    
    inRet = inBuildAndSendIsoData();
    if(inRet != d_OK)
        return inRet;
    
    memcpy(srtmpTransRec.byQRData, srTransRec.byQRData, srTransRec.inQRDataLen);
    srtmpTransRec.inQRDataLen = srTransRec.inQRDataLen;
    memcpy(srtmpTransRec.szRRN, srTransRec.szRRN, RRN_BYTES);
	memcpy(srtmpTransRec.szOrgInvoiceNo, srTransRec.szInvoiceNo, 3);
	
    memset(&srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    memcpy(&srTransRec, &srtmpTransRec, sizeof(TRANS_DATA_TABLE));

    return d_OK;
}

int inCTOS_ALIPAY_Reversal(void) 
{
    int inRet = d_NO;

    vdDebug_LogPrintf("inCTOS_ALIPAY_Reversal");

    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_AlipayReversalProcess();

    inCTOS_inDisconnect();

    vdCTOS_TransEndResetEx();

    return inRet;
}

int inCTOS_AlipayReversalProcess(void) 
{
    int inRet = d_NO;

    CHAR szFileName[d_BUFF_SIZE];

    vdDebug_LogPrintf("inCTOS_AlipayReversalProcess");

    //vdCTOS_SetTransType(AUTO_REVERSAL);

    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    /*MPU*/
    inRet = inCTOS_SelectHostAlipay();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return inRet;

    memset(szFileName, 0, sizeof (szFileName));
    sprintf(szFileName, "%s%02d%02d.rev"
            , strHDT.szHostLabel
            , strHDT.inHostIndex
            , srTransRec.MITid);
	
    DebugAddSTR("inCTOS_ReversalProcess", szFileName, 12);
    if ((inRet = inMyFile_CheckFileExist(szFileName)) < 0) {
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        return inRet;
    }

    CTOS_LCDTClearDisplay();
    vdCTOS_SetTransType(AUTO_REVERSAL);
    vdDispTransTitle(AUTO_REVERSAL);

    inRet = inCTOS_PreConnect();
    if (d_OK != inRet)
        return inRet;

	if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
	{
		inCTOS_inDisconnect();
		return ST_ERROR;
	}

    inRet = inSnedReversalToHost(&srTransRec, ALIPAY_REVERSAL);

    if(d_OK != inRet)
        return inRet;

    inMyFile_ReversalDelete();
	
    return d_OK;
}


