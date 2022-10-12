
#ifndef ___POS_HOST___
#define ___POS_HOST___




int inCTOS_GetTxnBaseAmount(void);
int inCTOS_GetTxnTipAmount(void);
int inCTOS_UpdateTxnTotalAmount(void);
int inCTOS_GetInvoice(void);
int inCTOS_CustComputeAndDispTotal(void);

int inCTOS_DisplayResponse(void);
int inCTOS_BatchSearch(void);

int inCTOS_GeneralGetInvoice(void);
int inCTOS_CheckTipadjust(void);
int inCTOS_CheckVOID(void);

int inCTOS_CheckVOID_IPP(void);
int inCTOS_CheckVOID_PREAUTH_COMP(void);

VS_BOOL fAmountLessThanFloorLimit(void) ;


int inCTOS_VoidSelectHost(void);
int inCTOS_ChkBatchEmpty(void);
int inCTOS_ConfirmInvAmt(void);
int inCTOS_GetTipAfjustAmount(void);
int inCTOS_GetOffApproveNO(void);
int inCTOS_SettlementSelectAndLoadHost(void);

int inCTOS_IPP_SettlementSelectAndLoadHost(void);

int inCTOS_LoadCDTandIIT(void);
int inCTOS_DisplayBatchRecordDetail(int inType);
int inCTOS_SettlementClearBathAndAccum(void);

int inCTOS_DisplayBatchTotal(void);

int inCTOSS_DeleteAdviceByINV(BYTE *szInvoiceNo);
int inCTOSS_BatchCheckDuplicateInvoice(void);
void vdCTOSS_GetBatchTotal(DOUBLE *ulSaleTotalAmount,DOUBLE  *ulRefundTotalAmount,DOUBLE  *ulTotalAmount);
int inCTOSS_CheckExceedMaxAMT(char *szAmount);


int inMPU_DisplayBatchTotalEx(void);
void vdCTOS_SetDateTrans(void); //@@IBR ADD 20170203

int inCTOS_ConfirmAmt(void);
int inCTOS_GetSaleAdjustAmount(void);
int inCTOS_DisplayBatchTotalEx(void);

void vdCTOS_SetNextAutoSettle(void);
void vdCheckTimeAutoSettle(void);

int inCTOS_CheckVOID_MPUPreAuth(void);

int inCTOS_CheckAllowCardVer(void);
int inCTOS_CheckAllowPreAuth(void);
void vdCTOS_Pad_String(char* str,int padlen,char padval,int padtype) ;
int inConfirmQRCode(void);

int inCTOS_CheckPreCompAuth(void);
int inCTOS_CheckApprovalCode(void);
int inCTOS_CheckLast4Digit(void);
int inRetryTransStatusEnquiry(void);
int inCTOS_BatchSearchByInvNo(void);
int inCTOS_DisplayBatchTotalEx2(void);

int inCTOSS_CheckVEPQPS(int flag);
int inCTOSS_CheckFlagVEPSQPS(void );

#ifdef DISCOUNT_FEATURE
int inCTOS_GetFixedAmount(void);
int inCTOS_UpdateTxnTotalAmountDisc(void);
int inCTOS_GetPercentage(void);
int inCTOS_UpdateTxnTotalAmountDiscPerc(void);
#endif
int inCTOS_ConfirmTipAfjustAmount(void);
int inCTOS_CAVGetTxnBaseAmount(void);

#endif //end ___POS_HOST___

