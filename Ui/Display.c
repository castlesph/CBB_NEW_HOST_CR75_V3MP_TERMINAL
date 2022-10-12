#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../Includes/msg.h"
#include "../Includes/wub_lib.h"
#include "../Includes/myEZLib.h"
#include "../Includes/POSTypedef.h"
#include "../Includes/POSTrans.h"
#include "..\Includes\MultiApLib.h"

#include "display.h"
#include "../FileModule/myFileFunc.h"
#include "../print/Print.h"
#include "../Includes/CTOSinput.h"
#include "../UI/Display.h"
#include "../Comm/V5Comm.h"
#include "..\Includes\POSSetting.h"

#include "..\Debug\Debug.h"


extern USHORT GPRSCONNETSTATUS;

#define ERRORLEN 30
static char szErrorMessage[ERRORLEN+1];

void setLCDReverse(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(TRUE);  //the reverse enable // 
    
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            //CTOS_LCDTPrintXY(1, line, pbBuf);
			CTOS_LCDTPrintAligned(line, pbBuf, d_LCD_ALIGNLEFT); 
			break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (16 - lens) / 2 + 1;
            //CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			CTOS_LCDTPrintAligned(line, pbBuf, d_LCD_ALIGNCENTER); 
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 16 - lens + 1;
            //CTOS_LCDTPrintXY(iInitX, line, pbBuf);            
			CTOS_LCDTPrintAligned(line, pbBuf, d_LCD_ALIGNRIGHT); 
            break;
    }

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(FALSE); //the reverse enable //     
}

void setLCDPrint(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            //CTOS_LCDTPrintXY(1, line, pbBuf);
			CTOS_LCDTPrintAligned(line, pbBuf, d_LCD_ALIGNLEFT);             
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (16 - lens) / 2 + 1;
            //CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			CTOS_LCDTPrintAligned(line, pbBuf, d_LCD_ALIGNCENTER);             
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 16 - lens + 1;
            //CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			CTOS_LCDTPrintAligned(line, pbBuf, d_LCD_ALIGNRIGHT); 
            break;
    }
}

void showAmount(IN  BYTE bY, BYTE bStrLen, BYTE *baBuf)
{
    int i;
    
    if(bStrLen > 2)
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            if ((16 - bStrLen + 1 + i) > 14)
                CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
            else
                CTOS_LCDTPutchXY(16 - bStrLen + i, bY, baBuf[i]);
        }
    }
    else
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
        }
    }
}

void vduiDisplayInvalidTLE(void)
{
    
    vduiClearBelow(2);
    vduiWarningSound();
    vduiDisplayStringCenter(3,"INVALID SESSION");
    vduiDisplayStringCenter(4,"KEY, PLEASE");
    vduiDisplayStringCenter(5,"DWD SESSION KEY");
    vduiDisplayStringCenter(6,"--INSTRUCTION---");
//    CTOS_LCDTPrintXY(1,7,"PRESS [F2] THEN");
//    CTOS_LCDTPrintXY(1,8,"PRESS [3]");
	CTOS_LCDTPrintAligned(7,"PRESS [F2] THEN", d_LCD_ALIGNLEFT);			 
	CTOS_LCDTPrintAligned(8,"PRESS [3]", d_LCD_ALIGNLEFT);			 
    CTOS_Delay(2500);
}

