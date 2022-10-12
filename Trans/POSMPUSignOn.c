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
#include "..\Database\DatabaseFunc.h"
#include "..\erm\Poserm.h"
#include "..\Includes\POSMPUSignOn.h" //@@IBR ADD 20161031
#include "..\Includes\wub_lib.h"
#include "..\Includes\POSSetting.h"
#include "..\PinPad\pinpad.h"


int inMPUCheckTMK(void)
{
	return d_OK;
}

void vdSetMPUSignOnStatus(int inFlag)
{	
	put_env_int("#MPUSIGNON", inFlag);
}

int inGetMPUSignOnStatus(void)
{
	int inStatus = 0;

	inStatus = get_env_int("#MPUSIGNON");

	return inStatus;
}

int inCTOS_GetOperCode(BYTE *szOperCode, int *inDataCnt){
    char szTemp[6+1];
    BYTE byMPUOper[3 + 1];
    
    memset(szTemp, 0x00, sizeof (szTemp));
    memset(byMPUOper, 0x00, sizeof (byMPUOper));
    get_env("#MPUOPER", szTemp, 6);
    wub_str_2_hex(szTemp, byMPUOper, 6);

    memcpy(szOperCode, byMPUOper, 3);
//    memcpy(szOperCode, POS_OPER_CODE, 3);
    *inDataCnt += 3;
    
    return d_OK;
}


int inMPU_PackSignonDE60Req(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg)
{
	BYTE szDE60ReqBuff[999];
	int inLen = 0;
//  inDataCnt = 0;

	memset(szDE60ReqBuff, 0x00, sizeof(szDE60ReqBuff));

	inLen = 8;
	memcpy(szDE60ReqBuff, "\x00\x11\x00\x00\x00\x00\x10\x03", inLen);

	memcpy(pszMsg, szDE60ReqBuff, inLen);

	return inLen;
}


int inMPU_PackSignonDE63Req(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg)
{
	BYTE szDE63ReqBuff[999];
	int inLen = 0;

	memset(szDE63ReqBuff, 0x00, sizeof(szDE63ReqBuff));

	inLen = 8;
	memcpy(szDE63ReqBuff, "\x00\x11\x00\x00\x00\x00\x10\x03", inLen);

	memcpy(pszMsg, szDE63ReqBuff, inLen);

	return inLen;
}

int inMPU_PackSignOffDE60Req(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg)
{
	BYTE szDE60ReqBuff[999];
	int inLen = 0;
//  inDataCnt = 0;

	memset(szDE60ReqBuff, 0x00, sizeof(szDE60ReqBuff));

	inLen = 8;
	memcpy(szDE60ReqBuff, "\x00\x11\x00\x00\x00\x00\x10\x02", inLen);

	memcpy(pszMsg, szDE60ReqBuff, inLen);

	return inLen;
}


int inMPU_PackSignOffDE63Req(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg)
{
	BYTE szDE63ReqBuff[999];
	int inLen = 0;

	memset(szDE63ReqBuff, 0x00, sizeof(szDE63ReqBuff));

	inLen = 8;
	memcpy(szDE63ReqBuff, "\x00\x03\x00\x00\x00", inLen);

	memcpy(pszMsg, szDE63ReqBuff, inLen);

	return inLen;
}


//static int inMPU_EncryptPIK(void){
//    return ushMPU_WriteEncPIK(strDCT.szPINKey, 16);
//}

int inCTOS_MPU_SignOnFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(MPU_SIGNON);
    
    //display title
    vdDispTransTitle(MPU_SIGNON);
    vdDebug_LogPrintf("**inCTOS_MPU_SignOnFlowProcess >>> strHDT.inHostIndex[%d] STLSIGNONANYHI[%d] STLSIGNONALLHI[%d]** start", 
		strHDT.inHostIndex,	get_env_int("STLSIGNONANYHI"), get_env_int("STLSIGNONALLHI"));

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectMPUHost();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

    vdDebug_LogPrintf("**inCTOS_MPU_SignOnFlowProcess1 fTPDUOnOff* [%d]*", strHDT.fTPDUOnOff);

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;
    vdDebug_LogPrintf("**inCTOS_MPU_SignOnFlowProcess2 fTPDUOnOff* [%d]*", strHDT.fTPDUOnOff);

	vdSetMPUSignOnStatus(1);

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	/*Display OK*/
	vduiDisplayStringCenter(7, strHDT.szHostLabel);
	vduiDisplayStringCenter(8, "SIGN-ON SUCCESS");
	
   	 vduiWarningSound();
	WaitKey(3);

    return ST_SUCCESS;
}



