

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <CTOSInput.h>

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
#include "..\Includes\CfgExpress.h"
#include "..\Includes\POSSettlement.h"
#include "..\ApTrans\MultiShareCOM.h"
#include "..\Includes\POSSetting.h"



#include "Poserm.h"
//#include "ERMDefine.h"
#define ERM_RECEIPT_FILE		    "/home/ap/pub/logo.bmp"
#define LOCAL_RECEIPT_FILE			"./fs_data/logo.bmp"
#define ERM_BUFF_FILE				"/home/ap/pub/ermbuff.txt"
#define ERM_RECVVIS_FILE			"/home/ap/pub/ermrecvvia.txt"


#define MAX_BUF	6000


BYTE szERMSendData[8094];
BYTE szERMReceiveData[8094];
int inSendLen;
int inReceiveLen;

BYTE g_szPhoneNo[30];
BYTE g_szEmail[100];

int ginSingatureStatus = 0;

void vdGetCenter(unsigned char *strIn,char *strOut)
{
	short i,spacestring;
    USHORT usCharPerLine = 32;
        
    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;
				
	memset(strOut,0x20,55);
	memcpy(strOut+spacestring,strIn,usCharPerLine);	
	
	strOut[i+spacestring]=0;
}



int inCTOSS_FromHeader_Footer(void)
{
	unsigned char uszERCPrinterBuffer [500 + 1];
	int inERCParsePrinterSize = 0;
	char szAPName[25];
	int inAPPID;
	int inHostIndex;
	int inNumOfRecords = 0;
	char tembuf[3+1] = "\n";
	char szOutbuf[100+1];
	
#if 0	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
	vdDebug_LogPrintf("szAPName[%s]....",szAPName);

	inHostIndex = inHDTReadByApname(szAPName);
	vdDebug_LogPrintf("inHostIndex[%d]....",strHDT.inHostIndex);
#endif
	//if (inHostIndex > 0)
	//if (strHDT.inHostIndex > 0)
	{
		//inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfRecords);
		//vdDebug_LogPrintf("inNumOfRecords[%d]....",inNumOfRecords);

		//if (inNumOfRecords > 0)
		{
			memset(uszERCPrinterBuffer,0x00,sizeof(uszERCPrinterBuffer));
			inERCParsePrinterSize = 0;
//			if (strlen(strMMT[0].szRctHdr1) > 0)
                        if (strlen(strTCT.szMerchantAddress) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
//				vdGetCenter(strMMT[0].szRctHdr1,szOutbuf);
                                vdGetCenter(strTCT.szMerchantAddress,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
//			if (strlen(strMMT[0].szRctHdr2) > 0)
                        if (strlen(strTCT.szMerchantAddress) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
//				vdGetCenter(strMMT[0].szRctHdr2,szOutbuf);
                                vdGetCenter(strTCT.szMerchantAddress,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
//			if (strlen(strMMT[0].szRctHdr3) > 0)
                        if (strlen(strTCT.szMerchantAddress) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
//				vdGetCenter(strMMT[0].szRctHdr3,szOutbuf);
                                vdGetCenter(strTCT.szMerchantAddress,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
//			if (strlen(strMMT[0].szRctHdr4) > 0)
                        if (strlen(strTCT.szMerchantAddress) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
//				vdGetCenter(strMMT[0].szRctHdr4,szOutbuf);
                                vdGetCenter(strTCT.szMerchantAddress,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
//			if (strlen(strMMT[0].szRctHdr5) > 0)
                        if (strlen(strTCT.szMerchantAddress) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
//				vdGetCenter(strMMT[0].szRctHdr5,szOutbuf);
                                vdGetCenter(strTCT.szMerchantAddress,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			inWriteFile(ERC_SHARE_HEADER_FILE, uszERCPrinterBuffer, inERCParsePrinterSize);

			memset(uszERCPrinterBuffer,0x00,sizeof(uszERCPrinterBuffer));
			inERCParsePrinterSize = 0;
			if (strlen(strMMT[0].szRctFoot1) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctFoot1,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctFoot2) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctFoot2,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			if (strlen(strMMT[0].szRctFoot3) > 0)
			{
				memset(szOutbuf,0x00,sizeof(szOutbuf));
				vdGetCenter(strMMT[0].szRctFoot3,szOutbuf);
				strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,szOutbuf);
				inERCParsePrinterSize += strlen(szOutbuf);

				strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
				inERCParsePrinterSize += strlen(tembuf);
			}
			strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
			inERCParsePrinterSize += strlen(tembuf);

			strcpy(uszERCPrinterBuffer+inERCParsePrinterSize,"   ***** MERCHANT COPY *****  ");
			inERCParsePrinterSize += strlen("   ***** MERCHANT COPY *****  ");
			strcpy(uszERCPrinterBuffer + inERCParsePrinterSize,tembuf);
			inERCParsePrinterSize += strlen(tembuf);
			inWriteFile(ERC_SHARE_FOOTER_FILE, uszERCPrinterBuffer, inERCParsePrinterSize);

			return d_OK;
		}
	}	

	return d_NO;
}


int inCTOSS_ERM_Form_Receipt(int inSettle)
{
	char szTemp[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	BYTE szfilename[128];

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	if (d_OK == isCheckTerminalMP200() && (strTCT.byERMMode == 4 || strTCT.byERMMode == 2))
	{
		if ((strTCT.byTerminalType%2) == 0)
			setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "Processing E-Receipt");
		else	
    		setLCDPrint(8, DISPLAY_POSITION_LEFT, "Processing  E-Receipt");  
	}

	memset(&strERMTransData,0x00,sizeof(strERMTransData));

	//vdCTOSS_SIGPField61();
	inCTOSS_FormERMReceipt(ERC_SHARE_HEADER_FILE);
	
	inERMAP_Database_BatchDelete();

	strERMTransData.shType = srTransRec.byTransType;
	//strcpy(strERMTransData.szTPDU,);
    if(srTransRec.byTransType == ALIPAY_SALE || srTransRec.byOrgTransType == ALIPAY_SALE)
        strcpy(strERMTransData.szPAN,srTransRec.szRRN);  // for Case 1595, 1600, 1603 , 1605
	else if(srTransRec.byTransType == CB_PAY_TRANS || srTransRec.byOrgTransType == CB_PAY_TRANS)
	{
		strcpy(srTransRec.szRRN, "000000001234");
	    strcpy(srTransRec.szAuthCode, "001234");
		strcpy(srTransRec.szPAN, "000000001234");
		strcpy(strERMTransData.szPAN, srTransRec.szPAN);
	}
	else if(srTransRec.byTransType == OK_DOLLAR_TRANS || srTransRec.byOrgTransType == OK_DOLLAR_TRANS)
	{
		strcpy(srTransRec.szRRN, "000000001234");
	    strcpy(srTransRec.szAuthCode, "001234");
		strcpy(srTransRec.szPAN, "000000001234");
		strcpy(strERMTransData.szPAN, srTransRec.szPAN);
	}	
    else
        strcpy(strERMTransData.szPAN,srTransRec.szPAN);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	strcpy(strERMTransData.szAmt,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTemp, INVOICE_BCD_SIZE);
	strcpy(strERMTransData.szInvNum,szTemp);

	sprintf(strERMTransData.szSTAN, "%06ld", srTransRec.ulTraceNum);

//        if(srTransRec.byTransType == VOID && (fGetMPUTrans() == TRUE)) {
//        CTOS_RTC SetRTC;
//
//        memset(&SetRTC, 0x00, sizeof (CTOS_RTC));
//
//        CTOS_RTCGet(&SetRTC);
//        sprintf(strERMTransData.szTime, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
//        sprintf(strERMTransData.szDate, "%02d%02d", SetRTC.bMonth, SetRTC.bDay);
//    } else {
//
//        memset(szTemp, 0x00, sizeof (szTemp));
//        wub_hex_2_str(srTransRec.szTime, szTemp, TIME_BCD_SIZE);
//        strcpy(strERMTransData.szTime, szTemp);
//
//        memset(szTemp, 0x00, sizeof (szTemp));
//        wub_hex_2_str(srTransRec.szDate, szTemp, DATE_BCD_SIZE);
//        strcpy(strERMTransData.szDate, szTemp);
//    }
	memset (szTemp, 0x00, sizeof(szTemp));
    wub_hex_2_str(srTransRec.szTime, szTemp,TIME_BCD_SIZE);
	strcpy(strERMTransData.szTime,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	strcpy(strERMTransData.szDate,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
	strcpy(strERMTransData.szExpDate,szTemp);

	strcpy(strERMTransData.szRefNum,srTransRec.szRRN);
	strcpy(strERMTransData.szApprCode,srTransRec.szAuthCode);

	strcpy(strERMTransData.szTID,srTransRec.szTID);
	strcpy(strERMTransData.szMID,srTransRec.szMID);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szBatchNo, szTemp,BATCH_NO_BCD_SIZE);
	strcpy(strERMTransData.szBatchNum,szTemp);

	szGetTransTitle(srTransRec.byTransType, strERMTransData.szPaymentType);
	vdDebug_LogPrintf("byTransType=[%d]---szPaymentType[%s]--",srTransRec.byTransType,strERMTransData.szPaymentType);
	strcpy(strERMTransData.szPaymentMedia,srTransRec.szCardLable);
	strcpy(strERMTransData.szReserved3,srTransRec.szHostLabel);

//#define STATUS_SIGNATURE_OK			0
//#define STATUS_SIGPAD_FAILED			1
//#define STATUS_NO_SIGN_REQU			2
//#define	STATUS_CUST_SKIP				3
	//strcpy(strERMTransData.szSingatureStatus,"02");
	sprintf(strERMTransData.szSingatureStatus,"%02d",ginSingatureStatus);

	//getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	//strcpy(exe_subdir, &exe_dir[9]);
	//inExeAPIndex = atoi(exe_subdir);
	inExeAPIndex = srTransRec.HDTid;
	sprintf(strERMTransData.szReceiptLogoIndex, "%02d", inExeAPIndex);
	vdDebug_LogPrintf("inExeAPIndex=[%d]-----",srTransRec.HDTid);

	if (inSettle == 1)
	{
		strERMTransData.fSettle = 1;
		strcpy(strERMTransData.szPaymentMedia,"SIGP");
	}
	else
	{
		strERMTransData.fSettle = 0;
		//if(strTCT.byERMMode == 4)
		{
			if (strlen(g_szPhoneNo) > 0)
				strcpy(strERMTransData.szBankCode,g_szPhoneNo);//huBing_redmine case: #1836
				//strcpy(strERMTransData.szReserved1,g_szPhoneNo);
			if (strlen(g_szEmail) > 0)
				strcpy(strERMTransData.szReserved2,g_szEmail);
			vdDebug_LogPrintf("g_szPhoneNoe=[%s]-g_szEmail=[%s]----",g_szPhoneNo,g_szEmail);
		}
#if 0		
		else
		{
		sprintf(strERMTransData.szReserved1, "ACC%02d%02d.total"
                                , strHDT.inHostIndex
                                , srTransRec.MITid);
		vdDebug_LogPrintf("local filename=[%s]-----",strERMTransData.szReserved1);

		sprintf(strERMTransData.szReserved2, "%d", srTransRec.byOrgTransType);
		}
#endif		
		memcpy(strERMTransData.szReceiptImageFileName,srTransRec.szOrgAmount,AMT_BCD_SIZE);
		vdPCIDebug_HexPrintf("szOrgAmount",srTransRec.szOrgAmount,AMT_BCD_SIZE);
	}

	inERMAP_Database_BatchInsert(&strERMTransData);

	usCTOSS_Erm_FromReceipt();
	
}

#ifdef ERM_ERROR_RECEIPT_FEATURE
int inCTOSS_ERM_Form_Error_Receipt(int inSettle)
{
	char szTemp[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	BYTE szfilename[128];
	char szStr[d_LINE_SIZE + 1];


#if 0
	//new enhancement for MP200 - 05232022
	//Change Request: #15 Change Request Add FLAG to Disable/Enable uploading Error e-receipts to iERS portal 
	int inRet;
	
	inRet = get_env_int("#ERRRCPT");

	vdDebug_LogPrintf("inCTOSS_ERM_Form_Error_Receipt inRet:[%d]",inRet);
   	if(inRet != 1)
       return d_OK;
#endif	

	vdDebug_LogPrintf("inCTOSS_ERM_Form_Error_Receipt HERE!!!");


	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;



	vdDebug_LogPrintf("inCTOSS_ERM_Form_Error_Receipt2 HERE!!!");

	if (d_OK == isCheckTerminalMP200() && (strTCT.byERMMode == 4 || strTCT.byERMMode == 2))
	{
		if ((strTCT.byTerminalType%2) == 0)
			setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "Processing E-Receipt");
		else	
    		setLCDPrint(8, DISPLAY_POSITION_LEFT, "Processing  E-Receipt");  
	}

	memset(&strERMTransData,0x00,sizeof(strERMTransData));

	//vdCTOSS_SIGPField61();
	inCTOSS_FormERMReceipt(ERC_SHARE_HEADER_FILE);
	
	inERMAP_Database_BatchDelete();

	strERMTransData.shType = srTransRec.byTransType;

#if 0	
	//strcpy(strERMTransData.szTPDU,);
    if(srTransRec.byTransType == ALIPAY_SALE || srTransRec.byOrgTransType == ALIPAY_SALE)
        strcpy(strERMTransData.szPAN,srTransRec.szRRN);
    else
        strcpy(strERMTransData.szPAN,srTransRec.szPAN);
#else

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	strcpy(strERMTransData.szAmt,szTemp);

	// RRN 
	strcpy(srTransRec.szRRN, "000000000000"); 
	
	// AuthCode 
	memset (szStr, 0x00, sizeof(szStr));	
	sprintf(szStr, "RC[%s]", srTransRec.szRespCode);		
	strcpy(srTransRec.szAuthCode, szStr); 

	
	// PAN 
	strcpy(strERMTransData.szPAN,srTransRec.szPAN);

#endif
	vdDebug_LogPrintf("inCTOSS_ERM_Form_Error_Receipt szRRN: %s",srTransRec.szRRN);
	vdDebug_LogPrintf("inCTOSS_ERM_Form_Error_Receipt szAuthCode: %s",srTransRec.szAuthCode);
	vdDebug_LogPrintf("inCTOSS_ERM_Form_Error_Receipt szRespCode: %s",srTransRec.szRespCode);
	
	vdDebug_LogPrintf("inCTOSS_ERM_Form_Error_Receipt PAN:%s",srTransRec.szPAN);
	DebugAddHEX("inCTOSS_ERM_Form_Error_Receipt szTotalAmount",srTransRec.szTotalAmount, 6);
	DebugAddHEX("inCTOSS_ERM_Form_Error_Receipt szInvoiceNo",srTransRec.szInvoiceNo, 3);
	DebugAddHEX("inCTOSS_ERM_Form_Error_Receipt szExpireDate",srTransRec.szExpireDate, 2);
	DebugAddHEX("inCTOSS_ERM_Form_Error_Receipt szTID",srTransRec.szTID, 8);	
	DebugAddHEX("inCTOSS_ERM_Form_Error_Receipt szMID",srTransRec.szMID, 15);

	memset (szTemp, 0x00, sizeof(szTemp));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTemp, INVOICE_BCD_SIZE);
	strcpy(strERMTransData.szInvNum,szTemp);

	sprintf(strERMTransData.szSTAN, "%06ld", srTransRec.ulTraceNum);

	memset (szTemp, 0x00, sizeof(szTemp));
    wub_hex_2_str(srTransRec.szTime, szTemp,TIME_BCD_SIZE);
	strcpy(strERMTransData.szTime,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	strcpy(strERMTransData.szDate,szTemp);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
	strcpy(strERMTransData.szExpDate,szTemp);

	strcpy(strERMTransData.szRefNum,srTransRec.szRRN);
	strcpy(strERMTransData.szApprCode,srTransRec.szAuthCode);

	strcpy(strERMTransData.szTID,srTransRec.szTID);
	strcpy(strERMTransData.szMID,srTransRec.szMID);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szBatchNo, szTemp,BATCH_NO_BCD_SIZE);
	strcpy(strERMTransData.szBatchNum,szTemp);

	szGetTransTitle(srTransRec.byTransType, strERMTransData.szPaymentType);
	vdDebug_LogPrintf("byTransType=[%d]---szPaymentType[%s]--",srTransRec.byTransType,strERMTransData.szPaymentType);
	strcpy(strERMTransData.szPaymentMedia,srTransRec.szCardLable);
	strcpy(strERMTransData.szReserved3,srTransRec.szHostLabel);

//#define STATUS_SIGNATURE_OK			0
//#define STATUS_SIGPAD_FAILED			1
//#define STATUS_NO_SIGN_REQU			2
//#define	STATUS_CUST_SKIP				3
	//strcpy(strERMTransData.szSingatureStatus,"02");
	sprintf(strERMTransData.szSingatureStatus,"%02d",ginSingatureStatus);

	//getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	//strcpy(exe_subdir, &exe_dir[9]);
	//inExeAPIndex = atoi(exe_subdir);
	inExeAPIndex = srTransRec.HDTid;
	sprintf(strERMTransData.szReceiptLogoIndex, "%02d", inExeAPIndex);
	vdDebug_LogPrintf("inExeAPIndex=[%d]-----",srTransRec.HDTid);

	if (inSettle == 1)
	{
		strERMTransData.fSettle = 1;
		strcpy(strERMTransData.szPaymentMedia,"SIGP");
	}
	else
	{
		strERMTransData.fSettle = 0;
		//if(strTCT.byERMMode == 4)
		{
			if (strlen(g_szPhoneNo) > 0)
				strcpy(strERMTransData.szBankCode,g_szPhoneNo);//huBing_redmine case: #1836
				//strcpy(strERMTransData.szReserved1,g_szPhoneNo);
			if (strlen(g_szEmail) > 0)
				strcpy(strERMTransData.szReserved2,g_szEmail);
			vdDebug_LogPrintf("g_szPhoneNoe=[%s]-g_szEmail=[%s]----",g_szPhoneNo,g_szEmail);
		}
#if 0		
		else
		{
		sprintf(strERMTransData.szReserved1, "ACC%02d%02d.total"
                                , strHDT.inHostIndex
                                , srTransRec.MITid);
		vdDebug_LogPrintf("local filename=[%s]-----",strERMTransData.szReserved1);

		sprintf(strERMTransData.szReserved2, "%d", srTransRec.byOrgTransType);
		}
#endif		
		memcpy(strERMTransData.szReceiptImageFileName,srTransRec.szOrgAmount,AMT_BCD_SIZE);
		vdPCIDebug_HexPrintf("szOrgAmount",srTransRec.szOrgAmount,AMT_BCD_SIZE);
	}

	inERMAP_Database_BatchInsert(&strERMTransData);

	usCTOSS_Erm_FromReceipt();
	
}

#endif

int inCTOS_ERMAllHosts_Initialization(void)
{
	int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	int inNumOfMit = 0,inMitNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[25];
	int inAPPID;
    char szParaName[10+1];
    char szEnvValue[3+1];
    int inHDTIndex=0, inHDTid[24];
	
	USHORT inExeAPIndex = 0;
	BOOL fInit = TRUE;

	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

//	inNumOfHost = inHDTNumRecord();
    //inNumOfHost = inHDTMAX();
    
    inNumOfHost=inHDTReadEnabledHostID(inHDTid);
	vdDebug_LogPrintf("x.inNumOfHost=[%d]-----",inNumOfHost);
	
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		inHDTIndex=inHDTid[inNum-1];
		//if(inHDTRead(inNum) == d_OK)
		if(inHDTRead(inHDTIndex) == d_OK)
		{

			#ifdef OK_DOLLAR_FEATURE
			vdHostToInitialize(FALSE, FALSE);
			setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
			#endif
		
			vdDebug_LogPrintf("szAPName=[%s]-[%s]-inHostIndex=[%d]---",szAPName,strHDT.szAPName,strHDT.inHostIndex);
			if (strcmp(szAPName, strHDT.szAPName)!=0)
			{
				continue;
			}
			if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
			{
				continue;
			}
                        
			inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
                        
                        memset(szParaName, 0x00, sizeof(szParaName));
                        memset(szEnvValue, 0x00, sizeof(szEnvValue));
                        sprintf(szParaName, "ERM%02d%02d", strHDT.inHostIndex, strMMT[0].MITid);
                        inCTOSS_GetEnv(szParaName, szEnvValue);
                        
                        vdDebug_LogPrintf("baim %s value = %s", szParaName, szEnvValue);
                        
                        if(atoi(szEnvValue) == 1)
                            continue;
                        
                        vdDebug_LogPrintf("MMT inNumOfMit=[%d]-----",inNumOfMit);
			for(inMitNum =0 ;inMitNum < inNumOfMit; inMitNum++)
			{

				#ifdef OK_DOLLAR_FEATURE
				fInit = TRUE;
				vdHostToInitialize(FALSE, FALSE);

				setLCDPrint27(8,DISPLAY_POSITION_LEFT,"PROCESSING...");
				#endif
				
				memcpy(&strMMT[0],&strMMT[inMitNum],sizeof(STRUCT_MMT));
				inCTOSS_ERM_Initialization();
			
				#ifdef OK_DOLLAR_FEATURE				
				memset(szParaName, 0x00, sizeof(szParaName));
				inExeAPIndex = strHDT.inHostIndex;
				sprintf(szParaName,"%s%02d%02d","ERM",inExeAPIndex,strMMT[0].MITid);
				vdDebug_LogPrintf("szParaName =[%s]",szParaName);
				
				if (get_env_int(szParaName) > 0)
				{
					vdHostToInitialize(TRUE, TRUE);
				}
				else
				{
					fInit = FALSE;
					vdHostToInitialize(TRUE, FALSE);	
				}
				#endif
			}
		}
	}

	#ifdef OK_DOLLAR_FEATURE
	vdDebug_LogPrintf("end inCTOS_ERMAllHosts_Initialization-----");
	vdDebug_LogPrintf("fInit[%d]", fInit);
	
	if (!fInit)
		return d_NO;
	else
		return ST_SUCCESS;
	#else
		return ST_SUCCESS;
	#endif
}

	
int inCTOSS_ERM_Initialization(void)
{
	int result;
	char szTemp[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	long length;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;
	BYTE szParaName[100];
	BYTE tmpbuf[100];
    int inParkingFee=0;
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	//getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	//strcpy(exe_subdir, &exe_dir[9]);
	//inExeAPIndex = atoi(exe_subdir);
	inExeAPIndex = strHDT.inHostIndex;

	memset(szParaName,0x00,sizeof(szParaName));
	//sprintf(szParaName,"%s%02d","ERM",inExeAPIndex);
	sprintf(szParaName,"%s%02d%02d","ERM",inExeAPIndex,strMMT[0].MITid);
	vdDebug_LogPrintf("szParaName =[%s]",szParaName);
        
        if(strTCT.byERMInit == 0){
            inCTOSS_PutEnvDB(szParaName, "0");
        }
        
	inCTOSS_GetEnvDB(szParaName,tmpbuf);
	vdDebug_LogPrintf("[%s] =[%s]",szParaName,tmpbuf);
	if (strlen(tmpbuf) > 0 && atoi(tmpbuf) == 1)
		return ST_SUCCESS;
	
	memset(&strERMTransData,0x00,sizeof(strERMTransData));	

	result = inCTOSS_FromHeader_Footer();

	//if (result == d_OK)
	{
		strcpy(strERMTransData.szTID,strMMT[0].szTID);
		strcpy(strERMTransData.szMID,strMMT[0].szMID);
		vdDebug_LogPrintf("szTID[%s]..szMID=[%s]..",strERMTransData.szTID,strERMTransData.szMID);
	}

	strcpy(strERMTransData.szLogoFileName,"logo.bmp");
	sprintf(strERMTransData.szReceiptLogoIndex, "%02d", inExeAPIndex);
	vdDebug_LogPrintf("inExeAPIndex=[%d]-----",inExeAPIndex);

#if 1
//receipt logo will upload in whole receipt
	length = lnGetFileSize(ERM_RECEIPT_FILE);
	if (length > 0)
		inDeleteFile(ERM_RECEIPT_FILE);

    #ifdef PARKING_FEE
        inParkingFee = get_env_int("PARKFEE");
    #endif
	if(inParkingFee == 1)
		inCopyFile("./fs_data/omni_logo.bmp",ERM_RECEIPT_FILE);
	else
		inCopyFile(LOCAL_RECEIPT_FILE,ERM_RECEIPT_FILE);
	
	lnGetFileSize(ERM_RECEIPT_FILE);
#endif	
	inERMAP_Database_BatchDelete();

	inERMAP_Database_BatchInsert(&strERMTransData);

	result = usCTOSS_Erm_Initialization();
	
	memset(szParaName,0x00,sizeof(szParaName));
	//sprintf(szParaName,"%s%02d","ERM",inExeAPIndex);
	sprintf(szParaName,"%s%02d%02d","ERM",inExeAPIndex,strMMT[0].MITid);
	vdDebug_LogPrintf("szParaName =[%s]",szParaName);
	if (result == d_OK)
		inCTOSS_PutEnvDB(szParaName,"1");
	else
		inCTOSS_PutEnvDB(szParaName,"0");
	
}


USHORT usCTOSS_Erm_Initialization(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	
	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_INITIALIZATION, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_EMV_MOL_Wget usOutLen[%d] bOutBuf[%s]",usOutLen, bOutBuf);
	if (usResult == d_OK)
	{
		if (bOutBuf[1] == d_SUCCESS)
			usResult = d_OK;
		else
			usResult = d_NO;
	}
    return usResult;
}

USHORT usCTOSS_Erm_FromReceipt(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_FORM_RECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_EMV_MOL_Wget usOutLen[%d] bOutBuf[%s]",usOutLen, bOutBuf);
    return usResult;
}

USHORT usCTOSS_Erm_UploadReceipt(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_UPLOAD_RECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);
        vdDebug_LogPrintf("usResult = %d", usResult);
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt usOutLen[%d] bOutBuf[%s]",usOutLen, bOutBuf);
	if (bOutBuf[1] == d_SUCCESS)
		usResult = d_OK;
	else
		usResult = d_NO;

	#ifdef OK_DOLLAR_FEATURE
	if (usResult == d_OK)
	{	
		vdDisplayErrorMsgResp2("E-RECEIPT SENT", "", "SUCCESSFULLY");
	}
	else
	{
		vdDisplayErrorMsgResp2("E-RECEIPT SEND", "", "FAILED");	
	}
	#endif
    return usResult;
}

UINT usCTOSS_Erm_GetReceiptTotal(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_GET_RECEIPTTOT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt usOutLen[%d] bOutBuf[%x,%d,%d]",usOutLen, bOutBuf[0],bOutBuf[1],bOutBuf[2]);
	if(d_OK == usResult)
	{
		if(bOutBuf[0] == d_IPC_CMD_ERM_GET_RECEIPTTOT)
			//inTotal = bOutBuf[1];
			inTotal = (bOutBuf[1] | bOutBuf[2] << 8);

		vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt inTotal[%d] ",inTotal);
	}
	
    return inTotal;
}


int usCTOSS_Erm_GetERMInfo(char* pszACCUMFileName,TRANS_TOTAL_ERM *srErmRec)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[2048];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;
	

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	strcpy(bInBuf,pszACCUMFileName);
	usInLen = strlen(bInBuf);
	vdDebug_LogPrintf("usInLen = [%d] bInBuf[%s]",usInLen, bInBuf);

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_GET_ERMINFO, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt usOutLen[%d] bOutBuf[%x,%d]",usOutLen, bOutBuf[0],bOutBuf[1]);
	if(d_OK == usResult)
	{
		if(bOutBuf[0] == d_IPC_CMD_ERM_GET_ERMINFO)
		{
			memcpy(srErmRec,&bOutBuf[1],sizeof(TRANS_TOTAL_ERM));

			vdDebug_LogPrintf("stUploadTotal SaleCount[%d] [%12.0f]", srErmRec->stUploadTotal.usSaleCount, srErmRec->stUploadTotal.ulSaleTotalAmount);
		    vdDebug_LogPrintf("stUploadTotal OfflCount[%d] [%12.0f]", srErmRec->stUploadTotal.usOffSaleCount, srErmRec->stUploadTotal.ulOffSaleTotalAmount);
		    vdDebug_LogPrintf("stUploadTotal RefdCount[%d] [%12.0f]", srErmRec->stUploadTotal.usRefundCount, srErmRec->stUploadTotal.ulRefundTotalAmount);            
		    vdDebug_LogPrintf("stUploadTotal VoidCount[%d] [%12.0f]", srErmRec->stUploadTotal.usVoidSaleCount, srErmRec->stUploadTotal.ulVoidSaleTotalAmount);    
		    vdDebug_LogPrintf("stUploadTotal TipCount [%d] [%12.0f]", srErmRec->stUploadTotal.usTipCount, srErmRec->stUploadTotal.ulTipTotalAmount);
		}
	}
	
    return ST_SUCCESS;
}



UINT usCTOSS_Erm_DeleteReceipt(int DelTotal)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	sprintf(bInBuf,"%03d",DelTotal);
	usInLen = 3;

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_DELETE_RECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt usOutLen[%d] bOutBuf[%x,%d]",usOutLen, bOutBuf[0],bOutBuf[1]);
	
    return usResult;
}

int inCTOSS_ERM_CheckSlipImage(void)
{
	int inTotal = 0;
	int loop = 0;

	if(strTCT.byERMMode == 0)
		return d_OK;
	vdDebug_LogPrintf("usCTOSS_Erm_GetReceiptTotal.........");
	
	inTotal = usCTOSS_Erm_GetReceiptTotal();
	vdDebug_LogPrintf("usCTOSS_Erm_GetReceiptTotal inTotal[%d],inERMMaximum=[%d]",inTotal,strTCT.inERMMaximum);
	if (inTotal > strTCT.inERMMaximum)
	{
		inCTOS_SETTLE_ALL();

		while (1)
		{
			inTotal = usCTOSS_Erm_GetReceiptTotal();
			vdDebug_LogPrintf("usCTOSS_Erm_GetReceiptTotal inTotal[%d],",inTotal);
			if (inTotal > 0)
			{
				inCTOSS_UploadReceipt();
				
				loop ++;
				if (loop > 3)
					break;
			}
			else
				break;
		}

		return d_NO;
	}

	return d_OK;
}


int inCTOSS_ERM_CheckInit(void)
{
	BYTE tmpbuf[100];
	BYTE szParaName[100];
	int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	int inNumOfMit = 0,inMitNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	int inAPPID;
    int inHDTIndex=0, inHDTid[24];
	
	vdDebug_LogPrintf("inCTOSS_ERM_CheckInit ............");
	
//	inNumOfHost = inHDTNumRecord();
        //inNumOfHost = inHDTMAX();
    inNumOfHost=inHDTReadEnabledHostID(inHDTid);
	vdDebug_LogPrintf("y.inNumOfHost=[%d]-----",inNumOfHost);
	if (inNumOfHost >= 1)
	{
		for(inNum =1 ;inNum <= inNumOfHost; inNum++)
		{
			inHDTIndex=inHDTid[inNum-1];
			//if(inHDTRead(inNum) == d_OK)
			if(inHDTRead(inHDTIndex) == d_OK)
			{
				vdDebug_LogPrintf("szHostLabel-[%s]-inHostIndex=[%d]---",strHDT.szHostLabel,strHDT.inHostIndex);
				if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
				{
					continue;
				}
				
				inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
				vdDebug_LogPrintf("MMT inNumOfMit=[%d]-----",inNumOfMit);
				for(inMitNum =0 ;inMitNum < inNumOfMit; inMitNum++)
				{
					memcpy(&strMMT[0],&strMMT[inMitNum],sizeof(STRUCT_MMT));
				memset(szParaName,0x00,sizeof(szParaName));
					sprintf(szParaName,"%s%02d%02d","ERM",strHDT.inHostIndex,strMMT[0].MITid);
				vdDebug_LogPrintf("szParaName =[%s]",szParaName);
				
				inCTOSS_GetEnvDB(szParaName,tmpbuf);
				vdDebug_LogPrintf("[%s] =[%s]",szParaName,tmpbuf);
				if (strlen(tmpbuf) > 0 && atoi(tmpbuf) == 1)
					continue;
				else
					return d_NO;
			}
		}
	}
	}
	else
		return d_NO;
	
	vdDebug_LogPrintf("inCTOSS_ERM_CheckInit-----");
	return d_OK;
	

}



long lnGetFileSize(const char* pchFileName)
{
    FILE  *fPubKey;
	long curpos,length;

	vdDebug_LogPrintf("lnGetFileSize[%s]", pchFileName);
	fPubKey = fopen( (char*)pchFileName, "rb" );
	vdDebug_LogPrintf("fPubKey[%x]", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fclose(fPubKey);
	vdDebug_LogPrintf("lnGetFileSize[%d],length=[%d]", curpos,length);

    return(length);
}


int inWriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	
	vdDebug_LogPrintf("inWriteFile[%s]", pchFileName);
	if((inRetVal = lnGetFileSize(pchFileName)) >= 0)
		remove(pchFileName);
	
	fPubKey = fopen((char *)pchFileName, "wb+" );
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	inRetVal = fwrite(pchRecBuf,inMaxRecSize, 1, fPubKey);
	fclose(fPubKey);

	vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}

int inCopyFile(unsigned char *pchFileName1, unsigned char *pchFileName2)
{
	int h_file;
	int inRetVal = 0,curpos;
	FILE  *fPubKey1,*fPubKey2;
	unsigned char uszBufferData [(SIGP_SEND_SIZE) + 1];
	int infileSize = 0;
	
	vdDebug_LogPrintf("inCopyFile[%s][%s]", pchFileName1,pchFileName2);
	
	fPubKey1 = fopen( (char*)pchFileName1, "rb" );
	vdDebug_LogPrintf("fPubKey1[%x]....", fPubKey1);
	if(fPubKey1 == NULL)
		return SM_RET_PARAM;

	curpos=ftell(fPubKey1);
	fseek(fPubKey1,0L,SEEK_END);
	infileSize=ftell(fPubKey1);
	fseek(fPubKey1,curpos,SEEK_SET);
	fread( (char *)uszBufferData, 1, infileSize, fPubKey1 );
	fclose(fPubKey1);
	
	
	fPubKey2 = fopen((char *)pchFileName2, "wb+" );
	vdDebug_LogPrintf("fPubKey2[%x]....", fPubKey1);
	if(fPubKey2 == NULL)
		return SM_RET_PARAM;

	inRetVal = fwrite(uszBufferData,infileSize, 1, fPubKey2);
	fclose(fPubKey2);

	vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,infileSize);
	vdDebug_LogPrintf("uszBufferData[%s]....", uszBufferData);

	return inRetVal;
}

int inDeleteGzipFile(unsigned char *pchFileName)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	unsigned char uszFileName[20 + 1];
	memset(uszFileName, 0x00, sizeof(uszFileName));
	strcpy(uszFileName, pchFileName);
	strcat(uszFileName, ".gz");
	
	vdDebug_LogPrintf("inDeleteGzipFile[%s]", uszFileName);
	if((inRetVal = lnGetFileSize(uszFileName)) >= 0)
		remove(uszFileName);
	
	fPubKey = fopen((char *)uszFileName, "wb+" );
	vdDebug_LogPrintf("inDeleteGzipFile[%x]", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	fclose(fPubKey);

	return ST_SUCCESS;
}


int inDeleteFile(unsigned char *pchFileName)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	unsigned char uszFileName[20 + 1];
	memset(uszFileName, 0x00, sizeof(uszFileName));
	strcpy(uszFileName, pchFileName);
	
	vdDebug_LogPrintf("inDeleteGzipFile[%s]", uszFileName);
	if((inRetVal = lnGetFileSize(uszFileName)) >= 0)
		remove(uszFileName);
	
	fPubKey = fopen((char *)uszFileName, "wb+" );
	vdDebug_LogPrintf("inDeleteGzipFile[%x]", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	fclose(fPubKey);

	return ST_SUCCESS;
}


int inReadGzipFile(unsigned char *pchFileName, unsigned char* uszBufferData, int *inBufferSize)
{
	int h_file;
	int inRetVal = 0, inFileSize = 0;
	unsigned char uszFileName [20 + 1];
	FILE  *fPubKey;
	long curpos,length;
	

	memset(uszFileName, 0x00, sizeof(uszFileName));
	strcpy(uszFileName, pchFileName);
	strcat(uszFileName, ".gz");

	vdDebug_LogPrintf("inReadGzipFile[%s]", uszFileName);

	fPubKey = fopen( (char*)uszFileName, "rb" );
	vdDebug_LogPrintf("fPubKey[%x]....", fPubKey);
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fread( (char *)uszBufferData, 1, length, fPubKey );
	fclose(fPubKey);

	*inBufferSize = length;
	vdDebug_LogPrintf("inReadGzipFile=[%d][%d]", length,*inBufferSize);

	return ST_SUCCESS;
}

int inCopyBMP2Buffer(unsigned char* uszFileName, unsigned char* uszBufferData, int *inBufferSize)
{
	int h_HandleLogoFile;
	int inFileSize = 0;
	FILE  *fPubKey;
	int inResult = d_NO;

	vdDebug_LogPrintf("inCopyBMP2Buffer[%s]", uszFileName);
	if((inFileSize = lnGetFileSize(uszFileName)) < 0)
    {
    	return ST_SUCCESS;
    }

	fPubKey = fopen( (char*)uszFileName, "rb" );
	if(fPubKey == NULL)
		return SM_RET_PARAM;

	*inBufferSize = fread( (char *)uszBufferData, 1, inFileSize, fPubKey );
	fclose(fPubKey);

	vdDebug_LogPrintf("inCopyBMP2Buffer=[%d][%d]", inFileSize,*inBufferSize);
	
	return(d_OK);		
}


int inCTOSS_ERMInit(void)
{
	int inResult = d_NO;
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;
	
	vdDebug_LogPrintf("inCTOSS_ERMInit..byERMInit=[%d]",strTCT.byERMInit);
	if(strTCT.byERMInit == 1)
		return ST_SUCCESS;

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		//inCTOSS_ERM_Initialization();
		inResult = inCTOS_ERMAllHosts_Initialization();
		inCTOS_MultiAPALLAppEventID(d_IPC_CMD_ERM_INIT);
		
		inResult = inCTOSS_ERM_CheckInit();
		vdDebug_LogPrintf("inCTOSS_ERM_CheckInit =[%d]",inResult);
		if (inResult == d_OK)
		{
			strTCT.byERMInit = 1;
			inTCTSave(1);
		}
	}
	else
		//inCTOSS_ERM_Initialization();
		inResult = inCTOS_ERMAllHosts_Initialization();

	// fix for issue Should display "ERM INIT COMPLETE" or other related message - 10001
	#ifdef OK_DOLLAR_FEATURE
	vdDebug_LogPrintf("inCTOS_ERMAllHosts_Initialization : inResult[%d]", inResult);

	if (inResult != d_OK)
		vdDisplayErrorMsgResp2("ERM INIT", "", "FAILED");
	else
		vdDisplayErrorMsgResp2("ERM INIT", "", "COMPLETE");
	
	return inResult;
	
	#else
	
	return ST_SUCCESS;
	#endif
}

int inCTOSS_UploadReceipt(void)
{
	BYTE key;
	
	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

	//CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("inCTOSS_UploadReceipt.........");
	inCTOSS_ERMInit();

	usCTOSS_Erm_UploadReceipt();

	return ST_SUCCESS;
}

UINT usCTOSS_Erm_RestoreBackupReceipt(void)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	UINT inTotal = 0;

	if(strTCT.byERMMode == 0)
		return ST_SUCCESS;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_RESTORE_BACKUPRECEIPT, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_Erm_UploadReceipt usOutLen[%d] bOutBuf[%x,%d,%d]",usOutLen, bOutBuf[0],bOutBuf[1],bOutBuf[2]);
	
    return inTotal;
}



long inCTOSS_ERM_GetFileSize(const char* pchFileName)
{
    FILE  *fPubKey;
	long curpos,length;

	vdDebug_LogPrintf("lnGetFileSize[%s]", pchFileName);
	fPubKey = fopen( (char*)pchFileName, "rb" );
	if(fPubKey == NULL)
		return -1;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fclose(fPubKey);
	vdDebug_LogPrintf("lnGetFileSize[%d],length=[%d]", curpos,length);

    return(length);
}


int inCTOSS_ERM_WriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	
	vdDebug_LogPrintf("inWriteFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);
	
	fPubKey = fopen((char *)pchFileName, "wb+" );
	if(fPubKey == NULL)
		return -1;

	if (inMaxRecSize > MAX_BUF)
	{
		times = inMaxRecSize/MAX_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_BUF],MAX_BUF, 1, fPubKey);
		}

		times = inMaxRecSize%MAX_BUF;
		if(times>0)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_BUF],times, 1, fPubKey);
		}
		
	}
	else
	inRetVal = fwrite(pchRecBuf,inMaxRecSize, 1, fPubKey);
	fclose(fPubKey);

	vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}

