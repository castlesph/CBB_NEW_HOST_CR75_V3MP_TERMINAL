#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

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
#include "..\Debug\Debug.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\Showbmp.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\ISOEnginee.h"
#include "..\Includes\EFTSec.h"
#include "..\Print\Print.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\epad.h"
#include "..\PCI100\COMMS.h"
#include "..\Ctls\POSCtls.h"
#include "..\Erm\PosErm.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\POSMain.h"
#include "..\Includes\POSSetting.h"


#include "External.h"

extern BOOL g_isForcedOnl;
extern int g_inTransType;
extern int inFallbackToMSR;


int inCTOS_EMVGetCardExternal(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	BYTE processID[100];
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];
	int begin;
	char szV3PSendBuf[4096+1];
	int inOffSet = 0;
	char szRecvBuf[4096+1];
	int inRecvlen = 0;
	unsigned char *pszPtr;
	BYTE    szTotalAmt[12+1];
	BYTE    szStr[45];
	CTLS_TransData strCTLSTransData;
	BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;


	memset(&strEMV_Trans,0x00,sizeof(EMV_Trans));
	strEMV_Trans.IPC_EVENT_ID = IPC_EVENT_ID;

	IPC_IN_LEN = 0;
	memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
	memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
	
	IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
	if (inlen > 0)
	{
		memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
		IPC_IN_LEN = IPC_IN_LEN + inlen;
	}
	
	strEMV_Trans.usDataLen1 = IPC_IN_LEN;
	memcpy(strEMV_Trans.pbaData1,IPC_IN_BUF,strEMV_Trans.usDataLen1);

	memset(szOutEMVData,0,sizeof(szOutEMVData));
	inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
	if (inTagLen > 0)
	{
		strEMV_Trans.usDataLen2 = inTagLen;
		memcpy(strEMV_Trans.pbaData2,szOutEMVData,strEMV_Trans.usDataLen2);
	}

	vdPCIDebug_HexPrintf("pbaData1",strEMV_Trans.pbaData1,strEMV_Trans.usDataLen1);
	vdPCIDebug_HexPrintf("pbaData2",strEMV_Trans.pbaData2,strEMV_Trans.usDataLen2);

	vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
	inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

	// send STX INJECT_KEY ETX LRC
	memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
	inOffSet = 0;
	szV3PSendBuf[inOffSet] = STX;
	inOffSet += 1;
	memcpy(&szV3PSendBuf[inOffSet], "EXT_READEMVCARD", strlen("EXT_READEMVCARD"));
	inOffSet += strlen("EXT_READEMVCARD");
	//////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = '|';
	inOffSet += 1;

	//strCTLSTrans.inForcedOnl = g_isForcedOnl;
	memcpy(&szV3PSendBuf[inOffSet], &strEMV_Trans, sizeof(EMV_Trans));
	inOffSet += sizeof(EMV_Trans);
	////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = ETX;
	inOffSet += 1;			
	szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
	inOffSet += 1;
	inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);	

	memset(szRecvBuf,0x00,sizeof(szRecvBuf));
	inRecvlen = 10000;
	g_inTransType = 1;
	status = inCTOSS_V3PRS232RecvBuf(szRecvBuf, &inRecvlen, (d_CEPAS_READ_BALANCE_CARD_PRESENT_TIMEOUT));
	g_inTransType = 0;
	inCTOSS_V3PRS232Close();

	if (status == d_MORE_PROCESSING)
		return d_MORE_PROCESSING;
	if (status == d_NO)
		return d_NO;				
	if (szRecvBuf[0]==0x15)
		return d_NO;				
	DebugAddHEX("inCTOSS_V3PRS232RecvBuf", szRecvBuf, inRecvlen);
	pszPtr = (char*)memchr(szRecvBuf, '|', inRecvlen); // check STX
	//////////////////////////////////////////////////////////////////////////
	memset(&strEMV_Trans,0x00,sizeof(EMV_Trans));
	memcpy(&strEMV_Trans, &pszPtr[1], sizeof(EMV_Trans));

	vdPCIDebug_HexPrintf("111111",strEMV_Trans.pbaData3,strEMV_Trans.usDataLen3);
	vdPCIDebug_HexPrintf("222222",strEMV_Trans.pbaData2,strEMV_Trans.usDataLen2);

	if (strEMV_Trans.usDataLen3 > 0)
	{
		*outlen = strEMV_Trans.usDataLen3;
		memcpy(outbuf,strEMV_Trans.pbaData3,strEMV_Trans.usDataLen3);
		vdDebug_LogPrintf("outlen=[%d]",*outlen);
		vdPCIDebug_HexPrintf("outbuf",outbuf,*outlen);
	}
	
	inMultiAP_Database_EMVTransferDataInit();
	if (strEMV_Trans.usDataLen2 > 0)
	{
    	inMultiAP_Database_EMVTransferDataWrite(strEMV_Trans.usDataLen2, strEMV_Trans.pbaData2);
	}

	return d_OK;
}


