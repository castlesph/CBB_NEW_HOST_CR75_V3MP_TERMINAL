
#ifndef ___MULTI_SHARLS_ECR___
#define ___MULTI_SHARLS_ECR___

typedef enum
{
	d_IPC_CMD_ECR_Initialize		    = 0x02,
    d_IPC_CMD_ECR_GetPackageData		= 0x08,
    d_IPC_CMD_ECR_SetPackageData	    = 0x09,
}IPC_ECRCMD_TYPES;

//thandar_added
typedef enum
{
	d_IPC_CMD_BT_OPEN		= 0x01,
    d_IPC_CMD_BT_LISTEN		= 0x02,
    d_IPC_CMD_BT_STATUS	    = 0x03,
    d_IPC_CMD_BT_RXREADY	= 0x04,
    d_IPC_CMD_BT_RXDATA		= 0x05,
    d_IPC_CMD_BT_TXDATA		= 0x06,
    d_IPC_CMD_BT_CLOSE		= 0x07,

	d_IPC_CMD_BT_STATUSEx	= 0x97,
}IPC_BTCMD_TYPES;

#define REQ_RESP_SIZE			1
#define TXN_CODE_SIZE			2
#define AMOUNT_VALUE_SIZE		12
#define CASHBACK_VALUE_SIZE		12
#define INVOICE_NUMBER_SIZE		6
#define DATE_SIZE              	6            /* Date in MMDDYY format        */
#define RET_REF_SIZE            26       /* Retrieval reference num size */
#define EXP_DATE_SIZE          	4            /* Expiry date MMYY             */


typedef struct tagECR_REQ
{
	char req_resp[REQ_RESP_SIZE + 1];
	char txn_code[TXN_CODE_SIZE + 1];
	char amount[AMOUNT_VALUE_SIZE + 1];
	char cashback[CASHBACK_VALUE_SIZE + 1];
	char inv_no[INVOICE_NUMBER_SIZE + 1];
	char date[DATE_SIZE + 1];
	char rref[RET_REF_SIZE+ 1];
	char Pan[PAN_SIZE + 1];
	char Expiry[EXP_DATE_SIZE+ 1];
	char prnt_no[3+ 1];
	char szCMD[50 + 1];
	char szTYPE[10 + 1];
	char szECRREF[26 + 1];
	char szAuthCode[AUTH_CODE_DIGITS+1];
	char szMID[MERCHANT_ID_BYTES+1];
	BYTE HDTid;
	char szTipAmount[AMOUNT_VALUE_SIZE + 1];
} ECR_REQ;


USHORT usCTOSS_ECRInitialize(void);
USHORT usCTOSS_ECR_DataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue);
USHORT usCTOSS_ECR_DataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue);

int inCTOS_ECR_ECHO(void);
int inCTOS_ECR_RETRIEVAL(void);

#endif  //end ___MULTI_SHARLS_ECR___

