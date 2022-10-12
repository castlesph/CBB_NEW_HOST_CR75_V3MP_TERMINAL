/*******************************************************************************

 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <ctoserr.h>
#include <stdlib.h>
#include <stdarg.h>
#include <emv_cl.h>

#include "..\Includes\POSTypedef.h"
#include "..\Debug\Debug.h"

#include "..\FileModule\myFileFunc.h"
#include "..\DataBase\DataBaseFunc.h"

#include "..\Includes\showbmp.h"
#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSConfig.h"
#include "..\Includes\POSSettlement.h"
#include "..\Includes\epad.h"

#include "..\ui\Display.h"
#include "..\Includes\Dmenu.h"
#include "..\Includes\POSVoid.h"
#include "..\powrfail\POSPOWRFAIL.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\CardUtil.h"

#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Includes\CfgExpress.h"
#include "..\Includes\Wub_lib.h"
#include "..\Aptrans\MultiShareECR.h"
#include "..\Aptrans\MultiShareCOM.h"
#include "..\TMS\TMS.h"
#include "..\Ctls\POSCtls.h"
#include "..\Includes\POSSetting.h"
#include "..\Ctls\PosWave.h"
#include "..\Erm\PosErm.h"
#include "..\Aptrans\MultiShareEMV.h"
#include "..\PCI100\PCI100.h"
#include "..\Htle\htlesrc.h"
#include "..\Includes\epad.h"
#include "..\Includes\CTOSInput.h"
#include "..\Includes\POSMPUSale.h"
#include "..\PinPad\pinpad.h"

#include <ctos_qrcode.h>

//#define UNUSE
//#define NEWICON
extern USHORT CTOS_VirtualFunctionKeyHit(BYTE *key);
extern USHORT CTOS_VirtualFunctionKeySet(CTOS_VIRTUAL_FUNCTION_KEY_SET_PARA* pPara, BYTE FuncKeyCount);

CTOS_VIRTUAL_FUNCTION_KEY_SET_PARA *nPtr = NULL;
BYTE funckey_count = 0;

CTOS_VIRTUAL_FUNCTION_KEY_SET_PARA stKeyPara [4] ={
    {0, 400, 80, 480}, //F1
    {80, 400, 160, 480}, //F2
    {160, 400, 240, 480}, //F3
    {240, 400, 320, 480} //F4
};

extern USHORT GPRSCONNETSTATUS;

static BYTE stgFirstIdleKey = 0x00;
static BYTE gIdleEventSC_MSR = 0x00;
extern BOOL fGPRSConnectOK;
extern USHORT usHWSupport;
static BYTE stgIdleEvent = 0;
static int g_inAppRidding = 1;
static BYTE stgCurrIdx = 0;
static int inIdleTimeoutCtr=0;

BOOL fSIMCardSlot;

void vdCTOSS_ClearTouchPanelTest(void) {
    USHORT ret;
    BYTE key;

    if (strTCT.byTerminalType == 4) {
        do {
            ret = CTOS_VirtualFunctionKeyHit(&key);
            vdDebug_LogPrintf("CTOS_VirtualFunctionKeyHit.key=[%d][%c]", key, key);
            if (key == d_KBD_INVALID) {
                CTOS_KBDHit(&key);
                break;
            }

        } while (key != d_KBD_INVALID);
    }

}

int inGetAppRidding(void) {
    return g_inAppRidding;
}

void vdSetAppRidding(int inRiddingID) {
    g_inAppRidding = inRiddingID;
}

BYTE chGetFirstIdleKey(void) {
    return stgFirstIdleKey;
}

void vdSetFirstIdleKey(BYTE bFirstKey) {
    stgFirstIdleKey = bFirstKey;
}

BYTE chGetIdleEvent(void) {
    return stgIdleEvent;
}

void vdSetIdleEvent(BYTE bIdleEvent) {
    stgIdleEvent = bIdleEvent;
}

BYTE chGetIdleEventSC_MSR(void) {
    return gIdleEventSC_MSR;
}

void vdSetIdleEventSC_MSR(BYTE bIdleEventSC_MSR) {
    gIdleEventSC_MSR = bIdleEventSC_MSR;
}

int inCTOS_ValidFirstIdleKey(void) {
    if ((chGetFirstIdleKey() >= d_KBD_1) && (chGetFirstIdleKey() <= d_KBD_9))
        return d_OK;
    else
        return d_NO;
}

int inCTOS_ECRTask(void) {

    if (strTCT.fECR) // tct
    {
        inMultiAP_ECRGetMainroutine();
    }
    if (strTCT.byPinPadMode == 1) //for External pinpad&CTLS&Signature
    {
        inMultiAP_EXTGetMainroutine();
    }
    return SUCCESS;
}

int inCTOSS_JumpToSleepMode(void) {
    static int inSleepTime = 2;
    static int Sleepflag = 0;
    static unsigned long linux_time = 0;
    unsigned long linux_crttime;

    linux_crttime = CTOS_TickGet();

    if (chGetIdleEvent() == 1) {
        inSleepTime = get_env_int("SLEEPTIME");
        if (inSleepTime < 0)
            inSleepTime = 90;

        if (inSleepTime == 0)
            Sleepflag = 1;

        vdDebug_LogPrintf("inCTOSS_JumpToSleepMode,inSleepTime=[%d],Sleepflag=[%d]", inSleepTime, Sleepflag);
        vdSetIdleEvent(0);

        linux_time = CTOS_TickGet();
    }

    //vdDebug_LogPrintf("linux_crttime=[%d],linux_time=[%d],(inSleepTime*100)=[%d]", linux_crttime,linux_time,(inSleepTime*100));
    if ((linux_crttime > linux_time + (inSleepTime * 100)) && Sleepflag != 1) {
        vdCTOS_uiIDLESleepMode();
    }
    if (inCTOSS_CheckBatteryChargeStatus() == d_OK) {
        linux_time = CTOS_TickGet();
    }
#if 0	
    if ((linux_crttime > linux_time + (inSleepTime * 100)) && Sleepflag != 1)//for auto d_BKLIT_LCD&d_BKLIT_KBD off
    {
        vdDebug_LogPrintf("auto d_BKLIT_LCD&d_BKLIT_KBD off...");
        CTOS_BackLightSet(d_BKLIT_LCD, d_OFF);
        CTOS_BackLightSet(d_BKLIT_KBD, d_OFF);
    }
#endif	
    return SUCCESS;
}

/*DMENU-Update 20160601 start With new DMenu, we can set all key event in DMENU Database*/
int inCTOS_Key0Event(void) {
    int ret = d_OK;
    vdDebug_LogPrintf("=====inCTOS_Key0Event=====");

    ret = inCTOS_PromptPassword();
    if (d_OK != ret)
        return ret;

    inSetColorMenuMode();
    inF1KeyEvent();

    return d_OK;
}

int inCTOS_Key1Event(void) {
    BYTE key = 0x00;
	
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key1Event=====");

    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);
	
    inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
		inCTOS_IPP_INSTALLMENT();    

    else if (TRUE == fGetCashAdvAppFlag())
		inCTOS_CashAdvance();

	
    else		
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();
	

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key2Event(void) {
    BYTE key = 0x00;	
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key2Event=====");

    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);
    vdSetButtonFromIdle(TRUE);

	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
		inCTOS_IPP_INSTALLMENT();

    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key3Event(void) {
    BYTE key = 0x00;	
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key3Event=====");
	
    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);

	
	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
		inCTOS_IPP_INSTALLMENT();

    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key4Event(void) {
    BYTE key = 0x00;
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key4Event=====");

    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);

	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
   		inCTOS_IPP_INSTALLMENT();

    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key5Event(void) {
    BYTE key = 0x00;
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key5Event=====");

    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);

	
	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
   		inCTOS_IPP_INSTALLMENT();

    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key6Event(void) {
    BYTE key = 0x00;
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key6Event=====");

    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);

	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
  		 inCTOS_IPP_INSTALLMENT();


    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key7Event(void) {
    BYTE key = 0x00;	
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key7Event=====");

    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);

	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
   		inCTOS_IPP_INSTALLMENT();


    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key8Event(void) {
    BYTE key = 0x00;
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key8Event=====");

    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);

	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
  		 inCTOS_IPP_INSTALLMENT();

    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

int inCTOS_Key9Event(void) {
    BYTE key = 0x00;
	int inOrgMenuid;

    vdDebug_LogPrintf("=====inCTOS_Key9Event=====");


    CTOS_KBDGet(&key);
    vdDebug_LogPrintf("key[%d]", key);

    vdSetFirstIdleKey(key);
    vdSetIdleEventSC_MSR(1);

    vdSetButtonFromIdle(TRUE);

	inTCTGetCurrMenuid(1, &inOrgMenuid);
	if(inOrgMenuid==3)
  		 inCTOS_IPP_INSTALLMENT();


    else if (TRUE == fGetCashAdvAppFlag())
        inCTOS_CashAdvance();
    else
        //		inCTOS_SALE();
        inCTOS_MPU_SALE();

    vdSetButtonFromIdle(FALSE);

    vdSetFirstIdleKey(0x00);
    vdSetIdleEventSC_MSR(0);

    return d_OK;
}

/*DMENU-Update 20160601 End With new DMenu, we can set all key event in DMENU Database*/


int inCTOS_IdleRunningTestFun4(void) {
    return SUCCESS;
}

int inCTOS_IdleRunningTestFun5(void) {
    return SUCCESS;
}

int inCTOS_IdleRunningTestFun6(void) {
    return SUCCESS;
}

int inCTOS_IdleRunningTestFun7(void) {
    return SUCCESS;
}

int inCTOS_IdleRunningTestFun8(void) {
    return SUCCESS;
}

#if 0
int inCTOS_AUTOSETTLE(void) {

    vdCheckTimeAutoSettle();
    if (strTCT.fMustAutoSettle == TRUE) {
        inCTOS_AUTO_SETTLE();
        strTCT.fMustAutoSettle = FALSE;
        inTCTSave(1);
    }

    return SUCCESS;
}
#endif

int inCTOSS_CheckAutoReversalTime(void) {
    //	USHORT i;
    //	CTOS_RTC LAST_DATE;
    //	CTOS_RTC TIME_SET;
    //	CTOS_RTC Update_Date;
    //	char u1t_Date[10];

    static unsigned long linux_time = 0;
    unsigned long linux_crttime;

    vdDebug_LogPrintf("inCTOSS_CheckAutoReversalTime....");


    linux_crttime = CTOS_TickGet();
    if (linux_crttime > linux_time + 6000)
	{
        linux_time = CTOS_TickGet();

        inCTOS_MPU_Reversal();
    }
    return SUCCESS;
}

