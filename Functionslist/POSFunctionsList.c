
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <ctosapi.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "POSFunctionsList.h"
#include "..\Includes\POSTypedef.h"

#include <ctosapi.h>
#include <semaphore.h>
#include <pthread.h>

#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSAuth.h"
#include "..\Includes\POSConfig.h"
#include "..\Includes\POSOffline.h"
#include "..\Includes\POSRefund.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSBatch.h"
#include "..\Includes\POSSettlement.h"

#include "..\Includes\POSCUPSale.h"
#include "..\Includes\POSCUPPreAuth.h"
#include "..\Includes\POSCUPLogon.h"
#include "..\Includes\POSCUPSaleAdjust.h"


#include "..\Includes\POSMPUSale.h"
#include "..\Includes\POSMPUVoid.h"
#include "..\Includes\POSMPUSignOn.h"
#include "..\Includes\POSMPUPreAuth.h"
#include "..\Includes\POSMPUSettle.h"

#include "..\Includes\POSIpp.h"

#ifdef CBPAY_DV
#include "..\QR_Payment\POSQR_Payment.h"
#endif

#include "..\Includes\CTOSInput.h"
#include "..\ERM\Poserm.h"
#include "..\tms\TMS.h"

#include "..\sam\sam.h"

#include "..\ui\Display.h"
#include "..\print\print.h"
#include "..\Debug\Debug.h"
#include "..\Includes\DMenu.h"
#include "..\Ctls\POSWave.h"
#include "..\Htle\htlesrc.h"
#ifdef NMX_LIB
#include "..\Nmxtle\Nmxtle.h"
#endif

#ifdef TOPUP_RELOAD
#include "..\Includes\POSBalanceEnquiry.h"
#include "..\Includes\POSTopupReload.h"
#endif

#define MAXFUNCTIONS 		2048

extern TRANS_DATA_TABLE* srGetISOEngTransDataAddress(void);

extern void vdFelica_PaymentFlow(void);
extern void vdFelica_ChargeFlow(void);
extern void vdFelica_BalanceFlow(void);

extern int inCTOS_Key0Event(void);
extern int inCTOS_Key1Event(void);
extern int inCTOS_Key2Event(void);
extern int inCTOS_Key3Event(void);
extern int inCTOS_Key4Event(void);
extern int inCTOS_Key5Event(void);
extern int inCTOS_Key6Event(void);
extern int inCTOS_Key7Event(void);
extern int inCTOS_Key8Event(void);
extern int inCTOS_Key9Event(void);

extern int inCTOSS_ManualSwitchSIM(void);

