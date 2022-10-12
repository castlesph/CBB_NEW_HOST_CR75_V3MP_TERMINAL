#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <vwdleapi.h>
#include <sqlite3.h>
#include <time.h>

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
//#include "..\PCI100\COMMS.h"

#include "..\POWRFAIL\POSPOWRFAIL.h"
#include <ctos_qrcode.h>
#include <curl\curl.h>

#include "..\Includes\CTOSInput.h"




#define QR_LOGIN 	1
#define QR_GENERATE 2
#define QR_TRANS_STATUS 3
#define QR_TRANS_DETAILS 4
#define QR_VOID 5
#define QR_TEST_FUNC 6
#define QR_CANCEL 7
#define QR_TEST_CURL 8


#define QR_FAILED	-1  //General purpose return value for failed QR transactions
#define QR_PENDING 	-2  //Transaction is still pending
#define QR_APPRVD 	-3  //Used for QR0701


typedef struct MemoryStruct {
	char *memory;
	char *function;
	size_t size;
}MemoryStruct;
extern BOOL fECRTxnFlg;

BOOL fFailedCurlStatus = FALSE;
BOOL ErmTrans_Approved;

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


#ifdef OK_DOLLAR_FEATURE


int GetOKDPerformResutlMessage(int iResult)
{
	BOOL fRetryFlag = FALSE;
	
	vdDebug_LogPrintf("--GetDPerformResutlMessage--");
	vdDebug_LogPrintf("iResult[%d]", iResult);

	switch (iResult)
	{
		case 200:
			vdDisplayErrorMsgResp2("APPROVED","","OK$ TRANSACTION");
			break;	
		case 101:
			vdDisplayErrorMsgResp2("PAYMENT HAS BEEN","PROCESS. PLEASE CHECK","WITH CC TEAM");
			break;
		case 201:
			vdDisplayErrorMsgResp2("ALREADY REFUNDED","PLS. CHECK WITH","CONCERN TEAMS.");
			break;
		case 202:
			vdDisplayErrorMsgResp2("INVALID INFO.","PLS. RECHECK","GIVEN INPUT");
			break;
		case 400:
			vdDisplayErrorMsgResp2("BAD REQUEST","MANDATORY FIELDS","MISSING...");
			break;
		case 401:
			vdDisplayErrorMsgResp2("INVLD REQUEST","SERVICE ARE DOWN","");
			break;
		case 402:
			vdDisplayErrorMsgResp2("RECORDS","NOT FOUND","FOR GIVEN REQUEST");
			break;
		case 500:
			vdDisplayErrorMsgResp2("INTERNAL ERROR","PLEASE CONTACT","ADMINISTRATOR");
			break;
		case 600:
			vdDisplayErrorMsgResp2("CANCELED","BY"," USER");
			break;
		case 601:
			vdDisplayErrorMsgResp2("SCANNED","BUT NOT PROCESS","PAYMENT");
			break;
		case 404:
			vdDisplayErrorMsgResp2("NOT FOUND","CURRENCY CODE OR","COUNTRY CODE");
			break;
		case 300:
			vdDisplayErrorMsgResp2("INVALID REQUEST","AUTHENTICATION","FAILED");
			break;
		case 301:
			vdDisplayErrorMsgResp2("INVALID","MERCHANT","KEY");
			break;
		case 302:
			vdDisplayErrorMsgResp2("INVALID","CREDIT","ACCOUNT NO.");
			break;
		case 303:
			vdDisplayErrorMsgResp2("","","INVALID AMOUNT");
			break;
		case 304:
			vdDisplayErrorMsgResp2("REFERENCE","NUMBER","ALREADY EXIST");
			break;
		case 311:
			vdDisplayErrorMsgResp2("MERCHANT","BACKEND NO.","NOT CONFIGURED");
			break;
		case 312:
			vdDisplayErrorMsgResp2("","PAYMENT LOGIN","FAILURE");
			break;
		case 313:
			vdDisplayErrorMsgResp2("","PAYMENT LOGIN","PARSING ERROR");
			break;
		case 314:
			vdDisplayErrorMsgResp2("","PAYMENT RESPONSE","IS FAILURE");
			break;
		case 315:
			vdDisplayErrorMsgResp2("PAYMENT","RESPONSE PARSING","ERROR");
			break;
		case 316:
			vdDisplayErrorMsgResp2("","PAYMENT NOT","SUCESSFUL");
			break;
		case 317:
			vdDisplayErrorMsgResp2("PAYMENT NOT","SUCCESSFUL","AMT NOT SUFFICIENT.");
			break;			
		default:
			vdDisplayErrorMsgResp2("UNDEFINED","ERROR","STATUS");
			break;
			
	}

	vdSetErrorMessage("");
		
	return d_NO;
}

unsigned int PadData (unsigned char *ibuf, unsigned int ilen, int blksize)
{
  unsigned int   i;         // loop counter
  unsigned char  pad;           // pad character (calculated)
  unsigned char *p;         // pointer to end of data



 if(ilen % blksize == 0)
   return ilen; 


  // calculate pad character
  pad = (unsigned char) (blksize - (ilen % blksize));


  // append pad to end of string
  p = ibuf + ilen;
  for (i = 0; i < (int) pad; i++) {
    *p = pad;
    ++p;
  }

  return (ilen + pad);
}


char *base64_encode(const char *data, size_t input_length, size_t *output_length) {
    
        int i = 0, j = 0;
    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = (char *)malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}


int inExtractField2(unsigned char *uszRecData, char *szField, char *szSearchString){
	char *ptr;
 	char szWorkBuff1[4096+1];
 	char szWorkBuff2[4096+1];
 	char szSearchToken[2];
 	int i;


	vdDebug_LogPrintf("inExtractField2");

	vdDebug_LogPrintf("extract field:[%s] szSearchString:[%s]", uszRecData, szSearchString);

 	memset(szSearchToken, 0x00, sizeof(szSearchToken));
 	szSearchToken[0] = '"';
 	memset(szWorkBuff1, 0x00, sizeof(szWorkBuff1));
 	ptr = NULL;
 	strcpy(szWorkBuff1,uszRecData);
 	ptr =strstr(szWorkBuff1, szSearchString);

	//vdDebug_LogPrintf("ptr :[%s]", ptr);
	//vdDebug_LogPrintf("szWorkBuff1.A :[%s]", szWorkBuff1);

    if (ptr == NULL)
	{
		vdDebug_LogPrintf("inExtractField2  NULL");
		return FAIL;
   	}
	
 	memset(szWorkBuff2, 0x00, sizeof(szWorkBuff2));
 	strcpy(szWorkBuff2, ptr);
	vdDebug_LogPrintf("szWorkBuff2 :[%s]", szWorkBuff2);
	

    for (i = 1; i<4; i++){


		//vdDebug_LogPrintf("inExtractField2 i [%d] ptr [%s] szWorkBuff2 [%s]", i, ptr, szWorkBuff2);
		
  		memset(szWorkBuff1, 0x00, sizeof(szWorkBuff1));
  		ptr = NULL;
  		strcpy(szWorkBuff1, szWorkBuff2);
		
  
  		ptr =strstr(szWorkBuff1, szSearchToken);
  
 		memset(szWorkBuff2, 0x00, sizeof(szWorkBuff2));
  		//strcpy(szWorkBuff2, ptr+1);
  		strcpy(szWorkBuff2, ptr);
		
  
    }
	//vdDebug_LogPrintf("szWorkBuff1.B %s", szWorkBuff1);

 	//memcpy(szField, szWorkBuff1, ptr - szWorkBuff1);
 	memcpy(szField, &szWorkBuff1[2], 3);
 

 
 	//vdDebug_LogPrintf("szSearchString [%s] szField [%s] ", szSearchString, szField);

	return d_OK;
}
size_t write_OKDpay_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	vdDebug_LogPrintf("write_OKDpay_callback");

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);

	vdDebug_LogPrintf("write_OKDpay_callback=[%d][%s]", realsize, contents);
	DebugAddHEX("CONTENTS",contents,realsize);
	
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}


size_t read_OKDpay_callback(void *dest, size_t size, size_t nmemb, void *userp)
{
	struct MemoryStruct *wt = (struct MemoryStruct *)userp;
	size_t buffer_size = size*nmemb;

	vdDebug_LogPrintf("read_OKDpay_callback");

	if (wt->size) {
		/* copy as much as possible from the source to the destination */
		size_t copy_this_much = wt->size;
		if (copy_this_much > buffer_size)
			copy_this_much = buffer_size;
		memcpy(dest, wt->memory, copy_this_much);

		wt->memory += copy_this_much;
		wt->size -= copy_this_much;
		
		vdDebug_LogPrintf("read_OKDpay_callback=[%d][%s]", wt->size, dest);
		return copy_this_much; /* we copied this many bytes */
	}

	return 0; /* no more data left to deliver */
}


int inCTOS_CBB_OKDollar(void)
{
	int inRet = d_NO;
	inSetColorMenuMode();
		
	CTOS_LCDTClearDisplay();
	vdCTOS_SetTransType(OK_DOLLAR_TRANS);
	vdDispTransTitle(srTransRec.byTransType);
	vdCTOS_DispStatusMessage("PROCESSING...");  

  	vdCTOS_TxnsBeginInit();


	srTransRec.HDTid = OK_DOLLAR_HOST_INDEX;
	strCDT.HDTid = OK_DOLLAR_HOST_INDEX;
	srTransRec.IITid = 11;

	inIITRead(srTransRec.IITid);
	inCPTRead(srTransRec.HDTid);
	strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);

#if 1
// for testing, to check unsent/failed erm files.
	inRet = inCTOSS_ERM_CheckSlipImage();
	if(d_OK != inRet)
        return inRet;
#endif	
	
	inRet = inCTOS_OKDollarFlowProcess();

	//fix for issue Should display "E-RECEIPT SENT SUCCESSFULLY" or other related message - 10002
	if(inRet != d_OK){		
		inCTOS_inDisconnect();		
		vdCTOS_TransEndReset();
		return inRet;
	}

	
    inCTOS_inDisconnect();
    
    if(strTCT.fUploadReceiptIdle != TRUE)
        inCTOSS_UploadReceipt();
	
	vdCTOS_TransEndReset();
	vdDebug_LogPrintf("AFTER TRANSENDRESET");
	
	return inRet;
}


int inCTOS_OKDollarFlowProcess(void)
{
	int inRet = d_NO;
	int inRetVal = d_NO;
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	int inEVENT_ID = 0;
	BOOL fBinRoute=TRUE;
	int inSMCardRetryCount=0;
	char szTemp1[d_LINE_SIZE + 1];

	vdDebug_LogPrintf("START inCTOS_OKDollarFlowProcess");

	vdCTOS_SetTransType(OK_DOLLAR_TRANS);
	
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

	CTOS_LCDTClearDisplay();	
	vdCTOS_DispStatusMessage("PROCESSING...");  

	
    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;
		

	vdDebug_LogPrintf("TESTQR SALE [%d] [%d]", strCDT.HDTid,srTransRec.MITid);

	inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
	{
	    strcpy(srTransRec.szRespCode,"");
        return inRet;
    }


	vduiClearBelow(1);


    inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
			return inRet;
	

    inRet = inGenerateOKDQRCode();

	vdDebug_LogPrintf("inCTOS_OKDollarFlowProcess inGenerateOKDQRCode inRet[%d]", inRet);
    if(d_OK != inRet)
        return inRet;


	inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    vdDebug_LogPrintf("inCTOS_UpdateAccumTotal");
	
    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
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

		
    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

	CTOS_LCDTClearDisplay();
			
    vdSetErrorMessage("");

	vdDebug_LogPrintf("END inCTOS_OKDollarFlowProcess");
	
    return d_OK;
}




int inGenerateOKDQRCode(void)
{
	int inRet, res;
  	MemoryStruct write_data, read_data;
	char szTempBuff[1000];
	char szEMVString[500];
	BYTE key;
	char	szQRCode[4096+1] = {0};
	int inStatusCode;
    BYTE byKeyBuf;

	
	
	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");

	vdDebug_LogPrintf("inGenerateOKDQRCode START");

	//check CURL init
#if 1
	res = inOKDPayGetTransactionStatus3();

	vdDebug_LogPrintf("inGenerateOKDQRCode inCBOKDCheckCurl_Send_request res [%d] fFailedCurlStatus [%d]", res, fFailedCurlStatus);

	
	if(res != 0){
		
		CTOS_LCDTClearDisplay();
		vdDebug_LogPrintf("inGenerateOKDQRCode inCTOSS_DisplayOKDQRCodeAndConfirm FAILED CURL COMMAND");
		vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));

		while(1)
		{
			setLCDPrint27(5,DISPLAY_POSITION_CENTER,"PLS CHECK TERMINAL");
			setLCDPrint27(6,DISPLAY_POSITION_CENTER,"CONNECTION");
			setLCDPrint27(7,DISPLAY_POSITION_CENTER,"AND TRY AGAIN");
		
			
			CTOS_Beep();
			CTOS_Delay(1000);
		
			CTOS_KBDHit(&byKeyBuf);
			if (byKeyBuf == d_KBD_CANCEL || byKeyBuf == d_KBD_ENTER){				
				break;
			}
		}						
		
		return res;
	}else
		vdDebug_LogPrintf("inGenerateOKDQRCode inCTOSS_DisplayOKDQRCodeAndConfirm SUCCESS CURL COMMAND");
	
#endif
	
	
	memset(szQRCode, 0x00, sizeof(szQRCode));
	//vdOKDCreatePOSTData(&szQRCode, QR_GENERATE);	
	//CONSTRUCT QRCODE
	vdOKDCreatePOSTData2(&szQRCode, QR_GENERATE);	
	
	CTOS_LCDTClearDisplay();

	//512 max qrcode lenght
	//strcpy(szQRCode, "00020101021250440028The Pizza Company Restaurant0108ThePizza51670028The Pizza Company Restaurant013158120000000012500000000125000015204581253031045406100.005802MM5925The Pizza Company Restaur6015Mingalar Taung 61051122162660113ref11201908250313ref12201908250516200430000000048707080100000164350002my0125The Pizza Company Restaur6304C4EC	00020101021250440028The Pizza Company Restaurant0108ThePizza51670028The Pizza Company Restaurant013158120000000012500000000125000015204581253031045406100.005802MM5925The Pizza");

	
	inRet = inCTOSS_DisplayOKDQRCodeAndConfirm(szQRCode);

	vdDebug_LogPrintf("inGenerateOKDQRCode inCTOSS_DisplayOKDQRCodeAndConfirm inRet[%d]", inRet);

	if(inRet != 0)
		vdDebug_LogPrintf("inGenerateOKDQRCode inCTOSS_DisplayOKDQRCodeAndConfirm FAILED TRANSACTION RESULT");
		

	return inRet;

}


int inOKDPayGetTransactionStatus3(void)
{
	int inRet;
	int res, i;
	BYTE key;
	char szTempBuff[1000];
	char szEMVString[500];
	char szBuffer[100+1];
	char szOkdollarTrnsId[100+1];
	char szMerRefNumber[100+1];
	
	char szAmtBuff[12+1];
    char szErrMsg[50+1];
	int inCode;
	int inLenszOkdollarTrnsId;

	MemoryStruct write_data, read_data;


	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");

	//inOKDPayRead(1);
	
	put_env_int("INSTATUSCODE",0); //initla value to zero		
	vdDebug_LogPrintf("***inOKDPayGetTransactionStatus3***");

		 

	//vdDebug_LogPrintf("strCBOKD.inQP_CurlRetries[%d]",strCBOKD.inQP_CurlRetries);

	//for (i = 1; i<= strCBOKD.inQP_CurlRetries;i++){
	for (i = 1; i<= 3;i++){
		write_data.memory = malloc(1); 
		write_data.size = 0;

		res = inCBOKDSend_request3(&read_data,&write_data, QR_TEST_CURL);
		
		memset(szTempBuff, 0x00, sizeof(szTempBuff));
		memcpy(szTempBuff, write_data.memory, write_data.size);
		
		vdDebug_LogPrintf("inOKDPayGetTransactionStatus3:Status --- [%s] res [%d]", szTempBuff, res);

		if(res != CURLE_OK){ 
			
			vdDebug_LogPrintf("inOKDPayGetTransactionStatus3 NOT CURLE_OK transaction Return.");
			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
			
		}
		else if (res == CURLE_OK)
		{		
			vdDebug_LogPrintf("inOKDPayGetTransactionStatus3 WITH OK transaction Return.");	
			inExtractField2(szTempBuff, szBuffer, "Status");		
			vdDebug_LogPrintf("Status --- [%s]  ---  [%d]", szBuffer, i);
			break;

			// commented for testing, not required to check status field for OK$ application.
		}
		
		if (res == CURLE_OPERATION_TIMEDOUT){

			vdDebug_LogPrintf("+++++++++++++++++++++++++++++++++++++", szBuffer);			
			vdDebug_LogPrintf("            STATUS TIMEOUT !!!", szBuffer);			
			vdDebug_LogPrintf("+++++++++++++++++++++++++++++++++++++", szBuffer);
			 continue;
		}
		else
			break;

	}

	vdDebug_LogPrintf("inOKDPayGetTransactionStatus3..HERE!!!");			

	//memset(szBuffer, 0x00, sizeof(szBuffer));
 	//inExtractField2(szTempBuff, szBuffer, "Code");

	//vdDebug_LogPrintf("Code  ---  %s", szBuffer);

	//For testing
	//With force Approved response on STATUS
	//strcpy(szBuffer, "200");
	//strcpy(szBuffer, "402");
	#if 0
	if (strcmp(szBuffer, "200") == 0)
	{
		
			vdDebug_LogPrintf("SUCCESS!!!");
			strcpy(srTransRec.szRespCode, "00");


			// OkdollarTrnsId - for Void/ Refund reference
			memset(szOkdollarTrnsId, 0x00, sizeof(szOkdollarTrnsId));
			inExtractField(szTempBuff, szOkdollarTrnsId, "OkdollarTrnsId");

			vdDebug_LogPrintf("OkdollarTrnsId  ---  [%s] Len  --- [%d]", szOkdollarTrnsId, strlen(szOkdollarTrnsId));

			inLenszOkdollarTrnsId = strlen(szOkdollarTrnsId);

			strcpy(srTransRec.szOKDBankTransId, szOkdollarTrnsId);		
			srTransRec.szOKDBankTransId[inLenszOkdollarTrnsId-1] = '\0';  
			//srTransRec.szOKDBankTransId[10] = '\0';  
			
			vdDebug_LogPrintf("srTransRec.szOKDBankTransId  ---  %s", srTransRec.szOKDBankTransId);

			memset(szMerRefNumber,0x00,sizeof(szMerRefNumber));
			inExtractField(szTempBuff, szMerRefNumber, "MerRefNumber");
			vdDebug_LogPrintf("MerRefNumber  ---  %s", szMerRefNumber);

			//inCode = atoi(szBuffer);
			//put_env_int("INSTATUSCODE",inCode);
			//GetOKDPerformResutlMessage(inCode);
			
			vdDisplayErrorMsgResp2("","APPROVED","");

			inCBPaySaveDateTime();	

			inRet = d_OK;
			
	}
	else
	{
			
			vdDebug_LogPrintf("NOT APPROVED!!!!");

			inCode = atoi(szBuffer);

			// if ok$ user cancelled the transaction and host return 600 "Canceled By User..". #3s
			if(inCode == 600)
			{								
				put_env_int("INSTATUSCODE600",1);
			}
			
			//put_env_int("INSTATUSCODE",inCode);
			//GetOKDPerformResutlMessage(inCode);
						
				
			vdDebug_LogPrintf("free memory2");
							
			free(write_data.memory);
				
			//CTOS_LCDTClearDisplay();

			return d_NO;		
  	}
	#endif
		
	vdDebug_LogPrintf("free memory4");

	free(write_data.memory);

	//CTOS_LCDTClearDisplay();

	//vdDebug_LogPrintf("*** inOKDPayGetTransactionStatus3 *** res [%d]", res);

    return res;

}



