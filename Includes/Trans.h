/* 
 * File:   TransFunc.h
 * Author: Administrator
 *
 * Created on 2012年8月23日, 下午 6:35
 */

#ifndef TRANSFUNC_H
#define	TRANSFUNC_H

#ifdef	__cplusplus
extern "C" {
#endif
//format amount 10+2 change all ULONG to DOUBLE
#define DOUBLE   double        		 // 8byte

typedef struct  
{
    USHORT usSaleCount;
    DOUBLE  ulSaleTotalAmount;
    USHORT usRefundCount;
    DOUBLE  ulRefundTotalAmount;
    USHORT usTipCount;
    DOUBLE  ulTipTotalAmount;   
    USHORT usVoidSaleCount;
    DOUBLE  ulVoidSaleTotalAmount;
    USHORT usOffSaleCount;
    DOUBLE  ulOffSaleTotalAmount; 
	USHORT usCashAdvCount;
    DOUBLE  ulCashAdvTotalAmount;
    USHORT usCUPSaleCount;
    DOUBLE  ulCUPSaleTotalAmount;
    USHORT usCUPPreAuthCount;
    DOUBLE  ulCUPPreAuthTotalAmount;
    USHORT usAlipaySaleCount;
    DOUBLE  ulAlipaySaleTotalAmount;
    USHORT usCBPaySaleCount;
    DOUBLE  ulCBPaySaleTotalAmount;	
    USHORT usTopupCount;
    DOUBLE  ulTopupTotalAmount;
    USHORT usRepaymentCount;
    DOUBLE  ulRepaymentTotalAmount;
    USHORT usOKDSaleCount;
    DOUBLE  ulOKDSaleTotalAmount;	
}STRUCT_TOTAL;

typedef struct
{
    STRUCT_TOTAL stCardTotal[20];
    STRUCT_TOTAL stHOSTTotal;
    USHORT usEMVTCCount;
}TRANS_TOTAL;


BYTE szOriginTipTrType;


typedef struct
{
    STRUCT_TOTAL stSettledCardTotal[10];
    BYTE	szSettledCardLabel[10][25] ;
    BYTE        szSettledDate[3+1];
    BYTE	szSettledBatchNo[3+1];
    BYTE        szSettledTime[3+1];
    BYTE	szSettledHostLabel[16] ;
    BYTE	szSettledMID[15+1];
    BYTE	szSettledTID[8+1];
}SETTLEDTRANS_TOTAL;



#ifdef	__cplusplus
}
#endif

#endif	/* TRANSFUNC_H */