// can declare any functions type and link with string.
Func_vdFunc stFunctionList_vdFunc[] = {
	{"inCTOS_SALE_OFFLINE", inCTOS_SALE_OFFLINE},
	{"inCTOS_WAVE_SALE", inCTOS_WAVE_SALE},
	{"inCTOS_WAVE_REFUND", inCTOS_WAVE_REFUND},
	{"inCTOS_VOID", inCTOS_VOID},
	{"inCTOS_SETTLEMENT", inCTOS_SETTLEMENT},
	{"inCTOS_SETTLE_ALL", inCTOS_SETTLE_ALL},
	{"inCTOS_PREAUTH", inCTOS_PREAUTH},
	{"inCTOS_PREAUTH_VOID", inCTOS_PREAUTH_VOID},
	{"inCTOS_REFUND", inCTOS_REFUND},
	{"inCTOS_TIPADJUST", inCTOS_TIPADJUST},
	{"inCTOS_BATCH_REVIEW", inCTOS_BATCH_REVIEW},
	{"inCTOS_BATCH_TOTAL", inCTOS_BATCH_TOTAL},
	{"inCTOS_REPRINT_LAST", inCTOS_REPRINT_LAST},
	{"inCTOS_REPRINT_ANY", inCTOS_REPRINT_ANY},
	{"inCTOS_REPRINTF_LAST_SETTLEMENT", inCTOS_REPRINTF_LAST_SETTLEMENT},
	{"inCTOS_PRINTF_SUMMARY", inCTOS_PRINTF_SUMMARY},
	{"inCTOS_PRINTF_DETAIL", inCTOS_PRINTF_DETAIL},
	{"inCTOS_PRINTF_SUMMARY", inCTOS_PRINTF_SUMMARY},
	{"inCTOS_PRINTF_DETAIL", inCTOS_PRINTF_DETAIL},
	{"inCTOS_REPRINT_LAST", inCTOS_REPRINT_LAST},
	{"inCTOS_REPRINT_ANY", inCTOS_REPRINT_ANY},
	{"inCTOSS_UploadReceipt", inCTOSS_UploadReceipt},
	{"inCTOSS_TMSDownloadRequest", inCTOSS_TMSDownloadRequest},
	{"inCTOS_REPRINTF_LAST_SETTLEMENT", inCTOS_REPRINTF_LAST_SETTLEMENT},
	{"vdCTOS_uiPowerOff", (DMENU_FUN)vdCTOS_uiPowerOff},
	{"vdCTOS_IPConfig", (DMENU_FUN)vdCTOS_IPConfig},
	{"vdCTOS_DialConfig", (DMENU_FUN)vdCTOS_DialConfig},
	{"vdCTOS_ModifyEdcSetting", (DMENU_FUN)vdCTOS_ModifyEdcSetting},
	{"vdCTOS_GPRSSetting", (DMENU_FUN)vdCTOS_GPRSSetting},
	{"vdCTOS_DeleteBatch", (DMENU_FUN)vdCTOS_DeleteBatch},
	{"vdCTOS_DeleteReversal", (DMENU_FUN)vdCTOS_DeleteReversal},
	{"vdCTOS_PrintEMVTerminalConfig", (DMENU_FUN)vdCTOS_PrintEMVTerminalConfig},
	{"vdCTOSS_PrintTerminalConfig", (DMENU_FUN)vdCTOSS_PrintTerminalConfig},
	{"vdCTOS_TipAllowd", (DMENU_FUN)vdCTOS_TipAllowd},
	{"vdCTOS_Debugmode", (DMENU_FUN)vdCTOS_Debugmode},
	{"vdCTOSS_SelectPinpadType", (DMENU_FUN)vdCTOSS_SelectPinpadType},
	{"vdCTOSS_InjectMKKey", (DMENU_FUN)vdCTOSS_InjectMKKey},
	{"vdCTOSS_CtlsMode", (DMENU_FUN)vdCTOSS_CtlsMode},
	{"vdCTOS_DemoMode", (DMENU_FUN)vdCTOS_DemoMode},	
	{"vdCTOSS_DownloadMode", (DMENU_FUN)vdCTOSS_DownloadMode},	
	{"vdCTOSS_CheckMemory", (DMENU_FUN)vdCTOSS_CheckMemory},	
	{"CTOSS_SetRTC", (DMENU_FUN)CTOSS_SetRTC},	
	{"vdCTOS_InitWaveData", (DMENU_FUN)vdCTOS_InitWaveData},
	{"vdCTOS_PartialInitWaveData", (DMENU_FUN)vdCTOS_PartialInitWaveData},
	{"vdCTOS_EditEnvParam", (DMENU_FUN)vdCTOS_EditEnvParamDB},
	{"vdCTOSS_EFTSECKeyCard", (DMENU_FUN)vdCTOSS_EFTSECKeyCard},
	{"vdHTLESelectloadMethods", (DMENU_FUN)vdHTLESelectloadMethods},
	/*sidumili: [prompt for password]*/
	{"inCTOS_PromptPassword", inCTOS_PromptPassword},	
	{"vdCTOS_ThemesSetting", (DMENU_FUN)vdCTOS_ThemesSetting},
	{"vdCTOSS_EditTable", (DMENU_FUN)vdCTOSS_EditTable},
	{"vdCBB_TMKRefNumSetting", (DMENU_FUN)vdCBB_TMKRefNumSetting},
        {"vdIPP_TMKRefNumSetting", (DMENU_FUN)vdIPP_TMKRefNumSetting},
	{"vdCBB_SetTerminalMode", (DMENU_FUN)vdCBB_SetTerminalMode},
        {"vdCTOS_TermSelectCurrency", (DMENU_FUN)vdCTOS_TermSelectCurrency}, //@@IBR ADD 20170116
        {"vdCTOS_MPUOperSetting", (DMENU_FUN)vdCTOS_MPUOperSetting}, //@@IBR ADD 20170131
        {"vdCTOS_EnableManualEntry", (DMENU_FUN)vdCTOS_EnableManualEntry}, //@@IBR ADD 20170202	
	{"vdCTOSS_ModifyStanNumber", (DMENU_FUN)vdCTOSS_ModifyStanNumber}, //@@IBR ADD 20170202
        {"vdCTOS_SetExchangeRate", (DMENU_FUN)vdCTOS_SetExchangeRate}, //@@IBR ADD 20170712
        {"vdCTOS_SetAutoSettleInterval", (DMENU_FUN)vdCTOS_SetAutoSettleInterval}, //@@IBR ADD 20170718
        {"vdCTOS_SetLanguage", (DMENU_FUN)vdCTOS_SetLanguage}, //@@IBR ADD 20170718
        {"vdCTOS_SetDuplicateReceipt", (DMENU_FUN)vdCTOS_SetDuplicateReceipt}, //@@IBR ADD 20170718
        {"vdEnableIPPMenu", (DMENU_FUN)vdEnableIPPMenu}, //@@IBR ADD 20170718
        {"vdEnableAlipayMenu", (DMENU_FUN)vdEnableAlipayMenu}, //@@IBR ADD 20170718
        {"vdEnablePreAuthMenu", (DMENU_FUN)vdEnablePreAuthMenu}, //THANDAR_20180425        
	    {"vdEnableUPIFinexusMenu", (DMENU_FUN)vdEnableUPIFinexusMenu}, //THANDAR_20180426  
	    {"vdShowTerminalIP", (DMENU_FUN)vdShowTerminalIP}, //THANDAR_20180426
	    {"vdEnableBTMenu",(DMENU_FUN)vdEnableBTMenu},//thandar_2018718_add BT
        

#ifdef NMX_LIB 
	{"inCTOS_InjectNMXTLE", inCTOS_InjectNMXTLE},
#endif        
	{"vdCTOS_TMSSetting", (DMENU_FUN)vdCTOS_TMSSetting},
	{"vdCTOS_TMSReSet", (DMENU_FUN)vdCTOS_TMSReSet},
	{"vdCTOS_TMSUploadFile", (DMENU_FUN)vdCTOS_TMSUploadFile},
	//{"vdCTOS_FelicaCardTxn", (DMENU_FUN)vdCTOS_FelicaCardTxn},
	{"vdFelica_PaymentFlow", (DMENU_FUN)vdFelica_PaymentFlow},
	{"vdFelica_ChargeFlow", (DMENU_FUN)vdFelica_ChargeFlow},
	{"vdFelica_BalanceFlow", (DMENU_FUN)vdFelica_BalanceFlow},
	{"vdFelica_SAMSlotConfig", (DMENU_FUN)vdFelica_SAMSlotConfig},

	{"inCTOS_CashAdvance", inCTOS_CashAdvance},
	{"inCTOS_CBB_LOGON", inCTOS_CBB_LOGON},

	{"inCTOS_CUP_LOGON", inCTOS_CUP_LOGON},
	{"inCTOS_CUP_SALE", inCTOS_CUP_SALE},
	{"inCTOS_CUP_PreAuth", inCTOS_CUP_PreAuth},
	{"inCTOS_CUP_PreAuthComp", inCTOS_CUP_PreAuthComp},
	{"inCTOS_CUP_VoidPreAuth", inCTOS_CUP_VoidPreAuth},
	{"inCTOS_CUP_SaleAdjust", inCTOS_CUP_SaleAdjust},

	{"inCTOS_MPU_SALE", inCTOS_MPU_SALE},
	{"inCTOS_MPU_VOID", inCTOS_MPU_VOID},
	{"inCTOS_MPU_SETTLEMENT", inCTOS_MPU_SETTLEMENT},
        {"inCTOS_MPU_SETTLE_ALL", inCTOS_MPU_SETTLE_ALL},
//	{"inCTOS_MPU_PreAuth", inCTOS_MPU_PreAuth},
	{"inCTOS_MPU_PREAUTH", inCTOS_MPU_PREAUTH},
	{"inCTOS_MPU_PREAUTH_COMP", inCTOS_MPU_PREAUTH_COMP},
	{"inCTOS_MPU_PREAUTH_COMP_ADVICE", inCTOS_MPU_PREAUTH_COMP_ADVICE},
	{"inCTOS_MPU_VOID_PREAUTH", inCTOS_MPU_VOID_PREAUTH},
	{"inCTOS_MPU_VOID_PREAUTH_COMP", inCTOS_MPU_VOID_PREAUTH_COMP},
	
	{"inCTOS_MPU_SIGNON", inCTOS_MPU_SIGNON},	
	{"inCTOS_MPU_SIGNON_F2", inCTOS_MPU_SIGNON_F2},		
	{"inCTOS_MPU_SIGNON_ALL", inCTOS_MPU_SIGNON_ALL},
	
	{"inCTOS_MPU_SIGNOFF", inCTOS_MPU_SIGNOFF},
	{"inCTOS_MPU_BatchUpload", inCTOS_MPU_BatchUpload},
        
        {"inCTOS_MPU_PRINTF_SUMMARY", inCTOS_MPU_PRINTF_SUMMARY},
        {"inCTOS_MPU_PRINTF_DETAIL", inCTOS_MPU_PRINTF_DETAIL},
        
        {"inCTOS_MPU_REPRINT_ANY", inCTOS_MPU_REPRINT_ANY},
        {"inCTOS_MPU_REPRINT_LAST", inCTOS_MPU_REPRINT_LAST},
        {"inCTOS_MPU_REPRINTF_LAST_SETTLEMENT", inCTOS_MPU_REPRINTF_LAST_SETTLEMENT},

        {"inCTOS_IPP_INSTALLMENT", inCTOS_IPP_INSTALLMENT},
        {"inCTOS_IPP_VOID", inCTOS_IPP_VOID},
        {"inCTOS_IPP_SETTLEMENT", inCTOS_IPP_SETTLEMENT},
        {"inCTOS_IPP_SIGNON", inCTOS_IPP_SIGNON},
        
        {"inCTOS_PREAUTH_COMPLETION", inCTOS_PREAUTH_COMPLETION},
        {"inCTOS_PREAUTHCOMP_VOID", inCTOS_PREAUTHCOMP_VOID},

#ifdef CBPAY_DV
	{"inCTOS_MPU_CBPay", inCTOS_MPU_CBPay},	 //for CB Pay
#endif

	#ifdef OK_DOLLAR_FEATURE	
	{"inCTOS_CBB_OKDollar", inCTOS_CBB_OKDollar},	 //for Ok Dollar
	#endif

	/*DMENU-Update 20160601 start With new DMenu, we can set all key event in DMENU Database*/
	{"inCTOS_Key0Event", (DMENU_FUN)inCTOS_Key0Event},
	{"inCTOS_Key1Event", (DMENU_FUN)inCTOS_Key1Event},
	{"inCTOS_Key2Event", (DMENU_FUN)inCTOS_Key2Event},
	{"inCTOS_Key3Event", (DMENU_FUN)inCTOS_Key3Event},
	{"inCTOS_Key4Event", (DMENU_FUN)inCTOS_Key4Event},
	{"inCTOS_Key5Event", (DMENU_FUN)inCTOS_Key5Event},
	{"inCTOS_Key6Event", (DMENU_FUN)inCTOS_Key6Event},
	{"inCTOS_Key7Event", (DMENU_FUN)inCTOS_Key7Event},
	{"inCTOS_Key8Event", (DMENU_FUN)inCTOS_Key8Event},
	{"inCTOS_Key9Event", (DMENU_FUN)inCTOS_Key9Event},
	/*DMENU-Update 20160601 end With new DMenu, we can set all key event in DMENU Database*/
    {"inCTOS_ALIPAY_SALE", (DMENU_FUN)inCTOS_ALIPAY_SALE},
    {"inCTOSS_ManualSwitchSIM", (DMENU_FUN)inCTOSS_ManualSwitchSIM},
    {"inCTOS_MPU_PRINTF_DETAIL_ALL", (DMENU_FUN)inCTOS_MPU_PRINTF_DETAIL_ALL},
#ifdef PARKING_FEE
    {"vdCTOS_SetParkingFee", (DMENU_FUN)vdCTOS_SetParkingFee},
#endif

#ifdef TOPUP_RELOAD
    {"inCTOS_BALANCE_ENQUIRY", (DMENU_FUN)inCTOS_BALANCE_ENQUIRY},
	{"inCTOS_TOPUP", (DMENU_FUN)inCTOS_TOPUP},
	{"inCTOS_RELOAD", (DMENU_FUN)inCTOS_RELOAD},
    {"inCTOS_TOPUP_RELOAD_LOGON", (DMENU_FUN)inCTOS_TOPUP_RELOAD_LOGON},
#endif

#ifdef DISCOUNT_FEATURE
	{"inCTOS_FIXED_AMOUNT", (DMENU_FUN)inCTOS_FIXED_AMOUNT},
	{"inCTOS_PERCENTAGE", (DMENU_FUN)inCTOS_PERCENTAGE},
#endif	
#ifdef PIN_CHANGE_ENABLE
		{"inCTOS_ChangePIN", (DMENU_FUN)inCTOS_ChangePIN},
#endif
    {"vdPrintTerminalConfigNew", (DMENU_FUN)vdPrintTerminalConfigNew},
	{"", (DFUNCTION_LIST)NULL},
};

