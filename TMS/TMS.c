/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

#include <unistd.h>
#include <pwd.h>


#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>


#include "..\Includes\Wub_lib.h"

#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\Display.h"

#include "..\Includes\V5IsoFunc.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\Trans.h"   


#include "..\Includes\CTOSInput.h"


#include "..\debug\debug.h"
#include "..\Accum\Accum.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\accum\accum.h"
#include "..\Includes\POSSetting.h"
#include "..\Debug\Debug.h"
#include "..\filemodule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\CTOSInput.h"
#include "..\Ctls\PosWave.h"
#include "..\TMS\TMS.h"
#include "..\print\print.h"

#include "..\POWRFAIL\POSPOWRFAIL.h"


#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"

#define d_CTMS_NAC_NORMAL		0x21
#define d_CTMS_NAC_DEFAULT_GPRS		0x22
#define d_CTMS_NAC_DEFAULT_MODEM	0x23
#define d_CTMS_NAC_DEFAULT_ETHERNET	0x24
#define d_CTMS_NAC_DEFAULT_WIFI		0x25

int inNACDefMdm = 0;

#if 0

#ifndef d_CTMS_INFO_LAST_UPDATE_TIME
#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
#endif

USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo)
{
    CTOS_RTC SetRTC;
    
    CTOS_RTCGet(&SetRTC);

    SetRTC.bMonth -= 2;

    memcpy(stInfo, &SetRTC, sizeof(CTOS_RTC));

    return d_OK;
}

#endif

int inCTOSS_TMSCheckIfDefaultApplication(void)
{
    CTOS_stCAPInfo stinfo;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;

    getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	strcpy(exe_subdir, &exe_dir[9]);
	inExeAPIndex = atoi(exe_subdir);
	memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
    if(CTOS_APGet(inExeAPIndex, &stinfo) != d_OK)
	{
		CTOS_APGet(inExeAPIndex, &stinfo);
	}

    vdDebug_LogPrintf("stinfo.bFlag[%02X]", stinfo.bFlag);
    if (stinfo.bFlag != d_AP_FLAG_DEF_SEL_EX)
        return d_NO;
    else
        return d_OK;
    
}

int inCTOSS_TMSChkBatchEmpty(void)
{
    int inResult;

    vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty start");
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOSS_TMSChkBatchEmptyProcess();
        if(inResult != d_OK)
            return inResult;
        else
		    inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_CHK_BATCH_EMPTY);

        if(inResult != d_FAIL)
            inResult = d_OK;
	}
	else
	{
		inResult = inCTOSS_TMSChkBatchEmptyProcess();
	}
    
    vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty return[%d]", inResult);
    
    return (inResult);
}


int inCTOSS_TMSChkBatchEmptyProcess(void)
{
    int inResult;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
    char szAPName[25];
	int inAPPID;

	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    //check host num
    inNumOfHost = inHDTNumRecord();
    
    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
			if (strcmp(szAPName, strHDT.szAPName)!=0)
			{
				continue;
			}
            
            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
            /*
                if((inResult = inMMTReadRecord(strHDT.inHostIndex, strMMT[inLoop-1].MMTid)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MMTid,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else    // delete batch where hostid and mmtid is match  
                { 
                */
                    strMMT[0].HDTid = strHDT.inHostIndex;
                    strMMT[0].MITid = strMMT[inLoop-1].MITid;
                    srTransRec.MITid = strMMT[inLoop-1].MITid;

                    vdDebug_LogPrintf("srTransRec.MITid[%d]strHDT.inHostIndex[%d]", srTransRec.MITid, strHDT.inHostIndex);
                    memset(&srAccumRec,0,sizeof(srAccumRec));
                    memset(&strFile,0,sizeof(strFile));
                    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
                    memset(&strFile,0,sizeof(strFile));
                    vdCTOS_GetAccumName(&strFile, &srAccumRec);

				
					if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) > 0)
                    {
                      vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty Not Empty");
                    //vdDisplayErrorMsg(1, 8, "BATCH NOT EMPTY");
                    //vdSetErrorMessage("");
                    return (d_FAIL);
                    }
					#if 0
                    else
                    {
                        vdDebug_LogPrintf("Batch Empty");
                    }
                    #endif
					//thandar_modifed to fix not to dsiplay Batch not empty after settlement receipt

					 if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) <= 0)
                    {
                        vdDebug_LogPrintf("Batch Empty");
                    }

					
            //    }
            }
        }
        else
            continue;

    }
    
    return (d_OK);
}