int inOKDPayGetTransactionStatus2(void)
{
	int inRet;
	int res, i;
	BYTE key;
	char szTempBuff[1000];
	char szEMVString[500];
	char szBuffer[100+1];
	char szOkdollarTrnsId[100+1];
	char szMerRefNumber[100+1];
	
	char szAmtBuff[12+1];
    char szErrMsg[50+1];
	int inCode;
	int inLenszOkdollarTrnsId;

	MemoryStruct write_data, read_data;


	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");

	inOKDPayRead(1);
	
	put_env_int("INSTATUSCODE",0); //initla value to zero		
	vdDebug_LogPrintf("***inOKDPayGetTransactionStatus2***");

		 

	vdDebug_LogPrintf("strCBOKD.inQP_CurlRetries[%d]",strCBOKD.inQP_CurlRetries);

	for (i = 1; i<= strCBOKD.inQP_CurlRetries;i++){
		write_data.memory = malloc(1); 
		write_data.size = 0;

		res = inCBOKDSend_request2(&read_data,&write_data, QR_TRANS_STATUS);
		
		memset(szTempBuff, 0x00, sizeof(szTempBuff));
		memcpy(szTempBuff, write_data.memory, write_data.size);
		
		vdDebug_LogPrintf("inOKDPayGetTransactionStatus:Status --- [%s] res [%d]", szTempBuff, res);

		if(res != CURLE_OK){ 
			
			vdDebug_LogPrintf("inOKDPayGetTransactionStatus NOT CURLE_OK transaction Return.");
			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
			
		}
		else if (res == CURLE_OK)
		{		
			vdDebug_LogPrintf("inOKDPayGetTransactionStatus WITH OK transaction Return.");	
			inExtractField2(szTempBuff, szBuffer, "Status");		
			vdDebug_LogPrintf("Status --- [%s]  ---  [%d]", szBuffer, i);
			break;

			// commented for testing, not required to check status field for OK$ application.
		}
		
		if (res == CURLE_OPERATION_TIMEDOUT){

			vdDebug_LogPrintf("+++++++++++++++++++++++++++++++++++++", szBuffer);			
			vdDebug_LogPrintf("            STATUS TIMEOUT !!!", szBuffer);			
			vdDebug_LogPrintf("+++++++++++++++++++++++++++++++++++++", szBuffer);
			 continue;
		}
		else
			break;

	}

	vdDebug_LogPrintf("inOKDPayGetTransactionStatus2..HERE!!!");			

	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField2(szTempBuff, szBuffer, "Code");

	vdDebug_LogPrintf("Code  ---  %s", szBuffer);

	//For testing
	//With force Approved response on STATUS
	//strcpy(szBuffer, "200");
	//strcpy(szBuffer, "402");

	if (strcmp(szBuffer, "200") == 0)
	{
		
			vdDebug_LogPrintf("SUCCESS!!!");
			strcpy(srTransRec.szRespCode, "00");


			// OkdollarTrnsId - for Void/ Refund reference
			memset(szOkdollarTrnsId, 0x00, sizeof(szOkdollarTrnsId));
			inExtractField(szTempBuff, szOkdollarTrnsId, "OkdollarTrnsId");

			vdDebug_LogPrintf("OkdollarTrnsId  ---  [%s] Len  --- [%d]", szOkdollarTrnsId, strlen(szOkdollarTrnsId));

			inLenszOkdollarTrnsId = strlen(szOkdollarTrnsId);

			strcpy(srTransRec.szOKDBankTransId, szOkdollarTrnsId);		
			srTransRec.szOKDBankTransId[inLenszOkdollarTrnsId-1] = '\0';  
			//srTransRec.szOKDBankTransId[10] = '\0';  
			
			vdDebug_LogPrintf("srTransRec.szOKDBankTransId  ---  %s", srTransRec.szOKDBankTransId);

			memset(szMerRefNumber,0x00,sizeof(szMerRefNumber));
			inExtractField(szTempBuff, szMerRefNumber, "MerRefNumber");
			vdDebug_LogPrintf("MerRefNumber  ---  %s", szMerRefNumber);

			//inCode = atoi(szBuffer);
			//put_env_int("INSTATUSCODE",inCode);
			//GetOKDPerformResutlMessage(inCode);
			
			vdDisplayErrorMsgResp2("","APPROVED","");

			inCBPaySaveDateTime();	

			inRet = d_OK;
			
	}
	else
	{
			
			vdDebug_LogPrintf("NOT APPROVED!!!!");

			inCode = atoi(szBuffer);

			// if ok$ user cancelled the transaction and host return 600 "Canceled By User..". #3s
			if(inCode == 600)
			{								
				put_env_int("INSTATUSCODE600",1);
			}
			
			//put_env_int("INSTATUSCODE",inCode);
			//GetOKDPerformResutlMessage(inCode);
						
				
			vdDebug_LogPrintf("free memory2");
							
			free(write_data.memory);
				
			//CTOS_LCDTClearDisplay();

			return d_NO;		
  	}

		
	vdDebug_LogPrintf("free memory3");

	free(write_data.memory);

	//CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("*** inOKDPayGetTransactionStatus *** inRet [%d]", inRet);

    return inRet;

}

int inOKDPayGetTransactionStatus(void)
{
	int inRet;
	int res, i;
	BYTE key;
	char szTempBuff[1000];
	char szEMVString[500];
	char szBuffer[100+1];
	char szOkdollarTrnsId[100+1];
	char szMerRefNumber[100+1];
	
	char szAmtBuff[12+1];
    char szErrMsg[50+1];
	int inCode;
	int inLenszOkdollarTrnsId;

	MemoryStruct write_data, read_data;


	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");
	//inCurl_CommsInit();

	inOKDPayRead(1);
	//4; fix for overlapped "SCAN QRCODE" production issue for cbpay - raised by Khine of UTS 05062020 #5
	CTOS_LCDTClearDisplay();	
	//vduiClearBelow(8);
	
	put_env_int("INSTATUSCODE",0); //initla value to zero		

	//vdCTOSS_QRCodeDisplayOnly(strGrabPay.szQRCode);


	vdCTOS_DispStatusMessage("PROCESSING...");  

	vdDebug_LogPrintf("***inOKDPayGetTransactionStatus***");

		 

	//inGrabPayRead(1);

	//vdSend_request(&read_data,&write_data, QR_TRANS_STATUS);
    //memset(szTempBuff, 0x00, sizeof(szTempBuff));
	//memcpy(szTempBuff, write_data.memory, write_data.size);



	//vdDebug_LogPrintf("inQP_CurlRetries[%d]",strCBPay.inQP_CurlRetries);
	vdDebug_LogPrintf("strCBOKD.inQP_CurlRetries[%d]",strCBOKD.inQP_CurlRetries);

	//for (i = 1; i<= strCBPay.inQP_CurlRetries;i++){
	for (i = 1; i<= strCBOKD.inQP_CurlRetries;i++){
		write_data.memory = malloc(1); 
		write_data.size = 0;

	
		res = inCBOKDSend_request(&read_data,&write_data, QR_TRANS_STATUS);
		
		memset(szTempBuff, 0x00, sizeof(szTempBuff));
		memcpy(szTempBuff, write_data.memory, write_data.size);
		
		vdDebug_LogPrintf("inOKDPayGetTransactionStatus:Status --- [%s] res [%d]", szTempBuff, res);

		if(res != CURLE_OK){ 
			
			vdDebug_LogPrintf("inOKDPayGetTransactionStatus NOT CURLE_OK transaction Return.");
			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
			
		}
		else if (res == CURLE_OK)
		{

		
			vdDebug_LogPrintf("inOKDPayGetTransactionStatus WITH OK transaction Return.");

				#if 0 //comment out the printing of logs for the meantime
				
				if (fXMLPrint)
					{
						char szResponseBuff[1000] = {0};

						strcpy(szResponseBuff, szTempBuff);
						
						inPrintSodexoPacket("RX-GRAB", szResponseBuff, strlen(szResponseBuff), FALSE);
					}
				#endif
		
			//inExtractField2(szTempBuff, szBuffer, "Data");
			inExtractField2(szTempBuff, szBuffer, "Status");		
			vdDebug_LogPrintf("Status --- [%s]  ---  [%d]", szBuffer, i);

			//inRet = d_OK;

			break;

			// commented for testing, not required to check status field for OK$ application.
			#if 0
			szBuffer[1] = '\0';
				
			if (strcmp(szBuffer, "P") == 0)//transaction not found
				{
					vdDebug_LogPrintf("TRANSACTION PENDING");
					free(write_data.memory);
					return QR_PENDING;
				}
			else if (strcmp(szBuffer, "E") == 0)
				{
					vdDisplayErrorMsgResp2("","QR","EXPIRED");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					return QR_FAILED;
				}
			else if (strcmp(szBuffer, "C") == 0)
				{
					vdDisplayErrorMsgResp2("","CANCELLED"," ");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					return QR_FAILED;
				}
			else if (strcmp(szBuffer, "L") == 0)
				{
					vdDisplayErrorMsgResp2("","OVER","LIMIT");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					return QR_FAILED;
				}
			else if (strcmp(szBuffer, "S") != 0) //other transaction status aside from 'S'
				{
					vdDisplayErrorMsgResp2("","TRANSACTION","UNSUCCESSFUL");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					return QR_FAILED;
				}
			else{
				vdDisplayErrorMsgResp2("","STATUS","NULL");
				vdDebug_LogPrintf("free memory");
				free(write_data.memory);
				return QR_FAILED;
					
				}
			#endif
		}
		
		if (res == CURLE_OPERATION_TIMEDOUT){

			vdDebug_LogPrintf("+++++++++++++++++++++++++++++++++++++", szBuffer);			
			vdDebug_LogPrintf("            STATUS TIMEOUT !!!", szBuffer);			
			vdDebug_LogPrintf("+++++++++++++++++++++++++++++++++++++", szBuffer);
			 continue;
		}
		else
			break;

	}

	vdDebug_LogPrintf("inOKDPayGetTransactionStatus..HERE!!!");			

	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField2(szTempBuff, szBuffer, "Code");

	vdDebug_LogPrintf("Code  ---  %s", szBuffer);

	//For testing
	//With force Approved response on STATUS
	//strcpy(szBuffer, "200");

	if (strcmp(szBuffer, "200") == 0)
	{
		
			vdDebug_LogPrintf("SUCCESS!!!");
			strcpy(srTransRec.szRespCode, "00");


			// OkdollarTrnsId - for Void/ Refund reference
			memset(szOkdollarTrnsId, 0x00, sizeof(szOkdollarTrnsId));
			inExtractField(szTempBuff, szOkdollarTrnsId, "OkdollarTrnsId");

			vdDebug_LogPrintf("OkdollarTrnsId  ---  [%s] Len  --- [%d]", szOkdollarTrnsId, strlen(szOkdollarTrnsId));

			inLenszOkdollarTrnsId = strlen(szOkdollarTrnsId);

			strcpy(srTransRec.szOKDBankTransId, szOkdollarTrnsId);		
			srTransRec.szOKDBankTransId[inLenszOkdollarTrnsId-1] = '\0';  
			//srTransRec.szOKDBankTransId[10] = '\0';  
			
			vdDebug_LogPrintf("srTransRec.szOKDBankTransId  ---  %s", srTransRec.szOKDBankTransId);

			memset(szMerRefNumber,0x00,sizeof(szMerRefNumber));
			inExtractField(szTempBuff, szMerRefNumber, "MerRefNumber");
			vdDebug_LogPrintf("MerRefNumber  ---  %s", szMerRefNumber);

			//inCode = atoi(szBuffer);
			//put_env_int("INSTATUSCODE",inCode);
			//GetOKDPerformResutlMessage(inCode);
			
			vdDisplayErrorMsgResp2("","APPROVED","");

			inCBPaySaveDateTime();	

			inRet = d_OK;
			
	}
	else
	{

			
			vdDebug_LogPrintf("NOT APPROVED!!!!");

			#if 0
			//To display Error message on screen
			inExtractField(szTempBuff, szBuffer, "Msg");
			vdDebug_LogPrintf("Msg  ---  %s", szBuffer);

		
			if (strlen(szBuffer) > 0)
				vdSetErrorMessage(szBuffer);
			else		
				vdSetErrorMessage("COMM ERROR");
			#else
			
			inCode = atoi(szBuffer);
			put_env_int("INSTATUSCODE",inCode);
			
			GetOKDPerformResutlMessage(inCode);
			
			#endif
				
			vdDebug_LogPrintf("free memory2");
							
			free(write_data.memory);
				
			CTOS_LCDTClearDisplay();

			return d_NO;		
  	}

		
#if 0	
	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField(szTempBuff, szBuffer, "updated");

	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField(szTempBuff, szBuffer, "currency");
#endif
	vdDebug_LogPrintf("free memory3");

	free(write_data.memory);

	//CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("*** inOKDPayGetTransactionStatus *** inRet [%d]", inRet);

    return inRet;

}


int inCBOKDSend_request(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType) 
{

	CURL *curl;
 	CURLcode res;
	int inResult;
	int i, inDay,inRet = 0;
	char szBuffer[1000];
	char szRFCdate[40+1];
	char szConent_len[30+1];
	char szAuthorization[100+1]={0};
	BYTE pbtBuff[4096+1]; 
	USHORT pusLen;


	char postthis[2000] = {0};
	char szURL[150] = {0};
	char szTempURL[150] = {0};
	char szAuthenToken[350] = {0};

	vdDebug_LogPrintf("inCBOKDSend_request TransType [%d]", TransType);
	vdCTOS_DispStatusMessage("PROCESSING...");

	inOKDPayRead(1);	
	//inCBPayRead(1);


	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	
	/* Check for errors */
	if (res != CURLE_OK) {		
		vdDebug_LogPrintf("curl_global_init failed");	
		return d_OK;
	}
	vdDebug_LogPrintf("curl_global_init successful");
	
	curl = curl_easy_init();
	
	if(curl) {
		
		vdDebug_LogPrintf("curl_easy_init successful");

		// NO API for QR Code generation
		/*if (TransType == QR_GENERATE)
		{
			memset(szURL, 0x00, sizeof(szURL));
			strcpy(szURL, strCBOKD.szTranStatusURL);
			vdDebug_LogPrintf("GENERATE QR URL is %s",szURL);
		}		
		else*/
		if (TransType == QR_TRANS_STATUS)
		{

			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://www.okdollar.co/RestService.svc/GetStatusByRefNumber");			
			strcpy(szURL, strCBOKD.szTranStatusURL);
			//strcpy(szURL, strCBPay.szTranStatusURL);
			vdDebug_LogPrintf("STATUS URL is %s",szURL);
			
		}
        else if (TransType == QR_VOID)
		{    
			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://www.okdollar.co/RestService.svc/GetStatusByRefNumber");			
			sprintf(szURL, "http://advertisement.api.okdollar.org/AdService.svc/RefundQrPayByRefNumber");
			//strcpy(szURL, strCBOKD.szVoidURL);
			vdDebug_LogPrintf("VOID URL is %s",szURL);
			
        }
		else if (TransType == QR_CANCEL)
		{
			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://122.248.120.187:4443/payment-api/v1/qr/cancel-transaction.service"); //Hardcoded values for now.
			//strcpy(szURL, strCBPay.szCancelURL);
			strcpy(szURL, strCBOKD.szCancelURL);
			vdDebug_LogPrintf("CANCEL URL is %s",szURL);
		}

		/*inogre SSL verify*/
		//if (strGrabPay.SSLEnable == 1){
			//vdDebug_LogPrintf("SSL ENABLE URL %s", szURL);
			curl_easy_setopt(curl, CURLOPT_URL,szURL);

			#if 1
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			#endif
		//}else
		//	curl_easy_setopt(curl, CURLOPT_URL,szURL);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

		

		vdDebug_LogPrintf("inCBOKDSend_request-1");

		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_OKDpay_callback);		

		/* pointer to pass to our read function */
		curl_easy_setopt(curl, CURLOPT_READDATA, (void *)contents);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_OKDpay_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

		/* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
		itself */ 

		vdDebug_LogPrintf("inCBOKDSend_request-2");

		struct curl_slist *headerlist = NULL;

		#if 0
        if (TransType == QR_GENERATE)
		{	
	       	memset(postthis, 0x00, sizeof(postthis));
        	vdOKDCreatePOSTData2(&postthis, TransType);			

        }
	    else if(TransType == QR_TRANS_STATUS || TransType == QR_VOID  || TransType == QR_CANCEL)
		{
			memset(postthis, 0x00, sizeof(postthis));
    		vdOKDCreatePOSTData2(&postthis, TransType);
		}
		#else
		if(TransType == QR_TRANS_STATUS || TransType == QR_VOID  || TransType == QR_CANCEL)
		{
			memset(postthis, 0x00, sizeof(postthis));
    		vdOKDCreatePOSTData2(&postthis, TransType);

		}		
		#endif

		vdDebug_LogPrintf("inCBOKDSend_request-3");

					
		
//		if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID){
			vdDebug_LogPrintf("QR GENERATE HEADER AND BODY");
			//headerlist = curl_slist_append(headerlist, "Authen-Token: eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE1NjY5ODIyNjQsIm1lcklkIjoiTTYwMTEwMDAwMDAwMDAxNiJ9.hWDqDWfxtK1tE8Iz-a-R7RiN2hYJZzkQMtn66rpf_NY");


//no need to append authen token for OK$
#if 0
			sprintf(szAuthenToken, "Authen-Token: %s", strCBPay.szAuthenToken);
			headerlist = curl_slist_append(headerlist, szAuthenToken);
#endif
			headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

			vdDebug_LogPrintf("curl_slist_append data1=[%s]",headerlist->data);
			//vdDebug_LogPrintf("curl_slist_append data2=[%s]",headerlist->next->data);			
			//vdDebug_LogPrintf("curl_slist_append data3=[%s]",headerlist->next->next->data);

 //       }		
			vdDebug_LogPrintf("inCBOKDSend_request-4");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
//        if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID)
//		{
			vdDebug_LogPrintf("DATA TO SEND %s",postthis);
			vdDebug_LogPrintf("POSTFIELDSIZE %li",(long)strlen(postthis));
			
			vdDebug_LogPrintf("inQP_CurlTimeout %d",strCBOKD.inQP_CurlTimeout);

			
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));			
  //      }

		 
        //for (i = 1; i<= strBDOPAY.inCurlRetries;i++){
	        
  		//curl_easy_setopt(curl, CURLOPT_TIMEOUT, strCBOKD.inQP_CurlTimeout);
  		
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);

		vdDebug_LogPrintf("curl_easy_perform %d retries=%d", res, i);


		/* FUNCTION CALL FOR printing logs.....
		if( fXMLPrint && 
			((res == CURLE_OPERATION_TIMEDOUT) || (res == CURLE_OK)))
			{

				char szReqBuff[2000] = {0};

				strcpy(szReqBuff, postthis);
			
				//inPrintSodexoPacket("TX-GRAB BODY", szReqBuff, strlen(szReqBuff), TRUE);
			}

		*/

		
		/* Check for errors */ 
		if(res != CURLE_OK) 
		{
		
			vdDebug_LogPrintf("CURL COMMAND ERROR WAS DETECTED !!!"); 		

			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
			GetDPerformResutlMessage(res);	

			vdDebug_LogPrintf("curl_easy_cleanup !!!");			
			curl_easy_cleanup(curl);// always cleanup		

			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist);// cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");
			curl_easy_reset(curl);	// reset all handles

			vdDebug_LogPrintf("Encountered error!!!");

			
			return(d_NO);
		}
		else
		{
			vdCTOS_DispStatusMessage("RECEIVING...");
			/* always cleanup */ 
			curl_easy_cleanup(curl);			
			
			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist); // cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");			
			curl_easy_reset(curl);// reset all handle

		}
		

		

		/* always cleanup */ 
		//vdDebug_LogPrintf("curl_easy_cleanup !!!");
		//curl_easy_cleanup(curl);
	}
    return res;
}

