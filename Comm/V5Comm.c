
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <error.h>         //perror
#include <stdio.h>
#include <errno.h>         //errno


#include "../Includes/wub_lib.h"
#include "../Includes/msg.h"
#include "../Includes/myEZLib.h"

#include "../FileModule/myFileFunc.h"
#include "../Includes/POSTypedef.h"
#include "V5Comm.h"
#include "../UI/Display.h"
#include "..\Debug\Debug.h"
#include "..\print\Print.h"


#include "..\ApTrans\MultiShareCOM.h"
#include "..\ApTrans\MultiApTrans.h"
#include "..\tcpserver\VCommTCP.h"


#define GPRS_RX_BUF_SIZE (1500) // must bigger than TCP windows size 1460
#define TCP_GPRS_STATE_NOERROR  0x00
#define TCP_GPRS_STATE_NOERROR2  0x10000000

#define TCP_GPRS_SERVER_CONNECTING  0x22
#define TCP_GPRS_STATE_SENDING2   0x42
#define TCP_GPRS_STATE_RECEIVING2 0x82
#define TCP_GPRS_STATE_DISCONNECTING2 0x102

BYTE Temp_Ip_Addr[] = "192.168.1.107";    //IP Address
BYTE Temp_Mask[] = "255.255.255.0";     //Mask
BYTE Temp_GateWay[] = "192.168.1.1";        //Gateway IP
BYTE Temp_Host_Ip[] = "192.168.1.106";  //Default Host IP 
BYTE Temp_Host_Port[] = "8012";            //Default Host Port

BYTE Temp_Phone_Num[] = "23301900,,124";        //Default Host Port

static USHORT shRet, ret;
DWORD dwStatus;
char szDebugBrffer[100];
BYTE key;
BYTE bgGPRSSocket;
int isDialUp, inTerminalCommunicationMode,isPredial;

USHORT GPRSCONNETSTATUS;
static USHORT inEthernetOpen = 0;
static USHORT inGPRSOpen = 0;
static BYTE gbPort = d_COM1;//for COM&USB communication
static int 		sockHandle = -1;

static int gFirstPower = 0;
extern USHORT usPreconnectStatus;

ULONG ulStrToLong(IN BYTE *baStr)
{
    ULONG ulValue;

    if (baStr == NULL)
        return 0;

    ulValue = 0;
    while (*baStr != 0)
    {
        if ( (*baStr >= '0') && (*baStr <= '9') )
            ulValue = (ulValue * 10) + (*baStr - '0');
        else
            return 0;

        baStr++;
    }

    return ulValue;
}


USHORT ushStrToShort(IN BYTE *baStr)
{
    return (USHORT) ulStrToLong(baStr);
}

BYTE bUnSingeLongToStr(IN ULONG ulValue, OUT BYTE *baStr)
{
    BYTE    bDigit;
    ULONG    i;

    bDigit = 0;
    i = ulValue;

    while(i)
    {
        i /= 10;
        bDigit++;
    }
    
    if(!bDigit)
        bDigit = 1;
    
    for(i = 0; i < bDigit; i++)
    {
        baStr[bDigit - 1 - i] = (BYTE)(ulValue % 10) + '0';
        ulValue /= 10;        
    }
    
    baStr[bDigit] = '\0';
    
    return bDigit;
}


int inFirstInitConnectFuncPoint(void)
{
    srCommFuncPoint.inInitComm = 0;
    srCommFuncPoint.inGetCommConfig = 0;
    srCommFuncPoint.inSetCommConfig = 0;
	srCommFuncPoint.inCheckComm = 0;
    srCommFuncPoint.inConnect = 0;
    srCommFuncPoint.inDisconnect = 0;
    srCommFuncPoint.inSendData = 0;
    srCommFuncPoint.inRecData = 0;
    srCommFuncPoint.inEndComm = 0;
    
    return ST_SUCCESS;
}

int inInitConnectFuncPoint(TRANS_DATA_TABLE *srTransPara)
{
        
    if(srTransPara->usTerminalCommunicationMode == ETHERNET_MODE)
    {   
        vdMyEZLib_LogPrintf("**usTerminalCommunicationMode ETHERNET_MODE START**");
        if( strTCT.fShareComEnable != TRUE)
        {
            srCommFuncPoint.inInitComm = inEthernet_InitEthernet;
            srCommFuncPoint.inGetCommConfig = inEthernet_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inEthernet_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inEthernet_CheckComm;
            srCommFuncPoint.inConnect = inEthernet_ConnectHost;
            srCommFuncPoint.inDisconnect = inEthernet_Disconnection;
            srCommFuncPoint.inSendData = inEthernet_SendData;
            srCommFuncPoint.inRecData = inEthernet_ReceiveData;
            srCommFuncPoint.inEndComm = inEthernet_CutOffConnection;
        }
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
        }

    }
    else if(srTransPara->usTerminalCommunicationMode == DIAL_UP_MODE)
    {
        vdMyEZLib_LogPrintf("**usTerminalCommunicationMode MODEM_MODE START**");

        if( strTCT.fShareComEnable != TRUE)
        {
        
            srCommFuncPoint.inInitComm = inModem_InitModem;
            srCommFuncPoint.inGetCommConfig = inModem_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inModem_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inModem_CheckComm;
            srCommFuncPoint.inConnect = inModem_ConnectHost;
            srCommFuncPoint.inDisconnect = inModem_Disconnection;
            srCommFuncPoint.inSendData = inModem_SendData;
            srCommFuncPoint.inRecData = inModem_ReceiveData;
            srCommFuncPoint.inEndComm = inModem_CutOffConnection;
        }
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End; 
        }
      
        
    }
    
    else if(srTransPara->usTerminalCommunicationMode == GPRS_MODE)
    {
    
        vdMyEZLib_LogPrintf("**usTerminalCommunicationMode GPRS_MODE START**");

        if( strTCT.fShareComEnable != TRUE)
        {
           
            srCommFuncPoint.inInitComm = inGPRS_InitGPRS;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inGPRS_ConnectHost;
            srCommFuncPoint.inDisconnect = inGPRS_Disconnection;
            srCommFuncPoint.inSendData = inGPRS_SendData;
            srCommFuncPoint.inRecData = inGPRS_ReceiveData;
            srCommFuncPoint.inEndComm = inGPRS_CutOffConnection;
        }
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
            srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
        }

    }
    return ST_SUCCESS;
}

int inCTOS_inDisconnect(void)
{
	int result = ST_ERROR;
	int inGetHDTDefCurrVal  = get_env_int("#CURRDEFVAL");

	
	vdDebug_LogPrintf("inCTOS_inDisconnect strHDT.inCurrencyIdx[%d], inGetHDTDefCurrVal [%d]",   strHDT.inCurrencyIdx, inGetHDTDefCurrVal);

	vdSetInit_Connect(0);
    if(VS_TRUE == strTCT.fDemo)
    {
        vdDebug_LogPrintf("DEMO Call inDisconnect!!");
        result = d_OK;
    }
    else
    {
        vdDebug_LogPrintf("start inCTOS_inDisconnect result[%d],strTCT.fShareComEnable[%d]", result,strTCT.fShareComEnable);
        if( strTCT.fShareComEnable != TRUE)
        {
            if(srCommFuncPoint.inDisconnect)
    	        result = srCommFuncPoint.inDisconnect(&srTransRec);
            vdDebug_LogPrintf("N0-inCTOS_inDisconnect result[%d]", result);

    		if(srCommFuncPoint.inEndComm)
            {      
                vdDebug_LogPrintf("srCommFuncPoint.inEndComm");
    	        result = srCommFuncPoint.inEndComm(&srTransRec);
            }
    		vdDebug_LogPrintf("inEndComm result[%d]", result);
        }
        else
        {
            if(srCommFuncPoint.inDisconnect)
                result = srCommFuncPoint.inDisconnect(&srTransRec);
            vdDebug_LogPrintf("inCTOS_inDisconnect result[%d]", result);

            //if(srCommFuncPoint.inEndComm)
            //result = srCommFuncPoint.inEndComm();
            //vdDebug_LogPrintf("inEndComm result[%d]", result);
        }

    }


	// Return default currency code based on inGetHDTDefCurrVal value
	#ifdef CBB_FIN_ROUTING
	if(inGetHDTDefCurrVal == 2 || strHDT.inCurrencyIdx == 2)
		inCSTRead(2);
	else
		inCSTRead(1);		
	#endif

	
	vdDebug_LogPrintf("inCTOS_inDisconnect END strHDT.inCurrencyIdx[%d], inGetHDTDefCurrVal [%d], result[%d]",   strHDT.inCurrencyIdx, inGetHDTDefCurrVal, result);

	return result;
}

int inCTOS_CheckInitComm(int inCommType)
{
	
	BYTE bLength;
	int result;
	BYTE tmpbabuff[d_BUFF_SIZE];

    if(VS_TRUE == strTCT.fDemo)
    {
        return ST_SUCCESS;
    }
	
	//only check ETHERNET_MODE, only need do one time INit, please check dial up and GPRS
	if(inCommType == ETHERNET_MODE)
	{    
		result= srCommFuncPoint.inInitComm(&srTransRec,ETHERNET_MODE);        
	}
	else if(inCommType == DIAL_UP_MODE)
	{
		result = srCommFuncPoint.inInitComm(&srTransRec,DIAL_UP_MODE);
	    //srCommFuncPoint.inGetCommConfig(&srTransRec);
	    //srCommFuncPoint.inSetCommConfig(&srTransRec); 

	}	
	else if(inCommType == GPRS_MODE)
	{

		result = srCommFuncPoint.inInitComm(&srTransRec,GPRS_MODE);
	    //srCommFuncPoint.inGetCommConfig(&srTransRec);
	    //srCommFuncPoint.inSetCommConfig(&srTransRec); 
    
	}
	else if(inCommType == WIFI_MODE)
	{
		result = srCommFuncPoint.inInitComm(&srTransRec,WIFI_MODE);
	}
	else if(inCommType == USB_MODE)////for COM&USB communication
	{
		srCommFuncPoint.inInitComm(&srTransRec,USB_MODE);
	}
	else if(inCommType == COM2_MODE || inCommType == COM1_MODE)
	{
		srCommFuncPoint.inInitComm(&srTransRec,inCommType);   
	}
	else if(inCommType == MDM_PPP_MODE)
	{

		srCommFuncPoint.inInitComm(&srTransRec, MDM_PPP_MODE);
	}	
	return result;
}

