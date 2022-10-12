
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>


#include "accum.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Debug\Debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\Includes\POSSetting.h"
#include "..\ui\Display.h"
#include "../Ctls/POSCtls.h"

void vdCTOS_GetAccumName(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal)
{
    sprintf(strFile->szFileName, "ACC%02d%02d.total"
                                , strHDT.inHostIndex
                                , srTransRec.MITid);
                                
    strFile->bSeekType           = d_SEEK_FROM_BEGINNING;
    strFile->bStorageType        = d_STORAGE_FLASH ;
    strFile->fCloseFileNow       = TRUE;
    strFile->ulRecSize           = sizeof(ACCUM_REC);
    strFile->ptrRec              = strTotal;    
    
}


int inCTOS_ReadAccumTotal(ACCUM_REC *strTotal)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    
    vdCTOS_GetAccumName(&strFile, strTotal);

    return (inMyFile_RecRead(&strFile));    
}

int inCTOS_SaveAccumTotal(ACCUM_REC *strTotal)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    
    vdCTOS_GetAccumName(&strFile, strTotal);

    return (inMyFile_RecSave(&strFile));    
}


//format amount 10+2 change all atol to atof
int inCTOS_UpdateAccumTotal(void)
{
	ACCUM_REC srAccumRec;
    BYTE        szTransAmt[12+1];
    BYTE        szTipAmt[12+1];
    BYTE        szOrgAmt[12+1];
    int         inResult;
	int inTranCardType = 0;

    vdDebug_LogPrintf("inCTOS_UpdateAccumTotal");
    
    memset(szTransAmt, 0x00, sizeof(szTransAmt));
    memset(szTipAmt, 0x00, sizeof(szTipAmt));
    memset(szOrgAmt, 0x00, sizeof(szOrgAmt));
    wub_hex_2_str(srTransRec.szTotalAmount, szTransAmt, 6);
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmt, 6);
    wub_hex_2_str(srTransRec.szOrgAmount, szOrgAmt, 6);

#if 1
    vdDebug_LogPrintf("inCTOS_UpdateAccumTotal szTransAmt[%s]", szTransAmt);
    vdDebug_LogPrintf("inCTOS_UpdateAccumTotal IITid:[%d]:HDTid[%d]:inHostIndex[%d]:inCardType[%d]", srTransRec.IITid, srTransRec.HDTid, strHDT.inHostIndex, srTransRec.inCardType);

	if (srTransRec.inCardType == CREDIT_CARD)
	{
		// for testing fix for case #409 - All MPU cards type need to check
		if(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 20 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)
			inTranCardType = 1; // save to debit accum struct for MPU hosts
		else
			inTranCardType = 0; // will be saved to credit accum struct
	}
    else
    {
    	inTranCardType = 1;//save debit card accue total
    }	
#else
	vdDebug_LogPrintf("inCTOS_UpdateAccumTotal szTransAmt[%s]", szTransAmt);

	if (srTransRec.inCardType == CREDIT_CARD)
	{
		inTranCardType = 0;//save credit card accue total
	}
	else
	{
		inTranCardType = 1;//save debit card accue total
	}