int inCBOKDSend_request2(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType) 
{

	CURL *curl;
 	CURLcode res;
	int inResult;
	int i, inDay,inRet = 0;
	char szBuffer[1000];
	char szRFCdate[40+1];
	char szConent_len[30+1];
	char szAuthorization[100+1]={0};
	BYTE pbtBuff[4096+1]; 
	USHORT pusLen;


	char postthis[2000] = {0};
	char szURL[150] = {0};
	char szTempURL[150] = {0};
	char szAuthenToken[350] = {0};

	vdDebug_LogPrintf("inCBOKDSend_request TransType [%d]", TransType);
	//vdCTOS_DispStatusMessage("PROCESSING...");

	inOKDPayRead(1);	
	//inCBPayRead(1);


	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	
	/* Check for errors */
	if (res != CURLE_OK) {		
		vdDebug_LogPrintf("curl_global_init failed");	
		return d_OK;
	}
	vdDebug_LogPrintf("curl_global_init successful");
	
	curl = curl_easy_init();
	
	if(curl) {
		
		vdDebug_LogPrintf("curl_easy_init successful");

		// NO API for QR Code generation
		/*if (TransType == QR_GENERATE)
		{
			memset(szURL, 0x00, sizeof(szURL));
			strcpy(szURL, strCBOKD.szTranStatusURL);
			vdDebug_LogPrintf("GENERATE QR URL is %s",szURL);
		}		
		else*/
		if (TransType == QR_TRANS_STATUS || TransType == QR_TEST_CURL)
		{

			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://www.okdollar.co/RestService.svc/GetStatusByRefNumber");			
			strcpy(szURL, strCBOKD.szTranStatusURL);
			//strcpy(szURL, strCBPay.szTranStatusURL);
			vdDebug_LogPrintf("STATUS URL is %s",szURL);
			
		}
        else if (TransType == QR_VOID)
		{    
			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://www.okdollar.co/RestService.svc/GetStatusByRefNumber");			
			sprintf(szURL, "http://advertisement.api.okdollar.org/AdService.svc/RefundQrPayByRefNumber");
			//strcpy(szURL, strCBOKD.szVoidURL);
			vdDebug_LogPrintf("VOID URL is %s",szURL);
			
        }
		else if (TransType == QR_CANCEL)
		{
			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://122.248.120.187:4443/payment-api/v1/qr/cancel-transaction.service"); //Hardcoded values for now.
			//strcpy(szURL, strCBPay.szCancelURL);
			strcpy(szURL, strCBOKD.szCancelURL);
			vdDebug_LogPrintf("CANCEL URL is %s",szURL);
		}

		/*inogre SSL verify*/
		//if (strGrabPay.SSLEnable == 1){
			//vdDebug_LogPrintf("SSL ENABLE URL %s", szURL);
			curl_easy_setopt(curl, CURLOPT_URL,szURL);

			#if 1
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			#endif
		//}else
		//	curl_easy_setopt(curl, CURLOPT_URL,szURL);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

		

		vdDebug_LogPrintf("inCBOKDSend_request-1");

		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_OKDpay_callback);		

		/* pointer to pass to our read function */
		curl_easy_setopt(curl, CURLOPT_READDATA, (void *)contents);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_OKDpay_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

		/* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
		itself */ 

		vdDebug_LogPrintf("inCBOKDSend_request-2");

		struct curl_slist *headerlist = NULL;

		#if 0
        if (TransType == QR_GENERATE)
		{	
	       	memset(postthis, 0x00, sizeof(postthis));
        	vdOKDCreatePOSTData2(&postthis, TransType);			

        }
	    else if(TransType == QR_TRANS_STATUS || TransType == QR_VOID  || TransType == QR_CANCEL)
		{
			memset(postthis, 0x00, sizeof(postthis));
    		vdOKDCreatePOSTData2(&postthis, TransType);
		}
		#else
		if(TransType == QR_TRANS_STATUS || TransType == QR_VOID  || TransType == QR_CANCEL)// || TransType == QR_TEST_CURL)
		{
			memset(postthis, 0x00, sizeof(postthis));
    		vdOKDCreatePOSTData2(&postthis, TransType);

		}		
		else if(TransType == QR_TEST_CURL)
		{			
			memset(postthis, 0x00, sizeof(postthis));
    		vdOKDCreatePOSTData3(&postthis, TransType);
		}
		#endif

		vdDebug_LogPrintf("inCBOKDSend_request-3");

					
		
//		if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID){
			vdDebug_LogPrintf("QR GENERATE HEADER AND BODY");
			//headerlist = curl_slist_append(headerlist, "Authen-Token: eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE1NjY5ODIyNjQsIm1lcklkIjoiTTYwMTEwMDAwMDAwMDAxNiJ9.hWDqDWfxtK1tE8Iz-a-R7RiN2hYJZzkQMtn66rpf_NY");


//no need to append authen token for OK$
#if 0
			sprintf(szAuthenToken, "Authen-Token: %s", strCBPay.szAuthenToken);
			headerlist = curl_slist_append(headerlist, szAuthenToken);
#endif
			headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

			vdDebug_LogPrintf("curl_slist_append data1=[%s]",headerlist->data);
			//vdDebug_LogPrintf("curl_slist_append data2=[%s]",headerlist->next->data);			
			//vdDebug_LogPrintf("curl_slist_append data3=[%s]",headerlist->next->next->data);

 //       }		
			vdDebug_LogPrintf("inCBOKDSend_request-4");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
//        if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID)
//		{
			vdDebug_LogPrintf("DATA TO SEND %s",postthis);
			vdDebug_LogPrintf("POSTFIELDSIZE %li",(long)strlen(postthis));
			
			vdDebug_LogPrintf("inQP_CurlTimeout %d",strCBOKD.inQP_CurlTimeout);

			
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));			
  //      }

		 
        //for (i = 1; i<= strBDOPAY.inCurlRetries;i++){
	        
  		//curl_easy_setopt(curl, CURLOPT_TIMEOUT, strCBOKD.inQP_CurlTimeout);
  		
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);

		vdDebug_LogPrintf("curl_easy_perform %d retries=%d", res, i);


		/* FUNCTION CALL FOR printing logs.....
		if( fXMLPrint && 
			((res == CURLE_OPERATION_TIMEDOUT) || (res == CURLE_OK)))
			{

				char szReqBuff[2000] = {0};

				strcpy(szReqBuff, postthis);
			
				//inPrintSodexoPacket("TX-GRAB BODY", szReqBuff, strlen(szReqBuff), TRUE);
			}

		*/

		//res = CURLE_COULDNT_RESOLVE_HOST;

		
		/* Check for errors */ 
		if(res != CURLE_OK) 
		{
		
			vdDebug_LogPrintf("CURL COMMAND ERROR WAS DETECTED !!!"); 	
			fFailedCurlStatus = TRUE; // prompt to check terminal communication status

			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
			GetDPerformResutlMessage(res);	

			vdDebug_LogPrintf("curl_easy_cleanup !!!");			
			curl_easy_cleanup(curl);// always cleanup		

			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist);// cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");
			curl_easy_reset(curl);	// reset all handles

			vdDebug_LogPrintf("Encountered error!!!");

			
			return(d_NO);
		}
		else
		{
			//vdCTOS_DispStatusMessage("RECEIVING...");
			
			/* always cleanup */ 
			curl_easy_cleanup(curl);			
			
			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist); // cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");			
			curl_easy_reset(curl);// reset all handle

		}
		

		

		/* always cleanup */ 
		//vdDebug_LogPrintf("curl_easy_cleanup !!!");
		//curl_easy_cleanup(curl);
	}
    return res;
}



//for CURL command check connection
int inCBOKDSend_request3(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType) 
{

	CURL *curl;
 	CURLcode res;
	int inResult;
	int i, inDay,inRet = 0;
	char szBuffer[1000];
	char szRFCdate[40+1];
	char szConent_len[30+1];
	char szAuthorization[100+1]={0};
	BYTE pbtBuff[4096+1]; 
	USHORT pusLen;


	char postthis[2000] = {0};
	char szURL[150] = {0};
	char szTempURL[150] = {0};
	char szAuthenToken[350] = {0};

	vdDebug_LogPrintf("inCBOKDSend_request3 TransType [%d]", TransType);
	//vdCTOS_DispStatusMessage("PROCESSING...");

	inOKDPayRead(1);	
	//inCBPayRead(1);


	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	
	/* Check for errors */
	if (res != CURLE_OK) {		
		vdDebug_LogPrintf("curl_global_init failed");	
		return d_OK;
	}
	vdDebug_LogPrintf("curl_global_init successful");
	
	curl = curl_easy_init();
	
	if(curl) {
		
		vdDebug_LogPrintf("curl_easy_init successful");

		memset(szURL, 0x00, sizeof(szURL));
		//sprintf(szURL, "https://www.okdollar.co/RestService.svc/GetStatusByRefNumber");				
		strcpy(szURL, strCBOKD.szTranStatusURL);
		vdDebug_LogPrintf("STATUS URL is %s",szURL);
			


		/*inogre SSL verify*/
		//if (strGrabPay.SSLEnable == 1){
			//vdDebug_LogPrintf("SSL ENABLE URL %s", szURL);
			curl_easy_setopt(curl, CURLOPT_URL,szURL);

			#if 1
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			#endif
		//}else
		//	curl_easy_setopt(curl, CURLOPT_URL,szURL);

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

		

		vdDebug_LogPrintf("inCBOKDSend_request-1");

		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_OKDpay_callback);		

		/* pointer to pass to our read function */
		curl_easy_setopt(curl, CURLOPT_READDATA, (void *)contents);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_OKDpay_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

		/* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
		itself */ 

		vdDebug_LogPrintf("inCBOKDSend_request-2");

		struct curl_slist *headerlist = NULL;


		//create dummy data	
		memset(postthis, 0x00, sizeof(postthis));
    	vdOKDCreatePOSTData3(&postthis, TransType);

		vdDebug_LogPrintf("inCBOKDSend_request-3");

					
		
//		if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID){
			vdDebug_LogPrintf("QR GENERATE HEADER AND BODY");
			//headerlist = curl_slist_append(headerlist, "Authen-Token: eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE1NjY5ODIyNjQsIm1lcklkIjoiTTYwMTEwMDAwMDAwMDAxNiJ9.hWDqDWfxtK1tE8Iz-a-R7RiN2hYJZzkQMtn66rpf_NY");


//no need to append authen token for OK$
#if 0
			sprintf(szAuthenToken, "Authen-Token: %s", strCBPay.szAuthenToken);
			headerlist = curl_slist_append(headerlist, szAuthenToken);
#endif
			headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

			vdDebug_LogPrintf("curl_slist_append data1=[%s]",headerlist->data);
			//vdDebug_LogPrintf("curl_slist_append data2=[%s]",headerlist->next->data);			
			//vdDebug_LogPrintf("curl_slist_append data3=[%s]",headerlist->next->next->data);

 //       }		
			vdDebug_LogPrintf("inCBOKDSend_request-4");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
//        if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID)
//		{
			vdDebug_LogPrintf("DATA TO SEND %s",postthis);
			vdDebug_LogPrintf("POSTFIELDSIZE %li",(long)strlen(postthis));
			
			//vdDebug_LogPrintf("inQP_CurlTimeout %d",strCBOKD.inQP_CurlTimeout);

			
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));			
  //      }

		 
        //for (i = 1; i<= strBDOPAY.inCurlRetries;i++){
	        
  		//curl_easy_setopt(curl, CURLOPT_TIMEOUT, strCBOKD.inQP_CurlTimeout);
  		
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);

		vdDebug_LogPrintf("curl_easy_perform %d retries=%d", res, i);


		/* FUNCTION CALL FOR printing logs.....
		if( fXMLPrint && 
			((res == CURLE_OPERATION_TIMEDOUT) || (res == CURLE_OK)))
			{

				char szReqBuff[2000] = {0};

				strcpy(szReqBuff, postthis);
			
				//inPrintSodexoPacket("TX-GRAB BODY", szReqBuff, strlen(szReqBuff), TRUE);
			}

		*/

		//res = CURLE_COULDNT_RESOLVE_HOST;

		
		/* Check for errors */ 
		if(res != CURLE_OK) 
		{
		
			vdDebug_LogPrintf("CURL COMMAND ERROR WAS DETECTED !!!"); 	
			fFailedCurlStatus = TRUE; // prompt to check terminal communication status

			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
			GetDPerformResutlMessage(res);	

			vdDebug_LogPrintf("curl_easy_cleanup !!!");			
			curl_easy_cleanup(curl);// always cleanup		

			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist);// cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");
			curl_easy_reset(curl);	// reset all handles

			vdDebug_LogPrintf("Encountered error!!!");

			
			return(d_NO);
		}
		else
		{
			//vdCTOS_DispStatusMessage("RECEIVING...");
			
			/* always cleanup */ 
			curl_easy_cleanup(curl);			
			
			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist); // cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");			
			curl_easy_reset(curl);// reset all handle

		}
		

		

		/* always cleanup */ 
		//vdDebug_LogPrintf("curl_easy_cleanup !!!");
		//curl_easy_cleanup(curl);
	}
    return res;
}



