/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <ctoskia.h>

#include "../Includes/DMenu.h"

#include "../Includes/myEZLib.h"
#include "../Includes/msg.h"
#include "../Includes/wub_lib.h"


#include "..\Includes\CTOSInput.h"
#include "../Includes/POSTypedef.h"
#include "../FileModule/myFileFunc.h"

#include "../print/Print.h"
#include "../FileModule/myFileFunc.h"
#include "../Comm/V5Comm.h"


#include "..\Includes\CTOSInput.h"
#include "..\Includes\CfgExpress.h"
#include "..\Includes\epad.h"
#include "..\Includes\EFTSec.h"


#include "../Accum/accum.h"
#include "../DataBase/DataBaseFunc.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSHost.h"
#include "..\ui\Display.h"
#include "..\Database\DatabaseFunc.h"
#include "..\debug\debug.h"
#include "..\Includes\POSTrans.h"
#include "..\Accum\Accum.h"
#include "..\filemodule\myFileFunc.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"
#include "..\Aptrans\MultiSharecom.h"
#include "..\Aptrans\MultiShareECR.h"
#include "..\PCI100\PCI100.h"
#include "..\tms\tms.h"
#include "..\PinPad\pinpad.h"
#include "..\Ctls\POSWave.h"


#define DISPLAY_POSITION_LEFT 0
#define DISPLAY_POSITION_CENTER 1
#define DISPLAY_POSITION_RIGHT 2
#define DISPLAY_LINE_SIZE 16

static int inSleepflag = 0;
BOOL BolDetachGPRSChangeSetting=FALSE, BolDetachDIALChangeSetting=FALSE;

extern int inEthernet_GetTerminalIP(char *szTerminalIP);

void vdCTOS_uiIDLEPowerOff(void)
{
    USHORT usRet1;
    BYTE  bPercentage ;

    usRet1 = CTOS_BatteryGetCapacity(&bPercentage);
    if(d_BATTERY_NOT_EXIST == usRet1 || d_BATTERY_NOT_SUPPORT == usRet1)
    {
		return;
    }

	vdCTOS_uiPowerOff();
}

void vdCTOS_uiIDLESleepMode(void)
{
    USHORT usRet1;
    BYTE  bPercentage ;
	UCHAR usRet;
    DWORD pdwStatus ;

	usRet1 = CTOS_PowerSource(&usRet);
	vdDebug_LogPrintf("CTOS_PowerSource=[%x]..usRet=[%d]..inSleepflag=[%d]....",usRet1,usRet,inSleepflag);
	if ((d_OK == usRet1) && (usRet == d_PWRSRC_BATTERY))
	{
		if (inSleepflag == 0)
		{
			vdDebug_LogPrintf("vdSetsysPowerLowSpeed......");
			//CTOS_BackLightSet (d_BKLIT_LCD, d_OFF);
			//CTOS_BackLightSet (d_BKLIT_KBD, d_OFF);
			///vdSetsysPowerLowSpeed();
			CTOS_PowerMode(d_PWR_SLEEP_MODE);
			inSleepflag = 1;
		}
	}
	else
		vdCTOS_uiIDLEWakeUpSleepMode();
	return;
	
}

void vdCTOS_uiIDLEWakeUpSleepMode(void)
{
	//vdDebug_LogPrintf("vdCTOS_uiIDLEWakeUpSleepMode,inSleepflag=[%d]......",inSleepflag);
	if (inSleepflag == 1)
	{
		vdDebug_LogPrintf("vdSetsysPowerHighSpeed......");
		//vdSetsysPowerHighSpeed();

		CTOS_BackLightSet (d_BKLIT_LCD, d_ON);
		CTOS_BackLightSet (d_BKLIT_KBD, d_ON);
		inSleepflag = 0;
	}
	return;
}



int inCTOSS_CheckBatteryChargeStatus(void)
{
	USHORT usRet1;
	BYTE  bPercentage ;
	UCHAR usRet;
	DWORD pdwStatus ;

	usRet1 = CTOS_PowerSource(&usRet);
	//vdDebug_LogPrintf("CTOS_PowerSource=[%x]..usRet=[%d]......",usRet1,usRet);
	if ((d_OK == usRet1) && (usRet == d_PWRSRC_BATTERY))
	{
		return d_NO;
	}
	return d_OK;
	
}


void vdCTOS_uiPowerOff(void)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;
        
    CTOS_LCDTClearDisplay();

    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"WANT TO POWER");
    vduiDisplayStringCenter(5,"OFF TERMINAL");
    vduiDisplayStringCenter(6,"   REBOOT[<-]");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");

    c=WaitKey(60);

    if (c==d_KBD_CLEAR)
    {
		CTOS_SystemReset();
    }
	
    if(c!=d_KBD_ENTER)
    {
        return;
    }
    
    for(ya =1; ya<5; ya++)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,ya);
        CTOS_LCDTClear2EOL();
    }
    for(yb=8; yb>4; yb--)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,yb);
        CTOS_LCDTClear2EOL();
    }
    CTOS_LCDTPrintXY(1,4,"----------------");
    for(xa=1; xa<8; xa++)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xa,4," ");
    }
    for(xb=16; xb>7; xb--)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xb,4," ");
    }
            
    CTOS_LCDGShowPic(58, 6, block, 0, 6);
    CTOS_Delay(250);
    CTOS_LCDTGotoXY(7,4);
    CTOS_LCDTClear2EOL();
    CTOS_Delay(250);

    CTOS_PowerOff();
}

int inCTOS_IPP_HostSetting(void){

    srTransRec.HDTid = 6;
    strHDT.inHostIndex = srTransRec.HDTid;
    inHDTRead(srTransRec.HDTid);
    inCPTRead(srTransRec.HDTid);
    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

    return srTransRec.HDTid;
}

#ifdef CBPAY_DV
//#if 0
int inCTOS_SelectLOGONHostSetting(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inCountData = 0;
    int inECRTrxFlag = 0;

	int inCount =0;
	int inHDTid[50];
	int inCurrencyIndex;



	vdDebug_LogPrintf("inCTOS_SelectLOGONHostSetting  --- BEGIN");



	
    if (inMultiAP_CheckSubAPStatus() == d_OK){
        vdDebug_LogPrintf("this is sub app");
        return d_OK;
    }
        
    
//    if(strTCT.fMustAutoSettle == TRUE)
//        return d_OK;
    
    inECRTrxFlag = get_env_int("ECRTRANS");
    vdDebug_LogPrintf("inECRTrxFlag = %d", inECRTrxFlag);
	//vdDebug_PrintOnPaper("inECRTrxFlag = %d", inECRTrxFlag);
	
    if (strTCT.fECR && inECRTrxFlag) {
        strHDT.inHostIndex = srTransRec.HDTid;
        
        vdDebug_LogPrintf("inCTOS_SelectLOGONHostSetting inHostIndex = %d", srTransRec.HDTid);
		//vdDebug_PrintOnPaper("inHostIndex = %d", srTransRec.HDTid);
        inHDTRead(srTransRec.HDTid);
        inCPTRead(srTransRec.HDTid);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        //vdDebug_PrintOnPaper("inHostIndex = %d", srTransRec.HDTid);
        return srTransRec.HDTid;
    }
    else 
    {
    memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
    memset((char*)inCPTID, 0x00, sizeof(inCPTID));
    
    vdDebug_LogPrintf("inCTOS_SelectLOGONHostSetting srTransRec.byTransType = %d",srTransRec.byTransType);

	// Not to include host in HOST Settings selection based on inHostSettingOn value.
	inCount = inHDTReadOrderBySequence2(szHostName, inCPTID);


 
    for (inLoop = 0; inCount < 50; inLoop++)
    {   
    
	//vdDebug_LogPrintf("inCTOS_SelectLOGONHostSetting szHostMenu = %s", szHostMenu);
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0){
                strcat((char *)szHostMenu, (char *)" \n");   
            	}
        }
        else
            break;
    }



   
    if(szHostMenu[strlen(szHostMenu)-1] == '\x0A')
        szHostMenu[strlen(szHostMenu)-1] = 0;
    
    if(strTCT.fMustAutoSettle == FALSE) {
            key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

            if (key == 0xFF) {
                CTOS_LCDTClearDisplay();
                setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
                vduiWarningSound();
                return -1;
            }
        }
    
    if(strTCT.fMustAutoSettle == TRUE){
        return strHDT.inHostIndex;
    }
    
    
//    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
//
//    if (key == 0xFF) 
//    {
//        CTOS_LCDTClearDisplay();
//        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
//        vduiWarningSound();
//        return -1;  
//    }
    

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;
        
        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = inCPTID[key-1];
        strHDT.inHostIndex = inCPTID[key-1];
        inHDTRead(inCPTID[key-1]);
        inCPTRead(inCPTID[key-1]);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		vdDebug_LogPrintf("1--strHDT.inHostIndex[%d]", strHDT.inHostIndex);
    }
    return inCPTID[key-1];
}
}


//#if 0
int inCTOS_SelectHostSetting(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inCountData = 0;
    int inECRTrxFlag = 0;

	int inCount =0;
	int inHDTid[50];
	int inCurrencyIndex;



	vdDebug_LogPrintf("inCTOS_SelectHostSetting  --- BEGIN");



	
    if (inMultiAP_CheckSubAPStatus() == d_OK){
        vdDebug_LogPrintf("this is sub app");
        return d_OK;
    }
        
    
//    if(strTCT.fMustAutoSettle == TRUE)
//        return d_OK;
    
    inECRTrxFlag = get_env_int("ECRTRANS");
    vdDebug_LogPrintf("inECRTrxFlag = %d", inECRTrxFlag);
	//vdDebug_PrintOnPaper("inECRTrxFlag = %d", inECRTrxFlag);
	
    if (strTCT.fECR && inECRTrxFlag) {
        strHDT.inHostIndex = srTransRec.HDTid;
        
        vdDebug_LogPrintf("inHostIndex = %d", srTransRec.HDTid);
		//vdDebug_PrintOnPaper("inHostIndex = %d", srTransRec.HDTid);
        inHDTRead(srTransRec.HDTid);
        inCPTRead(srTransRec.HDTid);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        //vdDebug_PrintOnPaper("inHostIndex = %d", srTransRec.HDTid);
        return srTransRec.HDTid;
    }
    else 
    {
    memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
    memset((char*)inCPTID, 0x00, sizeof(inCPTID));
    
    vdDebug_LogPrintf("srTransRec.byTransType = %d",srTransRec.byTransType);

#if 0
	 vdDebug_LogPrintf("srTransRec.byTransType = %d inCurrencyIndex = %d", srTransRec.byTransType, strCST.inCurrencyIndex);
	 
	 if(strCST.inCurrencyIndex == 1)
		 inCurrencyIndex = 1; // select only MMK host
	 else
		 inCurrencyIndex = 0; // Select All host
		 
	 
	 inCount = inHDTReadOrderBySequenceSignOn(szHostName, inCPTID, inCurrencyIndex);
#else
	inCount = inHDTReadOrderBySequence(szHostName, inCPTID);

#endif

 //   inHDTReadHostName(szHostName, inCPTID);

    
    for (inLoop = 0; inCount < 50; inLoop++)
    {   
    
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");   
        }
        else
            break;
    }



   
    if(szHostMenu[strlen(szHostMenu)-1] == '\x0A')
        szHostMenu[strlen(szHostMenu)-1] = 0;
    
    if(strTCT.fMustAutoSettle == FALSE) {
            key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

            if (key == 0xFF) {
                CTOS_LCDTClearDisplay();
                setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
                vduiWarningSound();
                return -1;
            }
        }
    
    if(strTCT.fMustAutoSettle == TRUE){
        return strHDT.inHostIndex;
    }
    
    
//    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
//
//    if (key == 0xFF) 
//    {
//        CTOS_LCDTClearDisplay();
//        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
//        vduiWarningSound();
//        return -1;  
//    }
    

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;
        
        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = inCPTID[key-1];
        strHDT.inHostIndex = inCPTID[key-1];
        inHDTRead(inCPTID[key-1]);
        inCPTRead(inCPTID[key-1]);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		vdDebug_LogPrintf("1--strHDT.inHostIndex[%d]", strHDT.inHostIndex);
    }
    return inCPTID[key-1];
}
}




#else
int inCTOS_SelectHostSetting(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inCountData = 0;
    int inECRTrxFlag = 0;
    
    if (inMultiAP_CheckSubAPStatus() == d_OK){
        vdDebug_LogPrintf("this is sub app");
        return d_OK;
    }
        
    
//    if(strTCT.fMustAutoSettle == TRUE)
//        return d_OK;
    
    inECRTrxFlag = get_env_int("ECRTRANS");
    vdDebug_LogPrintf("inECRTrxFlag = %d", inECRTrxFlag);
	//vdDebug_PrintOnPaper("inECRTrxFlag = %d", inECRTrxFlag);
	
    if (strTCT.fECR && inECRTrxFlag) {
        strHDT.inHostIndex = srTransRec.HDTid;
        
        vdDebug_LogPrintf("inHostIndex = %d", srTransRec.HDTid);
		//vdDebug_PrintOnPaper("inHostIndex = %d", srTransRec.HDTid);
        inHDTRead(srTransRec.HDTid);
        inCPTRead(srTransRec.HDTid);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        //vdDebug_PrintOnPaper("inHostIndex = %d", srTransRec.HDTid);
        return srTransRec.HDTid;
    }
    else 
    {
    memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
    memset((char*)inCPTID, 0x00, sizeof(inCPTID));
    
    vdDebug_LogPrintf("srTransRec.byTransType = %d",srTransRec.byTransType);

    inHDTReadHostName(szHostName, inCPTID);
//    if(srTransRec.byTransType == SETUP){
//        inHDTReadHostName(szHostName, inCPTID);
//    } else{
//        if (fGetMPUTrans() == VS_TRUE)
//            inCountData = inHDTCheckHostName(szHostName, inCPTID, 1);
//        else
//            inCountData = inHDTCheckHostName(szHostName, inCPTID, 0);
//    }
    
    for (inLoop = 0; inLoop < 50; inLoop++)
    {   
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");   
        }
        else
            break;
    }
   
    if(szHostMenu[strlen(szHostMenu)-1] == '\x0A')
        szHostMenu[strlen(szHostMenu)-1] = 0;
    
    if(strTCT.fMustAutoSettle == FALSE) {
            key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

            if (key == 0xFF) {
                CTOS_LCDTClearDisplay();
                setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
                vduiWarningSound();
                return -1;
            }
        }
    
    if(strTCT.fMustAutoSettle == TRUE){
        return strHDT.inHostIndex;
    }
    
    
//    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
//
//    if (key == 0xFF) 
//    {
//        CTOS_LCDTClearDisplay();
//        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
//        vduiWarningSound();
//        return -1;  
//    }
    

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;
        
        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
        srTransRec.HDTid = inCPTID[key-1];
        strHDT.inHostIndex = inCPTID[key-1];
        inHDTRead(inCPTID[key-1]);
        inCPTRead(inCPTID[key-1]);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
		vdDebug_LogPrintf("1--strHDT.inHostIndex[%d]", strHDT.inHostIndex);
    }
    return inCPTID[key-1];
}
}
#endif

#ifdef CBB_FIN_ROUTING
int inCTOS_SelectHostSettingSignONALL(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inCountData = 0;
    int inECRTrxFlag = 0;

	int inCount =0;
	int inHDTid[50];
	int inCurrencyIndex = 0;



	vdDebug_LogPrintf("inCTOS_SelectHostSettingSignONALL  --- BEGIN");

	
    if (inMultiAP_CheckSubAPStatus() == d_OK){
        vdDebug_LogPrintf("this is sub app");
        return d_OK;
    }
        
    
    inECRTrxFlag = get_env_int("ECRTRANS");
    vdDebug_LogPrintf("inECRTrxFlag = %d", inECRTrxFlag);
	
    if (strTCT.fECR && inECRTrxFlag) {
        strHDT.inHostIndex = srTransRec.HDTid;
        
        vdDebug_LogPrintf("inHostIndex = %d", srTransRec.HDTid);
        inHDTRead(srTransRec.HDTid);
        inCPTRead(srTransRec.HDTid);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        return srTransRec.HDTid;
    }
    else 
	{
	    memset(szHostMenu, 0x00, sizeof(szHostMenu));
	    memset(szHostName, 0x00, sizeof(szHostName));
	    memset((char*)inCPTID, 0x00, sizeof(inCPTID));
	    
	    vdDebug_LogPrintf("srTransRec.byTransType = %d inCurrencyIndex = %d", srTransRec.byTransType, strCST.inCurrencyIndex);
		
		if(strCST.inCurrencyIndex == 1)
			inCurrencyIndex = 1; // select only MMK host
		else
			inCurrencyIndex = 0; // Select All host
			

		inCount = inHDTReadOrderBySequenceSignOn(szHostName, inCPTID, inCurrencyIndex);
	    
	    for (inLoop = 0; inCount < 50; inLoop++)
	    {   
	        if (szHostName[inLoop][0]!= 0)
	        {
	            strcat((char *)szHostMenu, szHostName[inLoop]);
	            if (szHostName[inLoop+1][0]!= 0)
	                strcat((char *)szHostMenu, (char *)" \n");   
	        }
	        else
	            break;
	    }



	   
	    if(szHostMenu[strlen(szHostMenu)-1] == '\x0A')
	        szHostMenu[strlen(szHostMenu)-1] = 0;
	    
	    if(strTCT.fMustAutoSettle == FALSE) {
	            key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

	            if (key == 0xFF) {
	                CTOS_LCDTClearDisplay();
	                setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
	                vduiWarningSound();
	                return -1;
	            }
	        }
	    
	    if(strTCT.fMustAutoSettle == TRUE){
	        return strHDT.inHostIndex;
	    }
	    
	    if(key > 0)
	    {
	        if(d_KBD_CANCEL == key)
	            return -1;
	        
	        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
	        srTransRec.HDTid = inCPTID[key-1];
	        strHDT.inHostIndex = inCPTID[key-1];
	        inHDTRead(inCPTID[key-1]);
	        inCPTRead(inCPTID[key-1]);
	        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			vdDebug_LogPrintf("1--strHDT.inHostIndex[%d]", strHDT.inHostIndex);
	    }
	    return inCPTID[key-1];
	}
}


//SIGNON MENU FOR IDLE/ MAIN MENU
int inCTOS_SelectHostSettingSignON(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inCountData = 0;
    int inECRTrxFlag = 0;

	int inCount =0;
	int inHDTid[50];
	int inCurrencyIndex = 0;



	vdDebug_LogPrintf("inCTOS_SelectHostSettingSignON  --- BEGIN");

	
    if (inMultiAP_CheckSubAPStatus() == d_OK){
        vdDebug_LogPrintf("this is sub app");
        return d_OK;
    }
        
    
    inECRTrxFlag = get_env_int("ECRTRANS");
    vdDebug_LogPrintf("inECRTrxFlag = %d", inECRTrxFlag);
	
    if (strTCT.fECR && inECRTrxFlag) {
        strHDT.inHostIndex = srTransRec.HDTid;
        
        vdDebug_LogPrintf("inHostIndex = %d", srTransRec.HDTid);
        inHDTRead(srTransRec.HDTid);
        inCPTRead(srTransRec.HDTid);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        return srTransRec.HDTid;
    }
    else 
	{
	    memset(szHostMenu, 0x00, sizeof(szHostMenu));
	    memset(szHostName, 0x00, sizeof(szHostName));
	    memset((char*)inCPTID, 0x00, sizeof(inCPTID));
	    
	    vdDebug_LogPrintf("srTransRec.byTransType = %d inCurrencyIndex = %d", srTransRec.byTransType, strCST.inCurrencyIndex);
		
		if(strCST.inCurrencyIndex == 1)
			inCurrencyIndex = 1; // select only MMK host
		else
			inCurrencyIndex = 0; // Select All host
			

		inCount = inHDTReadOrderBySequenceSignOn(szHostName, inCPTID, inCurrencyIndex);
	    
	    for (inLoop = 0; inCount < 50; inLoop++)
	    {   
	        if (szHostName[inLoop][0]!= 0)
	        {
	            strcat((char *)szHostMenu, szHostName[inLoop]);
	            if (szHostName[inLoop+1][0]!= 0)
	                strcat((char *)szHostMenu, (char *)" \n");   
	        }
	        else
			{
			
				//strcat((char *)szHostMenu, (char *)"\nSIGN ON ALL");	 
	            break;
	        }
	    }



	   
	    if(szHostMenu[strlen(szHostMenu)-1] == '\x0A')
	        szHostMenu[strlen(szHostMenu)-1] = 0;
	    
	    if(strTCT.fMustAutoSettle == FALSE) {
	            key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

	            if (key == 0xFF) {
	                CTOS_LCDTClearDisplay();
	                setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
	                vduiWarningSound();
	                return -1;
	            }
	        }
	    
	    if(strTCT.fMustAutoSettle == TRUE){
	        return strHDT.inHostIndex;
	    }

		
	    
		vdDebug_LogPrintf("inCTOS_SelectHostSettingSignON key[%d]", key);
	    if(key > 0)
	    {
	        if(d_KBD_CANCEL == key)
	            return -1;

			//if key selected was SignOn ALL
			//if(inCPTID[key-1] == 0)
			//	return 99;
	        
	        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
	        srTransRec.HDTid = inCPTID[key-1];
	        strHDT.inHostIndex = inCPTID[key-1];
	        inHDTRead(inCPTID[key-1]);
	        inCPTRead(inCPTID[key-1]);
	        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			vdDebug_LogPrintf("1--strHDT.inHostIndex[%d]", strHDT.inHostIndex);
	    }
	    return inCPTID[key-1];
	}
}

//SIGNON MENU FOR F2 KEY
int inCTOS_SelectHostSettingSignON_F2(void)
{
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT HOST";
    char szHostMenu[1024];
    char szHostName[50][100];
    int inCPTID[50];
    int inLoop = 0;
    int inCountData = 0;
    int inECRTrxFlag = 0;

	int inCount =0;
	int inHDTid[50];
	int inCurrencyIndex = 0;



	vdDebug_LogPrintf("inCTOS_SelectHostSettingSignON_F2  --- BEGIN");

	
    if (inMultiAP_CheckSubAPStatus() == d_OK){
        vdDebug_LogPrintf("this is sub app");
        return d_OK;
    }
        
    
    inECRTrxFlag = get_env_int("ECRTRANS");
    vdDebug_LogPrintf("inECRTrxFlag = %d", inECRTrxFlag);
	
    if (strTCT.fECR && inECRTrxFlag) {
        strHDT.inHostIndex = srTransRec.HDTid;
        
        vdDebug_LogPrintf("inHostIndex = %d", srTransRec.HDTid);
        inHDTRead(srTransRec.HDTid);
        inCPTRead(srTransRec.HDTid);
        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
        return srTransRec.HDTid;
    }
    else 
	{
	    memset(szHostMenu, 0x00, sizeof(szHostMenu));
	    memset(szHostName, 0x00, sizeof(szHostName));
	    memset((char*)inCPTID, 0x00, sizeof(inCPTID));
	    
	    vdDebug_LogPrintf("srTransRec.byTransType = %d inCurrencyIndex = %d fGetSIGNON() = %d", srTransRec.byTransType, strCST.inCurrencyIndex, fGetSIGNON());
		
		if(strCST.inCurrencyIndex == 1)
			inCurrencyIndex = 1; // select only MMK host
		else
			inCurrencyIndex = 0; // Select All host
			

		inCount = inHDTReadOrderBySequenceSignOn(szHostName, inCPTID, inCurrencyIndex);
	    
	    for (inLoop = 0; inCount < 50; inLoop++)
	    {   
	        if (szHostName[inLoop][0]!= 0)
	        {
	            strcat((char *)szHostMenu, szHostName[inLoop]);
	            if (szHostName[inLoop+1][0]!= 0)
	                strcat((char *)szHostMenu, (char *)" \n");   
	        }
	        else
			{
				if(fGetSIGNON() == FALSE)
					strcat((char *)szHostMenu, (char *)"\nSIGN ON ALL");	
				
	            break;
	        }
	    }



	   
	    if(szHostMenu[strlen(szHostMenu)-1] == '\x0A')
	        szHostMenu[strlen(szHostMenu)-1] = 0;
	    
	    if(strTCT.fMustAutoSettle == FALSE) {
	            key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

	            if (key == 0xFF) {
	                CTOS_LCDTClearDisplay();
	                setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
	                vduiWarningSound();
	                return -1;
	            }
	        }
	    
	    if(strTCT.fMustAutoSettle == TRUE){
	        return strHDT.inHostIndex;
	    }

		
	    
		vdDebug_LogPrintf("inCTOS_SelectHostSettingSignON key[%d]", key);
	    if(key > 0)
	    {
	        if(d_KBD_CANCEL == key)
	            return -1;

			//if key selected was SignOn ALL
			if(inCPTID[key-1] == 0)
				return 99;
	        
	        vdDebug_LogPrintf("key[%d] HostID[%d]", key, inCPTID[key-1]);
	        srTransRec.HDTid = inCPTID[key-1];
	        strHDT.inHostIndex = inCPTID[key-1];
	        inHDTRead(inCPTID[key-1]);
	        inCPTRead(inCPTID[key-1]);
	        strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			vdDebug_LogPrintf("1--strHDT.inHostIndex[%d]", strHDT.inHostIndex);
	    }
	    return inCPTID[key-1];
	}
}
#endif