// can declare any functions type and link with string.
Func_inISOPack stFunctionList_inISOPack[] = {
	{"inPackIsoFunc02", inPackIsoFunc02},
	{"inPackIsoFunc03", inPackIsoFunc03},
	{"inPackIsoFunc04", inPackIsoFunc04},
	{"inPackIsoFunc07", inPackIsoFunc07}, //@@IBR ADD 20161020
	{"inPackIsoFunc11", inPackIsoFunc11},
	{"inPackIsoFunc12", inPackIsoFunc12},
	{"inPackIsoFunc13", inPackIsoFunc13},
	{"inPackIsoFunc14", inPackIsoFunc14},
	{"inPackIsoFunc15", inPackIsoFunc15},
	{"inPackIsoFunc18", inPackIsoFunc18},
	{"inPackIsoFunc22", inPackIsoFunc22},
	{"inPackIsoFunc23", inPackIsoFunc23},
	{"inPackIsoFunc24", inPackIsoFunc24},
	{"inPackIsoFunc25", inPackIsoFunc25},
	{"inPackIsoFunc26", inPackIsoFunc26}, //@@IBR ADD 20161020
	{"inPackIsoFunc32", inPackIsoFunc32},
	{"inPackIsoFunc35", inPackIsoFunc35},
	{"inPackIsoFunc36", inPackIsoFunc36}, //@@IBR ADD 20161025
	{"inPackIsoFunc37", inPackIsoFunc37},
	{"inPackIsoFunc38", inPackIsoFunc38},
	{"inPackIsoFunc39", inPackIsoFunc39},
	{"inPackIsoFunc41", inPackIsoFunc41},
	{"inPackIsoFunc42", inPackIsoFunc42},
    {"inPackIsoFunc43", inPackIsoFunc43},
	{"inPackIsoFunc45", inPackIsoFunc45},
	{"inPackIsoFunc48", inPackIsoFunc48},
	{"inPackIsoFunc49", inPackIsoFunc49}, //@@IBR ADD 20161020
	{"inPackIsoFunc52", inPackIsoFunc52},
	{"inPackIsoFunc54", inPackIsoFunc54},
	{"inPackIsoFunc55", inPackIsoFunc55},
	{"inPackIsoFunc56", inPackIsoFunc56},
	{"inPackIsoFunc57", inPackIsoFunc57},
	{"inPackIsoFunc60", inPackIsoFunc60},
	{"inPackIsoFunc61", inPackIsoFunc61},
	{"inPackIsoFunc62", inPackIsoFunc62},
	{"inPackIsoFunc63", inPackIsoFunc63},
	{"inPackIsoFunc64", inPackIsoFunc64},
	{"", (DFUNCTION_inISOPack)NULL},
};

