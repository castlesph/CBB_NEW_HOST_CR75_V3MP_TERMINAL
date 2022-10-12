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


#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\print\Print.h"
#include "..\Includes\POSHost.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Database\DatabaseFunc.h"
#include "../Debug/Debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\TMS\TMS.h"
#include "..\Erm\PosErm.h"

BOOL fIsSettleAll = 0;
BOOL PrintDetail=0;
BOOL fSettSucc=0; //09232022 >> for auto signon only if settlement has a successfull settlement, else wont trigger auto signon
int inCTOS_SettlementFlowProcess(void) {
    int inRet = d_NO;

    vdCTOS_SetTransType(SETTLE);

    //display title
    vdDispTransTitle(SETTLE);

    vdDebug_LogPrintf("inCTOS_SettlementFlowProcess");

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SettlementSelectAndLoadHost();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLEMENT);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetData();
            if (d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadHost();
            if (d_OK != inRet)
                return inRet;
        }
    }

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_ChkBatchEmpty();
    if (d_OK != inRet){
        return inRet;
    }

    inRet = inCTOS_DisplayBatchTotalEx();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_SettlementFlowProcess-1 strHDT.inHostIndex[%d]", strHDT.inHostIndex);

    /*settlement of host 20/Topup/Reload is offline*/
    //if(strHDT.inHostIndex != 20 && strHDT.inHostIndex != CBPAY_HOST_INDEX) 
    if(strHDT.inHostIndex != 20 && strHDT.inHostIndex != CBPAY_HOST_INDEX && strHDT.inHostIndex != OK_DOLLAR_HOST_INDEX)    
    {
        inRet = inCTOS_PreConnect();
        if (d_OK != inRet)
        return inRet;
        
        inRet = inBuildAndSendIsoData();
        if (d_OK != inRet)
        return inRet;
    }
	
    inRet = inCTOS_PrintSettleReport();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SettlementClearBathAndAccum();
    if (d_OK != inRet)
        return inRet;


#ifdef CBB_FIN_ROUTING
	//for AUTO signon flag
	//vdSetSettleSIGNON(TRUE);
	put_env_int("STLSIGNONANYHI",0);

	vdDebug_LogPrintf("inCTOS_SettlementFlowProcess-2 strHDT.inHostIndex[%d]", strHDT.inHostIndex);

	//thandar_added in 3July2018 to do auto sign on after settlement successful
	 if(fGetCashAdvAppFlag() != TRUE &&((strHDT.inHostIndex == 16)||(strHDT.inHostIndex == 17)||(strHDT.inHostIndex == 18)||(strHDT.inHostIndex == 19)
	 	||(strHDT.inHostIndex == 22 || (strHDT.inHostIndex == 23))))	
	 {		
			vdDebug_LogPrintf("inCTOS_SettlementFlowProcess-3");

			put_env_int("STLSIGNONANYHI",strHDT.inHostIndex);// for autosignon, no need to select During SignOn after successful settlement.
			inCTOS_MPU_SIGNON();/*perform Sign on*/	
			put_env_int("STLSIGNONANYHI",0);
	 }
	// vdSetSettleSIGNON(FALSE);	
#else

	 if(fGetCashAdvAppFlag() != TRUE &&((strHDT.inHostIndex == 16)||(strHDT.inHostIndex == 17)||(strHDT.inHostIndex == 18)||(strHDT.inHostIndex == 19)))	
		inCTOS_MPU_SIGNON();/*perform Sign on*/

#endif

    inRet = inCTOS_inDisconnect();
    if (d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");



    return ST_SUCCESS;
}

int inCTOS_SETTLEMENT(void) {
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    vdDebug_LogPrintf("inCTOS_SETTLEMENT");

    inRet = inCTOS_SettlementFlowProcess();

    inCTOSS_UploadReceipt();
    //if(d_OK == inRet)
    inCTOSS_SettlementCheckTMSDownloadRequest();

    vdCTOS_SetNextAutoSettle(); //to set time for next auto settle

    vdCTOS_TransEndReset();

    return inRet;
}