//void vdOKDCreatePOSTData(char *uszCreditPaymentEncrytedRequestDataHex, int inType){
//void vdOKDCreatePOSTData(char *szBuffer, int inType){	
void vdOKDCreatePOSTData2(unsigned char *send_data, int inType){
	BYTE szAmtBuff[20+1];
	BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
	BYTE szReqID [32+1] = {0};
	char szMerID [16+1] = {0};
	char szSubMerID [16+1] = {0};
	char szCB_TerminalID[16+1] = {0};
	char szTransAmount [13+1] = {0};
	char szCurrency [3+1] = {0};
	char szRef1 [25+1] = {0};
	char szRef2 [25+1] = {0};

	BYTE szRandomKey[32+1] = {0};

	//For Transaction
	char szGateWayType [20+1] = {0};
	char szMerchantName [20+1] = {0};
	char szPOSID [100+1] = {0};
	char szQrCreatedTime [20+1] = {0};
	char szRefNumber [16+1] = {0};//[32+1] = {0};
	char szRemarks [200+1] = {0};
	char szServiceFeeByProvider [12+1] = {0};
	char szSpid [20+1] = {0};

	//For Void
	char szCustomerNum [50+1] = {0};
	char szPassword [50+1] = {0};
	char szPaymentTransId [100+1] = {0};
	char szVoidRefNumber [32+1] = {0};
	

    long inCurrTime=0L, inCurrDate=0L;
    CTOS_RTC SetRTC;
    BYTE szCurrTime[9] = {0};
    BYTE szCurrDate[19+1] = {0};
    int inResult = 0;
	unsigned char szBuffer[1000+1];

	//unsigned char uszAES256Key[32+1] = {0};
	unsigned char uszAES256Key[16+1] = {0};
	int inAES256KeySet = CBB_AES_KEYSET;//0xC000; 
	int inAES256KeyIndex = CBB_AES_KEYIDX;//0x0008;

	unsigned char uszCreditPaymentRequestData[4096+1];
	unsigned char uszCreditPaymentEncrytedRequestData[4096+1];
	unsigned char uszCreditPaymentEncrytedRequestDataHex[4096+1];	
	int inCreditPaymentEncrytedRequestData = 0;
	
	int inRetVal = 0;
	int inBuffer = 0;
	//unsigned char send_data[102400 + 1] = { 0 };
	BYTE szIvectorSale [50+1] = {0};
	BYTE szIvectorVoid [50+1] = {0};
	
	unsigned char uszIVData[4096] = {0};
	char MerchantId [50+1] = {0};

   	CTOS_SHA256_CTX ctx;

	unsigned char uszIULPMACKey[32+1] = {0};
	unsigned char uszDigest[32+1] = {0};
	unsigned char uszSIIDTemp[16+1] = {0};
	//int inRetVal = 0;
    unsigned char uszMACSignature[1024+1] = {0};
	int inMACSignature = 0;	


	//inCBPayRead(1);
	inOKDPayRead(1);

	vdDebug_LogPrintf("***vdOKDCreatePOSTData2 HDTid[%d]", srTransRec.HDTid);

	if (inType == QR_GENERATE)
	{
		memset(szCurrTime, 0x00, sizeof(szCurrTime));
		memset(szCurrDate, 0x00, sizeof(szCurrDate));
		CTOS_RTCGet(&SetRTC);
		sprintf(szCurrTime," %02d:%02d:%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
		vdDebug_LogPrintf("***vdOKDCreatePOSTData2 szCurrTime[%s]", szCurrTime);
		
		//inCurrTime=wub_str_2_long(szCurrTime);
		sprintf(szCurrDate,"%04d-%02d-%02d", SetRTC.bYear+2000, SetRTC.bMonth, SetRTC.bDay);
		vdDebug_LogPrintf("***vdOKDCreatePOSTData2 szCurrDate[%s]", szCurrDate);
		
		//inCurrDate=wub_str_2_long(szCurrDate);
		//vdDebug_LogPrintf("Current Time: %ld nCurrent Date: %ld", inCurrTime, inCurrDate);
		
		
		//hard coded KMS for testing
#if 0
		// Here inject Master key into KMS based on key number
		// write AES 128 key		
		memset(uszIULPMACKey, 0x00, sizeof(uszIULPMACKey));
		strcpy((char *)uszIULPMACKey, (char *)"4ab9129abd5e2127"); 
		vdDebug_LogPrintf("uszKeyData [%s]", uszIULPMACKey);
		
		vdWriteAES128KEY(inAES256KeySet, inAES256KeyIndex, uszIULPMACKey);		
		vdCTOSS_GetKEYInfo(inAES256KeySet,inAES256KeyIndex);		
		
#endif
	
		    memset(szGateWayType, 0x00, sizeof(szGateWayType));			
		    memset(szAmtBuff, 0x00, sizeof(szAmtBuff));			
		    memset(szMerchantName, 0x00, sizeof(szMerchantName));
		    memset(szPOSID, 0x00, sizeof(szPOSID));
		    memset(szQrCreatedTime, 0x00, sizeof(szQrCreatedTime));
		    memset(szRefNumber, 0x00, sizeof(szRefNumber));
		    memset(szRemarks, 0x00, sizeof(szRemarks));
		    memset(szServiceFeeByProvider, 0x00, sizeof(szServiceFeeByProvider));
		    memset(szSpid, 0x00, sizeof(szSpid));
		    memset(szBuffer, 0x00, sizeof(szBuffer));
		    memset(szIvectorSale, 0x00, sizeof(szIvectorSale));				


			wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
			//vdCTOSS_FormatAmount("NNNNNNNNNNNn.nn", szBaseAmt,szAmtBuff);
			vdCTOSS_FormatAmount("NNNNNNNNNNNn.nn", szBaseAmt,szAmtBuff);
			vdDebug_LogPrintf("vdOKDCreatePOSTData2 szAmtBuff %s", szAmtBuff);

			strcat(szCurrDate, szCurrTime);
			vdDebug_LogPrintf("vdOKDCreatePOSTData2 szCurrDate %s", szCurrDate);


#if 1
			//Amount
			strcpy(szTransAmount, szAmtBuff);
			//Gateway
			get_env("OKDGateWayType",szGateWayType,sizeof(szGateWayType));
			//Merchant Name
			get_env("OKDMerchname",szMerchantName,sizeof(szMerchantName));
			//POS Id
			get_env("OKDPosID",szPOSID,sizeof(szPOSID));		
			//Current date and time
			strcpy(szQrCreatedTime, szCurrDate);	
			//Random key
			vdGenerateRandomKeyCBPay(8, szRefNumber);
			szRefNumber[16] = '\0';  
			strcpy(srTransRec.szOKDRefNo, szRefNumber);			
			//Remark
			get_env("OKDRemark",szRemarks,sizeof(szRemarks));
			//Service Fee from Provider
			get_env("OKDServFeeProv",szServiceFeeByProvider,sizeof(szServiceFeeByProvider));
			//SPId
			get_env("OKDSpID",szSpid,sizeof(szSpid));
			//Ivector
			get_env("IvectorSale",szIvectorSale,sizeof(szIvectorSale));
			
#else
			//Hard Coded Value for Testing		
			strcpy(szTransAmount, "10.0");
			strcpy(szGateWayType, "POS");
			strcpy(szMerchantName, "CB Merchant");
			strcpy(szPOSID, "CB0027");
			strcpy(szQrCreatedTime, "2021-02-05 09:39:18");			
			strcpy(srTransRec.szOKDRefNo, "D81696D7EC293DE56D8B5FF39AE1C0C4");
			strcpy(szRemarks, "");
			strcpy(szServiceFeeByProvider, "0.0");
			strcpy(szSpid, "7d343419");
			
#endif


			
#if 1
			//sprintf(szBuffer, (unsigned char *)"{\n\t\"Amount\":\"%s\",\n\t\"GatewayType\":\"%s\",\n\t\"MerchantName\":\"%s\",\n\t\"PosID\":\"%s\",\n\t\"QrCreatedTime\":\"%s\",\n\t\"RefNumber\":\"%s\",\n\t\"Remarks\":\"%s\",\n\t\"ServiceFeeByProvider\":\"%s\",\n\t\"Spid\":\"%s\",\n\t\"TotalAmount\":\"%s\"\n}", 
			sprintf(szBuffer, (unsigned char *)"{\n\t\"Amount\":\%s\,\n\t\"GatewayType\":\"%s\",\n\t\"MerchantName\":\"%s\",\n\t\"PosId\":\"%s\",\n\t\"QrCreatedTime\":\"%s\",\n\t\"RefNumber\":\"%s\",\n\t\"Remarks\":\"%s\",\n\t\"ServiceFeeByProvider\":\%s\,\n\t\"Spid\":\"%s\",\n\t\"TotalAmount\":\"%s\"\n}", 
			szTransAmount, szGateWayType, szMerchantName, szPOSID, szQrCreatedTime, srTransRec.szOKDRefNo, szRemarks, szServiceFeeByProvider, szSpid, szTransAmount);

			

			// store initial vektor			
			//memset(uszIVData, 0x00, sizeof(uszIVData)); 
			//strcpy((char *)uszIVData, (char *)"1234567890123456"); 
			vdDebug_LogPrintf("szIvectorSale [%s]", szIvectorSale);			
			inBuffer =  strlen((char *)szBuffer);

			
			// Encrypt plaintext with AES|CBC|PAD5|BASE64
			memset(uszCreditPaymentEncrytedRequestData, 0x00, sizeof(uszCreditPaymentEncrytedRequestData));
			inRetVal = inEncryptData_with_AES128_CBC(inAES256KeySet, inAES256KeyIndex, szBuffer, inBuffer, szIvectorSale, 16, uszCreditPaymentEncrytedRequestData, &inBuffer);

			vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
			vdDebug_LogPrintf("vdOKDCreatePOSTData inEncryptData_with_AES128_CBC inRetVal [%d] inCreditPaymentEncrytedRequestData Lenght [%d] uszCreditPaymentEncrytedRequestData [%s]", 
				inRetVal, inBuffer, uszCreditPaymentEncrytedRequestData);
			vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

			// Format to JSON ENCRYPTED WITH AES
		    memset(MerchantId, 0x00, sizeof(MerchantId));				
			strcpy(MerchantId, strCBOKD.szSubMerId);
			MerchantId[50] = '\0';  

			//hard coded data from Nyan document
			//memset(uszCreditPaymentEncrytedRequestDataHex, 0x00, sizeof(uszCreditPaymentEncrytedRequestDataHex));
			//strcpy(uszCreditPaymentEncrytedRequestDataHex,	"UAXhMo/6DxbEt/TCogkDya4Fgz5+RabqPof6C9blWSA6e+O0RzHNKUQ2yEB64AfpN83wLghZwce3BuMPZvjqbleaGmtxy54gSWoXBuI14ejY3HGF30ynHkHeHKbSXa75O0wK3nx+4RNDxlWuiB7m2dFhzAf+ILxMaCjE3GnerWvct+i3ew5F43S1+ErbKflLO8Q1+Ha6J7yQ+de1fB9XTb4efjn+q9SQ45XW4nrVNO6BH0ZcJnlpRPrqUzEpLfbaO7jNgrK/7aOcyVL1plX+bYS+rDvUfxEz/zbbyQ4NBVE=");
			//vdDebug_LogPrintf("vdOKDCreatePOSTData2 uszCreditPaymentEncrytedRequestData Lenght [%d]", strlen(uszCreditPaymentEncrytedRequestData));	

			// HEx format
			sprintf(send_data, "{\"EncryptedString\":\"%s\",\"Ivector\":\"%s\",\"MerchantId\":\"%s\"}", 
			uszCreditPaymentEncrytedRequestData, szIvectorSale, MerchantId);


// without AES encryption
#else
			sprintf(send_data, (unsigned char *)"{\t\"Amount\":\%s\,\n\t\"GatewayType\":\"%s\",\n\t\"MerchantName\":\"%s\",\n\t\"PosId\":\"%s\",\n\t\"QrCreatedTime\":\"%s\",\n\t\"RefNumber\":\"%s\",\n\t\"Remarks\":\"%s\",\n\t\"ServiceFeeByProvider\":\%s\,\n\t\"Spid\":\"%s\",\n\t\"TotalAmount\":\%s}", 
			szTransAmount, szGateWayType, szMerchantName, szPOSID, szQrCreatedTime, srTransRec.szOKDRefNo, szRemarks, szServiceFeeByProvider, szSpid, szTransAmount);
			
#endif
			
			vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
			vdDebug_LogPrintf("                           vdOKDCreatePOSTData2 GENERATED QR CODE [SEND DATA %s] Lenght [%d]", send_data, strlen(send_data));	
			vdDebug_LogPrintf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

	}
	else if(inType == QR_TRANS_STATUS)
	{
		vdDebug_LogPrintf("******************************************************************");
		vdDebug_LogPrintf("******************vdOKDCreatePOSTData2 STATUS*********************");
		vdDebug_LogPrintf("******************************************************************");

		    memset(MerchantId, 0x00, sizeof(MerchantId));	
			
			//strcpy(MerchantId, "de83fe562127");
			//strcpy(szRefNumber, "CB47552");			
			//vdGenerateRandomKeyCBPay(32, szRefNumber);
			//szRefNumber[32] = '\0';  
			
			//strcpy(MerchantId, srTransRec.szMID);			
			strcpy(MerchantId, strCBOKD.szSubMerId);
			MerchantId[50] = '\0';  

			
			//rename field (as per OK$ host) MerchantId to MerchatId to fix 0500 issue.
			sprintf((char *)send_data, (char *)"{\n\t\"MerchatId\":\"%s\",\n\t\"RefNumber\":\"%s\"}",
				MerchantId,	srTransRec.szOKDRefNo);
			
			
			vdDebug_LogPrintf("vdOKDCreatePOSTData2 QR_TRANS_STATUS [%s]", send_data);
	}
	else if (inType == QR_VOID)
	{
			vdDebug_LogPrintf("******************************************************************");
			vdDebug_LogPrintf("******************vdOKDCreatePOSTData2 VOID***********************");
			vdDebug_LogPrintf("******************************************************************");
			
		    memset(szBuffer, 0x00, sizeof(szBuffer));			

			//strcpy(srTransRec.szOKDBankTransId, "1821303714"); // To get from SALE Check STATUS result			
			vdDebug_LogPrintf("vdOKDCreatePOSTData2 szOKDCustomerNum = [%s] szOKDCustomerNumPword = [%s] szOKDRefNo = [%s] szOKDBankTransId = [%s]", 
			srTransRec.szOKDCustomerNum, srTransRec.szOKDCustomerNumPword, srTransRec.szOKDRefNo,srTransRec.szOKDBankTransId);			
		
			sprintf(szBuffer, "{\n\t\"CustomerNum\":\"%s\",\n\t\"Password\":\"%s\",\n\t\"RefNumber\":\"%s\",\n\t\"PaymentTransId\":\"%s\"}",
				srTransRec.szOKDCustomerNum,
				srTransRec.szOKDCustomerNumPword,
				srTransRec.szOKDRefNo,
				srTransRec.szOKDBankTransId);		
			
				vdDebug_LogPrintf("vdOKDCreatePOSTData2 QR_VOID szBuffer [%s]", szBuffer);

				memset(uszCreditPaymentEncrytedRequestData, 0x00, sizeof(uszCreditPaymentEncrytedRequestData));				
				memset(szIvectorVoid, 0x00, sizeof(szIvectorVoid)); 			

				inBuffer =	strlen((char *)szBuffer);				
				get_env("IvectorVoid",szIvectorVoid,sizeof(szIvectorVoid));

				//AES ECB encode
				#if 0
				// Encrypt plaintext with AES
				inRetVal = inEncryptData_with_AES256_ECB(inAES256KeySet, inAES256KeyIndex, szBuffer, strlen(szBuffer), uszCreditPaymentEncrytedRequestData, &inCreditPaymentEncrytedRequestData);
				
				vdDebug_LogPrintf("vdOKDCreatePOSTData2 QR_VOID uszCreditPaymentEncrytedRequestData [%s] inRetVal [%d]", uszCreditPaymentEncrytedRequestData, inRetVal);
				
				memset(uszCreditPaymentEncrytedRequestDataHex, 0x00, sizeof(uszCreditPaymentEncrytedRequestDataHex));
				wub_hex_2_str(uszCreditPaymentEncrytedRequestData, uszCreditPaymentEncrytedRequestDataHex, inCreditPaymentEncrytedRequestData);
				
				vdDebug_LogPrintf("vdOKDCreatePOSTData2 QR_VOID uszCreditPaymentEncrytedRequestDataHex [%s]", uszCreditPaymentEncrytedRequestDataHex); 
				#else
				// AES CBC encode
				inRetVal = inEncryptData_with_AES128_CBC(inAES256KeySet, inAES256KeyIndex, szBuffer, inBuffer, szIvectorVoid, 16, uszCreditPaymentEncrytedRequestData, &inBuffer);
				#endif


				
				// Format to JSON
				memset(send_data, 0x00, sizeof(send_data)); 		
				memset(MerchantId, 0x00, sizeof(MerchantId));	

				
				//get_env("IvectorVoid",szIvectorVoid,sizeof(szIvectorVoid));
				strcpy(MerchantId, strCBOKD.szSubMerId);
				MerchantId[50] = '\0';	

				//changed Ivector field to IVector to eliminate 0600 response
				sprintf((char *)send_data, (char *)"{\"EncryptedString\":\"%s\",\"IVector\":\"%s\",\"MerchantId\":\"%s\"}", 
					uszCreditPaymentEncrytedRequestData, szIvectorVoid, MerchantId);		


				vdDebug_LogPrintf("vdOKDCreatePOSTData2 QR_VOID send_data [%s]", send_data);	

			
	}
	else if (inType == QR_CANCEL)
	{

			vdDebug_LogPrintf("******************************************************************");
			vdDebug_LogPrintf("******************vdOKDCreatePOSTData2 CANCEL*********************");
			vdDebug_LogPrintf("******************************************************************");

			#if 0
			vdGenerateRandomKeyCBPay(32, szRandomKey);
			szRandomKey[32] = '\0'; 
			strcpy(szReqID, szRandomKey);
			strcpy(szMerID, srTransRec.szMID);
			szMerID[16] = '\0';  
			strcpy(srTransRec.szOKDTransRef, "123456789012345");
			
			//sprintf(send_data, "{\n\t\"reqId\":\"%s\",\n\t\"merId\":\"%s\",\n\t\"transRef\":\"%s\"\n}",
			sprintf(send_data, "{\n\t\"reqId\":\"%s\",\n\t\"merId\":\"%s\",\n\t\"transRef\":\"%s\"}",
				szReqID,
				szMerID,
				srTransRec.szOKDTransRef);

				vdDebug_LogPrintf("vdOKDCreatePOSTData2 QR_CANCEL send_data [%s]", send_data);	
			#else
			
				memset(MerchantId, 0x00, sizeof(MerchantId));	
				strcpy(MerchantId, strCBOKD.szSubMerId);
				MerchantId[50] = '\0';	
							
				//rename field (as per OK$ host) MerchantId to MerchatId to fix 0500 issue.
				sprintf((char *)send_data, (char *)"{\n\t\"MerchatId\":\"%s\",\n\t\"RefNumber\":\"%s\"}",
					MerchantId, srTransRec.szOKDRefNo);
			
			#endif	
			
	}
	else if(inType == QR_TEST_CURL)
	{
		vdDebug_LogPrintf("******************************************************************");
		vdDebug_LogPrintf("***************vdOKDCreatePOSTData2 TEST CURL COMMAND**********");
		vdDebug_LogPrintf("******************************************************************");

		    memset(MerchantId, 0x00, sizeof(MerchantId));				
		    memset(szRefNumber, 0x00, sizeof(szRefNumber));

			
			strcpy(MerchantId, strCBOKD.szSubMerId);
			MerchantId[50] = '\0';  

			//Random key
			vdGenerateRandomKeyCBPay(8, szRefNumber);
			szRefNumber[16] = '\0';  
			strcpy(srTransRec.szOKDRefNo, szRefNumber);			

			
			//rename field (as per OK$ host) MerchantId to MerchatId to fix 0500 issue.
			sprintf((char *)send_data, (char *)"{\n\t\"MerchatId\":\"%s\",\n\t\"RefNumber\":\"%s\"}",
				MerchantId,	srTransRec.szOKDRefNo);
			
			
			vdDebug_LogPrintf("vdOKDCreatePOSTData2 QR_TEST_CURL [%s]", send_data);
	}

	vdDebug_LogPrintf("***vdOKDCreatePOSTData2 END***");
}


void vdWriteAES128KEY(int inKeySet, int inKeyIndex, unsigned char* uszPlainText)
{
	unsigned short KeySet;
	unsigned short KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	unsigned short ret;
	unsigned char KeyData[32];
	unsigned char str[17];
	unsigned char key;
	
	CTOS_KMS2Init();

	CTOS_LCDTClearDisplay();            
	CTOS_LCDTPrintXY(1, 1, "Write AES 128 KEY");
	
	//---------------------------------------------------------------------
	// Write AES Key in plaintext	
	KeySet = inKeySet;
	KeyIndex = inKeyIndex;
	memcpy(KeyData, uszPlainText, 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_AES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = 16;

	ret = CTOS_KMS2KeyWrite(&para);
	vdDebug_LogPrintf("vdWriteAES128KEY = [%04X] KeyIndex[%04X] KeyIndex[%04X] KeyData[%s]", ret, KeySet, KeyIndex, KeyData);
	
	if(ret != d_OK)
	{
		sprintf(str, "ret A = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return;
	}

	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex + 2; // for AES 128 decrypt key.
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;

	ret = CTOS_KMS2KeyWrite(&para);
	vdDebug_LogPrintf("vdWriteAES128KEY = [%04X] KeyIndex[%04X] KeyIndex[%04X] KeyData[%s]", ret, KeySet, KeyIndex + 2, KeyData);

	if(ret != d_OK)
	{
		sprintf(str, "ret B = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return;
	}

	//CTOS_LCDTPrintXY(1, 7, "vdWriteAES128KEY Success!!!");	
	//CTOS_KBDGet(&key);

}


void vdWriteAES256_PlainText(int inKeySet, int inKeyIndex, unsigned char* uszPlainText)
{
	unsigned short KeySet;
	unsigned short KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	unsigned short ret;
	unsigned char KeyData[16];
	unsigned char str[17];
	unsigned char key;
	
	//CTOS_LCDTClearDisplay();            
	//CTOS_LCDTPrintXY(1, 1, "Write AES256 PT");

	
	//---------------------------------------------------------------------
	// Write AES Key in plaintext	
	KeySet = inKeySet;
	KeyIndex = inKeyIndex;
	memcpy(KeyData, uszPlainText, 16);

	
	vdDebug_LogPrintf("vdWriteAES256_PlainText = KeyIndex[%04X] KeyIndex[%04X] KeyData[%s]", KeySet, KeyIndex, KeyData);

	sprintf(str, "KeySet = %04X", KeySet);
	//CTOS_LCDTPrintXY(1, 3, str);
	sprintf(str, "KeyIndex = %04X", KeyIndex);
	//CTOS_LCDTPrintXY(1, 4, str);
	
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_AES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = 16;

	ret = CTOS_KMS2KeyWrite(&para);
	vdDebug_LogPrintf("CTOS_KMS2KeyWrite = [%04X] KeyIndex[%04X] KeyIndex[%04X]", ret, KeySet, KeyIndex);

	if(ret != d_OK)
	{
		//sprintf(str, "ret = 0x%04X", ret);
		//CTOS_LCDTPrintXY(1, 8, str);
		//CTOS_KBDGet(&key);
		CTOS_LCDTPrintXY(1, 7, "Write Error A !!!");
		CTOS_KBDGet(&key);	
		
		return;
	}

	//CTOS_LCDTPrintXY(1, 7, "Write Key Done");

	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex + 2; // for AES 256 decrypt key.
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_DECRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;

	ret = CTOS_KMS2KeyWrite(&para);
	vdDebug_LogPrintf("CTOS_KMS2KeyWrite = [%04X] KeyIndex[%04X] KeyIndex[%04X]", ret, KeySet, KeyIndex + 2);

	if(ret != d_OK)
	{
		//sprintf(str, "ret = 0x%04X", ret);
		//CTOS_LCDTPrintXY(1, 8, str);
		//CTOS_KBDGet(&key);
		CTOS_LCDTPrintXY(1, 7, "Write Error B !!!");
		CTOS_KBDGet(&key);	
		
		return;
	}

	CTOS_LCDTPrintXY(1, 7, "Write Key Successful!!");
	CTOS_KBDGet(&key);	
}


int inCTOSS_DisplayOKDQRCodeAndConfirm(char *intext)
{
	int inRet = -1, iQRSize, iQRX, iQRY;
	int iDelay = get_env_int("XMLCONFIRMDELAY");
	BYTE key;
	USHORT res;
	BOOL fConfirm = (BOOL)get_env_int("XMLCONFIRMQRCODE");
	CTOS_QRCODE_INFO qrcodeInfo;
	int inMax_screenX = 320;
	int inMAx_screenY = 240;
    char szErrMsg[50+1];

    BYTE byKeyBuf;
	BOOL fOnExit = FALSE;
	BOOL fQRCODELevel = FALSE;
	
	//BOOL fOnExit2 = FALSE;
	//BOOL fOnExit3 = FALSE;


	inOKDPayRead(1);
	vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm [%d][%d][%d]", fConfirm, strTCT.byTerminalType, strlen(intext));
	
	if(strlen(intext) < 100)
	{
		//vdDispTransTitle(SALE);
		
		vdDispTransTitle(OK_DOLLAR_TRANS);
		
		iQRSize = 5;
		iQRX = 85;
		iQRY = 50;
	}
	else if((strlen(intext) > 300) && (strlen(intext) <= 399))
	{
		iQRSize = 3;
		iQRX = 75;//85;
		iQRY = 25;//25;
	}
	else if((strlen(intext) > 400) && (strlen(intext) <= 430))
	{
		iQRSize = 3;
		iQRX = 50;//60;
		iQRY = 10;//5;
	}
	else // #1 fix for qrcode case referring to redmine case #2070 
	{

		vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm ELSE!!!!!");

		iQRSize = 2;
		iQRX = 60;
		iQRY = 20;
		fQRCODELevel = TRUE;
	}		


	qrcodeInfo.InfoVersion = QR_INFO_VERSION;
	qrcodeInfo.Size = iQRSize;
	qrcodeInfo.Version = QR_VERSION21X21;

	// #2 fix for qrcode case referring to redmine case #2070 
	if(fQRCODELevel == TRUE)
		qrcodeInfo.Level = QR_LEVEL_Q;//QR_LEVEL_L;
	else
		qrcodeInfo.Level = QR_LEVEL_L;


	//for issue 10003 - Balance on mobile wallet will be deducted even terminal displays "SCANNED BUT NOT PROCESS PAYMENT" #1
	#if 0
	if (fConfirm) // to remove this?
	{

		CTOS_KBDBufFlush();
		while (1)
		{	
		
			vdCTOS_DispStatusMessage("              ");
			res = CTOS_QRCodeDisplay(&qrcodeInfo, intext, iQRX, iQRY);

			if (iDelay <= 0) iDelay = 10;
		
			vdDebug_LogPrintf("LOOP 1");

			#if 0
			if ((strTCT.byTerminalType%2) == 0)
				setLCDPrint27(14,DISPLAY_POSITION_CENTER,"SCAN AND PRESS ENTER");
			else
				setLCDPrint27(8,DISPLAY_POSITION_CENTER,"SCAN AND PRESS ENTER");

			CTOS_Delay(iDelay * 1000); // Delay
			#endif


			//CTOS_SystemWait(20, 0, &dwWakeup);
				
			key = CBWaitKey(20);

			vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm key [%d]", key);

			if (key == d_KBD_CANCEL)
			{

				//ret = CTOS_SystemWait(20, dwWait, &dwWakeup); if use this function, just set dwWait = 0;
				//CTOS_KBDBufFlush();

				CTOS_LCDTClearDisplay();	

				//put_env_int("LOCKPROMPT", 0);

				#if 0
				if(fGetECRTransactionFlg() == TRUE)
				{
					//vdSetECRResponse(ECR_OPER_CANCEL_RESP);
					vdDisplayErrorMsgResp2("","TRANS CANCELLED","");
				}
				else
					vdSetErrorMessage("USER CANCEL");
				#else
					vdSetErrorMessage("");			
				#endif

				vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm USER CANCEL");

				inRet = d_NO; 				
				break;

				
			}

			/*************************************/
			/*         GET TRANSACTION STATUS            */
			/*************************************/
			inRet = inOKDPayGetTransactionStatus();

			vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm inOKDPayGetTransactionStatus inRet [%d]", inRet);

			if (inRet == d_OK){
				inRet = d_OK;
				//return d_OK;
				break;
			}
			else if (inRet != d_OK)//(inRet == QR_FAILED) //transaction is failed if transStatus if E,C,L, or not S
			{  
				vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm inRet == != d_OK");
				
				/*memset(szErrMsg,0x00,sizeof(szErrMsg));
				if (inGetErrorMessage(szErrMsg) > 0) {
					vdDisplayErrorMsgOKD(1, 8, szErrMsg);

				}*/
				
				break;
			}


			vdDebug_LogPrintf("LOOP 2");
			
		}

		
		vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm HERE");

		#if 0
		// TODO: remove this condition for automated receipt printing
		if (inRet != d_OK)
		{

			//put function for cancel transaction here
			
		
			if(fGetECRTransactionFlg() == TRUE)
			{
				//vdSetECRResponse(ECR_OPER_CANCEL_RESP);
				vdDisplayErrorMsgResp2("","TRANS CANCELLED","");
			}
			else
				vdSetErrorMessage("USER CANCEL");
		}
		// TODO: remove this condition for automated receipt printing
		#endif
		
	}
	else
	{
		inRet = d_OK;
	}
	#else
	//int inTimeOut = 1000; //10 secs
	int inTimeOut = strCBOKD.inQP_CurlTimeout * 1000;
	int inStatusCode;
	BOOL fSuccessTrans = FALSE;
	
	vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm TIMEOUT [%d]", inTimeOut);
	
	CTOS_TimeOutSet(TIMER_ID_3, inTimeOut);		  //disable keyboard till timeout
	
	put_env_int("INTIMEOUT",0); // initial/reset value
	
	//while(1)//loop for time out	 
	do
	{		  
		res = CTOS_QRCodeDisplay(&qrcodeInfo, intext, iQRX, iQRY);
	
		if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)		 
		{		
			vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm TIMEOUT REACHED!");
			CTOS_LCDTClearDisplay();

			if(fSuccessTrans != TRUE){

				#if 1
				while(1)
				{

					setLCDPrint27(3,DISPLAY_POSITION_CENTER,"TIMEOUT REACHED");
					setLCDPrint27(6,DISPLAY_POSITION_CENTER,"PRESS ENTER/CANCEL");
					setLCDPrint27(7,DISPLAY_POSITION_CENTER,"TO CONTINUE");
					
					CTOS_Delay(300);
			
					CTOS_KBDHit(&byKeyBuf); 	
					if (byKeyBuf == d_KBD_CANCEL)
					{		
					
						vdSetErrorMessage("");
						//vdSetErrorMessage("USER CANCEL");
						
						inRet = inOKDPayGetTransactionStatus();
					
						if (inRet == d_OK)
							inRet = d_OK;
						else
							inRet = d_NO;
						
						fOnExit = TRUE;
						
						break;
					}
					else if(byKeyBuf == d_KBD_ENTER)
					{
				
						inRet = inOKDPayGetTransactionStatus();
					
						if (inRet == d_OK)
							inRet = d_OK;
						else
							inRet = d_NO;
						
						fOnExit = TRUE;
						break;
						
					}
					
			  	}
				#else
						inRet = inOKDPayGetTransactionStatus();

						if(inRet == d_OK)
							inRet = d_OK;							
						else
							inRet = d_NO;
						
						fOnExit = TRUE;
						break;
				
				#endif
			}	
			
		}	
		else
		{
			inRet = inOKDPayGetTransactionStatus2();
			
			if (inRet == d_OK)
			{
				fSuccessTrans = TRUE;
				inRet = d_OK;
				fOnExit = TRUE;
				break;
			}
			else
			{
					// 600 - Canceled By User..
					if(get_env_int("INSTATUSCODE600") == 1) 
					{
						CTOS_LCDTClearDisplay();
						put_env_int("INSTATUSCODE600",0);


						vdDisplayErrorMsgResp2("CANCELED","BY"," USER");

						fSuccessTrans = TRUE;
						inRet = d_NO;
						fOnExit = TRUE;
						break;

					}	
					
					if(fFailedCurlStatus) // connection failed...check comms
					{
						
						fFailedCurlStatus = FALSE;					
						CTOS_LCDTClearDisplay();
						
						while(1)
						{
							setLCDPrint27(2,DISPLAY_POSITION_LEFT,"DO NOT PROCEED AND");
							setLCDPrint27(3,DISPLAY_POSITION_LEFT,"CANCEL OK$ PAYMENT!");
							
							setLCDPrint27(5,DISPLAY_POSITION_LEFT,"PLS CHECK TERMINAL");
							setLCDPrint27(6,DISPLAY_POSITION_LEFT,"CONNECTION");
							setLCDPrint27(7,DISPLAY_POSITION_LEFT,"AND TRY AGAIN");
					
							
							CTOS_Beep();
							CTOS_Delay(1000);
					
							CTOS_KBDHit(&byKeyBuf);
							if (byKeyBuf == d_KBD_CANCEL || byKeyBuf == d_KBD_ENTER){
								
								fSuccessTrans = TRUE;
								inRet = d_NO;
								fOnExit = TRUE;
								break;
							}
						}						
						
					}
					
				}	

			}		
		

	}while(fOnExit != TRUE);
	
	#endif
	
	vdDebug_LogPrintf("inCTOSS_DisplayOKDQRCodeAndConfirm END inRet [%d]", inRet);
	
	return inRet;
}


//to test CURL command connection
void vdOKDCreatePOSTData3(unsigned char *send_data, int inType){
	char szRefNumber [16+1] = {0};
	char MerchantId [50+1] = {0};

	vdDebug_LogPrintf("******************************************************************");
	vdDebug_LogPrintf("***************vdOKDCreatePOSTData3 TEST CURL COMMAND**********");
	vdDebug_LogPrintf("******************************************************************");

    memset(MerchantId, 0x00, sizeof(MerchantId));				
    memset(szRefNumber, 0x00, sizeof(szRefNumber));

			
    //strcpy(MerchantId, strCBOKD.szSubMerId);
    strcpy(MerchantId, "de83fe562127");
	MerchantId[50] = '\0';  

	//Random key
	vdGenerateRandomKeyCBPay(8, szRefNumber);
	szRefNumber[16] = '\0';  
	strcpy(srTransRec.szOKDRefNo, szRefNumber);			

			
	//rename field (as per OK$ host) MerchantId to MerchatId to fix 0500 issue.
	sprintf((char *)send_data, (char *)"{\n\t\"MerchatId\":\"%s\",\n\t\"RefNumber\":\"%s\"}",
		MerchantId,	srTransRec.szOKDRefNo);
						
	vdDebug_LogPrintf("vdOKDCreatePOSTData3 QR_TEST_CURL [%s]", send_data);
	vdDebug_LogPrintf("***vdOKDCreatePOSTData3 END***");
}


int inEncryptData_with_AES128_CBC(int inKeySet, int inKeyIndex, unsigned char *uszPlainText, int inPlainTextLength, unsigned char *uszInitialVektor, int inInitialVektorLength, unsigned char *uszCipherText, int *inCipherTextLength)
{
	USHORT ret;
	CTOS_KMS2DATAENCRYPT_PARA para;
	BYTE *pCihperKey;
	BYTE CihperKeyLength;
	BYTE str[17];
	BYTE key;	
	BYTE SK_Ciphered[16];
	BYTE plaindata[256];
	BYTE cipherdata[256];
    unsigned char uszZero[32+1] = {0};
    //unsigned char uszZero[16+1] = {0};

	CTOS_KMS2Init();
	
	CTOS_LCDTClearDisplay();

	/*		
	CTOS_LCDTPrintXY(1, 1, "EncryptData");
	CTOS_LCDTPrintXY(1, 2, "with AES 128 CBC");
	CTOS_LCDTPrintXY(1, 3, "CBC");
	*/
	inPlainTextLength = PadData (uszPlainText, inPlainTextLength, 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2DATAENCRYPT_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = inKeySet;
	para.Protection.CipherKeyIndex = inKeyIndex;
	para.Protection.CipherMethod = KMS2_DATAENCRYPTCIPHERMETHOD_CBC;

	para.Protection.SK_Length = 0;
	
	para.Input.ICVLength = inInitialVektorLength;
	para.Input.pICV = uszInitialVektor;

	para.Input.Length = inPlainTextLength;
	para.Input.pData = uszPlainText;

	para.Output.pData = uszCipherText;
	*inCipherTextLength = inPlainTextLength;

	ret = CTOS_KMS2DataEncrypt(&para);
	vdDebug_LogPrintf("inEncryptData_with_AES128_CBC = [%04X] inPlainTextLength[%d] KeyIndex[%04X] KeyIndex[%04X] uszPlainText[%s]", ret, inPlainTextLength, inKeySet, inKeyIndex, uszPlainText);

	if(ret != d_OK)
	{
		sprintf(str, "ret = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return ret;
	}

	DebugAddHEX("uszCipherText", (unsigned char*)uszCipherText, *inCipherTextLength);
	// if not need based 64 encode just return here	

	int encryptedDataLen = 0; // declare integer store data length for encryted message.
	int inasciiBase64Enc = 0;
	unsigned char uszECRAscData[4096] = {0};
	char* asciiBase64Enc = NULL; // declare pointer to store based 64 encode data.
	unsigned char data[4096] = {0};

	encryptedDataLen = *inCipherTextLength;
	unsigned char* encrypted = (unsigned char*)uszCipherText; // point to data need use for base 64 encode.

	//sprintf(str, "HERE 1 = 0x%04X", ret);
	//CTOS_LCDTPrintXY(1, 8, str);
	//CTOS_KBDGet(&key);
	
	asciiBase64Enc = base64_encode((char *)encrypted, encryptedDataLen, &inasciiBase64Enc);
	vdDebug_LogPrintf("asciiBase64Enc [%s]", asciiBase64Enc);

	//sprintf(str, "HERE 2 = 0x%04X", ret);
	//CTOS_LCDTPrintXY(1, 8, str);
	//CTOS_KBDGet(&key);

	//after copy data then free asciiBase64Enc.
	memcpy((char *)data, asciiBase64Enc, inasciiBase64Enc);
	vdDebug_LogPrintf("data [%s] [%ld]\n\n", data, inasciiBase64Enc);

	//sprintf(str, "HERE 3 = 0x%04X", ret);
	//CTOS_LCDTPrintXY(1, 8, str);
	//CTOS_KBDGet(&key);

	memcpy((char *)uszCipherText, data, inasciiBase64Enc);
	*inCipherTextLength = inasciiBase64Enc;
	vdDebug_LogPrintf("uszCipherText [%s] [%ld]\n\n", uszCipherText, *inCipherTextLength);	

	//sprintf(str, "HERE 4 = 0x%04X", ret);
	//CTOS_LCDTPrintXY(1, 8, str);
	//CTOS_KBDGet(&key);

	
	free(asciiBase64Enc);
	// return based64 encode data.

	
	//sprintf(str, "AES ENCRYPT SUCCESS = 0x%04X", ret);
	//CTOS_LCDTPrintXY(1, 8, str);
	//CTOS_KBDGet(&key);

	
	return ret;
}


int inOKDVoidTransaction(void)
{
	int inRet = -1;
	BYTE key;
	char szBuffer[100+1];  		
	char szTransactionid[100+1];  	
	char szMasterTransId[100+1];  		
	char szReferenceNum[100+1];
	char szRefNum[100+1];
	char szTempBuff[1000];
	char szEMVString[500];
	MemoryStruct write_data, read_data;
	int inCode;
	//inCurl_CommsInit();

	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");

	write_data.memory = malloc(1); 
	write_data.size = 0;	 

	vdDebug_LogPrintf("inOKDVoidTransaction");

	
	inRet = inCBOKDSend_request(&read_data,&write_data, QR_VOID);
	if(inRet != d_OK)
		return inRet;	
	
	memset(szTempBuff, 0x00, sizeof(szTempBuff));
	memcpy(szTempBuff, write_data.memory, write_data.size);


	/*
	if (fXMLPrint)
	{
		char szResponseBuff[1000] = {0};

		strcpy(szResponseBuff, szTempBuff);
		
		inPrintSodexoPacket("RX-GRAB", szResponseBuff, strlen(szResponseBuff), FALSE);
	}
	*/
	


	memset(szBuffer, 0x00, sizeof(szBuffer));
 	//inExtractField(szTempBuff, szBuffer, "code");
 	inExtractField2(szTempBuff, szBuffer, "Code");

	vdDebug_LogPrintf("CODE %s", szBuffer);

	//strcpy(szBuffer, "200");

    if (strcmp(szBuffer, "200") == 0){
		
        vdDebug_LogPrintf("VOID SUCCESSFUL!!!");
		strcpy(srTransRec.szRespCode, "00");

		//Master Trans ID result
		memset(szMasterTransId, 0x00, sizeof(szMasterTransId));
		inExtractField(szTempBuff, szMasterTransId, "MasterTransId");		
		vdDebug_LogPrintf("MasterTransId  ---	%s", szMasterTransId);

	
		//Transaction ID result
		memset(szTransactionid, 0x00, sizeof(szTransactionid));		
		inExtractField(szTempBuff, szTransactionid, "Transactionid");
		vdDebug_LogPrintf("Transactionid  ---	%s", szTransactionid);
		strcpy(srTransRec.szOKDBankTransId, szTransactionid);
		srTransRec.szOKDBankTransId[10] = '\0';  

		//Reference Number result
		memset(szReferenceNum,0x00,sizeof(szReferenceNum));
		inExtractField(szTempBuff, szReferenceNum, "ReferenceNum");
		vdDebug_LogPrintf("ReferenceNum  ---	%s", szReferenceNum);

		//RefNum result
		memset(szRefNum,0x00,sizeof(szRefNum));
		inExtractField(szTempBuff, szRefNum, "RefNum");
		vdDebug_LogPrintf("RefNum  ---	%s", szRefNum);
		
		inRet = d_OK;
		
		
    }else{

        vdDebug_LogPrintf("VOID FAILED!!!");

		#if 0
		inExtractField(szTempBuff, szBuffer, "Msg");
		vdDebug_LogPrintf("Msg  ---	%s", szBuffer);
		
	    if (strlen(szBuffer) > 0)
			vdSetErrorMessage(szBuffer);
		else		
			vdSetErrorMessage("VOID FAILED");
		#else
		inCode = atoi(szBuffer);
		GetOKDPerformResutlMessage(inCode);
		
		#endif
		vdDebug_LogPrintf("free memory");			
		free(write_data.memory);
			
		CTOS_LCDTClearDisplay();

		inRet = d_NO;
		
    }
//#endif

	CTOS_LCDTClearDisplay();

    return inRet;

}

int inCTOS_OKDQRVoidFlowProcess(void)
{
    int inRet = d_NO;
    
    //display title
    vdDispTransTitle(VOID); 

	ErmTrans_Approved = FALSE;


	inCSTRead(1); //currency for CBpay is always MMK

	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::srTransRec.IITid[%d]",srTransRec.IITid);
	vdDebug_LogPrintf("HDTid %d", srTransRec.HDTid);
	vdDebug_LogPrintf("szCurSymbol %s", strCST.szCurSymbol);

	
	inHDTRead(srTransRec.HDTid); // to fix accum void total - 11052019
	
    inRet = inCTOS_CheckMustSettle();	
    if(d_OK != inRet){		
        return inRet;
    }

	vdDebug_LogPrintf("before confirm invoice amount");

    inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet){		
        return inRet;
    }

	CTOS_LCDTClearDisplay();
	vdCTOS_DispStatusMessage("PROCESSING...");

	vdDebug_LogPrintf("before curl comms init");


    inRet = inOKDVoidTransaction();
    if(d_OK != inRet)
    {
		vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::inRet = inOKDVoidTransaction [%d]", inRet);
    
        return inRet;
    }

	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::A");
	
    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::B inRet [%d] byVoided [%d]", inRet, srTransRec.byVoided);


    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;
	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::C");

    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::D");

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;
	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::E srTransRec.byVoided [%d]", srTransRec.byVoided);


	vdDebug_LogPrintf("before print receipt");

    inRet = ushCTOS_printReceipt();

	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::After print receipt inRet [%d]", inRet, srTransRec.byVoided);
	
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::After print receipt");
	

	if(strTCT.byERMMode != 0)
		ErmTrans_Approved = TRUE;


    if (srTransRec.byVoided == TRUE) {
	
		vdDebug_LogPrintf("inCTOS_OKDPayQRVoidFlowProcess::srTransRec.byVoided == TRUE");
    }


    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

	
    return d_OK;
}


int inCTOS_VoidOKDGetCustomerNO(void) {
    BYTE key;
    USHORT usX = 1, usY = 6;
    BYTE baString[100 + 1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02;
    USHORT usInvoiceLen = 6;	
    USHORT usOKDCustNOLen = 20;
    BYTE baPIN[6 + 1];
    BYTE szInvNoAsc[30 + 1];
    BYTE szInvNoBcd[15];
    BYTE bRet;
    int iLens = 6;
    int inResult;
    char szBcd[INVOICE_BCD_SIZE + 1];

	vdDebug_LogPrintf("inCTOS_VoidOKDGetCustomerNO");

    if (inMultiAP_CheckSubAPStatus() == d_OK) {
        return d_OK;
    }

    if (strTCT.fECR) // tct
    {
        if (fECRTxnFlg == 1) {
            return ST_SUCCESS;
        }
    }


    memset(szInvNoAsc, 0x00, sizeof (szInvNoAsc));
    memset(szInvNoBcd, 0x00, sizeof (szInvNoBcd));

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "CUSTOMER NO: ");

    while (TRUE) {
        //usOKDCustNOLen = 30;
        vdDebug_LogPrintf("11bRet[%d]atoi(usOKDCustNOLen)=[%d]usOKDCustNOLen[%d]", bRet, atoi(szInvNoAsc), usOKDCustNOLen);
        bRet = InputString(usX, usY, 0x00, bShowAttr, szInvNoAsc, &usOKDCustNOLen, 1, d_INPUT_TIMEOUT);
        vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usOKDCustNOLen[%d]", bRet, atoi(szInvNoAsc), usOKDCustNOLen);
        if (bRet == d_KBD_CANCEL) {
            CTOS_LCDTClearDisplay();
            vdSetErrorMessage("USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }


        if (atoi(szInvNoAsc) != 0) {
            inAscii2Bcd(szInvNoAsc, szInvNoBcd, 20);
            DebugAddSTR("CUST NUM:", szInvNoAsc, 20);
            memcpy(srTransRec.szOKDCustomerNum, szInvNoAsc, 20);
            break;
        }
    }

	vdDebug_LogPrintf("inCTOS_VoidOKDGetCustomerNO srTransRec.szOKDCustomerNum [%s]", srTransRec.szOKDCustomerNum);

    return ST_SUCCESS;
}



int inCTOS_VoidOKDGetCustomerPword(void) {
    BYTE key;
    USHORT usX = 1, usY = 6;
    BYTE baString[100 + 1];
    USHORT iStrLen = 6;
    BYTE bShowAttr = 0x02;
    USHORT usInvoiceLen = 6;	
    USHORT usOKDCustNOLen = 20;
    BYTE baPIN[6 + 1];
    BYTE szInvNoAsc[30 + 1];
    BYTE szInvNoBcd[15];
    BYTE bRet;
    int iLens = 6;
    int inResult;
    char szBcd[INVOICE_BCD_SIZE + 1];

	vdDebug_LogPrintf("inCTOS_VoidOKDGetCustomerPword");

    if (inMultiAP_CheckSubAPStatus() == d_OK) {
        return d_OK;
    }

    if (strTCT.fECR) // tct
    {
        if (fECRTxnFlg == 1) {
            return ST_SUCCESS;
        }
    }


    memset(szInvNoAsc, 0x00, sizeof (szInvNoAsc));
    memset(szInvNoBcd, 0x00, sizeof (szInvNoBcd));

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "PASSWORD: ");

    while (TRUE) {
        //usOKDCustNOLen = 30;
        vdDebug_LogPrintf("11bRet[%d]atoi(usOKDCustNOLen)=[%d]usOKDCustNOLen[%d]", bRet, atoi(szInvNoAsc), usOKDCustNOLen);
        //bRet = InputString(usX, usY, 0x01, bShowAttr, szInvNoAsc, &usOKDCustNOLen, 1, d_INPUT_TIMEOUT);
		bRet = InputStringAlpha(usX, usY, 0x01, bShowAttr, szInvNoAsc, &usOKDCustNOLen, 1, d_INPUT_TIMEOUT);

		
        vdDebug_LogPrintf("bRet[%d]atoi(szInvNoAsc)=[%d]usOKDCustNOLen[%d]", bRet, atoi(szInvNoAsc), usOKDCustNOLen);
        if (bRet == d_KBD_CANCEL) {
            CTOS_LCDTClearDisplay();
            vdSetErrorMessage("USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }


        if (atoi(szInvNoAsc) != 0) {
            inAscii2Bcd(szInvNoAsc, szInvNoBcd, 20);
            DebugAddSTR("CUST NUM PWORD:", szInvNoAsc, 20);
            memcpy(srTransRec.szOKDCustomerNumPword, szInvNoAsc, 20);
            break;
        }
    }

	vdDebug_LogPrintf("szOKDCustomerNumPword srTransRec.szOKDCustomerNumPword [%s]", srTransRec.szOKDCustomerNumPword);

    return ST_SUCCESS;
}


short vduiOKDAskConfirmContinue(void)
{
    unsigned char key;
  
    vduiClearBelow(2);
    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"YOU WANT TO");
    vduiDisplayStringCenter(5,"CONTINUE?");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL)
            return -1;
        else
            vduiWarningSound();
    }
    
}


#endif


int inCTOS_MPU_CBPay(void)
{
	int inRet = d_NO;
	//fEnteredMenu = TRUE;
	inSetColorMenuMode();
		
	CTOS_LCDTClearDisplay();
	//vdCTOS_SetTransType(SALE);
	vdCTOS_SetTransType(CB_PAY_TRANS);
	vdDispTransTitle(srTransRec.byTransType);
	vdCTOS_DispStatusMessage("PROCESSING...");  

  	vdCTOS_TxnsBeginInit();

/*
	if (fECRTxnFlg == 1){
		
		inRet = inCTOS_MultiAPGetData();
		if(d_OK != inRet)
			return inRet;
		
	}
*/

    // TODO: set to 13-CBPAY HOST
	srTransRec.HDTid = CBPAY_HOST_INDEX;
	strCDT.HDTid = CBPAY_HOST_INDEX;
	//srTransRec.IITid = CBPAY_HOST_INDEX;
	srTransRec.IITid = 10;

	inIITRead(srTransRec.IITid);
	inCPTRead(srTransRec.HDTid);
	strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
	
	//BDOPAYGlobalTranType = QR_SALE;

	
	inRet = inCTOS_CBPayFlowProcess();

//	if (!fECRBuildSendOK){	
//		inMultiAP_ECRSendSuccessResponse();
//	}	
//	fECRBuildSendOK = FALSE;
	
	inCTOS_inDisconnect();
// for Case 1595, 1600, 1603 , 1605
    if(strTCT.fUploadReceiptIdle != TRUE)
        inCTOSS_UploadReceipt();
	
	vdCTOS_TransEndReset();
	vdDebug_LogPrintf("AFTER TRANSENDRESET");
	//vdCTOSS_MainAPMemoryRecover();
	
	return inRet;
}



int inCTOS_CBPayFlowProcess(void)
{
	int inRet = d_NO;
	int inRetVal = d_NO;
	USHORT ushEMVtagLen;
	BYTE   EMVtagVal[64];
	BYTE   szStr[64];
	int inEVENT_ID = 0;
	BOOL fBinRoute=TRUE;
	int inSMCardRetryCount=0;
	char szTemp1[d_LINE_SIZE + 1];
	//fTimeOutFlag = FALSE; 

	//vdCTOS_SetTransType(SALE);
	vdCTOS_SetTransType(CB_PAY_TRANS);
	
	//memset(szTransRef, 0x00, sizeof(szTransRef));

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

	CTOS_LCDTClearDisplay();	
	vdCTOS_DispStatusMessage("PROCESSING...");  


	
    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

		

	vdDebug_LogPrintf("TESTQR SALE [%d] [%d]", strCDT.HDTid,srTransRec.MITid);




/*
    if (inMMTReadRecordEx(srTransRec.HDTid,srTransRec.MITid) != d_OK) 
    {
        
		vdDebug_LogPrintf("TRANSACTION not allowed");
        vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
		inDatabase_TerminalCloseDatabase();
        return(d_NO);
    }	
	
	inDatabase_TerminalCloseDatabase();
*/

	inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
	{
	    strcpy(srTransRec.szRespCode,"");
		//strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);
        return inRet;
    }

	//inGrabPayRead(1);
#if 0
	inRet = inCurl_CommsInit();	
	if (inRet != d_OK)
	{
		vdSetECRResponse(ECR_COMM_ERROR_RESP);
		return inRet;
	}
#endif	
	vduiClearBelow(1);


    inRet = inCTOS_GetInvoice();
	if(d_OK != inRet)
			return inRet;
	

    inRet = inGenerateQRCode();
    if(d_OK != inRet)
        return inRet;

// TODO: Do this next using API check transaction
	

	/* No longer need to press enter to proceed with txn
	inRet = inCBPayGetTransactionStatus();

		if(d_OK != inRet)
	    {
	    		//vdSetECRResponse(ECR_COMMS_ERR);
	        return inRet;
	    }
	*/

//	vdSetWalletDateAndTime();

//	vdSetRRN_PAN_Authcode();


	//memset(srTransRec.szPartnerTxnID,0x00,sizeof(srTransRec.szPartnerTxnID));
	//strcpy(srTransRec.szPartnerTxnID,strGrabPay.szPartnerTxnID);//save partner txn ID.
	
	inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    vdDebug_LogPrintf("inCTOS_UpdateAccumTotal");
	
    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

/*
	fECRBuildSendOK = TRUE;	
	if (fECRBuildSendOK){	
	    inRet = inMultiAP_ECRSendSuccessResponse();
	}
*/
// for Case 1595, 1600, 1603 , 1605
    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;


    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;

		
    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

	CTOS_LCDTClearDisplay();
			
    vdSetErrorMessage("");
    return d_OK;
}


int inGenerateQRCode(void)
{
	int inRet;
  	MemoryStruct write_data, read_data;
	char szTempBuff[1000];
	char szEMVString[500];
	BYTE key;

	// TODO: replace instances of szQRCode to struct
	char	szQRCode[512+1] = {0};

	
	
	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");

	write_data.memory = malloc(1); 
	write_data.size = 0;	 

	inRet = inCBPaySend_request(&read_data,&write_data, QR_GENERATE);
	if(inRet != d_OK)
	{
		//vdSetECRResponse(ECR_COMMS_ERR);
		return inRet;
	}

	memset(szTempBuff, 0x00, sizeof(szTempBuff));
	memcpy(szTempBuff, write_data.memory, write_data.size);


	if (fXMLPrint)
		{
			char szResponseBuff[1000] = {0};

			strcpy(szResponseBuff, szTempBuff);
			
			//inPrintSodexoPacket("RX-GRAB", szResponseBuff, strlen(szResponseBuff), FALSE);
		}


	// TODO: define a structure for CBPAY here. for the meantime, use HC values
	memset(szQRCode, 0x00, sizeof(szQRCode));
 	inExtractField(szTempBuff, szQRCode, "merDqrCode");
	vdDebug_LogPrintf("qrcode %s", szQRCode);

	/* fields not needed for CBBPAy
	//memset(srTransRec.szTransactionID, 0x00, sizeof(srTransRec.szTransactionID));
 	*/
 	
	free(write_data.memory);

	if (strlen(szQRCode) <= 0){
		vdSetErrorMessage("Generate QR ERROR");
		//vdSetECRResponse(ECR_COMMS_ERR);
		return ST_ERROR;
	}

	inExtractField(szTempBuff, srTransRec.szTransRef, "transRef");
	vdDebug_LogPrintf("transRef --- %s", srTransRec.szTransRef);

	inExtractField(szTempBuff, srTransRec.szRefNo, "refNo");
	vdDebug_LogPrintf("refNo --- %s", srTransRec.szRefNo);

	//put_env_charEx("PARTNERTXNID",strGrabPay.szPartnerTxnID);
	
	CTOS_LCDTClearDisplay();

	inRet = inCTOSS_DisplayQRCodeAndConfirm(szQRCode);
	if (inRet == d_NO)
		{
			int inResult=0;
		
			inResult = inCBPayCancelTransaction();

			if (inResult == QR_APPRVD)
				{
					CTOS_LCDTClearDisplay();
					return d_OK;
				}
			else
				return d_NO;
			
		}
	else if(inRet != d_OK)
		{
			return inRet;		
		}
	
	CTOS_LCDTClearDisplay();

	return d_OK;
}




int inCTOS_CBPayQRVoidFlowProcess(void)
{
    int inRet = d_NO;

	//fECRBuildSendOK = FALSE; /* BDO: For ECR -- sidumili */
	//fTimeOutFlag = FALSE; /*BDO: Flag for timeout --sidumili*/
    //vdCTOS_SetTransType(VOID);
    
    //display title
    vdDispTransTitle(VOID); 


	inCSTRead(1); //currency for CBpay is always MMK

	/*
    inRet = inCTOS_LoadCDTandIIT();
    if(d_OK != inRet)
        return inRet;
     */
	vdDebug_LogPrintf("srTransRec.IITid[%d]",srTransRec.IITid);

//1 need this???	
//	inRet = inCTOS_VoidSelectHostNoPreConnect();
//	if(d_OK != inRet)
//        return inRet;
	
	vdDebug_LogPrintf("HDTid %d", srTransRec.HDTid);
	vdDebug_LogPrintf("szCurSymbol %s", strCST.szCurSymbol);
	inHDTRead(srTransRec.HDTid); // to fix accum void total - 11052019

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet){
		
		//if (fGetECRTransactionFlg() == TRUE){
		//    strcpy(srTransRec.szRespCode,"");
		//	strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);
		//}
			
        return inRet;

    }

	vdDebug_LogPrintf("before confirm invoice amount");

    inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet){		
		 //   strcpy(srTransRec.szRespCode,"");
		//	strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);		
        return inRet;

    }

	CTOS_LCDTClearDisplay();
	vdCTOS_DispStatusMessage("PROCESSING...");

	vdDebug_LogPrintf("before curl comms init");
#if 0	
	inRet = inCurl_CommsInit();	
	if (inRet != d_OK)
	{
		vdSetECRResponse(ECR_COMMS_ERR);
		return inRet;
	}
#endif


    inRet = inCBPayVoidTransaction();
    if(d_OK != inRet)
    {
        //vdSetECRResponse(ECR_COMMS_ERR);
        return inRet;
    }


 //   if(memcmp(srTransRec.szTempTime, srTransRec.szTime, TIME_BCD_SIZE) == 0) /*if time are the same it means there is no DE12 receive, get new date and time*/
 //       vdGetTimeDate(&srTransRec);


	
    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

	//fECRBuildSendOK = TRUE;	
	//if (fECRBuildSendOK){	
	//    inRet = inMultiAP_ECRSendSuccessResponse();
	//}
// for Case 1595, 1600, 1603 , 1605
    if (isCheckTerminalMP200() == d_OK) {
        vdCTOSS_DisplayStatus(d_OK);
    }

    inRet = inCTOSS_ERM_ReceiptRecvVia();
    if(d_OK != inRet)
       return inRet;


	vdDebug_LogPrintf("before print receipt");

    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

	
    return d_OK;
}



int inCBPayCancelTransaction(void)
{
	int inRet = -1;
	BYTE key;
	char szBuffer[100+1];  	
	char szTempBuff[1000];
	char szEMVString[500];
	MemoryStruct write_data, read_data;
	//inCurl_CommsInit();


	vdDebug_LogPrintf("inCBPayCancelTransaction START");

	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");

	write_data.memory = malloc(1); 
	write_data.size = 0;	 

	
	inRet = inCBPaySend_request(&read_data,&write_data, QR_CANCEL);
	if(inRet != d_OK)
		return inRet;	
	
	memset(szTempBuff, 0x00, sizeof(szTempBuff));
	memcpy(szTempBuff, write_data.memory, write_data.size);


	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField(szTempBuff, szBuffer, "code");

	vdDebug_LogPrintf("STATUS %s", szBuffer);

	if (strcmp(szBuffer, "0000") == 0 )
		{
			vdDebug_LogPrintf("success");
			strcpy(srTransRec.szRespCode, "00");
			inRet = d_OK;
		
		 }
	else if (strcmp(szBuffer, "QR0701") == 0)
		{
			vdDebug_LogPrintf("can't cancel txn, proceed with approval");
			strcpy(srTransRec.szRespCode, "00");
			//inRet = QR_APPRVD;

			inRet = inCBPayGetTransactionStatus(); //check transaction status again to get tranx ID

			vdDebug_LogPrintf("inRet == %d",inRet);

			if (inRet == d_OK)
				inRet = QR_APPRVD;
			else if (inRet == QR_FAILED)  //transaction is failed if transStatus is E,C,L, or not S
				inRet = d_NO;


		}
	else
		{

			vdDebug_LogPrintf("not approved");
			if (strlen(szBuffer) > 0)
				vdSetErrorMessage(szBuffer);
			else		
				vdSetErrorMessage("CANCEL TRANSACTION FAILED");
				
			vdDebug_LogPrintf("free memory");			
			free(write_data.memory);
				
			CTOS_LCDTClearDisplay();

			return d_NO;
		
    }
//#endif

	CTOS_LCDTClearDisplay();

    return inRet;

}


int inCBPayVoidTransaction(void)
{
	int inRet = -1;
	BYTE key;
	char szBuffer[100+1];  	
	char szTempBuff[1000];
	char szEMVString[500];
	MemoryStruct write_data, read_data;
	//inCurl_CommsInit();

	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");

	write_data.memory = malloc(1); 
	write_data.size = 0;	 

	
	inRet = inCBPaySend_request(&read_data,&write_data, QR_VOID);
	if(inRet != d_OK)
		return inRet;	
	
	memset(szTempBuff, 0x00, sizeof(szTempBuff));
	memcpy(szTempBuff, write_data.memory, write_data.size);


	/*
	if (fXMLPrint)
	{
		char szResponseBuff[1000] = {0};

		strcpy(szResponseBuff, szTempBuff);
		
		inPrintSodexoPacket("RX-GRAB", szResponseBuff, strlen(szResponseBuff), FALSE);
	}
	*/
	


	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField(szTempBuff, szBuffer, "code");

	vdDebug_LogPrintf("STATUS %s", szBuffer);

    if (strcmp(szBuffer, "0000") == 0){
		
        vdDebug_LogPrintf("success");
		strcpy(srTransRec.szRespCode, "00");
		inRet = d_OK;
		
		
    }else{

        vdDebug_LogPrintf("not approved");
	    if (strlen(szBuffer) > 0)
			vdSetErrorMessage(szBuffer);
		else		
			vdSetErrorMessage("VOID FAILED");
			
		vdDebug_LogPrintf("free memory");			
		free(write_data.memory);
			
		CTOS_LCDTClearDisplay();

		return d_NO;
		
    }
//#endif

	CTOS_LCDTClearDisplay();

    return inRet;

}




int inCBPayGetTransactionStatus(void)
{
	int inRet;
	int res, i;
	BYTE key;
	char szTempBuff[1000];
	char szEMVString[500];
	char szBuffer[100+1];
	char szAmtBuff[12+1];

	MemoryStruct write_data, read_data;


	BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");
	//inCurl_CommsInit();

	
	//4; fix for overlapped "SCAN QRCODE" production issue for cbpay - raised by Khine of UTS 05062020 #5
	CTOS_LCDTClearDisplay();	

	//vdCTOSS_QRCodeDisplayOnly(strGrabPay.szQRCode);


	vdCTOS_DispStatusMessage("PROCESSING...");  

	vdDebug_LogPrintf("***inGrabPayGetTransactionStatus***");

		 

	//inGrabPayRead(1);

	//vdSend_request(&read_data,&write_data, QR_TRANS_STATUS);
    //memset(szTempBuff, 0x00, sizeof(szTempBuff));
	//memcpy(szTempBuff, write_data.memory, write_data.size);



	vdDebug_LogPrintf("inQP_CurlRetries[%d]",strCBPay.inQP_CurlRetries);

	// TODO: hardcoded for now -- curlretries  -- REMOVE curl retries condition
	for (i = 1; i<= strCBPay.inQP_CurlRetries;i++){
	//for (i = 1; i<= 6;i++){
		write_data.memory = malloc(1); 
		write_data.size = 0;

	
		res = inCBPaySend_request(&read_data,&write_data, QR_TRANS_STATUS);
		
		memset(szTempBuff, 0x00, sizeof(szTempBuff));
		memcpy(szTempBuff, write_data.memory, write_data.size);
		

		if(res != CURLE_OK){ 
			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
		}else if (res == CURLE_OK){

				#if 0 //comment out the printing of logs for the meantime
				
				if (fXMLPrint)
					{
						char szResponseBuff[1000] = {0};

						strcpy(szResponseBuff, szTempBuff);
						
						inPrintSodexoPacket("RX-GRAB", szResponseBuff, strlen(szResponseBuff), FALSE);
					}
				#endif
		
			inExtractField(szTempBuff, szBuffer, "transStatus");
		
			vdDebug_LogPrintf("transStatus --- %s  ---  %d", szBuffer, i);

			szBuffer[1] = '\0';
				
			if (strcmp(szBuffer, "P") == 0)//transaction not found
				{
					// TODO: hardcoded for now -- curlretries
					//if(i == 6)

					//if(i == strCBPay.inQP_CurlRetries)
					//{
						//vdDisplayErrorMsgResp2("","TRANSACTION","PENDING");
						vdDebug_LogPrintf("TRANSACTION PENDING");
						free(write_data.memory);
						return QR_PENDING;
					//}
					//else
					//{
					//	continue;
					//	free(write_data.memory);
					//}
				}
			else if (strcmp(szBuffer, "E") == 0)
				{
					vdDisplayErrorMsgResp2("","QR","EXPIRED");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					//return d_NO;
					return QR_FAILED;
				}
			else if (strcmp(szBuffer, "C") == 0)
				{
					vdDisplayErrorMsgResp2("","CANCELLED"," ");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					//return d_NO;
					return QR_FAILED;
				}
			else if (strcmp(szBuffer, "L") == 0)
				{
					vdDisplayErrorMsgResp2("","OVER","LIMIT");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					//return d_NO;
					return QR_FAILED;
				}
			else if (strcmp(szBuffer, "S") != 0) //other transaction status aside from 'S'
				{
					vdDisplayErrorMsgResp2("","TRANSACTION","UNSUCCESSFUL");
					vdDebug_LogPrintf("free memory");
					free(write_data.memory);
					//return d_NO;
					return QR_FAILED;
				}
		}
		
		if (res == CURLE_OPERATION_TIMEDOUT)
			 continue;
		else
			break;

	}


	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField(szTempBuff, szBuffer, "code");

	vdDebug_LogPrintf("code  ---  %s", szBuffer);

	if (strcmp(szBuffer, "0000") == 0)
		{
		
			vdDebug_LogPrintf("success");
			strcpy(srTransRec.szRespCode, "00");


			memset(szBuffer, 0x00, sizeof(szBuffer));
			inExtractField(szTempBuff, szBuffer, "bankTransId");

			vdDebug_LogPrintf("bankTransId  ---  %s", szBuffer);
			strcpy(srTransRec.szBankTransId, szBuffer);


			inCBPaySaveDateTime();	

			inRet = d_OK;
			
		}
	else
		{

			vdDebug_LogPrintf("not approved");
			if (strlen(szBuffer) > 0)
				vdSetErrorMessage(szBuffer);
			else		
				vdSetErrorMessage("COMM ERROR");
				
			vdDebug_LogPrintf("free memory");
				
			free(write_data.memory);
				
			CTOS_LCDTClearDisplay();

			return d_NO;
		
  		  }




		
#if 0	
	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField(szTempBuff, szBuffer, "updated");

	memset(szBuffer, 0x00, sizeof(szBuffer));
 	inExtractField(szTempBuff, szBuffer, "currency");
#endif
	vdDebug_LogPrintf("free memory");

	free(write_data.memory);

	//CTOS_LCDTClearDisplay();

    return inRet;

}


unsigned char CBWaitKey(short Sec) {
    unsigned char c;
    long mlsec;

    mlsec = Sec * 100;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3, mlsec);
    while (1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey) { //If isKey is TRUE, represent key be pressed //

            vduiLightOn();
            CTOS_KBDGet(&c);
            return c;
        } else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES) {
            return d_KBD_ENTER;
        }
    }
}