void szGetTransTitle(BYTE byTransType, BYTE *szTitle)
{    
    int i;
    szTitle[0] = 0x00;
    
    vdDebug_LogPrintf("**szGetTransTitle START byTransType[%d]Orig[%d]**", byTransType, srTransRec.byOrgTransType);
    switch(byTransType)
    {
        case SALE:
            if(srTransRec.fIsInstallment == TRUE)
                strcpy(szTitle, "INSTALLMENT");
            else
                strcpy(szTitle, "SALE");
            break;
        case PRE_AUTH:
			//0826
            strcpy(szTitle, "PRE AUTH");
            //strcpy(szTitle, "CARD VERIFY");
			//0826
            break;
        case VOID_PREAUTH:
            strcpy(szTitle, "VOID PREAUTH");
            break;
        case PRE_COMP:
            strcpy(szTitle, "AUTH COMP");
            break;
        case REFUND:
            strcpy(szTitle, "REFUND");
            break;
        case VOID:
            if(REFUND == srTransRec.byOrgTransType)
                strcpy(szTitle, "VOID REFUND");
			else if(ALIPAY_SALE == srTransRec.byOrgTransType)
				strcpy(szTitle, "ALIPAY VOID");
			else if(PRE_AUTH == srTransRec.byOrgTransType)
				strcpy(szTitle, "PREAUTH VOID");
			//else if(PREAUTH_COMP == srTransRec.byOrgTransType)
			else if(srTransRec.byOrgTransType== PREAUTH_COMP)// && (srTransRec.HDTid == 7 || srTransRec.HDTid == 21))
				strcpy(szTitle, "PREAUTHCOMP VOID");
			else if(srTransRec.byOrgTransType == OK_DOLLAR_TRANS)		// http://118.201.48.210:8080/redmine/issues/1525.58		
				strcpy(szTitle, "VOID OK $");
			else if(srTransRec.byOrgTransType == CB_PAY_TRANS)		//http://118.201.48.210:8080/redmine/issues/1525.92 issue #s 2 and 3.
				strcpy(szTitle, "VOID CBPay");									
            else
                strcpy(szTitle, "VOID");
//                strcpy(szTitle, "VOID SALE");
            break;
		
        case SALE_TIP:
            strcpy(szTitle, "TIP ADJUST");
            break;
        case SALE_OFFLINE:
            strcpy(szTitle, "OFFLINE SALE");
            break;
        case PREAUTH_COMP:
            strcpy(szTitle, "PREAUTH COMP");
            break;
        case VOID_PREAUTH_COMP:
            strcpy(szTitle, "VOID PREAUTH COMP");
            break;
        case SALE_ADJUST: 
            strcpy(szTitle, "ADJUST");
            break;
        case SETTLE:
            strcpy(szTitle, "SETTLE");
            break;
        case SIGN_ON:
            strcpy(szTitle, "SIGN ON");
            break;
        case SIGNON_ALL:
            strcpy(szTitle, "SIGN ON ALL");
            break;			
        case IPP_SIGN_ON:
            strcpy(szTitle, "IPP SIGN ON");
            break;
        case BATCH_REVIEW:
            strcpy(szTitle, "BATCH REVIEW");
            break;
        case BATCH_TOTAL:
            strcpy(szTitle, "BATCH TOTAL");
            break;
        case REPRINT_ANY:
            strcpy(szTitle, "REPRINT RECEIPT");
            break;
		case EFTSEC_TWK:
			strcpy(szTitle, "EFTSEC TWK");
			break;
		case EFTSEC_TMK:
			strcpy(szTitle, "EFTSEC TMK");
			break;
		case EFTSEC_TWK_RSA:
			strcpy(szTitle, "EFTSEC TWK RSA");
			break;
		case EFTSEC_TMK_RSA:
			strcpy(szTitle, "EFTSEC TMK RSA");
			break;
		case CASH_ADVANCE:
			strcpy(szTitle, "CASH ADVANCE");
			break;
		case CUP_LOGON:
			strcpy(szTitle, "LOG ON");
			break;
		case CUP_SALE:
			strcpy(szTitle, "SALE");
			break;
		case CUP_PRE_AUTH:
			strcpy(szTitle, "PRE AUTH");
			break;
		case CUP_PREAUTH_COMP:
			strcpy(szTitle, "PRE AUTH COMP");
			break;
		case CUP_VOID_PREAUTH:
			strcpy(szTitle, "VOID PRE AUTH");
			break;
		case CUP_SALE_ADJUST:
			strcpy(szTitle, "SALE ADJUST");
			break;
		case MPU_SIGNON:
			strcpy(szTitle, "SIGN ON");
			break;
		case MPU_SIGNOFF:
			strcpy(szTitle, "SIGN OFF");
			break;
        case MPU_SETTLE:
			strcpy(szTitle, "SETTLEMENT");
			break;
        case MPU_PREAUTH:
            strcpy(szTitle, "PRE AUTH");
            break;
        case MPU_PREAUTH_COMP:
            strcpy(szTitle, "PREAUTH COMP.");
            break;
        case MPU_PREAUTH_COMP_ADV:
            strcpy(szTitle, "PREAUTH COMP. ADV");
            break;
        case MPU_VOID_PREAUTH:
            strcpy(szTitle, "VOID PREAUTH");
            break;
        case MPU_VOID_PREAUTH_COMP:
            strcpy(szTitle, "VOID PREAUTH COMP");
			break;
		case SETUP:
			strcpy(szTitle, "SETUP");
			break;
		case AUTO_REVERSAL:
			strcpy(szTitle, "REVERSAL");
			break;
		case ALIPAY_SALE:
            strcpy(szTitle, "ALIPAY SALE");
        break;
		
#ifdef TOPUP_RELOAD
        case BALANCE_ENQUIRY:
            strcpy(szTitle, "BALANCE ENQUIRY");
        break;
		
        case TOPUP:
            strcpy(szTitle, "TOPUP");
        break;
		
        case RELOAD:
            strcpy(szTitle, "REPAYMENT");
        break;

		case TOPUP_RELOAD_LOGON:
			strcpy(szTitle, "LOGON");
		break;
		
		case TOPUP_RELOAD_TRANS:
			strcpy(szTitle, "TOPUP/REPAYMENT");
		break;
#endif

        case CB_PAY_TRANS:			
			strcpy(szTitle, "CBPay");
		break;

#ifdef ECR_PREAUTH_AND_COMP
		case TRANS_ENQUIRY:
			strcpy(szTitle, "TRANSACTION ENQUIRY");
		break;
#endif

#ifdef PIN_CHANGE_ENABLE
		case CHANGE_PIN:			
			strcpy(szTitle, "CHANGE PIN");
		break;
		
#endif
#ifdef OK_DOLLAR_FEATURE
        case OK_DOLLAR_TRANS:
			strcpy(szTitle, "OK $"); //(szTitle, "OK DOLLAR"); - http://118.201.48.210:8080/redmine/issues/1525.50.2
		break;
#endif

		default:
            strcpy(szTitle, "");
            break;
    }
    i = strlen(szTitle);
    szTitle[i]=0x00;
    return ;
}

