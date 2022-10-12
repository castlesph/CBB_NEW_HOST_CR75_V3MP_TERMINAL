#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include "debug.h"
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>

#include "..\Includes\myEZLib.h"
#include "..\Includes\wub_lib.h"
#include "..\Includes\POSTypedef.h"

#include "..\FileModule/myFileFunc.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\CTOSInput.h"


#include <stdarg.h>
#include <Vwdleapi.h>

#define MAX_DEBUF_BUF 8096

static BYTE ifDebugMode = FALSE;
BOOL fDebugOpenedFlag = FALSE;

#define d_READY_TIMEOUT		100
#define d_DEBUG_PORT d_COM1

BYTE DebugLog[MAX_DEBUF_BUF + 2];
LONG DebugLen;
CTOS_RTC stRTC;
INT iDebugTOTimes = 0;
BYTE DebugPort = d_DEBUG_PORT;

#define DEBUG_BUFF_FILE "/media/mdisk/debugfle.txt"
#define DEBUG_BUFF_BAKFILE "/media/mdisk/debugfle.bak"

#define d_FILE			9
#define MAX_DEBUG_SIZE	(50*1024)
#define MAX_DEBUG_BUF	6000

static long inDEBUG_GetFileSize(const char* pchFileName)
{
    FILE  *fPubKey;
	long curpos,length;

	//vdDebug_LogPrintf("lnGetFileSize[%s]", pchFileName);
	fPubKey = fopen( (char*)pchFileName, "rb" );
	if(fPubKey == NULL)
		return 0;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fclose(fPubKey);
	//vdDebug_LogPrintf("lnGetFileSize[%d],length=[%d]", curpos,length);

    return(length);
}


static int inDEBUG_WriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	long length;
	char szSystemCmdPath[250];
	
	//vdDebug_LogPrintf("inWriteFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);

	length = inDEBUG_GetFileSize(pchFileName);
	if(length > MAX_DEBUG_SIZE)
	{
		memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath)); 			
		sprintf(szSystemCmdPath, "mv %s %s",DEBUG_BUFF_FILE,DEBUG_BUFF_BAKFILE);
		system(szSystemCmdPath);
	}

	fPubKey = fopen((char *)pchFileName, "ab+" );
	if(fPubKey == NULL)
	{
		fPubKey = fopen((char *)pchFileName, "wb+" );
		if(fPubKey == NULL)
			return -1;
	}

	if (inMaxRecSize > MAX_DEBUG_BUF)
	{
		times = inMaxRecSize/MAX_DEBUG_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_DEBUG_BUF],MAX_DEBUG_BUF, 1, fPubKey);
		}

		times = inMaxRecSize%MAX_DEBUG_BUF;
		if(times>0)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_DEBUG_BUF],times, 1, fPubKey);
		}
		
	}
	else
	inRetVal = fwrite(pchRecBuf,inMaxRecSize, 1, fPubKey);
	fclose(fPubKey);

	//vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}

static int inDEBUG_ReadFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	
	//vdDebug_LogPrintf("ReadFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);
	
	fPubKey = fopen((char *)pchFileName, "rb" );
	if(fPubKey == NULL)
		return -1;

	if (inMaxRecSize > MAX_DEBUG_BUF)
	{
		times = inMaxRecSize/MAX_DEBUG_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_DEBUG_BUF], 1, MAX_DEBUG_BUF, fPubKey);
		}

		times = inMaxRecSize%MAX_DEBUG_BUF;
		if(times>0)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_DEBUG_BUF], 1, times, fPubKey);
		}
		
	}
	else
	inRetVal = fread (pchRecBuf, 1, inMaxRecSize, fPubKey);
	fclose(fPubKey);

	//vdDebug_LogPrintf("ReadFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}


BYTE byGetDebugMode(void)
{
    return ifDebugMode;
}

/****************
 * if bPort == 0xFF --> USB mode
 ****************/
void SetDebugMode(BYTE bMode, BYTE bPort)
{
	if (0 ==strTCT.byRS232DebugPort)
	{
		ifDebugMode = FALSE;
		return;
	}
	else
		ifDebugMode = TRUE;
}

