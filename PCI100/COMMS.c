#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <iccmifare.h>
#include <tclcard.h>
#include <vwdleapi.h>


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>


#include "..\Debug\Debug.h"

#include "COMMS.h"
#include "..\Includes\POSTypedef.h"
#include "..\filemodule\myFileFunc.h"



static BYTE bPort = d_COM1;
static BYTE bV3PPort = 9;

static int bUH_Port = 0; // usb host port
#define USB_HOST_PORT	9
#define USB_PORT	8

static int inFistPoweron=1;

#define PCI100_USB		1
extern int g_inTransType;
extern int readFromCOM(int fdcom, unsigned char *rbuf, int length);

/*
void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg)
{
    
    CTOS_LCDTPrintXY(inColumn, inRow, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRow, msg);
    CTOS_Beep();
    CTOS_Delay(1500);
}
*/
void vdCTOSS_SetRS232Port(int CommPort)
{	
	if (CommPort == 0)
		bPort = -1;
	
	if (CommPort == 1)
		bPort = d_COM1;

	if (CommPort == 2)
		bPort = d_COM2;

	#ifdef PCI100_USB
	if (CommPort == 9)
	{
		vdCTOSS_SetV3PRS232Port(CommPort);
		return;
	}
	bV3PPort = -1;
	#endif

	return;
}

void vdCTOSS_SetV3PRS232Port(int CommPort)
{

	/*add for terminal USB host port*/
	#if 0
	if (CommPort == 9)
		bV3PPort = USB_HOST_PORT;

	if (CommPort == 8)
		bV3PPort = USB_PORT;

	if (CommPort == 0)
		bV3PPort = -1;
	
	if (CommPort == 1)
		bV3PPort = d_COM1;

	if (CommPort == 2)
		bV3PPort = d_COM2;
	#endif

	switch (CommPort)
	{
		case 1:
			bV3PPort = d_COM1;
			break;
		case 2:
			bV3PPort = d_COM2;
			break;
		case 8:
			bV3PPort = USB_PORT;
			break;
		case 9:
			bV3PPort = USB_HOST_PORT;
			break;
		default:
			bV3PPort = -1;
			break;
	}

	return;
}


void vdCTOSS_RNG(BYTE *baResult)
{
	CTOS_RNG(baResult);
}

int inCTOSS_USBOpen(void)
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

		CTOS_LCDTPrintXY(1, 8, "                                             ");
	}
	//Cancel the currently transmission of data to host // 
	CTOS_USBTxFlush(); 
	//Clear all the data currently received from host // 
	CTOS_USBRxFlush();

	return d_OK;
}

#define USB_SEND_LEN	1000


int inCTOSS_USBSendBuf(char *szSnedBuf,int inlen)
{
	char str[300];
	char tmpbuf[50];
	BYTE key;
	int i=0,j=1;
	int times = 0;
	
	// Check if COM1 is ready to send data 
	//CTOS_LCDTPrintXY(1, 7, "sending...     ");
	CTOS_KBDBufFlush();
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

		
	if (inlen > USB_SEND_LEN)
	{
		times = inlen/USB_SEND_LEN;
		for (i = 0;i<times;i++)
		{
			if (CTOS_USBTxData(&szSnedBuf[i*USB_SEND_LEN],USB_SEND_LEN) != d_OK)
			{ 
				vdDisplayErrorMsg(1, 8, "USB Send Buf Error");
				return d_NO; 
			}
			//vdPCIDebug_HexPrintf("send",&szSnedBuf[i*USB_SEND_LEN],USB_SEND_LEN);
		}

		times = inlen%USB_SEND_LEN;
		if(times>0)
		{
			if (CTOS_USBTxData(&szSnedBuf[i*USB_SEND_LEN],times) != d_OK)
			{ 
				vdDisplayErrorMsg(1, 8, "USB Send Buf Error");
				return d_NO; 
			}
			//vdPCIDebug_HexPrintf("send",&szSnedBuf[i*USB_SEND_LEN],times);
		}
		
	}
	else
	// Send data via COM1 port 
	if (CTOS_USBTxData(szSnedBuf,inlen) != d_OK)
	//if (CTOS_USBHostTxData(szSnedBuf,inlen,6000) != d_OK)
	{ 
		vdDisplayErrorMsg(1, 8, "USB Send Buf Error");
		return d_NO; 
	}

	#if 0
	memset(str,0x00,sizeof(str));
	sprintf(str,"send     [%d]       ",inlen);
	CTOS_LCDTPrintXY(1, 7, str);
	memset(str,0x00,sizeof(str));
	strcpy(str,"BUF ");
	for (i=0; i<inlen; i++)
	{
		memset(tmpbuf,0x00,sizeof(tmpbuf));
		sprintf(tmpbuf,"%02x ",szSnedBuf[i]);
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

int inCTOSS_USBRecvBuf(char *szRecvBuf,int *inlen)
{
	USHORT ret;
	USHORT len = 0;
	BYTE key;
	char str[30];

	CTOS_Delay(1500);
	
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
			CTOS_USBRxData(szRecvBuf,&len);
			//CTOS_USBHostRxData(szRecvBuf,&len,6000);
			*inlen = len;
			memset(str,0x00,sizeof(str));
			//sprintf(str,"receive     [%d]       ",len);
			//CTOS_LCDTPrintXY(1, 6, str);
			CTOS_USBRxFlush();
			CTOS_USBTxFlush();
			//inCTOSS_USBClose();
			//CTOS_Delay(100);
			//inCTOSS_USBOpen();
			return d_OK;
		} 
	}
	
	return d_NO;
	
}

int inCTOSS_USBRecvByte(char *szRecvBuf, int inlen)
{
	USHORT ret = d_NO;
	BYTE key;
	USHORT len = inlen;
	char str[50];

	vdDebug_LogPrintf("=====inCTOSS_USBRecvByte=====[%d]", len);	
	//CTOS_Delay(10);
	
	// Check if data is available in USB port // 
	ret = CTOS_USBRxReady(&len); 
	vdDebug_LogPrintf("CTOS_USBRxReady[%d]inlen[%d]", ret, len);
	CTOS_Delay(50);
	if(ret == d_OK && len)
	{ 
		// Get Data from USB port
		len = inlen;
		CTOS_USBRxData(szRecvBuf, &len);
		//CTOS_Delay(10);
		//*inlen = len;
		vdDebug_LogPrintf("CTOS_USBRxData[%d]", len);
		return len;
	} 

	return d_NO;
}