//static int inMPU_EncryptPIK(void){
//    return ushMPU_WriteEncPIK(strDCT.szPINKey, 16);
//}

int inCTOS_MPU_SignOnFlowProcess_ALL(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(MPU_SIGNON);
    
    //display title
    //vdDispTransTitle(MPU_SIGNON);
    vdDispTransTitle(SIGNON_ALL);
	
    vdDebug_LogPrintf("**inCTOS_MPU_SignOnFlowProcess_ALL** start");



	//no need to select host. run all enabled HOST
	#if 0
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	
    inRet = inCTOS_SelectMPUHost();
    if(d_OK != inRet)
        return inRet;

	#endif

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;	



    vdDebug_LogPrintf("**inCTOS_MPU_SignOnFlowProcess1 fTPDUOnOff* [%d]*", strHDT.fTPDUOnOff);

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;
    vdDebug_LogPrintf("**inCTOS_MPU_SignOnFlowProcess2 fTPDUOnOff* [%d]*", strHDT.fTPDUOnOff);

	vdSetMPUSignOnStatus(1);

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	/*Display OK*/
	vduiDisplayStringCenter(7,srTransRec.szHostLabel);
	vduiDisplayStringCenter(8, "SIGN-ON SUCCESS");
    vduiWarningSound();
	WaitKey(3);

	
    CTOS_LCDTClearDisplay();

    vdDebug_LogPrintf("**inCTOS_MPU_SignOnFlowProcess_ALL** END");
    return ST_SUCCESS;
}



int inCTOS_SIGNONAllHosts(void) {
    int inNumOfHost = 0, inNum;
    char szBcd[INVOICE_BCD_SIZE + 1];
    char szAPName[25];
    int inAPPID;

	#if 0
	BOOL fHostToSIgn =  FALSE;
	#endif

	vdDebug_LogPrintf("inCTOS_SIGNONAllHosts");

    memset(szAPName, 0x00, sizeof (szAPName));
    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("inCTOS_SIGNONAllHosts inCurrencyIndex = %d szAPName = %s",strCST.inCurrencyIndex, szAPName);

    inNumOfHost = inHDTMAX();
    
    vdDebug_LogPrintf("inNumOfHost=[%d]-----", inNumOfHost);

	//IF CURR CODE IS MMK, only signon hosts MPU/BPU and MMK else include USD host
	//if incase needed. enable.
	#if 0
	if(strCST.inCurrencyIndex == 1)
		fHostToSIgn = TRUE;
	#endif

	
    for (inNum = 1; inNum <= inNumOfHost; inNum++) 
	{
        if (inHDTRead(inNum) == d_OK) 
		{
            vdDebug_LogPrintf("szAPName=[%s]-strHDT.szAPName[%s]-strHDT.szHostLabel[%s]-strHDT.HDTid[%d]--", szAPName, strHDT.szAPName, strHDT.szHostLabel, strHDT.inHostIndex);
			
            if (strcmp(szAPName, strHDT.szAPName) != 0) {
                continue;
            }

	     #if 0
	     if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0) {
                continue;
				
		//to Exclude CBPAY and OK$ host during SIGNON -ALL
            if( (memcmp(strHDT.szHostLabel, "Visa&Master", 11) == 0)|| 
				(memcmp(strHDT.szHostLabel, "CBPay", 5) == 0)|| 
				(memcmp(strHDT.szHostLabel, "OK$", 3) == 0))
		{
                continue;
            }
	     #else		
		 //to Exclude CBPAY and OK$ host during SIGNON -ALL
		 //Fix for http://118.201.48.214:8080/issues/75 #35.  1)When we perform Sign On All menu, it is working for all host sign on(CBPay,OK$,MPU BPC,Finexus-MMK) like that we opened all hosts.
		//if(strHDT.inHostIndex == 13 || strHDT.inHostIndex == 14 || strHDT.inHostIndex == 21 || strHDT.inHostIndex == 1)	 
		if(strHDT.inHostIndex != 17 && strHDT.inHostIndex != 22 && strHDT.inHostIndex != 23)
			continue;
            #endif

            inCPTRead(inNum);
            srTransRec.HDTid = inNum;
            strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
            vdDebug_LogPrintf("inCTOS_SIGNONAllHosts srTransRec.szHostLabel=[%s]- srTransRec.HDTid=[%d]strCST.inCurrencyIndex[%d]---", 
				srTransRec.szHostLabel,  srTransRec.HDTid, strCST.inCurrencyIndex);

		//for testing. changing currency display
		if(strCST.inCurrencyIndex == 2)		
		 	inCSTRead(2);
				 
            memset(szBcd, 0x00, sizeof (szBcd));
            memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
            srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo, 3);
			


            // only one AP
            inCTOS_MPU_SignOnFlowProcess_ALL();
        }
    }

	//return to orig value
	#if 0
	fHostToSIgn = FALSE;
	#endif

    vdDebug_LogPrintf("end inCTOS_SIGNONAllHosts-----");
    return ST_SUCCESS;
}

