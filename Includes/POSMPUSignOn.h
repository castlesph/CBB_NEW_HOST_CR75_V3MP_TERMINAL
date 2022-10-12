
#ifndef ___POS_MPU_SIGNON___
#define ___POS_MPU_SIGNON___

/*Message type*/
#define MPU_MSG_TYPE_SIZE		1

#define MPU_MSG_TYPE_INQ			"\x01"
#define MPU_MSG_TYPE_PIN			"\x03"
#define MPU_MSG_TYPE_PREAUTH		"\x10"
#define MPU_MSG_TYPE_VOID_PREAUTH	"\x11"
#define MPU_MSG_TYPE_PAC			"\x20"
#define MPU_MSG_TYPE_VOID_PAC		"\x21"
#define MPU_MSG_TYPE_SALE			"\x22"
#define MPU_MSG_TYPE_VOID_SALE		"\x23"
#define MPU_MSG_TYPE_PAC2			"\x24"
#define MPU_MSG_TYPE_REFUND			"\x25"
#define MPU_MSG_TYPE_TRANSFER		"\x26"
#define MPU_MSG_TYPE_SETTLEMENT		"\x30"
#define MPU_MSG_TYPE_SETTLE_ADJ		"\x32"
#define MPU_MSG_TYPE_SETTLE_ADJ_TIP	"\x34"
#define MPU_MSG_TYPE_SALE_OFFLINE	"\x36"

//Network Managment Code
#define MPU_NMC_SIZE			2

#define MPU_NMC_SIGNON				"\x00\x01"
#define MPU_NMC_SIGNOFF				"\x00\x02"
#define MPU_NMC_SIGNON_TDES			"\x00\x03"
#define MPU_NMC_SETTLEMENT			"\x02\x01"
#define MPU_NMC_BATCH_UPLOAD		"\x02\x01"
#define MPU_NMC_LAST_BATCH_UPLOAD	"\x02\x02"
#define MPU_NMC_ECHO				"\x03\x01"
#define MPU_NMC_STATUS				"\x03\x62"
#define MPU_NMC_MSCARD_PAR_DL		"\x03\x60"
#define MPU_NMC_MSCARD_PAR_DL_END	"\x03\x61"

//Terminal access ability
#define MPU_TML_CAP_SIZE			1

#define MPU_TML_CAP_UNKNOWN			"\x00"
#define MPU_TML_CAP_MS				"\x02"
#define MPU_TML_CAP_CHIP			"\x05"
#define MPU_TML_CAP_CTLS			"\x06"

//Chip Condition Code
#define MPU_CHIP_CON_SIZE			1

#define MPU_CHIP_CON_NA				"\x00"
#define MPU_CHIP_CON_LAST_CHIP_OK	"\x01"
#define MPU_CHIP_CON_LAST_CHIP_NOK	"\x02"


//Trans Init Method

#define MPU_TXN_INIT_SIZE			1

#define MPU_TXN_INIT_UNKNOW			"\x00"
#define MPU_TXN_INIT_ATTENDED		"\x01"
#define MPU_TXN_INIT_UNATTENDED		"\x02"
#define MPU_TXN_INIT_AGENT			"\x03"
#define MPU_TXN_INIT_BATCH_AGENT	"\x04"


// Trans Medium
#define MPU_TXN_MEDIUM_SIZE			1

#define MPU_TXN_MEDIUM_UNKNOW		"\x00"
#define MPU_TXN_MEDIUM_MS			"\x01"
#define MPU_TXN_MEDIUM_CHIP			"\x02"
#define MPU_TXN_MEDIUM_MS_OF_CHIP	"\x03"
#define MPU_TXN_MEDIUM_VIRTUAL		"\x04"
#define MPU_TXN_MEDIUM_PURE_CHAR	"\x05"
#define MPU_TXN_MEDIUM_BIOLOGICAL	"\x06"
#define MPU_TXN_MEDIUM_NO_CARD		"\x07"

// Chip Card App Type
#define MPU_IC_APP_TYPE_SIZE			1

#define MPU_IC_APP_TYPE_UNKNOW_MS	"\x00"
#define MPU_IC_APP_TYPE_UICS		"\x01"
#define MPU_IC_APP_TYPE_EMV			"\x02"
#define MPU_IC_APP_TYPE_E_CASH		"\x03"
#define MPU_IC_APP_TYPE_E_WALLET	"\x04"

#define POS_OPER_CODE "\x00\x00\x01" //@@IBR ADD 20161031

int inCTOS_GetOperCode(BYTE *szOperCode, int *inDataCnt); //@@IBR ADD 20161031

int inCTOS_SIGNONAllHosts(void);

int inCTOS_MPU_SIGNON(void);
int inCTOS_MPU_SIGNON_F2(void);
int inCTOS_MPU_SIGNON_ALL(void);

int inCTOS_AutoMPU_SIGNON(void);

int inCTOS_MPU_SIGNOFF(void);

int inMPU_PackSignonDE60Req(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg);
int inMPU_PackSignonDE63Req(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg);

int inMPU_PackSignOffDE60Req(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg);


#endif //end ___POS_MPU_SIGNON___