//SETTLE ALL HOST
int inCTOS_SettleAMerchant(void) {
    int inRet = d_NO;
    char szDisplayMsg[50];

    memset(szDisplayMsg, 0x00, sizeof (szDisplayMsg));
//    sprintf(szDisplayMsg, "%s", strMMT[0].szMerchantName);
    sprintf(szDisplayMsg, "%s", strTCT.szMerchantName);
    CTOS_LCDTPrintXY(1, 8, "                   ");
    CTOS_LCDTPrintXY(1, 8, szDisplayMsg);
    vdDebug_LogPrintf("inCTOS_SettleAMerchant--hostname=[%s]-merchant=[%s]--", strHDT.szHostLabel, strMMT[0].szMerchantName);

    /*fix the issue that settle all will reset the trans type
    build and send will process reversal agian*/
    vdCTOS_SetTransType(SETTLE);

    inRet = inCTOS_ChkBatchEmpty();
    vdDebug_LogPrintf("inCTOS_SettleAMerchant--inRet [%d]", inRet);
	
    if (d_OK != inRet)
        return inRet;
/* EMMY comment this as per CBB request on 18092018
	if (isCheckTerminalMP200() != d_OK)//Emmy changed to fix issue with menu issue 11092018
	{
 	if(d_OK==inCTOS_ConfirmYesNo("PRINT DETAILS?"))
	vdCTOS_PrintDetailReportForSettleAll();
	}
*/
    inRet = inCTOS_DisplayBatchTotalEx();
    if (d_OK != inRet)
        return inRet;

    /*settlement of host 20/Topup/Reload is offline*/
    //if(strHDT.inHostIndex != 20 && strHDT.inHostIndex != CBPAY_HOST_INDEX) 
    if(strHDT.inHostIndex != 20 && strHDT.inHostIndex != CBPAY_HOST_INDEX && strHDT.inHostIndex != OK_DOLLAR_HOST_INDEX)     
    {
        inRet = inCTOS_PreConnect();
        if (d_OK != inRet)
            return inRet;
        
        inRet = inBuildAndSendIsoData();
        if (d_OK != inRet)
            return inRet;
    }
	
    inRet = inCTOS_PrintSettleReport();
    if (d_OK != inRet)
        return inRet;
	else
	{
		fSettSucc = TRUE;
		vdDebug_LogPrintf("inCTOS_SettleAMerchant inCTOS_PrintSettleReport - SUCCESS!!!");
	}

    inRet = inCTOS_SettlementClearBathAndAccum();
    if (d_OK != inRet)
        return inRet;


    vdDebug_LogPrintf("inCTOS_SettleAMerchant 2 strHDT.inHostIndex[%d]", strHDT.inHostIndex);

	//replace. SIGNON should be done after all host with batch are successfully settled. 09222022
	#if 0
	//ENV for SETTLE All host for SignON ALL processing
	put_env_int("STLSIGNONALLHI",0);
	
	//thandar_added in 3July2018 to do auto sign on after settlement successful
	if(fGetCashAdvAppFlag() != TRUE &&((strHDT.inHostIndex == 16)||(strHDT.inHostIndex == 17)||(strHDT.inHostIndex == 18)||(strHDT.inHostIndex == 19)
		|| (strHDT.inHostIndex == 22)|| (strHDT.inHostIndex == 23))) 	
	{
			
			put_env_int("STLSIGNONALLHI",1);
			
			inCTOS_MPU_SIGNON();/*perform Sign on*/
			
			put_env_int("STLSIGNONALLHI",0);
	}
	#endif

	
    // RSA update EFTSec working key
    inRet = inCTOSS_TWKRSAFlow();
    if (d_OK != inRet)
        return inRet;

    inRet = inMultiAP_ECRSendSuccessResponse();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if (d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	inCTOSS_UploadReceipt();

    return ST_SUCCESS;
}

int inCTOS_SettleAHost(void) {
    int inRet = d_NO;
    int inNumOfMit = 0, inNum;
    char szErrMsg[30 + 1];

	vdDebug_LogPrintf("inCTOS_SettleAHost");

    memset(szErrMsg, 0x00, sizeof (szErrMsg));
    sprintf(szErrMsg, "SETTLE %s", strHDT.szHostLabel);
    CTOS_LCDTPrintXY(1, 8, "                   ");
    CTOS_LCDTPrintXY(1, 8, szErrMsg);
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfMit);
    vdDebug_LogPrintf("inNumOfMit=[%d]-----", inNumOfMit);
    for (inNum = 0; inNum < inNumOfMit; inNum++) {
        memcpy(&strMMT[0], &strMMT[inNum], sizeof (STRUCT_MMT));
        srTransRec.MITid = strMMT[0].MITid;
        strcpy(srTransRec.szTID, strMMT[0].szTID);
        strcpy(srTransRec.szMID, strMMT[0].szMID);
        memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

        inRet = inCTOS_SettleAMerchant();
        if (d_OK != inRet) {
            memset(szErrMsg, 0x00, sizeof (szErrMsg));
            if (inGetErrorMessage(szErrMsg) > 0) {
                vdDisplayErrorMsg(1, 8, szErrMsg);
            }
            vdSetErrorMessage("");
        }
    }

    return ST_SUCCESS;
}

