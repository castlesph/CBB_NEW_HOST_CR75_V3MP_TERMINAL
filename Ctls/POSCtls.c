#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <emv_cl.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <vwdleapi.h>


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
#include "../Includes/CTOSinput.h"


#include "POSCtls.h"

#define	d_MsessionFlag		0x00
#define d_AsessionFlag		0x01
BOOL g_isForcedOnl = FALSE;
int g_inTransType = 0;


/*Please add you own function here*/
USHORT usCTOSS_NonEMVCLCardProcess(void)
{
	//uiRtn = CTOS_CLInitComm(38400);
	
	//ret = CTOS_CLAPDU( baTxBuf, usTxLen, baRxBuf, usRxLen );
	
	return d_OK;
}

int inCTOSS_CheckCVMPrint(void)
{
	int usret;

	int AIDlen;
	BYTE AID[16];
	//char szTemp[20];
	char tmpbuf[20];

	BYTE	szTotalAmt[12+1];
	BYTE	szStr[45];
	char szTemp[AMT_ASC_SIZE+1];
	
	char szTemp2[AMT_ASC_SIZE+1];
	int inTemp;


#ifdef ENHANCEMENT_1861
	int inUPISignLine = get_env_int("UPISIGNLINE");
#endif


	vdDebug_LogPrintf("inCTOSS_CheckCVMPrint START srTransRec.IITid[%d] strCST.szCurSymbol[%s] srTransRec.HDTid [%d] strHDT.inCurrencyIdx [%d]  strCST.inCurrencyIndex [%d]",
	srTransRec.IITid, strCST.szCurSymbol, srTransRec.HDTid, strHDT.inCurrencyIdx,  strCST.inCurrencyIndex);


	//commented- CST value was already called in ushCTOS_PrintBody() function. - line: 2557 - 08242022
	//#ifdef CBB_FIN_ROUTING
	#if 0
	if(srTransRec.HDTid == 23 || srTransRec.HDTid == 17)		
		inCSTRead(1);
	else	
		inCSTRead(strHDT.inCurrencyIdx);
	#endif

	// exit if card is visa/ mc
	//1) http://118.201.48.214:8080/issues/75.72. #1Visa/MC sign line issue The current flow is whatever fVEPS, fQPS values are 0 or 1, sign line is present for any amount.
	if(srTransRec.IITid == 1 ||  srTransRec.IITid == 2 || srTransRec.IITid == 4 || srTransRec.IITid == 8) // also for JCB cards
		return d_OK;


	memset(AID,0x00,sizeof(AID));
	AIDlen = srTransRec.stEMVinfo.T84_len;
	memcpy(AID,srTransRec.stEMVinfo.T84,AIDlen);

	memset(szTemp,0x00,sizeof(szTemp));

#if 1
	if(strcmp(strCST.szCurSymbol, "MMK") == 0)
	{
		memset(szTemp, 0x00, sizeof(szTemp));
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		sprintf(szTotalAmt, "%012.0f", atof(szTemp)/100);
		memset(szStr, 0x00, sizeof(szStr));
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmt, szStr);
	} 
	else 
	{
		memset(szTemp2, 0x00, sizeof(szTemp2));
	   wub_hex_2_str(srTransRec.szTotalAmount, szTemp2, AMT_BCD_SIZE);
		memset(szStr, 0x00, sizeof(szStr));
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp2, szStr);
	}