int inCTOSS_DisplayQRCodeAndConfirm(char *intext)
{
	int inRet = -1, iQRSize, iQRX, iQRY;
	int iDelay = get_env_int("XMLCONFIRMDELAY");
	BYTE key;
	USHORT res;
	BOOL fConfirm = (BOOL)get_env_int("XMLCONFIRMQRCODE");
	CTOS_QRCODE_INFO qrcodeInfo;
	int inMax_screenX = 320;
	int inMAx_screenY = 240;


	vdDebug_LogPrintf("inCTOSS_DisplayQRCodeAndConfirm [%d][%d][%d]", fConfirm, strTCT.byTerminalType, strlen(intext));
	
	if(strlen(intext) < 100)
	{
		//vdDispTransTitle(SALE);
		
		vdDispTransTitle(CB_PAY_TRANS);
		
		iQRSize = 5;
		iQRX = 85;
		iQRY = 50;
	}
	else if((strlen(intext) > 300) && (strlen(intext) <= 399))
	{
		iQRSize = 3;
		iQRX = 75;//85;
		iQRY = 25;//25;
	}
	else if(strlen(intext) >= 400)
	{
		iQRSize = 3;
		iQRX = 50;//60;
		iQRY = 10;//5;
	}
	else
	{
		iQRSize = 3;
		iQRX = 85;//60;
		iQRY = 25;//5;
	}		


	qrcodeInfo.InfoVersion = QR_INFO_VERSION;
	qrcodeInfo.Size = iQRSize;
	qrcodeInfo.Version = QR_VERSION21X21;
	qrcodeInfo.Level = QR_LEVEL_L;




	if (fConfirm) // to remove this?
	{


		while (1)
		{	
		
			vdCTOS_DispStatusMessage("              ");
			res = CTOS_QRCodeDisplay(&qrcodeInfo, intext, iQRX, iQRY);

			if (iDelay <= 0) iDelay = 10;
// as per CB no need to display text for larger QRCODE to fit the terminal screen.
#if 0
			
			if ((strTCT.byTerminalType % 2) == 0)
			{
				if (fConfirm)
					setLCDPrint27(14,DISPLAY_POSITION_CENTER,"SCAN QRCODE");			
				else
				{
					setLCDPrint27(14,DISPLAY_POSITION_CENTER,"     SCAN QRCODE    ");
					CTOS_Delay(iDelay * 1000); // Delay
				}
			}
			else
			{
				//4; fix for overlapped "SCAN QRCODE" production issue for cbpay - raised by Khine of UTS 05062020 #2
				//CTOS_LCDTSelectFontSize(d_FONT_16x16);
			
				if (fConfirm)
					setLCDPrint27(15,DISPLAY_POSITION_CENTER,"SCAN QRCODE");	//setLCDPrint27(8, DISPLAY_POSITION_CENTER,"SCAN QRCODE");	
				else
				{
				setLCDPrint27(15,DISPLAY_POSITION_CENTER,"     SCAN QRCODE    ");//setLCDPrint27(8, DISPLAY_POSITION_CENTER,"SCAN QRCODE");	
					CTOS_Delay(iDelay * 1000); // Delay
				}
			}
#endif

			
			vdDebug_LogPrintf("LOOP 1");

				
			key = CBWaitKey(30);


			//4; fix for overlapped "SCAN QRCODE" production issue for cbpay - raised by Khine of UTS 05062020 #3
			//CTOS_LCDTSelectFontSize(d_FONT_16x30);

			if (key == d_KBD_CANCEL)
			{
				//4; fix for overlapped "SCAN QRCODE" production issue for cbpay - raised by Khine of UTS 05062020 #4
				CTOS_LCDTClearDisplay();	
				
				if(fGetECRTransactionFlg() == TRUE)
				{
					//vdSetECRResponse(ECR_OPER_CANCEL_RESP);
					vdDisplayErrorMsgResp2("","TRANS CANCELLED","");
				}
				else
					vdSetErrorMessage("USER CANCEL");
				
				return d_NO;
			}


			inRet = inCBPayGetTransactionStatus();

			if (inRet == d_OK)
				return d_OK;
			else if (inRet == QR_FAILED)  //transaction is failed if transStatus if E,C,L, or not S
				break;


			vdDebug_LogPrintf("LOOP 2");
			
		}

		

		// TODO: remove this condition for automated receipt printing
		if (inRet != d_OK)
		{

			//put function for cancel transaction here
			
		
			if(fGetECRTransactionFlg() == TRUE)
			{
				//vdSetECRResponse(ECR_OPER_CANCEL_RESP);
				vdDisplayErrorMsgResp2("","TRANS CANCELLED","");
			}
			else
				vdSetErrorMessage("USER CANCEL");
		}
		// TODO: remove this condition for automated receipt printing
		
	}
	else
	{
		inRet = d_OK;
	}
	
	return inRet;
}