void vdCTOS_MdmPPPConfig(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    USHORT usLen;
    int shHostIndex = 1;
        
    inResult = inMPTRead(shHostIndex);  
    if (inResult != d_OK)
        return;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("MDM PPP Setting");
    
	while(1)
	{
		vduiClearBelow(3);
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "Primary ISP Line");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, strMPT.szPriISPPhoneNumber);

            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                memcpy(strMPT.szPriISPPhoneNumber,strOut,strlen(strOut));
               	strMPT.szPriISPPhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }

        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "Second ISP Line");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strMPT.szSecISPPhoneNumber);
            
            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                //BolDetachDIALChangeSetting = TRUE;
                memcpy(strMPT.szSecISPPhoneNumber,strOut,strlen(strOut));
                strMPT.szSecISPPhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }
		
		while(1)
		{
			vduiClearBelow(3);
			
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "ISP USER NAME");
			setLCDPrint(4, DISPLAY_POSITION_LEFT, strMPT.szUserName);
		
			strcpy(strtemp,"New:") ;
			CTOS_LCDTPrintXY(1, 7, strtemp);
			memset(strOut,0x00, sizeof(strOut));
			usLen = 18;
			ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
			if(ret==d_KBD_ENTER)
			{
					BolDetachGPRSChangeSetting=TRUE;
					memcpy(strMPT.szUserName, strOut,strlen(strOut));
					strMPT.szUserName[strlen(strOut)]=0;
					inResult = inTCPSave(1);
					break;
			}	
			if(ret == d_KBD_CANCEL)
				break;
		}				
		
		while(1)
		{
			vduiClearBelow(3);
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "ISP PASSWORD");
			setLCDPrint(4, DISPLAY_POSITION_LEFT, strMPT.szPassword);
		
			strcpy(strtemp,"New:") ;
			CTOS_LCDTPrintXY(1, 7, strtemp);
			memset(strOut,0x00, sizeof(strOut));
			usLen = 18;
			ret= InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
				
			if(ret==d_KBD_ENTER)
			{
					BolDetachGPRSChangeSetting=TRUE;
					memcpy(strMPT.szPassword, strOut,strlen(strOut));
					strMPT.szPassword[strlen(strOut)]=0;
					inResult = inTCPSave(1);
					break;
			}	
			if(ret == d_KBD_CANCEL)
				break;
		}	

		while(1)
        {   
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "MDM TxRx Blk");
            memset(szInputBuf, 0x00, sizeof(szInputBuf));
            sprintf(szInputBuf, "%d", strMPT.inTxRxBlkSize);
            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
			usLen = 4;
            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>=1)
            {
                strMPT.inTxRxBlkSize = atoi(strOut);      
                vdMyEZLib_LogPrintf("inTxRxBlkSize %d", strMPT.inTxRxBlkSize);
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }


    	inResult = inMPTSave(shHostIndex);      
    	inResult = inMPTRead(shHostIndex);      
        srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
}

/*configuration functions*/
//for COM&USB communication
void vdCTOS_IPConfig(void)
{
    BYTE bRet, key;
    BYTE szInputBuf[5];
    int inResult;
    BYTE strOut[30],strtemp[17];
    USHORT ret;
    USHORT usLen;
    BOOL BolDetachLANChange=FALSE;
    int shHostIndex = 1;
    int inAutoSwitchSim=get_env_int("AUTOSWITCHSIM");
	int inManualSwitchSim=get_env_int("MANUALSWITCHSIM");
	
	#ifndef DUAL_SIM_SETTINGS
	    inAutoSwitchSim=0; /*disable - not yet complete*/
	#else
	    if(inManualSwitchSim == 1)
			inAutoSwitchSim=1;
	#endif
	
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;
                
    inResult = inCPTRead(shHostIndex);
    if(inResult != d_OK)
        return;

    CTOS_LCDTClearDisplay();
    vdDispTitleString("HOST SETTING");
    while(1)
    {
        vduiClearBelow(2);//for COM&USB communication
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "Pri Connection Type");
        if((strCPT.inCommunicationMode)== DIAL_UP_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "0");        
        else if((strCPT.inCommunicationMode)== ETHERNET_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "1");
        else if((strCPT.inCommunicationMode)== GPRS_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "2");
        else if((strCPT.inCommunicationMode)== MDM_PPP_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "3");
        else if((strCPT.inCommunicationMode)== WIFI_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "4");
        else if((strCPT.inCommunicationMode)== USB_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "5");		
		else if((strCPT.inCommunicationMode)== COM1_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "6");
        else if((strCPT.inCommunicationMode)== COM2_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "7");
		else
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "0");        
        
        CTOS_LCDTPrintXY(1, 4, "0-DIAL-UP1-LAN 2-GPRS");
        CTOS_LCDTPrintXY(1, 5, "3-MODEM PPP 4-WIFI");
		CTOS_LCDTPrintXY(1, 6, "5-USB 6-COM1 7-COM2");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33 || strOut[0]==0x34 || strOut[0]==0x35 || strOut[0]==0x36 || strOut[0]==0x37)
            {
                 if(strOut[0] == 0x30)
                 {
                        strCPT.inCommunicationMode = DIAL_UP_MODE;
                        BolDetachDIALChangeSetting = TRUE;
                 }
                 else if(strOut[0] == 0x31)
                 {
                        strCPT.inCommunicationMode = ETHERNET_MODE;
                        BolDetachLANChange = TRUE;
                 }
                 else if(strOut[0] == 0x32)
                 {
                        strCPT.inCommunicationMode = GPRS_MODE;
                        BolDetachGPRSChangeSetting = TRUE;
                 }
				 else if(strOut[0] == 0x33)
                 {
                        strCPT.inCommunicationMode = MDM_PPP_MODE;
                 }
				 else if(strOut[0] == 0x34)
                 {
                        strCPT.inCommunicationMode = WIFI_MODE;
				 }
                 else if(strOut[0] == 0x35)
                 {
                        strCPT.inCommunicationMode = USB_MODE;
                 }
				 else if(strOut[0] == 0x36)////for COM&USB communication
                 {
                        strCPT.inCommunicationMode = COM1_MODE;
                 }
                 else if(strOut[0] == 0x37)
                 {
                        strCPT.inCommunicationMode = COM2_MODE;
                 }
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiClearBelow(6);
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"CONNECTION MODE");
                CTOS_Delay(2000);       
            }
        }
    }
    inResult = inCPTSave(shHostIndex);
    inResult = inCPTRead(shHostIndex);

	while(1)
    {
        vduiClearBelow(2);////for COM&USB communication
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "Sec Connection Type");
        if((strCPT.inSecCommunicationMode)== DIAL_UP_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "0");        
        else if((strCPT.inSecCommunicationMode)== ETHERNET_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "1");
        else if((strCPT.inSecCommunicationMode)== GPRS_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "2");
        else if((strCPT.inSecCommunicationMode)== MDM_PPP_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "3");
        else if((strCPT.inSecCommunicationMode)== WIFI_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "4");
        else if((strCPT.inSecCommunicationMode)== USB_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "5");		
		else if((strCPT.inSecCommunicationMode)== COM1_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "6");
        else if((strCPT.inSecCommunicationMode)== COM2_MODE)
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "7");
		else if((strCPT.inSecCommunicationMode)== NULL_MODE)
				setLCDPrint(3, DISPLAY_POSITION_LEFT, "9");

        CTOS_LCDTPrintXY(1, 4, "0-DIAL-UP1-LAN2-GPRS");
        CTOS_LCDTPrintXY(1, 5, "3-MODEM PPP 4-WIFI");
		CTOS_LCDTPrintXY(1, 6, "5-USB6-COM1-COM29-NA");
   
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33 || strOut[0]==0x34 || strOut[0]==0x35 || strOut[0]==0x36 || strOut[0]==0x37 || strOut[0]==0x39)
            {
				if(strOut[0] == 0x30)
				{
					   strCPT.inSecCommunicationMode = DIAL_UP_MODE;
					   BolDetachDIALChangeSetting = TRUE;
				}
				else if(strOut[0] == 0x31)
				{
					   strCPT.inSecCommunicationMode = ETHERNET_MODE;
					   BolDetachLANChange = TRUE;
				}
				else if(strOut[0] == 0x32)
				{
					   strCPT.inSecCommunicationMode = GPRS_MODE;
					   BolDetachGPRSChangeSetting = TRUE;
				}
				else if(strOut[0] == 0x33)
				{
					   strCPT.inSecCommunicationMode = MDM_PPP_MODE;
				}
				else if(strOut[0] == 0x34)
				{
					   strCPT.inSecCommunicationMode = WIFI_MODE;
				}
				else if(strOut[0] == 0x35)
				{
					   strCPT.inSecCommunicationMode = USB_MODE;
				}
				else if(strOut[0] == 0x36)////for COM&USB communication
				{
					   strCPT.inSecCommunicationMode = COM1_MODE;
				}
				else if(strOut[0] == 0x37)
				{
					   strCPT.inSecCommunicationMode = COM2_MODE;
				}
				else if(strOut[0] == 0x39)
				{
					   strCPT.inSecCommunicationMode = NULL_MODE;
				}
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiClearBelow(6);
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"CONNECTION MODE");
                CTOS_Delay(2000);       
            }
        }
    }
    inResult = inCPTSave(shHostIndex);

    if(((strCPT.inCommunicationMode)== COM1_MODE) || ((strCPT.inCommunicationMode)== COM2_MODE)
		|| ((strCPT.inSecCommunicationMode)== COM1_MODE) || ((strCPT.inSecCommunicationMode)== COM2_MODE)
		|| ((strCPT.inCommunicationMode)== USB_MODE) || ((strCPT.inSecCommunicationMode)== USB_MODE))
    {
		while(1)
	    {
	        vduiClearBelow(2);
	        setLCDPrint(2, DISPLAY_POSITION_LEFT, "Header Type");
	        if((strCPT.inIPHeader)== NO_HEADER_LEN)
	            setLCDPrint(3, DISPLAY_POSITION_LEFT, "0");
	        if((strCPT.inIPHeader)== HEX_EXCLUDE_LEN)
	            setLCDPrint(3, DISPLAY_POSITION_LEFT, "1");        
	        if((strCPT.inIPHeader)== BCD_EXCLUDE_LEN)
	            setLCDPrint(3, DISPLAY_POSITION_LEFT, "2");
			if((strCPT.inIPHeader)== HEX_INCLUDE_LEN)
	            setLCDPrint(3, DISPLAY_POSITION_LEFT, "3");
	        if((strCPT.inIPHeader)== BCD_INCLUDE_LEN)
	            setLCDPrint(3, DISPLAY_POSITION_LEFT, "4");        
	        
	        CTOS_LCDTPrintXY(1, 4, "0-NO LEN 1-HEX");
	        CTOS_LCDTPrintXY(1, 5, "2-BCD 3-HEX+2 LEN");
			CTOS_LCDTPrintXY(1, 6, "4-BCD+2 LEN");
	        
	        strcpy(strtemp,"New:") ;
	        CTOS_LCDTPrintXY(1, 7, strtemp);
	        memset(strOut,0x00, sizeof(strOut));
	        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
	        if (ret == d_KBD_CANCEL )
	            break;
	        else if(0 == ret )
	            break;
	        else if(ret==1)
	        {
	            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33 || strOut[0]==0x34)
	            {
	                 if(strOut[0] == 0x31)
	                 {
	                        strCPT.inIPHeader = HEX_EXCLUDE_LEN;
	                 }
	                 if(strOut[0] == 0x30)
	                 {
	                        strCPT.inIPHeader = NO_HEADER_LEN;
	                 }
	                 if(strOut[0] == 0x32)
	                 {
	                        strCPT.inIPHeader = BCD_EXCLUDE_LEN;
	                 }
					 if(strOut[0] == 0x33)
	                 {
	                        strCPT.inIPHeader = HEX_INCLUDE_LEN;
	                 }
	                 if(strOut[0] == 0x34)
	                 {
	                        strCPT.inIPHeader = BCD_INCLUDE_LEN;
	                 }
	                 break;
	             }
	             else
	             {
	                vduiWarningSound();
	                vduiClearBelow(6);
	                vduiDisplayStringCenter(6,"PLEASE SELECT");
	                vduiDisplayStringCenter(7,"A VALID");
	                vduiDisplayStringCenter(8,"CONNECTION MODE");
	                CTOS_Delay(2000);       
	            }
	        }
	    }
    	inResult = inCPTSave(shHostIndex);
	
    	if(((strCPT.inCommunicationMode)== COM1_MODE) || ((strCPT.inCommunicationMode)== COM2_MODE))
	    {
	    	inResult = inCPTRead(shHostIndex);
	        if(inResult != ST_SUCCESS)
	            return;
	        while(1)
		    {   
	            vduiClearBelow(2);
	            setLCDPrint(3, DISPLAY_POSITION_LEFT, "PRI Baud Rate");
	            memset(szInputBuf, 0x00, sizeof(szInputBuf));
	            sprintf(szInputBuf, "%d", strCPT.inPriTxnComBaudRate);
	            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
	            
	            strcpy(strtemp,"New:") ;
	            CTOS_LCDTPrintXY(1, 7, strtemp);
	            memset(strOut,0x00, sizeof(strOut));
	            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
	            if (ret == d_KBD_CANCEL )
	                break;
	            else if(0 == ret )
	                break;
	            else if(ret>=1)
	            {
	                BolDetachLANChange=TRUE;
	                strCPT.inPriTxnComBaudRate = atoi(strOut);      
	                vdMyEZLib_LogPrintf("new host port %d",strCPT.inPriTxnComBaudRate);
	                break;
	            }   
	            if(ret == d_KBD_CANCEL)
	                break;
	        }
	        inResult = inCPTSave(shHostIndex);
	        if(inResult != ST_SUCCESS)
	            return;
	    }

		if(((strCPT.inSecCommunicationMode)== COM1_MODE) || ((strCPT.inSecCommunicationMode)== COM2_MODE))
	    {
	    	inResult = inCPTRead(shHostIndex);
	        if(inResult != ST_SUCCESS)
	            return;
	        while(1)
	        {   
	            vduiClearBelow(2);
	            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SEC Baud Rate");
	            memset(szInputBuf, 0x00, sizeof(szInputBuf));
	            sprintf(szInputBuf, "%d", strCPT.inSecTxnComBaudRate);
	            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
	            
	            strcpy(strtemp,"New:") ;
	            CTOS_LCDTPrintXY(1, 7, strtemp);
	            memset(strOut,0x00, sizeof(strOut));
	            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
	            if (ret == d_KBD_CANCEL )
	                break;
	            else if(0 == ret )
	                break;
	            else if(ret>=1)
	            {
	                BolDetachLANChange=TRUE;
	                strCPT.inSecTxnComBaudRate = atoi(strOut);      
	                vdMyEZLib_LogPrintf("new host port %d",strCPT.inSecTxnComBaudRate);
	                break;
	            }   
	            if(ret == d_KBD_CANCEL)
	                break;
	        }
	        inResult = inCPTSave(shHostIndex);
	        inResult = inCPTRead(shHostIndex);
	        if(inResult != ST_SUCCESS)
	            return;
	    }
    }
	
	vduiClearBelow(2);
    if(((strCPT.inCommunicationMode)== ETHERNET_MODE) || ((strCPT.inCommunicationMode)== WIFI_MODE) || ((strCPT.inCommunicationMode)== GPRS_MODE)|| ((strCPT.inCommunicationMode)== MDM_PPP_MODE)
		 || ((strCPT.inSecCommunicationMode)== ETHERNET_MODE) || ((strCPT.inSecCommunicationMode)== WIFI_MODE) ||  ((strCPT.inSecCommunicationMode)== GPRS_MODE) ||  ((strCPT.inSecCommunicationMode)== MDM_PPP_MODE))
    {
        inResult = inTCPRead(1);
        if(inResult != ST_SUCCESS)
            return;

        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "IP Config");
            if (strTCP.fDHCPEnable == IPCONFIG_DHCP)
                setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");
            if (strTCP.fDHCPEnable == IPCONFIG_STATIC)  
                setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
            
            CTOS_LCDTPrintXY(1, 5, "0-STATIC     1-DHCP");
            CTOS_LCDTPrintXY(1, 6, "                   ");
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret==1)
            {
                if (strOut[0]==0x30 || strOut[0]==0x31)
                {
                    BolDetachLANChange=TRUE;
                    
                    if(strOut[0] == 0x30)  
                            strTCP.fDHCPEnable = IPCONFIG_STATIC;
                    if(strOut[0] == 0x31)
                            strTCP.fDHCPEnable = IPCONFIG_DHCP;
                    break;
                }
                else
                {
                    vduiWarningSound();
                    vduiClearBelow(6);
                    vduiDisplayStringCenter(6,"PLEASE SELECT");
                    vduiDisplayStringCenter(7,"A VALID");
                    vduiDisplayStringCenter(8,"IP Config");
                    CTOS_Delay(2000);       
                }
            }
        }               
        inResult = inTCPSave(1);
        inResult = inTCPRead(1);

        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "HOST PRI IP");
            if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
                setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriTxnIP_2);
            else
                setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriTxnHostIP);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strtemp, 0x00, sizeof(strtemp));
            memset(strOut,0x00, sizeof(strOut));
            ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
            if(ret==d_KBD_ENTER)
            {
                BolDetachLANChange=TRUE;
                if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
                {
                    memcpy(strCPT.szPriTxnIP_2,strOut,strlen(strOut));
                    strCPT.szPriTxnIP_2[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szPriTxnIP_2);
                }
                else
                {
                    memcpy(strCPT.szPriTxnHostIP,strOut,strlen(strOut));
                    strCPT.szPriTxnHostIP[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szPriTxnHostIP);
                }
                break;
            }   
            if(ret == d_KBD_CANCEL)
                        break;
            }
        inResult = inCPTSave(shHostIndex);
        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        
        while(1)
        {   
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "HOST PRI PORT");
            memset(szInputBuf, 0x00, sizeof(szInputBuf));
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				sprintf(szInputBuf, "%d", strCPT.inPriTxnPort_2);
			else
                sprintf(szInputBuf, "%d", strCPT.inPriTxnHostPortNum);
            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>=1)
            {
                BolDetachLANChange=TRUE;
				if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				{
					strCPT.inPriTxnPort_2 = atoi(strOut);
					vdMyEZLib_LogPrintf("new host port %d",strCPT.inPriTxnPort_2);
				}
				else
				{
                    strCPT.inPriTxnHostPortNum = atoi(strOut);      
                    vdMyEZLib_LogPrintf("new host port %d",strCPT.inPriTxnHostPortNum);
				}
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }
        inResult = inCPTSave(shHostIndex);

        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "HOST SEC IP");
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecTxnIP_2);
			else
                setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecTxnHostIP);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strtemp, 0x00, sizeof(strtemp));
            memset(strOut,0x00, sizeof(strOut));
            ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
            if(ret==d_KBD_ENTER)
            {
                BolDetachLANChange=TRUE;
				if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				{
                    memcpy(strCPT.szSecTxnIP_2,strOut,strlen(strOut));
                    strCPT.szSecTxnIP_2[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szSecTxnIP_2);
				}
				else
				{
                    memcpy(strCPT.szSecTxnHostIP,strOut,strlen(strOut));
                    strCPT.szSecTxnHostIP[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szSecTxnHostIP);
				}
                break;
            }   
            if(ret == d_KBD_CANCEL)
                        break;
            }
        inResult = inCPTSave(shHostIndex);
        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        
        while(1)
        {   
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "HOST SEC PORT");
            memset(szInputBuf, 0x00, sizeof(szInputBuf));
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				sprintf(szInputBuf, "%d", strCPT.inSecTxnPort_2);
			else
                sprintf(szInputBuf, "%d", strCPT.inSecTxnHostPortNum);
            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>=1)
            {
                BolDetachLANChange=TRUE;
				if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				{
                    strCPT.inSecTxnPort_2 = atoi(strOut);      
                    vdMyEZLib_LogPrintf("new host port %d",strCPT.inSecTxnPort_2);
				}
				else
				{
                    strCPT.inSecTxnHostPortNum = atoi(strOut);      
                    vdMyEZLib_LogPrintf("new host port %d",strCPT.inSecTxnHostPortNum);
				}
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }
        inResult = inCPTSave(shHostIndex);
		
        inResult = inTCPRead(1);
        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLE HOST PRI IP");
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriSettleIP_2);
			else
                setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriSettlementHostIP);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strtemp, 0x00, sizeof(strtemp));
            memset(strOut,0x00, sizeof(strOut));
            ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
            if(ret==d_KBD_ENTER)
            {
                BolDetachLANChange=TRUE;
				if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				{
                    memcpy(strCPT.szPriSettleIP_2,strOut,strlen(strOut));
                    strCPT.szPriSettleIP_2[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szPriSettleIP_2);
				}
				else
				{
                    memcpy(strCPT.szPriSettlementHostIP,strOut,strlen(strOut));
                    strCPT.szPriSettlementHostIP[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szPriSettlementHostIP);
				}
                break;
            }   
            if(ret == d_KBD_CANCEL)
                        break;
            }
        inResult = inCPTSave(shHostIndex);
        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        
        while(1)
        {   
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLE HOST PRI PORT");
            memset(szInputBuf, 0x00, sizeof(szInputBuf));
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				sprintf(szInputBuf, "%d", strCPT.inPriSettlePort_2);
			else
                sprintf(szInputBuf, "%d", strCPT.inPriSettlementHostPort);
            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>=1)
            {
                BolDetachLANChange=TRUE;
				if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				{
					strCPT.inPriSettlePort_2 = atoi(strOut);      
                    vdMyEZLib_LogPrintf("new host port %d",strCPT.inPriSettlePort_2);
				}
				else
				{
                    strCPT.inPriSettlementHostPort = atoi(strOut);      
                    vdMyEZLib_LogPrintf("new host port %d",strCPT.inPriSettlementHostPort);
				}
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }
        inResult = inCPTSave(shHostIndex);

		inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLE HOST SEC IP");
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecSettleIP_2);
			else
                setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecSettlementHostIP);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strtemp, 0x00, sizeof(strtemp));
            memset(strOut,0x00, sizeof(strOut));
            ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
            if(ret==d_KBD_ENTER)
            {
                BolDetachLANChange=TRUE;
				if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				{
                    memcpy(strCPT.szSecSettleIP_2,strOut,strlen(strOut));
                    strCPT.szSecSettleIP_2[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szSecSettleIP_2);
				}
				else
				{
                    memcpy(strCPT.szSecSettlementHostIP,strOut,strlen(strOut));
                    strCPT.szSecSettlementHostIP[strlen(strOut)]=0;
                    vdMyEZLib_LogPrintf("new host ip %s",strCPT.szSecSettlementHostIP);
				}
                break;
            }   
            if(ret == d_KBD_CANCEL)
                        break;
            }
        inResult = inCPTSave(shHostIndex);
        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        
        while(1)
        {   
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLE HOST SEC PORT");
            memset(szInputBuf, 0x00, sizeof(szInputBuf));
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
	            sprintf(szInputBuf, "%d", strCPT.inSecSettlePort_2);
			else
                sprintf(szInputBuf, "%d", strCPT.inSecSettlementHostPort);
            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 5, 0, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>=1)
            {
                BolDetachLANChange=TRUE;
				if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE && strTCP.inSIMSlot==2)
				{
                    strCPT.inSecSettlePort_2 = atoi(strOut);      
                    vdMyEZLib_LogPrintf("new host port %d",strCPT.inSecSettlePort_2);
				}
				else
				{
                    strCPT.inSecSettlementHostPort = atoi(strOut);      
                    vdMyEZLib_LogPrintf("new host port %d",strCPT.inSecSettlementHostPort);
				}
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }
        inResult = inCPTSave(shHostIndex);

        inResult = inCPTRead(shHostIndex);
        if(inResult != ST_SUCCESS)
            return;
        while(1)
        {   
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "IP HEADER");
            memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "%d", strCPT.inIPHeader);
            setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);
            
            strcpy(strtemp,"New:") ;
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>=1)
            {
                BolDetachLANChange=TRUE;
				strCPT.inIPHeader = atoi(strOut);
                break;
            }   
            if(ret == d_KBD_CANCEL)
                break;
        }
        inResult = inCPTSave(shHostIndex);
		
        inResult = inCPTRead(shHostIndex);
		inResult = inTCPRead(1);
        if(strTCP.fDHCPEnable == IPCONFIG_STATIC)
        {
        	vdDebug_LogPrintf("old Client ip %s",strTCP.szTerminalIP);
            while(1)
            {
                vduiClearBelow(3);
                setLCDPrint(3, DISPLAY_POSITION_LEFT, "TERMINAL IP");
                setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szTerminalIP);
                
                strcpy(strtemp,"New:") ;     
                CTOS_LCDTPrintXY(1, 7, strtemp);
                memset(strtemp, 0x00, sizeof(strtemp));
                memset(strOut,0x00, sizeof(strOut));
                ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
				vdDebug_LogPrintf("ret %d strOut[%s]", ret, strOut);
				if(ret==d_KBD_ENTER)
                {
                    BolDetachLANChange=TRUE;
                    memcpy(strTCP.szTerminalIP, strOut, strlen(strOut));
                    strTCP.szTerminalIP[strlen(strOut)]=0;
                    vdDebug_LogPrintf("new Client ip %s",strTCP.szTerminalIP);
                    break;
                }   
                if(ret == d_KBD_CANCEL)
                    break;
            }
            inResult = inTCPSave(1);
            inResult = inTCPRead(1);    
            
            while(1)
            {
                vduiClearBelow(3);
                setLCDPrint(3, DISPLAY_POSITION_LEFT, "Sub. Mask IP");
                setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szSubNetMask);
                vdDebug_LogPrintf("old sub mask[%s]", strTCP.szSubNetMask);
                strcpy(strtemp,"New:") ;   
                CTOS_LCDTPrintXY(1, 7, strtemp);
                memset(strtemp, 0x00, sizeof(strtemp));
                memset(strOut,0x00, sizeof(strOut));
                ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
                if(ret==d_KBD_ENTER)
                {
                    BolDetachLANChange=TRUE;
                    memcpy(strTCP.szSubNetMask, strOut,strlen(strOut));
                    strTCP.szSubNetMask[strlen(strOut)]=0;
                    vdDebug_LogPrintf("new Sub. Mask ip %s", strTCP.szSubNetMask);
                    break;
                }   
                if(ret == d_KBD_CANCEL)
                    break;
            }               
                inResult = inTCPSave(1);
                inResult = inTCPRead(1);
                while(1)
                {
                    vduiClearBelow(3);
                    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Gateway IP");
                    setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szGetWay);
                    
					vdDebug_LogPrintf("old gateway[%s]", strTCP.szGetWay);
                    strcpy(strtemp,"New:") ;  
                    CTOS_LCDTPrintXY(1, 7, strtemp);
                    memset(strtemp, 0x00, sizeof(strtemp));
                    memset(strOut,0x00, sizeof(strOut));
                    ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 7, IP_LEN);
                    if(ret==d_KBD_ENTER)
                    {
                        BolDetachLANChange=TRUE;
                        memcpy(strTCP.szGetWay, strOut,strlen(strOut));
                        strTCP.szGetWay[strlen(strOut)]=0;
                        vdDebug_LogPrintf("new Gateway ip %s", strTCP.szGetWay);
                        break;
                    }   
                    if(ret == d_KBD_CANCEL)
                        break;
                }
                inResult = inTCPSave(1);
                inResult = inTCPRead(1);
                
                while(1)
                {
                    vduiClearBelow(3);
                    setLCDPrint(3, DISPLAY_POSITION_LEFT, "DNS1 IP");
                    if (wub_strlen(strTCP.szHostDNS1)<=0)
                        setLCDPrint(4, DISPLAY_POSITION_LEFT, "[NOT SET]");
                    else
                        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szHostDNS1);
                    
                    strcpy(strtemp,"New:");     
                    CTOS_LCDTPrintXY(1, 7, strtemp);
                    memset(strtemp, 0x00, sizeof(strtemp));
                    memset(strOut,0x00, sizeof(strOut));
                    ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 0, IP_LEN);
                    if(ret==d_KBD_ENTER)
                    {
                        if(strlen(strOut)>0)
                        {
                            BolDetachLANChange=TRUE;
                            memcpy(strTCP.szHostDNS1,strOut,strlen(strOut));
                            strTCP.szHostDNS1[strlen(strOut)]=0;
                            vdMyEZLib_LogPrintf("new DNS ip %s", strTCP.szHostDNS1);
                            break;
                        }
                        else
                        {
                            vduiClearBelow(3);
                            vduiDisplayStringCenter(4,"NO DNS IP");
                            vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                            vduiDisplayStringCenter(6,"DNS IP?");
                            vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                            key=struiGetchWithTimeOut();
                            if(key==d_KBD_ENTER)
                            {
                                memset(strTCP.szHostDNS1, 0, sizeof(strTCP.szHostDNS1));
                                break;
                            }
                        }
                    }                       
                    if(ret == d_KBD_CANCEL)
                        break;
                }
                inResult = inTCPSave(1);
                inResult = inTCPRead(1);

                while(1)
                {
                    vduiClearBelow(3);
                    setLCDPrint(3, DISPLAY_POSITION_LEFT, "DNS2 IP");
                    if (wub_strlen(strTCP.szHostDNS2)<=0)
                        setLCDPrint(4, DISPLAY_POSITION_LEFT, "[NOT SET]");
                    else
                        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szHostDNS2);
                    
                    strcpy(strtemp,"New:");     
                    CTOS_LCDTPrintXY(1, 7, strtemp);
                    memset(strtemp, 0x00, sizeof(strtemp));
                    memset(strOut,0x00, sizeof(strOut));
                    ret= struiApiGetStringSub(strtemp, 1,  8, strOut, MODE_IPADDRESS, 0, IP_LEN);
                    if(ret==d_KBD_ENTER)
                    {
                        if(strlen(strOut)>0)
                        {
                            BolDetachLANChange=TRUE;
                            memcpy(strTCP.szHostDNS2,strOut,strlen(strOut));
                            strTCP.szHostDNS2[strlen(strOut)]=0;
                            vdMyEZLib_LogPrintf("new DNS ip %s", strTCP.szHostDNS2);
                            break;
                        }
                        else
                        {
                            vduiClearBelow(3);
                            vduiDisplayStringCenter(4,"NO DNS IP");
                            vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                            vduiDisplayStringCenter(6,"DNS IP?");
                            vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                            key=struiGetchWithTimeOut();
                            if(key==d_KBD_ENTER)
                            {
                                memset(strTCP.szHostDNS2, 0, sizeof(strTCP.szHostDNS2));
                                break;
                            }
                        }
                    }                       
                    if(ret == d_KBD_CANCEL)
                        break;
                }
                inResult = inTCPSave(1);
                inResult = inTCPRead(1);
            }

			if(strCPT.inCommunicationMode == WIFI_MODE)
			{
				inCTOSS_COMMWIFISCAN();
                if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
                {
                   
                    vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
                    return;
                }				
			}

			vdDebug_LogPrintf("BolDetachLANChange %d", BolDetachLANChange);
       
            if(BolDetachLANChange==TRUE)
            {
                srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
                vduiClearBelow(3);
                CTOS_LCDTPrintXY (1,7, "Please Wait     ");       
                if(strCPT.inCommunicationMode == ETHERNET_MODE)
                    CTOS_LCDTPrintXY(1, 8, "Init LAN...     ");
                else if(strCPT.inCommunicationMode == GPRS_MODE)
                    CTOS_LCDTPrintXY(1, 8, "Init GPRS...     ");
                if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
                {
                   
                    vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
                    return;
                }

				vdDebug_LogPrintf("start init [%d]", strCPT.inCommunicationMode);
                
                srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
                srCommFuncPoint.inGetCommConfig(&srTransRec);
                srCommFuncPoint.inSetCommConfig(&srTransRec);

				srCommFuncPoint.inDisconnect(&srTransRec);

            }          
    }
	
    if(strCPT.inCommunicationMode == ETHERNET_MODE)
    {
       if(strTCP.fDHCPEnable != IPCONFIG_STATIC)
       {
//           inEthernet_GetTerminalIP(strtemp);
//           if (strcmp(strtemp,"0.0.0.0") == 0)
           {
                srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
                if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
                {
                   
                    vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
                    return;
                }
                srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
                srCommFuncPoint.inGetCommConfig(&srTransRec);
                srCommFuncPoint.inSetCommConfig(&srTransRec); 
           }

			srCommFuncPoint.inDisconnect(&srTransRec);

			CTOS_EthernetOpen();
			CTOS_EthernetClose();
			CTOS_EthernetOpen();
			inEthernet_GetTerminalIP(strtemp);
			CTOS_EthernetClose();
			
           vduiClearBelow(3);
           setLCDPrint(3, DISPLAY_POSITION_LEFT, "DHCP TERMINAL IP");
           setLCDPrint(4, DISPLAY_POSITION_LEFT, strtemp);
           WaitKey(30);
       }
    }

    if ((strCPT.inCommunicationMode)== MDM_PPP_MODE)
	{
		vdCTOS_MdmPPPConfig();
	}

    return ;
}