int inCTOS_InitComm(int inCommType)
{
    if(inCommType == ETHERNET_MODE)
    {   
        if( strTCT.fShareComEnable != TRUE)
        {
        	inEthernetOpen = 0;
            vdDebug_LogPrintf("**usTerminalCommunicationMode ETHERNET_MODE START**");
            srCommFuncPoint.inInitComm = inEthernet_InitEthernet;
            srCommFuncPoint.inGetCommConfig = inEthernet_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inEthernet_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inEthernet_CheckComm;
            srCommFuncPoint.inConnect = inEthernet_ConnectHost;
            srCommFuncPoint.inDisconnect = inEthernet_Disconnection;
            srCommFuncPoint.inSendData = inEthernet_SendData;
            srCommFuncPoint.inRecData = inEthernet_ReceiveData;
            srCommFuncPoint.inEndComm = inEthernet_CutOffConnection;
        }
        else
        {
            inGPRSOpen = 0;
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
            srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
        }
    }
    else if(inCommType == DIAL_UP_MODE)
    {
        if( strTCT.fShareComEnable != TRUE)
        {
            vdDebug_LogPrintf("**usTerminalCommunicationMode MODEM_MODE START**");
            srCommFuncPoint.inInitComm = inModem_InitModem;
            srCommFuncPoint.inGetCommConfig = inModem_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inModem_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inModem_CheckComm;
            srCommFuncPoint.inConnect = inModem_ConnectHost;
            srCommFuncPoint.inDisconnect = inModem_Disconnection;
            srCommFuncPoint.inSendData = inModem_SendData;
            srCommFuncPoint.inRecData = inModem_ReceiveData;
            srCommFuncPoint.inEndComm = inModem_CutOffConnection;
        }
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End; 
        }      
    }
    else if(inCommType == GPRS_MODE)
    {
        vdDebug_LogPrintf("**usTerminalCommunicationMode GPRS_MODE START**fShareComEnable= [%d]",strTCT.fShareComEnable);
        if( strTCT.fShareComEnable != TRUE)
        {
            srCommFuncPoint.inInitComm = inGPRS_InitGPRS;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inGPRS_ConnectHost;
            srCommFuncPoint.inDisconnect = inGPRS_Disconnection;
            srCommFuncPoint.inSendData = inGPRS_SendData;
            srCommFuncPoint.inRecData = inGPRS_ReceiveData;
            srCommFuncPoint.inEndComm = inGPRS_CutOffConnection;
        }
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
        }        
    }
	else if(inCommType == WIFI_MODE)
	{
		vdDebug_LogPrintf("**usTerminalCommunicationMode WIFI_MODE START**fShareComEnable= [%d]",strTCT.fShareComEnable);
		if( strTCT.fShareComEnable != TRUE)
		{
			srCommFuncPoint.inInitComm = inDummyInitFlow;
			srCommFuncPoint.inGetCommConfig = inDummyFlow;
			srCommFuncPoint.inSetCommConfig = inDummyFlow;
			srCommFuncPoint.inCheckComm = inDummyFlow;
			srCommFuncPoint.inConnect = inDummyFlow;
			srCommFuncPoint.inDisconnect = inDummyFlow;
			srCommFuncPoint.inSendData = inDummySendFlow;
			srCommFuncPoint.inRecData = inDummyRecvFlow;
			srCommFuncPoint.inEndComm = inDummyFlow;
		}
		else
		{
			srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
			srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
			srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
			srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
			srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
			srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
			srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
			srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
			srCommFuncPoint.inEndComm = inCTOSS_COMM_End;  
		}		 
	}
	else if(inCommType == USB_MODE)//for COM&USB communication
    {
        vdDebug_LogPrintf("**usTerminalCommunicationMode USB_MODE START**");
        srCommFuncPoint.inInitComm = inUSB_Open;
        srCommFuncPoint.inGetCommConfig = inDummyFlow;
        srCommFuncPoint.inSetCommConfig = inDummyFlow;
		srCommFuncPoint.inCheckComm = inDummyFlow;
        srCommFuncPoint.inConnect = inDummyFlow;
        srCommFuncPoint.inDisconnect = inUSB_Close;
        srCommFuncPoint.inSendData = inUSB_SendBuf;
        srCommFuncPoint.inRecData = inUSB_RecvBuf;
        srCommFuncPoint.inEndComm = inDummyFlow;
    }
	else if(inCommType == COM1_MODE || inCommType == COM2_MODE)
    {
        vdDebug_LogPrintf("**usTerminalCommunicationMode COM_MODE START**inCommType=[%d]",inCommType);
        srCommFuncPoint.inInitComm = inRS232_Open;
        srCommFuncPoint.inGetCommConfig = inDummyFlow;
        srCommFuncPoint.inSetCommConfig = inDummyFlow;
		srCommFuncPoint.inCheckComm = inDummyFlow;
        srCommFuncPoint.inConnect = inDummyFlow;
        srCommFuncPoint.inDisconnect = inRS232_Close;
        srCommFuncPoint.inSendData = inRS232_SendBuf;
        srCommFuncPoint.inRecData = inRS232_RecvBuf;
        srCommFuncPoint.inEndComm = inDummyFlow;
    }
	else if(inCommType == MDM_PPP_MODE)
    {
        vdDebug_LogPrintf("**usTerminalCommunicationMode MDM_PPP_MODE START**fShareComEnable= [%d]",strTCT.fShareComEnable);
        if( strTCT.fShareComEnable != TRUE)
        {
            srCommFuncPoint.inInitComm = NULL;
            srCommFuncPoint.inGetCommConfig = NULL;
            srCommFuncPoint.inSetCommConfig = NULL;
    		srCommFuncPoint.inCheckComm = NULL;
            srCommFuncPoint.inConnect = NULL;
            srCommFuncPoint.inDisconnect = NULL;
            srCommFuncPoint.inSendData = NULL;
            srCommFuncPoint.inRecData = NULL;
            srCommFuncPoint.inEndComm = NULL;
        }
        else
        {
            srCommFuncPoint.inInitComm = inCTOSS_COMM_Initialize;
            srCommFuncPoint.inGetCommConfig = inGPRS_GetConfigFromTable;
            srCommFuncPoint.inSetCommConfig = inGPRS_SetConnectConfig;
    		srCommFuncPoint.inCheckComm = inGPRS_CheckComm;
            srCommFuncPoint.inConnect = inCTOSS_COMM_Connect;
            srCommFuncPoint.inDisconnect = inCTOSS_COMM_Disconnect;
            srCommFuncPoint.inSendData = inCTOSS_COMM_Send;
            srCommFuncPoint.inRecData = inCTOSS_COMM_Receive;
            srCommFuncPoint.inEndComm = inCTOSS_COMM_End;
        }        
    }

    return ST_SUCCESS;
}


void vdcommGPRSDisplyState(DWORD State)
{
	 BYTE str[20];
         GPRSCONNETSTATUS=0x23B0;
	if(State == TCP_GPRS_STATE_ESTABLISHING)
		CTOS_LCDTPrintXY (1,8, "Establishing..... ");
	else if(State == TCP_GPRS_STATE_ESTABLISHED)
    {
            GPRSCONNETSTATUS=d_OK;	
            CTOS_LCDTPrintXY (1,8, "GRPS Established");
    }
	else if(State == TCP_GPRS_STATE_CONNECTING)
		CTOS_LCDTPrintXY (1,8, "GRPS Connecting ");
	else if(State == TCP_GPRS_STATE_SENDING || State == TCP_GPRS_STATE_SENDING2 )
		CTOS_LCDTPrintXY (1,8, "Sending.........");
	else if(State == TCP_GPRS_STATE_RECEIVING || State == TCP_GPRS_STATE_RECEIVING2)
		CTOS_LCDTPrintXY (1,8, "Receiving.......");
	else if(State == TCP_GPRS_STATE_DISCONNECTING || State == TCP_GPRS_STATE_DISCONNECTING2)
		CTOS_LCDTPrintXY (1,8, "Disconnecting...");
	else if(State == TCP_GPRS_STATE_ONHOOKING)
		CTOS_LCDTPrintXY (1,8, "On Hooking...   ");
	else if(State == TCP_GPRS_SERVER_CONNECTING)
		CTOS_LCDTPrintXY (1,8, "Connecting...   ");	
	else if(State == TCP_GPRS_STATE_NOERROR || State == TCP_GPRS_STATE_NOERROR2)
		CTOS_LCDTPrintXY (1,8, "OK...           ");	
	else 
	{
		sprintf(str, "Status = %X      ", State);        
		CTOS_LCDTPrintXY (1,8, str);        
    }
}


int incommGPRSCheckState(USHORT RET, int TimeOutValue)
{
    BYTE key = 0;
    DWORD State;
	
    
    if (TimeOutValue!=GPRS_TIMEOUT_DISABLE)
    	CTOS_TimeOutSet(TIMER_ID_2, TimeOutValue);
    
    while(RET == d_TCP_IO_PROCESSING)
    {
    	RET = CTOS_TCP_GPRSStatus(&State);
        vdcommGPRSDisplyState(State);

        CTOS_KBDHit(&key);
        if(key == d_KBD_CANCEL)
           break;
		
        if (TimeOutValue!=GPRS_TIMEOUT_DISABLE)
        {
			if(CTOS_TimeOutCheck(TIMER_ID_2) == d_YES)
			{
				RET=d_TCP_IO_BUSY;
				CTOS_LCDTPrintXY (1,8, "Timeout         ");
				CTOS_Delay(500);
				break;
			}
		}
	    
    }
    vdcommGPRSDisplyState(State);
    if(RET != d_OK)
    {
        return FALSE;
    }
    //CTOS_Beep();
    return TRUE;
    
}

int inGPRS_InitGPRS(TRANS_DATA_TABLE *srTransPara,int mode)
{
  BYTE key,  i;
  BYTE baIP_S[] =  "\x00\x00\x00\x00";  
  BYTE baIP_G[4];              
  BOOL bolState;   
  int RET = d_TCP_IO_PROCESSING;
  DWORD State;
	
	vdMyEZLib_LogPrintf("**inGPRS_InitGPRS START**");
    CTOS_TCP_GPRSInit( );
        
        // GPRSOpen //
	CTOS_LCDTPrintXY (1,7, "Please Wait     ");			
  	CTOS_LCDTPrintXY (1,8, "Opening GPRS....");

	CTOS_TCP_GPRSClose();        
	incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);
  	ret = CTOS_TCP_GPRSOpen(baIP_S, strTCP.szAPN, strTCP.szUserName, strTCP.szPassword);
  	bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_DISABLE);
  	if(bolState != TRUE)
  	{
	  	CTOS_LCDTPrintXY (1,8, "GPRS Init Fail" );
		CTOS_TCP_GPRSClose();
	  	CTOS_KBDGet ( &key );
        vdMyEZLib_LogPrintf(". CTOS_GPRSOpen Err=(%04x)",ret);
        vdMyEZLib_LogPrintf("**inGPRS_InitGPRS END**");
	  	return ST_ERROR;
	}
	else
	{
	    inGPRSOpen= 1;
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);//check state
		vduiClearBelow(8);
		ret = CTOS_TCP_GPRSGetIP(baIP_G);
        vdMyEZLib_LogPrintf(". CTOS_GPRSOpen Ret=(%04x)",ret);
        vdMyEZLib_LogPrintf("**inGPRS_InitGPRS END**");
		return ST_SUCCESS;
	}
}
//for Sharls_COM modem
int inGPRS_GetConfigFromTable(TRANS_DATA_TABLE *srTransPara)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	 vdMyEZLib_LogPrintf("**inGPRS_GetConfigFromTable START**");	
	 
	 vdMyEZLib_LogPrintf(". ----------GPRS Set Config---------------");
	 
	 vdMyEZLib_LogPrintf("strGPRSCarrier.strAPN[%s]", strTCP.szAPN); 
	 vdMyEZLib_LogPrintf("strGPRSCarrier.strUID[%s]", strTCP.szUserName);
	 vdMyEZLib_LogPrintf("strGPRSCarrier.strPWD[%s]", strTCP.szUserName);
	 
	 vdMyEZLib_LogPrintf("**inGPRS_GetConfigFromTable END**");

    memset(bOutBuf, 0x00, sizeof(bOutBuf));     
    usResult = inMultiAP_RunIPCCmdTypes("SHARLS_COM", d_IPC_CMD_COMM_GETCONFIG, bInBuf, usInLen, bOutBuf, &usOutLen);

    return ST_SUCCESS;
}	
//for Sharls_COM modem
int inGPRS_SetConnectConfig(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**inGPRS_ setconnectconfig START**"); 
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	 vdMyEZLib_LogPrintf("**inGPRS_GetConfigFromTable START**");	
	 
	 vdMyEZLib_LogPrintf(". ----------GPRS Set Config---------------");
	 
	 vdMyEZLib_LogPrintf("strGPRSCarrier.strAPN[%s]", strTCP.szAPN); 
	 vdMyEZLib_LogPrintf("strGPRSCarrier.strUID[%s]", strTCP.szUserName);
	 vdMyEZLib_LogPrintf("strGPRSCarrier.strPWD[%s]", strTCP.szPassword);
	 
	 vdMyEZLib_LogPrintf("**inGPRS_GetConfigFromTable END**");

	memset(bOutBuf, 0x00, sizeof(bOutBuf)); 	
	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_COM", d_IPC_CMD_COMM_SETCONFIG, bInBuf, usInLen, bOutBuf, &usOutLen);

    vdMyEZLib_LogPrintf("**inGPRS_ setconnectconfig END**");
    return ST_SUCCESS;    
}

