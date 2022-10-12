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
#include "..\Includes\POSMPUSignOn.h"
#include "..\Erm\PosErm.h"

#define ENV_MPU_BAT_UP		"#MPUBATUP"

int inMPU_CheckBatchUploadFlag(TRANS_DATA_TABLE *srTransPara)
{
	int inNeedBatchUpload = 0;
	BYTE szBatchUploadStr[64];

	memset(szBatchUploadStr, 0x00, sizeof(szBatchUploadStr));
	sprintf(szBatchUploadStr, "%s%02d", ENV_MPU_BAT_UP, srTransPara->HDTid);
	
	inNeedBatchUpload = get_env_int(szBatchUploadStr);

	return inNeedBatchUpload;
}

void vdMPU_SetBatchUploadFlag(TRANS_DATA_TABLE *srTransPara, int inFlag)
{
	int inNeedBatchUpload = 0;
	BYTE szBatchUploadStr[64];

	memset(szBatchUploadStr, 0x00, sizeof(szBatchUploadStr));
	sprintf(szBatchUploadStr, "%s%02d", ENV_MPU_BAT_UP, srTransPara->HDTid);

	put_env_int(szBatchUploadStr, inFlag);
}

int inCTOS_MPU_SettlementFlowProcess(void)
{
    int inRet = d_NO;

    vdCTOS_SetTransType(MPU_SETTLE);
    
    //display title
    vdDispTransTitle(MPU_SETTLE);


    vdDebug_LogPrintf("inCTOS_MPU_SettlementFlowProcess");

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SettlementSelectAndLoadHost();
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

	inRet = inMPU_DisplayBatchTotalEx();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inMPU_PrintSettleReport();
    if(d_OK != inRet)
        return inRet; 

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

int inCTOS_MPU_SETTLEMENT(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

    vdDebug_LogPrintf("inCTOS_MPU_SETTLEMENT");

	vdSetMPUTrans(TRUE);
    inRet = inCTOS_MPU_SettlementFlowProcess();
	vdSetMPUTrans(FALSE);
	
	inCTOSS_UploadReceipt();
    //if(d_OK == inRet)
        inCTOSS_SettlementCheckTMSDownloadRequest();
        
        vdCTOS_SetNextAutoSettle();

    vdCTOS_TransEndReset();

    return inRet;
}

static int inCTOS_MPUBatchUploadFlowProcess(void){
    int inRet = d_NO;
	int inNeedBatUp = 0;

    vdCTOS_SetTransType(BATCH_UPLOAD);
    vdDispTransTitle(BATCH_UPLOAD);

	vdDebug_LogPrintf("inCTOS_MPUBatchUploadFlowProcess");

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

//    inRet = inCTOS_SettlementSelectAndLoadHost();
//    if(d_OK != inRet)
//        return inRet;
    
    inRet = inCTOS_SelectHostSetting();
//	if(d_OK != inRet)
//     return inRet;

	inNeedBatUp = inMPU_CheckBatchUploadFlag(&srTransRec);// set batch upload flag on

	
	vdDebug_LogPrintf("inCTOS_MPUBatchUploadFlowProcess [%d]", inNeedBatUp);
	if (1 != inNeedBatUp)
	{
		vdSetErrorMessage("NO BATCH UPLOAD!");
		return ST_ERROR;
	}

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

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

//    inRet = inBuildAndSendIsoData();
//    inRet = inMPU_ProcessBatchUpload(&srTransRec);
    inRet = inMPU_ProcessBatchUpload();
    if(d_OK != inRet)
    {
    	inCTOS_inDisconnect();
        return inRet;
    }

	vdMPU_SetBatchUploadFlag(&srTransRec, 0);// set batch upload flag on

	inRet = inMPU_PrintSettleReport();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SettlementClearBathAndAccum();
    if(d_OK != inRet)
        return inRet;
	
    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	CTOS_Beep();
	vduiDisplayStringCenter(8, "BATCH UPLOAD SUCCESS");

    return ST_SUCCESS;
}


int inCTOS_MPU_BatchUpload(void) 
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();

	vdSetMPUTrans(TRUE);
//    inRet = inPorcessTransUpLoad(&srTransRec);
    inCTOS_MPUBatchUploadFlowProcess();
	vdSetMPUTrans(FALSE);

    if (isCheckTerminalMP200() == d_OK) 
    {
		inCTOS_inDisconnect();
		inCTOSS_UploadReceipt();
    }
	
    vdCTOS_TransEndReset();

	/*perform Sign on*/// Emmy added this to prevent sending SignON when its not MPU HOST
	if(fGetCashAdvAppFlag() != TRUE &&((strHDT.inHostIndex == 16)||(strHDT.inHostIndex == 17)||(strHDT.inHostIndex == 18)||(strHDT.inHostIndex == 19))) 		   
	inCTOS_MPU_SIGNON();

    return inRet;

}