int inCTOSS_CheckAlipayAutoReversalTime(void) {
    static unsigned long linux_time = 0;
    unsigned long linux_crttime;
    int inHostIndex = 0;

    if (inGetAppRidding() == RIDDING_AMEX)
        return SUCCESS;

    //inSleepFlag = inCTOSS_GetSleepFlag();
    //vdDebug_LogPrintf("inCTOSS_CheckAutoReversalTime.inSleepFlag=[%d]....",inSleepFlag);

    //if (inSleepFlag == 1)
    //	return SUCCESS;

    linux_crttime = CTOS_TickGet();
    if (linux_crttime > linux_time + 12000) {
        linux_time = CTOS_TickGet();

        //go to Alipay to process reversal
        inHostIndex = inHDTRead(ALIPAY_HOST_INDEX);
        if (inHostIndex != d_OK) // Alipay is disabled, check the WeChat
        {
            vdDebug_LogPrintf("ALIPAY_HOST_INDEX err");
            inHostIndex = inHDTRead(WPAY_HOST_INDEX);
            if (inHostIndex != d_OK) // WeChat is also disabled, check QQWallet
            {
                vdDebug_LogPrintf("WPAY_HOST_INDEX err");
                inHostIndex = inHDTRead(QQWALLET_HOST_INDEX);
                if (inHostIndex != d_OK) {
                    vdDebug_LogPrintf("QQWALLET_HOST_INDEX err");
                    return SUCCESS; // All hosts are disabled, no need to check reversal
                }
            }
        }
        vdDebug_LogPrintf("Auto reversal inHostIndex[%d]....", inHostIndex);

        inCTOS_MultiAPSaveData(d_IPC_CMD_REVERSAL);

    }
    return SUCCESS;
}


extern int inEthernet_GetTerminalIP(char *szTerminalIP);

int inCTOS_ConfigureCommDevice(void) {
    BOOL BolDetachLANChange = TRUE;
    BYTE strtemp[17];
    USHORT usRtn;
    BOOL fPortable;
    BOOL fPCI;
    USHORT mkHWSupport;

    if (BolDetachLANChange == TRUE) {
        //inCPTRead(1);
        usRtn = CTOS_HWSettingGet(&fPortable, &fPCI, &mkHWSupport);
        vdDebug_LogPrintf("CTOS_HWSettingGet,usRtn=[%x],fPortable[%d],fPCI=[%d],mkHWSupport=[%04x]", usRtn, fPortable, fPCI, mkHWSupport);
        if (usRtn != d_OK) {
            return SUCCESS;
        }

        vdDebug_LogPrintf("mkHWSupport & d_MK_HW_GPRS=[%04x]", mkHWSupport & d_MK_HW_GPRS);
        if ((mkHWSupport & d_MK_HW_ETHERNET) != d_MK_HW_ETHERNET) {
            return SUCCESS;
        }

        srTransRec.usTerminalCommunicationMode = ETHERNET_MODE;
        //vduiClearBelow(3);
        if ((strTCT.byTerminalType % 2) == 0) {
            clearLine(12);
            clearLine(13);
            CTOS_LCDTPrintXY(1, 12, "Please Wait 	");
        } else {
            clearLine(7);
            clearLine(8);
            CTOS_LCDTPrintXY(1, 7, "Please Wait 	");
        }

        if ((strTCT.byTerminalType % 2) == 0)
            CTOS_LCDTPrintXY(1, 13, "Init LAN... 	");
        else
            CTOS_LCDTPrintXY(1, 8, "Init LAN... 	");

        if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) {

            vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
            return SUCCESS;
        }

        srCommFuncPoint.inInitComm(&srTransRec, srTransRec.usTerminalCommunicationMode);
        srCommFuncPoint.inGetCommConfig(&srTransRec);
        srCommFuncPoint.inSetCommConfig(&srTransRec);

    }

    //if(strCPT.inCommunicationMode == ETHERNET_MODE)
    {
        if (strTCP.fDHCPEnable != IPCONFIG_STATIC) {
            inEthernet_GetTerminalIP(strtemp);

            // set static IP			
            strTCP.fDHCPEnable = IPCONFIG_STATIC;
            strcpy(srEthernetRec.strLocalIP, strtemp);
            srCommFuncPoint.inSetCommConfig(&srTransRec);

            // set DHCP
            strTCP.fDHCPEnable = IPCONFIG_DHCP;
            srCommFuncPoint.inSetCommConfig(&srTransRec);

            //vduiClearBelow(3);
            if ((strTCT.byTerminalType % 2) == 0) {
                clearLine(12);
                clearLine(13);
                setLCDPrint(12, DISPLAY_POSITION_LEFT, "DHCP TERMINAL IP");
                setLCDPrint(13, DISPLAY_POSITION_LEFT, strtemp);
            } else {
                clearLine(7);
                clearLine(8);
                setLCDPrint(7, DISPLAY_POSITION_LEFT, "DHCP TERMINAL IP");
                setLCDPrint(8, DISPLAY_POSITION_LEFT, strtemp);
            }
        }
    }

    return SUCCESS;
}

int inCTOS_ConfigureCTLSReader(void) {
    // patrick add code 20140903
    inTCTRead(1);
    if (strTCT.fFirstInit) {
        if ((strTCT.byTerminalType % 2) == 0) {
            if (strTCT.inThemesType == 0) {
                vdCTOSS_CombineMenuBMP("BG_SPRING.BMP");
                vdBackUpWhiteBMP("SPRING.BMP", "WHITE.BMP");
                vdBackUpWhiteBMP("SPRINGTOUCH.BMP", "menutouch.bmp");
                vdBackUpWhiteBMP("SPRINGITEM.BMP", "menuitem.bmp");
            }
            if (strTCT.inThemesType == 1) {
                vdCTOSS_CombineMenuBMP("BG_SUMMER.BMP");
                vdBackUpWhiteBMP("SUMMER.BMP", "WHITE.BMP");
                vdBackUpWhiteBMP("SUMMERTOUCH.BMP", "menutouch.bmp");
                vdBackUpWhiteBMP("SUMMERITEM.BMP", "menuitem.bmp");
            }
        }
        inCTOS_ConfigureCommDevice();

        if (NO_CTLS != inCTOSS_GetCtlsMode()) {
            vdCTOS_InitWaveData();
        }

        strTCT.fFirstInit = 0;
        inTCTSave(1);

        if (NO_CTLS != inCTOSS_GetCtlsMode() && (strTCT.byPinPadMode == 1)) {
            CTOS_Delay(1000);
            CTOS_LCDTPrintXY(1, 8, "CONFIGURE READER OK");
            CTOS_Beep();
            CTOS_Delay(1500);
            CTOS_LCDTClearDisplay();
        }
    }
    
    CTOS_LCDTClearDisplay();

    return SUCCESS;
}

int inCTOS_DisplayIdleBMP(void) 
{

	int inGetHDTDefCurrVal  = get_env_int("#CURRDEFVAL");

    if (strTCT.byPinPadMode == 1) 
	{
        return SUCCESS;
    }
    //if (!strTCT.fECR) // tct
    //vdDisplayAnimateBmp(0, 32, "Insert1.bmp", "Insert2.bmp", "Insert3.bmp", NULL, NULL);
    
//    vduiClearBelow(2);
    if (fGetIdleScreenShow() == TRUE) 
	{
        if (strTCT.fEnableAmountIdle == TRUE) 
		{
			vdDebug_LogPrintf("CBB Idle BMP!!!");
//            if (strcmp(strCST.szCurCode, "840") == 0)
//                setLCDPrint(8, DISPLAY_POSITION_LEFT, "USD                          0.00");
//            else if (strcmp(strCST.szCurCode, "104") == 0)
//                setLCDPrint(8, DISPLAY_POSITION_LEFT, "MMK                                0");



			// new logo value is 1 else (2) old logo
			#if 0
			inTCTRead(1);			
			vdDebug_LogPrintf("fDisplayIDLEOption [%d]", strTCT.fDisplayIDLEOption);
			
			if (strTCT.fDisplayIDLEOption == 1) 				
				vdDisplayAnimateBmp(0, 32, "Insert5.bmp", NULL, NULL, NULL, NULL);
			else
                vdDisplayAnimateBmp(0, 32, "Insert5_orig.bmp", NULL, NULL, NULL, NULL);
			#endif
			
			vdDebug_LogPrintf("inCTOS_DisplayIdleBMP inHostIndex[%d] HDT.inCurrencyIdx[%d]CST.szCurCode[%s] CST.inCurrencyIndex[%d]inGetHDTDefCurrVal[%d]strTCT.inThemesType[%d]",
			strHDT.inHostIndex, strHDT.inCurrencyIdx, strCST.szCurCode,  strCST.inCurrencyIndex, inGetHDTDefCurrVal, strTCT.inThemesType);

			#ifdef CBB_FIN_ROUTING
			if(inGetHDTDefCurrVal == 2)				
				vdDisplayAnimateBmp(0, 32, "Insert5.bmp", NULL, NULL, NULL, NULL);
			else				
				vdDisplayAnimateBmp(0, 32, "Insert6.bmp", NULL, NULL, NULL, NULL);
			#endif
			#if 0
	            if (strcmp(strCST.szCurCode, "840") == 0)
	                vdDisplayAnimateBmp(0, 32, "Insert5.bmp", NULL, NULL, NULL, NULL);
	            else if (strcmp(strCST.szCurCode, "104") == 0)
	                vdDisplayAnimateBmp(0, 32, "Insert6.bmp", NULL, NULL, NULL, NULL);
			#endif
			#if 0
			vdDisplayAnimateBmp(0, 32, "Insert7.bmp", NULL, NULL, NULL, NULL);
			setLCDPrint(7, DISPLAY_POSITION_LEFT, "AMOUNT:");
			if (strcmp(strCST.szCurCode, "840") == 0)
				setLCDPrint(8, DISPLAY_POSITION_LEFT, "                                0.00");
            else if (strcmp(strCST.szCurCode, "104") == 0)
                setLCDPrint(8, DISPLAY_POSITION_LEFT, "MMK                                0");
			#endif
			
        }
		else 
		{
            if (strTCT.inThemesType == 7) 
			{
                vdDisplayAnimateBmp(0, 32, "Insert4.bmp", NULL, NULL, NULL, NULL);
            } 
			else 
           	{
                vdDisplayAnimateBmp(0, 32, "Insert1.bmp", NULL, NULL, NULL, NULL);
            }
        }
    }
	else 
	{
        if (strTCT.inThemesType == 7) 
		{
			vdDisplayAnimateBmp(0, 32, "Insert4.bmp", NULL, NULL, NULL, NULL);
		} 
		else
		{
			vdDisplayAnimateBmp(0, 32, "Insert1.bmp", NULL, NULL, NULL, NULL);
		}
    }
    
    return SUCCESS;
}