void vdCTOS_DialConfig(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    USHORT usLen;
    int shHostIndex = 1;
    
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;
    
    inResult = inCPTRead(shHostIndex);  
    if (inResult != d_OK)
        return;
    
    inResult = inTCTRead(1);
    if (inResult != d_OK)
        return;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("Dial Up Setting");
    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Primary PABX");
        if (wub_strlen(strTCT.szPabx)<=0)
            setLCDPrint(4,DISPLAY_POSITION_LEFT, "[DISABLED]");
        else
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szPabx);

        
        strcpy(strtemp,"New:") ; 
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        memset(strtemp, 0x00, sizeof(strtemp));
        usLen = 10;
        ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
                BolDetachDIALChangeSetting = TRUE;
                if(strlen(strOut)>0)
                {
                    memcpy(strTCT.szPabx,strOut,strlen(strOut));
                    strTCT.szPabx[strlen(strOut)]=',';
                    strTCT.szPabx[strlen(strOut)+1]=0;
                    break;
                }
                else
                {
                    vduiClearBelow(4);
                    vduiDisplayStringCenter(5,"NO PABX VALUE");
                    vduiDisplayStringCenter(6,"ENTERED,DISABLE");
                    vduiDisplayStringCenter(7,"PABX?");
                    vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                    key=struiGetchWithTimeOut();
                    if(key==d_KBD_ENTER)
                    {
                        memset(strTCT.szPabx,0,sizeof(strTCT.szPabx));
                                                break;
                    }
                }
        }
            if(ret == d_KBD_CANCEL)
                break;
        }

        inTCTSave(1);
        
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "Primary Line");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriTxnPhoneNumber);

            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szPriTxnPhoneNumber,strOut,strlen(strOut));
                strCPT.szPriTxnPhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }
            
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "Second Line");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecTxnPhoneNumber);
            
            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szSecTxnPhoneNumber,strOut,strlen(strOut));
                strCPT.szSecTxnPhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }
            
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLEMENT PRI LINE");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szPriSettlePhoneNumber);
            
            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szPriSettlePhoneNumber,strOut,strlen(strOut));
                strCPT.szPriSettlePhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }
            
        while(1)
        {
            vduiClearBelow(3);
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLEMENT SEC LINE");
            setLCDPrint(4, DISPLAY_POSITION_LEFT, strCPT.szSecSettlePhoneNumber);
            
            strcpy(strtemp,"New:") ; 
            CTOS_LCDTPrintXY(1, 7, strtemp);
            memset(strOut,0x00, sizeof(strOut));
            usLen = 18;
            ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 1, d_INPUT_TIMEOUT);
            if (ret == d_KBD_CANCEL )
                break;
            else if(0 == ret )
                break;
            else if(ret>= 1)
            {
                BolDetachDIALChangeSetting = TRUE;
                memcpy(strCPT.szSecSettlePhoneNumber,strOut,strlen(strOut));
                strCPT.szSecSettlePhoneNumber[strlen(strOut)]=0;
                break;
            }
            if(ret == d_KBD_CANCEL)
                break;

        }

    inResult = inCPTSave(shHostIndex);      
    inResult = inCPTRead(shHostIndex);      
            
    if(BolDetachDIALChangeSetting==TRUE)
    {
        srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
        vduiClearBelow(3);
        CTOS_LCDTPrintXY (1,7, "Please Wait     ");       
        CTOS_LCDTPrintXY(1, 8, "Init Modem...     ");

        if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
        {
           
            vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
            return;
        }
        
        srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
        srCommFuncPoint.inGetCommConfig(&srTransRec);
        srCommFuncPoint.inSetCommConfig(&srTransRec); 

    }
}

void vdCTOS_ModifyEdcSetting(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    strHDT.inHostIndex = shHostIndex;

    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("EDC SETTING");
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TERMINAL ID");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strMMT[0].szTID);
    
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strMMT[0].szTID, 0x00, sizeof(strMMT[0].szTID));
                memcpy(strMMT[0].szTID, strOut, TERMINAL_ID_BYTES);
                strMMT[0].szTID[TERMINAL_ID_BYTES]=0;
                inMMTSave(strMMT[0].MMTid);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
   if (ret == d_KBD_CANCEL )
        break ;
    }
    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "MERCHANT ID");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strMMT[0].szMID);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 15, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strMMT[0].szMID, 0x00, sizeof(strMMT[0].szMID));
                memcpy(strMMT[0].szMID, strOut, MERCHANT_ID_BYTES);
                strMMT[0].szMID[MERCHANT_ID_BYTES]=0;
                inMMTSave(strMMT[0].MMTid);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
       if (ret == d_KBD_CANCEL )
            break ;
    }

    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TPDU");
        wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTempBuf);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 10, 10, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==10)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strHDT.szTPDU, 0x00, sizeof(strHDT.szTPDU)); 
                wub_str_2_hex(strOut,strtemp,TPDU_BYTES);
                memcpy(strHDT.szTPDU, strtemp, TPDU_BYTES/2);
                inHDTSave(strHDT.inHostIndex);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
       if (ret == d_KBD_CANCEL )
            break ;
    }
    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "NII");
        wub_hex_2_str(strHDT.szNII,szTempBuf,2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTempBuf);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 4, 4, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==4)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strHDT.szNII, 0x00, sizeof(strHDT.szNII));
                wub_str_2_hex(strOut, strtemp, NII_BYTES);
                memcpy(strHDT.szNII, strtemp, NII_BYTES/2);
                inHDTSave(strHDT.inHostIndex);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
       }
       if (ret == d_KBD_CANCEL )
            break ;
        }

    inTCTRead(1);    
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "System PWD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szSystemPW);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szSystemPW,0x00,PASSWORD_LEN);
                memcpy(strTCT.szSystemPW,strOut, strlen(strOut));
                strTCT.szSystemPW[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"SYSTEM PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szSystemPW,0x00,PASSWORD_LEN);
                    inTCTSave(1);
                    break;
                }
            }
    }
    if (ret == d_KBD_CANCEL )
        break ;
    }
  
    inTCTRead(1);
    while(1)
    {    
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Engineer PWD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szEngineerPW);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szEngineerPW,0x00,PASSWORD_LEN);
                memcpy(strTCT.szEngineerPW,strOut, strlen(strOut));
                strTCT.szEngineerPW[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"ENGINEER PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szEngineerPW,0x00,PASSWORD_LEN);
                    inTCTSave(1);
                    break;
                }
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
        }

	inTCTRead(1);
    while(1)
	{    
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "PM password");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szPMpassword);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szPMpassword,0x00,8);
                memcpy(strTCT.szPMpassword,strOut, strlen(strOut));
                strTCT.szPMpassword[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"ENGINEER PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szPMpassword,0x00,8);
                    inTCTSave(1);
                    break;
                }
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
	}

	inTCTRead(1);
    while(1)
	{    
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "FunKey PWD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCT.szFunKeyPW);
        
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret>=1)
        {
            if (strlen(strOut)>0)
            {
                memset(strTCT.szFunKeyPW,0x00,8);
                memcpy(strTCT.szFunKeyPW,strOut, strlen(strOut));
                strTCT.szFunKeyPW[strlen(strOut)]=0;
                inTCTSave(1);
                break;
            }
            else
            {
                vduiClearBelow(3);
                vduiDisplayStringCenter(4,"NO PASSWORD");
                vduiDisplayStringCenter(5,"ENTERED,DISABLE");
                vduiDisplayStringCenter(6,"ENGINEER PWD?");
                vduiDisplayStringCenter(8,"NO[X] YES[OK]");
                key=WaitKey(60);
                if(key==d_KBD_ENTER)
                {
                    memset(strTCT.szFunKeyPW,0x00,8);
                    inTCTSave(1);
                    break;
                }
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
	}
            
	return ;
}

void vdCTOS_DeleteBatch(void)
{
    int         shHostIndex = 1;
    int         inResult,inRet;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    
    vduiLightOn();                

    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DELETE_BATCH);
        if(d_OK != inRet)
            return ;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return ;

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return ;
        }
    }

    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return;
            
    inResult = vduiAskConfirmContinue();
    if(inResult == d_OK)
    {

        if(CN_TRUE == strMMT[0].fMustSettFlag)
        {
            strMMT[0].fMustSettFlag = CN_FALSE;
            inMMTSave(strMMT[0].MMTid);
        }
    
        // delete batch where hostid and mmtid is matcj
        inDatabase_BatchDelete();

        memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
        memset(&strFile,0,sizeof(strFile));
        vdCTOS_GetAccumName(&strFile, &srAccumRec);
    
        if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
        {
            vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
        }
        //create the accum file
		memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    	inCTOS_ReadAccumTotal(&srAccumRec);
		
        inCTOS_DeleteBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);

        inMyFile_ReversalDelete();

        inMyFile_AdviceDelete();
        
        inMyFile_TCUploadDelete();
    
        CTOS_LCDTClearDisplay();
        setLCDPrint(5, DISPLAY_POSITION_CENTER, "CLEAR BATCH");
        setLCDPrint(6, DISPLAY_POSITION_CENTER, "RECORD DONE");
        CTOS_Delay(1000); 
    }                
}

void vdCTOS_DeleteReversal(void)
{
    CHAR szFileName[d_BUFF_SIZE];
    int         shHostIndex = 1;
    int         inResult,inRet;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    
    vduiLightOn();                

    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return;

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DELETE_REVERSAL);
        if(d_OK != inRet)
            return ;
    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetData();
            if(d_OK != inRet)
                return ;

            inRet = inCTOS_MultiAPReloadHost();
            if(d_OK != inRet)
                return ;
        }
    }

    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return;

    memset(szFileName,0,sizeof(szFileName));
    sprintf(szFileName, "%s%02d%02d.rev"
                    , strHDT.szHostLabel
                    , strHDT.inHostIndex
                    , srTransRec.MITid);

    vdDebug_LogPrintf("Rever Name %s",szFileName);
    
    if((inResult = inMyFile_CheckFileExist(szFileName)) < 0)
    {
        vdDebug_LogPrintf("inResult = %d", inResult);
        vdDebug_LogPrintf("inMyFile_CheckFileExist <0");
        vduiClearBelow(2);
        vdDisplayErrorMsg(1, 8, "NO REVERSAL EXIST");
        return ;
    }
            
    inResult = vduiAskConfirmContinue();
    if(inResult == d_OK)
    {
        inMyFile_ReversalDelete();
    
        CTOS_LCDTClearDisplay();
        setLCDPrint(5, DISPLAY_POSITION_CENTER, "CLEAR REVERSAL");
        setLCDPrint(6, DISPLAY_POSITION_CENTER, "RECORD DONE");
        CTOS_Delay(1000); 
    }                
}

void vdCTOS_PrintEMVTerminalConfig(void)
{
    CTOS_LCDTClearDisplay();
    vdPrintEMVTags();
    
    return;
}

void vdCTOSS_PrintTerminalConfig(void)
{
    CTOS_LCDTClearDisplay();
    #if 0
    vdPrintTerminalConfig();
	#else
    vdPrintTerminalConfigNew();
	#endif
    return;
}


void vdCTOS_ThemesSetting(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;
	// patrick test 20150115 start
	char szDatabaseName[100+1];
	strcpy(szDatabaseName, "DMENGTHAI.S3DB");

//    inRet = inTCTRead(1);  
//    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "THEMES SELECTION");
//        if(strTCT.byRS232DebugPort == 0)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
//        if(strTCT.byRS232DebugPort == 1)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
//        if(strTCT.byRS232DebugPort == 2)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
//        if(strTCT.byRS232DebugPort == 8)
//            setLCDPrint(4, DISPLAY_POSITION_LEFT, "8");  
        
        CTOS_LCDTPrintXY(1, 5, "0-SPRING       1-SUMMER");
//		CTOS_LCDTPrintXY(1, 6, "2-AUTUMN       3-WINTER");

//        CTOS_LCDTPrintXY(1, 6, "2-COM2    8-USB");
        
//        strcpy(strtemp,"New:") ;
//        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x37)
            {
                 if(strOut[0] == 0x31)
                 {
					 // patrick test 20150115 start
                     CTOS_LCDTTFSelect("tahoma.ttf", 0);
					 CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

					 CTOS_PrinterTTFSelect("tahoma.ttf", 0);
					 inCTOSS_SetERMFontType("tahoma.ttf", 0);
					 CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

					 inSetLanguageDatabase(szDatabaseName);
					 if ((strTCT.byTerminalType%2) == 0)
					 {
						 vdCTOSS_CombineMenuBMP("BG_SUMMER.BMP");
						 vdBackUpWhiteBMP("SUMMER.BMP","WHITE.BMP");
						 vdBackUpWhiteBMP("SUMMERTOUCH.BMP","menutouch.bmp");
						 vdBackUpWhiteBMP("SUMMERITEM.BMP","menuitem.bmp");
					 }
					 CTOS_LCDTClearDisplay();
					 inCTOSS_SetALLApFont("tahoma.ttf");
					 strTCT.inThemesType = 1;
					 inTCTSave(1);
					 break;
					 // patrick test 20150115 end
                 }
                 if(strOut[0] == 0x30)
                 {
					 // patrick test 20150115 start
					 CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
					 CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);					 

					 CTOS_PrinterTTFSelect(d_FONT_DEFAULT_TTF, 0);
					 inCTOSS_SetERMFontType(d_FONT_DEFAULT_TTF, 0);
					 CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);

					 inSetLanguageDatabase("");
					 if ((strTCT.byTerminalType%2) == 0)
					 {
						 vdCTOSS_CombineMenuBMP("BG_SPRING.BMP");
						 vdBackUpWhiteBMP("SPRING.BMP","WHITE.BMP");
						 vdBackUpWhiteBMP("SPRINGTOUCH.BMP","menutouch.bmp");
						 vdBackUpWhiteBMP("SPRINGITEM.BMP","menuitem.bmp");
					 }
					 CTOS_LCDTClearDisplay();
					 inCTOSS_SetALLApFont(d_FONT_DEFAULT_TTF);
					 strTCT.inThemesType = 0;
					 inTCTSave(1);
					 break;
					 // patrick test 20150115 end
                 }

				 if(strOut[0] == 0x37)
                 {
					 // patrick test 20150115 start
                     CTOS_LCDTTFSelect(THEMESTYPE_7_FONT, 0);
					 //CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

					 CTOS_PrinterTTFSelect(THEMESTYPE_7_FONT, 0);
					 inCTOSS_SetERMFontType(THEMESTYPE_7_FONT, 0);
					 //CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

					 inSetLanguageDatabase(szDatabaseName);
					 if ((strTCT.byTerminalType%2) == 0)
					 {
						 vdCTOSS_CombineMenuBMP("BG_SUMMER.BMP");
						 vdBackUpWhiteBMP("SUMMER.BMP","WHITE.BMP");
						 vdBackUpWhiteBMP("SUMMERTOUCH.BMP","menutouch.bmp");
						 vdBackUpWhiteBMP("SUMMERITEM.BMP","menuitem.bmp");
					 }
					 CTOS_LCDTClearDisplay();
					 inCTOSS_SetALLApFont(THEMESTYPE_7_FONT);
					 strTCT.inThemesType = 7;
					 inTCTSave(1);
					 break;
					 // patrick test 20150115 end
                 }
				 
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"THEMES");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}

void vdCTOS_Debugmode(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "DEBUG MODE");
        if(strTCT.byRS232DebugPort == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.byRS232DebugPort == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        if(strTCT.byRS232DebugPort == 2)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
        if(strTCT.byRS232DebugPort == 8)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "8");  
        
        CTOS_LCDTPrintXY(1, 5, "0-NO        1-COM1");
        CTOS_LCDTPrintXY(1, 6, "2-COM2    8-USB");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
		vdDebug_LogPrintf("shCTOS_GetNum(%d),usLen=[%d],strOut=[%s]",ret,usLen,strOut);
		if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x38)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.byRS232DebugPort = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.byRS232DebugPort = 0;
                 }
                 if(strOut[0] == 0x32)
                 {
                        strTCT.byRS232DebugPort = 2;
                 }
                 if(strOut[0] == 0x38)
                 {
                        strTCT.byRS232DebugPort = 8;
                 }
                
                 inRet = inTCTSave(1);
                 
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}