size_t write_CBpay_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);

	vdDebug_LogPrintf("write_callback=[%d][%s]", realsize, contents);
	DebugAddHEX("CONTENTS",contents,realsize);
	
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

size_t read_CBpay_callback(void *dest, size_t size, size_t nmemb, void *userp)
{
	struct MemoryStruct *wt = (struct MemoryStruct *)userp;
	size_t buffer_size = size*nmemb;

	vdDebug_LogPrintf("read_callback %s", dest);

	if (wt->size) {
		/* copy as much as possible from the source to the destination */
		size_t copy_this_much = wt->size;
		if (copy_this_much > buffer_size)
			copy_this_much = buffer_size;
		memcpy(dest, wt->memory, copy_this_much);

		wt->memory += copy_this_much;
		wt->size -= copy_this_much;
		
		vdDebug_LogPrintf("read_callback=[%d][%s]", wt->size, dest);
		return copy_this_much; /* we copied this many bytes */
	}

	return 0; /* no more data left to deliver */
}





int inCBPaySend_request(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType) 
{

	CURL *curl;
 	CURLcode res;
	int inResult;
	int i, inDay,inRet = 0;
	char szBuffer[1000];
	char szRFCdate[40+1];
	char szConent_len[30+1];
	char szAuthorization[100+1]={0};
	BYTE pbtBuff[4096+1]; 
	USHORT pusLen;


	char postthis[2000] = {0};
	char szURL[150] = {0};
	char szTempURL[150] = {0};
	char szAuthenToken[350] = {0};

	vdDebug_LogPrintf("vdSend_request TransType [%d]", TransType);
	vdCTOS_DispStatusMessage("PROCESSING...");

	inCBPayRead(1);	


	/*delete later*/
	//vdDebug_LogPrintf("QR_GENERATE[%s] ---- QR_TRANS_STATUS[%s] --- QR_VOID ---[%s] --- QR_CANCEL[%s]",strCBPay.szGenerateURL, strCBPay.szTranStatusURL, strCBPay.szVoidURL, strCBPay.szCancelURL );
	/*delete later*/

	//BOOL fXMLPrint = (BOOL)get_env_int("XMLPRINT");
	
	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	
	/* Check for errors */
	if (res != CURLE_OK) {		
		vdDebug_LogPrintf("curl_global_init failed");	
		return d_OK;
	}
	vdDebug_LogPrintf("curl_global_init successful");
	
	curl = curl_easy_init();
	
	if(curl) {
		
		vdDebug_LogPrintf("curl_easy_init successful");

		
		if (TransType == QR_GENERATE)
		{
			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://122.248.120.187:4443/payment-api/v1/qr/generate-transaction.service"); //Hardcoded values for now.
			strcpy(szURL, strCBPay.szGenerateURL);
			vdDebug_LogPrintf("GENERATE QR URL is %s",szURL);
		}		
		else if (TransType == QR_TRANS_STATUS)
		{

			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://122.248.120.187:4443/payment-api/v1/qr/check-transaction.service"); //Hardcoded values for now.
			strcpy(szURL, strCBPay.szTranStatusURL);
			vdDebug_LogPrintf("STATUS URL is %s",szURL);
			
		}
        else if (TransType == QR_VOID)
		{    
			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://122.248.120.187:4443/payment-api/v1/qr/refund-transaction.service"); //Hardcoded values for now.
			strcpy(szURL, strCBPay.szVoidURL);
			vdDebug_LogPrintf("VOID URL is %s",szURL);
			
        	}
	else if (TransType == QR_CANCEL)
		{
			memset(szURL, 0x00, sizeof(szURL));
			//sprintf(szURL, "https://122.248.120.187:4443/payment-api/v1/qr/cancel-transaction.service"); //Hardcoded values for now.
			strcpy(szURL, strCBPay.szCancelURL);
			vdDebug_LogPrintf("CANCEL URL is %s",szURL);
		}

		/*inogre SSL verify*/
		//if (strGrabPay.SSLEnable == 1){
			//vdDebug_LogPrintf("SSL ENABLE URL %s", szURL);
			curl_easy_setopt(curl, CURLOPT_URL,szURL);

			#if 1
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			#endif
		//}else
		//	curl_easy_setopt(curl, CURLOPT_URL,szURL);

		//1 Always Use "POST" for CB Project
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

		

		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_CBpay_callback);
		

		/* pointer to pass to our read function */
		curl_easy_setopt(curl, CURLOPT_READDATA, (void *)contents);

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_CBpay_callback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

		/* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
		itself */ 

		struct curl_slist *headerlist = NULL;


		//memset(szRFCdate,0x00,sizeof(szRFCdate));
		//vdGenerateDate(szRFCdate);
		
        if (TransType == QR_GENERATE)
		{	

	       	memset(postthis, 0x00, sizeof(postthis));
        		vdCBPayCreatePOSTData(&postthis, TransType);
        	}

		// TODO: other conditions aside from QR_GENERATE to be implemented....

	else if(TransType == QR_TRANS_STATUS || TransType == QR_VOID  || TransType == QR_CANCEL)
		{
				memset(postthis, 0x00, sizeof(postthis));
    				vdCBPayCreatePOSTData(&postthis, TransType);
		}

		

				
		
//		if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID){
			vdDebug_LogPrintf("QR GENERATE HEADER AND BODY");
			//headerlist = curl_slist_append(headerlist, "Authen-Token: eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE1NjY5ODIyNjQsIm1lcklkIjoiTTYwMTEwMDAwMDAwMDAxNiJ9.hWDqDWfxtK1tE8Iz-a-R7RiN2hYJZzkQMtn66rpf_NY");
			sprintf(szAuthenToken, "Authen-Token: %s", strCBPay.szAuthenToken);
			headerlist = curl_slist_append(headerlist, szAuthenToken);
			headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

			vdDebug_LogPrintf("curl_slist_append data1=[%s]",headerlist->data);
			vdDebug_LogPrintf("curl_slist_append data2=[%s]",headerlist->next->data);

 //       }		

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
//        if (TransType == QR_GENERATE || TransType == QR_TRANS_STATUS || TransType == QR_VOID)
//		{
			vdDebug_LogPrintf("DATA TO SEND %s",postthis);
			vdDebug_LogPrintf("POSTFIELDSIZE %li",(long)strlen(postthis));
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));			
  //      }


        //for (i = 1; i<= strBDOPAY.inCurlRetries;i++){

		// TODO: set timeout as variable.... set to 6 for the meantime
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);

		vdDebug_LogPrintf("curl_easy_perform %d retries=%d", res, i);


		/* FUNCTION CALL FOR printing logs.....
		if( fXMLPrint && 
			((res == CURLE_OPERATION_TIMEDOUT) || (res == CURLE_OK)))
			{

				char szReqBuff[2000] = {0};

				strcpy(szReqBuff, postthis);
			
				//inPrintSodexoPacket("TX-GRAB BODY", szReqBuff, strlen(szReqBuff), TRUE);
			}

		*/

			/* Check for errors */ 
		if(res != CURLE_OK) 
		{
			vdDebug_LogPrintf("curl_easy_perform %s ", curl_easy_strerror(res));
			GetDPerformResutlMessage(res);	

			vdDebug_LogPrintf("curl_easy_cleanup !!!");			
			curl_easy_cleanup(curl);// always cleanup		

			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist);// cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");
			curl_easy_reset(curl);	// reset all handles

			vdDebug_LogPrintf("Encountered error!!!");

			
			return(d_NO);
		}
		else
		{
			vdCTOS_DispStatusMessage("RECEIVING...");
			/* always cleanup */ 
			curl_easy_cleanup(curl);			
			
			vdDebug_LogPrintf("curl_slist_free_all !!!");			
			curl_slist_free_all(headerlist); // cleanup headerlist

			vdDebug_LogPrintf("curl_global_cleanup !!!");			
			curl_global_cleanup();// we're done with libcurl, so clean it up

			vdDebug_LogPrintf("curl_easy_reset !!!");			
			curl_easy_reset(curl);// reset all handle

		}
		

		

		/* always cleanup */ 
		//vdDebug_LogPrintf("curl_easy_cleanup !!!");
		//curl_easy_cleanup(curl);
	}
    return res;
}