int inCTOSS_ERM_ReadFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	
	vdDebug_LogPrintf("ReadFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);
	
	fPubKey = fopen((char *)pchFileName, "rb" );
	if(fPubKey == NULL)
		return -1;

	if (inMaxRecSize > MAX_BUF)
	{
		times = inMaxRecSize/MAX_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_BUF], 1, MAX_BUF, fPubKey);
		}

		times = inMaxRecSize%MAX_BUF;
		if(times>0)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_BUF], 1, times, fPubKey);
		}
		
	}
	else
	inRetVal = fread (pchRecBuf, 1, inMaxRecSize, fPubKey);
	fclose(fPubKey);

	vdDebug_LogPrintf("ReadFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}


int inMultiAP_Database_ERM_ClearEx(void)
{
	int inRetVal = 0;

	memset(szERMSendData,0x00,sizeof(szERMSendData));
	memset(szERMReceiveData,0x00,sizeof(szERMReceiveData));

	inRetVal = inCTOSS_ERM_GetFileSize(ERM_BUFF_FILE);
	vdDebug_LogPrintf("[%s] =[%d] ", ERM_BUFF_FILE,inRetVal);
	if (inRetVal >= 0)
		remove(ERM_BUFF_FILE);

	return d_OK;
}

int inMultiAP_Database_ERM_SaveEx(void)
{
	int inRetVal = 0;

	vdDebug_LogPrintf("inMultiAP_Database_ERM_SaveEx inSendLen=[%d] ", inSendLen);
	if (inSendLen > 0)
	{
		inRetVal = inCTOSS_ERM_WriteFile(ERM_BUFF_FILE,szERMSendData,inSendLen);
		if (inRetVal <= 0)
		return d_NO;
	}

	return d_OK;
}

int inMultiAP_Database_ERM_ReadEx(void)
{
	int inRetVal = 0;

	inReceiveLen = inCTOSS_ERM_GetFileSize(ERM_BUFF_FILE);
	vdDebug_LogPrintf("inMultiAP_Database_ERM_ReadEx inReceiveLen=[%d] ", inReceiveLen);
	if (inReceiveLen > 0)
	{
		memset(szERMReceiveData,0x00,sizeof(szERMReceiveData));
		inRetVal = inCTOSS_ERM_ReadFile(ERM_BUFF_FILE,szERMReceiveData,inReceiveLen);
		if (inRetVal <= 0)
			return d_NO;
	}

	return d_OK;
}



int inCTOSS_ERM_SaveERMInfo(void)
{
	char szTemp[d_LINE_SIZE + 1];
	ERM_TransData strERMTransData;
	CTOS_RTC SetRTC;
    BYTE szCurrentTime[20];

	memset(&strERMTransData,0x00,sizeof(strERMTransData));
	
	inERMAP_Database_BatchDelete();

	sprintf(strERMTransData.szSTAN, "%06ld", srTransRec.ulTraceNum);
	//testing
	//sprintf(strERMTransData.szSTAN, "%06ld", 99);
	vdDebug_LogPrintf("ulTraceNum=[%06ld]", srTransRec.ulTraceNum);


    CTOS_RTCGet(&SetRTC);
    sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
    wub_str_2_hex(szCurrentTime,srTransRec.szDate,DATE_ASC_SIZE);
    sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
    wub_str_2_hex(szCurrentTime,srTransRec.szTime,TIME_ASC_SIZE);
		
	memset (szTemp, 0x00, sizeof(szTemp));
    wub_hex_2_str(srTransRec.szTime, szTemp,TIME_BCD_SIZE);
	strcpy(strERMTransData.szTime,szTemp);
	vdDebug_LogPrintf("szTime=[%s]", strERMTransData.szTime);

	memset (szTemp, 0x00, sizeof(szTemp));
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	strcpy(strERMTransData.szDate,szTemp);
	vdDebug_LogPrintf("szDate=[%s]", strERMTransData.szDate);


	strcpy(strERMTransData.szTID,srTransRec.szTID);
	strcpy(strERMTransData.szMID,srTransRec.szMID);
	//testing
	//strcpy(strERMTransData.szTID,"10000001");
	//strcpy(strERMTransData.szMID,"100000000000001");
	vdDebug_LogPrintf("szTID=[%s],szMID=[%s]", strERMTransData.szTID,strERMTransData.szMID);

	strERMTransData.fReserved1 = 1;//Request Contact Info

	strcpy(strERMTransData.szPAN,srTransRec.szPAN);
	//testing
	//strcpy(strERMTransData.szPAN,"4119110002304324");
	
	inERMAP_Database_BatchInsert(&strERMTransData);
	
}




int inCTOSS_ERM_Send(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	//BYTE bySendTemp[2048];
	BYTE bySendTemp[8094];
	BYTE byTempLen[10];
	BYTE byTempHEXLen[10];
	int inHeader;
	
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	memset(bOutBuf, 0x00, sizeof(bOutBuf)); 

	//inMultiAP_Database_COM_Clear();
	inMultiAP_Database_ERM_ClearEx();

	inHeader = strCPT.inIPHeader;

	//inHeader = HEX_EXCLUDE_LEN;//for test
	vdDebug_LogPrintf("inCTOSS_ERM_Send[%ld]",ulSendLen);
	
	switch(inHeader)
	{
		case NO_HEADER_LEN:
			inSendLen = ulSendLen;
			vdDebug_LogPrintf("---***inCTOSS_COMM_Send[%ld]",ulSendLen);
			
			memcpy(bySendTemp, uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);
			break;
			
		case HEX_EXCLUDE_LEN:
			bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
			bySendTemp[1] = (ulSendLen & 0x000000FF);

			inSendLen = ulSendLen + 2;
			
			vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);

			break;

		case BCD_EXCLUDE_LEN:
			sprintf(byTempLen,"%04ld",ulSendLen);
			wub_str_2_hex(byTempLen,byTempHEXLen,4);
			memcpy(bySendTemp,byTempHEXLen,2);			 
			inSendLen = ulSendLen + 2;
			
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);
			break;

		case HEX_INCLUDE_LEN:
			ulSendLen += 2;

			bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
			bySendTemp[1] = (ulSendLen & 0x000000FF);

			inSendLen = ulSendLen + 2;
			vdDebug_LogPrintf("---HEX_INCLUDE_LEN[%ld]",ulSendLen);
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);   
			break;
			

		case BCD_INCLUDE_LEN:			 
			ulSendLen += 2;

			sprintf(byTempLen,"%04ld",ulSendLen);
			wub_str_2_hex(byTempLen,byTempHEXLen,4);
			memcpy(bySendTemp,byTempHEXLen,2);			 
			inSendLen = ulSendLen + 2;
			
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);
			
			break;

		default:
			
			bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
			bySendTemp[1] = (ulSendLen & 0x000000FF);

			inSendLen = ulSendLen + 2;
			
			vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
			memcpy(&bySendTemp[2], uszSendData, ulSendLen);
			memcpy(szERMSendData,bySendTemp,inSendLen);

			break;
	}
	
	//strCOM.inHeaderFormat = inHeader;
	
	//inMultiAP_Database_COM_Save();
	inMultiAP_Database_ERM_SaveEx();
	inCTOSS_ERM_SaveERMInfo();

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_FORWARD_TRANS_REQ, bInBuf, usInLen, bOutBuf, &usOutLen);
	
	vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",usResult);
	if(d_OK == usResult)
	{
		//status
		if(bOutBuf[0] == IPC_COMM_SUCC) 		   
			return d_OK;
		else
			return d_NO;
	}
	else
		return d_NO;
	
	//return usResult;
}