#else
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
#endif

	vdDebug_LogPrintf("inCTOSS_CheckCVMPrint szTemp[%s] szTemp2[%s], szTotalAmt[%s] szStr[%s]",szTemp, szTemp2, szTotalAmt, szStr);


	memset(tmpbuf,0x00,sizeof(tmpbuf));
	inCTOSS_GetEnvDB("CVMAMT",tmpbuf);
	if (strlen(tmpbuf)> 0)
	{
		vdDebug_LogPrintf("CVMAMT =[%s]",tmpbuf);
	}

	vdDebug_LogPrintf("AIDlen =[%d]-----",AIDlen);
	vdPCIDebug_HexPrintf("AID",AID,AIDlen);




	#ifdef ENHANCEMENT_1861
	vdDebug_LogPrintf("inCTOSS_CheckCVMPrint:inUPISignLine =[%d]",inUPISignLine);
	
	// 0 - no sign line on all entry mode and T9F34 (CVM) result
	// 1 - with sign line and CVM checking
	if(inUPISignLine == 1)
	{
		if ((memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x01",AIDlen) == 0)
			||(memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x02",AIDlen) == 0)
			|| (memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x03",AIDlen) == 0))
		{

			vdDebug_LogPrintf("inCTOSS_CheckCVMPrint Here");
			
			//related to http://118.201.48.210:8080/redmine/issues/1525.92.5
			// if greater, less than is working, if equal should not include signline
			#if 0
			if (atol(szTemp) > atol(tmpbuf))
				return d_OK;
			#else
			if(strcmp(strCST.szCurSymbol, "MMK") == 0)
				inTemp = atoi(szTemp);
			else
				inTemp = atoi(szTemp2);

			vdDebug_LogPrintf("inCTOSS_CheckCVMPrint inTemp[%d] tmpbuf[%d]", inTemp, atoi(tmpbuf));
			/*	
					//if(inTemp <= atoi(tmpbuf))
					if(atoi(tmpbuf) >= inTemp)
					{
						vdDebug_LogPrintf("inCTOSS_CheckCVMPrint CVMAMT >= TRANS AMOUNT, NO SIGNATURE REQUIRED");
						return d_NO;
					}
					else
					{
						vdDebug_LogPrintf("inCTOSS_CheckCVMPrint TRANS AMOUNT > CVMAMT");					
						return d_OK;
					}
			*/
			if(inTemp > atoi(tmpbuf)){				
				vdDebug_LogPrintf("inCTOSS_CheckCVMPrint TRANS AMOUNT > CVMAMT");									
				return d_OK;
			}
			else
			{
				vdDebug_LogPrintf("inCTOSS_CheckCVMPrint TRANS AMOUNT < CVMAMT  , NO SIGNATURE REQUIRED");			
				return d_NO;
			}

			
			#endif
		}
	}
	#else
	if ((memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x01",AIDlen) == 0)
		||(memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x02",AIDlen) == 0)
		|| (memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x03",AIDlen) == 0))
	{
		if (atol(szTemp) > atol(tmpbuf))
			return d_OK;
	}

	#endif

	
	return d_NO;

}

int inCTOSS_CheckCVMOnlinepin(void)
{
	int usret;

	int AIDlen;
	BYTE AID[16];
	char szTemp[20];
	char tmpbuf[20];


	memset(AID,0x00,sizeof(AID));
	AIDlen = srTransRec.stEMVinfo.T84_len;
	memcpy(AID,srTransRec.stEMVinfo.T84,AIDlen);

	memset(szTemp,0x00,sizeof(szTemp));
	wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
	vdDebug_LogPrintf("szTotalAmount =[%s]-----",szTemp);

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	inCTOSS_GetEnvDB("CVMAMT",tmpbuf);
	if (strlen(tmpbuf)> 0)
	{
		vdDebug_LogPrintf("CVMAMT =[%s]",tmpbuf);
	}

	vdDebug_LogPrintf("AIDlen =[%d]-----",AIDlen);
	vdPCIDebug_HexPrintf("AID",AID,AIDlen);

	if ((memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x02",AIDlen) == 0)
		|| (memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x03",AIDlen) == 0))
	{
		if (atol(szTemp) < atol(tmpbuf))
			return d_OK;
	}

	return d_NO;

}


ULONG inCTOSS_CLMOpen(void)
{
	ULONG ulAPRtn;
	ulAPRtn = EMVCL_Open();
	if(d_EMVCL_NO_ERROR != ulAPRtn) 
	{
		return ulAPRtn;
	}

	return d_OK;
}

void vdCTOSS_GetCLMVersion(char *cVer)
{
	EMVCL_GetAPIVersion(cVer);
}

void vdCTOSS_CLMClose(void)
{
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		EMVCL_Close();

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		if (1 ==strTCT.byQP3000SPort)
			VW_CloseComm(d_VW_COMM_COM1);
		else if (2 ==strTCT.byQP3000SPort)
			VW_CloseComm(d_VW_COMM_COM2);
		else if (3 ==strTCT.byQP3000SPort)
			VW_CloseComm(d_VW_COMM_COM3);
		else
			VW_CloseComm(d_VW_COMM_COM2);	
	}
}
//------------------------------------------------------------------------

ULONG MsessionAuthen(void) {
    AUTHKEY_DATA stKeyData;
	ULONG ulAPRtn;
    BYTE baRN_Reader[9], baTmpRN[17], baOutput[17];
    BYTE baIMEAAuthSK[17];

    //CIT_ClearDisplay();
    CTOS_BackLightSet(d_BKLIT_LCD, d_ON);
    //Initialize Communication
    
	vdDebug_LogPrintf("VW_InitializeCommunication");
    ulAPRtn = VW_InitializeCommunication(d_VW_MEK, 0x01, "\x01\x02\x03\x04\x05\x06\x07\x08", &stKeyData);
    if (ulAPRtn == d_NO_ERROR) {
        //Mutual Authentication
        memcpy(&baRN_Reader, &stKeyData.baRND_R[0], 8);
        //IMEK Authentication SK=3DES[IMEK, RND_R(5:8),RND_B(1:4),RND_R(1:4),RND_B(5:8)]
        memcpy(&baTmpRN[0], &baRN_Reader[4], 4);
        memcpy(&baTmpRN[4], "\x01\x02\x03\x04", 4);
        memcpy(&baTmpRN[8], &baRN_Reader[0], 4);
        memcpy(&baTmpRN[12], "\x05\x06\x07\x08", 4);

		vdDebug_LogPrintf("VW_DES d_ENCRYPTION");

        VW_DES(d_ENCRYPTION, 16, "\x00\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x00\x00\x00\x00\x00\x00\x00\x00", baTmpRN, baIMEAAuthSK);
        VW_DES(d_ENCRYPTION, 16, "\x00\xBB\xBB\xBB\xBB\xBB\xBB\xBB\x00\x00\x00\x00\x00\x00\x00\x00", baTmpRN + 8, baIMEAAuthSK + 8);

        //crypto2=3DES[AuthSK, RN_B(5:8),RN_R(1:4),RN_B(1:4),RND_R(5:8)]
        memcpy(&baTmpRN[0], "\x05\x06\x07\x08", 4);
        memcpy(&baTmpRN[4], &baRN_Reader[0], 4);
        memcpy(&baTmpRN[8], "\x01\x02\x03\x04", 4);
        memcpy(&baTmpRN[12], &baRN_Reader[4], 4);

		vdDebug_LogPrintf("VW_DES d_ENCRYPTION");

        VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN, baOutput);
        VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN + 8, baOutput + 8);

        stKeyData.bKeyType = d_VW_MEK;
        stKeyData.bKeyIndex = 0x01;
        memcpy(stKeyData.baEnRND, baOutput, 16);

		vdDebug_LogPrintf("VW_MutualAuthenticate");
        ulAPRtn = VW_MutualAuthenticate(&stKeyData);
        if (ulAPRtn == d_NO_ERROR) {
            //Generate Keys
            stKeyData.bKeyType = d_VW_Msession;
            stKeyData.bKeyIndex = 0x01;
            memcpy(&baTmpRN[0], "\x22\x22\x22\x22\x22\x22\x22\x22\x33\x33\x33\x33\x33\x33\x33\x33", 16);

			vdDebug_LogPrintf("VW_DES d_ENCRYPTION");
            VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN, baOutput);
            VW_DES(d_ENCRYPTION, 16, baIMEAAuthSK, baTmpRN + 8, baOutput + 8);
            memcpy(stKeyData.baEnRND, baOutput, 16);

			vdDebug_LogPrintf("VW_GenerateKeys");
            ulAPRtn = VW_GenerateKeys(&stKeyData);
            if (ulAPRtn != d_NO_ERROR)
                return ulAPRtn;
            else {

				vdDebug_LogPrintf("VW_SetSessionKey");
                ulAPRtn = VW_SetSessionKey(d_MsessionFlag, "\x22\x22\x22\x22\x22\x22\x22\x22\x33\x33\x33\x33\x33\x33\x33\x33");
                if (ulAPRtn != d_NO_ERROR)
                    return ulAPRtn;
            }
        } else
            return ulAPRtn;
    } else
        return ulAPRtn;

    return ulAPRtn;
}







/* ==========================================================================
 * Transaction Related Data include : the all parameter must ASCII string
 *	szAmount    Tag 9F02   (Amount Authorized(Numeric))
 *	szOtherAmt  Tag 9F03   (Amount Other(Numeric))
 *	szTransType Tag 9C     (Transaction Type)
 *	szCatgCode  Tag 9F53   (Transaction Category Code)
 *	szCurrCode  Tag 5F2A   (Transaction Currency Code)
 * ========================================================================== */
ULONG inCTOSS_CLMInitTransaction(BYTE *szAmount,BYTE *szOtherAmt,BYTE *szTransType,BYTE *szCatgCode,BYTE *szCurrCode)
{
	BYTE TagNum;
	BYTE TransaRelatedData[100];
	BYTE index;
	ULONG ulValue;
	BYTE temp[64];
	ULONG ulAPRtn;
	
	// perform transaction : EMVCL_InitTransactionEx + EMVCL_PollTransactionEx
	TagNum = 0;
	index = 0;
	memset(TransaRelatedData, 0, sizeof(TransaRelatedData));

	// V3 contactless reader
	//EMVCL_ShowContactlessSymbol(NULL);
	//Prepare Input Data
//	TransaRelatedData.bStart = d_EMVCL_ACT_DATA_START_A;
//	TransaRelatedData.bTagNum = 0;
//	TransaRelatedData.usTransactionDataLen = 0;

	vdDebug_LogPrintf("szAmount=[%s],szOtherAmt=[%s],szTransType=[%s],szCatgCode=[%s],szCurrCode=[%s]",szAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
	//Put 0x9F02 Amount, Authorized (Numeric)
	vdDebug_LogPrintf("strlen(szAmount)=[%d]",strlen(szAmount));
	if (strlen(szAmount) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szAmount,temp,12);
		
		TransaRelatedData[index++] = 0x9F;
		TransaRelatedData[index++] = 0x02;
		TransaRelatedData[index++] = 0x06;
		memcpy(&TransaRelatedData[index], temp, 6);
		index += 6;
	}
	//Put 0x9F03 Amount, Amount Other(Numeric)
	vdDebug_LogPrintf("strlen(szOtherAmt)=[%d]",strlen(szOtherAmt));
	if (strlen(szOtherAmt) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szOtherAmt,temp,12);
		
		TransaRelatedData[index++] = 0x9F;
		TransaRelatedData[index++] = 0x03;
		TransaRelatedData[index++] = 0x06;
		memcpy(&TransaRelatedData[index], temp, 6);
		index += 6;
	}
	//Put 9C
	vdDebug_LogPrintf("strlen(szTransType)=[%d]",strlen(szTransType));
	//if (strlen(szTransType) > 0)
	{
		TagNum ++;
		
		TransaRelatedData[index++] = 0x9C;
		TransaRelatedData[index++] = 0x01;
		TransaRelatedData[index++] = szTransType[0];
	}
	//Put 0x9F53 Transaction Category Code
	vdDebug_LogPrintf("strlen(szCatgCode)=[%d]",strlen(szCatgCode));
	if (strlen(szCatgCode) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szCatgCode,temp,4);
		
		TransaRelatedData[index++] = 0x9F;
		TransaRelatedData[index++] = 0x53;
		TransaRelatedData[index++] = 0x02;
		memcpy(&TransaRelatedData[index], temp, 2);
		index += 2;
	}
	//Put 0x5F2A Transaction Currency Code
	vdDebug_LogPrintf("strlen(szCurrCode)=[%d]",strlen(szCurrCode));
	if (strlen(szCurrCode) > 0)
	{
		TagNum ++;
		memset(temp, 0, sizeof(temp));
		wub_str_2_hex(szCurrCode,temp,4);
		
		TransaRelatedData[index++] = 0x5F;
		TransaRelatedData[index++] = 0x2A;
		TransaRelatedData[index++] = 0x02;
		memcpy(&TransaRelatedData[index], temp, 2);
		index += 2;
	}

	//Init a CL transaction : send out strat transction request  
	vdDebug_LogPrintf("TagNum=[%d],index=[%d]",TagNum,index);
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = EMVCL_InitTransactionEx(TagNum, TransaRelatedData, index);
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		ulAPRtn = MsessionAuthen();
        if (ulAPRtn != d_NO_ERROR) 
		{
			vdDisplayErrorMsg(1, 8, "Authen Fail");
			return d_NO;
        }

		vdDebug_LogPrintf("VW_InitTransactionEx");
		ulAPRtn = VW_InitTransactionEx(TagNum, TransaRelatedData, index);		
	}
	
	if(ulAPRtn != d_EMVCL_NO_ERROR)
	{
		return ulAPRtn;
	}
	
	return d_OK;
		
}

ULONG inCTOSS_CLMPollTransaction(EMVCL_RC_DATA_EX *stRCDataEx,ULONG ulMS)
{
	ULONG ulAPRtn;
	
	// Get Transaction Result
	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode()) 
	{
//		ulAPRtn = EMVCL_PerformTransactionEx(&stRCDataEx);
//		if(ulAPRtn != d_EMVCL_PENDING)
//			;
//		else
			ulAPRtn = EMVCL_PollTransactionEx(stRCDataEx, ulMS);
			vdDebug_LogPrintf("EMVCL_PollTransactionEx[%d][%x]", ulAPRtn,ulAPRtn);
	}
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		ulAPRtn = VW_PollTransactionEx((RC_DATA_EX *)stRCDataEx, ulMS);
		vdDebug_LogPrintf("VW_PollTransactionEx[%d][%x]", ulAPRtn,ulAPRtn);
	}
	
	return ulAPRtn;
}

ULONG inCTOSS_SetTimeOut(ULONG ulMS)
{
	ULONG ulAPRtn;
	
	ulAPRtn = EMVCL_SetTimeOut(ulMS);	
	return ulAPRtn;
}

ULONG inCTOSS_CLMSetDateTime(BYTE *baDateTime)
{
	ULONG ulAPRtn;
	
	ulAPRtn = EMVCL_SetDateTime(baDateTime);
	if(ulAPRtn != 0)
	{
		vdDebug_LogPrintf("SetDateTime error");
	}
	return ulAPRtn;
}

ULONG inCTOSS_CLMGetDateTime(BYTE *baDateTime)
{
	ULONG ulAPRtn;
	
	ulAPRtn = EMVCL_GetDateTime(baDateTime);
	if(ulAPRtn != 0)
	{
		vdDebug_LogPrintf("SetDateTime error");
	}
	return ulAPRtn;
}

ULONG inCTOSS_CLMCancelTransaction(void)
{
	ULONG ulAPRtn;

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode()) 
		ulAPRtn = EMVCL_CancelTransaction();
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
		ulAPRtn = VW_CancelTransaction();
	
	return ulAPRtn;
}