#ifdef HOST_ONE_BY_ONE

int inCTOS_SettleAllHosts(void) {
    int inRet = d_NO;
    int inNumOfHost = 0, inNum;
    char szBcd[INVOICE_BCD_SIZE + 1];
    char szErrMsg[30 + 1];

    vdCTOS_SetTransType(SETTLE);

    //display title
    vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
    {
        inNumOfHost = inHDTNumRecord();
        vdDebug_LogPrintf("inNumOfHost=[%d]-----", inNumOfHost);
        for (inNum = 1; inNum <= inNumOfHost; inNum++) {
            if (inHDTRead(inNum) == d_OK) {
                inCPTRead(inNum);
                srTransRec.HDTid = inNum;
                strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
                memset(szBcd, 0x00, sizeof (szBcd));
                memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
                inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
                srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

                if (inMultiAP_CheckMainAPStatus() == d_OK) {
                    //multi AP
                    inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_ALL);
                    if (d_OK == inRet)//Current AP
                    {
                        inRet = inCTOS_SettleAHost();
                    }
                    memset(szErrMsg, 0x00, sizeof (szErrMsg));
                    if (inGetErrorMessage(szErrMsg) > 0) {
                        vdDisplayErrorMsg(1, 8, szErrMsg);
                    }
                    vdSetErrorMessage("");

                } else {
                    // only one AP
                    inRet = inCTOS_SettleAHost();
                    return inRet;
                }
            }
        }
    } else// Sub APP
    {
        inRet = inCTOS_MultiAPGetData();
        if (d_OK != inRet)
            return inRet;

        inRet = inCTOS_MultiAPReloadHost();
        if (d_OK != inRet)
            return inRet;

        inRet = inCTOS_SettleAHost();
        return inRet;
    }

    return ST_SUCCESS;
}
#endif

int inCTOS_SettleAllHosts(void) {
    int inNumOfHost = 0, inNum;
    char szBcd[INVOICE_BCD_SIZE + 1];
    char szAPName[25];
    int inAPPID;

    memset(szAPName, 0x00, sizeof (szAPName));
    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("inCTOS_SettleAllHosts");

    //    inNumOfHost = inHDTNumRecord();
    inNumOfHost = inHDTMAX(); //@@IBR Modify, so we can get the maximum number of enabled host
    vdDebug_LogPrintf("inNumOfHost=[%d]-----", inNumOfHost);
    for (inNum = 1; inNum <= inNumOfHost; inNum++) {
        if (inHDTRead(inNum) == d_OK) {
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----", szAPName, strHDT.szAPName);
            if (strcmp(szAPName, strHDT.szAPName) != 0) {
                continue;
            }
            if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0) {
                continue;
            }

            //                        if(memcmp(strHDT.szHostLabel, "MPU", 3) == 0){ //@@IBR ADD 20170209
            //                            continue;
            //                        }

            //                        if(strHDT.inHostIndex == 17 || strHDT.inHostIndex == 18 || strHDT.inHostIndex == 19){
            //                            continue;
            //                        }

            inCPTRead(inNum);
            srTransRec.HDTid = inNum;
            strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
            memset(szBcd, 0x00, sizeof (szBcd));
            memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

            // only one AP
            inCTOS_SettleAHost();
        }
    }

    vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
    return ST_SUCCESS;
}

