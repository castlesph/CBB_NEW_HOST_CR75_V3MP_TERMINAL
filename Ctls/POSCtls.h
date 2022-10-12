
#ifndef _POSCTLS_H
#define	_POSCTLS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <emv_cl.h>

#define NO_CTLS		0
#define CTLS_INTERNAL	1
#define CTLS_EXTERNAL	2
#define CTLS_V3_SHARECTLS	3
#define CTLS_V3_INT_SHARECTLS	4



#define	d_BAUDRATE			38400


#define MAX_AID_NO 15
typedef struct 
{
	int		number;
	int		EMVid[MAX_AID_NO];
	BYTE    pbAID[MAX_AID_NO][16];
	int     inAIDLen[MAX_AID_NO];
} STRUCT_WAVE_DATA;

STRUCT_WAVE_DATA szWave_data;



#define SHARE_CTLS_CHK_NON_EMVCLCARD			"IN01"

#define SHARE_CTLS_SUB_IPC                "IPC"
#define SHARE_CTLS_NONEMVCARD			   "1"


/* ==========================================================================
 * FUNCTION NAME: inCTOS_CLMOpen
 * DESCRIPTION:   Initialize the communication between terminal and CLM
 * ========================================================================== */
ULONG inCTOSS_CLMOpen(void);

/* ==========================================================================
 * FUNCTION NAME: vdCTOS_GetCLMVersion
 * DESCRIPTION:   Get Version
 * 
 * [out]cVer : format is "EMVCLvXXXX-VWDLEAPI vX.X-CAS-V5CLM-VXXXX-CXXXX-LXXXX-DXXXX-SXXXX-MXXXX" (70 Bytes)
 * ========================================================================== */
void vdCTOSS_GetCLMVersion(char *cVer);

/* ==========================================================================
 * FUNCTION NAME: vdCTOS_CLMClose
 * DESCRIPTION:   Close the communication between terminal and CLM
 * ========================================================================== */
void vdCTOSS_CLMClose(void);

/* ==========================================================================
 * FUNCTION NAME: inCTOS_CLMCancelTransaction
 * DESCRIPTION:   Abort the current transaction. 
                         This API is used with inCTOS_CLMInitTransaction and inCTOS_CLMPollTransaction.
                         After send EMVCL_CancelTransaction API, please check return code 
                         by inCTOS_CLMPollTransaction.
 * ========================================================================== */
ULONG inCTOSS_CLMCancelTransaction(void);

/* ==========================================================================
 * Transaction Related Data include : the all parameter must ASCII string
 *	szAmount    Tag 9F02   (Amount Authorized(Numeric))
 *	szOtherAmt  Tag 9F03   (Amount Other(Numeric))
 *	szTransType Tag 9C     (Transaction Type)
 *	szCatgCode  Tag 9F53   (Transaction Category Code)
 *	szCurrCode  Tag 5F2A   (Transaction Currency Code)
 * ========================================================================== */
ULONG inCTOSS_CLMInitTransaction(BYTE *szAmount,BYTE *szOtherAmt,BYTE *szTransType,BYTE *szCatgCode,BYTE *szCurrCode);

/* ==========================================================================
 * FUNCTION NAME: inCTOS_CLMPollTransaction
 * DESCRIPTION:   Receive the result of transaction strated by inCTOS_CLMInitTransaction. 
 * 
 * [out]stRCData : transaction response data
 * [in]ulMS : RFU
 * ========================================================================== */
ULONG inCTOSS_CLMPollTransaction(EMVCL_RC_DATA_EX *stRCDataEx,ULONG ulMS);

/* ==========================================================================
 * FUNCTION NAME: inCTOS_CLMAnalyzeTransaction
 * DESCRIPTION:   Analyze transaction Response Data EMV_RC_DATA_EX
 * 
 * [in]stRCData : transaction response data
 * [out]stRCDataAnalyze : analyzed result
 * ========================================================================== */
void vdCTOSS_CLMAnalyzeTransaction(EMVCL_RC_DATA_EX *stRCData, EMVCL_RC_DATA_ANALYZE *stRCDataAnalyze);

ULONG inCTOSS_CLMSetDateTime(BYTE *baDateTime);
ULONG inCTOSS_SetTimeOut(ULONG ulMS);

void vdCTOSS_WaveGetEMVData(BYTE *baChipData, USHORT usChipDataLen);
void vdCTOSS_WaveGetCardLable(BYTE SID);
int inCTOSS_WaveAnalyzeTransaction(EMVCL_RC_DATA_EX *stRCDataEx);
void vdCTOSS_WaveCheckRtCode(ULONG ulAPRtn);
ULONG inCTOSS_CLMOpenAndGetVersion(void);
USHORT usCTOSS_CtlsV3Trans(BYTE *szAmount,BYTE *szOtherAmt,BYTE *szTransType,BYTE *szCatgCode,BYTE *szCurrCode,EMVCL_RC_DATA_EX *stRCDataEx);
int inCTOSS_V3AnalyzeTransaction(EMVCL_RC_DATA_EX *stRCDataEx);

EMVCL_RC_DATA_ANALYZE stRCDataAnalyze;
#define d_CEPAS_READ_BALANCE_CARD_PRESENT_TIMEOUT             30000 // 5 seconds // V000.11P Modified from 30 sec to 5 sec.

#ifdef	__cplusplus
}
#endif

#endif	/* _POSCTLS_H */