void vdDisplayErrorMsgResp2 (char *msg, char *msg2, char *msg3)
{
   CTOS_LCDTClearDisplay();

   if ((strTCT.byTerminalType % 2) == 1) 
   {
      vduiDisplayStringCenter(3,msg);
      vduiDisplayStringCenter(4,msg2);
      vduiDisplayStringCenter(5,msg3);		
   }
   else 
   {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
   }
	 
   CTOS_Beep();
   CTOS_Delay(1500);
   CTOS_LCDTClearDisplay();
}



void vdGenerateRandomKeyCBPay(int inLen, BYTE *ptrResult)
{
	int i;
	time_t t;
	BYTE szRandKey[64+1];
    BYTE sKey[2+1];
	int inRandNum=0;

    vdDebug_LogPrintf("vdGenerateRandomKey");
	vdDebug_LogPrintf("Len:%d", inLen);
	
	memset(szRandKey, 0, sizeof(szRandKey));
	
	/* Intializes random number generator */
	srand((unsigned) time(&t));
	
	/* get 16 random numbers from 0 to 49 */
	for( i = 0 ; i < inLen ; i++ ) {
	   inRandNum=rand() % 255;
	   //vdDebug_LogPrintf("b=%02X", (int)inRandonNum);
	   memset(sKey, 0, sizeof(sKey));
	   sprintf(sKey, "%02X", inRandNum);
	   memcpy(&szRandKey[i*2], sKey, 2);
	}
	//vdDebug_LogPrintf("*szRandomKey", szRandomKey);
	vdDebug_LogPrintf("szRandKey:%s", szRandKey);
	
	//wub_str_2_hex(szRandomKey, ptrResult, 32);
	//memcpy(ptrResult, szRandKey, strlen(szRandKey));
	strcpy(ptrResult, szRandKey);
}


