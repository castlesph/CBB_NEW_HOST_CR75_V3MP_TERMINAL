/*******************************************************************************

 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>


#include "..\Debug\debug.h"
#include "..\Includes\POSTypedef.h"

#include "..\Database\DatabaseFunc.h"
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

#ifdef CBPAY_DV
#include "..\QR_Payment\POSQR_Payment.h"
#endif
extern BOOL ErmTrans_Approved;


int inCTOS_MultiAPBatchSearchByRRN(int IPC_EVENT_ID) {
    int inResult = d_NO;

    inResult = inDatabase_BatchSearchByRRN(&srTransRec, srTransRec.szRRN);

    DebugAddSTR("inCTOS_BatchSearch", "Processing...", 20);

    DebugAddINT("inCTOS_BatchSearch", inResult);

    if (inResult != d_OK) {
        inResult = inCTOS_MultiAPALLAppEventID(IPC_EVENT_ID);
        if ((inResult == d_SUCCESS) || (inResult == d_FAIL))
            return d_NO;

        vdSetErrorMessage("NO RECORD FOUND");
        return d_NO;
    }

    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 3);

    return inResult;
}

static BOOL fCheckIsMPUTrans(void) {
    inCDTRead(srTransRec.CDTid);

    if (strCDT.inType == 1)
        return TRUE;
    else
        return FALSE;
}

int inCTOS_MPUVoidFlowProcess(void) {
    int inRet = d_NO;

	
    vdCTOS_SetTransType(VOID);

    //display title
    vdDispTransTitle(VOID);

	vdDebug_LogPrintf("**inCTOS_MPUVoidFlowProcess**");

    inRet = inCTOS_GetTxnPassword();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if (d_OK != inRet)
        return inRet;


    inRet = inCTOS_GeneralGetInvoice();
    if (d_OK != inRet)
        return inRet;

    if (inMultiAP_CheckMainAPStatus() == d_OK) {
        //        inRet = inCTOS_MultiAPBatchSearchByRRN(d_IPC_CMD_VOID_SALE);
        //        if(d_OK != inRet)
        //            return inRet;

        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_VOID_SALE);
        if (d_OK != inRet)
            return inRet;
    } else {
        if (inMultiAP_CheckSubAPStatus() == d_OK) {
            inRet = inCTOS_MultiAPGetVoid();
            if (d_OK != inRet)
                return inRet;
        }

		vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess ABCD");
		
        inRet = inCTOS_BatchSearch();
        if (d_OK != inRet)
            return inRet;
        
//        inRet = inCTOS_BatchSearchByRRN();
//        if(d_OK != inRet)
//            return inRet;

    }

	#ifdef CB_MPU_NH_MIGRATION
		// new adjustment for MPU new host application - 05/10/19 - based "POS function and  Regulation" doc
		/*if( srTransRec.HDTid == 17 ||
			srTransRec.HDTid == 18 ||
			srTransRec.HDTid == 19)
			*/
		vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess srTransRec.IITid [%d]", srTransRec.IITid);

		if(srTransRec.IITid == 7 || srTransRec.IITid == 8 || srTransRec.IITid == 9)	
		{

				vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess2 - byTransType [%d] byOrgTransType [%d] fGetMPUTrans[%d] fGetMPUCard[%d] inCurrencyIndex[%d]srTransRec.HDTid[%d]", 
									srTransRec.byTransType, 
									srTransRec.byOrgTransType, 
									fGetMPUTrans(), 
									fGetMPUCard(),
									strCST.inCurrencyIndex,
									srTransRec.HDTid);


				
				if(srTransRec.byTransType == PRE_AUTH)
				{			
					//orig code 09212022
					#if 0
					inRet = inCTOS_BatchSearch();
					if (d_OK != inRet)
						return inRet;
					#endif
					// as per CB if HDTID 7 or 21 and IITID is 7,8 or 9, skip functions below - 10-28-2021
					//if(srTransRec.HDTid != 7 && srTransRec.HDTid != 21)
					//fix for http://118.201.48.214:8080/issues/75 #35 - 2)When we perform Preauth void trxs, it should be displayed "Preauth Void " in terminal UI after typing Invoice number(Trace number)
					if(srTransRec.HDTid != 22 && srTransRec.HDTid != 23)
					{
					
							vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo XXXXXXXX");
							// new placement 09212022 - for testing
							#if 1
							inRet = inCTOS_BatchSearch();
							if (d_OK != inRet)
								return inRet;
							#endif
							
							memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
							inRet = inCTOS_WaveGetCardFields();
							if (d_OK != inRet)
								return inRet;		

							vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo after inCTOS_WaveGetCardFields [%d][%d]", fGetMPUCard(), fGetMPUTrans());

							if ((fGetMPUCard() == TRUE) || (fGetMPUTrans() == TRUE)) 
							{
								vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo GET PIN inMPU_GetOnlinePIN");
							
								inRet = inMPU_GetOnlinePIN();
								if (d_OK != inRet)
									return inRet;
							}		

							//POSIC023 - F12 in the pre-auth cancel request message need to be same as pre-auth message. #1
							inRet = inDatabase_BatchSearchByInvNo(&srTransRec, srTransRec.szInvoiceNo);
							vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo Result:[%d]", inRet);
							
							if (d_OK != inRet)
							{
								vdSetErrorMessage("NO RECORD FOUND");
								//return inRet;	
								return d_NOT_RECORD;
							}

							DebugAddHEX("inCTOS_MPUVoidFlowProcess3.srTransRec.szTime", srTransRec.szTime, 3);	
					}
					else						
					{
						//fix for uble to void Preauth transaction after settlement
						//http://118.201.48.214:8080/issues/75#change-4195.63 #3
						//3) MPU, MPU-JCB, MPU-UPI OffUs Preauth Void after settlement -> No Record Found The expected flow is Preauth trnx should be able to void before Preauth record date.
						vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo HDTID IS EITHER 22 OR 23 !!!");
						inRet = inDatabase_BatchSearchByInvNo(&srTransRec, srTransRec.szInvoiceNo);
						vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo Result:[%d]", inRet);
						
						if (d_OK != inRet)
						{
							vdSetErrorMessage("NO RECORD FOUND");
							//return inRet; 
							return d_NOT_RECORD;
						}

						
					}
				}

		}
	#endif

	vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo YYYYYYY");

    inRet = inCTOS_CheckVOID();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("1.srTransRec.fAlipay: %d    ----- srTransRec.HDTid : %d", srTransRec.fAlipay, srTransRec.HDTid);
	vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess4 inCurrencyIndex:[%d]", strCST.inCurrencyIndex);