short shCTOS_EMVGetChipDataReadyEx(CTLS_TransData strCTLSTransData)
{
    short       shResult;
    BYTE        byDataTmp1[64];
    BYTE        byVal[64];
    USHORT      usLen;
    USHORT      inIndex ;
    BYTE        szDataTmp[5];
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    
    USHORT inTagLen = 0;
    

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szOutEMVData,0,sizeof(szOutEMVData));

	vdDebug_LogPrintf("shCTOS_EMVGetChipDataReadyEx..");
	//for improve transaction speed
    //shResult = usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_SELECT_APP, &inTagLen, szOutEMVData);
	//inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
	inTagLen = strCTLSTransData.bTrack1Len;
	if (inTagLen > 1024)
	{
		memcpy(szOutEMVData,strCTLSTransData.baChipData,  1024);
		memcpy(&szOutEMVData[1024], strCTLSTransData.baAdditionalData, (inTagLen-1024));
	}
	else
	{
		memcpy(szOutEMVData, strCTLSTransData.baChipData, inTagLen);
	}
			
	DebugAddHEX("GET_EMV_TAG_AFTER_SELECT_APP",szOutEMVData,inTagLen);
        
    shResult = usCTOSS_FindTagFromDataPackage(TAG_57, byVal, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_57[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, byVal[0], byVal[1], byVal[2]);

    memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
    wub_hex_2_str(byVal, byDataTmp1, usLen);
    memcpy(srTransRec.szTrack2Data, byDataTmp1, (usLen*2));
    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] == 'F')
            srTransRec.szTrack2Data[inIndex]=0;
    }
    vdDebug_LogPrintf("szTrack2Data: %s %d", srTransRec.szTrack2Data, inIndex);
    
    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] != 'D')
            srTransRec.szPAN[inIndex] = byDataTmp1[inIndex];
        else
            break;
    }
    srTransRec.byPanLen = inIndex;
    vdDebug_LogPrintf("PAN: %s %d", srTransRec.szPAN, inIndex);
    inIndex++;
    memset(szDataTmp, 0x00, sizeof(szDataTmp));
    wub_str_2_hex(&byDataTmp1[inIndex], szDataTmp, 4);
    srTransRec.szExpireDate[0] = szDataTmp[0];
    srTransRec.szExpireDate[1] = szDataTmp[1];
    vdMyEZLib_LogPrintf("EMV functions Expiry Date [%02x%02x]",srTransRec.szExpireDate[0],srTransRec.szExpireDate[1]);
    inIndex = inIndex + 4;
    memcpy(srTransRec.szServiceCode, &byDataTmp1[inIndex], 3);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5A_PAN, srTransRec.stEMVinfo.T5A, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_5A_PAN[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, srTransRec.stEMVinfo.T5A[0], srTransRec.stEMVinfo.T5A[1], srTransRec.stEMVinfo.T5A[2]);

    srTransRec.stEMVinfo.T5A_len = (BYTE)usLen;
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F30_SERVICE_CODE, srTransRec.stEMVinfo.T5F30, &usLen, szOutEMVData, inTagLen);
    
    memset(byVal, 0x00, sizeof(byVal));
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F34_PAN_IDENTFY_NO, byVal, &usLen, szOutEMVData, inTagLen);
    vdMyEZLib_LogPrintf("5F34: %02x %d", byVal[0], usLen);
    srTransRec.stEMVinfo.T5F34_len = usLen;
    srTransRec.stEMVinfo.T5F34 = byVal[0];

    shResult = usCTOSS_FindTagFromDataPackage(TAG_82_AIP, srTransRec.stEMVinfo.T82, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_82_AIP-[%02x][%02x]-", srTransRec.stEMVinfo.T82[0], srTransRec.stEMVinfo.T82[1]); 

    shResult = usCTOSS_FindTagFromDataPackage(TAG_84_DF_NAME, srTransRec.stEMVinfo.T84, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T84_len = (BYTE)usLen;

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F24_EXPIRE_DATE, srTransRec.stEMVinfo.T5F24, &usLen, szOutEMVData, inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F08_IC_VER_NUMBER, szDataTmp, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F08_IC_VER_NUMBER-[%02x][%02x]-",szDataTmp[0],szDataTmp[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F09_TERM_VER_NUMBER-[%02x][%02x]-",srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1]); 
    if(usLen == 0)
        memcpy(srTransRec.stEMVinfo.T9F09, "\x00\x4C", 2);// can not get value from api like ...,so i hardcode a value from EMV level 2 cert document
    vdDebug_LogPrintf("9F09: %02x%02x %d", srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1], usLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F20, srTransRec.szCardholderName, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("5F20,szCardholderName: %s", srTransRec.szCardholderName);

	shResult = usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
	vdDebug_LogPrintf("-------TAG_50[%d] usLen[%d] [%s]--", shResult, usLen, srTransRec.stEMVinfo.szChipLabel);
        
}