void DebugInit(void)
{
    
    if (!ifDebugMode) return;

    DebugLen = 0;

	if (9 ==strTCT.byRS232DebugPort)
	{
		DebugPort = d_FILE;
	}


	if (8 ==strTCT.byRS232DebugPort)
	{
		DebugPort = 0xFF;
        if(fDebugOpenedFlag == FALSE)    
        {
            CTOS_USBOpen();
			VW_SetDebug(TRUE, d_DEBUG_PORT_USB);
            fDebugOpenedFlag = TRUE;
            
        }
        
	}
	
    if (1 ==strTCT.byRS232DebugPort)
    {
    	DebugPort= d_COM1;
        if(fDebugOpenedFlag == FALSE)    
        {
            CTOS_RS232Open(DebugPort, 115200, 'N', 8, 1);
            fDebugOpenedFlag = TRUE;
            
        }
    }

	if (2 ==strTCT.byRS232DebugPort)
	{
		DebugPort= d_COM2;
        if(fDebugOpenedFlag == FALSE)
        {
            CTOS_RS232Open(DebugPort, 115200, 'N', 8, 1);
            fDebugOpenedFlag = TRUE;
           
        }
	}
	
}

void DebugExport232(void)
{
	ULONG tick;
	USHORT ret;
	int inResult;
	
	if (!ifDebugMode) return;

	if (DebugPort == d_FILE)
	{
		inResult = inCTOSS_GetRemovableStorageStatus();
		if (inResult == 1 || inResult == 3)
			inDEBUG_WriteFile(DEBUG_BUFF_FILE,DebugLog,DebugLen);
		return;
	}
	
	tick = CTOS_TickGet();
	do {
        if (DebugPort == 0xFF)
          ret = CTOS_USBTxReady();
        else
          ret = CTOS_RS232TxReady(DebugPort);
		if (ret == d_OK)
			break;
		//CTOS_Delay(50);
	} while ((CTOS_TickGet() - tick) < d_READY_TIMEOUT);
	
	if (ret == d_OK) {
		DebugLog[DebugLen++] = 0x0D;
		DebugLog[DebugLen++] = 0x0A;
        if (DebugPort == 0xFF)
        {
			CTOS_USBTxFlush();			
			CTOS_USBTxData(DebugLog, DebugLen);
        }
        else
            CTOS_RS232TxData(DebugPort, DebugLog, DebugLen);
		tick = CTOS_TickGet();
		do {
          if (DebugPort == 0xFF)
          {
			CTOS_USBTxFlush();
			ret = CTOS_USBTxReady();
          }
          else
			ret = CTOS_RS232TxReady(DebugPort);
			if (ret == d_OK)
				break;
			//CTOS_Delay(50);
		} while ((CTOS_TickGet() - tick) < d_READY_TIMEOUT);
	}
    
    DebugLen = 0;
}

void DebugAddHEX(BYTE *title, BYTE *hex, USHORT len)
{
    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;

	if (len > (sizeof (DebugLog) / 2) - 8)
		len = (sizeof (DebugLog) / 2) - 8;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	DebugLen = wub_strlen(DebugLog);
	DebugLog[DebugLen++] = '[';
	DebugLog[DebugLen] = 0x00;
	wub_strcat(&DebugLog[DebugLen], title);
	DebugLen += wub_strlen(title);
	DebugLog[DebugLen++] = ']';
	DebugLog[DebugLen++] = ' ';
	DebugLen += wub_hex_2_str(hex, &DebugLog[DebugLen], len);
	
	DebugExport232();
}

void DebugAddINT(BYTE *title, LONG value)
{
	BYTE temp[50];

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	wub_strcat(DebugLog, "[");
	wub_strcat(DebugLog, title);
	wub_strcat(DebugLog, "] ");
	wub_memset(temp, 0x00, sizeof (temp));
	sprintf(temp, "%ld", value);
	wub_strcat(DebugLog, temp);
	
	DebugLen = wub_strlen(DebugLog);
	DebugExport232();
}

void DebugAddIntX(BYTE *title, LONG value)
{
	BYTE temp[50];

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();


	if (!ifDebugMode) return;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	wub_strcat(DebugLog, "[");
	wub_strcat(DebugLog, title);
	wub_strcat(DebugLog, "] ");
	
	wub_memset(temp, 0x00, sizeof (temp));
	sprintf(temp, "0x%08lX", value);
	wub_strcat(DebugLog, temp);
	
	DebugLen = wub_strlen(DebugLog);
	DebugExport232();
}