void vdDispTransTitle(BYTE byTransType)
{
    BYTE szTitle[MAX_CHAR_PER_LINE*2+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE*2+1];
    int iInitX = 1;

	if (inMultiAP_CheckSubAPStatus() == d_OK)
        return ;
   
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(byTransType, szTitle);
	iInitX = ((MAX_CHAR_PER_LINE/2 - CTOSS_strlen(szTitle)/2))+1;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, sizeof(szTitleDisplay));
    memcpy(&szTitleDisplay[iInitX-1], szTitle, strlen(szTitle));
    CTOS_LCDTSetReverse(TRUE);
    //CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTPrintAligned(1, "                                          ", d_LCD_ALIGNCENTER);    
	CTOS_LCDTPrintAligned(1, szTitle,d_LCD_ALIGNCENTER);	
    CTOS_LCDTSetReverse(FALSE);
}

void vdDispTitleString(BYTE *szTitle)
{
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1];
    int iInitX = 1;
       
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));
    CTOS_LCDTSetReverse(TRUE);
    //CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTPrintAligned(1, "                                          ", d_LCD_ALIGNCENTER);    
	CTOS_LCDTPrintAligned(1, szTitle,d_LCD_ALIGNCENTER);	    
    CTOS_LCDTSetReverse(FALSE);
}

USHORT clearLine(int line)
{
    char szClear[MAX_CHAR_PER_LINE+1];
    
    memset(szClear, ' ', MAX_CHAR_PER_LINE);
    setLCDPrint(line, DISPLAY_POSITION_LEFT, szClear);
//    CTOS_LCDTGotoXY(1,line);
//    CTOS_LCDTClear2EOL();
}

void vdDisplayTxnFinishUI(void)
{
	if(srTransRec.byTransType == TRANS_ENQUIRY)
		return;
		
    CTOS_LCDTClearDisplay();
    setLCDPrint(3, DISPLAY_POSITION_CENTER, "Transaction");
    setLCDPrint(4, DISPLAY_POSITION_CENTER, "Approved");
    setLCDPrint(5, DISPLAY_POSITION_CENTER, srTransRec.szAuthCode);     
}

