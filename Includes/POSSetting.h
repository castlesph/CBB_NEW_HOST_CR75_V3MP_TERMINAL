#ifndef ___POS_SETTING_H___
#define ___POS_SETTING_H___

#define STR_HEAD            0
#define STR_BOTTOM          1
#define STR_ALL             2



void vdCTOS_uiPowerOff(void);
void vdCTOS_IPConfig(void);
int inCTOS_SelectHostSetting(void);

int inCTOS_IPP_HostSetting(void);

void vdCTOS_DialConfig(void);
void vdCTOS_ModifyEdcSetting(void);
void vdCTOS_DeleteBatch(void);
void vdCTOS_DeleteReversal(void);
void vdCTOS_EditEnvParam(void);
void vdCTOS_EditEnvParamDB(void);
int get_env(char *tag, char *value, int len);
int put_env(char *tag, char *value, int len);
int get_env_int (char *tag);
void put_env_int(char *tag, int value);


void vdCTOS_PrintEMVTerminalConfig(void);
void vdCTOS_GPRSSetting(void);
void vdCTOS_Debugmode(void);
void vdCTOSS_CtlsMode(void);
void vdCTOS_TipAllowd(void);
void vdCTOS_DemoMode(void);
void DelCharInStr(char *str, char c, int flag);
void vdCTOSS_DownloadMode(void);
void vdCTOSS_CheckMemory(void);
void CTOSS_SetRTC(void);
int inCTOSS_GetCtlsMode(void);
void vdCTOSS_PrintTerminalConfig(void);
void vdCTOSS_SelectPinpadType(void);
void vdCTOSS_InjectMKKey(void);
void vdCTOS_ThemesSetting(void);

/*sidumili: Issue#:000087 [prompt password]*/
int inCTOS_PromptPassword(void);
int inCTOS_CommsFallback(int shHostIndex);
void vdCTOSS_DisplayAmount(USHORT usX, USHORT usY, char *szCurSymbol,char *szAmount);
void vdCTOSS_EditTable(void);
void vdCTOS_uiIDLEPowerOff(void);

void vdCTOS_TMSSetting(void);
void vdCTOS_TMSReSet(void);
int  inCTOS_TMSPreConfigSetting(void);
int inCTOSS_CheckBatteryChargeStatus(void);
int inCTOSS_PassData(void);
void vdCTOS_uiIDLESleepMode(void);
void vdCTOS_uiIDLEWakeUpSleepMode(void);
void vdCTOS_TMSUploadFile(void);

int isCheckTerminalMP200(void);
void vdCTOS_MdmPPPConfig(void);

void vdCBB_TMKRefNumSetting(void);
void vdCBB_SetTerminalMode(void);
void vdCTOS_TermSelectCurrency(void); //@@IBR ADD 20170116
void vdCTOS_TermSelectCurrencyMenu(void);
void vdCTOS_MPUOperSetting(void);
void vdCTOS_EnableManualEntry(void); //@@IBR ADD 20170202
void vdCTOSS_ModifyStanNumber(void); //@@IBR ADD 20170202
void vdCTOS_SetExchangeRate(void);
void vdCTOS_SetAutoSettleInterval(void);

void vdIPP_TMKRefNumSetting(void);

int put_env(char *tag, char *value, int len);
void put_env_int(char *tag, int value);

int get_env(char *tag, char *value, int len);
int get_env_int (char *tag);
int inCTOS_TrxRetrieval(void);

void vdCTOS_AutoWaveInit(void);
void vdCTOS_SetLanguage(void);
void vdCTOS_SwitchLanguage(void);
void vdDefaultLanguage(void);
int inTransSelectCurrency(void);
void vdCTOS_SetDuplicateReceipt(void);
void vdCheckMenu(void);
void vdEnableIPPMenu(void);
void vdEnableAlipayMenu(void);
void vdEnablePreAuthMenu(void);
void vdEnableUPIFinexusMenu(void);
void vdShowTerminalIP(void);
int Check_OverDateTime(unsigned char S_bYear,unsigned char S_bMonth,unsigned char S_bDay,unsigned char S_bHour,unsigned char S_bMinute,unsigned char S_Second);
void vdGetDateFromInt(int Date,CTOS_RTC *BUFFDATE);
void vdGetTimeFromInt(int Time,CTOS_RTC *BUFFDATE);
void vdCTOS_DualGPRSSetting(void);
void vdEnableBTMenu(void);