// can declare any functions type and link with string.
Func_inISOUnPack stFunctionList_inISOUnPack[] = {
	{"inUnPackIsoFunc02", inUnPackIsoFunc02},
    {"inUnPackIsoFunc07", inUnPackIsoFunc07},
    {"inUnPackIsoFunc11", inUnPackIsoFunc11},
    {"inUnPackIsoFunc12", inUnPackIsoFunc12},
    {"inUnPackIsoFunc13", inUnPackIsoFunc13},
	{"inUnPackIsoFunc14", inUnPackIsoFunc14},
    {"inUnPackIsoFunc15", inUnPackIsoFunc15},
    {"inUnPackIsoFunc32", inUnPackIsoFunc32},
    {"inUnPackIsoFunc33", inUnPackIsoFunc33},
    {"inUnPackIsoFunc37", inUnPackIsoFunc37},
    {"inUnPackIsoFunc38", inUnPackIsoFunc38},
    {"inUnPackIsoFunc39", inUnPackIsoFunc39},
    {"inUnPackIsoFunc41", inUnPackIsoFunc41},
    {"inUnPackIsoFunc43", inUnPackIsoFunc43},
    {"inUnPackIsoFunc44", inUnPackIsoFunc44},
    {"inUnPackIsoFunc48", inUnPackIsoFunc48},
    {"inUnPackIsoFunc49", inUnPackIsoFunc49},
    {"inUnPackIsoFunc54", inUnPackIsoFunc54},
    {"inUnPackIsoFunc55", inUnPackIsoFunc55},
    {"inUnPackIsoFunc57", inUnPackIsoFunc57},
    {"inUnPackIsoFunc60", inUnPackIsoFunc60}, //@@IBR ADD 20161125
    {"inUnPackIsoFunc62", inUnPackIsoFunc62},
    {"inUnPackIsoFunc63", inUnPackIsoFunc63},
    {"", (DFUNCTION_inISOUnPack)NULL},
};

