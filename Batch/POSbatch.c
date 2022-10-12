#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "../Includes/POSbatch.h"
#include "../Includes/POSTypedef.h"

#include "../FileModule/myFileFunc.h"
#include "../POWRFAIL/POSPOWRFAIL.h"
#include "..\debug\debug.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\myEZLib.h"
#include "..\ui\Display.h"
#include "..\Includes\POSHost.h"
#include "..\debug\debug.h"
#include "../Ctls/POSCtls.h"

int inCTOS_SaveBatchTxn(void)
{
    int inResult ;
    int inSaveType;

    vdDebug_LogPrintf("inCTOS_SaveBatchTxn");
	inCTLOS_Updatepowrfail(PFR_BEGIN_BATCH_UPDATE);

    
    vdDebug_LogPrintf("inCTOS_SaveBatchTxn HDTid[%d].byTransType[%d].byOffline[%d].byUploaded[%d]szAuthCode[%s]byEntryMode[%d]", 
		srTransRec.HDTid, srTransRec.byTransType, srTransRec.byOffline, srTransRec.byUploaded, srTransRec.szAuthCode, srTransRec.byEntryMode);
//    if(srTransRec.byTransType == VOID && srTransRec.byOffline == CN_TRUE)
    if((srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH) && srTransRec.byOffline == CN_TRUE)
    {
        //OFFLINE SALE, then VOID it, the VOID is offline, should delete the OFFLINE SALE Advice
        if(srTransRec.byUploaded == CN_FALSE)
        {
            inCTOSS_DeleteAdviceByINV(srTransRec.szInvoiceNo);
        }
    }

	
    //sssrTransRec.byTransType=CB_PAY_TRANS;
	
    if((srTransRec.byTransType == SALE) || 
		(srTransRec.byTransType == REFUND) || 
		(srTransRec.byTransType == SALE_OFFLINE) || 
		(srTransRec.byTransType == PRE_AUTH) ||
		(srTransRec.byTransType == CASH_ADVANCE) ||
		(srTransRec.byTransType == CUP_SALE) ||
		(srTransRec.byTransType == CUP_PREAUTH_COMP) ||
        (srTransRec.byTransType == MPU_PREAUTH) ||
        (srTransRec.byTransType == MPU_PREAUTH_COMP) ||
        (srTransRec.byTransType == MPU_PREAUTH_COMP_ADV) ||
        (srTransRec.byTransType == PREAUTH_COMP) ||
        (srTransRec.byTransType == ALIPAY_SALE) ||
        (srTransRec.byTransType == CB_PAY_TRANS) ||
        (srTransRec.byTransType == OK_DOLLAR_TRANS) || 
        (srTransRec.byTransType == TOPUP) || 
        (srTransRec.byTransType == RELOAD))
    {

		vdDebug_LogPrintf("inCTOS_SaveBatchTxn >>> DF_BATCH_APPEND");
	
        inSaveType = DF_BATCH_APPEND;

        //Should be Online void the Intial SALE amount.
        if(srTransRec.byOffline == CN_FALSE)
        {
            //use szStoreID to store how much amount fill up in DE4 for VOID
            //either enter tip or not enter tip are ok
            memcpy(srTransRec.szStoreID, srTransRec.szTotalAmount, 6);
        }
    }
    else
    {
        inSaveType = DF_BATCH_UPDATE;
    }
    
//    if(srTransRec.byTransType == VOID)
//    if(srTransRec.byTransType == VOID || srTransRec.byTransType == MPU_VOID_PREAUTH || srTransRec.byTransType == MPU_VOID_PREAUTH_COMP)
    if(srTransRec.byTransType == VOID || 
       srTransRec.byTransType == MPU_VOID_PREAUTH || 
       srTransRec.byTransType == MPU_VOID_PREAUTH_COMP || 
       srTransRec.byTransType == VOID_PREAUTH ||
       srTransRec.byTransType == VOID_PREAUTH_COMP)
        srTransRec.byVoided = TRUE;
    
	inResult = inDatabase_BatchSave(&srTransRec, inSaveType);
    
    if(inResult != ST_SUCCESS)
    {
    	vdSetErrorMessage("Batch Save Error");
        return ST_ERROR;
    }

			
			vdDebug_LogPrintf("inCTOS_SaveBatchTxn >>> AAAA");

    if((memcmp(srTransRec.szAuthCode, "Y3",2))  && 
       (srTransRec.byTransType != SALE_OFFLINE) &&
       (srTransRec.byTransType != SALE_ADJUST)  &&
       (srTransRec.byTransType != SALE_TIP) &&
		(srTransRec.byTransType != CUP_SALE_ADJUST) &&
       ( srTransRec.byOffline != VS_TRUE))
    {
        inMyFile_ReversalDelete();
    }

	
	vdDebug_LogPrintf("inCTOS_SaveBatchTxn >>> BBBB");
        
    if((srTransRec.byTransType == SALE_TIP) || 
		(srTransRec.byTransType == SALE_ADJUST) || 
		(srTransRec.byTransType == SALE_OFFLINE) ||
		(srTransRec.byTransType == CUP_SALE_ADJUST)
       )
    {
		vdDebug_LogPrintf("inCTOS_SaveBatchTxn >>> CCCC");

	
        //fix send tip adjust advie two times issue
        inCTOSS_DeleteAdviceByINV(srTransRec.szInvoiceNo);
        inMyFile_AdviceSave(&srTransRec, srTransRec.byPackType);
    }
    //if original txn is offline, then do a online txn piggy back the offline advice, the offline advice been deleted, then void it, the void is offline, need to create advice for this offline void
//    else if(srTransRec.byTransType == VOID && srTransRec.byOffline == CN_TRUE && srTransRec.byUploaded == CN_TRUE)
    else if((srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH) && srTransRec.byOffline == CN_TRUE && srTransRec.byUploaded == CN_TRUE)
    {
        vdDebug_LogPrintf("Create Advice for offline VOID");
        inMyFile_AdviceSave(&srTransRec, OFFLINE_VOID);
    }
//    else if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
	else if (((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
	/* EMV: Revised EMV details printing - start -- jzg */
	(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
	(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
	(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
	(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
	(srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
		(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) ||
		(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
		(srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD))	
	&& (srTransRec.byTransType == SALE || srTransRec.byTransType == CASH_ADVANCE))
	/* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
    {

	   vdDebug_LogPrintf("inCTOS_SaveBatchTxn	ABCD");
	
       if((srTransRec.byTCuploaded == 0) ||
          (!memcmp(srTransRec.szAuthCode, "Y1",2)) || 
          (!memcmp(srTransRec.szAuthCode, "Y3",2)) 
         )
       {

		   vdDebug_LogPrintf("inCTOS_SaveBatchTxn >>> ABCDE");
	   
           if((memcmp(srTransRec.szAuthCode, "Y1",2)) && 
              (memcmp(srTransRec.szAuthCode, "Y3",2)) 
             )
           {
           		vdDebug_LogPrintf("inMyFile_TCUploadFileSave***HERE*****HDTid=[%d] byTCuploaded=[%d]",srTransRec.HDTid, srTransRec.byTCuploaded);
						
				// Add to send de55 for JCB CTLS transactions #3
				#ifdef MPU_CARD_TC_UPLOAD_ENABLE
				srTransRec.byPackType = TC_UPLOAD;
				inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
				#else								
				// for issue 562 	MK Merchant issue		
				if (srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19 && srTransRec.byTCuploaded == 0)
				{
               		srTransRec.byPackType = TC_UPLOAD;
               		inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
				}
				#endif
               
           }
           else
           {
         	   vdDebug_LogPrintf("inCTOS_SaveBatchTxn	DEFG");  
               srTransRec.byPackType = SEND_ADVICE;
               inMyFile_AdviceSave(&srTransRec, srTransRec.byPackType);
           }
       }
    }
    else if((srTransRec.byTransType == REFUND || srTransRec.byTransType == PRE_AUTH) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))// || srTransRec.byEntryMode == CARD_ENTRY_EASY_ICC))
    {
       vdDebug_LogPrintf("inMyFile_TCUploadFileSave****HERE2****HDTid=[%d]",srTransRec.HDTid);

	   // Add to send de55 for JCB CTLS transactions #4
		#ifdef MPU_CARD_TC_UPLOAD_ENABLE		
		srTransRec.byPackType = TC_UPLOAD;
		inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
	    #else
	    // for issue 562 	MK Merchant issue
	    if (srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19 && srTransRec.byTCuploaded == 0)
	   	{
        	srTransRec.byPackType = TC_UPLOAD;
        	inMyFile_TCUploadFileSave(&srTransRec, srTransRec.byPackType);
	   	}	   
	    #endif
           
    }
	
	         	   vdDebug_LogPrintf("inCTOS_SaveBatchTxn	END");  

    return d_OK;
}



int inCTOS_BatchReviewFlow(void)
{
    int inResult;
    int inReadRecResult;
 	int inCount = 0;
	int inBatchRecordNum = 0;
    BYTE key;
	char szPanDisplay[24+1];
    BYTE szTemp1[30+1];
	int  *pinTransDataid = NULL;

	inBatchRecordNum = inBatchNumRecord();

    vdDebug_LogPrintf("inCTOS_BatchReviewFlow inBatchRecordNum =[%d]", inBatchRecordNum);

	pinTransDataid = (int*)malloc(inBatchRecordNum * sizeof(int));
	
	inBatchByMerchandHost(inBatchRecordNum, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);

	inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);

    vdDebug_LogPrintf("inCTOS_BatchReviewFlow inResult =[%d]", inResult);
	
	if(inResult == d_NO)
	{
	    vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
		free(pinTransDataid);
	    return d_OK;
	}

    inCTOS_DisplayBatchRecordDetail(BATCH_REVIEW);
    
    while(1)
	{ 
		CTOS_KBDHit(&key); 
		if (key == d_KBD_CANCEL) 
		{ 
			free(pinTransDataid);
		    return; 
		} 
		else if ((key == d_KBD_DOWN) || (key == d_KBD_DOT))
	    {
           inCount++;     
		   		   
		   DebugAddIntX("inCount", inCount);

		    if(inCount >= inBatchRecordNum)
		   {
		   	  vdDisplayErrorMsg(1, 8, "END OF RECORD");
			  inCount = 0;			  		  			  
		   }
		  
		   inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
		   
		   inCTOS_DisplayBatchRecordDetail(BATCH_REVIEW);	
		} 
        else if ((key == d_KBD_UP) || (key == d_KBD_00))
        { 
            inCount--;

            inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);

            if(inResult == d_NO)
            {
                vdDisplayErrorMsg(1, 8, "FIRST OF RECORD");
                inCount = 0;	  
                inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
            }

            inCTOS_DisplayBatchRecordDetail(BATCH_REVIEW);
		} 
    } 	
    
	free(pinTransDataid);
    return d_OK;
    
}