int inCTOSS_USBClose(void)
{
	USHORT ret;
	
	//Clear all the data currently received from host// 
	//CTOS_USBRxFlush(); 
	//Disable USB Function // 
	ret = CTOS_USBClose();
	//ret = CTOS_USBHostClose();

	return ret;
}

int inCTOSS_V3PRS232Open(ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits)
{
	USHORT ret;

	/*add USB host for V3P*/
	if (bV3PPort == USB_HOST_PORT)
	{
		ret = inCTOSS_USBHostOpenEx(ulBaudRate, bParity, bDataBits, bStopBits);
		if (ret == d_OK)
			inCTOSS_USBHostFlushEx();
		return ret;
	}
	
	if (8 == bV3PPort)
	{
		ret = inCTOSS_USBOpen();
		return ret;
	}

	if (bV3PPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}

	ret = CTOS_RS232Open(bV3PPort, ulBaudRate, bParity, bDataBits, bStopBits);
	if(ret != d_OK) 
	{ 
		vdDisplayErrorMsg(1, 8, "Open COM Error");
		return ret;
	}
	else
	{
		//vdDisplayErrorMsg(1, 8, "Open COM OK");
		//CTOS_LCDTPrintXY(1, 8, "										     ");
	}

	CTOS_RS232FlushTxBuffer(bV3PPort);
	CTOS_RS232FlushRxBuffer(bV3PPort);

	return d_OK;
}

int inCTOSS_RS232Open(ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits)
{
	USHORT ret;

	#ifdef PCI100_USB
	if (bV3PPort == USB_HOST_PORT)
	{
		return inCTOSS_V3PRS232Open(ulBaudRate, bParity, bDataBits, bStopBits);
	}
	#endif

	if (bPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}

	ret = CTOS_RS232Open(bPort, ulBaudRate, bParity, bDataBits, bStopBits);
	if(ret != d_OK) 
	{ 
		vdDisplayErrorMsg(1, 8, "Open COM Error");
		return ret;
	}
	else
	{
		//vdDisplayErrorMsg(1, 8, "Open COM OK");
		//CTOS_LCDTPrintXY(1, 8, "										     ");
	}

	CTOS_RS232FlushTxBuffer(bPort);
	CTOS_RS232FlushRxBuffer(bPort);
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

#define SEND_LEN	500

int inCTOSS_V3PRS232SendBuf(unsigned char *szSnedBuf,int inlen)
{
	char str[50];
	BYTE key;
	int times,i;
	int timeout = 1;

	int inRet = 0;

	//inPrintISOPacket(VS_TRUE, szSnedBuf, inlen);
	CTOS_KBDBufFlush();//fix CTLS trans press cancel key, cannt sent data to main ap

	// Check if COM1 is ready to send data 
	//CTOS_LCDTPrintXY(1, 7, "sending...");
	//CTOS_LCDTPrintXY(1, 8, "						   ");		
	//CTOS_Delay(50);

	/*add USB host for V3P*/
	if (bV3PPort == USB_HOST_PORT)
	{
		inRet = inCTOSS_USBHostSendBufEx(szSnedBuf, inlen);
		return inRet;
	}

	if (bV3PPort == USB_PORT)
	{
		inRet = inCTOSS_USBSendBuf(szSnedBuf, inlen);
		return inRet;
	}
	
	if (bV3PPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}
		//CTOS_LCDTPrintXY(1, 8, "                          ");
	CTOS_RS232FlushTxBuffer(bV3PPort);	

	CTOS_TimeOutSet (TIMER_ID_1 , timeout*100);

	while (1)
	{
		if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return d_NO;

		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			return d_NO; 
		}	

		//CTOS_Delay(200);
		CTOS_Delay(10);
		if (CTOS_RS232TxReady(bV3PPort) == d_OK)
			break;
	}

	{

		//CTOS_Delay(50);
		CTOS_Delay(20);

		DebugAddHEX("inCTOSS_V3PRS232SendBuf", szSnedBuf, inlen);
			
		if (inlen > SEND_LEN)
		{
			times = inlen/SEND_LEN;
			for (i = 0;i<times;i++)
			{
				CTOS_RS232TxData(bV3PPort, &szSnedBuf[i*SEND_LEN], SEND_LEN);
				vdPCIDebug_HexPrintf("send",&szSnedBuf[i*SEND_LEN],SEND_LEN);
			}

			times = inlen%SEND_LEN;
			if(times>0)
			{
				CTOS_RS232TxData(bV3PPort, &szSnedBuf[i*SEND_LEN], times);
				vdPCIDebug_HexPrintf("send",&szSnedBuf[i*SEND_LEN],times);
			}
		}
		else
		{
		// Send data via COM1 port 
		if(CTOS_RS232TxData(bV3PPort, szSnedBuf, inlen) != d_OK) 
		{ 
			DebugAddHEX("inCTOSS_V3PRS232SendBuf FAIL", szSnedBuf, inlen);

			vdDisplayErrorMsg(1, 8, "COM Send Buf Error");
			//Flushing the RS232 receive buffer // 
			CTOS_RS232FlushRxBuffer(bV3PPort); 
			return d_NO; 
		}
		}
	}
	//CTOS_Delay(10);
	
	return d_OK;
}