void DebugAddSTR(BYTE *title, BYTE *baMsg, USHORT len)
{

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;
	
	if (baMsg == NULL) return;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	//sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(DebugLog, "<%lu> ", CTOS_TickGet());
	DebugLen = wub_strlen(DebugLog);
	
	DebugLog[DebugLen++] = '[';
	wub_strcat(&DebugLog[DebugLen], title);
	DebugLen += wub_strlen(title);
	DebugLog[DebugLen++] = ']';
	DebugLog[DebugLen++] = ' ';
	
	wub_memcpy(&DebugLog[DebugLen], baMsg, strlen(baMsg));
	DebugLen += strlen(baMsg);
	
	DebugExport232();
}


void vdDebug_LogPrintf_old(const char* fmt, ...)
{
    char printBuf[MAX_DEBUF_BUF];
	char msg[MAX_DEBUF_BUF];
	char space[100];
	int inSendLen;
	va_list marker;
	int j = 0;
    char szAPName[25];
	int inAPPID;

    if (0 ==strTCT.byRS232DebugPort)
        return;
    
    memset(msg, 0x00, sizeof(msg));
	memset(printBuf, 0x00, sizeof(printBuf));
	memset(space, 0x00, sizeof(space));
	
	va_start( marker, fmt );
	vsprintf( msg, fmt, marker );
	va_end( marker );
	
	memset(printBuf, 0x00, sizeof(printBuf));		
	strcat(printBuf, msg);
	strcat(printBuf, space);
	strcat(printBuf ,"\n" );
	
	inSendLen = strlen(printBuf);


	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);


    DebugAddSTR(szAPName,printBuf,inSendLen);
}


void vdPCIDebug_HexPrintf(char *display,char *szHexbuf, int len)
{
	char szbuf[MAX_DEBUF_BUF];
	memset(szbuf,0x00,sizeof(szbuf));

	if (0 ==strTCT.byRS232DebugPort)
        return;
	
	wub_hex_2_str(szHexbuf,szbuf,len);
	vdDebug_LogPrintf("vdPCIDebug_HexPrintf [%s]=[%s]",display,szbuf);
}

void PrintDebugMessage(const char* filePath, int lineNumber, const char * functionName, const char* format, ...)
{
    static char appName[25] = "";
    int appId = 0;
    va_list arg_list;
    char buffer[MAX_DEBUF_BUF];
    char debugLine[MAX_DEBUF_BUF]; // [application_name]|[filename:line_number]|[tick]: <debug msg>
    int result;
    //unsigned long tick;
	char szTickLog[50+1];
    char * pFileName;

	if (0 ==strTCT.byRS232DebugPort)
		return;
	
    va_start(arg_list, format);
    
    result = vsnprintf(buffer, sizeof(buffer), format, arg_list);
    
    va_end(arg_list);
    
    if (result == sizeof(buffer))
        buffer[result - 1] = 0;
    else 
        buffer[result] = 0;
    
    if (strlen(appName) == 0)     
        inMultiAP_CurrentAPNamePID(appName, &appId);    
    
    //tick = CTOS_TickGet();
    memset(szTickLog,0x00,sizeof(szTickLog));
	CTOS_RTCGet(&stRTC);
//	sprintf(szTickLog, "<%02d:%02d:%02d>", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	sprintf(szTickLog, "<%ld>", CTOS_TickGet());
    
    pFileName = strrchr(filePath, '/');
    if (pFileName != NULL) 
        pFileName++;
    else 
        pFileName = (char*)filePath;
    sprintf(debugLine, "[%s]%s[%s:%d]: %s\n", appName, szTickLog,pFileName, lineNumber, buffer);
    
    //printf(debugLine);
    
	SetDebugMode(1, 0xFF);
	DebugInit();
	
	if (!ifDebugMode) return;
	
	//CTOS_RTCGet(&stRTC);
	
	memset(DebugLog, 0x00, sizeof(DebugLog));
	
	wub_memcpy(&DebugLog[DebugLen], debugLine, strlen(debugLine));
	DebugLen += strlen(debugLine);
	
	DebugExport232();
	
    CTOS_Delay(1);
}

void vdDebug_PrintOnPaper(const char* format, ...)
{
	#ifdef DEBUG_PRINT_ON_PAPER
    int appId = 0;
    va_list arg_list;
    char buffer[64];
    int result;
	
    va_start(arg_list, format);
    result = vsnprintf(buffer, sizeof(buffer), format, arg_list);
    va_end(arg_list);
    
    if (result == sizeof(buffer))
        buffer[result - 1] = 0;
    else 
        buffer[result] = 0;

	CTOS_PrinterPutString(buffer);
	#endif
}