int inCTOSS_TMSPreConfig(int inComType)
{    
    CTMS_GPRSInfo stgprs;    
    CTMS_ModemInfo stmodem;    
    CTMS_EthernetInfo st;
	CTMS_WIFIInfo stWF;
    int inNumOfRecords = 0;
    BYTE szSerialNum[17+1], szInputBuf[21+1];
    BYTE count = 2,i;
	BYTE tmpbuf[16 + 1];
	int len;
	USHORT usRet;
    unsigned char ckey;
	USHORT usStatusLine=8, usRes=0;
	int inAutoSwitchSim=get_env_int("AUTOSWITCHSIM");
	int inManualSwitchSim=get_env_int("MANUALSWITCHSIM");
	
    #ifndef DUAL_SIM_SETTINGS
        inAutoSwitchSim=0; /*disable - not yet complete*/
    #else
        if(inManualSwitchSim == 1)
            inAutoSwitchSim=1;
    #endif
	
	
	vdDebug_LogPrintf("inCTOSS_TMSPreConfig");

    inHDTRead(1);
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfRecords);
    
    memset(szSerialNum, 0x00, sizeof(szSerialNum));
	memset(tmpbuf, 0x00, sizeof(tmpbuf));

	CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	if(strlen(szSerialNum) <= 0)
	    CTOS_GetFactorySN(szSerialNum);
	for (i=0; i<strlen(szSerialNum); i++)
	{
		if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
		{
			szSerialNum[i] = 0;
			break;
		}
	}
	len = strlen(szSerialNum);
	strcpy(tmpbuf,"0000000000000000");
	memcpy(&tmpbuf[16-len],szSerialNum,len);
	vdDebug_LogPrintf("szSerialNum=[%s].tmpbuf=[%s]..",szSerialNum,tmpbuf);
	
    CTOS_CTMSSetConfig(d_CTMS_SERIALNUM, tmpbuf);//if TID is 12345678, SN is 0000000012345678
    CTOS_CTMSSetConfig(d_CTMS_RECOUNT, &count);

    if(strTCP.fDHCPEnable)
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, "0.0.0.0");
    else
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, strTCP.szTerminalIP);// If it is DHCP, also need to configure any value, otherwise please put the value from database
    
    if(strTCT.fMustAutoSettle == TRUE){
        if(inCTOS_CheckTMSAfterAutoSettle() != d_OK)
            return d_NO;
        else
            goto lblOK;
    }

    if(ETHERNET_MODE == strTCT.inTMSComMode)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("ETHERNET SETTINGS");
	
    	vdDebug_LogPrintf("ETHERNET_MODE..");
        memset(&st, 0x00, sizeof (CTMS_EthernetInfo));
		usRet = CTOS_CTMSGetConfig(d_CTMS_ETHERNET_CONFIG, &st);
		if (usRet != d_OK)
		{
			CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
			vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
			return d_NO;
	    }
#if 0   
        strcpy(st.strGateway, strTCP.szGetWay);
        strcpy(st.strMask, strTCP.szSubNetMask);
        st.bDHCP = strTCP.fDHCPEnable;

        strcpy(st.strRemoteIP, strTCT.szTMSRemoteIP);
        st.usRemotePort = strTCT.usTMSRemotePort;
#endif
		CTOS_LCDTClearDisplay();
		vdDispTitleString("ETHERNET SETTINGS");
//http://118.201.48.210:8080/redmine/issues/3320
//To Change Production CTMS 2 Server Connection from IP to URL
#ifdef CTMS_IP_TO_URL_MODE
		inCTOSS_COMMUrlParseIP(ETHERNET_MODE,st.strRemoteIP);

#endif
		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", st.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", st.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &st);
    }
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
    {
    	vdDebug_LogPrintf("DIAL_UP_MODE..");

        CTOS_LCDTClearDisplay();
        vdDispTitleString("DIALUP SETTINGS");

		memset(&stmodem, 0x00, sizeof (CTMS_ModemInfo));
		usRet = CTOS_CTMSGetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
		if (usRet != d_OK)
		{
	        //CTOS_LCDTPrintXY(1, 8, "  CTMS Get Modem Fail ");
			//CTOS_Delay(1500);
			CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
			vdDisplayErrorMsg(1, 8, "CTMS Get Modem Fail");
			return d_NO;
	    }

		/*DIAL-UP have NAC Dial-up and Mdm PPP dial-up*/
		if (stmodem.bType == 1)
			inNACDefMdm = 1;
		else
			inNACDefMdm = 0;

  #if 0      
        stmodem.bMode = d_M_MODE_AYNC_FAST;
        stmodem.bHandShake = d_M_HANDSHAKE_V32BIS_AUTO_FB;
        stmodem.bCountryCode = d_M_COUNTRY_SINGAPORE;

        strcpy(stmodem.strRemotePhoneNum, strTCT.szTMSRemotePhone);
		vdDebug_LogPrintf("strRemotePhoneNum[%s]..",stmodem.strRemotePhoneNum);
        strcpy(stmodem.strID, strTCT.szTMSRemoteID);	
		vdDebug_LogPrintf("strID[%s]..",stmodem.strID);
        strcpy(stmodem.strPW, strTCT.szTMSRemotePW);
		vdDebug_LogPrintf("strPW[%s]..",stmodem.strPW);

        strcpy(stmodem.strRemoteIP, strTCT.szTMSRemoteIP);
        stmodem.usRemotePort = strTCT.usTMSRemotePort;
#endif

		stmodem.usPPPRetryCounter = 2;
		stmodem.ulPPPTimeout = 34463;
		stmodem.ulDialTimeout = 34463;
		vdDebug_LogPrintf("strRemoteIP[%s].usRemotePort=[%d].",stmodem.strRemoteIP,stmodem.usRemotePort);

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stmodem.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stmodem.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
        
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PHONE: %s", stmodem.strRemotePhoneNum);         
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf); 
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "ID: %s", stmodem.strID);         
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stmodem.strPW);         
        setLCDPrint(7, DISPLAY_POSITION_LEFT, szInputBuf);
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
    }
    else if(GPRS_MODE == strTCT.inTMSComMode)
    {
		CTMS_GPRSInfo stgprs;

        CTOS_LCDTClearDisplay();
        vdDispTitleString("GPRS SETTINGS");
		
		memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		usRet = CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		if (usRet != d_OK)
		{
			CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
			vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
			return d_NO;
	    }
#if 0		
		CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		
        if(strlen(stgprs.strAPN) > 0)
            strcpy(strTCP.szAPN, stgprs.strAPN);

        if(strlen(stgprs.strID) > 0)
            strcpy(strTCP.szUserName, stgprs.strID);

        if(strlen(stgprs.strPW) > 0)
		    strcpy(strTCP.szPassword, stgprs.strPW);
		
              vdDebug_LogPrintf("GPRS_MODE..");
        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
        strcpy(stgprs.strAPN, strTCP.szAPN);
        strcpy(stgprs.strID, strTCP.szUserName);
        strcpy(stgprs.strPW, strTCP.szPassword);


        strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP);
        stgprs.usRemotePort = strTCT.usTMSRemotePort;
