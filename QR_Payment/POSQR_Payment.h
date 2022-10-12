
#ifndef _POSGCASH_H
#define	_POSGCASH_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ERR_ENC_INSUFF_BUFFER		-1101
#define OFFSET_LEN					28
#define EXPONENT_HDR_LEN			2
#define EXPONENT_LEN				3
#define OVERHEAD_LEN				4
#define POSITIVE_INIDICATOR_LEN		1


int inCTOS_MPU_CBPay(void);
int inCTOS_CBPayQRVoidFlowProcess(void);
unsigned char CBWaitKey(short Sec);

char *base64_encode(const char *data, size_t input_length,size_t *output_length);
int inCTOS_OKDGetTxnBaseAmount(void);
int inCTOS_CBB_OKDollar(void);
int inCTOS_OKDollarFlowProcess(void);
int inGenerateOKDQRCode(void);
int inCBOKDSend_request(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);
int inCTOS_OKDQRVoidFlowProcess(void);
int inOKDVoidTransaction(void);
//void vdOKDCreatePOSTData(char *szBuffer, int inType);
//void vdOKDCreatePOSTData(unsigned char* *send_data, int inType);
void vdWriteAES256_PlainText(int inKeySet, int inKeyIndex, unsigned char* uszPlainText);
int inEncryptData_with_AES256_ECB(int inKeySet, int inKeyIndex, unsigned char *uszPlainText, int inPlainTextLength, unsigned char *uszCipherText, int *inCipherTexttLength);
size_t read_OKDpay_callback(void *dest, size_t size, size_t nmemb, void *userp);
size_t write_OKDpay_callback(void *contents, size_t size, size_t nmemb, void *userp);
int inCTOSS_DisplayOKDQRCodeAndConfirm(char *intext);
//int inOKDPayGetTransactionStatus(void);
int inExtractField2(unsigned char *uszRecData, char *szField, char *szSearchString);
//int inOKDPayCancelTransaction(void);
int inCTOS_VoidOKDGetCustomerNO(void);
int inCTOS_VoidOKDGetCustomerPword(void);
void vdOKDCreatePOSTData2(unsigned char *send_data, int inType);
//int inAES128CBCEncryptTesting(void);
int inEncryptData_with_AES128_CBC(int inKeySet, int inKeyIndex, unsigned char *uszPlainText, int inPlainTextLength, unsigned char *uszInitialVektor, int inInitialVektorLength, unsigned char *uszCipherText, int *inCipherTextLength);
unsigned int PadData (unsigned char *ibuf, unsigned int ilen, int blksize);
void vdWriteAES256_PlainText(int inKeySet, int inKeyIndex, unsigned char* uszPlainText);
void vdDisplayErrorMsgOKD(int inColumn, int inRow,  char *msg);
int GetOKDPerformResutlMessage(int iResult);
int inOKDPayGetTransactionStatus2(void);
int inCBOKDSend_request2(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);
short vduiOKDAskConfirmContinue(void);
void vdOKDCreatePOSTData3(unsigned char *send_data, int inType);
int inCBOKDSend_request3(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);
//int inCBOKDSend_requestSaleFallBack(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);
int inCBOKDSend_requestSaleFallBack(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType, int DisplayComm);
//int inCBOKDSend_requestVoidFallBack(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);
//int inOKDPayGetTransactionStatusFallBack(int TimeOut);
int inOKDPayGetTransactionStatus(int DisplayResult);
int inOKDPayGetTransactionStatusFallBack(int DisplayResult, int DisplayComm);
int inCBOKDSend_request_Void(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);






#ifdef	__cplusplus
}
#endif

#endif