int inCTOSS_ERM_Receive(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	int inHeader;

	memset(bOutBuf, 0x00, sizeof(bOutBuf));    
	inHeader = strCPT.inIPHeader;

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_FORWARD_TRANS_RESP, bInBuf, usInLen, bOutBuf, &usOutLen);
	
	vdDebug_LogPrintf("---inCTOSS_ERM_Receive.usResult[%d]bOutBuf[0]=[%d]",usResult,bOutBuf[0]);
	if(d_OK == usResult)
	{
		//status
		if(bOutBuf[0] == IPC_COMM_SUCC) 
		{	   
			//inMultiAP_Database_COM_Read();
			inMultiAP_Database_ERM_ReadEx();
			vdDebug_LogPrintf("---strCOM.inReceiveLen[%d],.inIPHeader=[%d]",inReceiveLen,strCPT.inIPHeader);
			vdDebug_LogPrintf("rec data==[%d][%s]",inReceiveLen,szERMReceiveData);
			if(inHeader == NO_HEADER_LEN)
			{
				usResult = inReceiveLen;
				memcpy(uszRecData,szERMReceiveData,inReceiveLen);

				//puts("Server reply :");
				//puts(uszRecData); 
			}
			else
			{
				usResult = inReceiveLen-2;
				memcpy(uszRecData,&szERMReceiveData[2],inReceiveLen-2);
			}
			//after receive clear database
			//inMultiAP_Database_COM_Clear();
			inMultiAP_Database_ERM_ClearEx();
			return usResult;
		}
		else
			return 0;
	}
	
	return usResult;
}