int inGPRS_CheckComm(TRANS_DATA_TABLE *srTransPara)
{
	return ST_SUCCESS; 

	usPreconnectStatus = 1;
	if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
	{
	}

	return ST_SUCCESS; 
}

int inGPRS_ConnectHost(TRANS_DATA_TABLE *srTransPara)
{
  DWORD dwStatus;
  BOOL bolState;  
  USHORT iRetry = 0, usPort;
  BYTE strHostIP[50];


	vdMyEZLib_LogPrintf("**inGPRS_ ConnectHost START**"); 
		
    Reconnect:		
		CTOS_LCDTPrintXY(1, 8, "G Connecting... ");
		// Use default Host IP address & Port number //
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_ENABLE_VALUE);
        vduiClearBelow(8);
        //wub_ASCIIIP2hex(strHDT.strGPRSServerIP,strHostIP);
		wub_ASCIIIP2hex(strCPT.szPriTxnHostIP,strHostIP);
        //memcpy(strHostIP,strHDT.strGPRSServerIP,strlen(strHDT.strGPRSServerIP));
        vdMyEZLib_LogPrintf("strGPRSServerIP[%s]", strCPT.szPriTxnHostIP);                
        //usPort = strHDT.inGPRSPortNo;
		usPort = strCPT.inPriTxnHostPortNum;
        vdMyEZLib_LogPrintf("inGPRSPortNo[%d]", usPort);
        ret = CTOS_TCP_GPRSConnectEx(&bgGPRSSocket, strHostIP, usPort);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_ENABLE_VALUE);
		if(bolState != TRUE)
		{
		    inCTOS_inDisconnect();
			vdDisplayErrorMsg(1, 8, "Connect Fail    ");
			
			if (iRetry==1)
			{
		        CTOS_Delay(1500);//CTOS_KBDGet(&key);
			    vdMyEZLib_LogPrintf(". CTOS_GPRSConnectEx Err=(%04x)",bolState);	
                return -1;
			}
			else
			{
				iRetry+=1;
				// reinit
				CTOS_LCDTPrintXY (1,8, "Reset GPRS      ");
				CTOS_Delay(500);

				ret = srCommFuncPoint.inInitComm(&srTransRec,GPRS_MODE); 
           

                if(ret != TRUE)
				{
					vdDisplayErrorMsg (1,8, "Reset GPRS FAIL ");
                    vdMyEZLib_LogPrintf(". CTOS_GPRSConnectEx Err=(%04x)",ret);
					return -1;
				}
				else
					goto Reconnect;
			}
			
		}	
	
  vdMyEZLib_LogPrintf(". CTOS_GPRSConnectEx Ret=(%04x)",bolState);
  vdMyEZLib_LogPrintf("**inGPRS_ ConnectHost END**");   
  return ST_SUCCESS;     
}


int inGPRS_Disconnection(TRANS_DATA_TABLE *srTransPara)
{
    int shRet;
    BOOL bolState; 
    
    vdMyEZLib_LogPrintf("**inGPRS_Disconnection START**");    
    
    incommGPRSCheckState(d_TCP_IO_PROCESSING,500);//check state
    vduiClearBelow(8);    
    if(1 == inGPRSOpen)
    {
        shRet = CTOS_TCP_GPRSDisconnect(bgGPRSSocket);
        bolState = incommGPRSCheckState(shRet,500);   
    }
    vdMyEZLib_LogPrintf("**inGPRS_Disconnection END**");
    vduiClearBelow(7);
    return d_OK;
}


int inGPRS_SendData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
    ULONG ulRultSendLen;
    BYTE bySendTemp[1048];
    BOOL bolState;
    int i;
    
    vdMyEZLib_LogPrintf("**inGPRS_SendData START**");
    dwStatus = 0;

    vdMyEZLib_LogPrintf(". GPRS Send Len%d",ulSendLen);
		bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
		bySendTemp[1] = (ulSendLen & 0x000000FF);
		vdMyEZLib_LogPrintf((". *DataHead == H*"));
		vdMyEZLib_LogPrintf((". %02x%02x"),bySendTemp[0],bySendTemp[1]);
                
    
    ulRultSendLen = ulSendLen;
    
    memcpy(&bySendTemp[2], uszSendData, ulSendLen);
    
    ulRultSendLen += 2;
    
    for(i=0; i<3; i++)
	{
		CTOS_LCDTPrintXY (1,8, "Sending........." );
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);//check state
		vduiClearBelow(8);
		ret = CTOS_TCP_GPRSTx(bgGPRSSocket, bySendTemp, ulRultSendLen);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_DISABLE);
		if(bolState == TRUE)
		{
		    vdMyEZLib_LogPrintf("**inGPRS_SendData END**");	
                    return ST_SUCCESS;	  	
		}
	}

  return ST_ERROR;
}
    


int inGPRS_ReceiveData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
    int i;
    USHORT usMaxRecLen,usRealLen;
    BYTE usTempRecData[1024];
    BOOL bolState;
    
    
    vdMyEZLib_LogPrintf("**inGPRS_ReceiveData START**");
    usMaxRecLen = 0;
    usRealLen = 0;
    memset(usTempRecData, 0, sizeof(usTempRecData));
    CTOS_TimeOutSet(TIMER_ID_1, 2000);
    usMaxRecLen = GPRS_RX_BUF_SIZE;
    while(1)
    {
		CTOS_LCDTPrintXY(1, 8, "Receiving.......");
		//Check if the data is currently availabl in Ethernet , CTOS_TCP_GPRSRxReady cannot use, castle says so//
		incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);//check state
		vduiClearBelow(8);
		ret = CTOS_TCP_GPRSRx(bgGPRSSocket,&usTempRecData[usRealLen],&usMaxRecLen);
		bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_DISABLE);
                if(bolState == TRUE && usMaxRecLen>=2)
		{
		vdMyEZLib_LogPrintf(". Aft CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);   
                usRealLen += usMaxRecLen;
                memcpy(uszRecData, &usTempRecData[2], usRealLen);
		break;
		}
		else if (bolState != TRUE)
		{
		  CTOS_LCDTPrintXY(1, 8, "Rx Not Ready    ");
		  CTOS_Delay(300);
                  vdMyEZLib_LogPrintf(". CTOS_GPRSRx NOT Ready");
                  vdMyEZLib_LogPrintf(". CTOS_GPRSRxReady Err=(%04x)",shRet);
		}
                
                if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
                {
                    CTOS_LCDTPrintXY(1, 8, "Receive Timeout ");
                    vdMyEZLib_LogPrintf(". Receive timeout");
		    CTOS_Delay(1000);
                    shRet = ST_ERROR;
                    break;
                }

                CTOS_KBDHit(&key);
                if (key == d_KBD_CANCEL)
                {
                    usRealLen = ST_ERROR;
                    vdMyEZLib_LogPrintf(". Keb Return");
                    return ST_ERROR;
                }
                
                
    
 }
vdMyEZLib_LogPrintf("**inGPRS_ReceiveData END**");
return usRealLen;
}
    



int inGPRS_CutOffConnection(TRANS_DATA_TABLE *srTransPara)
{
    BOOL bolState;
    
    vdMyEZLib_LogPrintf("**inGPRS_CutOffConnection START**");
    
   	vduiClearBelow(2);

    if(1 == inGPRSOpen)
    {
    	vduiDisplayStringCenter(3,"PLEASE WAIT,");
    	vduiDisplayStringCenter(4,"CLOSING GPRS.");

    	ret = CTOS_TCP_GPRSClose();
        bolState = incommGPRSCheckState(ret,1000);
        if(bolState != TRUE)
        {
        	CTOS_LCDTPrintXY (1,8, "CLOSE ERROR     " );
         }

         inGPRSOpen = 0;
    }
    
    vdMyEZLib_LogPrintf("**inGPRS_CutOffConnection END**");
    return bolState;
}

int inModem_InitModem(TRANS_DATA_TABLE *srTransPara,int mode)
{
	BYTE bkey;
	BYTE response[128];
	USHORT value;
	int shRet;
	vdDebug_LogPrintf("**inModem_InitModem START**inParaMode=[%d],inHandShake[%d],inCountryCode[%d]",strCPT.inParaMode,strCPT.inHandShake,strCPT.inCountryCode);
    shRet = CTOS_ModemOpenEx(strCPT.inParaMode, strCPT.inHandShake, strCPT.inCountryCode);  /// d_M_MODE_SDLC_FAST	
	if (shRet != d_OK){
        vdMyEZLib_LogPrintf(". shRet OpenErr(%04x)", shRet);
        vdMyEZLib_LogPrintf("**inModem_InitModem OpenErr**");
        CTOS_KBDGet(&bkey);       
        return ST_ERROR;
    }
    
    vdDebug_LogPrintf("**inModem_InitModem END**");
    return ST_SUCCESS;
}