int inCTOSS_RS232SendBuf(char *szSnedBuf,int inlen)
{
	char str[50];
	BYTE key;
	int times,i;


	#ifdef PCI100_USB
	if (bV3PPort == USB_HOST_PORT)
	{
		return inCTOSS_V3PRS232SendBuf(szSnedBuf, inlen);
	}
	#endif
	
	if (bPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}

	// Check if COM1 is ready to send data 
	//CTOS_LCDTPrintXY(1, 7, "sending...");
	CTOS_Delay(10);
	CTOS_RS232FlushRxBuffer(bPort);
	while (1)
	{
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			return d_NO; 
		}	
		if (CTOS_RS232TxReady(bPort) == d_OK)
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
		// Send data via COM1 port 
		if(CTOS_RS232TxData(bPort, szSnedBuf, inlen) != d_OK) 
		{ 
			vdDisplayErrorMsg(1, 8, "COM Send Buf Error");
			//Flushing the RS232 receive buffer // 
			CTOS_RS232FlushRxBuffer(bPort); 
			return d_NO; 
		}
	}
	//memset(str,0x00,sizeof(str));
	//sprintf(str,"send     [%d]       ",inlen);
	//CTOS_LCDTPrintXY(1, 7, str);
	CTOS_Delay(10);
	
	return d_OK;
}

int inCTOSS_V3PRS232RecvACK(char *szRecvBuf,int *inlen,int timeout)
{
	USHORT ret = d_NO;
	BYTE key;
	USHORT len = 0;
	USHORT len1=0;
	USHORT usTotalLen=0;
	char str[50];
	VS_BOOL fWaitForAck = VS_TRUE;
    char chResp;
	ULONG tick;

	if (fWaitForAck == VS_TRUE) {
		 chResp = NAK;				/* Wait for reply till Timeout */

		CTOS_TimeOutSet (TIMER_ID_1 , timeout*100);

		do {

			if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
				return d_NO;

			// Check if Cancel key is pressed // 
			CTOS_KBDHit(&key); 
			if(key == d_KBD_CANCEL) 
			{ 
				return d_NO;
			}
			
			*inlen = 1;
			len = 1;

			/*add USB host for V3P*/
			if (bV3PPort == USB_HOST_PORT)
			{
				len = readFromCOM(bUH_Port, &chResp , len);
				if (len > 0)
					break;
			}
			else if (bV3PPort == USB_PORT)
			{
				ret = CTOS_USBRxReady(&len); 
				if (ret == d_OK)
				{	
					len= 1;
					CTOS_USBRxData(&chResp, &len);
					if (len > 0)
						break;
				}
			}
			else
			{
				ret = CTOS_RS232RxReady(bV3PPort, &len); 
				if (ret == d_OK)
				{	
					len= 1;
					CTOS_RS232RxData(bV3PPort, &chResp, &len);
					if (len > 0)
						break;
				}
			}			

			CTOS_Delay(50);	

		}while (1);
	 } else
	 chResp = ACK;

	 if (chResp != ACK)
		 return d_NO;
	 else
		 return d_OK;
	 	
}

