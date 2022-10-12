#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <emv_cl.h>

#include "../Includes/wub_lib.h"
#include "../Includes/Encryption.h"
#include "../Includes/msg.h"
#include "../Includes/myEZLib.h"

#include "../Includes/V5IsoFunc.h"
#include "../Includes/POSTypedef.h"
#include "../Comm/V5Comm.h"
#include "../FileModule/myFileFunc.h"
#include "../UI/Display.h"
#include "../Includes/Trans.h"
#include "../UI/Display.h"
#include "../Accum/Accum.h"
#include "../POWRFAIL/POSPOWRFAIL.h"
#include "../DataBase/DataBaseFunc.h"
#include "../Includes/POSTrans.h"
#include "../Debug/Debug.h"
#include "../Includes/POSTrans.h"
#include "../Includes/Showbmp.h"
#include "../Includes/POSHost.h"
#include "../Includes/ISOEnginee.h"
#include "../Includes/EFTSec.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\CfgExpress.h"
#include "..\print\Print.h"

#include "../Htle/htlesrc.h"
#include "../ctls/POSCtls.h"
#include "..\PinPad\pinpad.h"
#include "..\Includes\POSMPUSignOn.h"
#include "..\Includes\POSMPUSettle.h"
#include "..\Includes\POSIpp.h"

#include <emvaplib.h>

int inDataCnt;
BYTE TempKey;
int inFinalSend;
BYTE szDataForMAC[512];
int inMacMsgLen;


BYTE byField_02_ON;
BYTE byField_14_ON;
BYTE byField_35_ON;
BYTE byField_45_ON;
BYTE byField_48_ON;

extern USHORT usPreconnectStatus;

BYTE baData61[20 + 1];
BYTE baData62[6 + 1];

/*for MPU MAC Field*/
BYTE baMPU_ReqMTI[2];
BYTE baMPU_RespMTI[2];
BYTE baMPU_PAN[20];
BYTE baMPU_DE07[5];
BYTE baMPU_DE61[20];

ULONG ulOrgTraceNum = 0;
BYTE  byRRG[RRN_BYTES+1];

char DE2[19+1];
char DE3[6+1];
char DE4[12+1];
char DE11[6+1];
char DE12[12+1];
char DE14[4+1];
char DE18[4+1];
char DE32[4+1];
char DE41[8+1];
char DE43[40+1];
char DE48[128+1];
char DE49[7+1];
char DE52[32+1];
char DE62[768+1];
char szYYDateTime[6+1];
char szRandomKey[32+1];


ISO_FUNC_TABLE srIsoFuncTable[] ={
    {
        inPackMessageIdData, /*inPackMTI*/
        inPackPCodeData, /*inPackPCode*/
        vdModifyBitMapFunc, /*vdModifyBitMap*/
        inCheckIsoHeaderData, /*inCheckISOHeader*/
        inAnalyseReceiveData, /*inTransAnalyse*/
        inAnalyseAdviceData /*inAdviceAnalyse*/
    },
};

//extern int inPrintISOPacket(BOOL fSendPacket, unsigned char *pucMessage, int inLen);
static TRANS_DATA_TABLE *ptsrISOEngTransData = NULL;

void vdSetISOEngTransDataAddress(TRANS_DATA_TABLE *srTransPara) {
    ptsrISOEngTransData = srTransPara;
}

TRANS_DATA_TABLE* srGetISOEngTransDataAddress(void) {
    return ptsrISOEngTransData;
}

void vdDispTextMsg(char *szTempMsg) {
    //    CTOS_LCDUClearDisplay();
    //    CTOS_LCDTPutchXY(1,8,"TEST");
    //    CTOS_KBDGet(&TempKey);
}

void vdDecideWhetherConnection(TRANS_DATA_TABLE *srTransPara) {

	vdDebug_LogPrintf("**vdDecideWhetherConnection START**");
	
    vdDebug_LogPrintf("vdDecideWhetherConnection . BefTrnsType(%d) UpLoad(%d) Offline (%d) shTransResult(%d)byEntryMode(%d)", srTransPara->byTransType,
            srTransPara->byUploaded,srTransPara->byOffline, srTransPara->shTransResult, srTransPara->byEntryMode);

	

//    if (srTransPara->byTransType != VOID) {
    if (srTransPara->byTransType != VOID && srTransPara->byTransType != VOID_PREAUTH) {
        if (srTransPara->byTransType == SALE_TIP || srTransPara->byTransType == SALE_ADJUST) {
            srTransPara->byOffline = CN_TRUE;

        } else if (srTransPara->byTransType == SALE_OFFLINE) {
            srTransPara->byUploaded = CN_FALSE;
            srTransPara->byOffline = CN_TRUE;
//            srTransPara->byUploaded = CN_TRUE;
//            srTransPara->byOffline = CN_FALSE;
        }
		#if 0
		else if (srTransPara->byTransType == MPU_PREAUTH_COMP_ADV) {
            srTransPara->byUploaded = CN_FALSE;
            srTransPara->byOffline = CN_TRUE;
        }
		#endif
		else if (srTransPara->byTransType == SALE &&
                srTransPara->byEntryMode == CARD_ENTRY_ICC &&
                srTransPara->shTransResult == TRANS_AUTHORIZED &&
                0 == memcmp(&srTransPara->szAuthCode[0], "Y1", 2) &&
                0x40 == srTransPara->stEMVinfo.T9F27) {
            srTransPara->byUploaded = CN_FALSE;
            srTransPara->byOffline = CN_TRUE;
        } else if (srTransPara->byTransType == SALE &&
                srTransPara->byEntryMode != CARD_ENTRY_ICC &&
                srTransPara->shTransResult == TRANS_AUTHORIZED &&
                srTransPara->byOffline == CN_TRUE &&
                fAmountLessThanFloorLimit() == d_OK) {
			vdDebug_LogPrintf("**vdDecideWhetherConnection START*1*");
                
            srTransPara->byUploaded = CN_FALSE;
            srTransPara->byOffline = CN_TRUE;
        }
        else if(srTransPara->byTransType == CHANGE_PIN)
	{
            srTransPara->byOffline = CN_FALSE;
        }
		else 
		{
            srTransPara->byUploaded = CN_TRUE;
            srTransPara->byOffline = CN_FALSE;

		vdDebug_LogPrintf("**vdDecideWhetherConnection START*1.1*");
			
        }
    } else {

		vdDebug_LogPrintf("**vdDecideWhetherConnection START*2*");
	
        //Start Should be Online void the Intial SALE amount.
        if (srTransPara->byOrgTransType == SALE) {
			vdDebug_LogPrintf("**vdDecideWhetherConnection START*3*");
			
            if (srTransPara->byUploaded == CN_FALSE) //Y1 or below floor limit, and not upload yet
                srTransPara->byOffline = CN_TRUE;
            else{
                srTransPara->byOffline = CN_FALSE;
				vdDebug_LogPrintf("**vdDecideWhetherConnection START*4*");
				
            }
			
        } else if (srTransPara->byOrgTransType == SALE_OFFLINE) {
            if (srTransPara->byUploaded == CN_FALSE)
                srTransPara->byOffline = CN_TRUE;
            else
                srTransPara->byOffline = CN_FALSE; //piggy back and upload to host already
        
        }
		#if 0
		else if (srTransPara->byOrgTransType == MPU_PREAUTH_COMP_ADV) {
            if (srTransPara->byUploaded == CN_FALSE)
                srTransPara->byOffline = CN_TRUE;
            else
                srTransPara->byOffline = CN_FALSE; //piggy back and upload to host already
        }
		#endif
		else if (srTransPara->byOrgTransType == SALE_TIP) {
            if (srTransPara->byUploaded == CN_FALSE) //Y1 or below floor limit, and not upload yet
                srTransPara->byOffline = CN_TRUE;
            else
                srTransPara->byOffline = CN_FALSE;
        }
        //End Should be Online void the Intial SALE amount.
    }
    vdDebug_LogPrintf(". AftTrnsType(%d) srTransPara->byOrgTransType[%d]byEntryMode[%d] shTransResult[%d] szAuthCode[%s] 9F27[%02X] UpLoad(%d) Offline (%d)",
            srTransPara->byTransType,
            srTransPara->byOrgTransType,
            srTransPara->byEntryMode,
            srTransPara->shTransResult,
            srTransPara->szAuthCode,
            srTransPara->stEMVinfo.T9F27,
            srTransPara->byUploaded,
            srTransPara->byOffline);

    vdMyEZLib_LogPrintf("**vdDecideWhetherConnection END**");
    return;
}

/************************************************************************
Function Name: inBuildAndSendIsoData()
Description:
    To generate iso data and send to host
Parameters:
    [IN] srTransPara

Return: ST_SUCCESS  
        ST_ERROR
        TRANS_COMM_ERROR
        ST_SEND_DATA_ERR
        ST_UNPACK_DATA_ERR
    
 ************************************************************************/
int inBuildAndSendIsoData(void) {
    int inResult, i;
    char szErrorMessage[30 + 1];
    char szBcd[INVOICE_BCD_SIZE + 1];
    TRANS_DATA_TABLE *srTransPara;
    TRANS_DATA_TABLE srTransParaTmp;
    TRANS_DATA_TABLE *srTransNotUsed;
    //commsbackup
    BOOL fConnectFailed = FALSE;
    //commsbackup

	#ifdef ERM_ERROR_RECEIPT_FEATURE // upload to ERM error receipt.
    int inRet = d_NO;
	#endif 

	int inRetRCPT = 0;	
	inRetRCPT = get_env_int("#ERRRCPT");


    /*get szTraceNo with highest value in HDT*/
	inHDTReadMaxTraceNo();

    vdDebug_LogPrintf("inBuildAndSendIsoData szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);
    vdDebug_LogPrintf("**inBuildAndSendIsoData inHostIndex, fTPDUOnOff, inCurrencyIndex -> [%d][%d][%d]*", strHDT.inHostIndex, strHDT.fTPDUOnOff, strCST.inCurrencyIndex);

	/*set trans Date&Time Before any online*/
	vdPackISO_SetDateTimeForTrans(srTransNotUsed);

    memset(szBcd, 0x00, sizeof (szBcd));
    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);


	vdDebug_LogPrintf("inBuildAndSendIsoData:A:szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

    inHDTUpdateTraceNum(); //@@IBR 20170214 this will be update trace number for all host to avoid trace number conflict when send to MPU host
    
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

	vdDebug_LogPrintf("inBuildAndSendIsoData:B:szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

//    if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType))
    if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType) && (VOID_PREAUTH != srTransRec.byTransType))
        srTransRec.ulOrgTraceNum = srTransRec.ulTraceNum;
    inHDTSave(strHDT.inHostIndex);

    srTransPara = &srTransRec;

    vdDebug_LogPrintf("**inBuildAndSendIsoData V02**ulTraceNum=[%d]",srTransRec.ulTraceNum);
    vdDebug_LogPrintf("ZZZZZZZZ - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	
	
    inCTLOS_Updatepowrfail(PFR_BEGIN_SEND_ISO);

    strHDT.fSignOn = CN_TRUE; //notsupport SignON

    if (strHDT.fSignOn == CN_FALSE) {
        if ((srTransRec.byTransType == SALE) || // SALE
                (srTransRec.byTransType == REFUND) || // REFUND
                (srTransRec.byTransType == PRE_AUTH) || // PREAUTH
                (srTransRec.byTransType == SETTLE) || // SETTLE
                (srTransRec.byTransType == CASH_ADVANCE) ||
                (srTransRec.byTransType == CUP_SALE) ||
                (srTransRec.byTransType == CUP_PRE_AUTH) ||
                (srTransRec.byTransType == CUP_PREAUTH_COMP)
                ) {
            i = srTransPara->HDTid;
            inHDTRead(srTransPara->HDTid);
            memcpy(&srTransParaTmp, srTransPara, sizeof (TRANS_DATA_TABLE));
            memset(&srTransRec, 0x00, sizeof (TRANS_DATA_TABLE));
            CTOS_LCDTClearDisplay();

            DebugAddSTR("Sign on false", "---", 12);
            //SignOnTrans(i);            
            if (strHDT.fSignOn == CN_FALSE) {
                srTransRec.shTransResult = TRANS_TERMINATE;
                vdSetErrorMessage("TRANS TERMINATE");
                inCTOS_inDisconnect();
                return ST_ERROR;
            }
            memset(&srTransRec, 0x00, sizeof (TRANS_DATA_TABLE));
            CTOS_LCDTClearDisplay();
            memcpy(srTransPara, &srTransParaTmp, sizeof (TRANS_DATA_TABLE));
            inHDTRead(srTransPara->HDTid);
            srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
        }
    }
    //CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);

    if ((strTCT.byTerminalType % 2) == 0)
        setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "Processing...");
    else
        setLCDPrint(8, DISPLAY_POSITION_LEFT, "Processing...");
    vdDecideWhetherConnection(srTransPara);

    if (CN_FALSE == srTransPara->byOffline) {
        if (VS_TRUE == strTCT.fDemo) {
            vdDebug_LogPrintf("DEMO Call Connect!!");
        } else {
            vdDebug_LogPrintf("Call Connect!!sharecom[%d]", strTCT.fShareComEnable);
            memset(szErrorMessage, 0x00, sizeof (szErrorMessage));
            sprintf(szErrorMessage, "%d", strCPT.inCommunicationMode);
            inCTOSS_PutEnv("PRICOMMMODE", szErrorMessage);
            inCTOSS_PutEnv("COMMBACK", "0");
            memset(szErrorMessage, 0x00, sizeof (szErrorMessage));
            sprintf(szErrorMessage, "%d", strCPT.inSecCommunicationMode);
            inCTOSS_PutEnv("SECCOMMMODE", szErrorMessage);
            //			usPreconnectStatus = 2;
            if(srTransRec.byECRPreauthComp == TRUE)
            {
				//do not reconnect
            }
			else
            if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS) {
                fConnectFailed = TRUE;

				vdDebug_LogPrintf("inBuildAndSendIsoData ABCD");

                inCTOS_inDisconnect();

				
				vdDebug_LogPrintf("inBuildAndSendIsoData WXYZ");

                if (strCPT.inSecCommunicationMode != NULL_MODE) //Comms fallback
                {
                    fConnectFailed = FALSE;
                    if (inCTOS_CommsFallback(strHDT.inHostIndex) != d_OK) //Comms fallback 
                    {
                        vdSetErrorMessage("TRANS COMM ERROR");
                        return ST_ERROR;
                    }

                    inCTOSS_PutEnv("COMMBACK", "1");
                    if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS) {
                        fConnectFailed = TRUE;
                        inCTOS_inDisconnect();
                    }
                }

                if (fConnectFailed == TRUE) {
                    if (srTransPara->shTransResult == 0)
                        srTransPara->shTransResult = TRANS_COMM_ERROR;

                    vdSetErrorMessage("TRANS COMM ERROR");
                    return ST_ERROR;
                }
            }
        }

        if (inProcessReversal(srTransPara) != ST_SUCCESS) {
			
			vdDebug_LogPrintf("inBuildAndSendIsoData->Reversal Error");
            inCTOS_inDisconnect();
            vdSetErrorMessage("Reversal Error");
            return ST_ERROR;
        }
		
		if (srTransRec.byTransType == AUTO_REVERSAL) //auto reversal done
		{
            inCTOS_inDisconnect();
            inMyFile_ReversalDelete();
			return d_OK;
		}

        vdDebug_LogPrintf("inBuildAndSendIsoData byTransType(%d) !!", srTransPara->byTransType);

		
        if (srTransPara->byTransType == SETTLE || srTransPara->byTransType == MPU_SETTLE) 
		{
//			if (srTransPara->byTransType != MPU_SETTLE)
//			{
            #if 1
            if(srTransPara->byTransType == SETTLE && (srTransPara->HDTid != 17 && srTransPara->HDTid != 18 && srTransPara->HDTid != 19))
            {
            if ((inResult = inProcessEMVTCUpload(srTransPara, -1)) != ST_SUCCESS) {
                vdDebug_LogPrintf(". inProcessAdviceTrans(%d) ADV_ERROR!!", inResult);
                inCTOS_inDisconnect();
                vdSetErrorMessage("TC Upload Error");
                return ST_ERROR;
            }
			}

            if ((inResult = inProcessAdviceTrans(srTransPara, -1)) != ST_SUCCESS) {
                vdDebug_LogPrintf(". inProcessAdviceTrans(%d) ADV_ERROR!!", inResult);
                inCTOS_inDisconnect();
                vdSetErrorMessage("Advice Error");
                return ST_ERROR;
            }

			#endif
        }

    } 
	else 
	{
        CTOS_RTC SetRTC;
        BYTE szCurrentTime[20];

		vdDebug_LogPrintf("-----go offline-----");

        CTOS_RTCGet(&SetRTC);
        sprintf(szCurrentTime, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime, srTransPara->szDate, DATE_ASC_SIZE);
        sprintf(szCurrentTime, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
        wub_str_2_hex(szCurrentTime, srTransPara->szTime, TIME_ASC_SIZE);
    }

    if (srTransPara->byTransType == SETTLE || srTransPara->byTransType == MPU_SETTLE) 
	{
        strMMT[0].fMustSettFlag = CN_TRUE;
        inMMTSave(strMMT[0].MMTid);
        inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
        inHDTRead(srTransPara->HDTid);
        srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
    } else {
        /*for TLE field 57*/
        byField_02_ON = 0;
        byField_14_ON = 0;
        byField_35_ON = 0;
        byField_45_ON = 0;
        byField_48_ON = 0;
    }
    
    vdDebug_LogPrintf("inBuildAndSendIsoData transtype = %d, HDTid = %d, strTCT.byERMMode = %d, strTCT.byTerminalType = %d", srTransPara->byTransType, srTransPara->HDTid, strTCT.byERMMode, strTCT.byTerminalType);

    srTransPara->byContinueTrans = CN_FALSE;
    do {
        if (CN_FALSE == srTransPara->byOffline) {
                vdCTOS_SetDateTrans(); //@@IBR ADD 20170203 to get date and time and save to table batch struct
            if ((inResult = inBuildOnlineMsg(srTransPara)) != ST_SUCCESS) {
                vdDebug_LogPrintf("srTransPara->fSendAlipayAutoReversal %d", srTransPara->fSendAlipayAutoReversal);
				
				if(srTransPara->fSendAlipayAutoReversal == TRUE)
                    vdSetErrorMessage("");
				else
                    inProcessReversalEx(srTransPara);


                vdDebug_LogPrintf("inBuildAndSendIsoData after inProcessReversalEx");

				#if 0
				#ifdef ERM_ERROR_RECEIPT_FEATURE // upload to ERM error receipt.
					if (d_OK == isCheckTerminalMP200() && (strTCT.byERMMode == 4 || strTCT.byERMMode == 2))
					{ // fix for not printing error receipt
						inRet = inCTOSS_ERM_ReceiptRecvVia();
					
						if(d_OK != inRet)
						   return inRet;
					}
				#endif
				#endif

                vdDebug_LogPrintf("inBuildAndSendIsoData before 1st ErrorReceipt inRet [%d]", inRetRCPT);
				// for non-mpos terminal
				if(inRetRCPT == 1)
				{
                	ushCTOS_MPUPrintErrorReceipt(); //@@IBR ADD 20161205                
                	ushCTOS_CUPPrintErrorReceipt();
				}

				#ifdef ERM_ERROR_RECEIPT_FEATURE // upload to ERM error receipt.
				if (d_OK == isCheckTerminalMP200() && (strTCT.byERMMode == 4 || strTCT.byERMMode == 2))	
				{
					//new enhancement for MP200 - 05232022
					//Change Request: #15 Change Request Add FLAG to Disable/Enable uploading Error e-receipts to iERS portal
					if(inRetRCPT == 1)
					{
						//1) When error receipt flag is 1, terminal should not prompt ph no./email for any amount if trnx is not approved. (for all entry modes) - Reported 06202022
						#if 0
						inRetRCPT = inCTOSS_ERM_ReceiptRecvVia();				
						if(d_OK != inRetRCPT)
						return inRetRCPT;	
						#endif
								
						inCTOSS_ERM_Form_Error_Receipt(0);	
					}
				}
				#endif				

				vdDebug_LogPrintf("inBuildOnlineMsg inResult %d", inResult);

													// for SQA issue #10047 - Error receipt printing twice when no response received from host.
																					//http://118.201.48.210:8080/redmine/issues/1525.54
				if (ST_RESP_MATCH_ERR == inResult || ST_SEND_DATA_ERR == inResult || ST_UNPACK_DATA_ERR == inResult) {
                    return inResult;
                }
				
                //if ((srTransPara->byEntryMode == CARD_ENTRY_ICC) &&
                //    (srTransPara->byTransType == SALE || srTransPara->byTransType == PRE_AUTH))
                if (((srTransPara->byEntryMode == CARD_ENTRY_ICC) ||
                        (srTransPara->bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
                        (srTransPara->bWaveSID == d_VW_SID_AE_EMV) ||
                        (srTransPara->bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
                        (srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
                        (srTransPara->bWaveSID == d_EMVCL_SID_JCB_EMV) ||
                        (srTransPara->bWaveSID == d_EMVCL_SID_JCB_MSD) ||
                        (srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
                        (srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY)) &&
                        (srTransPara->byTransType == SALE || srTransPara->byTransType == PRE_AUTH))
                    /* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
                {
                    if ((inResult == ST_SEND_DATA_ERR) ||
                            (inResult == ST_UNPACK_DATA_ERR)) {
                        srTransPara->shTransResult = TRANS_COMM_ERROR;
                    }
                    vdDebug_LogPrintf("CARD_ENTRY_ICC Flow");
                }
                else {
                    vdDebug_LogPrintf("inBuildOnlineMsg %d", inResult);
                    return inResult;
                }
            }
        } else {
            /* If the transaction is completed, 
               offline transaction do not need to analyze any information*/
            if (inProcessOfflineTrans(srTransPara) != ST_SUCCESS) {
                vdDebug_LogPrintf("inProcessOfflineTrans Err");
                inCTOS_inDisconnect();
                return ST_ERROR;
            } else
                break;
        }


        if (inAnalyseIsoData(srTransPara) != ST_SUCCESS) {
            vdDebug_LogPrintf("inAnalyseIsoData Err byTransType[%d]shTransResult[%d]srTransPara->szRespCode[%s]srTransPara->stEMVinfo.T9F27[%02X]", 
				srTransPara->byTransType, srTransRec.shTransResult, srTransPara->szRespCode, srTransRec.stEMVinfo.T9F27);

            if(((srTransPara->byTransType == SETTLE && (srTransPara->HDTid != 17 && srTransPara->HDTid != 18 && srTransPara->HDTid != 19)) && (!memcmp(srTransPara->szRespCode,"95",2))) || 
                ((srTransPara->byTransType == SETTLE) && (fGetCashAdvAppFlag() == TRUE) && (!memcmp(srTransPara->szRespCode, "95", 2))))
			{

				vdDebug_LogPrintf("inBuildAndSendIsoData PROCESS BATCH UPLOAD!");
			
                if (inPorcessTransUpLoad(srTransPara) != ST_SUCCESS) 
				{
				
				vdDebug_LogPrintf("inBuildAndSendIsoData BATCH UPLOAD ERROR!");
                    srTransRec.shTransResult = TRANS_COMM_ERROR;
                    inCTOS_inDisconnect();
                    vdSetErrorMessage("BATCH UPLOAD ERROR");
                    return ST_ERROR;
				}
            }
            else if(((srTransPara->byTransType == SETTLE) && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19)) && (!memcmp(srTransPara->szRespCode,"95",2)))
			{

			
			vdDebug_LogPrintf("inBuildAndSendIsoData MPU host PROCESS BATCH UPLOAD!");
				#if 0
				srTransRec.shTransResult = TRANS_COMM_ERROR;
				vdMPU_SetBatchUploadFlag(srTransPara, 1);// set batch upload flag on
				inCTOS_inDisconnect();
				vdSetErrorMessage("PLS DO BATCH UPLOAD!");
				return ST_ERROR;
				#endif
				#if 1
				vdSetMPUTrans(TRUE);
				vdMPU_SetBatchUploadFlag(srTransPara, 1);// still set batch upload flag on

				
				vdDebug_LogPrintf("inBuildAndSendIsoData MPU host PROCESS BATCH UPLOAD - 2!");

				/*now CB Bank want send batch upload automatically, so i just use back old batch upload flow */
				if (inMPU_PorcessTransUpLoad(srTransPara) != ST_SUCCESS) 
				{
				
				vdDebug_LogPrintf("inBuildAndSendIsoData MPU host BATCH UPLOAD ERROR!");
                    srTransRec.shTransResult = TRANS_COMM_ERROR;
                    inCTOS_inDisconnect();
					vdSetMPUTrans(FALSE);
                    vdSetErrorMessage("BATCH UPLOAD ERROR");
                    return ST_ERROR;
				}
				vdSetMPUTrans(FALSE);
				#endif
			}
			else {
                if ((strHDT.fReversalEnable == CN_TRUE) && (srTransPara->byTransType != SETTLE) && (srTransRec.shTransResult == TRANS_REJECTED || srTransRec.shTransResult == TRANS_CALL_BANK)) {
                    inMyFile_ReversalDelete();
                } else
                    inProcessReversalEx(srTransPara);

                inCTOS_inDisconnect();

				
				vdDebug_LogPrintf("inBuildAndSendIsoData XXXX [%s][%d]", szErrorMessage, srTransRec.shTransResult);

                if ((inGetErrorMessage(szErrorMessage) > 0) || (srTransRec.shTransResult == TRANS_REJECTED)) {

					#if 0
					#ifdef ERM_ERROR_RECEIPT_FEATURE // upload to ERM error receipt.
							if (d_OK == isCheckTerminalMP200() && (strTCT.byERMMode == 4 || strTCT.byERMMode == 2))
							{//{} fix for not printing error receipt
								inRet = inCTOSS_ERM_ReceiptRecvVia();
								
								if(d_OK != inRet)
								   return inRet;
							}
					#endif
					#endif
					
                    vdDebug_LogPrintf("inBuildAndSendIsoData print 2 ERROR receipt inRet[%d]!!!", inRetRCPT);
					
					
					if(inRetRCPT == 1)
                    	ushCTOS_MPUPrintErrorReceipt(); //@@IBR ADD 20161205
                    
                    vdDebug_LogPrintf("2nd AC failed or Host reject");

					#ifdef ERM_ERROR_RECEIPT_FEATURE // upload to ERM error receipt.
					if (d_OK == isCheckTerminalMP200() && (strTCT.byERMMode == 4 || strTCT.byERMMode == 2))
					{
						if(inRetRCPT == 1)
						{
							//1) When error receipt flag is 1, terminal should not prompt ph no./email for any amount if trnx is not approved. (for all entry modes) - Reported 06202022
							#if 0
							inRetRCPT = inCTOSS_ERM_ReceiptRecvVia();							
							if(d_OK != inRetRCPT)
							   return inRetRCPT;
							#endif

							inCTOSS_ERM_Form_Error_Receipt(0);	
						}
					}
					#endif			
					
                }
                //else
                //    vdSetErrorMessage("Analyse Iso Error");

                return ST_ERROR;
            }


        } else {
            vdDebug_LogPrintf("srTransPara->byPackType: %d", srTransPara->byPackType);
			if(srTransPara->byPackType != VOID)
                vdCTOS_SyncHostDateTime();
            if ((strHDT.fReversalEnable == CN_TRUE) && (srTransPara->byTransType != SETTLE)) {
                inMyFile_ReversalDelete();
            }
        }

    } while (srTransPara->byContinueTrans);
    

    DebugAddSTR("inBuildAndSendIsoData", "end", 20);

    return ST_SUCCESS;

}

/************************************************************************
Function Name: inSnedReversalToHost()
Description:
    Send Reversal Data To Host
Parameters:
    [IN] srTransPara
         inTransCode

Return: ST_SUCCESS  
        ST_ERROR
        TRANS_COMM_ERROR
        ST_SEND_DATA_ERR
        ST_UNPACK_DATA_ERR
 ************************************************************************/
int inSnedReversalToHost(TRANS_DATA_TABLE *srTransPara, int inTransCode) {
    int inResult;
    int inSendLen, inReceLen;
    BYTE uszSendData[ISO_SEND_SIZE + 1], uszReceData[ISO_REC_SIZE + 1];
    CHAR szFileName[d_BUFF_SIZE];
    //if host number is more than 9, reversals would not be sent
    memset(szFileName, 0, sizeof (szFileName));
    //sprintf(szFileName, "%s%02x%02x.rev"
    sprintf(szFileName, "%s%02d%02d.rev"
            , strHDT.szHostLabel
            , strHDT.inHostIndex
            , srTransRec.MITid);

    DebugAddSTR("inSnedReversalToHost", szFileName, 12);

    vdDebug_LogPrintf("inMyFile_ReversalRead(%s)", szFileName);

    vdDebug_LogPrintf("Rever Name %s", szFileName);

    if ((inResult = inMyFile_CheckFileExist(szFileName)) < 0) {
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        return ST_SUCCESS;
    }

    inSendLen = inResult;

    vdDebug_LogPrintf("inMyFile_ReversalRead(%d)", inResult);
    if ((inResult = inMyFile_ReversalRead(&uszSendData[0], sizeof (uszSendData))) == ST_SUCCESS) {
        DebugAddHEX("Reversal orig", uszSendData, inSendLen);
        inCTOSS_ISOEngCheckEncrypt(srTransPara->HDTid, uszSendData, &inSendLen);
        DebugAddHEX("Reversal Encrypt", uszSendData, inSendLen);

        if (strTCT.fPrintISOMessage == VS_TRUE) {
            inPrintISOPacket(VS_TRUE, uszSendData, inSendLen);
        }

        if ((inReceLen = inSendAndReceiveFormComm(srTransPara,
                (unsigned char *) uszSendData,
                inSendLen,
                (unsigned char *) uszReceData)) <= ST_SUCCESS) {
            vdDebug_LogPrintf("inSnedReversalToHost Send Err");
            srTransRec.shTransResult = TRANS_COMM_ERROR;
            return ST_SEND_DATA_ERR;
        }

        vdSetISOEngTransDataAddress(srTransPara);
        inResult = inCTOSS_UnPackIsodataEx1(srTransPara->HDTid,
                (unsigned char *) uszSendData,
                inSendLen,
                (unsigned char *) uszReceData,
                &inReceLen);

        if (strTCT.fPrintISOMessage == VS_TRUE) {
            inPrintISOPacket(VS_FALSE, uszReceData, inReceLen);
        }

        if (inResult != ST_SUCCESS) {
            vdDebug_LogPrintf("inSnedReversalToHost inCTOSS_UnPackIsodata Err");
            return ST_UNPACK_DATA_ERR;
        } 
		else 
        {
            vdDebug_LogPrintf("inSnedReversalToHost here!!!");
			vdDebug_LogPrintf("Resp DE39:%s fGetMPUTrans :%d HDTid: %d",srTransPara->szRespCode, fGetMPUTrans(), srTransPara->HDTid);   
			//(srTransPara->HDTid != 17 && srTransPara->HDTid != 18 && srTransPara->HDTid != 19)
			
            if (memcmp(srTransPara->szRespCode, "00", 2)) 
			{
				vdDebug_LogPrintf("inSnedReversalToHost here2!!!");
			
				/*MPU Can not find Record for reversal, we can delete*/
				if (VS_TRUE == fGetMPUTrans() && 0 == memcmp(srTransPara->szRespCode, "25", 2))
				{
					vdDebug_LogPrintf("MPU Can not find Record for reversal, we can delete");
					inResult = CTOS_FileDelete(szFileName);
					if (inResult != d_OK)
					{
						vdDebug_LogPrintf(". inSnedReversalToHost %04x", inResult);
						inCTOS_inDisconnect();
						return ST_ERROR;
					}
					else
					{
						vdDebug_LogPrintf("rev. file deelted succesfully after send rev to host");
						vdDebug_LogPrintf("**inSnedReversalToHost END**");
    					return ST_SUCCESS;
					}
				}// for MPU_NPS_TESTCASE_POS_DIRECT test case POSIC014 - CB_MPU_NH_MIGRATION
				else if((memcmp(srTransPara->szRespCode, "68", 2) == 0) && 
					(srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19))
				{
					vdDebug_LogPrintf("MPU Can not find Record for reversal, we can delete");					
					vdDebug_LogPrintf("srTransPara->szRespCode is 68 (timeout!!!)");
					inResult = CTOS_FileDelete(szFileName);
					if (inResult != d_OK)
					{
						vdDebug_LogPrintf(". inSnedReversalToHost %04x", inResult);
						inCTOS_inDisconnect();
						return ST_ERROR;
					}
					else
					{
						vdDebug_LogPrintf("rev. file deelted succesfully after send rev to host");
						vdDebug_LogPrintf("**inSnedReversalToHost END**");
    					return ST_SUCCESS;
					}
				}
                vdDebug_LogPrintf(". inSnedReversalToHost Resp Err %02x%02x", srTransPara->szRespCode[0], srTransPara->szRespCode[1]);
                inCTOS_inDisconnect();

                return ST_ERROR;
            } 
			else 
			{
				vdDebug_LogPrintf("inSnedReversalToHost here3!!!");
			
                inResult = CTOS_FileDelete(szFileName);
                if (inResult != d_OK) {
                    vdDebug_LogPrintf(". inSnedReversalToHost %04x", inResult);
                    inCTOS_inDisconnect();
                    return ST_ERROR;
                }
                else
                    vdDebug_LogPrintf("rev. file deelted succesfully after send rev to host");

            }
        }
    }

    vdDebug_LogPrintf("**inSnedReversalToHost END**");

    return ST_SUCCESS;
}

/************************************************************************
Function Name: inSaveReversalFile()
Description:
    Save Reversal Data into file
Parameters:
    [IN] srTransPara
         inTransCode

Return: ST_SUCCESS  
        ST_ERROR
        ST_BUILD_DATD_ERR
 ************************************************************************/
int inSaveReversalFile(TRANS_DATA_TABLE *srTransPara, int inTransCode) {
    int inResult = ST_SUCCESS;
    int inSendLen, inReceLen;
    unsigned char uszSendData[ISO_SEND_SIZE + 1], uszReceData[ISO_REC_SIZE + 1];
	char temp[768+1];
	int inLen=0;

    vdDebug_LogPrintf("**inSaveReversalFile START TxnType[%d]Orig[%d]HDTid[%d]**", srTransPara->byTransType, srTransPara->byOrgTransType, srTransPara->HDTid);

    if (REFUND == srTransPara->byTransType)
        srTransPara->byPackType = REFUND_REVERSAL;
    else if (VOID == srTransPara->byTransType && REFUND == srTransPara->byOrgTransType)
        srTransPara->byPackType = VOIDREFUND_REVERSAL;
    else if (VOID == srTransPara->byTransType)
    {
        if(srTransPara->byOrgTransType == ALIPAY_SALE)
            srTransPara->byPackType = ALIPAY_VOID_REVERSAL;
        else		
            srTransPara->byPackType = VOID_REVERSAL;
    }
    else if(VOID_PREAUTH == srTransPara->byTransType)
        srTransPara->byPackType = VOID_PREAUTH_REV;
    else if (PRE_AUTH == srTransPara->byTransType)
        srTransPara->byPackType = PREAUTH_REVERSAL;
    else if (CASH_ADVANCE == srTransPara->byTransType) //@@IBR ADD 04102016
        srTransPara->byPackType = REVERSAL_CASH_ADV;
	else if (MPU_PREAUTH == srTransPara->byTransType)
        srTransPara->byPackType = MPU_PREAUTH_REV;				// fix for pre_auth comp reversal for new host app
	else if ((MPU_PREAUTH_COMP == srTransPara->byTransType) || (srTransPara->byTransType == PREAUTH_COMP))
	{
		if(0 == fGetMPUTrans() && (srTransPara->HDTid == 7) || (srTransPara->HDTid == 21))		// fix for preauth reversal error rasied by Khaing 042920	
	        srTransPara->byPackType = REVERSAL;
		else		
        	srTransPara->byPackType = MPU_PAC_REV;
	}
	else if (MPU_PREAUTH_COMP_ADV == srTransPara->byTransType)
        srTransPara->byPackType = MPU_PAC_ADV_REV;
	else if (MPU_VOID_PREAUTH == srTransPara->byTransType)
        srTransPara->byPackType = MPU_VOID_PREAUTH_REV;
	else if (MPU_VOID_PREAUTH_COMP == srTransPara->byTransType)
        srTransPara->byPackType = MPU_VOID_PAC_REV;
	else if (ALIPAY_SALE == srTransPara->byTransType)
        srTransPara->byPackType = ALIPAY_REVERSAL;
	else if(srTransPara->byTransType == TOPUP || srTransPara->byTransType == RELOAD)
        srTransPara->byPackType = TOPUP_RELOAD_REVERSAL;	
    else
        srTransPara->byPackType = REVERSAL;

	/*with new requirment, will combine MPU host and credit host flow togther.*/
	if (1 == fGetMPUTrans())
	{
		vdDebug_LogPrintf("MPU Reversal Packing Process");
		
		if (VOID == srTransPara->byTransType && PRE_AUTH == srTransPara->byOrgTransType)
        	srTransPara->byPackType = MPU_VOID_PREAUTH_REV;
	}

    vdDebug_LogPrintf("inSaveReversalFile->srTransPara->byPackType[%d]: srTransPara->HDTid[%d]: strHDT.szTPDU[%s]", 
		srTransPara->byPackType, srTransPara->HDTid, strHDT.szTPDU);
	
    vdClearISO_DataElement();
	
    vdSetISOEngTransDataAddress(srTransPara);
    if ((inSendLen = inCTOSS_PackIsoDataNoEncryp(srTransPara->HDTid, strHDT.szTPDU, uszSendData, srTransPara->byPackType)) <= ST_SUCCESS) {
        inCTOS_inDisconnect();
        vdDebug_LogPrintf(" inSave_inSendLen %d", inSendLen);
        vdDebug_LogPrintf("**inSaveReversalFile ST_BUILD_DATD_ERR**");
        srTransPara->byPackType = inTransCode;
        inResult = ST_BUILD_DATD_ERR;
        return inResult;
    }

    vdDebug_LogPrintf(". inSaveReversalFile Send(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)(%02x)", uszSendData[0], uszSendData[1], uszSendData[2], uszSendData[3], uszSendData[4], uszSendData[5], uszSendData[6], uszSendData[7], uszSendData[8], uszSendData[9]);

    vdDebug_LogPrintf(". inSaveReversalFile Send Len(%d)", inSendLen);
    
    DebugAddHEX("inSaveReversalFile SendData",uszSendData, inSendLen);

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(temp, 0, sizeof(temp));
        inLen=inBuildStringISO(uszSendData, inSendLen, &temp, srTransPara->byPackType);
      
		memset(uszSendData, 0, sizeof(uszSendData));
		memcpy(uszSendData, temp, inLen);
		inSendLen=inLen;
    }
    #endif

    if ((inResult = inMyFile_ReversalSave(&uszSendData[0], inSendLen)) != ST_SUCCESS) {
        vdDebug_LogPrintf(". inSave_inMyFile_ReversalSave(%04x)", inResult);
        inCTOS_inDisconnect();
        inResult = ST_ERROR;
    }

    srTransPara->byPackType = inTransCode;

    return inResult;
}

/************************************************************************
Function Name: inProcessReversal()
Description:
    Processing Reversal the transaction flow
Parameters:
    [IN] srTransPara

Return: ST_SUCCESS  
        ST_ERROR
 ************************************************************************/

int inProcessReversal(TRANS_DATA_TABLE *srTransPara) {
    int inResult, inTransCode;
    BOOL fIsReversal;

    /*for TLE field 57*/
    byField_02_ON = 0;
    byField_14_ON = 0;
    byField_35_ON = 0;
    byField_45_ON = 0;
    byField_48_ON = 0;

    vdDebug_LogPrintf("**inProcessReversal START*byPackType*[%d]", srTransPara->byPackType);
    if (VS_TRUE == strTCT.fDemo) {
        vdDebug_LogPrintf("DEMO Call ProcessReversal!!");
        inResult = ST_SUCCESS;
        return inResult;
    }

    inResult = ST_SUCCESS;
    inTransCode = srTransPara->byTransType;

    if (inTransCode == CASH_ADVANCE) //@@IBR ADD 05102016
        srTransPara->byPackType = REVERSAL_CASH_ADV;//#1 srTransPara->byPackType == REVERSAL_CASH_ADV; fix for reversal error on cash advance 20201112
    else
        srTransPara->byPackType = REVERSAL;

	if(srTransPara->fAlipay == TRUE)
	{
		if(inTransCode == ALIPAY_SALE)
			srTransPara->byPackType = ALIPAY_REVERSAL;
		else
			srTransPara->byPackType = ALIPAY_VOID_REVERSAL;
	}
	
    vdDebug_LogPrintf(". transtype %d", srTransPara->byTransType);
    vdDebug_LogPrintf(". pack type %d", srTransPara->byPackType);
    vdDebug_LogPrintf(". inTransCode %d", inTransCode);

    /* Check reversal flag, If flag is true then open reversal file and send data to host */
    if ((inResult = inSnedReversalToHost(srTransPara, inTransCode)) != ST_SUCCESS) {
        vdDebug_LogPrintf(". Process_inSnedReversalToHost %d", inResult);
        //if(inExceedMaxTimes_ReversalDelete() != ST_SUCCESS)
        //{
        //	vdDisplayErrorMsg(1, 8, "Delete REV Fail...");
        //}
    }
	vdDebug_LogPrintf("HERE Process_inSnedReversalToHost %d", inResult);

	
#if 1
    if (memcmp(srTransRec.szKSN, "\x00\x00\x00\x00\x00", 5) != 0)
        inCTOSS_ResetDUKPTKSN(srTransRec.szKSN);

    /* send/recv reversal will update STAN in function inUnPackIsoFunc11
    after reversal send out, set trace number back to current transaction*/
    srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

    if (inResult == ST_SUCCESS) {
        inMyFile_ReversalDelete();
        /* Check reversal flag, If flag is true then create reversal file */
        //        if ((strHDT.fReversalEnable == CN_TRUE) && (srTransRec.byTransType != SETTLE) && (srTransRec.byTransType != SIGN_ON))
        if ((strHDT.fReversalEnable == CN_TRUE) &&
                (srTransRec.byTransType != SETTLE) &&
                (srTransRec.byTransType != SIGN_ON) &&
                (srTransRec.byTransType != IPP_SIGN_ON) &&
                (srTransRec.byTransType != EFTSEC_TWK_RSA) &&
                (srTransRec.byTransType != EFTSEC_TWK) &&
                (srTransRec.byTransType != CUP_LOGON) &&
                (srTransRec.byTransType != MPU_SIGNON) &&
                (srTransRec.byTransType != MPU_SIGNOFF) &&
				(srTransRec.byTransType != MPU_SETTLE) &&
				(srTransRec.byTransType != BALANCE_ENQUIRY) &&
				(srTransRec.byTransType != QR_INQUIRY) &&
				(srTransRec.byTransType != CHANGE_PIN) &&
				#ifdef ECR_PREAUTH_AND_COMP
				(srTransRec.byTransType != TRANS_ENQUIRY) &&
				#endif
				(srTransRec.byTransType != TOPUP_RELOAD_LOGON))
            {
            vdDebug_LogPrintf("inSaveReversalFile START");
//            if (srTransRec.byTransType == VOID &&  strIIT.inIssuerNumber != 1 && strIIT.inIssuerNumber != 2) { //@@IBR ADD 20161206
            if (srTransRec.byTransType == VOID &&  (strIIT.inIssuerNumber == 1 || strIIT.inIssuerNumber == 2)) { //@@IBR ADD 20161206
            
           		 vdDebug_LogPrintf("inSaveReversalFile START-1");
               	 fIsReversal = FALSE;
            } 
			//thandar_added in for UPI (Finexus) host
			else  if (srTransRec.byTransType == VOID && strHDT.inHostIndex==7) 
		    { 
				vdDebug_LogPrintf("inSaveReversalFile START-2");
		    
                fIsReversal = FALSE;
            }
			else{
				
				vdDebug_LogPrintf("inSaveReversalFile START-2.1");
                fIsReversal = TRUE;
			}
            
            if(fIsReversal == TRUE)
		{
				if (srTransRec.byTransType != AUTO_REVERSAL)	//after auto reversal is finished, don't save a new reversal file, otherwise will auto reversal forever
				{
					vdDebug_LogPrintf("inSaveReversalFile START-3");
				
			                if ((inResult = inSaveReversalFile(srTransPara, inTransCode)) != ST_SUCCESS) {
			                    vdDebug_LogPrintf(". Process_inSaveReversalFile %04x", inResult);
			                }
				}
				else					
				vdDebug_LogPrintf("inSaveReversalFile START-4 - NO AUTO REVERSAL");
            }
        }
    }
#endif
    srTransPara->byTransType = inTransCode;

    vdDebug_LogPrintf("**inProcessReversal TYPE(%d) Rest(%d)END**", srTransPara->byTransType,
            inResult);

    return inResult;
}

int inProcessAdviceTrans(TRANS_DATA_TABLE *srTransPara, int inAdvCnt) {
    int inResult, inUpDateAdviceIndex;
    int inCnt;
    TRANS_DATA_TABLE srAdvTransTable;
    ISO_FUNC_TABLE srAdviceFunc;
    STRUCT_ADVICE strAdvice;

    /* Issue# 000187 - start -- jzg */
    //1127
    //add to do not do any advice for Y1, advice will be sent on the next online transaction
    if ((stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_APPROVAL) && (srTransPara->byTransType != SETTLE))
        return d_OK;
    //1127
    /* Issue# 000187 - end -- jzg */

    memset((char *) &srAdvTransTable, 0, sizeof (TRANS_DATA_TABLE));
    memset((char *) &strAdvice, 0, sizeof (strAdvice));

    memcpy((char *) &srAdvTransTable, (char *) srTransPara, sizeof (TRANS_DATA_TABLE));
    memcpy((char *) &srAdviceFunc, (char *) &srIsoFuncTable[0], sizeof (ISO_FUNC_TABLE));

    inResult = ST_SUCCESS;

    vdDebug_LogPrintf("inProcessAdviceTrans byPackType(%d)byTransType(%d)", srAdvTransTable.byPackType, strAdvice.byTransType);
    while (1) {
        inResult = inMyFile_AdviceRead(&inUpDateAdviceIndex, &strAdvice, &srAdvTransTable);

        vdDebug_LogPrintf("ADVICE inUpDateAdviceIndex[%d] inMyFile_AdviceRead Rult(%d)(%d)(%d)(%d)", inUpDateAdviceIndex, inResult, srAdvTransTable.ulTraceNum, srAdvTransTable.byPackType, strAdvice.byTransType);

        if (inResult == ST_ERROR || inResult == RC_FILE_READ_OUT_NO_DATA) {
            inResult = ST_SUCCESS;
            break;
        }

        if (inResult == ST_SUCCESS) {
            vdDebug_LogPrintf("srTransPara->HDTid = [%d] CVV2[%s]Tip[%02X%02X%02X%02X%02X%02X]", srTransPara->HDTid, srAdvTransTable.szCVV2, srAdvTransTable.szTipAmount[0], srAdvTransTable.szTipAmount[1], srAdvTransTable.szTipAmount[2], srAdvTransTable.szTipAmount[3], srAdvTransTable.szTipAmount[4], srAdvTransTable.szTipAmount[5]);
            inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
            inHDTRead(srTransPara->HDTid);
            //advice need add traceNum
            srAdvTransTable.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

            inResult = inPackSendAndUnPackData(&srAdvTransTable, strAdvice.byTransType);

            vdDebug_LogPrintf(". inProcessAdviceTrans Rult(%d)srAdvTransTable.byTCFailUpCnt[%d]", inResult, srAdvTransTable.byTCFailUpCnt);
            if (memcmp(srAdvTransTable.szRespCode, "00", 2))
                inResult = ST_ERROR;

            if ((inResult == ST_SUCCESS)) {
                if ((srAdviceFunc.inAdviceAnalyse != 0x00)) {
                    vdSetISOEngTransDataAddress(&srAdvTransTable);
                    inResult = srAdviceFunc.inAdviceAnalyse(CN_FALSE);
                }

                if (inResult == ST_SUCCESS) {
                    vdDebug_LogPrintf(". inAdviceAnalyse Rult(%d)", inResult);

                    srAdvTransTable.byUploaded = CN_TRUE;

                    //Should be Online void the Intial SALE amount.
                    //use szStoreID to store how much amount fill up in DE4 for VOID
                    memcpy(srAdvTransTable.szStoreID, srAdvTransTable.szTotalAmount, 6);

                    if ((inResult = inMyFile_BatchSave(&srAdvTransTable, DF_BATCH_UPDATE)) == ST_SUCCESS) {
                        inResult = inMyFile_AdviceUpdate(inUpDateAdviceIndex);
                        vdDebug_LogPrintf(". inProcessAdviceTrans Update Rult(%d)**", inResult);
                    }

                    if (inResult != ST_SUCCESS) {
                        vdDebug_LogPrintf(". inProcessAdviceTrans Err(%d)**", inResult);
                        break;
                    }
                }
            }
        }

        if (inResult != ST_SUCCESS) {
            if (srTransPara->byTransType == SETTLE) {
                srTransRec.shTransResult = TRANS_COMM_ERROR;
                inCTOS_inDisconnect();
                return ST_ERROR;
            } else
                return ST_SUCCESS;
        }


        if (inAdvCnt != -1) {
            inAdvCnt--;
            if (inAdvCnt == 0)
                break;
        }
    }

    vdDebug_LogPrintf("**inProcessAdviceTrans(%d) END**", inResult);
    return (inResult);
}

int inProcessEMVTCUpload(TRANS_DATA_TABLE *srTransPara, int inAdvCnt) {
    int inResult, inUpDateAdviceIndex;
    int inCnt;
    TRANS_DATA_TABLE srOrigTransFromBatch;
    TRANS_DATA_TABLE srAdvTransTable;
    ISO_FUNC_TABLE srAdviceFunc;
    STRUCT_ADVICE strAdvice;
    BYTE key;
	
    memset((char *) &srAdvTransTable, 0, sizeof (TRANS_DATA_TABLE));
    memset((char *) &strAdvice, 0, sizeof (strAdvice));

    memcpy((char *) &srAdvTransTable, (char *) srTransPara, sizeof (TRANS_DATA_TABLE));
    memcpy((char *) &srAdviceFunc, (char *) &srIsoFuncTable[0], sizeof (ISO_FUNC_TABLE));

    inResult = ST_SUCCESS;

    vdDebug_LogPrintf("inProcessEMVTCUpload byPackType(%d)byTransType(%d)", srAdvTransTable.byPackType, strAdvice.byTransType);

	vdDebug_LogPrintf("inProcessEMVTCUpload szTraceNo = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);
	
    while (1) {
        inResult = inMyFile_TCUploadFileRead(&inUpDateAdviceIndex, &strAdvice, &srAdvTransTable);

        if (strAdvice.byTransType == TC_UPLOAD)
            srAdvTransTable.byPackType = TC_UPLOAD;

        if (inResult == ST_ERROR || inResult == RC_FILE_READ_OUT_NO_DATA) {
            inResult = ST_SUCCESS;
            break;
        }

        memcpy(&srOrigTransFromBatch, &srAdvTransTable, sizeof (TRANS_DATA_TABLE));
        if (inResult == ST_SUCCESS) {

			vdDebug_LogPrintf("inProcessEMVTCUpload HERE!!!");			

			vdDebug_LogPrintf("srAdvTransTable.ulTraceNum A - [%06ld][%d]", srAdvTransTable.ulTraceNum, srAdvTransTable.HDTid);
			vdDebug_LogPrintf("inProcessEMVTCUpload szTraceNo1 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);
			
            //vdDebug_LogPrintf("srTransPara->HDTid = [%d] ", srTransPara->HDTid);

			// fix for NFC Payment - ERROR 094 - Dupl Transmission case #2020
			#if 1
			inHDTReadMaxTraceNo();
			
            inMyFile_HDTTraceNoAdd(srAdvTransTable.HDTid);
			inHDTRead(srAdvTransTable.HDTid);
			
			srAdvTransTable.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);

			inHDTUpdateTraceNum(); // for testing. fix duplicate trace number on sale, tc upload then void. TC upload and void share the same trace number.

			vdDebug_LogPrintf("srAdvTransTable.ulTraceNum B - [%06ld]", srAdvTransTable.ulTraceNum);
			vdDebug_LogPrintf("inProcessEMVTCUpload szTraceNo2 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);
			
            inResult = inPackSendAndUnPackData(&srAdvTransTable, strAdvice.byTransType);
			#else
			inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
			inHDTRead(srTransPara->HDTid);

			inResult = inPackSendAndUnPackData(&srAdvTransTable, strAdvice.byTransType);

			#endif

			vdDebug_LogPrintf("srAdvTransTable.ulTraceNum C - [%06ld]", srAdvTransTable.ulTraceNum);
			vdDebug_LogPrintf("inProcessEMVTCUpload szTraceNo3 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);			
            vdDebug_LogPrintf(". inProcessEMVTCUpload Rult(%d)srAdvTransTable.byTCFailUpCnt[%d]srTransPara->szRespCode[%s]", inResult, srAdvTransTable.byTCFailUpCnt, srAdvTransTable.szRespCode);


            if (srAdvTransTable.byTCFailUpCnt >= 2) {
                srAdvTransTable.byTCuploaded = CN_TRUE;
                srAdvTransTable.byUploaded = CN_TRUE;
                inResult = inMyFile_TCUploadFileUpdate(inUpDateAdviceIndex);
                vdDebug_LogPrintf(". inProcessEMVTCUpload Update Rult(%d)**", inResult);

                if (inResult != ST_SUCCESS) {
                    vdDebug_LogPrintf(". inProcessEMVTCUpload Err(%d)**", inResult);
                    break;
                }
            }
            if (inResult == ST_SUCCESS) {
                if (memcmp(srAdvTransTable.szRespCode, "00", 2) != 0) {
                    vdDebug_LogPrintf(". resp not succ(%s)**srTransPara->byPackType[%d]strAdvice.byTransType[%d]", srAdvTransTable.szRespCode, srTransPara->byPackType, strAdvice.byTransType);
                    if (strAdvice.byTransType == TC_UPLOAD) {
                        srOrigTransFromBatch.byTCFailUpCnt++;
                        inMyFile_BatchSave(&srOrigTransFromBatch, DF_BATCH_UPDATE);
                        inCTOS_inDisconnect();
                        return ST_ERROR;
                    }
                } else {
                    srAdvTransTable.byTCuploaded = CN_TRUE;
                    srAdvTransTable.byUploaded = CN_TRUE;
                    inResult = inMyFile_TCUploadFileUpdate(inUpDateAdviceIndex);
                    vdDebug_LogPrintf(". inProcessEMVTCUpload Update Rult(%d)**", inResult);

                    if (inResult != ST_SUCCESS) {
                        vdDebug_LogPrintf(". inProcessEMVTCUpload Err(%d)**", inResult);
                        break;
                    }
                }
            } else {
                if (strAdvice.byTransType == TC_UPLOAD) {
                    srOrigTransFromBatch.byTCFailUpCnt++;
                    inMyFile_BatchSave(&srOrigTransFromBatch, DF_BATCH_UPDATE);
                }
            }

        }

		if(strTCT.fSendTCBeforeSettle == TRUE)
		{
			if(isCheckTerminalMP200() == d_OK)
			{
		        CTOS_KBDHit(&key);
                if (key == d_KBD_CANCEL)
                {
                    //usRealLen = ST_ERROR;
                    //vdDebug_LogPrintf(". Keb Return");
                    return ST_ERROR;
                }
			    //return d_OK;
			}
		}

        if (inResult != ST_SUCCESS) {
            if (srTransPara->byTransType == SETTLE) {
                srTransRec.shTransResult = TRANS_COMM_ERROR;
                inCTOS_inDisconnect();
                return ST_ERROR;
            } else
                return ST_SUCCESS;
        }


        if (inAdvCnt != -1) {
            inAdvCnt--;
            if (inAdvCnt == 0)
                break;
        }
    }
	vdDebug_LogPrintf("srAdvTransTable.ulTraceNum D - [%06ld]", srAdvTransTable.ulTraceNum);
	vdDebug_LogPrintf("inProcessEMVTCUpload szTraceNo4 = %02x%02x%02x", strHDT.szTraceNo[0],strHDT.szTraceNo[1],strHDT.szTraceNo[2]);

    vdDebug_LogPrintf("**inProcessEMVTCUpload(%d) END**", inResult);
    return (inResult);
}

/************************************************************************
Function Name: inPackSendAndUnPackData()
Description:
    Composed upload host information.
    Analysis of host return message.
Parameters:
    [IN] srTransPara
         inTransCode
Return: ST_SUCCESS  
        ST_ERROR
        ST_BUILD_DATD_ERR
        ST_SEND_DATA_ERR
        ST_UNPACK_DATA_ERR
 ************************************************************************/

int inPackSendAndUnPackData(TRANS_DATA_TABLE *srTransPara, int inTransCode) {
    int inResult;
    int inSendLen, inReceLen;
    unsigned char uszSendData[ISO_SEND_SIZE + 1], uszReceData[ISO_REC_SIZE + 1];

	BYTE baMACBuf[1024];
	BYTE baMAC[8];
	int inMACBufLen = 0;

#ifdef CB_MPU_NH_MIGRATION	
	char uszSendDataNoTPDU[255+1]; // 02282019
	char usTPDU[255+1];
#endif

    vdDebug_LogPrintf("**inPackSendAndUnPackData START* fTPDUOnOff [%d]* inTransCode [%d]", strHDT.fTPDUOnOff, inTransCode);

    memset(uszSendData, 0x00, sizeof (uszSendData));
    memset(uszReceData, 0x00, sizeof (uszReceData));

    inResult = ST_SUCCESS;

    if (VS_TRUE == strTCT.fDemo) {
        vdDebug_LogPrintf("DEMO Call PackIsoDataEx!!");
        inSendLen = 1;
        inResult = d_OK;
    } else {
        vdClearISO_DataElement();
        vdSetISOEngTransDataAddress(srTransPara);
        if ((inSendLen = inCTOSS_PackIsoDataEx(srTransPara->HDTid, strHDT.szTPDU, uszSendData, inTransCode)) <= ST_SUCCESS) {
			inCTOS_inDisconnect();
            vdDebug_LogPrintf(" inSendLen %d", inSendLen);
            vdDebug_LogPrintf("inPackSendAndUnPackData MISSING OR INCORRECT TRANSACTION ID!!!");			
            vdDebug_LogPrintf("**inPackSendAndUnPackData ST_BUILD_DATD_ERR**");
            return ST_BUILD_DATD_ERR;
        }

        inResult = inCTOSS_CheckBitmapSetBit(5);
        vdDebug_LogPrintf("inCTOSS_CheckSetBit 5 [%ld]", inResult);
        inResult = inCTOSS_CheckBitmapSetBit(11);
        vdDebug_LogPrintf("inCTOSS_CheckSetBit 11 [%ld]", inResult);
    }
    vdDebug_LogPrintf(". inPackData Send Len(%d)", inSendLen);
	DebugAddHEX("XXXX send data", uszSendData, inSendLen);

    if (strTCT.fPrintISOMessage == VS_TRUE) {

		#ifdef CB_MPU_NH_MIGRATION
			if(strHDT.fTPDUOnOff == CN_FALSE)		
	        	inPrintISOPacket(VS_TRUE, &uszSendData[5], inSendLen-5);
			else
	        	inPrintISOPacket(VS_TRUE, uszSendData, inSendLen);
		#endif
    }

    if(srTransPara->fAlipay == TRUE && srTransPara->byPackType == ALIPAY_SALE)
    {
        if ((inReceLen = inSendAndReceiveAlipayFormComm(srTransPara,
        (unsigned char *) uszSendData,
        inSendLen,
        (unsigned char *) uszReceData)) <= 0) {
            vdDebug_LogPrintf(". inPackData ST_SEND_DATA_ERR");
            return ST_SEND_DATA_ERR;
        }
		else
			inResult=ST_SUCCESS;
    }
    else
    {
        if ((inReceLen = inSendAndReceiveFormComm(srTransPara,
        (unsigned char *) uszSendData,
        inSendLen,
        (unsigned char *) uszReceData)) <= 0) {
            vdDebug_LogPrintf(". inPackData ST_SEND_DATA_ERR");
            return ST_SEND_DATA_ERR;
        }

        //	if (strTCT.fPrintISOMessage == VS_TRUE){
        //		inPrintISOPacket(VS_FALSE , uszReceData, inReceLen);
        //	}
	        vdDebug_LogPrintf(". inPackData Rec Len(%d) - HDTid(%d)", inReceLen, srTransPara->HDTid);
        if (VS_TRUE == strTCT.fDemo) {
            CTOS_RTC SetRTC;
            BYTE szCurrentTime[20];
    
            CTOS_RTCGet(&SetRTC);
            sprintf(szCurrentTime, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);
            wub_str_2_hex(szCurrentTime, srTransPara->szDate, DATE_ASC_SIZE);
            sprintf(szCurrentTime, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
            wub_str_2_hex(szCurrentTime, srTransPara->szTime, TIME_ASC_SIZE);
    
            vdDebug_LogPrintf("DEMO Call UnPackIsodataEx!!");
            inResult = d_OK;
            strcpy(srTransPara->szRespCode, "00");
        } else {
            vdSetISOEngTransDataAddress(srTransPara);
            inResult = inCTOSS_UnPackIsodataEx1(srTransPara->HDTid,
                    (unsigned char *) uszSendData,
                    inSendLen,
                    (unsigned char *) uszReceData,
                    &inReceLen);
            vdDebug_LogPrintf("**inPackSendAndUnPackData inResult=[%d]srTransPara->szRespCode[%s]", inResult, srTransPara->szRespCode);
        }
    }
	
    if (strTCT.fPrintISOMessage == VS_TRUE) 
	{		
		#ifdef CB_MPU_NH_MIGRATION
		if(strHDT.fTPDUOnOff == CN_FALSE)		
        	inPrintISOPacket(VS_FALSE, &uszReceData[5], inReceLen-5);
		else	
	        inPrintISOPacket(VS_FALSE, uszReceData, inReceLen);
		#endif
    }

	vdDebug_LogPrintf("**inPackSendAndUnPackData XXXXX inResult=[%d]", inResult);

    if (inResult != ST_SUCCESS) {
		vdDebug_LogPrintf("**inPackSendAndUnPackData XXXXX INVALID RESPONSE");
		
        vdSetErrorMessage("INVALID RESPONSE");
        inResult = ST_UNPACK_DATA_ERR;
        inCTOS_inDisconnect();
    }

	/*MPU Check MAC here*/
	if ((MPU_DEBIT_HOST_IDX == srTransPara->HDTid || MPU_JCBI_HOST_IDX == srTransPara->HDTid || MPU_CUP_HOST_IDX == srTransPara->HDTid)
		&& 0 == strcmp(srTransPara->szRespCode, "00"))
	{
		vdDebug_LogPrintf("**inPackSendAndUnPackData YYYYY VALID RESPONSE");
	
		memset(baMACBuf, 0x00, sizeof(baMACBuf));
		inMACBufLen = inMPU_GenMACBuffer(srTransPara, baMACBuf, 1);
		vdDebug_LogPrintf("inMACBufLen[%d]", inMACBufLen);
		DebugAddHEX("baMACBuf", baMACBuf, inMACBufLen);
		memset(baMAC, 0x00, sizeof(baMAC));
		inCalculateMAC(baMACBuf, inMACBufLen, baMAC);
		
		DebugAddHEX("Calc baMAC", baMAC, 8);
		DebugAddHEX("Resp baMAC", &uszReceData[inReceLen-8], 8);
	}

    vdDebug_LogPrintf("**inPackSendAndUnPackData END**");
    return inResult;
}

/************************************************************************
Function Name: inBuildOnlineMsg()
Description:
   To handle online messages and check the response code and authorization code
Parameters:
    [IN] srTransPara

Return: ST_SUCCESS  
        ST_ERROR
        ST_UNPACK_DATA_ERR
        ST_SEND_DATA_ERR
 ************************************************************************/
int inBuildOnlineMsg(TRANS_DATA_TABLE *srTransPara) {
    int inResult;
    TRANS_DATA_TABLE srTransParaTmp;

    vdDebug_LogPrintf("**inBuildOnlineMsg START TxnType[%d]Orig[%d]**", srTransPara->byTransType, srTransPara->byOrgTransType);
    vdDebug_LogPrintf("fGetMPUTrans[%d] srTransPara->HDTid[%d]", fGetMPUTrans(), srTransPara->HDTid);
    vdDebug_LogPrintf("**inBuildOnlineMsg fTPDUOnOff* [%d]*", strHDT.fTPDUOnOff);

    //thandar_added in 20180530 VOid PRE AUTH MTI for Credit and UPI (Finexus)
    if (VOID == srTransPara->byTransType && PRE_AUTH == srTransPara->byOrgTransType && (21 == srTransPara->HDTid || 7 == srTransPara->HDTid || 22 == srTransPara->HDTid || 23 == srTransPara->HDTid))
       {
       		vdDebug_LogPrintf("**inBuildOnlineMsg VOID PREAUTH**");
       		srTransPara->byPackType = VOID_PREAUTH;
    	}		  
    else if (VOID == srTransPara->byTransType && REFUND == srTransPara->byOrgTransType){
        srTransPara->byPackType = VOID_REFUND;
		
		vdDebug_LogPrintf("**inBuildOnlineMsg VOID_REFUND**");
    	}
    else{
        srTransPara->byPackType = srTransPara->byTransType;
    	}

	/*with new requirment, will combine MPU host and credit host flow togther.
	So here need check for more detail*/
	if (1 == fGetMPUTrans())
	{
		vdDebug_LogPrintf("**inBuildOnlineMsg AAAAA");
	
		if (VOID == srTransPara->byTransType && PRE_AUTH == srTransPara->byOrgTransType)
			srTransPara->byPackType = MPU_VOID_PREAUTH;

		if (PRE_AUTH == srTransPara->byTransType && 19 == srTransPara->HDTid)
			srTransPara->byPackType = MPU_PREAUTH;
		
		if (PREAUTH_COMP == srTransPara->byTransType && 19 == srTransPara->HDTid)
			srTransPara->byPackType = MPU_PREAUTH_COMP;
		
		if (VOID == srTransPara->byTransType && PREAUTH_COMP == srTransPara->byOrgTransType)
			srTransPara->byPackType = MPU_VOID_PREAUTH_COMP;
	}
	else
	{

		vdDebug_LogPrintf("**inBuildOnlineMsg BBBBB");
		#if 0
		if (VOID == srTransPara->byTransType && PREAUTH_COMP == srTransPara->byOrgTransType && (21 == srTransPara->HDTid || 7 == srTransPara->HDTid)){
			
			vdDebug_LogPrintf("**inBuildOnlineMsg CCCCC");
			srTransPara->byPackType = VOID_PREAUTH_COMP; //- fix for comm error during preauthcomp void.
		}
		#else //fix for http://118.201.48.210:8080/redmine/issues/2528.26.5 (S1F2 ALSO)
		if (VOID == srTransPara->byTransType && srTransPara->byOrgTransType == PREAUTH_COMP)
		{
		
			if ((srTransPara->HDTid == 21 || srTransPara->HDTid == 7 || srTransPara->HDTid == 22 || srTransPara->HDTid == 23))
			{
				vdDebug_LogPrintf("**inBuildOnlineMsg BBBBBB");
				srTransPara->byPackType = VOID_PREAUTH_COMP; //- fix for comm error during preauthcomp void.
			}
			else
			{			
				vdDebug_LogPrintf("**inBuildOnlineMsg CCCCCC");
				srTransPara->byPackType = MPU_VOID_PREAUTH_COMP;
			}
		}
		#endif
		
	}

	if(srTransPara->fAlipay == TRUE && srTransPara->byTransType == VOID)
		srTransPara->byPackType=ALIPAY_VOID;
	
    memset(&srTransParaTmp, 0x00, sizeof (TRANS_DATA_TABLE));
    memcpy(&srTransParaTmp, srTransPara, sizeof (TRANS_DATA_TABLE));

	
    vdDebug_LogPrintf("inBuildOnlineMsg FINAL byPackType(%d)", srTransPara->byPackType);
    
    DebugAddHEX("szDate", srTransPara->szDate, 2);

    inResult = inPackSendAndUnPackData(srTransPara, srTransPara->byPackType);

    vdDebug_LogPrintf("inBuildOnlineMsg:inResult [%d]", inResult);

    if (inResult == ST_BUILD_DATD_ERR) {
        vdDebug_LogPrintf("**inBuildOnlineMsg TRANS_COMM_ERROR**");
        srTransPara->shTransResult = TRANS_COMM_ERROR;
        //vdSetErrorMessage("SEND DATA ERR");
        vdSetErrorMessage("COMM ERROR"); //aaa issue#00011 Connection Failure response for LAN displays "SEND DATA ERROR" instead of "COMM ERROR" 
        inCTOS_inDisconnect();
        return ST_ERROR;
    } else if (inResult == ST_SEND_DATA_ERR || inResult == ST_UNPACK_DATA_ERR) {
        srTransRec.shTransResult = TRANS_COMM_ERROR;
        vdSetErrorMessage("COMM ERROR");
        vdDebug_LogPrintf("**inBuildOnlineMsg ST_SEND_DATA_ERR**");
        return inResult;
    } else {
        srTransPara->shTransResult = inCheckHostRespCode(srTransPara);
        vdDebug_LogPrintf(". shTransResult %d", srTransPara->shTransResult);

        if (srTransPara->shTransResult == TRANS_AUTHORIZED) 
		{
			if(srTransPara->byPackType == ALIPAY_SALE)
				srTransParaTmp.ulTraceNum=srTransPara->ulTraceNum;
			
            if (ST_SUCCESS != inBaseRespValidation(&srTransParaTmp, srTransPara))
                return ST_RESP_MATCH_ERR;
			
        } 
		else if (srTransPara->shTransResult == ST_UNPACK_DATA_ERR) 
		{
            vdDebug_LogPrintf("**inBuildOnlineMsg shTransResult UNPACK_ERR**");
            srTransPara->shTransResult = TRANS_COMM_ERROR; // for not delete reversal file

            //vdSetErrorMessage("RESP ERROR");
            vdSetErrorMessage("DO NOT ALLOW ATTEMPT");
            return ST_UNPACK_DATA_ERR;
        }
    }

    vdDebug_LogPrintf("**inBuildOnlineMsg END**");
    return ST_SUCCESS;
}

/************************************************************************
Function Name: inSetBitMapCode()
Description:
    Use the transaction code to generate the corresponding bitmap code
Parameters:
    [IN] srTransPara
         srPackFunc
         inTempBitMapCode
Return: ST_SUCCESS  
        inBitMapArrayIndex
 ************************************************************************/

int inSetBitMapCode(TRANS_DATA_TABLE *srTransPara, int inTransCode) {
    int inBitMapIndex = -1;

    inBitMapIndex = inTransCode;

    return inBitMapIndex;
}

/************************************************************************
Function Name: inPackMessageIdData()
Description:
    Pack message id data
Parameters:
    [IN] srTransPara
         inTransCode
         uszPackData
Return: inPackLen
 ************************************************************************/
int inPackMessageIdData(int inTransCode, unsigned char *uszPackData, char *szMTI) {
    int inPackLen;
    TRANS_DATA_TABLE *srTransPara;

    inPackLen = 0;
    srTransPara = srGetISOEngTransDataAddress();

	vdDebug_LogPrintf("=====inPackMessageIdData=====");

    wub_str_2_hex(szMTI, (char *) &uszPackData[inPackLen], MTI_ASC_SIZE);

    if (srTransPara->byPackType != BATCH_UPLOAD && srTransPara->byPackType != TC_UPLOAD) {
        wub_str_2_hex(szMTI, (char *) srTransPara->szMassageType, MTI_ASC_SIZE);
    }

    //@@IBR ADD 20161031
    if (srTransPara->byPackType == SALE) {
        memcpy(&szMacData[inMacDataCnt], szMTI, MTI_ASC_SIZE);
        inMacDataCnt += MTI_ASC_SIZE;
        strcat(szMacData, " ");
        inMacDataCnt += 1;
    }

    inPackLen += MTI_BCD_SIZE;

	/*for MPU Generate MAC buffer*/
	memset(baMPU_ReqMTI, 0x00, sizeof(baMPU_ReqMTI));
	memcpy(baMPU_ReqMTI, srTransPara->szMassageType, MTI_BCD_SIZE);

    return inPackLen;
}

/************************************************************************
Function Name: inPackPCodeData()
Description:
    Pack message id data
Parameters:
    [IN] srTransPara
         inTransCode
         uszPackData
Return: inPackLen
 ************************************************************************/
int inPackPCodeData(int inTransCode, unsigned char *uszPackData, char *szPCode) {
    char szTempFile03[10];
    int inPackLen;
    TRANS_DATA_TABLE *srTransPara;

    inPackLen = 0;
    srTransPara = srGetISOEngTransDataAddress();


    vdDebug_LogPrintf(". inPackPCodeData [%02X] %s", inTransCode, szPCode);

    if (srTransPara->byPackType == BATCH_UPLOAD) {
        memset(szTempFile03, 0x00, sizeof (szTempFile03));
        memcpy(szTempFile03, srTransPara->szIsoField03, PRO_CODE_BCD_SIZE);
    }

    wub_str_2_hex(szPCode, srTransPara->szIsoField03, PRO_CODE_ASC_SIZE);

    if (srTransPara->byPackType == BATCH_UPLOAD) {
        memcpy(srTransPara->szIsoField03, szTempFile03, PRO_CODE_BCD_SIZE);

        if (inFinalSend == CN_TRUE)
            srTransPara->szIsoField03[2] = 0x00;
    }

    inPackLen += PRO_CODE_BCD_SIZE;

    vdDebug_LogPrintf(". szPCode %s [%02X%02X%02X]", szPCode, srTransPara->szIsoField03[0], srTransPara->szIsoField03[1], srTransPara->szIsoField03[2]);


    return inPackLen;
}

/************************************************************************
Function Name: vdModifyBitMapFunc()
Description:
    Modify bitmap array
Parameters:
    [IN] srTransPara
         inTransCode
         inBitMap
Return: 
 ************************************************************************/
void vdModifyBitMapFunc(int inTransCode, int *inBitMap) {
    BYTE szTipAmount[20];
    TRANS_DATA_TABLE *srTransPara;
    BYTE EMVtagVal[64];
    BYTE szPinBlock[64 + 1];
    USHORT EMVtagLen;
	int inResult=0;

	#if 1
	//http://118.201.48.210:8080/redmine/issues/1525.99 and 100 #4
	int inGetNEWPACFlow = 0;	
	inGetNEWPACFlow = get_env_int("#PREAUTHRRNDT");
	#endif



    srTransPara = srGetISOEngTransDataAddress();

    if (inTransCode == SIGN_ON || inTransCode == IPP_SIGN_ON) {
        return;
    }

	
    vdDebug_LogPrintf(". vdModifyBitMapFunc inTransCode[%d]byEntryMode[%d]byTransType[%d]szCVV2[%s]srTransPara->bWaveSID[%d]", inTransCode, srTransPara->byEntryMode, 
		srTransPara->byTransType, srTransPara->szCVV2, srTransPara->bWaveSID);
    vdDebug_LogPrintf("Current srTransRec.fIsInstallment [%d] srTransRec.byTransType [%d] fGetCashAdvAppFlag[%d]", srTransRec.fIsInstallment, srTransRec.byTransType, fGetCashAdvAppFlag());
    vdDebug_LogPrintf("srTransRec.IITid:strHDT.inHostIndex:fGetMPUTrans[%d][%d][%d]", srTransRec.IITid, strHDT.inHostIndex, fGetMPUTrans());
	
    if ((inTransCode != BATCH_UPLOAD) && (inTransCode != REVERSAL) && (inTransCode != SETTLE) && (inTransCode != CLS_BATCH)) {
        if (srTransPara->byEntryMode == CARD_ENTRY_MSR ||
                srTransPara->byEntryMode == CARD_ENTRY_ICC) {
            {
                vdMyEZLib_LogPrintf(". usTrack1Len>0");
                vdCTOSS_SetBitMapOff(inBitMap, 45);
            }
        }

		vdDebug_LogPrintf("vdModifyBitMapFunc HERE *******************");

		#if 0
		// JCB contactless card scheme pos entry code #2		
		if (srTransPara->bWaveSID == d_EMVCL_SID_JCB_MSD) // 	
			sprintf(szTempEnterMode,"091%d",byPINEntryCap);
			//sprintf(szTempEnterMode,"091%d",srTransPara->byPINEntryCapability);
		
		if (srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY)
			sprintf(szTempEnterMode,"007%d",byPINEntryCap);			
			//sprintf(szTempEnterMode,"007%d",srTransPara->byPINEntryCapability);			
		
		#endif

        if ((srTransPara->byEntryMode == CARD_ENTRY_ICC ||
                (srTransPara->byEntryMode == CARD_ENTRY_WAVE &&
                (srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_2 ||
                srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_QVSDC ||
                (srTransPara->bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
                (srTransPara->bWaveSID == d_VW_SID_AE_EMV) ||
                srTransPara->bWaveSID == d_VW_SID_PAYPASS_MCHIP ||
                srTransPara->bWaveSID == d_VW_SID_CUP_EMV ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_EMV ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_MSD ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY2 ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY))) &&
                (srTransPara->byTransType == SALE
                || srTransPara->byTransType == SALE_OFFLINE
                || srTransPara->byTransType == PREAUTH_COMP
                || srTransPara->byTransType == PRE_AUTH
                || srTransPara->byTransType == REFUND
                || srTransPara->byTransType == CASH_ADVANCE
                || srTransPara->byTransType == CUP_SALE
                || srTransPara->byTransType == CUP_PRE_AUTH
                //|| srTransPara->byTransType == SALE_TIP  // Remove the Tip Adjust DE23 & DE55
                || srTransPara->byTransType == VOID
                || srTransPara->byTransType == MPU_PREAUTH
                || srTransPara->byTransType == MPU_PREAUTH_COMP
                || srTransPara->byTransType == MPU_PREAUTH_COMP_ADV
                || srTransPara->byTransType == CHANGE_PIN))
        {
			vdDebug_LogPrintf("vdModifyBitMapFunc HERE 2*******************");

            vdDebug_LogPrintf("Transcation set DE55 T5F34_len[%d]", srTransPara->stEMVinfo.T5F34_len);
            vdCTOSS_SetBitMapOn(inBitMap, 55);

            // patrick make field 23 on for chip 20150116 thai
            if(srTransPara->stEMVinfo.T5F34_len > 0)
		{
					
			vdDebug_LogPrintf("vdModifyBitMapFunc HERE 3*******************");
	            vdCTOSS_SetBitMapOn(inBitMap, 23);
            	}
        }

// for New Host upgrade - remove sending of de36 02062019 - CB_MPU_NH_MIGRATION
#if 0
        //@@IBR ADD 20161025																		//12/27/2018 - disable de36 for CUP MSR PreAuth trans.  To eliminate rc96 host response. based on zin's test result.
        if ((srTransPara->byEntryMode == CARD_ENTRY_MSR) && (srTransPara->byTransType == PRE_AUTH) && (srTransRec.IITid != 6)) {
            vdCTOSS_SetBitMapOn(inBitMap, 36);
			vdDebug_LogPrintf("XXXXXXXXXXXXXXXXXXXXXXXX");
			
        }		
#endif		

        if (srTransPara->byEntryMode == CARD_ENTRY_MANUAL) {
            vdMyEZLib_LogPrintf(". byEntryMode CN_TRUE");
            vdCTOSS_SetBitMapOn(inBitMap, 2);
            vdCTOSS_SetBitMapOn(inBitMap, 14);

            vdCTOSS_SetBitMapOff(inBitMap, 35);
			if (strlen(srTransPara->szCVV2) > 0)
               { 
					vdDebug_LogPrintf(("1)BATCH_UPLOAD set DE48"));//thandar
			   		vdCTOSS_SetBitMapOn(inBitMap, 48); //@@IBR ADD 20170203 turn on the bit 48 cvv for manual entry mode
			   }
			}


        memset(szTipAmount, 0x00, sizeof (szTipAmount));
        wub_hex_2_str(srTransPara->szTipAmount, szTipAmount, 6);
        DebugAddSTR("Tip", szTipAmount, 12);
        if (atol(szTipAmount) > 0) {
            vdCTOSS_SetBitMapOn(inBitMap, 54);

            //Should be Online void the Intial SALE amount.
            //use szStoreID to store how much amount fill up in DE4 for VOID
//            if (srTransPara->byTransType == VOID && 0 == memcmp(srTransPara->szStoreID, srTransPara->szBaseAmount, 6)) {
            if ((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && 0 == memcmp(srTransPara->szStoreID, srTransPara->szBaseAmount, 6)) {
                vdCTOSS_SetBitMapOff(inBitMap, 54);
            }
        }

		vdDebug_LogPrintf("BATCH_UPLOAD set DE48 [%d] [%d]", inTransCode, srTransPara->byTransType);//thandar

        if (strlen(srTransPara->szCVV2) > 0) {
			vdDebug_LogPrintf(("2)BATCH_UPLOAD set DE48"));//thandar

				//should not send de48 for void, reversal
				#ifdef CBB_FIN_ROUTING
				if(srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH){	
					
					vdDebug_LogPrintf(("2.1)BATCH_UPLOAD set DE48"));//thandar
					vdCTOSS_SetBitMapOff(inBitMap, 48);
				}
				else{
					
					vdDebug_LogPrintf(("2.3)BATCH_UPLOAD set DE48"));//thandar
		           		 vdCTOSS_SetBitMapOn(inBitMap, 48);
				}
				#else
					vdCTOSS_SetBitMapOn(inBitMap, 48);
				#endif
        }

    } else if (inTransCode == BATCH_UPLOAD) {
        if (srTransPara->byEntryMode == CARD_ENTRY_ICC ||
                (srTransPara->byEntryMode == CARD_ENTRY_WAVE &&
                (srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_2 ||
                srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_QVSDC ||
                (srTransPara->bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
                (srTransPara->bWaveSID == d_VW_SID_AE_EMV) ||
                srTransPara->bWaveSID == d_VW_SID_PAYPASS_MCHIP ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_EMV ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY2 ||
                srTransPara->bWaveSID == d_EMVCL_SID_JCB_MSD))) {
            vdDebug_LogPrintf(("BATCH_UPLOAD set DE55"));
            vdCTOSS_SetBitMapOn(inBitMap, 55);

            if (srTransPara->stEMVinfo.T5F34_len > 0)
                vdCTOSS_SetBitMapOn(inBitMap, 23);
        }

        memset(szTipAmount, 0x00, sizeof (szTipAmount));
        wub_hex_2_str(srTransPara->szTipAmount, szTipAmount, 6);
        DebugAddSTR("Tip", szTipAmount, 12);
        if (atol(szTipAmount) > 0) {
            vdCTOSS_SetBitMapOn(inBitMap, 54);
        }

	 #ifdef CBB_FIN_ROUTING
        if (strlen(srTransPara->szCVV2) > 0)
	{			
			vdDebug_LogPrintf(("3)BATCH_UPLOAD set DE48"));//thandar

				vdCTOSS_SetBitMapOff(inBitMap, 48);
        }
	#else	
		if (strlen(srTransPara->szCVV2) > 0) {			
			vdDebug_LogPrintf(("3)BATCH_UPLOAD set DE48"));//thandar
		
			//if(srTransPara->HDTid != 7)
				vdCTOSS_SetBitMapOn(inBitMap, 48);
		}
	#endif
	
    } else if ((inTransCode == SETTLE) && (inTransCode == CLS_BATCH)) {
        vdDebug_LogPrintf(("Settlement modify field"));
    }

    // patrick fix online PIN for sale, preauth and refund only 20151103
    // tangjing add for CUP transaction
    vdCTOSS_SetBitMapOff(inBitMap, 52);

	
    if ((srTransPara->byEntryMode == CARD_ENTRY_ICC)
            && ((inTransCode == SALE)
            || (inTransCode == PRE_AUTH)
            || (inTransCode == CUP_SALE)
            || (inTransCode == CUP_PRE_AUTH)))
        //		|| (inTransCode == REFUND )))
    {
        memset(EMVtagVal, 0x00, sizeof (EMVtagVal));
        memset(szPinBlock, 0x00, sizeof (szPinBlock));
        EMVtagLen = 3;
        memcpy(EMVtagVal, srTransPara->stEMVinfo.T9F34, EMVtagLen);
        vdDebug_LogPrintf("T9F34 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
        //'02' or '42'(CVM Code for 'Online PIN') Byte 1(CVM Performed)
        if (EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42) {
            wub_hex_2_str(srTransPara->szPINBlock, szPinBlock, 8);
            vdDebug_LogPrintf("szPinBlock=[%s]......", szPinBlock);
            if (strlen(szPinBlock) > 0)
                vdCTOSS_SetBitMapOn(inBitMap, 52);
        }
    }

#ifdef PIN_CHANGE_ENABLE
	DebugAddHEX("vdModifyBitMapFunc->srTransPara->szNewPINBlock", srTransPara->szNewPINBlock, 8);
    if (memcmp(srTransPara->szVerifyNewPINBlock, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) != 0)
	{
		vdDebug_LogPrintf("szVerifyNewPINBlock > 0 DE52 WAS TURNED ON!!!");    
    }	
#endif

    //if (1 == srTransRec.byPINEntryCapability)
        //vdCTOSS_SetBitMapOn(inBitMap, 52);
	DebugAddHEX("srTransPara->szPINBlock", srTransPara->szPINBlock, 8);
    if (memcmp(srTransPara->szPINBlock, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) != 0){ //@@IBR ADD 20161109
		vdDebug_LogPrintf("DE52 WAS TURNED ON!!!");

		#ifdef CB_MPU_NH_MIGRATION 
		if(srTransPara->byTransType != VOID)
        vdCTOSS_SetBitMapOn(inBitMap, 52);
		#endif
    }

	#if 0
    if (VS_TRUE == fGetMPUCard() && 
		(MPU_DEBIT_HOST_IDX != srTransPara->HDTid && UNIONPAY_HOST_IDX != srTransPara->HDTid))
        vdCTOSS_SetBitMapOff(inBitMap, 52);
	#endif

	if (inTransCode == BATCH_UPLOAD)
		vdCTOSS_SetBitMapOff(inBitMap, 52);
        
        if((inTransCode == BATCH_UPLOAD) && (srTransPara->HDTid == 19)){
            vdCTOSS_SetBitMapOff(inBitMap, 52);
            vdCTOSS_SetBitMapOff(inBitMap, 23);
        }

//Add to send de55 for JCB CTLS transactions #4
#if 0
	/*for MPU application no need send DE55*/
	if (VS_TRUE == fGetMPUTrans() && CARD_ENTRY_ICC != srTransPara->byEntryMode)
	{
		
		vdDebug_LogPrintf(("BATCH_UPLOAD set OFF DE55"));
#if 0	
		if((srTransRec.IITid == 6) && (srTransPara->byTransType == PREAUTH_COMP)){		
			vdDebug_LogPrintf("DE23 TURNED ON!!!");			
			vdCTOSS_SetBitMapOn(inBitMap, 23);
		}
		else{
			vdDebug_LogPrintf("DE23 TURNED OFF!!!");						
			vdCTOSS_SetBitMapOff(inBitMap, 23);
		}
#else
		vdCTOSS_SetBitMapOff(inBitMap, 23); // for chip based only - pan sequence number.
		vdCTOSS_SetBitMapOff(inBitMap, 55); // requested by sein bading.
#endif

	}
#endif
	// for New Host upgrade - remove sending of de26 02062019
	#ifdef CB_MPU_NH_MIGRATION     
	#if 0
        /*26 should depned on DE52!!!!*/
	if ((srTransPara->byTransType == PRE_AUTH || 
			srTransPara->byTransType == PREAUTH_COMP ||
            (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PRE_AUTH) ||
            (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PREAUTH_COMP)) && srTransPara->HDTid != 21)
	{
//		if (srTransRec.byPINEntryCapability = 1) //@@IBR NOTE: this will cause the problem for PIN ENTRY CAP
		if ((srTransRec.byPINEntryCapability == 1) && (srTransPara->HDTid != 7))
		{
			vdDebug_LogPrintf("here we set DE26");
			vdCTOSS_SetBitMapOn(inBitMap, 26);
		}
	}
	#endif
	#endif

	/*MPU Cert 20170207 
	2.TIT-PAUWIT-001/002/003
   In Pre-Auth cancel request message, "field 37" value does not include. As per GRG specification, this field value must include.
	*/
//	if (srTransPara->byTransType == MPU_VOID_PREAUTH)
//	{
//		vdCTOSS_SetBitMapOn(inBitMap, 37);
//	}
//
//	if (srTransPara->byTransType == MPU_VOID_PREAUTH_COMP)
//	{
//		vdCTOSS_SetBitMapOn(inBitMap, 37);
//	}
        
	if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PRE_AUTH && srTransPara->HDTid != 21)
	{
		vdCTOSS_SetBitMapOn(inBitMap, 25);
		vdCTOSS_SetBitMapOn(inBitMap, 37);
	}

	if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PREAUTH_COMP && srTransPara->HDTid != 21)
	{

	
		vdDebug_LogPrintf("vdModifyBitMapFunc VOID PREAUTH COMP!!!"); 

		vdCTOSS_SetBitMapOn(inBitMap, 25);
		vdCTOSS_SetBitMapOn(inBitMap, 37);
	}

	/*MPU Cert 20170207 
	3.TIT-PAUREQ-001
   In Pre-Auth complete request message, "field 55" is being present. As per UPI expected result this field must be absent.
	*/
//	if (srTransPara->byTransType == MPU_PREAUTH_COMP 
//		&& fChkCUPCard())
//	{
//		vdCTOSS_SetBitMapOff(inBitMap, 55);
//	}
        
	if (srTransPara->byTransType == PREAUTH_COMP && srTransPara->HDTid == 19)
	{
		vdCTOSS_SetBitMapOff(inBitMap, 55);
	}


	#if 1
    vdDebug_LogPrintf("vdModifyBitMapFunc inGetNEWPACFlow [%d]", inGetNEWPACFlow);
	
	//http://118.201.48.210:8080/redmine/issues/1525.99 and 100 #5
	if(inGetNEWPACFlow != 1)
	{
		if (srTransPara->byTransType == PREAUTH_COMP && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 22 || srTransPara->HDTid == 23))
		{
		
			vdDebug_LogPrintf("vdModifyBitMapFunc NO RRN FOR PREAUTH COMP!!!");	
			vdCTOSS_SetBitMapOff(inBitMap, 37);
		}

		if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PREAUTH_COMP && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 ||
			srTransPara->HDTid ==  22 || srTransPara->HDTid == 23)){
			
			vdDebug_LogPrintf("vdModifyBitMapFunc NO RRN FOR VOID PREAUTH COMP!!!");	
			vdCTOSS_SetBitMapOff(inBitMap, 37);
		}
		
	}
	#else
	if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PREAUTH_COMP && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18)){
		
		vdDebug_LogPrintf("vdModifyBitMapFunc NO RRN FOR VOID PREAUTH COMP!!!");	
		vdCTOSS_SetBitMapOff(inBitMap, 37);
	}
	
	#endif
	

	vdDebug_LogPrintf(" fGetMPUTrans(%d) srTransPara->byPackType(%d)", fGetMPUTrans(), srTransPara->byPackType);
	if (VS_TRUE == fGetMPUTrans() && 
		(srTransPara->byPackType == REVERSAL ||
		srTransPara->byPackType == MPU_PREAUTH_REV ||
		srTransPara->byPackType == MPU_PAC_REV ||
		srTransPara->byPackType == MPU_PAC_ADV_REV ||
		srTransPara->byPackType == MPU_VOID_PREAUTH_REV ||
		srTransPara->byPackType == MPU_VOID_PAC_REV ||
		srTransPara->byPackType == REVERSAL_CASH_ADV)) //#2 srTransPara->byPackType == REVERSAL_CASH_ADV; fix for reversal error on cash advance 20201112
	{
vdDebug_LogPrintf("!!!!!!!!!!!!!!!!!!!!");	

		//vdCTOSS_SetBitMapOn(inBitMap, 38); - for testing. as per new host specs 03/11/2019 - CB_MPU_NH_MIGRATION
		vdCTOSS_SetBitMapOn(inBitMap, 49);

		#if 0 // for New Host upgrade 02092019 - CB_MPU_NH_MIGRATION
		vdCTOSS_SetBitMapOn(inBitMap, 61);
		vdCTOSS_SetBitMapOn(inBitMap, 64);
		#endif

		vdDebug_LogPrintf("reversal set off DE26, 35, 52");
		vdCTOSS_SetBitMapOff(inBitMap, 26);
		//vdCTOSS_SetBitMapOff(inBitMap, 35); //removed de35 based on MPU uat result  07/12/2019 - CB_MPU_NH_MIGRATION
		vdCTOSS_SetBitMapOff(inBitMap, 52);

		/*MPU Cert 20170207 
		2.TIT-PUR-011/TIT-PURWIT-001
		In reversal request message, "field 23" does not include. As per UPI expected result, this field must be present. */
		if (CARD_ENTRY_ICC == srTransPara->byEntryMode)
			vdCTOSS_SetBitMapOn(inBitMap, 23);
		
	}
	else
	{		
		#ifdef CBB_FIN_ROUTING
		vdDebug_LogPrintf("vdModifyBitMapFunc CCCCCCCC");	
		if (srTransPara->byPackType == REVERSAL && srTransPara->byTransType ==  SALE && CARD_ENTRY_MANUAL == srTransPara->byEntryMode){		
			
			vdDebug_LogPrintf("vdModifyBitMapFunc REMOVE DE35 ON Reveral Manual Entry!!!");	
			vdCTOSS_SetBitMapOff(inBitMap, 35);
		}
		#endif
	}

	if (VS_TRUE == fGetMPUTrans() && srTransPara->byPackType == VOID_REVERSAL)
	{
vdDebug_LogPrintf("+++++++++++++++++++");		
                
		if(srTransRec.fIsInstallment != TRUE)			
		vdCTOSS_SetBitMapOff(inBitMap, 49);
			//vdCTOSS_SetBitMapOn(inBitMap, 49); - CB_MPU_NH_MIGRATION
                


		#if 0 // for New Host upgrade 02092019 - no need to send on request - CB_MPU_NH_MIGRATION
		
		vdCTOSS_SetBitMapOn(inBitMap, 38);
		vdCTOSS_SetBitMapOn(inBitMap, 60);

		if(srTransRec.fIsInstallment != TRUE)
		{
			vdCTOSS_SetBitMapOn(inBitMap, 61);
			vdCTOSS_SetBitMapOn(inBitMap, 64);
        }
		
		#endif
        
		vdCTOSS_SetBitMapOff(inBitMap, 39);
		vdCTOSS_SetBitMapOff(inBitMap, 52);
		vdCTOSS_SetBitMapOff(inBitMap, 62);
	}

#ifdef CB_MPU_NH_MIGRATION
	/*for MPU-UPI card BIN rang (Credit app) no need send DE55*/
	if (CARD_ENTRY_ICC == srTransPara->byEntryMode && 
		(47 == srTransPara->CDTid || 51 == srTransPara->CDTid) )
	{
		vdCTOSS_SetBitMapOff(inBitMap, 23);
        vdCTOSS_SetBitMapOff(inBitMap, 55);
	}
#endif

// for New Host upgrade remove de23 02092019 - CB_MPU_NH_MIGRATION
#if 0
	if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PRE_AUTH)
	{
		vdCTOSS_SetBitMapOn(inBitMap, 61);
	}
#endif	

	/*Why send De23 for void????????*/
	
	/*MPU Cert 20170207 
	2.TIT-PUR-011/TIT-PURWIT-001
	In reversal request message, "field 23" does not include. As per UPI expected result, this field must be present. */
//	if (CARD_ENTRY_EASY_ICC == srTransPara->byEntryMode &&
//		(srTransPara->byTransType == MPU_VOID_PREAUTH || 
//		srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
//                ((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE == fGetMPUTrans()))
////		(srTransPara->byTransType == VOID && VS_TRUE == fGetMPUTrans()))
//		)
//	{
//		vdCTOSS_SetBitMapOn(inBitMap, 23);
//	}
        
//        if (CARD_ENTRY_EASY_ICC == srTransPara->byEntryMode &&
//		(srTransPara->byTransType == MPU_VOID_PREAUTH || 
//		srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
//                ((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE == fGetMPUTrans()))
////		(srTransPara->byTransType == VOID && VS_TRUE == fGetMPUTrans()))
//		)
//	{
//		vdCTOSS_SetBitMapOn(inBitMap, 23);
//	}

// not needed for CBB_FIN_ROUTING
#if 0
//#ifdef CB_MPU_NH_MIGRATION
	vdDebug_LogPrintf("byEntryMode [%d], srTransPara->byTransType [%d], srTransPara->HDTid [%d]", 
	srTransPara->byEntryMode, srTransPara->byTransType, srTransPara->HDTid);
        
	if (srTransPara->byEntryMode == CARD_ENTRY_EASY_ICC && (srTransPara->byTransType == VOID) && srTransPara->HDTid != 21)
	{
		vdDebug_LogPrintf("**********************");
		// for MPU new host implementation. no need to send de23 on void request
		if(srTransPara->HDTid != 17 && srTransPara->HDTid != 18 && srTransPara->HDTid !=19)
			vdCTOSS_SetBitMapOn(inBitMap, 23);
	}
#endif
	
    if (inTransCode == CUP_PREAUTH_COMP || inTransCode == CUP_VOID_PREAUTH) 
   	{
        vdCTOSS_SetBitMapOn(inBitMap, 37);
        vdCTOSS_SetBitMapOn(inBitMap, 38);

		vdCTOSS_SetBitMapOn(inBitMap, 61);
    }
        
        if(srTransRec.byTransType == SALE && srTransRec.fIsInstallment == TRUE){
            vdCTOSS_SetBitMapOn(inBitMap, 35);
            if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL || srTransPara->byEntryMode == CARD_ENTRY_MSR)
                vdCTOSS_SetBitMapOff(inBitMap, 23);
            
            if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL){
                vdCTOSS_SetBitMapOn(inBitMap, 2);
                vdCTOSS_SetBitMapOn(inBitMap, 14);
                vdCTOSS_SetBitMapOff(inBitMap, 35);
				if (strlen(srTransPara->szCVV2) > 0)
                   {
                        vdDebug_LogPrintf(("4)BATCH_UPLOAD set DE48"));//thandar
                   		vdCTOSS_SetBitMapOn(inBitMap, 48);
					}
            }
            
            vdCTOSS_SetBitMapOn(inBitMap, 60);

			vdCTOSS_SetBitMapOff(inBitMap, 49);
        }
        
        if(srTransRec.byTransType == VOID && srTransRec.fIsInstallment == TRUE){
//            vdCTOSS_SetBitMapOn(inBitMap, 35);
            vdCTOSS_SetBitMapOff(inBitMap, 52);
            if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
                vdCTOSS_SetBitMapOff(inBitMap, 35);
            
            if(fGetMPUCard() == TRUE){
                vdCTOSS_SetBitMapOff(inBitMap, 23);
//                vdCTOSS_SetBitMapOff(inBitMap, 14);
            }
                
            
            vdCTOSS_SetBitMapOn(inBitMap, 60);
            vdCTOSS_SetBitMapOn(inBitMap, 62);

			vdCTOSS_SetBitMapOff(inBitMap, 49);
        }
        
        if(inTransCode == BATCH_UPLOAD && fGetCashAdvAppFlag() == TRUE && 
			(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19))
		{
            vdCTOSS_SetBitMapOn(inBitMap, 2);
            vdCTOSS_SetBitMapOn(inBitMap, 3);
            vdCTOSS_SetBitMapOn(inBitMap, 4);
            
            vdCTOSS_SetBitMapOff(inBitMap, 7);
            
            vdCTOSS_SetBitMapOn(inBitMap, 14);
            vdCTOSS_SetBitMapOn(inBitMap, 23);
            vdCTOSS_SetBitMapOn(inBitMap, 24);
            
            vdCTOSS_SetBitMapOff(inBitMap, 25);
            
            vdCTOSS_SetBitMapOn(inBitMap, 37);
            vdCTOSS_SetBitMapOn(inBitMap, 38);
            
            vdCTOSS_SetBitMapOff(inBitMap, 48);
            vdCTOSS_SetBitMapOff(inBitMap, 49);
            
            vdCTOSS_SetBitMapOn(inBitMap, 62);
        }
        
        if(srTransRec.byTransType == VOID && fGetCashAdvAppFlag() == TRUE && 
			(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19))
		{
            vdCTOSS_SetBitMapOff(inBitMap, 7);
            vdCTOSS_SetBitMapOn(inBitMap, 14);
            vdCTOSS_SetBitMapOn(inBitMap, 24);
            vdCTOSS_SetBitMapOff(inBitMap, 26);
            
            vdCTOSS_SetBitMapOff(inBitMap, 35);
            vdCTOSS_SetBitMapOff(inBitMap, 38);
            
            vdCTOSS_SetBitMapOff(inBitMap, 49);
            vdCTOSS_SetBitMapOff(inBitMap, 60);
            vdCTOSS_SetBitMapOff(inBitMap, 61);
            
            vdCTOSS_SetBitMapOn(inBitMap, 62);
            
            vdCTOSS_SetBitMapOff(inBitMap, 64);
        }

		//if(srTransRec.byTransType == PRE_AUTH || srTransRec.HDTid == 19)
		if(srTransRec.byTransType == PRE_AUTH)
		{
			vdDebug_LogPrintf("PreAuth set DE26 off");
			vdCTOSS_SetBitMapOff(inBitMap, 26);
			
			if (19 == srTransRec.HDTid && 1 != fGetMPUCard())
			{
				vdDebug_LogPrintf("CUP PreAuth set DE26 on");
				vdCTOSS_SetBitMapOn(inBitMap, 26);
			}
		}

		/*Credit host*/
		if (VS_TRUE != fGetMPUTrans() && 
			(srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19))
		{
			vdDebug_LogPrintf("Credit host re-check bitmap...");
			if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PRE_AUTH)
			{
				vdCTOSS_SetBitMapOff(inBitMap, 23);
				vdCTOSS_SetBitMapOff(inBitMap, 55);
				vdCTOSS_SetBitMapOff(inBitMap, 61);
			}

			if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PREAUTH_COMP)
			{
				vdDebug_LogPrintf("MFL 3!!!"); 		
			
				vdCTOSS_SetBitMapOff(inBitMap, 23);
				vdCTOSS_SetBitMapOff(inBitMap, 55);
			}
			
			//thandar22May2018_added to remove as per Chan's request for UPI, Visa MC for VOid
			//thandar25May2018_added to have DE23 but to remove as per Chan's email request for UPI, Visa MC for VOid
			if(srTransPara->byTransType == VOID && srTransPara->byOrgTransType == SALE && (srTransRec.HDTid == 7 || srTransRec.HDTid == 21))
			{
				//vdCTOSS_SetBitMapOff(inBitMap, 23);
				vdCTOSS_SetBitMapOff(inBitMap, 55);
			}
		}

        if(srTransRec.fAlipay == TRUE)
            vdCTOSS_SetBitMapOn(inBitMap, 60);		

		if(fGetCashAdvAppFlag() == TRUE && (srTransRec.HDTid == 17))
		{
			vdDebug_LogPrintf("CASH_ADVANCE SETTLEMENT");
        
            vdCTOSS_SetBitMapOff(inBitMap, 7);		
            vdCTOSS_SetBitMapOff(inBitMap, 18);
			vdCTOSS_SetBitMapOff(inBitMap, 48);
			#ifdef CB_MPU_NH_MIGRATION
			if(srTransPara->byTransType != REFUND) // for MPU new host implementation
				vdCTOSS_SetBitMapOff(inBitMap, 49);
			#endif
        }

		
		vdDebug_LogPrintf("srTransRec.byPackType[%d]", srTransRec.byPackType);

		if(srTransRec.byPackType == REVERSAL_CASH_ADV)
		{   
            vdCTOSS_SetBitMapOff(inBitMap, 52);		

			//11162020 - Acquired from OLD HOST, for NEW CAV and PIN change module implementation.
			#ifdef PIN_CHANGE_ENABLE
            vdCTOSS_SetBitMapOff(inBitMap, 35);		
            vdCTOSS_SetBitMapOn(inBitMap, 2);
            vdCTOSS_SetBitMapOn(inBitMap, 14);	
			#endif
			
        }

		if((srTransRec.byPackType == VOID_PREAUTH ||srTransRec.byPackType == VOID_REVERSAL || srTransRec.byPackType == REVERSAL|| srTransRec.byPackType == VOID_PREAUTH_COMP || srTransRec.byPackType == PREAUTH_COMP) && srTransRec.HDTid == 7 && (srTransRec.byTransType == PREAUTH_COMP|| srTransRec.byTransType == VOID))
		{   
			vdDebug_LogPrintf("vdModifyBitMapFunc REVERSAL HEREEEEE");
			
            //vdCTOSS_SetBitMapOff(inBitMap, 7);// for http://118.201.48.210:8080/redmine/issues/1525.36
            vdCTOSS_SetBitMapOff(inBitMap, 26);
        }

		if(fGetCashAdvAppFlag() == TRUE && ((inTransCode == BATCH_UPLOAD) || (inTransCode == TC_UPLOAD)) )
		{   
            vdCTOSS_SetBitMapOff(inBitMap, 35);		
        }

		/*CB ask to remove DE18 for MPU host settlement, will change if they tested ok*/
		#if 0
		if( (srTransPara->byTransType == SETTLE || srTransPara->byTransType == MPU_SETTLE)
			&& (srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19))
		{
			vdDebug_LogPrintf("ALL MPU SETTLEMENT");
            vdCTOSS_SetBitMapOff(inBitMap, 18);
        }
		#endif

		// As mentioned by Yahya - no need to send de60 111519
		#if 0
		//#ifdef QUICKPASS
			vdDebug_LogPrintf("bWaveSID [%d]", srTransPara->bWaveSID);
			
		
		if(srTransPara->bWaveSID == d_VW_SID_CUP_EMV && srTransPara->byTransType == SALE && srTransRec.HDTid == 7){
			
			vdDebug_LogPrintf("HERE $$$$$$");
			vdCTOSS_SetBitMapOn(inBitMap, 60);
		}
		#endif


    #ifdef TOPUP_RELOAD		
    if(srTransRec.HDTid == 20)
    {
        #ifdef TOPUP_RELOAD
            vdCTOSS_SetBitMapOff(inBitMap, 24);
		
            vdCTOSS_SetBitMapOff(inBitMap, 23); /*card sequence number*/
            vdCTOSS_SetBitMapOff(inBitMap, 55); /*emv data*/
        #else
            vdCTOSS_SetBitMapOff(inBitMap, 12);
        #endif
    }
	#endif

    if(srTransRec.HDTid == 7 || srTransRec.HDTid == 21) /*Finexus host does not support DE54-additional amount*/
        vdCTOSS_SetBitMapOff(inBitMap, 54);



	vdDebug_LogPrintf("srTransRec.byPackType:[%d]byTransType:[%d]fGetMPUTrans:[%d]srTransPara->byOrgTransType[%d]",
		srTransRec.byPackType, srTransPara->byTransType, fGetMPUTrans(), srTransPara->byOrgTransType);

	//Acquired from OLD HOST, for NEW CAV and PIN change module implementation.
	#ifdef PIN_CHANGE_ENABLE
	if(fGetMPUTrans() == FALSE) // ADDED TO fix reversal error if MPU card (hdtdi 17, 18 and 19) reported by Yam - http://118.201.48.210:8080/redmine/issues/1525.22 (06302021)
	{
		if((srTransPara->byTransType == CASH_ADVANCE || srTransPara->byTransType == CHANGE_PIN) && (srTransPara->byEntryMode == CARD_ENTRY_EASY_ICC || 
			srTransPara->byEntryMode == CARD_ENTRY_WAVE || srTransPara->byEntryMode == CARD_ENTRY_ICC))
		{
			
			vdDebug_LogPrintf("did not remove EMV related field");
			vdCTOSS_SetBitMapOn(inBitMap, 55);
		}
	}
	#endif

	#ifdef CBB_FIN_ROUTING  // for testing...08/04/2022
	if(srTransRec.HDTid == 22 || srTransRec.HDTid == 23)// || srTransRec.HDTid == 17 )		
	{
		vdDebug_LogPrintf("vdModifyBitMapFunc ABCD");
	
		if(srTransPara->byTransType == MPU_SIGNON)			
			vdCTOSS_SetBitMapOff(inBitMap, 62);		
		else	
			vdCTOSS_SetBitMapOn(inBitMap, 62);		

		
		//http://118.201.48.214:8080/issues/75.56 #2 [2. Kindly remove DE 23 & 35 in void iso pkt.] de35 was removed in ISOENGINE22 and 23 s3db.
		if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == SALE || srTransRec.byPackType == MPU_VOID_PREAUTH_COMP || 
			srTransRec.byPackType == MPU_VOID_PREAUTH || srTransRec.byPackType == VOID_PREAUTH || srTransRec.byPackType == VOID_PREAUTH_COMP)
		{
			
				vdDebug_LogPrintf("vdModifyBitMapFunc EFGH");
				vdCTOSS_SetBitMapOff(inBitMap, 23);
				vdCTOSS_SetBitMapOn(inBitMap, 37); // http://118.201.48.214:8080/issues/75.61
		}				
		
		vdCTOSS_SetBitMapOff(inBitMap, 49);

		if(srTransPara->byTransType == REFUND)
		{			
			vdCTOSS_SetBitMapOff(inBitMap, 12);
			vdCTOSS_SetBitMapOff(inBitMap, 13);
		}		
	}
	
	vdDebug_LogPrintf("vdModifyBitMapFunc >>>>END");
	#endif
	
}

/************************************************************************
Function Name: inSendAndReceiveFormComm()
Description:
    Call function to send and receive data
Parameters:
    [IN] srTransPara
         uszSendData
         inSendLen
         uszReceData
Return: inResult --> Receive data len;
        ST_ERROR
 ************************************************************************/
int inSendAndReceiveFormComm(TRANS_DATA_TABLE* srTransPara,
        unsigned char* uszSendData,
        int inSendLen,
        unsigned char* uszReceData) {
    int inResult;
    BYTE key;
    int inPos=0;
	char temp[1024+1];
	int inLen=0;

#ifdef CB_MPU_NH_MIGRATION
	char uszSendDataNoTPDU[255+1];
	char usTPDU[255+1];
#endif

    vdDebug_LogPrintf("**inSendAndReceiveFormComm START* [%d]*", srTransPara->HDTid); 
    vdDebug_LogPrintf("inIPHeader = [%d][%d][%s][%d][%d]", 
		strCPT.inIPHeader, inSendLen, strHDT.szTPDU, strHDT.fTPDUOnOff, strCST.inCurrencyIndex);


    if (VS_TRUE == strTCT.fDemo) {
        vdDebug_LogPrintf("DEMO Call inSendData!!");
        inResult = d_OK;
    } else {
        #ifdef TOPUP_RELOAD
        if(srTransPara->HDTid == 20)
        {
			//60 12 04 00 01
            //inPos=inGetPosition(uszSendData, "\x60\x12\x04\x00\x01", 5, inSendLen);
			//vdDebug_LogPrintf("inPos:%d", inPos+5);
			if(srTransPara->byPackType == BALANCE_ENQUIRY || srTransPara->byPackType == TOPUP || srTransPara->byPackType == RELOAD 
			|| srTransPara->byPackType == TOPUP_RELOAD_LOGON)
			{
                memset(temp, 0, sizeof(temp));
                inLen=inBuildStringISO(uszSendData, inSendLen, &temp, srTransPara->byPackType);
                vdDebug_LogPrintf("inBuildStringISO[%s],len[%d]", temp, inLen);
                inResult = srCommFuncPoint.inSendData(srTransPara, temp, inLen);
			}
			else
			{
				vdDebug_LogPrintf("Topup/Reload Reversal[%s],len[%d]", uszSendData, inSendLen);
				inResult = srCommFuncPoint.inSendData(srTransPara, uszSendData, inSendLen);
			}
        }
		else
		#endif
		{
			#ifdef CB_MPU_NH_MIGRATION
			if(strHDT.fTPDUOnOff == CN_FALSE)		
			{	
				inResult = srCommFuncPoint.inSendData(srTransPara, &uszSendData[5], inSendLen - 5); 		
			}
			else
				inResult = srCommFuncPoint.inSendData(srTransPara, uszSendData, inSendLen); 
			#endif
		}
    }

	#if 0
	if (1 == get_env_int("#REVT")
		&& srTransPara->byPackType != REVERSAL
		&& srTransPara->byPackType != TC_UPLOAD
		&& srTransPara->byPackType != BATCH_UPLOAD)
	{
    	vdDebug_LogPrintf("REVT transtype %d", srTransPara->byTransType);
    	vdDebug_LogPrintf("REVT pack type %d", srTransPara->byPackType);
		
		CTOS_Delay(2000);
		vduiClearBelow(2);
		CTOS_LCDTPrintXY(1, 7,  "[ENTER]   CONTINUE");
		vdDisplayErrorMsg(1, 8, "[KEY 0]   REVERSAL");
		key = WaitKey(5);
		if (d_KBD_0 == key)
		{
			vdDebug_LogPrintf("REVT User Press d_KBD_0");
			vdDisplayErrorMsg(1, 8, "PRESS KEY0 RECV ERR");
			inCTOS_inDisconnect();
        	return ST_ERROR;
		}
	}
	#endif

    vdDebug_LogPrintf("**inSendAndReceiveFormComm [%d]", inResult);
    if (inResult != d_OK) {
        if (srTransPara->byPackType != TC_UPLOAD) {
            vdSetErrorMessage("SEND LEN ERR");
        }

        //inSetTextMode();				
        inCTOS_inDisconnect();
        return ST_ERROR;
    } else {
        //vdDisplayAnimateBmp(0,0, "Comms1.bmp", "Comms2.bmp", "Comms3.bmp", "Comms4.bmp", NULL);

        //vdSetCommThreadStatus(1);
        //vdThreadDisplayCommBmpEx(0, 32, "Comms1.bmp", "Comms2.bmp", "Comms3.bmp", "Comms4.bmp", NULL);

        if (VS_TRUE == strTCT.fDemo) {
            vdDebug_LogPrintf("DEMO Call inRecData!!");
            inResult = inCTOS_PackDemoResonse(srTransPara, uszReceData);
        } else {
            inResult = srCommFuncPoint.inRecData(srTransPara, uszReceData);
        }
        //vdSetCommThreadStatus(0);

		vdDebug_LogPrintf("@@@inResult@@@[%d]@@@HDTID[%d]@@@", inResult, srTransPara->HDTid);
		DebugAddHEX("recv da-2", uszReceData, inResult);

        if (inResult > 0) 
		{
            DebugAddHEX("recv da", uszReceData, inResult);
            vdMyEZLib_LogPrintff(uszReceData, inResult);
			
			#ifdef TOPUP_RELOAD
			if(srTransPara->HDTid == 20)
			{
				memset(temp, 0, sizeof(temp));
				strcpy(temp, "028960020000000210703440010ea1800016516263900001687102000000000000000000004019042410175322066012040001194O8FB69373B693730010107607CBBANK DEVELOPMENT        YGN         MM117001001100200374400300203021015000001000011302025003104026001D038012194O8FB6937310000400011010040001103012000695318100840");
				//028960000002000210703440010ea1800016516263900001687102000000000000000000005119032704064722066012040001193R9499E5E699E5E60010107607CBBANK DEVELOPMENT        YGN         MM117001001100200374400300203021015000001000011302025003104026001D038012193R9499E5E610000400011010040001103012000696920100003
				vdDebug_LogPrintf("uszReceData: %s", uszReceData);
				//inParseStringISO(temp+4, uszReceData);
				
  			    inResult=inParseStringISO(uszReceData, uszReceData, srTransPara->byPackType);
				DebugAddHEX("recv da after conversion", uszReceData, inResult);
			}
			#endif

			#ifdef CB_MPU_NH_MIGRATION
			if(strHDT.fTPDUOnOff == CN_FALSE)
			{
				memset(usTPDU,0x00,sizeof(usTPDU)); 	

				inHDTRead(srTransPara->HDTid);
				memcpy(usTPDU, strHDT.szTPDU, 5);
				DebugAddHEX("YYYY inSendAndReceiveFormComm RCVD data", usTPDU, 5);

				inResult = inResult + 5;

				vdDebug_LogPrintf("***inResult***[%d]", inResult);
				
				memcpy(&usTPDU[5], uszReceData, inResult);
				DebugAddHEX("ZZZZ inSendAndReceiveFormComm RCVD data", usTPDU, inResult);

				vdDebug_LogPrintf("###inResult [%d]###", inResult);

				memset(uszReceData,0x00,sizeof(uszReceData));
				memcpy(uszReceData, usTPDU, inResult);
				
			}
			
			#endif
        } 
		else 
		{
			vdDebug_LogPrintf("inSendAndReceiveFormComm FAILED!");
		
            inCTOS_inDisconnect();

            if (srTransPara->byPackType != TC_UPLOAD) {
				vdDebug_LogPrintf("NO RESP.FRM HOST!");
				
                /*
                    CTOS_LCDTClearDisplay();
                    if( srTransPara->byPackType == REVERSAL )
                          CTOS_LCDTPrintXY(1,1,"      REVERSAL");      
                    else
                        vdDispTransTitle(srTransPara->byTransType);
                    CTOS_Sound(1000, 50);
                    CTOS_LCDTPrintXY(1,8,"NO RESP.FRM HOST");

                    CTOS_KBDHit(&key);//clear key buffer
                        key=struiGetchWithTimeOut();
                 */
                vdSetErrorMessage("NO RESP.FRM HOST");
                return ST_ERROR;
            }

        }
    }
    vdDebug_LogPrintf("**inSendAndReceiveFormComm END** %d", inResult);
    //	inSetTextMode(); // patrick temp remark 20140421				
    return inResult;
}

/************************************************************************
Function Name: inCheckIsoHeaderData()
Description:
    Check message id value
Parameters:
    [IN] srTransPara
         szSendISOHeader
         szReceISOHeader

Return: ST_SUCCESS
        ST_ERROR
 ************************************************************************/
int inCheckIsoHeaderData(char *szSendISOHeader, char *szReceISOHeader) {
    int inCnt = 0;
    TRANS_DATA_TABLE *srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

    inCnt += TPDU_BCD_SIZE;

    szSendISOHeader[inCnt + 1] += 0x10;
    if (memcmp(&szSendISOHeader[inCnt], &szReceISOHeader[inCnt], MTI_BCD_SIZE)) {
        if (VS_TRUE == strTCT.fDemo) {
            return ST_SUCCESS;
        }

        vdMyEZLib_LogPrintf("**ISO header data Error**");
        inCTOS_inDisconnect();
        return ST_ERROR;
    }

	/*for MPU Generate MAC buffer*/
	memset(baMPU_RespMTI, 0x00, sizeof(baMPU_RespMTI));
	memcpy(baMPU_RespMTI, &szReceISOHeader[inCnt], MTI_BCD_SIZE);

    return ST_SUCCESS;
}

/************************************************************************
Function Name: inProcessOfflineTrans()
Description:
    Setup and save the file offline transactions need
Parameters:
    [IN] srTransPara
         szSendISOHeader
         szReceISOHeader

Return: ST_SUCCESS
        ST_ERROR
 ************************************************************************/
int inProcessOfflineTrans(TRANS_DATA_TABLE *srTransPara) {
    int inResult;

    srTransPara->byPackType = srTransPara->byTransType;
    //for Y1 offline trans, also need save the SEND_ADVICE 
    //if(!memcmp(srTransRec.szAuthCode, "Y1", 2))// for save trans as Y1 TC UPLOAD format
    //    srTransPara->byPackType = TC_UPLOAD;
    return ST_SUCCESS;
}

/************************************************************************
Function Name: inAnalyseIsoData()
Description:
    Analysis of the host to send back information
Parameters:
    [IN] srTransPara
         
Return: ST_SUCCESS
        ST_ERROR
 ************************************************************************/
int inAnalyseIsoData(TRANS_DATA_TABLE *srTransPara) {
    int inResult;
    ISO_FUNC_TABLE srPackFunc;

    inResult = ST_SUCCESS;

	vdDebug_LogPrintf("inAnalyseIsoData start [%d][%d][%d][%d]", srTransPara->byTransType, 
		srTransPara->byEntryMode, srTransPara->shTransResult, srPackFunc.inTransAnalyse);

    if (srTransPara->byTransType == SALE &&
            srTransPara->byEntryMode == CARD_ENTRY_ICC &&
            srTransPara->shTransResult == TRANS_AUTHORIZED &&
            !memcmp(&srTransPara->szAuthCode[0], "Y1", 2)) {
        return ST_SUCCESS;
    }

    if (srTransPara->byOffline == CN_TRUE) {
        return inResult;
    }

    memset((char *) &srPackFunc, 0x00, sizeof (srPackFunc));
    memcpy((char *) &srPackFunc, (char *) &srIsoFuncTable[0], sizeof (srPackFunc));

    if (srPackFunc.inTransAnalyse != 0x00) {
        vdSetISOEngTransDataAddress(srTransPara);
        inResult = srPackFunc.inTransAnalyse();
    }
    vdDebug_LogPrintf("inAnalyseIsoData[%d]", inResult);

    if (VS_TRUE == strTCT.fDemo)
        CTOS_LCDTPrintXY(1, 8, "APPROVE        ");

    return inResult;
}

void vdClearISO_DataElement(void)
{
	memset(DE2, 0, sizeof(DE2));
    memset(DE3, 0, sizeof(DE3));
    memset(DE4, 0, sizeof(DE4));
	memset(DE11, 0, sizeof(DE11));
	memset(DE12, 0, sizeof(DE12));
	memset(DE14, 0, sizeof(DE14));
	memset(DE18, 0, sizeof(DE18));
	memset(DE32, 0, sizeof(DE32));
	memset(DE41, 0, sizeof(DE41));
	memset(DE43, 0, sizeof(DE43));
	memset(DE48, 0, sizeof(DE48));
	memset(DE49, 0, sizeof(DE49));
	//memset(DE52, 0, sizeof(DE52));
	memset(DE62, 0, sizeof(DE62));
}
/************************************************************************
Function Name: inCheckHostRespCode()
Description:
    Check the host response code
Parameters:
    [IN] srTransPara
         
Return: TRANS_AUTHORIZED
        TRANS_COMM_ERROR
        TRANS_AUTHORIZED
        TRANS_CALL_BANK
        TRANS_CANCELLED
        ST_UNPACK_DATA_ERR
 ************************************************************************/
int inCheckHostRespCode(TRANS_DATA_TABLE *srTransPara) {
    int inResult = TRANS_COMM_ERROR;
	extern BYTE szErrRespMsg[64+1];
	
    vdDebug_LogPrintf("inCheckHostRespCode %s", srTransPara->szRespCode);

    //if ((strncmp(srTransPara->szRespCode, "00" , 2) == 0)||(strncmp(srTransPara->szRespCode, "08" , 2) == 0) ||(strncmp(srTransPara->szRespCode, "88" , 2) == 0)  ||(strncmp(srTransPara->szRespCode, "21" , 2) == 0))
    if (strncmp(srTransPara->szRespCode, "00", 2) == 0) 
	{
        inResult = TRANS_AUTHORIZED;
        srTransPara->shTransResult = TRANS_AUTHORIZED;
        //		strcpy(srTransPara->szRespCode, "00");
        DebugAddSTR("txn approval", srTransPara->szAuthCode, 6);
    } 
	else 
	{

        if ((srTransPara->szRespCode[0] >= '0' && srTransPara->szRespCode[0] <= '9') &&
                (srTransPara->szRespCode[1] >= '0' && srTransPara->szRespCode[1] <= '9')) 
        {
			vdDebug_LogPrintf("inCheckHostRespCode here");
        
            inResult = TRANS_REJECTED;
            if ((srTransRec.byTransType == SETTLE) && (memcmp(srTransPara->szRespCode, "95", 2))) 
			{
                vdDispErrMsg("SETTLE FAILED");
            } 
			else 
            {
                if (memcmp(srTransPara->szRespCode, "95", 2))
                    inCTOS_DisplayResponse();

            }

        }	
		else if (0 == strcmp(srTransPara->szRespCode, "A0"))
		{
			inResult = TRANS_REJECTED;
			//vdDispErrMsg("MAC ERROR");
			vdDisplayErrorMsg(1, 8, "MAC ERROR");
			strcpy(szErrRespMsg, "MAC ERROR");
			/*reset to do auto signon again*/
            if(get_env_int("RESETSIGNON") == 1){
				vdSetMPUSignOnStatus(0);
            }
		}
		else{
			
			vdDebug_LogPrintf("inCheckHostRespCode here2");
			strcpy(szErrRespMsg, "DO NOT ALLOW ATTEMPT"); // //http://118.201.48.210:8080/redmine/issues/1525.54
            inResult = ST_UNPACK_DATA_ERR;
		}
    }

    vdDebug_LogPrintf("inCheckHostRespCode %d"), inResult;

    return (inResult);
}

int inBaseRespValidation(TRANS_DATA_TABLE *srOrgTransPara, TRANS_DATA_TABLE *srTransPara) {
    vdDebug_LogPrintf("inBaseRespValidation ulTraceNum=[%ld][%ld]", srOrgTransPara->ulTraceNum, srTransPara->ulTraceNum);
    if (srOrgTransPara->ulTraceNum != srTransPara->ulTraceNum) {
        vdSetErrorMessage("STAN Not Match");
        return ST_RESP_MATCH_ERR;
    }

    vdDebug_LogPrintf("inBaseRespValidation szTID=[%s][%s]", srOrgTransPara->szTID, srTransPara->szTID);
    if (memcmp(srOrgTransPara->szTID, srTransPara->szTID, TERMINAL_ID_BYTES) != 0) {
        vdSetErrorMessage("TID Not Match");
        return ST_RESP_MATCH_ERR;
    }

    return ST_SUCCESS;
}

/************************************************************************
Function Name: inCheckTransAuthCode()
Description:
    Check the host authorization code
Parameters:
    [IN] srTransPara
         
Return: ST_SUCCESS
        ST_ERROR
      
 ************************************************************************/

int inCheckTransAuthCode(TRANS_DATA_TABLE *srTransPara) {
    int inResult = ST_SUCCESS;

    if (srTransPara->byTransType != SETTLE && srTransPara->byTransType != CLS_BATCH) {
        if (!memcmp(&srTransPara->szAuthCode[0], "000000", 6) ||
                !memcmp(&srTransPara->szAuthCode[0], "      ", 6)) {
//            if (srTransPara->byTransType != VOID) //Synergy host does not return Auth.code for void sale
            if (srTransPara->byTransType != VOID && srTransPara->byTransType == VOID_PREAUTH) //Synergy host does not return Auth.code for void sale
                inResult = ST_ERROR;
        }
    }

    return (inResult);
}

int inAnalyseChipData(TRANS_DATA_TABLE *srTransPara) {
    int inResult;
    ushort inlen = 0;
    unsigned char stScript[512];

    //    vduiClearBelow(8); // patrick fix code 20140421

	vdDebug_LogPrintf("=====inAnalyseChipData=====");
	
    memset(stScript, 0, sizeof (stScript));

    vdDebug_LogPrintf("tag71[%d] tag72[%d]", srTransPara->stEMVinfo.T71Len, srTransPara->stEMVinfo.T72Len);
    if (srTransPara->stEMVinfo.T71Len > 0) {
        memcpy(&stScript[inlen], srTransPara->stEMVinfo.T71, srTransPara->stEMVinfo.T71Len);
        inlen = srTransPara->stEMVinfo.T71Len;
    }
    if (srTransPara->stEMVinfo.T72Len > 0) {
        memcpy(&stScript[inlen], srTransPara->stEMVinfo.T72, srTransPara->stEMVinfo.T72Len);
        inlen = inlen + srTransPara->stEMVinfo.T72Len;
    }
    DebugAddHEX("inAnalyseChipData ", stScript, inlen);
	vdDebug_LogPrintf("byOrgTransType[%d]byPackType[%d]byTransType=[%d],HDTid=[%d],szRespCode=[%s]",srTransPara->byOrgTransType, srTransPara->byPackType,srTransPara->byTransType,srTransPara->HDTid,srTransPara->szRespCode);
	if (srTransPara->byTransType == MPU_PREAUTH_COMP && (srTransPara->HDTid == MPU_CUP_HOST_IDX))// for MPU_PREAUTH_COMP no need check second AC
	{
		if (strncmp(srTransPara->szRespCode, "00", 2) == 0)
			inResult = PP_OK;
		else
			inResult = ST_ERROR;
	}
	else
    inResult = shCTOS_EMVSecondGenAC(stScript, inlen);

    vdDebug_LogPrintf("inAnalyseChipData[%d] srTransPara->shTransResult[%d] srTransPara->byOffline[%d]", inResult, srTransPara->shTransResult, srTransPara->byOffline);

    if (inResult == PP_OK) {
        vdDisplayTxnFinishUI();
        //if(0 != memcmp(srTransRec.szAuthCode, "Y3", 2))
        {
            inMyFile_ReversalDelete();


            if (srTransPara->byOffline == CN_FALSE) {
                if (strHDT.inNumAdv > 0) {
                    inCTLOS_Updatepowrfail(PFR_BEGIN_BATCH_UPDATE);
                    
                    inProcessAdviceTrans(srTransPara, strHDT.inNumAdv);
                }
            }

        }


        inResult = ST_SUCCESS;

    } else {
        if (srTransPara->shTransResult == TRANS_AUTHORIZED || srTransPara->shTransResult == TRANS_COMM_ERROR) {
        } else {

            vdDebug_LogPrintf(". Resp Err");
        }
        inResult = ST_ERROR;

    }

    return inResult;
}

int inAnalyseNonChipData(TRANS_DATA_TABLE *srTransPara) {
    int inResult = ST_SUCCESS;

    vdDebug_LogPrintf("**inAnalyseNonChipData(TxnResult = %d) [%d] byTransType[%d] START**", srTransPara->shTransResult, srTransPara->byPackType, srTransPara->byTransType);

	if (srTransPara->byPackType == SETTLE || srTransPara->byPackType == CLS_BATCH) {
        if (srTransPara->shTransResult != TRANS_AUTHORIZED)
            inResult = ST_ERROR;

    } else if (srTransPara->shTransResult == TRANS_AUTHORIZED) {
        if (VS_TRUE != strTCT.fDemo)
            vdDisplayTxnFinishUI();

        //Should be Online void the Intial SALE amount.
//        if (srTransPara->byTransType == VOID) {
        if (srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) {
            inCTOSS_DeleteAdviceByINV(srTransPara->szInvoiceNo);
        }

        if (srTransPara->byOffline == CN_FALSE) {
            if (strHDT.inNumAdv > 0) {
                inCTLOS_Updatepowrfail(PFR_BEGIN_BATCH_UPDATE);

//                if (srTransPara->byTransType != MPU_PREAUTH_COMP_ADV) { //@@IBR todo: uncomment the condition for piggyback advice
                    inProcessAdviceTrans(srTransPara, strHDT.inNumAdv);
//                }
            }
        }
    } else if (srTransPara->shTransResult == TRANS_CANCELLED) {
        inResult = ST_ERROR;
    } else {
        inResult = ST_ERROR;
    }

    vdDebug_LogPrintf("**inAnalyseNonChipData(%d) END**", inResult);
    return inResult;
}

int inAnalyseReceiveData(void) {
    int inResult;
    TRANS_DATA_TABLE* srTransPara;

	
	vdDebug_LogPrintf("=====inAnalyseReceiveData=====");
	vdDebug_LogPrintf("fGetMPUTrans[%d] fGetMPUCard[%d]", fGetMPUTrans(), fGetMPUCard());

    srTransPara = srGetISOEngTransDataAddress();
	
    if ((srTransPara->byEntryMode == CARD_ENTRY_ICC) && 
            ((srTransPara->byTransType == SALE) || 
             (srTransPara->byTransType == PRE_AUTH) || 
             (srTransPara->byTransType == REFUND) || 
             (srTransPara->byTransType == CASH_ADVANCE) ||
             (srTransPara->byTransType == CUP_SALE) || 
             (VS_TRUE == fGetMPUTrans() && VS_FALSE == fGetMPUCard()))
            )
    {
        inResult = inAnalyseChipData(srTransPara);
    } else {
        inResult = inAnalyseNonChipData(srTransPara);
    }

    return inResult;
}

int inAnalyseAdviceData(int inPackType) {
    TRANS_DATA_TABLE *srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

    if (srTransPara->byTransType == SALE_OFFLINE) {
        srTransPara->byUploaded = CN_FALSE;

    } else if (srTransPara->byTransType == MPU_PREAUTH_COMP_ADV) {
        srTransPara->byUploaded = CN_FALSE;

    } else if (srTransPara->byTransType == SALE_TIP || srTransPara->byTransType == SALE_ADJUST) {
        if (srTransPara->byOffline == CN_TRUE)
            srTransPara->byUploaded = CN_FALSE;
    }

    return ST_SUCCESS;
}

int inPorcessTransUpLoad(TRANS_DATA_TABLE *srTransPara) {
    int inSendCount, inTotalCnt, inFileMaxLen;
    int inResult;
    int inBatchRecordNum = 0;
    int i;
    TRANS_DATA_TABLE srUploadTransRec, srTransParaTmp;
    STRUCT_FILE_SETTING strFile;
    int *pinTransDataid = NULL;
    int *pinTransDataidSend = NULL;
    int inBatchCount = 0;
    BATCH_UP_DATA srBatchUpTmp;
    BOOL fIsSend = FALSE;

    memset(&srBatchUpTmp, 0x00, sizeof (BATCH_UP_DATA));
    memset(&strBatchUp, 0x00, sizeof (BATCH_UP_DATA));

    inResult = ST_SUCCESS;

    vdDebug_LogPrintf("inPorcessTransUpLoad");

    memset(&srUploadTransRec, 0x00, sizeof (TRANS_DATA_TABLE));
    memset(&strFile, 0x00, sizeof (STRUCT_FILE_SETTING));

    inBatchRecordNum = inBatchNumRecord();

    vdDebug_LogPrintf("inPorcessTransUpLoad::BatchUpload totaltxn[%d]", inBatchRecordNum);
    if (inBatchRecordNum > 0) {
        pinTransDataid = (int*) malloc(inBatchRecordNum * sizeof (int));
        pinTransDataidSend = (int*) malloc(inBatchRecordNum * sizeof (int));

        inBatchByMerchandHost(inBatchRecordNum, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);

        inTotalCnt = 0;
        for (i = 0; i < inBatchRecordNum; i++) {
            inDatabase_BatchReadByTransId(&srUploadTransRec, pinTransDataid[i]);

            vdDebug_LogPrintf("BatchUpload curren[%d] Void?[%d]", pinTransDataid[i], srUploadTransRec.byVoided);
            //            if((srUploadTransRec.byVoided != TRUE) && (srUploadTransRec.byTransType != PRE_AUTH))
            if ((srUploadTransRec.byVoided != TRUE) && ((srUploadTransRec.byTransType != PRE_AUTH) ||
                    (srUploadTransRec.byTransType != MPU_PREAUTH))) {
                pinTransDataidSend[inTotalCnt] = pinTransDataid[i];
                inTotalCnt++;
            }
        }

        //in case all is void
        inResult = ST_SUCCESS;
        inFinalSend = CN_TRUE;

        vdDebug_LogPrintf("BatchUpload total None void txn[%d]", inTotalCnt);
        for (inSendCount = 0; inSendCount < inTotalCnt; inSendCount++)
            //        for(inSendCount = 0; inSendCount < inTotalCnt; inSendCount += 8)
        {
            if (((inSendCount + 1) == inTotalCnt))
                inFinalSend = CN_TRUE;
            else
                inFinalSend = CN_FALSE;

            vdDebug_LogPrintf("Before HDTid[%d]MITid[%d]AMT[%02X%02X%02X%02X%02X%02X]", srTransPara->HDTid, srTransPara->MITid, srTransPara->szTotalAmount[0]
                    , srTransPara->szTotalAmount[1]
                    , srTransPara->szTotalAmount[2]
                    , srTransPara->szTotalAmount[3]
                    , srTransPara->szTotalAmount[4]
                    , srTransPara->szTotalAmount[5]);
            inDatabase_BatchReadByTransId(&srUploadTransRec, pinTransDataidSend[inSendCount]);

            vdDebug_LogPrintf("After HDTid[%d]MITid[%d]AMT[%02X%02X%02X%02X%02X%02X]", srUploadTransRec.HDTid, srUploadTransRec.MITid, srUploadTransRec.szTotalAmount[0]
                    , srUploadTransRec.szTotalAmount[1]
                    , srUploadTransRec.szTotalAmount[2]
                    , srUploadTransRec.szTotalAmount[3]
                    , srUploadTransRec.szTotalAmount[4]
                    , srUploadTransRec.szTotalAmount[5]);

            //            if(srUploadTransRec.byTransType == PRE_AUTH)
            if (srUploadTransRec.byTransType == PRE_AUTH || srUploadTransRec.byTransType == MPU_PREAUTH) {
                continue;
            }
            //OFFLINE SALE, then VOID it, the VOID is offline, should not batch upload 
            if (srUploadTransRec.byTransType == VOID && srUploadTransRec.byOffline == CN_TRUE && srUploadTransRec.byUploaded == CN_FALSE) {
                continue;
            }

            srUploadTransRec.byPackType = BATCH_UPLOAD;
            if(srUploadTransRec.fAlipay == TRUE)
            {
				if(srUploadTransRec.byTransType == ALIPAY_SALE)
                    srUploadTransRec.byPackType=ALIPAY_SALE_BATCH_UPLOAD;				
            }

			vdDebug_LogPrintf("srUploadTransRec.fAlipay: %d", srUploadTransRec.fAlipay);
			vdDebug_LogPrintf("srUploadTransRec.byTransType: %d", srUploadTransRec.byTransType);
			vdDebug_LogPrintf("srUploadTransRec.byPackType: %d", srUploadTransRec.byPackType);
			
            vdDebug_LogPrintf(". Bef Add szTraceNo = %02x%02x%02x", strHDT.szTraceNo[0],
                    strHDT.szTraceNo[1],
                    strHDT.szTraceNo[2]);

            inMyFile_HDTTraceNoAdd(srUploadTransRec.HDTid);

            vdDebug_LogPrintf(". Aft Add szTraceNo = %02x%02x%02x", strHDT.szTraceNo[0],
                    strHDT.szTraceNo[1],
                    strHDT.szTraceNo[2]);

            srUploadTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
            vdDebug_LogPrintf(". UploadSendTracNum(%d) [%s]", srUploadTransRec.ulTraceNum, srUploadTransRec.szTID);
            memcpy(&srTransParaTmp, &srUploadTransRec, sizeof (TRANS_DATA_TABLE));

            memcpy(srBatchUpTmp.stBatchDetail[inBatchCount].szCardType, "00", 2);
            memcpy(srBatchUpTmp.stBatchDetail[inBatchCount].szAmount, srTransParaTmp.szTotalAmount, AMT_BCD_SIZE);
            memcpy(srBatchUpTmp.stBatchDetail[inBatchCount].szPAN, srTransParaTmp.szPAN, strlen(srTransParaTmp.szPAN));
            srBatchUpTmp.stBatchDetail[inBatchCount].ulTraceNum = srTransParaTmp.ulTraceNum;

            DebugAddHEX("szCardType", srBatchUpTmp.stBatchDetail[inBatchCount].szCardType, 2);
            DebugAddHEX("szAmount", srBatchUpTmp.stBatchDetail[inBatchCount].szAmount, AMT_BCD_SIZE);
            DebugAddHEX("szPAN", srBatchUpTmp.stBatchDetail[inBatchCount].szPAN, strlen(srTransParaTmp.szPAN));
            vdDebug_LogPrintf("ultrace num %d", srBatchUpTmp.stBatchDetail[inBatchCount].ulTraceNum);

//            inBatchCount++;

//            vdDebug_LogPrintf("inBatchCount++ = %d", inBatchCount);


            //                        if(((inSendCount % 8) != 0) && (inFinalSend == CN_FALSE) && inSendCount == 0){
            //                            continue;
            //                        }

            //                        if(((inSendCount == 0) || ((inSendCount % 8) != 0)) || ((inSendCount % 8) != 0) && (inFinalSend == CN_FALSE)){
            //                            continue;
            //                        }
//            if (inFinalSend == CN_FALSE) {
//                if (((inSendCount + 1) % 8) != 0) {
//                    continue;
//                }
//            }
//
//
//            srBatchUpTmp.inBatchCount = inBatchCount;
//
//            memcpy(&strBatchUp, &srBatchUpTmp, sizeof (BATCH_UP_DATA));
//            fIsSend = TRUE;



            if ((inResult = inPackSendAndUnPackData(&srUploadTransRec, srUploadTransRec.byPackType) != ST_SUCCESS)) {
                vdDebug_LogPrintf(". inPorcessTransUpLoad(%d)_Err", inResult);
                vdDebug_LogPrintf(". byTransType %d", srUploadTransRec.byTransType);
                inResult = ST_UNPACK_DATA_ERR;
                break;
            } else {
                if (memcmp(srUploadTransRec.szRespCode, "00", 2)) {
                    vdDebug_LogPrintf(". inPorcessTransUpLoad(%s) BatchUpload Fail", srUploadTransRec.szRespCode);
                    inResult = ST_ERROR;
                    break;
                } else {
                    if (ST_SUCCESS != inBaseRespValidation(&srTransParaTmp, &srUploadTransRec)) {
                        inResult = ST_UNPACK_DATA_ERR;
                        break;
                    }
                }

                vdDebug_LogPrintf(". inPorcessTransUpLoad(%d)BatchUpload OK", inResult);
            }

//            memset(&strBatchUp, 0x00, sizeof (BATCH_UP_DATA));
//            memset(&srBatchUpTmp, 0x00, sizeof (BATCH_UP_DATA));
//
//            inBatchCount = 0;
        }

        free(pinTransDataid);
        free(pinTransDataidSend);

    } else {
        //inResult = ST_ERROR;//thandar_comment off to fix void pre auth settlment issue
        inResult = ST_SUCCESS;
		inFinalSend=CN_TRUE;
        
        vdDebug_LogPrintf("No batch record found");
    }

    inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
    srUploadTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
    vdMyEZLib_LogPrintf(". Aft Upload TraceNum(%d)", srUploadTransRec.ulTraceNum);

    if (inResult == ST_SUCCESS && inFinalSend == CN_TRUE) {
        srTransPara->byPackType = CLS_BATCH;
        srTransPara->ulTraceNum = srUploadTransRec.ulTraceNum;
        if ((inResult = inPackSendAndUnPackData(srTransPara, srTransPara->byPackType) != ST_SUCCESS)) {
            vdMyEZLib_LogPrintf(". FinalSettle(%d)_Err", inResult);
            vdMyEZLib_LogPrintf(". byTransType %d", srTransPara->byTransType);
            inResult = ST_UNPACK_DATA_ERR;
        }


        /* BDO: Check settlement reconcillation response code - start -- jzg */
        if (memcmp(srTransPara->szRespCode, "00", 2) != 0) {
            vdDebug_LogPrintf("JEFF::ST_CLS_BATCH_ERR");
            inResult = ST_CLS_BATCH_ERR;
        }
        /* BDO: Check settlement reconcillation response code - end -- jzg */

        inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
        srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
        vdMyEZLib_LogPrintf(". Aft CLS_BATCH TraceNum(%d)", srTransPara->ulTraceNum);
    }

    return inResult;
}

int inPackIsoFunc02(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    int inPANLen;
    char szTempPan[PAN_SIZE + 1];
    char szTemp[25 + 1];

    vdDebug_LogPrintf("**inPackIsoFunc02 START**");

    inDataCnt = 0;
    inPANLen = 0;

    memset(szTempPan, 0x00, sizeof (szTempPan));
    inPANLen = strlen(srTransPara->szPAN);
    memcpy(szTempPan, srTransPara->szPAN, inPANLen);

    uszSendData[inDataCnt++] = (inPANLen / 10 * 16) + (inPANLen % 10);

    //    //@@IBR ADD 20161031
    //    if(srTransPara->byTransType == SALE) {
    //        memset(szTemp, 0x00, sizeof (szTemp));
    //        sprintf(szTemp, "%d%s", (inPANLen / 10 * 16) + (inPANLen % 10), srTransPara->szPAN);
    //        strcat(&szMacData[inMacDataCnt], szTemp);                               inMacDataCnt += strlen(szTemp);
    //        strcat(szMacData, " ");                                                 inMacDataCnt += 1;
    //    }


    if (inPANLen % 2)
        szTempPan[inPANLen++] = '0';

#ifdef TLE
    memset(&uszSendData[inDataCnt], 0x00, (inPANLen + 1) / 2);
    byField_02_ON = TRUE;
#else
    wub_str_2_hex(szTempPan, (char *) &uszSendData[inDataCnt], inPANLen);
#endif    
    inDataCnt += (inPANLen / 2);

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
		memset(DE2, 0, sizeof(DE2));
		sprintf(DE2, "%02d", inPANLen);
		memcpy(&DE2[2], srTransPara->szPAN, inPANLen);
    }
	#endif
	
    vdDebug_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdDebug_LogPrintf("**inPackIsoFunc02 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field02", uszSendData, inDataCnt, 1);

    return (inDataCnt);

}

int inPackIsoFunc03(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    inDataCnt = 0;

    vdDebug_LogPrintf("**inPackIsoFunc03 START**inFinalSend [%d]srTransPara->HDTid[%d]", inFinalSend, srTransPara->HDTid);

    DebugAddHEX("srTransPara->szIsoField03", &srTransPara->szIsoField03[0], PRO_CODE_BCD_SIZE);

    if (srTransPara->fIsInstallment == TRUE && srTransPara->byTransType == SALE) {
        memcpy(srTransPara->szIsoField03, "\x51\x00\x00", 3);
    }
    
    if(srTransPara->byTransType == VOID && fGetCashAdvAppFlag() == TRUE){
        memcpy(srTransPara->szIsoField03, "\x02\x00\x00", 3);
    }
	
    memcpy(&uszSendData[inDataCnt], &srTransPara->szIsoField03[0], PRO_CODE_BCD_SIZE);
    inDataCnt += PRO_CODE_BCD_SIZE;

    if ((BATCH_UPLOAD == srTransPara->byPackType)) {
        //fix Tip adjust Sale , batch upload process code is 02000x, should be 00000x
        uszSendData[0] &= 0xF0;

	//http://118.201.48.214:8080/issues/75.56 #1 [1.DE03 of the batch upload refund simple has to follow the original 0200 transactions'.]
	#ifdef CBB_FIN_ROUTING
        if (inFinalSend != CN_TRUE)
	{
		//applicable for MPU host only
		if(srTransPara->HDTid == 17)
          		  uszSendData[2] |= 0x01;
        }
	#else	
	if (inFinalSend != CN_TRUE)
		uszSendData[2] |= 0x01;
	#endif
    }

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(DE3, 0, sizeof(DE3));
        wub_hex_2_str(uszSendData, DE3, inDataCnt);
    }
    #endif
	
//    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
//    vdMyEZLib_LogPrintf("**inPackIsoFunc03 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field03", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc04(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    inDataCnt = 0;
    char szAmtAsc[12 + 1];

    //Should be Online void the Intial SALE amount.
//    if (srTransPara->byTransType == VOID) {
    if (srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) {
        //use szStoreID to store how much amount fill up in DE4 for VOID
        if(srTransPara->fIsInstallment == TRUE){
            memcpy((char *) &uszSendData[inDataCnt], srTransPara->szBaseAmount, 6);
        }else
            memcpy((char *) &uszSendData[inDataCnt], srTransPara->szStoreID, 6);
    } else
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szTotalAmount, 6);

#ifdef ECR_PREAUTH_AND_COMP
    if(srTransPara->byPackType == TRANS_ENQUIRY)
		memcpy((char *) &uszSendData[inDataCnt], "\x00\x00\x00\x00\x00\x00", 6);
#endif
	
    inDataCnt += 6;

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(DE4, 0, sizeof(DE4));
        wub_hex_2_str(uszSendData, DE4, inDataCnt);
    }
    #endif

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field04", uszSendData, inDataCnt, 1);
	
    return (inDataCnt);
}



//Reversal, Sale, run in diffrent time, the DE07 not same, Wrong
//you can only get one time!!!
BYTE baTransDT[5];
void vdPackISO_SetDateTimeForTrans(TRANS_DATA_TABLE *srTransPara) 
{
    CTOS_RTC SetRTC;
    char szTemp[4 + 1] = "";
    char szTemp1[6 + 1] = "";
	BYTE byDTPos = 0;

    vdDebug_LogPrintf("**vdPackISO_GetDateTimeForTrans START**");

    memset(szTemp, 0x00, sizeof (szTemp));
    memset(szTemp1, 0x00, sizeof (szTemp1));
	memset(baTransDT, 0x00, sizeof (baTransDT));

    CTOS_RTCGet(&SetRTC);

	sprintf(szTemp, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);
    wub_str_2_hex(szTemp, szTemp1, 4);
//    memcpy(srTransPara->szDate, szTemp1, DATE_BCD_SIZE);
    memcpy((char *)&baTransDT[byDTPos], szTemp1, 2);
    byDTPos += 2;


    memset(szTemp, 0x00, sizeof (szTemp));
    memset(szTemp1, 0x00, sizeof (szTemp1));
    sprintf(szTemp, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
    wub_str_2_hex(szTemp, szTemp1, 6);
//    memcpy(srTransPara->szTime, szTemp1, TIME_BCD_SIZE);
     memcpy((char *)&baTransDT[byDTPos], szTemp1, 3);
    byDTPos += 3;

	DebugAddHEX("baTransDT", baTransDT, 5);

}


//@@IBR ADD 20161019

int inPackIsoFunc07(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {

    CTOS_RTC SetRTC;
    char szTemp[4 + 1] = "";
    char szTemp1[6 + 1] = "";

    vdDebug_LogPrintf("**inPackIsoFunc07 START**");


    inDataCnt = 0;
    memset(szTemp, 0x00, sizeof (szTemp));
    memset(szTemp1, 0x00, sizeof (szTemp1));

    memcpy((char *) &uszSendData[inDataCnt], baTransDT, 5);
    inDataCnt = 5;

    /*MPU void use the orignal date&time, but pre-auth void no need check batch,
    use current*/
    if ((srTransPara->byTransType == VOID))
    //if ((srTransPara->byTransType == VOID || 
		//srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE == fGetMPUTrans()) 
	{
        inDataCnt = 0;

        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szOrgDate, 2);    inDataCnt += 2;
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szOrgTime, 3);    inDataCnt += 3;
    }

    /*MPU MAC store DE07 first*/
    memcpy(baMPU_DE07, uszSendData, 5);
    vdDebug_LogPrintf("**inPackIsoFunc07 FINISH**");

    return (inDataCnt);
}

int inPackIsoFunc11(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szSTAN[6 + 1];

    inDataCnt = 0;
    memset(szSTAN, 0x00, sizeof (szSTAN));


    if (srTransPara->byPackType == SETTLE)// 20121204
    {
        sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
        wub_str_2_hex(&szSTAN[0], (char *) &uszSendData[inDataCnt], 6);
        vdDebug_LogPrintf("sys trace %d", uszSendData[0], uszSendData[1], uszSendData[2]);
    } else {
		//case #2020 - Duplicate Stan Transaction in NFC Payment
		#if 0
        if ((srTransPara->byPackType == TC_UPLOAD)/* || srTransPara->byTransType == CLS_BATCH*/) {
            vdDebug_LogPrintf("**inPackIsoFunc11 %d**", srTransPara->ulTraceNum);
            sprintf(szSTAN, "%06ld", (srTransPara->ulTraceNum + 1));
        } else
            sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
		#else
		sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
		#endif

        wub_str_2_hex(&szSTAN[0], (char *) &uszSendData[inDataCnt], 6);
    }

    inDataCnt += 3;

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(DE11, 0, sizeof(DE11));
        wub_hex_2_str(uszSendData, DE11, inDataCnt);
    }
    #endif
	
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field11", uszSendData, inDataCnt, 1);
    vdDebug_LogPrintf("  PACK_LEN[%d] %d [%02X%02X%02X] inFinalSend[%d]byPackType[%d]", inDataCnt, srTransPara->byPackType, uszSendData[0], uszSendData[1], uszSendData[2], inFinalSend, srTransPara->byPackType);
    return (inDataCnt);
}

int inPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szTempTime[6];
    CTOS_RTC SetRTC;
    char szTemp[6 + 1];
    BYTE szTemp1[3 + 1];
    char szYear[2+1];

	#if 0
	int inGetNEWPACFlow = 0;
	inGetNEWPACFlow = get_env_int("#PREAUTHRRNDT");
	#endif
	
	vdDebug_LogPrintf("**inPackIsoFunc12 START**");
	//vdDebug_LogPrintf("inPackIsoFunc12: %d, %d, %d %d", srTransPara->byTransType, srTransPara->byOrgTransType, fGetMPUTrans(), inGetNEWPACFlow);
	DebugAddHEX("inPackIsoFunc12 srTransRec.szOrgTime", srTransRec.szOrgTime, 3);
	vdDebug_LogPrintf("inPackIsoFunc12(%s)", srTransPara->szRRN);

	

    inDataCnt = 0;

    memset(&SetRTC, 0x00, sizeof (CTOS_RTC));

    if (srTransPara->byPackType == SALE_TIP ||
		srTransPara->byPackType == SALE_ADJUST ||
		srTransPara->byPackType == SALE_TIP ||
		srTransPara->byTransType == REFUND ||				// FOR new host application 							
		((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE == fGetMPUTrans()))
        //(srTransPara->byTransType == VOID && VS_TRUE == fGetMPUTrans()) )
    {
		vdDebug_LogPrintf("**inPackIsoFunc12-A**");
    
        memcpy((char *)&uszSendData[inDataCnt], srTransPara->szOrgTime, 3);
    } 
	else if (srTransPara->byTransType == SALE ||
            srTransPara->byTransType == PRE_AUTH ||
            srTransPara->byTransType == MPU_SETTLE ||
            srTransPara->byTransType == MPU_PREAUTH ||
            srTransPara->byTransType == MPU_VOID_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
            (srTransPara->byTransType == VOID && VS_TRUE != fGetMPUTrans()) ||
            (srTransPara->byTransType == SETTLE && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19))
            )
    {

		vdDebug_LogPrintf("**inPackIsoFunc12-A1**");

		// MPU_UPI 6244 1901 should now be routed to CB host 1st before FINexus
		#if 1
		if((fChkCUPCard() == TRUE) && (srTransPara->byTransType == VOID) && (srTransPara->byOrgTransType == SALE) && 
			(srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19)){			
			vdDebug_LogPrintf("**inPackIsoFunc12-B**");
			memcpy((char *)&uszSendData[inDataCnt], srTransPara->szOrgTime, 3);
		
		}
    	else
		{
			vdDebug_LogPrintf("**inPackIsoFunc12-C**");		
	        memcpy((char *) &uszSendData[inDataCnt], &baTransDT[2], 3);
    	}
		#else
			vdDebug_LogPrintf("**inPackIsoFunc12-C**"); 	
			memcpy((char *) &uszSendData[inDataCnt], &baTransDT[2], 3);
		
		#endif
    } 


	#ifdef TOPUP_RELOAD
	else if(srTransPara->HDTid == 20)
	{
		//YY MM DD hh mm ss
        CTOS_RTCGet(&SetRTC);
        memset(szYear, 0, sizeof(szYear));
        sprintf(szYear ,"%02d",SetRTC.bYear);
        wub_str_2_hex(szYear, &uszSendData[inDataCnt], 2); inDataCnt+=1;
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szDate, 2); inDataCnt+=2;
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szTime, 3);
        
        DebugAddHEX("T/R srTransPara->szTime", srTransPara->szTime, 3);
		
		if(srTransPara->byPackType == TOPUP_RELOAD_REVERSAL)
		{
			memset(szYYDateTime, 0, sizeof(szYYDateTime));
			memcpy(szYYDateTime, uszSendData, 6);
		}
		else if(srTransPara->byPackType == TOPUP || srTransPara->byPackType == RELOAD)
		{
			memcpy(uszSendData, szYYDateTime, 6);
		}
	}
	#endif
	else
	{
	
		vdDebug_LogPrintf("**inPackIsoFunc12-D**"); 	
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szTime, 3);
		
		DebugAddHEX("inPackIsoFunc12-D1", &uszSendData[0], 3);
	}


	

	#ifdef CB_MPU_NH_MIGRATION
	/*if MPU and Credit host combine togther*/
	if (1 == fGetMPUTrans())
	{
		vdDebug_LogPrintf("DE12 MPU and Credit host combine checking [%d][%d]", srTransPara->byTransType, srTransPara->byOrgTransType);

		
		if (srTransPara->byTransType == VOID && PRE_AUTH == srTransPara->byOrgTransType)	
			memcpy((char *) &uszSendData[inDataCnt], srTransPara->szTime, 3); //POSIC023 - F12 in the pre-auth cancel request message need to be same as pre-auth message. #2
			//memcpy((char *) &uszSendData[inDataCnt], &baTransDT[2], 3);

		// Fix for PreAuthCompletion VOID/ cancellation.  Send Original de12 (time) of Preauth Completion. - based "POS function and  Regulation" doc
		if (srTransPara->byTransType == VOID && PREAUTH_COMP == srTransPara->byOrgTransType)
		{
			memset(uszSendData, 0x00, sizeof(uszSendData));
			vdDebug_LogPrintf("WWWWWWWWWWWWWWWWWWWW");
			//memcpy((char *) &uszSendData[inDataCnt], &baTransDT[2], 3);			
			memcpy((char *)&uszSendData[inDataCnt], srTransPara->szOrgTime, 3);
			//memcpy((char *)&uszSendData[inDataCnt], srTransRec.szOrgTime, 3);
		}

		#if 0 //get current date and time instead for PAC
		// for PreAuthComp CR flow - //http://118.201.48.210:8080/redmine/issues/1525.99 and 100.
		if(inGetNEWPACFlow != 1)
		{
		
			vdDebug_LogPrintf("**inPackIsoFunc12-D2**"); 	
			
			if(srTransPara->byTransType == PREAUTH_COMP)
			{						
				vdDebug_LogPrintf("**inPackIsoFunc12-E**"); 	
				memset(uszSendData,0x00,sizeof(uszSendData));
	        	memcpy((char *)&uszSendData[inDataCnt], srTransPara->szOrgTime, 3);
				
				//memcpy((char *) &uszSendData[inDataCnt], &baTransDT[2], 3);
				
				DebugAddHEX("inPackIsoFunc12-E1", &uszSendData[0], 3);
			}
		}
		#endif
		
	}
	#endif
	
    inDataCnt += 3;

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
		DebugAddHEX("T/R DE12 uszSendData", &uszSendData[0], 6);
        memset(DE12, 0, sizeof(DE12));
        wub_hex_2_str(uszSendData, DE12, inDataCnt);
    }
    #endif

    memcpy(srTransPara->szTime, &uszSendData[0], TIME_BCD_SIZE);

	DebugAddHEX("inPackIsoFunc12 FINAL T I M E: ", srTransPara->szTime, 3);

//    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
//    vdMyEZLib_LogPrintf("**inPackIsoFunc12 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field12", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szTempDate[6];
    CTOS_RTC SetRTC;
    char szTemp[4 + 1];
    BYTE szTemp1[2 + 1];

	#if 0
	int inGetNEWPACFlow = 0;
	inGetNEWPACFlow = get_env_int("#PREAUTHRRNDT");
	#endif
	
    inDataCnt = 0;

    memset(&SetRTC, 0x00, sizeof (CTOS_RTC));


	vdDebug_LogPrintf("inPackIsoFunc13: %d, %d, %d", srTransPara->byTransType, srTransPara->byOrgTransType, fGetMPUTrans());
	DebugAddHEX("inPackIsoFunc13 srTransRec.szDate", srTransPara->szDate, 2);
	DebugAddHEX("inPackIsoFunc13 srTransRec.szOrgDate", srTransPara->szOrgDate, 2);

    if (srTransPara->byPackType == SALE_TIP
            || srTransPara->byPackType == SALE_ADJUST
            || srTransPara->byPackType == SALE_TIP
            || srTransPara->byPackType == CUP_VOID_PREAUTH            
			|| srTransPara->byTransType == REFUND 	  // FOR new host application			 			             
            || ((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE == fGetMPUTrans())
            //		|| (srTransPara->byTransType == VOID && VS_TRUE == fGetMPUTrans())
            ) 
    {
    
		vdDebug_LogPrintf("inPackIsoFunc13 - AAAA");	
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szOrgDate, 2);
    } 
	else if (srTransPara->byTransType == SALE ||
            srTransPara->byTransType == PRE_AUTH ||
            srTransPara->byTransType == MPU_SETTLE ||
            srTransPara->byTransType == MPU_PREAUTH ||
            srTransPara->byTransType == MPU_VOID_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
            ((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE != fGetMPUTrans()) ||
            (srTransPara->byTransType == SETTLE && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19))) 
    {

		vdDebug_LogPrintf("inPackIsoFunc13 - BBBB");	
        memcpy((char *) &uszSendData[inDataCnt], &baTransDT[0], 2);

    } 
	else
	{
		
		vdDebug_LogPrintf("inPackIsoFunc13 - CCCC");	
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szDate, 2);
	}

	/*if MPU and Credit host combine togther*/
	#if 0 // for testing 02/17/2022
	if (1 == fGetMPUTrans())
	{
		vdDebug_LogPrintf("DE13 MPU and Credit host combine checking");
		if (srTransPara->byTransType == VOID && PRE_AUTH == srTransPara->byOrgTransType)
			memcpy((char *) &uszSendData[inDataCnt], &baTransDT[0], 2);

		if (srTransPara->byTransType == VOID && PREAUTH_COMP == srTransPara->byOrgTransType)
			memcpy((char *) &uszSendData[inDataCnt], &baTransDT[0], 2);

		#if 0 //get current date and time instead for PAC
		if(inGetNEWPACFlow != 1)
		{
		
			vdDebug_LogPrintf("inPackIsoFunc13 - DDDD");	
			if(srTransPara->byTransType == PREAUTH_COMP)
			{			
			
				vdDebug_LogPrintf("inPackIsoFunc13 - EEEE");	
				memset(uszSendData,0x00,sizeof(uszSendData));
				memcpy((char *) &uszSendData[inDataCnt], &baTransDT[0], 2);
				//memcpy((char *) &uszSendData[inDataCnt], srTransPara->szOrgDate, 2);
			}
		}
		#endif
	}
	#endif
	
    inDataCnt += 2;

    memcpy(srTransPara->szDate, &uszSendData[0], DATE_BCD_SIZE);
    //    DebugAddHEX("srTransPara->szDate", srTransPara->szDate, DATE_BCD_SIZE);

    DebugAddHEX("DE13 uszSendData", &uszSendData[0], 2);

    //    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    //    vdMyEZLib_LogPrintf("**inPackIsoFunc13 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field13", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc14(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szTempExpDate[6];
    vdMyEZLib_LogPrintf("**inPackIsoFunc14 START**");
    inDataCnt = 0;

#ifdef TLE 
    memcpy((char *) &uszSendData[inDataCnt], "\x00\x00", 2);
    byField_14_ON = TRUE;
#else
    memcpy((char *) &uszSendData[inDataCnt], srTransPara->szExpireDate, 2);
#endif
    inDataCnt += 2;

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(DE14, 0, sizeof(DE14));
        wub_hex_2_str(uszSendData, DE14, inDataCnt);
    }
    #endif


    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc14 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field14", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc15(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szTempDate[6];
    CTOS_RTC SetRTC;
    char szTemp[4 + 1];
    BYTE szTemp1[2 + 1];

    vdMyEZLib_LogPrintf("**inPackIsoFunc15 START**");
    inDataCnt = 0;

    memset(&SetRTC, 0x00, sizeof (CTOS_RTC));

    if (srTransPara->byPackType == SALE_TIP
            || srTransPara->byPackType == SALE_ADJUST
            || srTransPara->byPackType == SALE_TIP
            || srTransPara->byPackType == CUP_VOID_PREAUTH
            || ((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE == fGetMPUTrans())
            //		|| (srTransPara->byTransType == VOID && VS_TRUE == fGetMPUTrans())
            ) {
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szOrgDate, 2);
    } else if (srTransPara->byTransType == SALE ||
            srTransPara->byTransType == PRE_AUTH ||
            srTransPara->byTransType == MPU_SETTLE ||
            srTransPara->byTransType == MPU_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH_COMP ||
            ((srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) && VS_TRUE != fGetMPUTrans()) ||
            (srTransPara->byTransType == SETTLE && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19))
            //            (srTransPara->byTransType == VOID && VS_TRUE != fGetMPUTrans())
            ) {
        memcpy((char *) &uszSendData[inDataCnt], &baTransDT[0], 2);
    } else
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szDate, 2);
    inDataCnt += 2;

    DebugAddHEX("DE13 uszSendData", &uszSendData[0], 2);

    //    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    //    vdMyEZLib_LogPrintf("**inPackIsoFunc13 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field13", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}


int inPackIsoFunc18(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    //    vdMyEZLib_LogPrintf("**inPackIsoFunc18 START**");
    inDataCnt = 0;
    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
	    memcpy((char *) &uszSendData[inDataCnt], "\x60\x12", 2);
	else
	#endif
        memcpy((char *) &uszSendData[inDataCnt], "\x60\x11", 2);
    inDataCnt += 2;

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(DE18, 0, sizeof(DE18));
        wub_hex_2_str(uszSendData, DE18, inDataCnt);
    }
    #endif

    //    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    //    vdMyEZLib_LogPrintf("**inPackIsoFunc18 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field18", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}


int inPackIsoFunc22(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szTempEnterMode[5];
    BYTE byPINEntryCap = 0;
    char szTemp[4 + 1];

    vdDebug_LogPrintf("**inPackIsoFunc22 START**");

    inDataCnt = 0;

    vdDebug_LogPrintf("srTransPara->byEntryMode[%d]: srTransPara->bWaveSID[%d]", srTransPara->byEntryMode, srTransPara->bWaveSID);
    vdDebug_LogPrintf("srTransPara->byPINEntryCapability[%d]", srTransPara->byPINEntryCapability);

	vdDebug_LogPrintf("srTransPara->byTransType:[%d]byOrgTransType:[%d]fGetMPUTrans:[%d]fGetMPUCard:[%d]fChkCUPCard[%d]:IITid[%d]", 
		srTransPara->byTransType, srTransPara->byOrgTransType, fGetMPUTrans(), fGetMPUCard(), fChkCUPCard(), srTransPara->IITid);

    byPINEntryCap = srTransPara->byPINEntryCapability;

    memset(szTempEnterMode, 0, sizeof (szTempEnterMode));


    /*MPU Cert 20170207 
    3.TIT-PURWIT-001
    In void request message, "field 22" value must be "052" instead of "051". 
    4.TIT-PURWIT-002
In void request message, "field 22" value must be "022" instead of "051". 

    1.TIT-PAUREQ-006
    In Pre-Auth complete cancel request message, "field 22" value must be "052" instead of "051". 

    2.TIT-PAUWIT-001
    In Pre-Auth Cancel request message, "field 22" value must be "052" instead of "051".

    4.TIT-PAUREQ-007
    In Pre-Auth complete request message, "field 22" value must be "022" instead of "021".

    1.TIT-PAUREQ-006
    In Pre-Auth complete cancel request message, "field 22" value must be "052" instead of "051". 

    2.TIT-PAUWIT-001
    In Pre-Auth Cancel request message, "field 22" value must be "052" instead of "051".

     */
    if (VS_TRUE == fGetMPUTrans() &&
            VS_TRUE == fChkCUPCard() &&
            (VOID == srTransPara->byTransType ||
            VOID_PREAUTH == srTransPara->byTransType ||
            MPU_VOID_PREAUTH == srTransPara->byTransType ||
            MPU_PREAUTH_COMP == srTransPara->byTransType ||
            MPU_VOID_PREAUTH_COMP == srTransPara->byTransType)
            ) 
       {
        byPINEntryCap = 2;

        vdDebug_LogPrintf("srTransPara->byTransType[%d]", srTransPara->byTransType);
        DebugAddHEX("srTransPara->szPINBlock", srTransPara->szPINBlock, 8);
        if (MPU_PREAUTH_COMP == srTransPara->byTransType) {
            if (memcmp(srTransPara->szPINBlock, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0) //@@IBR ADD 20161109
                byPINEntryCap = 0;
        }
    }


	vdDebug_LogPrintf("byPINEntryCap[%d]srTransPara->byTransType[%d]srTransPara->CDTid[%d]", 
		byPINEntryCap, srTransPara->byTransType, srTransPara->CDTid);

    /*MPU Chip card not EMV, no EMV process, so we set Entry mode as CARD_ENTRY_EASY_ICC.
    for MPU credit card, need use 00xx and 90xx, but for MPU stand alone app(MPU debit)
    still use 00 51*/
    if (srTransPara->byEntryMode == CARD_ENTRY_ICC || srTransPara->byEntryMode == CARD_ENTRY_EASY_ICC) {

		vdDebug_LogPrintf("**inPackIsoFunc22 HERE byEntryMode is equal to 3 or 6**");
		// for cacse 2028 - Fallback scenario DE22 value for UPI and JCB on MPU new switch #1
        //byPINEntryCap = 1;

         if(srTransPara->CDTid == 47 ||
            srTransPara->CDTid == 53 ||
            srTransPara->CDTid == 55 ||
            srTransPara->CDTid == 59 ||
            srTransPara->CDTid == 60 ||
            srTransPara->CDTid == 61 ||
            srTransPara->CDTid == 62 ||
            srTransPara->CDTid == 18 || /*new added by albert 20190516*/
            srTransPara->CDTid == 65 ||
            srTransPara->CDTid == 19 ||
            srTransPara->CDTid == 46 ||
            srTransPara->CDTid == 66 ||
            srTransPara->CDTid == 91)  //624419030000 - pure UPI for case #1861 - ENHANCEMENT_1861
        {

			//for case #1986 - UPI Floor Limit Parameter and CVM required limit (DE 22 value with chip entering pin is 50 in bin range 6234 3300 0000. 
			//It should be 51 because entering pin is required. (please refer attached Log))
        	if(srTransPara->byPINEntryCapability == 1) 
            	byPINEntryCap = 1;				
			else				
            	byPINEntryCap = 0;

            if (srTransPara->fIsInstallment == TRUE)
                byPINEntryCap = 1;

        }

		#ifdef PIN_CHANGE_ENABLE		
		if(srTransPara->byPackType == CHANGE_PIN)
		{
			if(memcmp(srTransPara->szVerifyNewPINBlock, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) != 0)
		    	byPINEntryCap = 1;
		}
		#endif

        sprintf(szTempEnterMode, "005%d", byPINEntryCap);


		#if 0
        if (VS_FALSE == fGetMPUTrans() && VS_TRUE == fGetMPUCard()) 
        {
            if (srTransPara->byTransType == CASH_ADVANCE) 
                sprintf(szTempEnterMode, "00%d", 2); //@@IBR MODIFY 20161111
            else if(srTransPara->byOrgTransType == CASH_ADVANCE)
                sprintf(szTempEnterMode, "90%d", 2); //@@IBR MODIFY 20161111
        }
		#endif

		//thandar_250718 fix on DE22 for MPU UPI Installment 
		if ((srTransPara->byTransType == SALE || srTransPara->byTransType == VOID) && (srTransPara->fIsInstallment == TRUE))
			sprintf(szTempEnterMode, "005%d", byPINEntryCap);


        vdDebug_LogPrintf("srTransPara->byTransType[%d][%d][%d][%d][%d][%d]", 
			srTransPara->IITid, byPINEntryCap, srTransPara->byTransType, srTransPara->bWaveSID, srTransPara->byOrgTransType, fGetMPUCard());

		vdDebug_LogPrintf("szTempEnterMode::[%s]", szTempEnterMode);


    }
    else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK) {
		// for cacse 2028 - Fallback scenario DE22 value for UPI and JCB on MPU new switch #2
		if(srTransPara->IITid == 4 || srTransPara->IITid == 6 || srTransPara->IITid == 8 || srTransPara->IITid == 9)
			strcpy(szTempEnterMode, "0901");
		else
        	sprintf(szTempEnterMode, "080%d", byPINEntryCap);
    } 
	else if (srTransPara->byEntryMode == CARD_ENTRY_MSR) {

        vdDebug_LogPrintf("inPackIsoFunc22 [CARD_ENTRY_MSR] [IITid %d], [HDTid %d]", srTransPara->IITid, srTransPara->HDTid);
		
		
		//118.201.48.210:8080/redmine/issues/1525.68
		#if 0
		if(srTransPara->IITid == 4 || srTransPara->IITid == 6 || srTransPara->IITid == 8 || srTransPara->IITid == 9)
			sprintf(szTempEnterMode, "0901");
		else
        	sprintf(szTempEnterMode, "002%d", byPINEntryCap);
		#else		
		
		//if(srTransPara->IITid == 4 || srTransPara->IITid == 6 || srTransPara->IITid == 8 || srTransPara->IITid == 9){
		if(srTransPara->IITid == 6 || srTransPara->IITid == 8 || srTransPara->IITid == 9)
			strcpy(szTempEnterMode, "0022");
		else if(srTransPara->IITid == 4 || srTransPara->IITid == 2 || srTransPara->IITid == 1) //For Pure JCB, VISA and MC
			sprintf(szTempEnterMode,"002%d",byPINEntryCap); 				
		else
			strcpy(szTempEnterMode, "0901");
			
		#endif
    } 
	else if (srTransPara->byEntryMode == CARD_ENTRY_MANUAL) {
        sprintf(szTempEnterMode, "001%d", byPINEntryCap);
    } 
	else if (srTransPara->byEntryMode == CARD_ENTRY_WAVE) 
	{
		vdDebug_LogPrintf("**inPackIsoFunc22 HERE 2 *bWaveSID*[%d][%d][%d]", srTransPara->bWaveSID, srTransPara->byOrgTransType, srTransPara->byTransType);
	
		
		if (srTransPara->bWaveSID == d_VW_SID_PAYPASS_MAG_STRIPE)	
		{
        	sprintf(szTempEnterMode, "008%d", byPINEntryCap);
		}				
		else if (srTransPara->bWaveSID == d_EMVCL_SID_JCB_MSD)  // JCB contactless card scheme pos entry code #2
		{		
			sprintf(szTempEnterMode,"091%d",byPINEntryCap);
		}
		else if (srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY)
		{ 
			sprintf(szTempEnterMode,"007%d",byPINEntryCap);			
		}
		else if((srTransPara->bWaveSID == d_EMVCL_SID_JCB_EMV) || (srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY2))
		{	
			vdDebug_LogPrintf("(srTransPara->bWaveSID == d_EMVCL_SID_JCB_EMV) || (srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY2)");
			
			sprintf(szTempEnterMode,"007%d",byPINEntryCap); // fix for expected POS entry mode for JCB CTLS cards.
		}
		else
		{	// for JCB ctls bWaveSID 99 etc.
			vdDebug_LogPrintf("ELSE !!!");
        	sprintf(szTempEnterMode, "007%d", byPINEntryCap);
		}

	    vdDebug_LogPrintf("inPackIsoFunc22 HERE 2.1 szTempEnterMode>>[%s]", szTempEnterMode);
			
    }
    
    if (srTransPara->byTransType == SETTLE && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19)) {
        sprintf(szTempEnterMode, "000%d", 2);
    }

// old code, for testing (PASSED). to fix rc68 on sale - 03122020-CB_MPU_NH_MIGRATION
#if 0
    if (VS_TRUE == fGetMPUTrans() && srTransPara->byEntryMode == CARD_ENTRY_FALLBACK) {
        sprintf(szTempEnterMode, "002%d", byPINEntryCap);
    }
#endif	
	
    if(srTransPara->fAlipay == TRUE)
    {
		memset(szTempEnterMode, 0, sizeof (szTempEnterMode));
		strcpy(szTempEnterMode, "0010"); /*QR_INQUIRY*/
    }

	// For PreAuthComp Void - New Host implementation - based "POS function and  Regulation" doc
#ifdef CB_MPU_NH_MIGRATION
    if ((VOID == srTransPara->byTransType && srTransPara->byOrgTransType == PREAUTH_COMP
		//|| srTransPara->byOrgTransType == PRE_AUTH //02/17/2022
		|| srTransPara->byOrgTransType == SALE
		|| srTransPara->byOrgTransType == CASH_ADVANCE) //Fix for redmine case http://118.201.48.210:8080/redmine/issues/1525.98.5
		&& (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19)) 
	{

		vdDebug_LogPrintf("**inPackIsoFunc22 HERE 3**");

		memset(szTempEnterMode, 0, sizeof (szTempEnterMode));

		//if(srTransPara->byEntryMode == CARD_ENTRY_FALLBACK) //05122020 - to be finalize
		//	strcpy(szTempEnterMode, "0801");			
		//else
			strcpy(szTempEnterMode, "0012");

    }

    vdDebug_LogPrintf("@@@@@ byTransType[%d]byEntryMode[%d]HDTid[%d]", srTransPara->byTransType, srTransPara->byEntryMode, srTransPara->HDTid);
	
	if((srTransPara->byTransType == REFUND && srTransPara->byEntryMode == CARD_ENTRY_MANUAL) && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19)){
		memset(szTempEnterMode, 0, sizeof (szTempEnterMode));
		strcpy(szTempEnterMode, "0012");
	}
#endif		
	
    vdDebug_LogPrintf("szTempEnterMode>>[%s]", szTempEnterMode);

    wub_str_2_hex(szTempEnterMode, (char *) &uszSendData[inDataCnt], 4);

    inDataCnt += 2; //+= 3; 
    //    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    //    vdMyEZLib_LogPrintf("**inPackIsoFunc22 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field22", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc23(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
//    vdMyEZLib_LogPrintf("**inPackIsoFunc23 START**");
    inDataCnt = 0;

    uszSendData[inDataCnt] = 0x00;
    uszSendData[inDataCnt + 1] = srTransPara->stEMVinfo.T5F34;

    inDataCnt += 2;

    //    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    //    vdMyEZLib_LogPrintf("**inPackIsoFunc23 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field23", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc24(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    inDataCnt = 0;

    DebugAddHEX("inPackIsoFunc24", strHDT.szNII, 2);

    if (srTransPara->byTransType == EFTSEC_TMK_RSA) {
        char szAqcID[4 + 1];
        char szAqcIDHex[2 + 1];

        memset(szAqcID, 0x00, sizeof (szAqcID));
        sprintf(szAqcID, "%04d", atoi(szGetAqcID()));
        wub_str_2_hex((char*) szAqcID, (char*) szAqcIDHex, 4);
        memcpy((char *) &uszSendData[inDataCnt], szAqcIDHex, 2);
        inDataCnt += 2;
        return (inDataCnt);
    }
    memcpy((char *) &uszSendData[inDataCnt], strHDT.szNII, 2);

    inDataCnt += 2;
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field24", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}
#if 0
int inPackIsoFunc25(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    vdMyEZLib_LogPrintf("**inPackIsoFunc25 START**");
    inDataCnt = 0;

    //    if(srTransRec.byTransType == PRE_AUTH)
    if (srTransRec.byTransType == PRE_AUTH ||
            srTransPara->byTransType == MPU_PREAUTH ||
            srTransPara->byTransType == MPU_VOID_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_PREAUTH_COMP_ADV ||
            srTransPara->byTransType == PREAUTH_COMP){
        vdDebug_LogPrintf("***srTransPara->byTransType [%d]", srTransPara->byTransType);
        wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);
    }
//        wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);
    else
        wub_str_2_hex("00", (char *) &uszSendData[inDataCnt], 2);


	vdDebug_LogPrintf("-----inPackIsoFunc25 1-----");

	//thandar
	if(srTransPara->byTransType == VOID && (srTransPara->byOrgTransType == PREAUTH_COMP || srTransPara->byOrgTransType == PRE_AUTH) && (srTransRec.HDTid == 7 || srTransRec.HDTid == 21))
		wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);	

	vdDebug_LogPrintf("-----inPackIsoFunc25 2-----");
	vdDebug_LogPrintf("fGetMPUTrans()[%d]", fGetMPUTrans());
	/*MPU and Credit Combine*/
	if (1 == fGetMPUTrans())
	{
		vdDebug_LogPrintf("----- MPU and Credit Combin -----");
		if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PRE_AUTH)
			wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);

		if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PREAUTH_COMP)
			wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);

		if (srTransPara->byTransType == PRE_AUTH && 19 == srTransPara->HDTid)
			wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);
	}

	vdDebug_LogPrintf("-----inPackIsoFunc25 3-----");

    if(srTransPara->fAlipay == TRUE)
		wub_str_2_hex("67", (char *)&uszSendData[inDataCnt], 2);
     
    #ifdef ECR_PREAUTH_AND_COMP
	if(srTransPara->byPackType == TRANS_ENQUIRY)
		wub_str_2_hex("06", (char *)&uszSendData[inDataCnt], 2);
	#endif
	
    inDataCnt += 1;
//    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
//    vdMyEZLib_LogPrintf("**inPackIsoFunc25 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field25", uszSendData, inDataCnt, 1);

	vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc25 END**");

	vdDebug_LogPrintf("inPackIsoFunc25 END");
	   
    return (inDataCnt);
}
#else // for MPU new host implementation
int inPackIsoFunc25(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    inDataCnt = 0;

	vdDebug_LogPrintf("-----inPackIsoFunc25 start-----");
	vdDebug_LogPrintf("srTransPara->byTransType [%d], srTransPara->byOrgTransType [%d], srTransRec.byTransType[%d]srTransRec.HDTid[%d]srTransRec.CDTid[%d]", 
		srTransPara->byPackType, srTransPara->byOrgTransType, srTransRec.byTransType, srTransRec.HDTid, srTransRec.CDTid);

	
    if (srTransRec.byTransType == PRE_AUTH ||
            srTransPara->byTransType == MPU_PREAUTH ||
            srTransPara->byTransType == MPU_VOID_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_PREAUTH_COMP_ADV ||
            srTransPara->byTransType == PREAUTH_COMP )
    {
        vdDebug_LogPrintf("inPackIsoFunc25 AAAAAA");
		
		if(srTransRec.byTransType == PRE_AUTH ){
			
			vdDebug_LogPrintf("inPackIsoFunc25 BBBBBB");
			wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);			
		}
		else
		{
			if(srTransRec.HDTid == 7 || srTransRec.HDTid == 21 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) // fix for if preauth comp exceed 15%. host still accept.
				wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);				
			else
        		wub_str_2_hex("00", (char *) &uszSendData[inDataCnt], 2);
		}
			//wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2); - original code
    }
	else{
		
        vdDebug_LogPrintf("inPackIsoFunc25 CCCCCC");
		wub_str_2_hex("00", (char *) &uszSendData[inDataCnt], 2);
		}


	vdDebug_LogPrintf("-----inPackIsoFunc25 1-----");

	//thandar
	if(srTransPara->byTransType == VOID && (srTransPara->byOrgTransType == PREAUTH_COMP || srTransPara->byOrgTransType == PRE_AUTH) &&
		(srTransRec.HDTid == 7 || srTransRec.HDTid == 21 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23))
	{
		
       	 vdDebug_LogPrintf("inPackIsoFunc25 DDDDDD");
		wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);	
	}

	vdDebug_LogPrintf("-----inPackIsoFunc25 2-----");
	vdDebug_LogPrintf("fGetMPUTrans()[%d]", fGetMPUTrans());

	
	/*MPU and Credit Combine*/
	if (1 == fGetMPUTrans())
	{
		vdDebug_LogPrintf("----- MPU and Credit Combin -----");
		#if 0
		if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PRE_AUTH)
			wub_str_2_hex("00", (char *) &uszSendData[inDataCnt], 2);

		if (srTransPara->byTransType == VOID && srTransPara->byOrgTransType == PREAUTH_COMP)
		{
			//Incorrect De25 value for PAC (JCB card). Current is 00, should be 06. 09232022
			if(srTransRec.HDTid == 7 || srTransRec.HDTid == 21 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)				
				wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);	
			else	
				wub_str_2_hex("00", (char *) &uszSendData[inDataCnt], 2);
		}
		#else
		if (srTransPara->byTransType == VOID && (srTransPara->byOrgTransType == PREAUTH_COMP ||  srTransPara->byOrgTransType == PRE_AUTH))
		{
		
		vdDebug_LogPrintf("----- MPU and Credit Combin2 -----");
			//Incorrect De25 value for PAC (JCB card). Current is 00, should be 06. 09232022
			if(srTransRec.HDTid == 7 || srTransRec.HDTid == 21 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)				
				wub_str_2_hex("06", (char *) &uszSendData[inDataCnt], 2);	
			else	
				wub_str_2_hex("00", (char *) &uszSendData[inDataCnt], 2);
		}
		
		#endif

		if (srTransPara->byTransType == PRE_AUTH && 19 == srTransPara->HDTid)
			wub_str_2_hex("00", (char *) &uszSendData[inDataCnt], 2);
	}

	vdDebug_LogPrintf("-----inPackIsoFunc25 3-----");

    if(srTransPara->fAlipay == TRUE)
		wub_str_2_hex("67", (char *)&uszSendData[inDataCnt], 2);

#ifdef ECR_PREAUTH_AND_COMP
	if(srTransPara->byPackType == TRANS_ENQUIRY)
		wub_str_2_hex("06", (char *)&uszSendData[inDataCnt], 2);
#endif
	
    inDataCnt += 1;

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field25", uszSendData, inDataCnt, 1);

	vdDebug_LogPrintf("  PACK_LEN%d", inDataCnt);
	vdDebug_LogPrintf("inPackIsoFunc25 END");
	   
    return (inDataCnt);
}

#endif
//@@IBR ADD 20161020
extern BYTE g_byPINDigits;
int inPackIsoFunc26(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    vdMyEZLib_LogPrintf("**inPackIsoFunc26 START**");
    inDataCnt = 0;
    char szTemp[2 + 1];

    memset(szTemp, 0x00, sizeof (szTemp));

    if (srTransPara->byTransType == SALE ||
            srTransPara->byTransType == VOID ||
            srTransPara->byTransType == VOID_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH ||
            srTransPara->byTransType == MPU_VOID_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_PREAUTH_COMP_ADV ||
            srTransPara->byTransType == PRE_AUTH ||
            srTransPara->byTransType == PREAUTH_COMP) {
        inDCTRead(strHDT.inHostIndex);
        //sprintf(szTemp, "%02d", strDCT.inMaxPINDigit);
        sprintf(szTemp, "%02d", g_byPINDigits);
        wub_str_2_hex(szTemp, &uszSendData[0], 2);
        inDataCnt += 1;
    }

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field26", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc32(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) 
{
    int inPacketCnt = 0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[100 + 1];
    
    char szTemp[12 + 1];

	inDataCnt = 0;
	
    vdDebug_LogPrintf("**inPackIsoFunc32 byPackType[%d]ulOrgTraceNum[%ld]szMassageType[%02X%02X]**", srTransPara->byPackType, srTransPara->ulOrgTraceNum, srTransPara->szMassageType[0], srTransPara->szMassageType[1]);

    memset(szPacket, 0, sizeof(szPacket));
    memset(szAscBuf, 0, sizeof(szAscBuf));
	memset(szBcdBuf, 0, sizeof(szBcdBuf));
	memset(szTemp, 0, sizeof(szTemp));
	
    memcpy(szTemp, "0001", 4);
	wub_str_2_hex(szTemp, (char *) szPacket, 4);
    inPacketCnt += 4;

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%02d", inPacketCnt);
	inPacketCnt/=2; /*divide the length by 2*/
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 2);
    memcpy((char *) &uszSendData[inDataCnt], &szBcdBuf[0], 1);
    inDataCnt += 1;
    /* Packet Data */
    memcpy((char *) &uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;     

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(DE32, 0, sizeof(DE32));
        wub_hex_2_str(uszSendData, DE32, inDataCnt);
    }
    #endif


    return inDataCnt;
}

int inPackIsoFunc35(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {

    char szTrack2Data[512];
    char szTrack2Temp[512];
    int inLen;
    int i;

	vdDebug_LogPrintf("-----inPackIsoFunc35 Start-----");

//    vdMyEZLib_LogPrintf("**inPackIsoFunc35 START**");
    inDataCnt = 0;
    inLen = 0;

    if (strlen(srTransPara->szTrack2Data) > 40)
        srTransPara->szTrack2Data[40] = 0x00;

    memset(szTrack2Data, 0x00, sizeof (szTrack2Data));
    strcpy(szTrack2Data, srTransPara->szTrack2Data);

    vdDebug_LogPrintf("srTransPara->szTrack2Data [%s]", srTransPara->szTrack2Data);
    vdDebug_LogPrintf("szTrack2Data [%s]", szTrack2Data);

    for (i = 0; i < strlen(szTrack2Data); i++) {
        if (szTrack2Data[i] == '=')
            szTrack2Data[i] = 'D';

        if (szTrack2Data[i] == '?' || szTrack2Data[i] == 'F')
            szTrack2Data[i] = 0x00;
    }

    inLen = strlen(szTrack2Data);

    vdDebug_LogPrintf("inPackIsoFunc35 new szTrack2Data [%d][%s]", inLen, szTrack2Data);

    /* Data Length */
    uszSendData[inDataCnt++] = (inLen / 10 * 16) + (inLen % 10);

    if (inLen % 2) {
        inLen++;
        strcat(szTrack2Data, "F");
    }

    vdDebug_LogPrintf("inPackIsoFunc35 Fmt szTrack2Data [%d][%s][%d][%d]", inLen, szTrack2Data, fGetMPUTrans(),fGetMPUTrans());

#if 1
    /*for MPU host trans, T2 pad with 0 in front*/
    //if (MPU_DEBIT_HOST_IDX == srTransPara->HDTid)
    //if (VS_TRUE == fGetMPUTrans() || VS_TRUE == fGetMPUCard() || srTransPara->HDTid == 18) {
    if(srTransPara->HDTid == 17) // 08122022
    {
        inDataCnt = 0;
        inLen = 0;

		vdDebug_LogPrintf("inPackIsoFunc35 (VS_TRUE == fGetMPUTrans() || VS_TRUE == fGetMPUCard() || srTransPara->HDTid == 18)");

        memset(szTrack2Data, 0x00, sizeof (szTrack2Data));
        memset(szTrack2Temp, 0x00, sizeof (szTrack2Temp));

        strcpy(szTrack2Temp, srTransPara->szTrack2Data);

        for (i = 0; i < strlen(szTrack2Temp); i++) {
            if (szTrack2Temp[i] == '=')
                szTrack2Temp[i] = 'D';

            if (szTrack2Temp[i] == '?' || szTrack2Temp[i] == 'F')
                szTrack2Temp[i] = 0x00;
        }

        inLen = strlen(szTrack2Temp);

	vdDebug_LogPrintf("inPackIsoFunc35 inLen TTTT :::  [%d]", inLen);

        /* Data Length */
        uszSendData[inDataCnt++] = (inLen / 10 * 16) + (inLen % 10);

	vdDebug_LogPrintf("inPackIsoFunc35 inDataCnt WWWW :::  [%d]", inDataCnt);

        /*pad with '0' in front*/
		#if 1
        if (inLen % 2) {
            inLen++;
            //strcat(szTrack2Data, "F");
            szTrack2Data[0] = '0';
            memcpy(&szTrack2Data[1], szTrack2Temp, inLen);
        } else {
            memcpy(szTrack2Data, szTrack2Temp, inLen);
        }
		#else
			//memcpy(szTrack2Data, szTrack2Temp, inLen);
			memcpy(szTrack2Data, szTrack2Temp, strlen(szTrack2Data));
		#endif
    }
#endif

vdDebug_LogPrintf("inPackIsoFunc35 Final result szTrack2Data [%d][%s][%d]", inLen, szTrack2Data, strlen(szTrack2Data));


#ifdef TLE
    memset((char *) &uszSendData[inDataCnt], 0x00, (inLen / 2) + 1);
    byField_35_ON = TRUE;
#else
    wub_str_2_hex(&szTrack2Data[0], (char *) &uszSendData[inDataCnt], inLen);
#endif
    inDataCnt += (inLen / 2);

vdDebug_LogPrintf("inPackIsoFunc35 Final result inDataCnt [%d]", inDataCnt);

//    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
//    vdMyEZLib_LogPrintf("**inPackIsoFunc35 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field35", uszSendData, inDataCnt, 1);


	
	vdDebug_LogPrintf("-----inPackIsoFunc35 End-----");
    return (inDataCnt);
}

//@@IBR ADD 20161025
int inPackIsoFunc36(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szTrack3Data[107];
    int inLen;
    int i;

    vdMyEZLib_LogPrintf("**inPackIsoFunc36 START**");
    inDataCnt = 0;
    inLen = 0;

    memset(szTrack3Data, 0x00, sizeof (szTrack3Data));
    strcpy(szTrack3Data, srTransPara->szTrack3Data);

    //    for (i = 0; i < strlen(szTrack2Data); i ++)
    //    {
    //        if (szTrack2Data[i] == '=')
    //            szTrack2Data[i] = 'D';
    //			
    //		if (szTrack2Data[i] == '?' || szTrack2Data[i] == 'F')
    //            szTrack2Data[i] = 0x00;
    //    }

    inLen = strlen(szTrack3Data);

    /* Data Length */

    uszSendData[inDataCnt++] = (inLen / 10 * 16) + (inLen % 10);

    //    if (inLen % 2)
    //    {
    //        inLen ++;
    //        strcat(szTrack2Data, "F");
    //    }
#ifdef TLE
    memset((char *) &uszSendData[inDataCnt], 0x00, (inLen / 2) + 1);
    byField_35_ON = TRUE;
#else
    wub_str_2_hex(&szTrack3Data[0], (char *) &uszSendData[inDataCnt], inLen);
#endif
    inDataCnt += (inLen / 2);

    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc36 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field36", uszSendData, inDataCnt, 1);
    return (inDataCnt);
}

int inPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {

	vdDebug_LogPrintf("inPackIsoFunc37 *srTransRec.szRRN: %s", srTransRec.szRRN);
	vdDebug_LogPrintf("inPackIsoFunc37 *srTransRec.szRRN: %s", srTransPara->szRRN);


    inDataCnt = 0;

    memcpy((char *) &uszSendData[inDataCnt], srTransPara->szRRN, 12);
    inDataCnt += 12;

    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc37 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field37", uszSendData, inDataCnt, 0);
    return inDataCnt;
}

int inPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char sztmpAuthCode[6 + 1];
    int len;

    vdMyEZLib_LogPrintf("**inPackIsoFunc38 START**");

    inDataCnt = 0;

    memset(sztmpAuthCode, ' ', 6);
    len = strlen(srTransPara->szAuthCode);
    if (len > 6)
        len = 6;
    memcpy(sztmpAuthCode, srTransPara->szAuthCode, len);

    memcpy((char *) &uszSendData[inDataCnt], sztmpAuthCode, 6);

    inDataCnt += 6;
    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc38 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field38", uszSendData, inDataCnt, 0);
    return inDataCnt;
}

int inPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    vdMyEZLib_LogPrintf("**inPackIsoFunc39 START**");
    inDataCnt = 0;
    memcpy((char *) &uszSendData[inDataCnt], srTransPara->szRespCode, 2);

    inDataCnt += 2;
    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc39 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field39", uszSendData, inDataCnt, 0);
    return inDataCnt;
}

int inPackIsoFunc41(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    BYTE szTID[TERMINAL_ID_BYTES + 1];

    inDataCnt = 0;

    memset(szTID, 0x00, sizeof (szTID));
    memset(szTID, 0x20, TERMINAL_ID_BYTES);
    //memcpy(szTID, srTransPara->szTID, strlen(srTransPara->szTID));
	memcpy(szTID, srTransPara->szTID, TERMINAL_ID_BYTES);
    memcpy((char *) &uszSendData[inDataCnt], szTID, TERMINAL_ID_BYTES);
    inDataCnt += TERMINAL_ID_BYTES;

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
         memset(DE41, 0, sizeof(DE41));
         memcpy(DE41, szTID, TERMINAL_ID_BYTES);
    }
    #endif

    vdDebug_LogPrintf(" TID[%s] PACK_LEN[%d]", szTID, inDataCnt);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field41", uszSendData, inDataCnt, 0);
    return inDataCnt;
}

int inPackIsoFunc42(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    BYTE szMID[MERCHANT_ID_BYTES + 1];

    inDataCnt = 0;

    memset(szMID, 0x00, sizeof (szMID));
    memset(szMID, 0x20, MERCHANT_ID_BYTES);
    //memcpy(szMID, srTransPara->szMID, strlen(srTransPara->szMID));
	memcpy(szMID, srTransPara->szMID, MERCHANT_ID_BYTES);
    memcpy((char *) &uszSendData[inDataCnt], szMID, MERCHANT_ID_BYTES);
    inDataCnt += MERCHANT_ID_BYTES;
    vdDebug_LogPrintf(" MID[%s] PACK_LEN[%d]", szMID, inDataCnt);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field42", uszSendData, inDataCnt, 0);
    return inDataCnt;
}

int inPackIsoFunc43(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    BYTE szCardAcceptorName[CARD_ACCEPTOR_NAME_BYTES + 1];

    inDataCnt = 0;

    memset(szCardAcceptorName, 0x00, sizeof (szCardAcceptorName));
    memset(szCardAcceptorName, 0x20, CARD_ACCEPTOR_NAME_BYTES);
	memcpy(szCardAcceptorName, "CBBANK DEVELOPMENT        YGN         MM", CARD_ACCEPTOR_NAME_BYTES);
    memcpy((char *) &uszSendData[inDataCnt], szCardAcceptorName, CARD_ACCEPTOR_NAME_BYTES);
	
    inDataCnt += CARD_ACCEPTOR_NAME_BYTES;

    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memset(DE43, 0, sizeof(DE43));
        memcpy(DE43, uszSendData, CARD_ACCEPTOR_NAME_BYTES);
    }
    #endif
	
    vdDebug_LogPrintf(" Card Acceptor Name[%s] PACK_LEN[%d]", szCardAcceptorName, inDataCnt);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field43", uszSendData, inDataCnt, 0);
    return inDataCnt;
}

int inPackIsoFunc45(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {

    vdDebug_LogPrintf("**inPackIsoFunc45 START**");
    inDataCnt = 0;
    uszSendData[inDataCnt++] = (srTransPara->usTrack1Len % 100) / 10 * 16 +
            (srTransPara->usTrack1Len % 100) % 10;
    vdMyEZLib_LogPrintf("  45Len%02x", uszSendData[0]);
#ifdef TLE
    memset((char *) &uszSendData[inDataCnt], 0x00, srTransPara->usTrack1Len);
    byField_45_ON = TRUE;
#else
    memcpy((char *) &uszSendData[inDataCnt], srTransPara->szTrack1Data, srTransPara->usTrack1Len);
#endif
    inDataCnt += srTransPara->usTrack1Len;

    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc45 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field45", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

//@@IBR ADD 20161103

int inMPUPackSettleBit48(BYTE *szPacket, int *inLengthData) {
    ACCUM_REC srAccumRec;
    char szTemp[15 + 1];
    int inTransCardType;
    int inPacketCnt = 0;
    int i;

    DOUBLE ulSaleTotalAmount;
    USHORT usSaleCount;

    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));

    inCTOS_ReadAccumTotal(&srAccumRec);

    /*MPU we set debit card only*/
    inTransCardType = 1;

    /*Part 1: aggregate settlement amount of on-us card */
    inTransCardType = 0; // use 0 as on-us, may have problem, need re-check CDT again

    /*aggregate amount of debit transaction*/
    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%012.0f", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.ulSaleTotalAmount);
    memcpy(&szPacket[inPacketCnt], szTemp, 12);
    inPacketCnt += 12;

    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%03d", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.usSaleCount);
    memcpy(&szPacket[inPacketCnt], szTemp, 3);
    inPacketCnt += 3;

    /*aggregate amount of credit transaction*/
    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%012.0f", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.ulVoidSaleTotalAmount);
    memcpy(&szPacket[inPacketCnt], szTemp, 12);
    inPacketCnt += 12;

    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%03d", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.usVoidSaleCount);
    memcpy(&szPacket[inPacketCnt], szTemp, 3);
    inPacketCnt += 3;

    /*reconciliation response code*/
    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%s", "1");
    memcpy(&szPacket[inPacketCnt], szTemp, 1);
    inPacketCnt += 1;
    
    /*Part 2: aggregate settlement amount of off-us card */
    inTransCardType = 1; // use 1 as off-us, may have problem, need re-check CDT again

    /*aggregate amount of debit transaction*/ /*For MPU, the debit total include the voided one*/
    memset(szTemp, 0x00, sizeof (szTemp));
    //sprintf(szTemp, "%012.0f", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.ulSaleTotalAmount);
    ulSaleTotalAmount = srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.ulSaleTotalAmount + srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.ulVoidSaleTotalAmount;
    sprintf(szTemp, "%012.0f", ulSaleTotalAmount);
    memcpy(&szPacket[inPacketCnt], szTemp, 12);
    inPacketCnt += 12;

    memset(szTemp, 0x00, sizeof (szTemp));
    //sprintf(szTemp, "%03d", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.usSaleCount);
    usSaleCount = srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.usSaleCount + srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.usVoidSaleCount;
    sprintf(szTemp, "%03d", usSaleCount);
    memcpy(&szPacket[inPacketCnt], szTemp, 3);
    inPacketCnt += 3;

    /*aggregate amount of credit transaction*/
    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%012.0f", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.ulVoidSaleTotalAmount);
    memcpy(&szPacket[inPacketCnt], szTemp, 12);
    inPacketCnt += 12;

    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%03d", srAccumRec.stBankTotal[inTransCardType].stHOSTTotal.usVoidSaleCount);
    memcpy(&szPacket[inPacketCnt], szTemp, 3);
    inPacketCnt += 3;

    /*reconciliation response code*/
    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%s", "1");
    memcpy(&szPacket[inPacketCnt], szTemp, 1);
    inPacketCnt += 1;

    vdDebug_LogPrintf("MPU Settle DE48 inPacketCnt[%d]", inPacketCnt);
    DebugAddHEX("szPacket", szPacket, inPacketCnt);

    *inLengthData = inPacketCnt;

    return d_OK;
}

int inPackIsoFunc48(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    int inCVV2Len;
    BYTE szBuf[20];
    inDataCnt = 0;
    BYTE szPacket[350 + 1], szMID[MERCHANT_ID_BYTES+1];
    int inPacketCnt = 0;
    char szTemp[128 + 1];
    int inBatchCount = 0;
    int i = 0;
    int inLen=0;
	
    BYTE baCVV_BCD[2 + 1];
    BYTE baCVV_STR[4 + 1];

    vdDebug_LogPrintf("**inPackIsoFunc48 START*byTransType=[%d]*byPackType=[%d],IITid=[%d]", srTransPara->byTransType, srTransPara->byPackType, srTransPara->IITid);

#ifdef TLE
    memset(&uszSendData[inDataCnt], 0x00, 2);
    byField_48_ON = TRUE;
    inDataCnt += 2;
#else

    if ((srTransPara->byTransType == SETTLE && (srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19)) || srTransPara->byTransType == CLS_BATCH) {
        vdDebug_LogPrintf("1");
        memset(szPacket, 0x00, sizeof (szPacket));
        inMPUPackSettleBit48(&szPacket[0], &inPacketCnt);

        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%04d", inPacketCnt);
        wub_str_2_hex(szTemp, &uszSendData[inDataCnt], 4);
        inDataCnt += 2;
        memcpy((char *) &uszSendData[inDataCnt], szPacket, inPacketCnt);
        inDataCnt += inPacketCnt;

    } else if (((srTransPara->byTransType == SALE && srTransPara->fIsInstallment == FALSE) || srTransPara->byTransType == VOID || srTransPara->byTransType == BATCH_UPLOAD)
            && (srTransPara->byPackType == BATCH_UPLOAD) && ((srTransPara->IITid != 1) && (srTransPara->IITid != 2))&& (srTransPara->HDTid != 7)) {
        vdDebug_LogPrintf("2");
        memset(szPacket, 0x00, sizeof (szPacket));
        inPacketCnt = 0;
        
        /*Txn Count*/
        memcpy(&szPacket[inPacketCnt], "01", 2);
        inPacketCnt += 2;

        /*Card Type, MPU card 00*/
        memcpy(&szPacket[inPacketCnt], "00", 2);
        inPacketCnt += 2;

        /*Old Trace Number*/
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%06ld", srTransPara->ulOrgTraceNum);
        memcpy(&szPacket[inPacketCnt], szTemp, 6);
        inPacketCnt += 6;

        /*PAN, upto 20*/
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%s", srTransPara->szPAN);
        wub_strpad(szTemp, szTemp, '0', 20, JF_RIGHT);
        memcpy(&szPacket[inPacketCnt], szTemp, 20);
        inPacketCnt += 20;

        /*Txn Amount*/
        memset(szTemp, 0x00, sizeof (szTemp));
        wub_hex_2_str(srTransPara->szTotalAmount, szTemp, AMT_BCD_SIZE);
        szTemp[AMT_ASC_SIZE] = 0x00;
        memcpy(&szPacket[inPacketCnt], szTemp, AMT_ASC_SIZE);
        inPacketCnt += AMT_ASC_SIZE;

        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%04d", inPacketCnt);
        wub_str_2_hex(szTemp, &uszSendData[inDataCnt], 4);
        inDataCnt += 2;

        memcpy((char *) &uszSendData[inDataCnt], szPacket, inPacketCnt);
        inDataCnt += inPacketCnt;

    }
#ifdef TOPUP_RELOAD
	else if(srTransPara->HDTid == 20)
	{
		char szKCV[16+1];
		
		inPacketCnt=0;
		memset(szPacket, 0x00, sizeof(szPacket));

        inLen=0;
		if(srTransPara->byTransType == TOPUP_RELOAD_LOGON)
		{
            memset(szMID, 0x20, MERCHANT_ID_BYTES);
            memcpy(szMID, srTransPara->szMID, MERCHANT_ID_BYTES);
            inLen=0;
            memcpy(&szPacket[inPacketCnt], TAG_021, strlen(TAG_021)); inPacketCnt+=3; /*Tag*/
            inLen=MERCHANT_ID_BYTES;
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/
            memcpy(&szPacket[inPacketCnt], szMID, inLen); inPacketCnt+=inLen; /*value*/

            inLen=0;
            memcpy(&szPacket[inPacketCnt], TAG_090, strlen(TAG_090)); inPacketCnt+=3; /*Tag*/
            inLen=6;
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/

            memset(szRandomKey, 0, sizeof(szRandomKey));
			vdGenerateRandomKey(16, szRandomKey);
			memset(szKCV, 0, sizeof(szKCV));
			vdGenerateKCV(szRandomKey, szKCV, TRUE);
            memcpy(&szPacket[inPacketCnt], szKCV, inLen); inPacketCnt+=inLen; /*value*/
		}
		else
		{
            memcpy(szPacket, TAG_001, strlen(TAG_001)); inPacketCnt+=3; /*Tag*/
            inLen=atoi("001");
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/
            memcpy(&szPacket[inPacketCnt], "1", inLen); inPacketCnt+=inLen; /*value*/
            
            inLen=0;
            memcpy(&szPacket[inPacketCnt], TAG_002, strlen(TAG_002)); inPacketCnt+=3; /*Tag*/
            inLen=atoi("003");
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/
            memcpy(&szPacket[inPacketCnt], "744", inLen); inPacketCnt+=inLen; /*value*/
            
            inLen=0;
            memcpy(&szPacket[inPacketCnt], TAG_003, strlen(TAG_003)); inPacketCnt+=3; /*Tag*/
            inLen=atoi("002");
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/
            memcpy(&szPacket[inPacketCnt], "03", inLen); inPacketCnt+=inLen; /*value*/
            
            memset(szMID, 0x20, MERCHANT_ID_BYTES);
            memcpy(szMID, srTransPara->szMID, MERCHANT_ID_BYTES);
            inLen=0;
            memcpy(&szPacket[inPacketCnt], TAG_021, strlen(TAG_021)); inPacketCnt+=3; /*Tag*/
            inLen=MERCHANT_ID_BYTES;
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/
            memcpy(&szPacket[inPacketCnt], szMID, inLen); inPacketCnt+=inLen; /*value*/
            
            inLen=0;
            memcpy(&szPacket[inPacketCnt], TAG_100, strlen(TAG_100)); inPacketCnt+=3; /*Tag*/
            inLen=atoi("004");
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/
            memcpy(&szPacket[inPacketCnt], "0001", inLen); inPacketCnt+=inLen; /*value*/
            
            inLen=0;
            memcpy(&szPacket[inPacketCnt], TAG_101, strlen(TAG_101)); inPacketCnt+=3; /*Tag*/
            inLen=atoi("004");
            sprintf(&szPacket[inPacketCnt], "%03d", inLen); inPacketCnt+=3; /*length*/
            memcpy(&szPacket[inPacketCnt], "0001", inLen); inPacketCnt+=inLen; /*value*/
		}
		
        if(srTransPara->HDTid == 20)
        {
            memset(DE48, 0, sizeof(DE48));
            sprintf(DE48, "%03d", inPacketCnt);
            memcpy(&DE48[3], szPacket, inPacketCnt);
        }
		 
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%04d", inPacketCnt);
        wub_str_2_hex(szTemp, &uszSendData[inDataCnt], 4);
        inDataCnt += 2;

        memcpy((char *) &uszSendData[inDataCnt], szPacket, inPacketCnt);
        inDataCnt += inPacketCnt;

		DebugAddHEX("DE48", uszSendData, inDataCnt);
	}
#endif
	else {
    
    
    	vdDebug_LogPrintf("**srTransPara->szCVV2 [%s]", srTransPara->szCVV2);
        DebugAddSTR("inPackIsoFunc48", srTransPara->szCVV2, 4);
        inCVV2Len = strlen(srTransPara->szCVV2);

        /*CBB CVV2 4 digits with BCD format*/
        memset(baCVV_BCD, 0x00, sizeof (baCVV_BCD));
        memset(baCVV_STR, 0x00, sizeof (baCVV_STR));
        strcpy(baCVV_STR, "0000");
        if (inCVV2Len < 4 || 4 == inCVV2Len)
            memcpy(&baCVV_STR[4 - inCVV2Len], srTransPara->szCVV2, inCVV2Len);
        wub_str_2_hex(baCVV_STR, baCVV_BCD, 4);

        memcpy((char *) &uszSendData[inDataCnt], baCVV_BCD, 2);
        //inDataCnt += inCVV2Len;
        inDataCnt += 2;
    }

#endif

    vdDebug_LogPrintf("inPackIsoFunc48	inDataCnt[%d]", inDataCnt);
//    vdPCIDebug_HexPrintf("inPackIsoFunc48", uszSendData, inDataCnt);

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field48", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

int inPackIsoFunc49(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) 
{
    char szTemp[4+1];
    BYTE szCurrencyCode[3+1];

	vdDebug_LogPrintf("**inPackIsoFunc49 START szCurCode[%s]inCurrencyIndex[%d]**", strCST.szCurCode, strCST.inCurrencyIndex);
	
    inDataCnt = 0;
	
    if(srTransPara->byPackType == ALIPAY_SALE || srTransPara->byOrgTransType == ALIPAY_SALE)
    {
		vdDebug_LogPrintf("**inPackIsoFunc49-1 **");
    
		memset(szCurrencyCode, 0x00, sizeof(szCurrencyCode));
		memcpy(szCurrencyCode, strCST.szCurCode, 3);
		memcpy((char *)&uszSendData[inDataCnt], szCurrencyCode, 3);
		inDataCnt += 3;
    }
#ifdef TOPUP_RELOAD	
	else if(srTransPara->HDTid == 20)
	{
	
	vdDebug_LogPrintf("**inPackIsoFunc49-2 **");
        memset(szCurrencyCode, 0x00, sizeof(szCurrencyCode));
        memcpy(szCurrencyCode, strCST.szCurCode, 3);
        memcpy((char *)&uszSendData[inDataCnt], szCurrencyCode, 3);
        inDataCnt += 3;

        if(srTransPara->HDTid == 20)
        {
            memset(DE49, 0, sizeof(DE49));
            //sprintf(DE49, "%03d", inDataCnt);
            memcpy(DE49, uszSendData, inDataCnt);
        }
	}
#endif	
	else
	{
	
	vdDebug_LogPrintf("**inPackIsoFunc49-3 **");
        memset(szTemp, 0x00, sizeof (szTemp));
		
        strcpy(szTemp, strCST.szCurCode);
        inFmtPad(szTemp, -4, '0');
		
		vdDebug_LogPrintf("**inPackIsoFunc49-4 *[%s]*", szTemp);	

		
        wub_str_2_hex(szTemp, (char *) &uszSendData[0], 4);
        inDataCnt += 2;
	}

	
	vdDebug_LogPrintf("**inPackIsoFunc49-5 *[%s]*", szTemp);	
    vdDebug_LogPrintf("inPackIsoFunc49	PACK_LEN [%d]", inDataCnt);

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field49", uszSendData, inDataCnt, 2);

    return inDataCnt;
}

int inPackIsoFunc52(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    inDataCnt = 0;

    memcpy((BYTE *) & uszSendData[inDataCnt], srTransPara->szPINBlock, 8);
    inDataCnt += 8;
    vdDebug_LogPrintf("inPackIsoFunc52	PACK_LEN%d", inDataCnt);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field52", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

int inPackIsoFunc54(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    int inPacketCnt = 0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[20 + 1];
    char szBaseAmt[20 + 1];
    char szVoidTotalAmt[20 + 1];

    inDataCnt = 0;
    DebugAddHEX("inPackIsoFunc54", srTransPara->szTipAmount, 6);

    memset(szPacket, 0x00, sizeof (szPacket));

    //Should be Online void the Intial SALE amount.
//    if (srTransPara->byTransType == VOID) {
    if (srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) {
        //use szStoreID to store how much amount fill up in DE4 for VOID
        memset(szBaseAmt, 0x00, sizeof (szBaseAmt));
        memset(szVoidTotalAmt, 0x00, sizeof (szVoidTotalAmt));

        wub_hex_2_str(srTransPara->szBaseAmount, szBaseAmt, 6);
        wub_hex_2_str(srTransPara->szStoreID, szVoidTotalAmt, 6);
        //format amount 10+2
        sprintf(szPacket, "%012.0f", atof(szVoidTotalAmt) - atof(szBaseAmt));
    } else {
        wub_hex_2_str(srTransPara->szTipAmount, szPacket, 6);
    }

    inPacketCnt = strlen(szPacket);
    memset(szAscBuf, 0x00, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;

    DebugAddHEX("inPackIsoFunc54", szBcdBuf, 2);

    /* Packet Data */
    memcpy((char *) &uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    DebugAddSTR("inPackIsoFunc54", szPacket, 12);

    vdMyEZLib_LogPrintf("**inPackIsoFunc54 START**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field54", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

int inPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    vdDebug_LogPrintf("**inPackIsoFunc55 START* bWaveSID [%d]*", srTransPara->bWaveSID);

    inDataCnt = 0;

    if (srTransPara->byEntryMode == CARD_ENTRY_ICC)
	{
//		if (VS_TRUE == fGetMPUTrans())
        if (VS_TRUE == fGetMPUTrans())
            inDataCnt = inMPU_PackISOEMVData(srTransPara, uszSendData);
		else
			inDataCnt = inPackISOEMVData(srTransPara, uszSendData);
	}
    if (srTransPara->byEntryMode == CARD_ENTRY_WAVE) {
        if (srTransPara->bWaveSID == d_VW_SID_PAYPASS_MCHIP)
            inDataCnt = inPackISOPayPassData(srTransPara, uszSendData);

        if (srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_2 ||
                srTransPara->bWaveSID == d_VW_SID_VISA_WAVE_QVSDC)
            inDataCnt = inPackISOPayWaveData(srTransPara, uszSendData);

        if (srTransPara->bWaveSID == d_VW_SID_JCB_WAVE_QVSDC)
            inDataCnt = inPackISOPayWaveData(srTransPara, uszSendData);

        if (srTransPara->bWaveSID == d_VW_SID_AE_EMV)
            inDataCnt = inPackISOExpressPayData(srTransPara, uszSendData);
		
		if (srTransPara->bWaveSID == d_VW_SID_CUP_EMV)
			inDataCnt = inPackISOQuickpassData(srTransPara, uszSendData);

		// Add to send de55 for JCB CTLS transactions #1
		#ifdef MPU_CARD_TC_UPLOAD_ENABLE
		if((srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY) || (srTransPara->bWaveSID == d_EMVCL_SID_JCB_MSD) || 
			(srTransPara->bWaveSID == d_EMVCL_SID_JCB_LEGACY2) || (srTransPara->bWaveSID == d_EMVCL_SID_JCB_EMV))			
            			inDataCnt = inPackISOJCBPayWaveData(srTransPara, uszSendData);
			//inDataCnt = inPackISOPayWaveData(srTransPara, uszSendData);
            
		#endif
		
    }

    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc55 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field55", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

int inPackIsoFunc56(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    vdDebug_LogPrintf("**inPackIsoFunc56 START**");
    inDataCnt = 0;

	if(srTransPara->byPackType == ALIPAY_SALE)
	{
		uszSendData[inDataCnt++] = 0x00;
		uszSendData[inDataCnt++] = 0x04;
		sprintf((char *)&uszSendData[inDataCnt], "%04d", 1);
		inDataCnt += 4;
	}

    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc56 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field56", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

/*CUP sign on Req Data*/
int inPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    BYTE szTMKRefNum[8 + 1];
    vdDebug_LogPrintf("**inPackIsoFunc57 START*HDTID[%d], byTransType[%d]*", srTransPara->HDTid, srTransPara->byTransType);
    inDataCnt = 0;

    vdDebug_LogPrintf("  PACK_LEN%d", inDataCnt);

    if (srTransPara->byTransType == CUP_LOGON) {
        /*Len*/
        memcpy((BYTE *) & uszSendData[inDataCnt], "\x00\x10", 2);
        inDataCnt += 2;

        /*version*/
        memcpy((BYTE *) & uszSendData[inDataCnt], "\x00\x03", 2);
        inDataCnt += 2;

        /*TMK Refer NO.*/
        memset(szTMKRefNum, 0x00, sizeof (szTMKRefNum));

		#ifdef PIN_CHANGE_ENABLE
		if(srTransPara->HDTid == 21 || srTransPara->HDTid == 1)
        	get_env("#TMKREF2", szTMKRefNum, 8);
		else
		{
			#ifdef CBB_FIN_ROUTING
			if(srTransPara->HDTid == 22)
				get_env("#FINUSDTMKREF", szTMKRefNum, 8);
			else if (srTransPara->HDTid == 23)
				get_env("#FINMMKTMKREF", szTMKRefNum, 8);
			else	
				get_env("#TMKREF", szTMKRefNum, 8);
			#else			
				get_env("#TMKREF", szTMKRefNum, 8);
			#endif
		}
		#else
		get_env("#TMKREF", szTMKRefNum, 8);
		#endif
		
        memcpy((BYTE *) & uszSendData[inDataCnt], szTMKRefNum, 8);
        inDataCnt += 8;
    } else if (srTransPara->byTransType == IPP_SIGN_ON) {
        /*Len*/
        memcpy((BYTE *) & uszSendData[inDataCnt], "\x00\x10", 2);
        inDataCnt += 2;

        /*version*/
        memcpy((BYTE *) & uszSendData[inDataCnt], "\x00\x03", 2);
        inDataCnt += 2;

        /*TMK Refer NO.*/
        memset(szTMKRefNum, 0x00, sizeof (szTMKRefNum));
        get_env("#IPPTMKREF", szTMKRefNum, 8);
        memcpy((BYTE *) & uszSendData[inDataCnt], szTMKRefNum, 8);
        inDataCnt += 8;
    }
	else if (srTransPara->byTransType == MPU_SIGNON) {
			/*Len*/
			memcpy((BYTE *) & uszSendData[inDataCnt], "\x00\x10", 2);
			inDataCnt += 2;
	
			/*version*/
			memcpy((BYTE *) & uszSendData[inDataCnt], "\x00\x03", 2);
			inDataCnt += 2;
	
			/*TMK Refer NO.*/
			memset(szTMKRefNum, 0x00, sizeof (szTMKRefNum));
			
			if(srTransPara->HDTid == 22)
				get_env("#FINUSDTMKREF", szTMKRefNum, 8);
			else if(srTransPara->HDTid == 23)
				get_env("#FINMMKTMKREF", szTMKRefNum, 8);
				
			memcpy((BYTE *) & uszSendData[inDataCnt], szTMKRefNum, 8);
			inDataCnt += 8;
		}

    vdDebug_LogPrintf("inPackIsoFunc57	PACK_LEN%d", inDataCnt);

    vdDebug_LogPrintf("**inPackIsoFunc57 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field57", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

int inPackIsoFunc60(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) 
{
    int inPacketCnt = 0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[100 + 1];
    char szTemp[256 + 1];
	BYTE baMPU_DE60[40+1];
	char szTemBuff[6];

	int inMPUDataStrLen = 0;  // Data len in bytes
	int inMPUDataHexLen = 0;  // Data len in bytes
	int inMPUFmtHLen = 0;	// Formatted 2 bytes header 

	inDataCnt = 0;
	
    vdDebug_LogPrintf("**inPackIsoFunc60 byPackType[%d]ulOrgTraceNum[%ld]szMassageType[%02X%02X]**", srTransPara->byPackType, srTransPara->ulOrgTraceNum, srTransPara->szMassageType[0], srTransPara->szMassageType[1]);
//    DebugAddHEX("60 Batch Num", srTransPara->szBatchNo, 3);
	memset(szPacket, 0x00, sizeof(szPacket));
    
    if((srTransPara->byPackType == BATCH_UPLOAD) || (srTransPara->byPackType == TC_UPLOAD))
    {   
        /* Load the Original Data Message in field 60 */
        /* Field 60 contains 4 digits of MID, 6 digits of STAN
           and 12 digits of Reserved space set to spaces.
           */
        wub_hex_2_str(srTransPara->szMassageType,szPacket,2);
        inPacketCnt += 4;

        sprintf(&szPacket[inPacketCnt], "%06ld", srTransPara->ulOrgTraceNum);
        inPacketCnt += 6;
        
        memcpy(&szPacket[inPacketCnt],srTransPara->szRRN,RRN_BYTES);
        inPacketCnt += RRN_BYTES;  
    }
    else if(srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH)
    {
        //wub_hex_2_str(srTransRec.szBatchNo,szPacket,3);
        wub_hex_2_str(srTransPara->szBatchNo,szPacket,3);
        
        inPacketCnt += 6;
		if(srTransPara->HDTid == 6) /*IPP Host*/ 
			srTransPara->fIsInstallment=TRUE;
    }
	else if((srTransPara->byTransType == SALE) && (srTransPara->fIsInstallment == TRUE))
    {        
        //IPP Scheme ID
        memset(szTemp, 0x00, sizeof(szTemp));
        vdGetIPPSchemeID(szTemp);
        memcpy(szPacket+inPacketCnt, srTransPara->stIPPinfo.szIPPSchemeID, 6);
		inPacketCnt += 6;
        //Total Amount
        memcpy(szPacket+inPacketCnt, "000000000000", 12);
		inPacketCnt += 12;
        //Transaction Currency
        memcpy(szPacket+inPacketCnt, "000", 3);
		inPacketCnt += 3;
        //Number of Installment
        memcpy(szPacket+inPacketCnt, srTransPara->stIPPinfo.szInstallmentTerms, 3);
		inPacketCnt += 3;
        //Frequency of Installment
        memcpy(szPacket+inPacketCnt, "M", 1);
		inPacketCnt += 1;
        //Monthly 
        memcpy(szPacket+inPacketCnt, "000000000000", 12);
		inPacketCnt += 12;
        //Installment Payment Plan Fee
        memcpy(szPacket+inPacketCnt, "000000000000", 12);
		inPacketCnt += 12;
        //???
        memcpy(szPacket+inPacketCnt, "000000000000000000000000", 24);
		inPacketCnt += 24;
    }
	else if((srTransPara->byTransType == VOID) && (srTransPara->fIsInstallment == TRUE))
	{
        memcpy(szPacket+inPacketCnt, srTransPara->stIPPinfo.szIPPSchemeID, 6);                  
		inPacketCnt += 6;
        
        memset(szTemp, 0x00, sizeof(szTemp));
        wub_hex_2_str(srTransPara->szTotalAmount, szTemp, AMT_BCD_SIZE);
        memcpy(szPacket+inPacketCnt, szTemp, AMT_ASC_SIZE);                                     
		inPacketCnt += AMT_ASC_SIZE;
        
        memcpy(szPacket+inPacketCnt, srTransPara->stIPPinfo.szTransCurrency, 3);                
		inPacketCnt += 3;
        memcpy(szPacket+inPacketCnt, srTransPara->stIPPinfo.szInstallmentTerms, 3);             
		inPacketCnt += 3;
        memcpy(szPacket+inPacketCnt, srTransPara->stIPPinfo.szFreqInstallment, 1);              
		inPacketCnt += 1;
        
        memset(szTemp, 0x00, sizeof(szTemp));
        wub_hex_2_str(srTransPara->stIPPinfo.szMonthlyAmt, szTemp, AMT_BCD_SIZE);
        memcpy(szPacket+inPacketCnt, szTemp, AMT_ASC_SIZE);                                     
		inPacketCnt += AMT_ASC_SIZE;
        
        memcpy(szPacket+inPacketCnt, srTransPara->stIPPinfo.szInterestRate, 12);                
		inPacketCnt += 12;
        
        memset(szTemp, 0x00, sizeof(szTemp));
        wub_hex_2_str(srTransPara->stIPPinfo.szTotalInterest, szTemp, AMT_BCD_SIZE);
        memcpy(szPacket+inPacketCnt, szTemp, AMT_ASC_SIZE);                                     
		inPacketCnt += AMT_ASC_SIZE;
        
        memset(szTemp, 0x00, sizeof(szTemp));
        wub_hex_2_str(srTransPara->stIPPinfo.szHandlingFee, szTemp, AMT_BCD_SIZE);
        memcpy(szPacket+inPacketCnt, szTemp, AMT_ASC_SIZE);                                     
		inPacketCnt += AMT_ASC_SIZE;
        
        DebugAddHEX("szPacket 60", szPacket, inPacketCnt);
    } 
    else if(srTransPara->fAlipay == TRUE)
    {
        memcpy(szPacket, "0401", 4);
        inPacketCnt += 4;
    }
	else 
	{
		#ifdef QUICKPASS
		if((srTransPara->HDTid == 7) && (srTransPara->byTransType != CHANGE_PIN)) /*Host 7 - UPI Finexus*/
		{
			memset(szTemBuff, 0, sizeof(szTemBuff));
			memcpy((char *)szTemBuff,"000",3);

			memset(szBcdBuf, 0, sizeof (szBcdBuf));
			
            if(srTransPara->byEntryMode == CARD_ENTRY_ICC || srTransPara->byEntryMode == CARD_ENTRY_EASY_ICC) 
            {
                memcpy((char *)szBcdBuf,"510",3);
            }
            else if(srTransPara->byEntryMode == CARD_ENTRY_MSR) 
            {
                memcpy((char *)szBcdBuf,"500",3);
            }
            else if(srTransPara->byEntryMode == CARD_ENTRY_MANUAL) 
            {
                memcpy((char *)szBcdBuf,"500",3);
            }
            else if(srTransPara->byEntryMode == CARD_ENTRY_FALLBACK) 
            {
                memcpy((char *)szBcdBuf,"520",3);
            }
            else if(srTransPara->byEntryMode == CARD_ENTRY_WAVE) 
            {
                memcpy((char *)szBcdBuf,"600",3);
            }
			
            strcat(szTemBuff,szBcdBuf);
            memset(szPacket,0,sizeof(szPacket));
            wub_str_2_hex(szTemBuff, szPacket, 6);
			inPacketCnt=3;
        }		
		#ifdef PIN_CHANGE_ENABLE
		//else if((srTransPara->HDTid == 7 || srTransPara->HDTid == 21) && (srTransPara->byTransType == CHANGE_PIN))
		else if(srTransPara->byTransType == CHANGE_PIN)
		{

			DebugAddHEX("inPackIsoFunc60->srTransPara->szVerifyNewPINBlock", srTransPara->szVerifyNewPINBlock, 8);

			//memcpy(&szPacket[inDataCnt], srTransPara->szNewPINBlock, 8);
			memcpy(&szPacket[inDataCnt], srTransPara->szVerifyNewPINBlock, 8);
						
			DebugAddHEX("uszSendData1", szPacket, 8);
			inPacketCnt += 8;
		}		
		#endif
        else
        {
            wub_hex_2_str(srTransPara->szBaseAmount,szPacket,6);
            inPacketCnt += 12;
        }
		#else
            wub_hex_2_str(srTransPara->szBaseAmount,szPacket,6);
            inPacketCnt += 12;
		#endif
    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
	
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
	
    memcpy((char *) &uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

// for new host implementation of de60 for testing - fix for SETTLE FAILED, Comm error on Void - CB_MPU_NH_MIGRATION
#if 0
    if (fGetCashAdvAppFlag() == FALSE)
	{
		 if (srTransPara->fIsInstallment != TRUE)
		{
			// old host implemention of de60
			#if 0
            if ((VS_TRUE == fGetMPUCard() || 
				VS_TRUE == fGetMPUTrans()) || 
				(srTransPara->HDTid == 17 || 
				srTransPara->HDTid == 18 || 
				srTransPara->HDTid == 19)) 
			{
                inDataCnt = 0;
                inDataCnt = inMPU_PackDE60Data(srTransPara, uszSendData);
            }
			#else // for new host implementation of de60 for testing - fix for SETTLE FAILED
            if ((VS_TRUE == fGetMPUCard() || 
				VS_TRUE == fGetMPUTrans()) || 
				(srTransPara->HDTid == 18 || srTransPara->HDTid == 19)) 
			{
                inDataCnt = 0;
                inDataCnt = inMPU_PackDE60Data(srTransPara, uszSendData);
            }
			
			#endif
        }
    }
#endif        

    vdDebug_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdDebug_LogPrintf("**inPackIsoFunc60 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field60", uszSendData, inDataCnt, 1);
    return inDataCnt;
}


int inPackIsoFunc61(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szTemp[8 + 1];
    char szPacket[20 + 1];
    int inPacketCnt = 0;

    char szSTAN[6 + 1];

    BYTE baMPU_DE61_HEX[64];
    int inHexLen = 0;
    extern BYTE szAuthDate[2];

    vdDebug_LogPrintf("**inPackIsoFunc61 START**");
    inDataCnt = 0;

	vdDebug_LogPrintf("fGetMPUTrans() = %d", fGetMPUTrans());
   	vdDebug_LogPrintf("srTransPara->byTransType = %d", srTransPara->byTransType);
   
    if (srTransPara->byTransType == VOID || srTransPara->byTransType == VOID_PREAUTH) 
	{
		vdDebug_LogPrintf("condition void and VOID_PREAUTH");
		
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szPacket, 0x00, sizeof (szPacket));

        wub_hex_2_str(srTransPara->szBatchNo, szTemp, BATCH_NO_BCD_SIZE);
        memcpy(szPacket + inPacketCnt, szTemp, BATCH_NO_ASC_SIZE);
		inPacketCnt += BATCH_NO_ASC_SIZE;

        memset(szTemp, 0x00, sizeof (szTemp));
        if (srTransPara->byPackType == VOID_REVERSAL || 
			srTransPara->byPackType == REVERSAL || 
			srTransPara->byPackType == MPU_VOID_PREAUTH ||
			srTransPara->byTransType == VOID_PREAUTH_REV)
            sprintf(szTemp, "%06lu", srTransPara->ulTraceNum);
        else
            sprintf(szTemp, "%06lu", srTransPara->ulOrgTraceNum);

        memcpy(szPacket + inPacketCnt, szTemp, 6);
		inPacketCnt += 6;

        memset(szTemp, 0x00, sizeof (szTemp));
        wub_hex_2_str(srTransPara->szDate, szTemp, 2);
        memcpy(szPacket + inPacketCnt, szTemp, 4);
		inPacketCnt += 4;

        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%04d", inPacketCnt);
        wub_str_2_hex(szTemp, &uszSendData[0], 4);
		inDataCnt += 2;

        memset(baMPU_DE61_HEX, 0x00, sizeof (baMPU_DE61_HEX));
        wub_str_2_hex(szPacket, baMPU_DE61_HEX, inPacketCnt);
        inHexLen = inPacketCnt / 2;

        //memcpy((char *) &uszSendData[inDataCnt], szPacket, inPacketCnt);
        //inDataCnt += inPacketCnt;
        memcpy((char *) &uszSendData[inDataCnt], baMPU_DE61_HEX, inHexLen);
        inDataCnt += inHexLen;
    } 
	else if (srTransPara->byTransType == MPU_VOID_PREAUTH ||
            srTransPara->byTransType == MPU_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
            srTransPara->byTransType == MPU_PREAUTH_COMP_ADV ||
            srTransPara->byTransType == PREAUTH_COMP)
    {

		vdDebug_LogPrintf("**inPackIsoFunc61 XXXXX**");

        inDataCnt = 0;
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szPacket, 0x00, sizeof (szPacket));

        uszSendData[inDataCnt] = 0x00;
		inDataCnt += 1;
        uszSendData[inDataCnt] = 0x16;
		inDataCnt += 1;

		// for testing. fatal error encountered on CUP PreAuth Completion - as per Zin testing 81 series bins.
		// dont sedn Batch no and Trance num as compared with other brands de61 (PreAuth Comp).
		// 12282018
		if(srTransRec.IITid == 6 && strHDT.inHostIndex == 19)
		{			
			vdDebug_LogPrintf("**inPackIsoFunc61 YYYYYY**");
		
			uszSendData[inDataCnt] = 0x00;																	
			inDataCnt += 6;

			memset(baMPU_DE61, 0x00, sizeof (baMPU_DE61));
			memcpy(baMPU_DE61, uszSendData, inDataCnt);
			DebugAddHEX("baMPU_DE61. START", baMPU_DE61, inDataCnt);
			
		}
		else
		{
	        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szBatchNo, BATCH_NO_BCD_SIZE);
			inDataCnt += 3;

	        memset(szSTAN, 0x00, sizeof (szSTAN));
	        sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
	        wub_str_2_hex(szSTAN, (char *) &uszSendData[inDataCnt], 6);
			inDataCnt += 3;
		}
        
        if (srTransPara->byTransType == MPU_VOID_PREAUTH || 
			srTransPara->byTransType == MPU_VOID_PREAUTH_COMP
			)
		{
            memcpy((char *) &uszSendData[inDataCnt], srTransPara->szOrgDate, 2);
		}
        else if (srTransPara->byTransType == MPU_PREAUTH_COMP ||
                 srTransPara->byTransType == MPU_PREAUTH_COMP_ADV ||
                 srTransPara->byTransType == PREAUTH_COMP)
		{
			vdDebug_LogPrintf("**inPackIsoFunc61 ZZZZZ**");
		
            memcpy((char *) &uszSendData[inDataCnt], szAuthDate, 2);
			inDataCnt += 2;
		}
    }
	else 
	{
        uszSendData[inDataCnt++] = 0x00;
        uszSendData[inDataCnt++] = 0x06;
        sprintf((char *) &uszSendData[inDataCnt], "%06ld", wub_bcd_2_long(srTransPara->szInvoiceNo, 3));        
		inDataCnt += 6;
    }

    if (VS_TRUE == fGetMPUTrans() && (srTransPara->byPackType == REVERSAL && srTransPara->byTransType != VOID)) {
        inDataCnt = 0;
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szPacket, 0x00, sizeof (szPacket));

        uszSendData[inDataCnt] = 0x00;                                                                  
		inDataCnt += 1;
        uszSendData[inDataCnt] = 0x16;                                                                  
		inDataCnt += 1;
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szBatchNo, BATCH_NO_BCD_SIZE);            
		inDataCnt += 3;

        memset(szSTAN, 0x00, sizeof (szSTAN));
        sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
        wub_str_2_hex(szSTAN, (char *) &uszSendData[inDataCnt], 6);                                     
		inDataCnt += 3;

        memcpy((char *) &uszSendData[inDataCnt], baMPU_DE07, 2);                                        
		inDataCnt += 2;
    }

    if (VS_TRUE == fGetMPUTrans() &&
            (srTransPara->byPackType == MPU_PREAUTH_REV ||
            srTransPara->byPackType == MPU_PAC_REV ||
            srTransPara->byPackType == MPU_PAC_ADV_REV ||
            srTransPara->byPackType == MPU_VOID_PREAUTH_REV ||
            srTransPara->byPackType == MPU_VOID_PAC_REV)) 
     {
		vdDebug_LogPrintf("**inPackIsoFunc61 AAAAAA**");
     
        inDataCnt = 0;
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szPacket, 0x00, sizeof (szPacket));

        uszSendData[inDataCnt] = 0x00;                                                                  
		inDataCnt += 1;
        uszSendData[inDataCnt] = 0x16;                                                                  
		inDataCnt += 1;
        memcpy((char *) &uszSendData[inDataCnt], srTransPara->szBatchNo, BATCH_NO_BCD_SIZE);            
		inDataCnt += 3;

        memset(szSTAN, 0x00, sizeof (szSTAN));
        sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
        wub_str_2_hex(szSTAN, (char *) &uszSendData[inDataCnt], 6);                                     
		inDataCnt += 3;

        memcpy((char *) &uszSendData[inDataCnt], &baTransDT[0], 2);                                  
		inDataCnt += 2;
    }

    /*prepare MPU MAC buffer*/
    memset(baMPU_DE61, 0x00, sizeof (baMPU_DE61));
    memcpy(baMPU_DE61, uszSendData, inDataCnt);
    DebugAddHEX("baMPU_DE61", baMPU_DE61, inDataCnt);

//    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
//    vdMyEZLib_LogPrintf("**inPackIsoFunc61 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field61", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

extern int inHTLEField62Data(unsigned char *pbtBuff, unsigned short *pusLen);

int inPackIsoFunc62(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    char szPacket[7];
    unsigned short usLength = 0;
    unsigned char szTextBuff[10 + 1];
    unsigned char szHexBuff[10 + 1];
    int inTotalLength = 0;
    unsigned char bTagOutput[512];
    char szTemp[8 + 1];
    char szBcd[3+1];
	
    DebugAddHEX("inPackIsoFunc62", srTransPara->szInvoiceNo, 3);

    if (srTransPara->byTransType == EFTSEC_TMK) {
        inDataCnt = 0;
        memset(szTextBuff, 0x00, sizeof (szTextBuff));
        memset(szHexBuff, 0x00, sizeof (szHexBuff));
        memset(bTagOutput, 0x00, sizeof (bTagOutput));

        inHTLEField62Data((unsigned char *) bTagOutput, &usLength);
        inTotalLength = usLength;

        sprintf((char *) szTextBuff, "%04d", inTotalLength);
        wub_str_2_hex((char *) szTextBuff, (char *) szHexBuff, 4);
        memcpy(uszSendData, szHexBuff, 2);
        memcpy(&uszSendData[2], &bTagOutput[0], inTotalLength);
        inDataCnt = inTotalLength + 2;
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", uszSendData, inDataCnt, 0);
        return inDataCnt;
    }

    if (srTransPara->byTransType == EFTSEC_TWK) {
        inDataCnt = 0;
        memset(szTextBuff, 0x00, sizeof (szTextBuff));
        memset(szHexBuff, 0x00, sizeof (szHexBuff));
        memset(bTagOutput, 0x00, sizeof (bTagOutput));

        inTWKField62Data((unsigned char *) bTagOutput, &usLength);

        inTotalLength = usLength;
        sprintf((char *) szTextBuff, "%04d", inTotalLength);
        wub_str_2_hex((char *) szTextBuff, (char *) szHexBuff, 4);
        memcpy(uszSendData, szHexBuff, 2);
        memcpy(&uszSendData[2], &bTagOutput[0], inTotalLength);
        inDataCnt = inTotalLength + 2;
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", uszSendData, inDataCnt, 0);
        return inDataCnt;
    }

    if (srTransPara->byTransType == EFTSEC_TMK_RSA) {
        inDataCnt = 0;
        memset(szTextBuff, 0x00, sizeof (szTextBuff));
        memset(szHexBuff, 0x00, sizeof (szHexBuff));
        memset(bTagOutput, 0x00, sizeof (bTagOutput));

        vdHTLELoadField62DataRSA((unsigned char *) bTagOutput, &usLength);
        inTotalLength = usLength;
        vdPCIDebug_HexPrintf("HTLELoadField62DataRS", bTagOutput, inTotalLength);
        sprintf((char *) szTextBuff, "%04d", inTotalLength);
        wub_str_2_hex((char *) szTextBuff, (char *) szHexBuff, 4);
        memcpy(uszSendData, szHexBuff, 2);
        memcpy(&uszSendData[2], &bTagOutput[0], inTotalLength);
        inDataCnt = inTotalLength + 2;
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", uszSendData, inDataCnt, 0);
        return inDataCnt;
    }

    if (srTransPara->byTransType == EFTSEC_TWK_RSA) {
        //		inDataCnt = 0;	
        //		memset(szTextBuff, 0x00, sizeof (szTextBuff));
        //		memset(szHexBuff, 0x00, sizeof (szHexBuff));
        //		memset(bTagOutput, 0x00, sizeof (bTagOutput));

        //		inTWKRSAField62Data((unsigned char *)bTagOutput, &usLength);

        //		inTotalLength = usLength;
        //		sprintf((char *)szTextBuff, "%04d", inTotalLength);
        //		wub_str_2_hex((char *)szTextBuff, (char *)szHexBuff, 4);
        //		memcpy(uszSendData, szHexBuff, 2);
        //		memcpy(&uszSendData[2], &bTagOutput[0], inTotalLength);
        //		inDataCnt = inTotalLength + 2;
        //		return inDataCnt;			
        inDataCnt = 0;
        memset(szTextBuff, 0x00, sizeof (szTextBuff));
        memset(szHexBuff, 0x00, sizeof (szHexBuff));
        memset(bTagOutput, 0x00, sizeof (bTagOutput));

        inTWKField62Data((unsigned char *) bTagOutput, &usLength);

        inTotalLength = usLength;
        sprintf((char *) szTextBuff, "%04d", inTotalLength);
        wub_str_2_hex((char *) szTextBuff, (char *) szHexBuff, 4);
        memcpy(uszSendData, szHexBuff, 2);
        memcpy(&uszSendData[2], &bTagOutput[0], inTotalLength);
        inDataCnt = inTotalLength + 2;
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", uszSendData, inDataCnt, 0);
        return inDataCnt;
    }

    #ifdef TOPUP_RELOAD
    if (srTransPara->byTransType == TOPUP_RELOAD_LOGON) {
        inDataCnt = 0;
		unsigned char encrypted[768+1]={};
		int encrypted_length=0;
        encrypted_length=inRSA_EncryptPublicKey(szRandomKey, encrypted);

        int inPacketCnt=encrypted_length;
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%04d", encrypted_length);
        wub_str_2_hex(szTemp, &uszSendData[inDataCnt], 4);
        inDataCnt += 2;
        
        memcpy((char *) &uszSendData[inDataCnt], encrypted, inPacketCnt);
        inDataCnt += inPacketCnt;
        DebugAddHEX("DE62", uszSendData, inDataCnt);

        //build string ISO
        memset(DE62, 0, sizeof(DE62));
		sprintf(DE62, "%03d", (encrypted_length*2));
		wub_hex_2_str(encrypted, &DE62[3], encrypted_length);
		vdDebug_LogPrintf("len:%d, data:%s", strlen(DE62), DE62);
		//memcpy(&DE62[3], szPacket, inPacketCnt);
        return inDataCnt;
    }
    #endif
	
    /////////////////////////////////////////////////////////////////////////////////////////////////////////	

    inDataCnt = 0;
    uszSendData[inDataCnt++] = 0x00;
    uszSendData[inDataCnt++] = 0x06;
    if (srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH)
	{
        //memcpy((char *)&uszSendData[inDataCnt],"000000",6);
        /*Alipay - Start*/
        //inTCTRead(1);
        memset(szBcd, 0x00, sizeof(szBcd));
        memcpy(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);    
        inBcdAddOne(szBcd, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);
		memcpy(srTransPara->szInvoiceNo, strTCT.szInvoiceNo, INVOICE_BCD_SIZE);    
		inTCTSave(1);
        /*Alipay - End*/
        sprintf((char *) &uszSendData[inDataCnt], "%06ld", wub_bcd_2_long(srTransPara->szInvoiceNo, 3));
    } 
	else 
    {
        if(srTransPara->fAlipay == TRUE)
        {
            if(srTransPara->byTransType == QR_INQUIRY)
                sprintf((char *) &uszSendData[inDataCnt], "%06ld", wub_bcd_2_long(srTransPara->szInvoiceNo, 3));			
            else
                sprintf((char *) &uszSendData[inDataCnt], "%06ld", wub_bcd_2_long(srTransPara->szOrgInvoiceNo, 3));
        }
        else	{
			
			vdDebug_LogPrintf("inPackIsoFunc62 KKKKKK");
            sprintf((char *) &uszSendData[inDataCnt], "%06ld", wub_bcd_2_long(srTransPara->szInvoiceNo, 3));
        	}
    }
    inDataCnt += 6;

    //memset(baData62, 0x00, sizeof (baData62));
    //memcpy(baData62, uszSendData, inDataCnt);

    vdMyEZLib_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackIsoFunc62 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field62", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

int inPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszSendData) {

    int inPacketCnt = 0;
    int inResult;
    int inTranCardType;
    ACCUM_REC srAccumRec;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szTemplate[100], szPacket[100 + 1];
    BYTE byMPUOper[3 + 1];


    memset(szPacket, 0, sizeof (szPacket));
    inDataCnt = 0;

    vdDebug_LogPrintf("Test1111");
    memset(&srAccumRec, 0x00, sizeof (ACCUM_REC));
    if ((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR) {
        vdDebug_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return ST_ERROR;
    }
    vdDebug_LogPrintf("Test2222");

#if 0	// origina code 02082019
    //0 is for Credit type, 1 is for debit type
    inTranCardType = 0;
#else
	// for testing fix for case #409 - All MPU cards type need to check
	if(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 13
    #ifdef TOPUP_RELOAD
    || srTransRec.HDTid == 20 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23
    #endif
		)
	{
		inTranCardType = 1; // save to debit accum struct for MPU hosts
	}
	else
		inTranCardType = 0; // will be saved to credit accum struct
#endif
	
    vdDebug_LogPrintf("**inPackIsoFunc63 START**byTransType[%d]Sale[%d]Refund[%d]", srTransPara->byTransType, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
    vdDebug_LogPrintf("inPackIsoFunc63 HDTid[%d]:inCardType[%d]", srTransRec.HDTid, srTransRec.inCardType);

    if (srTransPara->byTransType == SETTLE || srTransPara->byTransType == CLS_BATCH)
	{
vdDebug_LogPrintf("Test3333");

        if ((srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount) == 0 && 
			(srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount) == 0)
		{
vdDebug_LogPrintf("Test4444");		
            /*Cash Adv only application*/
            if (TRUE == fGetCashAdvAppFlag()) 
			{
                /* Cash Adv */
                memset(szTemplate, 0x00, sizeof (szTemplate));
                sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount);
                strcpy(szPacket, szTemplate);
                inPacketCnt += 3;
                //format amount 10+2
                sprintf(szTemplate, "%012.0f", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount));
                strcat(szPacket, szTemplate);
                inPacketCnt += 12;

                memcpy(&szPacket[15], "000000000000000", 15);
                inPacketCnt += 15;

                memset(szTemplate, 0x00, sizeof (szTemplate));
                sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].usEMVTCCount);
                strcat(&szPacket[inPacketCnt], szTemplate);
                inPacketCnt += 3;
            } 
			else 
			{
				vdDebug_LogPrintf("Test5555");		
			
                strcpy(szPacket, "000000000000000");
                strcat(szPacket, "000000000000000");
                strcat(szPacket, "000");
                inPacketCnt += 33;
				#if 0
				/*now MPU settle all set as Settle*/
				if (srTransPara->byTransType == SETTLE && 
					(srTransPara->HDTid == 17 || 
					srTransPara->HDTid == 18 || 
					srTransPara->HDTid == 19))
				{
        			inCTOS_GetOperCode(&szPacket[inPacketCnt], &inPacketCnt);
				}
				#endif

            }

        } 
		else 
		{
			// old host implemention for DE63
			#if 0
            /* SALE */
            memset(szTemplate, 0x00, sizeof (szTemplate));
            sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
            strcpy(szPacket, szTemplate);
            inPacketCnt += 3;
            //format amount 10+2
            sprintf(szTemplate, "%012.0f", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount));
            strcat(szPacket, szTemplate);
            inPacketCnt += 12;


            /* REFUND */
            memset(szTemplate, 0x00, sizeof (szTemplate));
            sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
            strcat(szPacket, szTemplate);
            inPacketCnt += 3;
            //format amount 10+2
            sprintf(szTemplate, "%012.0f", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount));
            strcat(szPacket, szTemplate);
            inPacketCnt += 12;

            memset(szTemplate, 0x00, sizeof (szTemplate));
            sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].usEMVTCCount);
            strcat(&szPacket[inPacketCnt], szTemplate);
            inPacketCnt += 3;
			#else 
			
			//New host implemention for DE63
            /* SALE */
            memset(szTemplate, 0x00, sizeof (szTemplate));
            sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount);
            strcpy(szPacket, szTemplate);
            inPacketCnt += 3;
            //format amount 10+2
            sprintf(szTemplate, "%012.0f", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount));
            strcat(szPacket, szTemplate);
            inPacketCnt += 12;


            /* REFUND */
            memset(szTemplate, 0x00, sizeof (szTemplate));
            sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount);
            strcat(szPacket, szTemplate);
            inPacketCnt += 3;
			
            //format amount 10+2
            sprintf(szTemplate, "%012.0f", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount));
            strcat(szPacket, szTemplate);
            inPacketCnt += 12;

			// add ONS with dummy values - start
            memset(szTemplate, 0x00, sizeof (szTemplate));
            sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount);
            strcat(&szPacket[inPacketCnt], szTemplate);
            inPacketCnt += 3;

            sprintf(szTemplate, "%012.0f", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount));
            strcat(szPacket, szTemplate);
            inPacketCnt += 12;

            sprintf(szTemplate, "%03d", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPPreAuthCount);
            strcat(&szPacket[inPacketCnt], szTemplate);
            inPacketCnt += 3;

            sprintf(szTemplate, "%012.0f", (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount));
            strcat(szPacket, szTemplate);
            inPacketCnt += 12;			
			// add ONS - end
			
			#endif

        }
    }
    else if (srTransPara->byTransType == MPU_SIGNON) 
	{
        //        memcpy(&szPacket[inPacketCnt], "\x00\x00\x00", 3);

        //@@IBR ADD 20170131 now for MPU can send the value other than default "\x00\x00\x00"
        memset(szTemplate, 0x00, sizeof (szTemplate));
        memset(byMPUOper, 0x00, sizeof (byMPUOper));
        get_env("#MPUOPER", szTemplate, 6);
        wub_str_2_hex(szTemplate, byMPUOper, 6);
        memcpy(&szPacket[inPacketCnt], byMPUOper, 3);
        //@@IBR FINISH ADD 20170131
        inPacketCnt += 3;
    }
	else if (srTransPara->byTransType == MPU_SIGNOFF) 
   	{
        inCTOS_GetOperCode(&szPacket[inPacketCnt], &inPacketCnt);
        //    } else if (srTransPara->byTransType == MPU_SETTLE) {
    } 
	else if (srTransPara->byTransType == SETTLE && 
		(srTransPara->HDTid == 17 || srTransPara->HDTid == 18 || srTransPara->HDTid == 19)) 
	{
        inCTOS_GetOperCode(&szPacket[inPacketCnt], &inPacketCnt);
    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszSendData[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszSendData[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    vdDebug_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdDebug_LogPrintf("**inPackIsoFunc63 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field63", uszSendData, inDataCnt, 1);
    return inDataCnt;

}

int inPackIsoFunc64(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
    BYTE szMAC[64 + 1];
    inDataCnt = 0;
    int inCheckData = 0;
    char szTemp[25 + 1], szOut[25 + 1];
    CTOS_RTC SetRTC;
    BYTE byKey[16 + 1];
    BYTE byVector[8 + 1];
    int inPANLen = 0;
    int inDataLen = 0;
    int inOutLen;

	vdDebug_LogPrintf("**inPackIsoFunc64 START**");
	
	#if 0
    /*Calc MAC here, pad 8 bytes 0x00 first*/
    //    if (srTransPara->byTransType == SALE) {
    memset(szMAC, 0x00, sizeof (szMAC));
    memset(szMacData, 0x00, sizeof (szMacData));
    inMacDataCnt = 0;

    //MTI
    memset(szTemp, 0x00, sizeof (szTemp));
    wub_hex_2_str(srTransPara->szMassageType, szTemp, MTI_BCD_SIZE);
    memcpy(&szMacData[inMacDataCnt], szTemp, MTI_ASC_SIZE);
    inMacDataCnt += MTI_ASC_SIZE;
    strcat(szMacData, " ");
    inMacDataCnt += 1;

    //BIT2 - PAN
    memset(szTemp, 0x00, sizeof (szTemp));
    inPANLen = strlen(srTransPara->szPAN);
    sprintf(szTemp, "%d%s", (inPANLen / 10 * 16) + (inPANLen % 10), srTransPara->szPAN);
    strcat(&szMacData[inMacDataCnt], szTemp);
    inMacDataCnt += strlen(szTemp);
    strcat(szMacData, " ");
    inMacDataCnt += 1;

    //BIT4 - AMT
    memset(szTemp, 0x00, sizeof (szTemp));
    wub_hex_2_str(srTransPara->szTotalAmount, szTemp, AMT_BCD_SIZE);
    memcpy(&szMacData[inMacDataCnt], szTemp, AMT_ASC_SIZE);
    inMacDataCnt += AMT_ASC_SIZE;
    strcat(szMacData, " ");
    inMacDataCnt += 1;

    //BIT7 - DATE_TIME
    CTOS_RTCGet(&SetRTC);
    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);
    memcpy(&szMacData[inMacDataCnt], szTemp, 4);
    inMacDataCnt += 4;

    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
    memcpy(&szMacData[inMacDataCnt], szTemp, 6);
    inMacDataCnt += 6;
    strcat(szMacData, " ");
    inMacDataCnt += 1;

    //BIT11 - STAN
    memset(szTemp, 0x00, sizeof (szTemp));
    sprintf(szTemp, "%06ld", srTransPara->ulTraceNum);
    memcpy(&szMacData[inMacDataCnt], szTemp, 6);
    inMacDataCnt += 6;
    strcat(szMacData, " ");
    inMacDataCnt += 1;

    //BIT41 - TID
    memset(szTemp, 0x00, sizeof (szTemp));
    memset(szOut, 0x00, sizeof (szOut));
    memcpy(szTemp, srTransPara->szTID, 8);
    inOutLen = 0;
    inCheckStringMAC(szTemp, 8, &szOut, &inOutLen);
    memcpy(&szMacData[inMacDataCnt], szOut, inOutLen);
    inMacDataCnt += inOutLen;

    //BIT61
    if (memcmp(&baData61[0], "\x00\x00", 2) != 0) {
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szOut, 0x00, sizeof (szOut));
        wub_hex_2_str(baData61, szTemp, 2);
        memcpy(&szMacData[inMacDataCnt], szTemp, 4);
        inMacDataCnt += 4;

        inDataLen = 0;
        inDataLen = atoi(szTemp);
        inOutLen = 0;

        inCheckStringMAC(&baData61[2], inDataLen, &szOut, &inOutLen);
        memcpy(&szMacData[inMacDataCnt], szOut, inOutLen);
        inMacDataCnt += inOutLen;
    }

    //BIT62
    if (memcmp(&baData62[0], "\x00\x00", 2) != 0) {
        memset(szTemp, 0x00, sizeof (szTemp));
        memset(szOut, 0x00, sizeof (szOut));
        wub_hex_2_str(baData62, szTemp, 2);
        memcpy(&szMacData[inMacDataCnt], szTemp, 4);
        inMacDataCnt += 4;

        inDataLen = 0;
        inDataLen = atoi(szTemp);
        inOutLen = 0;

        inCheckStringMAC(&baData62[2], inDataLen, &szOut, &inOutLen);
        memcpy(&szMacData[inMacDataCnt], szOut, inOutLen);
        inMacDataCnt += inOutLen;
    }

    wub_toupper(&szMacData);

    if ((inMacDataCnt % 8) != 0) {
        while (1) {
            inMacDataCnt++;

            if ((inMacDataCnt % 8) == 0)
                break;
        }
    }

    inFmtPad(szMacData, inMacDataCnt, '0');

    memset(byVector, 0x00, sizeof (byVector));
    memcpy(byVector, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
    DebugAddHEX("szMacData", szMacData, inMacDataCnt);
    //        vdEncrypt3DES(CBC_MODE, szMacData, inMacDataCnt, byKey, szMAC, byVector);
    //        inCalcMAC_CBC(szMacData,inMacDataCnt, byVector, szMAC);

    inCalculateMAC(szMacData, inMacDataCnt, szMAC);
    DebugAddHEX("szMAC", szMAC, inMacDataCnt);
    memcpy((BYTE *) & uszSendData[inDataCnt], szMAC, 8);
    //    } else {
    //        memcpy((BYTE *) & uszSendData[inDataCnt], "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
    //    }

	#endif

	/*Gen MAC buffer*/
	inMacDataCnt = inMPU_GenMACBuffer(srTransPara, szMacData, 0);
	DebugAddHEX("szMacData", szMacData, inMacDataCnt);
	inCalculateMAC(szMacData, inMacDataCnt, szMAC);
	
	memcpy((BYTE *)&uszSendData[inDataCnt], szMAC, 8);
	
    inDataCnt += 8;
    vdDebug_LogPrintf("  PACK_LEN%d", inDataCnt);
    vdDebug_LogPrintf("**inPackIsoFunc64 END**");
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field64", uszSendData, inDataCnt, 1);
    return inDataCnt;
}

int inUnPackIsoFunc02(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) 
{
#ifdef ECR_PREAUTH_AND_COMP    
    //int inLen = 0;
    int inRealLen = 0;
    char szLen[4+1];
    
    BYTE szAIIC[20+1];
    
    vdDebug_LogPrintf("=====inUnPackIsoFunc02=====");
    
    inDataCnt = 0;
    
    DebugAddHEX("uszUnPackBuf", uszUnPackBuf, 20);
    
    memset(szLen, 0x00, sizeof(szLen));
    wub_hex_2_str(&uszUnPackBuf[0], szLen, 2);
    szLen[2] = 0x00;
    srTransPara->byPanLen = atoi(szLen);
	wub_hex_2_str(&uszUnPackBuf[1], srTransPara->szPAN, (srTransPara->byPanLen+1)/2);

	srTransPara->szPAN[srTransPara->byPanLen]=0x00;
    vdDebug_LogPrintf("srTransPara->szPAN:%s", srTransPara->szPAN);
	
    memcpy(szAIIC, (char *) uszUnPackBuf, DATE_BCD_SIZE);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field02", uszUnPackBuf, DATE_BCD_SIZE, 1);

#endif

    return ST_SUCCESS;
}

int inUnPackIsoFunc07(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) 
{
	BYTE baTxnDT[DATE_BCD_SIZE + TIME_BCD_SIZE + 1];

	vdDebug_LogPrintf("**inUnPackIsoFunc07 START**");

	memset(baTxnDT, 0x00, sizeof(baTxnDT));
	
    memcpy(baTxnDT, (char *) uszUnPackBuf, DATE_BCD_SIZE + TIME_BCD_SIZE);

	/*MPU MAC store DE07 first*/
	memcpy(baMPU_DE07, baTxnDT, 5);
	DebugAddHEX("baMPU_DE07", baMPU_DE07, 5);
	
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field07", uszUnPackBuf, DATE_BCD_SIZE + TIME_BCD_SIZE, 1);
	
    return ST_SUCCESS;
}

int inUnPackIsoFunc11(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    unsigned char szSTAN[6 + 1];

    memset(szSTAN, 0x00, sizeof (szSTAN));

    wub_hex_2_str(uszUnPackBuf, szSTAN, 3);

    srTransPara->ulTraceNum = atol(szSTAN);

    vdDebug_LogPrintf("inUnPackIsoFunc11(%s) [%d]", szSTAN, srTransPara->ulTraceNum);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field11", uszUnPackBuf, 3, 1);
    return ST_SUCCESS;
}

int inUnPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    #ifdef TOPUP_RELOAD
    if(srTransPara->HDTid == 20)
    {
        memcpy(srTransPara->szTime, (char *) uszUnPackBuf+3, TIME_BCD_SIZE);
    }
    else
        memcpy(srTransPara->szTime, (char *) uszUnPackBuf, TIME_BCD_SIZE);
    #else
    memcpy(srTransPara->szTime, (char *) uszUnPackBuf, TIME_BCD_SIZE);
    #endif
    
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field12", uszUnPackBuf, TIME_BCD_SIZE, 1);
    return ST_SUCCESS;
}

int inUnPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    memcpy(srTransPara->szDate, (char *) uszUnPackBuf, DATE_BCD_SIZE);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field13", uszUnPackBuf, DATE_BCD_SIZE, 1);
    return ST_SUCCESS;
}

int inUnPackIsoFunc14(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
#ifdef ECR_PREAUTH_AND_COMP    	
    memcpy(srTransPara->szExpireDate, (char *) uszUnPackBuf, DATE_BCD_SIZE);
	DebugAddHEX("srTransPara->szExpireDate",srTransPara->szExpireDate,DATE_BCD_SIZE);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field14", uszUnPackBuf, DATE_BCD_SIZE, 1);
#endif
    return ST_SUCCESS;
}

int inUnPackIsoFunc15(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    //memcpy(srTransPara->szDate, (char *) uszUnPackBuf, DATE_BCD_SIZE);
    DebugAddHEX("UN-PACK DE15", uszUnPackBuf, DATE_BCD_SIZE);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field13", uszUnPackBuf, DATE_BCD_SIZE, 1);
    return ST_SUCCESS;
}



/*For MPU --- Acquiring Institution Indentification code*/
int inUnPackIsoFunc32(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{

	int inLen = 0;
    int inRealLen = 0;
    char szLen[4+1];
    
	BYTE szAIIC[20+1];

	vdDebug_LogPrintf("=====inUnPackIsoFunc32=====");
	
    inDataCnt = 0;

	DebugAddHEX("uszUnPackBuf", uszUnPackBuf, 8);
	
    memset(szLen, 0x00, sizeof(szLen));
    wub_hex_2_str(&uszUnPackBuf[0], szLen, 2);
	szLen[2] = 0x00;
    inLen = atoi(szLen);
    inLen += 1;
	
	vdDebug_LogPrintf("inLen[%d]", inLen);
	
    inRealLen = inLen/2;
    inDataCnt += 2;

    vdDebug_LogPrintf("inRealLen[%d]", inRealLen);
    memset(szAIIC, 0x00, sizeof(szAIIC));
    memcpy(szAIIC, &uszUnPackBuf[inDataCnt], inRealLen);
    inDataCnt += inRealLen;
    
    DebugAddHEX("DE32", szAIIC, inRealLen);
	
    memcpy(szAIIC, (char *) uszUnPackBuf, DATE_BCD_SIZE);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field32", uszUnPackBuf, DATE_BCD_SIZE, 1);
	
    return ST_SUCCESS;
}


/*For MPU --- Foewarding Institution Indentification code*/
int inUnPackIsoFunc33(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{

	int inLen = 0;
    int inRealLen = 0;
    char szLen[4+1];
    
	BYTE szFIIC[20+1];
    
    inDataCnt = 0;

	vdDebug_LogPrintf("=====inUnPackIsoFunc33=====");

	DebugAddHEX("uszUnPackBuf", uszUnPackBuf, 8);
	
    memset(szLen, 0x00, sizeof(szLen));
    wub_hex_2_str(&uszUnPackBuf[0], szLen, 2);
	szLen[2] = 0x00;
    inLen = atoi(szLen);
    inLen += 1;
	
	vdDebug_LogPrintf("inLen[%d]", inLen);
	
    inRealLen = inLen/2;
    inDataCnt += 2;
    
    memset(szFIIC, 0x00, sizeof(szFIIC));
    memcpy(szFIIC, &uszUnPackBuf[inDataCnt], inRealLen);
    inDataCnt += inRealLen;
    
    DebugAddHEX("DE33", szFIIC, inRealLen);
	
    memcpy(szFIIC, (char *) uszUnPackBuf, DATE_BCD_SIZE);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field33", uszUnPackBuf, DATE_BCD_SIZE, 1);
	
    return ST_SUCCESS;
}


int inUnPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    memcpy(srTransPara->szRRN, (char *) uszUnPackBuf, 12);
	vdDebug_LogPrintf("inUnPackIsoFunc37(%s)", srTransPara->szRRN);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field37", uszUnPackBuf, 12, 0);
    return ST_SUCCESS;
}

int inUnPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    memcpy(srTransPara->szAuthCode, (char *) uszUnPackBuf, AUTH_CODE_DIGITS);
	vdDebug_LogPrintf("inUnPackIsoFunc38(%s)", srTransPara->szAuthCode);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field38", uszUnPackBuf, AUTH_CODE_DIGITS, 0);
    return ST_SUCCESS;
}

int inUnPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    memcpy(srTransPara->szRespCode, (char *) uszUnPackBuf, RESP_CODE_SIZE);
    vdDebug_LogPrintf("inUnPackIsoFunc39(%s)", srTransPara->szRespCode);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field39", uszUnPackBuf, RESP_CODE_SIZE, 0);
    return ST_SUCCESS;
}

int inUnPackIsoFunc41(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    memcpy(srTransPara->szTID, (char *) uszUnPackBuf, TERMINAL_ID_BYTES);
    vdDebug_LogPrintf("inUnPackIsoFunc41(%s)", srTransPara->szTID);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field41", uszUnPackBuf, TERMINAL_ID_BYTES, 0);
    return ST_SUCCESS;
}


int inUnPackIsoFunc43(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {

	BYTE szAccpetName[40+1];

	memset(szAccpetName, 0x00, sizeof(szAccpetName));
    memcpy(szAccpetName, (char *) uszUnPackBuf, 40);
    vdDebug_LogPrintf("inUnPackIsoFunc43(%s)", szAccpetName);
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field43", uszUnPackBuf, 40, 0);
    return ST_SUCCESS;
}


int inUnPackIsoFunc44(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    int inLen = 0;
    int inRealLen = 0;
    char szLen[4+1];
	BYTE szAddRespData[25 + 1];
    
    inDataCnt = 0;
    
    memset(szLen, 0x00, sizeof(szLen));
    wub_hex_2_str(&uszUnPackBuf[0], szLen, 2);
	szLen[2] = 0x00;
    inLen = atoi(szLen);
    inLen += 1;
    
    inRealLen = inLen/2;
    inDataCnt += 2;
    
    memset(szAddRespData, 0x00, sizeof(szAddRespData));
    memcpy(szAddRespData, &uszUnPackBuf[inDataCnt], inRealLen);
    inDataCnt += inRealLen;
    
    DebugAddHEX("szAddRespData", szAddRespData, inRealLen);

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field44", uszUnPackBuf, inLen + 2, 1);
	
    return ST_SUCCESS;
}

int inUnPackIsoFunc48(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) 
{
    int inIndex=0, inLen=0;
    char szCurrCode[6+1], szSignValue[2+1], szBalAmount[24+1], szData[256+1], szKCV[6+1];
    
    //memset(szTempAmount, 0x00, sizeof(szTempAmount));
    
    vdDebug_LogPrintf("**inUnPackIsoFunc48 START");
    
    if(srTransPara->byPackType == BALANCE_ENQUIRY || srTransPara->byPackType == TOPUP || srTransPara->byPackType == RELOAD
	|| srTransPara->byPackType == TOPUP_RELOAD_LOGON)
    {
        vdDebug_LogPrintf("**inUnPackIsoFunc48 uszUnPackBuf[%02X %02X]",  uszUnPackBuf[0], uszUnPackBuf[1]);
        
        inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
        inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;
        
        if(inLen<=0)
            return(ST_ERROR); 

		//303031 303031 31 inIndex+=7;
		//303032 303033 373434 inIndex+=9;
		//303033 303032 3033 inIndex+=8; 
		//303231 303038 3131323039343839 inIndex+=14;
		//index=38
		//303235 303033 313034 - Currency Code inIndex+=9;
		//index=47
		//303236 303031 43 - Sign Value  inIndex+=7;
		
		//313030 303034 30303031  inIndex+=10;
		//313031 303034 30303031  inIndex+=10;
        //00100110020037440030020302100811209489025003104026001C10000400011010040001
		memset(szCurrCode, 0x00, sizeof(szCurrCode));
		memset(szSignValue, 0x00, sizeof(szSignValue));
		memset(szBalAmount, 0x00, sizeof(szBalAmount));
		memset(szKCV, 0, sizeof(szKCV));
        if (inLen > 0) 
        {	
			#if 0
            memcpy(szTempAmount,&uszUnPackBuf[11], 12);
            vdDebug_LogPrintf("**inUnPackIsoFunc54 inLen[%d] szTempAmount[%s]",inLen,szTempAmount);
            wub_str_2_hex(szTempAmount,srTransPara->szTipAmount,12);			 
            
            wub_hex_2_str(srTransPara->szTipAmount, szTempAmount, 6);
            vdDebug_LogPrintf("**szTempAmount[%s] szTipAmount[%s]",szTempAmount,srTransPara->szTipAmount);
			#else		
			DebugAddHEX("3.new inUnPackIsoFunc48", &uszUnPackBuf[2], inLen);

			memset(szData, 0, sizeof(szData));
			memcpy(szData, &uszUnPackBuf[2], inLen); 
			//memcpy(szCurrCode, &uszUnPackBuf[44+2], 3);
			//memcpy(szSignValue, &uszUnPackBuf[53+2], 1);
			sprintf(szCurrCode, "%s", getTagVal(szData, TAG_025)); /*currency code*/
			vdDebug_LogPrintf("szCurrCode");
			sprintf(szSignValue, "%s", getTagVal(szData, TAG_026)); /*sign value*/
			vdDebug_LogPrintf("szSignValue");
			sprintf(szBalAmount, "%s", getTagVal(szData, TAG_103)); /*balance amount*/
			vdDebug_LogPrintf("szBalAmount");

			sprintf(szKCV, "%s", getTagVal(szData, TAG_090));
			vdDebug_LogPrintf("szKCV");

            if(memcmp(szCurrCode, "N/A", 3) != 0)
            {
			    memcpy(srTransPara->szCurrCode1, szCurrCode, 3);
			
                if(strlen(szCurrCode) > 3)
                    memcpy(srTransPara->szCurrCode2, szCurrCode+3, 3);
				
            }
			vdDebug_LogPrintf("szCurrCode[%s]", szCurrCode);
			
            if(memcmp(szSignValue, "N/A", 3) != 0)
            {
			    memcpy(srTransPara->szSign1, szSignValue, 1);
			
                if(strlen(szSignValue) > 1)
                    memcpy(srTransPara->szSign2, szSignValue+1, 1);
            }
            vdDebug_LogPrintf("szSignValue[%s]", szSignValue);
			
            if(memcmp(szBalAmount, "N/A", 3) != 0)
            {
				vdDebug_LogPrintf("srTransPara->szCurrCode1[%s]", srTransPara->szCurrCode1);
				if(memcmp(srTransPara->szCurrCode1, "104", 3) == 0)
				{
					memcpy(srTransPara->szBalAmount1, szBalAmount, 10);
					vdDebug_LogPrintf("srTransPara->szBalAmount1[%s]", srTransPara->szBalAmount1);
				}
				else	
			        memcpy(srTransPara->szBalAmount1, szBalAmount, 12);

			    vdDebug_LogPrintf("szBalAmount[%s]", szBalAmount);
                if(strlen(szBalAmount) > 12)
                {
					if(memcmp(srTransPara->szCurrCode2, "104", 3) == 0)
						memcpy(srTransPara->szBalAmount2, szBalAmount+12, 10);
					else
                        memcpy(srTransPara->szBalAmount2, szBalAmount+12, 12);
                }
            }
			vdDebug_LogPrintf("szBalAmount[%s]", szBalAmount);

			
            if(memcmp(szKCV, "N/A", 3) != 0)
            {
			    memcpy(srTransPara->szKCV, szKCV, 6);
            }
			vdDebug_LogPrintf("szKCV[%s]", szKCV);
 		    #endif
        }
    }
    return ST_SUCCESS;
}

int inUnPackIsoFunc49(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) 
{
    int inLen = 0;
    int inRealLen = 0;
    char szLen[4+1];
	BYTE baCurrCode[4 + 1];
    
    inDataCnt = 0;
    
    memset(baCurrCode, 0x00, sizeof(baCurrCode));
	if(srTransPara->byPackType == BALANCE_ENQUIRY || srTransPara->byPackType == TOPUP || srTransPara->byPackType == RELOAD)
	{
        memcpy(baCurrCode, &uszUnPackBuf[inDataCnt], 3);
        inDataCnt += 3;
	}
	else
	{
        memcpy(baCurrCode, &uszUnPackBuf[inDataCnt], 2);
        inDataCnt += 2;
	}
	
    DebugAddHEX("baCurrCode", baCurrCode, inDataCnt);

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field49", uszUnPackBuf, inLen, 1);
	
    return ST_SUCCESS;
}

/*Cash Advance Fee*/
int inUnPackIsoFunc54(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) 
{
	int inIndex = 2, inLen=0;
	char szTemp[9+1], szTempAmount[12+1];

	memset(szTempAmount, 0x00, sizeof(szTempAmount));

	
    vdDebug_LogPrintf("**inUnPackIsoFunc54 START");

    if(srTransPara->byPackType == CASH_ADVANCE)
    {
    	 vdDebug_LogPrintf("**inUnPackIsoFunc54 uszUnPackBuf[%02X %02X]",  uszUnPackBuf[0], uszUnPackBuf[1]);
		 
        inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
        inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;
		
        if(inLen<=0)
            return(ST_ERROR); 
		
		if (inLen > 0) 
			{			  
			  memcpy(szTempAmount,&uszUnPackBuf[11], 12);
			  vdDebug_LogPrintf("**inUnPackIsoFunc54 inLen[%d] szTempAmount[%s]",inLen,szTempAmount);
			  wub_str_2_hex(szTempAmount,srTransPara->szTipAmount,12);			 

			  wub_hex_2_str(srTransPara->szTipAmount, szTempAmount, 6);
			  vdDebug_LogPrintf("**szTempAmount[%s] szTipAmount[%s]",szTempAmount,srTransPara->szTipAmount);
			  
			}
    }
	
    return ST_SUCCESS;
}

int inUnPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    int inLen, inTotalLen, inTagLen;
    unsigned short usTag;

    if ((srTransPara->byEntryMode == CARD_ENTRY_MSR) ||
            (srTransPara->byEntryMode == CARD_ENTRY_MANUAL) ||
            (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)) {
        return ST_SUCCESS;
    }

    inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) *100;
    inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;


    vdDebug_LogPrintf("**inEDC_EMV_UnPackData55(%d) START** uszUnPackBuf[%02X %02X]", inLen, uszUnPackBuf[0], uszUnPackBuf[1]);
    DebugAddHEX("DE 55", uszUnPackBuf, inLen + 2);

    if (inLen > 0) {
        memset(srTransPara->stEMVinfo.T8A, 0x00, sizeof (srTransPara->stEMVinfo.T8A));
        memcpy(srTransPara->stEMVinfo.T8A, srTransPara->szRespCode, strlen(srTransPara->szRespCode));

        for (inTotalLen = 2; inTotalLen < inLen;) {
            usTag = (unsigned short) uszUnPackBuf[inTotalLen] * 256;

            if ((uszUnPackBuf[inTotalLen++] & 0x1F) == 0x1F)
                usTag += ((unsigned short) uszUnPackBuf[inTotalLen++]);

            vdDebug_LogPrintf("usTag[%X]", usTag);
            switch (usTag) {
                case 0x9100:
                    memset(srTransPara->stEMVinfo.T91, 0x00, sizeof (srTransPara->stEMVinfo.T91));
                    srTransPara->stEMVinfo.T91Len = (unsigned short) uszUnPackBuf[inTotalLen++];
                    memcpy(srTransPara->stEMVinfo.T91, (char *) &uszUnPackBuf[inTotalLen], srTransPara->stEMVinfo.T91Len);
                    inTotalLen += srTransPara->stEMVinfo.T91Len;
                    vdDebug_LogPrintf(". 91Len(%d)", srTransPara->stEMVinfo.T91Len);
                    DebugAddHEX("Tag 91", srTransPara->stEMVinfo.T91, srTransPara->stEMVinfo.T91Len);
                    ushCTOS_EMV_NewTxnDataSet(TAG_91_ARPC, srTransPara->stEMVinfo.T91Len, srTransPara->stEMVinfo.T91);
                    break;
                case 0x7100:
                    memset(srTransPara->stEMVinfo.T71, 0x00, sizeof (srTransPara->stEMVinfo.T71));
                    srTransPara->stEMVinfo.T71Len = (unsigned short) uszUnPackBuf[inTotalLen++];
                    srTransPara->stEMVinfo.T71Len += 2;
                    memcpy(&srTransPara->stEMVinfo.T71[0], (char *) &uszUnPackBuf[inTotalLen - 2], srTransPara->stEMVinfo.T71Len);
                    inTotalLen += srTransPara->stEMVinfo.T71Len - 2;
                    vdDebug_LogPrintf(". 71Len(%d)", srTransPara->stEMVinfo.T71Len);
                    DebugAddHEX("Tag 71", srTransPara->stEMVinfo.T71, srTransPara->stEMVinfo.T71Len);
                    ushCTOS_EMV_NewTxnDataSet(TAG_71, srTransPara->stEMVinfo.T71Len, srTransPara->stEMVinfo.T71);
                    break;
                case 0x7200:
                    memset(srTransPara->stEMVinfo.T72, 0x00, sizeof (srTransPara->stEMVinfo.T72));
                    srTransPara->stEMVinfo.T72Len = (unsigned short) uszUnPackBuf[inTotalLen++];
                    srTransPara->stEMVinfo.T72Len += 2;
                    memcpy(&srTransPara->stEMVinfo.T72[0], (char *) &uszUnPackBuf[inTotalLen - 2], srTransPara->stEMVinfo.T72Len);
                    inTotalLen += srTransPara->stEMVinfo.T72Len - 2;
                    vdDebug_LogPrintf(". 72Len(%d)", srTransPara->stEMVinfo.T72Len);
                    DebugAddHEX("Tag 72", srTransPara->stEMVinfo.T72, srTransPara->stEMVinfo.T72Len);
                    ushCTOS_EMV_NewTxnDataSet(TAG_72, srTransPara->stEMVinfo.T72Len, srTransPara->stEMVinfo.T72);
                    break;
                default:
                    vdDebug_LogPrintf("**inEDC_EMV_UnPackData55(%X) Err**", usTag);
                    inTagLen = (unsigned short) uszUnPackBuf[inTotalLen++];
                    inTotalLen += inTagLen;
                    vdDebug_LogPrintf("**inTagLen(%d) inTotalLen[%d] Err**", inTagLen, inTotalLen);
                    break;
            }
        }
    } else {
    
    vdDebug_LogPrintf("**inEDC_EMV_UnPackData55*NO DE55 DATA**");
		// commnetd to fix issue on comm error after void txn with RC00 response.
        //inCTOS_inDisconnect();
        //return (ST_ERROR);
    }
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field55", uszUnPackBuf, inLen + 2, 1);
    return ST_SUCCESS;
}

int inUnPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    //BYTE szSignOnRespData[128];
    //int inSignOnRespLen = 60;
    int inLen, inTotalLen, inTagLen;
    int inPos = 0;

    BYTE szHexLen[2];
    char szStrLen[4+1];
    BYTE szVerNum[2];
    BYTE szTMKRefNum[8 + 1];
    BYTE szTPK[16];
    BYTE szTAK[16];
    BYTE szWEK[16];
    int ret = 0;

    inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) *100;
    inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;
	
	vdDebug_LogPrintf("inUnPackIsoFunc57 HDTid:%d",srTransPara->HDTid);

    //memset(szSignOnRespData, 0x00, sizeof(szSignOnRespData));
    //memcpy(szSignOnRespData, &uszUnPackBuf[2], inSignOnRespLen);
    memset(szHexLen, 0x00, sizeof (szHexLen));
    memset(szVerNum, 0x00, sizeof (szVerNum));
    memset(szTMKRefNum, 0x00, sizeof (szTMKRefNum));
    memset(szTPK, 0x00, sizeof (szTPK));
    memset(szTAK, 0x00, sizeof (szTAK));
    memset(szWEK, 0x00, sizeof (szWEK));
    
    if(srTransPara->byTransType == IPP_SIGN_ON){
        memset(szStrLen, 0x00, sizeof(szStrLen));
        memcpy(szHexLen, &uszUnPackBuf[inPos], 2);
        wub_hex_2_str(szHexLen, szStrLen, 2);
        inPos = 2;
        
        
        if(atoi(szStrLen) == 18){
            memcpy(szVerNum, &uszUnPackBuf[inPos], 2);                          inPos += 2;
            memcpy(szTPK, &uszUnPackBuf[inPos], 16);                            inPos += 16;
        } else {
            memcpy(szVerNum, &uszUnPackBuf[inPos], 2);                          inPos += 2;
            memcpy(szTPK, &uszUnPackBuf[inPos], 8);                             inPos += 8;
            memcpy(szTPK + 8, &uszUnPackBuf[inPos], 8);                         inPos += 8;
            
        }
        DebugAddHEX("TPK IPP", szTPK, 16);
        ushIPP_WriteEncTPK(szTPK, 16);
        
        //MAC Key here
        
        //Line Key Here
    } 
	
	else 
    {
    	 /*Version Number*/
    		inPos = 2;
   			 memcpy(szVerNum, &uszUnPackBuf[inPos], 2);
    
		if (inLen > 16) 
	{
		/*TPK*/
		inPos += 2;
		memcpy(szTPK, &uszUnPackBuf[inPos], 16);
		DebugAddHEX("szTPK", szTPK, 16);
		// ushCBB_WriteEncTPK(szTPK, 16);
	}

		// For Finexus host
		if(fGetCashAdvAppFlag() == TRUE)
		{
				// TO SAVE TPK FOR VISA/ MC KEYINDEX AND UPI
				#ifdef PIN_CHANGE_ENABLE
				if(srTransPara->HDTid == 21 || srTransPara->HDTid == 1)
					ret = ushCAV_WriteEncPIK2(szTPK, 16, "0000", 4); // for VISA/MC TPK
				else
					ret = ushCAV_WriteEncPIK(szTPK, 16, "0000", 4); // for UPI TPK
				#else
					ret = ushCAV_WriteEncPIK(szTPK, 16, "0000", 4); // for UPI TPK
				
				#endif
				
				if(ret != d_OK)
					return ST_ERROR;
		}
		else 
	    {
	    	#ifdef CBB_FIN_ROUTING				
			vdDebug_LogPrintf("inUnPackIsoFunc57 CBB_FIN_ROUTING");
			
				if(srTransPara->HDTid == 22)
					ushCBB_WriteEncTPKUSD(szTPK, 16);//ushUSD_WriteEncPIK2(szTPK, 16, "0000", 4); // for FIN-USD - mimic VISA/MC TPK		
				else if (srTransPara->HDTid == 23)							
					ushCBB_WriteEncTPKMMK(szTPK, 16);//ushMMK_WriteEncPIK2(szTPK, 16, "0000", 4); // for FIN-MMK - mimic VISA/MC TPK							
				else
					ushCBB_WriteEncTPK(szTPK, 16); //original code for MPU, MPU-UPI host.
					
			#else
				ushCBB_WriteEncTPK(szTPK, 16);
			#endif
		}
		
		if (inLen > 32) 
	   {
		/*TAK*/
			inPos += 16;
			memcpy(szTAK, &uszUnPackBuf[inPos], 16);
			DebugAddHEX("szTAK", szTAK, 16);
		}
		if (inLen > 48) 
	  {
		/*WEK*/
		inPos += 16;
		memcpy(szWEK, &uszUnPackBuf[inPos], 16);
		DebugAddHEX("szWEK", szWEK, 16);
	}
		
}
#if 0
{
        
        /*Version Number*/
    inPos = 2;
    memcpy(szVerNum, &uszUnPackBuf[inPos], 2);

    /*TPK*/
    inPos += 2;
    memcpy(szTPK, &uszUnPackBuf[inPos], 16);
    DebugAddHEX("szTPK", szTPK, 16);
//    ushCBB_WriteEncTPK(szTPK, 16);
    
    if(fGetCashAdvAppFlag() == TRUE){
        ret = ushCAV_WriteEncPIK(szTPK, 16, "0000", 4);
        if(ret != d_OK)
            return ST_ERROR;
    } else {
        ushCBB_WriteEncTPK(szTPK, 16);		

    }

    /*TAK*/
    inPos += 16;
    memcpy(szTAK, &uszUnPackBuf[inPos], 16);
    DebugAddHEX("szTAK", szTAK, 16);

    /*WEK*/
    inPos += 16;
    memcpy(szWEK, &uszUnPackBuf[inPos], 16);
    DebugAddHEX("szWEK", szWEK, 16);
    }

    
#endif 

    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field57", uszUnPackBuf, inLen + 2, 1);
    return ST_SUCCESS;
}

//@@IBR ADD 20161125
int inUnPackIsoFunc60(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) 
{
    int inLen = 0;
    int inRealLen = 0;
    char szLen[4+1];
    BYTE *szTemp;
    BYTE szTemp1[20+1];
    int inPos = 0;
    
    inDataCnt = 0;
//    BYTE szMessageType;
//    BYTE szBatch[3+1];
//    BYTE szNetCode[2+1];

    vdDebug_LogPrintf("inUnPackIsoFunc60(%s)", uszUnPackBuf);

    if(srTransPara->byPackType == QR_INQUIRY)
    {
		vdDebug_LogPrintf("inUnPackIsoFunc60(%s)", uszUnPackBuf);
		DebugAddHEX("DE60", uszUnPackBuf, 100);
		
		
		vdDebug_LogPrintf("Len(%02x%02x)", uszUnPackBuf[0], uszUnPackBuf[1]);
		
		memset(szLen, 0x00, sizeof(szLen));
		wub_hex_2_str(&uszUnPackBuf[0], szLen, 2);
		
		
		vdDebug_LogPrintf("Len(%02x%02x)", szLen[0], szLen[1]);
		szLen[5] = 0x00;
		
		vdDebug_LogPrintf("szLen(%s)", szLen);
		inLen = atoi(szLen);
		//inLen += 1;
		
		
		vdDebug_LogPrintf("inLen(%d)", inLen);
		
		inRealLen = inLen;
		
		vdDebug_LogPrintf("inLen(%d)", inLen);
		
		
		srTransPara->inQRDataLen = inRealLen;
		inDataCnt += 2;
		
		szTemp = malloc(inRealLen+1);
		memset(szTemp, 0x00, sizeof(inRealLen+1));
		memcpy(szTemp, &uszUnPackBuf[inDataCnt], inRealLen);
		
		inDataCnt += 4;
			
		memcpy(srTransPara->byQRData, &uszUnPackBuf[inDataCnt], inRealLen-4);
		inDataCnt += inRealLen;
    }
	else
	{
        memset(szLen, 0x00, sizeof(szLen));
        wub_hex_2_str(&uszUnPackBuf[0], szLen, 2);
    	szLen[2] = 0x00;
        inLen = atoi(szLen);
        inLen += 1;
        
        inRealLen = inLen/2;
        inDataCnt += 2;
        
        szTemp = malloc(inRealLen+1);
        memset(szTemp, 0x00, sizeof(inRealLen+1));
        memcpy(szTemp, &uszUnPackBuf[inDataCnt], inRealLen);
        inDataCnt += inRealLen;
        
        DebugAddHEX("DE60", szTemp, inRealLen);
        
        if(srTransPara->byTransType == SALE && srTransPara->fIsInstallment == TRUE)
        {
            inPos = 2;
            memcpy(srTransPara->stIPPinfo.szIPPSchemeID, uszUnPackBuf+inPos, 6);          inPos += 6;
            
            memset(szTemp1, 0x00, sizeof(szTemp1));
            memcpy(szTemp1, uszUnPackBuf+inPos, AMT_ASC_SIZE);                            inPos += AMT_ASC_SIZE;
            wub_str_2_hex(szTemp1, srTransPara->stIPPinfo.szIPPTotalAmount, AMT_ASC_SIZE);
            
            memcpy(srTransPara->stIPPinfo.szTransCurrency, uszUnPackBuf+inPos, 3);        inPos += 3;
            memcpy(srTransPara->stIPPinfo.szInstallmentTerms, uszUnPackBuf+inPos, 3);     inPos += 3;
            memcpy(srTransPara->stIPPinfo.szFreqInstallment, uszUnPackBuf+inPos, 1);      inPos += 1;
            
            memset(szTemp1, 0x00, sizeof(szTemp1));
            memcpy(szTemp1, uszUnPackBuf+inPos, AMT_ASC_SIZE);                            inPos += AMT_ASC_SIZE;
            wub_str_2_hex(szTemp1, srTransPara->stIPPinfo.szMonthlyAmt, AMT_ASC_SIZE);
            
            memcpy(srTransPara->stIPPinfo.szInterestRate, uszUnPackBuf+inPos, 12);        inPos += 12;
            
            memset(szTemp1, 0x00, sizeof(szTemp1));
            memcpy(szTemp1, uszUnPackBuf+inPos, AMT_ASC_SIZE);                            inPos += AMT_ASC_SIZE;
            wub_str_2_hex(szTemp1, srTransPara->stIPPinfo.szTotalInterest, AMT_ASC_SIZE);
            
            memset(szTemp1, 0x00, sizeof(szTemp1));
            memcpy(szTemp1, uszUnPackBuf+inPos, AMT_ASC_SIZE);                            inPos += AMT_ASC_SIZE;
            wub_str_2_hex(szTemp1, srTransPara->stIPPinfo.szHandlingFee, AMT_ASC_SIZE);
        }
	}
	
    if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
        inPrintISOfield("Field60", uszUnPackBuf, inLen + 2, 1);
    
    free(szTemp);
    return ST_SUCCESS;
}


int inUnPackIsoFunc62(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    extern TMK_RES_DATA TMKresponse;
    extern TWK_RES_DATA TWKresponse;
    extern TMK_RSA_RES_DATA TMKRSAresponse;
    extern TWK_RSA_RES_DATA TWKRSAresponse;

    int inIndex = 0;
    BYTE szTmp[32], szTmp2[16 + 1];
    BYTE byMAK[16 + 1], byVector[8 + 1];
    int inLen;
    BYTE szClearTMK[17], szClearTAK[17];
    int ret;
    inDataCnt = 0;
    BYTE byKCV[8 + 1];

	BYTE baEncPIK[16];
	BYTE baEncMAK[16];


	vdDebug_LogPrintf("**inUnPackIsoFunc62 **");

    if (srTransPara->byTransType == EFTSEC_TMK) {
        memset((void*) &TMKresponse, 0, sizeof (TMK_RES_DATA));
        memcpy((void*) &TMKresponse, &uszUnPackBuf[2], sizeof (TMK_RES_DATA));
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", &uszUnPackBuf[2], sizeof (TMK_RES_DATA), 0);
        return ST_SUCCESS;
    }

    if (srTransPara->byTransType == EFTSEC_TWK) {
        memset((void*) &TWKresponse, 0, sizeof (TWK_RES_DATA));
        memcpy((void*) &TWKresponse, &uszUnPackBuf[2], sizeof (TWK_RES_DATA));
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", &uszUnPackBuf[2], sizeof (TWK_RES_DATA), 0);
        return ST_SUCCESS;
    }

    if (srTransPara->byTransType == EFTSEC_TMK_RSA) {
        memset((void*) &TMKRSAresponse, 0, sizeof (TMK_RSA_RES_DATA));
        memcpy((void*) &TMKRSAresponse, &uszUnPackBuf[2], sizeof (TMK_RSA_RES_DATA));
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", &uszUnPackBuf[2], sizeof (TMK_RSA_RES_DATA), 0);
        return ST_SUCCESS;
    }

    if (srTransPara->byTransType == EFTSEC_TWK_RSA) {
        //		memset((void*)&TWKRSAresponse, 0, sizeof(TWK_RSA_RES_DATA));
        //		memcpy((void*)&TWKRSAresponse, &uszUnPackBuf[2], sizeof(TWK_RSA_RES_DATA));
        memset((void*) &TWKresponse, 0, sizeof (TWK_RES_DATA));
        memcpy((void*) &TWKresponse, &uszUnPackBuf[2], sizeof (TWK_RES_DATA));
        if (inCheckReversalTrans(srTransPara->byPackType) != d_OK)
            inPrintISOfield("Field62", &uszUnPackBuf[2], sizeof (TWK_RES_DATA), 0);
        return ST_SUCCESS;
    }

#ifdef TOPUP_RELOAD
    if (srTransPara->byTransType == TOPUP_RELOAD_LOGON) {
		char szRandKeyBCD[16+1];
		char szDataBCD[16+1];
		char szPINKey[32+1];
		char szKCV[16+1];

		inDataCnt=2;
		DebugAddHEX("TOPUP_RELOAD DE62", &uszUnPackBuf[inDataCnt], 32);

		memset(szRandKeyBCD, 0, sizeof(szRandKeyBCD));
		memset(szDataBCD, 0, sizeof(szDataBCD));
        memset(szTmp, 0, sizeof(szTmp));
		memset(szKCV, 0, sizeof(szKCV));
        memset(szPINKey, 0, sizeof(szPINKey));
		
		wub_str_2_hex(&uszUnPackBuf[inDataCnt], szDataBCD, 32);
        wub_str_2_hex(szRandomKey, szRandKeyBCD, 32);
		
        Decrypt3Des(szDataBCD, szRandKeyBCD, szTmp);
		Decrypt3Des(szDataBCD+8, szRandKeyBCD, szTmp+8);
		
		inDCTRead(20);
		memcpy(strDCT.szPINKey, szTmp, 16);
		inDCTSave(20);
		
		DebugAddHEX("PIN KEY", szTmp, 16);

        wub_hex_2_str(szTmp, szPINKey, 16);
		
		vdGenerateKCV(szPINKey, szKCV, 1); /*use */
		vdDebug_LogPrintf("szKCV:%s", szKCV);
		if(memcmp(srTransPara->szKCV, szKCV ,6) != 0)
			return ST_ERROR;
		
        return ST_SUCCESS;
    }
#endif


	// ill this be use also for FIN-USD AND FIN-MMK?
    if (srTransPara->byTransType == MPU_SIGNON){
		DebugAddHEX("MPU_SIGNON DE62", &uszUnPackBuf[inDataCnt], 42);
		
        	inDataCnt = 2;

		
		vdDebug_LogPrintf("**inUnPackIsoFunc62 srTransPara->HDTid [%d]", srTransPara->HDTid);


       	 inDCTRead(srTransPara->HDTid);
		memset(baEncPIK, 0x00, sizeof(baEncPIK));

		

		#ifdef CB_MPU_NH_MIGRATION
 		memcpy(baEncPIK, &uszUnPackBuf[inDataCnt], 8);		// for new host - Encrypted working Key should be 8 bytes.
 		memcpy(&baEncPIK[8], &uszUnPackBuf[inDataCnt], 8);	  
		#else
		memcpy(baEncPIK, &uszUnPackBuf[inDataCnt], 16); // for old host
		#endif


		inDataCnt += 16;
		
		DebugAddHEX("baEncPIK", baEncPIK, 16);
		DebugAddHEX("baEncPIK KCV", &uszUnPackBuf[inDataCnt], 4);
		#ifdef CBB_FIN_ROUTING
		vdDebug_LogPrintf("inUnPackIsoFunc62 CBB_FIN_ROUTING");
		
		if(srTransPara->HDTid == 22)		 // for USD	
			ret = ushUSD_WriteEncPIK(baEncPIK, 16, &uszUnPackBuf[inDataCnt], 4);
		else if(srTransPara->HDTid == 23)	//for MMK			
			ret = ushMMK_WriteEncPIK(baEncPIK, 16, &uszUnPackBuf[inDataCnt], 4);
		else										//MPU
			ret = ushMPU_WriteEncPIK(baEncPIK, 16, &uszUnPackBuf[inDataCnt], 4);
			
		#else
		ret = ushMPU_WriteEncPIK(baEncPIK, 16, &uszUnPackBuf[inDataCnt], 4);
		#endif
		
		inDataCnt += 4;
                
                
	        memcpy(strDCT.szPINKey, baEncPIK, 16);
	        inDCTSave(srTransPara->HDTid);
		

	        memset(baEncMAK, 0x00, sizeof(baEncMAK));
	        memcpy(baEncMAK, &uszUnPackBuf[inDataCnt], 16);
		inDataCnt += 16;
		
		DebugAddHEX("baEncMAK", baEncMAK, 16);
		DebugAddHEX("baEncMAK KCV", &uszUnPackBuf[inDataCnt], 4);

		#ifdef CBB_FIN_ROUTING
		vdDebug_LogPrintf("inUnPackIsoFunc62.A CBB_FIN_ROUTING");
		
			if(srTransPara->HDTid == 22)			 //for USD	
				ret = ushUSD_WriteEncMAK(baEncMAK, 16, &uszUnPackBuf[inDataCnt], 4);	
			else if(srTransPara->HDTid == 23)		//for MMK
				ret = ushMMK_WriteEncMAK(baEncMAK, 16, &uszUnPackBuf[inDataCnt], 4);	
			else											//MPU
				ret = ushMPU_WriteEncMAK(baEncMAK, 16, &uszUnPackBuf[inDataCnt], 4);	
		#else
       	ret = ushMPU_WriteEncMAK(baEncMAK, 16, &uszUnPackBuf[inDataCnt], 4);
		#endif

    }

    return ST_SUCCESS;
}

int inUnPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) 
{
    int inIndex = 2, inLen=0;
    char szTemp[9+1], szTempAmount[12+1];
    
    memset(szTempAmount, 0x00, sizeof(szTempAmount));
    
    vdDebug_LogPrintf("**inUnPackIsoFunc63 START");
    
    if(srTransPara->byPackType == BALANCE_ENQUIRY || srTransPara->byPackType == TOPUP || srTransPara->byPackType == RELOAD)
    {
        vdDebug_LogPrintf("**inUnPackIsoFunc63 uszUnPackBuf[%02X %02X]",  uszUnPackBuf[0], uszUnPackBuf[1]);

        inLen = ((uszUnPackBuf[0] / 16 * 10) + uszUnPackBuf[0] % 16) * 100;
        inLen += (uszUnPackBuf[1] / 16 * 10) + uszUnPackBuf[1] % 16;

        if(inLen<=0)
            return(ST_ERROR); 
        
        if (inLen > 0) 
        {	
			#if 0
            memcpy(szTempAmount,&uszUnPackBuf[11], 12);
            vdDebug_LogPrintf("**inUnPackIsoFunc54 inLen[%d] szTempAmount[%s]",inLen,szTempAmount);
            wub_str_2_hex(szTempAmount,srTransPara->szTipAmount,12);			 
            
            wub_hex_2_str(srTransPara->szTipAmount, szTempAmount, 6);
            vdDebug_LogPrintf("**szTempAmount[%s] szTipAmount[%s]",szTempAmount,srTransPara->szTipAmount);
			#else
			DebugAddHEX("new inUnPackIsoFunc63", &uszUnPackBuf[3], inLen);
			#endif
        }
    }
    return ST_SUCCESS;
}

int inPackISOEMVData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    int inPacketCnt = 0, inTagLen;
    unsigned short usLen, usRetVal;
    BYTE btTrack2[20];
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[512 + 1];
    char szTmp[32 + 1];
    USHORT ushEMVtagLen = 0;
    BYTE EMVtagVal[64];
    int inRet;


    DebugAddSTR("load f55", "emv", 2);
    vdDebug_LogPrintf("*** inPackISOEMVData START ***");

    vdMyEZLib_LogPrintf("**inPackISOEMVData START**");
    memset(szPacket, 0, sizeof (szPacket));
    inDataCnt = 0;

    {

        szPacket[inPacketCnt++] = 0x5F;
        szPacket[inPacketCnt++] = 0x2A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
        inPacketCnt += 2;


        DebugAddINT("5F34 Len", srTransPara->stEMVinfo.T5F34_len);
        if (srTransPara->stEMVinfo.T5F34_len > 0) {
            szPacket[inPacketCnt++] = 0x5F;
            szPacket[inPacketCnt++] = 0x34;
            szPacket[inPacketCnt++] = 1;
            szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T5F34;
            DebugAddSTR("EMV tag", "5f34--finish--", 2);
        }


        szPacket[inPacketCnt++] = 0x82;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;


        szPacket[inPacketCnt++] = 0x84;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T84_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
        inPacketCnt += srTransPara->stEMVinfo.T84_len;


        szPacket[inPacketCnt++] = 0x95;
        szPacket[inPacketCnt++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;


        szPacket[inPacketCnt++] = 0x9A;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;


        szPacket[inPacketCnt++] = 0x9C;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9C; // SL check again  //spec said 2 byte


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x02;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x03;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        inPacketCnt += 6;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x09;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x10;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1E;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
        inPacketCnt += 8;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x26;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x27;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F27;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x33;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x34;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
        inPacketCnt += 3;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x35;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F35;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x36;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x37;
        szPacket[inPacketCnt++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x41;
        szPacket[inPacketCnt++] = 3;

        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3); // get chip transaction counter
        inPacketCnt += 3;

        /*TCC*/
        //if (baEMVBackupT9F53[3] != 0x00)
        ushEMVtagLen = strlen(baEMVBackupT9F53);

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x53;
        szPacket[inPacketCnt++] = ushEMVtagLen;

        memcpy(&szPacket[inPacketCnt], baEMVBackupT9F53, ushEMVtagLen); // TCC
        inPacketCnt += ushEMVtagLen;


	#if 0  //for testing - do not send 9F63 for chip insert
		vdDebug_LogPrintf("inPackISOEMVData:T5F63_len [%d]", srTransPara->stEMVinfo.T9F63_len);
		/*Card Product Identification Information*/		
        if (srTransPara->stEMVinfo.T9F63_len > 0) {
	        szPacket[inPacketCnt++] = 0x9F;
	        szPacket[inPacketCnt++] = 0x63;
	        szPacket[inPacketCnt++] = 16;

	        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F63, 16);
	        inPacketCnt += 16;		
        }
	#endif
    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    vdMyEZLib_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOEMVData END**");
    return (inDataCnt);
}


int inPackISOJCBPayWaveData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {

    int inPacketCnt = 0, inTagLen;
    unsigned short usLen, usRetVal;
    BYTE btTrack2[20];
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[512 + 1];
    char szTmp[32 + 1];
    USHORT ushEMVtagLen = 0;
    BYTE EMVtagVal[64];
    int inRet;
    BYTE szCatgCode[4 + 1];
    BYTE szTemp[5];

	//int AIDlen;

    DebugAddSTR("load f55", "emv", 2);

    vdDebug_LogPrintf("*** inPackISOJCBPayWaveData START ***");
    memset(szPacket, 0, sizeof (szPacket));
    inDataCnt = 0;

    {
        szPacket[inPacketCnt++] = 0x5F;
        szPacket[inPacketCnt++] = 0x2A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x82;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

		//JCB ctls card trx issue on New Switch REDMINE #2257
		//AID
		// Add JCB AID value in terminal config for Contactless transaction on New Switch - 2523 T84 only send 2 bytes value. orig value of t84 is 2
		#if 0
		AIDlen = srTransRec.stEMVinfo.T84_len;

		vdDebug_LogPrintf("*** inPackISOJCBPayWaveData START AIDlen [%d]***", AIDlen);
	
        szPacket[inPacketCnt++] = 0x84;
        szPacket[inPacketCnt++] = AIDlen; //8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, AIDlen);//8);
        inPacketCnt += AIDlen; //8;
        #else
        szPacket[inPacketCnt++] = 0x84;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T84_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
        inPacketCnt += srTransPara->stEMVinfo.T84_len;		
		#endif
		
        szPacket[inPacketCnt++] = 0x95;
        szPacket[inPacketCnt++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

        szPacket[inPacketCnt++] = 0x9A;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9C;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9C; // SL check again  //spec said 2 byte

        DebugAddINT("5F34 Len", srTransPara->stEMVinfo.T5F34_len);
        if (srTransPara->stEMVinfo.T5F34_len > 0) {
            szPacket[inPacketCnt++] = 0x5F;
            szPacket[inPacketCnt++] = 0x34;
            szPacket[inPacketCnt++] = 1;
            szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T5F34;
            DebugAddSTR("EMV tag", "5f34--finish--", 2);
        }

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x02;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x03;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x09;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x10;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;

        strcpy(srTransPara->stEMVinfo.T9F1E, "12345678");

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1E;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x26;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x27;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F27;

		//removed 9f33 to address redmine case #2257
#if 0
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x33;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;
#else
		//Cardholder Verification Method (CVM) Results
		// to address redmine case #2257
		szPacket[inPacketCnt++] = 0x9F;
		szPacket[inPacketCnt++] = 0x34;
		szPacket[inPacketCnt++] = 3;
		memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
		inPacketCnt += 3;
#endif

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x35;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F35;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x36;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x37;
        szPacket[inPacketCnt++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x41;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3); // get chip transaction counter
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x53;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = 0x52;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x6E;
        szPacket[inPacketCnt ++] = 4;	
        memcpy(&szPacket[inPacketCnt], "\x20\x70\x00\x00", 4);// get chip transaction counter
        inPacketCnt += 4;		 

		/*Card Product Identification Information*/
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x63;
        szPacket[inPacketCnt++] = 16;

        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F63, 16);
        inPacketCnt += 16;			

#ifdef JCB_LEGACY_FEATURE
		/*Device Information*/
        DebugAddINT("9F6E Len", srTransPara->stEMVinfo.T9F6E_len);
		if(srTransPara->stEMVinfo.T9F6E_len > 0){
			szPacket[inPacketCnt++] = 0x9F;
			szPacket[inPacketCnt++] = 0x6E;
			szPacket[inPacketCnt++] = 4;

			memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F6E, 4);
			inPacketCnt += 4;						
		}

		/*Partner Discretion Data*/
        DebugAddINT("9F7C Len", srTransPara->stEMVinfo.T9F7C_len);
		if(srTransPara->stEMVinfo.T9F7C_len){
			szPacket[inPacketCnt++] = 0x9F;
			szPacket[inPacketCnt++] = 0x7C;
			szPacket[inPacketCnt++] = 32;
			
			memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F7C, 32);
			inPacketCnt += 32;			
		}
#endif

    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    vdMyEZLib_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOPayWaveData END**");
    return (inDataCnt);
}


int inPackISOPayWaveData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {

    int inPacketCnt = 0, inTagLen;
    unsigned short usLen, usRetVal;
    BYTE btTrack2[20];
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[512 + 1];
    char szTmp[32 + 1];
    USHORT ushEMVtagLen = 0;
    BYTE EMVtagVal[64];
    int inRet;
    BYTE szCatgCode[4 + 1];
    BYTE szTemp[5];


    DebugAddSTR("load f55", "emv", 2);

    vdMyEZLib_LogPrintf("**inPackISOPayWaveData START**");
    memset(szPacket, 0, sizeof (szPacket));
    inDataCnt = 0;

    {
        szPacket[inPacketCnt++] = 0x5F;
        szPacket[inPacketCnt++] = 0x2A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x82;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x95;
        szPacket[inPacketCnt++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

        szPacket[inPacketCnt++] = 0x9A;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9C;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9C; // SL check again  //spec said 2 byte

        DebugAddINT("5F34 Len", srTransPara->stEMVinfo.T5F34_len);
        if (srTransPara->stEMVinfo.T5F34_len > 0) {
            szPacket[inPacketCnt++] = 0x5F;
            szPacket[inPacketCnt++] = 0x34;
            szPacket[inPacketCnt++] = 1;
            szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T5F34;
            DebugAddSTR("EMV tag", "5f34--finish--", 2);
        }

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x02;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x03;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x09;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x10;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;

        strcpy(srTransPara->stEMVinfo.T9F1E, "12345678");

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1E;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x26;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x27;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F27;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x33;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x35;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F35;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x36;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x37;
        szPacket[inPacketCnt++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x41;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3); // get chip transaction counter
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x53;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = 0x52;

        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x6E;
        szPacket[inPacketCnt ++] = 4;	
        memcpy(&szPacket[inPacketCnt], "\x20\x70\x00\x00", 4);// get chip transaction counter
        inPacketCnt += 4;		 

		/*Card Product Identification Information*/
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x63;
        szPacket[inPacketCnt++] = 16;

        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F63, 16);
        inPacketCnt += 16;			

#ifdef JCB_LEGACY_FEATURE
		/*Device Information*/
        DebugAddINT("9F6E Len", srTransPara->stEMVinfo.T9F6E_len);
		if(srTransPara->stEMVinfo.T9F6E_len > 0){
			szPacket[inPacketCnt++] = 0x9F;
			szPacket[inPacketCnt++] = 0x6E;
			szPacket[inPacketCnt++] = 4;

			memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F6E, 4);
			inPacketCnt += 4;						
		}

		/*Partner Discretion Data*/
        DebugAddINT("9F7C Len", srTransPara->stEMVinfo.T9F7C_len);
		if(srTransPara->stEMVinfo.T9F7C_len){
			szPacket[inPacketCnt++] = 0x9F;
			szPacket[inPacketCnt++] = 0x7C;
			szPacket[inPacketCnt++] = 32;
			
			memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F7C, 32);
			inPacketCnt += 32;			
		}
#endif

    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    vdMyEZLib_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOPayWaveData END**");
    return (inDataCnt);
}

int inPackISOExpressPayData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    int inPacketCnt = 0, inTagLen;
    unsigned short usLen, usRetVal;
    BYTE btTrack2[20];
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[512 + 1];
    char szTmp[32 + 1];
    USHORT ushEMVtagLen = 0;
    BYTE EMVtagVal[64];
    int inRet;


    DebugAddSTR("load f55", "emv", 2);

    vdMyEZLib_LogPrintf("**inPackISOExpressPayData START**");
    memset(szPacket, 0, sizeof (szPacket));
    inDataCnt = 0;

    {
        szPacket[inPacketCnt++] = 0x82;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x95;
        szPacket[inPacketCnt++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

        szPacket[inPacketCnt++] = 0x9C;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9C; // SL check again  //spec said 2 byte

        szPacket[inPacketCnt++] = 0x5F;
        szPacket[inPacketCnt++] = 0x2A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x02;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x03;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], "\x00\x00\x00\x00\x00\x00", 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x10;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x26;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x27;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F27;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x34;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x35;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F35;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x36;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x37;
        szPacket[inPacketCnt++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;

        if (srTransPara->stEMVinfo.T5F34_len > 0) {
            szPacket[inPacketCnt++] = 0x5F;
            szPacket[inPacketCnt++] = 0x34;
            szPacket[inPacketCnt++] = 1;
            szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T5F34;
            DebugAddSTR("EMV tag", "5f34--finish--", 2);
        }

		/*Card Product Identification Information*/
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x63;
        szPacket[inPacketCnt++] = 16;

        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F63, 16);
        inPacketCnt += 16;		

		#ifdef JCB_LEGACY_FEATURE
			/*Device Information*/
			DebugAddINT("9F6E Len", srTransPara->stEMVinfo.T9F6E_len);
			if(srTransPara->stEMVinfo.T9F6E_len > 0){
				szPacket[inPacketCnt++] = 0x9F;
				szPacket[inPacketCnt++] = 0x6E;
				szPacket[inPacketCnt++] = 4;
	
				memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F6E, 4);
				inPacketCnt += 4;						
			}
	
			/*Partner Discretion Data*/
			DebugAddINT("9F7C Len", srTransPara->stEMVinfo.T9F7C_len);
			if(srTransPara->stEMVinfo.T9F7C_len){
				szPacket[inPacketCnt++] = 0x9F;
				szPacket[inPacketCnt++] = 0x7C;
				szPacket[inPacketCnt++] = 32;
				
				memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F7C, 32);
				inPacketCnt += 32;			
			}
		#endif
		
    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    vdMyEZLib_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOPayWaveData END**");
    return (inDataCnt);
}


int inMPU_PackISOEMVData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    int inPacketCnt = 0, inTagLen;
    unsigned short usLen, usRetVal;
    BYTE btTrack2[20];
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[512 + 1];
    char szTmp[32 + 1];
    USHORT ushEMVtagLen = 0;
    BYTE EMVtagVal[64];
    int inRet;


    DebugAddSTR("load f55", "emv", 2);
    vdDebug_LogPrintf("*** inMPU_PackISOEMVData START ***");

    vdMyEZLib_LogPrintf("**inPackISOEMVData START**");
    memset(szPacket, 0, sizeof (szPacket));
    inDataCnt = 0;

    {

        szPacket[inPacketCnt++] = 0x5F;
        szPacket[inPacketCnt++] = 0x2A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
        inPacketCnt += 2;

		#if 0
        DebugAddINT("5F34 Len", srTransPara->stEMVinfo.T5F34_len);
        if (srTransPara->stEMVinfo.T5F34_len > 0) {
            szPacket[inPacketCnt++] = 0x5F;
            szPacket[inPacketCnt++] = 0x34;
            szPacket[inPacketCnt++] = 1;
            szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T5F34;
            DebugAddSTR("EMV tag", "5f34--finish--", 2);
        }
		#endif

        szPacket[inPacketCnt++] = 0x82;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

		#if 0
        szPacket[inPacketCnt++] = 0x84;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T84_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
        inPacketCnt += srTransPara->stEMVinfo.T84_len;
		#endif

        szPacket[inPacketCnt++] = 0x95;
        szPacket[inPacketCnt++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

		
        szPacket[inPacketCnt++] = 0x9A;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

		
        szPacket[inPacketCnt++] = 0x9C;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9C; // SL check again  //spec said 2 byte


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x02;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

		
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x03;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        inPacketCnt += 6;

		#if 0
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x09;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;
		#endif

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x10;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;
		

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;

		#if 0
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1E;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
        inPacketCnt += 8;
		#endif


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x26;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x27;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F27;

		
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x33;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;

		#if 0
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x34;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
        inPacketCnt += 3;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x35;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F35;
		#endif

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x36;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;


        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x37;
        szPacket[inPacketCnt++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;

		#if 0
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x41;
        szPacket[inPacketCnt++] = 3;

        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3); // get chip transaction counter
        inPacketCnt += 3;

        /*TCC*/
        //if (baEMVBackupT9F53[3] != 0x00)
        ushEMVtagLen = strlen(baEMVBackupT9F53);

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x53;
        szPacket[inPacketCnt++] = ushEMVtagLen;
		
        memcpy(&szPacket[inPacketCnt], baEMVBackupT9F53, ushEMVtagLen); // TCC
        inPacketCnt += ushEMVtagLen;
		#endif


	#if 0 //for testing - do not send 9F63 for chip insert
		/*Card Product Identification Information*/
        if (srTransPara->stEMVinfo.T9F63_len > 0) {		
	        szPacket[inPacketCnt++] = 0x9F;
	        szPacket[inPacketCnt++] = 0x63;
	        szPacket[inPacketCnt++] = 16;

	        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F63, 16);
	        inPacketCnt += 16;		
        }
	#endif
    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    vdMyEZLib_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOEMVData END**");
    return (inDataCnt);
}


int inPackISOPayPassData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf) {
    int inPacketCnt = 0, inTagLen;
    unsigned short usLen, usRetVal;
    BYTE btTrack2[20];
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szPacket[512 + 1];
    char szTmp[32 + 1];
    USHORT ushEMVtagLen = 0;
    BYTE EMVtagVal[64];
    int inRet;


    DebugAddSTR("load f55", "emv", 2);

    vdMyEZLib_LogPrintf("**inPackISOPayWaveData START**");
    memset(szPacket, 0, sizeof (szPacket));
    inDataCnt = 0;

    {
        szPacket[inPacketCnt++] = 0x5F;
        szPacket[inPacketCnt++] = 0x2A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1A;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x82;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x95;
        szPacket[inPacketCnt++] = 5;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
        inPacketCnt += 5;

        szPacket[inPacketCnt++] = 0x9A;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9C;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9C; // SL check again  //spec said 2 byte

        DebugAddINT("5F34 Len", srTransPara->stEMVinfo.T5F34_len);
        if (srTransPara->stEMVinfo.T5F34_len > 0) {
            szPacket[inPacketCnt++] = 0x5F;
            szPacket[inPacketCnt++] = 0x34;
            szPacket[inPacketCnt++] = 1;
            szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T5F34;
            DebugAddSTR("EMV tag", "5f34--finish--", 2);
        }

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x02;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x03;
        szPacket[inPacketCnt++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        inPacketCnt += 6;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x09;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x10;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F10_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
        inPacketCnt += srTransPara->stEMVinfo.T9F10_len;

        strcpy(srTransPara->stEMVinfo.T9F1E, "12345678");

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x1E;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x26;
        szPacket[inPacketCnt++] = 8;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
        inPacketCnt += 8;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x27;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F27;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x33;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x35;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T9F35;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x36;
        szPacket[inPacketCnt++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
        inPacketCnt += 2;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x37;
        szPacket[inPacketCnt++] = 4;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
        inPacketCnt += 4;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x41;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3); // get chip transaction counter
        inPacketCnt += 3;

		
		szPacket[inPacketCnt++] = 0x84;
        szPacket[inPacketCnt++] = srTransPara->stEMVinfo.T84_len;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
        inPacketCnt += srTransPara->stEMVinfo.T84_len;

		
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x34;
        szPacket[inPacketCnt++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
        inPacketCnt += 3;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x53;
        szPacket[inPacketCnt++] = 1;
        szPacket[inPacketCnt++] = 0x52;

        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x6E;
        szPacket[inPacketCnt++] = 4;
        memcpy(&szPacket[inPacketCnt], "\x20\x70\x00\x00", 4); // get chip transaction counter
        inPacketCnt += 4;

		/*Card Product Identification Information*/
        szPacket[inPacketCnt++] = 0x9F;
        szPacket[inPacketCnt++] = 0x63;
        szPacket[inPacketCnt++] = 16;

        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F63, 16);
        inPacketCnt += 16;			

		#ifdef JCB_LEGACY_FEATURE
			/*Device Information*/
			DebugAddINT("9F6E Len", srTransPara->stEMVinfo.T9F6E_len);
			if(srTransPara->stEMVinfo.T9F6E_len > 0){
				szPacket[inPacketCnt++] = 0x9F;
				szPacket[inPacketCnt++] = 0x6E;
				szPacket[inPacketCnt++] = 4;
	
				memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F6E, 4);
				inPacketCnt += 4;						
			}
	
			/*Partner Discretion Data*/
			DebugAddINT("9F7C Len", srTransPara->stEMVinfo.T9F7C_len);
			if(srTransPara->stEMVinfo.T9F7C_len){
				szPacket[inPacketCnt++] = 0x9F;
				szPacket[inPacketCnt++] = 0x7C;
				szPacket[inPacketCnt++] = 32;
				
				memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F7C, 32);
				inPacketCnt += 32;			
			}
		#endif
		
    }

    /* Packet Data Length */
    memset(szAscBuf, 0, sizeof (szAscBuf));
    sprintf(szAscBuf, "%04d", inPacketCnt);
    memset(szBcdBuf, 0, sizeof (szBcdBuf));
    wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
    inDataCnt += 2;
    /* Packet Data */
    memcpy((char *) &uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
    inDataCnt += inPacketCnt;

    vdMyEZLib_LogPrintf(". Pack Len(%d)", inDataCnt);
    vdMyEZLib_LogPrintf("**inPackISOPayWaveData END**");
    return (inDataCnt);
}

int inCTOS_PackDemoResonse(TRANS_DATA_TABLE *srTransPara, unsigned char *uszRecData) {
    int inPackLen;
    BYTE szSTAN[6 + 1];
    BYTE szTID[TERMINAL_ID_BYTES + 1];
    BYTE szMTI[MTI_BCD_SIZE + 1];
    BYTE szBitMap[8 + 1];
    BYTE szCurrentTime[20];
    CTOS_RTC SetRTC;
    unsigned char temp[16 + 1];

    //default response turn on 3, 11, 12, 13, 24, 37, 38, 39, 41
    memset(szBitMap, 0x00, sizeof (szBitMap));
    memcpy(szBitMap, "\x20\x38\x01\x00\x0E\x80\x00\x00", 8);

    inPackLen = 0;

    memcpy(&uszRecData[inPackLen], "\x60\x00\x01\x00\x00", TPDU_BCD_SIZE);
    inPackLen += TPDU_BCD_SIZE;

    memcpy(szMTI, "\x02\x10", MTI_BCD_SIZE);
    szMTI[1] |= 0x10;
    memcpy(&uszRecData[inPackLen], szMTI, MTI_BCD_SIZE);
    inPackLen += MTI_BCD_SIZE;

    memcpy(&uszRecData[inPackLen], szBitMap, 8);
    inPackLen += 8;

    //DE 3
    memcpy(&uszRecData[inPackLen], "\x00\x00\x00", PRO_CODE_BCD_SIZE);
    inPackLen += PRO_CODE_BCD_SIZE;

    //DE 11
    sprintf(szSTAN, "%06ld", srTransPara->ulTraceNum);
    wub_str_2_hex(&szSTAN[0], (char *) &uszRecData[inPackLen], 6);
    inPackLen += 3;

    //DE 12
    CTOS_RTCGet(&SetRTC);
    memset(szCurrentTime, 0x00, sizeof (szCurrentTime));
    sprintf(szCurrentTime, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
    wub_str_2_hex(&szCurrentTime[0], (char *) &uszRecData[inPackLen], 6);
    inPackLen += 3;

    //DE 13
    memset(szCurrentTime, 0x00, sizeof (szCurrentTime));
    sprintf(szCurrentTime, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);
    wub_str_2_hex(&szCurrentTime[0], (char *) &uszRecData[inPackLen], 4);
    inPackLen += 2;

    //DE 24
    memcpy((char *) &uszRecData[inPackLen], strHDT.szNII, 2);
    inPackLen += 2;

    //DE 37
    memcpy((char *) &uszRecData[inPackLen], "111111111111", 12);
    inPackLen += 12;

    //DE 38
    if (0 == strlen(srTransPara->szAuthCode)) {
        memset(temp, 0x00, sizeof (temp));
        sprintf(temp, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
        memcpy((char *) &uszRecData[inPackLen], temp, 6);
    } else
        memcpy((char *) &uszRecData[inPackLen], srTransPara->szAuthCode, 6);
    inPackLen += 6;

    //DE 39
    memcpy((char *) &uszRecData[inPackLen], "00", 2);
    inPackLen += 2;

    //DE 41
    memset(szTID, 0x00, sizeof (szTID));
    memset(szTID, 0x20, TERMINAL_ID_BYTES);
    memcpy(szTID, srTransPara->szTID, strlen(srTransPara->szTID));
    memcpy((char *) &uszRecData[inPackLen], szTID, TERMINAL_ID_BYTES);
    inPackLen += TERMINAL_ID_BYTES;

    return inPackLen;
}

void vdInitialISOFunction(ISO_FUNC_TABLE *srPackFunc) {
    /* Choose ISO_FUNC_TABLE Array */
    memcpy((char *) srPackFunc, (char *) &srIsoFuncTable[0], sizeof (ISO_FUNC_TABLE));
}

int inCheckReversalTrans(int byPackType) {
    if (byPackType == REFUND_REVERSAL
            || byPackType == VOIDREFUND_REVERSAL
            || byPackType == VOID_REVERSAL
            || byPackType == PREAUTH_REVERSAL
            || byPackType == REVERSAL
            || byPackType == VOID_PREAUTH_REV)
        return d_OK;

    return d_NO;
}


/*re-pack the filed for MPU to get the MAC buffer, 
may need check both send or receive msg
0 - send
1- recv
*/
int inMPU_GenMACBuffer(TRANS_DATA_TABLE *srTransPara, BYTE *baMACBuffer, int inSendRecv)
{
	BYTE baMACData[1024];
	int inMACDataLen = 0;

	BYTE baTemp[1024];
	int inTempLen = 0;

	BYTE szMTIStr[4+1];
	BYTE szPANStr[20+1];
	int inPANLen = 0;
	
	BYTE szPANHex[10+1];
	BYTE szAmtStr[12+1];
	BYTE szDTStr[10+1];
	

	memset(baMACData, 0x00, sizeof(baMACData));
	inMACDataLen = 0;

	vdDebug_LogPrintf("=====inMPU_GenMACBuffer=====");

	DebugAddHEX("baMPU_ReqMTI", baMPU_ReqMTI, 2);

	/*1. MTI*/
	memset(baTemp, 0x00, sizeof (baTemp));
	if (0 == inSendRecv)
    	wub_hex_2_str(baMPU_ReqMTI, baTemp, MTI_BCD_SIZE);
	else
		wub_hex_2_str(baMPU_RespMTI, baTemp, MTI_BCD_SIZE);

	
    memcpy(&baMACData[inMACDataLen], baTemp, MTI_ASC_SIZE);
    inMACDataLen += MTI_ASC_SIZE;

	/*Space*/
	memcpy(&baMACData[inMACDataLen], " ", 1);
	inMACDataLen += 1;


	/*2. PAN*/
	memset(baTemp, 0x00, sizeof (baTemp));
	inTempLen = 0;
	
    inPANLen = strlen(srTransPara->szPAN);

	sprintf(baTemp, "%02d", inPANLen);
	inTempLen += 2;

	strcpy(&baTemp[inTempLen], srTransPara->szPAN);
	inTempLen += inPANLen;

	/* !!! need double check the padding format, at the end or in front?
	and it also need same as DE02*/
	if (inPANLen % 2)
	{
        baTemp[inTempLen] = '0';
		inTempLen += 1;
	}
	
	memcpy(&baMACData[inMACDataLen], baTemp, inTempLen);
	inMACDataLen += inTempLen;

	/*Space*/
	memcpy(&baMACData[inMACDataLen], " ", 1);
	inMACDataLen += 1;

	/*3. Amount*/
	memset(baTemp, 0x00, sizeof (baTemp));
    wub_hex_2_str(srTransPara->szTotalAmount, baTemp, AMT_BCD_SIZE);
    memcpy(&baMACData[inMACDataLen], baTemp, AMT_ASC_SIZE);
	inMACDataLen += AMT_ASC_SIZE;

	/*Space*/
	memcpy(&baMACData[inMACDataLen], " ", 1);
	inMACDataLen += 1;

	
	/*4. DE07 Date&Time*/
	memset(baTemp, 0x00, sizeof (baTemp));
    wub_hex_2_str(baMPU_DE07, baTemp, 5);
    memcpy(&baMACData[inMACDataLen], baTemp, 10);
	inMACDataLen += 10;

	/*Space*/
	memcpy(&baMACData[inMACDataLen], " ", 1);
	inMACDataLen += 1;

	/*5. DE11 STAN*/
	memset(baTemp, 0x00, sizeof (baTemp));
//    sprintf(baTemp, "%06d", srTransPara->ulTraceNum);
    sprintf(baTemp, "%06lu", srTransPara->ulTraceNum);
    memcpy(&baMACData[inMACDataLen], baTemp, 6);
	inMACDataLen += 6;

	/*Space*/
	memcpy(&baMACData[inMACDataLen], " ", 1);
	inMACDataLen += 1;


	/*6. DE39 Resp Code*/
	if (1 == inSendRecv)
	{
    	memcpy(&baMACData[inMACDataLen], srTransPara->szRespCode, 2);
		inMACDataLen += RESP_CODE_SIZE;

		/*Space*/
		memcpy(&baMACData[inMACDataLen], " ", 1);
		inMACDataLen += 1;
	}

	/*7. DE41 TID*/
	memcpy(&baMACData[inMACDataLen], srTransPara->szTID, TERMINAL_ID_BYTES);
	inMACDataLen += TERMINAL_ID_BYTES;

	/*Space*/
	memcpy(&baMACData[inMACDataLen], " ", 1);
	inMACDataLen += 1;
	
	/*8. DE61 Org Msg*/
	if (0 == inSendRecv && 
		(srTransPara->byTransType == VOID ||
		srTransPara->byTransType == VOID_PREAUTH ||
		srTransPara->byTransType == MPU_VOID_PREAUTH ||
		srTransPara->byTransType == MPU_PREAUTH_COMP ||
		srTransPara->byTransType == MPU_VOID_PREAUTH_COMP ||
		srTransPara->byTransType == MPU_PREAUTH_COMP_ADV ||
		srTransPara->byPackType == REVERSAL ||
		srTransPara->byPackType == MPU_PREAUTH_REV ||
		srTransPara->byPackType == MPU_PAC_REV ||
		srTransPara->byPackType == MPU_PAC_ADV_REV ||
		srTransPara->byPackType == MPU_VOID_PREAUTH_REV ||
		srTransPara->byPackType == MPU_VOID_PAC_REV ||
		srTransPara->byTransType == PREAUTH_COMP ||
		srTransPara->byPackType == PREAUTH_COMP)
		)
	{
		wub_hex_2_str(baMPU_DE61, baTemp, 10);
		baTemp[20] = 0x00;

		memcpy(&baMACData[inMACDataLen], baTemp, 20);
		inMACDataLen += 20;
		
		/*Space*/
		memcpy(&baMACData[inMACDataLen], " ", 1);
		inMACDataLen += 1;
	}
	
	#if 0
	/*9. DE62 Reserver data*/
	if (1 == inSendRecv)
	{
		/*Space*/
		memcpy(&baMACData[inMACDataLen], " ", 1);
		inMACDataLen += 1;
	}
	#endif

	/*remove the last space*/
	if (0x20 == baMACData[inMACDataLen-1])
	{
		baMACData[inMACDataLen-1] = 0x00;
		inMACDataLen = inMACDataLen - 1;
	}
	
	
	memcpy(baMACBuffer, baMACData, inMACDataLen);
	
	return inMACDataLen;
	
}


int inMPU_PackDE60Data(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg)
{
	BYTE baMPU_DE60[256];
	BYTE baMPU_DE60_HEX[256];
	BYTE szTemp[256];

	BYTE baLenHex[2];
	BYTE szLenStr[4+1];
	
	int inDataLen = 0;
	int inHexLen = 0;
	int inStrLen = 0;

	int inPos = 0;

	vdDebug_LogPrintf("=====inMPU_PackDE60Data=====");

	memset(baMPU_DE60, 0x00, sizeof(baMPU_DE60));
	
	switch(srTransPara->byTransType)
	{
		case MPU_SIGNON:
			inHexLen = inMPU_PackSignonDE60Req(srTransPara, pszMsg);
			return inHexLen;
			
		case MPU_SIGNOFF:
			inHexLen = inMPU_PackSignOffDE60Req(srTransPara, pszMsg);
			return inHexLen; 
			
		case SALE:
			memcpy(&baMPU_DE60[inPos], "22", 2); //Sale type
			inPos += 2;
			
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_hex_2_str(strMMT[0].szBatchNo, szTemp, BATCH_NO_BCD_SIZE);
			memcpy(&baMPU_DE60[inPos], szTemp, BATCH_NO_BCD_SIZE*2);
			inPos += BATCH_NO_BCD_SIZE*2;
			if (srTransPara->byEntryMode == CARD_ENTRY_MSR)
				memcpy(&baMPU_DE60[inPos], "000201000000", 12);
			else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
				memcpy(&baMPU_DE60[inPos], "000521000000", 12);
			else
				memcpy(&baMPU_DE60[inPos], "000501000000", 12);
			
			inPos += 12;

			break;
		case VOID:
		case VOID_PREAUTH:
			vdDebug_LogPrintf("case void VOID_PREAUTH byOrgTransType[%d]", srTransPara->byOrgTransType);
			if (PREAUTH_COMP == srTransPara->byOrgTransType)
				memcpy(&baMPU_DE60[inPos], "21", 2); //void type
			else
				memcpy(&baMPU_DE60[inPos], "23", 2); //void type
			inPos += 2;
			
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_hex_2_str(strMMT[0].szBatchNo, szTemp, BATCH_NO_BCD_SIZE);
			memcpy(&baMPU_DE60[inPos], szTemp, BATCH_NO_BCD_SIZE*2);
			inPos += BATCH_NO_BCD_SIZE*2;
			if (srTransPara->byEntryMode == CARD_ENTRY_MSR)
				memcpy(&baMPU_DE60[inPos], "000201000000", 12);
			else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
				memcpy(&baMPU_DE60[inPos], "000521000000", 12);
			else
				memcpy(&baMPU_DE60[inPos], "000501000000", 12);
			
			inPos += 12;

			//memcpy(&baMPU_DE60[inPos], "000", 3);
			//inPos += 3;
			
			break;

		case MPU_PREAUTH:
		case PRE_AUTH:

			if (MPU_PREAUTH_REV == srTransPara->byPackType)
			{
				memcpy(&baMPU_DE60[inPos], "10", 2);
				inPos += 2;
			}
			else
			{
				memcpy(&baMPU_DE60[inPos], "20", 2);
				inPos += 2;
			}
		
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_hex_2_str(strMMT[0].szBatchNo, szTemp, BATCH_NO_BCD_SIZE);
			memcpy(&baMPU_DE60[inPos], szTemp, BATCH_NO_BCD_SIZE*2);
			inPos += BATCH_NO_BCD_SIZE*2;
			if (srTransPara->byEntryMode == CARD_ENTRY_MSR)
				memcpy(&baMPU_DE60[inPos], "000201000000", 12);
			else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
				memcpy(&baMPU_DE60[inPos], "000521000000", 12);
			else
				memcpy(&baMPU_DE60[inPos], "000501000000", 12);
			
			inPos += 12;
			
			break;

		case MPU_VOID_PREAUTH:
			memcpy(&baMPU_DE60[inPos], "11", 2);  //pre-auth type
			inPos += 2;
		
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_hex_2_str(strMMT[0].szBatchNo, szTemp, BATCH_NO_BCD_SIZE);
			memcpy(&baMPU_DE60[inPos], szTemp, BATCH_NO_BCD_SIZE*2);
			inPos += BATCH_NO_BCD_SIZE*2;
			if (srTransPara->byEntryMode == CARD_ENTRY_MSR)
				memcpy(&baMPU_DE60[inPos], "000201000000", 12);
			else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
				memcpy(&baMPU_DE60[inPos], "000521000000", 12);
			else
				memcpy(&baMPU_DE60[inPos], "000501000000", 12);
			
			inPos += 12;
			
			break;
		case MPU_PREAUTH_COMP:
		case MPU_PREAUTH_COMP_ADV:
		case PREAUTH_COMP:
			memcpy(&baMPU_DE60[inPos], "20", 2);  // type
			inPos += 2;
		
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_hex_2_str(strMMT[0].szBatchNo, szTemp, BATCH_NO_BCD_SIZE);
			memcpy(&baMPU_DE60[inPos], szTemp, BATCH_NO_BCD_SIZE*2);
			inPos += BATCH_NO_BCD_SIZE*2;
			if (srTransPara->byEntryMode == CARD_ENTRY_MSR)
				memcpy(&baMPU_DE60[inPos], "000201000000", 12);
			else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
				memcpy(&baMPU_DE60[inPos], "000521000000", 12);
			else
				memcpy(&baMPU_DE60[inPos], "000501000000", 12);
			
			inPos += 12;
			
			break;

		case MPU_VOID_PREAUTH_COMP:
			memcpy(&baMPU_DE60[inPos], "21", 2);
			inPos += 2;
		
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_hex_2_str(strMMT[0].szBatchNo, szTemp, BATCH_NO_BCD_SIZE);
			memcpy(&baMPU_DE60[inPos], szTemp, BATCH_NO_BCD_SIZE*2);
			inPos += BATCH_NO_BCD_SIZE*2;
			if (srTransPara->byEntryMode == CARD_ENTRY_MSR)
				memcpy(&baMPU_DE60[inPos], "000201000000", 12);
			else if (srTransPara->byEntryMode == CARD_ENTRY_FALLBACK)
				memcpy(&baMPU_DE60[inPos], "000521000000", 12);
			else
				memcpy(&baMPU_DE60[inPos], "000501000000", 12);
			
			inPos += 12;
			
			break;

		case MPU_SETTLE:
		case SETTLE:
			vdDebug_LogPrintf("MPU Settle srTransPara->HDTid[%d]", srTransPara->HDTid);
			//if(srTransPara->HDTid != 17 && srTransPara->HDTid != 18 && srTransPara->HDTid == 19)
				//break;
				
            vdDebug_LogPrintf("MPU Settle DE60 data");
			memcpy(&baMPU_DE60[inPos], "00000001201", 11);
			inPos += 11;
			break;
			
		case BATCH_UPLOAD:
			memcpy(&baMPU_DE60[inPos], "00000001201", 11);
			inPos += 11;
			vdDebug_LogPrintf("Batch upload DE60 inFinalSend[%d]", inFinalSend);
			if (inFinalSend == CN_TRUE)
				baMPU_DE60[10] = '2';
			break;
		default:
			
			break;
		
	}


	if (srTransPara->byPackType == BATCH_UPLOAD)
	{
		memset(baMPU_DE60, 0x00, sizeof(baMPU_DE60));
		memcpy(&baMPU_DE60[0], "00000001201", 11);
		inPos = 11;
		vdDebug_LogPrintf("Batch upload DE60 inFinalSend[%d]", inFinalSend);
		if (inFinalSend == CN_TRUE)
			baMPU_DE60[10] = '2';
	}

	inDataLen = strlen(baMPU_DE60);
	inStrLen = inDataLen;

	/*padding zero in front*/
	if (inDataLen % 2)
	{
		memset(szTemp, 0x00, sizeof(szTemp));
		szTemp[0] = '0';
		memcpy(&szTemp[1], baMPU_DE60, inDataLen);
		memset(baMPU_DE60, 0x00, sizeof(baMPU_DE60));
		strcpy(baMPU_DE60, szTemp);
		inStrLen = inDataLen + 1;
	}

	/*covert to hex, calc final hex bytes*/
	memset(baMPU_DE60_HEX, 0x00, sizeof(baMPU_DE60_HEX));
	wub_str_2_hex(baMPU_DE60, baMPU_DE60_HEX, inStrLen);
	inHexLen = inStrLen / 2;

	/*Length*/
	memset(szLenStr, 0, sizeof (szLenStr));
    sprintf(szLenStr, "%04d", inDataLen); // Please remeber count in actual data length
	
    memset(baLenHex, 0, sizeof (baLenHex));
    wub_str_2_hex(szLenStr, baLenHex, 4);
	
	memcpy(pszMsg, baLenHex, 2);
	memcpy(pszMsg+2, baMPU_DE60_HEX, inHexLen);

	DebugAddHEX("MPU DE60", pszMsg, inHexLen+2);

	return inHexLen+2;
}



int inMPU_PorcessTransUpLoad(TRANS_DATA_TABLE *srTransPara) 
{
    int inSendCount, inTotalCnt, inFileMaxLen;
    int inResult;
    int inBatchRecordNum = 0;
    int i;
    TRANS_DATA_TABLE srUploadTransRec, srTransParaTmp;
    STRUCT_FILE_SETTING strFile;
    int *pinTransDataid = NULL;
    int *pinTransDataidSend = NULL;
    int inBatchCount = 0;
    BATCH_UP_DATA srBatchUpTmp;
    BOOL fIsSend = FALSE;
	int inNeedBatUp;

    memset(&srBatchUpTmp, 0x00, sizeof (BATCH_UP_DATA));
    memset(&strBatchUp, 0x00, sizeof (BATCH_UP_DATA));

    inResult = ST_SUCCESS;

    memset(&srUploadTransRec, 0x00, sizeof (TRANS_DATA_TABLE));
    memset(&strFile, 0x00, sizeof (STRUCT_FILE_SETTING));

#if 1
	//inNeedBatUp = inMPU_CheckBatchUploadFlag(&srTransRec);// check batch upload flag

	vdDebug_LogPrintf("inMPU_PorcessTransUpLoad [%d][%d][%d[%d]]", srTransRec.HDTid, srTransRec.MITid, inNeedBatUp, srUploadTransRec.byTransType);
	DebugAddHEX("inMPU_PorcessTransUpLoad srTransRec.szBatchNo", srTransRec.szBatchNo, 3);
	
    inBatchRecordNum = inMPUBatchNumRecord(srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo);
#else
	inBatchRecordNum = inBatchNumRecord();
#endif


    vdDebug_LogPrintf("BatchUpload totaltxn[%d]", inBatchRecordNum);

	
    if (inBatchRecordNum > 0) {
        pinTransDataid = (int*) malloc(inBatchRecordNum * sizeof (int));
        pinTransDataidSend = (int*) malloc(inBatchRecordNum * sizeof (int));

        inBatchByMerchandHost(inBatchRecordNum, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);

        inTotalCnt = 0;
        for (i = 0; i < inBatchRecordNum; i++) {
            inDatabase_BatchReadByTransId(&srUploadTransRec, pinTransDataid[i]);

            vdDebug_LogPrintf("BatchUpload curren[%d] Void?[%d]", pinTransDataid[i], srUploadTransRec.byVoided);
            //            if((srUploadTransRec.byVoided != TRUE) && (srUploadTransRec.byTransType != PRE_AUTH))
            if ((srUploadTransRec.byVoided != TRUE) && ((srUploadTransRec.byTransType != PRE_AUTH) ||
                    (srUploadTransRec.byTransType != MPU_PREAUTH))) {
                pinTransDataidSend[inTotalCnt] = pinTransDataid[i];
                inTotalCnt++;
            }
        }

        //in case all is void
        inResult = ST_SUCCESS;
        inFinalSend = CN_FALSE;

        vdDebug_LogPrintf("BatchUpload total None void txn[%d]", inTotalCnt);
        for (inSendCount = 0; inSendCount < inTotalCnt; inSendCount++)
            //        for(inSendCount = 0; inSendCount < inTotalCnt; inSendCount += 8)
        {
            if (((inSendCount + 1) == inTotalCnt))
                inFinalSend = CN_TRUE;
            else
                inFinalSend = CN_FALSE;

			
			vdDebug_LogPrintf("inFinalSend[%d]", inFinalSend);

            vdDebug_LogPrintf("Before HDTid[%d]MITid[%d]AMT[%02X%02X%02X%02X%02X%02X]", srTransPara->HDTid, srTransPara->MITid, srTransPara->szTotalAmount[0]
                    , srTransPara->szTotalAmount[1]
                    , srTransPara->szTotalAmount[2]
                    , srTransPara->szTotalAmount[3]
                    , srTransPara->szTotalAmount[4]
                    , srTransPara->szTotalAmount[5]);
            inDatabase_BatchReadByTransId(&srUploadTransRec, pinTransDataidSend[inSendCount]);

            vdDebug_LogPrintf("After HDTid[%d]MITid[%d]AMT[%02X%02X%02X%02X%02X%02X]", srUploadTransRec.HDTid, srUploadTransRec.MITid, srUploadTransRec.szTotalAmount[0]
                    , srUploadTransRec.szTotalAmount[1]
                    , srUploadTransRec.szTotalAmount[2]
                    , srUploadTransRec.szTotalAmount[3]
                    , srUploadTransRec.szTotalAmount[4]
                    , srUploadTransRec.szTotalAmount[5]);

            //            if(srUploadTransRec.byTransType == PRE_AUTH)
            if (srUploadTransRec.byTransType == PRE_AUTH || srUploadTransRec.byTransType == MPU_PREAUTH) {
                continue;
            }
            //OFFLINE SALE, then VOID it, the VOID is offline, should not batch upload 
            if (srUploadTransRec.byTransType == VOID && srUploadTransRec.byOffline == CN_TRUE && srUploadTransRec.byUploaded == CN_FALSE) {
                continue;
            }

            srUploadTransRec.byPackType = BATCH_UPLOAD;

            vdDebug_LogPrintf(". Bef Add szTraceNo = %02x%02x%02x", strHDT.szTraceNo[0],
                    strHDT.szTraceNo[1],
                    strHDT.szTraceNo[2]);

            inMyFile_HDTTraceNoAdd(srUploadTransRec.HDTid);

            vdDebug_LogPrintf(". Aft Add szTraceNo = %02x%02x%02x", strHDT.szTraceNo[0],
                    strHDT.szTraceNo[1],
                    strHDT.szTraceNo[2]);

            srUploadTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
            vdDebug_LogPrintf(". UploadSendTracNum(%d) [%s]", srUploadTransRec.ulTraceNum, srUploadTransRec.szTID);
            memcpy(&srTransParaTmp, &srUploadTransRec, sizeof (TRANS_DATA_TABLE));

            memcpy(srBatchUpTmp.stBatchDetail[inBatchCount].szCardType, "00", 2);
            memcpy(srBatchUpTmp.stBatchDetail[inBatchCount].szAmount, srTransParaTmp.szTotalAmount, AMT_BCD_SIZE);
            memcpy(srBatchUpTmp.stBatchDetail[inBatchCount].szPAN, srTransParaTmp.szPAN, strlen(srTransParaTmp.szPAN));
            srBatchUpTmp.stBatchDetail[inBatchCount].ulTraceNum = srTransParaTmp.ulTraceNum;

            DebugAddHEX("szCardType", srBatchUpTmp.stBatchDetail[inBatchCount].szCardType, 2);
            DebugAddHEX("szAmount", srBatchUpTmp.stBatchDetail[inBatchCount].szAmount, AMT_BCD_SIZE);
            DebugAddHEX("szPAN", srBatchUpTmp.stBatchDetail[inBatchCount].szPAN, strlen(srTransParaTmp.szPAN));
            vdDebug_LogPrintf("ultrace num %d", srBatchUpTmp.stBatchDetail[inBatchCount].ulTraceNum);

            if ((inResult = inPackSendAndUnPackData(&srUploadTransRec, srUploadTransRec.byPackType) != ST_SUCCESS)) {
                vdDebug_LogPrintf(". inPorcessTransUpLoad(%d)_Err", inResult);
                vdDebug_LogPrintf(". byTransType %d", srUploadTransRec.byTransType);
                inResult = ST_UNPACK_DATA_ERR;
                break;
            } else {
                if (memcmp(srUploadTransRec.szRespCode, "00", 2)) {
                    vdDebug_LogPrintf(". inPorcessTransUpLoad(%s) BatchUpload Fail", srUploadTransRec.szRespCode);
                    inResult = ST_ERROR;
                    break;
                } else {
                    if (ST_SUCCESS != inBaseRespValidation(&srTransParaTmp, &srUploadTransRec)) {
                        inResult = ST_UNPACK_DATA_ERR;
                        break;
                    }
                }

                vdDebug_LogPrintf(". inPorcessTransUpLoad(%d)BatchUpload OK", inResult);
            }

//            memset(&strBatchUp, 0x00, sizeof (BATCH_UP_DATA));
//            memset(&srBatchUpTmp, 0x00, sizeof (BATCH_UP_DATA));
//
//            inBatchCount = 0;
        }

        free(pinTransDataid);
        free(pinTransDataidSend);

    } else {
        inResult = ST_ERROR;
        vdDebug_LogPrintf("inMPU_PorcessTransUpLoad No batch record found");
    }

	//enabled to support final recon (960000) - for MPU new host enhancement
	#if 1
    inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
    srUploadTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
    vdMyEZLib_LogPrintf(". Aft Upload TraceNum(%d)", srUploadTransRec.ulTraceNum);

    if (inResult == ST_SUCCESS && inFinalSend == CN_TRUE) {
        srTransPara->byPackType = CLS_BATCH;
        srTransPara->ulTraceNum = srUploadTransRec.ulTraceNum;
        if ((inResult = inPackSendAndUnPackData(srTransPara, srTransPara->byPackType) != ST_SUCCESS)) {
            vdMyEZLib_LogPrintf(". FinalSettle(%d)_Err", inResult);
            vdMyEZLib_LogPrintf(". byTransType %d", srTransPara->byTransType);
            inResult = ST_UNPACK_DATA_ERR;
        }


        /* BDO: Check settlement reconcillation response code - start -- jzg */
        if (memcmp(srTransPara->szRespCode, "00", 2) != 0) {
            vdDebug_LogPrintf("JEFF::ST_CLS_BATCH_ERR");
            inResult = ST_CLS_BATCH_ERR;
        }
        /* BDO: Check settlement reconcillation response code - end -- jzg */

        inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
        srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
        vdMyEZLib_LogPrintf(". Aft CLS_BATCH TraceNum(%d)", srTransPara->ulTraceNum);
    }
	#endif
	
	vdDebug_LogPrintf("inMPU_PorcessTransUpLoad END [%d]", inResult);
    return inResult;
}


int inMPU_ProcessBatchUpload(void) 
{
    int inResult, i;
    char szErrorMessage[30 + 1];
    char szBcd[INVOICE_BCD_SIZE + 1];
    TRANS_DATA_TABLE *srTransPara;
    TRANS_DATA_TABLE srTransParaTmp;
    //commsbackup
    BOOL fConnectFailed = FALSE;
    //commsbackup

    memset(szBcd, 0x00, sizeof (szBcd));
    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
    if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType))
        srTransRec.ulOrgTraceNum = srTransRec.ulTraceNum;
    inHDTSave(strHDT.inHostIndex);

    srTransPara = &srTransRec;

    vdDebug_LogPrintf("**inMPU_ProcessBatchUpload V02**");
    inCTLOS_Updatepowrfail(PFR_BEGIN_SEND_ISO);

    
    vdDispTransTitle(srTransRec.byTransType);

    if ((strTCT.byTerminalType % 2) == 0)
        setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "Processing...");
    else
        setLCDPrint(8, DISPLAY_POSITION_LEFT, "Processing...");
    vdDecideWhetherConnection(srTransPara);

    if (CN_FALSE == srTransPara->byOffline) 
	{
        if (VS_TRUE == strTCT.fDemo) 
		{
            vdDebug_LogPrintf("DEMO Call Connect!!");
        } 
		else 
        {
            vdDebug_LogPrintf("Call Connect!!sharecom[%d]", strTCT.fShareComEnable);
            memset(szErrorMessage, 0x00, sizeof (szErrorMessage));
            sprintf(szErrorMessage, "%d", strCPT.inCommunicationMode);
            inCTOSS_PutEnv("PRICOMMMODE", szErrorMessage);
            inCTOSS_PutEnv("COMMBACK", "0");
            memset(szErrorMessage, 0x00, sizeof (szErrorMessage));
            sprintf(szErrorMessage, "%d", strCPT.inSecCommunicationMode);
            inCTOSS_PutEnv("SECCOMMMODE", szErrorMessage);
            //			usPreconnectStatus = 2;
            if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS) {
                fConnectFailed = TRUE;

                inCTOS_inDisconnect();
                if (strCPT.inSecCommunicationMode != NULL_MODE) //Comms fallback
                {
                    fConnectFailed = FALSE;
                    if (inCTOS_CommsFallback(strHDT.inHostIndex) != d_OK) //Comms fallback 
                    {
                        vdSetErrorMessage("TRANS COMM ERROR");
                        return ST_ERROR;
                    }

                    inCTOSS_PutEnv("COMMBACK", "1");
                    if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS) {
                        fConnectFailed = TRUE;
                        inCTOS_inDisconnect();
                    }
                }

                if (fConnectFailed == TRUE) {
                    if (srTransPara->shTransResult == 0)
                        srTransPara->shTransResult = TRANS_COMM_ERROR;

                    vdSetErrorMessage("TRANS COMM ERROR");
                    return ST_ERROR;
                }
            }
        }

		#if 0
        if (inProcessReversal(srTransPara) != ST_SUCCESS) 
		{
            inCTOS_inDisconnect();
            vdSetErrorMessage("Reversal Error");
            return ST_ERROR;
        }
		#endif

		#if 0
        if (srTransPara->byTransType == SETTLE || srTransPara->byTransType == MPU_SETTLE) {
            if ((inResult = inProcessEMVTCUpload(srTransPara, -1)) != ST_SUCCESS) {
                vdDebug_LogPrintf(". inProcessAdviceTrans(%d) ADV_ERROR!!", inResult);
                inCTOS_inDisconnect();
                vdSetErrorMessage("TC Upload Error");
                return ST_ERROR;
            }

            if ((inResult = inProcessAdviceTrans(srTransPara, -1)) != ST_SUCCESS) {
                vdDebug_LogPrintf(". inProcessAdviceTrans(%d) ADV_ERROR!!", inResult);
                inCTOS_inDisconnect();
                vdSetErrorMessage("Advice Error");
                return ST_ERROR;
            }
        }
		#endif

    }

    if (inMPU_PorcessTransUpLoad(srTransPara) != ST_SUCCESS) 
	{
		srTransRec.shTransResult = TRANS_COMM_ERROR;
		inCTOS_inDisconnect();
		vdSetErrorMessage("BATCH UPLOAD ERROR");
		return ST_ERROR;
	}
    
    DebugAddSTR("inMPU_ProcessBatchUpload", "end", 20);

    return ST_SUCCESS;

}


int inProcessReversalEx(TRANS_DATA_TABLE *srTransPara)
{    
    int inResult,inTransCode;

	return d_OK;
    
    /*for TLE field 57*/
    byField_02_ON = 0;
    byField_14_ON = 0;
    byField_35_ON = 0;
    byField_45_ON = 0;
    byField_48_ON = 0;
    
    vdDebug_LogPrintf("**inProcessReversal START**", inTransCode);
#if 0	
	if(VS_TRUE == strTCT.fDemo)
    {
        vdDebug_LogPrintf("DEMO Call ProcessReversal!!");
        inResult = ST_SUCCESS;
		return inResult;
    }
    
    inResult = ST_SUCCESS;    
    inTransCode = srTransPara->byTransType;    
    srTransPara->byPackType = REVERSAL;
    vdDebug_LogPrintf(". transtype %d",srTransPara->byTransType);
    vdDebug_LogPrintf(". pack type %d",srTransPara->byPackType);

    /* Check reversal flag, If flag is true then open reversal file and send data to host */    
    if ((inResult = inSnedReversalToHost(srTransPara,inTransCode)) != ST_SUCCESS)
    {
        vdDebug_LogPrintf(". Process_inSnedReversalToHost %d",inResult);
		//if(inExceedMaxTimes_ReversalDelete() != ST_SUCCESS)
		//{
		//	vdDisplayErrorMsg(1, 8, "Delete REV Fail...");
		//}
    }
#endif

	if (memcmp(srTransRec.szKSN, "\x00\x00\x00\x00\x00", 5) !=0)
		inCTOSS_ResetDUKPTKSN(srTransRec.szKSN);
	
	/* send/recv reversal will update STAN in function inUnPackIsoFunc11
	after reversal send out, set trace number back to current transaction*/
    srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
	
    if(inResult == ST_SUCCESS)
    {
        inMyFile_ReversalDelete();
        /* Check reversal flag, If flag is true then create reversal file */
//        if ((strHDT.fReversalEnable == CN_TRUE) && (srTransRec.byTransType != SETTLE) && (srTransRec.byTransType != SIGN_ON))
        if ((strHDT.fReversalEnable == CN_TRUE) && 
			(srTransRec.byTransType != SETTLE) && 
			(srTransRec.byTransType != SIGN_ON) && 
			(srTransRec.byTransType != IPP_SIGN_ON) &&
			(srTransRec.byTransType != EFTSEC_TWK_RSA) && 
			(srTransRec.byTransType != EFTSEC_TWK) &&
			(srTransRec.byTransType != QR_INQUIRY))
        {
            vdDebug_LogPrintf("inSaveReversalFile START");
            if ((inResult = inSaveReversalFile(srTransPara,inTransCode)) != ST_SUCCESS)
            {
                vdDebug_LogPrintf(". Process_inSaveReversalFile %04x",inResult);
            }
        }
    }
    
    srTransPara->byTransType = inTransCode; 
    
    vdDebug_LogPrintf("**inProcessReversal TYPE(%d) Rest(%d)END**",srTransPara->byTransType,
                                                                     inResult);
    
    return inResult;
}

int inSendAndReceiveAlipayFormComm(TRANS_DATA_TABLE* srTransPara,
        unsigned char* uszSendData,
        int inSendLen,
        unsigned char* uszReceData)
{
    int inResult, inRetry=d_NO, inResult2=0, inTransCode=0;
    BYTE key;
	BOOL fRetry=TRUE, fExit=FALSE;
	char szBcd[INVOICE_BCD_SIZE+1];
	int inReceLen;
	BOOL fCheck98=FALSE, fCheckRetry=FALSE;
	
    vdDebug_LogPrintf("**inSendAndReceiveFormComm START**");
    vdDebug_LogPrintf("inIPHeader = [%d]", strCPT.inIPHeader);
    DebugAddHEX("1. inSendAndReceiveAlipayFormComm send da", uszSendData, inSendLen);

    do
    {
        if (VS_TRUE == strTCT.fDemo) 
        {
            vdDebug_LogPrintf("DEMO Call inSendData!!");
            inResult = d_OK;
        } 
        else 
        {
            inResult = srCommFuncPoint.inSendData(srTransPara, uszSendData, inSendLen);
        }

        vdDebug_LogPrintf("**inSendAndReceiveFormComm [%d]", inResult);
        if (inResult != d_OK) 
        {
            if (srTransPara->byPackType != TC_UPLOAD) 
            {
                vdSetErrorMessage("SEND LEN ERR");
            }

            //inSetTextMode();				
            inCTOS_inDisconnect();
            return ST_ERROR;
        } 
	    else 
	    {
            if (VS_TRUE == strTCT.fDemo) 
            {
                vdDebug_LogPrintf("DEMO Call inRecData!!");
                inResult = inCTOS_PackDemoResonse(srTransPara, uszReceData);
            } 
            else 
            {
                inResult = srCommFuncPoint.inRecData(srTransPara, uszReceData);
                /*prompt for retry*/
                if(inResult <= 0 && fCheckRetry != TRUE)
                {
ASK_FOR_TSE_RETRY:
                    inTransCode=ALIPAY_SALE;
                    inRetry=inRetryTransStatusEnquiry();
                    if(inRetry == d_OK) 
                    {
						inMyFile_HDTTraceNoAdd(srTransPara->HDTid);
						inHDTRead(srTransPara->HDTid);
						srTransPara->ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
						
						vdDebug_LogPrintf("srTransPara->ulTraceNum: %06ld", srTransPara->ulTraceNum);
						
						if ((inSendLen = inCTOSS_PackIsoDataEx(srTransPara->HDTid, strHDT.szTPDU, uszSendData, inTransCode)) <= ST_SUCCESS) 
						{
							inCTOS_inDisconnect();
							vdDebug_LogPrintf("inSendAndReceiveAlipayFormComm inSendLen %d", inSendLen);
							vdDebug_LogPrintf("inSendAndReceiveAlipayFormComm **inPackSendAndUnPackData ST_BUILD_DATD_ERR**");
							return ST_BUILD_DATD_ERR;
						}

						DebugAddHEX("2. inSendAndReceiveAlipayFormComm send da", uszSendData, inSendLen);

                        fCheck98=fCheckRetry=TRUE;
                        continue;
                    }
                    else /*send reversal*/
                    {
						vduiClearBelow(2);
						vdDisplayErrorMsg(1, 8, "COMM ERROR");
						vduiClearBelow(2);
                        inResult2 = inSaveReversalFile(srTransPara, inTransCode);
                        if(inResult2 == ST_SUCCESS)
                        {
                            inSnedReversalToHost(srTransPara, inTransCode);
							srTransPara->fSendAlipayAutoReversal=TRUE;
							memset(srTransPara->szRespCode, 0x00, 2);
                        }
                    }
                }
				else if(inResult > 0) /*with response*/
				{
                    vdSetISOEngTransDataAddress(srTransPara);
                    inResult2=inCTOSS_UnPackIsodataEx1(srTransPara->HDTid,
                    (unsigned char *) uszSendData,
                    inSendLen,
                    (unsigned char *) uszReceData,
                    &inReceLen);

					if(inResult2 != ST_SUCCESS)
					{
						vdSetErrorMessage("INVALID RESPONSE");
						inResult=ST_UNPACK_DATA_ERR;
						break;
					}
					
                    vdDebug_LogPrintf("**inPackSendAndUnPackData inResult=[%d]srTransPara->szRespCode[%s]", inResult, srTransPara->szRespCode);
                    DebugAddHEX("inSendAndReceiveAlipayFormComm recv da", uszReceData, inReceLen);

                    if(fCheck98 != TRUE)
                    {
                        fCheck98=TRUE;
                        if(memcmp(srTransPara->szRespCode, "98", 2) == 0)
                            goto ASK_FOR_TSE_RETRY;			
                    }
					else
					{
						if(memcmp(srTransPara->szRespCode, "98", 2) == 0)
						{
                            vduiClearBelow(2);
                            inResult2 = inSaveReversalFile(srTransPara, inTransCode);
                            if(inResult2 == ST_SUCCESS)
                            {
                                inSnedReversalToHost(srTransPara, inTransCode);
                                srTransPara->fSendAlipayAutoReversal=TRUE;
                            }		
							memcpy(srTransPara->szRespCode, "98", 2);
						}
					}
				}
            }

            if (inResult > 0) 
            {
                DebugAddHEX("recv da", uszReceData, inResult);
                vdMyEZLib_LogPrintff(uszReceData, inResult);
    			fExit=TRUE;
            } 
            else 
    		{
                //inCTOS_inDisconnect();
                if (srTransPara->byPackType != TC_UPLOAD) 
                {
                    vdSetErrorMessage("NO RESP.FRM HOST");
                    return ST_ERROR;
                }
            }
	    }
    }while(fExit != TRUE);
	
    vdDebug_LogPrintf("**inSendAndReceiveFormComm END**");
    //	inSetTextMode(); // patrick temp remark 20140421				
    return inResult;
}

int inBuildStringISO(char *uzSrc, int inSrcLen, char *uzDest, int inPackType)
{
	int inLen=0;
	char szHeader[30+1];
    char szPacket[1024+1];
    char szLen[4+1];
	
	memset(szHeader, 0, sizeof(szHeader));
	if(inPackType == TOPUP || inPackType == RELOAD)
	{
		uzSrc[13]|=0x10;
	}
	
    wub_hex_2_str(uzSrc, szHeader, 15);
	vdDebug_LogPrintf("szHeader:%s", szHeader);

    memset(szPacket, 0, sizeof(szPacket));
	strcpy(szPacket, szHeader);
	if(strlen(DE2))
	{
	    strcat(szPacket, DE2);
		vdDebug_LogPrintf("DE2:%s", DE2);
	}

	if(strlen(DE3))
	{
	    strcat(szPacket, DE3);
		vdDebug_LogPrintf("DE3:%s", DE3);
	}

	if(strlen(DE4))
	{
	    strcat(szPacket, DE4);
		vdDebug_LogPrintf("DE4:%s", DE4);
	}

	if(strlen(DE11))
	{
	    strcat(szPacket, DE11);
		vdDebug_LogPrintf("DE11:%s", DE11);
	}

	if(strlen(DE12))
	{
	    strcat(szPacket, DE12);
		vdDebug_LogPrintf("DE12:%s", DE12);
	}

	if(strlen(DE14))
	{
	    strcat(szPacket, DE14);
		vdDebug_LogPrintf("DE14:%s", DE14);
	}

	if(strlen(DE18))
	{
	    strcat(szPacket, DE18);
		vdDebug_LogPrintf("DE18:%s", DE18);
	}

	if(strlen(DE32))
	{
	    strcat(szPacket, DE32);
		vdDebug_LogPrintf("DE32:%s", DE32);
	}

	if(strlen(DE41))
	{
	    strcat(szPacket, DE41);
		vdDebug_LogPrintf("DE41:%s", DE41);
	}

	if(strlen(DE43))
	{
	    strcat(szPacket, DE43);
		vdDebug_LogPrintf("DE43:%s", DE43);
	}

	if(strlen(DE48))
	{
	    strcat(szPacket, DE48);
		vdDebug_LogPrintf("DE48:%s", DE48);
	}

	if(strlen(DE49))
	{
	    strcat(szPacket, DE49);
		vdDebug_LogPrintf("DE49:%s", DE49);
	}
	
    if(inPackType == TOPUP || inPackType == RELOAD)
    {
		if(strlen(DE52))
		{
            strcat(szPacket, DE52);
			vdDebug_LogPrintf("DE52:%s", DE52);
		}
    }

    if(strlen(DE62))
    {
        strcat(szPacket, DE62);
        vdDebug_LogPrintf("DE62:%s", DE62);
    }

#if 0
    memset(szLen, 0, sizeof(szLen));
	sprintf(szLen, "%04d", strlen(szPacket));
	memcpy(uzDest, szLen, strlen(szLen));
	
	vdDebug_LogPrintf("szPacket:%s", szPacket);
	memcpy(uzDest+4, szPacket, strlen(szPacket));
		
	return strlen(szPacket)+4;
#else
    memcpy(uzDest, szPacket, strlen(szPacket));
    return strlen(szPacket);
#endif
}

int inParseStringISO(char *uzSrc, char *uzDest, int inPackType)
{
	int inLen=0, inSrcIndex=0, inDataCnt=0, inDestIndex=0;
	char szHeader[30+1];
    char szPacket[512+1], szTemp[128+1], szTempHex[64+1];
    char szLen[4+1];

	memset(szHeader, 0, sizeof(szHeader));
    wub_str_2_hex(uzSrc, szHeader, 30);
	DebugAddHEX("recv da header", szHeader, 15);
	inSrcIndex+=30; //skip header

    if(inPackType != TOPUP_RELOAD_LOGON)
        szHeader[14]=0x00; //remove last bitmap
	
	memset(szPacket, 0, sizeof(szPacket));
	memcpy(szPacket, szHeader, 15); inDestIndex+=15;
		
    /*byte 1*/
    if(szHeader[7] & 0x40) //DE2
    {
		//vdDebug_LogPrintf("DE2");
		memset(szLen, 0, sizeof(szLen));
		memcpy(szLen, uzSrc+inSrcIndex, 2); inSrcIndex+=2;
        vdDebug_LogPrintf("Pan Len: %s", szLen);
		
		memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, szLen, 2);
		
		memcpy(szTemp+2, uzSrc+inSrcIndex, atoi(szLen)); inSrcIndex+=atoi(szLen);

		vdDebug_LogPrintf("Pan: %s", szTemp);
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

        DebugAddHEX("DE02", szTempHex, inLen);

		memcpy(szPacket+inDestIndex, szTempHex, inLen); 
		inDestIndex+=inLen;
    }
    
    if(szHeader[7] & 0x20) //DE3
    {
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, 6); inSrcIndex+=6;
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

		DebugAddHEX("DE03", szTempHex, inLen);

        memcpy(szPacket+inDestIndex, szTempHex, inLen); 
        inDestIndex+=inLen;
    }
    
    if(szHeader[7] & 0x10) //DE4
    {
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, 12); inSrcIndex+=12;
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

		DebugAddHEX("DE04", szTempHex, inLen);

        memcpy(szPacket+inDestIndex, szTempHex, inLen); 
        inDestIndex+=inLen;
    }

    /*byte 2*/
    if(szHeader[8] & 0x20) //DE11
    {
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, 6); inSrcIndex+=6;
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

		DebugAddHEX("DE11", szTempHex, inLen);

        memcpy(szPacket+inDestIndex, szTempHex, inLen); 
        inDestIndex+=inLen;
    }
    
    if(szHeader[8] & 0x10) //DE12
    {
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, 12); inSrcIndex+=12;
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

		DebugAddHEX("DE12", szTempHex, inLen);

        memcpy(szPacket+inDestIndex, szTempHex, inLen); 
        inDestIndex+=inLen;

    }
    
    if(szHeader[8] & 0x04) //DE14
    {
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, 4); inSrcIndex+=4;
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

		DebugAddHEX("DE14", szTempHex, inLen);

        memcpy(szPacket+inDestIndex, szTempHex, inLen); 
        inDestIndex+=inLen;
    }

	/*byte 3*/
    if(szHeader[9] & 0x40) //DE18
    {
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, 4); inSrcIndex+=4;
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

		DebugAddHEX("DE18", szTempHex, inLen);

        memcpy(szPacket+inDestIndex, szTempHex, inLen); 
        inDestIndex+=inLen;
    }
    
	/*byte 4*/
    if(szHeader[10] & 0x01) //DE32
    {
		memset(szLen, 0, sizeof(szLen));
		memcpy(szLen, uzSrc+inSrcIndex, 2); inSrcIndex+=2;
        //vdDebug_LogPrintf("Pan Len: %s", szLen);
		
		memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, szLen, 2);
		
		memcpy(szTemp+2, uzSrc+inSrcIndex, atoi(szLen)); inSrcIndex+=atoi(szLen);

		//vdDebug_LogPrintf("Pan: %s", szTemp);
		
        memset(szTempHex, 0, sizeof(szTempHex));
		inLen=wub_str_2_hex(szTemp, szTempHex, strlen(szTemp)+1);

        DebugAddHEX("DE32", szTempHex, inLen);

		memcpy(szPacket+inDestIndex, szTempHex, inLen); 
		inDestIndex+=inLen;
    }

    /*byte 5*/
    if(szHeader[11] & 0x08) //DE37
    {
		inLen=12;
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, inLen); inSrcIndex+=inLen;

		memcpy(szPacket+inDestIndex, szTemp, inLen); 
		inDestIndex+=inLen;
		
        DebugAddHEX("DE37", szTemp, inLen);
    }
    
    if(szHeader[11] & 0x04) //DE38
    {
		inLen=6;
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, inLen); inSrcIndex+=inLen;

		memcpy(szPacket+inDestIndex, szTemp, inLen); 
		inDestIndex+=inLen;
		
        DebugAddHEX("DE38", szTemp, inLen);
    }
    
    if(szHeader[11] & 0x02) //DE39
    {
		inLen=2;
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, inLen); inSrcIndex+=inLen;

		memcpy(szPacket+inDestIndex, szTemp, inLen); 
		inDestIndex+=inLen;
		
        DebugAddHEX("DE39", szTemp, inLen);
    }

    /*byte 6*/
    if(szHeader[12] & 0x80) //DE41
    {
		inLen=8;
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, inLen); inSrcIndex+=inLen;

		memcpy(szPacket+inDestIndex, szTemp, inLen); 
		inDestIndex+=inLen;
		
        DebugAddHEX("DE41", szTemp, inLen);
    }
    
    if(szHeader[12] & 0x20) //DE43
    {
		inLen=40;
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, inLen); inSrcIndex+=inLen;

		memcpy(szPacket+inDestIndex, szTemp, inLen); 
		inDestIndex+=inLen;
		
        DebugAddHEX("DE43", szTemp, inLen);
    }
    
    if(szHeader[12] & 0x01) //DE48
    {
		memset(szLen, 0, sizeof(szLen));
		memcpy(szLen, uzSrc+inSrcIndex, 3); inSrcIndex+=3;
		
        vdDebug_LogPrintf("DE48 Len: %s", szLen);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%04d", atoi(szLen));

		memset(szTempHex, 0, sizeof(szTempHex));
        inLen=wub_str_2_hex(szTemp, szTempHex, 4);

        memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, szTempHex, 2);
		memcpy(szTemp+2, uzSrc+inSrcIndex, atoi(szLen)); inSrcIndex+=atoi(szLen);

        DebugAddHEX("DE48", szTemp, (inLen+atoi(szLen)));

		memcpy(szPacket+inDestIndex, szTemp, (inLen+atoi(szLen))); 
		inDestIndex+=(inLen+atoi(szLen));
    }

    /*byte 7*/
    if(szHeader[13] & 0x80) //DE49
    {
		inLen=3;
        memset(szTemp, 0, sizeof(szTemp));
        memcpy(szTemp, uzSrc+inSrcIndex, inLen); inSrcIndex+=inLen;

		memcpy(szPacket+inDestIndex, szTemp, inLen); 
		inDestIndex+=inLen;
		
        DebugAddHEX("DE49", szTemp, inLen);
    }

	/*byte 8*/
    if(szHeader[14] & 0x04) //DE62
    {
		memset(szLen, 0, sizeof(szLen));
		memcpy(szLen, uzSrc+inSrcIndex, 3); inSrcIndex+=3;
		
        vdDebug_LogPrintf("DE62 Len: %s", szLen);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "%04d", atoi(szLen));

		memset(szTempHex, 0, sizeof(szTempHex));
        inLen=wub_str_2_hex(szTemp, szTempHex, 4);

        memset(szTemp, 0, sizeof(szTemp));
		memcpy(szTemp, szTempHex, 2);
		memcpy(szTemp+2, uzSrc+inSrcIndex, atoi(szLen)); inSrcIndex+=atoi(szLen);

        DebugAddHEX("DE62", szTemp, (inLen+atoi(szLen)));

		memcpy(szPacket+inDestIndex, szTemp, (inLen+atoi(szLen))); 
		inDestIndex+=(inLen+atoi(szLen));
    }

	vdDebug_LogPrintf("inDestIndex Len: %d", inDestIndex);
    DebugAddHEX("form packet", szPacket, inDestIndex);
    memcpy(uzDest, szPacket, inDestIndex);
	
    return inDestIndex;
}

int inPackISOQuickpassData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf)
{
	int inPacketCnt = 0, inTagLen;
	unsigned short usLen, usRetVal;
	BYTE btTrack2[20];
	char szAscBuf[4 + 1], szBcdBuf[2 + 1];
	char szPacket[512 + 1];
    char szTmp[32+1];
	USHORT ushEMVtagLen = 0;
	BYTE   EMVtagVal[64];
	int inRet;

    
	DebugAddSTR("inPackISOQuickpassData load f55","emv",2);	

	vdMyEZLib_LogPrintf("**inPackISOEMVData START**");
	memset(szPacket, 0, sizeof(szPacket));
	inDataCnt = 0;



//remove later --jzg
vdDebug_LogPrintf("inPackISOQuickpassData:: txn type = [%d]", srTransPara->byTransType);
vdDebug_LogPrintf("inPackISOQuickpassData:: pack type = [%d]", srTransPara->byPackType);
		
	if(srTransPara->byPackType != REVERSAL)
	{
	    szPacket[inPacketCnt ++] = 0x5F;
	    szPacket[inPacketCnt ++] = 0x2A;
	    szPacket[inPacketCnt ++] = 2;
	    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T5F2A, 2);
	    inPacketCnt += 2;
	   
#if 0
	    DebugAddINT("5F34 Len",srTransPara->stEMVinfo.T5F34_len );
	    if(srTransPara->stEMVinfo.T5F34_len > 0)
	    {
	        szPacket[inPacketCnt ++] = 0x5F;
	        szPacket[inPacketCnt ++] = 0x34;
	        szPacket[inPacketCnt ++] = 1;
	        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T5F34;
			DebugAddSTR("EMV tag","5f34--finish--",2);
	    }
#endif
	    
	    szPacket[inPacketCnt ++] = 0x82;
	    szPacket[inPacketCnt ++] = 2;
	    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T82, 2);
	    inPacketCnt += 2;

	    
	    szPacket[inPacketCnt ++] = 0x84;		
	    szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T84_len;
	    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T84, srTransPara->stEMVinfo.T84_len);
	    inPacketCnt += srTransPara->stEMVinfo.T84_len;
	}
		
    szPacket[inPacketCnt ++] = 0x95;
    szPacket[inPacketCnt ++] = 5;
    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T95, 5);
    inPacketCnt += 5;

	if(srTransPara->byPackType != REVERSAL)
	{
        szPacket[inPacketCnt ++] = 0x9A;
        szPacket[inPacketCnt ++] = 3;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9A, 3);
        inPacketCnt += 3;

        
        szPacket[inPacketCnt ++] = 0x9C;
        szPacket[inPacketCnt ++] = 1;
        szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9C;  // SL check again  //spec said 2 byte

       
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x02;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F02, 6);
        inPacketCnt += 6;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x03;
        szPacket[inPacketCnt ++] = 6;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F03, 6);
        inPacketCnt += 6;

        
        szPacket[inPacketCnt ++] = 0x9F;
        szPacket[inPacketCnt ++] = 0x09;
        szPacket[inPacketCnt ++] = 2;
        memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F09, 2);
        inPacketCnt += 2;
       	
  	}
    
    szPacket[inPacketCnt ++] = 0x9F;
    szPacket[inPacketCnt ++] = 0x10;
    szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F10_len;
    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F10, srTransPara->stEMVinfo.T9F10_len);
    inPacketCnt += srTransPara->stEMVinfo.T9F10_len;
    
	if(srTransPara->byPackType != REVERSAL)
	{
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x1A;
      szPacket[inPacketCnt ++] = 2;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1A, 2);
      inPacketCnt += 2;
      
      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x1E;
      szPacket[inPacketCnt ++] = 8;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F1E, 8);
      inPacketCnt += 8;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x26;
      szPacket[inPacketCnt ++] = 8;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F26, 8);
      inPacketCnt += 8;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x27;
      szPacket[inPacketCnt ++] = 1;
      szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F27;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x33;
      szPacket[inPacketCnt ++] = 3;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F33, 3);
      inPacketCnt += 3;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x34;
      szPacket[inPacketCnt ++] = 3;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F34, 3);
      inPacketCnt += 3;

      
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x35;
      szPacket[inPacketCnt ++] = 1;
      szPacket[inPacketCnt ++] = srTransPara->stEMVinfo.T9F35;
	}
		
    szPacket[inPacketCnt ++] = 0x9F;
    szPacket[inPacketCnt ++] = 0x36;
    szPacket[inPacketCnt ++] = 2;
    memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F36, 2);
    inPacketCnt += 2;

	if(srTransPara->byPackType != REVERSAL)
	{
      szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x37;
      szPacket[inPacketCnt ++] = 4;
      memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F37, 4);
      inPacketCnt += 4;
	  
	  szPacket[inPacketCnt ++] = 0x9F;
      szPacket[inPacketCnt ++] = 0x41;
      szPacket[inPacketCnt ++] = 3;
	  memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F41, 3);// get chip transaction counter
      inPacketCnt += 3;     

	  
    }

  /*Card Product Identification Information*/  
	vdDebug_LogPrintf("inPackISOQuickpassData::T9F63_len [%d]", srTransPara->stEMVinfo.T9F63_len);
	  if(srTransPara->stEMVinfo.T9F63_len > 0){
	  szPacket[inPacketCnt++] = 0x9F;
	  szPacket[inPacketCnt++] = 0x63;
	  szPacket[inPacketCnt++] = 16;
	  
	  memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F63, 16);
	  inPacketCnt += 16;  

	  #ifdef JCB_LEGACY_FEATURE
	  /*Device Information*/
	  DebugAddINT("9F6E Len", srTransPara->stEMVinfo.T9F6E_len);
	  if(srTransPara->stEMVinfo.T9F6E_len > 0){
		  szPacket[inPacketCnt++] = 0x9F;
		  szPacket[inPacketCnt++] = 0x6E;
		  szPacket[inPacketCnt++] = 4;

		  memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F6E, 4);
		  inPacketCnt += 4; 					  
	  }

	  /*Partner Discretion Data*/
	  DebugAddINT("9F7C Len", srTransPara->stEMVinfo.T9F7C_len);
	  if(srTransPara->stEMVinfo.T9F7C_len){
		  szPacket[inPacketCnt++] = 0x9F;
		  szPacket[inPacketCnt++] = 0x7C;
		  szPacket[inPacketCnt++] = 32;
		  
		  memcpy(&szPacket[inPacketCnt], srTransPara->stEMVinfo.T9F7C, 32);
		  inPacketCnt += 32;		  
	  }
	  #endif
	  
	  }

	
  /* Packet Data Length */
  memset(szAscBuf, 0, sizeof(szAscBuf));
  sprintf(szAscBuf, "%04d", inPacketCnt);
  memset(szBcdBuf, 0, sizeof(szBcdBuf));
  wub_str_2_hex(szAscBuf, szBcdBuf, 4);
  memcpy((char *)&uszUnPackBuf[inDataCnt], &szBcdBuf[0], 2);
  inDataCnt += 2;
  /* Packet Data */
  memcpy((char *)&uszUnPackBuf[inDataCnt], &szPacket[0], inPacketCnt);
  inDataCnt += inPacketCnt;
  
  vdMyEZLib_LogPrintf(". Pack Len(%d)",inDataCnt);
  vdMyEZLib_LogPrintf("**inPackISOEMVData END**");
  return (inDataCnt);
}