int inModem_GetConfigFromTable(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**inModem_GetConfigFromTable START**");
    memset(&srModemRec,0x00,sizeof(srModemRec));
    srModemRec.usDialDuration = ushStrToShort("30");
    srModemRec.usHookDuration = ushStrToShort("3");
    srModemRec.usPreDialDelayTime = ushStrToShort("0");
    srModemRec.usDialToneDetectDuration = ushStrToShort("7000");
    srModemRec.usDialToneMinOnTime = ushStrToShort("14400");
    srModemRec.usDTMFOnTime = ushStrToShort("100");
    srModemRec.usDTMFOffTime = ushStrToShort("100");
    srModemRec.usBusyToneMinTotalTime = ushStrToShort("1728");
    srModemRec.usBusyToneDeltaTime = ushStrToShort("7632");
    srModemRec.usBusyToneMinOnTime = ushStrToShort("864");
    srModemRec.usRingBackToneMinTotalTime = ushStrToShort("18000");
    srModemRec.usRingBackToneDeltaTime = ushStrToShort("61200");
    srModemRec.usRingBackToneMinOnTime = ushStrToShort("4608");
    srModemRec.usAnswerToneWaitDuration = ushStrToShort("50");
    srModemRec.usBlindDialDelayTime = ushStrToShort("2");
    srModemRec.usCarrierPresentTime = ushStrToShort("6");
    
    
    vdMyEZLib_LogPrintf(". ----------Modem Set Config---------------");
    vdMyEZLib_LogPrintf(". srModemRec.strRemotePhoneNum %s",srModemRec.strRemotePhoneNum);
    vdMyEZLib_LogPrintf(". srModemRec.usDialDuration %d",srModemRec.usDialDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usHookDuration %d",srModemRec.usHookDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usPreDialDelayTime %d",srModemRec.usPreDialDelayTime);
    vdMyEZLib_LogPrintf(". srModemRec.usDialToneDetectDuration %d",srModemRec.usDialToneDetectDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usDialToneMinOnTime %d",srModemRec.usDialToneMinOnTime);
    vdMyEZLib_LogPrintf(". srModemRec.usDTMFOnTime %d",srModemRec.usDTMFOnTime);
    vdMyEZLib_LogPrintf(". srModemRec.usDTMFOffTime %d",srModemRec.usDTMFOffTime);
    vdMyEZLib_LogPrintf(". srModemRec.usBusyToneMinTotalTime %d",srModemRec.usBusyToneMinTotalTime);
    vdMyEZLib_LogPrintf(". srModemRec.usBusyToneDeltaTime %d",srModemRec.usBusyToneDeltaTime);
    vdMyEZLib_LogPrintf(". srModemRec.usRingBackToneMinTotalTime %d",srModemRec.usRingBackToneMinTotalTime);
    vdMyEZLib_LogPrintf(". srModemRec.usRingBackToneDeltaTime %d",srModemRec.usRingBackToneDeltaTime);
    vdMyEZLib_LogPrintf(". srModemRec.usRingBackToneMinOnTime %d",srModemRec.usRingBackToneMinOnTime);
    vdMyEZLib_LogPrintf(". srModemRec.usAnswerToneWaitDuration %d",srModemRec.usAnswerToneWaitDuration);
    vdMyEZLib_LogPrintf(". srModemRec.usBlindDialDelayTime %d",srModemRec.usBlindDialDelayTime);
    vdMyEZLib_LogPrintf(". srModemRec.usCarrierPresentTime %d",srModemRec.usCarrierPresentTime);
    
    
    
    
    vdMyEZLib_LogPrintf("**inModem_GetConfigFromTable END**");
    return ST_SUCCESS;
}

int inModem_SetConnectConfig(TRANS_DATA_TABLE *srTransPara)
{
	BYTE temp_buffer[128];
	USHORT temp_len;
	int len;

	
	vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]",strMMT[0].szATCMD1,strMMT[0].szATCMD2,strMMT[0].szATCMD3,strMMT[0].szATCMD4,strMMT[0].szATCMD5);

	len = strlen(strMMT[0].szATCMD1);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD1,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD1=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD2);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD2,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD2=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD3);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD3,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD3=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD4);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD4,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD4=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}

	len = strlen(strMMT[0].szATCMD5);
	if (len > 0)
	{
		temp_len = sizeof(temp_buffer);
		memset(temp_buffer,0x00,temp_len);
	    shRet = CTOS_ModemATCommand(strMMT[0].szATCMD5,len,temp_buffer,&temp_len);
		vdDebug_LogPrintf("szATCMD5=[%d] [%d] [%s]",shRet,temp_len,temp_buffer);
	}
	
    
    return ST_SUCCESS;
}

int inModem_CheckComm(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("byTransType=[%d],szPriTxnPhoneNumber=[%s],szPriSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szPriTxnPhoneNumber,strCPT.szPriSettlePhoneNumber);

	if ((srTransPara->byTransType == SETTLE) || (srTransPara->byTransType == CLS_BATCH))
	{
		if(strlen(strCPT.szPriSettlePhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriSettlePhoneNumber);
		}
	}
	else
	{
		if(strlen(strCPT.szPriTxnPhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriTxnPhoneNumber);
		}
	}

	vdDebug_LogPrintf("default phone num =[%s],fPreDial[%d]",srModemRec.strRemotePhoneNum,strCPT.fPreDial);
	isPredial = 0;
	
	if (strCPT.fPreDial == TRUE)
	{
    	shRet = CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));
	
 		isPredial = 1;		 
 	}

	return ST_SUCCESS;
}

int inModem_ReConnectHost(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("byTransType=[%d],szSecTxnPhoneNumber=[%s],szSecSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szSecTxnPhoneNumber,strCPT.szSecSettlePhoneNumber);

	if ((srTransPara->byTransType == SETTLE) || (srTransPara->byTransType == CLS_BATCH))
	{
		if(strlen(strCPT.szSecSettlePhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szSecSettlePhoneNumber);
		}
	}
	else
	{
		if(strlen(strCPT.szSecTxnPhoneNumber)>0)
		{
			memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
			sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szSecTxnPhoneNumber);
		}
	}
		
	inCTOS_inDisconnect();
	CTOS_Delay (500);
	vdDebug_LogPrintf("default phone num =[%s]",srModemRec.strRemotePhoneNum);
	
	CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));
	vdMyEZLib_LogPrintf(" Ring Back..");
	CTOS_LCDTPrintXY(1, 7, (BYTE *)srModemRec.strRemotePhoneNum);
	CTOS_LCDTPrintXY(1, 8, "Ring Back		");


	return ST_SUCCESS;
	
}


int inModem_ConnectHost(TRANS_DATA_TABLE *srTransPara)
{
    int isListen;
    char szBabuff[20];
    USHORT usRetry = 0;
    char szDebugbuff[50+1];

	BYTE str[17];
	DWORD start;			
	DWORD status;
    
    vdMyEZLib_LogPrintf("**inModem_ConnectHost START**");
    
    isDialUp = FALSE;
    isListen = FALSE;

	if (isPredial != 1)
	{	
	    vdDebug_LogPrintf("byTransType=[%d],szPriTxnPhoneNumber=[%s],szPriSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szPriTxnPhoneNumber,strCPT.szPriSettlePhoneNumber);

		if ((srTransPara->byTransType == SETTLE) || (srTransPara->byTransType == CLS_BATCH))
		{
			if(strlen(strCPT.szPriSettlePhoneNumber)>0)
			{
				memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
				sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriSettlePhoneNumber);
			}
		}
		else
		{
			if(strlen(strCPT.szPriTxnPhoneNumber)>0)
			{
				memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
				sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szPriTxnPhoneNumber);
			}
		}
		
	    vdDebug_LogPrintf("default phone num =[%s]",srModemRec.strRemotePhoneNum);
	    shRet = CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));
	         //PREDIAL=TRUE;
	}
	else
		isPredial = 0;
	
    CTOS_LCDTPrintXY(1, 7, (BYTE *)srModemRec.strRemotePhoneNum);
    CTOS_LCDTPrintXY(1, 8, "DialUp......");
    
    /* Set 300 ms = 30*10 */
    /* Set 30000 ms = 3000*10 */
    CTOS_TimeOutSet(TIMER_ID_1, 3000);
   
    while(1)
    {   
        /* Get the status of modem */
        CTOS_ModemStatus(&dwStatus);

        {
            /* Connect success to the host */
            if (dwStatus & d_M_STATUS_DIALING)
            {
                CTOS_LCDTPrintXY(1, 8, "Dialing...      ");        
            }
            else if (dwStatus & d_M_STATUS_MODEM_ONLINE)
            {
                vdMyEZLib_LogPrintf("Modem d_M_STATUS_MODEM_ONLINE");
                CTOS_LCDTPrintXY(1, 8, "Connected       ");
                isDialUp = TRUE;
                break;
            }
            else if(dwStatus & d_M_STATUS_NO_DIAL_TONE)
            {
                vdMyEZLib_LogPrintf("Modem no dial tone");
                vdDispErrMsg("NO DIAL TONE");
                srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
                if(usRetry == 0)
                {
					CTOS_TimeOutSet(TIMER_ID_1, 3000);
                    usRetry++;
					inModem_ReConnectHost(srTransPara);
                    continue;
                }
                else
                isDialUp = FALSE;
                CTOS_Delay(1000);
                return MODEM_FAILED;
                
            }
            else if(dwStatus & d_M_STATUS_NO_CARRIER)
            {
                vdMyEZLib_LogPrintf("Modem no carrier");
                vdDispErrMsg("NO CARRIER");
                srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
                if(usRetry == 0)
                {
					CTOS_TimeOutSet(TIMER_ID_1, 3000);
                    usRetry++;
					inModem_ReConnectHost(srTransPara);
                    continue;
                }
                else
                isDialUp = FALSE;
                CTOS_Delay(1000);
                return MODEM_FAILED;
                
            }
            else if(dwStatus & d_M_STATUS_LINE_BUSY)
            {
                vdMyEZLib_LogPrintf("line busy %d", usRetry);
                vdDispErrMsg("LINE BUSY");
                srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
                if(usRetry == 0)
                {
					CTOS_TimeOutSet(TIMER_ID_1, 3000);
                    usRetry++;
					inModem_ReConnectHost(srTransPara);
                    continue;
                }
                else
                    vdMyEZLib_LogPrintf(" Remote No Answer");
                    CTOS_LCDTPrintXY(1, 8, "Remote No Answer");
                    isDialUp = FALSE;
            }
			/* Connect fail to the host */
            else{  
                //vdMyEZLib_LogPrintf("Modem fail %d", dwStatus);// even does not plug phone line still come to here
                vdMyEZLib_LogPrintf("Other Error");
                CTOS_LCDTPrintXY(1, 8, "Other Error     ");
                isDialUp = FALSE;

	            srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
	            return USER_ABORT;
            }	            
        }
                
        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL){
            vdMyEZLib_LogPrintf(". Modem d_KBD_CANCEL");
            vdDispErrMsg("USER CANCEL");
            srTransRec.shTransResult = TRANS_TERMINATE;// for no display error
            return USER_ABORT;
        }
        
        
        if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
        {
            vdMyEZLib_LogPrintf("Modem Connect TimeOut");
            CTOS_LCDTPrintXY(1, 8, "Modem Connect TimeOut");
            if(usRetry == 0)
        	{
				CTOS_TimeOutSet(TIMER_ID_1, 3000);
                usRetry++;
				inModem_ReConnectHost(srTransPara);
                continue;
            }
            else
                isDialUp = FALSE;
            return TIME_OUT;
        }
    }
    
    
    vdMyEZLib_LogPrintf("**inModem_ConnectHost END**");
    
    return ST_SUCCESS;
    
}


