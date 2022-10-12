
#ifndef MULTIAPTRANS_H
#define MULTIAPTRANS_H

#define d_MAX_APP				25
#define	d_AP_FLAG_DEF_SEL_EX	0xC0

#define	d_CTLS_EVENT			99
#define	d_SUCCESS				100
#define	d_FAIL					101
#define	d_NOT_RECORD			102

#define d_MAX_IPC_BUFFER		2048

#define MULTI_AP_READ_DB_FLAG 	"IN01"
#define MULTI_AP_INVOICE 	"IN02"



typedef enum
{
    d_IPC_CMD_ONLINES_SALE		= 0x01,
	d_IPC_CMD_OFFLINE_SALE		= 0x02,
	d_IPC_CMD_VOID_SALE			= 0x03,
    d_IPC_CMD_PRE_AUTH			= 0x04,
    d_IPC_CMD_REFUND				= 0x05,
    d_IPC_CMD_TIP_ADJUST			= 0x06,
    d_IPC_CMD_SETTLEMENT			= 0x07,
    d_IPC_CMD_BATCH_REVIEW		= 0x08,
    d_IPC_CMD_BATCH_TOTAL			= 0x09,
    d_IPC_CMD_REPRINT_LAST		= 0x0A,
    d_IPC_CMD_REPRINT_ANY			= 0x0B,
    d_IPC_CMD_REPRINT_LAST_SETT	= 0x0C,
    d_IPC_CMD_SUMMARY_REPORT		= 0x0D,
    d_IPC_CMD_DETAIL_REPORT		= 0x0E,
    d_IPC_CMD_GET_PID				= 0x10,
    d_IPC_CMD_POWER_FAIL			= 0x11,
    d_IPC_CMD_DELETE_BATCH		= 0x12,
    d_IPC_CMD_DELETE_REVERSAL	= 0x13,
    d_IPC_CMD_SETTLE_ALL			= 0x14,
    d_IPC_CMD_CHECK_DUP_INV      = 0x15,
    d_IPC_CMD_WAVE_SALE			= 0x16,
    d_IPC_CMD_WAVE_REFUND			= 0x17,
    d_IPC_CMD_FUN_KEY1			= 0x18,
    d_IPC_CMD_FUN_KEY2		    = 0x19,
    d_IPC_CMD_FUN_KEY3			= 0x1A,
    d_IPC_CMD_FUN_KEY4		    = 0x1B,
    d_IPC_CMD_CHK_BATCH_EMPTY    = 0x1C,
    d_IPC_CMD_ECHO    = 0x1F,
    d_IPC_CMD_ERM_INIT			= 0x20,

	d_IPC_CMD_CTLS_EXT_TRANS			= 0x2B,
	d_IPC_CMD_EXT_SIGNATURE			= 0x2C,
	d_IPC_CMD_REVERSAL 			= 0x2E,
	d_IPC_CMD_PRE_AUTH_COMP			= 0x2F,
	d_IPC_CMD_EXT_READCARD			= 0x40,
	d_IPC_CMD_EXT_READEMVCARD			= 0x41,
	
	d_IPC_CMD_EXT_LOADTMK				= 0x50,
	d_IPC_CMD_EXT_GET_ONLINEPIN			= 0x51,
        d_IPC_CMD_TRX_RETRIEVAL                         = 0x52,

	d_IPC_CMD_PASSDATA			= 0x97,
	d_IPC_CMD_WIFISCAN			= 0x98,
    d_IPC_CMD_SETFONT			= 0x99,
    d_IPC_CMD_APP_SUB_MENU		=0xFD,//reserved , Auto trigger other App sub menu 
	d_IPC_CMD_APP_MENU_FUNC		=0xFE,//reserved , Auto trigger other App menu function
    d_IPC_BACKUP_ALLDATA		=0xFF,//reserved

}IPC_CMD_TYPES;

typedef enum
{
	d_IPC_CMD_CTLS_TRANS		= 0x02,
	d_IPC_CMD_CTLS_CANCLE		= 0x03,
	d_IPC_CMD_CTLS_GETDATA		= 0x04,
	d_IPC_CMD_CTLS_SETPOWPARA	= 0x05,
	d_IPC_CMD_CTLS_PARTIALSETTAGS	= 0x0E,

}IPC_CTLSCMD_TYPES;



int inMultiAP_RunIPCCmdTypes(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen);
int inMultiAP_RunIPCCmdTypesEx(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen);
USHORT inMultiAP_HandleIPC(BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen);
int inMultiAP_GetMainroutine(void);
int inMultiAP_SendChild(BYTE *inbuf, USHORT inlen);

int inCTOS_MultiAPALLAppEventID(int IPC_EVENT_ID);
int inCTOS_MultiAPALLAppEventIDEx(int IPC_EVENT_ID);
int inCTOS_MultiAPBatchSearch(int IPC_EVENT_ID);
int inCTOS_MultiAPSaveData(int IPC_EVENT_ID);
int inCTOS_MultiAPGetData(void);
int inCTOS_MultiAPGetpowrfail(void);
int inCTOS_MultiAPGetVoid(void);
int inCTOS_MultiAPReloadTable(void);
int inCTOS_MultiAPReloadHost(void);
int inCTOS_MultiAPCheckAllowd(void);
int inMultiAP_ECRGetMainroutine(void);
int inMultiAP_EXTGetMainroutine(void);
int inCTOSS_MultiDislplayMenu(int IPC_EVENT_ID);
int inMultiAP_ECRSendSuccessResponse(void);
int inCTOSS_SetALLApFont(char *font);
void vdCTOSS_MultiAPAnalysis(BYTE *inbuf, USHORT inlen);

#endif


