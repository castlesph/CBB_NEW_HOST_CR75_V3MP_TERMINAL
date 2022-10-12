
#ifndef ___MULTI_SHARLS_COM___
#define ___MULTI_SHARLS_COM___

typedef enum
{
    d_IPC_CMD_COMM_INIT		        = 0x01,
	d_IPC_CMD_COMM_CONNECT		    = 0x02,
	d_IPC_CMD_COMM_SEND		        = 0x03,
    d_IPC_CMD_COMM_RECEIVE	        = 0x04,
    d_IPC_CMD_COMM_DISCONNECT		= 0x05,
    d_IPC_CMD_COMM_END		        = 0x06,
    d_IPC_CMD_GPRS_SIGNAL		    = 0x07,
	d_IPC_CMD_COMM_GETCONFIG 		= 0x08,
	d_IPC_CMD_COMM_SETCONFIG		= 0x09,
	d_IPC_CMD_GPRS_SIGNALEX		    = 0x0A,
	d_IPC_CMD_GPRS_GETIMSI			= 0x0B,
	d_IPC_CMD_GPRS_GETCCID			= 0x0C,
	d_IPC_CMD_COMM_SLEEPMODE		= 0x0D,
	d_IPC_CMD_COMM_SETSIMSLOT		= 0x0E,
	d_IPC_CMD_COMM_GETGPRSIP		= 0x0F,
	d_IPC_CMD_COMM_CONNECT_PreConnect1			= 0x10,
	d_IPC_CMD_COMM_CONNECT_PreConnect2			= 0x11,
	d_IPC_CMD_COMM_GETGPRSPING                = 0x12,
	d_IPC_CMD_COMM_GETGPRSIPINFO		= 0x13,
	d_IPC_CMD_COMM_URLPARSEIP		= 0x14,

	d_IPC_CMD_COMM_INIT_CONNECTEx			= 0x19,
	d_IPC_CMD_COMM_SENDEx		        = 0x1A,

	//d_IPC_CMD_SETFONT				= 0x99,
}IPC_COMM_CMD_TYPES;

#define IPC_COMM_SUCC       100
#define IPC_COMM_FAIL       101

#define NO_HEADER_LEN                0
#define HEX_EXCLUDE_LEN              1
#define BCD_EXCLUDE_LEN              2
#define HEX_INCLUDE_LEN              3
#define BCD_INCLUDE_LEN              4
#define STR_EXCLUDE_LEN				 5
#define STR_INCLUDE_LEN				 6

typedef struct
{
	unsigned short usGPRSNetworkType;
	char szGPRSNetworkName[20+1];
	BYTE bgGPRSSignal;
	BYTE bgGPRSType;
	BYTE usEthType;
	BYTE usWIFIType;
	BYTE szWIFISSID[50];
	BYTE usGPSType;
	BYTE usBLUETOOTHType;
	BYTE usDIALPPType;
	BYTE inReserved1;//for medom status
	BYTE inReserved2;
	BYTE inReserved3;
	BYTE szReserved1[50];
	BYTE szReserved2[50];
	BYTE szReserved3[50];
	BYTE szReserved4[50];
} STRUCT_SHARLS_COM ;

typedef struct
{
	BYTE	szIP[30] ;
    BYTE	szGetWay[30] ;
    BYTE	szSubNetMask[30] ;
    BYTE	szHostDNS1[30] ;
    BYTE	szHostDNS2[30] ;
    BYTE 	szReserved1[50];
	BYTE 	szReserved2[50];
	BYTE 	szReserved3[50];
	BYTE 	szReserved4[50];

} STRUCT_IP_INFO;



int inCTOSS_COMM_Initialize(TRANS_DATA_TABLE *srTransPara,int mode);
int inCTOSS_COMM_Connect(TRANS_DATA_TABLE *srTransPara);
int inCTOSS_COMM_Send(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen);
int inCTOSS_COMM_Receive(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData);
int inCTOSS_COMM_Disconnect(TRANS_DATA_TABLE *srTransPara);
int inCTOSS_COMM_End(TRANS_DATA_TABLE *srTransPara);
int inCTOSS_GetGPRSSignal(unsigned short* usNetworkType, char* szNetworkName,unsigned short* usEthType);
int inCTOSS_GetGPRSSignalEx(unsigned short* usNetworkType, char* szNetworkName,unsigned short* usEthType);
int inCTOSS_GetGPRSSignalEx1(unsigned short* usNetworkType, char* szNetworkName,unsigned short* usEthType, STRUCT_SHARLS_COM* Sharls_COMData);
int inCTOSS_COMMSetFont(char *font);
int inCTOSS_COMMWIFISCAN(void);
int inCTOSS_COMMSetSleepMode(void);
int inCTOSS_COMMSetSIMSlot(int inSIMSlot);
int inCTOSS_SIMGetGPRSIP(void);
int inCTOSS_COMMUrlParseIP(int inMode, char *szURLOrIP);

void vdSetInit_Connect(int Init_Connect);
int inCTOSS_COMM_InitializeAndConnect(TRANS_DATA_TABLE *srTransPara,int inMode);

#endif  //end ___MULTI_SHARLS_ECR___