int inCTOS_MPUSettleAMerchant(void)
{
    int inRet = d_NO;

    //vdCTOS_SetTransType(MPU_SETTLE);
    
    //display title
    vdDispTransTitle(MPU_SETTLE);

    //inRet = inCTOS_GetTxnPassword();
    //if(d_OK != inRet)
   //     return inRet;

//    inRet = inCTOS_SettlementSelectAndLoadHost();
//    if(d_OK != inRet)
//        return inRet;

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

	inRet = inMPU_DisplayBatchTotalEx();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inMPU_PrintSettleReport();
    if(d_OK != inRet)
        return inRet; 

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


int inCTOS_MPUSettleAHost(void)
{
	int inRet = d_NO;
	int inNumOfMit = 0,inNum;
	char szErrMsg[30+1];

	memset(szErrMsg,0x00,sizeof(szErrMsg));
	sprintf(szErrMsg,"SETTLE %s",strHDT.szHostLabel);
	CTOS_LCDTPrintXY(1, 8, "                   ");
	CTOS_LCDTPrintXY(1, 8, szErrMsg);
	inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMit);
	vdDebug_LogPrintf("inNumOfMit=[%d]-----",inNumOfMit);
	for(inNum =0 ;inNum < inNumOfMit; inNum++)
	{
		memcpy(&strMMT[0],&strMMT[inNum],sizeof(STRUCT_MMT));
		srTransRec.MITid = strMMT[0].MITid;
	    strcpy(srTransRec.szTID, strMMT[0].szTID);
	    strcpy(srTransRec.szMID, strMMT[0].szMID);
	    memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
	    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

	    inRet = inCTOS_MPUSettleAMerchant();
		if (d_OK != inRet)
		{
			memset(szErrMsg,0x00,sizeof(szErrMsg));
		    if (inGetErrorMessage(szErrMsg) > 0)
		    {
		        vdDisplayErrorMsg(1, 8, szErrMsg);
		    }
			vdSetErrorMessage("");
		}
    }

	return ST_SUCCESS;
}

#ifdef HOST_ONE_BY_ONE
int inCTOS_SettleAllHosts(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];

    vdCTOS_SetTransType(SETTLE);
    
    //display title
    vdDispTransTitle(SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

	if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
	{
	    inNumOfHost = inHDTNumRecord();
		vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
		for(inNum =1 ;inNum <= inNumOfHost; inNum++)
		{
			if(inHDTRead(inNum) == d_OK)
			{
				inCPTRead(inNum);
				srTransRec.HDTid = inNum;
        		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
				memset(szBcd, 0x00, sizeof(szBcd));
			    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
			    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
			    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
				
				if (inMultiAP_CheckMainAPStatus() == d_OK)
				{
					//multi AP
			        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SETTLE_ALL);
			        if (d_OK == inRet)//Current AP
					{
						inRet = inCTOS_SettleAHost();	
					}
					memset(szErrMsg,0x00,sizeof(szErrMsg));
				    if (inGetErrorMessage(szErrMsg) > 0)
				    {
				        vdDisplayErrorMsg(1, 8, szErrMsg);
				    }
					vdSetErrorMessage("");
					
				}
				else
				{
					// only one AP
					inRet = inCTOS_SettleAHost();
		                return inRet;
				}
			}
		}
	}
	else// Sub APP
    {
        inRet = inCTOS_MultiAPGetData();
        if(d_OK != inRet)
            return inRet;

        inRet = inCTOS_MultiAPReloadHost();
        if(d_OK != inRet)
            return inRet;

		inRet = inCTOS_SettleAHost();
            return inRet;
    }

    return ST_SUCCESS;
}
#endif