void vdCTOS_SelectWIFI(void);
void vdCTOS_SetParkingFee(void);
void vdCTOS_ECRmode(void);
void vdCheckMenuID4(void);


//Menu Index
//http://118.201.48.210:8080/redmine/issues/2070.100 
//#define CBPAY_MENU_INDEX 				3 - orig
//#define ALIPAY_MENU_INDEX 				3+1
//#define SETTLEMENT_MENU_INDEX 			4+1 - ORIG
//#define BATCHUPLOAD_MENU_INDEX 			5+1 - orig
//#define DISCOUNT_MENU_INDEX  			33+1 - ORIG
//#define OK_DOLLAR_MENU_INDEX			35+1 - orig

// ORIGINAL MACRO VALUE
/*
#define CBPAY_MENU_INDEX 				2 
#define OK_DOLLAR_MENU_INDEX			3 
#define SETTLEMENT_MENU_INDEX 			3+1
#define SALE_MENU_INDEX		 			4+1
#define CARDVER_MENU_INDEX 				5+1
#define PREAUTH_MENU_INDEX 				6+1
#define VOID_MENU_INDEX 				7+1 
#define DISCOUNT_MENU_INDEX  			8+1
#define SIGNONALL_MENU_INDEX			9+1
*/

#define SIGNONALL_MENU_INDEX 			2 
#define CBPAY_MENU_INDEX				3 //ORIG 35+1
#define OK_DOLLAR_MENU_INDEX 			3+1
#define SETTLEMENT_MENU_INDEX		 	4+1
#define SALE_MENU_INDEX 				5+1
#define CARDVER_MENU_INDEX 				6+1
#define PREAUTH_MENU_INDEX 				7+1 
#define VOID_MENU_INDEX  				8+1 //ORIG 33+1
#define DISCOUNT_MENU_INDEX				9+1



#define REPRINT_MENU_INDEX 				10+1
#define REPORT_MENU_INDEX 				11+1


#define BATCHTOTAL_MENU_INDEX 			12+1
#define BATCHREVIEW_MENU_INDEX 			13+1
#define REFUND_MENU_INDEX 				14+1
#define TMSDL_MENU_INDEX 				15+1
#define UPLOADRECEIPT_MENU_INDEX 		16+1
#define TIPADJUST_MENU_INDEX 			17+1
#define WAVEREFUND_MENU_INDEX 			18+1
//#define OFFLINE_MENU_INDEX 			19+1
#define SIGNOFF_MENU_INDEX 				19+1
#define SETTINST_MENU_INDEX				20+1

#define PREAUTH_MPU_MENU_INDEX  		21+1
#define LASTSETTLE_MENU_INDEX 			22+1
#define SUMMARYREPORT_MENU_INDEX 		23+1
#define UNIONPAY_MENU_INDEX 			24+1
#define MPU_MENU_INDEX					25+1
#define VOIDPREAUTH_MENU_INDEX  		26+1
#define IPP_MENU_INDEX 					27+1
#define ALIPAY_MENU_INDEX2 				28+1
#define VOIDPREAUTHCOMP_MENU_INDEX 		29+1
#define CASHADV_MENU_INDEX				30+1

#define SWITCHSIM_MENU_INDEX  			31+1
#define TOPUP_MENU_INDEX				32+1
#define ALIPAY_MENU_INDEX 				33+1
#define CHANGEPIN_MENU_INDEX  			34+1
#define BATCHUPLOAD_MENU_INDEX 			35+1
//#define BATCHUPLOAD_MENU_INDEX 			36+1


// for menuid 4
/*#define REPRINTLAST_MENU_INDEX  			35+1
#define REPRINTANY_MENU_INDEX  			36+1
#define REPRINTLASTSETT_MENU_INDEX  			37+1
#define DETAILREPORT_MENU_INDEX  			38+1
*/
// for menuid 4
#define REFUND_MENU_INDEX2 				36+1
#define WAVEREFUND_MENU_INDEX2 				37+1


#endif //end ___POS_SETTING_H___