int inModem_Disconnection(TRANS_DATA_TABLE *srTransPara)
{
    if(isDialUp == FALSE)
    {
        vdMyEZLib_LogPrintf("**inModem_Disconnected Already**");
        return ST_SUCCESS; //check what suppose to 
    }
    vdMyEZLib_LogPrintf("**inModem_Disconnection START**");
    shRet = CTOS_ModemHookOn();
    
    vdMyEZLib_LogPrintf(". CTOS_ModemHookOn(%04x)",shRet);
    vdMyEZLib_LogPrintf("**inModem_Disconnection END**");
    isDialUp = FALSE;
    return d_OK;;
}

int inModem_SendData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
    vdMyEZLib_LogPrintf("**inModem_SendData START**");
    
    CTOS_TimeOutSet(TIMER_ID_1, 6000);/* 6000 means 60 sec  */
    
    CTOS_ModemFlushRxData();
    
    while(1)
    {
        if (CTOS_ModemTxReady()== d_OK){
        	CTOS_LCDTPrintXY(1, 8, "Sending...");
            shRet = CTOS_ModemTxData((BYTE *)uszSendData,ulSendLen);
            CTOS_Delay(1000);
            if (shRet == d_OK)
				break;
        }
        
        if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
        {
            vdMyEZLib_LogPrintf(". ModemTxData Time out");
            CTOS_LCDTPrintXY(1, 8, "Send Timeout    ");
            shRet = ST_ERROR;
            break;
        }
        
    }
    vdMyEZLib_LogPrintf(". CTOS_ModemTxData %d",shRet);
    vdMyEZLib_LogPrintf("**inModem_SendData END**");
    return shRet;
}

int inModem_ReceiveData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
    int i;
    USHORT usMaxRecLen,usRealLen;
    BYTE usTempRecData[2048];
    
    vdDebug_LogPrintf("**inModem_ReceiveData START**");
    usMaxRecLen = sizeof(usTempRecData);
    usRealLen = 0;
    memset(usTempRecData, 0, sizeof(usTempRecData));
    CTOS_TimeOutSet(TIMER_ID_1, 6000);
    
    while(1){

        //Check if the data is currently available in Modem //
        CTOS_LCDTPrintXY(1, 8, "Receiving...");
        shRet = CTOS_ModemRxReady(&usMaxRecLen);
        //if (17 < (USHORT)usMaxRecLen)
		if (1 < (USHORT)usMaxRecLen)
        {
            if (shRet == d_OK)
            {
                //Receive data via the modem channel //
                vdDebug_LogPrintf(". Bef CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
                shRet = CTOS_ModemRxData(&usTempRecData[usRealLen],&usMaxRecLen);
                vdDebug_LogPrintf(". Aft CTOS_ModemRxData usMaxRecLen(%d)",usMaxRecLen);
              
              
                usRealLen += usMaxRecLen;
                memcpy(uszRecData, usTempRecData, usRealLen);
                break;
            }else
            {
                CTOS_LCDTPrintXY(1, 8, "RxReady Failed");
                vdDebug_LogPrintf(". CTOS_ModemRxReady Err=(%04x)",shRet);
            }
        }
        
        if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
        {
            shRet = ST_ERROR;
			//Clear the receive buffer of Modem //
			CTOS_ModemFlushRxData();			
            break;
        }
  
        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL){
            usRealLen = ST_ERROR;
			//Clear the receive buffer of Modem //
			CTOS_ModemFlushRxData();			
            vdDebug_LogPrintf(". Keb Return");
            return ST_ERROR;
        }
        
    }
    
    vdMyEZLib_LogPrintf("**inModem_ReceiveData END**");
    return usRealLen;
}

int inModem_CutOffConnection(TRANS_DATA_TABLE *srTransPara)
{
    vdMyEZLib_LogPrintf("**inModem_CutOffConnection START**");
    
    shRet = CTOS_ModemCloseEx();
    
    vdMyEZLib_LogPrintf(". CTOS_ModemCloseEx Ret=(%04x)",shRet);
    vdMyEZLib_LogPrintf("**inModem_CutOffConnection END**");
    return shRet;
}

int inEthernet_InitEthernet(TRANS_DATA_TABLE *srTransPara,int mode)
{   
    shRet = CTOS_EthernetOpenEx();

	inEthernetOpen = 1;

    vdDebug_LogPrintf(". CTOS_EthernetOpenEx Ret=(%04x)",shRet);
	
	if (gFirstPower == 0)
	{//fix power on, first time connot connect
		CTOS_Delay(100);
		shRet = CTOS_EthernetClose();
		vdDebug_LogPrintf(". CTOS_EthernetClose Ret=(%04x)",shRet);
		CTOS_Delay(100);
		shRet = CTOS_EthernetOpenEx();
		gFirstPower = 1;
		inEthernetOpen = 1;
	}
	vdDebug_LogPrintf(". CTOS_EthernetOpenEx Ret=(%04x)",shRet);
	
    return shRet;    
}


int inEthernet_GetConfigFromTable(TRANS_DATA_TABLE *srTransPara)
{
    vdDebug_LogPrintf("**inEthernet_GetConfigFromTable START**");
    memset(&srEthernetRec,0,sizeof(srEthernetRec));
    
    /* Terminal Ip */
	//if(strTCP.fDHCPEnable == IPCONFIG_STATIC)
    {
	    memcpy(srEthernetRec.strLocalIP,strTCP.szTerminalIP,strlen(strTCP.szTerminalIP));
	    vdMyEZLib_LogPrintf(". srEthernetRec.strLocalIP(%s)",srEthernetRec.strLocalIP);
	    /* Mask */
	    memcpy(srEthernetRec.strMask,strTCP.szSubNetMask,strlen(strTCP.szSubNetMask));
	    /* Gateway */
	    memcpy(srEthernetRec.strGateway,strTCP.szGetWay,strlen(strTCP.szGetWay));
	    /* DHCP */
	    srEthernetRec.bDHCP = '0';
	     vdDebug_LogPrintf(". srEthernetRec.strLocalIP(%s)",srEthernetRec.strLocalIP);
	    vdDebug_LogPrintf(". srEthernetRec.strMask(%s)",srEthernetRec.strMask);
	    vdDebug_LogPrintf(". srEthernetRec.strGateway(%s)",srEthernetRec.strGateway);
    }

    /* AutoConnect */
    srEthernetRec.bAutoConn = '2';

    return ST_SUCCESS;
}

 /*Set up the configuration of Ethernet */
int inEthernet_SetConnectConfig(TRANS_DATA_TABLE *srTransPara)
{
    BYTE hPort[5];
    unsigned char const manual_connect[] = "2";
    BYTE babuff[d_BUFF_SIZE];
    
    memset(babuff, 0, sizeof(babuff));
    vdDebug_LogPrintf("**inEthernet_SetConnectConfig START**");
    vdDebug_LogPrintf("IP CONFIG = %d",strTCP.fDHCPEnable);
    if (strTCP.fDHCPEnable == IPCONFIG_DHCP)
    {
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, "1", 1);
        srEthernetRec.bDHCP = '1';
        vdDebug_LogPrintf(". Set DHCP (%d)",srEthernetRec.bDHCP);
    }
	else
    {
  		shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, "0", 1);
        srEthernetRec.bDHCP = '0';
        vdDebug_LogPrintf(". Set DHCP (%d)",srEthernetRec.bDHCP);
    }

	if(shRet != d_OK)
	{
	  	vdDebug_LogPrintf(". Set DHCP Not OK(%04x)",shRet);
        vdDisplayErrorMsg(1, 8, "Set DHCP Not OK ");
	  	CTOS_KBDGet(&key);  
	  	return ST_ERROR;
	}
	 
   
    if (strTCP.fDHCPEnable == IPCONFIG_STATIC)//is static
    {
        //Set IP Address
        vdDebug_LogPrintf(". Set IP (%s)",srEthernetRec.strLocalIP);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_IP, srEthernetRec.strLocalIP, strlen(srEthernetRec.strLocalIP));
		
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set IP Not OK(%04x)",shRet);
            vdDisplayErrorMsg(1, 8, "Set H_IP Not OK ");
            CTOS_KBDGet(&key);  
            return ST_ERROR;
        }

        // Set Mask //
        vdDebug_LogPrintf(". Set Mask (%s)",srEthernetRec.strMask);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_MASK, srEthernetRec.strMask, strlen(srEthernetRec.strMask));
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set Mask Not OK(%04x)",shRet);
            vdDisplayErrorMsg(1, 8, "Set Mask Not OK ");
            CTOS_KBDGet(&key);
            return ST_ERROR;
        }

        // Set Geteway IP //
        vdDebug_LogPrintf(". Set Gateway (%s)",srEthernetRec.strGateway);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_GATEWAY, srEthernetRec.strGateway, strlen(srEthernetRec.strGateway));
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set Gateway Not OK(%04x)",shRet);
            vdDisplayErrorMsg(1, 8, "Set GATE Not OK ");
            CTOS_KBDGet(&key);
            return ST_ERROR;
        }

        // Set DNS IP //
        vdDebug_LogPrintf(". Set DNSIP (%s)",strTCP.szHostDNS1);
        if (wub_strlen(strTCP.szHostDNS1)>0)//only set DNS if value assign, it is due to older firmware doesn't support
        {
            shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DNSIP, strTCP.szHostDNS1, strlen(strTCP.szHostDNS1));
            if(shRet != d_OK)
            {
                vdDebug_LogPrintf(". Set DNSIP Not OK(%04x)",shRet);
                vdDisplayErrorMsg(1, 8, "Set DNSIP Not OK ");
                CTOS_KBDGet(&key);
                return ST_ERROR;
            }
        }
    }
    
    vdDebug_LogPrintf(". Set AutoConn (%02x)",srEthernetRec.bAutoConn);

    shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_AUTOCON,&srEthernetRec.bAutoConn, 1);
    if(shRet != d_OK)
    {
        vdDebug_LogPrintf(". Set AutoConn Not OK(%04x)",shRet);
        vdDisplayErrorMsg(1, 8, "Set AutoConn Not OK ");
        CTOS_KBDGet(&key);
        return ST_ERROR;
    }
 
    shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_UPDATE_EXIT, babuff, 0);
    if(shRet != d_OK)
    {
        vdDebug_LogPrintf(". save Exit Not OK(%04x)",shRet);
        vdDisplayErrorMsg(1, 8, "Save Exit Not OK ");
        CTOS_KBDGet(&key);
        return ST_ERROR;
    }
     
    vdDebug_LogPrintf("**inEthernet_SetConnectConfig END**");
    return ST_SUCCESS;
}


int inEthernet_GetTerminalIP(char *szTerminalIP)
{
	BYTE bLength;
	int result;
	BYTE tmpbabuff[d_BUFF_SIZE];

	// Get the configuration value of Ethernet //
	memset(tmpbabuff,0x00,sizeof(tmpbabuff));
	bLength = sizeof(tmpbabuff);
	result = CTOS_WifiConfigGet(d_WIFI_CONFIG_IP, tmpbabuff, &bLength);
    vdDebug_LogPrintf("inEthernet_CheckComm =[%d][%s]",result,tmpbabuff);
    strcpy(szTerminalIP, tmpbabuff);
	return ST_SUCCESS;

}
int inEthernet_CheckComm(TRANS_DATA_TABLE *srTransPara)
{
	BYTE bLength;
	int result;
	BYTE tmpbabuff[d_BUFF_SIZE];

	// Get the configuration value of Ethernet //
	memset(tmpbabuff,0x00,sizeof(tmpbabuff));
	bLength = sizeof(tmpbabuff);
	result = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, tmpbabuff, &bLength);

	vdDebug_LogPrintf("inEthernet_CheckComm =[%d][%s]",result,tmpbabuff);
	
	if (strcmp(tmpbabuff,"0.0.0.0") == 0)
	{
		CTOS_LCDTClearDisplay();
		CTOS_LCDTPrintXY(1, 7, "Please Setting");
		vdDisplayErrorMsg(1, 8, "  Ethernet");

		return ST_ERROR;
	}

	return ST_SUCCESS;

}