int inCTOSS_ERM_CardinfoENQ(void)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	//BYTE bySendTemp[2048];
	BYTE bySendTemp[8094];
	BYTE byTempLen[10];
	BYTE byTempHEXLen[10];
	int inHeader;
	
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	memset(bOutBuf, 0x00, sizeof(bOutBuf)); 

	inCTOSS_ERM_SaveERMInfo();

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ERM", d_IPC_CMD_ERM_CARDINFO_ENQ, bInBuf, usInLen, bOutBuf, &usOutLen);
	
	vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",usResult);
	if(d_OK == usResult)
	{
		//status
		if(bOutBuf[0] == IPC_COMM_SUCC) 		   
			return d_OK;
		else
			return d_NO;
	}
	else
		return d_NO;
	
	//return usResult;
}



int inCTOSS_ERM_ReceiptRecvVia(void)
{
	int inRecvViaLen;
	BYTE szERMRecvViaData[1024];
	char ReqInfo;
	BYTE szPhoneNo[30];
	BYTE szEmail[100];
	int inResult=0;//thandar_added
    //int inRcptVia = get_env_int("RCPTVIA");	
	int inRet;

	memset(g_szPhoneNo, 0x00, sizeof(g_szPhoneNo));
	memset(g_szEmail, 0x00, sizeof(g_szEmail));

    memset(szPhoneNo, 0x00, sizeof(szPhoneNo));
	memset(szEmail, 0x00, sizeof(szEmail));


	vdDebug_LogPrintf("inCTOSS_ERM_ReceiptRecvVia AAAAA");

	//new enhancement for MP200 - 05232022
	//Change Request: #15 Change Request Add FLAG to Disable/Enable uploading Error e-receipts to iERS portal
	//inRet = get_env_int("#ERRRCPT");
	//vdDebug_LogPrintf("inCTOSS_ERM_ReceiptRecvVia inRet:[%d]",inRet);


	
	if(strTCT.byERMMode == 0)
		return d_OK;
	
//Fix for - http://118.201.48.214:8080/issues/15 #24
#if 0
	  if(inRet != 1)
		 return d_OK; 
#endif

	  vdDebug_LogPrintf("inCTOSS_ERM_ReceiptRecvVia BBBBB");
	
	  //thandar_start_added for MPOS to remove the below screen for contact & contactless transaction with the amount under 70000 MMK and 50 USD and upload the receipt to ERM.
    #if 1
	if (isCheckTerminalMP200() == d_OK) 
	{
       //if (srTransRec.byEntryMode==CARD_ENTRY_WAVE)
	   if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
			    inResult=inCTOSS_CheckFlagVEPSQPS();

	   
	   if(inResult != 0 && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND))
         return d_OK;
    }
	
	#endif 
	   //thandar_end_added
    if (strTCT.inRcptVia == 1)
    {
        inCTOSS_ERM_ChoicePhoneNo(szPhoneNo);
        return d_OK;
    }
    else if(strTCT.inRcptVia == 2)
    {
        inCTOSS_ERM_ChoiceEmail(szEmail);
        return d_OK;
    }
    else if (strTCT.inRcptVia == 3)
    {
        inCTOSS_ERM_ChoicePhoneNoAndEmail(szPhoneNo,szEmail);
        return d_OK;
    }
    #if 0
	inRecvViaLen = inCTOSS_ERM_GetFileSize(ERM_RECVVIS_FILE);
	vdDebug_LogPrintf("inCTOSS_ERM_ReceiptRecvVia inRecvViaLen=[%d] ", inRecvViaLen);
	if (inRecvViaLen <= 0)
		inCTOSS_ERM_CardinfoENQ();

	inRecvViaLen = inCTOSS_ERM_GetFileSize(ERM_RECVVIS_FILE);
	if (inRecvViaLen > 0)
	{
		memset(szERMRecvViaData,0x00,sizeof(szERMRecvViaData));
		memset(szPhoneNo,0x00,sizeof(szPhoneNo));
		memset(szEmail,0x00,sizeof(szEmail));
		inCTOSS_ERM_ReadFile(ERM_RECVVIS_FILE,szERMRecvViaData,inRecvViaLen);
		vdPCIDebug_HexPrintf("ERM_RECVVIS_FILE",szERMRecvViaData,inRecvViaLen);
		remove(ERM_RECVVIS_FILE);
		
		ReqInfo = szERMRecvViaData[0];
		vdDebug_LogPrintf("ReqInfo=[%c][%d]",ReqInfo,ReqInfo);

		memcpy(szPhoneNo,&szERMRecvViaData[1],15);
		vdDebug_LogPrintf("szPhoneNo=[%s]",szPhoneNo);
		if (strlen(szPhoneNo) > 0)
			strcpy(g_szPhoneNo,szPhoneNo);
		memcpy(szEmail,&szERMRecvViaData[16],50);
		vdDebug_LogPrintf("szEmail=[%s]",szEmail);
		if (strlen(szEmail) > 0)
			strcpy(g_szEmail,szEmail);

		if (ReqInfo == '0')
			return d_OK;
		else if (ReqInfo == '1')
		{
			inCTOSS_ERM_ChoicePhoneNo(szPhoneNo);
		}
		else if (ReqInfo == '2')
		{
			inCTOSS_ERM_ChoiceEmail(szEmail);
		}
		else if (ReqInfo == '3')
		{
			inCTOSS_ERM_ChoicePhoneNoAndEmail(szPhoneNo,szEmail);
		}

		CTOS_LCDTClearDisplay();
	}
    #endif
	
	vdDebug_LogPrintf("g_szPhoneNo=[%s]",g_szPhoneNo);
	vdDebug_LogPrintf("g_szEmail=[%s]",g_szEmail);
	return d_OK;
}