int inCTOS_IdleDisplayDateAndTime(void) {
    CTOS_RTC SetRTC;
    BYTE szCurrentTime[50];
    BYTE szCurrentDate[50];
    static unsigned long linux_time = 0;
    unsigned long linux_crttime;
    static int gIdleDisplayTime = 0;

    if (strTCT.byPinPadMode == 1) {
        return SUCCESS;
    }

    linux_crttime = CTOS_TickGet();


    memset(szCurrentTime, 0x00, sizeof (szCurrentTime));
    memset(szCurrentDate, 0x00, sizeof (szCurrentDate));
    CTOS_RTCGet(&SetRTC);
    //	sprintf(szCurrentTime,"%d-%02d-%02d  %02d:%02d",SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour,SetRTC.bMinute);
    //	sprintf(szCurrentTime,"%02d-%02d %02d:%02d",SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour,SetRTC.bMinute);
    sprintf(szCurrentDate, "%d-%02d-%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
    sprintf(szCurrentTime, "%02d:%02d", SetRTC.bHour, SetRTC.bMinute);
	
    //    CTOS_LCDTPrintXY (13,1, szCurrentTime);
    CTOS_LCDTSelectFontSize(d_FONT_16x16);

    if (linux_crttime > linux_time + 300) {
        linux_time = CTOS_TickGet();
        if (gIdleDisplayTime == 0) {
            gIdleDisplayTime = 1;
            //CTOS_LCDTPrintXY (1, 1, "            ");
            CTOS_LCDTPrintAligned(1, "                ", d_LCD_ALIGNLEFT);
        } else
            gIdleDisplayTime = 0;
    }
    if (gIdleDisplayTime == 1)
        //CTOS_LCDTPrintXY (1, 1, szCurrentTime);
        CTOS_LCDTPrintAligned(1, szCurrentTime, d_LCD_ALIGNLEFT);
    else
        //CTOS_LCDTPrintXY (1, 1, szCurrentDate);
        CTOS_LCDTPrintAligned(1, szCurrentDate, d_LCD_ALIGNLEFT);

    CTOS_LCDTSelectFontSize(d_FONT_16x30);

    return SUCCESS;
}

int inCTOS_DisplayComTypeICO(void) {
#define SMALL_ICON_LEN	24
#define SMALL_ICON_GAP	2
#define FIRST_SMALL_ICON	1
#define SMALL_ICON_LEN2		5


    USHORT usRet,usRtn;
    USHORT usRet1;
    DWORD pdwStatus;
    BYTE bPercentage;
    static int inLoop = 0;
    static int inGPRSSignal = 0;
    static USHORT usNetworkType = 0;
    static USHORT usEthType = 1;
    static BYTE szNetworkName[128 + 1];
    static BYTE szWIFISSID[50];
    static unsigned long linux_time = 0;
    unsigned long linux_crttime;
    static int gIdleDisplayIcon = 0;
    char szMessage[30 + 1];
    int inCommBack;
    int inPriCommMode;
    int inSecCommMode;
    STRUCT_SHARLS_COM Sharls_COMData;
    int inResult = -1;
    static int inWifiSignal = 0;

	 int inGPRSResult = 0;//thandar	 
	 static USHORT usBTType = 1;

    memset(&Sharls_COMData, 0x00, sizeof (STRUCT_SHARLS_COM));

    linux_crttime = CTOS_TickGet();
    if (strTCT.byPinPadMode == 1) {
        return SUCCESS;
    }

    if (strTCT.fDemo) {
        displaybmpEx(300, 90, "DEMO.bmp");
    }

#ifdef UNUSE
    //display bluetooth
    {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*5, 0, "BTNone.bmp");
    }
    //display vifi
    {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VFNone.bmp");
    }
    //display GPS
    {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*5 - 10, 16, "GPSNone.bmp");
    }
#endif

    //display ECR
    if (strTCT.fECR) {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*3 - 10, 16, "ECR.bmp");
    } else {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*3 - 10, 16, "None.bmp");
    }

    //display ERM
    if (strTCT.byERMMode != 0) {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4 - 10, 16, "ERM.bmp");
    } else {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4 - 10, 16, "None.bmp");
    }
    //for External pinpad&CTLS&Signature
    if (strTCT.byPinPadType == 3 || strTCT.byPinPadMode == 1) {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*5 - 10, 16, "EXT.bmp");
    } else {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*5 - 10, 16, "None.bmp");
    }
    //display Transaction COMMS mode
    {
        memset(szMessage, 0x00, sizeof (szMessage));
        inCTOSS_GetEnv("PRICOMMMODE", szMessage);
        if (strlen(szMessage) > 0)
            inPriCommMode = atoi(szMessage);
        else
            inPriCommMode = 1;

        memset(szMessage, 0x00, sizeof (szMessage));
        inCTOSS_GetEnv("SECCOMMMODE", szMessage);
        if (strlen(szMessage) > 0)
            inSecCommMode = atoi(szMessage);
        else
            inSecCommMode = 0;

        memset(szMessage, 0x00, sizeof (szMessage));
        inCTOSS_GetEnv("COMMBACK", szMessage);
        if (strlen(szMessage) > 0)
            inCommBack = atoi(szMessage);
        else
            inCommBack = 0;
        if (inSecCommMode == inPriCommMode)
            inSecCommMode = NULL_MODE;
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2 - 10, 16, "No2.bmp");
        switch (inSecCommMode) {
            case DIAL_UP_MODE:
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2 - 5, 16, "Modem.bmp");
                break;
            case ETHERNET_MODE:
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2 - 5, 16, "LAN.bmp");
                break;
            case GPRS_MODE:
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2 - 5, 16, "G.bmp");
                break;
            case WIFI_MODE:
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2 - 5, 16, "VF4.bmp");
                break;
            case COM1_MODE:
            case COM2_MODE:
            case USB_MODE:
            case NULL_MODE:
            default:
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2 - 5, 16, "None.bmp");
                break;
        }
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP) - SMALL_ICON_GAP - 5, 16, "12.bmp");
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP) - 5, 16, "No1.bmp");
        if (inCommBack == 0) {
            switch (inPriCommMode) {
                case DIAL_UP_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "Modem.bmp");
                    break;
                case ETHERNET_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "LAN.bmp");
                    break;
                case GPRS_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "G.bmp");
                    break;
                case WIFI_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "VF4.bmp");
                    break;
                case COM1_MODE:
                case COM2_MODE:
                case USB_MODE:
                case NULL_MODE:
                default:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "None.bmp");
                    break;
            }
        } else {
            switch (inPriCommMode) {
                case DIAL_UP_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "ModemFail.bmp");
                    break;
                case ETHERNET_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "LANFail.bmp");
                    break;
                case GPRS_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "GFail.bmp");
                    break;
                case WIFI_MODE:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "VFFail.bmp");
                    break;
                case COM1_MODE:
                case COM2_MODE:
                case USB_MODE:
                case NULL_MODE:
                default:
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 16, "None.bmp");
                    break;
            }
        }
    }

    pdwStatus = 0x00;
    usRet = CTOS_BatteryStatus(&pdwStatus);
    //vdDebug_LogPrintf("**CTOS_BatteryStatus [%X] [%02X]**", usRet, pdwStatus);
    usRet1 = CTOS_BatteryGetCapacity(&bPercentage);
    //vdDebug_LogPrintf("**CTOS_BatteryGetCapacity [%X] [%d]**", usRet1, bPercentage);

    if (d_BATTERY_NOT_EXIST == usRet1 || d_BATTERY_NOT_SUPPORT == usRet1 || d_BATTERY_INVALID == usRet1) {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "Plugged.bmp");
    } else if (d_OK == usRet1) {
#if 0
        if (bPercentage <= 10) {
            if (d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE)) {
            } else {
                / CTOS_SystemReset();
            }
        } else if (bPercentage <= 20)
