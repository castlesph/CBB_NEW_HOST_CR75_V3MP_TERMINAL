#ifndef HTLESRC_H
#define HTLESRC_H

#define PUBLICKEY_FILENAME		"pub"
#define PUBLICKEY_FILE_EXTN 	".dat"
#define TMK_FILE_EXTN			".tmk"
#define TWK_FILE_EXTN			".twk"
#define PTLEN   				24
#define CTLEN  					128

// Data Element Tags
#define CTC_Tag					0x27    // Card Transaction Counter
#define CTR_Bal_Tag				0x70   // Balance
#define REC_PERS_Tag      		0x40 // Personalization Data
#define SERNO_Tag				0x44   // Serial No. Tag
#define CTC_FIELD_SIZE   		2
#define BAL_FIELD_SIZE  		3

// Card Function TAGS
#define CLA_NORM				0x90
#define CLA_SECURE				0x94
#define CLA_SELECT				0x00

#define INS_SELECT				0xA4

#define INS_VER					0x01
#define INS_STR					0x10
#define INS_END					0x11
#define INS_PIN					0x12
#define INS_CARD				0x13
#define INS_KEK					0x14
#define INS_TMK					0x15
#define INS_TMKD				0x16
#define INS_PINC				0x17

#define P1_SESS					0x01
#define P1_SELECT				0x04
#define P1_TE_PIN				0x01

#define CARD_SUCCESS			0x9000

#define SA_LOGGED_ON			1
#define TE_LOGGED_ON			2

#define ERR_SELECT_AID			-2

#define MEM_ALLOC(q)			calloc(1, q)

#define HTLE_FIELD_62					62
#define HTLE_FIELD62_SIZE				78
//#define HTLE_FIELD62_POSITION			FIELD62_POSITION


typedef struct _TMK_RES_DATA
{
		unsigned char resTLEind[4];
		unsigned char resVer[2];
		unsigned char resTMKid[4];
		unsigned char resTMKe[16];
		unsigned char resKCVTMK[8];
} TMK_RES_DATA;

typedef struct _TMK_RSA_RES_DATA
{
		unsigned char resTLEind[4];
		unsigned char resVer[2];
		unsigned char resDownType;
		unsigned char resRespType;
		unsigned char resLTMKAcqId[3];
		unsigned char resKMSRSAKey[282];
} TMK_RSA_RES_DATA;

typedef struct _HTLE_CARD_DATA
{
		unsigned char szCardSerial[8];
		unsigned char szKEKid[4];
		unsigned char eRANsim[16];
		unsigned char szKCVsim[4];
		unsigned char szTMK[16];

} HTLE_CARD_DATA;


typedef struct _TWK_RES_DATA
{
		unsigned char resTLEind[4];
		unsigned char resVer[2];
		unsigned char resTWKid[4];
		unsigned char resTWKeDEK[16];
		unsigned char resTWKeMAK[16];
		unsigned char resKCVDEK[8];
		unsigned char resKCVMAK[8];

} TWK_RES_DATA;

typedef struct _TWK_RSA_RES_DATA
{
		unsigned char resTLEind[4];
		unsigned char resVer[2];
		unsigned char resRespType[1];
		unsigned char resTWKid[4];
		unsigned char resTWKeDEK[16];
		unsigned char resTWKeMAK[16];
		unsigned char resKCVDEK[8];
		unsigned char resKCVMAK[8];
		unsigned char resRenewedAcqId[3];
} TWK_RSA_RES_DATA;

typedef struct _TWK_REQ_DATA
{
		unsigned char reqTLEind[4];
		unsigned char reqVer[2];
		unsigned char reqAcqId[3];
		unsigned char reqTermId[8];
		unsigned char reqTMKId[4];
		unsigned char reqTWKId[4];
} TWK_REQ_DATA;

typedef struct _TWK_RSA_REQ_DATA
{
		unsigned char reqTLEind[4];
		unsigned char reqVer[2];
		unsigned char reqDownType[1];
		unsigned char reqLTMKAcqId[3];
		unsigned char reqAcqId[3];
		unsigned char reqTermId[8];
		unsigned char reqVendorId[8];
		unsigned char reqTMKId[4];
		unsigned char reqTWKId[4];
} TWK_RSA_REQ_DATA;

typedef struct _TLED_HEADER
{
		char szTLE_Indicator[4];
		char szVersion[2];
		char szAcquirerId[3];
		char szTerminalId[8];
		char szEncryptionMethod[3];
		char szTWK_ID[4];
		char szEncryptionCounter[4];
		char szProtectTextLength[3];
		char szNA[5];
} TLED_HEADER;


typedef struct _TWK_FILE
{
		unsigned char szTWKid[4];
		unsigned char szTWKeDEK[16];
		unsigned char szTWKeMAK[16];
} TWK_FILE;


typedef struct _TMK_FILE
{
		unsigned char szTMKid[4];
		unsigned char szTMKey[16];
} TMK_FILE;

int SendCardCommandAID(void);


unsigned int inReadRecordJavaCard(
		unsigned char bCLA,
		unsigned char bINS,
		unsigned char bP1,
		unsigned char bP2,
		int inSize,
		unsigned char *cSendBuffer,
		unsigned char bLOUT,
		unsigned char *cRecvBuffer);


int Read_PublicKeyFile(
		char * inputfile,
		unsigned char * HTLE_MOD,
		unsigned char * HTLE_EXP);


int in3DES_Encrypt(
		unsigned char *cSendBuffer,
		unsigned char *sessionKEY,
		unsigned char * uchOut3DESe);


int in3DES_CBC_Encrypt(
		unsigned char *cSendBuffer,
		int inSize,
		unsigned char *sessionKEY,
		unsigned char * uchOut3DESe);

int Read_FilePublicKey(char * inputfile, unsigned char * HTLE_MOD, unsigned char * HTLE_EXP);

void vdHTLEDisplayMSG(unsigned int usCondition);

int inGenSSK(unsigned char * uchSSK);


int inSaveTMK(void);
int inTWKField62Data(unsigned char *pbtBuff, unsigned short *pusLen);
int inSaveTWK(void);
int inPrintLoadKeysuccess(void);

char *szGetAqcID(void);
char *szGetPOSVendorID(void);
void vdCTOSS_EFTSECKeyCard(void);
void vdHTLESelectloadMethods(void);
void vdTLELoadMasterKeyOnline(void);
void vdDisplayErrorString(char* szErrorString);
int inHTLESelectMutipleAcqVendorID(void);
int inHTLEDecryptAndSaveOnlineKey(void);
int inHTLESelectAcqVendorIDByHostId(int inHDTid);
int inCTOSS_TWKRSAFlowProcess(void);
void vdHTLELoadField62DataRSA(unsigned char *pbtBuff, unsigned short *pusLen);
int inTWKRSAField62Data(unsigned char *pbtBuff, unsigned short *pusLen);

int inVXEMVAPCardPresent(void);
int inVXEMVAPCardInit(void);

#define FILENAME_SIZE         100           /* 2 char drive, 8 char name, a '.' & 3 char extension */

#define VS_SUCCESS													0		/* General purpose error code */
#define VS_ERR														(-1)
#define VS_ERROR													(-1)		/* Operator error */
#define VS_ESCAPE													(-3)		/* Operator quit transaction */
#define SM_RET_OK						0

#endif /* HTLESRC_H */