void vdCTOSS_InjectMKKey(void)
{
	BYTE bInBuf[250];
	BYTE bOutBuf[250];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	 USHORT usKeySet;
	USHORT usKeyIndex;

	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
  BYTE	x = 1;
  BYTE key;
	char szHeaderString[50] = "SELECT TO LOAD KEY";
	char szInjectKeyMenu[1024] = {0};

	BYTE baMPUTestKey[16];


	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 1, "INJECT MK");

	put_env_int("KEYSET",0);
	put_env_int("KEYINDEX",0);
	

	/*********************************************/
	//sidumili: [Select Pinpad Type for Injection]

	memset(szInjectKeyMenu, 0x00, sizeof(szInjectKeyMenu));
//	strcpy((char*)szInjectKeyMenu, "IPP \nPCi100 \nIPP/PCi100 \nHKLM DUKPT \nMPU INJECT KEY \nMPU-UPI KEY \nCANCEL"); //@@IBR MODIFY 06102016
        //strcpy((char*)szInjectKeyMenu, "IPP \nPCi100 \nIPP/PCi100 \nHKLM DUKPT \nMPU INJECT KEY \nMPU-UPI KEY \nIPP INJECT KEY \nUPI Finexus KEY \nCANCEL"); //@@IBR MODIFY 06102016
/*
#ifdef OK_DOLLAR_FEATURE
	strcpy((char*)szInjectKeyMenu, "IPP \nPCi100 \nIPP/PCi100 \nHKLM DUKPT \nMPU INJECT KEY \nVisa MC Finexus KEY \nIPP INJECT KEY \nUPI Finexus KEY \nOK$ Inject KEY \nCANCEL"); //@@IBR MODIFY 06102016
#else
	strcpy((char*)szInjectKeyMenu, "IPP \nPCi100 \nIPP/PCi100 \nHKLM DUKPT \nMPU INJECT KEY \nVisa MC Finexus KEY \nIPP INJECT KEY \nUPI Finexus KEY \nCANCEL"); //@@IBR MODIFY 06102016
#endif
*/

	#ifdef CBB_FIN_ROUTING
	vdDebug_LogPrintf("vdCTOSS_InjectMKKey CBB_FIN_ROUTING");
	
	strcpy((char*)szInjectKeyMenu, "IPP \nPCi100 \nIPP/PCi100 \nHKLM DUKPT \nMPU INJECT KEY \nVisa MC Finexus KEY \nIPP INJECT KEY \nUPI Finexus KEY \nOK$ Inject KEY \nFIN-MMK \nFIN-USD \nCANCEL"); //@@IBR MODIFY 06102016
	#else	
	strcpy((char*)szInjectKeyMenu, "IPP \nPCi100 \nIPP/PCi100 \nHKLM DUKPT \nMPU INJECT KEY \nVisa MC Finexus KEY \nIPP INJECT KEY \nUPI Finexus KEY \nOK$ Inject KEY \nCANCEL"); //@@IBR MODIFY 06102016
	#endif

	
	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szInjectKeyMenu, TRUE);

	if (key == 0xFF) 
	{	
		CTOS_LCDTClearDisplay();
		setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		vduiWarningSound();
		return; 
	}

	if (key > 0){
		
		if(d_KBD_CANCEL == key)
		return;

		memset(bOutBuf, 0x00, sizeof(bOutBuf));
		memset(bInBuf, 0x00, sizeof(bInBuf));

		// --->> IPP
		if (key == 1){
	
			// --- Inject to IPP
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "INJECT MK");
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_INJECTMK, bInBuf, usInLen, bOutBuf, &usOutLen);

			// -- sidumili: debugging of key injected
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT IN", bInBuf, usInLen);
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT OUT", bOutBuf, usOutLen);
			
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "Save MK error");
			else
				vdDisplayErrorMsg(1, 8, "Save MK OK");
	
			return ;
		}
			
		// --->> PCi100 / BOTH
		if ((key == 2) || (key == 3)){
			
			// --- Inject to IPP
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "INJECT MK");
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_INJECTMK, bInBuf, usInLen, bOutBuf, &usOutLen);

			// -- sidumili: debugging of key injected
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT IN", bInBuf, usInLen);
			//vdPrintPCIDebug_HexPrintf(TRUE, "INJECT OUT", bOutBuf, usOutLen);
			
			if (usResult != d_OK)
			return ;

			// --- Inject to PCi100
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "INJECT MK");
 			usKeySet = get_env_int("KEYSET");
			usKeyIndex = get_env_int("KEYINDEX");
			vdDebug_LogPrintf("usKeySet=[%x]",usKeySet);
			vdDebug_LogPrintf("usKeyIndex=[%x]",usKeyIndex);
			
			if (usOutLen == 8)
			usResult = inPCI100_SaveMKProcess(MK_DES_KEY,usOutLen,bOutBuf);
			if (usOutLen == 16)
			usResult = inPCI100_SaveMKProcess(MK_3DES_DOUBLE_KEY,usOutLen,bOutBuf);
			if (usOutLen == 24)
			usResult = inPCI100_SaveMKProcess(MK_3DES_TRIPLE_KEY,usOutLen,bOutBuf);

			if (usResult != d_OK)
			return ;

		}
		if (key == 4){
			inCTOSS_HKLMInjectDUKPTKey(0xc000,0x8888);
			return;
		}
                
		if (key == 5){ //@@IBR ADD 06102016
			vdCTOS_MPUInjectKey();
			memset(baMPUTestKey, 0x00, sizeof(baMPUTestKey));

			#if 0
			//EA57C11CAD5B51FE51oD76BC38E66497
			memcpy(baMPUTestKey, "\xEA\x57\xC1\x1C\xAD\x5B\x51\xFE\x51\x0D\x76\xBC\x38\xE6\x64\x97", 16); // for testing TMK for NEW CR
			ushMMK_WriteEncMAK(baMPUTestKey, 16, "\x36\xAF\x25\xF8", 4);
			ushMMK_WriteEncPIK(baMPUTestKey, 16, "\x36\xAF\x25\xF8", 4);

			#else
			memcpy(baMPUTestKey, "\x87\x73\xCA\x12\x3D\xD8\x5E\xF8\x59\xD4\xCE\x3F\x70\x58\xF5\x78", 16);			
			ushMPU_WriteEncMAK(baMPUTestKey, 16, "\x91\x93\x6C\xF8", 4);
			ushMPU_WriteEncPIK(baMPUTestKey, 16, "\x91\x93\x6C\xF8", 4);
			#endif

			vdDisplayErrorMsg(1, 8, "BPC TMK");			
			
			return;
		}

		if (key == 6){ //@@IBR ADD 06102016

			#ifdef PIN_CHANGE_ENABLE
			// FOR VISA/MC UAT HARD CODED TMK
			vdHardCodeVISAMC_FinexusUATKey();
			
			#else
			vdHardCodeCUP_UATKey();
			//ushCBB_WriteEncTPK("1234567812345678", 16); - orig

			//TPK from LOG ON DE57 value
			ushCBB_WriteEncTPK("\x12\xC1\x75\x82\xD4\xC2\xC6\x23\x62\xCD\x73\x0F\x69\x40\x73\x56", 16);
           		 #endif  
			
			vdDisplayErrorMsg(1, 8, "VISA/MC TMK");			
			return;
		}
                
        if(key == 7){
           vdCTOS_IPPInjectKey();
           return;
        }

		if(key == 8){		
			#ifdef PIN_CHANGE_ENABLE
			// FOR UPI UAT HARD CODED TMK
			vdHardCodeCUP_FinexusUATKey2();
			
		   #else
			 if(get_env_int("#UAT") == 1)						 
				 vdHardCodeCUP_FinexusUATKey();
			 else
				  vdCTOS_UPI_Finexus_InjectKey();
		   
		   #endif
		   vdDisplayErrorMsg(1, 8, "UPI Finexus KEY");		   

		   return;
        }
		
#ifdef OK_DOLLAR_FEATURE
				if(key == 9){		
				   // FOR AES UAT HARD CODED KEY
				   vdHardCodedOKD_UATKey();
				   
				   //vdDisplayErrorMsg(1, 8, "OK$ KEY Injected"); 	   
				   return;
				}

#else
				if (key == 9){
					vdDisplayErrorMsg(1, 8, "Inject cancelled");
				return;
				}
		
#endif

#ifdef CBB_FIN_ROUTING

			//EA57C11CAD5B51FE510D76BC38E66497
			//

			//refer to inUnPackIsoFunc62 for the PIK
			// for testing.  SHould be given on inUnPackIsoFunc62
			//MMK
			if (key == 10)
			{
				
				vdDebug_LogPrintf("vdCTOSS_InjectMKKey (key == 10)");
				#if 1
				vdCTOS_MMKInjectKey();

				/*
				memset(baMPUTestKey, 0x00, sizeof(baMPUTestKey));
				memcpy(baMPUTestKey, "\xEA\x57\xC1\x1C\xAD\x5B\x51\xFE\x51\x0D\x76\xBC\x38\xE6\x64\x97", 16); // for testing TMK for NEW CR
				ushMMK_WriteEncMAK(baMPUTestKey, 16, "\x36\xAF\x25\xF8", 4);
				ushMMK_WriteEncPIK(baMPUTestKey, 16, "\x36\xAF\x25\xF8", 4);
				*/

				#else
				// NO TMK UI entry 
				vdHardCodeMMK_FinexusUATKey();
				#endif
				
				vdDisplayErrorMsg(1, 8, "FIN-MMK TMK");			
				return;				

			}
			//USD
			if (key == 11)
			{ 
				vdDebug_LogPrintf("vdCTOSS_InjectMKKey (key == 11)");

				#if 1
				vdCTOS_USDInjectKey();
				
				/*memset(baMPUTestKey, 0x00, sizeof(baMPUTestKey));				
				memcpy(baMPUTestKey, "\xEA\x57\xC1\x1C\xAD\x5B\x51\xFE\x51\x0D\x76\xBC\x38\xE6\x64\x97", 16); // for testing TMK for NEW CR
				ushMMK_WriteEncMAK(baMPUTestKey, 16, "\x36\xAF\x25\xF8", 4);
				ushMMK_WriteEncPIK(baMPUTestKey, 16, "\x36\xAF\x25\xF8", 4);*/

				#else
				// NO TMK UI entry 
				vdHardCodeUSD_FinexusUATKey();
				#endif
				
				vdDisplayErrorMsg(1, 8, "FIN-USD TMK");			
				return;				

			}

#endif


		// --->> CANCEL
		if (key == 12){
			vdDisplayErrorMsg(1, 8, "Inject cancelled");
			return;
		}


	}
	/*********************************************/

	
#if 0
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	
	vdDebug_LogPrintf("d_IPC_CMD_EMV_INJECTMK usInLen[%d] ",usInLen);
	
	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_INJECTMK, bInBuf, usInLen, bOutBuf, &usOutLen);

	if (usResult != d_OK)
		return ;

	CTOS_LCDTPrintXY(1, 1, "INJECT MK");
	vdDebug_LogPrintf("d_IPC_CMD_EMV_INJECTMK usOutLen[%d] ",usOutLen);
	DebugAddHEX("MK",bOutBuf,usOutLen);
	
	inTCTRead(1);
	vdDebug_LogPrintf("InjectMKKey..byPinPadType=[%d],byPinPadPort=[%d]",strTCT.byPinPadType,strTCT.byPinPadPort);
	if (1 == strTCT.byPinPadType)
	{
		if (usOutLen == 8)
			inPCI100_SaveMKProcess(MK_DES_KEY,usOutLen,bOutBuf);
		if (usOutLen == 16)
			inPCI100_SaveMKProcess(MK_3DES_DOUBLE_KEY,usOutLen,bOutBuf);
		if (usOutLen == 24)
			inPCI100_SaveMKProcess(MK_3DES_TRIPLE_KEY,usOutLen,bOutBuf);
	}
#endif

	return;
}


void vdCTOSS_SelectPinpadType(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "PINPAD TYPE");
        if(strTCT.byPinPadType == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.byPinPadType == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        if(strTCT.byPinPadType == 2)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
		if(strTCT.byPinPadType == 3)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "3");
  
        
        CTOS_LCDTPrintXY(1, 5, "0-None	1-PCI100");
        CTOS_LCDTPrintXY(1, 6, "2-OTHER 3-V3P");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.byPinPadType = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.byPinPadType = 0;
                 }
                 if(strOut[0] == 0x32)
                 {
                        strTCT.byPinPadType = 2;
                 }
				 if(strOut[0] == 0x33)
                 {
                        strTCT.byPinPadType = 3;
                 }
 
                
                 inRet = inTCTSave(1);
                 
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"PINPAD TYPE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}


int inCTOSS_GetCtlsMode(void)
{
	inTCTRead(1);
	return strTCT.byCtlsMode;
}

void vdCTOSS_CtlsMode(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;

    inRet = inTCTRead(1);  
    vdDebug_LogPrintf(". inTCTRead(%d)",inRet);

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "CTLS MODE");
        if(strTCT.byCtlsMode == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.byCtlsMode == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");        
        if(strTCT.byCtlsMode == 2)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "2");
		if(strTCT.byCtlsMode == 3)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "3");
		if(strTCT.byCtlsMode == 4)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "4");
 
        
        CTOS_LCDTPrintXY(1, 5, "0-Disable 1-Internal");
        CTOS_LCDTPrintXY(1, 6, "2-External 3-V3 CTLS");
		CTOS_LCDTPrintXY(1, 7, "4-V3 INT CTLS");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 8, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32 || strOut[0]==0x33 || strOut[0]==0x34)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.byCtlsMode = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.byCtlsMode = 0;
                 }
                 if(strOut[0] == 0x32)
                 {
                        strTCT.byCtlsMode = 2;
                 }
				 if(strOut[0] == 0x33)
                 {
                        strTCT.byCtlsMode = 3;
                 }
				 if(strOut[0] == 0x34)
                 {
                        strTCT.byCtlsMode = 4;
                 }
                
                 inRet = inTCTSave(1);
                 
                 vdDebug_LogPrintf(". inTCTSave(%d)",inRet);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}


void vdCTOS_DemoMode(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1,usResult;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
	int inRet = d_NO;

	//int inResult = 0;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

    inTCTRead(1);   
	//CTOS_LCDTClearDisplay();
	//vduiClearBelow(7);
	vdCTOS_SetTransType(DEMO_MODE);
	vdDispTransTitle(SETUP);

	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
	{
		//inCTOS_IdleEventProcess();
		return inRet;
	}

    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "DEMO MODE");
        if(strTCT.fDemo == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.fDemo == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");     
        
        CTOS_LCDTPrintXY(1, 5, "0-DISABLE      1-ENABLE");
        
   
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            //check host num
            inNumOfHost = inHDTNumRecord();
            
            vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
            for(inNum =1 ;inNum <= inNumOfHost; inNum++)
            {
                if(inHDTRead(inNum) == d_OK)
                {
                    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
                
                    vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
                    for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
                    {
                        if((inResult = inMMTReadRecord(strHDT.inHostIndex,strMMT[inLoop-1].MITid)) !=d_OK)
                        {
                            vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex,inResult);
                            continue;
                            //break;
                        }
                        else    // delete batch where hostid and mmtid is match  
                        {
                            strMMT[0].HDTid = strHDT.inHostIndex;
                            strMMT[0].MITid = strMMT[inLoop-1].MITid;
                            inDatabase_BatchDeleteHDTidMITid();
                            vdDebug_LogPrintf("[inDatabase_BatchDelete]-Mitid[%d]strHDT.inHostIndex[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex);
                            
                            memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
                            memset(&strFile,0,sizeof(strFile));
                            srTransRec.MITid = strMMT[inLoop-1].MITid;
                            vdCTOS_GetAccumName(&strFile, &srAccumRec);
                            
                            if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
                            {
                                vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
                            }

                            inCTOS_DeleteBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);

                            inMyFile_ReversalDelete();

                            inMyFile_AdviceDelete();
                            
                            inMyFile_TCUploadDelete();

                        }
                    }
                }
                else
                    continue;

            }
            
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.fDemo = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.fDemo = 0;
                 }

                 inTCTSave(1);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}


void vdCTOS_TipAllowd(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;

    inTCTRead(1);    
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Tip Allowed");
        if(strTCT.fTipAllowFlag == 0)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "0");
        if(strTCT.fTipAllowFlag == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "1");     
        
        CTOS_LCDTPrintXY(1, 5, "0-DISABLE      1-ENABLE");
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            if (strOut[0]==0x30 || strOut[0]==0x31)
            {
                 if(strOut[0] == 0x31)
                 {
                        strTCT.fTipAllowFlag = 1;
                 }
                 if(strOut[0] == 0x30)
                 {
                        strTCT.fTipAllowFlag = 0;
                 }

                 inTCTSave(1);
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"DEBUG MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}

void vdCTOS_DualGPRSSetting(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    short inRtn;
    USHORT shMaxLen = 30;	
	char szHeaderString[50] = "GPRS SETUP";
	char szSIMMenu[1024] = {0};
	int inRecno=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE	x = 1;

	CTOS_LCDTClearDisplay();
	
	while (1)
	{
	    memset(szSIMMenu, 0x00, sizeof(szSIMMenu));
		strcpy((char*)szSIMMenu, "SIM1 \nSIM2");
		key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szSIMMenu, TRUE);
		if (key > 0)
		{
			if(d_KBD_CANCEL == key)
			{
				vdDisplayErrorMsg(1, 8, "CANCELED");
				return d_NO;
			}


			if (key == 1)
			{
				inRecno=1;
				break;
				
			}

			if (key == 2)
			{
				inRecno=2;
				break;
				
			}

			
		}
	}


	  inResult = inTCPRead(inRecno);
    if (inResult != d_OK)
        return;

		
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "APN");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szAPN);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szAPN, strOut,strlen(strOut));
            strTCP.szAPN[strlen(strOut)]=0;
            inResult = inTCPSave(inRecno);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }               

    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS USER NAME");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szUserName);

        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szUserName, strOut,strlen(strOut));
            strTCP.szUserName[strlen(strOut)]=0;
            inResult = inTCPSave(inRecno);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }               

    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS PASSWORD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szPassword);

        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szPassword, strOut,strlen(strOut));
            strTCP.szPassword[strlen(strOut)]=0;
            inResult = inTCPSave(inRecno);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }   

    if(BolDetachGPRSChangeSetting==TRUE)
    {
            srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
            clearLine(3);
            clearLine(4);
            clearLine(5);
            clearLine(6);
            clearLine(7);
            clearLine(8);
            CTOS_LCDTPrintXY (1,7, "Please Wait     ");       
            CTOS_LCDTPrintXY(1, 8, "Init GPRS...     ");
            if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
            {
               
                vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
                return;
            }
                          
            srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
            srCommFuncPoint.inGetCommConfig(&srTransRec);
            srCommFuncPoint.inSetCommConfig(&srTransRec);

    }

    return ;
}


void vdCTOS_GPRSSetting(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    short inRtn;
    USHORT shMaxLen = 30;

	if (get_env_int("AUTOSWITCHSIM") == 1)
		{
			vdCTOS_DualGPRSSetting();
			 return;
		}
    
    inResult = inTCPRead(1);
    if (inResult != d_OK)
        return;
        
    CTOS_LCDTClearDisplay();
    vdDispTitleString("GPRS SETUP");
    
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "APN");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szAPN);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szAPN, strOut,strlen(strOut));
            strTCP.szAPN[strlen(strOut)]=0;
            inResult = inTCPSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }               

    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS USER NAME");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szUserName);

        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szUserName, strOut,strlen(strOut));
            strTCP.szUserName[strlen(strOut)]=0;
            inResult = inTCPSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }               

    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "GPRS PASSWORD");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, strTCP.szPassword);

        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= InputStringAlphaEx2(1, 8, 0x00, 0x02, strOut, &shMaxLen, 1, d_INPUT_TIMEOUT);
        
        if(ret==d_KBD_ENTER)
        {
            BolDetachGPRSChangeSetting=TRUE;
            memcpy(strTCP.szPassword, strOut,strlen(strOut));
            strTCP.szPassword[strlen(strOut)]=0;
            inResult = inTCPSave(1);
            break;
        }   
        if(ret == d_KBD_CANCEL)
            break;
    }   

    if(BolDetachGPRSChangeSetting==TRUE)
    {
            srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
            clearLine(3);
            clearLine(4);
            clearLine(5);
            clearLine(6);
            clearLine(7);
            clearLine(8);
            CTOS_LCDTPrintXY (1,7, "Please Wait     ");       
            CTOS_LCDTPrintXY(1, 8, "Init GPRS...     ");
            if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
            {
               
                vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
                return;
            }
                          
            srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
            srCommFuncPoint.inGetCommConfig(&srTransRec);
            srCommFuncPoint.inSetCommConfig(&srTransRec);

    }

    return ;
}

/* delete a Char in string */
void DelCharInStr(char *str, char c, int flag)
{
    int i,l;

    l=strlen(str);

    if (flag == STR_HEAD)
    {
        for (i=0;i<l&&str[i]==c;i++);
        if (i>0) strcpy(str,str+i);
    }

    if (flag == STR_BOTTOM)
    {
        for (i=l-1;i>=0&&str[i]==c;i--);
        str[i+1]='\0';
    }

    if (flag == STR_ALL)
    {
        for (i=0;i<l;i++)
            if (str[i]==c)
            {
                strcpy(str+i,str+i+1);
                i--;
            }
    }
}

void vdCTOSS_DownloadMode(void)
{
	CTOS_LCDTClearDisplay();
	CTOS_EnterDownloadMode();

	inCTOS_KillALLSubAP();
	
	exit(0);
	return ;
}

void vdCTOSS_CheckMemory(void)
{
	vdCTOSS_GetMemoryStatus("MEMORY");
	return ;
}