#endif
            if (bPercentage <= 1) {
                if (d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE)) {
                } else {
                    vdDisplayAnimateBmp(245, 150, "Low_battery.bmp", NULL, NULL, NULL, NULL);
                    CTOS_Beep();
                }
            }

        if (bPercentage <= 25) {
            if (d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE)) {
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "25N.bmp");
            } else {
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "25.bmp");
            }
        } else if (bPercentage <= 50) {
            if (d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE)) {
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "50N.bmp");
            } else {
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "50.bmp");
            }
        } else if (bPercentage <= 75) {
            if (d_OK == usRet && (pdwStatus & d_MK_BATTERY_CHARGE)) {
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "75N.bmp");
            } else {
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "75.bmp");
            }

        } else if (bPercentage <= 100) {
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 0, "100.bmp");
        }
    }

    if (linux_crttime > linux_time + 500 || inLoop == 0) {
        inLoop = 1;
        linux_time = CTOS_TickGet();
        //		inGPRSSignal = inCTOSS_GetGPRSSignalEx(&usNetworkType, szNetworkName,&usEthType);
        inGPRSSignal = inCTOSS_GetGPRSSignalEx1(&usNetworkType, szNetworkName, &usEthType, &Sharls_COMData);
        inWifiSignal = Sharls_COMData.inReserved2;
		usBTType = Sharls_COMData.usBLUETOOTHType;
		
        vdDebug_LogPrintf("**inCTOSS_GetGPRSSignal inGPRSSignal=[%d]**usEthType=[%d] *** strTCP.inSIMSlot[%d] linux_time [%d] ", inGPRSSignal, usEthType,strTCP.inSIMSlot,linux_time);
//		if((inGPRSSignal==0)&&(get_env_int("AUTOSWITCHSIM") == 1))
//		inCTOSS_AutoSwitchSIM(2);//Emmy 4OCT2018
		
		
/*		if((inGPRSSignal==0)&&(get_env_int("AUTOSWITCHSIM") == 1)&&(strTCP.inSIMSlot==1))	
			{
			vdDebug_LogPrintf("(inGPRSSignal==0)&&(get_env_int(AUTOSWITCHSIM) == 1)&&(strTCP.inSIMSlot==1)");			
			inTCPRead(2);
			inTCPSave(1);
			usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM2);//v5s rootfs=R0041 need wait 10 secs
			CTOS_Delay(30);
			usRtn = CTOS_SIMCheckReady();
			CTOS_Delay(45);

			}
		else if ((inGPRSSignal==0)&&(get_env_int("AUTOSWITCHSIM") == 1)&&(strTCP.inSIMSlot==2))
			{
			vdDebug_LogPrintf("(inGPRSSignal==0)&&(get_env_int(AUTOSWITCHSIM) == 1)&&(strTCP.inSIMSlot==2)");			
			inTCPRead(3);
			inTCPSave(1);
			usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM1);//v5s rootfs=R0041 need wait 10 secs
			CTOS_Delay(30);
			usRtn = CTOS_SIMCheckReady();
			CTOS_Delay(45);
			}*/
        vdDebug_LogPrintf("**inCTOSS_GetGPRSSignal inGPRSSignal=[%d]**usEthType=[%d]", inGPRSSignal, usEthType);
        vdDebug_LogPrintf("**usNetworkType [%2x][%2x][%d]**", d_MOBILE_NETWORK_UMTS, usNetworkType, (d_MOBILE_NETWORK_UMTS & usNetworkType));
        CTOS_LCDTSelectFontSize(d_FONT_16x16);
        //CTOS_LCDTPrintXY (1, 2, "                    ");
        CTOS_LCDTPrintAligned(2, "                    ", d_LCD_ALIGNLEFT);
        CTOS_LCDTSelectFontSize(d_FONT_16x30);

        memset(szWIFISSID, 0x00, sizeof (szWIFISSID));
        if (Sharls_COMData.usWIFIType == 1)
            strcpy(szWIFISSID, Sharls_COMData.szWIFISSID);

        if (gIdleDisplayIcon == 0) {
            gIdleDisplayIcon = 1;
        } else
            gIdleDisplayIcon = 0;

    }


    //display modem icon
    if ((usHWSupport & d_MK_HW_MODEM) == d_MK_HW_MODEM) {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2, 0, "Modem.bmp");
    } else
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2, 0, "ModemNone.bmp");

		//display BT  icon
	if ((usHWSupport & d_MK_HW_BT) == d_MK_HW_BT)
	{
		// vdDebug_LogPrintf("**BlueTooth usBTType=[%d]", usBTType);
		 
		if(usBTType == 1)
		{	
			displaybmpEx(319-(SMALL_ICON_LEN+SMALL_ICON_GAP)*5, 0, "BT.bmp");
		}
		else
		{
			displaybmpEx(319-(SMALL_ICON_LEN+SMALL_ICON_GAP)*5, 0, "BTNone.bmp");
		}
	}
	else
	{ 		//vdDebug_LogPrintf("**BlueTooth [BTInit]");
		if ((usHWSupport & d_MK_HW_BT) == d_MK_HW_BT)
			displaybmpEx(319-(SMALL_ICON_LEN+SMALL_ICON_GAP)*5, 0, "BTInit.bmp");
		else
			displaybmpEx(319-(SMALL_ICON_LEN+SMALL_ICON_GAP)*5, 0, "BTNone.bmp");
	}
	

    ////display wifi icon
    if ((usHWSupport & d_MK_HW_WIFI) == d_MK_HW_WIFI) {
        if (0 == inWifiSignal)
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VFNone.bmp");
        else if (1 == inWifiSignal)
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VF1.bmp");
        else if (2 == inWifiSignal)
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VF2.bmp");
        else if (3 == inWifiSignal)
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VF3.bmp");
        else if (4 == inWifiSignal)
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VF4.bmp");
        else {
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VF4.bmp");
        }
    } else {
        if (0 == usHWSupport)
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VFInit.bmp");
        else
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*4, 0, "VFNone.bmp");
    }
    //	vdDebug_LogPrintf("d_MK_HW_WIFI[%d], signal[%d]", usHWSupport & d_MK_HW_WIFI, Sharls_COMData.inReserved2);

    //vdDebug_LogPrintf("**gIdleDisplayIcon[%d]**",gIdleDisplayIcon);
    //display ETHERNET icon
    if ((usHWSupport & d_MK_HW_ETHERNET) == d_MK_HW_ETHERNET) {
        if (usEthType == 1) {
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*3, 0, "LAN.bmp");
        } else {
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*3, 0, "None.bmp");
        }
    } else
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*3, 0, "LANNone.bmp");

    // display wifi icon
    if ((usHWSupport & d_MK_HW_WIFI) == d_MK_HW_WIFI) {
        if (strlen(szWIFISSID) > 0) {
            if (gIdleDisplayIcon == 1) {
                CTOS_LCDTSelectFontSize(d_FONT_16x16);
                //CTOS_LCDTPrintXY (1, 2, szNetworkName);
                CTOS_LCDTPrintAligned(2, szWIFISSID, d_LCD_ALIGNLEFT);
                CTOS_LCDTSelectFontSize(d_FONT_16x30);
            }
        } else
            gIdleDisplayIcon = 0;
    } else
        gIdleDisplayIcon = 0;

    //display GPRS icon
    if ((usHWSupport & d_MK_HW_GPRS) == d_MK_HW_GPRS) {
        if (inGPRSSignal > 0) {
            if (gIdleDisplayIcon == 0) {
                CTOS_LCDTSelectFontSize(d_FONT_16x16);
                //CTOS_LCDTPrintXY (1, 2, szNetworkName);
                CTOS_LCDTPrintAligned(2, szNetworkName, d_LCD_ALIGNLEFT);
                CTOS_LCDTSelectFontSize(d_FONT_16x30);
            }

            if (fGPRSConnectOK == TRUE) {
#if 0			
                if (d_MOBILE_NETWORK_UMTS & usNetworkType)
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "3G.bmp");
                else if (d_MOBILE_NETWORK_GPRS & usNetworkType)
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "G.bmp");
                else if (d_MOBILE_NETWORK_GPRS & usNetworkType)
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "G.bmp");
                else
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "GNone.bmp");
#endif
                if ((d_GSM_NETWORK_WCDMA == usNetworkType) || (d_GSM_NETWORK_HSDPA == usNetworkType))
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "3G.bmp");
                else if ((d_MOBILE_NETWORK_GPRS == usNetworkType) || (d_GSM_NETWORK_EGPRS == usNetworkType))
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "G.bmp");
                else
                    displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "GNone.bmp");
            } else
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "GNone.bmp");

            #if 0
			//testing with hardcode
            if (2 == inGPRSSignal)//hardocode
					{
						inGPRSSignal=1;
						inGPRSResult=1;
            	    }
			#endif 
			
			if (1 == inGPRSSignal)
			{	inGPRSResult=1;
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*7, 0, "G1.bmp");
        	}
            else if (2 == inGPRSSignal)
            {
				//inGPRSResult=1;
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*7, 0, "G2.bmp");	
            }
            else if (3 == inGPRSSignal)
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*7, 0, "G3.bmp");
            else if (4 == inGPRSSignal)
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*7, 0, "G4.bmp");
            else {
				inGPRSResult=1;
                displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*7, 0, "None.bmp");
            }

			vdDebug_LogPrintf("x.inGPRSSignal=%d",inGPRSSignal);

			if(inGPRSResult==1 && (strTCP.inSIMSlot ==1 || strTCP.inSIMSlot ==2))
			{
				vdDebug_LogPrintf("switch, signal is low");
				inCTOSS_AutoSwitchSIM(2);
				inGPRSSignal=0;
				//linux_time = CTOS_TickGet();
			}


        } else {
            gIdleDisplayIcon = 1;
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*7, 0, "None.bmp");
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "None.bmp");
            //	CTOS_LCDTSelectFontSize(d_FONT_16x16);
            //	CTOS_LCDTPrintXY (1, 2, "No Service");
            //	CTOS_LCDTSelectFontSize(d_FONT_16x30);
        }
    } else {
        gIdleDisplayIcon = 1;
        if (0 == usHWSupport)
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "GInit.bmp");
        else
            displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*6, 0, "GNone.bmp");
    }

    inResult = inCTOSS_GetRemovableStorageStatus();
    if (inResult == 1)
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 32, "SDMount.bmp");
    else if (inResult == 2)
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2, 32, "PenDriveMount.bmp");
    else if (inResult == 3) {
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP), 32, "SDMount.bmp");
        displaybmpEx(319 - (SMALL_ICON_LEN + SMALL_ICON_GAP)*2, 32, "PenDriveMount.bmp");
    }

    return SUCCESS;
}

typedef struct {
    int (*inCTOS_IdleRunningFun)(void);
} IDLE_FUNC;

IDLE_FUNC g_IdleRunningFun[] ={
//    inCTOS_ConfigureCTLSReader,
    inCTOS_DisplayIdleBMP,
    inCTOS_DisplayComTypeICO,
    inCTOS_IdleDisplayDateAndTime,
    inCTOS_ECRTask,
    inCTOSS_JumpToSleepMode,
    inCTOSS_CheckAutoReversalTime,
    inCTOS_AUTOSETTLE,
    inCTOS_CHECKMUSTSETTLE,
    inCTOSS_RecoverRAM,
    //inCTOS_UploadERMReceipt,
    //    inCTOS_IdleRunningTestFun7,
    //    inCTOS_IdleRunningTestFun8,
    NULL
};


int inCTOS_UploadERMReceipt(void)
{
    vdDebug_LogPrintf("inCTOS_UploadERMReceipt");
    vdDebug_LogPrintf("strTCT.fUploadReceiptIdle=[%d]", strTCT.fUploadReceiptIdle);
	vdDebug_LogPrintf("strTCT.inIdleTimeout=[%d]", strTCT.inIdleTimeout);
    vdDebug_LogPrintf("inIdleTimeoutCtr=[%d]", inIdleTimeoutCtr);
	
    if(strTCT.fUploadReceiptIdle == TRUE)
    {
		inIdleTimeoutCtr++;
		
		if(inIdleTimeoutCtr >= (strTCT.inIdleTimeout * 60))
		{
		    inCTOSS_UploadReceipt();
			inIdleTimeoutCtr=0;
		}
    }
	
    return d_OK;  	
}

int inCTOSS_InitAP(void) {
    USHORT ret;
    char szDatabaseName[100 + 1];
    strcpy(szDatabaseName, "DMENGTHAI.S3DB");

    ret = CTOS_SetFunKeyPassword(strTCT.szFunKeyPW, 1);
    vdDebug_LogPrintf("CTOS_SetFunKeyPassword=[%s],ret=[%d]", strTCT.szFunKeyPW, ret);

    ret = CTOS_SetPMEnterPassword(strTCT.szPMpassword, 1);
    vdDebug_LogPrintf("CTOS_SetPMEnterPassword=[%s],ret=[%d]", strTCT.szPMpassword, ret);

    /**********************
            CTOS_PrinterSetHeatLevel 
            default value is 2. 
            = 0 : Ultra light. 	= 1 : Very light. 
            = 2 : Light. 		= 3 : Medium. 
            = 4 : Dark. 		= 5 : Very dark. 
            = 6 : Ultra dark.
     ***********************/
    ret = CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
    vdDebug_LogPrintf("CTOS_PrinterSetHeatLevel=[%d],ret=[%d]", strTCT.inPrinterHeatLevel, ret);

    if (strTCT.inThemesType == 1) {
        CTOS_LCDTTFSelect("tahoma.ttf", 0);
        CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

        CTOS_PrinterTTFSelect("tahoma.ttf", 0);
        inCTOSS_SetERMFontType("tahoma.ttf", 0);
        CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

        inSetLanguageDatabase(szDatabaseName);

        inCTOSS_SetALLApFont("tahoma.ttf");
    } else if (strTCT.inThemesType == 7) {
        //		CTOS_LCDTTFSelect(THEMESTYPE_7_FONT, 0);
        CTOS_LCDTTFSelect(ZAWGYI_FONT, 0);
        //CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

        //		CTOS_PrinterTTFSelect(THEMESTYPE_7_FONT, 0);
        //		inCTOSS_SetERMFontType(THEMESTYPE_7_FONT, 0);

        CTOS_PrinterTTFSelect(ZAWGYI_FONT, 0);
        inCTOSS_SetERMFontType(ZAWGYI_FONT, 0);
        //CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
        strcpy(szDatabaseName, "DMENGMYANMAR.S3DB");
        inSetLanguageDatabase(szDatabaseName);

        //		inCTOSS_SetALLApFont(THEMESTYPE_7_FONT);
        inCTOSS_SetALLApFont(ZAWGYI_FONT);
    } else {
        // patrick test 20150115 start
        CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
        CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);

        CTOS_PrinterTTFSelect(d_FONT_DEFAULT_TTF, 0);
        inCTOSS_SetERMFontType(d_FONT_DEFAULT_TTF, 0);
        CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);

        inSetLanguageDatabase(d_FONT_DEFAULT_TTF);
        inCTOSS_SetALLApFont(d_FONT_DEFAULT_TTF);
    }
    
    inCTOS_ConfigureCTLSReader();
    return SUCCESS;
}

