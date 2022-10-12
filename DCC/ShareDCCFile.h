#ifndef __SHAREDCC_FILE_H__
#define	__SHAREDCC_FILE_H__

//#define DB_DCCCFG		"/home/ap/pub/DCCCFG.S3DB"


typedef struct
{
	VS_BOOL		fDCCFlag;
	int			inCommMode;
	char		szHostIP[50+1];
	long		ulHostPort;
	char		szSecHostIP[50+1];
	long 		ulSecHostPort;
	char 		szPriPhoneNo[30];
	char 		szSecPhoneNo[30];
	int 		inHDTIndex;
	BYTE		szTPDU[5+1];
	int			inIPHeader;
	char 		szReserved1[65];								
	char 		szReserved2[65];
	char 		szReserved3[65];
}DCC_CFG;

/*=====DCC Pipe share data structure start=====*/
typedef struct
{
	char	szTime[3+1];
	char	szDate[2+1];
	char	szCardNum[20+1];
	char	szCardType[3+1];
	
}DCC_STD; //DCC share Trans Data;

typedef struct
{
	VS_BOOL		fSetCfg;
	DCC_CFG		stDCCCfg;

	VS_BOOL		fSetTxn;
	DCC_STD		stTxnData;
	
}DCC_SHARE_DATA;


#define DCC_SHARE_DATA_SIZE		sizeof(DCC_SHARE_DATA)

/*=====DCC Pipe share data structure End=====*/



#if 0
int inDCCCFGRead(int inSeekCnt);
int inDCCCFGSave(int inSeekCnt);
#endif

void vdSetShareDCCCfgParam(VS_BOOL fSetFlag);
void vdSetShareDCCTransData(VS_BOOL fSetFlag);
void vdSetDCCShareData(BYTE *pszData, USHORT *inLen);

#endif

