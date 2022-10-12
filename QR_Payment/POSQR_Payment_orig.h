
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

#ifdef OK_DOLLAR_FEATURE
int GetOKDPerformResutlMessage(int iResult);

unsigned int PadData (unsigned char *ibuf, unsigned int ilen, int blksize);
char *base64_encode(const char *data, size_t input_length, size_t *output_length);
int inExtractField2(unsigned char *uszRecData, char *szField, char *szSearchString);
size_t write_OKDpay_callback(void *contents, size_t size, size_t nmemb, void *userp);
size_t read_OKDpay_callback(void *dest, size_t size, size_t nmemb, void *userp);


int inCTOS_CBB_OKDollar(void);
int inCTOS_OKDollarFlowProcess(void);
int inGenerateOKDQRCode(void);
int inOKDPayGetTransactionStatus3(void);
int inOKDPayGetTransactionStatus2(void);

int inOKDPayGetTransactionStatus(void);
int inCBOKDSend_request(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);
int inCBOKDSend_request2(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);

int inCBOKDSend_request3(struct MemoryStruct *contents,struct MemoryStruct *response, int TransType);
int inCTOSS_DisplayOKDQRCodeAndConfirm(char *intext);
void vdOKDCreatePOSTData3(unsigned char *send_data, int inType);
void vdWriteAES128KEY(int inKeySet, int inKeyIndex, unsigned char* uszPlainText);
void vdWriteAES256_PlainText(int inKeySet, int inKeyIndex, unsigned char* uszPlainText);


int inEncryptData_with_AES128_CBC(int inKeySet, int inKeyIndex, unsigned char *uszPlainText, int inPlainTextLength, unsigned char *uszInitialVektor, int inInitialVektorLength, unsigned char *uszCipherText, int *inCipherTextLength);
int inOKDVoidTransaction(void);

int inCTOS_OKDQRVoidFlowProcess(void);
int inCTOS_VoidOKDGetCustomerNO(void);
int inCTOS_VoidOKDGetCustomerPword(void);
short vduiOKDAskConfirmContinue(void);

#endif



#ifdef	__cplusplus
}
#endif

#endif