int inCTOS_WaveGetCardFieldsExternal(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
	ULONG ulAPRtn;
	BYTE temp[64],temp1[64];
	char szTotalAmount[AMT_ASC_SIZE+1];
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[4+1],szCurrCode[4+1];
	CTLS_Trans strCTLSTrans;
	CTLS_TransData strCTLSTransData;
	BYTE TransaRelatedData[100];

 	vdDebug_LogPrintf("inCTOS_WaveGetCardFieldsExternal.........");
    
 
SWIPE_AGAIN:
EntryOfStartTrans:

	memset(szTotalAmount,0x00,sizeof(szTotalAmount));
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	vdDebug_LogPrintf("szTotalAmount[%s].strTCT.szCTLSLimit=[%s]........",szTotalAmount,strTCT.szCTLSLimit);


	memset(szTransType,0x00,sizeof(szTransType));
	strcpy(szOtherAmt,"000000000000");
	if (srTransRec.byTransType == REFUND)
		strcpy(szTransType,"20");
	else
		strcpy(szTransType,"00");

	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
	strcpy(szCurrCode, szCatgCode);

	
		memset(&strCTLSTransData,0x00,sizeof(CTLS_TransData));
		strcpy(strCTLSTransData.szAmount,szTotalAmount);
		strcpy(strCTLSTransData.szOtherAmt,szOtherAmt);
		strcpy(strCTLSTransData.szTransType,szTransType);
		strcpy(strCTLSTransData.szCatgCode,szCatgCode);
		strcpy(strCTLSTransData.szCurrCode,szCurrCode);
	
	//start for HLB MCCS CONTACTLESS
		strCTLSTransData.inReserved1 = 0;
		strCTLSTransData.inReserved2 = 0;
		memset(TransaRelatedData,0x00,sizeof(TransaRelatedData));
		if(g_isForcedOnl == TRUE)
		{
			TransaRelatedData[strCTLSTransData.inReserved2++] = 0xDF;
			TransaRelatedData[strCTLSTransData.inReserved2++] = 0x9F;
			TransaRelatedData[strCTLSTransData.inReserved2++] = 0x01;
			TransaRelatedData[strCTLSTransData.inReserved2++] = 0x01;
			TransaRelatedData[strCTLSTransData.inReserved2++] = 0x01;
			strCTLSTransData.inReserved1++;
		}
		
		//Add DF7F
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0xDF;
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0x7F;
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0x05;
		memcpy(&TransaRelatedData[strCTLSTransData.inReserved2], "\xA0\x00\x00\x06\x15", 5);
		strCTLSTransData.inReserved2 += 5;
		strCTLSTransData.inReserved1++;
		
		//Add DFAF0E
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0xDF;
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0xAF;
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0x0E;
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0x01;
		TransaRelatedData[strCTLSTransData.inReserved2++] = 0x00;//allow mccs and normal master card
		//TransaRelatedData[strCTLSTransData.inReserved2++] = 0x01;//allow mccs card, not allow normal master card
		strCTLSTransData.inReserved1++;
	
		memcpy(strCTLSTransData.szReserved1,TransaRelatedData,strCTLSTransData.inReserved2);
		vdDebug_LogPrintf("bTagNum=[%d]-usTransactionDataLen=[%d]----",strCTLSTransData.inReserved1,strCTLSTransData.inReserved2);
		vdPCIDebug_HexPrintf("pbaTransactionData",strCTLSTransData.szReserved1,strCTLSTransData.inReserved2);
		vdDebug_LogPrintf("-----szTransType[%s]-----",strCTLSTransData.szTransType);
	
		vdDebug_LogPrintf("**inCTOS_WaveGetCardFieldsExternal**byPinPadMode=[%d],byPinPadType=[%d],byPinPadPort=[%d]",strTCT.byPinPadMode,strTCT.byPinPadType,strTCT.byPinPadPort);


	if(strTCT.byPinPadMode == 0 && strTCT.byPinPadType == 3)
	{
		char szV3PSendBuf[1024+1];
		int inOffSet = 0;
		int status;
		char szRecvBuf[3072+1];
		int inRecvlen = 0;
		unsigned char *pszPtr;
		BYTE    szTotalAmt[12+1];
		BYTE    szStr[45];
#if 0
		vdDispTransTitle(srTransRec.byTransType);
	    CTOS_LCDTPrintXY(1, 5, "TOTAL AMOUNT :");
	    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
	    memset(szStr, 0x00, sizeof(szStr));
		//format amount 10+2
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
	    setLCDPrint(6, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
		CTOS_LCDTPrintAligned(6,szStr,d_LCD_ALIGNRIGHT);
#endif		
		vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
		inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

		// send STX INJECT_KEY ETX LRC
		memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
		inOffSet = 0;
		szV3PSendBuf[inOffSet] = STX;
		inOffSet += 1;
		memcpy(&szV3PSendBuf[inOffSet], "EXT_READCARD", strlen("EXT_READCARD"));
		inOffSet += strlen("EXT_READCARD");
		//////////////////////////////////////////////////////////////////////
		szV3PSendBuf[inOffSet] = '|';
		inOffSet += 1;

		memset(&strCTLSTrans, 0x00, sizeof(CTLS_Trans));
		strcpy(strCTLSTrans.szAmount, szTotalAmount);
		strcpy(strCTLSTrans.szOtherAmt, szOtherAmt);
		strcpy(strCTLSTrans.szTransType, szTransType);
		strcpy(strCTLSTrans.szCatgCode, szCatgCode);
		strcpy(strCTLSTrans.szCurrCode, szCurrCode);
		strCTLSTrans.bTagNum = strCTLSTransData.inReserved1;
		if (strCTLSTrans.bTagNum > 0)
		{
			strCTLSTrans.usTransactionDataLen = strCTLSTransData.inReserved2;
			memcpy(strCTLSTrans.pbaTransactionData, strCTLSTransData.szReserved1,strCTLSTransData.inReserved2);
		}
		//strCTLSTrans.inForcedOnl = g_isForcedOnl;
		memcpy(&szV3PSendBuf[inOffSet], &strCTLSTrans, sizeof(CTLS_Trans));
		inOffSet += sizeof(CTLS_Trans);
		////////////////////////////////////////////////////////////////////
		szV3PSendBuf[inOffSet] = ETX;
		inOffSet += 1;			
		szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
		inOffSet += 1;
		inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);	

		memset(szRecvBuf,0x00,sizeof(szRecvBuf));
		inRecvlen = 10000;
		g_inTransType = 1;
		status = inCTOSS_V3PRS232RecvBuf(szRecvBuf, &inRecvlen, (d_CEPAS_READ_BALANCE_CARD_PRESENT_TIMEOUT));
		g_inTransType = 0;
		inCTOSS_V3PRS232Close();

		if (status == d_MORE_PROCESSING)
			return d_MORE_PROCESSING;
		if (status == d_NO)
			return d_NO;				
		if (szRecvBuf[0]==0x15)
			return d_NO;				
		DebugAddHEX("inCTOSS_V3PRS232RecvBuf", szRecvBuf, inRecvlen);
		pszPtr = (char*)memchr(szRecvBuf, '|', inRecvlen); // check STX
		//////////////////////////////////////////////////////////////////////////
		memcpy(&strCTLSTransData, &pszPtr[1], sizeof(CTLS_TransData));
		usResult = strCTLSTransData.status;
		if (usResult != d_OK)
			usResult = d_NO;
		///////////////////////////////////////////////////////////////////////////
	}


  	vdDebug_LogPrintf("-----TransEntryMode[%d]--status=[%d]---",strCTLSTransData.inReserved3,strCTLSTransData.status);
	if (strCTLSTransData.status != d_OK)
		return d_NO;
	
    {
        if(strCTLSTransData.inReserved3 == CARD_ENTRY_ICC)
        {
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
 #if 0           
            vdDebug_LogPrintf("--EMV flow----" );
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
#endif			
			shCTOS_EMVGetChipDataReadyEx(strCTLSTransData);
            vdDebug_LogPrintf("--EMV Read succ----" );
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            

        }

   
        if(strCTLSTransData.inReserved3 == CARD_ENTRY_MSR)
        {
        	vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
			//pack track1
			if (strCTLSTransData.bTrack1Len > 0)
			{
				usTk1Len = strCTLSTransData.bTrack1Len;
				memcpy(szTk1Buf, strCTLSTransData.baTrack1Data, strCTLSTransData.bTrack1Len);
			}
			//pack track2
			if (strCTLSTransData.bTrack2Len > 0)
			{
				usTk2Len = strCTLSTransData.bTrack2Len;
				memcpy(szTk2Buf, strCTLSTransData.baTrack2Data, strCTLSTransData.bTrack2Len);
			}
			//pack track3
			if (strCTLSTransData.usChipDataLen > 0)
			{
				usTk3Len  = strCTLSTransData.usChipDataLen;
				memcpy(szTk3Buf, strCTLSTransData.baChipData, strCTLSTransData.usChipDataLen);
			}

			usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 
			if (usResult != d_OK)
			{
                 CTOS_KBDBufFlush();
				 vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                 return USER_ABORT;
             }
		
            if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
             }
            
            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
				if (1 == chGetIdleEventSC_MSR())
					return USER_ABORT;
                
                goto SWIPE_AGAIN;

             }
                

        }

		if(strCTLSTransData.inReserved3 == CARD_ENTRY_WAVE)
		{
			vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
			ulAPRtn = d_EMVCL_RC_DATA;
			DebugAddINT("RC Analyze, Trans Result", strCTLSTransData.usTransResult);		
			DebugAddINT("RC Analyze, CVM Analysis", strCTLSTransData.bCVMAnalysis);
			DebugAddHEX("RC Analyze, CVM Result", strCTLSTransData.baCVMResults, sizeof(strCTLSTransData.baCVMResults));
			DebugAddINT("RC Analyze, Visa AOSA Present", strCTLSTransData.bVisaAOSAPresent);
			DebugAddHEX("RC Analyze, Visa AOSA", strCTLSTransData.baVisaAOSA, sizeof(strCTLSTransData.baVisaAOSA));
			DebugAddINT("RC Analyze, ODA Fail", strCTLSTransData.bODAFail);

			DebugAddHEX("SCDataEx DateTime", strCTLSTransData.baDateTime, sizeof(strCTLSTransData.baDateTime));
			DebugAddHEX("SCDataEx Track1", strCTLSTransData.baTrack1Data, strCTLSTransData.bTrack1Len);
			DebugAddHEX("SCDataEx Track2", strCTLSTransData.baTrack2Data, strCTLSTransData.bTrack2Len);		
			DebugAddHEX("SCDataEx Chip", strCTLSTransData.baChipData, strCTLSTransData.usChipDataLen);
			DebugAddHEX("SCDataEx Additional", strCTLSTransData.baAdditionalData, strCTLSTransData.usAdditionalDataLen);

			stRCDataAnalyze.usTransResult = strCTLSTransData.usTransResult;
			stRCDataAnalyze.bCVMAnalysis = strCTLSTransData.bCVMAnalysis;
			memcpy(stRCDataAnalyze.baCVMResults,strCTLSTransData.baCVMResults, sizeof(strCTLSTransData.baCVMResults));
				
			stRCDataEx.bSID = strCTLSTransData.bSID;
			strcpy(stRCDataEx.baDateTime,strCTLSTransData.baDateTime);
			//pack track1
			if (strCTLSTransData.bTrack1Len > 0)
			{
				stRCDataEx.bTrack1Len = strCTLSTransData.bTrack1Len;
				memcpy(stRCDataEx.baTrack1Data, strCTLSTransData.baTrack1Data, strCTLSTransData.bTrack1Len);
			}
			//pack track2
			if (strCTLSTransData.bTrack2Len > 0)
			{
				stRCDataEx.bTrack2Len = strCTLSTransData.bTrack2Len;
				memcpy(stRCDataEx.baTrack2Data, strCTLSTransData.baTrack2Data, strCTLSTransData.bTrack2Len);
			}
			//pack ChipData
			if (strCTLSTransData.usChipDataLen > 0)
			{
				stRCDataEx.usChipDataLen = strCTLSTransData.usChipDataLen;
				memcpy(stRCDataEx.baChipData, strCTLSTransData.baChipData, strCTLSTransData.usChipDataLen);
			}
			//pack AdditionalData
			if (strCTLSTransData.usAdditionalDataLen > 0)
			{
				stRCDataEx.usAdditionalDataLen = strCTLSTransData.usAdditionalDataLen;
				memcpy(stRCDataEx.baAdditionalData, strCTLSTransData.baAdditionalData, strCTLSTransData.usAdditionalDataLen);
			}
				
			CTOS_Beep();
		    CTOS_Delay(50);
		    CTOS_Beep();
			CTOS_Delay(50);
		    CTOS_Beep();
			
		}

	}

	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
	{
		if(ulAPRtn != d_EMVCL_RC_DATA)
		{
			vdCTOSS_WaveCheckRtCode(ulAPRtn);
			if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

			return d_NO;
		}

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
		{
			if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
			{
				inCTOSS_CLMCancelTransaction();
				return d_NO;
			}
		}
		else
		{
			if (d_OK != inCTOSS_V3AnalyzeTransaction(&stRCDataEx))
				return d_NO;
		}

		//Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();
			
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        	inCTOSS_CLMCancelTransaction();

	}

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;
    
    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
}



