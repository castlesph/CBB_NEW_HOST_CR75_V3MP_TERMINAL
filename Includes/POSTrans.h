
#ifndef ___POS_TRANS___
#define ___POS_TRANS___

#include <Typedef.h>

#include"..\Includes\POSTypedef.h"



//CVV define
#define 	CVV2_NONE							0
#define 	CVV2_MANUAL						1
#define 	CVV2_MSR						2
#define 	CVV2_MANUAL_MSR			3
#define 	CVV2_MANUAL_MSR_CHIP		4

#define		CVV2_SIZE				4


#define d_ONLINE_KEYSET     0x2000
#define d_ONLINE_KEYINDEX   0x0000
#define d_OFFLINE_KEYINDEX  0x0001

#define d_ONLINE_KEY        "\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11"

#define d_FONTSIZE 16

/* Card Types */

#define ADMIN_CARD          -1
#define CREDIT_CARD         0
#define DEBIT_CARD          1
//ALL_TOTALS is 2
#define NON_CARD            3
#define LOYALTY_CARD        6  
#define LOYCREDIT_CARD      7
#define EBT_CARD            8
#define PURCHASE_CARD       9 
#define COMMERCIAL_CARD     10
#define FLEET_CARD          11


#define CTOS_RET_OK						0
#define CTOS_RET_PARAM					-100
#define CTOS_RET_CERT_FAILED			-102
#define CTOS_RET_CALC_FAILED			-103
#define CTOS_PED_RSA_MAX				256


#define FORMATTED_EXP_DATE_SIZE   5             /* Date in MM/YY format     */
#define FORMATTED_PAN_SIZE       41             /* final pan size */
#define NULL_CH            ((char) 0)
#define  RIGHT             0x80
#define  CENTER             0x40
#define  LEFT             0x20
#define EXP_DATE_SIZE          4
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

/* Debit Allowed    */

#define NODEBIT     0
#define DEBIT       1


#define GET_EMV_TAG_AFTER_SELECT_APP		"575A5F305F3482845F249F099F085F2050"
#define GET_EMV_TAG_AFTER_1STAC		    "9F125F2A959A9C9F069F109F1A9F1E9F269F279F339F349F359F369F379F419F539F429F0D9F0E9F0F9F639F6E9F7C"
//#define GET_EMV_TAG_AFTER_2NDAC		    "959F109F269F279F34"
#define GET_EMV_TAG_AFTER_2NDAC		    "959F109F269F279F349B"

#define PRINT_EMV_TAGS_LIST               "50575F2A5F34828A8F91959A9B9C9F029F039F109F129F1A9F1B9F269F279F339F349F359F369F379F5B9F639F6E9F7C7172"



void vdSetECRTransactionFlg(BOOL flg);
BOOL fGetECRTransactionFlg(void);

unsigned char WaitKey(short Sec);
unsigned char WaitKeyNoTIMEOUT(void);


unsigned char struiApiGetStringSub(unsigned char	*strDisplay,short x, short y, unsigned char	*strOut, unsigned char	mode, short minlen, short maxlen);

USHORT getCardNO(OUT BYTE *baBuf);
USHORT getExpDate( OUT BYTE *baBuf);

USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS);
USHORT shCTOS_GetExpDate(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS);
USHORT ushCTOS_EMV_NewDataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue);
USHORT ushCTOS_EMV_NewTxnDataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue);

void vdCTOS_SyncHostDateTime(void);
void vdCTOS_TxnsBeginInit(void);
void vdCTOS_TransEndReset(void);
void vdCTOS_TransEndResetEx(void);

