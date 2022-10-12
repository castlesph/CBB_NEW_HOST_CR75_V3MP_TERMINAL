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
#include "..\Includes\POSSetting.h"
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
#include "..\Includes\CTOSInput.h"
#include "..\Erm\PosErm.h"
#include "..\TMS\TMS.h"

BOOL fIPPSettleFlag;

void vdSetSettleIPPFlag(BOOL flag){
    fIPPSettleFlag = flag;
}

BOOL fIPPGetSettleFlag(void){
    return fIPPSettleFlag;
}


static void vdSetIPPTrans(BOOL flag){
    
    srTransRec.fIsInstallment = flag;
}

static void vdSetIPPTMKRef(void){
    put_env("#IPPTMKREF", "00000303", 8);
}

static int inSelectInstallmentTerms(void){
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "INSTALLMENT";
    char szTermsMenu[1024+1];
    int inIPSCount = 0;
    int i;
    char szTemp[10+1];
	int inRet = d_OK;

	vdDebug_LogPrintf("=====inSelectInstallmentTerms=====");
    
    memset(szTermsMenu, 0x00, sizeof(szTermsMenu));
    
    inIPSCount = inIPSMAX();
    
    for(i = 1; i <= inIPSCount; i++)
	{
        inIPSRead(i);
        
        if(i < inIPSCount){
            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%d Months\n", strIPS.inInstallmentPlan);
            strcat(szTermsMenu, szTemp);
        } else if( i == inIPSCount){
            memset(szTemp, 0x00, sizeof(szTemp));
            sprintf(szTemp, "%d Months", strIPS.inInstallmentPlan);
            strcat(szTermsMenu, szTemp);
        }
    }

	/*Flush before selection*/
	CTOS_KBDBufFlush();
		
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szTermsMenu, TRUE);

    if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;
    }

	vdDebug_LogPrintf("IPP MenuDisplay key[%d]", key);
	
    if(key == d_KBD_CANCEL)
	{
		vdSetErrorMessage("USER CANCEL");
        return -1;
	}
    
    inRet = inIPSRead(key);
    
    sprintf(srTransRec.stIPPinfo.szInstallmentTerms, "%03d", strIPS.inInstallmentPlan);
    strcpy(srTransRec.stIPPinfo.szIPPSchemeID, strIPS.szSchemeID);

	vdDebug_LogPrintf("srTransRec.stIPPinfo.szIPPSchemeID[%s]", srTransRec.stIPPinfo.szIPPSchemeID);
	
    return d_OK;
}

void vdGetIPPSchemeID(char *szSchemeID){
    int inSchemeID = 0;
    
    inSchemeID = atoi(srTransRec.stIPPinfo.szInstallmentTerms);
    
    switch(inSchemeID){
        case 3:
            strcpy(szSchemeID, "IPPSC4");
            break;
        case 6:
            strcpy(szSchemeID, "IPPSC9");
            break;
        case 12:
            strcpy(szSchemeID, "IPPSC0");
            break;
        default:
            strcpy(szSchemeID, "IPPSC0");
            break;
    }
}

static int inCTOS_IPP_InstallmentFlowProcess(void){
    int inRet = d_NO;

    vdCTOS_SetTransType(SALE);
    vdDispTransTitle(SALE);


	vdDebug_LogPrintf("inCTOS_IPP_InstallmentFlowProcess");
	
    inRet = inCTOSS_CheckMemoryStatus();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

	inRet = inCTOS_GetTxnBaseAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_GetTxnTipAmount();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateTxnTotalAmount();
    if (d_OK != inRet)
        return inRet;
    
    vdSetMPUCard(VS_FALSE);
    inRet = inCTOS_WaveGetCardFields();
    if (d_OK != inRet)
        return inRet;
    
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
    
    inRet = inCTOS_GetCVV2();
    if(d_OK != inRet)
        return inRet;
    
    inRet = inSelectInstallmentTerms();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;

    if (VS_FALSE == fGetMPUCard()) {
        inRet = inCTOS_EMVProcessing();
        if (d_OK != inRet)
            return inRet;
    }

	vdDebug_LogPrintf("inCTOS_IPP_InstallmentFlowProcess1 fGetMPUCard : [%d]", fGetMPUCard());

    inRet = inCTOSS_GetOnlinePIN();
    if (d_OK != inRet)
        return inRet;

    if (fGetMPUCard() == TRUE) {
        inRet = inMPU_GetOnlinePIN();
        if (d_OK != inRet)
            return inRet;
    }

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_IPP_InstallmentFlowProcess2");

    inRet = inCTOS_SaveBatchTxn();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_IPP_InstallmentFlowProcess3");

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

    return d_OK;
}

int inCTOS_IPP_INSTALLMENT(void){
    int inRet = d_NO;

    vdCTOS_TxnsBeginInit();

    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;
    
    vdSetIPPTrans(TRUE);
    
    inRet = inCTOS_IPP_InstallmentFlowProcess();
    
    vdSetIPPTrans(FALSE);

	vdSetMPUTrans(FALSE);// Temp fix the issue that use CBB-UPI card, will set card type as 1, 

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
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_IPP_InstallmentVoidFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(VOID);
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

    inRet = inCTOS_CheckVOID_IPP();
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
    
    inCDTRead(srTransRec.CDTid);

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

    if (isCheckTerminalMP200() == d_OK) {
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
    
    //inRet = inMultiAP_ECRSendSuccessResponse();
        
    return d_OK;
}

int inCTOS_IPP_VOID(void)
{
    int inRet = d_NO;
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_IPP_InstallmentVoidFlowProcess();

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
	
    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_IPP_SettlementFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(SETTLE);
    vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_IPP_SettlementSelectAndLoadHost();
    if(d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLEMENT);
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

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return inRet;
        }
    }

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_ChkBatchEmpty();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_DisplayBatchTotalEx();
    if(d_OK != inRet)
        return inRet;
        
        inRet = inCTOS_GetInvoice();
        if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_PrintSettleReport();
    if(d_OK != inRet)
        return inRet; 
	else		
		vdSetSettleIPPFlag(FALSE); // termp, for testing. allways followed IPP setlement outline if settle was made on IDLE settlement menu.

    inRet = inCTOS_SettlementClearBathAndAccum();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return ST_SUCCESS;
}

int inCTOS_IPP_SETTLEMENT(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    vdSetSettleIPPFlag(TRUE);
    
    inRet = inCTOS_IPP_SettlementFlowProcess();
    
    vdSetSettleIPPFlag(FALSE);

    inCTOSS_UploadReceipt();
    inCTOSS_SettlementCheckTMSDownloadRequest();
    vdCTOS_TransEndReset();

    return inRet;
}

static int inCTOS_IPP_SignOnFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(IPP_SIGN_ON);
    
    //display title
    vdDispTransTitle(IPP_SIGN_ON);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHost();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;
    
    //vdSetIPPTMKRef();

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

//	vdSetMPUSignOnStatus(1);

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	/*Display OK*/
	vduiDisplayStringCenter(8, "SIGN-ON SUCCESS");
    vduiWarningSound();
	WaitKey(3);

    return ST_SUCCESS;
}

int inCTOS_IPP_SIGNON(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_IPP_SignOnFlowProcess();
    
//    ushIPP_WriteEncTPK();
	
    vdCTOS_TransEndReset();

    return inRet;
}
