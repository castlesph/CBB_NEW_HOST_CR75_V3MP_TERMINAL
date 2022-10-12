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

#include "POSCtls.h"
#include "POSWave.h"

int inWaveTransType = 0;

void vdCTOSS_SetWaveTransType(int type) {
    inWaveTransType = type;
}

int inCTOSS_GetWaveTransType(void) {
    return inWaveTransType;
}

int inCTOS_MultiAPReloadWaveData(void) {
    if (srTransRec.usChipDataLen > 0) {
        vdCTOSS_WaveGetEMVData(srTransRec.baChipData, srTransRec.usChipDataLen);
    }

    if (srTransRec.usAdditionalDataLen > 0) {
        vdCTOSS_WaveGetEMVData(srTransRec.baAdditionalData, srTransRec.usAdditionalDataLen);
    }

    return (d_OK);
}

int inCTOS_WaveFlowProcess(void) {
    int inRet = d_NO;


    int inEVENT_ID = 0;

    memset(&stRCDataAnalyze, 0x00, sizeof (EMVCL_RC_DATA_ANALYZE));

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

            inRet = inCTOS_MultiAPReloadTable();
            if (d_OK != inRet)
                return inRet;

            inRet = inCTOS_MultiAPReloadWaveData();
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

    inRet = inCTOS_PreConnectEx();
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
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_CustComputeAndDispTotal();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOS_EMVProcessing();
    if (d_OK != inRet)
        return inRet;

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

    // patrick add code 20141205 start
    //inRet = inMultiAP_ECRSendSuccessResponse();
    //if (d_OK != inRet)
        //return inRet;

    inRet = inCTOS_EMVTCUpload();
    if (d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return d_OK;
}

int inCTOS_WAVE_SALE(void) {
    int inRet = d_NO;

    vdCTOS_TxnsBeginInit();

    vdCTOS_SetTransType(SALE);
    vdCTOSS_SetWaveTransType(1);

    vdCTOSS_GetAmt();

    inRet = inCTOSS_CLMOpenAndGetVersion();
    if (d_OK != inRet)
        return inRet;

    inRet = inCTOSS_ERM_CheckSlipImage();
    if (d_OK != inRet)
        return inRet;

    //display title
    vdDispTransTitle(SALE);

    inRet = inCTOS_WaveFlowProcess();

    vdCTOSS_CLMClose();

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

int inCTOS_WAVE_REFUND(void) {
    int inRet = d_NO;

    //CTOS_LCDTClearDisplay();

//    if (0 == inCTOSS_GetCtlsMode()) {
//        inCTOS_SALE();
//    }

    vdCTOS_TxnsBeginInit();

    vdCTOS_SetTransType(REFUND);
    vdCTOSS_SetWaveTransType(1);

//    vdCTOSS_GetAmt();

//    inRet = inCTOSS_CLMOpenAndGetVersion();
//    if (d_OK != inRet)
//        return inRet;
    //display title
    vdDispTransTitle(REFUND);

    inRet = inCTOS_WaveFlowProcess();

    vdCTOSS_CLMClose();

    inCTOS_inDisconnect();

    if (isCheckTerminalMP200() == d_OK) {
        //CTOS_KBDBufFlush();
        if (d_OK != inRet)
            vdCTOSS_DisplayStatus(inRet);
        //WaitKey(5);
    }

    vdCTOS_TransEndReset();

    return inRet;
}

void vdCTOS_InitWaveData(void) {
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    int inCTLSmode = 0;

    inCTLSmode = inCTOSS_GetCtlsMode();
    if (NO_CTLS == inCTLSmode)
        return;

    //Open the Back Light in the LCD Display //
    vduiKeyboardBackLight(VS_TRUE);
    vduiLightOn();
    CTOS_LCDTClearDisplay();
    vdDispTitleString((BYTE *) "CONTACTLESS");
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "CONFIGURE READER...");

    memset(bOutBuf, 0x00, sizeof (bOutBuf));
    memset(bInBuf, 0x00, sizeof (bInBuf));

    vdDebug_LogPrintf("d_IPC_CMD_EMV_WAVESETTAGS usInLen[%d],inCTLSmode=[%d],GetCtlsMode=[%d] ", usInLen, inCTLSmode, inCTOSS_GetCtlsMode());

    if (strTCT.byPinPadMode == 0) {
        if (strTCT.byPinPadType == 3 && CTLS_V3_SHARECTLS == inCTLSmode) {
            char szV3PSendBuf[1024 + 1];
            int inOffSet = 0;
            int status;
            char szRecvBuf[1024 + 1];
            int inRecvlen = 0;
            unsigned char *pszPtr;

            vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
            inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

            // send STX INJECT_KEY ETX LRC
            memset(szV3PSendBuf, 0x00, sizeof (szV3PSendBuf));
            inOffSet = 0;
            szV3PSendBuf[inOffSet] = STX;
            inOffSet += 1;
            memcpy(&szV3PSendBuf[inOffSet], "INIT_WAVE", strlen("INIT_WAVE"));
            inOffSet += strlen("INIT_WAVE");
            szV3PSendBuf[inOffSet] = ETX;
            inOffSet += 1;
            szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet - 1);
            inOffSet += 1;
            inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);
            status = inCTOSS_V3PRS232RecvACK(szRecvBuf, &inRecvlen, (d_CEPAS_READ_BALANCE_CARD_PRESENT_TIMEOUT));

            //inCTOSS_USBHostCloseEx();
            inCTOSS_V3PRS232Close();
            return;
        }
    }

    if (CTLS_INTERNAL == inCTLSmode || CTLS_EXTERNAL == inCTLSmode) {
        usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_WAVESETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
    }

    if (CTLS_V3_SHARECTLS == inCTLSmode || CTLS_V3_INT_SHARECTLS == inCTLSmode) {
        usResult = inMultiAP_RunIPCCmdTypes("SHARLS_CTLS", d_IPC_CMD_EMV_WAVESETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
    }

}

