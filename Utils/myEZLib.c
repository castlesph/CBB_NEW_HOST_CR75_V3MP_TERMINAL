/** 
**  A Template for developing new terminal shared application
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

/** These two files are necessary for calling CTOS API **/
#include "../Includes/myEZLib.h"
#include "../Includes/POSTypedef.h"

#include "../FileModule/myFileFunc.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\Includes\epad.h"
#include "..\debug\debug.h"


#define SHOW_SCRREN 1
#define d_LINE_DOT 12

BOOL fLogByUSB;
BOOL fLogByRS232;
BOOL fZMKByRS232;
BOOL ONUSB = FALSE; 

#define d_LCD_Width 16+1


void vdMyEZLib_ComPortInit()
{
    int inResult;
    USHORT usVendorID=0x0CA6, usProductID=0xA050, usRtn;
    
    fLogByUSB   = FALSE;
    fLogByRS232 = FALSE; 
    
    CTOS_RS232Close(DBG_PORT);
    if(ONUSB==TRUE)
    {
    
        CTOS_USBHostOpen(usVendorID, usProductID); 
        fLogByUSB = TRUE;   
    }
    else
    {            
        if((inResult=CTOS_RS232Open(DBG_PORT, 115200, 'N', 8, 1)) == d_OK)
        {
            fLogByRS232 = TRUE;    
        }        
    }
}



void vdMyEZLib_Printf(const char* fmt, ...)
{
	char printBuf[2048];
	char msg[2048];
	char tmpbuf[50];
	int times = 0,i;
	int inSendLen;
	va_list marker;
	BYTE baTemp[PAPER_X_SIZE * 64];   
	CTOS_FONT_ATTRIB stFONT_ATTRIB;

	memset(msg, 0x00, sizeof(msg));
	
	va_start( marker, fmt );
	vsprintf( msg, fmt, marker );
	va_end( marker );
	
	memset(printBuf, 0x00, sizeof(printBuf));
	strcat(printBuf, msg);
	strcat(printBuf ,"\n" );
	
	inSendLen = strlen(printBuf);
	
	memset (baTemp, 0x00, sizeof(baTemp));
	stFONT_ATTRIB.FontSize = d_FONT_16x16;		// Font Size = 12x24
	stFONT_ATTRIB.X_Zoom = 1;		// The width magnifies X_Zoom diameters
	stFONT_ATTRIB.Y_Zoom = 2;		// The height magnifies Y_Zoom diameters
	stFONT_ATTRIB.X_Space = 0;		// The width of the space between the font with next font
	stFONT_ATTRIB.Y_Space = 0;		// The Height of the space between the font with next font		  
	if (strTCT.inFontFNTMode != 1)
	{
		stFONT_ATTRIB.FontSize = d_FONT_9x18;
		stFONT_ATTRIB.Y_Zoom = 1;
	}
	if (inSendLen > 48)
	{
		times = inSendLen/48;
		for (i=0; i<times; i++)
		{
			memset (baTemp, 0x00, sizeof(baTemp));
			memset (tmpbuf, 0x00, sizeof(tmpbuf));
			memcpy(tmpbuf,&printBuf[i*48],48);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tmpbuf, &stFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(tmpbuf,&stFONT_ATTRIB,0);
		}

		times = inSendLen%48;
		if (times > 0)
		{
			memset (baTemp, 0x00, sizeof(baTemp));
			memset (tmpbuf, 0x00, sizeof(tmpbuf));
			memcpy(tmpbuf,&printBuf[i*48],times);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tmpbuf, &stFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(tmpbuf,&stFONT_ATTRIB,0);
		}
	}
	else
	{
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, printBuf, &stFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(printBuf,&stFONT_ATTRIB,0);
	}
	
}




/*==========================================================================*
 *Name:vdMyEZLib_LogPrintf                                                  *                           
 *input: as Like vdMyEZLib_LogPrintf("%s", test);                           *           
 *output: void                                                              *
 *Description:                                                              *
 *  Caller can ouput that any data type wanted to check to console by RS232 *
 *==========================================================================*/