int inEthernet_ConnectHost(TRANS_DATA_TABLE *srTransPara)
{
    BYTE byPortLen,byIpLen;
    BYTE baPort[10],bytemp[10];
    BYTE bkey;
    USHORT usRealLen;
    BYTE babuff[d_BUFF_SIZE];
	struct sockaddr_in  sockHost;
	int sockType;
	int retVal;

	DebugAddSTR("inEthernet_ConnectHost","START**...",100); 
	 
    usRealLen = 0;
    /* Host Ip */	
    byIpLen = strlen(strCPT.szPriTxnHostIP);
    memcpy(srEthernetRec.strRemoteIP,strCPT.szPriTxnHostIP,byIpLen);
    vdDebug_LogPrintf(". srEthernetRec.strRemoteIP =(%s) byIpLen =(%d)",srEthernetRec.strRemoteIP,byIpLen);

    /* Host Port */
    memset(baPort,0,sizeof(baPort));
    srEthernetRec.usRemotePort = strCPT.inPriTxnHostPortNum;
    byPortLen = bUnSingeLongToStr(srEthernetRec.usRemotePort,baPort); 
    vdDebug_LogPrintf(". Set Port (%d)  baPort =(%s) byPortLen =(%d)",strCPT.inPriTxnHostPortNum,baPort,byPortLen);

    shRet = CTOS_EthernetConnectEx(srEthernetRec.strRemoteIP, byIpLen, baPort, byPortLen);

    CTOS_LCDTPrintXY(1, 8, "Connecting...   ");
    if (shRet != d_OK)
    {
        vdDisplayErrorMsg(1, 8, "Connect Fail    ");
    	CTOS_KBDGet(&key);	
        vdDebug_LogPrintf(". CTOS_EthernetConnectEx Err=(%04x)",shRet);

    }
    
    vdDebug_LogPrintf(". CTOS_EthernetConnectEx Ret=(%04x)",shRet);

    return shRet;
}

int inEthernet_Disconnection(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("Disconnect,inEthernetOpen=[%d]",inEthernetOpen);
	
   if (1 ==inEthernetOpen)
   	{
		shRet = CTOS_EthernetDisconnect();
		//inEthernetOpen= 0;
   	}
    
    vdDebug_LogPrintf(". CTOS_EthernetDisconnect Ret=(%04x)",shRet);
    return d_OK;
}

int inEthernet_SendData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
    ULONG ulRultSendLen;
    BYTE bySendTemp[1048];
    
	//gcitra
		unsigned char szTmp[25];
		unsigned char szHex[25];
		
		vdMyEZLib_LogPrintf("**inEthernet_SendData START**");
		dwStatus = 0;
	
	/*
		{
			vdMyEZLib_LogPrintf(". Ethernet Send Len%d",ulSendLen);
			bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
			bySendTemp[1] = (ulSendLen & 0x000000FF);
			vdMyEZLib_LogPrintf((". *DataHead == H*"));
			vdMyEZLib_LogPrintf((". %02x%02x"),bySendTemp[0],bySendTemp[1]);
		}
	  */ 
		if(strCPT.inIPHeader == 1)
			sprintf((char *) szTmp, "%04X", (unsigned int)ulSendLen);
		else
			sprintf((char *) szTmp, "%04d", (int)ulSendLen);
		
			wub_str_2_hex(szTmp,szHex,4);
			memcpy(bySendTemp, szHex, 2);
	//gcitra  
    
    ulRultSendLen = ulSendLen;
    
    memcpy(&bySendTemp[2], uszSendData, ulSendLen);
    
    ulRultSendLen += 2;
    
    shRet = CTOS_EthernetFlushRxData();
    vdMyEZLib_LogPrintf(". Send CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
    
    CTOS_TimeOutSet(TIMER_ID_1, 200);
    while(1)
    {
#ifdef _TRANS_CONNECT_USE_RS232_   
        if(CTOS_RS232TxReady(DBG_PORT) == d_OK)
        {
           shRet = CTOS_RS232TxData(DBG_PORT, uszSendData, ulSendLen);
           vdMyEZLib_LogPrintf(". CTOS_RS232TxData Ret=(%04x)",shRet);
		    if (shRet == d_OK)
                break;
        }
#else        
        shRet = CTOS_EthernetTxReady(); // Wait Ethernet ready to transmit data to remote
		if (shRet == d_OK)
		{
			shRet = CTOS_EthernetTxData(bySendTemp, ulRultSendLen); // Ethernet transmit data to remote
			 vdMyEZLib_LogPrintf(". CTOS_EthernetTxData Ret=(%04x)",shRet);
			if (shRet == d_OK)
				break;
		}
#endif
     
        if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
        {
            CTOS_LCDTPrintXY(1, 8, "Tx Data Fail    ");
            CTOS_Delay(1000);
            shRet = ST_ERROR;

            vdMyEZLib_LogPrintf(". Time Out ErrRet=(%04x)",shRet);
            break;
        }
        
    }
   
    vdMyEZLib_LogPrintf("**inEthernet_SendData END**");
    return shRet;
}

int inEthernet_ReceiveData(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
    DWORD dwStatus;
    int inRealLen,inResult;
    USHORT usMaxRecLen;
    BYTE usTempRecData[1024];
    
    vdMyEZLib_LogPrintf("**inEthernet_ReceiveData START**");
    usMaxRecLen = 0;
    inRealLen = 0;
    dwStatus = 0;
    short inLen = 0;

    memset(usTempRecData,0,sizeof(usTempRecData));
    CTOS_TimeOutSet(TIMER_ID_1, 6000); //------ 1500 to 6000 -Meena 07/03/13

    while(1)
    {    
        /* Check if sockect disconnect or LAN disconnect, ROOTFS38 ok, ROOTFS35 nok */ 
        shRet = CTOS_EthernetStatus(&dwStatus);
        vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d) 1dwStatus=[%X]",shRet, dwStatus);
        if(((d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE) & dwStatus) != (d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE))
        {
            inRealLen = ST_ERROR;
            break;
        }
        
        /* Check if the data is currently availabl in Ethernet */
        shRet = CTOS_EthernetRxReady(&usMaxRecLen);

        //vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d)",shRet);
        if (shRet == d_OK && usMaxRecLen>=2){
            
            vdMyEZLib_LogPrintf(". Bef RxData usMaxRecLen(%d)",usMaxRecLen);
            /* Receive data via Ethernet channel */
            shRet = CTOS_EthernetRxData(&usTempRecData[inRealLen],&usMaxRecLen);
            
            vdMyEZLib_LogPrintf(". Aft RxData usMaxRecLen(%d)",usMaxRecLen);

            vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d) usMaxRecLen[%d]",shRet, usMaxRecLen);
            if(shRet != d_OK)
            {
                inRealLen = ST_ERROR;
                vdMyEZLib_LogPrintf(". CTOS_EthernetRxData shRet=(%04x)",shRet);
                usMaxRecLen =0;
            }
            else if(usMaxRecLen>=2)
            {
            	vdMyEZLib_LogPrintf(". Aft RxData usMaxRecLen(%d)",usMaxRecLen);
                inRealLen =  inRealLen+usMaxRecLen;
                vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData length received=(%d)",inRealLen);
                if(inLen == 0)
                {
                    inLen = ((usTempRecData[0] / 16 * 10) + usTempRecData[0] % 16) * 100;
                    inLen += (usTempRecData[1] / 16 * 10) + usTempRecData[1] % 16;
                }
                
                
                vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData length send by host=(%d)",inLen);
                if(inRealLen >= inLen)
                {
                memcpy(uszRecData,&usTempRecData[2],inRealLen);
                //shRet = CTOS_EthernetFlushRxData();
                vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
                break;
                }
            }

        }

/*
        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL){
            usRealLen = ST_ERROR;
            vdMyEZLib_LogPrintf(". Keb Return");
            shRet = CTOS_EthernetFlushRxData();
            vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
            vdDisplayErrorMsg(1, 8, "Cancel");
            return ST_ERROR;
        }
*/        
        
        if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
        {
            CTOS_EthernetFlushRxData();
            vdDebug_LogPrintf("Ethernet Connect TimeOut");
            //vdDisplayErrorMsg(1, 8, "No Response");
            //CTOS_Delay(1000);
            return ST_ERROR;
        }
        

    }

    vdMyEZLib_LogPrintf("**inEthernet_ReceiveData END**");
    return inRealLen;
}

int inEthernet_CutOffConnection(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("inEthernet_CutOffConnection,inEthernetOpen=[%d]",inEthernetOpen);

   if (1 ==inEthernetOpen)
   	{
		shRet = CTOS_EthernetClose();
		inEthernetOpen= 0;
   	}
    
    vdDebug_LogPrintf(". CTOS_EthernetClose Ret=(%04x)",shRet);
    vdDebug_LogPrintf("**inEthernet_CutOffConnection END**");
    return shRet;
}

USHORT incommSignal(void)
{
	BYTE bpStrength;
	USHORT inSignal;
	USHORT  usRtn;
        
  	usRtn=CTOS_GSMSignalQuality(&bpStrength); 	
  	if (usRtn != d_OK)
			return -1;

		if ((USHORT)bpStrength>=99)
		{
			return -1;
		}
		else
		{
			inSignal=(USHORT)bpStrength;	        
			/*if(inSignal>1 && inSignal <= 10 )
				inSignal=2;
			else if(inSignal>10 && inSignal <= 20 )
				inSignal=2;
			else if(inSignal>20 && inSignal <= 30 )
				inSignal=3;
			else if(inSignal>30)
				inSignal=4;*/
		}
		return inSignal;
}

//for COM&USB communication
int inUSB_Open(TRANS_DATA_TABLE *srTransPara,int mode)
{
	//Enable USB Function // 
	if (CTOS_USBOpen() != d_OK)
	//if (CTOS_USBHostOpen(0x0CA6,0xA010) != d_OK)
	{ 
		vdDisplayErrorMsg(1, 8, "USB Open Fail");
		return d_NO;
	}
	else
	{
		//vdDisplayErrorMsg(1, 8, "USB Open OK");

		//CTOS_LCDTPrintXY(1, 8, "                                             ");
	}
	//Cancel the currently transmission of data to host // 
	CTOS_USBTxFlush(); 
	//Clear all the data currently received from host // 
	CTOS_USBRxFlush();

	return d_OK;
}

