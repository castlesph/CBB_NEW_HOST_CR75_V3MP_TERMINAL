/* 
 * File:   Print.h
 * Author: ASUS
 *
 * Created on 2012年8月25日, 上午 1:35
 */

#ifndef PRINT_H
#define	PRINT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../Includes/Trans.h"

#define d_LINE_DOT 12
#define d_LINE_SIZE 44
#define d_FIRST_PAGE 1
#define d_SECOND_PAGE 2

#define TTF_FONT_REGULAR			0
#define TTF_FONT_ITALIC			1
#define TTF_FONT_BOLD				2
#define TTF_FONT_BOLD_ITALIC		3


#define NORMAL_SIZE	1
#define DOUBLE_SIZE	2

#define PRINT_SALE_REPORT 0
#define PRINT_REFUNT_REPORT 1
#define PRINT_SETTLE_REPORT 2

#define DIVIDING_LINE 1
#define DIVIDING_LINE_DOUBLE 2    
    
#define PRINT_HOST_TOTAL 0
#define PRINT_CARD_TOTAL 1    

#define SETTLE_PRINT 1
#define TOTAL_REPORT 0

#define PRINT_CARD_MASKING_1 0
#define PRINT_CARD_MASKING_2 1
#define PRINT_CARD_MASKING_3 2
#define PRINT_CARD_MASKING_4 3

#define ONE_LINE_DOT	"\n"

short printCheckPaper(void);  
USHORT printDateTime(void);
USHORT ushCTOS_printReceipt(void);
USHORT ushCTOS_PrintAgreement(void);
USHORT batchAllRecord(BOOL isPrint);
void vdPrintEMVTags(void);
void vdPrintCenter(unsigned char *strIn);
USHORT printBatchInvoiceNO(void);
void setTotalCountandAmountCardWise(int i);
void PrintTotalCountandAmountCardWise (int inReportType, USHORT usSaleCount, 
    double ulSaleTotalAmount,    
    USHORT usSaleAloneCount,
    double  ulSaleAloneTotalAmount,
    USHORT usRefundCount,
    double  ulRefundTotalAmount,    
    USHORT usVoidSaleCount,
    double  ulVoidSaleTotalAmount,
    USHORT usOffSaleCount,
    double  ulOffSaleTotalAmount);

void vdPrintTitleCenter(unsigned char *strIn);
void vdPrintTerminalConfig(void);
void printCardHolderName(void);
void vdSetGolbFontAttrib(USHORT FontSize, USHORT X_Zoom, USHORT Y_Zoom, USHORT X_Space, USHORT Y_Space);

USHORT ushCTOS_printAll(int page);
USHORT ushCTOS_PrintHeader(int page);
USHORT ushCTOS_PrintBody(int page);
USHORT ushCTOS_PrintFooter(int page);
int inCTOS_rePrintReceipt(void);
int inCTOS_PrintSettleReport(void);

USHORT ushCTOS_printReceipt(void);
int inCTOS_REPRINT_ANY(void);
int inCTOS_REPRINT_LAST(void);
int inCTOS_PRINTF_SUMMARY(void);
int inCTOS_PRINTF_DETAIL(void);
int inCTOS_REPRINTF_LAST_SETTLEMENT(void);

void vdCTOS_PrintDetailReportForSettleAll(void);

int inCTOS_rePrintLastReceipt(void);
void vdCTOS_PrintSummaryReport(void);
void vdCTOS_PrintDetailReport(void);
USHORT ushCTOS_printErrorReceipt(void);
USHORT ushCTOS_printBatchRecordHeader(void);
USHORT ushCTOS_printBatchRecordFooter(void);
USHORT ushCTOS_GetFontInfor(void);
USHORT ushCTOS_ReprintLastSettleReport(void);
void vdCTOSS_PrinterBMPPic(unsigned int usX,unsigned int y, const char *path);
void vdCTOSS_Print2QRCodeOneLine(char *intext);
int inCTOSS_CapturePrinterQRCodeBuffer(unsigned char* pchTemp, int endflag);

void vdCTOS_PrintAccumeByHostAndCard (int inReportType,
	USHORT usSaleCount, 
    double ulSaleTotalAmount,    
    USHORT usRefundCount,
    double  ulRefundTotalAmount,    
    USHORT usVoidSaleCount,
    double  ulVoidSaleTotalAmount,
    USHORT usOffSaleCount,
    double  ulOffSaleTotalAmount,
	USHORT usCashAdvCount, 
    double ulCashAdvTotalAmount,
    USHORT usCUPSaleCount, 
    double ulCUPSaleTotalAmount,
    USHORT usCUPPreAuthCount, 
    double ulCUPPreAuthTotalAmount, 
    USHORT usCBPaySaleCount,
    double ulCBPaySaleTotalAmount,    
    USHORT usOKDSaleCount,
    double ulOKDSaleTotalAmount,            
    STRUCT_TOTAL Totals 
);