void vdDispErrMsg(IN BYTE *szMsg)
{
    char szDisplayMsg[40];
    BYTE byKeyBuf;
    
    CTOS_LCDTClearDisplay();
    if(srTransRec.byTransType != 0)
        vdDispTransTitle(srTransRec.byTransType);

    memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
    strcpy(szDisplayMsg, szMsg);
    vduiClearBelow(8);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szDisplayMsg);
    CTOS_TimeOutSet (TIMER_ID_2 , 2*100);
    CTOS_Sound(1000, 50);
    
    while (1)
    {        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL ||byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return ;
        }
    }
}


int vdDispTransactionInfo(void)
{
    BYTE byKeyBuf;
    BYTE szTmp1[16+1];
    BYTE szTmp2[16+1];
	BYTE szTmp[130+1];
    
    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "Card NO.");
    setLCDPrint(3, DISPLAY_POSITION_LEFT, srTransRec.szPAN);
    memset(szTmp1, 0x00, sizeof(szTmp1));
    memset(szTmp2, 0x00, sizeof(szTmp2));
	memset(szTmp, 0x00, sizeof(szTmp));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTmp1, 3);
    sprintf(szTmp2,"Inv No:%s", szTmp1);
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szTmp2);
    
    wub_hex_2_str(srTransRec.szTotalAmount, szTmp1, 6);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Amount:");
	//format amount 10+2
	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTmp1, szTmp);
	sprintf(szTmp2,"%s%s", strCST.szCurSymbol,szTmp);
    //sprintf(szTmp2,"SGD%7lu.%02lu", (atol(szTmp1)/100), (atol(szTmp1)%100));
    setLCDPrint(6, DISPLAY_POSITION_RIGHT, szTmp2);  
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRS ENTR TO CONF");
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
            return  READ_CARD_TIMEOUT;
        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return d_OK;
        }
    }
}

USHORT showBatchRecord(TRANS_DATA_TABLE *strTransData)
{
    char szStr[DISPLAY_LINE_SIZE + 1];
    char szTemp[DISPLAY_LINE_SIZE + 1];
    BYTE byKeyBuf;
    CTOS_LCDTClearDisplay();
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szPAN);
    setLCDPrint(1, DISPLAY_POSITION_LEFT, "Card NO:");
    setLCDPrint(2, DISPLAY_POSITION_LEFT, szStr);
    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szBaseAmount, szTemp, AMT_BCD_SIZE);
	//format amount 10+2
	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szStr);
	//sprintf(szStr,"%10.0f.%02.0f",(atof(szTemp)/100), (atof(szTemp)%100));
    //sprintf(szStr, "%lu.%lu", atol(szTemp)/100, atol(szTemp)%100);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Amount:");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szStr);

    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szAuthCode);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Auth Code:");
    setLCDPrint(6, DISPLAY_POSITION_LEFT,  szStr);


    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szInvoiceNo, szTemp, INVOICE_BCD_SIZE);
    sprintf(szStr, "%s", szTemp);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "Invoice NO:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szTemp);
     
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);   
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
        {
            CTOS_LCDTClearDisplay();
            return  READ_CARD_TIMEOUT;
        }
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return d_OK;
        }
    }
}

void vduiLightOn(void)
{
#if 0
	if (inCTOSS_CheckBatteryChargeStatus() != d_OK)
	{
		CTOS_BackLightSet (d_BKLIT_KBD, d_OFF);
		return;
	}
#endif	
    if (strTCT.fHandsetPresent)  
        //CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,80000);
		CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffffff);
    else
        CTOS_BackLightSet (d_BKLIT_LCD, d_ON);
}

void vduiKeyboardBackLight(BOOL fKeyBoardLight)
{
	if (inCTOSS_CheckBatteryChargeStatus() != d_OK)
	{
		CTOS_BackLightSet (d_BKLIT_KBD, d_OFF);
		return;
	}
	
    if (strTCT.fHandsetPresent) 
    {
        if(VS_TRUE == fKeyBoardLight)
        {
            
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffffff);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,0xffffffff);
        }
        else
        {
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_OFF,3000);
        }

    }
    else
    {
        if(VS_TRUE == fKeyBoardLight)
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffffff);
        else
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
    }
}

