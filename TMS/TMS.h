#ifndef __CTOSS_TMS_H__
#define __CTOSS_TMS_H__

int inCTOSS_SettlementCheckTMSDownloadRequest(void);

int inCTOSS_CheckIfPendingTMSDownload(void);
int inCTOSS_TMSChkBatchEmptyProcess(void);
int inCTOSS_TMSChkBatchEmpty(void);
int inCTOSS_TMSDownloadRequest(void);
int inCTOSS_TMSUploadFile(BYTE *baFileName);
void VdCTOSS_TMSUpload(void);


/*Terminal backup data, for TMS update*/
typedef struct tagTxnBackupData
{
	int 	inHDTid;	// match MMT HDTid
	int 	inMITid;	// match MMT MITid
	int		inBKUpdate;	// Update flag
	BYTE	szTraceNo[TRACE_NO_BCD_SIZE+1];
	BYTE	szBatchNo[BATCH_NO_ASC_SIZE+1];
	BYTE	szInvoiceNo[INVOICE_BCD_SIZE+1];
	
}TXN_BAK_DATA;


#define TBD_FILE_NAME		"TBD.BAK"

int inCTOSS_TMSBackupTxnData(void);
int inCTOSS_TMSRestoreTxnData(void);
int inCTOS_CheckTMSAfterAutoSettle(void);


#endif //end __CTOSS_TMS_H__
