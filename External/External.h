
#ifndef __EXTERNAL_H__
#define	__EXTERNAL_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*for EMV online PIN, 
better handle in share EMV but we need the para from main application*/
typedef struct tagOnlinePINParam
{
	BYTE byPinPadMode;
	BYTE byPinPadType;
	BYTE byPinPadPort;
	BYTE szPAN[19+1];
	BYTE szTransTitle[40+1];
	USHORT ushKeySet;
	USHORT ushKeyIdx;
	long lnTxnAmt;
	BYTE szPINKey[40+1];//szFmtAmtStr[40+1];
	int inMinDigit;
	int inMaxDigit;
	BYTE HDTid;//byAcctType;
	BYTE szPINBlock[8+1];
	BYTE szNewPINBlock[8+1];	
	BYTE szVerifyNewPINBlock[8+1];	
	int inPINEntered;
	int inSupportPINBypass;
	BYTE baKSN[10];
	BYTE baRFU1[64];
	BYTE baRFU2[64];
}OLPIN_PARAM;

OLPIN_PARAM	stOLPinParam;


/*for EMV offline PIN, 
better handle in share EMV but we need the para from main application*/
typedef struct tagOfflinePINParam
{
	BYTE byPinPadMode;
	BYTE byPinPadType;
	BYTE byPinPadPort;
	BYTE szPAN[19+1];
	BYTE szTransTitle[40+1];
	USHORT ushKeySet;
	USHORT ushKeyIdx;
	long lnTxnAmt;
	BYTE szFmtAmtStr[40+1];
	int inMinDigit;
	int inMaxDigit;
	BYTE byAcctType;
	BYTE szPINBlock[8+1];
	BYTE szNewPINBlock[8+1];	
	BYTE szVerifyNewPINBlock[8+1];	
	int inPINEntered;
	int inSupportPINBypass;
	BYTE baKSN[10];
	BYTE baRFU1[64];
	BYTE baRFU2[64];
}OFPIN_PARAM;

OFPIN_PARAM	stOFPinParam;


typedef struct
{	 
	 int IPC_EVENT_ID;
	 int usDataLen1;
	 int usDataLen2;
	 int usDataLen3;
	 BYTE pbaData1[200+1];
	 BYTE pbaData2[1024+1];
	 BYTE pbaData3[512+1];
} EMV_Trans;
EMV_Trans strEMV_Trans;


#define DB_SIGN_BMP "signtest.bmp"
#define DB_SIGN_BMP_GZ "signtest.bmp.gz"



USHORT usCTOSS_CtlsV3TransEXT(BYTE *inbuf, USHORT inlen);
int inCTOSS_EXTSignatureEXT(void);
int inCTOSS_EXTSignature(void);

int inCTOSS_PinPadLoad_3DES_TMK_Plaintext(BYTE *inbuf, USHORT inlen);
int inCTOSS_PinPadGetIPPPin(BYTE *inbuf, USHORT inlen);

int inCTOSS_EXTGetIPPPin(void);


int inCTOSS_EXTLoad_3DES_TMK_Plaintext(char *szKeyData,int HDTid);


int inCTOS_WaveGetCardFieldsEXT(BYTE *inbuf, USHORT inlen);
int inCTOS_WaveGetCardFieldsExternal(void);

int inCTOS_EMVGetCardEXT(BYTE *inbuf, USHORT inlen);
int inCTOS_EMVGetCardExternal(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen);


#ifdef	__cplusplus
}
#endif

#endif