int inCTOS_SelectFont(int inFontMode,int inFontSize ,int inFontStyle,char * szFontName);
int inPrintISOPacket(int fSendPacket, unsigned char *pucMessage, int inLen);

int ushCTOS_ePadSignature(void);
int ushCTOS_ePadPrintSignature(void);	
int ushCTOS_ClearePadSignature(void);
int inCTOSS_PrintERMInfo(void);


int inPrintISOfield(char *szFieldname,unsigned char *pucMessage, int inLen, int fHexFlag);
void vdCTOSS_PrinterBMPPic_Pub(unsigned int usX,unsigned int y, const char *path);
void vdCTOSS_RemovePrinterBMPPic_Pub(const char *path);

void vdCTOS_PrintTerminalConfigNew(void); //@@IBR ADD 10102016
USHORT ushCTOS_MPUPrintErrorReceipt(void); //@@IBR ADD 20161205
USHORT ushCTOS_CUPPrintErrorReceipt(void); //@@IBR ADD 20170130

int inMPU_PrintSettleReport(void);

int inCTOS_MPU_PRINTF_SUMMARY(void); //@@IBR ADD 20170222
int inCTOS_MPU_PRINTF_DETAIL(void); //@@IBR ADD 20170222

int inCTOS_MPU_REPRINT_ANY(void);
int inCTOS_MPU_REPRINT_LAST(void);
int inCTOS_MPU_REPRINTF_LAST_SETTLEMENT(void);
void vdCTOS_SetFontType(char *chFontType);
void vdDebugPrint(BYTE *byPrint, int inPrintLen);

USHORT ushCTOS_TerminalConfigPrintHeader(void);
void vdPrintCenterTerminalConfig(unsigned char *strIn);
USHORT ushCTOS_PrintBodyAlipay(int page);
USHORT ushCTOS_PrintFooterAlipay(int page);



//for cbpay
USHORT ushCTOS_PrintBodyCBPay(int page);
USHORT ushCTOS_PrintFooterCBPay(int page);
USHORT ushCTOS_PrintAgreementCBPay(void);// for Case 1595, 1600, 1603 , 1605
USHORT printTIDMIDCBPay(void);
USHORT ushCTOS_printBatchRecordHeaderCBPay(void);
USHORT ushCTOS_printBatchRecordFooterCBPay(void);

//FOR OKD
USHORT ushCTOS_printBatchRecordHeaderOKD(void);
USHORT ushCTOS_printBatchRecordFooterOKD(void);
USHORT printTIDMIDOKD(void);
USHORT ushCTOS_PrintBodyOKD2(int page);
USHORT ushCTOS_PrintFooterOKD(int page);
USHORT ushCTOS_PrintAgreementOKD(void);
void vdCTOS_PrintAccumeByHostAndCardOKD (int inReportType,
	USHORT usSaleCount, 
    double ulSaleTotalAmount,    
    USHORT usRefundCount,
    double  ulRefundTotalAmount,    
    USHORT usVoidSaleCount,
    double  ulVoidSaleTotalAmount,
    USHORT usOffSaleCount,
    double  ulOffSaleTotalAmount,
    USHORT usCashAdvCount, 
    double ulCashAdvTotalAmount,
    USHORT usCUPSaleCount, 
    double ulCUPSaleTotalAmount,
    USHORT usCUPPreAuthCount, 
    double ulCUPPreAuthTotalAmount, 
    USHORT usCBPaySaleCount,
    double ulCBPaySaleTotalAmount,
    USHORT usRepaymentCount,
    double ulRepaymentTotalAmount,    
    USHORT usTopupCount,
    double ulTopupTotalAmount,    
    USHORT usOKDSaleCount,
    double ulOKDSaleTotalAmount,    
    STRUCT_TOTAL Totals);


void vdHeaderPrintCenterTTF(unsigned char *strIn);
int inCTOS_MPU_PRINTF_DETAIL_ALL(void);
void vdCTOS_PrintDetailReportAll(void);


USHORT ushCTOS_PrintHeaderParkingFee(int page);
USHORT ushCTOS_PrintBodyParkingFee(int page);
void vdPrintTerminalConfigNew(void);
USHORT ushCTOS_PrintFooterSignature(int page);
USHORT ushCTOS_PrintBodyCBPay2(int page);// for Case 1595, 1600, 1603 , 1605

#ifdef	__cplusplus
}
#endif

#endif	/* PRINT_H */