void vdMyEZLib_LogPrintff(unsigned char *fmt, short inlen)
{

    char printBuf[2048];
    char msg[2048];
    short i=0;
    
        
    memset(msg, 0x00, sizeof(msg));
    memset(printBuf, 0x00, sizeof(printBuf));
    memset(printBuf, 0x30, inlen*2);
    for ( i=0 ;i<inlen; i++)
    {
        sprintf(&printBuf[i*2], "%02X", fmt[i]);
    }

    vdMyEZLib_LogPrintf(printBuf);
      
}


void vdMyEZLib_Rpad(char *pad, char *space)
{
    short inlen;
    
    for(inlen=0; inlen<(79-strlen(pad)); inlen ++)
        space[inlen]=0x20;
}


void vdMyEZLib_LogPrintf(const char* fmt, ...)
{
    char printBuf[2048];
    char msg[2048];
    char space[100];
    int inSendLen;
    va_list marker;
    int j = 0;
               
    if(fLogByUSB == FALSE && fLogByRS232 == FALSE)
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

    
    j = (int)strTCT.byRS232DebugPort;        
    if(j == 1 ) //Remark this line to view log
    {
           
        if(fLogByUSB==TRUE)
        {       
            if (CTOS_USBTxReady() == d_OK)
                CTOS_USBTxData(printBuf, inSendLen);
        }
        else
        {        
            while(CTOS_RS232TxReady(DBG_PORT) != d_OK);
            CTOS_RS232TxData(DBG_PORT, printBuf, inSendLen);    
         
        }
    }
    return;        
}

int inAscii2Bcd(BYTE *szAscii, BYTE *szBcd, int inBcdLen)
{
    int inAsciiLen = inBcdLen*2;
    int inBcdIdx;
    int inAsciiIdx;
    BYTE szAscii2[inBcdLen*2+1];
    BYTE byLo;
    BYTE byHi;
        
    memset(szAscii2, 0x00, sizeof(szAscii2));
    /*pad zero for ascii buffer*/
    if((inAsciiLen = strlen(szAscii)) <= (inBcdLen*2))
    {
        memset(szAscii2, 0x30, inBcdLen*2);
        memcpy(&szAscii2[(inBcdLen*2)-inAsciiLen], szAscii, inAsciiLen);        
        vdMyEZLib_LogPrintf("[inAscii2Bcd]---szAscii2[%s], szAscii[%s]", szAscii2, szAscii);
    }
    else
    {       
        memcpy(szAscii2, szAscii, inBcdLen*2);
    }   
        
    
    inAsciiIdx=0;
    inBcdIdx=0;
    while(inBcdIdx < inBcdLen)
    {
        //for Hi Bit
        if(szAscii2[inAsciiIdx] >= 'A' && szAscii2[inAsciiIdx] <= 'F')
            byHi = (szAscii2[inAsciiIdx]-0x37) << 4; // 0x61 - 0x37 = 0x0A  
        else if(szAscii2[inAsciiIdx] >= 'a' && szAscii2[inAsciiIdx] <= 'f')
            byHi = (szAscii2[inAsciiIdx]-0x57) << 4;        
        else
            byHi = szAscii2[inAsciiIdx] << 4;   
        
        
        //for Low Bit
        if(szAscii2[inAsciiIdx+1] >= 'A' && szAscii2[inAsciiIdx+1] <= 'F')
            byLo = (szAscii2[inAsciiIdx+1]-0x37) & 0x0F; // 0x61('A') - 0x37 = 0x0A 
        else if(szAscii2[inAsciiIdx+1] >= 'a' && szAscii2[inAsciiIdx+1] <= 'f')
            byLo = (szAscii2[inAsciiIdx+1]-0x57) & 0x0F;        
        else
            byLo = szAscii2[inAsciiIdx+1] & 0x0F;   
                
        
        //byLo = szAscii2[inAsciiIdx+1] & 0x0F;
                                        
        szBcd[inBcdIdx] = byHi | byLo;
               
        
        inBcdIdx++;
        inAsciiIdx = inBcdIdx *2;
                
    }
    
    
    return TRUE;        
    
}