#endif



        #ifdef DUAL_SIM_SETTINGS
        if(inAutoSwitchSim == 1)
        {
			memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
            strcpy(stgprs.strAPN, strTCP.szAPN);
            strcpy(stgprs.strID, strTCP.szUserName);
            strcpy(stgprs.strPW, strTCP.szPassword);
			
            if(strTCP.inSIMSlot == 1)
            {
                strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP);
                stgprs.usRemotePort = strTCT.usTMSRemotePort;
            }
            else if(strTCP.inSIMSlot == 2)
            {
                strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP2);
                stgprs.usRemotePort = strTCT.usTMSRemotePort2;            
			}
		}
        #endif

		//http://118.201.48.210:8080/redmine/issues/3320
		//To Change Production CTMS 2 Server Connection from IP to URL
#ifdef CTMS_IP_TO_URL_MODE
	    inCTOSS_COMMUrlParseIP(GPRS_MODE,stgprs.strRemoteIP);
			
#endif
		

        stgprs.ulSIMReadyTimeout = 10000;
        stgprs.ulGPRSRegTimeout = 10000;
        stgprs.usPPPRetryCounter = 5;
        stgprs.ulPPPTimeout = 10000;
		stgprs.ulTCPConnectTimeout = 10000;

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stgprs.strRemoteIP);		 
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stgprs.usRemotePort);		 
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "APN: %s", stgprs.strAPN);		 
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "USER NAME: %s", stgprs.strID);		 
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stgprs.strPW);		 
        setLCDPrint(7, DISPLAY_POSITION_LEFT, szInputBuf);
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    }
	else if(WIFI_MODE == strTCT.inTMSComMode)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("WIFI SETTINGS");
	
    	vdDebug_LogPrintf("WIFI..");
        memset(&stWF, 0x00, sizeof (CTMS_WIFIInfo));
		usRet = CTOS_CTMSGetConfig(d_CTMS_WIFI_CONFIG, &stWF);
		if (usRet != d_OK)
		{
			CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
			vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
			return d_NO;
	    }
		CTOS_LCDTClearDisplay();
		vdDispTitleString("WIFI SETTINGS");

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stWF.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stWF.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &stWF);
    }
	setLCDPrint(8, DISPLAY_POSITION_LEFT, "ANY KEY TO CONTINUE");
	CTOS_KBDGet(&ckey);
        
        lblOK:
	return d_OK;
}

int inCTOSS_CheckIntervialDateFrom2013(int y,int m,int d)
{
    int x[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    long i,s=0;
    
    for(i=2013;i<y;i++)
    {
        if(i%4==0&&i%100!=0||i%400==0)
            s+=366;
        else 
            s+=365;
    }
            
    if(y%4==0&&y%100!=0||y%400==0)
        x[2]=29;
    
    for(i=1;i<m;i++)
        s+=x[i];
        
    s+=d;

    vdDebug_LogPrintf("Date[%ld]", s);
    return s;
}


int inCTOSS_SettlementCheckTMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;

	/*if GAP set as 0, no need perform TMS*/
	if (strTCT.usTMSGap == 0)
		return d_NO;

	if(DIAL_UP_MODE == strTCT.inTMSComMode)
		return d_NO;
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;

	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;		   
	    CTOS_RTCGet(&SetRTC);
		
		vdDebug_LogPrintf("(1)inYear = %d)(bMonth = %d )[bDay=%d]",(SetRTC.bYear+2000),SetRTC.bMonth,SetRTC.bDay); 
		vdDebug_LogPrintf("(2)inYear = %d)(bMonth = %d )[bDay=%d]",inYear,inMonth,inDate); 

		inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);
		
		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}
    //check if batch settle
    //should check all application?
    vdDebug_LogPrintf("Check Batch Empty");
    if(inCTOSS_TMSChkBatchEmpty() != d_OK)
        return d_NO;

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;

    
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
    usResult = inCTOSS_TMSPreConfig(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);
	if (d_OK != usResult)
	{
		return d_NO;						
	}
//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/
	/*before start downlaad, we close current communacation device - 20151102*/
	CTOS_EthernetClose();

	/*Backup current transaction data before TMS start - 20151102*/
	inCTOSS_TMSBackupTxnData();

	//inCTOSS_TMSUploadFile("ULtestFile");

    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}