void vdCTOSS_CLMAnalyzeTransaction(EMVCL_RC_DATA_EX *stRCData, EMVCL_RC_DATA_ANALYZE *stRCDataAnalyze)
{
	RC_DATA_ANALYZE stVWRCDataAnalyze;

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode()) 
		EMVCL_AnalyzeTransactionEx(stRCData, stRCDataAnalyze);
	else if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		VW_AnalyzeTransactionEx((RC_DATA_EX *)stRCData, &stVWRCDataAnalyze);
		vdDebug_LogPrintf("VW_AnalyzeTransactionEx [%d][%d][%s]", stVWRCDataAnalyze.usTransResult, stVWRCDataAnalyze.bCVMAnalysis, stVWRCDataAnalyze.baCVMResults);
		memset(stRCDataAnalyze, 0x00, sizeof(EMVCL_RC_DATA_ANALYZE));
		stRCDataAnalyze->usTransResult = stVWRCDataAnalyze.usTransResult;
		stRCDataAnalyze->bCVMAnalysis = stVWRCDataAnalyze.bCVMAnalysis;
		memcpy(stRCDataAnalyze->baCVMResults, stVWRCDataAnalyze.baCVMResults, 3);
	}
}

void vdCTOSS_WaveGetEMVData(BYTE *baChipData, USHORT usChipDataLen)
{
	USHORT usLen = 64;
	BYTE szHexT57[100],szT57[200];        // Terminal Verification Results
	BYTE szHexT55[100],szT55[200];        // signature CVM
	BYTE szHexT99[100],szT99[200];        // Online PIN CVM 

	memset(szHexT57,0x00,sizeof(szHexT57));
	memset(szT57,0x00,sizeof(szT57));
	usCTOSS_FindTagFromDataPackage(TAG_57, szHexT57, &usLen, baChipData, usChipDataLen);
	if (usLen > 0)
	{
		wub_hex_2_str(szHexT57,szT57,usLen);
		shAnalysisTrack2(szT57, usLen*2);
		vdDebug_LogPrintf("usTrack2Len=[%d][%s]...	",srTransRec.usTrack2Len,srTransRec.szTrack2Data);
	}
	
	usCTOSS_FindTagFromDataPackage(TAG_9F02_AUTH_AMOUNT, srTransRec.stEMVinfo.T9F02, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F03_OTHER_AMOUNT, srTransRec.stEMVinfo.T9F03, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_82_AIP, srTransRec.stEMVinfo.T82, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_5F2A_TRANS_CURRENCY_CODE, srTransRec.stEMVinfo.T5F2A, &usLen, baChipData, usChipDataLen);
			
	usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9A_TRANS_DATE, srTransRec.stEMVinfo.T9A, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen,  baChipData, usChipDataLen);
		
	usCTOSS_FindTagFromDataPackage(TAG_9F06, srTransRec.stEMVinfo.T9F06, &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen,  baChipData, usChipDataLen);

	if (srTransRec.stEMVinfo.T9F10_len <= 0)
	{
		usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen,  baChipData, usChipDataLen);
		srTransRec.stEMVinfo.T9F10_len = usLen;
	}
	vdDebug_LogPrintf("T9F10_len =[%d]-----",srTransRec.stEMVinfo.T9F10_len);

	usCTOSS_FindTagFromDataPackage(TAG_9F1A_TERM_COUNTRY_CODE, srTransRec.stEMVinfo.T9F1A, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F1E, srTransRec.stEMVinfo.T9F1E, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen,  baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F33_TERM_CAB, srTransRec.stEMVinfo.T9F33, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, baChipData, usChipDataLen);

	usCTOSS_FindTagFromDataPackage(TAG_9F35_TERM_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9F35), &usLen,  baChipData, usChipDataLen);

	if (srTransRec.stEMVinfo.T9F36_len <= 0)
	{
		usCTOSS_FindTagFromDataPackage(TAG_9F36_ATC, srTransRec.stEMVinfo.T9F36, &usLen,  baChipData, usChipDataLen);
		srTransRec.stEMVinfo.T9F36_len = usLen;
	}

	usCTOSS_FindTagFromDataPackage(TAG_9F37_UNPREDICT_NUM, srTransRec.stEMVinfo.T9F37, &usLen,  baChipData, usChipDataLen);
	
    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, baChipData, usChipDataLen);
            
    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, baChipData, usChipDataLen);
    
    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, baChipData, usChipDataLen);
        
    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, baChipData, usChipDataLen);
// patrick fix print application label
	//usCTOSS_FindTagFromDataPackage(TAG_9F12, srTransRec.stEMVinfo.szChipLabel, &usLen, baChipData, usChipDataLen);	
	usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, baChipData, usChipDataLen);
	vdDebug_LogPrintf("-------TAG_50 usLen[%d] [%s]--",usLen, srTransRec.stEMVinfo.szChipLabel);
// patrick fix print AID
	vdDebug_LogPrintf("T84_len =[%d]-----",srTransRec.stEMVinfo.T84_len);
	if (srTransRec.stEMVinfo.T84_len <= 0)
	{
		usCTOSS_FindTagFromDataPackage(TAG_84_DF_NAME, srTransRec.stEMVinfo.T84, &usLen, baChipData, usChipDataLen);
		srTransRec.stEMVinfo.T84_len = usLen;
		DebugAddHEX("Save TAG_84_DF_NAME", srTransRec.stEMVinfo.T84, usLen);
	}