int inCTOSS_ERM_ChoicePhoneNo(char *szPhoneNo)
{
	unsigned char key;
	BYTE sztmpPhoneNo[30];
	int usLen;

	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("inCTOSS_ERM_ChoicePhoneNo=[%s]",sztmpPhoneNo);
	if (strlen(szPhoneNo) > 0)
	{
		CTOS_LCDTClearDisplay();
    	vdDispTransTitle(srTransRec.byTransType);
		CTOS_LCDTPrintAligned(3, "Receipt SMS to", d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(4, szPhoneNo, d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);

		while (1)
		{
			CTOS_KBDGet(&key);
	        if(key==d_KBD_ENTER)
	            return d_OK;
	        else if(key==d_KBD_CANCEL)
	        {
	        	inCTOSS_ERM_EnterPhoneNo();
	        	return d_OK;
	        }
		}
	}
	else
	{
    	inCTOSS_ERM_EnterPhoneNo();
    	return d_OK;
    }
}

int inCTOSS_ERM_ChoiceEmail(char *szEmail)
{
	unsigned char key;
	BYTE sztmpEmail[100];
	int usLen;
	
	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("inCTOSS_ERM_ChoiceEmail=[%s]",szEmail);
	if (strlen(szEmail) > 0)
	{
		CTOS_LCDTClearDisplay();
    	vdDispTransTitle(srTransRec.byTransType);
		CTOS_LCDTPrintAligned(3, "Email Receipt to", d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(4, szEmail, d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);

		while (1)
		{
			CTOS_KBDGet(&key);
	        if(key==d_KBD_ENTER)
	            return d_OK;
	        else if(key==d_KBD_CANCEL)
	        {
	        	inCTOSS_ERM_EnterEmail();
	        	return d_OK;
	        }
		}
	}
	else
	{
		inCTOSS_ERM_EnterEmail();
    	return d_OK;
    }
	
}

int inCTOSS_ERM_ChoicePhoneNoAndEmail(char *szPhoneNo,char *szEmail)
{
	unsigned char key;
	BYTE sztmpPhoneNo[30];
	BYTE sztmpEmail[100];
	int usLen;
	
	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("inCTOSS_ERM_ChoicePhoneNoAndEmail=[%s][%s]",szPhoneNo, szEmail);
	if (strlen(szPhoneNo) > 0)
	{
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);
		CTOS_LCDTPrintAligned(3, "Receipt SMS to", d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(4, szPhoneNo, d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);

		while (1)
		{
			CTOS_KBDGet(&key);
	        if(key==d_KBD_ENTER)
	            return d_OK;
	        else if(key==d_KBD_CANCEL)
	        {
				inCTOSS_ERM_EnterPhoneNoAndEmail();
	        	return d_OK;
	        }
		}
	}
	else if (strlen(szEmail) > 0)
	{
		
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);
		CTOS_LCDTPrintAligned(3, "Email Receipt to", d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(4, szEmail, d_LCD_ALIGNCENTER);
		CTOS_LCDTPrintAligned(5, "NO[X] YES[OK]", d_LCD_ALIGNCENTER);

		while (1)
		{
			CTOS_KBDGet(&key);
	        if(key==d_KBD_ENTER)
	            return d_OK;
	        else if(key==d_KBD_CANCEL)
	        {
				inCTOSS_ERM_EnterPhoneNoAndEmail();
	        	return d_OK;
	        }
		}
	}
	else
	{
		inCTOSS_ERM_EnterPhoneNoAndEmail();
	}

	return d_OK;
}


int inCTOSS_ERM_EnterPhoneNoAndEmail(void)
{
    unsigned char key;
    BYTE sztmpPhoneNo[30];
    BYTE sztmpEmail[100];
    int usLen;

    while (1)
    {
		CTOS_KBDBufFlush();
		CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);
		
		CTOS_LCDTPrintAligned(3, "Receipt receive via", d_LCD_ALIGNLEFT);
		CTOS_LCDTPrintAligned(4, "	   1) SMS", d_LCD_ALIGNLEFT);
		CTOS_LCDTPrintAligned(5, "	   2) Email", d_LCD_ALIGNLEFT);
		CTOS_LCDTPrintAligned(6, "	   X) Cancel", d_LCD_ALIGNLEFT);

        CTOS_KBDGet(&key);
        if(key==d_KBD_1)
        {
            if(inCTOSS_ERM_EnterPhoneNo() != d_OK)
                continue;	
			CTOS_LCDTClearDisplay();
            memset(g_szEmail, 0x00, sizeof(g_szEmail));
            return d_OK;
        }
        else if(key==d_KBD_2)
        {
            if(inCTOSS_ERM_EnterEmail() != d_OK)
                continue;		
			CTOS_LCDTClearDisplay();
            memset(g_szPhoneNo, 0x00, sizeof(g_szPhoneNo));
            return d_OK;
        }
		else if(key==d_KBD_CANCEL)
		{
			CTOS_LCDTClearDisplay();
			memset(g_szEmail, 0x00, sizeof(g_szEmail));
			memset(g_szPhoneNo, 0x00, sizeof(g_szPhoneNo));
			return d_OK;
		}
    }
}


int inCTOSS_ERM_EnterPhoneNo(void)
{
	unsigned char key;
	BYTE sztmpPhoneNo[30];
	USHORT usLen;

        //do
        while(1)
        {
            CTOS_KBDBufFlush();
            CTOS_LCDTClearDisplay();
            vdDispTransTitle(srTransRec.byTransType);
            vdDebug_LogPrintf("inCTOSS_ERM_EnterPhoneNo");
            CTOS_LCDTPrintAligned(3, "Enter Phone No.", d_LCD_ALIGNLEFT);

            usLen = 16;
            memset(sztmpPhoneNo,0x00,sizeof(sztmpPhoneNo));
            key = shCTOS_GetNum(4, 0x01,  sztmpPhoneNo, &usLen, 1, 15, 0, d_INPUT_TIMEOUT);
            if (key == d_KBD_CANCEL)
            {
				vduiClearBelow(4);
                vduiDisplayStringCenter(7,"Cancel Phone No.?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
				key=WaitKey(60);
                if(key == d_KBD_ENTER)
                {
					CTOS_LCDTClearDisplay();
					return d_NO;
					//break;
                }
				else
                    continue;
            }

		else
				{

				vdDebug_LogPrintf("sztmpPhoneNo=[%s]",sztmpPhoneNo);			
                vduiDisplayStringCenter(7,"Confirm?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
				//key=WaitKey(60);

				while (1)
					{
				  CTOS_KBDGet(&key);
         
               // if(key == d_KBD_ENTER)
				 if(key==d_KBD_ENTER)
                {
					 CTOS_LCDTClearDisplay();
					 memset(g_szPhoneNo,0x00,sizeof(g_szPhoneNo));
				     strcpy(g_szPhoneNo,sztmpPhoneNo);
				     return d_OK;
				
					
                }

				else if (key == d_KBD_CANCEL)
					{
					   if(inCTOSS_ERM_EnterPhoneNo()==d_OK)
					   	 return d_OK;
					   else
					   	return d_NO;

					}
			
					}
				
				}
					
        }
        //while (key == d_KBD_CANCEL);
	return d_OK;
}


int inCTOSS_ERM_EnterEmail(void)
{
	unsigned char key;
	BYTE sztmpEmail[100];
	USHORT usLen;
	
        //do
        while(1)
        {
            CTOS_KBDBufFlush();
            CTOS_LCDTClearDisplay();
            vdDispTransTitle(srTransRec.byTransType);
            vdDebug_LogPrintf("inCTOSS_ERM_ChoiceEmail...");
            CTOS_LCDTPrintAligned(3, "Enter Email Address", d_LCD_ALIGNLEFT);

            usLen = 50;
            memset(sztmpEmail,0x00,sizeof(sztmpEmail));
            key = InputStringAlphaEx2(1, 4, 0x00, 0x02, sztmpEmail, &usLen, 1, d_INPUT_TIMEOUT);
            if (key == d_KBD_CANCEL)
            {
				vduiClearBelow(4);
                vduiDisplayStringCenter(7,"Cancel Email Add?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
				key=WaitKey(60);
                if(key == d_KBD_ENTER)
                {
					CTOS_LCDTClearDisplay();
					return d_NO;
					//break;
                }
				else
                    continue;
            }
			else
			{
                vdDebug_LogPrintf("sztmpEmail=[%s]",sztmpEmail);

			
                vduiDisplayStringCenter(7,"Confirm?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
				//key=WaitKey(60);

				while (1)
					{
				  CTOS_KBDGet(&key);
         
               // if(key == d_KBD_ENTER)
				 if(key==d_KBD_ENTER)
                {
					 CTOS_LCDTClearDisplay();
					 memset(g_szEmail,0x00,sizeof(g_szEmail));
                     strcpy(g_szEmail,sztmpEmail);
				     return d_OK;
				
					
                }

				else if (key == d_KBD_CANCEL)
					{
					   if(inCTOSS_ERM_EnterEmail()==d_OK)
					   	 return d_OK;
					   else
					   	return d_NO;

					}
			
					}
               
			}
        }
        //while (key == d_KBD_CANCEL);
	return d_OK;
	
}

void vdCTOSS_DisplayStatus(int inRet)
{
	BYTE bySC_status;
	BYTE    szTotalAmt[12+1];
    BYTE    szTempBuf[12+1];
    BYTE    szTempBuf1[12+1]; 
    BYTE    szDisplayBuf[30];
    BYTE    szStr[45];
	int tmpflag = 0;
	char szErrMsg[30+1];

	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("vdCTOSS_DisplayStatus, szRespCode=[%s],byEntryMode=[%d]",srTransRec.szRespCode,srTransRec.byEntryMode);
	if(inRet == d_OK)
	{
		displaybmpEx(0, 0, "approved.bmp");
	}
	else if(inRet == FUNCTION_ABORT)
	{
		return;
	}
	else
	{
		displaybmpEx(0, 0, "declined.bmp");

		memset(szErrMsg,0x00,sizeof(szErrMsg));
	    if (inGetErrorMessage(szErrMsg) > 0)
	    {
	        vdCTOS_DispStatusMessage(szErrMsg);
	    }
	}


    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
    memset(szStr, 0x00, sizeof(szStr));	
	//format amount 10+2
	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
	CTOS_LCDTPrintAligned(1,"AMOUNT:",d_LCD_ALIGNLEFT);
	CTOS_LCDTPrintAligned(1,szStr,d_LCD_ALIGNRIGHT);

	if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
	{
		while(1)
	    { 
	        CTOS_SCStatus(d_SC_USER, &bySC_status);
	        if(bySC_status & d_MK_SC_PRESENT)
	        {
				if ((strTCT.byTerminalType%2) == 0)
					vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"PLS REMOVE CARD");
				else
		            vduiDisplayStringCenter(7,"PLS REMOVE CARD");
	            CTOS_Beep();
	            CTOS_Delay(300);
	            CTOS_Beep();

				tmpflag ++;
	            continue;
	        }

			if (tmpflag == 0)
			{
				if ((strTCT.byTerminalType%2) == 0)
					vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"Press Any Key");
				else
				vduiDisplayStringCenter(7,"Press Any Key");
				CTOS_KBDBufFlush();
				WaitKey(15);
			}
			
	        break;
	    }
	}
	else
	{
		if ((strTCT.byTerminalType%2) == 0)
			vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"Press Any Key");
		else
		vduiDisplayStringCenter(7,"Press Any Key");
		CTOS_KBDBufFlush();
		WaitKey(15);
	}
	
}

void vdCTOSS_DisplayStatusNoKey(int inRet)
{
	BYTE bySC_status;
	BYTE    szTotalAmt[12+1];
    BYTE    szTempBuf[12+1];
    BYTE    szTempBuf1[12+1]; 
    BYTE    szDisplayBuf[30];
    BYTE    szStr[45];
	int tmpflag = 0;
	char szErrMsg[30+1];

	CTOS_KBDBufFlush();
	vdDebug_LogPrintf("vdCTOSS_DisplayStatus, szRespCode=[%s],byEntryMode=[%d]",srTransRec.szRespCode,srTransRec.byEntryMode);
	if(inRet == d_OK)
	{
		displaybmpEx(0, 0, "approved.bmp");
	}
	else if(inRet == FUNCTION_ABORT)
	{
		return;
	}
	else
	{
		displaybmpEx(0, 0, "declined.bmp");

		memset(szErrMsg,0x00,sizeof(szErrMsg));
	    if (inGetErrorMessage(szErrMsg) > 0)
	    {
	        vdCTOS_DispStatusMessage(szErrMsg);
	    }
	}


    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
    memset(szStr, 0x00, sizeof(szStr));	
	//format amount 10+2
	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
	CTOS_LCDTPrintAligned(1,"AMOUNT:",d_LCD_ALIGNLEFT);
	CTOS_LCDTPrintAligned(1,szStr,d_LCD_ALIGNRIGHT);

	if(CARD_ENTRY_ICC == srTransRec.byEntryMode)
	{
		while(1)
	    { 
	        CTOS_SCStatus(d_SC_USER, &bySC_status);
	        if(bySC_status & d_MK_SC_PRESENT)
	        {
				if ((strTCT.byTerminalType%2) == 0)
					vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"PLS REMOVE CARD");
				else
		            vduiDisplayStringCenter(7,"PLS REMOVE CARD");
	            CTOS_Beep();
	            CTOS_Delay(300);
	            CTOS_Beep();

				tmpflag ++;
	            continue;
	        }

			if (tmpflag == 0)
			{
				if ((strTCT.byTerminalType%2) == 0)
					vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"Press Any Key");
				else
				vduiDisplayStringCenter(7,"Press Any Key");
				CTOS_KBDBufFlush();
				WaitKey(15);
			}
			
	        break;
	    }
	}
	else
	{
		#if 0
		if ((strTCT.byTerminalType%2) == 0)
			vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"Press Any Key");
		else
		vduiDisplayStringCenter(7,"Press Any Key");
		CTOS_KBDBufFlush();
		WaitKey(15);
		#endif
	}
	
}


#ifdef OK_DOLLAR_FEATURE

void vdHostToInitialize(BOOL fPrompt, BOOL fSuccess)
{
	char szErrMsg[30+1];
	
	memset(szErrMsg,0x00,sizeof(szErrMsg));
    sprintf(szErrMsg,"INIT %s",strHDT.szHostLabel);
	vdCTOS_Pad_String(szErrMsg, 23, IS_SPACE, POSITION_RIGHT);
    setLCDPrint27(7,DISPLAY_POSITION_LEFT,szErrMsg);

	if (fPrompt)
	{
		if (fSuccess)
		{
			setLCDPrint27(8,DISPLAY_POSITION_LEFT, "SUCCESS");
			CTOS_Beep();
		}			
		else
		{
			setLCDPrint27(8,DISPLAY_POSITION_LEFT, "FAILED");
			CTOS_Beep();
			CTOS_Delay(300);
			CTOS_Beep();
		}			
	}
}

#endif