int inCTOS_SettleAllOperation(void) {
    int inRet = d_NO;
	BOOL  fOrgFlag = 0;
    int inResult = ST_SUCCESS; //Auto-settlement: return result if a settlement failed or not -- jzg

    vdCTOS_SetTransType(SETTLE);

    //display title
    vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

	/*backup and MustAutoSettle Flag will make terminal print details*/
	fOrgFlag = strTCT.fMustAutoSettle;
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(SETTLE);
/*	if(d_OK == inCTOS_ConfirmYesNo("PRINT DETAILS?"))
		PrintDetail= TRUE;
	else
		PrintDetail = FALSE;
*/
/*	if (isCheckTerminalMP200() == d_OK)
	PrintDetail= 0;
	else 
	PrintDetail= 1;
*/
	vdDebug_LogPrintf("inCTOS_SettleAllOperation");

	fIsSettleAll = 1;
	
    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inResult = inCTOS_SettleAllHosts();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);
    } else {
        inCTOS_SettleAllHosts();
    }
	fIsSettleAll = 0;
	
	strTCT.fMustAutoSettle = fOrgFlag;
	
    return ST_SUCCESS;
}

int inCTOS_SETTLE_ALL(void) 
{
    int inRet = d_NO;
	
	
    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

	
    vdDebug_LogPrintf("inCTOS_SETTLE_ALL");
	
	fSettSucc = FALSE;

    inRet = inCTOS_SettleAllOperation();

    vdDebug_LogPrintf("inCTOS_SETTLE_ALL inRet[%d] inHostIndex[%d] fSettSucc [%d]", inRet, strHDT.inHostIndex, fSettSucc);


	//http://118.201.48.214:8080/issues/75.63 #4
	//4) After Settlement All, auto Sign On All should be performed one time.
	if(inRet == ST_SUCCESS && fSettSucc == TRUE)	
	{
		#if 1
			vdDebug_LogPrintf("inCTOS_SETTLE_ALL PERFORM SIGNON START!!!");
				
				put_env_int("STLSIGNONALLHI",1);
				
				inCTOS_MPU_SIGNON();/*perform Sign on*/
				
				put_env_int("STLSIGNONALLHI",0);
				
				vdDebug_LogPrintf("inCTOS_SETTLE_ALL PERFORMED SIGNON!!!");
		#endif
	}

    vdDebug_LogPrintf("inCTOS_SETTLE_ALL HERE!!!");

	
    inCTOS_inDisconnect();

    inCTOSS_UploadReceipt();

    if (d_OK == inRet)
        inCTOSS_SettlementCheckTMSDownloadRequest();

    vdCTOS_SetNextAutoSettle(); //to set time for next auto settle

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_BATCH_TOTAL_Process(void) {
    int inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BATCH_TOTAL);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetData();
            if (d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadHost();
            if (d_OK != inRet)
                return inRet;
        }
    }


    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return;

    inRet = inCTOS_ChkBatchEmpty();
    if (d_OK != inRet)
        return inRet;

    //inRet = inCTOS_DisplayBatchTotal();
	inRet =inCTOS_DisplayBatchTotalEx2();
    if (d_OK != inRet)
        return inRet;

    vdSetErrorMessage("");

    return d_OK;

}

int inCTOS_BATCH_REVIEW_Process(void) {
    int inRet;

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectHostSetting();
    if (inRet == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_BATCH_REVIEW);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetData();
            if (d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadHost();
            if (d_OK != inRet)
                return inRet;
        }
    }


    inRet = inCTOS_CheckAndSelectMutipleMID();
    if (d_OK != inRet)
        return;

    inRet = inCTOS_ChkBatchEmpty();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_BatchReviewFlow();
    if (d_OK != inRet)
        return inRet;

    vdSetErrorMessage("");

    return d_OK;

}

int inCTOS_BATCH_TOTAL(void) {
    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    vdDispTransTitle(BATCH_TOTAL);

    inCTOS_BATCH_TOTAL_Process();

    vdCTOS_TransEndReset();

    return d_OK;

}

int inCTOS_BATCH_REVIEW(void) {
    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    vdDispTransTitle(BATCH_REVIEW);

    inCTOS_BATCH_REVIEW_Process();

    vdCTOS_TransEndReset();

    return d_OK;

}