/* ==========================================================================
 *
 * FUNCTION NAME: SetRTC
 *
 * DESCRIPTION:   Use this function to set the real-time clock's data and time.
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
void CTOSS_SetRTC(void)
{
	//Declare Local Variable //
	CTOS_RTC SetRTC;
	USHORT i = 0;
	BYTE isSet = FALSE;
	BYTE baYear[4+1] = {0},
		baMonth[2+1] = {0},
		baDay[2+1] = {0},
		baHour[2+1] = {0},
		baMinute[2+1] = {0},
		baSecond[2+1]  = {0};
	BYTE key = 0;
	BYTE babuff[d_BUFF_SIZE] = {0};
	BYTE g_DeviceModel;

//	CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);

	CTOS_LCDTClearDisplay();
	CTOS_LCDTSetReverse(TRUE);
	CTOS_LCDTPrintXY(1, 1, "	 Set RTC	 ");
	CTOS_LCDTSetReverse(FALSE);

	//Read the date and the time //
	CTOS_RTCGet(&SetRTC);

	//Show on the LCD Display //
	CTOS_LCDTPrintXY(1, 2, "   Get	 Set");
	sprintf(babuff,"YY:%04d",SetRTC.bYear + 2000);
	CTOS_LCDTPrintXY(1, 3, babuff);
	sprintf(babuff,"MM:%02d",SetRTC.bMonth);
	CTOS_LCDTPrintXY(1, 4, babuff);
	sprintf(babuff,"DD:%02d",SetRTC.bDay);
	CTOS_LCDTPrintXY(1, 5, babuff);
	sprintf(babuff,"hh:%02d",SetRTC.bHour);
	CTOS_LCDTPrintXY(1, 6, babuff);
	sprintf(babuff,"mm:%02d",SetRTC.bMinute);
	CTOS_LCDTPrintXY(1, 7, babuff);
	sprintf(babuff,"ss:%02d",SetRTC.bSecond);
	CTOS_LCDTPrintXY(1, 8, babuff);
//	  sprintf(babuff,"%02d",SetRTC.bDoW);
//	  CTOS_LCDTPrintXY(15, 8, babuff);

	//Input data for the setting //
	//i = sizeof(baYear);
	i = 5;
	if (InputStringAlphaEx(10,3, 0x05, 0x00, baYear, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Year //
	SetRTC.bYear = (wub_str_2_long(baYear) - 2000);
	isSet = TRUE;
	}
	//i = sizeof(baMonth);
	i = 3;
	if (InputStringAlphaEx(10,4, 0x05, 0x00, baMonth, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){ //Input Month //
	SetRTC.bMonth = wub_str_2_long(baMonth);
	isSet = TRUE;
	}
	//i = sizeof(baDay);
	i = 3;
	if (InputStringAlphaEx(10,5, 0x05, 0x00, baDay, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){ //Input Day //
	SetRTC.bDay = wub_str_2_long(baDay);
	isSet = TRUE;
	}
	//i = sizeof(baHour);
	i = 3;
	if (InputStringAlphaEx(10,6, 0x05, 0x00, baHour, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Hour //
	SetRTC.bHour = wub_str_2_long(baHour);
	isSet = TRUE;
	}
	//i = sizeof(baMinute);
	i = 3;
	if (InputStringAlphaEx(10,7, 0x05, 0x00, baMinute, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Minute //
	SetRTC.bMinute = wub_str_2_long(baMinute);
	isSet = TRUE;
	}
	//i = sizeof(baSecond);
	i = 3;
	if (InputStringAlphaEx(10,8, 0x05, 0x00, baSecond, &i, i, d_INPUT_TIMEOUT) == d_KBD_ENTER){
	//Input Second //
	SetRTC.bSecond = wub_str_2_long(baSecond);
	isSet = TRUE;
	}

	if (isSet){
		//Set the date and time //
		if (CTOS_RTCSet(&SetRTC) == d_OK)
			CTOS_LCDTPrintXY(15, 2, "OK");
		else
			CTOS_LCDTPrintXY(13, 2, "Fail");
		isSet = FALSE;
	}

	WaitKey(10);
//	  CTOS_KBDGet ( &key );

	return;
}

//void CTOSS_SetRTC(void) {
//    //Declare Local Variable //
//    int inRet;
//    CTOS_RTC SetRTC;
//    BYTE baYear[2 + 1] = "", baMonth[2 + 1] = "", baDay[2 + 1] = "", baHour[2 + 1] = "", baMinute[2 + 1] = "", baSecond[2 + 1] = "";
//    BYTE babuff[d_BUFF_SIZE];
//
//    CTOS_LCDTClearDisplay();
//    CTOS_LCDTSetReverse(TRUE);
//    setLCDPrint(1, DISPLAY_POSITION_LEFT, "      SET  RTC      ");
//    CTOS_LCDTSetReverse(FALSE);
//
//    //Read the date and the time //
//    CTOS_RTCGet(&SetRTC);
//
//    memset(babuff, 0x00, sizeof (babuff));
//    sprintf(babuff, "%02d%02d%02d", SetRTC.bDay, SetRTC.bMonth, SetRTC.bYear);
//    setLCDPrint(3, DISPLAY_POSITION_LEFT, "SET DATE[DD/MM/YY]");
//    do {
//        inRet = inEnterFmtStr(4, babuff, '/', 6 + 1);
//        if (inRet == d_KBD_CANCEL || inRet == d_NO) {
//            return;
//        }
//    } while (strlen(babuff) != 6);
//    memcpy(baDay, babuff, 2);
//    SetRTC.bDay = atoi(baDay);
//    memcpy(baMonth, babuff + 2, 2);
//    SetRTC.bMonth = atoi(baMonth);
//    memcpy(baYear, babuff + 4, 2);
//    SetRTC.bYear = atoi(baYear);
//
//    memset(babuff, 0x00, sizeof (babuff));
//    sprintf(babuff, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
//    setLCDPrint(6, DISPLAY_POSITION_LEFT, "SET DATE[hh:mm:ss]");
//    do {
//        inRet = inEnterFmtStr(7, babuff, ':', 6 + 1);
//        if (inRet == d_KBD_CANCEL || inRet == d_NO) {
//            return;
//        }
//    } while (strlen(babuff) != 6);
//    memcpy(baHour, babuff, 2);
//    SetRTC.bHour = atoi(baHour);
//    memcpy(baMinute, babuff + 2, 2);
//    SetRTC.bMinute = atoi(baMinute);
//    memcpy(baSecond, babuff + 4, 2);
//    SetRTC.bSecond = atoi(baSecond);
//
//    //Set the date and time //
//    if (CTOS_RTCSet(&SetRTC) == d_OK)
//        setLCDPrint(2, DISPLAY_POSITION_CENTER, "OK");
//    else
//        setLCDPrint(2, DISPLAY_POSITION_CENTER, "FAIL");
//
//    return;
//}

void vdCTOSS_FileUpload(void)
{
    int st, uintRet;
	unsigned char szDisplay[40];
    unsigned char temp[1024];
    unsigned char public_path[50] = "/home/ap/pub/";
    unsigned char private_path[50] = "./fs_data";
    FILE  *fPubKey;
    unsigned char szFileName1[50];
	char szFileName[50];
	ULONG ulSize;
	int iExp;
    STR *keyboardLayoutEnglish[]={" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
    "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>_",
    ".!@#$%^&*()-"};
    int inRet;

	CTOS_LCDTClearDisplay ();                                                                                
                                                                                                           
                               
    CTOS_LCDTSetReverse(FALSE);    
    CTOS_USBOpen();
    
    CTOS_LCDTPrintXY(1,1,"ENTER FILENAME:");
    inRet=CTOS_UIKeypad(1, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0,szFileName1, 50);
    if(inRet==d_OK)
    {
        if (strlen(szFileName1)>0)
		{
            memset(szFileName, 0x00, sizeof(szFileName));
            sprintf(szFileName,"%s%s", public_path,szFileName1);
        }
        else
        {
             CTOS_LCDTPrintXY(1,1,"TRANSFER CANCEL");
             WaitKey(3);
             return;
        }
    }
    else
    {
        CTOS_LCDTPrintXY(1,1,"TRANSFER CANCEL");
        WaitKey(3);
        return;
    }
    //strcpy(szFileName1,"V5S_VISAMASTER.prm");
    //sprintf(szFileName,"%s%s", public_path,szFileName1);
    
    ulSize = 0;
	fPubKey = fopen( (char*)szFileName1, "rb");
	if (fPubKey == NULL)
	{
        CTOS_LCDTPrintXY(1,1,"OPEN FILE ERROR");
        CTOS_LCDTPrintXY(1,2,szFileName1);
        //CTOS_LCDTPrintXY(1,3,&szFileName[19]);
        WaitKey(5);
		//memset(szFileName, 0x00, sizeof(szFileName));
        //sprintf(szFileName,"%s%s", private_path,szFileName1);
        fPubKey = fopen( (char*)szFileName, "rb");
        if (fPubKey == NULL)
        {
            CTOS_LCDTPrintXY(1,1,"OPEN FILE ERROR");
            CTOS_LCDTPrintXY(1,2,szFileName);
            CTOS_LCDTPrintXY(1,3,&szFileName[19]);
            
            memset(szFileName, 0x00, sizeof(szFileName));
            sprintf(szFileName,"%s%s", private_path,szFileName1);
            fPubKey = fopen( (char*)szFileName, "rb");
            if (fPubKey == NULL)
            {
                CTOS_LCDTPrintXY(1,1,"OPEN FILE ERROR");
                CTOS_LCDTPrintXY(1,2,szFileName);
                CTOS_LCDTPrintXY(1,3,&szFileName[19]);
                WaitKey(5);
                return;
            }
        }
		
	}
    
    CTOS_LCDTPrintXY(1,1,"                 ");
    CTOS_LCDTPrintXY(1,2,"                 ");
    CTOS_LCDTPrintXY(1,3,"                 ");
	CTOS_LCDTPrintXY(1,1,"Transfer Size");
	while(1)
	{
		uintRet = fread( temp, 1, sizeof(temp), fPubKey);
		ulSize += uintRet;
		sprintf(szDisplay,"%ld", ulSize);
		CTOS_LCDTPrintXY(1,2,szDisplay);
		if(uintRet > 0)
			CTOS_USBTxData(temp, uintRet);
		if(uintRet <=0)
			break;
		CTOS_Delay(5);
	}
	fclose(fPubKey); 
	CTOS_LCDTPrintXY(1,3,"File Transmitted");
	WaitKey(3); 
    /*if(remove(szFileName1) == -1)
    {
        CTOS_LCDTPrintXY(1,3,"File Delete Error");
   
    }
    else CTOS_LCDTPrintXY(1,3,"File Deleted");
    WaitKey(3);*/
}

void vdDisplayEnvMenuOption(char *pszTag, char *pszValue)
{
	/*display current tag & value*/
	vduiClearBelow(3);
	setLCDPrint(3, DISPLAY_POSITION_LEFT, pszTag);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, pszValue);
		
	/*F1-F4 func display*/
	CTOS_LCDTPrintXY(1, 5, "1 - NEW");
	CTOS_LCDTPrintXY(20-10, 5, "2 - FIND");

	CTOS_LCDTPrintXY(1, 6, "3 - EDIT");
	CTOS_LCDTPrintXY(20-9, 6, "4 - DEL");

	CTOS_LCDTPrintXY(1, 8, "7 - PREV");
	CTOS_LCDTPrintXY(20-9, 8, "9 - NEXT");
}

void vdCTOS_EditEnvParam(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    USHORT usLen;


	unsigned char keyval;
    BOOL isKey;

	char szCurrTag[64+1];
	char szCurrValue[128+1];

	char szNewTag[64+1];
	char szNewValue[128+1];

	int inEnvTotal = 0;
	int inIdx = 0;

	int inFindIdx = 0;
    char szFindTag[64+1];

	/*title*/
    CTOS_LCDTClearDisplay();
    vdDispTitleString("EDIT ENV PARAM");

	//vdDebug_LogPrintf("=====vdCTOS_EditEnvParam=====");
	
	inEnvTotal = inCTOSS_EnvTotal();

	if (0 == inEnvTotal)
		return;
	
	inIdx = 1;

	
	//vdDebug_LogPrintf("inEnvTotal %d", inEnvTotal);

	/*retrive Env data by index*/
	memset(szCurrTag, 0, sizeof(szCurrTag));
	memset(szCurrValue, 0, sizeof(szCurrValue));
	if (inEnvTotal > 0)
	{
		inCTOSS_GetEnvByIdx(inIdx, szCurrTag, szCurrValue);
	}
	else
	{
		strcpy(szCurrTag, "__________");
		strcpy(szCurrValue, "__________");
	}
	//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

	vdDisplayEnvMenuOption(szCurrTag, szCurrValue);

	/*set idle timout*/
	CTOS_TimeOutSet(TIMER_ID_3, 3000);
	
    while(1)
    {
    	/*check timeout*/
		if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return;
        }

		keyval = 0;
		
		/*wait for user*/
		CTOS_KBDInKey(&isKey);
        if (isKey)
		{ //If isKey is TRUE, represent key be pressed //
			vduiLightOn();
            //Get a key from keyboard //
            CTOS_KBDGet(&keyval);

			/*set idle timout agian*/
			CTOS_TimeOutSet(TIMER_ID_3, 3000);
        }

		/*
		#define d_KBD_F1							'X'
		#define d_KBD_F2							'Y'
		#define d_KBD_F3							'I'
		#define d_KBD_F4							'J'
		#define d_KBD_UP							'U'
		#define d_KBD_DOWN							'D'
		#define d_KBD_CANCEL						'C'
		#define d_KBD_CLEAR							'R'
		#define d_KBD_ENTER							'A'
		*/
		switch (keyval)
		{
			case d_KBD_1: //New
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
						
				strcpy(strtemp,"TAG:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				clearLine(8);//thandar
				//CTOS_LCDTPrintXY(1,8,"                 ");//thandar
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewTag, strOut);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						//inCTOSS_PutEnv(szNewTag, szNewValue);
						//break;
					}
					else
					{
					}
				}

				/*new tag not set, break*/
				if (strlen(szNewTag) <= 0)
					break;

				vduiClearBelow(7);
				strcpy(strtemp,"VAL:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				clearLine(8);//thandar
				//CTOS_LCDTPrintXY(1,8,"                 ");//thandar
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewValue, strOut);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						//inCTOSS_PutEnv(szNewTag, szNewValue);
						//break;
					}
					else
					{
					}
				}

				/*new tag not set, break*/
				if (strlen(szNewValue) <= 0)
					break;

				
				//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
				inCTOSS_PutEnv(szNewTag, szNewValue);
				inEnvTotal = inCTOSS_EnvTotal();
				inIdx = inEnvTotal;

				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdx(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				
				break;
				
			case d_KBD_2: //Find
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
						
				strcpy(strtemp,"TAG:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				clearLine(8);//thandar
				//CTOS_LCDTPrintXY(1,8,"                 ");//thandar
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						//strcpy(szNewValue, strOut);
						//strcpy(szNewTag, szCurrTag);
						strcpy(szFindTag, strOut);

						inCTOSS_GetEnvIdx(szFindTag, &inFindIdx);
						//vdDebug_LogPrintf("inIdx %d szFindTag [%s]", inFindIdx, szFindTag);
						inIdx = inFindIdx;					}
					else
					{

					}
				}
				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdx(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_3: //Edit
				
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
				
				strcpy(strtemp,"New:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				clearLine(8);//thandar
				//CTOS_LCDTPrintXY(1,8,"                 ");//thandar
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewValue, strOut);
						strcpy(szNewTag, szCurrTag);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						inCTOSS_PutEnv(szNewTag, szNewValue);
						//break;
					}
					else
					{
						vduiClearBelow(4);
				
						vduiDisplayStringCenter(5,"NO VALUE SET");
						//vduiDisplayStringCenter(7,"PREV[UP] NEXT[DOWN]");
						//vduiDisplayStringCenter(7,"PABX?");
						//vduiDisplayStringCenter(8,"NO[X] YES[OK]");
						//key=struiGetchWithTimeOut();
					}
				}

				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdx(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_4: //Del
				inCTOSS_DelEnv(szCurrTag);
				inEnvTotal = inCTOSS_EnvTotal();
				inIdx = 1;
				/*retrive Env data by index*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
					inCTOSS_GetEnvByIdx(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_7: //prev
				inIdx--;
				if (inIdx < 1)
					inIdx = inEnvTotal;
				/*retrive Env data by index*/
    			memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdx(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_9: //next
				inIdx++;
				if (inIdx > inEnvTotal)
					inIdx = 1;
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdx(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_CANCEL: //exit
				return ;
				//break;
			default:
				break;
		}
	}

}

void vdCTOS_EditEnvParamDB(void)
{
    BYTE bRet,strOut[30],strtemp[17],key;
    BYTE szInputBuf[24+1];
    BYTE szIntComBuf[2];
    BYTE szPhNoBuf[9];
    BYTE szExtNoBuf[4];
    int inResult;
    USHORT ret;
    USHORT usLen;


	unsigned char keyval;
    BOOL isKey;

	char szCurrTag[64+1];
	char szCurrValue[128+1];

	char szNewTag[64+1];
	char szNewValue[128+1];

	int inEnvTotal = 0;
	int inIdx = 0;

	int inFindIdx = 0;
    char szFindTag[64+1];

	/*title*/
    CTOS_LCDTClearDisplay();
    vdDispTitleString("EDIT ENV PARAM");

	vdDebug_LogPrintf("=====vdCTOS_EditEnvParamDB=====");
	
	inEnvTotal = inCTOSS_EnvTotalDB();

	vdDebug_LogPrintf("inCTOSS_EnvTotal inEnvTotal[%d]", inEnvTotal);
	
	//if (0 == inEnvTotal)
		//return;
	
	inIdx = 1;

	/*retrive Env data by index*/
	memset(szCurrTag, 0, sizeof(szCurrTag));
	memset(szCurrValue, 0, sizeof(szCurrValue));
	if (inEnvTotal > 0)
	{
		inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
	}
	else
	{
		strcpy(szCurrTag, "__________");
		strcpy(szCurrValue, "__________");
	}
	//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

	vdDisplayEnvMenuOption(szCurrTag, szCurrValue);

	/*set idle timout*/
	CTOS_TimeOutSet(TIMER_ID_3, 3000);
	
    while(1)
    {
    	/*check timeout*/
		if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return;
        }

		keyval = 0;
		
		/*wait for user*/
		CTOS_KBDInKey(&isKey);
        if (isKey)
		{ //If isKey is TRUE, represent key be pressed //
			vduiLightOn();
            //Get a key from keyboard //
            CTOS_KBDGet(&keyval);

			/*set idle timout agian*/
			CTOS_TimeOutSet(TIMER_ID_3, 3000);
        }

		switch (keyval)
		{
			case d_KBD_1: //New
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
						
				strcpy(strtemp,"TAG:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewTag, strOut);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						//inCTOSS_PutEnv(szNewTag, szNewValue);
						//break;
					}
					else
					{
					}
				}

				/*new tag not set, break*/
				if (strlen(szNewTag) <= 0)
					break;

				vduiClearBelow(7);
				strcpy(strtemp,"VAL:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewValue, strOut);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						//inCTOSS_PutEnv(szNewTag, szNewValue);
						//break;
					}
					else
					{
					}
				}

				/*new tag not set, break*/
				if (strlen(szNewValue) <= 0)
					break;

				
				//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
				inCTOSS_PutEnvDB(szNewTag, szNewValue);
				inEnvTotal = inCTOSS_EnvTotalDB();
				inIdx = inEnvTotal;

				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				
				break;
				
			case d_KBD_2: //Find
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
						
				strcpy(strtemp,"TAG:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						//strcpy(szNewValue, strOut);
						//strcpy(szNewTag, szCurrTag);
						strcpy(szFindTag, strOut);

						inCTOSS_GetEnvIdxDB(szFindTag, &inFindIdx);
						//vdDebug_LogPrintf("inIdx %d szFindTag [%s]", inFindIdx, szFindTag);
						inIdx = inFindIdx;
					}
					else
					{

					}
				}
				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_3: //Edit
				
				memset(szNewTag, 0, sizeof(szNewTag));
				memset(szNewValue, 0, sizeof(szNewValue));
				
				strcpy(strtemp,"New:") ; 
				CTOS_LCDTPrintXY(1, 7, strtemp);
				memset(strOut,0x00, sizeof(strOut));
				memset(strtemp, 0x00, sizeof(strtemp));
				usLen = 20;
				ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
				if(ret==d_KBD_ENTER)
				{
					if(strlen(strOut)>0)
					{
						strcpy(szNewValue, strOut);
						strcpy(szNewTag, szCurrTag);
								
						//vdDebug_LogPrintf("inIdx %d szNewTag [%s] szNewValue [%s]", inIdx, szNewTag, szNewValue);
						inCTOSS_PutEnvDB(szNewTag, szNewValue);
						//break;
					}
					else
					{
						vduiClearBelow(4);
				
						vduiDisplayStringCenter(5,"NO VALUE SET");
						//vduiDisplayStringCenter(7,"PREV[UP] NEXT[DOWN]");
						//vduiDisplayStringCenter(7,"PABX?");
						//vduiDisplayStringCenter(8,"NO[X] YES[OK]");
						//key=struiGetchWithTimeOut();
					}
				}

				/*update current display*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_4: //Del
				inCTOSS_DelEnvDB(szCurrTag);
				inEnvTotal = inCTOSS_EnvTotalDB();
				inIdx = 1;
				/*retrive Env data by index*/
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
					inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_7: //prev
				inIdx--;
				if (inIdx < 1)
					inIdx = inEnvTotal;
				/*retrive Env data by index*/
    			memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_9: //next
				inIdx++;
				if (inIdx > inEnvTotal)
					inIdx = 1;
				memset(szCurrTag, 0, sizeof(szCurrTag));
				memset(szCurrValue, 0, sizeof(szCurrValue));
				if (inEnvTotal > 0)
				{
    				inCTOSS_GetEnvByIdxDB(inIdx, szCurrTag, szCurrValue);
				}
				else
				{
					strcpy(szCurrTag, "__________");
					strcpy(szCurrValue, "__________");
				}
				//vdDebug_LogPrintf("inIdx %d szCurrTag [%s] szCurrValue [%s]", inIdx, szCurrTag, szCurrValue);

				vdDisplayEnvMenuOption(szCurrTag, szCurrValue);
				break;
				
			case d_KBD_CANCEL: //exit
				return ;
				//break;
			default:
				break;
		}
	}

}


/*sidumili: [prompt for password]*/
int inCTOS_PromptPassword(void)
{
	int inRet = d_NO;

	//CTOS_LCDTClearDisplay();
	//vduiClearBelow(7);
	vdCTOS_SetTransType(SETUP);
	vdDispTransTitle(SETUP);

	inRet = inCTOSS_CheckMemoryStatus();
	if(d_OK != inRet)
		return inRet;
	
	inRet = inCTOS_GetTxnPassword();
	if(d_OK != inRet)
	{
		//inCTOS_IdleEventProcess();
		return inRet;
	}

	return d_OK;
}
////for COM&USB communication
int inCTOS_CommsFallback(int shHostIndex)
{
	int inResult = 0;

	vdDebug_LogPrintf("inCTOS_CommsFallback, Host Index [%d]", shHostIndex);
	vdDebug_LogPrintf("srTransRec.usTerminalCommunicationMode [%d]", srTransRec.usTerminalCommunicationMode);
	vdDebug_LogPrintf("strCPT.inCommunicationMode [%d],inSecCommunicationMode=[%d]", strCPT.inCommunicationMode,strCPT.inSecCommunicationMode);

	//if (srTransRec.usTerminalCommunicationMode == DIAL_UP_MODE)
	//	return d_NO;
	if (strCPT.inCommunicationMode == strCPT.inSecCommunicationMode)
		return d_NO;

	vduiClearBelow(3);
	CTOS_LCDTPrintXY (1,6, "Comms Fallback");
	CTOS_LCDTPrintXY (1,7, "Please Wait 	");

	//if ((srTransRec.usTerminalCommunicationMode == ETHERNET_MODE) || (srTransRec.usTerminalCommunicationMode == GPRS_MODE))
	//{
	//	CTOS_LCDTPrintXY(1, 8, "Init Modem... 	");
		srTransRec.usTerminalCommunicationMode = strCPT.inSecCommunicationMode;
	//}
	//else
	//{
	//	CTOS_LCDTPrintXY(1, 8, "Init IP...	  ");
	//	srTransRec.usTerminalCommunicationMode = ETHERNET_MODE;
	//}
	
	if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
	{
		vdDebug_LogPrintf("inCTOS_CommsFallback, COMM INIT ERR");
		vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
		return d_NO;
	}

	inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);

	vdDebug_LogPrintf("vdCTOS_DialBackupConfig, inCTOS_CheckInitComm");

	if (srTransRec.byOffline == CN_FALSE)
	{
		inResult = srCommFuncPoint.inCheckComm(&srTransRec);
	}

	vduiClearBelow(3);
	
	return d_OK;	
}

//format amount 10+2
void vdCTOSS_DisplayAmount(USHORT usX, USHORT usY, char *szCurSymbol,char *szAmount)
{
	int x=0;
	int len, index;

	CTOS_LCDTPrintXY(usX, usY, szCurSymbol);
	x=0;
	len=strlen(szAmount);
	for(index=0; index < len; index++)
	{
	   if(szAmount[index] == '.')
	       x+=1;
	   else
	       x+=2;
	}
	CTOS_LCDTPrintXY(37-x, usY, szAmount);
}


void vdCTOSS_EditTable(void)
{
	BYTE strOut[100];
	BYTE szdatabase[100];
	BYTE sztable[100];
    USHORT usLen;
	USHORT ret;
	char szDispay[50];

	memset(szDispay,0x00,sizeof(szDispay));
	memset(szdatabase,0x00,sizeof(szdatabase));
	memset(sztable,0x00,sizeof(sztable));
	sprintf(szDispay,"EDIT DATABASE");
	CTOS_LCDTClearDisplay();
	
    vdDispTitleString(szDispay);            
	CTOS_LCDTPrintXY(1, 3, "DATABASE NAME:");
	
	usLen = 20;
	CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
    ret = InputStringAlphaEx(1, 7, 0x00, 0x02, szdatabase, &usLen, 1, d_INPUT_TIMEOUT);
	if (strTCT.inThemesType == 1)
	{
		CTOS_LCDTTFSelect("tahoma.ttf", 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
	} else if(strTCT.inThemesType == 7){
            CTOS_LCDTTFSelect(ZAWGYI_FONT, 0);
//		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
	}
	else
	{
		CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);
	}

	if (ret == d_KBD_CANCEL || 0 == ret )
	{
		return ;
	}
	if(ret>= 1)
    {
    	//vduiClearBelow(2);
		CTOS_LCDTClearDisplay();
		vdDispTitleString(szDispay); 
        vdDebug_LogPrintf("szdatabase[%s].usLen=[%d].",szdatabase,usLen);
		CTOS_LCDTPrintXY(1, 3, "TABLE NAME:");
	
		usLen = 20;
		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
	    ret = InputStringAlphaEx(1, 7, 0x00, 0x02, sztable, &usLen, 1, d_INPUT_TIMEOUT);
		if (strTCT.inThemesType == 1)
		{
			CTOS_LCDTTFSelect("tahoma.ttf", 0);
			CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
		} else if(strTCT.inThemesType == 7){
                    CTOS_LCDTTFSelect(ZAWGYI_FONT, 0);
        //		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
		}
		else
		{
			CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
			CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);
		}
		
		if (ret == d_KBD_CANCEL || 0 == ret )
		{
			return ;
		}
		if(ret>= 1)
	    {
	        vdDebug_LogPrintf("sztable[%s].usLen=[%d].szdatabase[%s]",sztable,usLen,szdatabase);
			inCTOSS_EditTalbe(sztable,szdatabase);
	    }
	
    }
	
}