#define DMENU_TOUCH_BAR

int inCTOS_IdleEventProcess(void) {
#define CTOS_ILDE_TIMER  60

    DWORD dwWait = 0, dwWakeup = 0;
    USHORT ret;
    BYTE key;
    BYTE bySC_status;
    int i = 0;
    char szErrorMessage[30];
	int inOrgMenuid;
    int inParkingFee=0;

    CTOS_RTC SetRTC;
	int inCheckDrainBatt = get_env_int("CHECKBATT");

    CTOS_RTCGet(&SetRTC); 		// for coin battery drain - : MasterCard DIMP Edit 14 - AC_INV_EMVTAGS_DE55 - raised #1
	
    CTOS_SCStatus(d_SC_USER, &bySC_status);
    if (bySC_status & d_MK_SC_PRESENT) {
        //inSetTextMode();
        vdRemoveCard();
    }

    CTOS_TimeOutSet(TIMER_ID_4, CTOS_ILDE_TIMER); //Only IDLE loop use TIMER_ID_4, please don't use this TIMER_ID_4 in other place in application
    vduiKeyboardBackLight(VS_FALSE);
    vdSetIdleEvent(1);

    vdDebug_LogPrintf("**inCTOS_IdleEventProcess strTCT.fECR[%d][%d]**", strTCT.fECR, SetRTC.bYear);
    if (strTCT.fECR) // tct
    {
        usCTOSS_ECRInitialize();
        put_env_int("ECRBUSY", FALSE);
    }

    inCTOSS_InitAP();

    memset(szErrorMessage, 0x00, sizeof (szErrorMessage));
    sprintf(szErrorMessage, "%d", strCPT.inCommunicationMode);
    inCTOSS_PutEnv("PRICOMMMODE", szErrorMessage);
    inCTOSS_PutEnv("COMMBACK", "0");
    memset(szErrorMessage, 0x00, sizeof (szErrorMessage));
    sprintf(szErrorMessage, "%d", strCPT.inSecCommunicationMode);
    inCTOSS_PutEnv("SECCOMMMODE", szErrorMessage);

    if (strTCT.byTerminalType == 4) {
        BYTE Tkey = 0;
        Tkey = (sizeof (stKeyPara) / sizeof (CTOS_VIRTUAL_FUNCTION_KEY_SET_PARA)) % 0x100;

        ret = CTOS_VirtualFunctionKeySet(stKeyPara, Tkey);
    }

    while (1) {
		vdSetIdleScreenShow(TRUE);

        #ifdef PARKING_FEE
        inParkingFee = get_env_int("PARKFEE");
        if(inParkingFee == TRUE)
        {
            inCTOS_MPU_SALE_PARKING();
            continue;
        }
        #endif
		
		#if 0
        if(fGetIdleScreenShow() == FALSE)
            displaybmpEx(0, 0, "Tlogo.bmp");
		#endif

		// for coin battery drain - : MasterCard DIMP Edit 14 - AC_INV_EMVTAGS_DE55 - raised #2
		#ifdef ENHANCEMENT_1861
		if (inCheckDrainBatt == 1)
		{
			if(SetRTC.bYear < 20)
			{
				CTOS_LCDTClearDisplay();		
				CTOS_Delay(1000);
				CTOS_LCDTPrintXY(1, 5, "PLEASE CALL HELPDESK");
				CTOS_LCDTPrintXY(1, 7, "TERMINAL CHECK FAILED");		
				CTOS_Beep();
				CTOS_Delay(1500);
				CTOS_LCDTClearDisplay();	
				continue; 	
			}
		}
		#endif
        
        //Open the Back Light in the LCD Display //
        //vduiLightOn();
        //return idle delay problem, move to head
        if (CTOS_TimeOutCheck(TIMER_ID_4) == d_YES) {
            i = 0;
            while (g_IdleRunningFun[i].inCTOS_IdleRunningFun != NULL)
                g_IdleRunningFun[i++].inCTOS_IdleRunningFun();
        }

        dwWait = d_EVENT_KBD | d_EVENT_MSR | d_EVENT_SC;
        //System waits the device the be set acts.   //System waits for target devices to be triggered and will not return until any target device is triggered //

        // patrick ECR clear
        //if (strTCT.fECR) // tct
        //    dwWait = d_EVENT_MSR | d_EVENT_SC;
        if (strTCT.byPinPadMode == 1) {
            USHORT usTk1Len, usTk2Len, usTk3Len;
            BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
            usTk1Len = TRACK_I_BYTES;
            usTk2Len = TRACK_II_BYTES;
            usTk3Len = TRACK_III_BYTES;

            dwWait = 0; // Disable the keybaord event. because of PinPad.
            CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

            CTOS_KBDBufFlush();
        }

        if (strTCT.byTerminalType == 4) {
            do {
                ret = CTOS_VirtualFunctionKeyHit(&key);
                if (key == d_KBD_INVALID) {
                    CTOS_KBDHit(&key);
                    break;
                }

                CTOS_KBDBufPut(key);
            } while (key != d_KBD_INVALID);

            if (key != d_KBD_INVALID) // patrick, please do not remove this part of code
            {
                CTOS_KBDBufPut(key);
            }
        }


        if (strCST.inCurrencyIndex == 3) {
            inCSTRead(stgCurrIdx);
        }

        ret = CTOS_SystemWait(20, dwWait, &dwWakeup);

        vdSetFirstIdleKey(0x00);
        CTOS_SCStatus(d_SC_USER, &bySC_status);

        if ((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD) {
            vduiKeyboardBackLight(VS_TRUE);
            //Detect if any key is pressed and get which key, but not get actual key.
            //If you get actual key, use to "CTOS_KBDGet" function//
            vdSetIdleScreenShow(FALSE);
            CTOS_KBDGet(&key);
            vdCTOS_uiIDLEWakeUpSleepMode();
            vduiLightOn();

			//put_env_int("ECRBUSY", TRUE);
			//vdDebug_LogPrintf("key[%d=%x]", key,key);//thandar
				
            switch (key) {
                case d_KBD_00:
                    vdCTOS_uiIDLEPowerOff();
                    break;

                case d_KBD_F1:
                    ret = inCTOS_PromptPassword();
                    if (d_OK != ret) {
                        CTOS_LCDTClearDisplay();
                        break;
                    }

                    inF1KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;
                case d_KBD_F2:
                    inF2KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;
                case d_KBD_F3:
                    inF3KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;

                case d_KBD_F4:
                    inF4KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;

                case d_KBD_ENTER:
                    inSetColorMenuMode();
//                    vdDebug_LogPrintf("ENV_APP_RIDDING =[%d]", g_inAppRidding);
//                    if (g_inAppRidding >= 1 && d_OK == inMAPMRead(g_inAppRidding)) {
//                        inTCTUpdateMenuid(1, strMAPM.inDefAppMenuID);
//                    }

                    ret = inEnterKeyEvent();
                    CTOS_LCDTClearDisplay();
                    vdDebug_LogPrintf("inEnterKeyEvent ret[%d]", ret);
                    break;

                case d_KBD_CANCEL:
                    break;

                case d_KBD_0:
                    //                    ret = in0KeyEvent();
                    ret = inCTOS_PromptPassword();
                    if (d_OK != ret) {
                        CTOS_LCDTClearDisplay();
                        break;
                    }

                    inF1KeyEvent();
                    CTOS_LCDTClearDisplay();
                    break;
                case d_KBD_1:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_1);
                    ret = in1KeyEvent();
                    break;
                case d_KBD_2:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_2);
                    ret = in2KeyEvent();
                    break;
                case d_KBD_3:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_3);
                    ret = in3KeyEvent();
                    break;
                case d_KBD_4:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_4);
                    ret = in4KeyEvent();
                    break;
                case d_KBD_5:
//                    CTOS_LCDTClearDisplay();
//                    vdCTOSS_DisplayQRCodeOneLine("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
//                            "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ"
//                            "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
//                            "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ"
//                            "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
//                            "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ"
//                            "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
//                            "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ");
////                    "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ"
//                    
//                    setLCDPrint(8, DISPLAY_POSITION_LEFT, "OK TO CONTINUE");
//                    CTOS_LCDTClearDisplay();
//                    setLCDPrint(2, DISPLAY_POSITION_LEFT, "GAMBAR");
//                    vdDisplayAnimateBmp(50, 0, "Tlogo.bmp", NULL, NULL, NULL, NULL);
//                    BYTE key1;
//                    
//                    key1 = 0;
//                    CTOS_KBDGet(&key1);
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_5);
                    ret = in5KeyEvent();
                    break;
                case d_KBD_6:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_6);
                    ret = in6KeyEvent();
                    break;
                case d_KBD_7:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_7);
                    ret = in7KeyEvent();
                    break;
                case d_KBD_8:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_8);
                    ret = in8KeyEvent();
                    break;
                case d_KBD_9:
                    inSetColorMenuMode();
                    CTOS_KBDBufPut(d_KBD_9);
                    ret = in9KeyEvent();
                    break;

				case d_KBD_DOWN:
					if (isCheckTerminalMP200() == d_NO) 
						break;
					
					else						
					{
                        vdCTOS_SwitchLanguage();
                    	vduiClearBelow(2);
                   		break;
                    }
					
                case d_KBD_DOT:
                    vdCTOS_SwitchLanguage();
                    vduiClearBelow(2);
                    break;
                    //                case d_KBD_DOT:
                    //                    inF1KeyEvent();
                    //                    break;
                    //                case d_KBD_0:
                    //                    strTCT.byRS232DebugPort = 2;
                    //                    memset(strCPT.szPriTxnHostIP, 0x00, sizeof(strCPT.szPriTxnHostIP));
                    //                    memcpy(strCPT.szPriTxnHostIP, "192.168.0.27",12);
                    //                    strCPT.inPriTxnHostPortNum = 2010;
                    //                    
                    //                    memset(strCPT.szSecTxnHostIP, 0x00, sizeof(strCPT.szSecTxnHostIP));
                    //                    memcpy(strCPT.szSecTxnHostIP, "192.168.0.27",12);
                    //                    strCPT.inSecTxnHostPortNum = 2010;
                    //                    break;


                default:
                    break;
            }

            //put_env_int("ECRBUSY", FALSE);
			
            clearLine(1);
            clearLine(2);
            vduiKeyboardBackLight(VS_FALSE);
            vdSetIdleEvent(1);
            vdCTOSS_ClearTouchPanelTest();
			inIdleTimeoutCtr=0;