#ifdef CBPAY_DV	
    if(srTransRec.HDTid == CBPAY_HOST_INDEX)
    {
        vdDebug_LogPrintf("proceed to QR void");
        inRet = inCTOS_CBPayQRVoidFlowProcess();
        return inRet;
    }
#endif

#ifdef OK_DOLLAR_FEATURE
		if(srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
		{
			vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess proceed to QR void");
	
			inRet = inCTOS_VoidOKDGetCustomerNO();
			if (d_OK != inRet)
				return inRet;
	
			inRet = inCTOS_VoidOKDGetCustomerPword();
			if (d_OK != inRet)
				return inRet;
			
			inRet = inCTOS_OKDQRVoidFlowProcess();{
			
			vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess inRet = inCTOS_OKDQRVoidFlowProcess [%d]", inRet);
			return inRet;
			}
		}
	
#endif
	vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess proceed to QR void AFTER!!!");

    	
    if(srTransRec.fAlipay != TRUE )
    {
        inRet = inCTOS_LoadCDTandIIT();
        if(d_OK != inRet)
            return inRet;
    }

    //vdDebug_LogPrintf("2.srTransRec.fAlipay: %d", srTransRec.fAlipay);
	
    inRet = inCTOS_VoidSelectHost();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess5 - byTransType [%d]fGetMPUTrans[%d]fGetMPUCard[%d]inCurrencyIndex[%d]HDTinCurrencyIdx[%d]", 
		srTransRec.byTransType, fGetMPUTrans(), fGetMPUCard(), strCST.inCurrencyIndex, strHDT.inCurrencyIdx);

    inRet = inCTOS_CheckMustSettle();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess5.1 - byTransType [%d]fGetMPUTrans[%d]fGetMPUCard[%d]inCurrencyIndex[%d]", 
		srTransRec.byTransType, fGetMPUTrans(), fGetMPUCard(), strCST.inCurrencyIndex);

    inRet = inCTOS_ConfirmInvAmt();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess5.2 - byTransType [%d]fGetMPUTrans[%d]fGetMPUCard[%d]inCurrencyIndex[%d]", srTransRec.byTransType, fGetMPUTrans(), fGetMPUCard(), strCST.inCurrencyIndex);

    if (fGetMPUTrans() == TRUE) {
        vdSetMPUCard(VS_FALSE);


		vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess5.3 - byTransType [%d]fGetMPUTrans[%d]fGetMPUCard[%d]inCurrencyIndex[%d]", srTransRec.byTransType, fGetMPUTrans(), fGetMPUCard(), strCST.inCurrencyIndex);

		#ifdef CB_MPU_NH_MIGRATION
			#if 0
	        memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
	        inRet = inCTOS_WaveGetCardFields();
	        if (d_OK != inRet)
	            return inRet;

	        if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
	            inRet = inMPU_GetOnlinePIN();
	            if (d_OK != inRet)
	                return inRet;
	        }		
			#endif
		#endif

    }

    inRet = inBuildAndSendIsoData();
    if (d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess6 - byTransType [%d]fGetMPUTrans[%d]fGetMPUCard[%d]inCurrencyIndex[%d]",
		srTransRec.byTransType, fGetMPUTrans(), fGetMPUCard(), strCST.inCurrencyIndex);

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

    if(srTransRec.fAlipay != TRUE)
    {
       inRet = ushCTOS_ePadSignature();
       if (d_OK != inRet)
           return inRet;
    }
	
    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;

    inRet = ushCTOS_printReceipt();
    if (d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();

    return d_OK;
}

int inCTOS_MPU_VOID(void) {
    int inRet = d_NO;
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_MPUVoidFlowProcess();
    vdSetMPUTrans(FALSE);

	vdDebug_LogPrintf("inCTOS_MPU_VOID [%d]", inRet);

    inCTOS_inDisconnect();


	vdDebug_LogPrintf("inCTOS_MPU_VOID - after disconnect [%d]", ErmTrans_Approved);

	//fix for issue Should display "E-RECEIPT SENT SUCCESSFULLY" or other related message - 10002
	/*#ifdef OK_DOLLAR_FEATURE
	if(ErmTrans_Approved == TRUE)
	{
    	if(strTCT.fUploadReceiptIdle != TRUE){
        	inCTOSS_UploadReceipt();
    	}
		ErmTrans_Approved = FALSE;
	}
	#else
	if(strTCT.fUploadReceiptIdle != TRUE){
		inCTOSS_UploadReceipt();
	}
	
	#endif*/
    if(strTCT.fUploadReceiptIdle != TRUE){
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