void vdCTOS_SetTransEntryMode(BYTE bEntryMode);
void vdCTOS_SetTransType(BYTE bTxnType);
void vdCTOS_ResetMagstripCardData(void);
int inCTOS_CheckEMVFallbackTimeAllow(char* szStartTime, char* szEndTime, int inAllowTime);
int inCTOS_CheckEMVFallback(void);
int inCTOS_CheckIssuerEnable(void);
int inCTOS_CheckTranAllowd(void);
int inCTOS_CheckMustSettle(void);
int inCTOS_PreConnect(void);
int inCTOS_GetCardFields(void);
int inCTOS_WaveGetCardFields(void);
int inGetIssuerRecord(int inIssuerNumber) ;
int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine);
short inCTOS_LoadCDTIndex(void);
int inCTOS_EMVCardReadProcess (void);
int inCTOS_ManualEntryProcess (BYTE *szPAN);
int inCTOS_SelectHost(void) ;
int inCTOS_CheckAndSelectMutipleMID(void);
int inCTOS_GetTxnPassword(void);
int inCTOS_GetCVV2(void);
int inCTOS_getCardCVV2(OUT BYTE *baBuf);
int inCTOS_EMVProcessing(void);
int inCTOS_EMVTCUpload(void);
int inCTOS_CheckTipAllowd(void);
int inCTOS_BootEMVTransInit(void);
void vdCTOS_FormatPAN(char *szFmt,char* szInPAN,char* szOutPAN);
//int inCTOS_EMV1stProcessing(void);
short shCTOS_EMVAppSelectedProcess(void);
short shCTOS_EMVSecondGenAC(BYTE *szIssuerScript, UINT inIssuerScriptlen);
int inCTOS_GetPubKey(const char *filename, unsigned char *modulus, int *mod_len, unsigned char *exponent, int *exp_len);
short shCTOS_EMVGetChipDataReady(void);
int inCTOS_showEMV_TagLog (void);
int inCTOS_FirstGenACGetAndSaveEMVData(void);
int inCTOS_SecondGenACGetAndSaveEMVData(void);
void vdCTOSS_GetMemoryStatus(char *Funname);
int inCTOSS_CheckMemoryStatus(void);
void vdCTOSS_GetAmt(void);
void vdCTOSS_SetAmt(BYTE *baAmount);
int inCTOSS_GetOnlinePIN(void);
void vdDispAppLabel(unsigned char *ucHex, int inLen, char *szOutStr);
void vdCTOSS_PrintFormatPAN(char* pchPAN,char* pchFmtPAN,int inFmtPANSize,int page);
int inCTOSS_TWKRSAFlow(void);
short shCTOS_SetMagstripCardTrackData(BYTE* baTk1Buf, USHORT usTk1Len, BYTE* baTk2Buf, USHORT usTk2Len, BYTE* baTk3Buf, USHORT usTk3Len);

int inMPU_GetAuthDate(void);
void vdCTOSS_RecoverRAM(void);
void vdGetMultipleAppRidding(void);

void vdSetECRTransactionFlg(BOOL flg);
BOOL fGetECRTransactionFlg(void);
void vdSetCUPTransFlag(BOOL flag);
BOOL fGetCUPTransFlag(void);
void vdSetCashAdvAppFlag(BOOL flag);
BOOL fGetCashAdvAppFlag(void);
void vdSetMPUTrans(BOOL flag);
BOOL fGetMPUTrans(void);
void vdSetMPUCard(BOOL flag);
BOOL fGetMPUCard(void);
BOOL fChkCUPCard(void);

void vdSetButtonFromIdle(BOOL fSet);
BOOL fCheckButtonFromIdle(void);
void vdSetCardFromIdle(BOOL fSet);
BOOL fCheckCardFromIdle(void);
void vdSetIdleScreenShow(BOOL fSet);
BOOL fGetIdleScreenShow(void);

int inCTOSS_RecoverRAM(void);
void vdChkCashAdvAppMenu(void);
int inCAV_MPU_UPI_GetOnlinePIN(void);;

int inMPU_UPI_GetOnlinePIN(void);
int inCTOS_SelectMPUHost(void);
int inMPU_GetOnlinePIN(void);
int inCTOS_SelectMPUHost(void);
int inCTOS_SelectJCBHost(void);
int inCTOS_SelectCUPHost(void);

int inCTOS_GetMPUCardFields(void);
int inCUP_GetOnlinePIN(void);

int inCTOS_GetTransOrgDate(void);
int inCTOS_GetAuthCode(void);
int inCTOS_GetTransRRN(void);

int inCTOS_SelectCBBHost(void);

void vdSetAppRidding(int inRiddingID);
int inCTOS_PreConnectEx(void);
int inCTOS_SelectHostAlipay(void);
int inCTOS_CheckMPUSignOn(void);

int inCTOSS_ManualSwitchSIM(void);
int inCTOS_WaveGetCardFieldsCTLS(void);
char *getTagVal(char* szData, char* szTag);
int inGetPosition(char* szData, char* szValue, int inValueLen, int inLen);

int inCTOS_DisplayCardInfo(USHORT usCardTypeLine, USHORT usPANLine);
int inCTOS_WaveGetCardFieldsDisc(void);
int inCTOS_WaveGetCardFieldsDiscPerc(void);

void vdCTOSS_MultiAPAnalysisECR(BYTE *inbuf, USHORT inlen);
int inCTOS_CAVSelectHost(void);



#endif //end ___POS_TRANS___