// patrick fix not signature
	usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, baChipData, usChipDataLen);
	vdDebug_LogPrintf("TAG_9F34_CVM usLen =[%d]-----",usLen);

	if (usLen > 0)
	{	
		if (memcmp(srTransRec.stEMVinfo.T9F34, "\x00\x00\x00", 3) == 0)
			memcpy(srTransRec.stEMVinfo.T9F34, "\x00\x00\x02", 3);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////	
    BYTE        byVal[64];
    unsigned char szTransSeqCounter[6+1];
	unsigned char szHEXTransSeqCounter[3+1];

    memset(byVal, 0x00, sizeof(byVal));
    usCTOSS_FindTagFromDataPackage(TAG_5F34_PAN_IDENTFY_NO, byVal, &usLen, baChipData, usChipDataLen);
    srTransRec.stEMVinfo.T5F34_len = usLen;
    srTransRec.stEMVinfo.T5F34 = byVal[0];

    inIITRead(srTransRec.IITid);
    sprintf(szTransSeqCounter, "%06ld", strIIT.ulTransSeqCounter);
    wub_str_2_hex(szTransSeqCounter, (char *)szHEXTransSeqCounter, 6);
    memcpy(srTransRec.stEMVinfo.T9F41,szHEXTransSeqCounter,3);
    strIIT.ulTransSeqCounter++;
    inIITSave(srTransRec.IITid);    
    vdDebug_LogPrintf("srTransRec.IITid[%d]strIIT.ulTransSeqCounter[%ld]",srTransRec.IITid,strIIT.ulTransSeqCounter);

	usCTOSS_FindTagFromDataPackage(TAG_55, szHexT55, &usLen, baChipData, usChipDataLen);
	if (usLen > 0)
	{
		memcpy(srTransRec.stEMVinfo.T9F34, "\x5E\x03\x00", 3);
	}

	usCTOSS_FindTagFromDataPackage(TAG_99, szHexT99, &usLen, baChipData, usChipDataLen);
	if (usLen > 0)
	{
		memcpy(srTransRec.stEMVinfo.T9F34, "\x02\x02\x00", 3);
	}

	vdDebug_LogPrintf("T9F34 =[%d]-----",srTransRec.stEMVinfo.T9F34);


	usCTOSS_FindTagFromDataPackage(TAG_9F63, srTransRec.stEMVinfo.T9F63, &usLen,  baChipData, usChipDataLen);
	srTransRec.stEMVinfo.T9F63_len = usLen;
	vdDebug_LogPrintf("T9F63 =[%d]-----",srTransRec.stEMVinfo.T9F63);

#ifdef JCB_LEGACY_FEATURE
	usCTOSS_FindTagFromDataPackage(TAG_9F6E, srTransRec.stEMVinfo.T9F6E, &usLen,  baChipData, usChipDataLen);
	srTransRec.stEMVinfo.T9F6E_len = usLen;
	vdDebug_LogPrintf("T9F6E =[%d]-----",srTransRec.stEMVinfo.T9F6E);

	usCTOSS_FindTagFromDataPackage(TAG_9F7C, srTransRec.stEMVinfo.T9F7C, &usLen,  baChipData, usChipDataLen);
	srTransRec.stEMVinfo.T9F7C_len = usLen;
	vdDebug_LogPrintf("T9F7C =[%d]-----",srTransRec.stEMVinfo.T9F7C);
#endif	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

void vdCTOSS_WaveGetCardLable(BYTE SID)
{
	char msg[50+1];
	int len;
	memset(msg,0x00,sizeof(msg));
	switch(SID) 
	{
		case d_VW_SID_PAYPASS_MAG_STRIPE:
			strcpy(msg,"PayPass M-Stripe");
			break;
		case d_VW_SID_PAYPASS_MCHIP:
            strcpy(msg," PayPass MChip  ");
			break;
		case d_VW_SID_VISA_OLD_US:
            strcpy(msg," VISA Old US MSD");
			break;
		case d_VW_SID_VISA_WAVE_MSD:
            strcpy(msg," VisaWave MSD   ");
			break;
		case d_VW_SID_VISA_WAVE_2:
            strcpy(msg," VisaWave 2     ");
			break;
		case d_VW_SID_VISA_WAVE_QVSDC:
            strcpy(msg," VisaWave qVSDC ");
			break;
		case d_VW_SID_AE_EMV:
            strcpy(msg," ExpressPay EMV ");
			break;
		case d_VW_SID_AE_MAG_STRIPE:
            strcpy(msg," AE M-Stripe    ");
			break;
		case d_VW_SID_JCB_WAVE_2:
            strcpy(msg," J/Speedy Wave 2");
			break;
		case d_VW_SID_JCB_WAVE_QVSDC:
            strcpy(msg," J/Speedy qVSDC ");
			break;
		case d_VW_SID_DISCOVER:
            strcpy(msg," Zip            ");
			break;
		default:
            strcpy(msg,"CardType No Def.");
			break;
	}

	len = strlen(msg);
	if (len > 20)
		len = 20;
	
	strncpy(srTransRec.szCardLable, msg,len);
	
}

int inCTOSS_WaveAnalyzeTransaction(EMVCL_RC_DATA_EX *stRCDataEx)
{
	ULONG ulAPRtn;
	BYTE cVer[128];
	int inRet = d_NO;
	USHORT usLen = 64;

//	TLVDataParse(stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
//	TLVDataParse(stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);
	vdDebug_LogPrintf("inCTOSS_WaveAnalyzeTransaction");

	DebugAddHEX("SCDataEx DateTime", stRCDataEx->baDateTime, sizeof(stRCDataEx->baDateTime));
	DebugAddHEX("SCDataEx Track1", stRCDataEx->baTrack1Data, stRCDataEx->bTrack1Len);
	DebugAddHEX("SCDataEx Track2", stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);

	//3B3337373637383830303031313934303D3139303537303231343035393637303230303030303F
	//5413330089601075D14122010123409172
	vdDebug_LogPrintf("!SCDataEx Track2=[%s][%ld]", stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);
	
	// patrick fix code 20140826
	/*
	if (stRCDataEx->baTrack2Data[0] == ';')
	{
		wub_str_2_hex(&stRCDataEx->baTrack2Data[1], stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);
		stRCDataEx->bTrack2Len = (stRCDataEx->bTrack2Len / 2) -1;	
		
		DebugAddHEX("SCDataEx Track2", stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);
	}
	*/
	DebugAddHEX("SCDataEx Chip", stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
	DebugAddHEX("SCDataEx Additional", stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);
	
	vdCTOSS_CLMAnalyzeTransaction(stRCDataEx, &stRCDataAnalyze);

	vdDebug_LogPrintf("inCTOSS_WaveAnalyzeTransaction-usTransResult[%d]", stRCDataAnalyze.usTransResult);


	if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_SIGNATURE)
	{
		vdDebug_LogPrintf("CVM->Signature REQUIRED");
		//CTOS_PrinterPutString("*****CVM->Signature REQUIRED*****");		
		//add signature funcation
	} 
	else if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_ONLPIN)
	{
		vdDebug_LogPrintf("CVM->ONLPIN REQUIRED");
		//CTOS_PrinterPutString("*****CVM->ONLPIN REQUIRED*****");		
		//add online pin funcation
	} 
	else if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_NOCVM)
	{
		vdDebug_LogPrintf("No CVM REQUIRED");
		//CTOS_PrinterPutString("*****No CVM REQUIRED*****");		
	}

	
	//Online
	if(stRCDataAnalyze.usTransResult == d_EMV_PAYPASS_MSD_ONLINE || stRCDataAnalyze.usTransResult == d_EMV_CHIP_ONL || stRCDataAnalyze.usTransResult == d_EMV_VISA_MSD_ONLINE) 
	{
		vdDebug_LogPrintf("Go Online...  ");
		srTransRec.byUploaded = CN_TRUE;
        srTransRec.byOffline = CN_FALSE;
		inRet = d_OK;
	}
	//Offline Approval
	else if(stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_APPROVAL) 
	{
		vdDebug_LogPrintf("Off Approval...  ");
		#ifdef EMV_Y1_DISABLE
		srTransRec.byUploaded = CN_TRUE;
        srTransRec.byOffline = CN_FALSE;
    	vdCTOS_SetTransType(SALE);						
		#else
//		ulAPRtn = EMVCL_SetLED(0x0F, 0x02); // patrick this must remove because QP3000S will happen system crash 20150717
		srTransRec.byUploaded = CN_FALSE;
        srTransRec.byOffline = CN_TRUE;
    	vdCTOS_SetTransType(SALE_OFFLINE);		
		strcpy(srTransRec.szAuthCode, "Y1");	
		srTransRec.shTransResult = TRANS_AUTHORIZED;		
		#endif
		inRet = d_OK;
	}
	//Offline Declined
	else if(stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_DECLINED) 
	{
		vdDebug_LogPrintf("Off Declined...  ");
		vdDisplayErrorMsg(1, 8, "Offline Declined. ");
//		ulAPRtn = EMVCL_SetLED(0x0F, 0x01); // patrick this must remove because QP3000S will happen system crash 20150717
		inRet = d_NO;
//		inRet = d_OK;
	}
	else 
	{
		vdDebug_LogPrintf("Unknow Result...  ");
		vdDisplayErrorMsg(1, 8, "Unknow Result...  ");
		inRet = d_NO;
//		inRet = d_OK;        
	}

	if (inRet == d_OK)
	{
//		EMVCL_SetLED(0x0F, 0x02); // patrick this must remove because QP3000S will happen system crash 20150717
		
		vdCTOSS_WaveGetCardLable(stRCDataEx->bSID);
		srTransRec.bWaveSID = stRCDataEx->bSID;
		srTransRec.usWaveSTransResult = stRCDataAnalyze.usTransResult;
		srTransRec.bWaveSCVMAnalysis = stRCDataAnalyze.bCVMAnalysis;
		vdDebug_LogPrintf("bWaveSID=[%x]",srTransRec.bWaveSID);
	
		vdDebug_LogPrintf("baDateTime=[%s]",stRCDataEx->baDateTime);
		memcpy(srTransRec.szYear,&stRCDataEx->baDateTime[2],2);
		wub_str_2_hex(&stRCDataEx->baDateTime[4],srTransRec.szDate,4);
		wub_str_2_hex(&stRCDataEx->baDateTime[8],srTransRec.szTime,6);
		DebugAddHEX("srTransRec.szDate", srTransRec.szDate, 4);
		DebugAddHEX("srTransRec.szTime", srTransRec.szTime, 6);

		srTransRec.usTrack1Len = stRCDataEx->bTrack1Len;
		memcpy(srTransRec.szTrack1Data, stRCDataEx->baTrack1Data, stRCDataEx->bTrack1Len);
		vdDebug_LogPrintf("usTrack1Len=[%d][%s]...  ",srTransRec.usTrack1Len,srTransRec.szTrack1Data);
		if (srTransRec.usTrack1Len > 0)
			vdGetCardHolderName(srTransRec.szTrack1Data, srTransRec.usTrack1Len);

		// patrick fix code 20140826
		if (stRCDataEx->baTrack2Data[0] != ';')
		{
			srTransRec.usTrack2Len = (stRCDataEx->bTrack2Len * 2);
			wub_hex_2_str(stRCDataEx->baTrack2Data,srTransRec.szTrack2Data,stRCDataEx->bTrack2Len);
		}
		else
		{
			memcpy(srTransRec.szTrack2Data, &stRCDataEx->baTrack2Data[1], stRCDataEx->bTrack2Len);
			srTransRec.usTrack2Len = stRCDataEx->bTrack2Len - 1;
		}
		
		vdDebug_LogPrintf("usTrack2Len=[%d][%s]...  ",srTransRec.usTrack2Len,srTransRec.szTrack2Data);
		if (srTransRec.usTrack2Len > 0)
		{
			shAnalysisTrack2(srTransRec.szTrack2Data, srTransRec.usTrack2Len);
			vdDebug_LogPrintf("usTrack2Len=[%d][%s]...	",srTransRec.usTrack2Len,srTransRec.szTrack2Data);

			if (strTCT.fPrintISOMessage == VS_TRUE){
				inPrintISOPacket(2 , srTransRec.szTrack2Data, srTransRec.usTrack2Len);
			}						
		}
		vdDebug_LogPrintf("szPAN=[%s]",srTransRec.szPAN);
		
		vdDebug_LogPrintf("usChipDataLen=[%d][%s]...  ",stRCDataEx->usChipDataLen,stRCDataEx->baChipData);
		if (stRCDataEx->usChipDataLen > 0)
		{			
			srTransRec.usChipDataLen = stRCDataEx->usChipDataLen;
			memcpy(srTransRec.baChipData, stRCDataEx->baChipData, stRCDataEx->usChipDataLen); 
			vdCTOSS_WaveGetEMVData(stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
	
		
			if (strTCT.fPrintISOMessage == VS_TRUE){
				inPrintISOPacket(3 , stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
			}			
		}
		
		vdDebug_LogPrintf("baAdditionalData=[%d][%s]...  ",stRCDataEx->usAdditionalDataLen,stRCDataEx->baAdditionalData);
		if (stRCDataEx->usAdditionalDataLen > 0)
		{			
			srTransRec.usAdditionalDataLen = stRCDataEx->usAdditionalDataLen;
			memcpy(srTransRec.baAdditionalData, stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen); 
			vdCTOSS_WaveGetEMVData(stRCDataEx->baAdditionalData, (stRCDataEx->usAdditionalDataLen));
			
		
			if (strTCT.fPrintISOMessage == VS_TRUE){
				inPrintISOPacket(4 , stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);
			}			
		}
	}

	return 	inRet;
}


void vdCTOSS_WaveCheckRtCode(ULONG ulAPRtn)
{
	//The returned code other than 0xA0000001, the transaction is terminated.
	vdDebug_LogPrintf("ulAPRtn=[%x]---------",ulAPRtn);
	if(ulAPRtn == d_EMVCL_RC_MORE_CARDS)
	{
		CTOS_LCDTPrintXY(1, 7, "Please Select 1 ");
		vdDisplayErrorMsg(1, 8, "Card            ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_DDA_AUTH_FAILURE)
	{
		CTOS_LCDTPrintXY(1, 7, "Please Use Other");
		vdDisplayErrorMsg(1, 8, "Card            ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_US_CARDS)
	{
		CTOS_LCDTPrintXY(1, 7, "US Card...      ");
		vdDisplayErrorMsg(1, 8, "Please Swipe    ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_CARD_DIFF_FROM_OTHER_AP_COUNTRY)
	{
		CTOS_LCDTPrintXY(1, 7, "Oversea Card... ");
		vdDisplayErrorMsg(1, 8, "Please Insert   ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_FAILURE)
	{
		CTOS_LCDTPrintXY(1, 7, "Terminated, Pls ");
		vdDisplayErrorMsg(1, 8, "Use Other Card  ");
	} 
	else if(ulAPRtn == d_EMVCL_RC_EXCEED_CL_TX_LIMIT)
	{
		vdDisplayErrorMsg(1, 8, "Insert Card     ");
	}
	else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
	{
		vdDisplayErrorMsg(1, 8, "Timeout");
	}
	else
	{
		vdDisplayErrorMsg(1, 8, "Please Try Again");
	}
}

ULONG inCTOSS_CLMOpenAndGetVersion(void)
{
	ULONG ulAPRtn = d_OK;
	BYTE cVer[128];
	
	if (NO_CTLS == inCTOSS_GetCtlsMode())
	{
		if (1 == chGetIdleEventSC_MSR())
			return d_OK;
		
		//vdDisplayErrorMsg(1, 8, "CTLS NOT ALLOWED");
		//return d_NO;
	}

	if (CTLS_V3_SHARECTLS == inCTOSS_GetCtlsMode()
		|| CTLS_V3_INT_SHARECTLS == inCTOSS_GetCtlsMode())
		return d_OK;

	if (CTLS_INTERNAL == inCTOSS_GetCtlsMode())
	{
		ulAPRtn = inCTOSS_CLMOpen();
		if(d_EMVCL_NO_ERROR == ulAPRtn) 
		{
			memset(cVer,0x00,sizeof(cVer));
			vdCTOSS_GetCLMVersion(cVer);
		}

		//if (8 ==strTCT.byRS232DebugPort)
		//	EMVCL_SetDebug(1,d_DEBUG_PORT_USB);
	}

	if (CTLS_EXTERNAL == inCTOSS_GetCtlsMode())
	{
		if (1 ==strTCT.byQP3000SPort)
			ulAPRtn = VW_InitComm(d_VW_COMM_COM1, d_BAUDRATE);
		else if (2 ==strTCT.byQP3000SPort)
			ulAPRtn = VW_InitComm(d_VW_COMM_COM2, d_BAUDRATE);
		else if (3 ==strTCT.byQP3000SPort)
			ulAPRtn = VW_InitComm(d_VW_COMM_COM3, d_BAUDRATE);
		else
			ulAPRtn = VW_InitComm(d_VW_COMM_COM2, d_BAUDRATE);

		if(d_EMVCL_NO_ERROR == ulAPRtn) 
		{
			memset(cVer,0x00,sizeof(cVer));
			VW_GetAPIVersion(cVer);
		}
	}
	vdDebug_LogPrintf("ulAPRtn=[%d],cVer=[%s]",ulAPRtn,cVer);
	return ulAPRtn;
}


#if 0
USHORT usCTOSS_CtlsV3Trans(BYTE *szAmount,BYTE *szOtherAmt,BYTE *szTransType,BYTE *szCatgCode,BYTE *szCurrCode,EMVCL_RC_DATA_EX *stRCDataEx)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	char szAPName[25];
	int inAPPID;
	CTLS_TransData strCTLSTransData;
	unsigned char usStatus;

	inMultiAP_Database_CTLS_Delete();

	memset(&strCTLSTransData,0x00,sizeof(CTLS_TransData));
	strcpy(strCTLSTransData.szAmount,szAmount);
	strcpy(strCTLSTransData.szOtherAmt,szOtherAmt);
	strcpy(strCTLSTransData.szTransType,szTransType);
	strcpy(strCTLSTransData.szCatgCode,szCatgCode);
	strcpy(strCTLSTransData.szCurrCode,szCurrCode);

//	vdDebug_LogPrintf("-----szAmount[%s]-----",strCTLSTransData.szAmount);
//	vdDebug_LogPrintf("-----szOtherAmt[%s]-----",strCTLSTransData.szOtherAmt);
	vdDebug_LogPrintf("-----szTransType[%s]-----",strCTLSTransData.szTransType);
//	vdDebug_LogPrintf("-----szCatgCode[%s]-----",strCTLSTransData.szCatgCode);
//	vdDebug_LogPrintf("-----szCurrCode=[%s]-----",strCTLSTransData.szCurrCode);
	inMultiAP_Database_CTLS_Insert(&strCTLSTransData);

	vdDebug_LogPrintf("**usCTOSS_CtlsV3Trans**");
    usResult = inMultiAP_RunIPCCmdTypes("SHARLS_CTLS", d_IPC_CMD_CTLS_TRANS, bInBuf, usInLen, bOutBuf, &usOutLen);
    vdDebug_LogPrintf("**usCTOSS_CtlsV3Trans [%d]**", usResult);
    if(d_OK == usResult)
    {
    	usStatus = (unsigned char)bOutBuf[1];
    	vdDebug_LogPrintf("usCTOSS_CtlsV3Trans[%x][%c][%d] ",usStatus,usStatus,usStatus);
		if (usStatus == d_SUCCESS)
		{
			usResult = d_OK;
			inMultiAP_Database_CTLS_Read(&strCTLSTransData);
			inMultiAP_Database_CTLS_Delete();

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
			stRCDataEx->bSID = strCTLSTransData.bSID;
			strcpy(stRCDataEx->baDateTime,strCTLSTransData.baDateTime);
			//pack track1
			if (strCTLSTransData.bTrack1Len > 0)
			{
				stRCDataEx->bTrack1Len = strCTLSTransData.bTrack1Len;
				memcpy(stRCDataEx->baTrack1Data,strCTLSTransData.baTrack1Data,strCTLSTransData.bTrack1Len);
			}
			//pack track2
			if (strCTLSTransData.bTrack2Len > 0)
			{
				stRCDataEx->bTrack2Len = strCTLSTransData.bTrack2Len;
				memcpy(stRCDataEx->baTrack2Data,strCTLSTransData.baTrack2Data,strCTLSTransData.bTrack2Len);

			}
			//pack ChipData
			if (strCTLSTransData.usChipDataLen > 0)
			{
				stRCDataEx->usChipDataLen = strCTLSTransData.usChipDataLen;
				memcpy(stRCDataEx->baChipData,strCTLSTransData.baChipData,strCTLSTransData.usChipDataLen);

			}
			//pack AdditionalData
			if (strCTLSTransData.usAdditionalDataLen > 0)
			{
				stRCDataEx->usAdditionalDataLen = strCTLSTransData.usAdditionalDataLen;
				memcpy(stRCDataEx->baAdditionalData,strCTLSTransData.baAdditionalData,strCTLSTransData.usAdditionalDataLen);
			}

			stRCDataAnalyze.usTransResult = strCTLSTransData.usTransResult;
			stRCDataAnalyze.bCVMAnalysis = strCTLSTransData.bCVMAnalysis;
			memcpy(stRCDataAnalyze.baCVMResults,strCTLSTransData.baCVMResults, sizeof(strCTLSTransData.baCVMResults));
			//strCTLSTransData.bVisaAOSAPresent = stRCDataAnalyze.bVisaAOSAPresent;
			//memcpy(strCTLSTransData.baVisaAOSA,stRCDataAnalyze.baVisaAOSA, sizeof(stRCDataAnalyze.baVisaAOSA));
			//strCTLSTransData.bVisaAOSAPresent = stRCDataAnalyze.bODAFail;
			
		}
		else if (usStatus == d_CTLS_EVENT)
			usResult = d_CTLS_EVENT;
		else
			usResult = d_NO;	

    }  
    return usResult;
}
#endif

USHORT usCTOSS_CtlsV3Trans(BYTE *szAmount,BYTE *szOtherAmt,BYTE *szTransType,BYTE *szCatgCode,BYTE *szCurrCode,EMVCL_RC_DATA_EX *stRCDataEx)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	char szAPName[25];
	int inAPPID;
	CTLS_Trans strCTLSTrans;
	CTLS_TransData strCTLSTransData;
	unsigned char usStatus;
	BYTE TransaRelatedData[100];

	USHORT usNonEMV = d_OK;

	vdDebug_LogPrintf("usCTOSS_CtlsV3Trans");
	

	inMultiAP_Database_CTLS_Delete();

	memset(&strCTLSTransData,0x00,sizeof(CTLS_TransData));
	strcpy(strCTLSTransData.szAmount,szAmount);
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
//end for HLB MCCS CONTACTLESS
//	vdDebug_LogPrintf("-----szAmount[%s]-----",strCTLSTransData.szAmount);
//	vdDebug_LogPrintf("-----szOtherAmt[%s]-----",strCTLSTransData.szOtherAmt);
	vdDebug_LogPrintf("-----szTransType[%s]-----",strCTLSTransData.szTransType);
//	vdDebug_LogPrintf("-----szCatgCode[%s]-----",strCTLSTransData.szCatgCode);
//	vdDebug_LogPrintf("-----szCurrCode=[%s]-----",strCTLSTransData.szCurrCode);
	inMultiAP_Database_CTLS_Insert(&strCTLSTransData);

	vdDebug_LogPrintf("**usCTOSS_CtlsV3Trans**byPinPadMode=[%d],byPinPadType=[%d],byPinPadPort=[%d],GetCtlsMode=[%d]",strTCT.byPinPadMode,strTCT.byPinPadType,strTCT.byPinPadPort,inCTOSS_GetCtlsMode());

	if(strTCT.byPinPadMode == 0)
	{
		if(strTCT.byPinPadType == 3 && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
		{
			char szV3PSendBuf[1024+1];
			int inOffSet = 0;
			int status;
			char szRecvBuf[3072+1];
			int inRecvlen = 0;
			unsigned char *pszPtr;
			BYTE    szTotalAmt[12+1];
    		BYTE    szStr[45];

			vdDispTransTitle(srTransRec.byTransType);
		    CTOS_LCDTPrintXY(1, 5, "TOTAL AMOUNT :");
		    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
		    memset(szStr, 0x00, sizeof(szStr));
			//format amount 10+2
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
		    setLCDPrint(6, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
			CTOS_LCDTPrintAligned(6,szStr,d_LCD_ALIGNRIGHT);
			
			vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
			inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

			// send STX INJECT_KEY ETX LRC
			memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
			inOffSet = 0;
			szV3PSendBuf[inOffSet] = STX;
			inOffSet += 1;
			memcpy(&szV3PSendBuf[inOffSet], "CTLS_TRANS", strlen("CTLS_TRANS"));
			inOffSet += strlen("CTLS_TRANS");
			//////////////////////////////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = '|';
			inOffSet += 1;

			memset(&strCTLSTrans, 0x00, sizeof(CTLS_Trans));
			strcpy(strCTLSTrans.szAmount, szAmount);
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
		else
		{
			vdDebug_LogPrintf("**usCTOSS_CtlsV3Trans** AAA");
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_CTLS", d_IPC_CMD_CTLS_TRANS, bInBuf, usInLen, bOutBuf, &usOutLen);
			vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
			if (d_OK == usResult)
			{
			/*handle if check Non EMV in share EMV*/
			if (memcmp(&bOutBuf[3], SHARE_CTLS_NONEMVCARD, 1) == 0)
			{
				usNonEMV = usCTOSS_NonEMVCLCardProcess();
				vdDebug_LogPrintf("usNonEMV=[%d]",usNonEMV);
				
				usInLen = 0;
    			memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_CTLS_SUB_IPC);
				strcat(bInBuf,SHARE_CTLS_NONEMVCARD);
				usInLen = strlen(bInBuf);
				if (d_OK == usNonEMV)
					memcpy(&bInBuf[usInLen], "\x00",sizeof(usResult));
				else
					memcpy(&bInBuf[usInLen], "\x01",sizeof(usResult));
				
				usInLen += 1;
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

				memset(bOutBuf, 0x00, sizeof(bOutBuf));    
				usResult = inMultiAP_RunIPCCmdTypes("SHARLS_CTLS", d_IPC_CMD_CTLS_TRANS, bInBuf, usInLen, bOutBuf, &usOutLen);

				/*if Non EMV CTLS card, no need process next. 
				need handle the the follow here, to avoid the error display*/
				if(d_OK == usNonEMV)
					return d_OK;
			}
			}
		}
	}
	else
	{

		vdDebug_LogPrintf("**usCTOSS_CtlsV3Trans** AAA2");
	
		usResult = inMultiAP_RunIPCCmdTypes("SHARLS_CTLS", d_IPC_CMD_CTLS_TRANS, bInBuf, usInLen, bOutBuf, &usOutLen);
		vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);

		if (d_OK == usResult)
		{
			/*handle if check Non EMV in share EMV*/
			if (memcmp(&bOutBuf[3], SHARE_CTLS_NONEMVCARD, 1) == 0)
			{
				
				usNonEMV = usCTOSS_NonEMVCLCardProcess();
				vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
				
				usInLen = 0;
    			memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_CTLS_SUB_IPC);
				strcat(bInBuf,SHARE_CTLS_NONEMVCARD);
				usInLen = strlen(bInBuf);
				if (d_OK == usNonEMV)
					memcpy(&bInBuf[usInLen], "\x00",sizeof(usResult));
				else
					memcpy(&bInBuf[usInLen], "\x01",sizeof(usResult));
				
				usInLen += 1;
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

				memset(bOutBuf, 0x00, sizeof(bOutBuf));    
				usResult = inMultiAP_RunIPCCmdTypes("SHARLS_CTLS", d_IPC_CMD_CTLS_TRANS, bInBuf, usInLen, bOutBuf, &usOutLen);

				/*if Non EMV CTLS card, no need process next. 
				need handle the the follow here, to avoid the error display*/
				if(d_OK == usNonEMV){
					vdDebug_LogPrintf("**usCTOSS_CtlsV3Trans** BBB");
					
					return d_OK;
				}
			}
		}

	}
	
    vdDebug_LogPrintf("**usCTOSS_CtlsV3Trans CCC [%d]**", usResult);
    if(d_OK == usResult)
    {
    	usStatus = (unsigned char)bOutBuf[1];
		if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3) && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
			usStatus = d_SUCCESS;

		
    	vdDebug_LogPrintf("usCTOSS_CtlsV3Trans[%x][%c][%d] ",usStatus,usStatus,usStatus);
		if (usStatus == d_SUCCESS)
		{
			usResult = d_OK;
			if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3) && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
			{
				vdDebug_LogPrintf("status [%d]**", strCTLSTransData.status);
			}
			else
			{
				inMultiAP_Database_CTLS_Read(&strCTLSTransData);
				inMultiAP_Database_CTLS_Delete();
			}

			
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
			stRCDataEx->bSID = strCTLSTransData.bSID;
			strcpy(stRCDataEx->baDateTime,strCTLSTransData.baDateTime);
			//pack track1
			if (strCTLSTransData.bTrack1Len > 0)
			{
				stRCDataEx->bTrack1Len = strCTLSTransData.bTrack1Len;
				memcpy(stRCDataEx->baTrack1Data,strCTLSTransData.baTrack1Data,strCTLSTransData.bTrack1Len);
			}
			//pack track2
			if (strCTLSTransData.bTrack2Len > 0)
			{
				stRCDataEx->bTrack2Len = strCTLSTransData.bTrack2Len;
				memcpy(stRCDataEx->baTrack2Data,strCTLSTransData.baTrack2Data,strCTLSTransData.bTrack2Len);

			}
			//pack ChipData
			if (strCTLSTransData.usChipDataLen > 0)
			{
				stRCDataEx->usChipDataLen = strCTLSTransData.usChipDataLen;
				memcpy(stRCDataEx->baChipData,strCTLSTransData.baChipData,strCTLSTransData.usChipDataLen);

			}
			//pack AdditionalData
			if (strCTLSTransData.usAdditionalDataLen > 0)
			{
				stRCDataEx->usAdditionalDataLen = strCTLSTransData.usAdditionalDataLen;
				memcpy(stRCDataEx->baAdditionalData,strCTLSTransData.baAdditionalData,strCTLSTransData.usAdditionalDataLen);
			}

			stRCDataAnalyze.usTransResult = strCTLSTransData.usTransResult;
			stRCDataAnalyze.bCVMAnalysis = strCTLSTransData.bCVMAnalysis;
			memcpy(stRCDataAnalyze.baCVMResults,strCTLSTransData.baCVMResults, sizeof(strCTLSTransData.baCVMResults));
			//strCTLSTransData.bVisaAOSAPresent = stRCDataAnalyze.bVisaAOSAPresent;
			//memcpy(strCTLSTransData.baVisaAOSA,stRCDataAnalyze.baVisaAOSA, sizeof(stRCDataAnalyze.baVisaAOSA));
			//strCTLSTransData.bVisaAOSAPresent = stRCDataAnalyze.bODAFail;
			
		}
		else if (usStatus == d_CTLS_EVENT)
			usResult = d_CTLS_EVENT;
                else if(usStatus == 155)
                    usResult = 155;
		else
			usResult = d_NO;	

    }  

	vdDebug_LogPrintf("usCTOSS_CtlsV3Trans-end-[%x][%c][%d]",usResult,usResult,usResult);
	
    return usResult;
}


int inCTOSS_V3AnalyzeTransaction(EMVCL_RC_DATA_EX *stRCDataEx)
{
	ULONG ulAPRtn;
	BYTE cVer[128];
	int inRet = d_NO;
	USHORT usLen = 64;
    int inParkingFee=0;
	
//	TLVDataParse(stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
//	TLVDataParse(stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);

	vdDebug_LogPrintf("inCTOSS_V3AnalyzeTransaction");

	DebugAddHEX("SCDataEx DateTime", stRCDataEx->baDateTime, sizeof(stRCDataEx->baDateTime));
	DebugAddHEX("SCDataEx Track1", stRCDataEx->baTrack1Data, stRCDataEx->bTrack1Len);
	DebugAddHEX("SCDataEx Track2", stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);

	//3B3337373637383830303031313934303D3139303537303231343035393637303230303030303F
	//5413330089601075D14122010123409172
	vdDebug_LogPrintf("!SCDataEx Track2=[%s][%ld]", stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);
	
	// patrick fix code 20140826
	/*
	if (stRCDataEx->baTrack2Data[0] == ';')
	{
		wub_str_2_hex(&stRCDataEx->baTrack2Data[1], stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);
		stRCDataEx->bTrack2Len = (stRCDataEx->bTrack2Len / 2) -1;	
		
		DebugAddHEX("SCDataEx Track2", stRCDataEx->baTrack2Data, stRCDataEx->bTrack2Len);
	}
	*/
	DebugAddHEX("SCDataEx Chip", stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
	DebugAddHEX("SCDataEx Additional", stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);
	
	//vdCTOSS_CLMAnalyzeTransaction(stRCDataEx, &stRCDataAnalyze);

	if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_SIGNATURE)
	{
		vdDebug_LogPrintf("CVM->Signature REQUIRED");
		//CTOS_PrinterPutString("*****CVM->Signature REQUIRED*****");		
		//add signature funcation
	} 
	else if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_ONLPIN)
	{
		vdDebug_LogPrintf("CVM->ONLPIN REQUIRED");
		//CTOS_PrinterPutString("*****CVM->ONLPIN REQUIRED*****");		
		//add online pin funcation
	} 
	else if(stRCDataAnalyze.bCVMAnalysis == d_CVM_REQUIRED_NOCVM)
	{
		vdDebug_LogPrintf("No CVM REQUIRED");
		//CTOS_PrinterPutString("*****No CVM REQUIRED*****");		
	}

	
	//Online
	if(stRCDataAnalyze.usTransResult == d_EMV_PAYPASS_MSD_ONLINE || stRCDataAnalyze.usTransResult == d_EMV_CHIP_ONL || stRCDataAnalyze.usTransResult == d_EMV_VISA_MSD_ONLINE) 
	{
		vdDebug_LogPrintf("Go Online...  ");
		srTransRec.byUploaded = CN_TRUE;
        srTransRec.byOffline = CN_FALSE;
		inRet = d_OK;

        vdDebug_LogPrintf("check parking fee");
		
        #ifdef PARKING_FEE
        inParkingFee = get_env_int("PARKFEE");
		vdDebug_LogPrintf("inParkingFee:%d", inParkingFee);
        if(inParkingFee == TRUE)
        {
			stRCDataAnalyze.usTransResult=d_EMV_CHIP_OFF_APPROVAL;
            vdDebug_LogPrintf("Off Approval...	");
            srTransRec.byUploaded = CN_FALSE;
            srTransRec.byOffline = CN_TRUE;
            vdCTOS_SetTransType(SALE_OFFLINE);		
            strcpy(srTransRec.szAuthCode, "Y1");
            inRet = d_OK;
        }
        #endif
	
	}
	//Offline Approval
	else if(stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_APPROVAL) 
	{
		vdDebug_LogPrintf("Off Approval...  ");
		#ifdef EMV_Y1_DISABLE
		srTransRec.byUploaded = CN_TRUE;
        srTransRec.byOffline = CN_FALSE;
    	vdCTOS_SetTransType(SALE);				
		#else				
		srTransRec.byUploaded = CN_FALSE;
        srTransRec.byOffline = CN_TRUE;
    	vdCTOS_SetTransType(SALE_OFFLINE);		
		strcpy(srTransRec.szAuthCode, "Y1");		
		srTransRec.shTransResult = TRANS_AUTHORIZED;		
		#endif
		inRet = d_OK;
	}
	//Offline Declined
	else if(stRCDataAnalyze.usTransResult == d_EMV_CHIP_OFF_DECLINED) 
	{
		vdDebug_LogPrintf("Off Declined...  ");
		vdDisplayErrorMsg(1, 8, "Offline Declined. ");
		inRet = d_NO;
//		inRet = d_OK;
	}
	else 
	{
		vdDebug_LogPrintf("Unknow Result...  ");
		vdDisplayErrorMsg(1, 8, "Unknow Result...  ");
		inRet = d_NO;
//		inRet = d_OK;        
	}

	if (inRet == d_OK)
	{
		
		vdCTOSS_WaveGetCardLable(stRCDataEx->bSID);
		srTransRec.bWaveSID = stRCDataEx->bSID;
		srTransRec.usWaveSTransResult = stRCDataAnalyze.usTransResult;
		srTransRec.bWaveSCVMAnalysis = stRCDataAnalyze.bCVMAnalysis;
		vdDebug_LogPrintf("bWaveSID=[%x]",srTransRec.bWaveSID);
	
		vdDebug_LogPrintf("baDateTime=[%s]",stRCDataEx->baDateTime);
		memcpy(srTransRec.szYear,&stRCDataEx->baDateTime[2],2);
		wub_str_2_hex(&stRCDataEx->baDateTime[4],srTransRec.szDate,4);
		wub_str_2_hex(&stRCDataEx->baDateTime[8],srTransRec.szTime,6);
		DebugAddHEX("srTransRec.szDate", srTransRec.szDate, 4);
		DebugAddHEX("srTransRec.szTime", srTransRec.szTime, 6);

		srTransRec.usTrack1Len = stRCDataEx->bTrack1Len;
		memcpy(srTransRec.szTrack1Data, stRCDataEx->baTrack1Data, stRCDataEx->bTrack1Len);
		vdDebug_LogPrintf("usTrack1Len=[%d][%s]...  ",srTransRec.usTrack1Len,srTransRec.szTrack1Data);
		if (srTransRec.usTrack1Len > 0)
			vdGetCardHolderName(srTransRec.szTrack1Data, srTransRec.usTrack1Len);

		// patrick fix code 20140826
		if (stRCDataEx->baTrack2Data[0] != ';')
		{
			srTransRec.usTrack2Len = (stRCDataEx->bTrack2Len * 2);
			wub_hex_2_str(stRCDataEx->baTrack2Data,srTransRec.szTrack2Data,stRCDataEx->bTrack2Len);
		}
		else
		{
			memcpy(srTransRec.szTrack2Data, &stRCDataEx->baTrack2Data[1], stRCDataEx->bTrack2Len);
			srTransRec.usTrack2Len = stRCDataEx->bTrack2Len - 1;
		}
		
		vdDebug_LogPrintf("usTrack2Len=[%d][%s]...  ",srTransRec.usTrack2Len,srTransRec.szTrack2Data);
		if (srTransRec.usTrack2Len > 0)
		{
			shAnalysisTrack2(srTransRec.szTrack2Data, srTransRec.usTrack2Len);
			vdDebug_LogPrintf("usTrack2Len=[%d][%s]...	",srTransRec.usTrack2Len,srTransRec.szTrack2Data);

			if (strTCT.fPrintISOMessage == VS_TRUE){
				inPrintISOPacket(2 , srTransRec.szTrack2Data, srTransRec.usTrack2Len);
			}						
		}
		vdDebug_LogPrintf("szPAN=[%s]",srTransRec.szPAN);
		
		vdDebug_LogPrintf("usChipDataLen=[%d][%s]...  ",stRCDataEx->usChipDataLen,stRCDataEx->baChipData);
		if (stRCDataEx->usChipDataLen > 0)
		{			
			srTransRec.usChipDataLen = stRCDataEx->usChipDataLen;
			memcpy(srTransRec.baChipData, stRCDataEx->baChipData, stRCDataEx->usChipDataLen); 
			vdCTOSS_WaveGetEMVData(stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
	
		
			if (strTCT.fPrintISOMessage == VS_TRUE){
				inPrintISOPacket(3 , stRCDataEx->baChipData, stRCDataEx->usChipDataLen);
			}			
		}
		
		vdDebug_LogPrintf("baAdditionalData=[%d][%s]...  ",stRCDataEx->usAdditionalDataLen,stRCDataEx->baAdditionalData);
		if (stRCDataEx->usAdditionalDataLen > 0)
		{			
			srTransRec.usAdditionalDataLen = stRCDataEx->usAdditionalDataLen;
			memcpy(srTransRec.baAdditionalData, stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen); 
			vdCTOSS_WaveGetEMVData(stRCDataEx->baAdditionalData, (stRCDataEx->usAdditionalDataLen));
			
		
			if (strTCT.fPrintISOMessage == VS_TRUE){
				inPrintISOPacket(4 , stRCDataEx->baAdditionalData, stRCDataEx->usAdditionalDataLen);
			}			
		}
	}

	return 	inRet;
}