int inCTOSS_CheckIfPendingTMSDownload(void)
{
    USHORT usResult;
    USHORT usStatus, usReterr;
    BYTE bDisplayBuf[50];
    USHORT i, loop = 0;
    USHORT usComType = d_CTMS_NORMAL_MODE;


    //if(FALSE == strTCT.usTMSStatus)
    //    return d_OK;

    CTOS_LCDTClearDisplay();

    if(ETHERNET_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_GPRS;


    usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    vdDebug_LogPrintf("CTOS_CTMSGetStatus usResult[%d] usStatus[%d] usReterr[%d]", usResult, usStatus, usReterr);
    if (usResult == d_CTMS_UPDATE_FINISHED)
    {
        //strTCT.usTMSStatus = FALSE;
        //inTCTSave(1);
            
        return d_OK;
    }
    else
    {
        CTOS_CTMSUtility(usComType);
    }
}


int inCTOSS_TMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	vdDebug_LogPrintf("inCTOSS_TMSDownloadRequest");

	CTOS_LCDTClearDisplay();
	usResult = inCTOS_PromptPassword();
	if (d_OK != usResult)
	{
		return d_NO;						
	}

	CTOS_LCDTClearDisplay();
	vdDispTitleString("CTMS Init");
    CTOS_LCDTPrintXY (1,8, "Please Wait");
    //only default APP support TMS download
	#if 0	//disable checking for manual TMS update
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;
	
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;

	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;
	    CTOS_RTCGet(&SetRTC);

	    inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}
    //check if batch settle
    //should check all application?
    vdDebug_LogPrintf("Check Batch Empty");
    if(inCTOSS_TMSChkBatchEmpty() != d_OK)
        return d_NO;
	#endif

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    
    inCTOS_TMSPreConfigSetting();
    
	//if(inCTOS_TMSPreConfigSetting() != d_OK)
	//{
        //vdDisplayErrorMsg(1, 8, "USER CANCEL");
        //return d_NO;		
	//}
	
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;
	else if(WIFI_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_WIFI;

    
    vdDebug_LogPrintf("inCTOSS_TMSDownloadRequest inCTOSS_TMSPreConfig");
    usResult = inCTOSS_TMSPreConfig(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);
	if (d_OK != usResult)
	{
		return d_NO;						
	}

	/*DIAL-UP have NAC Dial-up and Mdm PPP dial-up*/
	if(DIAL_UP_MODE == strTCT.inTMSComMode && inNACDefMdm == 1)
		usComType = d_CTMS_NAC_DEFAULT_MODEM;
	
//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/
	/*before start downlaad, we close current communacation device - 20151102*/
	//CTOS_EthernetClose();

	/*Backup current transaction data before TMS start - 20151102*/
	inCTOSS_TMSBackupTxnData();

	//inCTOSS_TMSUploadFile("ULtestFile");

	/*call system API to start download*/
    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

    return d_OK;
}

void GetCTMSUpdateInfo(void)
{	
	BYTE counts = 0;
	CTMS_ItemInfo stItem;
	char szStr[150 + 1];
	USHORT usResult;
	CTOS_RTC SetRTC;
	int i = 0;
	BYTE baTemp[PAPER_X_SIZE * 64];
	CTOS_FONT_ATTRIB stFONT_ATTRIB;

	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

	usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	if (d_OK == usResult)
	{
		CTOS_PrinterPutString("LAST_UPDATE_TIME");
		sprintf(szStr, "Year:20%d,Month:%d,Day:%d", SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay);
		memset (baTemp, 0x00, sizeof(baTemp));		
		CTOS_PrinterPutString(szStr);
//		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	}
	
	//Get download information
	CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_DOWNLOAD_COUNT, &counts);
	CTOS_PrinterPutString("LAST_DOWNLOAD_COUNT");
	for(i = 0; i < counts; i++)
	{
		memset(&stItem, 0x00, sizeof(CTMS_ItemInfo));
		stItem.bNumber = i+1;
		CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_DOWNLOAD_ITME, &stItem);
		//sprintf(str, "Count:%d, Name:%s, Ver:%s, TypeName:%s", i+1, stItem.baName, stItem.baVersion, stItem.baTypeName);
		//printf(str);
		sprintf(szStr, "Count:%d, Name:%s", i+1, stItem.baName);
		memset (baTemp, 0x00, sizeof(baTemp));		
		CTOS_PrinterPutString(szStr);
//		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

		sprintf(szStr, "Ver:%s, TypeName:%s", stItem.baVersion, stItem.baTypeName);
		memset (baTemp, 0x00, sizeof(baTemp));		
		CTOS_PrinterPutString(szStr);
//		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		CTOS_PrinterPutString("---------------------------");
	}	
	//Get update information
	counts = 0;
	CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_COUNT, &counts);
	CTOS_PrinterPutString("---------------------------");
	CTOS_PrinterPutString("LAST_UPDATE_COUNT");
	for(i = 0; i < counts; i++)
	{
		memset(&stItem, 0x00, sizeof(CTMS_ItemInfo));
		stItem.bNumber = i+1;
		CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_ITEM, &stItem);
		//sprintf(str, "Count:%d, Name:%s, Ver:%s, TypeName:%s, Result:%d", i+1, stItem.baName, stItem.baVersion, stItem.baTypeName, stItem.usUpdateResult);
		//printf(str);
		sprintf(szStr, "Count:%d, Name:%s, Ver:%s", i+1, stItem.baName, stItem.baVersion);
		memset (baTemp, 0x00, sizeof(baTemp));		
		CTOS_PrinterPutString(szStr);
//		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

		sprintf(szStr, "TypeName:%s, Result:%d", stItem.baTypeName, stItem.usUpdateResult);
		memset (baTemp, 0x00, sizeof(baTemp));		
		CTOS_PrinterPutString(szStr);
//		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		CTOS_PrinterPutString("---------------------------");
	}
}


/*TMS backup the transaction data and re-store*/