//int inUSB_SendBuf(char *szSnedBuf,int inlen)
int inUSB_SendBuf(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
	char str[300];
	char tmpbuf[50];
	BYTE key;
	int i=0,j=1;
	int inHeader;
	ULONG ulRultSendLen = 0;
    BYTE bySendTemp[2048];
	BYTE byTempLen[10];
    BYTE byTempHEXLen[10];
	
	// Check if COM1 is ready to send data 
	//CTOS_LCDTPrintXY(1, 7, "sending...     ");
	memset(bySendTemp,0x00,sizeof(bySendTemp));
	inHeader = strCPT.inIPHeader;
	switch(inHeader)
    {
        case NO_HEADER_LEN:
            ulRultSendLen = ulSendLen;
            vdDebug_LogPrintf("---***inCTOSS_COMM_Send[%ld]",ulSendLen);
            
            memcpy(bySendTemp, uszSendData, ulSendLen);
            break;
            
        case HEX_EXCLUDE_LEN:
            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            ulRultSendLen = ulSendLen + 2;
            
            vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);

            break;

        case BCD_EXCLUDE_LEN:
            sprintf(byTempLen,"%04ld",ulSendLen);
            wub_str_2_hex(byTempLen,byTempHEXLen,4);
            memcpy(bySendTemp,byTempHEXLen,2);           
            ulRultSendLen = ulSendLen + 2;
            
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            break;

        case HEX_INCLUDE_LEN:
            ulSendLen += 2;

            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            ulRultSendLen = ulSendLen + 2;
            vdDebug_LogPrintf("---HEX_INCLUDE_LEN[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            break;
            

        case BCD_INCLUDE_LEN:            
            ulSendLen += 2;

            sprintf(byTempLen,"%04ld",ulSendLen);
            wub_str_2_hex(byTempLen,byTempHEXLen,4);
            memcpy(bySendTemp,byTempHEXLen,2);           
            ulRultSendLen = ulSendLen + 2;
            
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            
            break;

        default:
            
            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            ulRultSendLen = ulSendLen + 2;
            
            vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);

            break;
    }
	
	while (1)
	{
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			return d_NO; 
		}	
		if (CTOS_USBTxReady() == d_OK)
			break;
	}
	
	// Send data via COM1 port 
	if (CTOS_USBTxData(bySendTemp,ulRultSendLen) != d_OK)
	//if (CTOS_USBHostTxData(szSnedBuf,inlen,6000) != d_OK)
	{ 
		vdDisplayErrorMsg(1, 8, "USB Send Buf Error");
		return d_NO; 
	}
#if 0
	memset(str,0x00,sizeof(str));
	sprintf(str,"send     [%ld]       ",ulRultSendLen);
	CTOS_LCDTPrintXY(1, 7, str);
	
	memset(str,0x00,sizeof(str));
	strcpy(str,"BUF ");
	for (i=0; i<ulRultSendLen; i++)
	{
		memset(tmpbuf,0x00,sizeof(tmpbuf));
		sprintf(tmpbuf,"%02x ",bySendTemp[i]);
		strcat(str,tmpbuf);

		if (i%6 == 0 && i != 0)
		{
			CTOS_LCDTPrintXY(1, 8+j, str);
			memset(str,0x00,sizeof(str));
			strcpy(str,"BUF ");
			j++;
		}
	}
	CTOS_LCDTPrintXY(1, 8+8, str);
#endif
	return d_OK;
}

//int inUSB_RecvBuf(char *szRecvBuf,int *inlen)
int inUSB_RecvBuf(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	USHORT ret;
	USHORT len = 0;
	BYTE key;
	char str[30];
	int inRealLen;
    BYTE usTempRecData[1024];

	//CTOS_Delay(1500);
	
	while (1)
	{ 
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			break; 
		} 
		// Check if data is available in USB port // 
		//CTOS_LCDTPrintXY(1, 6, "receiving...");

		ret = CTOS_USBRxReady(&len);
		if(ret == d_OK && len) 
		{
			// Get Data from USB port 
			CTOS_USBRxData(usTempRecData,&len);
			//CTOS_USBHostRxData(szRecvBuf,&len,6000);

			//memset(str,0x00,sizeof(str));
			//sprintf(str,"receive     [%d]       ",len);
			//CTOS_LCDTPrintXY(1, 6, str);
			
			//CTOS_USBRxFlush();
			//CTOS_USBTxFlush();
			//inCTOSS_USBClose();
			//CTOS_Delay(100);
			//inCTOSS_USBOpen();
			if (strCPT.inIPHeader == NO_HEADER_LEN)
			{
				memcpy(uszRecData,usTempRecData,len);
				return len;
			}
			else
			{
				memcpy(uszRecData,&usTempRecData[2],len-2);
				return len;
			}
			return d_OK;
		} 
	}
	
	return d_NO;
	
}

//int inUSB_Close(void)
int inUSB_Close(TRANS_DATA_TABLE *srTransPara)
{
	USHORT ret;
	
	//Clear all the data currently received from host// 
	CTOS_USBRxFlush(); 
	//Disable USB Function // 
	ret = CTOS_USBClose();
	//ret = CTOS_USBHostClose();

	return ret;
}




//int inRS232_Open(ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits)
int inRS232_Open(TRANS_DATA_TABLE *srTransPara,int mode)
{
	USHORT ret;
	ULONG	ulBaudRate;

	if (mode == COM1_MODE)
		gbPort = d_COM1;
	else
		gbPort = d_COM2;

	if (srTransRec.usTerminalCommunicationMode == strCPT.inCommunicationMode)
		ulBaudRate = strCPT.inPriTxnComBaudRate;
	else
		ulBaudRate = strCPT.inSecTxnComBaudRate;
	
	vdDebug_LogPrintf("-->>inRS232_Open bPort[%d]--ulBaudRate[%d]-]", gbPort, ulBaudRate);
	if (gbPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}
	//(p_inBaudrate,'N',8,1)
	ret = CTOS_RS232Open(gbPort, ulBaudRate, 'N', 8, 1);

	vdDebug_LogPrintf("-->>inCTOSS_RS232Open bPort[%d]--ret[%d]", gbPort, ret);
		
	if(ret != d_OK) 
	{ 
		vdDisplayErrorMsg(1, 8, "Open COM Error");
		return ret;
	}
	else
	{
		//if (strTCT.byRS232DebugPort != 0){
		//	vdDisplayErrorMsg(1, 8, "Open COM OK");
			//CTOS_LCDTPrintXY(1, 8, "										     ");
	//	}
		
	}

	CTOS_RS232FlushTxBuffer(gbPort);
	CTOS_RS232FlushRxBuffer(gbPort);
/*
	//The RTS control signal from this system which indicates this system is ready to receive data
	ret = CTOS_RS232SetRTS(bPort, d_ON); 
	if (ret != d_OK) 
	{ 
		vdDisplayErrorMsg(1, 8, "V7 Rec Err");
		return ret;
	} 
	else
	{
		vdDisplayErrorMsg(1, 8, "V7 Rec OK");
		CTOS_LCDTPrintXY(1, 8, "										     ");
	}

	//The CTS status of remote host/device for the specified port // 
	ret = CTOS_RS232GetCTS(bPort); 
	if (ret != d_OK) 
	{
		vdDisplayErrorMsg(1, 8, "PC Rec Err");
		return ret;
	} 
	else
	{
		vdDisplayErrorMsg(1, 8, "PC Rec OK");
		CTOS_LCDTPrintXY(1, 8, "										     ");
	}
*/
	return d_OK;
}

#define SEND_LEN	10

