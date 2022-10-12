#define IPP_TDES_KEY_SIZE 16
#define IPP_DISPLAY_SIZE 25
#define IPP_PIN_BLOCK 8

typedef struct
{
	UINT HDTid;
	int inPIN3des;
	BYTE szPINKey[IPP_TDES_KEY_SIZE];
	BYTE szMACKey[IPP_TDES_KEY_SIZE];
	int inMinPINDigit;
	int inMaxPINDigit;
	USHORT usKeySet;
	USHORT usKeyIndex;
	BYTE szDisplayLine1[IPP_DISPLAY_SIZE];
	BYTE szDisplayLine2[IPP_DISPLAY_SIZE];
	BYTE szDisplayProcessing[IPP_DISPLAY_SIZE];
}STRUCT_DCT;

STRUCT_DCT strDCT;

int inDCTRead(int inSeekCnt);
int inDCTSave(int inSeekCnt);
int inDCTNumRecord(void);