/*backup transaction data should by merchant by host, so follow the MMT*/
int inCTOSS_TMSBackupTxnData(void)
{
	int inMMTNum = 0;
	int inIdx = 0;
	int inRet = d_OK;
	
	TXN_BAK_DATA		strTBD;
	extern STRUCT_MMT strMMTRec;

	vdDebug_LogPrintf("=====inCTOSS_TMSBackupTxnData=====");

	
	if (inCheckFileExist(TBD_FILE_NAME) == d_OK)
		inRemoveConfRecFile(TBD_FILE_NAME);

	CTOS_LCDTPrintXY (1, 7, "Backup Txn Data...");
	CTOS_Delay(2000);

	/*the backup txn data record depend on the merchant record*/
	inMMTNum = inMMTNumRecord();
	vdDebug_LogPrintf("inMMTNumRecord[%d]", inMMTNum);

	/*get transaction data by MMT*/
	for (inIdx=1; inIdx<=inMMTNum; inIdx++)
	{
		memset(&strTBD, 0x00, sizeof(strTBD));
		inMMTReadSelectedData(inIdx);

		vdDebug_LogPrintf("inIdx[%d]HDTid[%d] MITid[%d]", inIdx, strMMTRec.HDTid, strMMTRec.MITid);

		vdDebug_LogPrintf("szMerchantName[%s]", strMMTRec.szMerchantName);		/*HDT id*/

		/*HDT id*/
		strTBD.inHDTid = strMMTRec.HDTid;

		/*MIT id*/
		strTBD.inMITid = strMMTRec.MITid;

		/*Batch number*/
		memcpy(strTBD.szBatchNo, strMMTRec.szBatchNo, BATCH_NO_BCD_SIZE);

		/*Trace number get from HDT, (will it be set in MMT???)*/
		inHDTRead(strMMTRec.HDTid);
		memcpy(strTBD.szTraceNo, strHDT.szTraceNo, TRACE_NO_BCD_SIZE);

		DebugAddHEX("strTBD.szBatchNo", strTBD.szBatchNo, 3);
		DebugAddHEX("strTBD.szTraceNo", strTBD.szTraceNo, 3);

		/*Save to backup file -- append only*/
		inRet = inAppendConfRec(TBD_FILE_NAME, sizeof(TXN_BAK_DATA), (char *)&strTBD);
		vdDebug_LogPrintf("Save Rec[%d]", inRet);
	}

	return d_OK;
	
}

/*Restore the txn data base on Backup record*/
int inCTOSS_TMSRestoreTxnData(void)
{
	int inTBDNum = 0;
	int inMMTNum = 0;
	int inIdx = 0;
	int inRet = d_OK;
	int inFileSize = 0;
	
	TXN_BAK_DATA		strTBD;
	extern STRUCT_MMT strMMTRec;

	vdDebug_LogPrintf("=====inCTOSS_TMSRestoreTxnData=====");

	if (inCheckFileExist(TBD_FILE_NAME) != d_OK)
		return d_OK;

	CTOS_LCDTPrintXY (1, 7, "Restore Txn Data...");
	CTOS_Delay(2000);

	inTBDNum = inGetNumberOfConfRecs(TBD_FILE_NAME, sizeof(TXN_BAK_DATA));
	
	vdDebug_LogPrintf("inDB_GetTableTotalRecNum[%d]", inTBDNum);

	/*loop read record from backup file*/
	for (inIdx=1; inIdx<=inTBDNum; inIdx++)
	{
		memset(&strTBD, 0x00, sizeof(strTBD));
		inRet = inLoadConfRec(TBD_FILE_NAME, sizeof(TXN_BAK_DATA), inIdx-1, (char *)&strTBD);
		vdDebug_LogPrintf("Read TBD Tab[%d]HDTid[%d] MITid[%d]", inRet, strTBD.inHDTid, strTBD.inMITid);		
		DebugAddHEX("strTBD.szBatchNo", strTBD.szBatchNo, 3);
		DebugAddHEX("strTBD.szTraceNo", strTBD.szTraceNo, 3);

		/*Update MMT*/
		/*find the match MMT record, by HDTid and MITid*/
		inRet = inMMTReadRecord(strTBD.inHDTid, strTBD.inMITid);
		if (inRet == d_OK)//only read success then update the result
		{
			memcpy(strMMT[0].szBatchNo, strTBD.szBatchNo, BATCH_NO_BCD_SIZE);
			inMMTSave(strMMT[0].MMTid);
		}
		vdDebug_LogPrintf("updating [%s]", strMMT[0].szMerchantName);

		/*Update HDT*/
		inRet = inHDTRead(strTBD.inHDTid);
		if (inRet == d_OK)//only read success then update the result
		{
			memcpy(strHDT.szTraceNo, strTBD.szTraceNo, TRACE_NO_BCD_SIZE);
			inHDTSave(strTBD.inHDTid);
		}
		
	}

	/*after restore data, delete the backup data*/
	if (inCheckFileExist(TBD_FILE_NAME) == d_OK)
		inRemoveConfRecFile(TBD_FILE_NAME);

	return d_OK;
}