int inCTOS_MPUSettleAllHosts(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[25];
	int inAPPID;

	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
        
        
//    inNumOfHost = inHDTNumRecord();
        inNumOfHost = inHDTMAX(); //@@IBR Modify, so we can get the maximum number of enabled host
	vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
	for(inNum =1 ;inNum <= inNumOfHost; inNum++)
	{
		if(inHDTRead(inNum) == d_OK)
		{
			vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
			if (strcmp(szAPName, strHDT.szAPName)!=0)
			{
				continue;
			}
			if (memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
			{
				continue;
			}
                        
//                        if(memcmp(strHDT.szHostLabel, "MPU", 3) != 0){ //@@IBR ADD 20170209
//                            continue;
//                        }
                        
                        if((strHDT.inHostIndex != 17) && (strHDT.inHostIndex != 18) && (strHDT.inHostIndex != 19))
                            continue;
			
			inCPTRead(inNum);
			srTransRec.HDTid = inNum;
    		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			memset(szBcd, 0x00, sizeof(szBcd));
		    memcpy(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);    
		    inBcdAddOne(szBcd, strHDT.szTraceNo, INVOICE_BCD_SIZE);
			vdPCIDebug_HexPrintf("strHDT.szTraceNo22",strHDT.szTraceNo,3);
		    srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szTraceNo,3);
			vdDebug_LogPrintf("eulTraceNum-=[%d]----",srTransRec.ulTraceNum);
			
			// only one AP
			inCTOS_MPUSettleAHost();
		}
	}

	vdDebug_LogPrintf("end inCTOS_SettleAllHosts-----");
    return ST_SUCCESS;
}


int inCTOS_MPUSettleAllOperation(void)
{
    int inRet = d_NO;
	int inNumOfHost = 0,inNum;
	char szBcd[INVOICE_BCD_SIZE+1];
	char szErrMsg[30+1];
	char szAPName[25];
	int inAPPID;

	int inResult = ST_SUCCESS; //Auto-settlement: return result if a settlement failed or not -- jzg

    vdCTOS_SetTransType(MPU_SETTLE);
//    
//    //display title
    vdDispTransTitle(MPU_SETTLE);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

//	if (inMultiAP_CheckMainAPStatus() == d_OK)
//	{
//		inResult = inCTOS_SettleAllHosts();
//		inCTOS_MultiAPALLAppEventID(d_IPC_CMD_SETTLE_ALL);
//	}
//	else
//	{
		inResult = inCTOS_MPUSettleAllHosts();
                
//	}

    return ST_SUCCESS;
}



int inCTOS_MPU_SETTLE_ALL(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    vdSetMPUTrans(TRUE);
    
    inRet = inCTOS_MPUSettleAllOperation();
    vdSetMPUTrans(FALSE);

	inCTOS_inDisconnect();

	inCTOSS_UploadReceipt();
	
    if(d_OK == inRet)
        inCTOSS_SettlementCheckTMSDownloadRequest();
        
        vdCTOS_SetNextAutoSettle();

    vdCTOS_TransEndReset();

    return inRet;
}