//int inCRS232_SendBuf(char *szSnedBuf,int inlen)
int inRS232_SendBuf(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
	char str[50];
	BYTE key;
	int times,i;
	int inHeader;
	ULONG ulRultSendLen = 0;
    BYTE bySendTemp[2048];
	BYTE byTempLen[10];
    BYTE byTempHEXLen[10];
	
	if (gbPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}

	memset(bySendTemp,0x00,sizeof(bySendTemp));
	inHeader = strCPT.inIPHeader;
	switch(inHeader)
    {
        case NO_HEADER_LEN:
            ulRultSendLen = ulSendLen;
            vdDebug_LogPrintf("---***inRS232_SendBuf[%ld]",ulSendLen);
            
            memcpy(bySendTemp, uszSendData, ulSendLen);
            break;
            
        case HEX_EXCLUDE_LEN:
            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            ulRultSendLen = ulSendLen + 2;
            
            vdDebug_LogPrintf("---inRS232_SendBuf[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);

            break;

        case BCD_EXCLUDE_LEN:
            sprintf(byTempLen,"%04ld",ulSendLen);
            wub_str_2_hex(byTempLen,byTempHEXLen,4);
            memcpy(bySendTemp,byTempHEXLen,2);           
            ulRultSendLen = ulSendLen + 2;
            
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            break;

        case HEX_INCLUDE_LEN:
            ulSendLen += 2;

            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            ulRultSendLen = ulSendLen + 2;
            vdDebug_LogPrintf("---HEX_INCLUDE_LEN[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            break;
            

        case BCD_INCLUDE_LEN:            
            ulSendLen += 2;

            sprintf(byTempLen,"%04ld",ulSendLen);
            wub_str_2_hex(byTempLen,byTempHEXLen,4);
            memcpy(bySendTemp,byTempHEXLen,2);           
            ulRultSendLen = ulSendLen + 2;
            
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            
            break;

        default:
            
            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            ulRultSendLen = ulSendLen + 2;
            
            vdDebug_LogPrintf("---inRS232_SendBuf[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);

            break;
    }
	
	// Check if COM1 is ready to send data
	//if (strTCT.byRS232DebugPort != 0){
	//	CTOS_LCDTPrintXY(1, 7, "sending...");
	//}
	CTOS_Delay(10);
	
	CTOS_RS232FlushRxBuffer(gbPort);
	while (1)
	{
		CTOS_KBDBufFlush(); // sidumili: clear buffer of keyboard
		
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			vdDebug_LogPrintf("-->>inCTOSS_RS232SendBuf CANCEL.!!!");
			return d_NO; 
		}	
		if (CTOS_RS232TxReady(gbPort) == d_OK)
			break;
	}
/*	
	if (inlen > SEND_LEN)
	{
		times = inlen/SEND_LEN;
		for (i = 0;i<times;i++)
		{
			CTOS_RS232TxData(bPort, &szSnedBuf[i*SEND_LEN], SEND_LEN);
			vdPCIDebug_HexPrintf("send",&szSnedBuf[i*SEND_LEN],SEND_LEN);
		}

		times = inlen%SEND_LEN;
		if(times>0)
		{
			CTOS_RS232TxData(bPort, &szSnedBuf[i*SEND_LEN], times);
			vdPCIDebug_HexPrintf("send",&szSnedBuf[i*SEND_LEN],times);
		}
		
	}
	else
*/
	{
		/*************************************************/
		//sidumili:
		//vdPrintPCIDebug_HexPrintf(TRUE, "PCi100 SEND", szSnedBuf, inlen);
		/*************************************************/
	
		// Send data via COM1 port 
		if(CTOS_RS232TxData(gbPort, bySendTemp, ulRultSendLen) != d_OK) 
		{ 
			vdDisplayErrorMsg(1, 8, "COM Send Buf Error");
			//Flushing the RS232 receive buffer // 
			CTOS_RS232FlushRxBuffer(gbPort); 
			return d_NO; 
		}
	}

	//if (strTCT.byRS232DebugPort != 0){
	//	memset(str,0x00,sizeof(str));
	//	sprintf(str,"send     [%d]       ",inlen);
	//	CTOS_LCDTPrintXY(1, 7, str);
	//}
	CTOS_Delay(10);
	vdPCIDebug_HexPrintf("CTOS_RS232TxData",bySendTemp,ulRultSendLen);
	return d_OK;
}

//int inRS232_RecvBuf(char *szRecvBuf,int *inlen,int timeout)
int inRS232_RecvBuf(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	USHORT ret = d_NO;
	BYTE key;
	USHORT len = 0;
	USHORT len1=0;
	char str[50];
	int status;
	VS_BOOL fRcvdOK = FALSE;
	int inRealLen;
    BYTE usTempRecData[1024];

	
	if (gbPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}
	
	CTOS_RS232FlushTxBuffer(gbPort);
	CTOS_TimeOutSet (TIMER_ID_1 , 60*100);

	//vdDebug_LogPrintf("-->>inCTOSS_RS232RecvBuf timeout[%d]", timeout);
	//vdDebug_LogPrintf("-->>inCTOSS_RS232RecvBuf timeout*100[%d]", timeout*100);
	
	while (1)
	{
		CTOS_KBDBufFlush(); // sidumili: clear buffer of keyboard
		
		if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES){
						vdDebug_LogPrintf("-->>inCTOSS_RS232RecvBuf TIMEOUT.!!!");
            return d_NO;
		}
		
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if((key == d_KBD_CANCEL) || (key == d_USER_CANCEL))
		{ 
			CTOS_KBDBufFlush(); // sidumili: flash keyboard buffer to disable cancel key
			//break; 			// sidumili: [COMMENTED] flash keyboard buffer to disable cancel key
			vdDebug_LogPrintf("-->>inCTOSS_RS232RecvBuf CANCEL.!!!");
		}
		
		//CTOS_LCDTPrintXY(1, 8, "receiving...");
		CTOS_Delay(10);
		
		// Check if data is available in COM1 port // 
		ret = CTOS_RS232RxReady(gbPort, &len); 

		//vdDebug_LogPrintf("-->>CTOS_RS232RxReady ret[%d]--len[%d]", ret, len);
		
		//CTOS_Delay(1500);
		CTOS_Delay(100);
		
		if(ret == d_OK && len) 
		{ 
			// Get Data from COM1 port 
			CTOS_RS232RxData(gbPort, usTempRecData, &len);
			//if (usTempRecData[len-1] != 0x03)
			//{
				//CTOS_Delay(1000);
			//	CTOS_Delay(500);
			//	CTOS_RS232RxData(gbPort, &usTempRecData[len], &len1);
			//	len = len+len1;
			//}

			//if (strTCT.byRS232DebugPort != 0){
			//	memset(str,0x00,sizeof(str));
			//	sprintf(str,"receive     [%d]       ",len);
			//	CTOS_LCDTPrintXY(1, 8, str);
			//}
			CTOS_Delay(10);
			
			//*inlen = len;

			fRcvdOK = TRUE;
			
			//return d_OK;
		} 

		//sidumili
		if (fRcvdOK)
			break;
		
	}

	/*************************************************/
	//sidumili:
	//vdPrintPCIDebug_HexPrintf(FALSE, "PCi100 RCVD", szRecvBuf, len);
	/*************************************************/
		
	//sidumili:
	if (fRcvdOK)
	{
		if (strCPT.inIPHeader == NO_HEADER_LEN)
		{
			memcpy(uszRecData,usTempRecData,len);
			return len;
		}
		else
		{
			memcpy(uszRecData,&usTempRecData[2],len-2);
			return len;
		}
	}

	return d_NO;
}


//int inRS232_Close()
int inRS232_Close(TRANS_DATA_TABLE *srTransPara)
{
	USHORT ret;

	if (gbPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}
	
	ret = CTOS_RS232Close(gbPort);

	vdDebug_LogPrintf("-->>inCTOSS_RS232Close bPort[%d]--ret[%d]", gbPort, ret);

	return ret;
}

int inDummyFlow(TRANS_DATA_TABLE *srTransPara)
{
	return d_OK;
}

int inDummyInitFlow(TRANS_DATA_TABLE *srTransPara,int inMode)
{
	return d_OK;
}

int inDummySendFlow(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
	return d_OK;
}

int inDummyRecvFlow(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
	return d_OK;
}



//end for COM&USB communication


void check_Ethernet(BYTE *baValue_now,int number){
 // CONN MODE  number = 1
 // IP MODE  number = 2    
    if(number == 1)
	{
        if(*baValue_now == '0') sprintf(baValue_now, "Auto-connect");
        if(*baValue_now == '1') sprintf(baValue_now, "Not support");
        if(*baValue_now == '2') sprintf(baValue_now, "Manual");
    }
	
    if(number == 2)
	{
        if(*baValue_now == '0') sprintf(baValue_now, "Static");
        if(*baValue_now == '1') sprintf(baValue_now, "DHCP");
    }
    
}


void vdPrint_EthernetStatus(void)
{
        CTOS_RTC clock;
        char Time[30];
        int i = 0;
        BYTE Display[31][30]={0};
        BYTE bTag, baValue[20][50], pbLen[20];

		BYTE baTmpBuf[128];
		BYTE bTmpLen = 0;
    
        CTOS_RTCGet(&clock);  //Get the real time clock
        sprintf( Time, "20%02d/%02d/%02d %02d:%02d:%02d", 
                clock.bYear, clock.bMonth, clock.bDay, clock.bHour, clock.bMinute, clock.bSecond);
        //CTOS_PrinterPutString(Time);
        //CTOS_PrinterPutString("VEGA Series Terminal");
        //CTOS_PrinterFline(5);
        CTOS_PrinterPutString("[Ethernet Info]");
		inCTOSS_CapturePrinterQRCodeBuffer("[Ethernet Info]",0);
        memset(Display,0x00,sizeof(Display)); //clean the memory of Display 
        for(i=1;i<=18;i++){
        pbLen[i] = 50;
        CTOS_EthernetConfigGet ( i, baValue[i], &pbLen[i] ); 
		//CTOS_Delay(100);
        baValue[i][pbLen[i]] = '\0';
		//printf("%02d.%22s, %2d\n", i, baValue[i], pbLen[i]);
        }
		CTOS_Delay(100);
		for(i=1;i<=18;i++){
        pbLen[i] = 50;
        CTOS_EthernetConfigGet ( i, baValue[i], &pbLen[i] ); 
		//CTOS_Delay(100);
        baValue[i][pbLen[i]] = '\0';
		//printf("%02d.%22s, %2d\n", i, baValue[i], pbLen[i]);
        }
        
        check_Ethernet( baValue[14], 1); // CONN MODE  number = 1                    
		check_Ethernet( baValue[12], 2); // IP MODE  number = 2                                           
		sprintf(Display[1],"ETHERNET  : %s", baValue[10]);                    
		sprintf(Display[2],"LOCAL IP  : %s", baValue[1]);                    
		sprintf(Display[3],"MASK      : %s", baValue[2]);                    
		sprintf(Display[4],"GATEWAY IP: %s", baValue[3]);                    
		sprintf(Display[5],"HOST IP   : %s", baValue[4]);                    
		sprintf(Display[6],"HOST PORT : %s", baValue[8]);                    
		sprintf(Display[7],"MAC       : %s", baValue[11]);                    
		sprintf(Display[8],"CONN MODE : %s", baValue[14]);                    
		sprintf(Display[9],"IP MODE   : %s", baValue[12]);                    
		sprintf(Display[10],"DNS SERVER: %s", baValue[16]);                    
		//sprintf(Display[11],"HOST URL  : %s", baValue[17]);                    
		for(i=1; i<=10; i++) 
		{
			CTOS_PrinterPutString(Display[i]);
			inCTOSS_CapturePrinterQRCodeBuffer(Display[i],0);
		}
		//CTOS_PrinterFline(60);
}


int inCTOSS_PrintEthernetStatus(void)
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
int inCTOSS_GPRS_Init(void)
{
			
			 BYTE baIP_G[4];  
	 		 USHORT  usRtn;
			 BYTE baIP_S[] =  "\x00\x00\x00\x00";
			 BOOL bolState; 
			 int RET = d_TCP_IO_PROCESSING;

			//vdMyEZLib_LogPrintf("**inGPRS_InitGPRS START**");
    		//CTOS_TCP_GPRSInit( );
        
      		  // GPRSOpen //
			//CTOS_LCDTPrintXY (1,7, "Please Wait     ");			
  			//CTOS_LCDTPrintXY (1,8, "Opening GPRS....");	 

			
	        //usRtn=CTOS_TCP_GPRSClose();			 
			//vdDebug_LogPrintf("**CTOS_TCP_GPRSClose usRtn[%d]**",usRtn);


			inTCPRead(1);

			vdDebug_LogPrintf("strAPN[%s]", strTCP.szAPN); 
	 		vdDebug_LogPrintf("strUID[%s]", strTCP.szUserName);
	 		vdDebug_LogPrintf("strPWD[%s]", strTCP.szPassword);

		    //incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);	
			 
			 //usRtn = CTOS_TCP_GPRSOpen(baIP_S, strTCP.szAPN, strTCP.szUserName, strTCP.szPassword);
			 //vdDebug_LogPrintf("**CTOS_TCP_GPRSOpen usRtn[%d]**",usRtn);

			#if 0
			 bolState = incommGPRSCheckState(ret,GPRS_TIMEOUT_DISABLE);
			 if(bolState != TRUE)
			 {
				 CTOS_LCDTPrintXY (1,8, "GPRS Init Fail" );
				 CTOS_TCP_GPRSClose();
				 CTOS_KBDGet ( &key );
				 vdMyEZLib_LogPrintf(". CTOS_GPRSOpen Err=(%04x)",ret);
				 vdMyEZLib_LogPrintf("**inGPRS_InitGPRS END**");
				 return ST_ERROR;
			 }

			 else
			{
	   			 inGPRSOpen= 1;
				 incommGPRSCheckState(d_TCP_IO_PROCESSING,GPRS_TIMEOUT_DISABLE);//check state
				vduiClearBelow(8);
				ret = CTOS_TCP_GPRSGetIP(baIP_G);
        		vdMyEZLib_LogPrintf(". CTOS_GPRSOpen Ret=(%04x)",ret);
        		vdMyEZLib_LogPrintf("**inGPRS_InitGPRS END**");
				return ST_SUCCESS;
			}
			 #endif


			
#if 0
			 {
				 srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;
				 clearLine(3);
				 clearLine(4);
				 clearLine(5);
				 clearLine(6);
				 clearLine(7);
				 clearLine(8);
				 CTOS_LCDTPrintXY (1,7, "Please Wait	 ");	   
				 CTOS_LCDTPrintXY(1, 8, "Init GPRS...	  ");
				 if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) 
				 {
					
					 vdDisplayErrorMsg(1, 8, "COMM INIT ERR");
					 return;
				 }
							   
				 srCommFuncPoint.inInitComm(&srTransRec,strCPT.inCommunicationMode);
				 srCommFuncPoint.inGetCommConfig(&srTransRec);
				 srCommFuncPoint.inSetCommConfig(&srTransRec);
	 
		 }
#endif
	  return d_OK;
}