USHORT GetLastUploadState(BYTE *baFileName)
{
    USHORT usRet;
    BYTE baStr[32], bKey, i, baNewFileName[64];
    CTMS_UploadCount stCount;
    CTMS_UploadInfo stInfo;
    
    //show_LCDHead();
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 2, "  Last Upload State ");
    CTOS_LCDTPrintXY(1, 3, "====================");
    CTOS_LCDTPrintXY(1, 8, "  Please waiting... ");
    
    memset(&stCount, 0x00, sizeof(CTMS_UploadCount));
    usRet = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPLOAD_COUNT, &stCount);
    if(usRet == d_OK)
    {
        memset(baStr, 0x00, sizeof(baStr));
        sprintf(baStr, "Total Uplaod    :%03d", stCount.bTotalCount);
        CTOS_LCDTPrintXY(1, 4, baStr);
        memset(baStr, 0x00, sizeof(baStr));
        sprintf(baStr, "Uplaod Success  :%03d", stCount.bUploadCount);
        CTOS_LCDTPrintXY(1, 5, baStr);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 4, "Get UploadCount:Fail");
    }
    CTOS_LCDTPrintXY(1, 8, "       Press Any Key");
    CTOS_KBDGet(&bKey);
    
    //show_LCDHead();
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 2, "  Last Upload State ");
    CTOS_LCDTPrintXY(1, 3, "====================");
    CTOS_LCDTPrintXY(1, 8, "  Please waiting... ");

	//stCount.bTotalCount = 1;
    if(stCount.bTotalCount != 0)
    {
        i = 0;
        while(i < stCount.bTotalCount)
        {
            //Clean_LCD(4,7);
            memset(baNewFileName, 0x00, sizeof(baNewFileName));
            switch(i)
            {
                case 0:
                    strcpy(baNewFileName, baFileName);
                    break;
                case 1:
                    sprintf(baNewFileName, "C_%s", baFileName);
                    break;
            }
               
            memset(&stInfo, 0x00, sizeof(CTMS_UploadInfo));
            sprintf((BYTE *)&stInfo.baFilePath, "fs_data/%s", baNewFileName);
            CTOS_LCDTPrintXY(1, 5, baNewFileName);
            CTOS_LCDTPrintXY(1, 6, stInfo.baFilePath);
            usRet = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPLOAD_ITEM, &stInfo);
            if(usRet != d_OK)
            {
                CTOS_LCDTPrintXY(1, 4, "Get upload info fail");
                break;
            }
            else
            {
                memset(baStr, 0x00, sizeof(baStr));
                sprintf(baStr, "File : %s", baNewFileName);
                CTOS_LCDTPrintXY(1, 4, baStr);
                memset(baStr, 0x00, sizeof(baStr));
                switch(stInfo.bState)
                {
                    case d_CTMS_UL_WAIT:
                        sprintf(baStr, "State: Wait         ");
                        break;
                    case d_CTMS_UL_PROCESSING:
                        sprintf(baStr, "State: Half         ");
                        break;
                    case d_CTMS_UL_COMPLETE:
                        sprintf(baStr, "State: Complete     ");
                        break;
                    case d_CTMS_UL_FAIL:
                        sprintf(baStr, "State: Fail         ");
                        break;
                    default:
                        sprintf(baStr, "State: No Defined   ");
                }
                CTOS_LCDTPrintXY(1, 5, baStr);
                CTOS_LCDTPrintXY(1, 6, "Upload CompleteTime:");
                memset(baStr, 0x00, sizeof(baStr));
                sprintf(baStr, "%02d %02d-%02d-%02d %02d:%02d:%02d", stInfo.stUploadTime.bDoW, stInfo.stUploadTime.bYear, stInfo.stUploadTime.bMonth, stInfo.stUploadTime.bDay, stInfo.stUploadTime.bHour, stInfo.stUploadTime.bMinute, stInfo.stUploadTime.bSecond);
                CTOS_LCDTPrintXY(1, 7, baStr);
                CTOS_LCDTPrintXY(1, 8, "       Press Any Key");
                CTOS_KBDGet(&bKey);
            }
            i++;
        }
    }
    else
    {
        CTOS_LCDTPrintXY(1, 4, " No any file uplaod ");
    }
    
    if(usRet != d_OK || stCount.bTotalCount == 0)
    {
        CTOS_LCDTPrintXY(1, 8, "       Press Any Key");
        CTOS_KBDGet(&bKey);
    }

	return d_OK;
}


USHORT CreateUploadFile(BYTE *baFileName) 
{
    ULONG ulHandle;
    USHORT usRet;
    BYTE baFileContent[] = "This is CTMS NAC Upload Example.";

    CTOS_LCDTPrintXY(1, 8, "Create Upload File      ");
    usRet = CTOS_FileOpen(baFileName, d_STORAGE_FLASH, &ulHandle);
    if (usRet != d_OK) 
	{
		CTOS_Delay(200);
		usRet = CTOS_FileOpen(baFileName, d_STORAGE_FLASH, &ulHandle);
		if (usRet != d_OK) 
		    return usRet;
    }
    usRet = CTOS_FileWrite(ulHandle, baFileContent, strlen(baFileContent));
    if (usRet != d_OK) {
        return usRet;
    }
    usRet = CTOS_FileClose(ulHandle);
    if (usRet != d_OK) {
        return usRet;
    }
    return usRet;
}


void UtilityCTMS(void) 
{
	BYTE bKey;
	//ShowLCDHead("Utility Trigger");
	vduiClearBelow(2);
	CTOS_LCDTPrintXY(1, 2, "1.Util NAC Normal       ");
	CTOS_LCDTPrintXY(1, 3, "2.Util NAC GPRS         ");
	CTOS_LCDTPrintXY(1, 4, "3.Util NAC Modem        ");
	CTOS_LCDTPrintXY(1, 5, "4.Util NAC Ethernet     ");
	CTOS_LCDTPrintXY(1, 6, "5.Util NAC WiFi         ");
	while (1) {
	    CTOS_KBDGet(&bKey);
	    switch (bKey) {
	        case d_KBD_1:
	            CTOS_CTMSUtility(d_CTMS_NAC_NORMAL);
	            break;
	        case d_KBD_2:
	            CTOS_CTMSUtility(d_CTMS_NAC_DEFAULT_GPRS);
	            break;
	        case d_KBD_3:
	            CTOS_CTMSUtility(d_CTMS_NAC_DEFAULT_MODEM);
	            break;
	        case d_KBD_4:
	            CTOS_CTMSUtility(d_CTMS_NAC_DEFAULT_ETHERNET);
	            break;
	        case d_KBD_5:
	            CTOS_CTMSUtility(d_CTMS_NAC_DEFAULT_WIFI);
	            break;
	        case d_KBD_CANCEL:
	            return;
	    }
	}	
}