void vduiPowerOff(void)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;
        
    vduiClearBelow(1);
    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"WANT TO POWER");
    vduiDisplayStringCenter(5,"OFF TERMINAL");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
    c=WaitKey(60);
    
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

void vduiDisplayStringCenter(unsigned char  y,unsigned char *sBuf)
{
    //CTOS_LCDTPrintXY(1,y,"                ");
    //CTOS_LCDTPrintXY(1,y,"                          ");
    //CTOS_LCDTPrintXY((20-strlen(sBuf))/2+1,y,sBuf);
	CTOS_LCDTPrintAligned(y,"                          ", d_LCD_ALIGNLEFT);	  
	CTOS_LCDTPrintAligned(y,sBuf, d_LCD_ALIGNCENTER);     
}

void vduiClearBelow(int line)
{
    int i;
	int count = 0;
	if ((strTCT.byTerminalType%2) == 0)
		count = 16;
	else
		count = 8;
    for(i=line;i<=count;i++)
        clearLine(i);
        
}

void vduiWarningSound(void)
{
    CTOS_LEDSet(d_LED1, d_ON);
    CTOS_LEDSet(d_LED2, d_ON);
    CTOS_LEDSet(d_LED3, d_ON);
    
    CTOS_Beep();
    CTOS_Delay(300);
    CTOS_Beep();
    
    CTOS_LEDSet(d_LED1, d_OFF);
    CTOS_LEDSet(d_LED2, d_OFF);
    CTOS_LEDSet(d_LED3, d_OFF);
}

void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg)
{
	int inRowtmp;
	
	if ((strTCT.byTerminalType%2) == 0)
		inRowtmp = V3_ERROR_LINE_ROW;
	else
        inRowtmp = inRow;
			
    CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRowtmp, msg);
    CTOS_Beep();
    CTOS_Delay(1500);
}

void vdCTOS_DispStatusMessage(char* szMsg)
{
	if ((strTCT.byTerminalType%2) == 0)
	{
			CTOS_LCDTPrintXY(1, 14, "                                        ");
			CTOS_LCDTPrintXY(1, 14, szMsg);
	}
	else
	{
		CTOS_LCDTPrintXY(1, 8, "                                        ");
		CTOS_LCDTPrintXY(1, 8, szMsg);
	}
	CTOS_Delay(500);
}


/* functions for loyalty - Meena 15/01/2012 - start*/
short vduiAskConfirmContinue(void)
{
    unsigned char key;
  
    vduiClearBelow(2);
    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"YOU WANT TO");
    vduiDisplayStringCenter(5,"CONTINUE?");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL)
            return -1;
        else
            vduiWarningSound();
    }
    
}



BYTE struiGetchWithTimeOut(void)
{
    unsigned char c;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3,3000);
    
    while(1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey){ //If isKey is TRUE, represent key be pressed //
            vduiLightOn();
            //Get a key from keyboard //
            CTOS_KBDGet(&c);
            return c;   
        }
        else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return d_KBD_CANCEL;
        }
    }
}

/* functions for loyalty - Meena 15/01/2012 - End*/

short inuiAskSettlement(void)
{
    unsigned char key;
    while(1) 
    {
        vduiClearBelow(2);
        vduiDisplayStringCenter(2,"DAILY SETTLEMENT");
        vduiDisplayStringCenter(3,"NOTIFICATION");

        vduiDisplayStringCenter(5,"PERFORM");
        vduiDisplayStringCenter(6,"SETTLEMENT?");
        vduiDisplayStringCenter(8,"NO[X] YES[OK]");

        CTOS_KBDGet(&key);
        if(key==d_KBD_ENTER)
            return d_OK;
        else if(key==d_KBD_CANCEL)
            return d_KBD_CANCEL;
        else if(key==d_KBD_F1)
            vduiPowerOff();
    }
        
}

