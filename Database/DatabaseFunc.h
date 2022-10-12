
#ifndef _DATABASEFUNC__H
#define	_DATABASEFUNC__H

#ifdef	__cplusplus
extern "C" {
#endif
#include "../Database/dct.h"
//#include "../Database/pas.h" 
#include "../Includes/POSTypedef.h"

#include "../Includes/Trans.h"
#include "../FileModule/myFileFunc.h"

#include "pas.h"

//��ע�⣬��Ҫ����ļ���Ĵ�Сд
#define DB_TERMINAL "/home/ap/pub/TERMINAL.S3DB"
#define DB_BATCH	"./fs_data/V5S_CBB.S3DB"
#define DB_EMV	"/home/ap/pub/EMV.S3DB"
#define DB_COM	"/home/ap/pub/COM.S3DB"

#define DB_MULTIAP	"/home/ap/pub/MULTIAP.S3DB"
#define DB_WAVE		"/home/ap/pub/WAVE.S3DB"

#define DB_ERM	"/home/ap/pub/ERM.S3DB"
#define DB_CTLS	"/home/ap/pub/CTLS.S3DB"
#define DB_EFT_LIB "/home/ap/pub/EFT.S3DB"

#define DB_MULTIAP_JOURNAL "/home/ap/pub/MULTIAP.S3DB-journal"

#define DB_TMLTMP	"/home/ap/pub/TMLTMP.S3DB"

int inAIDNumRecord(void);
int inHDTNumRecord(void);
int inTCTNumRecord(void);
int inCDTNumRecord(void);
int inRDTNumRecord(void);
int inEMVNumRecord(void);
int inAIDNumRecord(void);
int inMSGNumRecord(void);
int inIITNumRecord(void);
int inMITRead(int inSeekCnt);

int inCPTRead(int inSeekCnt);
int inCPTSave(int inSeekCnt);


int inHDTRead(int inSeekCnt);
int inHDTSave(int inSeekCnt);
int inHDTMAX(void); //@@IBR ADD 05102016
int inHDTUpdateTraceNum(void); //@@IBR ADD 20170214
int inCDTRead(int inSeekCnt);
int inCDTReadMulti(char *szPAN, int *inFindRecordNum);
int inCDTReadMultiByType(char *szPAN, int *inFindRecordNum, int inType);
int inCDTReadbyHost(int inSeekCnt, int *inMinCnt, int *inMaxCnt); //@@IBR ADD 10102016
int inCDTReadDisabled(int inSeekCnt, int inHDTid); //@@IBR ADD 10102016
int inCDTMAX(void);

int inEMVRead(int inSeekCnt);
int inEMVUpdateCurrency(void); //@@IBR ADD 20170116
int inAIDRead(int inSeekCnt);
int inAIDReadbyRID(int inSeekCnt, char * PRid);
int inTCTRead(int inSeekCnt);
int inTCTSave(int inSeekCnt);
int inTCTGetCurrMenuid(int inSeekCnt, int *inMenuID);
int inTCTUpdateMenuid(int inSeekCnt, int inMenuid);
int inTCPRead(int inSeekCnt);
int inTCPSave(int inSeekCnt);
int inIITRead(int inSeekCnt);
int inIITSave(int inSeekCnt);

int inCSTNumRecord(void);
int inCSTRead(int inSeekCnt);
int inCSTSave(int inSeekCnt);

int inMMTReadRecord(int inHDTid,int inMITid);

int inMMTNumRecord(void);

int inMMTReadNumofRecords(int inSeekCnt,int *inFindRecordNum);

int inMMTSave(int inSeekCnt);

int inPITNumRecord(void);
int inPITRead(int inSeekCnt);
int inPITSave(int inSeekCnt);

int inMSGResponseCodeRead(char* szMsg, int inMsgIndex, int inHostIndex);
int inMSGResponseCodeReadByActionCode(char* szMsg, int inActionCode, int inHostIndex);

int inDatabase_BatchDeleteHDTidMITid(void);
int inDatabase_BatchDelete(void);
int inDatabase_BatchInsert(TRANS_DATA_TABLE *transData);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchRead(TRANS_DATA_TABLE *transData, int inSeekCnt);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_BatchCheckDuplicateInvoice(char *hexInvoiceNo);
int inBatchNumRecord(void);
int inDatabase_BatchReadByHostidAndMITid(TRANS_DATA_TABLE *transData,int inHDTid,int inMITid);
int inBatchByMerchandHost(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID);
int inDatabase_BatchReadByTransId(TRANS_DATA_TABLE *transData, int inTransDataid);

int inMPUBatchNumRecord(int inHostIndex, int inMerchIndex, char *szBatchNo);

int inHDTReadHostName(char szHostName[][100], int inCPTID[]);
int inERMAP_Database_BatchDelete(void);
int inERMAP_Database_BatchInsert(ERM_TransData *strERMTransData);

int inMultiAP_Database_BatchRead(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchUpdate(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchDelete(void);
int inMultiAP_Database_BatchInsert(TRANS_DATA_TABLE *transData);

int inMultiAP_Database_EMVTransferDataInit(void);
int inMultiAP_Database_EMVTransferDataWrite(USHORT usDataLen, BYTE *bEMVData);
int inMultiAP_Database_EMVTransferDataRead(USHORT *usDataLen, BYTE *bEMVData);

int inMultiAP_Database_COM_Read(void);
int inMultiAP_Database_COM_Save(void);
int inMultiAP_Database_COM_Clear(void);
int inHDTReadByApname(char *szAPName);

int inMultiAP_Database_CTLS_Delete(void);
int inMultiAP_Database_CTLS_Read(CTLS_TransData *strCTLSTransData);
int inMultiAP_Database_CTLS_Insert(CTLS_TransData *strCTLSTransData);

int inEFTReadNumofRecords(int *inFindRecordNum);
int inEFTReadNumofRecordsByHostId(int *inFindRecordNum, int inHostId);


int inMMTReadSelectedData(int inSeekCnt);
int inMPTRead(int inSeekCnt);
int inMPTSave(int inSeekCnt);

int inHDTCurrencyUpdate(int inCurrency);
int inHDTCheckHostName(char szHostName[][100], int inCPTID[], int inType);

int inMAPMRead(int inSeekCnt);
int inTCTUpdateMenuid(int inSeekCnt, int inMenuid);
int inTCTGetCurrMenuid(int inSeekCnt, int *inMenuID);

int inDatabase_BatchSearchByRRN(TRANS_DATA_TABLE *transData, char *strRRN);

int inDatabase_WaveUpdate(char *szTableName, char *szTag, char *szTagValue);

int inIPSRead(int inSeekCnt);
int inIPSMAX(void);
int inHDTCheckByHostName(char *szHostName);

int inDB_CreateTable(char *pszTabName, int inStructSize);
int inDB_ReadTableRecByRecid(char *pszTabName, int inSeekCnt, char *pstBuf, int inStructSize);
int inDB_ReadTableRecBySeq(char *pszTabName, int inSeqNum, char *pstBuf, int inStructSize);
int inDB_SaveTableRecByRecid(char *pszTabName, int inSeekCnt, char *pstBuf, int inStructSize);
int inDB_AppendTableRec(char *pszTabName, char *pstBuf, int inStructSize);
int inDB_SaveTableRecBySeq(char *pszTabName, int inSeqNum, char *pstBuf, int inStructSize);
int inDB_DeleteTableRecByRecid(char *pszTabName, int inSeekCnt);
int inDB_DeleteTmpTableBySeq(char *pszTabName, int inSeqNum);
int inDB_RemoveTable(char *pszTabName);
int inDB_CheckTableExist(char *pszTabName);
int inDB_GetTableTotalRecNum(char *pszTabName);
int inDatabase_PreauthBatchInsert(TRANS_DATA_TABLE *transData);
int inDatabase_PreauthBatchDeletePerAuthCode(char *strAuthCode);
int inDatabase_PreauthBatchDeleteExpired(void);
int inDatabase_BatchSearchByInvNo(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_BatchUpdatePreauth(TRANS_DATA_TABLE *transData);
int inHDTReadEnabledHostID(int inHDTid[]);
int inHDTReadMaxTraceNo(void);
int inCDTReadEnabled(int inSeekCnt, int inHDTid);
int inCDTReadCDTid(int inCDTID[]);

int inOKDPayRead(int inSeekCnt);


#ifdef MINOR_CHANGES
int inPreauthNumRecord(void);
#endif

int inHDTUpdateHostEnable(int inSeekCnt, int fHostEnable);


#ifdef	__cplusplus
}
#endif

#endif	/* _DATABASEFUNC__H */