#define d_FDL_ERR_NOFIND 0x1005

int inCTOSS_TMSUploadFile(BYTE *baFileName)
{
	USHORT usRet, usLogCode = 3001;

	//BYTE baFileName[] = "ULtestFile";
	BYTE baUploadPath[32];

	//ShowLCDHead("CTMS NAC Upload Test");
	
	// Create Upload File
	usRet = CreateUploadFile(baFileName);
	if(usRet != d_OK)
	{
		CTOS_LCDTPrintXY(1, 8, "Create Upload File Error");
		CTOS_Delay(2000);
		//exit(1);
		return usRet;
	}
	CTOS_Delay(500);
	
	// Write Log
	CTOS_LCDTPrintXY(1, 8, "Write Log File		 ");
	usRet = CTOS_CTMSSaveLog(1, 0, 0, usLogCode, "TestLog");
	if (usRet != d_OK) {
		CTOS_LCDTPrintXY(1, 8, "Save Log Error			");
		CTOS_Delay(2000);
		//exit(1);
		return usRet;
	}
	CTOS_Delay(500);
	
	// Delete Upload File from Upload List
	CTOS_LCDTPrintXY(1, 8, "Del Upload File 	 ");
	memset(baUploadPath, 0x00, sizeof (baUploadPath));
	sprintf(baUploadPath, "fs_data/%s", baFileName);
	usRet = CTOS_CTMSSetUploadFile(baUploadPath, d_CTMS_UPLOAD_DELETE);
	/************************************************
	 
	 It will get error code when delete a file not exist in upload list.
	 Then the function return error before add file to upload list, cause always no file to upload.
	 
	 ************************************************/
	//if (usRet != d_OK) {
	if (usRet != d_OK && usRet != d_FDL_ERR_NOFIND)
	{
		CTOS_LCDTPrintXY(1, 8, "DelUploadFile Error 	");
		CTOS_Delay(2000);
		//exit(1);
		return usRet;
	}
	CTOS_Delay(500);
	
	// Set Upload File to Upload List
	CTOS_LCDTPrintXY(1, 8, "Set Upload File   ");
	usRet = CTOS_CTMSSetUploadFile(baUploadPath, d_CTMS_UPLOAD_ADD);
	if (usRet != d_OK) {
		CTOS_LCDTPrintXY(1, 8, "SetUploadFile Error ");
		CTOS_Delay(2000);
		//exit(1);
		return usRet;
	}
	// Trigger CTMS 
	//CTOS_CTMSUtility(d_CTMS_NAC_DEFAULT_ETHERNET);
	//exit(0);

	return d_OK;
}


void VdCTOSS_TMSUpload(void)
{
	USHORT usResult;

	GetLastUploadState("ULtestFile");

	CTOS_LCDTClearDisplay();
	vdDispTitleString("CTMS Upload");

#if 0	
    USHORT usComType = d_CTMS_NORMAL_MODE;
	
	inCTOS_TMSPreConfigSetting();
	
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;
#endif
    
//    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
//    usResult = inCTOSS_TMSPreConfig(usComType);
//    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

	/*before start downlaad, we close current communacation device - 20151102*/
//	CTOS_EthernetClose();


	inCTOSS_TMSUploadFile("ULtestFile");

	/*call system API to start download*/
    UtilityCTMS();
}


CTMS_TransferSize stTransSize;

USHORT GetTransInfo(void)
{
    USHORT usRet;
    BYTE bKey, baStr[21];
    
    //show_LCDHead();
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 2, " Get Transfer Size  ");
    memset(&stTransSize, 0x00, sizeof(CTMS_TransferSize));
    usRet = CTOS_CTMSGetConfig(d_CTMS_TRANSFERSIZE, &stTransSize);
    if(usRet == d_OK)
    {
        CTOS_LCDTPrintXY(1, 3, "GetTransferSize:  OK");
        memset(baStr, 0x00, sizeof(baStr));
        sprintf(baStr, "GPRS TransSize:%5d", stTransSize.GPRS_Size);
        CTOS_LCDTPrintXY(1, 4, baStr);
        memset(baStr, 0x00, sizeof(baStr));
        sprintf(baStr, "ModemTransSize:%5d", stTransSize.Modem_Size);
        CTOS_LCDTPrintXY(1, 5, baStr);
        memset(baStr, 0x00, sizeof(baStr));
        sprintf(baStr, "Eth TransSize :%5d", stTransSize.Ethernet_Size);
        CTOS_LCDTPrintXY(1, 6, baStr);
    }
    else
    {
        CTOS_LCDTPrintXY(1, 3, "GetTransferSize:Fail");
        //LCD_Print_Ret(1, 4, usRet);
    }
    CTOS_LCDTPrintXY(1, 8, "       Press Any Key");
    CTOS_KBDGet(&bKey);
    return usRet;
}