int inCTOS_WaveGetCardFieldsEXT(BYTE *inbuf, USHORT inlen)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
	ULONG ulAPRtn;
	BYTE temp[64],temp1[64];
	char szTotalAmount[AMT_ASC_SIZE+1];
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[4+1],szCurrCode[4+1];
	char szV3PSendBuf[4096+1];
	int inOffSet = 0;
	int status;
	char szRecvBuf[300+1];
	int inRecvlen = 0;
	unsigned char *pszPtr;
	CTLS_Trans strCTLSTrans;
	CTLS_TransData strCTLSTransData;
	BYTE szOutEMVData[2048];
    
    USHORT inTagLen = 0;
	//ULONG ulAPRtn;

 	vdDebug_LogPrintf("inCTOS_WaveGetCardFieldsEXT.........");

	CTOS_LCDTClearDisplay();

	memset(&strCTLSTransData, 0x00, sizeof(CTLS_TransData));
	memset(&strCTLSTrans, 0x00, sizeof(CTLS_Trans));

	memcpy(&strCTLSTrans, inbuf, sizeof(CTLS_Trans));

	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szCurrCode,0x00,sizeof(szCurrCode));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));

	strcpy(szTotalAmount,strCTLSTrans.szAmount);
	strcpy(szTransType,strCTLSTrans.szTransType);
	strcpy(szCatgCode,strCTLSTrans.szCatgCode);
	strcpy(szCurrCode,strCTLSTrans.szCurrCode);
	strcpy(szOtherAmt,strCTLSTrans.szOtherAmt);

	strCTLSTransData.status = d_NO;

	
    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

            break;
        }


		CTOS_KBDHit(&byKeyBuf);
		if (byKeyBuf == d_KBD_CANCEL)
		{
			break;
		}