//            vduiClearBelow(2);
        } else if ((dwWakeup & d_EVENT_MSR) == d_EVENT_MSR) {
            if (strTCT.byPinPadMode == 1) {
                continue;
            }

            //put_env_int("ECRBUSY", TRUE);
						
            //Open the Back Light in the LCD Display //
            vdCTOS_uiIDLEWakeUpSleepMode();
            vduiKeyboardBackLight(VS_TRUE);
            vduiLightOn();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
            vdSetIdleEventSC_MSR(1);
            vdSetCardFromIdle(TRUE);

			inTCTGetCurrMenuid(1, &inOrgMenuid);
			if(inOrgMenuid==3)
   				inCTOS_IPP_INSTALLMENT();

            else if (TRUE == fGetCashAdvAppFlag())
                inCTOS_CashAdvance();
            else
                inCTOS_MPU_SALE();
            //CTOS_LCDTClearDisplay();

			//put_env_int("ECRBUSY", FALSE);
			
            vdSetCardFromIdle(FALSE);
            clearLine(1);
            clearLine(2);
            vduiKeyboardBackLight(VS_FALSE);
            vdSetIdleEventSC_MSR(0);
            vdSetIdleEvent(1);
//            vduiClearBelow(2);
            inIdleTimeoutCtr=0;
            continue;
        } else if (((dwWakeup & d_EVENT_SC) == d_EVENT_SC) || (bySC_status & d_MK_SC_PRESENT)) {
            if (strTCT.byPinPadMode == 1)
                continue;

#ifdef APP_AUTO_TEST
            if (inCTOS_GetAutoTestCnt() > 1) {
                vdDebug_LogPrintf("Auto Test ---> Start Trans");
                inCTOS_AutoTestFunc();
                continue;
            }
#endif

            //put_env_int("ECRBUSY", TRUE);

            //Open the Back Light in the LCD Display //
            vdCTOS_uiIDLEWakeUpSleepMode();
            vduiKeyboardBackLight(VS_TRUE);
            vduiLightOn();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            vdSetIdleEventSC_MSR(1);
            vdSetCardFromIdle(TRUE);

			inTCTGetCurrMenuid(1, &inOrgMenuid);
			if(inOrgMenuid==3)
   				inCTOS_IPP_INSTALLMENT();
			
            else if (TRUE == fGetCashAdvAppFlag())
                inCTOS_CashAdvance();
            else
                inCTOS_MPU_SALE();
            //            	inCTOS_WAVE_SALE();
            //CTOS_LCDTClearDisplay();

			//put_env_int("ECRBUSY", FALSE);
			
            vdSetCardFromIdle(FALSE);
            clearLine(1);
            clearLine(2);
            vduiKeyboardBackLight(VS_FALSE);
            vdSetIdleEventSC_MSR(0);
            vdSetIdleEvent(1);
//            vduiClearBelow(2);
            inIdleTimeoutCtr=0;            
            continue;
        }

#if 0
        else if (CTOS_TimeOutCheck(TIMER_ID_4) == d_YES) {
            i = 0;
            while (g_IdleRunningFun[i].inCTOS_IdleRunningFun != NULL)
                g_IdleRunningFun[i++].inCTOS_IdleRunningFun();

            continue;
        }
#endif   
    }
}

int inAutoSwitchSIMSlot(void)
{
	USHORT  usRtn;
	BOOL fPortable;
	BOOL fPCI;
	USHORT mkHWSupport;
	BYTE szDisp[64];

	BOOL fSIMCard = 0;		
	

//	if (strTCTEX.fHawkerMode != 1)
//		return SUCCESS;

    inCPTRead(21);//just load CB credit host instead of using host index 1
    
	vdDebug_LogPrintf("=====inAutoSwitchSIMSlot=====[%d]", strCPT.inCommunicationMode);

	if (strCPT.inCommunicationMode != GPRS_MODE)
		return d_OK;

	
	//if ((strcmp(srTransRec.szTID, "XXXXXXXX") == 0) || (strcmp(srTransRec.szMID, "XXXXXXXXXXXX") == 0))
	//{
	//vdDisplayAt(1, 3, (char *)"Check SIM Card...");

	setLCDPrint(8, DISPLAY_POSITION_LEFT,"Check SIM Card...");
	inTCPRead(1);				   

	usRtn = CTOS_HWSettingGet(&fPortable,&fPCI,&mkHWSupport);
	
	//vdDebug_LogPrintf("CTOS_HWSettingGet,usRtn=[%x],fPortable[%d],fPCI=[%d],mkHWSupport=[%04x]", usRtn,fPortable,fPCI,mkHWSupport);
	//vdDebug_LogPrintf("mkHWSupport & d_MK_HW_GPRS=[%04x]", mkHWSupport & d_MK_HW_GPRS);

	if ((mkHWSupport & d_MK_HW_GPRS) == d_MK_HW_GPRS)
	{
		usRtn = CTOS_GSMOpen(9600, 1);		
		//vdDebug_LogPrintf("CTOS_GSMOpen[%X].....", usRtn);		

		//memset(szDisp, 0x00, sizeof(szDisp));
		//sprintf(szDisp, "Check SIM Slot [%d]", strTCP.inSIMSlot);
		//CTOS_LCDTPrintXY(1, 4, "                             ");
		//CTOS_LCDTPrintXY(1, 4, szDisp);
		
		if (strTCP.inSIMSlot == 1)
			usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM1);
		else
			usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM2);//v5s rootfs=R0041 need wait 10 secs

			CTOS_Delay(30);
			
			vdDebug_LogPrintf("CTOS_GSMSelectSIM[%x],inSIMSlot=[%d]", usRtn, strTCP.inSIMSlot);
      	
		
		usRtn = CTOS_SIMCheckReady();
		vdDebug_LogPrintf("CTOS_SIMCheckReady[%X].....", usRtn);

        //if inSIMSlot=[1] is ready, then
		if(d_GSM_SIM_READY==usRtn)
			return d_OK;

		if (d_GSM_SIM_NOT_READY == usRtn)
		{
			fSIMCard = 0;
			
			if (strTCP.inSIMSlot == 1)
			{
				memset(szDisp, 0x00, sizeof(szDisp));
				sprintf(szDisp, "SIM Slot 1 Not Ready");
			    CTOS_LCDTPrintXY(1, 5, "                             ");
				CTOS_LCDTPrintXY(1, 5, szDisp);
				
				usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM2);

				CTOS_Delay(30);
			
			    //vdDebug_LogPrintf("2_CTOS_GSMSelectSIM[%x],inSIMSlot2=[%d]", usRtn, strTCP.inSIMSlot);

			
				usRtn = CTOS_SIMCheckReady();

				CTOS_Delay(45);
				
				vdDebug_LogPrintf(" dCTOS_SIMCheckReady_GPRS_SIM2[%X].....", usRtn);

				if (d_GSM_SIM_NOT_READY == usRtn)
				{
					fSIMCardSlot = 0;
				}
				else
				{
					fSIMCardSlot = 2;
				}
				
			}
			
			if (strTCP.inSIMSlot == 2)
			{
				memset(szDisp, 0x00, sizeof(szDisp));
				sprintf(szDisp, "SIM Slot 2 Not Ready");
				CTOS_LCDTPrintXY(1, 5, "                             ");
				CTOS_LCDTPrintXY(1, 5, szDisp);

				usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM1);
				usRtn = CTOS_SIMCheckReady();

				vdDebug_LogPrintf("CTOS_SIMCheckReady d_GPRS_SIM1[%X].....", usRtn);

				if (d_GSM_SIM_NOT_READY == usRtn)
				{
					fSIMCardSlot = 0;
					
				}
				else
				{
					fSIMCardSlot = 1;
				}
			}

			fSIMCard = strTCP.inSIMSlot;
			
			if (fSIMCard == 1)
			{			
					
				//strTCP.inSIMSlot = 2;					
				//fSIMCard = strTCP.inSIMSlot;
				
				fSIMCard=2;
				vdCTOSS_SavingInfo_SIM1SIM2(2);
				
				vdDebug_LogPrintf("Change SIM slot 1 to 2");
				
				inCTOSS_SwitchSIM2();
				
			}
			else if (fSIMCard == 2)
			{
				//strTCP.inSIMSlot = 1;				
				//fSIMCard = strTCP.inSIMSlot;

				fSIMCard=1;
				vdCTOSS_SavingInfo_SIM1SIM2(1);		
				
				vdDebug_LogPrintf("Change SIM slot 2 to 1");

				inCTOSS_SwitchSIM1();
				
			}
			
		}
		else
		{
			fSIMCard = strTCP.inSIMSlot;
			fSIMCardSlot = strTCP.inSIMSlot;			
		}
	}

	//fSIMCardSlot = fSIMCard;
	#if 0
	if (0 == fSIMCardSlot)
	{
		vdDispErrMsg("SIM Card Not Ready");
		
	}
	//else
    {
		
	}
	#endif
	//}

	return d_OK;
}