int inCTOSS_V3PRS232RecvBuf(unsigned char *szRecvBuf,int *inlen,int timeout)
{
	USHORT ret = d_NO;
	BYTE key;
	USHORT len = 0;
	USHORT len1=0;
	USHORT usTotalLen=0;
	unsigned char str[50];
	VS_BOOL fWaitForAck = VS_TRUE;
    char chResp;
	ULONG tick;
	DWORD dwWait=0, dwWakeup=0;
	BYTE bySC_status = 0;

	if (fWaitForAck == VS_TRUE) {
		 chResp = NAK;				/* Wait for reply till Timeout */

		//CTOS_TimeOutSet (TIMER_ID_1 , timeout*100);
		CTOS_TimeOutSet (TIMER_ID_1 , 10*100);//wait ACK no need long time

		do {
			if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
				return d_NO;
			
			if (g_inTransType == 1)
			{
				dwWait = d_EVENT_KBD | d_EVENT_MSR | d_EVENT_SC;
				CTOS_SystemWait(20, dwWait, &dwWakeup);
				vdDebug_LogPrintf("=====CTOS_SystemWait 111=====");
				CTOS_SCStatus(d_SC_USER, &bySC_status);
				
				if (((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD)
					|| ((dwWakeup & d_EVENT_MSR) == d_EVENT_MSR)
					|| ((dwWakeup & d_EVENT_SC) == d_EVENT_SC) 
					|| (bySC_status & d_MK_SC_PRESENT)) 
				{
					if (((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD))
					{
						CTOS_KBDHit(&key);
					}
					
					inCTOSS_V3PRS232SendBuf(&chResp, 1);
					if (((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD))
					{
						CTOS_KBDBufPut(key);
					}
					return d_MORE_PROCESSING;				
				}
			}
			else
			{
			// Check if Cancel key is pressed // 
			CTOS_KBDHit(&key); 
			if(key == d_KBD_CANCEL) 
			{
				inCTOSS_V3PRS232SendBuf(&chResp, 1);
				return d_NO;
			}
			}

			len = 1;

			/*add USB host for V3P*/
			if (bV3PPort == USB_HOST_PORT)
			{
				len = readFromCOM(bUH_Port, &chResp , len);
				if (len > 0)
					break;
			}
			else if (bV3PPort == USB_PORT)
			{
				ret = CTOS_USBRxReady(&len); 
				if (ret == d_OK)
				{	
					len= 1;
					CTOS_USBRxData(&chResp, &len);
					if (len > 0)
						break;
				}
			}
			else
			{
				ret = CTOS_RS232RxReady(bV3PPort, &len);
				//vdDebug_LogPrintf("CTOS_RS232RxReady=[%d]",ret);
				if (ret == d_OK)
				{	
					len= 1;
					CTOS_RS232RxData(bV3PPort, &chResp, &len);
					//vdDebug_LogPrintf("CTOS_RS232RxData,len=[%d],chResp=[%02x]",len,chResp);
					if (len > 0)
						break;
				}
			}		

			CTOS_Delay(50);		
			
		}while (1);
	
	 } else
		 chResp = ACK;
	 
	 vdDebug_LogPrintf("chResp=[%02x]",chResp);

	 if (chResp != ACK)
		 return d_NO;

	CTOS_TimeOutSet (TIMER_ID_1 , timeout*100);

	usTotalLen=0;
	while (1)
	{
		if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return d_NO;
		if (g_inTransType == 1)
		{
			dwWait = d_EVENT_KBD | d_EVENT_MSR | d_EVENT_SC;
			CTOS_SystemWait(20, dwWait, &dwWakeup);
			vdDebug_LogPrintf("=====CTOS_SystemWait 222=====");
			CTOS_SCStatus(d_SC_USER, &bySC_status);
			if (((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD)
				|| ((dwWakeup & d_EVENT_MSR) == d_EVENT_MSR)
				|| ((dwWakeup & d_EVENT_SC) == d_EVENT_SC) 
				|| (bySC_status & d_MK_SC_PRESENT))
			{
				if (((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD))
				{
					CTOS_KBDHit(&key);
				}
				inCTOSS_V3PRS232SendBuf(&chResp, 1);
				if (((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD))
				{
					CTOS_KBDBufPut(key);
				}
				return d_MORE_PROCESSING;		
			}
		}
		else
		{
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{
			inCTOSS_V3PRS232SendBuf(&chResp, 1);
			return d_NO;
		}
		}
		//CTOS_LCDTClearDisplay();
		//CTOS_LCDTPrintXY(1, 7, "	         			   ");		
		//CTOS_LCDTPrintXY(1, 8, "receiving...");
//		CTOS_Delay(50);
		// Check if data is available in COM1 port // 

		/*add USB host for V3P*/
		if (bV3PPort == USB_HOST_PORT)
		{
			len = 10240;		
			len1 = 0;
			len1 = readFromCOM(bUH_Port, &szRecvBuf[usTotalLen], len);
			usTotalLen += len1;
			vdDebug_LogPrintf("usTotalLen=[%d],len1=[%d]",usTotalLen,len1);
			
			if (usTotalLen > 0)
			{
				//DebugAddHEX("CTOS_RS232RxData", szRecvBuf, usTotalLen);
				int tmplen,times=0,i;
				unsigned char tmpbuf[10000];
				#define MAX_DEBUG 200

				tmplen = usTotalLen;
				memset(tmpbuf,0x00,sizeof(tmpbuf));
				memcpy(tmpbuf,szRecvBuf,usTotalLen);
				if (tmplen > MAX_DEBUG)
				{
					times = tmplen/MAX_DEBUG;
					for (i = 0;i<times;i++)
					{
						vdPCIDebug_HexPrintf("CTOS_RS232RxData", &tmpbuf[i*MAX_DEBUG], MAX_DEBUG);
					}

					times = tmplen%MAX_DEBUG;
					if(times>0)
					{
						vdPCIDebug_HexPrintf("CTOS_RS232RxData", &tmpbuf[i*MAX_DEBUG], times);
					}
					
				}
				else
				vdPCIDebug_HexPrintf("CTOS_RS232RxData", szRecvBuf, usTotalLen);
			}
			else
				continue;

			sprintf(str, "HOST= %2X %2X %2X %2X %2X[%d]", szRecvBuf[usTotalLen-5], szRecvBuf[usTotalLen-4], szRecvBuf[usTotalLen-3], szRecvBuf[usTotalLen-2], szRecvBuf[usTotalLen-1], usTotalLen);
//			CTOS_LCDTPrintXY(1, 9, str);
			vdDebug_LogPrintf("str=[%s]",str);

				
			if (szRecvBuf[usTotalLen-2] != 0x03)
				continue;
			
			*inlen = usTotalLen;

			//inPrintISOPacket(VS_TRUE, szRecvBuf, *inlen);
			if (usTotalLen>0)
				return d_OK;
			else
				return d_NO;
		}
		else if (bV3PPort == USB_PORT)
		{
			ret = CTOS_USBRxReady(&len); 
			if (ret == d_OK)
			{	
				len1 = 10240;				
				CTOS_USBRxData(&szRecvBuf[usTotalLen], &len1);
				usTotalLen += len1;
				
				if (usTotalLen > 0)
					DebugAddHEX("CTOS_RS232RxData", szRecvBuf, usTotalLen); 			
				else
					continue;
					
				if (szRecvBuf[usTotalLen-2] != 0x03)
					continue;
				
				*inlen = usTotalLen;

				return d_OK;
			}
		}
		else
		{
			ret = CTOS_RS232RxReady(bV3PPort, &len); 
			//CTOS_Delay(1500);
			CTOS_Delay(100);
			vdDebug_LogPrintf("CTOS_RS232RxReady,ret=[%d],len=[%d]",ret,len);
			if(ret == d_OK) 
			{ 
				// Get Data from COM1 port 
				//CTOS_Delay(500);
				len1 = 10240;				
				CTOS_RS232RxData(bV3PPort, &szRecvBuf[usTotalLen], &len1);
				usTotalLen += len1;
				
				if (usTotalLen > 0)
					DebugAddHEX("CTOS_RS232RxData", szRecvBuf, usTotalLen); 			
				else
					continue;
					
				if (szRecvBuf[usTotalLen-2] != 0x03)
					continue;
				
				*inlen = usTotalLen;
			
				return d_OK;
			} 
		}
		CTOS_Delay(50);		
	}

	return d_NO;
}

int inCTOSS_RS232RecvBuf(char *szRecvBuf,int *inlen,int timeout)
{
	USHORT ret = d_NO;
	BYTE key;
	USHORT len = 0;
	USHORT len1=0;
	char str[50];

	#ifdef PCI100_USB
	if (bV3PPort == USB_HOST_PORT)
	{
		//return inCTOSS_V3PRS232RecvBuf(szRecvBuf, inlen, timeout);
		return inCTOSS_USBHostRecvBufEx(szRecvBuf, inlen, timeout);
	}
	#endif

	if (bPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}
	
	CTOS_RS232FlushTxBuffer(bPort);
	CTOS_TimeOutSet (TIMER_ID_1 , timeout*100);
	
	while (1)
	{
		if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return d_NO;
		
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			break; 
		}
		//CTOS_LCDTPrintXY(1, 8, "receiving...");
		CTOS_Delay(10);
		// Check if data is available in COM1 port // 
		ret = CTOS_RS232RxReady(bPort, &len); 
		//CTOS_Delay(1500);
		CTOS_Delay(100);
		if(ret == d_OK && len) 
		{ 
			// Get Data from COM1 port 
			CTOS_RS232RxData(bPort, szRecvBuf, &len);
			if (szRecvBuf[len-1] != 0x03)
			{
				//CTOS_Delay(1000);
				CTOS_Delay(500);
				CTOS_RS232RxData(bPort, &szRecvBuf[len], &len1);
				len = len+len1;
			}
			//memset(str,0x00,sizeof(str));
			//sprintf(str,"receive     [%d]       ",len);
			//CTOS_LCDTPrintXY(1, 8, str);
			CTOS_Delay(10);
			
			*inlen = len;

			return d_OK;
		} 
	}

	return d_NO;
}

int inCTOSS_V3PRS232RecvByte(unsigned char *szRecvBuf, int inlen)
{
	USHORT ret = d_NO;
	BYTE key;
	USHORT len = inlen;

	char str[50];

	CTOS_LCDTPrintXY(1, 7, "						   ");		
	CTOS_LCDTPrintXY(1, 8, "receiving...");

	/*add USB host for V3P*/
	if (bV3PPort == USB_HOST_PORT)
	{
		len = inlen;
		len = readFromCOM(bUH_Port, szRecvBuf , len);
		return len;
	}
	else if (bV3PPort == USB_PORT)
	{
		ret = CTOS_USBRxReady(&len); 
		if (ret == d_OK)
		{	
			len = inlen;
			CTOS_USBRxData(szRecvBuf, &len);
			return len;
		}
	}
	else
	{
		CTOS_RS232FlushTxBuffer(bV3PPort);
		
		CTOS_Delay(10);
		
		// Check if data is available in COM1 port // 
		ret = CTOS_RS232RxReady(bV3PPort, &len); 
		
		CTOS_Delay(20);
		if(ret == d_OK && len)
		{ 
			// Get Data from COM1 port
			len = inlen;
			CTOS_RS232RxData(bV3PPort, szRecvBuf, &len);
			CTOS_Delay(10);
			//*inlen = len;
			return len;
		} 
	}
	
	return d_NO;
}

int inCTOSS_RS232RecvByte(char *szRecvBuf, int inlen)
{
	USHORT ret = d_NO;
	BYTE key;
	USHORT len = inlen;

	char str[50];

	CTOS_RS232FlushTxBuffer(bPort);

	CTOS_Delay(10);
	
	// Check if data is available in COM1 port // 
	ret = CTOS_RS232RxReady(bPort, &len); 

	CTOS_Delay(20);
	if(ret == d_OK && len)
	{ 
		// Get Data from COM1 port
		len = inlen;
		CTOS_RS232RxData(bPort, szRecvBuf, &len);
		CTOS_Delay(10);
		//*inlen = len;
		return len;
	} 

	return d_NO;
}

int inCTOSS_V3PRS232Close()
{
	USHORT ret;

	/*add USB host for V3P*/
	if (bV3PPort == USB_HOST_PORT)
	{
//		return d_OK;
	
		ret = inCTOSS_USBHostCloseEx();
		return ret;
	}

	if (bV3PPort == USB_PORT)
	{
		ret = inCTOSS_USBClose();
		return ret;
	}

	if (bV3PPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}
	
	ret = CTOS_RS232Close(bV3PPort);

	return ret;
}

int inCTOSS_RS232Close()
{
	USHORT ret;

	#ifdef PCI100_USB
	if (bV3PPort == USB_HOST_PORT)
		return inCTOSS_V3PRS232Close();
	#endif

	if (bPort < 0)
	{
		vdDisplayErrorMsg(1, 8, "COM Port Error");
		return d_NO;
	}
	
	ret = CTOS_RS232Close(bPort);

	return ret;
}



int inCTOSS_USBHostOpen(void)
{
	//Enable USB Function // 
	if (CTOS_USBHostOpen(0x0CA6,0xA010) != d_OK)
	{ 
		vdDisplayErrorMsg(1, 8, "USB Open Fail");
		return d_NO;
	}
	else
	{
		vdDisplayErrorMsg(1, 8, "USB Open OK");

		CTOS_LCDTPrintXY(1, 8, "                                             ");
	}

	return d_OK;
}

int inCTOSS_USBHostSendBuf(char *szSnedBuf,int inlen)
{
	char str[300];
	char tmpbuf[50];
	BYTE key;
	int i=0,j=1;
	
	// Check if COM1 is ready to send data 
	CTOS_LCDTPrintXY(1, 7, "sending...     ");

	// Send data via COM1 port 
	if (CTOS_USBHostTxData(szSnedBuf,inlen,6000) != d_OK)
	{ 
		vdDisplayErrorMsg(1, 8, "USB Send Buf Error");
		return d_NO; 
	}

	memset(str,0x00,sizeof(str));
	sprintf(str,"send     [%d]       ",inlen);
	CTOS_LCDTPrintXY(1, 7, str);
	memset(str,0x00,sizeof(str));
	strcpy(str,"BUF ");
	for (i=0; i<inlen; i++)
	{
		memset(tmpbuf,0x00,sizeof(tmpbuf));
		sprintf(tmpbuf,"%02x ",szSnedBuf[i]);
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

	return d_OK;
}

int inCTOSS_USBHostRecvBuf(char *szRecvBuf,int *inlen)
{
	USHORT ret;
	ULONG len = 0;
	BYTE key;
	char str[30];

	CTOS_Delay(1500);
	
	while (1)
	{ 
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
			break; 
		} 
		// Check if data is available in USB port // 
		CTOS_LCDTPrintXY(1, 6, "receiving...");

		{
			// Get Data from USB port 
			CTOS_USBHostRxData((unsigned char*)szRecvBuf,&len,6000);
			*inlen = len;
			memset(str,0x00,sizeof(str));
			sprintf(str,"receive     [%ld]       ",len);
			CTOS_LCDTPrintXY(1, 6, str);

			return d_OK;
		} 
	}
	
	return d_NO;
	
}

int inCTOSS_USBHostClose(void)
{
	USHORT ret;
	
	//Disable USB Function // 
	ret = CTOS_USBHostClose();

	return ret;
}


/*Here add the code for linux use USB host*/
int uh_setportparam(int fd, int baud, int parity, int databits, int stopbits)
{
	int baudrate;
 	struct   termios   newtio;  
 	switch(baud)
 	{
 		case 300:
  			baudrate=B300;
  			break;
 		case 600:
  			baudrate=B600;
  			break;
 		case 1200:
  			baudrate=B1200;
  			break;
 		case 2400:
  			baudrate=B2400;
  			break;
 		case 4800:
  			baudrate=B4800;
  			break;
 		case 9600:
  			baudrate=B9600;
  			break;
 		case 19200:
  			baudrate=B19200;
  			break;
 		case 38400:
  			baudrate=B38400;
  			break;
		case 115200:
  			baudrate=B115200;
  			break;
 		default :
  			baudrate=B9600;  
  			break;
 	}
 	tcgetattr(fd,&newtio);    
 	bzero(&newtio,sizeof(newtio));  
 	//setting   c_cflag
 	newtio.c_cflag   &=~CSIZE;    
 	switch (databits) /*???????λ??*/
 	{  
 		case 7:  
  			newtio.c_cflag |= CS7; //7λ???λ
  			break;
		case 8:    
  			newtio.c_cflag |= CS8; //8λ???λ
  			break;  
 		default:   
  			newtio.c_cflag |= CS8;
  			break;    
 	}
	
 	switch (parity) //????У??
 	{  
 		case 'n':
 		case 'N':   
  			newtio.c_cflag &= ~PARENB;   /* Clear parity enable */
  			newtio.c_iflag &= ~INPCK;     /* Enable parity checking */
  			break; 
 		case 'o':  
 		case 'O':    
  			newtio.c_cflag |= (PARODD | PARENB); /* ???????Ч??*/ 
  			newtio.c_iflag |= INPCK;             /* Disnable parity checking */
  			break; 
 		case 'e': 
 		case 'E':  
  			newtio.c_cflag |= PARENB;     /* Enable parity */   
			newtio.c_cflag &= ~PARODD;   /* ?????Ч??*/    
  			newtio.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's':  /*as no parity*/  
			newtio.c_cflag &= ~PARENB;
			newtio.c_cflag &= ~CSTOPB;break; 
		default:  
			newtio.c_cflag &= ~PARENB;   /* Clear parity enable */
  			newtio.c_iflag &= ~INPCK;     /* Enable parity checking */
  			break;   
	}
	
	switch (stopbits)//??????λ
 	{  
		case 1:   
  			newtio.c_cflag &= ~CSTOPB;
  			break; 
 		case 2:   
  			newtio.c_cflag |= CSTOPB;
    		break;
		default: 
  			newtio.c_cflag &= ~CSTOPB; 
  			break; 
 	}
	
	newtio.c_cc[VTIME] = 0;   
	newtio.c_cc[VMIN] = 0;
	newtio.c_cflag   |=   (CLOCAL|CREAD);
	newtio.c_oflag|=OPOST;
	newtio.c_iflag   &=~(IXON|IXOFF|IXANY);                    
    cfsetispeed(&newtio,baudrate);  
    cfsetospeed(&newtio,baudrate);  
    tcflush(fd,   TCIFLUSH);
	
	if (tcsetattr(fd,TCSANOW,&newtio) != 0)  
	{
		perror("SetupSerial 3"); 
		return -1; 
	} 
	return 0;
}

int inCTOSS_USBHostOpenEx(ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits)
{
	int inRet = d_NO;
	int inACMNumber = 10;
	unsigned char uszACMNumber[50+1];
	BYTE key;
//#ifdef PCI100_USB
if(strTCT.byPinPadType == 1)
{
	do 
    {
    	inRet = CTOS_USBHostOpen(0x0ca6, 0xa010);
        if (inRet == d_OK)
        {
            break;
        }
        CTOS_KBDHit(&key);
        CTOS_Delay(500);
    } while (key != d_KBD_CANCEL && key != d_KBD_ENTER);
	vdDebug_LogPrintf("inCTOSS_USBHostOpenEx inRet[%d]", inRet);
	if (0 == inRet)
		return d_OK;
	else
		return d_NO; 
}	
//#else
	vdDebug_LogPrintf("inCTOSS_USBHostOpenEx bUH_Port[%d]", bUH_Port);
	/*check if it was opened*/
	if (bUH_Port > 0)
		return d_OK;

	for (inACMNumber = 10; inACMNumber >= 0; inACMNumber--)
	{
		memset(uszACMNumber, 0x00, sizeof(uszACMNumber));
		sprintf(uszACMNumber, "/dev/ttyACM%d", inACMNumber);

//		CTOS_LCDTPrintXY(1, 3, uszACMNumber);
		
		bUH_Port = openCOM(uszACMNumber);
		if (bUH_Port > 0)
			break;
	}
	
	if (inFistPoweron == 1)
	{
		vdDebug_LogPrintf("inCTOSS_USBHostOpenEx bUH_Port[%d]", bUH_Port);
		if (bUH_Port > 0)
		{
			closeCOM(bUH_Port);
			bUH_Port = 0;
			inFistPoweron = 0;
			
			for (inACMNumber = 10; inACMNumber >= 0; inACMNumber--)
			{
				memset(uszACMNumber, 0x00, sizeof(uszACMNumber));
				sprintf(uszACMNumber, "/dev/ttyACM%d", inACMNumber);
			
//				CTOS_LCDTPrintXY(1, 3, uszACMNumber);
				
				bUH_Port = openCOM(uszACMNumber);
				if (bUH_Port > 0)
					break;
			}
		}
	}
	//bUH_Port = open("/dev/ttyACM0", O_RDWR);
	//bUH_Port = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
	//bUH_Port = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	//bUH_Port = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	

	vdDebug_LogPrintf("inCTOSS_USBHostOpenEx bUH_Port[%d]", bUH_Port);

	if (bUH_Port > 0)
		return d_OK;
	else
		return d_NO;
	

	/*i would like to fix the baud rate , com param for now*/
	//inRet = uh_setportparam(bUH_Port, ulBaudRate, bParity, bDataBits, bStopBits);
	inRet = 0;
	vdDebug_LogPrintf("uh_setportparam inRet[%d]", inRet);

	if (0 == inRet)
		return d_OK;
	else
		return d_NO; 
//#endif

}

int inCTOSS_USBHostSendBufEx(unsigned char *szSendBuf, int inlen)
{
	int inRet = -1;
	unsigned char str[50];
//#ifdef PCI100_USB
	if(strTCT.byPinPadType == 1)
	{
		vdPCIDebug_HexPrintf("CTOS_USBHostTxData",szSendBuf,inlen);
		inRet = CTOS_USBHostTxData(szSendBuf, inlen, 100);
		vdDebug_LogPrintf("CTOS_USBHostTxData inRet[%d]", inRet);
	    if (inRet <= 0) 
	    {
	        return inRet;
	    }

		return d_OK;
	}
//#else
	tcflush(bUH_Port,TCIOFLUSH);  //flush I/O Buffer

//	sprintf(str, "SEND=%2X %2X %2X %2X %2X[%d]", szSendBuf[inlen-5], szSendBuf[inlen-4], szSendBuf[inlen-3], szSendBuf[inlen-2], szSendBuf[inlen-1], inlen);
//	CTOS_LCDTPrintXY(1, 10, str);	
	vdPCIDebug_HexPrintf("USBHostSendBufEx",szSendBuf,inlen);
	usleep(10);
	inRet = writeToCOM(bUH_Port,szSendBuf,inlen);	
 	//inRet = write(bUH_Port, szSendBuf, inlen);
	//inPrintISOPacket(VS_TRUE, szSendBuf,inlen);

	vdDebug_LogPrintf("writeport bUH_Port[%d] len[%d] inRet[%d]", bUH_Port, inlen, inRet);

	if (inRet == inlen)
		return d_OK;
	else
	{
		vdDisplayErrorMsg(1, 8, "COM Send Buf Error");
		return d_NO;
	}
//#endif	
}

int Get10Count(BYTE *CheckBuf, USHORT CheckLen) {
    if (CheckBuf[CheckLen - 1] == 0x10) {
        return Get10Count(CheckBuf, CheckLen - 1) + 1;
    } else {
        return 0;
    }
}

//****************************************************************************

BOOL CheckDLEEnd(BYTE *CheckBuf, USHORT CheckLen) {
    USHORT iCount;
    iCount = 0;

    iCount = Get10Count(CheckBuf, CheckLen);

    if ((iCount % 2) == 0)
        return TRUE;
    else
        return FALSE;
}


int inCTOSS_USBHostRecvBufEx(unsigned char *szRecvBuf, int *inlen, int inTimeOut)
{
	int no=0;
	int rc;
	int max = *inlen;
	struct timeval tv;
	fd_set readfd;

	char szReadBuf[512];

	VS_BOOL fWaitForAck = VS_TRUE;
    char chResp;
	int len = 0;
	ULONG GLen;
	USHORT usTotalLen=0;
	BYTE key;
	int queuelen;
	vdDebug_LogPrintf("inCTOSS_USBHostRecvBufEx,inTimeOut=[%d]",inTimeOut);
//#ifdef PCI100_USB
	if(strTCT.byPinPadType == 1)
	{
	CTOS_TimeOutSet (TIMER_ID_1 , inTimeOut*100);
	usTotalLen=0;
	while (1) 
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return d_NO;

		if (*inlen == 0)
			GLen = 512;
		else
		GLen = *inlen;
        CTOS_USBHostRxData(&szRecvBuf[usTotalLen], &GLen, (ULONG) 100);
		vdDebug_LogPrintf("CTOS_USBHostRxData=[%d]",GLen);
        if (GLen >= 1) 
        {
            usTotalLen += GLen;
			vdPCIDebug_HexPrintf("CTOS_USBHostRxData",szRecvBuf,usTotalLen);
            if (szRecvBuf[usTotalLen - 2] == 0x10 && szRecvBuf[usTotalLen - 1] == 0x03) 
            {
                if (CheckDLEEnd(szRecvBuf, usTotalLen - 2)) 
                {
                    *inlen = usTotalLen;
					return d_OK;
                }
            }
        }

    }//end while

	return d_NO;
	}
//#else	
	tv.tv_sec= inTimeOut/1000;    //SECOND
	tv.tv_usec= inTimeOut%1000*1000;  //USECOND
	
	vdDebug_LogPrintf("select bUH_Port[%d] rc[%d]", bUH_Port, rc);

	CTOS_TimeOutSet (TIMER_ID_1 , inTimeOut*100);
	
	usTotalLen=0;
	while (1)
	{
		if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return d_NO;
		
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
            return d_NO;
		}
		//CTOS_LCDTPrintXY(1, 8, "receiving...");
		//if(rc == d_OK) 
		{
			if (*inlen == 0)
				len = 512;
			else
			len = *inlen;
			//rc = read(bUH_Port, &szRecvBuf[usTotalLen], len);
			rc = readFromCOM(bUH_Port, &szRecvBuf[usTotalLen], len);
			usTotalLen += rc;

			vdDebug_LogPrintf("readport rc[%d] len[%d]", rc, len);
			
			if (usTotalLen > 0)
				//DebugAddHEX("CTOS_RS232RxData", szRecvBuf, usTotalLen);
				vdDebug_LogPrintf("usTotalLen[%d]", usTotalLen);
			else
				continue;
							
			*inlen = usTotalLen;

			return d_OK;
		} 
	}

	return d_NO;
//#endif	
}

int inCTOSS_USBHostRecvACKBufEx(unsigned char *szRecvBuf, int *inlen, int inTimeOut)
{
	int no=0;
	int rc;
	int max = *inlen;
	struct timeval tv;
	fd_set readfd;

	char szReadBuf[512];

	VS_BOOL fWaitForAck = VS_TRUE;
    char chResp;
	int len = 0;
	USHORT usTotalLen=0;
	BYTE key;
	int queuelen;
	vdDebug_LogPrintf("inCTOSS_USBHostRecvACKBufEx]");

	//fcntl(bUH_Port, F_SETFL, FNDELAY);
	
	tv.tv_sec= inTimeOut/1000;    //SECOND
	tv.tv_usec= inTimeOut%1000*1000;  //USECOND
	
	//FD_ZERO(&readfd);
	//FD_SET(bUH_Port, &readfd);

	//CTOS_Delay(100);
	
	//rc = select(bUH_Port+1, &readfd, NULL, NULL, &tv);

	vdDebug_LogPrintf("select bUH_Port[%d] rc[%d]", bUH_Port, rc);

	//if(rc<=0)
		//return d_NO;

	CTOS_Delay(50);

	#if 0
	if(rc>0)
 	{
  		while(1)
  		{
   			rc = read(bUH_Port, &szRecvBuf[no], 1);
   			if(rc>0)
				no=no+1;
			else
				break;

			/*check max received.*/
			if (max != 0 && no >= max)
				break;
  		}
		
		vdDebug_LogPrintf("readport bUH_Port[%d] len[%d]", bUH_Port, no);

  		*inlen = no;
		return d_OK;
	}
	else
 	{
  		return d_NO;
 	}
	#endif

	CTOS_TimeOutSet (TIMER_ID_1 , inTimeOut*100);
	
	if (fWaitForAck == VS_TRUE)
	{
		chResp = NAK;				/* Wait for reply till Timeout */
		do
		{
			if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            	return d_NO;

			len = 1;
			//rc = read(bUH_Port, &chResp, len);
			rc = readFromCOM(bUH_Port, &chResp , len);
			
			vdDebug_LogPrintf("readport rc[%d] chResp[%02X] len[%d]", rc, chResp, len);

			if (rc >= 1)
				break;
			
		}while(1);
	
	}
	else
	{
		chResp = ACK;
	}

	 if (chResp != ACK)
		 return d_NO;

	//CTOS_TimeOutSet (TIMER_ID_1 , inTimeOut/10);
	CTOS_TimeOutSet (TIMER_ID_1 , inTimeOut*100);

	usTotalLen=0;
	while (1)
	{
		if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return d_NO;
		
		// Check if Cancel key is pressed // 
		CTOS_KBDHit(&key); 
		if(key == d_KBD_CANCEL) 
		{ 
            return d_NO;
		}
		CTOS_LCDTPrintXY(1, 8, "receiving...");
		//if(rc == d_OK) 
		{
			if (*inlen == 0)
				len = 512;
			else
			len = *inlen;
			//rc = read(bUH_Port, &szRecvBuf[usTotalLen], len);
			rc = readFromCOM(bUH_Port, &szRecvBuf[usTotalLen], len);
			usTotalLen += rc;

			vdDebug_LogPrintf("readport rc[%d] len[%d]", rc, len);
			
			if (usTotalLen > 0)
				//DebugAddHEX("CTOS_RS232RxData", szRecvBuf, usTotalLen);
				vdDebug_LogPrintf("usTotalLen[%d]", usTotalLen);
			else
				continue;
				
			if (szRecvBuf[usTotalLen-2] != 0x03)
				continue;
			
			*inlen = usTotalLen;

			return d_OK;
		} 
	}

	return d_NO;
	
}


int inCTOSS_USBHostRecvACKEx(char *szRecvBuf, int *inlen, int inTimeOut)
{
	int no=0;
	int rc;
	int max = *inlen;
	struct timeval tv;
	fd_set readfd;

	char szReadBuf[512];

	VS_BOOL fWaitForAck = VS_TRUE;
    char chResp;
	int len = 0;
	USHORT usTotalLen=0;
	BYTE key;

	vdDebug_LogPrintf("=====inCTOSS_USBHostRecvACKEx=====");

	//fcntl(bUH_Port, F_SETFL, FNDELAY);
	
	//tv.tv_sec= inTimeOut/1000;    //SECOND
	//tv.tv_usec= inTimeOut%1000*1000;  //USECOND
	
	//FD_ZERO(&readfd);
	//FD_SET(bUH_Port, &readfd);

	//CTOS_Delay(100);
	
	//rc = select(bUH_Port+1, &readfd, NULL, NULL, &tv);

	vdDebug_LogPrintf("select bUH_Port[%d] rc[%d]", bUH_Port, rc);

	//if(rc<=0)
		//return d_NO;

	CTOS_Delay(100);

	//CTOS_TimeOutSet (TIMER_ID_1 , inTimeOut/10);
	CTOS_TimeOutSet (TIMER_ID_1 , inTimeOut*100);

	if (fWaitForAck == VS_TRUE)
	{
		chResp = NAK;				/* Wait for reply till Timeout */
		do
		{
			vdDebug_LogPrintf("---readport---");

			if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            	return d_NO;
			
			len = 1;
			//rc = read(bUH_Port, &chResp, len);
			rc = readFromCOM(bUH_Port, &chResp , len);
			
			vdDebug_LogPrintf("readport rc[%d] chResp[%02X] len[%d]", rc, chResp, len);

			if (rc >= 1)
				break;
			
		}while(1);
	
	}
	else
	{
		chResp = ACK;
	}

	 if (chResp != ACK)
		 return d_NO;
	 else
		 return d_OK;
	
}


int inCTOSS_USBHostCloseEx(void)
{
	vdDebug_LogPrintf("=====inCTOSS_USBHostCloseEx=====[%d]", bUH_Port);
//#ifdef PCI100_USB
	if(strTCT.byPinPadType == 1)
	{

	CTOS_USBHostClose();

	return d_OK;
	}
//#else	
	if (bUH_Port > 0)
	{
		closeCOM(bUH_Port);
		bUH_Port = 0;
		vdDebug_LogPrintf("=====closeCOM=====");
		return d_OK;
		
		tcflush(bUH_Port, TCIOFLUSH);
		close(bUH_Port);
		bUH_Port = 0;

		return d_OK;
	}

	return d_NO;
//#endif	
}

int inCTOSS_USBHostFlushEx(void)
{
	if(strTCT.byPinPadType == 1)
	{
		return d_OK;
	}

	CTOS_Delay(20);
	if (bUH_Port > 0)
		tcflush(bUH_Port,TCIOFLUSH);  //flush I/O Buffer
}