#if 0

        CTOS_KBDInKey(&byKeyBuf);
        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();
            
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }
			vdSetFirstIdleKey(0x00);
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }
#endif
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            
            vdDebug_LogPrintf("--EMV flow----" );
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                break;
            }

	    	memset(szOutEMVData,0,sizeof(szOutEMVData));
			inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
			strCTLSTransData.inReserved3 = CARD_ENTRY_ICC;
			strCTLSTransData.status = d_OK;
			strCTLSTransData.bTrack1Len = inTagLen;
			if (inTagLen > 1024)
			{
				strCTLSTransData.usChipDataLen = 1024;
				memcpy(strCTLSTransData.baChipData, szOutEMVData, strCTLSTransData.usChipDataLen);

				strCTLSTransData.usAdditionalDataLen = inTagLen-1024;
				memcpy(strCTLSTransData.baAdditionalData, &szOutEMVData[1024], strCTLSTransData.usAdditionalDataLen);
			}
			else
			{
				strCTLSTransData.usChipDataLen = inTagLen;
				memcpy(strCTLSTransData.baChipData, szOutEMVData, inTagLen);
			}
			
            break;
        }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
		if(byMSR_status == d_OK )//Fix for Track2 Len < 35
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

			{
				strCTLSTransData.inReserved3 = CARD_ENTRY_MSR;
				strCTLSTransData.status = d_OK;
				//pack track1
				if (usTk1Len > 0)
				{
					strCTLSTransData.bTrack1Len = usTk1Len;
					memcpy(strCTLSTransData.baTrack1Data, szTk1Buf, strCTLSTransData.bTrack1Len);
				}
				//pack track2
				if (usTk2Len > 0)
				{
					strCTLSTransData.bTrack2Len = usTk2Len;
					memcpy(strCTLSTransData.baTrack2Data, szTk2Buf, strCTLSTransData.bTrack2Len);
				}
				//pack ChipData
				if (usTk3Len > 0)
				{
					strCTLSTransData.usChipDataLen = usTk3Len;
					memcpy(strCTLSTransData.baChipData, szTk3Buf, strCTLSTransData.usChipDataLen);
				}
			}

            break;
        }

		{
			//memset(szTransType,0x00,sizeof(szTransType));
			//strcpy(szOtherAmt,"000000000000");
			//if (srTransRec.byTransType == REFUND)
			//	strcpy(szTransType,"20");
			//else
			//	strcpy(szTransType,"00");
			ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode,&stRCDataEx);	
			if (ulAPRtn == d_CTLS_EVENT)
				continue;

			strCTLSTransData.status = ulAPRtn;
			if (ulAPRtn == d_OK)
			{
				strCTLSTransData.inReserved3 = CARD_ENTRY_WAVE;
				strCTLSTransData.usTransResult = stRCDataAnalyze.usTransResult;
				strCTLSTransData.bCVMAnalysis = stRCDataAnalyze.bCVMAnalysis;
				memcpy(strCTLSTransData.baCVMResults, stRCDataAnalyze.baCVMResults,sizeof(stRCDataAnalyze.baCVMResults));
					
				strCTLSTransData.bSID = stRCDataEx.bSID;
				strcpy(strCTLSTransData.baDateTime, stRCDataEx.baDateTime);
				//pack track1
				if (stRCDataEx.bTrack1Len > 0)
				{
					strCTLSTransData.bTrack1Len = stRCDataEx.bTrack1Len;
					memcpy(strCTLSTransData.baTrack1Data, stRCDataEx.baTrack1Data, strCTLSTransData.bTrack1Len);
				}
				//pack track2
				if (stRCDataEx.bTrack2Len > 0)
				{
					strCTLSTransData.bTrack2Len = stRCDataEx.bTrack2Len;
					memcpy(strCTLSTransData.baTrack2Data, stRCDataEx.baTrack2Data, strCTLSTransData.bTrack2Len);
				}
				//pack ChipData
				if (stRCDataEx.usChipDataLen > 0)
				{
					strCTLSTransData.usChipDataLen = stRCDataEx.usChipDataLen;
					memcpy(strCTLSTransData.baChipData, stRCDataEx.baChipData, strCTLSTransData.usChipDataLen);
				}
				//pack AdditionalData
				if (stRCDataEx.usAdditionalDataLen > 0)
				{
					strCTLSTransData.usAdditionalDataLen = stRCDataEx.usAdditionalDataLen;
					memcpy(strCTLSTransData.baAdditionalData, stRCDataEx.baAdditionalData, strCTLSTransData.usAdditionalDataLen);
				}
			}

			break;
			
		}

	}


	vdCTOSS_SetV3PRS232Port(strTCT.byRS232ECRPort);
	inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

	// send STX INJECT_KEY ETX LRC
	memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
	inOffSet = 0;
	szV3PSendBuf[inOffSet] = STX;
	inOffSet += 1;
	memcpy(&szV3PSendBuf[inOffSet], "EXT_READCARD", strlen("EXT_READCARD"));
	inOffSet += strlen("EXT_READCARD");
	szV3PSendBuf[inOffSet] = '|';
	inOffSet += 1;				
	//////////////////////////////////////////////////////////////////////////
	memcpy(&szV3PSendBuf[inOffSet], &strCTLSTransData, sizeof(CTLS_TransData));
	inOffSet += sizeof(CTLS_TransData);
	///////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = ETX;
	inOffSet += 1;			
	szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
	inOffSet += 1;
	inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);

	inCTOSS_V3PRS232Close();
	
    return d_OK;
}