// can declare any functions type and link with string.
Func_inISOCheck stFunctionList_inISOCheck[] = {
	{"", (DFUNCTION_inISOCheck)NULL},
};

int inPOSFunctionList(void)
{		
}

int inCTOSS_ExeFunction(char *INuszFunctionName)
{
	int inDex, inRetVal = -1;

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
			if (stFunctionList_vdFunc[inDex].uszFunctionName[0]==0x00)
			{
		        vduiWarningSound();
				vduiDisplayStringCenter(7,INuszFunctionName);
				vduiDisplayStringCenter(8,"FUNCTION INVALID");
				break;
			}
			
			if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_vdFunc[inDex].uszFunctionName))
			{
			   vdDebug_LogPrintf("%s", INuszFunctionName); 	  			
			   inRetVal = stFunctionList_vdFunc[inDex].d_FunctionP();
			   break;
			}
	 }
	 return(inRetVal);
}

int inExeFunction_PackISO(char *INuszFunctionName, unsigned char *uszSendData)
{
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOPack[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOPack[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		 
			   inRetVal = stFunctionList_inISOPack[inDex].d_FunctionP(srTransPara, uszSendData);
			   break;
		  }
	 }
	 return(inRetVal);
}

int inExeFunction_UnPackISO(char *INuszFunctionName, unsigned char *uszReceiveData)
{	
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;

    srTransPara = srGetISOEngTransDataAddress();

	if (INuszFunctionName[0] == 0x00)
		return inRetVal;
		
	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOUnPack[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOUnPack[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		  
			   inRetVal = stFunctionList_inISOUnPack[inDex].d_FunctionP(srTransPara, uszReceiveData);
			   break;
		  }
	 }
	 return(inRetVal);
}

int inExeFunction_CheckISO(char *INuszFunctionName, unsigned char *uszSendData, unsigned char *uszReceiveData)
{
	int inDex, inRetVal = ST_SUCCESS;
    TRANS_DATA_TABLE* srTransPara;
    
    srTransPara = srGetISOEngTransDataAddress();

	 if (INuszFunctionName[0] == 0x00)
		 return inRetVal;

	 for (inDex = 0; inDex < MAXFUNCTIONS; ++inDex)
	 {
		  if (stFunctionList_inISOCheck[inDex].uszFunctionName[0]==0x00)
		  {
			  vduiWarningSound();
			  vduiDisplayStringCenter(7,INuszFunctionName);
			  vduiDisplayStringCenter(8,"FUNCTION INVALID");
			  break;
		  }
		  if (!strcmp((char *)INuszFunctionName, (char *)stFunctionList_inISOCheck[inDex].uszFunctionName))
		  {
			   vdDebug_LogPrintf("%s", INuszFunctionName);		 		  
			   inRetVal = stFunctionList_inISOCheck[inDex].d_FunctionP(srTransPara, uszSendData, uszReceiveData);
			   break;
		  }
	 }
	 return(inRetVal);
}

