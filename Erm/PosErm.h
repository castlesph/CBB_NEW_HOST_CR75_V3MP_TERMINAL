
#ifndef __POSERM_H__
#define	__POSERM_H__


typedef struct  
{
    USHORT usSaleCount;
    double  ulSaleTotalAmount;
    USHORT usRefundCount;
    double  ulRefundTotalAmount;
    USHORT usTipCount;
    double  ulTipTotalAmount;   
    USHORT usVoidSaleCount;
    double  ulVoidSaleTotalAmount;
    USHORT usOffSaleCount;
    double  ulOffSaleTotalAmount; 
}STRUCT_TOTAL_ERM;

typedef struct
{
    STRUCT_TOTAL_ERM stUploadTotal;
    STRUCT_TOTAL_ERM stAllTotal;
}TRANS_TOTAL_ERM;


typedef enum
{
    d_IPC_CMD_ERM_INITIALIZATION		        = 0x01,
	d_IPC_CMD_ERM_FORM_RECEIPT		    = 0x02,
	d_IPC_CMD_ERM_UPLOAD_RECEIPT		        = 0x03,
	d_IPC_CMD_ERM_GET_RECEIPTTOT		        = 0x04,
	d_IPC_CMD_ERM_DELETE_RECEIPT		        = 0x05,
	d_IPC_CMD_ERM_GET_ERMINFO			        = 0x06,
	d_IPC_CMD_ERM_RESTORE_BACKUPRECEIPT			= 0x07,
	d_IPC_CMD_ERM_PRINT_BACKUPRECEIPT			= 0x08,
	d_IPC_CMD_ERM_FORWARD_TRANS_REQ		        = 0x09,
	d_IPC_CMD_ERM_FORWARD_TRANS_RESP		    = 0x0A,
	d_IPC_CMD_ERM_CARDINFO_ENQ		    		= 0x0B,
//    d_IPC_CMD_COMM_RECEIVE	        = 0x04,
//    d_IPC_CMD_COMM_DISCONNECT		= 0x05,
//    d_IPC_CMD_COMM_END		        = 0x06,
//    d_IPC_CMD_GPRS_SIGNAL		        = 0x07,

}IPC_ERM_CMD_TYPES;

#define	SM_RET_PARAM					-500

#define SIGP_SEND_SIZE					(1024 * 8)           /* SIGNATURE */
#define ERC_LOGO_FILE					"ERCLOGO.TXT"
#define ERC_HEADER_FILE					"ERCHEADER.TXT"
#define ERC_FOOTER_FILE					"ERCFOOTER.TXT"
#define BMP_RECEIPT_FILE_NAME		    "/home/ap/pub/logo.bmp"
#define ERC_SHARE_HEADER_FILE		    "/home/ap/pub/ERCHEADER.TXT"

#define ERC_SHARE_FOOTER_FILE		    "/home/ap/pub/ERCFOOTER.TXT"

#define VFBMPORINAME	"signVFOri.bmp"	//128 * 64
#define VFBMPONAME		"signVFO.bmp"	//256 * 128



long lnGetFileSize(const char* pchFileName);

int inWriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize);
int inReadGzipFile(unsigned char *pchFileName, unsigned char* uszBufferData, int *inBufferSize);


int inInitializePrinterBufferLib(void);
int inSIGPStartCapturePrinterDataLib(void);
int inSIGPEndCapturePrinterDataLib(void);

int inCTOSS_CapturePrinterBuffer(unsigned char* pchTemp,CTOS_FONT_ATTRIB* ATTRIB);
int inCTOSS_ERM_Form_Receipt(int inSettle);
int inCTOSS_ERM_Form_Error_Receipt(int inSettle);
int inCTOSS_ERM_Initialization(void);

USHORT usCTOSS_Erm_Initialization(void);
USHORT usCTOSS_Erm_FromReceipt(void);
USHORT usCTOSS_Erm_UploadReceipt(void);
int usCTOSS_ReadLGOfile(void);

int inCTOSS_UploadReceipt(void);
int inCTOSS_ERMInit(void);
int inCTOSS_ERM_CheckSlipImage(void);
int usCTOSS_Erm_GetERMInfo(char* pchFileName,TRANS_TOTAL_ERM *srErmRec);


int inDeleteGzipFile(unsigned char *pchFileName);
int inDeleteFile(unsigned char *pchFileName);
int inCopyFile(unsigned char *pchFileName1, unsigned char *pchFileName2);

void vdCTOSS_DisplayStatus(int inRet);

int inCTOSS_ERM_ChoicePhoneNo(char *szPhoneNo);
int inCTOSS_ERM_ChoiceEmail(char *szEmail);
int inCTOSS_ERM_ChoicePhoneNoAndEmail(char *szPhoneNo,char *szEmail);
int inCTOSS_ERM_EnterPhoneNo(void);
int inCTOSS_ERM_EnterEmail(void);
int inCTOSS_ERM_EnterPhoneNoAndEmail(void);
void vdCTOSS_DisplayStatusNoKey(int inRet);

#ifdef OK_DOLLAR_FEATURE	
void vdHostToInitialize(BOOL fPrompt, BOOL fSuccess);
#endif

#endif