int get_env(char *tag, char *value, int len)
{
	int inRet = 0;
	inRet = inCTOSS_GetEnvDB (tag, value);

	vdDebug_LogPrintf("get_env tag[%s] value[%s] Ret[%d]", tag, value, inRet);
	return inRet;
}

int put_env(char *tag, char *value, int len)
{
	int inRet = 0;
	
	inRet = inCTOSS_PutEnvDB (tag, value);

	vdDebug_LogPrintf("put_env tag[%s] value[%s] Ret[%d]", tag, value, inRet);
	return inRet;
}


int get_env_int (char *tag)
{
	int     ret = -1;	
	//char	  buf[6];
	char	buf[64];//thandar_1June2018_Patrick advice to not make memory overlap if  tag name more than 6 bytes.
	

    memset (buf, 0, sizeof (buf));
    if ( inCTOSS_GetEnvDB (tag, buf) == d_OK )
    {
        ret = atoi (buf);
    }

	vdDebug_LogPrintf("get_env_int [%s]=[%d]", tag, ret);

    return ret;
}

void put_env_int(char *tag, int value)
{
	int     ret = -1;
	//char    buf[6];
	char    buf[64];//thandar_1June2018_Patrick advice to not make memory overlap if  tag name more than 6 bytes.

    memset (buf, 0, sizeof (buf));
    //int2str (buf, value);
    sprintf(buf, "%d", value);
    ret = inCTOSS_PutEnvDB (tag, buf);

	vdDebug_LogPrintf("put_env_int [%s]=[%d]", tag, ret);
}

#ifdef UNUSE
#define CALC_BDK2	"\xC0\xC0\xC0\xC0\x00\x00\x00\x00\xC0\xC0\xC0\xC0\x00\x00\x00\x00"
#define CALC_KSN2	"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xE0"
void vdCTOSS_DUKPTCalcIPEK(char *szBDK,char *szKSN,char *szIPEK)
{
	unsigned char szBDK2[16+1];
	unsigned char sztmpIPEK[16+1];
	unsigned char sztmpKSN[8+1];
	

	memset(szBDK2,0x00,sizeof(szBDK2));
	memset(sztmpIPEK,0x00,sizeof(sztmpIPEK));
	memset(sztmpKSN,0x00,sizeof(sztmpKSN));

	memcpy(szBDK2,szBDK,16);
	XOR(szBDK2,CALC_BDK2,16);
	vdPCIDebug_HexPrintf("szBDK2",szBDK2,16);

	memcpy(sztmpKSN,szKSN,8);
	AND(sztmpKSN,CALC_KSN2,8);
	vdPCIDebug_HexPrintf("sztmpKSN",sztmpKSN,8);

	TripleDes_16Key(szBDK,sztmpKSN,sztmpIPEK,'E');
	vdPCIDebug_HexPrintf("szIPEK",sztmpIPEK,8);
	TripleDes_16Key(szBDK2,sztmpKSN,&sztmpIPEK[8],'E');
	vdPCIDebug_HexPrintf("szIPEK",&sztmpIPEK[8],8);

	vdPCIDebug_HexPrintf("szIPEK",sztmpIPEK,16);
	memcpy(szIPEK,sztmpIPEK,16);
	
}
#endif

void vdCTOS_TMSSetting(void)
{
	vdCTOS_InputTMSSetting();
}

void vdCTOS_TMSReSet(void)
{
	vdCTOS_InputTMSReSet();
}

void vdCTOS_TMSUploadFile(void)
{
	VdCTOSS_TMSUpload();
}



int  inCTOS_TMSPreConfigSetting(void)
{
	BYTE strOut[30],strtemp[17];
	BYTE szInputBuf[5];
    int inResult;
    int ret;
	USHORT usLen;

    CTOS_LCDTClearDisplay();
    vdDispTitleString("TMS SETTINGS");

    inResult = inTCTRead(1);
    if(inResult != d_OK)
        return d_NO;	
	
    while(1)
    {   
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMS COMM MODE");
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "0-DIALUP 1-ETH");
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "2-GPRS 4-WIFI");
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "%d", strTCT.inTMSComMode);
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
        
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
		ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
		
		//ret = InputStringAlpha(1, 8, 0x00, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
		//ret = InputStringAlphaEx2(1, 4, 0x06, 0x02, strOut, &usLen, 0, d_INPUT_TIMEOUT);
		
        if (ret == d_KBD_CANCEL )
            return d_NO;
        else if(0 == ret )
            return d_NO;
        else if(ret>=1)
        {
			if(strOut[0] == 0x30 || strOut[0] == 0x31 || strOut[0] == 0x32 || strOut[0] == 0x34)
			{
                strTCT.inTMSComMode=atoi(strOut);      
                vdMyEZLib_LogPrintf("strTCT.usTMSGap %d",strTCT.inTMSComMode);
			    inResult = inTCTSave(1);
                break;				
			}
            else
                vdDisplayErrorMsg(1, 8, "INVALID INPUT");
        }   
    }	
	
    return d_OK;
}

static int firsttime = 0;
int inCTOSS_PassData(void)
{
	BYTE inRet,inRetVal;
	int inResult;
	int inSendLen, inReceLen;
    BYTE uszSendData[2048 + 1], uszReceData[2048 + 1];

	memset(&srTransRec,0x00,sizeof(TRANS_DATA_TABLE));
	memset(uszSendData,0x00,sizeof(uszSendData));
	memset(uszReceData,0x00,sizeof(uszReceData));

	CTOS_LCDTClearDisplay();
    vdDispTitleString("PASSTHROUGH");
	
	inRet = inCTOS_MultiAPGetData();
    if(d_OK != inRet)
        return inRet;

#ifdef VERSION2
	vdDebug_LogPrintf("inCommunicationMode=[%d]",srTransRec.usTrack1Len);
	vdDebug_LogPrintf("szPriTxnHostIP=[%s]",srTransRec.szTrack1Data);
	vdDebug_LogPrintf("szSecTxnHostIP=[%s]",srTransRec.szTrack2Data);
	vdDebug_LogPrintf("inPriTxnHostPortNum=[%d]",srTransRec.ulTraceNum);
	vdDebug_LogPrintf("inSecTxnHostPortNum=[%d]",srTransRec.ulOrgTraceNum);
	vdDebug_LogPrintf("inLen=[%d]",srTransRec.usTrack2Len);
#endif	
	vdDebug_LogPrintf("usAdditionalDataLen=[%d],usChipDataLen=[%d]", srTransRec.usAdditionalDataLen,srTransRec.usChipDataLen);

	if (srTransRec.usAdditionalDataLen > 0)
	{
		memcpy(uszSendData,srTransRec.baChipData,1024);

		memcpy(&uszSendData[1024],srTransRec.baAdditionalData,srTransRec.usAdditionalDataLen);
		inSendLen = 1024 + srTransRec.usAdditionalDataLen;
	}
	else
	{
		inSendLen = srTransRec.usChipDataLen;
		memcpy(uszSendData,srTransRec.baChipData,inSendLen);
	}

	inCPTRead(1);
#ifdef VERSION2	
	strCPT.inCommunicationMode = srTransRec.usTrack1Len;
	strCPT.inPriTxnHostPortNum = srTransRec.ulTraceNum;
	strCPT.inSecTxnHostPortNum = srTransRec.ulOrgTraceNum;
	strcpy(strCPT.szPriTxnHostIP,srTransRec.szTrack1Data);
	strcpy(strCPT.szSecTxnHostIP,srTransRec.szTrack2Data);
#endif	
	if (inCTOS_InitComm(strCPT.inCommunicationMode) != d_OK) 
    {
        vdDisplayErrorMsg(1,8,"COMM INIT ERR");
        return(d_NO);
	}
	inRetVal = inCTOS_CheckInitComm(strCPT.inCommunicationMode); 
	if (inRetVal != d_OK)
	{
		if (strCPT.inCommunicationMode == GPRS_MODE)
			vdDisplayErrorMsg(1,8,"GPRS NOT ESTABLISHED");
		else
        vdDisplayErrorMsg(1,8,"COMM INIT ERR");
        return(d_NO);
    }
	if (srCommFuncPoint.inCheckComm(&srTransRec) != d_OK)
	{
		inCTOS_inDisconnect();
		return(d_NO);
	}

	if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
	{
		inCTOS_inDisconnect();
		return ST_ERROR;
	}

	vdPCIDebug_HexPrintf("uszSendData", uszSendData, inSendLen);
	srCommFuncPoint.inSendData(&srTransRec,uszSendData,inSendLen);

	inReceLen = srCommFuncPoint.inRecData(&srTransRec,uszReceData);

	vdDebug_LogPrintf("inRecData=[%d]", inReceLen);
	vdPCIDebug_HexPrintf("inRecData", uszReceData, inReceLen);
	memset(&srTransRec,0x00,sizeof(TRANS_DATA_TABLE));
	if (inReceLen <= 1024)
	{
		srTransRec.usChipDataLen = inReceLen;
		memcpy(srTransRec.baChipData,uszReceData,inReceLen);
	}
	else
	{
		srTransRec.usChipDataLen = 1024;
		memcpy(srTransRec.baChipData,uszReceData,1024);

		srTransRec.usAdditionalDataLen = inReceLen-1024;
		memcpy(srTransRec.baAdditionalData,&uszReceData[1024],srTransRec.usAdditionalDataLen);
	}
	inRet = inMultiAP_Database_BatchInsert(&srTransRec);
	vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", inRet);
	if(d_OK != inRet)
	{
		vdDisplayErrorMsg(1,8,"MultiAP BatchInsert ERR");
	}
				
//	inCTOS_inDisconnect();
	CTOS_LCDTClearDisplay();
}



int inCTOSS_PassDataSend(void)
{
	ULONG tick,offset;
	USHORT ret,inRet,bret;
    unsigned char ucTemp;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	int inLen = 232;
	BYTE uszData[2048];
	BYTE uszSendData[2048] = "600004000002003020078020C0020400000000000000011100000900500002000400355264710003795113D190722100000668000F303930303030313134303130303038383838383030303101465F2A0207025F340102820238008407A0000000041010950500000080009A031501279C01009F02060000000001119F03060000000000009F090200029F10120110A08003220000E01D00000000000000FF9F1A0207029F1E0831323334353637389F26082CFD08188B5964639F2701809F3303E0B0C89F34031E03009F3501229F360201CC9F3704D7422F239F41030000010006303030303038";

	vdDebug_LogPrintf("inCTOSS_PassData");
	wub_str_2_hex(uszSendData,uszData,inLen*2);
	memset(&srTransRec,0x00,sizeof(TRANS_DATA_TABLE));
	inCPTRead(1);
	
	{
		srTransRec.usTrack1Len = strCPT.inCommunicationMode;
		srTransRec.usTrack2Len = inLen;
		strcpy(srTransRec.szTrack1Data,strCPT.szPriTxnHostIP);
		strcpy(srTransRec.szTrack2Data,strCPT.szSecTxnHostIP);
		srTransRec.ulTraceNum = strCPT.inPriTxnHostPortNum;
		srTransRec.ulOrgTraceNum = strCPT.inSecTxnHostPortNum;
		if (inLen <= 1024)
		{
			srTransRec.usChipDataLen = inLen;
			memcpy(srTransRec.baChipData,uszData,inLen);
		}
		else
		{
			srTransRec.usChipDataLen = 1024;
			memcpy(srTransRec.baChipData,uszData,1024);

			srTransRec.usAdditionalDataLen = inLen-1024;
			memcpy(srTransRec.baAdditionalData,&uszData[1024],srTransRec.usAdditionalDataLen);
		}
		
		
		inTCTSave(1);
		
		bret= inMultiAP_Database_BatchDelete();
		vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchDelete ERR");
			return bret;
		}
		
		bret = inMultiAP_Database_BatchInsert(&srTransRec);
		vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchInsert ERR");
			return bret;
		}
	
	
		vdDebug_LogPrintf("szAPName[%s],bret=[%d]", strHDT.szAPName,bret);
		
		inMultiAP_RunIPCCmdTypes("V5S_Mandiri",d_IPC_CMD_PASSDATA,"",0, outbuf,&out_len);
	
		inTCTRead(1);


		memset(&srTransRec,0x00,sizeof(TRANS_DATA_TABLE));
		offset = 0;
		memset(uszData,0x00,sizeof(uszData));
		
        inRet = inCTOS_MultiAPGetData();
		if(d_OK != inRet)
		{
			return inRet;
		}

		if (srTransRec.usAdditionalDataLen > 0)
		{
			memcpy(uszData,srTransRec.baChipData,1024);

			memcpy(&uszData[1024],srTransRec.baAdditionalData,srTransRec.usAdditionalDataLen);
			offset = 1024 + srTransRec.usAdditionalDataLen;
		}
		else
		{
			offset = srTransRec.usChipDataLen;
			memcpy(uszData,srTransRec.baChipData,offset);
		}
	}
        return d_OK;
}


int inCTOSS_SendData(void)
{
	BYTE inRet,inRetVal;
	int inResult;
	int inReceLen;
    BYTE uszReceData[2048 + 1];
	
	BYTE uszData[2048];
	BYTE uszSendData[2048] = "\x60\x00\x01\x00\x00\x50\x4F\x53\x54\x20\x2F\x74\x6D\x73\x5F\x32\x2F\x54\x4D\x53\x32\x5F\x4E\x61\x63\x5F\x43\x6F\x6E\x6E\x65\x63\x74\x2E\x70\x68\x70\x20\x48\x54\x54\x50\x2F\x31\x2E\x31\x0D\x0A\x41\x63\x63\x65\x70\x74\x3A\x20\x2A\x2F\x2A\x0D\x0A\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x54\x79\x70\x65\x3A\x20\x61\x70\x70\x6C\x69\x63\x61\x74\x69\x6F\x6E\x2F\x78\x2D\x77\x77\x77\x2D\x66\x6F\x72\x6D\x2D\x75\x72\x6C\x65\x6E\x63\x6F\x64\x65\x64\x0D\x0A\x48\x6F\x73\x74\x3A\x20\x0D\x0A\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x4C\x65\x6E\x67\x74\x68\x3A\x20\x35\x30\x0D\x0A\x43\x6F\x6E\x6E\x65\x63\x74\x69\x6F\x6E\x3A\x20\x4B\x65\x65\x70\x2D\x41\x6C\x69\x76\x65\x0D\x0A\x0D\x0A\x56\x49\x44\x3D\x41\x67\x3D\x3D\x26\x53\x4E\x3D\x4D\x54\x49\x7A\x4E\x44\x55\x32\x4E\x7A\x67\x35\x4D\x44\x45\x79\x4D\x7A\x51\x31\x4E\x67\x3D\x3D\x26\x54\x50\x44\x55\x3D\x41\x47\x41\x41\x41\x51\x41\x41";
	int inSendLen = 214;

	vdDebug_LogPrintf("inCTOSS_SendData");

	memset(&srTransRec,0x00,sizeof(TRANS_DATA_TABLE));
	memset(uszReceData,0x00,sizeof(uszReceData));

	CTOS_LCDTClearDisplay();
    vdDispTitleString("Send Data");

	inCPTRead(1);
	if (inCTOS_InitComm(strCPT.inCommunicationMode) != d_OK) 
    {
        vdDisplayErrorMsg(1,8,"COMM INIT ERR");
        return(d_NO);
	}
	inRetVal = inCTOS_CheckInitComm(strCPT.inCommunicationMode); 
	if (inRetVal != d_OK)
	{
		if (strCPT.inCommunicationMode == GPRS_MODE)
			vdDisplayErrorMsg(1,8,"GPRS NOT ESTABLISHED");
		else
        vdDisplayErrorMsg(1,8,"COMM INIT ERR");
        return(d_NO);
    }
	if (srCommFuncPoint.inCheckComm(&srTransRec) != d_OK)
	{
		inCTOS_inDisconnect();
		return(d_NO);
	}

	if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
	{
		inCTOS_inDisconnect();
		return ST_ERROR;
	}

	vdPCIDebug_HexPrintf("uszSendData", uszSendData, inSendLen);
	srCommFuncPoint.inSendData(&srTransRec,uszSendData,inSendLen);

	inReceLen = srCommFuncPoint.inRecData(&srTransRec,uszReceData);

	vdDebug_LogPrintf("inRecData=[%d]", inReceLen);
	vdPCIDebug_HexPrintf("inRecData", uszReceData, inReceLen);
				
	inCTOS_inDisconnect();
	CTOS_LCDTClearDisplay();
}


int isCheckTerminalMP200(void)
{
	 
	// vdDebug_LogPrintf("isCheckTerminalMP200 byTerminalType[%d]", strTCT.byTerminalType );
	 
	 
	if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
		return d_OK;

	return d_NO;
}

int isCheckTerminalUPT1000(void)
{
	if ((strTCT.byTerminalType == 7) || (strTCT.byTerminalType == 8))
		return d_OK;

	return d_NO;
}

#if 0
void vdSystem_InjectKey(void)
{
	USHORT ushRet = d_OK;

	ushRet = CTOS_KeyInjectionPerform(NULL, NULL);

	vdDebug_LogPrintf("CTOS_KeyInjectionPerform ushRet=%d", ushRet);
}
#endif


/*Edit for CBB*/

void vdCBB_TMKRefNumSetting(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTMKRefNum[8+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;


    CTOS_LCDTClearDisplay();
    vdDispTitleString("TMK REF. NUM");

	memset(szTMKRefNum, 0x00, sizeof (szTMKRefNum));
	get_env("#TMKREF", szTMKRefNum, 8);
		
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TMK REF. NUM");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTMKRefNum);
    
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 8, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if(ret==8)
        {
        	put_env("#TMKREF", strOut, 8);
			break;
       	}
   		if (ret == d_KBD_CANCEL)
        	break ;
    }
            
	return ;
}

void vdIPP_TMKRefNumSetting(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTMKRefNum[8+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;


    CTOS_LCDTClearDisplay();
    vdDispTitleString("TMK REF. NUM");

	memset(szTMKRefNum, 0x00, sizeof (szTMKRefNum));
	get_env("#IPPTMKREF", szTMKRefNum, 8);
		
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "IPP TMK REF. NUM");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTMKRefNum);
    
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 8, 8, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if(ret==8)
        {
        	put_env("#IPPTMKREF", strOut, 8);
			break;
       	}
   		if (ret == d_KBD_CANCEL)
        	break ;
    }
            
	return ;
}


/*set retail or Cash Adv only*/
void vdCBB_SetTerminalMode(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
    //int inResult = 0;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;

	int inOrgMenuid;

	vdDebug_LogPrintf("vdCBB_SetTerminalMode");
	return d_OK;

    inTCTGetCurrMenuid(1, &inOrgMenuid);
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("SETTING");
    while(1)
    {
        clearLine(3);
        clearLine(4);
        clearLine(5);
        clearLine(6);
        clearLine(7);
        clearLine(8);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TERMINAL MODE");
        if(inOrgMenuid == 3)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "3");
        if(inOrgMenuid == 4)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "4");
        
//        CTOS_LCDTPrintXY(1, 5, "3-RETAIL     4-CASH ADV");
        CTOS_LCDTPrintXY(1, 5, "4-CASH ADV   5-NEW ENH.");
		CTOS_LCDTPrintXY(1, 6, "3-IPP");

        
   
        strcpy(strtemp,"New:") ;
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==1)
        {
            //check host num
            inNumOfHost = inHDTNumRecord();
            
            vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
            for(inNum =1 ;inNum <= inNumOfHost; inNum++)
            {
                if(inHDTRead(inNum) == d_OK)
                {
                    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
                
                    vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
                    for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
                    {
                        if((inResult = inMMTReadRecord(strHDT.inHostIndex,strMMT[inLoop-1].MITid)) !=d_OK)
                        {
                            vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex,inResult);
                            continue;
                            //break;
                        }
                        else    // delete batch where hostid and mmtid is match  
                        {
                            strMMT[0].HDTid = strHDT.inHostIndex;
                            strMMT[0].MITid = strMMT[inLoop-1].MITid;
                            inDatabase_BatchDeleteHDTidMITid();
                            vdDebug_LogPrintf("[inDatabase_BatchDelete]-Mitid[%d]strHDT.inHostIndex[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex);
                            
                            memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
                            memset(&strFile,0,sizeof(strFile));
                            srTransRec.MITid = strMMT[inLoop-1].MITid;
                            vdCTOS_GetAccumName(&strFile, &srAccumRec);
                            
                            if((inResult = CTOS_FileDelete(strFile.szFileName)) != d_OK)
                            {
                                vdDebug_LogPrintf("[inMyFile_SettleRecordDelete]---Delete Record error[%04x]", inResult);
                            }

                            inCTOS_DeleteBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);

                            inMyFile_ReversalDelete();

                            inMyFile_AdviceDelete();
                            
                            inMyFile_TCUploadDelete();

                        }
                    }
                }
                else
                    continue;

            }
            
//            if (strOut[0]==0x33 || strOut[0]==0x34)
           // if (strOut[0]==0x34 || strOut[0]==0x35)
			 if (strOut[0]==0x33 || strOut[0]==0x34 || strOut[0]==0x35)
            {
            	if(strOut[0] == 0x33)
                 {
					inTCTUpdateMenuid(1, IPP_MENU);
					
					if (inOrgMenuid != IPP_MENU)
					{
						vduiWarningSound();
						CTOS_LCDTPrintXY(1, 7, "Restart Device");
						CTOS_Delay(2000);
						CTOS_SystemReset();
					}
                 }
				
                 if(strOut[0] == 0x34)
                 {
					inTCTUpdateMenuid(1, CASH_ADV_APP_MENU);
					
					if (inOrgMenuid != CASH_ADV_APP_MENU)
					{
						vduiWarningSound();
						CTOS_LCDTPrintXY(1, 7, "Restart Device");
						CTOS_Delay(2000);
						CTOS_SystemReset();
					}
                 }
                 if(strOut[0] == 0x35)
                 {
					inTCTUpdateMenuid(1, NEW_ENHANCEMENT_MENU);
					if (inOrgMenuid != NEW_ENHANCEMENT_MENU)
					{
						vduiWarningSound();
						CTOS_LCDTPrintXY(1, 7, "Restart Device");
						CTOS_Delay(2000);
						CTOS_SystemReset();
					}
                 }

                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"TERMINAL MODE");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}

//@@IBR ADD 20170116
// EDIT CURRENCY FROM SETUP > CURRENCY MENU.
void vdCTOS_TermSelectCurrency(void)
{
	int		inRet = d_OK;
  	BYTE 	key;
        char    szStrCurrSymbol[4+1];
		char    szStrCurrSymbol2[4+1];
        BYTE    szEMVCurSymbol[2+1];
        BYTE    szEMVCurSymbol2[2+1];
		
        int     inCurrCode = 0;
        int ret = 0;
        USHORT usLen;
        char szInput[2+1];
        int inCurr = 0;
        int inNum = 0;

		vdDebug_LogPrintf("vdCTOS_TermSelectCurrency strHDT.inHostIndex[%d] strHDT.inCurrencyIdx[%d]strCST.szCurCode[%s] strCST.inCurrencyIndex[%d]",
		strHDT.inHostIndex, strHDT.inCurrencyIdx, strCST.szCurCode,  strCST.inCurrencyIndex);
		
        CTOS_LCDTClearDisplay();
        
        inNum = inBatchNumRecord();
        
        if(inNum > 0){
            setLCDPrint(8, DISPLAY_POSITION_LEFT, "BATCH NOT EMPTY");
            vduiWarningSound();
            CTOS_Delay(1500);
            return;
        }
        
        CTOS_LCDTClearDisplay();


#if 0
        //inHDTRead(21);       
        inHDTRead(17);
#else
	if(strCST.inCurrencyIndex == 2)
		strHDT.inCurrencyIdx = 2;//inHDTRead(22);
	else
		strHDT.inCurrencyIdx = 1;//inHDTRead(23);
#endif		
        
        vdDispTitleString("CURRENCY");
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "DEFAULT CURRENCY");
        inCSTRead(strHDT.inCurrencyIdx);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
        
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "1-MMK         2-USD");
        setLCDPrint(7, DISPLAY_POSITION_LEFT, "INPUT NEW:");
