
#ifndef ___POS_MAIN_H___
#define ___POS_MAIN_H___


int inCTOS_DisplayIdleBMP(void);
int inCTOS_DisplayComTypeICO(void);
int inCTOS_ValidFirstIdleKey(void);
BYTE chGetFirstIdleKey(void);
void vdSetFirstIdleKey(BYTE bFirstKey);
int inCTOS_IdleEventProcess(void);
int inCTOSS_InitAP(void);
BYTE chGetIdleEventSC_MSR(void);
void vdSetIdleEventSC_MSR(BYTE bIdleEventSC_MSR);
int inCTOS_AUTOSETTLE(void);
int inCTOS_CHECKMUSTSETTLE(void);
int inAutoSwitchSIMSlot(void);
int inCTOSS_AutoSwitchSIM(int inMode);
void vdCTOSS_SavingInfo_SIM1SIM2(int inSwitchNum);
int inCTOSS_SwitchSIM2(void);
int inCTOSS_SwitchSIM1(void);
int inCTOS_UploadERMReceipt(void);
void vdCTMS_DualSIMSetting(int inSwitchNum);
int inCTOSS_ManualSwitchSIMProcess(int inMode);










#endif //end ___POS_MAIN_H___