void vdCBPayCreatePOSTData(char *szBuffer, int inType){
	
	BYTE szAmtBuff[20+1];
	BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
	vdDebug_LogPrintf("***vdCBPayCreatePOSTData START***");


	BYTE szReqID [32+1] = {0};
	char szMerID [16+1] = {0};
	char szSubMerID [16+1] = {0};
	char szCB_TerminalID[16+1] = {0};
	char szTransAmount [13+1] = {0};
	char szCurrency [3+1] = {0};
	char szRef1 [25+1] = {0};
	char szRef2 [25+1] = {0};

	BYTE szRandomKey[32+1] = {0};


	inCBPayRead(1);


vdDebug_LogPrintf("***vdCBPayCreatePOSTData HDTid[%d]", srTransRec.HDTid);

// TODO: hardcoded values for now.... 	
	if (inType == QR_GENERATE)
	{
		    	memset(szAmtBuff, 0x00, sizeof(szAmtBuff));
			wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
			vdCTOSS_FormatAmount("NNNNNNNNNNNn.nn", szBaseAmt,szAmtBuff);
						
			vdGenerateRandomKeyCBPay(32, szRandomKey);
			szRandomKey[32] = '\0';  
			strcpy(szMerID, srTransRec.szMID);
			szMerID[16] = '\0';  
			strcpy(szSubMerID, strCBPay.szSubMerId);
			strcpy(szCB_TerminalID, srTransRec.szTID);
			strcpy(szTransAmount, szAmtBuff);
			strcpy(szCurrency, "MMK");
			strcpy(szRef1, "ref1120190825");
			strcpy(szRef2, "ref1220190825");

			sprintf(szBuffer, "{\n\t\"reqId\":\"%s\",\n\t\"merId\":\"%s\",\n\t\"subMerId\":\"%s\",\n\t\"terminalId\":\"%s\",\n\t\"transAmount\":\"%s\",\n\t\"transCurrency\":\"%s\",\n\t\"ref1\":\"%s\",\n\t\"ref2\":\"%s\"\n}",
			//szReqID, 
			szRandomKey,
			szMerID, 
			szSubMerID,
			szCB_TerminalID,
			szTransAmount,
			szCurrency,
			szRef1,
			szRef2);

			vdDebug_LogPrintf("vdCBPayCreatePOSTData %s", szBuffer);
	}


// TODO: implement later....

	else if(inType == QR_TRANS_STATUS)
	{
	

			strcpy(szMerID, srTransRec.szMID);
			szMerID[16] = '\0';  
			
			sprintf(szBuffer, "{\n\t\"transRef\":\"%s\",\n\t\"merId\":\"%s\"\n}",
				srTransRec.szTransRef,
				szMerID);
			
			
			vdDebug_LogPrintf("vdCreatePOSTData %s", szBuffer);
	}

	else if (inType == QR_VOID)
	{
		
			vdGenerateRandomKeyCBPay(32, szRandomKey);
			szRandomKey[32] = '\0'; 
			strcpy(szReqID, szRandomKey);
			strcpy(szMerID, srTransRec.szMID);
			szMerID[16] = '\0';  
			
			sprintf(szBuffer, "{\n\t\"reqId\":\"%s\",\n\t\"merId\":\"%s\",\n\t\"refNo\":\"%s\"\n}",
				szReqID,
				szMerID,
				srTransRec.szRefNo);
	}


	else if (inType == QR_CANCEL)
	{
		
			vdGenerateRandomKeyCBPay(32, szRandomKey);
			szRandomKey[32] = '\0'; 
			strcpy(szReqID, szRandomKey);
			strcpy(szMerID, srTransRec.szMID);
			szMerID[16] = '\0';  
			
			sprintf(szBuffer, "{\n\t\"reqId\":\"%s\",\n\t\"merId\":\"%s\",\n\t\"transRef\":\"%s\"\n}",
				szReqID,
				szMerID,
				srTransRec.szTransRef);
	}

	vdDebug_LogPrintf("***vdCBPayCreatePOSTData END***");
}


// TODO: commented for now... this function will print logs.
#if 0

int inPrintSodexoPacket(unsigned char *pucTitle,unsigned char *pucMessage, int inLen, BOOL fRequest)
{
    char ucLineBuffer[d_LINE_SIZE+4];
    unsigned char *pucBuff;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 4];
	BOOL fLoop = TRUE;
	int i = 0;
	int iLimit = 48;
	BYTE ch = 0;

	//vdDebug_LogPrintf("--inPrintXMLPacket--");
	
    if (inLen <= 0)
        return(ST_SUCCESS);

	if( printCheckPaper()==-1)
	return -1;

#if 0	
	vdCTOSS_PrinterStart(100);
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  
#endif


	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetWorkTime(50000,1000);
	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);  




	if (fRequest)	
		inCCTOS_PrinterBufferOutput("[REQUEST]", &stgFONT_ATTRIB, FALSE);
	else
		inCCTOS_PrinterBufferOutput("[RESPONSE]", &stgFONT_ATTRIB, FALSE);
	
    memset(szStr, 0x00, sizeof(szStr));
	memset(baTemp, 0x00, sizeof(baTemp));
    sprintf(szStr,"[%s] [%d] \n", pucTitle, inLen);
	inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, FALSE);
	vdCTOS_PrinterFline(1);		

	do
	{
		memset(szStr, 0x00, sizeof(szStr));
		memcpy(szStr, &pucMessage[i], iLimit);
		pucMessage[i+1] = '\0';
		purge_chr(szStr, '\n');
		//vdDebug_LogPrintf("i[%d].Len[%d].szStr[%s]", i, strlen(szStr), szStr);	
		inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, FALSE);
			
		i+=iLimit;
		if (i > inLen) break;

		
	}while (fLoop);	
	
	vdCTOS_PrinterFline(4);
	
    vdCTOSS_PrinterEnd();

	//vdDebug_LogPrintf("--Exit--");
	
    return d_OK;
}
#endif




int inExtractField(unsigned char *uszRecData, char *szField, char *szSearchString){
	char *ptr;
 	char szWorkBuff1[4096+1];
 	char szWorkBuff2[4096+1];
 	char szSearchToken[2];
 	int i;


	vdDebug_LogPrintf("inExtractField");

	//vdDebug_LogPrintf("extract field %s", uszRecData);

 	memset(szSearchToken, 0x00, sizeof(szSearchToken));
 	szSearchToken[0] = '"';
 	memset(szWorkBuff1, 0x00, sizeof(szWorkBuff1));
 	ptr = NULL;
 	strcpy(szWorkBuff1,uszRecData);
 	ptr =strstr(szWorkBuff1, szSearchString);

    if (ptr == NULL)
		{
			vdDebug_LogPrintf("inExtractField  NULL");
			
			return FAIL;
   	 	}
	
 	memset(szWorkBuff2, 0x00, sizeof(szWorkBuff2));
 	strcpy(szWorkBuff2, ptr);
	

    for (i = 1; i<4; i++){


		vdDebug_LogPrintf("inExtractField  --- i [%d]", i);
		
  		memset(szWorkBuff1, 0x00, sizeof(szWorkBuff1));
  		ptr = NULL;
  		strcpy(szWorkBuff1, szWorkBuff2);
		
  
  		ptr =strstr(szWorkBuff1, szSearchToken);
  
 		memset(szWorkBuff2, 0x00, sizeof(szWorkBuff2));
  		strcpy(szWorkBuff2, ptr+1);
		
  
    }

 	memcpy(szField, szWorkBuff1, ptr - szWorkBuff1);

 
 	vdDebug_LogPrintf("szField %s-[%s] ", szSearchString, szField);

	return d_OK;
}




int GetDPerformResutlMessage(int iResult)
{
	BOOL fRetryFlag = FALSE;
	
	vdDebug_LogPrintf("--GetDPerformResutlMessage--");
	//vdDebug_LogPrintf("inDPTranType[%d]", srTransRec.inDPTranType);
	vdDebug_LogPrintf("iResult[%d]", iResult);

	//iCURLMode = CURL_ERROR;
	//put_env_int("CURLMODE",iCURLMode);
	
	switch (iResult)
	{
		case CURLE_UNSUPPORTED_PROTOCOL:
			vdDisplayErrorMsgResp2("","UNSUPPORTED","PROTOCOL");
			break;
		case CURLE_FAILED_INIT:
			vdDisplayErrorMsgResp2("","INITIALIZATION","FAILED");
			break;
		case CURLE_URL_MALFORMAT:
			vdDisplayErrorMsgResp2("","CURL URL","MALFORMAT");
			break;
		case CURLE_NOT_BUILT_IN:
			vdDisplayErrorMsgResp2("","CURL NOT","BUILT IN");
			break;
		case CURLE_COULDNT_RESOLVE_PROXY:
			vdDisplayErrorMsgResp2("PROXY","COULD NOT","BE RESOLVED");
			break;
		case CURLE_COULDNT_RESOLVE_HOST:
			vdDisplayErrorMsgResp2("","COULD NOT","RESOLVE HOST");
			break;
		case CURLE_COULDNT_CONNECT:
			vdDisplayErrorMsgResp2("","CONNECT","FAILED");
			break;
		case CURLE_FTP_WEIRD_SERVER_REPLY:
			vdDisplayErrorMsgResp2("FTP WEIRD","SERVER","REPLY");
			break;
		case CURLE_REMOTE_ACCESS_DENIED:
			vdDisplayErrorMsgResp2("REMOTE","ACCESS","DENIED");
			break;
		case CURLE_HTTP_RETURNED_ERROR:
			vdDisplayErrorMsgResp2("HTTP","RETURNED","ERROR");
			break;
		case CURLE_READ_ERROR:
			vdDisplayErrorMsgResp2("","CURL READ","ERROR");
			break;
		case CURLE_OUT_OF_MEMORY:
			vdDisplayErrorMsgResp2("","OUT OF","MEMORY");
			break;
		case CURLE_OPERATION_TIMEDOUT:
			vdDisplayErrorMsgResp2("","TIMEOUT","REACHED");
			fRetryFlag = TRUE;
			break;
		default:
			vdDisplayErrorMsgResp2("","UNDEFINED","ERROR");
			break;
			
	}

	//CTOS_Beep();
	//CTOS_Beep();
	//CTOS_Delay(1000);

	vdSetErrorMessage("");
		
	if(fRetryFlag == TRUE)
		return d_OK;

	return d_NO;
}


int inCBPaySaveDateTime(void)
{
	CTOS_RTC SetRTC;
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	char szDateTime[14+1];
	char szYear[3];


	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	vdDebug_LogPrintf("year[%s],date[%02x][%02x]",srTransRec.szYear,srTransRec.szDate[0],srTransRec.szDate[1]);
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
	vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );


	if(atol(szTemp) == 0)
		{
			sprintf(szTemp ,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);	
			memset(srTransRec.szDate,0x00,sizeof(srTransRec.szDate));
			wub_str_2_hex(szTemp,srTransRec.szDate,DATE_ASC_SIZE);
		}

	if(atol(szTemp1) == 0)
		{
			sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	
			memset(srTransRec.szTime,0x00,sizeof(srTransRec.szTime));
			wub_str_2_hex(szTemp1,srTransRec.szTime,TIME_ASC_SIZE);
		}
}