int inCTOS_EMVGetCardEXT(BYTE *inbuf, USHORT inlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	BYTE processID[100];
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];
	int begin;
	char szV3PSendBuf[4098+1];
	int inOffSet = 0;
	char szRecvBuf[4098+1];
	int inRecvlen = 0;
	unsigned char *pszPtr;
	BYTE    szTotalAmt[12+1];
	BYTE    szStr[45];
	CTLS_TransData strCTLSTransData;
	BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;
	EMV_Trans strtmpEMV_Trans;

	vdDebug_LogPrintf("inCTOS_EMVGetCardEXT..");

	memset(&strEMV_Trans,0x00,sizeof(EMV_Trans));
	memcpy(strEMV_Trans.pbaData1,inbuf,inlen);
	vdDebug_LogPrintf("IPC_EVENT_ID=[%x].",strEMV_Trans.pbaData1[0]);
	strEMV_Trans.usDataLen1 = inlen;
	vdPCIDebug_HexPrintf("pbaData1",strEMV_Trans.pbaData1,strEMV_Trans.usDataLen1);


	inMultiAP_RunIPCCmdTypes("SHARLS_EMV", strEMV_Trans.pbaData1[0], &strEMV_Trans.pbaData1[1], (strEMV_Trans.usDataLen1-1), szOutEMVData, &inTagLen);

	
	memset(&strtmpEMV_Trans,0x00,sizeof(EMV_Trans));
	strtmpEMV_Trans.IPC_EVENT_ID = strEMV_Trans.pbaData1[0];
	if (inTagLen > 0)
	{
		strtmpEMV_Trans.usDataLen3 = inTagLen;
		memcpy(strtmpEMV_Trans.pbaData3,szOutEMVData,strtmpEMV_Trans.usDataLen3);
	}

	memset(szOutEMVData,0,sizeof(szOutEMVData));
	inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
	if (inTagLen > 0)
	{
		strtmpEMV_Trans.usDataLen2 = inTagLen;
		memcpy(strtmpEMV_Trans.pbaData2,szOutEMVData,strtmpEMV_Trans.usDataLen2);
	}
	vdPCIDebug_HexPrintf("pbaData3",strtmpEMV_Trans.pbaData3,strtmpEMV_Trans.usDataLen3);
	vdPCIDebug_HexPrintf("pbaData2",strtmpEMV_Trans.pbaData2,strtmpEMV_Trans.usDataLen2);

	vdCTOSS_SetV3PRS232Port(strTCT.byRS232ECRPort);
	inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

	// send STX INJECT_KEY ETX LRC
	memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
	inOffSet = 0;
	szV3PSendBuf[inOffSet] = STX;
	inOffSet += 1;
	memcpy(&szV3PSendBuf[inOffSet], "EXT_READEMVCARD", strlen("EXT_READEMVCARD"));
	inOffSet += strlen("EXT_READEMVCARD");
	szV3PSendBuf[inOffSet] = '|';
	inOffSet += 1;				
	//////////////////////////////////////////////////////////////////////////
	memcpy(&szV3PSendBuf[inOffSet], &strtmpEMV_Trans, sizeof(EMV_Trans));
	inOffSet += sizeof(EMV_Trans);
	///////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = ETX;
	inOffSet += 1;			
	szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
	inOffSet += 1;
	inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);

	inCTOSS_V3PRS232Close();
	
    return d_OK;
}