// SigOn from IDLE/ Main menu
int inCTOS_MPU_SIGNON(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();

    vdDebug_LogPrintf("inCTOS_MPU_SIGNON");
    
    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(TRUE);
	vdSetSIGNON(TRUE);	//check menu to display if signon ALL will be included
		
    inRet = inCTOS_MPU_SignOnFlowProcess();
	
	vdSetMPUTrans(FALSE);
	vdSetSIGNON(FALSE);
	
    vdCTOS_TransEndReset();

    return inRet;
}

//F2 key was pressed
int inCTOS_MPU_SIGNON_F2(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();

    vdDebug_LogPrintf("inCTOS_MPU_SIGNON_F2");
    
    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(TRUE);	
	vdSetSIGNON(FALSE);
	
    inRet = inCTOS_MPU_SignOnFlowProcess();
	vdSetMPUTrans(FALSE);
	
    vdCTOS_TransEndReset();

    return inRet;
}


//For SignOn - ALL Option
int inCTOS_MPU_SIGNON_ALL(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();

    vdDebug_LogPrintf("inCTOS_MPU_SIGNON_ALL");
	
    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(TRUE);

    //inRet = inCTOS_MPU_SignOnFlowProcess_ALL();

    vdDebug_LogPrintf("inCTOS_MPU_SIGNON_ALL2");

	inRet = inCTOS_SIGNONAllHosts();

    vdDebug_LogPrintf("inCTOS_MPU_SIGNON_ALL END inRet [%d]", inRet);
	
	vdSetMPUTrans(FALSE);
	
    vdCTOS_TransEndReset();

    return inRet;
}


int inCTOS_AutoMPU_SIGNON(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(MPU_SIGNON);
    
    //display title
    vdDispTransTitle(MPU_SIGNON);

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

	vdSetMPUSignOnStatus(1);

	/*Display OK*/
	vduiDisplayStringCenter(8, "SIGN-ON SUCCESS");

	//vdCTOS_TransEndReset();
 
    return ST_SUCCESS;  
}


int inCTOS_MPU_SignOffFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(MPU_SIGNOFF);
    
    //display title
    vdDispTransTitle(MPU_SIGNOFF);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectMPUHost();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

	vdSetMPUSignOnStatus(0);

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    return ST_SUCCESS;
}


int inCTOS_MPU_SIGNOFF(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(TRUE);
    inRet = inCTOS_MPU_SignOffFlowProcess();
	vdSetMPUTrans(FALSE);
	
    vdCTOS_TransEndReset();

    return inRet;
}