int inCTOS_AutoSettleAllHosts(void) {
    int inNumOfHost = 0, inNum;
    char szBcd[INVOICE_BCD_SIZE + 1];
    char szAPName[25];
    int inAPPID;

    memset(szAPName, 0x00, sizeof (szAPName));
    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    //    inNumOfHost = inHDTNumRecord();
    inNumOfHost = inHDTMAX(); //@@IBR Modify, so we can get the maximum number of enabled host
    vdDebug_LogPrintf("inNumOfHost=[%d]-----", inNumOfHost);
    for (inNum = 1; inNum <= inNumOfHost; inNum++) {
        if (inHDTRead(inNum) == d_OK) {
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----", szAPName, strHDT.szAPName);
            if (strcmp(szAPName, strHDT.szAPName) != 0) {
                continue;
            }
            if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0) {
                continue;
            }

            inCPTRead(inNum);
            srTransRec.HDTid = inNum;
            strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
            memset(szBcd, 0x00, sizeof (szBcd));
            memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

            // only one AP
            inCTOS_SettleAHost();
        }
    }

    vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
    return ST_SUCCESS;
}

int inCTOS_AutoSettleAllOperation(void) {
    int inResult = ST_SUCCESS; //Auto-settlement: return result if a settlement failed or not -- jzg

    vdCTOS_SetTransType(SETTLE);

    //display title
    vdDispTransTitle(SETTLE);

    //    inRet = inCTOS_GetTxnPassword();
    //    if(d_OK != inRet)
    //        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        inResult = inCTOS_SettleAllHosts();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);
    } else {
        inCTOS_SettleAllHosts();
    }

    return ST_SUCCESS;
}

int inCTOS_AUTO_SETTLE(void) {
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_AutoSettleAllOperation();

    inCTOS_inDisconnect();

    inCTOSS_UploadReceipt();

    if (d_OK == inRet)
        inCTOSS_SettlementCheckTMSDownloadRequest();

    vdCTOS_TransEndReset();

    return inRet;
}

int inCTOS_CHECKSETTLE_ALL(void) 
{
    int inNumOfHost = 0, inNum;
    char szBcd[INVOICE_BCD_SIZE + 1];
    char szAPName[25];
    int inAPPID;
    int inRet = d_NO;
	char szTemp1[d_LINE_SIZE + 1];
	int inNumOfMerchant = 0;
    int inLoop =0;

    CTOS_LCDTClearDisplay();
    vdCTOS_TxnsBeginInit();
	
      inNumOfHost = inHDTNumRecord();
		
      //vdDebug_LogPrintf("inNumOfHost=[%d]-----", inNumOfHost);
		
        for (inNum = 1; inNum <= inNumOfHost; inNum++) 
	{
            if (inHDTRead(inNum) == d_OK) 
			{
			  if(strHDT.fHostEnable)
			 {
			  	
               // inCPTRead(inNum);
                 srTransRec.HDTid = inNum;
                 strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);	
				 
				 inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
                 //vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
                for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
               {
                  if((inRet = inMMTReadRecord(strHDT.inHostIndex,strMMT[inLoop-1].MITid)) !=d_OK)
                   {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex,inRet);
                    continue;
                    //break;
                  }
                else 
                {
                    if(strMMT[0].fMMTEnable)
                        {				
				                    inRet = inCTOS_ChkBatchEmpty();
				
									vdDebug_LogPrintf("inCTOS_ChkBatchEmpty inRet=[%d]-----",inRet);//thandar
							   
									if(d_OK == inRet)
									{

											memset(szTemp1, 0x00, sizeof(szTemp1));

											CTOS_LCDTClearDisplay();

											vdDisplayErrorMsg(1, 5, "Do settlement");

											sprintf(szTemp1,"%s",srTransRec.szHostLabel);
								
											vdDisplayErrorMsg(1, 6, szTemp1);

											vdDisplayErrorMsg(1, 7, "BATCH NOT EMPTY");											
																	 
											vduiWarningSound();
											
											inCTOS_DisplayBatchTotalEx2();														
										
				
											 strMMT[0].fMustSettFlag = CN_TRUE;
											 inMMTSave(strMMT[0].MMTid);
									}	

				

						           
                    	}
                }

				

				
					
            	}
              		 
            }

			    else
						continue;

            	}

			
			  else
				continue;
        
      }

    //return inRet;
	return ST_SUCCESS;
}