void vdCTOS_PartialInitWaveData(void) {
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    int inCTLSmode = 0;

    inCTLSmode = inCTOSS_GetCtlsMode();
    if (NO_CTLS == inCTLSmode)
        return;

    //inDatabase_WaveDelete("MCCS", "DF04");
    //inDatabase_WaveUpdate("MCCS", "DFAF11", "888888888888");
    //inDatabase_WaveUpdate("MCCS", "DFAF13", "555555555555");
    //inDatabase_WaveUpdate("MCCS", "DFAF12", "000000000011");
    //inDatabase_WaveInsert("MCCS", "DF05", "11");

    CTOS_LCDTClearDisplay();
    vdDispTitleString((BYTE *) "CONTACTLESS");
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "CONFIGURE READER...");

    memset(bOutBuf, 0x00, sizeof (bOutBuf));
    memset(bInBuf, 0x00, sizeof (bInBuf));

    vdDebug_LogPrintf("d_IPC_CMD_EMV_WAVESETTAGS usInLen[%d],inCTLSmode=[%d],GetCtlsMode=[%d] ", usInLen, inCTLSmode, inCTOSS_GetCtlsMode());
#if 0
    if (strTCT.byPinPadMode == 0) {
        if (strTCT.byPinPadType == 3 && CTLS_V3_SHARECTLS == inCTLSmode) {
            char szV3PSendBuf[1024 + 1];
            int inOffSet = 0;
            int status;
            char szRecvBuf[1024 + 1];
            int inRecvlen = 0;
            unsigned char *pszPtr;

            vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
            inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

            // send STX INJECT_KEY ETX LRC
            memset(szV3PSendBuf, 0x00, sizeof (szV3PSendBuf));
            inOffSet = 0;
            szV3PSendBuf[inOffSet] = STX;
            inOffSet += 1;
            memcpy(&szV3PSendBuf[inOffSet], "INIT_WAVE", strlen("INIT_WAVE"));
            inOffSet += strlen("INIT_WAVE");
            szV3PSendBuf[inOffSet] = ETX;
            inOffSet += 1;
            szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet - 1);
            inOffSet += 1;
            inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);
            status = inCTOSS_V3PRS232RecvACK(szRecvBuf, &inRecvlen, (d_CEPAS_READ_BALANCE_CARD_PRESENT_TIMEOUT));

            //inCTOSS_USBHostCloseEx();
            inCTOSS_V3PRS232Close();
            return;
        }
    }

    if (CTLS_INTERNAL == inCTLSmode || CTLS_EXTERNAL == inCTLSmode) {
        usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_WAVESETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
    }
#endif
    if (CTLS_V3_SHARECTLS == inCTLSmode || CTLS_V3_INT_SHARECTLS == inCTLSmode) {
        usResult = inMultiAP_RunIPCCmdTypes("SHARLS_CTLS", d_IPC_CMD_CTLS_PARTIALSETTAGS, bInBuf, usInLen, bOutBuf, &usOutLen);
    }

}