void vduiDisplaySignalStrengthBatteryCapacity(void)
{
    
    BYTE bCapacity, msg2[50];
    USHORT dwRet;
    short insign;
    
    
    if(GPRSCONNETSTATUS== d_OK && strTCT.inMainLine == GPRS_MODE)
    {
        insign=incommSignal();
        if(insign==-1)
        {
            CTOS_LCDTPrintXY (9,1, "SIGNAL:NA");
        }
        else
        {           
            if(insign/6 == 0)
                CTOS_LCDTPrintXY (9,1, "NO SIGNAL");
            else if(insign/6 == 1)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:l____"); 
            }
            else if(insign/6 == 2)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:ll___"); 
            }
            else if(insign/6 == 3)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lll__"); 
            }
            else if(insign/6 == 4)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:llll_"); 
            }
            else if(insign/6 == 5)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lllll"); 
            }
            
        }
    }
    
    dwRet= CTOS_BatteryGetCapacityByIC(&bCapacity);  
    if(dwRet==d_OK)
    {
        sprintf(msg2, "B:%d%% ", bCapacity);
        CTOS_LCDTPrintXY (3,1, msg2);
    }
                
}

void vdSetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szMessage);
    memset(szErrorMessage,0x00,sizeof(szErrorMessage));
    
    if (inErrLen > 0)
    {
        if (inErrLen > ERRORLEN)
            inErrLen = ERRORLEN;
        
        memcpy(szErrorMessage,szMessage,inErrLen);
    }
}

int inGetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szErrorMessage);

    if (inErrLen > 0)
    {       
        memcpy(szMessage,szErrorMessage,inErrLen);
    }
    
    return inErrLen;
}

//gcitra
void setLCDPrint27(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (27 - lens) / 2 + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 27 - lens + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
    }
}
//gcitra
//sidumili: display message
void vdDisplayMessage(char *szLine1Msg, char *szLine2Msg, char *szLine3Msg)
{
    CTOS_LCDTClearDisplay();
		//vduiClearBelow(2);
		vduiDisplayStringCenter(4, szLine1Msg);
		vduiDisplayStringCenter(5, szLine2Msg);
		vduiDisplayStringCenter(6, szLine3Msg);
		WaitKey(1);
}
//sidumili

#ifdef PIN_CHANGE_ENABLE
void vdDisplayErrorMsg2(int inColumn, int inRow,  char *msg, char *msg2)
{
	int inRowtmp;
	
	if ((strTCT.byTerminalType%2) == 0)
		inRowtmp = V3_ERROR_LINE_ROW;
	else
        inRowtmp = inRow;
			
    CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
	CTOS_LCDTPrintXY(inColumn, inRowtmp+1, "                                        ");
	
    CTOS_LCDTPrintXY(inColumn, inRowtmp, msg);
	CTOS_LCDTPrintXY(inColumn, inRowtmp+1, msg2);
    CTOS_Beep();
    CTOS_Delay(1500);
}
#endif


//sidumili: confirmation
short vduiAskEnterToConfirm(void)
{
    unsigned char key;
  
    
    CTOS_LCDTPrintXY(1,8,"CONFIRM?NO[X]YES[OK]");
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL){
					
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
						if (strCPT.inCommunicationMode == DIAL_UP_MODE){
										inCTOS_inDisconnect();
						}
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
					
            return -1;
        	}
        else
            vduiWarningSound();
    }
    
}
//sidumili:
 
void inDisplayLeftRight(int inLine, unsigned char *strLeft, unsigned char *strRight, int inMode)
{
	char szStr[48+1]; 
    int inLength=0, inSize=0;
	inLength=inMode;	
	memset(szStr, 0x20, sizeof(szStr));
	inSize=strlen(strRight);
    memcpy(&szStr[inLength-inSize], strRight, inSize);
	inSize=strlen(strLeft);
    memcpy(szStr, strLeft, strlen(strLeft));

	//if (isCheckTerminalMP200() == d_OK)
		//setLCDPrint27(inLine, d_LCD_ALIGNLEFT, szStr);
	//else
		//CTOS_LCDTPrintAligned(inLine, szStr, d_LCD_ALIGNLEFT);
	CTOS_LCDTPrintXY(1, inLine, szStr);
}