int inBcd2Ascii(BYTE *szBcd, BYTE *szAscii, int inBcdLen)
{
    int inBcdIdx;
    int inAsciiIdx;
    BYTE szAscii2[3];
    BYTE byLo;
    BYTE byHi;
    
    inBcdIdx = 0;
    inAsciiIdx = 0;
    while(inBcdIdx < inBcdLen)
    {
        sprintf(szAscii2, "%02x", szBcd[inBcdIdx]);
        memcpy(&szAscii[inAsciiIdx], szAscii2, 2);
        inBcdIdx ++;
        inAsciiIdx = inBcdIdx*2;        
    }

    return TRUE;    
    
}

int inBcd2Bin(BYTE *szBcd, int inBcdLen)
{
    char szAscii[inBcdLen*2+1];
    int inBin;
    
    memset(szAscii, 0x00, sizeof(szAscii));

    if(inBcd2Ascii(szBcd, szAscii, inBcdLen))
    {
        inBin = atoi(szAscii);
        return inBin;
    }                        
    
    return ST_ERROR;
}

int inBin2Bcd(int inBin, BYTE *szBcd, int inBcdLen)
{
    char szAscii[inBcdLen*2+1];
    memset(szAscii, 0x00, sizeof(szAscii));

    sprintf(szAscii, "%d", inBin);
    vdMyEZLib_LogPrintf("[inBin2Bcd]---Bin2Ascii[%s]",szAscii);
    vdDebug_LogPrintf("**inBcdAddOne szAscii[%s]**",szAscii);
    
    
    return(inAscii2Bcd(szAscii, szBcd, inBcdLen));
}

int inBcdAddOne(BYTE *szInBcd, BYTE *szOutBcd, int inBcdLen)
{
    int inBin;
    int inResult;
    char szAscii[inBcdLen*2+1];
    
    
    memset(szAscii, 0x00, sizeof(szAscii));
        
    /*BCD to Bin*/
    inBin = inBcd2Bin(szInBcd, inBcdLen);
    vdMyEZLib_LogPrintf("[inBcdAddOne]---BCD 2 Bin[%d]", inBin);
    vdDebug_LogPrintf("**inBcdAddOne inBin[%d]**",inBin);

    if(inBin >= 999999)
        inBin=0;
		
    /*Bin++*/
    inBin++;
	
    /*Bin to BCD*/
    inResult = inBin2Bcd(inBin, szOutBcd, inBcdLen);
    vdDebug_LogPrintf("**inBcdAddOne inResult[%d]**",inResult);
        
    return inResult;
    
}

   
//gcitra
void vdFormatAmount(char *str, char *strCur, char *strAmt, BOOL fVoid)
{
	int inAmtLen = 0,
		inCtr = 0,
		inTmpLen = 0,
		i = 0;
	char szTmpBuf[101] = {0},
		szTmpAmt[101] = {0},
		szDec[3] = {0};

	memset(szTmpBuf, 0, sizeof(szTmpBuf));
	inAmtLen = strlen(strAmt) -2;

	memcpy(szDec, &strAmt[inAmtLen], 2);
	memcpy(szTmpAmt, strAmt, inAmtLen);

	for (i = strlen(szTmpAmt)-1; i >= 0; i--)
	{
		if (inCtr >= 3)
		{
			szTmpBuf[inTmpLen] = ',';
			++i;
			inCtr = 0;
		}
		else
		{
			szTmpBuf[inTmpLen] = szTmpAmt[i];
			inCtr++;
		}
		inTmpLen++;
	}
	inTmpLen--;

	memset(szTmpAmt, 0, sizeof(szTmpAmt));
	for (i = 0; i <= inTmpLen; i++)
		szTmpAmt[i] = szTmpBuf[inTmpLen - i];

	if (strlen(strCur) > 0)
	{
		strCur[3] = 0x00;
		
		if (fVoid == TRUE)
			sprintf(str, "-%s %s.%s", strCur, szTmpAmt, szDec);
		else
			sprintf(str, "%s %s.%s", strCur, szTmpAmt, szDec);
	}
	else
	{
		if (fVoid == TRUE)
			sprintf(str, "-%s.%s", szTmpAmt, szDec);
		else
			sprintf(str, "%s.%s", szTmpAmt, szDec);
	}

}
//gcitra
