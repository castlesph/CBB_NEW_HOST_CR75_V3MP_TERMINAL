
#ifndef ___POS_AUTH___
#define ___POS_AUTH___

int inCTOS_PREAUTH(void);
int inCTOS_PreAuthFlowProcess(void);
int inCTOS_PREAUTH_VOID(void);
int inCTOS_PREAUTH_COMPLETION(void);
int inCTOS_PREAUTHCOMP_VOID(void);

int inCTOS_PreAuthCompletionAuthCodeFlowProcess(void);
int inCTOS_PREAUTH_COMPLETION_AUTHCODE(void);
int inCTOS_PREAUTH_COMPLETION_ECR(void);
int inProcessTransactionEnquiry(void);

#define NORMAL_PREAUTHCOMP 0
#define PREAUTHCOMP_AUTH_CODE_ENTRY 1


typedef struct tagPreAuthTxnRec
{
	BYTE szPAN[PAN_SIZE + 1];
	BYTE szDateTime[8 + 1]; // YYYYMMDD
	BYTE szAmt[AMT_ASC_SIZE + 1];
	BYTE szAuthCode[AUTH_CODE_DIGITS + 1];
	BYTE szRRN[RRN_BYTES + 1];

}PRE_AUTH_TXN_REC;


#define PRE_AUTH_TXN_TAB			"PATXN"

#endif //end ___POS_AUTH___