#endif
	
	vdDebug_LogPrintf("inTranCardType[%d]", inTranCardType);	
	vdDebug_LogPrintf("srTransRec.fIsInstallment[%d]", srTransRec.fIsInstallment);	
	vdDebug_LogPrintf("srTransRec.byTransType[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("srTransRec.byOrgTransType[%d]", srTransRec.byOrgTransType);
	
    if((srTransRec.fIsInstallment == TRUE) || (srTransRec.byTransType == CASH_ADVANCE) || (srTransRec.byOrgTransType == CASH_ADVANCE)
		|| (srTransRec.byTransType == REFUND))
	{

		#ifdef CASH_ADV_NEW_FEATURE
			if(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 20 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)
				inTranCardType = 1; // save to debit accum struct for MPU hosts
			else	
	        	inTranCardType = 0;
		#else		
			inTranCardType = 0;
		#endif
		
		vdDebug_LogPrintf("IPP/CAV/REFND set inTranCardType as %d", inTranCardType);
    }

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }        

    vdDebug_LogPrintf("byTransType[%d].byOrgTransType[%d].szOriginTipTrType[%d]IITid[%d]", srTransRec.byTransType, srTransRec.byOrgTransType, szOriginTipTrType, srTransRec.IITid);
    
	vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType);
    vdDebug_LogPrintf("CardTotal SaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal OfflCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal RefdCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount);            
    vdDebug_LogPrintf("CardTotal VoidCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("CardTotal TipCount [%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount);
	vdDebug_LogPrintf("CardTotal CUPSaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCUPPreAuthCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPPreAuthTotalAmount);
	vdDebug_LogPrintf("CardTotal usCashAdvCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashAdvCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashAdvTotalAmount);

    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);
    vdDebug_LogPrintf("HostTotal CUPSaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPPreAuthCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount);
    vdDebug_LogPrintf("HostTotal usCashAdvCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount);

	switch(srTransRec.byTransType)
    {
    #ifdef TOPUP_RELOAD
            case TOPUP:
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTopupCount++;
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTopupTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTopupTotalAmount + atof(szTransAmt);

				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTopupCount++;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTopupTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTopupTotalAmount + atof(szTransAmt); 		   
			break;
			
            case RELOAD:
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRepaymentCount++;
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRepaymentTotalAmount= srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRepaymentTotalAmount + atof(szTransAmt);

				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRepaymentCount++;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRepaymentTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRepaymentTotalAmount + atof(szTransAmt); 		   
			break;
    #endif
		case ALIPAY_SALE:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usAlipaySaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulAlipaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulAlipaySaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usAlipaySaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulAlipaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulAlipaySaleTotalAmount + atof(szTransAmt);            
        
        case SALE:
        case PREAUTH_COMP:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt);            
//            if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
			if (((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
				/* EMV: Revised EMV details printing - start -- jzg */
				(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
				(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
				(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
				(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
				(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
				(srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
				(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) ||
				(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) || // for jcb ctls
				(srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD)) && (srTransRec.byTransType == SALE))
				/* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
                srAccumRec.stBankTotal[inTranCardType].usEMVTCCount++;
            break;
        case CUP_SALE:
//        case CUP_PREAUTH_COMP:
//		case MPU_PREAUTH_COMP:
        srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCUPSaleCount++;
        srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPSaleTotalAmount + atof(szTransAmt);

        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPSaleCount++;
        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPSaleTotalAmount + atof(szTransAmt);            
//            if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
                    if (((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
                            /* EMV: Revised EMV details printing - start -- jzg */
                            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
                            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
                            (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
                            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
                            (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) || 
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD)) && (srTransRec.byTransType == SALE))
                            /* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
            srAccumRec.stBankTotal[inTranCardType].usEMVTCCount++;
                 break;
        case CUP_PREAUTH_COMP:
        case MPU_PREAUTH_COMP:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCUPPreAuthCount++;
        srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPPreAuthTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPPreAuthTotalAmount + atof(szTransAmt);

        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPPreAuthCount++;
        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount + atof(szTransAmt);            
//            if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
                    if (((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
                            /* EMV: Revised EMV details printing - start -- jzg */
                            (srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
                            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
                            (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
                            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
                            (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) || 
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) ||
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
                            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD)) && (srTransRec.byTransType == SALE))
                            /* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
            srAccumRec.stBankTotal[inTranCardType].usEMVTCCount++;
                    break;
        case CASH_ADVANCE:		
			vdDebug_LogPrintf("CASH_ADVANCE");
			srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashAdvCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashAdvTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashAdvTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount + atof(szTransAmt);            
//            if((srTransRec.byTransType == SALE) && (srTransRec.byEntryMode == CARD_ENTRY_ICC))
			if (((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
				/* EMV: Revised EMV details printing - start -- jzg */
				(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
				(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
				(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
				(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
				(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC)) && (srTransRec.byTransType == CASH_ADVANCE))
				/* EMV: Revised EMV details printing - end -- jzg */ // patrick fix contactless 20140828
                srAccumRec.stBankTotal[inTranCardType].usEMVTCCount++;
            break;
		case CB_PAY_TRANS:
				vdDebug_LogPrintf("CB_PAY_TRANS");			
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCBPaySaleCount++;
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCBPaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCBPaySaleTotalAmount + atof(szTransAmt);
				
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCBPaySaleCount++;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount + atof(szTransAmt);			 
			 break; 			
			 
#ifdef OK_DOLLAR_FEATURE
		case OK_DOLLAR_TRANS:
				vdDebug_LogPrintf("OK_DOLLAR_TRANS");			
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOKDSaleCount++;
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOKDSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOKDSaleTotalAmount + atof(szTransAmt);
				
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOKDSaleCount++;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount + atof(szTransAmt);			 
			 break; 			
#endif
        case SALE_OFFLINE:
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atof(szTransAmt);

            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt);
                    
            break;
        case REFUND:
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount + atof(szTransAmt);
            
            break;
        case VOID:
			vdDebug_LogPrintf("CASE VOID!!! [%d]", srTransRec.byOrgTransType);

	            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
	            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atof(szTransAmt);
	            
	            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
	            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atof(szTransAmt);
				
            if(srTransRec.byOrgTransType == SALE || srTransRec.byOrgTransType == ALIPAY_SALE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szTransAmt);

                if(srTransRec.byOrgTransType == ALIPAY_SALE)
                {
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usAlipaySaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulAlipaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulAlipaySaleTotalAmount - atof(szTransAmt);
                    
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usAlipaySaleCount--;
                    srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulAlipaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulAlipaySaleTotalAmount - atof(szTransAmt); 		   
                }
				
			}
            else if(srTransRec.byOrgTransType == CB_PAY_TRANS)
            {
            	vdDebug_LogPrintf("CB_PAY_TRANS VOID!!!");
			
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCBPaySaleCount--;
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCBPaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCBPaySaleTotalAmount - atof(szTransAmt);
				
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCBPaySaleCount--;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount - atof(szTransAmt);			 
			
			}
            else if(srTransRec.byOrgTransType == OK_DOLLAR_TRANS)
            {
            	vdDebug_LogPrintf("OK_DOLLAR_TRANS VOID!!!");
			
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOKDSaleCount--;
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOKDSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOKDSaleTotalAmount - atof(szTransAmt);
				
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOKDSaleCount--;
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount - atof(szTransAmt);			 
			
			}

            else if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szTransAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szTransAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atof(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atof(szTransAmt);

            }
			else if(srTransRec.byOrgTransType == CASH_ADVANCE)
            {
				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashAdvCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashAdvTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashAdvTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount - atof(szTransAmt);
            }
            else if(srTransRec.byOrgTransType == SALE_TIP)
            {
                if (szOriginTipTrType == SALE_OFFLINE)
                {                    
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szOrgAmt);
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szOrgAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount - atof(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount - atof(szTransAmt);

                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atof(szTipAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atof(szTipAmt);
                }
                else if(szOriginTipTrType == SALE) 
                {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount - atof(szTipAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount - atof(szTipAmt);
                
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szTransAmt);            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szTransAmt);
                }
            }
            else if(srTransRec.byOrgTransType == REFUND)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount - atof(szTransAmt);
            }
			else if(srTransRec.byOrgTransType == MPU_VOID_PREAUTH_COMP)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCUPSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPSaleTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPSaleTotalAmount - atof(szTransAmt);
            } else if(srTransRec.byOrgTransType == PREAUTH_COMP){
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szTransAmt);
            }
            break;
        case MPU_VOID_PREAUTH:
        case MPU_VOID_PREAUTH_COMP:
        case CUP_VOID_PREAUTH:
            vdDebug_LogPrintf("Pre Auth Comp voided!");
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount + atof(szTransAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount++;
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount + atof(szTransAmt);

			/*if it is MPU trans void, will remain sale count*/
			//if (VS_TRUE == fGetMPUTrans())
			//	break;
			
            if(srTransRec.byOrgTransType == MPU_PREAUTH_COMP || srTransRec.byOrgTransType == CUP_PREAUTH_COMP)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCUPPreAuthCount--;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPPreAuthTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPPreAuthTotalAmount - atof(szTransAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPPreAuthCount--;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount - atof(szTransAmt);
            }
            break;
        case SALE_TIP:
            if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount + atof(szTipAmt);  
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount + atof(szTipAmt) ;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atof(szTipAmt);
            
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atof(szTipAmt);
            }
            else 
            {
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount + atof(szTipAmt);

				srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt) - atof(szOrgAmt);
				srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt) - atof(szOrgAmt); 		   

                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount++;
                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount + atof(szTipAmt);
            }
            break;
        case SALE_ADJUST:            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount - atof(szOrgAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount - atof(szOrgAmt);
            
            srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount + atof(szTransAmt);
            srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount = srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount + atof(szTransAmt);
            break;
        default:
            break;
    }

#if 0    
    vdDebug_LogPrintf("szTotalAmount=[%s],szTipAmount=[%s],szOrgAmount=[%s],.inCardType[%d]",szTransAmt,szTipAmt,szOrgAmt,srTransRec.inCardType);
    vdDebug_LogPrintf("CardTotal SaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal OfflCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOffSaleTotalAmount);
    vdDebug_LogPrintf("CardTotal RefdCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRefundCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRefundTotalAmount);            
    vdDebug_LogPrintf("CardTotal VoidCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("CardTotal TipCount [%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTipCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTipTotalAmount);
	vdDebug_LogPrintf("CardTotal CUPSaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCUPPreAuthCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCUPPreAuthTotalAmount);
	vdDebug_LogPrintf("CardTotal usCashAdvCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCashAdvCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCashAdvTotalAmount);
	vdDebug_LogPrintf("CardTotal usCBPaySaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usCBPaySaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulCBPaySaleTotalAmount);
	vdDebug_LogPrintf("CardTotal usOKDSaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usOKDSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulOKDSaleTotalAmount);

    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);
	vdDebug_LogPrintf("HostTotal CUPSaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPPreAuthCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount);  
    vdDebug_LogPrintf("HostTotal usCashAdvCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount);
    vdDebug_LogPrintf("HostTotal usCBPaySaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCBPaySaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount);
    vdDebug_LogPrintf("HostTotal usOKDSaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOKDSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount);

#endif

	vdDebug_LogPrintf("CardTotal usRepaymentCount[%d] ulRepaymentTotalAmount[%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usRepaymentCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulRepaymentTotalAmount);
    vdDebug_LogPrintf("HostTotal usRepaymentCount[%d] ulRepaymentTotalAmount[%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRepaymentCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRepaymentTotalAmount);

	vdDebug_LogPrintf("CardTotal usTopupCount[%d] ulTopupTotalAmount[%12.0f]", srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].usTopupCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[srTransRec.IITid].ulTopupTotalAmount);
    vdDebug_LogPrintf("HostTotal usTopupCount[%d] ulTopupTotalAmount[%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTopupCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTopupTotalAmount);

	if((inResult = inCTOS_SaveAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Save Total Rec. error");
		vdSetErrorMessage("Save Accum Error");
        return ST_ERROR;    
    }

   	inCTLOS_Updatepowrfail(PFR_BATCH_UPDATE_COMPLETE);

    vdMyEZLib_LogPrintf("total file saved successfully");

    return ST_SUCCESS;
}


int inCTOS_ClearAccumTotal(void)
{
	short shHostIndex;
	int inResult;
	ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
	
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;
	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	memset(&strFile,0,sizeof(strFile));
	inResult = inCTOS_CheckAndSelectMutipleMID();
    vdCTOS_GetAccumName(&strFile, &srAccumRec);
	
	if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdMyEZLib_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
        return ST_ERROR;
    }     
	
    return ST_SUCCESS;
	
}

void vdCTOS_SetBackupAccumFile(char *szOriFileName)
{
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
	char szBKAccumeFileName[30];
	ULONG ulFileSize;
	int inResult;
	
	memset(&srAccumRec,0x00,sizeof(srAccumRec));
	memset(&strFile,0x00,sizeof(strFile));
	memset(szBKAccumeFileName,0x00,sizeof(szBKAccumeFileName));
	strcpy(szBKAccumeFileName,szOriFileName);
	CTOS_FileGetSize(szOriFileName, &ulFileSize);
	strcat(szBKAccumeFileName,".BK");
		
	vdDebug_LogPrintf("delete old BK acculFileSize[%d],del[%d]szOriFileName[%s][%s]",ulFileSize,inResult,szOriFileName,szBKAccumeFileName);
	inCTOS_FileCopy(szOriFileName,szBKAccumeFileName,ulFileSize);

	vdDebug_LogPrintf("szOriFileName[%s]set BKAccum[%s]ulFileSize[%ld]",szOriFileName,szBKAccumeFileName,ulFileSize);
	CTOS_FileGetSize(szBKAccumeFileName, &ulFileSize);
	vdDebug_LogPrintf("BK Acc ulFileSize[%ld]",ulFileSize);

	
}

void vdCTOS_GetBackupAccumFile(STRUCT_FILE_SETTING *strFile, ACCUM_REC *strTotal, int HostIndex, int MITid)
{

	sprintf(strFile->szFileName, "ACC%02d%02d.total%s"
								, HostIndex
								, MITid,".BK");
	
	vdDebug_LogPrintf("get BKAccum[%s]",strFile->szFileName);

								
	strFile->bSeekType			 = d_SEEK_FROM_BEGINNING;
	strFile->bStorageType		 = d_STORAGE_FLASH ;
	strFile->fCloseFileNow		 = TRUE;
	strFile->ulRecSize			 = sizeof(ACCUM_REC);
	strFile->ptrRec 			 = strTotal;	
		
}

int inCTOS_ReadBKAccumTotal(ACCUM_REC *strTotal,int HostIndex, int MITid)
{
    STRUCT_FILE_SETTING strFile;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    vdCTOS_GetBackupAccumFile(&strFile, strTotal,HostIndex, MITid);

    return (inMyFile_RecRead(&strFile));    
}

int inCTOS_DeleteBKAccumTotal(ACCUM_REC *strTotal,int HostIndex, int MITid)
{
	
    STRUCT_FILE_SETTING strFile;
	int inResult;
    
    memset(&strFile, 0x00, sizeof(STRUCT_FILE_SETTING));
    memset(strTotal, 0x00, sizeof(ACCUM_REC));
    vdCTOS_GetBackupAccumFile(&strFile, strTotal,HostIndex, MITid);
	
	if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
    {
        vdMyEZLib_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
        return ST_ERROR;
    }     
	
}
int inCTOS_FileCopy(char *szSourceFile, char *szDesFile,ULONG ulFileSize)
{

	FILE *in = NULL;
	FILE *out = NULL;
	unsigned long ulFileHandle;
	unsigned long ulOutFileHandle;
	char ch;
	ULONG ulFileSizeTemp;
	char *pAccumeData;
	int inresult;
	
	CTOS_FileGetSize(szSourceFile, &ulFileSizeTemp);
    inresult = CTOS_FileOpen(szSourceFile, d_STORAGE_FLASH, &ulFileHandle);
	vdDebug_LogPrintf("CTOS_FileOpen[%d]ulFileSizeTemp[%d]",inresult,ulFileSizeTemp);

	inresult = CTOS_FileSeek(ulFileHandle, 0, d_SEEK_FROM_BEGINNING);
	
	vdDebug_LogPrintf("CTOS_FileSeek[%d]ulFileSizeTemp[%d]",inresult,ulFileSizeTemp);
	pAccumeData = (unsigned char *) malloc(ulFileSizeTemp+1);
	vdDebug_LogPrintf("ulFileSizeTemp[%d]",ulFileSizeTemp);
	
	if(pAccumeData == NULL)
	{	
		vdDisplayErrorMsg(1, 8,	"accume bk fail");
		free(pAccumeData);
		return FAIL;
	}
	
	
    inresult = CTOS_FileRead(ulFileHandle, pAccumeData, &ulFileSizeTemp);
	
	vdDebug_LogPrintf("CTOS_FileRead[%d]ulFileSizeTemp[%d]ulFileSize[%d]",inresult,ulFileSizeTemp,ulFileSize);

	if((inresult  = CTOS_FileClose(ulFileHandle)) != d_OK)
	{				 
		vdDebug_LogPrintf("[CTOS_FileClose]---FileClz err[%04x]", inresult);
		free(pAccumeData);

		return ST_ERROR;
	}

	inresult = CTOS_FileDelete(szDesFile);
	vdDebug_LogPrintf("CTOS_FileDelete[%d]",inresult);


	vdDebug_LogPrintf("33333[%d]",ulFileSize);

	CTOS_Delay(50);
	
	
	inresult = CTOS_FileOpen(szDesFile , d_STORAGE_FLASH , &ulOutFileHandle);
	vdDebug_LogPrintf("CTOS_FileOpen[%d]",inresult);
	if(inresult == d_OK)
	{																						
		/* Move the file pointer to a specific position. 
		* Move backward from the end of the file.		 */
		inresult = CTOS_FileSeek(ulOutFileHandle, 0, d_SEEK_FROM_BEGINNING);
		if (inresult != d_OK)
		{
			vdDebug_LogPrintf("[inMyFile_RecSave]---Rec Seek inResult[%04x]", inresult);
			CTOS_FileClose(ulOutFileHandle);
			free(pAccumeData);
			return ST_ERROR;																		
		}
		else
			;
		/* Write data into this opened file */
		inresult = CTOS_FileWrite(ulOutFileHandle ,pAccumeData ,ulFileSize); 										
		if (inresult != d_OK)
		{
			vdDebug_LogPrintf("[inMyFile_RecSave]---Rec Write error, inResult[%04x]", inresult);
			CTOS_FileClose(ulOutFileHandle);
			
			free(pAccumeData);
			return ST_ERROR; 
		}																		
		
		vdDebug_LogPrintf("[inMyFile_RecSave]---Write finish,  inResult[%d]",  inresult);
		
		{	 
			if((inresult  = CTOS_FileClose(ulOutFileHandle)) != d_OK)
			{				 
				vdDebug_LogPrintf("[inMyFile_RecSave]---FileClz err[%04x]", inresult);
				free(pAccumeData);

				return ST_ERROR;
			}
			else
				ulOutFileHandle = 0x00;

			vdDebug_LogPrintf("[inMyFile_RecSave]---User close immed.");
		}

        vdDebug_LogPrintf("[inMyFile_RecSave]---User did not close immed.");
	}

	free(pAccumeData);
	
	vdDebug_LogPrintf(("last--CTOS_FileWrite,inresult[%d]", (char*)&inresult));
} 