int main(int argc, char *argv[]) {
    inSetTextMode();

    inCTOSS_ProcessCfgExpress();

    inTCTRead(1);
    inCPTRead(1);
    inCSTRead(1);
    inTCPRead(1);

	inHDTRead(6);//thandar_add to fix IPP Load CST Err
	
    //vdDebug_LogPrintf("HDT6=>IPP.inHostIndex[%d]",strHDT.inHostIndex);
	//vdDebug_LogPrintf("HDT6=>IPP.inCurrencyIdx[%d]",strHDT.inCurrencyIdx);
	//vdDebug_LogPrintf("HDT6=>IPP.szTPDU[%02X%02X]",strHDT.szTPDU[0],strHDT.szTPDU[1]);
	//vdDebug_LogPrintf("HDT6=>IPP.szNII[%02X%02X]",strHDT.szNII[0],strHDT.szNII[1]);
	//vdDebug_LogPrintf("HDT6=>IPP.szHostLabel[%s]",strHDT.szHostLabel);
	//vdDebug_LogPrintf("HDT6=>IPP.fHostEnable[%d]",strHDT.fHostEnable);
 
	    
    if(strTCT.byPinPadType == 3 && strTCT.byPinPadPort == 9)
        CTOS_USBSelectMode(d_USB_HOST_MODE);

	//CHECK Default Menu ID
    vdChkCashAdvAppMenu();

    /*check if need TMS update*/
    inCTOSS_TMSRestoreTxnData();

    if(strTCT.byPinPadPort != 9){
		
        vdDebug_LogPrintf("MAIN vdCTOS_TermSelectCurrencyMenu!!!");
		
		vdDebug_LogPrintf("vdCTOS_TermSelectCurrencyMenu strHDT.inHostIndex[%d] strHDT.inCurrencyIdx[%d]",strHDT.inHostIndex, strHDT.inCurrencyIdx);
		
        vdCTOS_TermSelectCurrencyMenu(); //@@IBR ADD 20170116

		
        vdDebug_LogPrintf("MAIN vdCTOS_TermSelectCurrencyMenu AFTER!!!");
    }
    
    vdDefaultLanguage();

    stgCurrIdx = strCST.inCurrencyIndex;
    vdSetECRTransactionFlg(0);

    vdGetMultipleAppRidding();
    
    if (g_inAppRidding <= 0)
        g_inAppRidding = 1;

	vdDebug_LogPrintf("g_inAppRidding[%d][%d]", g_inAppRidding, strTCT.byPinPadPort);

    if(g_inAppRidding > 0)
        inMAPMRead(g_inAppRidding);

	
    #if 0
    /*update the menu id*/
	if (g_inAppRidding>=1 && d_OK == inMAPMRead(g_inAppRidding))
	{
		vdDebug_LogPrintf("g_inAppRidding[%d]", g_inAppRidding);
		vdDebug_LogPrintf("strMAPM.inDefAppMenuID[%d]", strMAPM.inDefAppMenuID);

		inTCTUpdateMenuid(1, strMAPM.inDefAppMenuID);
		CTOS_Delay(50);
		inTCTRead(1);
	}
	#endif

    vdThreadRunAppColdInit();
    if (get_env_int("AUTORECOVER") != 8) {
        inMultiAP_ForkTask();
        if (d_OK == inMultiAP_CheckSubAPStatus()) {
            vdDebug_LogPrintf("inWaitTime =[%d]", strTCT.inWaitTime);
            if (strTCT.inWaitTime <= 0)
                strTCT.inWaitTime = 100;

            while (1) {
                inMultiAP_GetMainroutine();
                //CTOS_Delay(strTCT.inWaitTime);
            }
        } else {
            inMultiAP_ForkSharlsAp();
        }
    } else {
        inMultiAP_ForkTaskEx();
    }

    /*check if it is external pinpad V3P*/
    if ((strTCT.byPinPadType == 3 && strTCT.byPinPadPort == 9)
            || (strTCT.byPinPadType == 1 && strTCT.byPinPadPort == 9)) {
        if (strTCT.byRS232DebugPort == 8) {
            strTCT.byRS232DebugPort = 0;
            inTCTSave(1);

            CTOS_LCDTClearDisplay();
            vduiDisplayStringCenter(3, "USB HOST MODE");
            vduiDisplayStringCenter(4, "USB DEBUG ENABLE");
            vduiDisplayStringCenter(5, "TERMINAL REBOOT");

            CTOS_Delay(2000);
            CTOS_SystemReset();
        }
        CTOS_USBSelectMode(d_USB_HOST_MODE);
    }

	//vdHardCodeMPU_UATKey();//thandar-test MPU UAT key
    //CTOS_LCDTClearDisplay();
    if (get_env_int("AUTORECOVER") != 8) 
	{
        
        if(strTCT.fEnableAmountIdle != TRUE)
            inCTOS_DisplayIdleBMP();
		
        CTOS_Delay(1000);

        inCTLOS_Getpowrfail();
    }

    put_env_int("AUTORECOVER", 0);
    //inCTOSS_CheckIfPendingTMSDownload();


	
	vdDebug_LogPrintf("MAIN here!!!");
	
/*
	#ifdef PIN_CHANGE_ENABLE
	if (fGetCashAdvAppFlag() == TRUE)
		vdCheckMenuID4();
	else	
		vdCheckMenu();
	
	#else
	vdCheckMenu();
	#endif
*/	


	#ifdef SET_IPP_MENU
	
		if (fGetCashAdvAppFlag() == TRUE) //menuID 4
			vdCheckMenuID4();
		else if(fGetIPPAppFlag() == TRUE) //menuID 3	
			vdCheckMenuID3();
		else	
			vdCheckMenu();			//MenuID 5
	
	#else

	
		if (fGetCashAdvAppFlag() == TRUE)
			vdCheckMenuID4();
		else	
			vdCheckMenu();
	
	#endif

	
	vdDebug_LogPrintf("MAIN end !!!");
	//vdDebug_LogPrintf("inCTOSS_AutoSwitchSIM in MAIN..inSIMSlot=%d..",strTCP.inSIMSlot);//EMMY debug
	//inCTOSS_AutoSwitchSIM(2);//thandar
	//inAutoSwitchSIMSlot();
	
	inDatabase_PreauthBatchDeleteExpired();
	
    inCTOS_IdleEventProcess();
}