//        setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY TO CONT.");
        memset(szInput, 0x00, sizeof(szInput));
        ret= shCTOS_GetNum(8, 0x01,  szInput, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
//        CTOS_KBDGet(&key);
        
        inCurr = atoi(szInput);
//        key = 0;

        vdDebug_LogPrintf("invdCTOS_TermSelectCurrency inCurr = %d ret = %d", inCurr, ret);

//just making sure currency code (CURRDEFVAL) value rides with currenct change option.
// to fix CURRDEFVAL value, always zero on cancelled input...
#ifdef CBB_FIN_ROUTING
	if(ret == 1)
	put_env_int("#CURRDEFVAL",inCurr);
#endif
		// commented, host 23 (MMK) will be disabled. terminal will return "host selection error".
		// for testing
		//#ifdef CBB_FIN_ROUTING
		#if 0
		if(inCurr ==  1){
			inHDTUpdateHostEnable(22, FALSE);	// for SIGNON - ALL feature. should noT include FIN-USD IF selected CURRENCY is MMK.
			inHDTUpdateHostEnable(23, TRUE);    //FIN-MMK
		}
		else
		{
			inHDTUpdateHostEnable(22, TRUE);	//FIN-USD
			inHDTUpdateHostEnable(23, FALSE);
	//FIN-MMK
		}
        #endif

	//inHDTUpdateHostCurr(inHostIndex, strCST.inCurrencyIndex);
	
		//	inCurr
		// 1 >> MMK
		// 2 >> USD
		
        inRet = inHDTCurrencyUpdate(inCurr);

		
        inRet = inCSTRead(inCurr);
        inCurrCode = atoi(strCST.szCurCode);

		
        vdDebug_LogPrintf("invdCTOS_TermSelectCurrency inCurrCode = %d", inCurrCode);

        memset(szStrCurrSymbol, 0x00, sizeof(szStrCurrSymbol));
        memset(szStrCurrSymbol2, 0x00, sizeof(szStrCurrSymbol2));
		
        memset(szEMVCurSymbol, 0x00, sizeof(szEMVCurSymbol));
        memset(szEMVCurSymbol2, 0x00, sizeof(szEMVCurSymbol2));
		
        sprintf(szStrCurrSymbol, "%04d", inCurrCode);
        wub_str_2_hex(szStrCurrSymbol, szEMVCurSymbol, 4);

	
		// fix for issues raised by FINEXUS dated 03052020 #1
		strcpy(szStrCurrSymbol2, "0104");
        wub_str_2_hex(szStrCurrSymbol2, szEMVCurSymbol2, 4);

		// fix for issues raised by FINEXUS dated 03052020 #2
        //memcpy(strEMVT.szEMVTermCountryCode, szEMVCurSymbol, 2); - original code
        memcpy(strEMVT.szEMVTermCountryCode, szEMVCurSymbol2, 2); // make it always 0104 for the country code       
        memcpy(strEMVT.szEMVTermCurrencyCode, szEMVCurSymbol, 2);
        
        inRet = inEMVUpdateCurrency();

		
        inRet = inDatabase_WaveUpdate("MASTERCARD","5F2A", szStrCurrSymbol);		
        inRet = inDatabase_WaveUpdate("MASTERCARD","9F1A", "0104");		// fix for MC issue 03-02-2022
		
        inRet = inDatabase_WaveUpdate("VISA","5F2A", szStrCurrSymbol);
        inRet = inDatabase_WaveUpdate("VISA","9F1A", "0104");		

		
		inRet = inDatabase_WaveUpdate("JCB","5F2A", szStrCurrSymbol); // fix for void/ sale incorrect 5f2a value if currency is USD, still MMK
		inRet = inDatabase_WaveUpdate("JCB","9F1A", "0104"); // fix for void/ sale incorrect 9F1A value if Country Code should be MMK

		// fix for issues raised by FINEXUS dated 03052020 #3
		inRet = inDatabase_WaveUpdate("CUP","5F2A", szStrCurrSymbol);
		inRet = inDatabase_WaveUpdate("CUP","9F1A", "0104");
		
        vdDebug_LogPrintf("inRet EMV = %d", inRet);

		vdCTOS_PartialInitWaveData();//thandar_added in 23Apr2018
        
	return;
}


//DEFAULT CURRENCY DISPLAY FROM IDLE
void vdCTOS_TermSelectCurrencyMenu(void) {
    int inRet = d_OK;
    BYTE key;
    char szStrCurrSymbol[4 + 1];
    BYTE szEMVCurSymbol[2 + 1];
    int inCurrCode = 0;
	BYTE	szEMVCurSymbol2[2+1];
	char	szStrCurrSymbol2[4+1];

	// The initial value should not be change via ENV var menu if  app was already initialized, else need to retart the terminal for the value to take on effect.
	int inGetHDTDefCurrVal  = get_env_int("#CURRDEFVAL");

    CTOS_LCDTClearDisplay();


vdDebug_LogPrintf("vdCTOS_TermSelectCurrencyMenu inGetHDTDefCurrVal [%d]!!!", inGetHDTDefCurrVal);

#if 0
    //inHDTRead(21);
    inHDTRead(17);
#else
	
	if(inGetHDTDefCurrVal == 2)
		strHDT.inCurrencyIdx = 2;//inHDTRead(22);
	else
		strHDT.inCurrencyIdx = 1;//inHDTRead(23);
#endif

	vdDebug_LogPrintf("vdCTOS_TermSelectCurrencyMenu!!! strHDT.inCurrencyIdx [%d]", strHDT.inCurrencyIdx);

    vdDispTitleString("CURRENCY");
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "DEFAULT CURRENCY");
    inCSTRead(strHDT.inCurrencyIdx);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRESS ANY KEY TO CONT.");


    inRet = inHDTCurrencyUpdate(strHDT.inCurrencyIdx);

    inCurrCode = atoi(strCST.szCurCode);

	vdDebug_LogPrintf("vdCTOS_TermSelectCurrencyMenu!!! inCurrCode [%d]", inCurrCode);

// commented, host 23 (MMK) will be disabled. terminal will return "host selection error".
// for testing
//#ifdef CBB_FIN_ROUTING
#if 0
	if(inCurrCode ==  104){
		inHDTUpdateHostEnable(22, FALSE);	// for SIGNON - ALL feature. should noT include FIN-USD IF selected CURRENCY is MMK.
		inHDTUpdateHostEnable(23, TRUE);	//FIN-MMK
	}
	else
	{
		inHDTUpdateHostEnable(22, TRUE);	//FIN-USD
		inHDTUpdateHostEnable(23, FALSE);
	//FIN-MMK
	}
#endif

	memset(szStrCurrSymbol, 0x00, sizeof (szStrCurrSymbol));
    memset(szEMVCurSymbol, 0x00, sizeof (szEMVCurSymbol));
    sprintf(szStrCurrSymbol, "%04d", inCurrCode);
    wub_str_2_hex(szStrCurrSymbol, szEMVCurSymbol, 4);

	// fix for issues raised by FINEXUS dated 03052020 #1
	
	memset(szEMVCurSymbol2, 0x00, sizeof(szEMVCurSymbol2));	
	memset(szStrCurrSymbol2, 0x00, sizeof(szStrCurrSymbol2));
	
	strcpy(szStrCurrSymbol2, "0104");
	wub_str_2_hex(szStrCurrSymbol2, szEMVCurSymbol2, 4);


    memcpy(strEMVT.szEMVTermCountryCode, szEMVCurSymbol2, 2);
    memcpy(strEMVT.szEMVTermCurrencyCode, szEMVCurSymbol, 2);

    inRet = inEMVUpdateCurrency();
    inRet = inDatabase_WaveUpdate("MASTERCARD", "5F2A", szStrCurrSymbol);
	inRet = inDatabase_WaveUpdate("MASTERCARD","9F1A", "0104"); 	// fix for MC issue 03-02-2022
	
    inRet = inDatabase_WaveUpdate("VISA", "5F2A", szStrCurrSymbol);
	inRet = inDatabase_WaveUpdate("VISA","9F1A", "0104"); 	// fix for MC issue 03-02-2022

	inRet = inDatabase_WaveUpdate("JCB","5F2A", szStrCurrSymbol); // fix for void/ sale incorrect 5f2a value if currency is USD, still MMK
	inRet = inDatabase_WaveUpdate("JCB","9F1A", "0104"); // fix for void/ sale incorrect 9F1A value if Country Code should be MMK
	
    vdDebug_LogPrintf("inRet EMV = %d", inRet);

//    if (strcmp(strCST.szCurCode, "104") == 0)
//        put_env("MAINCURR", "1", 1);
//    else
//        put_env("MAINCURR", "2", 1);

    CTOS_KBDGet(&key);

    key = 0;

    CTOS_LCDTClearDisplay();

    return;
}

void vdCTOS_MPUOperSetting(void){
    char szMPUOper[6+1];
    char szInput[6+1];
    USHORT usLen;
    int ret;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("MPU OPERATOR");

	memset(szMPUOper, 0x00, sizeof (szMPUOper));
	get_env("#MPUOPER", szMPUOper, 6);
		
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "MPU OPERATOR");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szMPUOper);
        
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(szInput,0x00, sizeof(szInput));
        ret= shCTOS_GetNum(8, 0x01,  szInput, &usLen, 6, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if(ret==8)
        {
        	put_env("#MPUOPER", szInput, 6);
			break;
       	}
   		if (ret == d_KBD_CANCEL)
        	break ;
    }
            
	return ;
    
}

//@@IBR ADD 20170202
void vdCTOS_EnableManualEntry(void){
    char szDisp[2+1];
    char szInput[2+1];
    USHORT usLen;
    int ret;
    int inGetEnv;
    
    CTOS_LCDTClearDisplay();
    vdDispTitleString("MANUAL ENTRY SETTING");

	memset(szDisp, 0x00, sizeof (szDisp));
	get_env("#MANENTRY", szDisp, 1);
//        inGetEnv = get_env_int("MANENTRY");
		
    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "MANUAL ENTRY");
        
        if(szDisp[0] == 0x31){
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "ENABLE");
        } else if(szDisp[0] == 0x30){
            setLCDPrint(3, DISPLAY_POSITION_LEFT, "DISABLE");
        }
        
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "0-DISABLE   1-ENABLE");
        
        CTOS_LCDTPrintXY(1, 7, "New:");
        memset(szInput,0x00, sizeof(szInput));
        ret= shCTOS_GetNum(8, 0x01,  szInput, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if(ret==1)
        {
        	put_env("#MANENTRY", szInput, 1);
//            if(szInput[0] == 0x31)
//                put_env_int("MANENTRY", 1);
//            else if(szInput[0] == 0x30)
//                put_env_int("MANENTRY", 0);
//            else{
//                continue;
//            }
			break;
       	}
   		if (ret == d_KBD_CANCEL)
        	break ;
    }
            
	return ;
    
}


void vdCTOSS_ModifyStanNumber(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szStanNo[8+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;


    CTOS_LCDTClearDisplay();
    vdDispTitleString("Modify STAN");

	inHDTRead(1);
	memset(szStanNo, 0x00, sizeof (szStanNo));
	wub_hex_2_str(strHDT.szTraceNo,szStanNo,3);
		
    while(1)
    {
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Original STAN ");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szStanNo);
    
        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 6, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if(ret==6)
        {
        	vdDebug_LogPrintf("strOut=[%s]",strOut);
			wub_str_2_hex(strOut,strHDT.szTraceNo,6);
			inHDTUpdateTraceNum();
			break;
       	}
   		if (ret == d_KBD_CANCEL)
        	break ;
    }
            
	return ;
}


void vdCTOS_SetExchangeRate(void){
    char szRateDisp[d_LINE_SIZE+1];
    BYTE strOut[30];
    USHORT usLen;
    int ret;
    
    CTOS_LCDTClearDisplay();
    
    inTCTRead(1);
    
    vdDispTitleString("EXCHANGE RATE");
    
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "IN 1 USD");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, "CURRENT RATE");
    
    memset(szRateDisp, 0x00, sizeof(szRateDisp));
    sprintf(szRateDisp, "MMK %d", strTCT.inExchangeRate);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, szRateDisp);
    
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
    
    while(1)
    {
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 12, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if (ret >= 1) {
            strTCT.inExchangeRate = atoi(strOut);
            inTCTSave(1);
            break;
        }
    }
    
    
    
    return;
    
}

#if 1
void vdCTOS_SetAutoSettleInterval(void)
{
    char szRateDisp[d_LINE_SIZE+1];
    char szTemp[4+1];
    BYTE strOut[2+1];
    USHORT usLen;
    int ret;
    
    CTOS_LCDTClearDisplay();
    
    //inTCTRead(1);
    inPASRead(1);
	
    vdDispTitleString("AUTO SETTLE SET");
    
    while(1)
    {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "00:00-23:59");
        setLCDPrint(4, DISPLAY_POSITION_LEFT, "CURRENT AUTO SETTLE");
        
        memset(szRateDisp, 0x00, sizeof(szRateDisp));
        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp, "%s", strPAS.szSTLTime1);
        memcpy(szRateDisp, szTemp, 2);
        strcat(szRateDisp, ":");
        memcpy(szRateDisp+3, szTemp+2, 2);
        strcat(szRateDisp, ":");
        strcat(szRateDisp, szTemp+4);
        //	  sprintf(szRateDisp, "%d HOUR", strTCT.inAutoSettleIntervalHour);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szRateDisp);
        
        setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
        usLen=6;

        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 6, 6, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if (ret >= 0 && ret < 240000) {
//            strTCT.inAutoSettleIntervalHour = atoi(strOut);
            strcpy(strPAS.szSTLTime1, strOut);
			strcpy(strPAS.szlastSettleDate, "000000");
            inPASSave(1);
            break;
        }
    }

    inPASRead(1);
	
		while(1)
		{
			vduiClearBelow(3);
			setLCDPrint(2, DISPLAY_POSITION_LEFT, "AUTO SETTLE");
			
			if(strPAS.fEnable == 1){
				setLCDPrint(3, DISPLAY_POSITION_LEFT, "ENABLE");
			} else if(strPAS.fEnable == 0){
				setLCDPrint(3, DISPLAY_POSITION_LEFT, "DISABLE");
			}
			
			setLCDPrint(5, DISPLAY_POSITION_LEFT, "0-DISABLE   1-ENABLE");
			
			CTOS_LCDTPrintXY(1, 7, "NEW:");
			usLen=1;
			memset(strOut,0x00, sizeof(strOut));
			ret=shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
			if (ret == d_KBD_CANCEL )
				break;
			else if(ret > 0)
			{
                if(strOut[0] == 0x30)
                {
                    strPAS.fEnable = 0;
					inPASSave(1);
                }
                else if(strOut[0] == 0x31)
                {
                    strPAS.fEnable = 1;
					inPASSave(1);
                }
				else
					continue;
                break;
			}
			if (ret == d_KBD_CANCEL)
				break ;
		}

    inPASRead(1);
	
    return;
    
}

#else
void vdCTOS_SetAutoSettleInterval(void){
    char szRateDisp[d_LINE_SIZE+1];
    char szTemp[4+1];
    BYTE strOut[2+1];
    USHORT usLen;
    int ret;
    
    CTOS_LCDTClearDisplay();
    
    inTCTRead(1);
    
    vdDispTitleString("AUTO SETTLE SET");
    
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "00:00-23:59");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, "CURRENT AUTO SETTLE");
    
    memset(szRateDisp, 0x00, sizeof(szRateDisp));
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%s", strTCT.szAutoSettleIntervalHour);
    memcpy(szRateDisp, szTemp, 2);
    strcat(szRateDisp, ":");
    memcpy(szRateDisp+3, szTemp+2, 2);
//    sprintf(szRateDisp, "%d HOUR", strTCT.inAutoSettleIntervalHour);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, szRateDisp);
    
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
    
    while(1)
    {
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 4, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if (ret >= 0 && ret < 2400) {
//            strTCT.inAutoSettleIntervalHour = atoi(strOut);
            strcpy(strTCT.szAutoSettleIntervalHour, strOut);
            memset(strTCT.chNextSettleTime, 0, sizeof(strTCT.chNextSettleTime));
            inTCTSave(1);
            break;
        }
    }
    
    return;
    
}
#endif
int inCTOS_TrxRetrievalFlowProcess(void)
{
    int inRet = d_NO;
    
    inRet = inCTOS_GeneralGetInvoice();
    if(d_OK != inRet)
        return inRet;
	
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_TRX_RETRIEVAL);
		if(d_OK != inRet)
			return inRet;
	}
	else
	{		
		inRet = inCTOS_BatchSearch();
		if(d_OK != inRet)
			return inRet;
	}
	
	// patrick add code 20141205 start
    inRet = inMultiAP_ECRSendSuccessResponse();
    if (d_OK != inRet)
        return inRet;
    // patrick add code 20141205 end
    
    return inRet;
}

int inCTOS_TrxRetrieval(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

    inRet = inCTOS_TrxRetrievalFlowProcess();
    
    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();
    
    return inRet;
}

void vdCTOS_AutoWaveInit(void){
    if(strTCT.fFirstInit == 0)
        return;
    
    if(inCTOSS_GetCtlsMode() == 3 || inCTOSS_GetCtlsMode() == 4)
        vdCTOS_InitWaveData();
}

void vdCTOS_SetLanguage(void){
    char szRateDisp[d_LINE_SIZE+1];
    BYTE strOut[2+1];
    USHORT usLen;
    int ret;
    int inOrgVal;
    
    CTOS_LCDTClearDisplay();
    
    inOrgVal = strTCT.inThemesType;
    
    vdDispTitleString("LANGUAGE SETTING");
    
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "CURRENT LANGUAGE");
            
    if(strTCT.inThemesType == 0)
        setLCDPrint(4, DISPLAY_POSITION_LEFT, "ENGLISH");
    else if(strTCT.inThemesType == 7)
        setLCDPrint(4, DISPLAY_POSITION_LEFT, "MYANMAR");
    
    setLCDPrint(6, DISPLAY_POSITION_LEFT, "0-ENGLISH  1-MYANMAR");
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
    
    while(1)
    {
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if (ret == 0 || ret == 1) {
            if(atoi(strOut) == 0){
                strTCT.inThemesType = 0;
            } else if(atoi(strOut) == 1){
                strTCT.inThemesType = 7;
            }
            inTCTSave(1);
            break;
        } else
            break;
    }
    
    if(strTCT.inThemesType != inOrgVal){
        CTOS_LCDTClearDisplay();
        setLCDPrint(8, DISPLAY_POSITION_CENTER, "RESTARTING DEVICE");
        vduiWarningSound();
        CTOS_SystemReset();
    }
        
    
}

static void vdInitFont(int inLanguage){
    char szDatabaseName[20+1];
    
    memset(szDatabaseName, 0x00, sizeof(szDatabaseName));
    
    if (inLanguage == 7)
	{
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
	}
	else
	{
		// patrick test 20150115 start
		CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);					 

		CTOS_PrinterTTFSelect(d_FONT_DEFAULT_TTF, 0);
		inCTOSS_SetERMFontType(d_FONT_DEFAULT_TTF, 0);
		CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);

		inSetLanguageDatabase(d_FONT_DEFAULT_TTF);
		inCTOSS_SetALLApFont(d_FONT_DEFAULT_TTF);
	}
}

void vdCTOS_SwitchLanguage(void){
    int inOrgLang;
    int inSwitchLang;
    
    inOrgLang = strTCT.inThemesType;

	 vdDebug_LogPrintf("vdCTOS_SwitchLanguage inOrgLang= [ %d ]",inOrgLang);
    
    if(inOrgLang == 7)
        inSwitchLang = 0;
    else
        inSwitchLang = 7;
    
    CTOS_LCDTClearDisplay();
    strTCT.inThemesType = inSwitchLang;
    inTCTSave(1);
    
    if(inSwitchLang == 7){
        vduiDisplayStringCenter(3, "SET TO");
        vduiDisplayStringCenter(4, "MYANMAR LANGUAGE");
    }
    else{
        vduiDisplayStringCenter(3, "SET TO");
        vduiDisplayStringCenter(4, "ENGLISH LANGUAGE");
    }
    CTOS_Delay(300);

	vdDebug_LogPrintf("vdCTOS_SwitchLanguage inSwitchLang= [ %d ]",inSwitchLang);
       
    vdInitFont(inSwitchLang);
    
    CTOS_LCDTClearDisplay();
}

void vdDefaultLanguage(void){
    int inDefLang;
    
    if(strTCT.fFirstInit){
        if(strTCT.inThemesType == 7){
//            inCTOSS_PutEnv("#DEFLANG", "1");
            put_env_int("#DEFLANG", 1);
        } else{
            put_env_int("#DEFLANG", 0);
        }
    } else {
        inDefLang = get_env_int("#DEFLANG");
        
        if(inDefLang == 0){
            strTCT.inThemesType = 0;
        } else {
            strTCT.inThemesType = 7;
        }
        inTCTSave(1);
    }
    
//    inCTOSS_PutEnv("#DEFLANG", )
}

int inTransSelectCurrency(void){
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT CURRENCY";
    char szCurrencyMenu[1024] = "MMK\n"
                            "USD";
    char szTemp[5+1];
    
    memset(szTemp, 0x00, sizeof(szTemp));
    get_env("MAINCURR",szTemp, 1);
    
    if(strcmp(szTemp, "1") == 0)
        return d_OK;
    
    CTOS_LCDTClearDisplay();
    
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szCurrencyMenu, TRUE);
    
    if(key == d_KBD_CANCEL)
        return d_NO;
    
    switch(key){
        case 1:
            inCSTRead(1);
            break;
        case 2:
            inCSTRead(2);
            break;
    }
    
    return d_OK;
}

void vdCTOS_SetDuplicateReceipt(void){
    char szReceiptCntDisp[3+1];
    BYTE strOut[30];
    USHORT usLen;
    int ret;
    
    CTOS_LCDTClearDisplay();
    
    inTCTRead(1);
    
    vdDispTitleString("NUMBER OF RECEIPT");
    
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "CURRENT COPY RECEIPT");
//    setLCDPrint(4, DISPLAY_POSITION_LEFT, "CURRENT RATE");
    
    memset(szReceiptCntDisp, 0x00, sizeof(szReceiptCntDisp));
//    memset(szRateDisp, 0x00, sizeof(szRateDisp));
//    sprintf(szRateDisp, "MMK %d", strTCT.inExchangeRate);
    sprintf(szReceiptCntDisp, "%d", strTCT.inDupReceipt);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, szReceiptCntDisp);
    
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
    
    while(1)
    {
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 12, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret)
            break;
        else if (ret >= 1) {
            strTCT.inDupReceipt = atoi(strOut);
            inTCTSave(1);
            break;
        }
    }
    
    
    
    return;
    
}

void vdCheckMenu(void){
	int iManualSwitchSim=get_env_int("MANUALSWITCHSIM");
	int inParkingFee = get_env_int("PARKFEE");
	int inTopupReload = get_env_int("TOPUPRELOAD");
	int inQR_PAY = get_env_int("QRPAY");
	int inCAVMenu = get_env_int("CASHADVANCE");
	int inRefundMenu = get_env_int("REFUNDMENU");

	#ifdef PIN_CHANGE_ENABLE
	int inPINCHANGEMenu = get_env_int("PINCHANGE");	
	#endif

	#ifdef OK_DOLLAR_FEATURE
	int inOKDollar = get_env_int("OKDOLLAR");	
	#endif

	
	vdDebug_LogPrintf("vdCheckMenu strTCT.fEnableIPPMenu [%d]!!!", strTCT.fEnableIPPMenu);

    inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnableIPPMenu, IPP_MENU_INDEX);
	
    inHDTRead(6);
    if(strTCT.fEnableIPPMenu == FALSE){

		//int inOrgMenuid;
    	//inTCTGetCurrMenuid(1, &inOrgMenuid);
  		//if(inOrgMenuid != 3)	
        strHDT.fHostEnable = FALSE;
    } 
	else 
	{
        strHDT.fHostEnable = TRUE;

		//http://118.201.48.210:8080/redmine/issues/1525.118
        inDSubMenuUpdateFlag("DMIPPMenu", FALSE, 2);
        inDSubMenuUpdateFlag("DMIPPMenu", FALSE, 3);

    }
    //inHDTSave(6);
    // temp, to fix tpdu and nii zero value upon enabling fEnableIPPMenu flag
	inHDTUpdateHostEnable(6, strHDT.fHostEnable);	

    if(inParkingFee != 1)
    {
        inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnableAlipayMenu, ALIPAY_MENU_INDEX);
        
        //thandar
        inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnableUPISignONMenu, UNIONPAY_MENU_INDEX);
        inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnablePreAuthMenu, CARDVER_MENU_INDEX);
        inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnablePreAuthMenu, PREAUTH_MENU_INDEX);
    }