USHORT SetTransInfo(IN CTMS_TransferSize stNewTransInfo, IN BOOL boDisplay)
{
    USHORT usRet;
    BYTE bKey;
    
    if(boDisplay == TRUE)
    {
        //show_LCDHead();
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 2, " Set Transfer Size  ");
    }
    usRet = CTOS_CTMSSetConfig(d_CTMS_TRANSFERSIZE, &stNewTransInfo);
    if(boDisplay == TRUE)
    {
        if(usRet == d_OK)
        {
                CTOS_LCDTPrintXY(1, 3, "SetTransferSize:  OK");
        }
        else
        {
                CTOS_LCDTPrintXY(1, 3, "SetTransferSize:Fail");
                //LCD_Print_Ret(1, 4, usRet);
        }
        CTOS_LCDTPrintXY(1, 8, "       Press Any Key");
        CTOS_KBDGet(&bKey);
    }
    return usRet;
}


int VdCTOSS_GetSetTransInfo(void)
{
    BYTE bKey;
    USHORT usRet;
    CTMS_TransferSize stSetTransferInfo;
    
    // Get the transfer size by API.
    usRet = GetTransInfo();
    
    // Set the transfer size by API.
    memset(&stSetTransferInfo, 0x00, sizeof(CTMS_TransferSize));
    stSetTransferInfo.GPRS_Size = 12345;
    stSetTransferInfo.Modem_Size = 23456;
    stSetTransferInfo.Ethernet_Size = 34567;
    SetTransInfo(stSetTransferInfo, TRUE);
    if(usRet == d_OK)
    {
        // ReWrite original transfer size to terminal.
       // show_LCDHead();
       CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 2, "ReWrite TransferSize");
        usRet = SetTransInfo(stTransSize, FALSE);
        if(usRet == d_OK)
        {
            // ReWrite original setting success.
            CTOS_LCDTPrintXY(1, 3, "ReWriteTranSize:  OK");
        }
        else
        {
            // ReWrite original setting fail.
            CTOS_LCDTPrintXY(1, 3, "ReWriteTranSize:Fail");
            //LCD_Print_Ret(1, 4, usRet);
        }
        CTOS_LCDTPrintXY(1, 8, "       Press Any Key");
        CTOS_KBDGet(&bKey);
    }

}

int inCTOS_CheckTMSAfterAutoSettle(void) {
    CTMS_GPRSInfo stgprs;
    CTMS_ModemInfo stmodem;
    CTMS_EthernetInfo st;
    CTMS_WIFIInfo stWF;
    BYTE szSerialNum[17 + 1];
    USHORT usRet;
    USHORT usRes = 0;

    if (ETHERNET_MODE == strTCT.inTMSComMode) {
        memset(&st, 0x00, sizeof (CTMS_EthernetInfo));
        usRet = CTOS_CTMSGetConfig(d_CTMS_ETHERNET_CONFIG, &st);
        if (usRet != d_OK) {
            CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
            vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
            return d_NO;
        }

        memset(szSerialNum, 0x00, sizeof (szSerialNum));
        CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        usRes = CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &st);
    } else if (DIAL_UP_MODE == strTCT.inTMSComMode) {

        memset(&stmodem, 0x00, sizeof (CTMS_ModemInfo));
        usRet = CTOS_CTMSGetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
        if (usRet != d_OK) {
            //CTOS_LCDTPrintXY(1, 8, "  CTMS Get Modem Fail ");
            //CTOS_Delay(1500);
            CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
            vdDisplayErrorMsg(1, 8, "CTMS Get Modem Fail");
            return d_NO;
        }

        /*DIAL-UP have NAC Dial-up and Mdm PPP dial-up*/
        if (stmodem.bType == 1)
            inNACDefMdm = 1;
        else
            inNACDefMdm = 0;

        stmodem.usPPPRetryCounter = 2;
        stmodem.ulPPPTimeout = 34463;
        stmodem.ulDialTimeout = 34463;
        vdDebug_LogPrintf("strRemoteIP[%s].usRemotePort=[%d].", stmodem.strRemoteIP, stmodem.usRemotePort);

        memset(szSerialNum, 0x00, sizeof (szSerialNum));
        CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        usRes = CTOS_CTMSSetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
    } else if (GPRS_MODE == strTCT.inTMSComMode) {
        CTMS_GPRSInfo stgprs;

        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
        usRet = CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
        if (usRet != d_OK) {
            CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
            vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
            return d_NO;
        }

        stgprs.ulSIMReadyTimeout = 10000;
        stgprs.ulGPRSRegTimeout = 10000;
        stgprs.usPPPRetryCounter = 5;
        stgprs.ulPPPTimeout = 10000;
        stgprs.ulTCPConnectTimeout = 10000;

        memset(szSerialNum, 0x00, sizeof (szSerialNum));
        CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        usRes = CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    } else if (WIFI_MODE == strTCT.inTMSComMode) {

        memset(&stWF, 0x00, sizeof (CTMS_WIFIInfo));
        usRet = CTOS_CTMSGetConfig(d_CTMS_WIFI_CONFIG, &stWF);
        if (usRet != d_OK) {
            CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
            vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
            return d_NO;
        }

        memset(szSerialNum, 0x00, sizeof (szSerialNum));
        CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        usRes = CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &stWF);
    }
    
    return d_OK;
}

void vdPrintLastUpdateTime(void)
{
	BYTE strOut[64];
	
	//CTMS_UpdateInfo st;
    CTOS_RTC stCtmsRTC;
    USHORT usResult;
	
    
	usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &stCtmsRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	memset(strOut, 0x00, sizeof(strOut));
	sprintf(strOut, "CTMS LAST UPDATE:[20%d %d %d]", stCtmsRTC.bYear, stCtmsRTC.bMonth, stCtmsRTC.bDay);

	CTOS_PrinterPutString(strOut);
	
}