#if 1
int inCTOS_AUTOSETTLE(void)
{    
    long inCurrTime=0L, inCurrDate=0L;
    CTOS_RTC SetRTC;
    BYTE szCurrTime[7] = {0};
    BYTE szCurrDate[8] = {0};
    int inResult = 0;

    memset(szCurrTime, 0x00, sizeof(szCurrTime));
	memset(szCurrDate, 0x00, sizeof(szCurrDate));
    CTOS_RTCGet(&SetRTC);
    sprintf(szCurrTime,"%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
    inCurrTime=wub_str_2_long(szCurrTime);
    sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
	inCurrDate=wub_str_2_long(szCurrDate);

    vdDebug_LogPrintf("Current Time: %ld nCurrent Date: %ld", inCurrTime, inCurrDate);
	
    inPASRead(1);
		
	vdDebug_LogPrintf("strPAS.fEnable: %d", strPAS.fEnable);

    if(strPAS.fEnable)
    {
		vdDebug_LogPrintf("Current Date: %ld strPAS.szlastSettleDate: %ld", wub_str_2_long(szCurrDate), wub_str_2_long(strPAS.szlastSettleDate));
		if (wub_str_2_long(szCurrDate) > wub_str_2_long(strPAS.szlastSettleDate))
		{
			vdDebug_LogPrintf("inCurrTime: %ld strPAS.szSTLTime1: %ld", inCurrTime, wub_str_2_long(strPAS.szSTLTime1));

			if(inCurrTime >= wub_str_2_long(strPAS.szSTLTime1))
			{
				vdDebug_LogPrintf("Auto Settle ON");
				inCTOS_AUTO_SETTLE();
				memcpy(strPAS.szlastSettleDate, szCurrDate, sizeof(szCurrDate));
				inPASSave(1);
			}	
    	}
    }

	return SUCCESS;
}
#endif

int inCTOS_CHECKMUSTSETTLE(void)
	{	 
		USHORT i;
		CTOS_RTC LAST_DATE;
		CTOS_RTC TIME_SET;
		CTOS_RTC Update_Date;
		char u1t_Date[10];
		int result = d_NO;
		int temp = 0;
		int inRet = 0;	
	
		vdDebug_LogPrintf("inCTOS_CHECKMUSTSETTLE.....");
	
		vdGetDateFromInt(get_env_int("LAST_DATE"),&LAST_DATE);
		vdGetTimeFromInt(get_env_int("TIMESET"),&TIME_SET);
		
		//vdDebug_LogPrintf("LAST_DATE[1] = [ %02d%02d%02d ]",LAST_DATE.bYear,LAST_DATE.bMonth,LAST_DATE.bDay);
		//vdDebug_LogPrintf("SETTLE_FLAG = [ %d ]",get_env_int("SETTLE_FLAG"));
		//vdDebug_LogPrintf("TIMESET = [ %02d%02d%02d ]",TIME_SET.bHour,TIME_SET.bMinute,TIME_SET.bSecond);
	
		if(get_env_int("SETTLE_FLAG") != 1)
		{
			temp = Check_OverDateTime(LAST_DATE.bYear,LAST_DATE.bMonth,LAST_DATE.bDay,TIME_SET.bHour,TIME_SET.bMinute,TIME_SET.bSecond);
			vdDebug_LogPrintf("Check_OverDateTime %d",temp);

			
			if( temp == 1 )
			{
			
				//result = inCTOS_SETTLE_ALL();
				result = inCTOS_CHECKSETTLE_ALL();
				
				if( result == d_OK )
				{
					put_env_int("SETTLE_FLAG",1);
				}
				
			}
		}
	
		//vdDebug_LogPrintf("SETTLE_FLAG = [ %d ]",inGetParamsInt("SETTLE_FLAG")); 

		
		CTOS_RTCGet(&Update_Date);
		if(( Update_Date.bYear!= LAST_DATE.bYear)||( Update_Date.bMonth!= LAST_DATE.bMonth )||( Update_Date.bDay!= LAST_DATE.bDay ))
		{
			 if(get_env_int("SETTLE_FLAG") == 1)
			 {			   
				 put_env_int("SETTLE_FLAG",0);
			 }
			 sprintf(u1t_Date,"%02d%02d%02d",Update_Date.bYear,Update_Date.bMonth,Update_Date.bDay); 
			 vdDebug_LogPrintf("u1t_Date = [ %s ]",u1t_Date);
			 //inSetParam("LAST_DATE",u1t_Date);
			 inCTOSS_PutEnvDB ("LAST_DATE",u1t_Date);
			 vdDebug_LogPrintf("LAST_DATE[2] = [ %d ]",get_env_int("LAST_DATE"));
		}
	   // vdDebug_LogPrintf("SETTLE_FLAG2 = [ %d ]",inGetParamsInt("SETTLE_FLAG")); 
		return d_OK;
	}


	int inCTOSS_AutoSwitchSIM(int inMode)
	{

		  vdDebug_LogPrintf("inCTOSS_AutoSwitchSIM..inSIMSlot=%d",strTCP.inSIMSlot);
		  vdDebug_LogPrintf("inMode=%d",inMode);
		  
	      if (get_env_int("AUTOSWITCHSIM") == 1)
	      	{
				vdDebug_LogPrintf("AUTOSWITCHSIM is 1");
	      	    if(inMode==1)
		  	       inAutoSwitchSIMSlot();

				if(inMode==2)
				{
                    if(strTCP.inSIMSlot==1)
                    {
                        vdCTOSS_SavingInfo_SIM1SIM2(2);
                        //inCTOSS_SwitchSIM2();
                        //inAutoSwitchSIMSlot(); let sharls com do the switching...
                        setLCDPrint(8, DISPLAY_POSITION_LEFT,"Switching to SIM2...   ");
                        WaitKey(15);
                        //CTOS_SystemReset();
                        return d_OK;
                    }
                    
                    
                    if(strTCP.inSIMSlot==2)
                    {
                        vdCTOSS_SavingInfo_SIM1SIM2(1);
                        //inCTOSS_SwitchSIM1();
                        //inAutoSwitchSIMSlot(); let sharls com do the switching...
                        setLCDPrint(8, DISPLAY_POSITION_LEFT,"Switching to SIM1...   ");
                        WaitKey(15);
                        //CTOS_SystemReset();
                        return d_OK;
                    }
							
				}
	      	}
	
		  return d_OK;
	}

	int inCTOSS_SwitchSIM2(void)
		{
		        USHORT  usRtn;
				BYTE szDisp[64];

				BYTE bInBuf[40];
    			BYTE bOutBuf[40];
				USHORT usInLen = 0;
   				USHORT usOutLen = 0;
    			USHORT usResult;

				int result = ST_ERROR;

				
				inCTOS_inDisconnect();
	
	
			  	CTOS_LCDTClearDisplay();
				//clearLine(5);
				memset(szDisp, 0x00, sizeof(szDisp));	

				sprintf(szDisp, "Switching to SIM 2");
				
			    CTOS_LCDTPrintXY(1, 5, "                             ");
				CTOS_LCDTPrintXY(1, 5, szDisp);
				
				usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM2);

				CTOS_Delay(30);
			
			    vdDebug_LogPrintf("1_inCTOSS_SwitchSIM2[%x],inSIMSlot2=[%d]", usRtn, strTCP.inSIMSlot);

			
				usRtn = CTOS_SIMCheckReady();

				CTOS_Delay(45);
				
				vdDebug_LogPrintf("2_inCTOSS_SwitchSIM2 CTOS_SIMCheckReady[%X].....", usRtn);


				//if (d_GSM_SIM_READY == usRtn)
					//{
							//inCTOSS_COMMSetSIMSlot(2);							

							//inCTOS_ReForkSubAP("SHARLS_COM");
 							//CTOS_Delay(1000);

							  //vduiWarningSound();
						     // CTOS_LCDTPrintXY(1, 7, "Restart Device");
						     // CTOS_Delay(2000);
						      //CTOS_SystemReset();				  
										      
					
					//}     
					
						
				if (d_GSM_SIM_NOT_READY == usRtn)
				{
					fSIMCardSlot = 0;   						
					
						
				}
				else
				{
					fSIMCardSlot = 2;
				}			

			  
			  return d_OK;
		}

	
	int inCTOSS_SwitchSIM1(void)
			{

				USHORT  usRtn;
				BYTE szDisp[64];				
				BYTE bInBuf[40];
    			BYTE bOutBuf[40];
				USHORT usInLen = 0;
   				USHORT usOutLen = 0;
    			USHORT usResult;

				int result = ST_ERROR;

				inCTOS_inDisconnect();
				
	
			  	CTOS_LCDTClearDisplay();
				//clearLine(5);
				memset(szDisp, 0x00, sizeof(szDisp));			
				

				sprintf(szDisp, "Switching to SIM 1");
			    CTOS_LCDTPrintXY(1, 5, "                             ");
				CTOS_LCDTPrintXY(1, 5, szDisp);
				
				usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM1);

				CTOS_Delay(30);
			
			   vdDebug_LogPrintf("inCTOSS_SwitchSIM1[%x],inSIMSlot1=[%d]", usRtn, strTCP.inSIMSlot);

			
				usRtn = CTOS_SIMCheckReady();

				CTOS_Delay(45);
				
				vdDebug_LogPrintf("inCTOSS_SwitchSIM1 [%X].....", usRtn);

               //if (d_GSM_SIM_READY == usRtn)
			  // {     inCTOSS_COMMSetSIMSlot(1);

			         //vduiWarningSound();
					 //CTOS_LCDTPrintXY(1, 7, "Restart Device");
					 //CTOS_Delay(2000);
					 //CTOS_SystemReset();	

			   		// inCTOS_ReForkSubAP("SHARLS_COM");
 					// CTOS_Delay(1000);
 					
			     
               	//}

			  
				if (d_GSM_SIM_NOT_READY == usRtn)
				{
					fSIMCardSlot = 0;										
				}
				else
				{
					fSIMCardSlot = 2;
				}			

			  
			  return d_OK;
		}


	void vdCTOSS_SavingInfo_SIM1SIM2(int inSwitchNum)
{			
	BYTE	szTCPRec1APN[30] ;
    BYTE	szTCPRec1UserName[30] ;
    BYTE	szTCPRec1Password[30] ;

	BYTE	szTCPRec2APN[30] ;
    BYTE	szTCPRec2UserName[30] ;
    BYTE	szTCPRec2Password[30] ;

	BYTE	szTCPRec3APN[30] ;
    BYTE	szTCPRec3UserName[30] ;
    BYTE	szTCPRec3Password[30] ;

    USHORT usRes=0;

	vdDebug_LogPrintf("vdCTOSS_SavingInfo_SIM1SIM2. start"); 

	memset(szTCPRec1APN, 0x00, sizeof(szTCPRec1APN));
	memset(szTCPRec1UserName, 0x00, sizeof(szTCPRec1UserName));
	memset(szTCPRec1Password, 0x00, sizeof(szTCPRec1Password));

	memset(szTCPRec2APN, 0x00, sizeof(szTCPRec2APN));
	memset(szTCPRec2UserName, 0x00, sizeof(szTCPRec2UserName));
	memset(szTCPRec2Password, 0x00, sizeof(szTCPRec2Password));

	memset(szTCPRec3APN, 0x00, sizeof(szTCPRec3APN));
	memset(szTCPRec3UserName, 0x00, sizeof(szTCPRec3UserName));
	memset(szTCPRec3Password, 0x00, sizeof(szTCPRec3Password));
	
	
	//assuming TCP Rec 3 always have the same info as TCP Rec 1
	inTCPRead(3);
							
	memcpy(szTCPRec3APN, strTCP.szAPN, sizeof (strTCP.szAPN));
	memcpy(szTCPRec3UserName, strTCP.szUserName, sizeof (strTCP.szUserName));
	memcpy(szTCPRec3Password, strTCP.szPassword, sizeof (strTCP.szPassword));

	vdDebug_LogPrintf("TCP_REC(3)_APN[%s]", szTCPRec3APN); 
	vdDebug_LogPrintf("TCP_REC(3)_User[%s]", szTCPRec3UserName);
	vdDebug_LogPrintf("TCP_REC(3)_PWD[%s]", szTCPRec3Password);	

	inTCPRead(2);
	
	memcpy(szTCPRec2APN, strTCP.szAPN, sizeof (strTCP.szAPN));
	memcpy(szTCPRec2UserName, strTCP.szUserName, sizeof (strTCP.szUserName));
	memcpy(szTCPRec2Password, strTCP.szPassword, sizeof (strTCP.szPassword));	

	vdDebug_LogPrintf("TCP_REC(2)_APN[%s]", szTCPRec2APN); 
	vdDebug_LogPrintf("TCP_REC(2)_User[%s]", szTCPRec2UserName);
	vdDebug_LogPrintf("TCP_REC(2)_PWD[%s]", szTCPRec2Password);	
	

	inTCPRead(1);		
	
    if(inSwitchNum==1)
    {
        strTCP.inSIMSlot = 1;
        
        memcpy(strTCP.szAPN, szTCPRec3APN, sizeof (szTCPRec3APN));
        memcpy(strTCP.szUserName,szTCPRec3UserName,  sizeof (szTCPRec3UserName));
        memcpy(strTCP.szPassword,szTCPRec3Password,  sizeof (szTCPRec3Password));
        
        vdDebug_LogPrintf("inSwitchNum(1)_APN[%s]", strTCP.szAPN); 
        vdDebug_LogPrintf("inSwitchNum(1)_User[%s]", strTCP.szUserName);
        vdDebug_LogPrintf("inSwitchNum(1)_PWD[%s]", strTCP.szPassword);			
        
        inTCPSave(1);		
        
        /*add for CTMS*/
        #ifdef DUAL_SIM_SETTINGS
        vdCTMS_DualSIMSetting(1);
        #endif
    }

    if(inSwitchNum==2)
    {
        strTCP.inSIMSlot = 2;				
        
        memcpy(strTCP.szAPN, szTCPRec2APN, sizeof (szTCPRec2APN));
        memcpy(strTCP.szUserName,szTCPRec2UserName,  sizeof (szTCPRec2UserName));
        memcpy(strTCP.szPassword,szTCPRec2Password,  sizeof (szTCPRec2Password));
        
        vdDebug_LogPrintf("inSwitchNum(2)_APN[%s]", strTCP.szAPN); 
        vdDebug_LogPrintf("inSwitchNum(2)_User[%s]", strTCP.szUserName);
        vdDebug_LogPrintf("inSwitchNum(2)_PWD[%s]", strTCP.szPassword);	
        
        inTCPSave(1);
        
        /*add for CTMS*/
        #ifdef DUAL_SIM_SETTINGS
        vdCTMS_DualSIMSetting(2);
        #endif
    }	

	vdDebug_LogPrintf("vdCTOSS_SavingInfo_SIM1SIM2. end"); 

    return;
}

void vdCTMS_DualSIMSetting(int inSwitchNum) 	
{
	CTMS_GPRSInfo stgprs;
	USHORT usRes=0;
	
    memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
    CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    
    vdDebug_LogPrintf("stgprs.bSIMSlot=%02x", stgprs.bSIMSlot);
    if(inSwitchNum == 1)
    {
		stgprs.bSIMSlot=0x00;
    }
	else if(inSwitchNum == 2)
	{
        stgprs.bSIMSlot=0x01;
	}
    usRes=CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);	
}

int inCTOSS_ManualSwitchSIMProcess(int inMode)
{
  vdDebug_LogPrintf("inCTOSS_ManualSwitchSIMProcess..inSIMSlot=%d",strTCP.inSIMSlot);
  vdDebug_LogPrintf("inMode=%d",inMode);
  
  if (get_env_int("MANUALSWITCHSIM") == 1)
	{
	
		CTOS_LCDTClearDisplay();

		vdDebug_LogPrintf("MANUALSWITCHSIM is 1");
		
		if(inMode==1)
		   inAutoSwitchSIMSlot();

		if(inMode==2)
		{
			if(strTCP.inSIMSlot==1)
			{
				vdDebug_LogPrintf("inCTOSS_ManualSwitchSIMProcess..strTCP.inSIMSlot==1");
			
				//fix for SQA issue 10034 - Terminal restarts after selecting SWITCH SIM function - 07052021			
				vdCTOS_DispStatusMessage("Switching to SIM2...");

				vdCTOSS_SavingInfo_SIM1SIM2(2);

				vdDebug_LogPrintf("inCTOSS_ManualSwitchSIMProcess..AFTER vdCTOSS_SavingInfo_SIM1SIM2");
				
				//inCTOSS_SwitchSIM2();
				//CTOS_SystemReset();
				vdDebug_LogPrintf("inCTOSS_ManualSwitchSIMProcess..END");
				
				return d_OK;
			}
			
			
			if(strTCP.inSIMSlot==2)
			{
				vdDebug_LogPrintf("inCTOSS_ManualSwitchSIMProcess..strTCP.inSIMSlot==2");
			
				//fix for SQA issue 10034 - Terminal restarts after selecting SWITCH SIM function - 07052021			
				vdCTOS_DispStatusMessage("Switching to SIM1...");	
				

				vdCTOSS_SavingInfo_SIM1SIM2(1);

				vdDebug_LogPrintf("inCTOSS_ManualSwitchSIMProcess..AFTER vdCTOSS_SavingInfo_SIM1SIM2");
				
				//inCTOSS_SwitchSIM1();
				//inAutoSwitchSIMSlot(); let sharls com do the switching...
				//CTOS_SystemReset();
				vdDebug_LogPrintf("inCTOSS_ManualSwitchSIMProcess..END");
				
				return d_OK;
			}
					
		}
	}

  return d_OK;
}