//for discount
    #ifdef DISCOUNT_FEATURE
	inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnableDiscountMenu, DISCOUNT_MENU_INDEX);
    #endif
	
	#if 0
    inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnableAlipayMenu, 28);
    if(strTCT.fEnableAlipayMenu == FALSE){
        inHDTRead(10);
        strHDT.fHostEnable = FALSE;
        inHDTSave(10);
        
        inHDTRead(12);
        strHDT.fHostEnable = FALSE;
        inHDTSave(12);
    } else {
        inHDTRead(10);
        strHDT.fHostEnable = TRUE;
        inHDTSave(10);
        
        inHDTRead(12);
        strHDT.fHostEnable = TRUE;
        inHDTSave(12);
    }
	#endif
	
    #ifdef DUAL_SIM_SETTINGS
	if(iManualSwitchSim == 1)
	    inDSubMenuUpdateFlag("DMTrxMenu", TRUE, SWITCHSIM_MENU_INDEX);
	else
	    inDSubMenuUpdateFlag("DMTrxMenu", FALSE, SWITCHSIM_MENU_INDEX);
	#endif


	#ifdef TOPUP_RELOAD
	if(inTopupReload == 1)
	    inDSubMenuUpdateFlag("DMTrxMenu", TRUE, TOPUP_MENU_INDEX);
	else
	    inDSubMenuUpdateFlag("DMTrxMenu", FALSE, TOPUP_MENU_INDEX);
	#endif

    #ifdef CBPAY_DV
    if(inQR_PAY == 1)	
        inDSubMenuUpdateFlag("DMTrxMenu", TRUE, CBPAY_MENU_INDEX);
    else
        inDSubMenuUpdateFlag("DMTrxMenu", FALSE, CBPAY_MENU_INDEX);
    #endif

	#ifdef CASH_ADV_NEW_FEATURE
    if(inCAVMenu == 1)	
        inDSubMenuUpdateFlag("DMTrxMenu", TRUE, CASHADV_MENU_INDEX);
    else
        inDSubMenuUpdateFlag("DMTrxMenu", FALSE, CASHADV_MENU_INDEX);
	
	#endif

	//list of enhancement logged to redmine with case #1861
	#ifdef ENHANCEMENT_1861
	if(inRefundMenu == 1)
        inDSubMenuUpdateFlag("DMTrxMenu", TRUE, REFUND_MENU_INDEX);
	else
        inDSubMenuUpdateFlag("DMTrxMenu", FALSE, REFUND_MENU_INDEX);
	#endif

	#ifdef PIN_CHANGE_ENABLE
	if(inPINCHANGEMenu == 1)
		inDSubMenuUpdateFlag("DMTrxMenu", TRUE, CHANGEPIN_MENU_INDEX);
	else
		inDSubMenuUpdateFlag("DMTrxMenu", FALSE, CHANGEPIN_MENU_INDEX);
	#endif

	#ifdef OK_DOLLAR_FEATURE
	if(inOKDollar == 1)		
		inDSubMenuUpdateFlag("DMTrxMenu", TRUE, OK_DOLLAR_MENU_INDEX);
	else		
		inDSubMenuUpdateFlag("DMTrxMenu", FALSE, OK_DOLLAR_MENU_INDEX);
	#endif
	
}

#ifdef SET_IPP_MENU
void vdCheckMenuID3(void){

	vdDebug_LogPrintf("vdCheckMenuID3)");

	/*
	1. INSTALLMENT
	2. VOID
	3. SETTLEMENT
	4. SIGN ON 

	1. REPRINT
	2. REPORT (SUMMARY / DETAIL)
	3. BATCH REVIEW
	4. TMS DOWNLOAD

	1. UPLOAD RECEIPT
	2. SWITCH SIM
	*/

	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 4);
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 9);
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 11);
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 13);	

	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 14);		
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 19);
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 21);
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 22);

	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 23);
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 31);
	
}

#endif


//new request for change menu id 4 menu arrangement - 09262022
#if 0
//#ifdef PIN_CHANGE_ENABLE
void vdCheckMenuID4(void){
	int inPINCHANGEMenu = get_env_int("PINCHANGE");	
	int iManualSwitchSim=get_env_int("MANUALSWITCHSIM");

#ifdef OK_DOLLAR_FEATURE
		int inOKDollar = get_env_int("OKDOLLAR");	
#endif
	
	vdDebug_LogPrintf("vdCheckMenuID4)");

	inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 3);	// follow dynamicmenuid4 usButtonID value. for this case 3, Disabled OFFLINE menu.
	inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 7);	// follow dynamicmenuid4 usButtonID value. for this case 7, Disabled REFUND menu.
	inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 8);	// follow dynamicmenuid4 usButtonID value. for this case 8, Disabled WAVEREFUND menu.
	//inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 25);	// follow dynamicmenuid4 usButtonID value. for this case 25, Disabled SWITCH SIM menu.
	
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 15);	// follow dynamicmenuid4 usButtonID value. for this case 15, Enabled LAST RECEIPT menu.
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 4); 	// follow dynamicmenuid4 usButtonID value. for this case 4, Enabled Cash Advance menu.
	

	if(inPINCHANGEMenu == 1)
		inDSubMenuUpdateFlag("DMTrxMenu", TRUE, CHANGEPIN_MENU_INDEX);
	else
		inDSubMenuUpdateFlag("DMTrxMenu", FALSE, CHANGEPIN_MENU_INDEX);
		

#ifdef DUAL_SIM_SETTINGS
	if(iManualSwitchSim == 1)
	    inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 25);
	else
	    inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 25);
#endif

#ifdef OK_DOLLAR_FEATURE
	if(inOKDollar == 1) 	
		inDSubMenuUpdateFlag("DMTrxMenu", TRUE, OK_DOLLAR_MENU_INDEX);
	else		
		inDSubMenuUpdateFlag("DMTrxMenu", FALSE, OK_DOLLAR_MENU_INDEX);
#endif


}
#else

void vdCheckMenuID4(void){
	int inPINCHANGEMenu = get_env_int("PINCHANGE");	
	int iManualSwitchSim=get_env_int("MANUALSWITCHSIM");

#ifdef OK_DOLLAR_FEATURE
		int inOKDollar = get_env_int("OKDOLLAR");	
#endif
	
	vdDebug_LogPrintf("vdCheckMenuID4)");

	#if 0
	inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 3);	// follow dynamicmenuid4 usButtonID value. for this case 3, Disabled OFFLINE menu.
	inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 7);	// follow dynamicmenuid4 usButtonID value. for this case 7, Disabled REFUND menu.
	inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 8);	// follow dynamicmenuid4 usButtonID value. for this case 8, Disabled WAVEREFUND menu.
	
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 15);	// follow dynamicmenuid4 usButtonID value. for this case 15, Enabled LAST RECEIPT menu.
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 4); 	// follow dynamicmenuid4 usButtonID value. for this case 4, Enabled Cash Advance menu.
	#else
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 1); //LOGON
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 2); // CASH ADVANCE
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 3); //TOPTUP

	if(inPINCHANGEMenu == 1)
		inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 4); //PIN CHANGE
	else
		inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 4);
	
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 5);		//VOID
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 6);		//SETTLEMENT
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 7);		//BATCH REVIEW
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 8);		//REPORT
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 9);		//REPRINT
	inDSubMenuUpdateFlag("DMTrxMenu", TRUE,10);	//TMS DOWNLOAD
	
	#endif

		

#ifdef DUAL_SIM_SETTINGS
	if(iManualSwitchSim == 1)
	    inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 11);
	else
	    inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 11);
#endif

#ifdef OK_DOLLAR_FEATURE
	if(inOKDollar == 1) 	
		inDSubMenuUpdateFlag("DMTrxMenu", TRUE, 36);
	else		
		inDSubMenuUpdateFlag("DMTrxMenu", FALSE, 36);
#endif


}


#endif

void vdEnableIPPMenu(void){
    BYTE strOut[30];
    USHORT usLen;
    int ret;

	int inOrgMenuid;
    inTCTGetCurrMenuid(1, &inOrgMenuid);
  	if(inOrgMenuid == 3)
		return;
  	
    
    CTOS_LCDTClearDisplay();
    
    vdDispTitleString("ENABLE IPP");
    
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "IPP MENU");
	if (TRUE == strTCT.fEnableIPPMenu)
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "ENABLE");
	else
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "DISABLE");
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "0-DISABLE   1-ENABLE");
    
    inHDTRead(6);
    
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
    while(1)
    {
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if (ret == 0 || ret == 1) {
            if(atoi(strOut) == 0){
                strTCT.fEnableIPPMenu = FALSE;
                strHDT.fHostEnable = FALSE;
            }
            else{
                strTCT.fEnableIPPMenu = TRUE;
                strHDT.fHostEnable = TRUE;
            }
            
            inHDTSave(6);
            inTCTSave(1);
            vduiClearBelow(2);
            vduiWarningSound();
            setLCDPrint(8, DISPLAY_POSITION_LEFT, "RESTARTING...");
            CTOS_SystemReset();
            break;
        } else
            break;
    }
}

#if 0
void vdEnableAlipayMenu(void){
    BYTE strOut[30];
    USHORT usLen;
    int ret;
    
    CTOS_LCDTClearDisplay();
    
    vdDispTitleString("ENABLE ALIPAY");
    
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "ALIPAY MENU");
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "0-DISABLE   1-ENABLE");
    
//    inHDTRead(6);
    
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
    while(1)
    {
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if (ret == 0 || ret == 1) {
            if(atoi(strOut) == 0){
                strTCT.fEnableAlipayMenu = FALSE;
                inHDTRead(10);
                strHDT.fHostEnable = FALSE;
                inHDTSave(10);
                
                inHDTRead(12);
                strHDT.fHostEnable = FALSE;
                inHDTSave(12);
            }
            else{
                strTCT.fEnableAlipayMenu = TRUE;
                inHDTRead(10);
                strHDT.fHostEnable = TRUE;
                inHDTSave(10);
                
                inHDTRead(12);
                strHDT.fHostEnable = TRUE;
                inHDTSave(12);
            }
            
            
            inTCTSave(1);
            vduiClearBelow(2);
            vduiWarningSound();
            setLCDPrint(8, DISPLAY_POSITION_LEFT, "RESTARTING...");
            CTOS_SystemReset();
            break;
        } else
            break;
    }
}
#endif

/*Alipay now with CBB flow, same as Visa/Master host*/
void vdEnableAlipayMenu(void)
{
	BYTE strOut[30];
    USHORT usLen;
    int ret;
    
    CTOS_LCDTClearDisplay();



//ALIPAY MENU as init in posmain.c
#if 1
	//if(TRUE == strTCT.fEnableAlipayMenu)
	//{
		vduiClearBelow(2);
		vduiWarningSound();
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "MENU NOT SUPPORTED");
		WaitKey(3);

		inDSubMenuUpdateFlag("DMTrxMenu", FALSE, ALIPAY_MENU_INDEX2);
		
		return d_OK;
	//}
#endif	


    
    vdDispTitleString("ENABLE ALIPAY");
    
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "ALIPAY MENU");
	if (TRUE == strTCT.fEnableAlipayMenu)
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "ENABLE");
	else
		setLCDPrint(4, DISPLAY_POSITION_LEFT, "DISABLE");
	
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "0-DISABLE   1-ENABLE");
    
//    inHDTRead(6);
    
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
    while(1)
    {
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if (ret == 0 || ret == 1) {
            if(atoi(strOut) == 0){
                strTCT.fEnableAlipayMenu = FALSE;
            }
            else{
                strTCT.fEnableAlipayMenu = TRUE;
            }

            inTCTSave(1);

			inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnableAlipayMenu, ALIPAY_MENU_INDEX2);
			
            //vduiClearBelow(2);
            //vduiWarningSound();
            //setLCDPrint(8, DISPLAY_POSITION_LEFT, "RESTARTING...");
            //CTOS_SystemReset();
            break;
        } else
            break;
    }
}

void vdEnablePreAuthMenu(void)
	{
		BYTE strOut[30];
		USHORT usLen;
		int ret;
		
		CTOS_LCDTClearDisplay();
		
		vdDispTitleString("ENABLE PREAUTH");
		
		setLCDPrint(3, DISPLAY_POSITION_LEFT, "PREAUTH MENU");
		if (TRUE == strTCT.fEnablePreAuthMenu)
			setLCDPrint(4, DISPLAY_POSITION_LEFT, "ENABLE");
		else
			setLCDPrint(4, DISPLAY_POSITION_LEFT, "DISABLE");
		
		setLCDPrint(5, DISPLAY_POSITION_LEFT, "0-DISABLE   1-ENABLE");
		
	//	  inHDTRead(6);
		
		setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
		while(1)
		{
			memset(strOut,0x00, sizeof(strOut));
			ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
			if (ret == d_KBD_CANCEL )
				break;
			else if (ret == 0 || ret == 1) {
				if(atoi(strOut) == 0){
					strTCT.fEnablePreAuthMenu = FALSE;
				}
				else{
					strTCT.fEnablePreAuthMenu = TRUE;
				}
	
				inTCTSave(1);
	
				inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnablePreAuthMenu, CARDVER_MENU_INDEX);
				inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnablePreAuthMenu, PREAUTH_MENU_INDEX);
				
			
				break;
			} else
				break;
		}
	}

void vdEnableUPIFinexusMenu(void)
{
			BYTE strOut[30];
			USHORT usLen;
			int ret;
			
			CTOS_LCDTClearDisplay();
			
			vdDispTitleString("ENABLE UPI");
			
			setLCDPrint(3, DISPLAY_POSITION_LEFT, "UPI MENU");
			if (TRUE == strTCT.fEnableUPISignONMenu)
				setLCDPrint(4, DISPLAY_POSITION_LEFT, "ENABLE");
			else
				setLCDPrint(4, DISPLAY_POSITION_LEFT, "DISABLE");
			
			setLCDPrint(5, DISPLAY_POSITION_LEFT, "0-DISABLE   1-ENABLE");
			
		//	  inHDTRead(6);
			
			setLCDPrint(7, DISPLAY_POSITION_LEFT, "NEW:");
			while(1)
			{
				memset(strOut,0x00, sizeof(strOut));
				ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 1, 1, 0, d_INPUT_TIMEOUT);
				if (ret == d_KBD_CANCEL )
					break;
				else if (ret == 0 || ret == 1) {
					if(atoi(strOut) == 0){
						strTCT.fEnableUPISignONMenu = FALSE;
					}
					else{
						strTCT.fEnableUPISignONMenu = TRUE;
					}
		
					inTCTSave(1);
		
					inDSubMenuUpdateFlag("DMTrxMenu", strTCT.fEnableUPISignONMenu, UNIONPAY_MENU_INDEX);
				
					
				
					break;
				} else
					break;
			}
}


void vdShowTerminalIP(void)
{

#if 1
	BYTE szStr[30], strtemp[17];
	
	memset(strtemp, 0x00, sizeof(strtemp));
	memset(szStr, 0x00, sizeof(szStr));

	inEthernet_GetTerminalIP(strtemp);

    sprintf(szStr, "IP: %s", strtemp);     
	CTOS_LCDTPrintXY(1, 3, szStr);
	CTOS_Delay(5000);
#endif
	{
	BYTE key;
	USHORT ret;
	BYTE Get_DHCP[1];
	BYTE len_Get_DHCP;
	BYTE display[15];
	
	CTOS_LCDTClearDisplay();
	do
	{     
		ret = CTOS_EthernetOpen();
		if(ret != d_OK)
		{
			CTOS_LCDTPrintXY(1, 7, "Open Not OK  ");
			CTOS_KBDGet(&key);
			CTOS_Beep();
			return 0;
		}

		//inCTOSS_EthernetSetDHCP();
		//CTOS_LCDTPrintXY(1, 2, "SetConfig OK");
		vdPrint_EthernetStatus();// (2) after EthernetSetDHCP()
                                
		ret = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_DHCP, Get_DHCP, &len_Get_DHCP);
		//if(ret == d_OK)
		//{
		//	sprintf( display, "DHCP = %s", Get_DHCP);
		//	CTOS_LCDTPrintXY(1, 6, display);
		//}
		//else
		//	CTOS_LCDTPrintXY(1, 6, "Get_DHCP NG");
		
		//CTOS_LCDTPrintXY(1, 7, "Press X to leave");
		//CTOS_KBDGet(&key);
		//CTOS_Beep();
                
		CTOS_EthernetClose();

		break;
	}while(1);

}

}


int Check_OverDateTime(unsigned char S_bYear,unsigned char S_bMonth,unsigned char S_bDay,unsigned char S_bHour,unsigned char S_bMinute,unsigned char S_Second)
{
    CTOS_RTC CurRTC;
    CTOS_RTCGet(&CurRTC);
    vdDebug_LogPrintf("CurRTC = [ %d:%d:%d ]",CurRTC.bHour,CurRTC.bMinute,CurRTC.bSecond);
	//vdDebug_LogPrintf("CurRTCYY = %d)(bMonth = %d )[bDay=%d]",(CurRTC.bYear),CurRTC.bMonth,CurRTC.bDay); 
	//vdDebug_LogPrintf("(S_bYear = %d)(S_bMonth = %d )",S_bYear,S_bMonth); 
	if((S_bYear == 0)&&(S_bMonth == 0)&&(S_bDay == 0))
    {
        return 0;
    }
    else if((CurRTC.bYear > S_bYear)||(CurRTC.bMonth > S_bMonth))
    {
        vdDebug_LogPrintf("(CurRTC.bYear = %d)(CurRTC.bMonth = %d )[1]",CurRTC.bYear,CurRTC.bMonth); 
        return 1;
    }
    else if(CurRTC.bDay > S_bDay)
    {
        vdDebug_LogPrintf("(CurRTC.bDay = %d)[2]",CurRTC.bDay); 
        return 1;
    }
    else
    {
        if(CurRTC.bHour > S_bHour)
        {
            vdDebug_LogPrintf("(CurRTC.bHour = %d)[3]",CurRTC.bHour);
            return 1;
        }
        else if((CurRTC.bHour == S_bHour)&&(CurRTC.bMinute >= S_bMinute))
        {
            vdDebug_LogPrintf("(CurRTC.bMinute = %d)[4]",CurRTC.bMinute);
            return 1;
        }
    }
    return 0;
}


void vdGetDateFromInt(int Date,CTOS_RTC *BUFFDATE)
{
			if(Date == 0)
			{
				BUFFDATE->bYear = 0;
				BUFFDATE->bMonth = 0;
				BUFFDATE->bDay = 0; 	   
			}
			else
			{
				BUFFDATE->bDay = (Date%100);
				BUFFDATE->bMonth = (Date/100)%100;
				BUFFDATE->bYear = (Date/10000);
			}
}

void vdGetTimeFromInt(int Time,CTOS_RTC *BUFFDATE)
{
    if(Time == 0)
    {
        BUFFDATE->bHour = 0;
        BUFFDATE->bMinute = 0;
        BUFFDATE->bSecond = 0;        
    }
    else
    {
        BUFFDATE->bHour = (Time/100)%100;
        BUFFDATE->bMinute = (Time%100);
        BUFFDATE->bSecond = 0;
    }
}


void vdEnableBTMenu(void)
{
    char szBTMenu[1024] = {0};
	char szHeaderString[50] = "SELECT BT FUNCS";
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE key;
	BYTE x = 1;
	BYTE bInBuf[250];
	BYTE bOutBuf[250];
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	vdDebug_LogPrintf("vdEnableBTMenu");

	 if (strTCT.fECR!=1) 
	     	return;

	  	
	 if (6 ==strTCT.byRS232ECRPort) 
{
   
	 
	CTOS_LCDTClearDisplay();
	CTOS_LCDTPrintXY(1, 1, "BT MENU");
	memset(szBTMenu, 0x00, sizeof(szBTMenu));
	strcpy((char*)szBTMenu, "Open\nListen\nStatus\nRxReady\nTxData\nClose\nCANCEL");
	key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szBTMenu, TRUE);
	
	if (key == 0xFF) 
	{	
		CTOS_LCDTClearDisplay();
		setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
		vduiWarningSound();
		return; 
	}

	if (key > 0)
	{
		
		if(d_KBD_CANCEL == key)
		return;

		memset(bOutBuf, 0x00, sizeof(bOutBuf));
		memset(bInBuf, 0x00, sizeof(bInBuf));


		if (key == 1)
	 {
	
			// BT Open
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "Open BT");
	
			//usResult = inMultiAP_RunIPCCmdTypes("SHARLS_ECR", d_IPC_CMD_ECR_Initialize, bInBuf, usInLen, bOutBuf, &usOutLen);
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_BT", d_IPC_CMD_BT_OPEN, bInBuf, usInLen, bOutBuf, &usOutLen);
    		vdDebug_LogPrintf("**CTOSS_BluetoothOpen [%d]**", usResult);
		
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "BluetoothOpen error");
			else
				vdDisplayErrorMsg(1, 8, "BluetoothOpen OK");
	
			return ;
		}
			
		
		if (key == 2)
		{
			
			// BT Listen
			
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "BT Listen");		
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_BT", d_IPC_CMD_BT_LISTEN, bInBuf, usInLen, bOutBuf, &usOutLen);
    		vdDebug_LogPrintf("**CTOSS_BluetoothListen [%d]**", usResult);
		
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "BluetoothListen error");
			else
				vdDisplayErrorMsg(1, 8, "BluetoothListen OK");
	
			return ;
		}
		
		if (key == 3)
		{
			// BT Status
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "BT Status");		
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_BT", d_IPC_CMD_BT_STATUSEx, bInBuf, usInLen, bOutBuf, &usOutLen);
    		vdDebug_LogPrintf("**CTOSS_BluetoothStatusEx [%d]**", usResult);
		
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "Bluetooth Status error");
			else
				vdDisplayErrorMsg(1, 8, "Bluetooth Status  OK");
	
			return ;	
		}

		
        if (key == 4)
		{
			// BT RxReady 
			//Check BT connected or not
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "BT RxReady");		
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_BT", d_IPC_CMD_BT_RXREADY, bInBuf, usInLen, bOutBuf, &usOutLen);
    		vdDebug_LogPrintf("**CTOSS_BluetoothRxReady [%d]**", usResult);
		
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "Bluetooth RxReady  error");
			else
				vdDisplayErrorMsg(1, 8, "Bluetooth RxReady   OK");
	
			return ;	
		      
         }

		if (key == 5)
		{
			// BT TxData
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "BT TxData");		
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_BT", d_IPC_CMD_BT_TXDATA, bInBuf, usInLen, bOutBuf, &usOutLen);
    		vdDebug_LogPrintf("**CTOSS_BluetoothTxData [%d]**", usResult);
			vdDebug_LogPrintf("usResult [%d] usOutLen [%d]", usResult, usOutLen);	
			
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "Bluetooth TxReady  error");
			else
				vdDisplayErrorMsg(1, 8, "Bluetooth TxReady   OK");
	
			return ;	
		      
         }

		if (key == 6)
		{
			// BT Close
			CTOS_LCDTClearDisplay();
			CTOS_LCDTPrintXY(1, 1, "Close BT");	
		
			usResult = inMultiAP_RunIPCCmdTypes("SHARLS_BT", d_IPC_CMD_BT_CLOSE, bInBuf, usInLen, bOutBuf, &usOutLen);
    		vdDebug_LogPrintf("**CTOSS_BluetoothClose [%d]**", usResult);
		
			if (usResult != d_OK)		
				vdDisplayErrorMsg(1, 8, "BluetoothClose error");
			else
				vdDisplayErrorMsg(1, 8, "BluetoothClose OK");
	
			return ;
			
		}
					
		if (key == 7)
		{// --->> CANCEL
			vdDisplayErrorMsg(1, 8, "Cancelled");
			return;
		}

	
		}

	 }

	 
}

void vdCTOS_SelectWIFI(void)
{
    if(strCPT.inCommunicationMode == WIFI_MODE)
    {
        inCTOSS_COMMWIFISCAN();
        if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
        {
            vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
            return;
        }				
    }
}

#ifdef PARKING_FEE
void vdCTOS_SetParkingFee(void)
{
    int		inRet = d_OK;
    BYTE 	key;
    char    szStrCurrSymbol[4+1];
    BYTE    szEMVCurSymbol[2+1];
    int     inCurrCode = 0;
    int ret = 0;
    USHORT usLen;
    char szInput[12+1];
    int inCurr = 0;
    int inNum = 0;

	char szParkFeeAmt[12+1], szTmp1[12+1], szTmp2[12+1];
    int inParkFeeAmt=0;

	CTOS_LCDTClearDisplay();

    inNum = inBatchNumRecord();
    
    if(inNum > 0)
    {
        setLCDPrint(8, DISPLAY_POSITION_LEFT, "BATCH NOT EMPTY");
        vduiWarningSound();
        CTOS_Delay(1500);
        return;
    }
	
	inParkFeeAmt = get_env_int("PARKFEEAMT");
	memset(szTmp1, 0, sizeof(szTmp1));
    memset(szTmp2, 0, sizeof(szTmp2));
	memset(szParkFeeAmt, 0, sizeof(szParkFeeAmt));
    
	sprintf(szTmp1, "%d", inParkFeeAmt);
	
    vdDispTitleString("SET AMOUNT");
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "AMOUNT");
    //inCSTRead(strHDT.inCurrencyIdx);

    setLCDPrint(3, DISPLAY_POSITION_LEFT, szTmp1);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "INPUT NEW:");
    memset(szInput, 0x00, sizeof(szInput));
    ret= shCTOS_GetNum(8, 0x01,  szInput, &usLen, 1, 12, 0, d_INPUT_TIMEOUT);

	if(strlen(szInput) > 0)
        put_env("PARKFEEAMT",szInput, strlen(szInput));

	return;
}
#endif

