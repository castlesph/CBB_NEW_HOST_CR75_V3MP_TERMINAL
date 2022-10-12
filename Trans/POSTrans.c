/*******************************************************************************

 *******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <emv_cl.h>
#include <vwdleapi.h>

#include <sys/stat.h>   
#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\display.h"

#include "..\Debug\Debug.h"

#include "..\Includes\CTOSInput.h"

#include "..\comm\v5Comm.h"
#include "..\Accum\Accum.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Comm\V5Comm.h"
#include "..\debug\debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\CardUtil.h"
#include "..\Debug\Debug.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\myEZLib.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Ctls\POSCtls.h"
#include "..\Ctls\PosWave.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\print\print.h"
#include "..\pinpad\Pinpad.h"
#include "..\Includes\POSSetting.h"
#include "..\Htle\htlesrc.h"
#include "..\External\External.h"
#include "..\SmartCard\MPUCard.h"

#include "..\Aptrans\MultiShareCOM.h"
#include "..\Aptrans\MultiShareECR.h"

// patrck fix code 20141208
extern EMVCL_RC_DATA_ANALYZE stRCDataAnalyze;
extern int ginPinByPass;

static BOOL fGetCardNO = 0;
int inFallbackToMSR = FAIL;
static char szBaseAmount[6 + 1];

BOOL fCUPTrans = 0;
BOOL fCashAdvApp = 0;

#ifdef SET_IPP_MENU
BOOL fIPPApp = 0;
#endif

BOOL fMPUTrans = 0;
BOOL fMPUCard = 0;

BOOL fECRTxnFlg = 0;

BOOL fButtonIdle = 0;
BOOL fCardIdle = 0;

BOOL fIdleScreen = 0;

BOOL fSIGNON = 0;

BOOL fSETTLESIGNON = 0;


#ifdef MPUPIOnUsFlag
BOOL fMPUPIOnUs = 0;
#endif

char g_szBaseAmount[6+1];

char szECRTipAmount[6+1];

extern ECR_REQ ECRReq;


#ifdef CBB_FIN_ROUTING
void vdSetSIGNON(BOOL flag) {
    fSIGNON = flag;
}

BOOL fGetSIGNON(void) {
    return fSIGNON;
}

//for auto signon after settlement
void vdSetSettleSIGNON(BOOL flag) {
    fSETTLESIGNON = flag;
}

BOOL fGetSettleSIGNON(void) {
    return fSETTLESIGNON;
}

#endif

void vdSetECRTransactionFlg(BOOL flg) {
    put_env_int("ECRTRANS", flg);
    fECRTxnFlg = flg;
}

BOOL fGetECRTransactionFlg(void) {
    fECRTxnFlg = get_env_int("ECRTRANS");
    return fECRTxnFlg;
}

void vdSetCUPTransFlag(BOOL flag) {
    fCUPTrans = flag;
}

BOOL fGetCUPTransFlag(void) {
    return fCUPTrans;
}

void vdSetCashAdvAppFlag(BOOL flag) {
    fCashAdvApp = flag;
}

BOOL fGetCashAdvAppFlag(void) {
    return fCashAdvApp;
}

void vdSetIPPAppFlag(BOOL flag) {
    fIPPApp = flag;
}

BOOL fGetIPPAppFlag(void) {
    return fIPPApp;
}


void vdSetMPUTrans(BOOL flag) {
    fMPUTrans = flag;
}

/*MPU trans mean, the MPU menu/MPU Application. not same as Noraml MPU credit in Credit application*/
BOOL fGetMPUTrans(void) {
    return fMPUTrans;
}

void vdSetMPUCard(BOOL flag) {
    fMPUCard = flag;
}

BOOL fGetMPUCard(void) {
    return fMPUCard;
}

#ifdef MPUPIOnUsFlag
void vdSetMPUPIOnUs(BOOL flag) {
    fMPUPIOnUs = flag;
}

BOOL fGetMPUPIOnUs(void) {
    return fMPUPIOnUs;
}
#endif

BOOL fChkCUPCard(void) {
    if (srTransRec.szPAN[0] == '6')
        return VS_TRUE;

    return VS_FALSE;
}

void vdSetButtonFromIdle(BOOL fSet) {
    fButtonIdle = fSet;
}

BOOL fCheckButtonFromIdle(void) {
    return fButtonIdle;
}

void vdSetCardFromIdle(BOOL fSet) {
    fCardIdle = fSet;
}

BOOL fCheckCardFromIdle(void) {
    return fCardIdle;
}

extern int isdigit(int c);

USHORT shCTOS_GetNum(IN USHORT usY, IN USHORT usLeftRight, OUT BYTE *baBuf, OUT USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS) {

    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    BYTE bKey = 0x00;
    BYTE bInputStrData[128];
    USHORT usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof (bInputStrData));

    if (usTimeOutMS > 0)
        CTOS_TimeOutSet(TIMER_ID_1, usTimeOutMS);

    vdDebug_LogPrintf("start [%d] data[%s]", strlen(baBuf), baBuf);
    if (strlen(baBuf) > 0) {
        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        memset(bDisplayStr, 0x20, usMaxLen * 2);
        usInputStrLen = strlen(baBuf);
        strcpy(bInputStrData, baBuf);
        if (0x01 == usLeftRight) {
            strcpy(&bDisplayStr[(usMaxLen - strlen(bInputStrData))*2], bInputStrData);
            //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
            CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNRIGHT);
        } else {
            memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
            //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
            CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNLEFT);
        }
    }

    while (1) {
        //        vduiLightOn(); // patrick remark for flash light always
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL;
        }

        CTOS_KBDHit(&bKey);

        switch (bKey) {
            case d_KBD_DOT:
                break;
            case d_KBD_CLEAR:
                if (usInputStrLen) {
                    usInputStrLen--;
                    bInputStrData[usInputStrLen] = 0x00;

                    memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
                    memset(bDisplayStr, 0x20, usMaxLen * 2);
                    if (0x01 == usLeftRight) {
                        strcpy(&bDisplayStr[(usMaxLen - strlen(bInputStrData))*2], bInputStrData);
                        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNRIGHT);
                    } else {
                        memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                        //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNLEFT);
                    }

                    if (1 == fGetCardNO && usInputStrLen == 0) {
                        *usStrLen = 0;
                        baBuf[0] = 0x00;
                        return d_KBD_CANCEL;
                    }
                }
                break;
            case d_KBD_CANCEL:
                *usStrLen = 0;
                baBuf[0] = 0x00;
                return d_KBD_CANCEL;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                if (usInputStrLen < usMaxLen) {
                    bInputStrData[usInputStrLen++] = bKey;

                    memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
                    memset(bDisplayStr, 0x20, usMaxLen * 2);
                    if (0x01 == usLeftRight) {
                        strcpy(&bDisplayStr[(usMaxLen - strlen(bInputStrData))*2], bInputStrData);
                        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNRIGHT);
                    } else {
                        memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                        //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNLEFT);
                    }
                }
                break;
            case d_KBD_ENTER:
                if (usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen) {
                    *usStrLen = usInputStrLen;
                    strcpy(baBuf, bInputStrData);
                    return *usStrLen;
                } else if (usByPassAllow && 0 == usInputStrLen) {
                    *usStrLen = usInputStrLen;
                    baBuf[0] = 0x00;
                    return *usStrLen;
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

USHORT shCTOS_GetExpDate(IN USHORT usY, IN USHORT usLeftRight, OUT BYTE *baBuf, OUT USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS) {

    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    BYTE bKey = 0x00;
    BYTE bInputStrData[20];
    BYTE bInputFormatStr[20];
    USHORT usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof (bInputStrData));

    if (usTimeOutMS > 0)
        CTOS_TimeOutSet(TIMER_ID_1, usTimeOutMS);

    while (1) {
        //vduiLightOn();
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL;
        }

        CTOS_KBDHit(&bKey);

        switch (bKey) {
            case d_KBD_DOT:
                break;
            case d_KBD_CLEAR:
                if (usInputStrLen) {
                    usInputStrLen--;
                    bInputStrData[usInputStrLen] = 0x00;

                    memset(bInputFormatStr, 0x00, sizeof (bInputFormatStr));
                    if (usInputStrLen >= 2) {
                        memcpy(bInputFormatStr, bInputStrData, 2);
                        strcat(bInputFormatStr, "/");
                        if (usInputStrLen > 2)
                            strcat(bInputFormatStr, &bInputStrData[2]);
                    } else {
                        strcpy(bInputFormatStr, bInputStrData);
                    }

                    memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
                    memset(bDisplayStr, 0x20, (usMaxLen + 1)*2);

                    if (0x01 == usLeftRight) {
                        strcpy(&bDisplayStr[(usMaxLen + 1 - strlen(bInputFormatStr))*2], bInputFormatStr);
                        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNRIGHT);
                    } else {
                        memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                        //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNLEFT);
                    }
                }
                break;
            case d_KBD_CANCEL:
                *usStrLen = 0;
                baBuf[0] = 0x00;
                return d_KBD_CANCEL;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                if (usInputStrLen < usMaxLen) {
                    bInputStrData[usInputStrLen++] = bKey;

                    memset(bInputFormatStr, 0x00, sizeof (bInputFormatStr));
                    if (usInputStrLen >= 2) {
                        memcpy(bInputFormatStr, bInputStrData, 2);
                        strcat(bInputFormatStr, "/");
                        if (usInputStrLen > 2)
                            strcat(bInputFormatStr, &bInputStrData[2]);
                    } else {
                        strcpy(bInputFormatStr, bInputStrData);
                    }

                    memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
                    memset(bDisplayStr, 0x20, (usMaxLen + 1)*2);
                    if (0x01 == usLeftRight) {
                        strcpy(&bDisplayStr[(usMaxLen + 1 - strlen(bInputFormatStr))*2], bInputFormatStr);
                        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNRIGHT);
                    } else {
                        memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                        //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNLEFT);
                    }
                }
                break;
            case d_KBD_ENTER:
                if (usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen) {
                    *usStrLen = usInputStrLen;
                    strcpy(baBuf, bInputStrData);
                    return *usStrLen;
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

USHORT getExpDate(OUT BYTE *baBuf) {
    BYTE szMonth[3];
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 4;
    USHORT usMaxLen = 4;
    USHORT usInputLine = 8;

    while (1) {
        usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL)
            return (d_EDM_USER_CANCEL);
        memset(szMonth, 0x00, sizeof (szMonth));
        memcpy(szMonth, baBuf, 2);
        if (atol(szMonth) > 12 || 0 == atol(szMonth)) {
            baBuf[0] = 0x00;
            clearLine(8);
            vdDisplayErrorMsg(1, 8, "INVALID FORMAT");
            clearLine(8);
            continue;
        } else {
            return (d_OK);
        }
    }
}

USHORT getCardNO(OUT BYTE *baBuf) {
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 14;
    USHORT usMaxLen = 19;
    USHORT usInputLine = 8;

    while (1) {
        usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL)
            return (d_EDM_USER_CANCEL);
        if (usRet >= usMinLen && usRet <= usMaxLen) {
            return (d_OK);
        }

        baBuf[0] = 0x00;
    }
}

unsigned char WaitKey(short Sec) {
    unsigned char c;
    long mlsec;

    mlsec = Sec * 100;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3, mlsec);
    while (1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey) { //If isKey is TRUE, represent key be pressed //

            vduiLightOn();
            CTOS_KBDGet(&c);
            return c;
        } else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES) {
            return d_KBD_CANCEL;
        }
    }
}

unsigned char WaitKeyNoTIMEOUT(void) {
    unsigned char c;
    BOOL isKey;

    while (1)//loop for time out
    {
        CTOS_Delay(200);
        CTOS_KBDInKey(&isKey);
        if (isKey) { //If isKey is TRUE, represent key be pressed //

            vduiLightOn();
            CTOS_KBDGet(&c);
            return c;
        }

    }
}

void vduiApiAmount(unsigned char *ou, unsigned char *ascamt, unsigned char len) {
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int ii, jj, tt;
    unsigned char ch;
    unsigned char aa;
    unsigned char buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;
    ii = 0;


    ou[jj++] = strCST.szCurSymbol[0];
    ou[jj++] = strCST.szCurSymbol[1];
    ou[jj++] = strCST.szCurSymbol[2];

    for (ii = 0; ii < len; ii++) {
        ch = ascamt[ii];
        if ((tt == 0) && (ch == 'C')) {
            tt = 1;
        } else if ((tt == 0) && (ch == 'D')) {
            tt = 1;
            ou[jj++] = '-';
        } else if (ch < 0x30 && ch > 0x39) {
            break;
        }
    }


    len = ii;
    aa = 0;
    for (ii = tt; (ii + 3) < len; ii++) {
        ch = ascamt[ii];
        if ((ch == '0') && (aa == 0)) {
            continue;
        }

        if (ch > 0x29 && ch < 0x40)//if(isdigit(ch) /* && (ch !='0') */ )
        {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for (ii = 0; ii < len; ii++) {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

void vduiApiPoint(unsigned char *ou, unsigned char *ascamt, unsigned char len) {
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int ii, jj, tt;
    unsigned char ch;
    unsigned char aa;
    unsigned char buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;

    ou[jj++] = 'P';
    ou[jj++] = 'T';
    ou[jj++] = 'S';
    ou[jj++] = ' ';
    for (ii = 0; ii < len; ii++) {
        ch = ascamt[ii];
        if ((tt == 0) && (ch == 'C')) {
            tt = 1;
        } else if ((tt == 0) && (ch == 'D')) {
            tt = 1;
            ou[jj++] = '-';
        } else if (ch < 0x30 && ch > 0x39) {
            break;
        }
    }


    len = ii;
    aa = 0;
    for (ii = tt; (ii + 3) < len; ii++) {
        ch = ascamt[ii];
        if ((ch == '0') && (aa == 0)) {
            continue;
        }

        if (ch > 0x29 && ch < 0x40) {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for (ii = 0; ii < len; ii++) {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

//mode  1=amount , 2=string, 3=IP  4=password, 5=Point

unsigned char struiApiGetStringSub
(
        unsigned char *strDisplay,
        short x,
        short y,
        unsigned char *ou,
        unsigned char mode,
        short minlen,
        short maxlen
        ) {

    unsigned char srDestIP[MAX_CHAR_PER_LINE + 1];
    unsigned char amtdis[MAX_CHAR_PER_LINE + 1];
    unsigned char c;
    int n;
    int i;

    memset(srDestIP, 0x00, sizeof (srDestIP));
    n = 0;

    vduiClearBelow(y);

    if (mode == MODE_AMOUNT) {
        vduiClearBelow(8);
        vduiApiAmount(amtdis, srDestIP, n);
        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);
        CTOS_LCDTPrintAligned(8, amtdis, d_LCD_ALIGNRIGHT);

    } else if (mode == MODE_POINT) {
        vduiClearBelow(8);
        vduiApiPoint(amtdis, srDestIP, n);
        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);
        CTOS_LCDTPrintAligned(8, amtdis, d_LCD_ALIGNRIGHT);

    }

    while (1) {

        //CTOS_LCDTPrintXY(x,y,strDisplay);
        CTOS_LCDTPrintAligned(y, strDisplay, d_LCD_ALIGNLEFT);
        c = WaitKey(30); //CTOS_KBDGet(&c);
        vduiLightOn();

        if (c == d_KBD_ENTER) {
            if ((n >= minlen) && (n <= maxlen)) {
                ou[n] = 0;
                memcpy(ou, srDestIP, n + 1);
                if (mode == MODE_FOODITEM && atoi(ou) == 0) {
                    return d_KBD_CANCEL;
                }

                if (mode == MODE_AMOUNT && atoi(ou) == 0) {
                    i = atoi(ou);
                    memset(srDestIP, 0x00, sizeof (srDestIP));
                    n = 0;
                    vduiWarningSound();
                } else
                    return d_KBD_ENTER;
            }

        }
        else if ((c == d_KBD_CANCEL) && (n == 0)) {
            memset(srDestIP, 0x00, sizeof (srDestIP));
            return d_KBD_CANCEL;
        }

        else {
            if (c == d_KBD_CLEAR) {
                if (n > 0) {
                    n--;
                    srDestIP[n] = '\0';
                }
            } else if ((c == d_KBD_CANCEL)) {
                memset(srDestIP, 0x00, sizeof (srDestIP));
                n = 0;
            }
            else if (((c == d_KBD_DOT) & (mode == MODE_IPADDRESS)) || ((c == d_KBD_DOWN) & (mode == MODE_IPADDRESS))) {
                srDestIP[n] = '.';
                n++;
            } else if (c == d_KBD_DOT || c == d_KBD_F3 || c == d_KBD_F4 || c == d_KBD_00) {
                ;
            } else if (c == d_KBD_UP || c == d_KBD_DOWN) {
                return c;
            } else if (n < maxlen) {
                srDestIP[n] = c;
                n++;
            } else {

            }

            if (mode == MODE_AMOUNT) {
                vduiClearBelow(8);
                vduiApiAmount(amtdis, srDestIP, n);
                //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);
                CTOS_LCDTPrintAligned(8, amtdis, d_LCD_ALIGNRIGHT);

            } else if (mode == MODE_POINT) {
                vduiClearBelow(8);
                vduiApiPoint(amtdis, srDestIP, n);
                //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);
                CTOS_LCDTPrintAligned(8, amtdis, d_LCD_ALIGNRIGHT);

            } else if (mode == MODE_PASSWORD) {
                for (i = 0; i < n; i++)
                    amtdis[i] = '*';
                amtdis[n] = 0;
                vduiClearBelow(8);
                //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,amtdis);
                CTOS_LCDTPrintAligned(8, amtdis, d_LCD_ALIGNRIGHT);
            } else {
                vduiClearBelow(8);
                //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,srDestIP);
                CTOS_LCDTPrintAligned(8, srDestIP, d_LCD_ALIGNRIGHT);
            }
        }

    }

}

void vdCTOSS_GetMemoryStatus(char *Funname) {
    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    memset(szUsedDiskSize, 0, sizeof (szUsedDiskSize));
    memset(szTotalDiskSize, 0, sizeof (szTotalDiskSize));
    memset(szUsedRamSize, 0, sizeof (szUsedRamSize));
    memset(szTotalRamSize, 0, sizeof (szTotalRamSize));
    memset(szAvailableRamSize, 0, sizeof (szAvailableRamSize));
    memset(szAvailableDiskSize, 0, sizeof (szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus(&ulUsedDiskSize, &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize, "%s:%ld", "Total disk", ulTotalDiskSize);
    sprintf(szUsedDiskSize, "%s:%ld", "Used   disk", ulUsedDiskSize);
    sprintf(szAvailableDiskSize, "%s:%ld", "Avail disk", ulAvailDiskSize);

    sprintf(szTotalRamSize, "%s:%ld", "Total RAM", ulTotalRamSize);
    sprintf(szUsedRamSize, "%s:%ld", "Used   RAM", ulUsedRamSize);
    sprintf(szAvailableRamSize, "%s:%ld", "Avail RAM", ulAvailableRamSize);
    vdDebug_LogPrintf("[%s][%ld],[%ld][%ld][%ld]", Funname, ulUsedDiskSize, ulTotalRamSize, ulUsedRamSize, ulAvailableRamSize);

    CTOS_LCDTClearDisplay();

    CTOS_LCDTPrintXY(1, 1, szTotalDiskSize);
    CTOS_LCDTPrintXY(1, 2, szUsedDiskSize);
    CTOS_LCDTPrintXY(1, 3, szAvailableDiskSize);

    CTOS_LCDTPrintXY(1, 5, szTotalRamSize);
    CTOS_LCDTPrintXY(1, 6, szUsedRamSize);
    CTOS_LCDTPrintXY(1, 7, szAvailableRamSize);
    WaitKey(60);

}

int inCTOSS_CheckMemoryStatus() {
#define SAFE_LIMIT_SIZE 5000

    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    memset(szUsedDiskSize, 0, sizeof (szUsedDiskSize));
    memset(szTotalDiskSize, 0, sizeof (szTotalDiskSize));
    memset(szUsedRamSize, 0, sizeof (szUsedRamSize));
    memset(szTotalRamSize, 0, sizeof (szTotalRamSize));
    memset(szAvailableRamSize, 0, sizeof (szAvailableRamSize));
    memset(szAvailableDiskSize, 0, sizeof (szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus(&ulUsedDiskSize, &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize);
    //    vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize, "%s:%ld", "Total disk", ulTotalDiskSize);
    sprintf(szUsedDiskSize, "%s:%ld", "Used   disk", ulUsedDiskSize);
    sprintf(szAvailableDiskSize, "%s:%ld", "Avail disk", ulAvailDiskSize);

    sprintf(szTotalRamSize, "%s:%ld", "Total RAM", ulTotalRamSize);
    sprintf(szUsedRamSize, "%s:%ld", "Used   RAM", ulUsedRamSize);
    sprintf(szAvailableRamSize, "%s:%ld", "Avail RAM", ulAvailableRamSize);
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]", ulAvailDiskSize, ulAvailableRamSize);

    if (ulAvailDiskSize < SAFE_LIMIT_SIZE) {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 7, "Settle  soon");
        vdDisplayErrorMsg(1, 8, "Insufficient Memory");
        return FAIL;
    }

    if (ulAvailableRamSize < SAFE_LIMIT_SIZE) {
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("Insufficient RAM");
        return FAIL;
    }

    return d_OK;

}

void vdCTOS_SyncHostDateTime() {
    CTOS_RTC SetRTC;
    char szDate[4 + 1];
    char szTime[6 + 1];
    char szBuf[2 + 1];

    if (srTransRec.byOffline == CN_TRUE)
        return;
    CTOS_RTCGet(&SetRTC);
    vdDebug_LogPrintf("sys year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);

    vdDebug_LogPrintf("year[%02X],time[%02x:%02x:%02x]date[%02x][%02x]", SetRTC.bYear, srTransRec.szTime[0], srTransRec.szTime[1], srTransRec.szTime[2], srTransRec.szDate[0], srTransRec.szDate[1]);

    if(srTransRec.szDate[0] == 0x00)
    {
		memset(srTransRec.szDate, 0x00, sizeof(srTransRec.szDate));
		memset(szDate, 0, sizeof (szDate));
		sprintf(szDate ,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
		wub_str_2_hex(szDate, srTransRec.szDate, 4);
    }
	
    memset(szDate, 0, sizeof (szDate));
    memset(szTime, 0, sizeof (szTime));

    wub_hex_2_str(srTransRec.szDate, szDate, 2);
    wub_hex_2_str(srTransRec.szTime, szTime, 3);
    if ((strlen(szDate) <= 0) || (strlen(szTime) <= 0))
        return;
    sprintf(szBuf, "%02x", (unsigned int) atol(szDate) % 100);
    wub_str_2_hex(szBuf, &(SetRTC.bDay), 2);

    sprintf(szBuf, "%02x", (unsigned int) atol(szDate) / 100);
    wub_str_2_hex(szBuf, &(SetRTC.bMonth), 2);

    sprintf(szBuf, "%02x", (unsigned int) atol(szTime) / 10000);
    wub_str_2_hex(szBuf, &(SetRTC.bHour), 2);

    sprintf(szBuf, "%02x", (unsigned int) atol(szTime) % 10000 / 100);
    wub_str_2_hex(szBuf, &(SetRTC.bMinute), 2);

    sprintf(szBuf, "%02x", (unsigned int) atol(szTime) % 100);
    wub_str_2_hex(szBuf, &(SetRTC.bSecond), 2);

    /*set system clock*/
    CTOS_RTCSet(&SetRTC);

    vdDebug_LogPrintf("set year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);

    return;
}

int file_exist(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int inCLearTablesStructure(void) {

    memset(&strCDT, 0x00, sizeof (STRUCT_CDT));
    memset(&strIIT, 0x00, sizeof (STRUCT_IIT));
    memset(&strEMVT, 0x00, sizeof (STRUCT_EMVT));
    memset(&strHDT, 0x00, sizeof (STRUCT_HDT));
    return d_OK;
}

void vdCTOS_TxnsBeginInit(void) {
    BYTE bEntryMode = 0;
    int inRet = d_NO;
    extern BYTE szErrRespMsg[64 + 1];

    put_env("#P1TEXT", " ", 1);
    put_env("#P2TEXT", " ", 1);
    put_env("#P3TEXT", " ", 1);
    put_env("#P4TEXT", " ", 1);

    memset(szMacData, 0x00, sizeof (szMacData));
    inMacDataCnt = 0;

    if (d_OK == inCTOS_ValidFirstIdleKey())
        bEntryMode = srTransRec.byEntryMode;

    memset(&srTransRec, 0x00, sizeof (TRANS_DATA_TABLE));

    memset(szErrRespMsg, 0x00, sizeof (szErrRespMsg));

    ginPinByPass = 0;

    //Clear Previous Transaction Data
    inCLearTablesStructure();
    if (0 != bEntryMode)
        srTransRec.byEntryMode = bEntryMode;

    vdSetErrorMessage("");
    vdCTOSS_SetWaveTransType(0);
    vdSetInit_Connect(0);

    vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check if main app");
    //add for ECR only MianAP get the ECR command, if Sub AP will cannot error
    //if (inMultiAP_CheckMainAPStatus() == d_OK)
    if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
    {
        vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check ECR on?[%d]", strTCT.fECR);
        if (strTCT.fECR && 1 == fGetECRTransactionFlg()) // tct
        {
            vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check Database exist");
			#if 0
            if (0)) {
                vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data");
                inRet = inCTOS_MultiAPGetData();
                vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data ret[%d]", inRet);
                if (d_OK != inRet)
                    return;
            }
			#else
			
			wub_str_2_hex(ECRReq.amount, srTransRec.szBaseAmount, 12);
		    wub_str_2_hex(ECRReq.inv_no,srTransRec.szInvoiceNo, 6);

			strcpy(srTransRec.szAuthCode, ECRReq.szAuthCode);
			strcpy(srTransRec.szRRN, ECRReq.szECRREF);
			strcpy(srTransRec.szMID, ECRReq.szMID);

            memset(szECRTipAmount, 0, sizeof(szECRTipAmount));
			wub_str_2_hex(ECRReq.szTipAmount, szECRTipAmount, 12);
            //vdDebug_LogPrintf("ECRReq.szAuthCode[%s]", ECRReq.szAuthCode);
            //vdDebug_LogPrintf("ECRReq.szECRREF[%s]", ECRReq.szECRREF);
            //vdDebug_LogPrintf("ECRReq.szMID[%s]", ECRReq.szMID);
			 
			srTransRec.HDTid=ECRReq.HDTid;
			#endif
        }

    }
}

void vdCTOS_TransEndReset(void) {
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    char szErrMsg[30 + 1];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;

    // patrick to print contactless EMV data 20151019
    inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);
    if (CARD_ENTRY_ICC == srTransRec.byEntryMode) {
        usCTOSS_EMV_MultiDataGet(PRINT_EMV_TAGS_LIST, &srTransRec.usChipDataLen, srTransRec.baChipData);
        inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);
    } else
        inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);

    DebugAddHEX("PRINT_EMV_TAGS_LIST", srTransRec.baChipData, srTransRec.usChipDataLen);
    // patrick to print contactless EMV data 20151019

    vdDebug_LogPrintf("vdCTOS_TransEndReset ECR?[%d]", fGetECRTransactionFlg());
    CTOS_LCDTClearDisplay();
    // patrick fix code 20141209
    //vduiClearBelow(2);
    ginPinByPass = 0;

    memset(&stRCDataAnalyze, 0x00, sizeof (EMVCL_RC_DATA_ANALYZE));

    memset(szErrMsg, 0x00, sizeof (szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0) {
        vdDisplayErrorMsg(1, 8, szErrMsg);
    }

    vdDebug_LogPrintf("byEntryMode 1 = %d", srTransRec.byEntryMode);

    if (CARD_ENTRY_ICC == srTransRec.byEntryMode || CARD_ENTRY_EASY_ICC == srTransRec.byEntryMode)
        vdRemoveCard();
    else
        CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

    if (!fGetECRTransactionFlg()) {
        memset(&srTransRec, 0x00, sizeof (TRANS_DATA_TABLE));
    }

    CTOS_KBDBufFlush();

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    vdSetErrorMessage("");
    vdCTOSS_SetWaveTransType(0);

    vdSetMPUCard(FALSE);

    ushCTOS_ClearePadSignature();
    //Clear Previous Transaction Data
    inCLearTablesStructure();

    vduiClearBelow(2);

    return;
}

void vdCTOS_TransEndResetEx(void) {
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    char szErrMsg[30 + 1];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;

    // patrick to print contactless EMV data 20151019
    inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);
    if (CARD_ENTRY_ICC == srTransRec.byEntryMode) {
        usCTOSS_EMV_MultiDataGet(PRINT_EMV_TAGS_LIST, &srTransRec.usChipDataLen, srTransRec.baChipData);
        inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);
    } else
        inMultiAP_Database_EMVTransferDataWrite(srTransRec.usChipDataLen, srTransRec.baChipData);

    DebugAddHEX("PRINT_EMV_TAGS_LIST", srTransRec.baChipData, srTransRec.usChipDataLen);
    // patrick to print contactless EMV data 20151019

    vdDebug_LogPrintf("vdCTOS_TransEndReset ECR?[%d]", fGetECRTransactionFlg());
    //CTOS_LCDTClearDisplay();
    // patrick fix code 20141209
    //vduiClearBelow(2);
    ginPinByPass = 0;

    memset(&stRCDataAnalyze, 0x00, sizeof (EMVCL_RC_DATA_ANALYZE));

    memset(szErrMsg, 0x00, sizeof (szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0) {
        vdDisplayErrorMsg(1, 8, szErrMsg);
    }

    vdDebug_LogPrintf("byEntryMode 1 = %d", srTransRec.byEntryMode);

    if (CARD_ENTRY_ICC == srTransRec.byEntryMode || CARD_ENTRY_EASY_ICC == srTransRec.byEntryMode)
        vdRemoveCard();
    else
        CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

    if (!fGetECRTransactionFlg()) {
        memset(&srTransRec, 0x00, sizeof (TRANS_DATA_TABLE));
    }

    CTOS_KBDBufFlush();

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    vdSetErrorMessage("");
    vdCTOSS_SetWaveTransType(0);

    vdSetMPUCard(FALSE);

    ushCTOS_ClearePadSignature();
    //Clear Previous Transaction Data
    inCLearTablesStructure();

    return;
}

void vdCTOS_SetTransEntryMode(BYTE bEntryMode) {
    srTransRec.byEntryMode = bEntryMode;
}

void vdCTOS_SetTransType(BYTE bTxnType) {
    srTransRec.byTransType = bTxnType;
    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
}

short shCTOS_SetMagstripCardTrackData(BYTE* baTk1Buf, USHORT usTk1Len, BYTE* baTk2Buf, USHORT usTk2Len, BYTE* baTk3Buf, USHORT usTk3Len) //Invalid card reading fix -- jzg
{
    short shRet = d_OK;

    vdDebug_LogPrintf("=====shCTOS_SetMagstripCardTrackData=====");

    memcpy(srTransRec.szTrack1Data, &baTk1Buf[1], (usTk1Len - 3));
    srTransRec.usTrack1Len = (usTk1Len - 3); // REMOVE %, ? AND LRC

    /*Check for MPU-UPI specail card.*/
    if (baTk2Buf[usTk2Len - 1] == 0x3F) {
        vdDebug_LogPrintf("MPU Special Card 0x3F");
        memcpy(srTransRec.szTrack2Data, &baTk2Buf[1], (usTk2Len - 2));
        srTransRec.usTrack2Len = (usTk2Len - 2); // REMOVE %, ? AND LRC
    } else {
        memcpy(srTransRec.szTrack2Data, &baTk2Buf[1], (usTk2Len - 3));
        srTransRec.usTrack2Len = (usTk2Len - 3); // REMOVE %, ? AND LRC
    }

    vdDebug_LogPrintf("srTransRec.szTrack2Data [%s]", srTransRec.szTrack2Data);

    memcpy(srTransRec.szTrack3Data, baTk3Buf, usTk3Len);
    srTransRec.usTrack3Len = usTk3Len;

    vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);

    //Fix for card without Track 1
    //vdGetCardHolderName(baTk1Buf, usTk1Len);
    if (usTk1Len > 0)
        vdAnalysisTrack1(&baTk1Buf[1], usTk1Len);
    if (usTk2Len > 0)
        //Fix for card without Track 2
        shRet = shAnalysisTrack2(&baTk2Buf[1], usTk2Len); //Invalid card reading fix -- jzg
    else {
        vdDebug_LogPrintf("JEFF::T2 INVALID! [%d]", usTk2Len);
        return INVALID_CARD;
    }
    vdDebug_LogPrintf("srTransRec.szExpireDate [%02X][%02X]",
            srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);

    return shRet;
}

void vdCTOS_ResetMagstripCardData(void) {
    memset(srTransRec.szTrack1Data, 0x00, sizeof (srTransRec.szTrack1Data));
    srTransRec.usTrack1Len = 0;

    memset(srTransRec.szTrack2Data, 0x00, sizeof (srTransRec.szTrack2Data));
    srTransRec.usTrack2Len = 0;

    memset(srTransRec.szTrack3Data, 0x00, sizeof (srTransRec.szTrack3Data));
    srTransRec.usTrack3Len = 0;

    memset(srTransRec.szCardholderName, 0x00, sizeof (srTransRec.szCardholderName));

    memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
    srTransRec.byPanLen = 0;
    memset(srTransRec.szExpireDate, 0x00, sizeof (srTransRec.szExpireDate));
    memset(srTransRec.szServiceCode, 0x00, sizeof (srTransRec.szServiceCode));

    srTransRec.byEntryMode = 0;
}

int inCTOS_CheckEMVFallbackTimeAllow(char* szStartTime, char* szEndTime, int inAllowTime) {
    char szTempBuf[20];
    int inHH1, inHH2, inMM1, inMM2, inSS1, inSS2, inGap;

    if ((strlen(szStartTime) == 0) || (strlen(szStartTime) == 0))
        return (d_OK);

    if (0 == atoi(szStartTime))
        return (d_OK);

    memset(szTempBuf, 0, sizeof (szTempBuf));
    memcpy(szTempBuf, &szStartTime[0], 2);
    inHH1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof (szTempBuf));
    memcpy(szTempBuf, &szStartTime[2], 2);
    inMM1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof (szTempBuf));
    memcpy(szTempBuf, &szStartTime[4], 2);
    inSS1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof (szTempBuf));
    memcpy(szTempBuf, &szEndTime[0], 2);
    inHH2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof (szTempBuf));
    memcpy(szTempBuf, &szEndTime[2], 2);
    inMM2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof (szTempBuf));
    memcpy(szTempBuf, &szEndTime[4], 2);
    inSS2 = atoi(szTempBuf);

    inGap = ((inHH2 * 3600)+(inMM2 * 60) + inSS2) - ((inHH1 * 3600)+(inMM1 * 60) + inSS1);

    if (inGap < 0)
        return (d_OK);

    if (inGap > inAllowTime)
        return (d_NO);

    return d_OK;
    ;

}

int inCTOS_CheckEMVFallback(void) {
    BYTE szFallbackStartTime[20];
    BYTE szCurrentTime[20];
    int inRet;
    CTOS_RTC SetRTC;

    vdDebug_LogPrintf("inCTOS_CheckEMVFallback:fChkServiceCode[%d]byEntryMode[%d]inFallbackToMSR[%d]", strCDT.fChkServiceCode, srTransRec.byEntryMode, inFallbackToMSR);

	
    if (strCDT.fChkServiceCode) {
        if (((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
                && (CARD_ENTRY_ICC != srTransRec.byEntryMode)) {
			if (inFallbackToMSR == SUCCESS){
                CTOS_RTCGet(&SetRTC);
                sprintf(szCurrentTime, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
                inRet = inCTOS_CheckEMVFallbackTimeAllow(strTCT.szFallbackTime, szCurrentTime, strTCT.inFallbackTimeGap);

                vdDebug_LogPrintf("inCTOS_CheckEMVFallbackTimeAllow:szCurrentTime[%s]byEntryMode[%s][%ld]", szCurrentTime, strTCT.szFallbackTime, strTCT.inFallbackTimeGap);

                inFallbackToMSR = FAIL;
                memset(strTCT.szFallbackTime, 0x00, sizeof (strTCT.szFallbackTime));

                if (d_OK != inRet)
                    return FAIL;
                else{
	                    vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);					
	                    //vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                }
            } else{
				vdDebug_LogPrintf("inCTOS_CheckEMVFallback::return FAIL");            
                return FAIL;
            }
        }
    }

    return d_OK;
    
}

int inCTOS_CheckIssuerEnable(void) {
    int inEnable = 0;

    inEnable = strIIT.inCheckHost;
    vdDebug_LogPrintf("inCTOS_CheckIssuerEnable: [%d]", inEnable);

    if (0 == inEnable) {
        vdSetErrorMessage("TRANS NOT ALLWD,ISSUER");
        return (ST_ERROR);
    } else
        return (ST_SUCCESS);
}

int inCTOS_CheckTranAllowd(void) {
    int inEnable = 0;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    inEnable = strPIT.fTxnEnable;
    vdDebug_LogPrintf("inCTOS_CheckTranAllowd: [%d]", inEnable);

    if (0 == inEnable) {
        vdSetErrorMessage("TRANS NOT ALLWD,PIT");
        return (ST_ERROR);
    } else {
        if (srTransRec.byTransType == PURCHASE_CASHBACK) {
            if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) && (srTransRec.IITid == 7)) //TODO: 7 is assumed to be SPAN
                return (ST_SUCCESS);
            else
                return (ST_ERROR);
        } else if (srTransRec.byTransType == PURCHASE_ADVICE || srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == CASH_ADVANCE) {
            //			if (srTransRec.IITid == 1 || srTransRec.IITid == 2 || srTransRec.IITid == 3)	//Visa, Master, Amex
            if (srTransRec.IITid == 1 ||
                    srTransRec.IITid == 2 ||
                    srTransRec.IITid == 3 ||
                    srTransRec.IITid == 4 ||
                    srTransRec.IITid == 6 ||
                    srTransRec.IITid == 7 ||
                    srTransRec.IITid == 8 ||
                    srTransRec.IITid == 9) //Visa, Master, Amex //@@IBR ADD MODIFY 20161025
                return (ST_SUCCESS);
            else
                return (ST_ERROR);
        }

        return (ST_SUCCESS);
    }
}

int inCTOS_CheckMustSettle(void) {
    int inEnable = 0;

    inEnable = strMMT[0].fMustSettFlag;
    vdDebug_LogPrintf("1111111111111111111111 inCTOS_CheckMustSettle strMMT[0].HDTid [%d]", strMMT[0].HDTid); //EMMY Sept2018
    if (1 == inEnable) {
        vdDebug_LogPrintf("1111111111111111111111 inCTOS_CheckMustSettle inEnable[%d]", inEnable); //EMMY Sept2018
        vdSetErrorMessage("MUST SETTLE");
        return (ST_ERROR);
    } else
        return (ST_SUCCESS);
}


void vdCTOS_FormatPAN(char *szFmt, char* szInPAN, char* szOutPAN) {
    char szCurrentPAN[20];
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int inFmtPANSize;

	vdDebug_LogPrintf("vdCTOS_FormatPAN");

    inFmtPANSize = strlen(szFmt);
    if (strlen(szFmt) == 0) {
        strncpy(szOutPAN, szInPAN, inFmtPANSize);
        return;
    }

	vdDebug_LogPrintf("vdCTOS_FormatPAN-1");

    memset(szCurrentPAN, 0x00, sizeof (szCurrentPAN));
    memcpy(szCurrentPAN, szInPAN, strlen(szInPAN));

    while (szFmt[inFmtIdx] != 0x00) {
        if (szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n') {
            szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
            inFmtIdx++;
            inPANIdx++;
        } else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' || szFmt[inFmtIdx] == '*') {

            memcpy(&szOutPAN[inFmtIdx], &szFmt[inFmtIdx], 1);
            inFmtIdx++;
            inPANIdx++;
        } else if (!isdigit(szFmt[inFmtIdx])) {
            szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
            inFmtIdx++;
        }
    }

    while (szCurrentPAN[inPANIdx] != 0x00) {
        szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
        inFmtIdx++;
        inPANIdx++;
    }

	vdDebug_LogPrintf("vdCTOS_FormatPAN-2");

    return;
}

//start base on issuer IIT and mask card pan and expire data

int ntocs(char *dest_buf, char *src_buf) {
    int i;

    // DCS #1045 : Included following 2 lines
    if ((dest_buf == NULL) || (src_buf == NULL))
        return -1;

    /* get length of current string add one for count */
    i = strlen(src_buf) + 1;

    /* max counted string length is 255 */
    if ((i > 255) || (i == 0))
        dest_buf[0] = 0x00;
    else {
        strcpy(dest_buf + 1, src_buf);
        dest_buf[0] = i;
    }

    /* return the count length */
    return (dest_buf [0]);

}

int str2int(char *buffer) {
    int sign = 0;
    int num;

    /* incoming string must not be longer than 40 characters */
    char temp[42];

    // DCS #1045 : Included following 2 lines
    if (buffer == NULL)
        return -1;

    if (strlen(buffer) > 40)
        return (0);

    if (*buffer == '-')
        sign = '-';

    /* SVC_2INT requires counted strings */
    ntocs(temp, buffer);

    //num = SVC_2INT (temp);
    num = atol(temp);

    return (sign ? -num : num);

}

int chars2int(char * s_buffer, int i_num) {
    char temp[6];
    i_num = MIN(i_num, 5);

    if (i_num < 0) {
        return 0;
    }
    temp[i_num] = 0x00;
    memcpy(temp, s_buffer, i_num);
    return (str2int(temp));
}

int append_char(char *string, char c) {
    int i;

    /* get the current length of the string, this is the 
        position for the new character.
     */
    i = strlen(string);

    /* now place the passed character at the end of the 
        string.  since the length is the number of characters
        in the string, then the pointer plus the number of characters
        is the current NULL position.  The user may pass an empty
        string.  In this case, the apended character will be in the 
        first position.
     */
    *(string + i++) = c;
    /* Now add a NULL after the newly appended character. */

    if (c != 0)
        *(string + i) = 0x00;

    /* i is the position of the NULL which is also the new string
length.  Return i.
     */
    return (strlen(string));
}

int pad(char *pdest_buf, char *psrc_buf, char pad_char, int pad_size,
        int align) {
    int ch_left, ch_right;
    int num_pad;
    char *d_ptr, *s_ptr;

    /* pad _size cannot be negative */

    if (pad_size < 0)
        pad_size = 0;

    /* determine how many characters to add */
    /* ensure we need to add characters */

    if (0 > (num_pad = pad_size - (int) strlen(psrc_buf)))
        num_pad = 0;


    /* the source and destination buffer may be the same
     *  buffer.  if they are different, copy the source
     *  to the destination and do not molest the source
     *
     *	02/18/92  jwm
     */

    if (psrc_buf != pdest_buf) {
        /* initialized destination and copy source 
         *  2/18/92 jwm
         */
        memcpy(pdest_buf, psrc_buf, strlen(psrc_buf) + 1);
    }

    /* determine the number of characters to pad on */
    /* each end.                                    */

    switch (align) {

        case RIGHT:
        {
            ch_left = num_pad;
            ch_right = 0;
            break;
        }

        case CENTER:
        {
            ch_left = num_pad / 2;
            ch_right = num_pad - ch_left;
            break;
        }

        case LEFT:
        default:
        {
            ch_left = 0;
            ch_right = num_pad;
            break;
        }
    }
    /* pad the front of the string */

    if (ch_left) {
        s_ptr = psrc_buf + strlen(psrc_buf);
        d_ptr = pdest_buf + strlen(psrc_buf) + ch_left;
        while (psrc_buf <= s_ptr) /* copy string to destination */
            *d_ptr-- = *s_ptr--;
        while (ch_left--) /* add pad characters before string */
            *d_ptr-- = pad_char;
    }

    /* pad the end of the string */

    while (ch_right--) {
        append_char(pdest_buf, pad_char);
    }

    return (num_pad);
}

void vdCTOSS_PrintFormatTIDMID(char* pchPAN, char* pchFmtPAN, int inFmtPANSize, int page) {
    char szFmt[FORMATTED_PAN_SIZE + 1];
    char szScratch[FORMATTED_PAN_SIZE + 1];
    char szCurrentPAN[FORMATTED_PAN_SIZE + 1];
	char szTempPAN[FORMATTED_PAN_SIZE + 1];
    char chLastPad = ' ';
    VS_BOOL fReverseMode = VS_FALSE;
    VS_BOOL fMerchFormat = VS_FALSE;
    int inBytesLeft;
    int inLen;
    int inLastDigits;
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int i = 0;
    
    /* Remove 1 from count for NULL terminator, assume caller uses sizeof */
    //inFmtPANSize--;

    vdDebug_LogPrintf("vdCTOSS_PrintFormatPAN inFmtPANSize:[%d] strIIT.inIssuerNumber:[%d] byTransType:[%d] page:[%d]", inFmtPANSize, strIIT.inIssuerNumber, srTransRec.byTransType, page);


    memset(szFmt, 0x00, sizeof (szFmt));

    if (page == d_FIRST_PAGE) 
		strcpy(szFmt, strIIT.szMaskTID);
	else if (page == d_SECOND_PAGE) 
        strcpy(szFmt, strIIT.szMaskMID);
    else
        strcpy(szFmt, strIIT.szPANFormat);

	inLen = strlen(pchPAN);

    if (strIIT.inIssuerNumber == 6 && inLen > 16) {
		#if 1
        //memset(szFmt, 0x00, sizeof (szFmt));
        //strcpy(szFmt, "************NNNN");
        //strcpy(szFmt, "NNNN NN** **** NNNN");
		//#else
		memset(szTempPAN, 0x30, inLen);
		memcpy(szTempPAN, pchPAN, 6);
		memcpy(szTempPAN+12, pchPAN+(inLen-4), 4);

		memcpy(pchPAN, szTempPAN, 16);
		pchPAN[16]=0;
		#endif
    }


    if (inFmtPANSize == EXP_DATE_SIZE) {
        strcpy(szFmt, strIIT.szMaskExpireDate);

        if (strIIT.inIssuerNumber == 6 && srTransRec.byTransType != MPU_PREAUTH) {
            memset(szFmt, 0x00, sizeof (szFmt));
            strcpy(szFmt, "****");
        }

#if 1 //for redmine case #2071 sub case 2 (unmasking expire card date for only preauth transaction receipts.
	 // http://118.201.48.210:8080/redmine/issues/1525.34
		//masked only merchant copy.
		if ((srTransRec.byTransType == PRE_AUTH) && (page == d_FIRST_PAGE)) {
				memset(szFmt, 0x00, sizeof (szFmt));
				strcpy(szFmt, "NNNN");
			}
#endif


    }



    vdDebug_LogPrintf("szFmt = [%d]  szFmt = [%s]  pchPAN = [%s]", inFmtPANSize, szFmt, pchPAN);

    fMerchFormat = strIIT.fMerchPANFormat;


    /* First check on reverse mode, only indicated by first byte */
    memset(szCurrentPAN, 0x00, sizeof (szCurrentPAN));
    if (szFmt[inFmtIdx] == 'R' || szFmt[inFmtIdx] == 'r') {
        fReverseMode = VS_TRUE;
        inFmtIdx++; /* Move past the 'r' */
        inBytesLeft = inLen = strlen(pchPAN);
        inBytesLeft--; /* array index starts at 0 */
        for (i = 0; i < inLen && i < FORMATTED_PAN_SIZE; i++, inBytesLeft--) {
            /* Copy the PAN backwards */
            szCurrentPAN[i] = *(pchPAN + inBytesLeft);
        }
    }/* end if reverse mode */
    else
        strncpy(szCurrentPAN, pchPAN, FORMATTED_PAN_SIZE); /* end else normal mode */

    //LOG_PRINTFF((0X08L, "szCurrentPAN = [%s]",szCurrentPAN));

    /* While rcpt.acctnum not full && ! end of PAN  && ! end of Format string */
    while (szFmt[inFmtIdx] != NULL_CH && szCurrentPAN[inPANIdx] != NULL_CH && strlen(pchFmtPAN) < inFmtPANSize) {
        if (szFmt[inFmtIdx] == 'n' || szFmt[inFmtIdx] == 'N') {
            /* They want this one digit in full view */
            append_char(pchFmtPAN, szCurrentPAN[inPANIdx]);
            inFmtIdx++;
            inPANIdx++;
            if (szFmt[inFmtIdx] == NULL_CH)
                inFmtIdx--;
        }/* end if n or N, print PAN digit */
        else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' || szFmt[inFmtIdx] == '*') {
            /* They want one pad character */
            append_char(pchFmtPAN, szFmt[inFmtIdx]);
            chLastPad = szFmt[inFmtIdx];
            inFmtIdx++;
            inPANIdx++;
            if (szFmt[inFmtIdx] == NULL_CH) {
                /* No more formatting so print the rest w/ pads */
                inBytesLeft = strlen(&szCurrentPAN[inPANIdx]);
                memset(szScratch, 0x00, sizeof (szScratch));
                pad(szScratch, szScratch, chLastPad, inBytesLeft, RIGHT);
                strcat(pchFmtPAN, szScratch);
            }
        }/* end if pad character (x,X or * ) */
        else if (!isdigit(szFmt[inFmtIdx])) {
            /* They want a format separator */
            append_char(pchFmtPAN, szFmt[inFmtIdx]);
            inFmtIdx++;
        }/* end if not 0-9 */
        else if (szFmt[inFmtIdx] == '0') {
            /* Zero is not a valid value */
            inFmtIdx++;
        }/* end if zero */
        else {
            /* We must have 1-9 in szFmt[inFmtIdx] */
            inBytesLeft = strlen(&szCurrentPAN[inPANIdx]);
            inLastDigits = chars2int(&szFmt[inFmtIdx], 1);

            /* if we are reverse mode the last digits are in the front.
             if we are not in reverse then if what we have left in the PAN
             is less than or equal to the LastDigit PAN Format indicator
             if either is true then print a PAN digit in the open */
            if ((fReverseMode && inPANIdx < inLastDigits) || (!fReverseMode && inBytesLeft <= inLastDigits)) {
                append_char(pchFmtPAN, szCurrentPAN[inPANIdx]);
                inFmtIdx++;
                inPANIdx++;
                if (fReverseMode && (inPANIdx < inLastDigits || szFmt[inFmtIdx] == NULL_CH)) {
                    /* Still have more PAN digits to print */
                    inFmtIdx--;
                }
                if (szFmt[inFmtIdx] == NULL_CH) {
                    /* No more formatting so print the rest of the PAN */
                    strcat(pchFmtPAN, &szCurrentPAN[inPANIdx]);
                }
            }/* if last x digits then print the PAN in the open */
            else {
                /* If we are not at the end of PAN then print the last pad char */
                if (chLastPad != ' ')
                    append_char(pchFmtPAN, chLastPad);
                inFmtIdx++;
                inPANIdx++;
                /* if that was our last format character then use it again */
                if (szFmt[inFmtIdx] == NULL_CH)
                    inFmtIdx--;
            } /* end if PAN remaining greater than last digit indicator */
        } /* end else, must be 1-9 */
    } /* end while print buffer not full, not end of Format string or PAN */

    if (fReverseMode) {
        memset(szScratch, 0x00, sizeof (szScratch));
        inBytesLeft = inLen = strlen(pchFmtPAN);
        inBytesLeft--; /* array index starts at 0 */
        for (i = 0; i < inLen && i < FORMATTED_PAN_SIZE; i++, --inBytesLeft) {
            /* Copy the Formatted PAN back so that it is forward */
            szScratch[i] = *(pchFmtPAN + inBytesLeft);
        }
        strncpy(pchFmtPAN, szScratch, FORMATTED_PAN_SIZE);
    } /* end if reverse mode */

    vdDebug_LogPrintf("pchFmtPAN = [%s]", pchFmtPAN);

    return;
} /* end func vdFormatPAN() */

void vdCTOSS_PrintFormatPAN(char* pchPAN, char* pchFmtPAN, int inFmtPANSize, int page) {
    char szFmt[FORMATTED_PAN_SIZE + 1];
    char szScratch[FORMATTED_PAN_SIZE + 1];
    char szCurrentPAN[FORMATTED_PAN_SIZE + 1];
	char szTempPAN[FORMATTED_PAN_SIZE + 1];
    char chLastPad = ' ';
    VS_BOOL fReverseMode = VS_FALSE;
    VS_BOOL fMerchFormat = VS_FALSE;
    int inBytesLeft;
    int inLen;
    int inLastDigits;
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int i = 0;
    
    /* Remove 1 from count for NULL terminator, assume caller uses sizeof */
    inFmtPANSize--;

    vdDebug_LogPrintf("vdCTOSS_PrintFormatPAN strIIT.inIssuerNumber:[%d] byTransType:[%d] page:[%d]", strIIT.inIssuerNumber, srTransRec.byTransType, page);


    memset(szFmt, 0x00, sizeof (szFmt));

    if (page == d_FIRST_PAGE) {
		#if 1 // for redmine case @2071 Unmasking PreAuth recipt of merchant copy for Hotel Merchant
			if (srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == MPU_PREAUTH)
				strcpy(szFmt, strIIT.szMaskDisplay);			
			else
				strcpy(szFmt, strIIT.szMaskMerchantCopy);
		#else
		        strcpy(szFmt, strIIT.szMaskMerchantCopy);
		#endif
		

    } else if (page == d_SECOND_PAGE) {
        strcpy(szFmt, strIIT.szMaskCustomerCopy);
    } else
        strcpy(szFmt, strIIT.szPANFormat);

	inLen = strlen(pchPAN);

    if (strIIT.inIssuerNumber == 6 && inLen > 16) {
		#if 1
        //memset(szFmt, 0x00, sizeof (szFmt));
        //strcpy(szFmt, "************NNNN");
        //strcpy(szFmt, "NNNN NN** **** NNNN");
		//#else
		memset(szTempPAN, 0x30, inLen);
		memcpy(szTempPAN, pchPAN, 6);
		memcpy(szTempPAN+12, pchPAN+(inLen-4), 4);

		memcpy(pchPAN, szTempPAN, 16);
		pchPAN[16]=0;
		#endif
    }

    //if (srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == MPU_PREAUTH || srTransRec.byTransType == MPU_PREAUTH_COMP)
#if 0
	if (srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == MPU_PREAUTH)
        strcpy(szFmt, "NNNNNNNNNNNNNNNN");
#endif

    if (inFmtPANSize == EXP_DATE_SIZE) {
        strcpy(szFmt, strIIT.szMaskExpireDate);

        if (strIIT.inIssuerNumber == 6 && srTransRec.byTransType != MPU_PREAUTH) {
            memset(szFmt, 0x00, sizeof (szFmt));
            strcpy(szFmt, "****");
        }
#if 0 /*need to mask*/
        if (srTransRec.byTransType == PRE_AUTH) {
            memset(szFmt, 0x00, sizeof (szFmt));
            strcpy(szFmt, "NNNN");
        }
#endif

#if 1 //for redmine case #2071 sub case 2 (unmasking expire card date for only preauth transaction receipts.
	 // http://118.201.48.210:8080/redmine/issues/1525.34
		//masked only merchant copy.
		if ((srTransRec.byTransType == PRE_AUTH) && (page == d_FIRST_PAGE)) {
				memset(szFmt, 0x00, sizeof (szFmt));
				strcpy(szFmt, "NNNN");
			}
#endif


    }



    vdDebug_LogPrintf("szFmt = [%d] [%s] [%s]", inFmtPANSize, szFmt, pchPAN);

    fMerchFormat = strIIT.fMerchPANFormat;


    /* First check on reverse mode, only indicated by first byte */
    memset(szCurrentPAN, 0x00, sizeof (szCurrentPAN));
    if (szFmt[inFmtIdx] == 'R' || szFmt[inFmtIdx] == 'r') {
        fReverseMode = VS_TRUE;
        inFmtIdx++; /* Move past the 'r' */
        inBytesLeft = inLen = strlen(pchPAN);
        inBytesLeft--; /* array index starts at 0 */
        for (i = 0; i < inLen && i < FORMATTED_PAN_SIZE; i++, inBytesLeft--) {
            /* Copy the PAN backwards */
            szCurrentPAN[i] = *(pchPAN + inBytesLeft);
        }
    }/* end if reverse mode */
    else
        strncpy(szCurrentPAN, pchPAN, FORMATTED_PAN_SIZE); /* end else normal mode */

    //LOG_PRINTFF((0X08L, "szCurrentPAN = [%s]",szCurrentPAN));

    /* While rcpt.acctnum not full && ! end of PAN  && ! end of Format string */
    while (szFmt[inFmtIdx] != NULL_CH && szCurrentPAN[inPANIdx] != NULL_CH && strlen(pchFmtPAN) < inFmtPANSize) {
        if (szFmt[inFmtIdx] == 'n' || szFmt[inFmtIdx] == 'N') {
            /* They want this one digit in full view */
            append_char(pchFmtPAN, szCurrentPAN[inPANIdx]);
            inFmtIdx++;
            inPANIdx++;
            if (szFmt[inFmtIdx] == NULL_CH)
                inFmtIdx--;
        }/* end if n or N, print PAN digit */
        else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' || szFmt[inFmtIdx] == '*') {
            /* They want one pad character */
            append_char(pchFmtPAN, szFmt[inFmtIdx]);
            chLastPad = szFmt[inFmtIdx];
            inFmtIdx++;
            inPANIdx++;
            if (szFmt[inFmtIdx] == NULL_CH) {
                /* No more formatting so print the rest w/ pads */
                inBytesLeft = strlen(&szCurrentPAN[inPANIdx]);
                memset(szScratch, 0x00, sizeof (szScratch));
                pad(szScratch, szScratch, chLastPad, inBytesLeft, RIGHT);
                strcat(pchFmtPAN, szScratch);
            }
        }/* end if pad character (x,X or * ) */
        else if (!isdigit(szFmt[inFmtIdx])) {
            /* They want a format separator */
            append_char(pchFmtPAN, szFmt[inFmtIdx]);
            inFmtIdx++;
        }/* end if not 0-9 */
        else if (szFmt[inFmtIdx] == '0') {
            /* Zero is not a valid value */
            inFmtIdx++;
        }/* end if zero */
        else {
            /* We must have 1-9 in szFmt[inFmtIdx] */
            inBytesLeft = strlen(&szCurrentPAN[inPANIdx]);
            inLastDigits = chars2int(&szFmt[inFmtIdx], 1);

            /* if we are reverse mode the last digits are in the front.
             if we are not in reverse then if what we have left in the PAN
             is less than or equal to the LastDigit PAN Format indicator
             if either is true then print a PAN digit in the open */
            if ((fReverseMode && inPANIdx < inLastDigits) || (!fReverseMode && inBytesLeft <= inLastDigits)) {
                append_char(pchFmtPAN, szCurrentPAN[inPANIdx]);
                inFmtIdx++;
                inPANIdx++;
                if (fReverseMode && (inPANIdx < inLastDigits || szFmt[inFmtIdx] == NULL_CH)) {
                    /* Still have more PAN digits to print */
                    inFmtIdx--;
                }
                if (szFmt[inFmtIdx] == NULL_CH) {
                    /* No more formatting so print the rest of the PAN */
                    strcat(pchFmtPAN, &szCurrentPAN[inPANIdx]);
                }
            }/* if last x digits then print the PAN in the open */
            else {
                /* If we are not at the end of PAN then print the last pad char */
                if (chLastPad != ' ')
                    append_char(pchFmtPAN, chLastPad);
                inFmtIdx++;
                inPANIdx++;
                /* if that was our last format character then use it again */
                if (szFmt[inFmtIdx] == NULL_CH)
                    inFmtIdx--;
            } /* end if PAN remaining greater than last digit indicator */
        } /* end else, must be 1-9 */
    } /* end while print buffer not full, not end of Format string or PAN */

    if (fReverseMode) {
        memset(szScratch, 0x00, sizeof (szScratch));
        inBytesLeft = inLen = strlen(pchFmtPAN);
        inBytesLeft--; /* array index starts at 0 */
        for (i = 0; i < inLen && i < FORMATTED_PAN_SIZE; i++, --inBytesLeft) {
            /* Copy the Formatted PAN back so that it is forward */
            szScratch[i] = *(pchFmtPAN + inBytesLeft);
        }
        strncpy(pchFmtPAN, szScratch, FORMATTED_PAN_SIZE);
    } /* end if reverse mode */

    vdDebug_LogPrintf("pchFmtPAN = [%s]", pchFmtPAN);

    return;
} /* end func vdFormatPAN() */
//end base on issuer IIT and mask card pan and expire data

int inGetIssuerRecord(int inIssuerNumber) {
    int inRec = 1;
    do {
        //if (inIITRead(inRec) != d_OK) //for improve transaction speed 
        if (inIITRead(inIssuerNumber) != d_OK) {
            return (d_NO);
        }
        inRec++;
    } while (inIssuerNumber != strIIT.inIssuerNumber);

    return (d_OK);
}

int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine) {
    char szStr[50 + 1];
    USHORT EMVtagLen;
    BYTE EMVtagVal[64];
    BYTE szTemp1[30 + 1];
	
	#ifdef MINOR_CHANGES
    char szTempPAN[d_LINE_SIZE + 1];
    int inLen=0;
    #endif
	
    memset(szStr, 0x00, sizeof (szStr));
    memset(EMVtagVal, 0x00, sizeof (EMVtagVal));

	vdDebug_LogPrintf("inCTOS_DisplayCardTitle byTransType[%d]inCurrencyIndex[%d]", srTransRec.byTransType, strCST.inCurrencyIndex);

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        EMVtagLen = 0;

        if (EMVtagLen > 0) {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s", szStr);
        }
    }

    if (0 == strlen(szStr)) {
        sprintf(szStr, "%s", strCDT.szCardLabel);
        vdDebug_LogPrintf("Card label: %s", strCDT.szCardLabel);
    }

    memset(szTemp1, 0, sizeof (szTemp1));
#ifdef MINOR_CHANGES
		inLen=strlen(srTransRec.szPAN);
		if(inLen > 16) {
	
			memset(szTempPAN, 0x30, inLen);
			memcpy(szTempPAN, srTransRec.szPAN, 6);
			memcpy(szTempPAN+12, srTransRec.szPAN+(inLen-4), 4);
			szTempPAN[16]=0;
		}
		vdCTOS_FormatPAN("NNNN NN** **** NNNN", (inLen > 16)?szTempPAN:srTransRec.szPAN, szTemp1);
#else	
#if 0	// orignal code
    if (srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == VOID) {
        //vdCTOS_FormatPAN("NNNNNNNNNNNNNNNN", srTransRec.szPAN, szTemp1);

        vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
    } else
        vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
#else // raised under issue #607
	if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH || srTransRec.byTransType == MPU_VOID_PREAUTH_COMP || srTransRec.byTransType == MPU_VOID_PREAUTH)		
	{
        strcpy(szTemp1, srTransRec.szPAN);
	} 
	else
		vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);

#endif
   
#endif

    if (0 >= usCardTypeLine || 0 >= usPANLine) {
        usCardTypeLine = 3;
        usPANLine = 4;
    }


    CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);
    CTOS_LCDTPrintXY(1, usPANLine, szTemp1);

    return (d_OK);
}

short inCTOS_LoadCDTIndex(void) {
    signed int inRetVal;
    short shStatus;
    int i = 0, selectedRDTIndex;
    BYTE shSuitableRDTIndex[10];
    int inIssuer = 0;
    int inNumberOfMatches = 0;
    int inRecNumArray[4];
    char szChoiceMsg[20 + 1];
    int inCardLen, j;
    int inMaxCDTid = 0;
    int inFindRecordNum = 0;



    memset(szChoiceMsg, 0x00, sizeof (szChoiceMsg));
    vdDebug_LogPrintf("inCTOS_LoadCDTIndex START");

	

    //CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    //if ((strTCT.byTerminalType%2) == 0)
    //	CTOS_LCDTPrintXY(1, V3_ERROR_LINE_ROW, "Checking Card... ");
    //else
    //	CTOS_LCDTPrintXY(1, 8, "Checking Card... ");

#if 0
    // TODO: SAMA requirement, Appendix E - service code X2X
    if ((strTCT.fDebitFlag == DEBIT) && (strcmp(srTransRec.szServiceCode, "X2X") == 0)) {
        inCDTReadMultiByType(srTransRec.szPAN, &inFindRecordNum, DEBIT_CARD);
    } else
        inCDTReadMulti(srTransRec.szPAN, &inFindRecordNum);
#endif

    //	if (VS_TRUE == fGetMPUTrans())
    //	{
    //		inCDTReadMultiByType(srTransRec.szPAN, &inFindRecordNum, DEBIT_CARD);
    //	}
    //	else
    //	{
    //		inCDTReadMultiByType(srTransRec.szPAN, &inFindRecordNum, CREDIT_CARD);
    //	}
    //strcpy(srTransRec.szPAN, "9505080000160807");
    //if (VS_TRUE == srTransRec.fIsInstallment)
    //inCDTReadMultiForIPP(srTransRec.szPAN, &inFindRecordNum);
    //else
    inCDTReadMulti(srTransRec.szPAN, &inFindRecordNum);

    //        if(strCDT.inType == 1){
    //            vdDebug_LogPrintf("MPU TRANS");
    //            vdSetMPUTrans(TRUE);
    //        } else {
    //            vdDebug_LogPrintf("NOT MPU TRANS");
    //            vdSetMPUTrans(FALSE);
    //        }

    if (inFindRecordNum == 0) {
        vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    vdDebug_LogPrintf("inFindRecordNum[%d]", inFindRecordNum);

    for (j = 0; j < inFindRecordNum; j++) {
        if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD))
            //		if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) && !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)) 
        {
            if ((strMCDT[j].inType == DEBIT_CARD) || (strMCDT[j].inType == EBT_CARD) || (strMCDT[j].IITid != inIssuer)) {
                if (strMCDT[j].inType != DEBIT_CARD && strMCDT[j].inType != EBT_CARD)
                    inIssuer = strMCDT[j].IITid;

                inRecNumArray[inNumberOfMatches++] = strMCDT[j].CDTid;
                if (inNumberOfMatches > 0)
                    break;

                if (inNumberOfMatches > 1)
                    szChoiceMsg[strlen(szChoiceMsg)] = '~';

                switch (strMCDT[j].inType) {
                    case DEBIT_CARD:
                        break;
                    case EBT_CARD:
                        break;
                    case PURCHASE_CARD:
                        break;
                    default:
                        break;
                }

                if (inNumberOfMatches > 3)
                    break;
            }
        }
    }

    vdDebug_LogPrintf("inNumberOfMatches[%d]", inNumberOfMatches);
    vdDebug_LogPrintf("inRecNumArray[0]=[%d]", inRecNumArray[0]);

    if (inNumberOfMatches == 1) {
        inRetVal = inRecNumArray[0];
    } else if (inNumberOfMatches > 1) {
        //CTOS_LCDTClearDisplay();
        //vduiClearBelow(2);
        vdDispTransTitle(srTransRec.byTransType);
    }

    vdDebug_LogPrintf("inRetVal[%d]", inRetVal);

    if (inRetVal >= 0) {
        inCDTRead(inRetVal);
        srTransRec.CDTid = inRetVal; // save the current CDT that is loaded

        /* Get the CDT also for card labels */
        inGetIssuerRecord(strCDT.IITid);
    } else {
        vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    vdDebug_LogPrintf("strCDT.inMinPANDigit[%d]", strCDT.inMinPANDigit);
    vdDebug_LogPrintf("strCDT.inMaxPANDigit[%d]", strCDT.inMaxPANDigit);

    /* Check for proper card length */
    inCardLen = strlen(srTransRec.szPAN);
    vdDebug_LogPrintf("inCardLen = %d", inCardLen);
    if ((inCardLen < strCDT.inMinPANDigit) ||
            (inCardLen > strCDT.inMaxPANDigit)) {
        vdDebug_LogPrintf("BAD CARD LEN");
        vdSetErrorMessage("BAD CARD LEN");
        return INVALID_CARD;
    }

    if (strCDT.fluhnCheck == VS_TRUE) /* Check Luhn */ {
        if (chk_luhn(srTransRec.szPAN)) {
			
			vdDebug_LogPrintf("Postrans.c INVALID LUHN");
            vdDisplayErrorMsg(1, 8, "INVALID LUHN");
            return INVALID_CARD;
        }
    }

    if (strCDT.fManEntry == FALSE && srTransRec.byEntryMode == CARD_ENTRY_MANUAL) {
        vduiClearBelow(2);
        setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO MANUAL ENTRY");
        vduiWarningSound();
        CTOS_Delay(1500);
        return INVALID_CARD;
    }

    //    if(!(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL))
    //        return INVALID_CARD;


    if (strTCT.fDebitFlag != DEBIT && strCDT.inType == DEBIT_CARD) {
        vdDisplayErrorMsg(1, 8, "INVALID CARD");
        return INVALID_CARD;
    }

    if (strCDT.fExpDtReqd) {
        if (shChk_ExpireDate() != d_OK) {
            vdDisplayErrorMsg(1, 8, "CARD EXPIRED");
            return CARD_EXPIRED;
        }
    }

    //for save Accum file
    srTransRec.IITid = strCDT.IITid;
    srTransRec.HDTid = strCDT.HDTid;
    srTransRec.inCardType = strCDT.inType;

    vdDebug_LogPrintf("srTransRec.inCardType[%d] srTransRec.CDTid[%d]", srTransRec.inCardType, srTransRec.CDTid);

    if (srTransRec.inCardType == 1) {
        vdSetMPUTrans(TRUE);
    } else {
        vdSetMPUTrans(FALSE);
    }

#ifdef MPUPIOnUsFlag
	vdDebug_LogPrintf("inCTOS_LoadCDTIndex:byTransType:[%d]HDTid:[%d]byEntryMode:[%d]fMagSwipeEnable:[%d]IITid:[%d]inFallbackToMSR:[%d]fPreAuthAllowed:[%d]fRefundAllowed[%d]fCTLSAllowed[%d]fAddRefRRNAPP[%d]fGetMPUPIOnUs[%d]", 
		srTransRec.byTransType, srTransRec.HDTid, srTransRec.byEntryMode, strCDT.fMagSwipeEnable, srTransRec.IITid, inFallbackToMSR, strCDT.fPreAuthAllowed, strCDT.fRefundAllowed, strCDT.fCTLSAllowed, strCDT.fAddRefRRNAPP, fGetMPUPIOnUs());
#else
vdDebug_LogPrintf("inCTOS_LoadCDTIndex:byTransType:[%d]HDTid:[%d]byEntryMode:[%d]fMagSwipeEnable:[%d]IITid:[%d]inFallbackToMSR:[%d]fPreAuthAllowed:[%d]fRefundAllowed[%d]fCTLSAllowed[%d]fAddRefRRNAPP[%d]", 
	srTransRec.byTransType, srTransRec.HDTid, srTransRec.byEntryMode, strCDT.fMagSwipeEnable, srTransRec.IITid, inFallbackToMSR, strCDT.fPreAuthAllowed, strCDT.fRefundAllowed, strCDT.fCTLSAllowed, strCDT.fAddRefRRNAPP);
#endif

	
//#ifdef MPUPIOnUsFlag
  	 if(srTransRec.CDTid == 58 || srTransRec.CDTid == 62 || srTransRec.CDTid == 63 || srTransRec.CDTid == 64 || srTransRec.CDTid == 71){  // ADDRESSED NOTE #4
		 vdDebug_LogPrintf("inCTOS_LoadCDTIndex CDTid == 58 || CDTid == 62 || CDTid == 63 || CDTid == 64 || CDTid == 71");
		 //vdSetMPUPIOnUs(TRUE);
  	 }
//#endif

   // if (srTransRec.IITid == 7 || srTransRec.IITid == 8 || srTransRec.IITid == 9)
	//{
        vdSetMPUCard(TRUE);
		#ifdef CB_MPU_NH_MIGRATION
		//enhancement #9 Mgtstripe Sales Tranx will not be allowed for Pure MPU,Pure UPI, MPU-JCB Co-brand, MPU-UPI Co-Brand.
		//   But FALLBACK TO MSR will be allowed.		
		//if(inFallbackToMSR == SUCCESS){
		//	return d_OK;
		//}

		// enhancement for scenario #1 of redmine case #1583 - "Need to check MPU bin to disable magstripe')		
		if(srTransRec.byEntryMode == CARD_ENTRY_MSR && strCDT.fMagSwipeEnable == FALSE){
			vduiClearBelow(2);
			setLCDPrint(8, DISPLAY_POSITION_LEFT, "SWIPE NOT ALLOWED");
			
			vdDebug_LogPrintf("inCTOS_LoadCDTIndex HERE (srTransRec.byEntryMode == CARD_ENTRY_MSR && strCDT.fMagSwipeEnable == FALSE)");
			vduiWarningSound();
			CTOS_Delay(1500);
			//return PLS_INSERT_CARD;
			return MSR_NOT_ALLOW;
		}		
		#endif

		//for Redmine case http://118.201.48.210:8080/redmine/issues/1525.8
		#ifdef MPU_PREAUTH_ENABLE
		if((strCDT.fPreAuthAllowed == FALSE) && (srTransRec.byTransType == MPU_PREAUTH || srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == CUP_PRE_AUTH))
		{
			vduiClearBelow(2);
			vdDebug_LogPrintf("inCTOS_LoadCDTIndex HERE (strCDT.fPreAuthAllowed == FALSE");
			
			setLCDPrint(8, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
			vduiWarningSound();
			CTOS_Delay(1500);
			//return PLS_INSERT_CARD;
			return MSR_NOT_ALLOW;
		}		
		#endif

		#ifdef CBB_FIN_ROUTING
		vdDebug_LogPrintf("inCTOS_LoadCDTIndex HERE CBB_FIN_ROUTING");

		//Request #2 - 2) To add fRefund flag for each BIN
		if(srTransRec.byTransType == REFUND)
		{
			if(strCDT.fRefundAllowed == FALSE && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
			{
				vduiClearBelow(2);
				setLCDPrint(8, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
				vduiWarningSound();
				CTOS_Delay(1500);
				return MSR_NOT_ALLOW;
			}
		}
		
		vdDebug_LogPrintf("inCTOS_LoadCDTIndex HERE2");

        // request #8 - 8) Create a flag which can allow/deny ctls trnx for MPU-UPI OffUs cards.		//MPU-UPI
		if(strCDT.fCTLSAllowed == FALSE && srTransRec.byEntryMode == CARD_ENTRY_WAVE && srTransRec.IITid == 9)
		{
			vduiClearBelow(2);
			setLCDPrint(8, DISPLAY_POSITION_LEFT, "TRANS NOT ALLOWED");
			vduiWarningSound();
			CTOS_Delay(1500);
			return MSR_NOT_ALLOW;
		}				
		#endif
		
   // }

    //CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    //    CTOS_LCDTPrintXY(1, 8, "                   ");  
    
    vdDebug_LogPrintf("inCTOS_LoadCDTIndex END");
    return d_OK;
}

/*
short inCTOS_LoadCDTIndex(void)
{
        signed int inRetVal;
        short   shStatus;
        int     inIndex = 1,i=0, selectedRDTIndex;
        BYTE    shSuitableRDTIndex[10];
        int inIssuer = 0;
        int inNumberOfMatches = 0;
        int inRecNumArray[4];
        char szChoiceMsg[20 + 1];
        int    inCardLen;
        int inMaxCDTid = 0;

    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
  
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 8, "Checking Card... ");

    inMaxCDTid = inCDTMAX();
        if (inMaxCDTid <= 0)
                inMaxCDTid = 99;

    vdDebug_LogPrintf("inCDTMAX [%ld]", inMaxCDTid);
    
    do
    {
        if (inMaxCDTid <inIndex)
        {
            vdDebug_LogPrintf("inCDTMAX [%ld] inIndex[%d]", inMaxCDTid, inIndex);
                vdSetErrorMessage("CARD NOT SUPPORTED");
            return INVALID_CARD;
        }

        shStatus = inCDTRead(inIndex);
        vdDebug_LogPrintf("--EMV load cdt--[%d]-shStatus[%d]srTransRec.szPAN[%s]-",inIndex ,shStatus,srTransRec.szPAN);

        if(shStatus != d_OK)
        {   
            inIndex++;        
            continue;           
        }

        if (( memcmp ( strCDT.szPANLo , srTransRec.szPAN , strlen(strCDT.szPANLo) ) <= 0 ) &&
            ( memcmp ( strCDT.szPANHi , srTransRec.szPAN , strlen(strCDT.szPANHi) ) >= 0 ) )
        {

          if (!(strTCT.fDebitFlag == VS_FALSE && strCDT.inType == DEBIT_CARD) && !(!strCDT.fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)) 
            { 
                if ((strCDT.inType == DEBIT_CARD) || (strCDT.inType == EBT_CARD) || (strCDT.IITid != inIssuer)) 
                {
                    if (strCDT.inType != DEBIT_CARD && strCDT.inType != EBT_CARD)
                        inIssuer = strCDT.IITid;

                    inRecNumArray[inNumberOfMatches++] = inIndex;
                    if(inNumberOfMatches > 0)
                        break;

                    if (inNumberOfMatches > 1)
                        szChoiceMsg[strlen(szChoiceMsg)] = '~';

                    switch (strCDT.inType) {
                    case DEBIT_CARD:
                        break;
                    case EBT_CARD:
                        break;
                    case PURCHASE_CARD:
                        break;
                    default:                    
                        break;
                    }

                    if (inNumberOfMatches > 3)
                        break;
                }
            }
        }
        inIndex++;
       
     }while(1);

    if (inNumberOfMatches == 1) 
    {
        inRetVal = inRecNumArray[0];
    }
    else if (inNumberOfMatches > 1) 
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
    }

    if (inRetVal >= 0) 
    {
        inCDTRead(inRetVal);
        srTransRec.CDTid = inRetVal;// save the current CDT that is loaded

        // Get the CDT also for card labels
        inGetIssuerRecord(strCDT.IITid);
    }
    else
    {
        vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    // Check for proper card length 
    inCardLen = strlen(srTransRec.szPAN);

    if ((inCardLen < strCDT.inMinPANDigit) ||
        (inCardLen > strCDT.inMaxPANDigit))
    {
        vdDebug_LogPrintf("BAD CARD LEN");
        vdSetErrorMessage("BAD CARD LEN");
        return INVALID_CARD;
    }

    if (strCDT.fluhnCheck == VS_TRUE)   // Check Luhn
    {
        if (chk_luhn(srTransRec.szPAN))
        {
            vdDisplayErrorMsg(1, 8, "INVALID LUHN");
            return INVALID_CARD;
        }
    }


    if (strTCT.fDebitFlag != DEBIT && strCDT.inType == DEBIT_CARD )
    {
        vdDisplayErrorMsg(1, 8, "INVALID CARD");
        return INVALID_CARD;
    }

    if(strCDT.fExpDtReqd)
    {
        if(shChk_ExpireDate() != d_OK)
        {
            vdDisplayErrorMsg(1, 8, "CARD EXPIRED");
            return CARD_EXPIRED;
        }
    }
    
    //for save Accum file
    srTransRec.IITid= strCDT.IITid;
    srTransRec.HDTid = strCDT.HDTid;
    srTransRec.inCardType = strCDT.inType;

        CTOS_LCDTClearDisplay();
//    CTOS_LCDTPrintXY(1, 8, "                   ");  
    return d_OK;;
}
 */
int inCTOS_EMVCardReadProcess(void) {
    short shResult = 0;
    USHORT usMsgFailedResult = 0;
    int inCount = 0;
    int inIITid = 0;

    BYTE szTag5AStr[20 + 1];

    vdDebug_LogPrintf("-------shCT0S_EMVInitialize1---[%d]--", shResult);
    inMultiAP_Database_EMVTransferDataInit(); //for improve transaction speed 
    //	shResult = shCTOS_EMVAppSelectedProcess();

    //        shResult = usCTOSS_NonEMVCardProcess();
    //		if (d_OK != shResult)
    //			return shResult;
    if (usCTOSS_NonEMVCardProcess() != d_OK) {
        vdDebug_LogPrintf("read as normal emv");
        shResult = shCTOS_EMVAppSelectedProcess();
    }

    vdDebug_LogPrintf("-------shCT0S_EMVInitialize---[%d]--", shResult);

    if (shResult == EMV_CHIP_FAILED) {
        usMsgFailedResult = MSG_TRANS_ERROR;
        return usMsgFailedResult;
    } else if (shResult == EMV_USER_ABORT) {
        usMsgFailedResult = MSG_USER_CANCEL;
        return usMsgFailedResult;
    }

    if (TRUE == fGetMPUCard())
        return d_OK;

    shCTOS_EMVGetChipDataReady();

    /*if it is MPU-UPI card, EMV but not EMV*/
    /**/
    memset(szTag5AStr, 0x00, sizeof (szTag5AStr));
    wub_hex_2_str(srTransRec.stEMVinfo.T5A, szTag5AStr, srTransRec.stEMVinfo.T5A_len);
    szTag5AStr[srTransRec.stEMVinfo.T5A_len * 2] = 0x00;

    vdDebug_LogPrintf("szTag5AStr[%s]", szTag5AStr);
    vdDebug_LogPrintf("srTransRec.szPAN[%s]", srTransRec.szPAN);

    //        if(strlen(srTransRec.szPAN) >= 16){
    //            inIITid = inCDTCheckCard(srTransRec.szPAN, inCount);
    //            
    //            vdDebug_LogPrintf("inIITid = %d", inIITid);
    //            
    //            if(inIITid == 7 || inIITid == 8 || inIITid == 9){
    //                vdCTOS_ResetMagstripCardData();
    //            }
    //        }
    //	if (strlen(srTransRec.szPAN) < 16 && 0 != strcmp(srTransRec.szPAN, szTag5AStr))
    //	{
    //		shResult = usCTOSS_NonEMVCardProcess();
    //		if (d_OK != shResult)
    //			return shResult;
    //	}

    return (d_OK);
}

int inCTOS_ManualEntryProcess(BYTE *szPAN) {
    USHORT usMaxLen = 19;
    BYTE szTempBuf[10];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    char szManEntry[1 + 1];

    get_env("#MANENTRY", szManEntry, 1);

    //@@IBR ADD 20170202 no manual entry
    if (szManEntry[0] == 0x30) {
        vduiClearBelow(8);
        setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO MANUAL ENTRY");
        vduiWarningSound();
        CTOS_Delay(1500);
        return USER_ABORT;
    }
    //@@IBR FINISH ADD 20170202 no manual entry

    //    if(fGetECRTransactionFlg() == TRUE && )

    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "CARD NO: ");

    fGetCardNO = 1;
    if (getCardNO(szPAN) != d_OK) {
        fGetCardNO = 0;
        return USER_ABORT;
    }

    fGetCardNO = 0;
    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(4, DISPLAY_POSITION_LEFT, "CARD NO: ");
    memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
    memset(bDisplayStr, 0x20, usMaxLen * 2);
    strcpy(&bDisplayStr[(usMaxLen - strlen(szPAN))*2], szPAN);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen * 2, 5, bDisplayStr);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

    memset(szTempBuf, 0, sizeof (szTempBuf));
    if (getExpDate(szTempBuf) != d_OK) {
        return USER_ABORT;
    }
    wub_str_2_hex(szTempBuf, &srTransRec.szExpireDate[1], 2);
    wub_str_2_hex(&szTempBuf[2], srTransRec.szExpireDate, 2);
    CTOS_KBDBufFlush();
    vdCTOS_SetTransEntryMode(CARD_ENTRY_MANUAL);
    return d_OK;
    ;
}

int inCTOS_GetCardFields(void) {
    USHORT EMVtagLen;
    BYTE EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;
    int usResult;

    DebugAddSTR("inCTOS_GetCardFields", "Processing...", 20);

#ifdef MPUPIOnUsFlag	
    vdDebug_LogPrintf("inCTOS_GetCardFields byTransType[%d]CDTid[%d]HDTid[%d] IITid[%d] strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]fGetMPUTrans[%d]fGetMPUCard[%d]fGetMPUPIOnUs[%d]", 
		srTransRec.byTransType, srTransRec.CDTid, srTransRec.HDTid, srTransRec.IITid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, fGetMPUTrans(), fGetMPUCard(), fGetMPUPIOnUs());
#else
vdDebug_LogPrintf("inCTOS_GetCardFields byTransType[%d]CDTid[%d]HDTid[%d] IITid[%d] strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]fGetMPUTrans[%d]fGetMPUCard[%d]", 
	srTransRec.byTransType, srTransRec.CDTid, srTransRec.HDTid, srTransRec.IITid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, fGetMPUTrans(), fGetMPUCard());
#endif


    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

SWIPE_AGAIN:

    if (d_OK != inCTOS_ValidFirstIdleKey()) {
		vdDebug_LogPrintf("inCTOS_GetCardFields ABABABABA");
		
        //CTOS_LCDTClearDisplay();
        //vduiClearBelow(2);
        vdDispTransTitle(srTransRec.byTransType);

        inCTOS_DisplayIdleBMP();
    }

#ifdef CBB_FIN_ROUTING
			vdDebug_LogPrintf(" inCTOS_GetCardFields srTransRec.CDTid =[%d]", srTransRec.CDTid); 	

			//fix for case http://118.201.48.214:8080/issues/75.53 #1
			//1) We are getting currency issue with MPU-UPI offus card on USD setup terminal . MPU onus/offus, MPU-JCB onus/off, MPU-UPI offus should be always MMK.
					//fin-mmk				mpu-bpc 				installment
			if(srTransRec.HDTid == 23 || srTransRec.HDTid == 17){
				if(srTransRec.CDTid == 58 || srTransRec.CDTid == 62 || srTransRec.CDTid == 63 || srTransRec.CDTid == 64 || srTransRec.CDTid == 71)	// ADDRESSED NOTE #4
					inCSTRead( strCST.inCurrencyIndex); 
				else	
					inCSTRead(1);	
			}
			else			
				inCSTRead(strCST.inCurrencyIndex);	
#endif

	
    // patrick ECR 20140516 start
    if (strTCT.fECR) // tct
    {
		vdDebug_LogPrintf("inCTOS_GetCardFields strTCT.fECR");
    
        if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0) {
            char szDisplayBuf[30];
            BYTE szTemp1[30 + 1];
			char szTempAmt[AMT_ASC_SIZE+1];

            CTOS_LCDTPrintXY(1, 6, "        ");
            CTOS_LCDTPrintXY(1, 7, "        ");
            CTOS_LCDTPrintXY(1, 8, "               ");
			
            memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
            wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

			if(strcmp(strCST.szCurSymbol, "MMK") == 0){
				memset(szTempAmt, 0x00, sizeof(szTempAmt));
				sprintf(szTempAmt, "%012.0f", atof(szTemp1)/100);
				//memset(szTemp3, 0x00, sizeof(szTemp3));
				//strcpy(szTemp3, szTempAmt);
			}
			
            //gcitra-0728
            //sprintf(szDisplayBuf, "%s %10lu.%02lu", strCST.szCurSymbol,atol(szTemp1)/100, atol(szTemp1)%100);
            //CTOS_LCDTPrintXY(1, 8, szDisplayBuf);	
            
            //CTOS_LCDTPrintXY(1, 6, "AMOUNT:");
            //CTOS_LCDTPrintXY(1, 7, strCST.szCurSymbol);
            setLCDPrint(6, DISPLAY_POSITION_LEFT, "AMOUNT:");
            setLCDPrint(7, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
            
            memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
            //format amount 10+2
			if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
            	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTempAmt, szDisplayBuf);
			}
			else
            	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szDisplayBuf);
			
            //sprintf(szDisplayBuf,"%10.0f.%02.0f",(atof(szTemp1)/100), (atof(szTemp1)%100));
            //sprintf(szDisplayBuf, "%lu.%02lu", atol(szTemp1)/100, atol(szTemp1)%100);
            setLCDPrint(7, DISPLAY_POSITION_RIGHT, szDisplayBuf);
            //gcitra-0728

        }
    }
    // patrick ECR 20140516 end
    CTOS_TimeOutSet(TIMER_ID_1, GET_CARD_DATA_TIMEOUT_VALUE);

    while (1) {
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
            return READ_CARD_TIMEOUT;

        CTOS_KBDInKey(&byKeyBuf);

        if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
            CTOS_KBDBufFlush();
            vdSetFirstIdleKey(0x00);
            byKeyBuf = 0x00;
            goto SWIPE_AGAIN;
        }

        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey())) {
            if (byKeyBuf == d_KBD_CANCEL) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
            if (d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN)) {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            vdSetFirstIdleKey(0x00);
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if (strCDT.fManEntry == FALSE && srTransRec.byEntryMode == CARD_ENTRY_MANUAL) {
                vduiClearBelow(2);
                setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);
                return USER_ABORT;
            }

            break;
        }

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (bySC_status & d_MK_SC_PRESENT) {
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            vdDebug_LogPrintf("--EMV flow----");
            if (d_OK != inCTOS_EMVCardReadProcess()) {
                if (inFallbackToMSR == SUCCESS) {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                } else {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----");
            if (srTransRec.byTransType == REFUND)
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

            /*for MPU Application*/
            if (VS_TRUE == fGetMPUTrans() && VS_TRUE == fGetMPUCard()){

				vdDebug_LogPrintf("inCTOS_GetCardFields FORCE TO CARD_ENTRY_EASY_ICC!!!");
				
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);
            	}

            if (VS_TRUE == fGetMPUTrans() &&
                    (srTransRec.byTransType == VOID ||
                    srTransRec.byTransType == MPU_VOID_PREAUTH ||
                    srTransRec.byTransType == MPU_VOID_PREAUTH_COMP ||
                    srTransRec.byTransType == VOID_PREAUTH)
                    )
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

            vdDebug_LogPrintf("--EMV Read succ---- srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0) {
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

            if (d_OK != inCTOS_CheckEMVFallback()) {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");

                goto SWIPE_AGAIN;

            }

            break;

        }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //Fix for Track2 Len < 35
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if (byMSR_status == d_OK)
            //Fix for Track2 Len < 35
        {
            usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);
            if (usResult != d_OK) {
                CTOS_KBDBufFlush();
                vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                return USER_ABORT;
            }

            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if (d_OK != inCTOS_CheckEMVFallback()) {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
				goto SWIPE_AGAIN;

            }

            break;
        }

    }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        EMVtagLen = 0;
        if (EMVtagLen > 0) {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        } else {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    } else {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
	vdDebug_LogPrintf("inCTOS_GetCardFields END");
	
	CTOS_LCDTClearDisplay();
	
    return d_OK;
    ;
}

int inCTOS_WaveGetCardFields(void) {
    USHORT EMVtagLen;
    BYTE EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;
    int usResult;
    ULONG ulAPRtn;
    BYTE temp[64], temp1[64];
    char szTotalAmount[AMT_ASC_SIZE + 1];
    EMVCL_RC_DATA_EX stRCDataEx;
    BYTE szOtherAmt[12 + 1], szTransType[2 + 1], szCatgCode[4 + 1], szCurrCode[4 + 1];
    char szTemp[12 + 1];

   // vdDebug_LogPrintf("inCTOS_WaveGetCardFields..byExtReadCard=[%d].srTransRec.byTransType.[%d].....", strTCT.byExtReadCard, srTransRec.byTransType);
    vdDebug_LogPrintf("inCTOS_WaveGetCardFields byTransType[%d]CDTid[%d]HDTid[%d] IITid[%d] strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]fGetMPUTrans[%d]fGetMPUCard[%d]", 
		srTransRec.byTransType, srTransRec.CDTid, srTransRec.HDTid, srTransRec.IITid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, fGetMPUTrans(), fGetMPUCard());

    vdDebug_LogPrintf("ECR [%d]", fGetECRTransactionFlg());

    //        if(fGetECRTransactionFlg() == TRUE){
    //            vdSetFirstIdleKey(0x00);
    //            CTOS_KBDBufFlush();
    //            byKeyBuf = FALSE;
    //        }

    //        if(strTCT.fEnableAmountIdle == TRUE && (d_OK == inCTOS_ValidFirstIdleKey()))
    //            return d_OK;

    if (strTCT.byExtReadCard == 1) {
        usResult = inCTOS_WaveGetCardFieldsExternal();
        return usResult;
    }

    switch (srTransRec.HDTid) {
        case 10://alipay
        case 12: //we chat
        case 16://qq
            return d_OK;
            break;
    }

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

SWIPE_AGAIN:
    EntryOfStartTrans :

		vdDebug_LogPrintf("BACK TO START TRANS!!!");

#if 0
            if (d_OK != inCTOS_ValidFirstIdleKey()) {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);

        inCTOS_DisplayIdleBMP();
    }
#endif
    memset(&stRCDataEx, 0x00, sizeof (EMVCL_RC_DATA_EX));
    memset(szOtherAmt, 0x00, sizeof (szOtherAmt));
    memset(szTransType, 0x00, sizeof (szTransType));
    memset(szCatgCode, 0x00, sizeof (szCatgCode));
    memset(szCurrCode, 0x00, sizeof (szCurrCode));
    memset(temp, 0x00, sizeof (temp));
    memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    vdDebug_LogPrintf("szTotalAmount[%s].strTCT.szCTLSLimit=[%s]........", szTotalAmount, strTCT.szCTLSLimit);
    if (NO_CTLS == inCTOSS_GetCtlsMode() || atol(szTotalAmount) > atol(strTCT.szCTLSLimit)) {
        //		CTOS_LCDTPrintXY(1, 3, "Insert/Swipe Card");
        //		CTOS_Beep();
        CTOS_Delay(500);
        return (inCTOS_GetCardFields());
    }
    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);//this function will delay the transaction speed
    if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR()) {
        //CTOS_LCDTClearDisplay();

		vdDebug_LogPrintf("inCTOS_WaveGetCardFields - OOO !!!");
		
        vdDispTransTitle(srTransRec.byTransType);
        CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
        wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%012.0f", atof(szTotalAmount));
        memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
        strcpy(szTotalAmount, szTemp);
        //format amount 10+2
        memset(temp1, 0x00, sizeof (temp1));
        if (memcmp(strCST.szCurSymbol, "MMK", 3) == 0) {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, temp1);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
        }


        //		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
        sprintf(temp, " Amount: %s", temp1);
        //sprintf(temp," Amount: %10.0f.%02.0f",(atof(szTotalAmount)/100), (atof(szTotalAmount)%100));
        //sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
        CTOS_LCDTPrintXY(1, 4, temp);
    } else
        wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);

    if (srTransRec.byTransType == REFUND)
        szTransType[0] = 0x20;

    sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
    strcpy(szCurrCode, szCatgCode);

    //if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
    if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR()) {
        ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount, szOtherAmt, szTransType, szCatgCode, szCurrCode);
        //	ulAPRtn = EMVCL_InitTransaction(atol(szTotalAmount));
        if (ulAPRtn != d_EMVCL_NO_ERROR) {
            vdSetErrorMessage("CTLS InitTrans Fail!");
            return d_NO;
        }
    }

    /*Clear KB bufer*/
    CTOS_KBDBufFlush();
    byKeyBuf = 0x00;

    CTOS_TimeOutSet(TIMER_ID_1, GET_CARD_DATA_TIMEOUT_VALUE);

    while (1) {
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            return READ_CARD_TIMEOUT;
        }

        CTOS_KBDInKey(&byKeyBuf); //input  

        if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
            CTOS_KBDBufFlush();
            vdSetFirstIdleKey(0x00);
            byKeyBuf = 0x00;
        }

        vdDebug_LogPrintf("byKeyBuf [%d], inCTOS_ValidFirstIdleKey [%d]", byKeyBuf, inCTOS_ValidFirstIdleKey());

        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey())) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            //                CTOS_KBDGet(&byKeyBuf);



            if (byKeyBuf == d_KBD_CANCEL) {
                CTOS_LCDTClearDisplay();
                CTOS_KBDBufFlush();
                //                    vdSetErrorMessage("TXN CANCELLED");
                return USER_ABORT;
            }

            memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
            if (d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN)) {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            vdSetFirstIdleKey(0x00);
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            //@@IBR ADD 20170202 no manual entry
            if (strCDT.fManEntry == FALSE) {
                vduiClearBelow(8);
                setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);
                return USER_ABORT;
            }
            //@@IBR FINISH ADD 20170202 no manual entry

            break;
        }

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (bySC_status & d_MK_SC_PRESENT) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            
            vdDebug_LogPrintf("--EMV flow----" );

			#ifdef CB_MPU_NH_MIGRATION
			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if(srTransRec.byTransType == REFUND)
			{
				vduiClearBelow(7);
                vduiClearBelow(8);
				setLCDPrint(7, DISPLAY_POSITION_CENTER, "PLEASE REMOVE CARD");
                setLCDPrint(8, DISPLAY_POSITION_CENTER, "AND DO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);	
				goto SWIPE_AGAIN;
			}
			#endif

            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            vdDebug_LogPrintf("--inCTOS_WaveGetCardFields EMV flow----");
            if (d_OK != inCTOS_EMVCardReadProcess()) {

				vdDebug_LogPrintf("--inCTOS_WaveGetCardFields EMV flow----inFallbackToMSR::[%d]", inFallbackToMSR);
				
                if (inFallbackToMSR == SUCCESS) {

					vdDebug_LogPrintf("--inCTOS_WaveGetCardFields---PLS SWIPE CARD---");
					
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                } else {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----");
            if (srTransRec.byTransType == REFUND)
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

            /*for MPU Application*/
            if (VS_TRUE == fGetMPUTrans() && VS_TRUE == fGetMPUCard()){
				
				vdDebug_LogPrintf("inCTOS_WaveGetCardFields FORCE TO CARD_ENTRY_EASY_ICC!!!");
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);
            	}

            if (VS_TRUE == fGetMPUTrans() &&
                    (srTransRec.byTransType == VOID ||
                    srTransRec.byTransType == MPU_VOID_PREAUTH ||
                    srTransRec.byTransType == MPU_VOID_PREAUTH_COMP ||
                    srTransRec.byTransType == VOID_PREAUTH)
                    )
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);


            vdDebug_LogPrintf("--EMV Read succ---- srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }

		vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A-[%d][%d]", inFallbackToMSR, srTransRec.byEntryMode);

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0){
			vdDebug_LogPrintf("inCTOS_WaveGetCardFields-AB");
			
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

			vdDebug_LogPrintf("inCTOS_WaveGetCardFields-ABC ");

            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }
			vdDebug_LogPrintf("inCTOS_WaveGetCardFields-ABCD");


			
			//if(inFallbackToMSR == SUCCESS && srTransRec.byEntryMode == CARD_ENTRY_WAVE)				
			//	vdDebug_LogPrintf("inCTOS_WaveGetCardFields-ABCDE");
			//else{
				#ifndef ENHANCEMENT_1861 // for testing - 9) Mgtstripe Sales Tranx will not allowed for Pure MPU,Pure UPI, MPU-JCB Co-brand, MPU-UPI Co-Brand.
									    //But Mgt Stripe with Fallback will be allowed.
	            if (d_OK != inCTOS_CheckEMVFallback()) {
	                vdCTOS_ResetMagstripCardData();

					vdDebug_LogPrintf("inCTOS_WaveGetCardFields-ABCDE");
					
	                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
	                if (1 == chGetIdleEventSC_MSR())
	                    return USER_ABORT;

	                goto SWIPE_AGAIN;

	            }
				#endif
				vdDebug_LogPrintf("inCTOS_WaveGetCardFields-ABCDEF-EntryMode [%d]", srTransRec.byEntryMode);
				break;
			//}
        }
		

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if (byMSR_status == d_OK)//Fix for Track2 Len < 35
        {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);
            if (usResult != d_OK) {
                CTOS_KBDBufFlush();
                vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                return USER_ABORT;
            }

            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if (d_OK != inCTOS_CheckEMVFallback()) {
                vdCTOS_ResetMagstripCardData();

				vdDebug_LogPrintf("inCTOS_WaveGetCardFields-ABCDEFG");
				
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                if (1 == chGetIdleEventSC_MSR())
                    return USER_ABORT;

                goto SWIPE_AGAIN;

            }

            break;
        }


		vdDebug_LogPrintf("inCTOS_WaveGetCardFields-CTLS MODE-[%d]", inCTOSS_GetCtlsMode());

        if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode()) {
            ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

            // V3 contactless reader
            //		EMVCL_StopIdleLEDBehavior(NULL);
            //		EMVCL_SetLED(0x0F, 0x08);

            // patrick test code 20141230 start
#ifndef d_EMVCL_RC_SEE_PHONE
#define d_EMVCL_RC_SEE_PHONE		0xA00000AF // AMEX
#endif
#define d_EMVCL_RC_SEE_PHONE2		0xA00000E4
            // patrick test code 20141230 end		

            if (ulAPRtn == d_EMVCL_RC_DEK_SIGNAL) {
                vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen, stRCDataEx.baChipData);
            } else if (ulAPRtn == d_EMVCL_RC_SEE_PHONE || d_EMVCL_RC_SEE_PHONE2) {
                //CTOS_LCDTClearDisplay();
                vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
                CTOS_Delay(3000);
                goto EntryOfStartTrans;
            }
            else if (ulAPRtn == d_EMVCL_TX_CANCEL) {
                vdDisplayErrorMsg(1, 8, "USER CANCEL");
                return USER_ABORT;
            } else if (ulAPRtn == d_EMVCL_RX_TIMEOUT) {
                if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                    inCTOSS_CLMCancelTransaction();

                CTOS_Beep();
                CTOS_Delay(50);
                CTOS_Beep();
                CTOS_Delay(50);
                vdDisplayErrorMsg(1, 8, "TIMEOUT");
                return USER_ABORT;
            } else if (ulAPRtn != d_EMVCL_PENDING) {
                CTOS_Beep();
                CTOS_Delay(50);
                CTOS_Beep();
                CTOS_Delay(50);
                CTOS_Beep();
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                break;
            }
        } else {

			vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A2-byTransType-[%d]", srTransRec.byTransType);
		
            memset(szTransType, 0x00, sizeof (szTransType));
            strcpy(szOtherAmt, "000000000000");
			
            if (srTransRec.byTransType == REFUND)
                strcpy(szTransType, "20");
            else
                strcpy(szTransType, "00");


			#ifdef CB_MPU_NH_MIGRATION
			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if (srTransRec.byTransType == REFUND){
				//CTOS_LCDTClearDisplay();		
				CTOS_LCDTPrintXY(1, 8, "PLEASE ENTER CARD NO.");
				CTOS_Beep();
				CTOS_Delay(1000);
			}			
			#endif
			else
			{
            vdDebug_LogPrintf("Ctls V3 Trans szTotalAmount=[%s], szOtherAmt=[%s]", szTotalAmount, szOtherAmt);
            ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount, szOtherAmt, szTransType, szCatgCode, szCurrCode, &stRCDataEx);

			vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A3-[%ld]", ulAPRtn);
			
            CTOS_Beep();
            CTOS_Delay(50);
            CTOS_Beep();
            CTOS_Delay(50);
            CTOS_Beep();

			//provision for case #1468 - Decline Quickpass function rotue to MPU pacakge
			#if 0
			{
				int AIDlen;
				BYTE AID[16];
			
				memset(AID,0x00,sizeof(AID));
				AIDlen = srTransRec.stEMVinfo.T84_len;
				memcpy(AID,srTransRec.stEMVinfo.T84,AIDlen);
			
	            if (ulAPRtn == d_OK && srTransRec.HDTid != 7 && srTransRec.byEntryMode == CARD_ENTRY_WAVE &&
					(memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x01",AIDlen) != 0)||
					(memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x02",AIDlen) != 0)|| 
					(memcmp(AID,"\xA0\x00\x00\x03\x33\x01\x01\x03",AIDlen) != 0))
				{
	                vduiClearBelow(2);
	                vdSetErrorMessage("CARD NOT SUPPORTED");
	                return USER_ABORT;				
	            }					
        	}
			#endif
			
            if (ulAPRtn == d_OK) {
				vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A30-");
				
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                ulAPRtn = d_EMVCL_RC_DATA;
                break;
            }

            if (ulAPRtn == 155) {
				vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A31-");
				
                vduiClearBelow(2);
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return USER_ABORT;
            }
            if (ulAPRtn == d_NO){
				vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A32-");
				
                return USER_ABORT;
            }

            if (ulAPRtn == d_MORE_PROCESSING){
				vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A33-");
				
                return (inCTOS_GetCardFields());
            }

				vdDebug_LogPrintf("inCTOS_WaveGetCardFields-A34-");
          }
			
        }

    }

	vdDebug_LogPrintf("inCTOS_WaveGetCardFields-B-[%d]", srTransRec.byEntryMode);

    if (srTransRec.byEntryMode == CARD_ENTRY_WAVE) {
        if (ulAPRtn != d_EMVCL_RC_DATA) {
            vdCTOSS_WaveCheckRtCode(ulAPRtn);
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();
			vdDebug_LogPrintf("inCTOS_WaveGetCardFields-B1");

            return d_NO;
        }

		vdDebug_LogPrintf("inCTOS_WaveGetCardFields-B2");

        if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode()) {
            if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx)) {
                inCTOSS_CLMCancelTransaction();
                return d_NO;
            }
        } else {
            if (d_OK != inCTOSS_V3AnalyzeTransaction(&stRCDataEx))
                return d_NO;
        }

		vdDebug_LogPrintf("inCTOS_WaveGetCardFields-B3");

        //Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex()) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		vdDebug_LogPrintf("inCTOS_WaveGetCardFields-B4");

        if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
            inCTOSS_CLMCancelTransaction();

    }

	vdDebug_LogPrintf("inCTOS_WaveGetCardFields-B5");

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        EMVtagLen = 0;
        if (EMVtagLen > 0) {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        } else {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    } else {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
}

int inCTOS_PreConnect(void) {
    int inRetVal;
    int inParkingFee;

    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]", strCPT.inCommunicationMode);
    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) {
        vdSetErrorMessage("COMM INIT ERR");
        return (d_NO);
    }

    if (VS_TRUE == strTCT.fDemo)
        return (d_OK);

#ifdef PARKING_FEE
    inParkingFee = get_env_int("PARKFEE");
    if (inParkingFee == TRUE) {
        if (srTransRec.byOffline == CN_TRUE)
            return (d_OK);
    }
#endif

    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);
    if (inRetVal != d_OK) {
        if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
            vdSetErrorMessage("GPRS NOT ESTABLISHED");
        else
            vdSetErrorMessage("COMM INIT ERR");
        return (d_NO);
    }

    if (CN_FALSE == srTransRec.byOffline) {
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);
        //for improve transaction speed
    }

    return (d_OK);
}

int inCTOS_PreConnectEx(void) {
    int inRetVal;

    vdSetInit_Connect(1);
    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]", strCPT.inCommunicationMode);
    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK) {
        vdSetErrorMessage("COMM INIT ERR");
        return (d_NO);
    }

    if (VS_TRUE == strTCT.fDemo)
        return (d_OK);

    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);
    if (inRetVal != d_OK) {
        if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
            vdSetErrorMessage("GPRS NOT ESTABLISHED");
        else
            vdSetErrorMessage("COMM INIT ERR");
        return (d_NO);
    }

    if (CN_FALSE == srTransRec.byOffline) {
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);
        //for improve transaction speed
    }

    return (d_OK);
}

int inCTOS_CheckAndSelectMutipleMID(void) {
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;
    short shFalshMenu = 1;
    BYTE isUP = FALSE, isDOWN = FALSE;
    BYTE bHeaderAttr = 0x01 + 0x04, iCol = 1;
    BYTE x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT MERCHANT";
    char szMitMenu[1024];
    int inLoop = 0;
    short shMinLen = 1;
    short shMaxLen = 20;
    BYTE Bret;
    unsigned char szOutput[30];

    unsigned char bstatus = 0;

    //if (inMultiAP_CheckSubAPStatus() == d_OK)
    //    return d_OK;

    memset(szMitMenu, 0x00, sizeof (szMitMenu));
    vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID=[%d]", strHDT.inHostIndex);
    //get the index , then get all MID from the MMT list and prompt to user to select
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfRecords);

    CTOS_KBDBufFlush(); //cleare key buffer
    if (inNumOfRecords > 1) {
        for (inLoop = 0; inLoop < inNumOfRecords; inLoop++) {
            strcat((char *) szMitMenu, strMMT[inLoop].szMerchantName);
            if (inLoop + 1 != inNumOfRecords)
                strcat((char *) szMitMenu, (char *) " \n");
        }

        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

        if (key == 0xFF) {
            CTOS_LCDTClearDisplay();
            setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
            vduiWarningSound();
            return -1;
        }

        if (key > 0) {
            if (d_KBD_CANCEL == key)
                return -1;

            vdDebug_LogPrintf("key[%d]--fEnablePSWD[%d]-----", key, strMMT[key - 1].fEnablePSWD);
            if (strMMT[key - 1].fEnablePSWD == 1) {
                //CTOS_LCDTClearDisplay();
                vduiClearBelow(2);
                vdDispTransTitle(srTransRec.byTransType);
                CTOS_LCDTPrintXY(1, 3, "ENTER PASSWORD:");
                while (1) {
                    memset(szOutput, 0x00, sizeof (szOutput));
                    shMinLen = strlen(strMMT[key - 1].szPassWord);

                    Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    if (strncmp(szOutput, strMMT[key - 1].szPassWord, shMinLen) == 0) {
                        break;
                    } else if (d_KBD_CANCEL == Bret)
                        return Bret;
                    else {
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        //CTOS_LCDTClearDisplay();
                        vduiClearBelow(2);
                        vdDispTransTitle(srTransRec.byTransType);
                        CTOS_LCDTPrintXY(1, 3, "ENTER PASSWORD:");
                    }
                }
            }

            memcpy(&strMMT[0], &strMMT[key - 1], sizeof (STRUCT_MMT));
        }
    }
#if 0
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush(); //cleare key buffer
    if (inNumOfRecords > 1) {
        if (inNumOfRecords > ITEMS_PER_PAGE)
            isDOWN = TRUE;

        shTotalPageNum = (inNumOfRecords / ITEMS_PER_PAGE == 0) ? (inNumOfRecords / ITEMS_PER_PAGE) : (inNumOfRecords / ITEMS_PER_PAGE + 1);
        shLastPageItems = (inNumOfRecords / ITEMS_PER_PAGE == 0) ? (ITEMS_PER_PAGE) : (inNumOfRecords % ITEMS_PER_PAGE);

        do {
            //display items perpage
            if (shTotalPageNum == 0)//the total item is amaller than ITEMS_PER_PAGE
            {
                shPageItems = inNumOfRecords;
            } else if (shCurrentPageNum == shTotalPageNum)//Last page
                shPageItems = shLastPageItems;
            else
                shPageItems = ITEMS_PER_PAGE;

            if (shFalshMenu) {
                CTOS_LCDTClearDisplay();
                vdDispTitleString("SELECT MERCHANT");
                CTOS_LCDTPrintXY(2, 8, "PLS SELECT");
                for (shLoop = 0; shLoop < shPageItems/*ITEMS_PER_PAGE*/; shLoop++) {

                    memset(szDisplay, 0, sizeof (szDisplay));
                    sprintf(szDisplay, "%d: %s", shLoop + 1, strMMT[shLoop + (shCurrentPageNum - 1) * ITEMS_PER_PAGE].szMerchantName);
                    CTOS_LCDTPrintXY(2, shLoop + 3, szDisplay);

                }
                vdCTOS_LCDGShowUpDown(isUP, isDOWN);
            }


            key = WaitKey(60);

            switch (key) {
                case d_KBD_DOWN:
                case d_KBD_DOT:

                    shFalshMenu = 1;

                    shCurrentPageNum++;
                    if (shCurrentPageNum > shTotalPageNum)
                        shCurrentPageNum = 1;
                    bstatus = 2;
                    break;

                    /*
                    case d_KBD_UP:

                        shCurrentPageNum --;
                        if(shCurrentPageNum < 1)
                            shCurrentPageNum = shTotalPageNum;
                        bstatus = 2;        
                        break;          
                     */
                case d_KBD_CANCEL:
                    return FAIL;
                case d_KBD_1:
                    //set the unique MMT num

                    memcpy(&strMMT[0], &strMMT[0 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                    bstatus = 0;
                    break;
                case d_KBD_2:
                    if (shPageItems < 2) {
                        bstatus = -1;
                    } else {
                        memcpy(&strMMT[0], &strMMT[1 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;
                case d_KBD_3:
                    if (shPageItems < 3) {
                        bstatus = -1;
                    } else {
                        memcpy(&strMMT[0], &strMMT[2 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;

                case d_KBD_4:
                    if (shPageItems < 4) {
                        bstatus = -1;
                    } else {
                        memcpy(&strMMT[0], &strMMT[3 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;
                case d_KBD_5:
                    if (ITEMS_PER_PAGE < 5) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 5) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[4 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_6:
                    if (ITEMS_PER_PAGE < 6) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 6) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[5 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_7:
                    if (ITEMS_PER_PAGE < 7) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 7) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[6 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_8: //Max 8 items for one page
                    if (ITEMS_PER_PAGE < 8) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 8) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[7 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                default:
                    shFalshMenu = 0;
                    bstatus = -1;
                    break;

            }

            if ((-1) == bstatus) {
                vdSetErrorMessage("LOAD MMT ERR");
                return FAIL;

            } else if (0 == bstatus) {
                break;
            }

        } while (1);

    } else {
        //One merchant only     
        //vduiDisplayStringCenter(1,strMMT[0].szMID);

    }
#endif
    srTransRec.MITid = strMMT[0].MITid;
    strcpy(srTransRec.szTID, strMMT[0].szTID);
    strcpy(srTransRec.szMID, strMMT[0].szMID);
    memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);


	vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID szTID[%s]", srTransRec.szTID);		
	vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID szMID[%s]", srTransRec.szMID);			
	vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID szHostLabel[%s]", srTransRec.szHostLabel);			
	

    vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]", strMMT[0].szATCMD1, strMMT[0].szATCMD2, strMMT[0].szATCMD3, strMMT[0].szATCMD4, strMMT[0].szATCMD5);

    return SUCCESS;
}

int inCTOS_CheckAndSelectMID(void) {
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;

    unsigned char key;
    unsigned char bstatus = 0;

    DebugAddSTR("inCTOS_CheckAndSelectMutipleMID", "Processing...", 20);

    //get the index , then get all MID from the MMT list and prompt to user to select
    inMMTReadNumofRecords(srTransRec.HDTid, &inNumOfRecords);
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush(); //cleare key buffer
    if (inNumOfRecords > 1) {
        shTotalPageNum = (inNumOfRecords / ITEMS_PER_PAGE == 0) ? (inNumOfRecords / ITEMS_PER_PAGE) : (inNumOfRecords / ITEMS_PER_PAGE + 1);
        shLastPageItems = (inNumOfRecords / ITEMS_PER_PAGE == 0) ? (ITEMS_PER_PAGE) : (inNumOfRecords % ITEMS_PER_PAGE);

        do {
            //display items perpage
            if (shTotalPageNum == 0)//the total item is amaller than ITEMS_PER_PAGE
            {
                shPageItems = inNumOfRecords;
            } else if (shCurrentPageNum == shTotalPageNum)//Last page
                shPageItems = shLastPageItems;
            else
                shPageItems = ITEMS_PER_PAGE;

            CTOS_LCDTClearDisplay();
            for (shLoop = 0; shLoop < shPageItems/*ITEMS_PER_PAGE*/; shLoop++) {

                memset(szDisplay, 0, sizeof (szDisplay));
                sprintf(szDisplay, "%d: %s", shLoop + 1, strMMT[shLoop + (shCurrentPageNum - 1) * ITEMS_PER_PAGE].szMerchantName);
                CTOS_LCDTPrintXY(2, shLoop + 2, szDisplay);

            }

            key = WaitKey(60);

            switch (key) {
                case d_KBD_DOWN:

                    shCurrentPageNum++;
                    if (shCurrentPageNum > shTotalPageNum)
                        shCurrentPageNum = 1;
                    bstatus = 2;
                    break;


                case d_KBD_UP:

                    shCurrentPageNum--;
                    if (shCurrentPageNum < 1)
                        shCurrentPageNum = shTotalPageNum;
                    bstatus = 2;
                    break;

                case d_KBD_CANCEL:
                    return FAIL;
                case d_KBD_1:
                    //set the unique MMT num

                    memcpy(&strMMT[0], &strMMT[0 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                    //vduiDisplayStringCenter(2,strMMT[0].szMID);
                    bstatus = 0;
                    break;
                case d_KBD_2:
                    if (shPageItems < 2) {
                        bstatus = -1;
                    } else {
                        memcpy(&strMMT[0], &strMMT[1 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;
                case d_KBD_3:
                    if (shPageItems < 3) {
                        bstatus = -1;
                    } else {
                        memcpy(&strMMT[0], &strMMT[2 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;

                case d_KBD_4:
                    if (shPageItems < 4) {
                        bstatus = -1;
                    } else {
                        memcpy(&strMMT[0], &strMMT[3 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;
                case d_KBD_5:
                    if (ITEMS_PER_PAGE < 5) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 5) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[4 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_6:
                    if (ITEMS_PER_PAGE < 6) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 6) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[5 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_7:
                    if (ITEMS_PER_PAGE < 7) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 7) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[6 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_8: //Max 8 items for one page
                    if (ITEMS_PER_PAGE < 8) {
                        bstatus = -1;
                        break;
                    } else {
                        if (shPageItems < 8) {
                            bstatus = -1;
                        } else {
                            memcpy(&strMMT[0], &strMMT[7 + (shCurrentPageNum - 1) * ITEMS_PER_PAGE], sizeof (STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                default:
                    bstatus = -1;
                    break;

            }

            if ((-1) == bstatus) {
                return FAIL;

            } else if (0 == bstatus) {
                break;
            }

        } while (1);

    } else {
        //One merchant only     
        //vduiDisplayStringCenter(1,strMMT[0].szMID);

    }

    srTransRec.MITid = strMMT[0].MITid;
    return SUCCESS;


}

int inCTOS_GetTxnPassword(void) {
#define NO_PW           0
#define SUPER_PW            1
#define SYSTERM_PW          2
#define ENGINEERPW         3
#define APPLICATIONPW      4

    DebugAddSTR("inCTOS_GetTxnPassword", "Processing...", 20);


    unsigned char szOutput[30], szDisplay[30];
    int inTxnTypeID;
    short ret = 0;
    short shMinLen = 4;
    short shMaxLen = 12;
    BYTE key;
    BYTE Bret;
    short shCount = 0;
    short shRsesult = d_OK;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;


    memset(szOutput, 0, sizeof (szOutput));
    memset(szDisplay, 0, sizeof (szDisplay));
    inTxnTypeID = srTransRec.byTransType;

    vduiLightOn();

    inPITRead(inTxnTypeID);
    if (NO_PW == strPIT.inPasswordLevel) {
        return d_OK;
    }

    CTOS_KBDHit(&key); //clear key buffer
    while (shCount < 3) {
        shMaxLen = 12;
        switch (srTransRec.byTransType) {
            case SALE:
            case PRE_AUTH:
            case VOID_PREAUTH:
            case REFUND:
            case SALE_OFFLINE:
            case EPP_SALE:
            case VOID:
            case SALE_TIP:
            case SALE_ADJUST:
            case SETTLE:
            case BATCH_REVIEW:
            case BATCH_TOTAL:
            case SETUP: /*sidumili: Issue#:000087 [prompt password]*/
            case MPU_PREAUTH:
            case MPU_PREAUTH_COMP:
            case MPU_PREAUTH_COMP_ADV:
            case MPU_SETTLE:
            case DEMO_MODE:
            case BALANCE_ENQUIRY:

                if (SUPER_PW == strPIT.inPasswordLevel) {

                    vdDebug_LogPrintf("inCTOS_GetTxnPassword     SUPER_PW ");
                    CTOS_LCDTClearDisplay();
                    //vduiClearBelow(2);
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 3, "ENTER PASSWORD:");

                    Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

                    //issue:285
                    if (Bret == 255) //timeout
                        return Bret;
                    //issue:285

                    //#issue:231
                    if (d_KBD_CANCEL == Bret)
                        return Bret;
                    else
                        //#issue:231
                        if ((strcmp(szOutput, strTCT.szSuperPW) == 0) ||
                            (strcmp(szOutput, strTCT.szSystemPW) == 0) ||
                            (strcmp(szOutput, strTCT.szEngineerPW) == 0)) {
                        return d_OK;
                    }                        //#issue:23
                        /*else if(d_KBD_CANCEL == Bret)
                                                    return Bret;*/
                        //#issue:23
                    else {
                        //CTOS_LCDTClearDisplay();
                        //vduiClearBelow(2);
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                } else if (SYSTERM_PW == strPIT.inPasswordLevel) {

                    vdDebug_LogPrintf("inCTOS_GetTxnPassword     SYSTERM_PW ");
                    CTOS_LCDTClearDisplay();
                    //vduiClearBelow(2);
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 3, "SYSTEM PASSWORD:");

                    Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

                    //issue:285
                    if (Bret == 255) //timeout
                        return Bret;
                    //issue:285

                    //#issue:231
                    if (d_KBD_CANCEL == Bret)
                        return Bret;
                    else
                        //#issue:231

                        if (strcmp(szOutput, strTCT.szSystemPW) == 0) {
                        return d_OK;
                    }                        //#issue:23
                        /*else if(d_KBD_CANCEL == Bret)
                                                    return Bret;*/
                        //#issue:23
                    else {
                        //CTOS_LCDTClearDisplay();
                        //vduiClearBelow(2);
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                }
                else if (ENGINEERPW == strPIT.inPasswordLevel) {
                    vdDebug_LogPrintf("inCTOS_GetTxnPassword	 ENGINEERPW ");
                    CTOS_LCDTClearDisplay();
                    //vduiClearBelow(2);
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 3, "PASSWORD:");

                    Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

                    //issue:285
                    if (Bret == 255) //timeout
                        return Bret;
                    //issue:285

                    //#issue:231
                    if (d_KBD_CANCEL == Bret)
                        return Bret;
                    else
                        //#issue:231

                        if (strcmp(szOutput, strTCT.szEngineerPW) == 0) {
                        return d_OK;
                    }
                    //#issue:23
                    /*else if(d_KBD_CANCEL == Bret)
                                                return Bret;*/
                    //#issue:23
                    {
                        CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }

                } else if (APPLICATIONPW == strPIT.inPasswordLevel) {
                    vdDebug_LogPrintf("inCTOS_GetTxnPassword	 APPLICATIONPW ");

                    CTOS_LCDTClearDisplay();
                    //vduiClearBelow(2);
                    vdDispTransTitle(srTransRec.byTransType);
                    CTOS_LCDTPrintXY(1, 3, "PASSWORD:");

                    Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);

                    //issue:285
                    if (Bret == 255) //timeout
                        return Bret;
                    //issue:285

                    //#issue:231
                    if (d_KBD_CANCEL == Bret)
                        return Bret;
                    else
                        //#issue:231

                        vdDebug_LogPrintf("szOutput=%s", szOutput);
                    vdDebug_LogPrintf("strTCT.szApplicationPW=%s", strTCT.szApplicationPW);
                    if (strcmp(szOutput, strTCT.szApplicationPW) == 0) {
                        return d_OK;
                    }
                    //#issue:23
                    /*else if(d_KBD_CANCEL == Bret)
                                                return Bret;*/
                    //#issue:23
                    {
                        CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                } else {
                    return d_OK;
                }


            default:

                return d_OK;

        }
        if (FAIL == shRsesult)
            shCount++;
    }

    return shRsesult;
}



int inCTOS_TUPSelectHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;

    inHostIndex = (short) srTransRec.HDTid;

    vdDebug_LogPrintf("inCTOS_TUPSelectHost >>byTransType[%d]>>inCurrencyIdx[%d]>> inHostIndex >> inHostIndex[%d] >> fGetCashAdvAppFlag[%d]", srTransRec.byTransType, strHDT.inCurrencyIdx, inHostIndex, fGetCashAdvAppFlag());


#ifdef TOPUP_RELOAD
		if (srTransRec.byTransType == BALANCE_ENQUIRY || srTransRec.byTransType == RELOAD || srTransRec.byTransType == TOPUP || srTransRec.byTransType == TOPUP_RELOAD_LOGON)
			inHostIndex = 20;
#endif
	

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {

        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;


	vdDebug_LogPrintf("inCTOS_TUPSelectHost >inCurrencyIndex>inHostIndex>CDTid>[%d][%d][%d]", inCurrencyIdx, inHostIndex, srTransRec.CDTid);

        if (srTransRec.fIsInstallment == TRUE)
            inHostIndex = 6;

        if (inCSTRead(inCurrencyIdx) != d_OK) {

            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

	#if 0
        if (inMMTReadRecord(inHostIndex, srTransRec.MITid) != d_OK) {
            vdSetErrorMessage("LOAD MMT ERR");
            return (d_NO);
        }
		#endif

        if (inCPTRead(inHostIndex) != d_OK) {
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

        inCTOS_PreConnect();

        return (d_OK);
    }


}


int inCTOS_CAVSelectHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;

    inHostIndex = (short) srTransRec.HDTid;

    vdDebug_LogPrintf("inCTOS_CAVSelectHost >>[%d] inHostIndex >> inHostIndex[%d] , fGetCashAdvAppFlag[%d]", strHDT.inCurrencyIdx, inHostIndex, fGetCashAdvAppFlag());

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {

        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;


	vdDebug_LogPrintf("inCTOS_CAVSelectHost >inCurrencyIndex>inHostIndex>CDTid>[%d][%d][%d][%d]", inCurrencyIdx, inHostIndex, srTransRec.CDTid, srTransRec.MITid);

        if (srTransRec.fIsInstallment == TRUE)
            inHostIndex = 6;

        if (inCSTRead(inCurrencyIdx) != d_OK) {

            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

		#if 0
        if (inMMTReadRecord(inHostIndex, srTransRec.MITid) != d_OK) {
			vdDebug_LogPrintf("inCTOS_CAVSelectHost > LOAD MMT ERR!!!");
			
            vdSetErrorMessage("LOAD MMT ERR");
            return (d_NO);
        }
		#endif

        if (inCPTRead(inHostIndex) != d_OK) {
			vdDebug_LogPrintf("inCTOS_CAVSelectHost > LOAD CPT ERR!!!");
			
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

        inCTOS_PreConnect();
		vdDebug_LogPrintf("inCTOS_CAVSelectHost > END!!!");

        return (d_OK);
    }


}


int inCTOS_SelectHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;
    char szAPName[25];
    int inAPPID, i, inNumOfHDTRecord;
    char szTemp[2 + 1];

#ifdef QUICKPASS_FNX_HOST // change request, all UPI quickpass (CTLS) txn should be routed to Finexus host.
	BOOL fMPUPICardFlag = 0;
	int AIDlen;
	BYTE AID[16];
#endif	


    inHostIndex = (short) strCDT.HDTid;

    vdDebug_LogPrintf("START inCTOS_SelectHost =[%d][%d][%d][%d][%d]", srTransRec.byTransType, inHostIndex, strHDT.inHostIndex, srTransRec.HDTid, srTransRec.fIsInstallment);
	vdDebug_LogPrintf("inCTOS_SelectHost >> strCST.inCurrencyIndex = [%d] strHDT.inCurrencyIdx = [%d]", strCST.inCurrencyIndex, strHDT.inCurrencyIdx);

    if ((srTransRec.byTransType == EFTSEC_TMK) || (srTransRec.byTransType == EFTSEC_TWK) || (srTransRec.byTransType == EFTSEC_TMK_RSA) || (srTransRec.byTransType == EFTSEC_TWK_RSA)) {
        inHDTRead(strEFT[0].inHDTid);

		/*                        
				    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
                        inNumOfHDTRecord = inHDTNumRecord();
                        for(i=0; i <= inNumOfHDTRecord; i++)
                        {
                                inHDTRead(i);
                                if(memcmp(strHDT.szHostLabel, "EFTSEC", 6) == 0)
                                {
                                        inEFTReadByHDTid(i);
                                        break;
                                }
                        }
         */
		 
        inHostIndex = strHDT.inHostIndex;
    }


    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (srTransRec.HDTid != 0)
        inHostIndex = srTransRec.HDTid;

    if (srTransRec.fIsInstallment == TRUE) {
        inHostIndex = 6;
    }

    if (srTransRec.byTransType == IPP_SIGN_ON){
        inHostIndex = 6;
    }

vdDebug_LogPrintf("1. inCTOS_SelectHost =[%d]", inHostIndex);

#ifdef TOPUP_RELOAD
    if (srTransRec.byTransType == BALANCE_ENQUIRY || srTransRec.byTransType == RELOAD || srTransRec.byTransType == TOPUP)
        inHostIndex = 20;
#endif


/*
NOTES:
>>>> (1) MPU-JCB OnUs cards (3571 0851, 3571 0951, 3571 0975 and 3571 0925) should be able to route either Finexus host or MPU host.  Host routing rides with the Current Terminal Currency index.
>>>> (2) MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus should be always MMK.
>>>> (3) IF Default Currency IS MMK all BIN regardless of HDTIDx, transcation will be routed to FIN-MMK (HDTID 23).
>>>> (4) for MPU-UPI OnUs cards (MPU-UPI-Class, MPU-UPI-Gold, MPU-UPI-Platinum and MPU-UPI).
*/

#ifdef CBB_FIN_ROUTING	
		vdDebug_LogPrintf("inCTOS_SelectHost CBB_FIN_ROUTING");

		#if 0
		//MMK currency
		if(strCST.inCurrencyIndex == 1)
		{ 
			if(inHostIndex == 17 && srTransRec.IITid == 8) // Addressed NOTE #1.
				inHostIndex = 17;
			else				
				inHostIndex = 23; 	// Addressed NOTES #2 and #3 above
		}
		else // USD CURRENCY
		{	
			//Addressed NOTE #1 and #2
			if((inHostIndex == 23) && (srTransRec.IITid == 7 || srTransRec.IITid == 8 || srTransRec.IITid == 9))
				inHostIndex = 23;
			else if(inHostIndex == 17 && srTransRec.IITid == 8)	//Addressed NOTE #1.
				inHostIndex = 17; // make a provision for 3571 0975 and 3571 0925 (which should fall either FIN/ MPU host. (http://118.201.48.214:8080/issues/75.42)
			else
				inHostIndex = 22;
		}
		#else
		//MMK currency
		if(strCST.inCurrencyIndex == 1)
		{ 		
			if(inHostIndex == 17) // Addressed NOTE #1.
				inHostIndex = 17;
			else				
				inHostIndex = 23; 	// Addressed NOTES #2 and #3 above
		}
		else // USD CURRENCY
		{	
			//Addressed NOTE #1 and #2																						for Pure JCB bin
			if((inHostIndex == 23) && (srTransRec.IITid == 7 || srTransRec.IITid == 8 || srTransRec.IITid == 9 ||  srTransRec.IITid == 4))
				inHostIndex = 23;
			else
				inHostIndex = 22;
		}
		
		#endif

		vdDebug_LogPrintf("2. inCTOS_SelectHost =[%d]", inHostIndex);
#endif

    if(inHDTRead(inHostIndex) != d_OK) 
	{
		vdDebug_LogPrintf("2.1 inCTOS_SelectHost HOST SELECTION ERR");

        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else 
	{

		//for testing
		//inHDTUpdateHostCurr(inHostIndex, strCST.inCurrencyIndex);
	

        srTransRec.HDTid = inHostIndex;
       // inCurrencyIdx = strHDT.inCurrencyIdx; -  orig code


		#ifdef CBB_FIN_ROUTING

		
#ifdef MPUPIOnUsFlag	
		vdDebug_LogPrintf("3. inCTOS_SelectHost srTransRec.CDTid =[%d] >> fGetMPUPIOnUs[%d]", srTransRec.CDTid, fGetMPUPIOnUs());		
#else
		vdDebug_LogPrintf("3. inCTOS_SelectHost srTransRec.CDTid =[%d] >> fGetMPUPIOnUs[%d]", srTransRec.CDTid);
#endif
				//fin-mmk				mpu-bpc 				installment
		if(inHostIndex == 23 || inHostIndex == 17 || srTransRec.fIsInstallment == TRUE)
		{
			if(srTransRec.CDTid == 58 || srTransRec.CDTid == 62 || srTransRec.CDTid == 63 || srTransRec.CDTid == 64 || srTransRec.CDTid == 71)	// ADDRESSED NOTE #4
				inCurrencyIdx = strHDT.inCurrencyIdx;
			else	
		 		inCurrencyIdx = 1;
		}
		else			
		 	 inCurrencyIdx = strHDT.inCurrencyIdx;
		
		#endif
		
		vdDebug_LogPrintf("3.1 inCTOS_SelectHost inCurrencyIdx =[%d]", inCurrencyIdx);

	
        if ((strcmp(szTemp, "2") == 0) && (strcmp(strCST.szCurCode, "104") == 0) && (inHostIndex == 18 || inHostIndex == 19 || inHostIndex == 21) && (srTransRec.byTransType == SALE)) 
		{
            CTOS_LCDTClearDisplay();
            vdDispErrMsg("CURRENCY DOESN'T MATCH");
            return d_NO;
        }


        if (inCSTRead(inCurrencyIdx) != d_OK) 
		{
            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }


        if (inCPTRead(inHostIndex) != d_OK) 
		{
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

		vdDebug_LogPrintf("END inCTOS_SelectHost =[%d][%d][%d][%d]", srTransRec.HDTid, strHDT.inHostIndex, inHostIndex, inCurrencyIdx);
		

        return (d_OK);
    }
}

int inGetCBBHostIndex(void) {
    int inHostIndex = 0;

#if 1
	inHostIndex = inCTOS_SelectLOGONHostSetting();
#else //original 
    inHostIndex = inCTOS_SelectHostSetting();
#endif

    //	inHostIndex = get_env_int("#CBBHIDX");
    //        inHostIndex = inHDTCheckByHostName("CREDIT");
    //	inHostIndex = 1;

    return inHostIndex;
}

int inCTOS_SelectCBBHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;
    char szAPName[25];
    int inAPPID, i, inNumOfHDTRecord;

    inHostIndex = inGetCBBHostIndex();

    if ((srTransRec.byTransType == EFTSEC_TMK) || (srTransRec.byTransType == EFTSEC_TWK) || (srTransRec.byTransType == EFTSEC_TMK_RSA) || (srTransRec.byTransType == EFTSEC_TWK_RSA)) {
        inHDTRead(strEFT[0].inHDTid);
        inHostIndex = strHDT.inHostIndex;
    }

    vdDebug_LogPrintf("inCTOS_SelectCBBHost =[%d]", inHostIndex);
	if(inHostIndex == -1)
		return d_NO;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTRead(inCurrencyIdx) != d_OK) {
            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

        if (inCPTRead(inHostIndex) != d_OK) {
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

        return (d_OK);
    }
}

int inGetCUPHostIndex(void) {
    int inHostIndex = 0;

    inHostIndex = get_env_int("#CUPHIDX");
    //inHostIndex = 19;//thandar_make it configureable for MPU UPI or UPU Finexus

    return inHostIndex;
}

int inCTOS_SelectCUPHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;
    char szAPName[25];
    int inAPPID, i, inNumOfHDTRecord;

    inHostIndex = inGetCUPHostIndex();


    if ((srTransRec.byTransType == EFTSEC_TMK) || (srTransRec.byTransType == EFTSEC_TWK) || (srTransRec.byTransType == EFTSEC_TMK_RSA) || (srTransRec.byTransType == EFTSEC_TWK_RSA)) {
        inHDTRead(strEFT[0].inHDTid);
        inHostIndex = strHDT.inHostIndex;
    }

    vdDebug_LogPrintf("inCTOS_SelectCUPHost =[%d]", inHostIndex);

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inCSTRead(inCurrencyIdx) != d_OK) {
            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

        if (inCPTRead(inHostIndex) != d_OK) {
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

        return (d_OK);
    }
}

int inGetJCBHostIndex(void) {
    int inHostIndex = 0;

    inHostIndex = get_env_int("#JCBHIDX");
    inHostIndex = 18;

    return inHostIndex;
}

int inCTOS_SelectJCBHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;
    char szAPName[25];
    int inAPPID, i, inNumOfHDTRecord;

    inHostIndex = inGetJCBHostIndex();

    if ((srTransRec.byTransType == EFTSEC_TMK) || (srTransRec.byTransType == EFTSEC_TWK) || (srTransRec.byTransType == EFTSEC_TMK_RSA) || (srTransRec.byTransType == EFTSEC_TWK_RSA)) {
        inHDTRead(strEFT[0].inHDTid);
        inHostIndex = strHDT.inHostIndex;
    }

    vdDebug_LogPrintf("inCTOS_SelectJCBHost =inHostIndex[%d]inCurrencyIdx[%d]", inHostIndex, strHDT.inCurrencyIdx);

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

		vdDebug_LogPrintf("inCTOS_SelectJCBHost =inCurrencyIdx[%d]", inCurrencyIdx);

        if (inCSTRead(inCurrencyIdx) != d_OK) {
            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

        if (inCPTRead(inHostIndex) != d_OK) {
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }
		vdDebug_LogPrintf("inCTOS_SelectJCBHost END");

        return (d_OK);
    }
}

int inGetMPUHostIndex(void) {
    int inHostIndex = 0;

    inHostIndex = get_env_int("#MPUHIDX");
    inHostIndex = MPU_DEBIT_HOST_IDX;

    return inHostIndex;
}

int inCTOS_SelectMPUHost(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;
    char szAPName[25];
    int inAPPID, i, inNumOfHDTRecord;
    int  fSTLSIGNONANY = get_env_int("STLSIGNONANYHI"); 	//SIGNON ANY HOST after Settlement transction.
    int  fSTLSIGNONALL = get_env_int("STLSIGNONALLHI"); 	//SIGNON ALL HOST after Settlement transaction.

    vdDebug_LogPrintf("inCTOS_SelectMPUHost > TransType = [%d],  fGetSIGNON() = [%d],  fSTLSIGNONANY=[%d], fSTLSIGNONALL=[%d] ", 
		srTransRec.byTransType, fGetSIGNON(),fSTLSIGNONANY, fSTLSIGNONALL);


#ifdef CBB_FIN_ROUTING

		BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	  	BYTE	x = 1;
	  	BYTE key;	
		char szHeaderString[50] = "SELECT A HOST";
		char szInjectKeyMenu[1024] = {0};	
		char szInjectKeyMenu2[1024+1];
		int inHDTCount = 0;	
	    char szTemp[10+1];
	    char szTermsMenu[1024+1];
		int inRet;
		
	//	CTOS_LCDTPrintXY(1, 1, "INJECT MK");

	vdDebug_LogPrintf("inCTOS_SelectMPUHost CBB_FIN_ROUTING");

		// add here host selection sub menu, refer to vdCTOSS_InjectMKKey()	
		if(srTransRec.byTransType == MPU_SIGNON)
		{
			#if 0
			if(fGetSIGNON() == TRUE)
				inHostIndex = inCTOS_SelectHostSettingSignON();	
			else
				inHostIndex = inCTOS_SelectHostSettingSignON_F2(); 
			#else
				//passed value of HDTID after Settlement
				if(fSTLSIGNONANY > 0)
				{
					vdDebug_LogPrintf("inCTOS_SelectMPUHost CBB_FIN_ROUTING-1");
				
					inHostIndex = fSTLSIGNONANY; // passed value from inCTOS_SettlementFlowProcess (ANY/ Single) settlement.  
					 put_env_int("STLSIGNONANYHI", 0);
				}
				else if(fSTLSIGNONALL > 0)			//For Settle ALL
				{
					vdDebug_LogPrintf("inCTOS_SelectMPUHost CBB_FIN_ROUTING-2");
				
					put_env_int("STLSIGNONALLHI", 0);
					CTOS_LCDTClearDisplay();
										
					CTOS_LCDTPrintXY(1, 8, "Processing...");
					CTOS_Delay(500);

					CTOS_LCDTClearDisplay();
					inRet = inCTOS_SIGNONAllHosts();
					
					return d_NO;
				}
				else //SIGN SINGLE HOST
				{
					vdDebug_LogPrintf("inCTOS_SelectMPUHost CBB_FIN_ROUTING-3");

						inHostIndex = inCTOS_SelectHostSettingSignON_F2();

					vdDebug_LogPrintf("inCTOS_SelectMPUHost Key [%d]", inHostIndex);

					if(inHostIndex < 1)
					{
						vdDisplayErrorMsg(1, 8, "User Cancel");
						return d_NO;
					}
					else if(inHostIndex == 99) // means SIGNON ALL host
					{
					
						vdDebug_LogPrintf("inCTOS_SelectMPUHost Key inHostIndex == 99 ");
							
						CTOS_LCDTClearDisplay();						
						
						CTOS_LCDTPrintXY(1, 8, "Processing...");
						CTOS_Delay(500);

						CTOS_LCDTClearDisplay();
						inRet = inCTOS_SIGNONAllHosts();
						
						return d_NO;
					}
					else
						CTOS_LCDTClearDisplay();
				}				
			#endif				
		}
		else
		{		
	    	inHostIndex = inGetMPUHostIndex();
		}
		
		
#else	
    inHostIndex = inGetMPUHostIndex();		
#endif


    vdDebug_LogPrintf("inCTOS_SelectMPUHost =[%d]", inHostIndex);

    if ((srTransRec.byTransType == EFTSEC_TMK) || (srTransRec.byTransType == EFTSEC_TWK) || (srTransRec.byTransType == EFTSEC_TMK_RSA) || (srTransRec.byTransType == EFTSEC_TWK_RSA)) {
        inHDTRead(strEFT[0].inHDTid);
        inHostIndex = strHDT.inHostIndex;
    }


    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if (inHDTRead(inHostIndex) != d_OK) {
       	vdDebug_LogPrintf("inCTOS_SelectMPUHost > HOST SELECTION ERR");
		
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {

        vdDebug_LogPrintf("strHDT.inCurrencyIdx = %d", strHDT.inCurrencyIdx);
		
        srTransRec.HDTid = inHostIndex;

#if 0
	// fix issue http://118.201.48.214:8080/issues/75 #39
	if(srTransRec.byTransType == MPU_SIGNON)
	{
	        vdDebug_LogPrintf("inCTOS_SelectMPUHost >>> (srTransRec.byTransType == MPU_SIGNON)");	
		// fix for Load CST ERR
		if(inHostIndex == 17 || inHostIndex == 23)	
			inCurrencyIdx = 1;
		else if(inHostIndex == 22)
			inCurrencyIdx = 2;
		else		
	       	inCurrencyIdx = strHDT.inCurrencyIdx;
	}
	else
       	inCurrencyIdx = strHDT.inCurrencyIdx;
#else
inCurrencyIdx = strHDT.inCurrencyIdx;
#endif		

        vdDebug_LogPrintf("inCurrencyIdx = %d", inCurrencyIdx);

        if (inCSTRead(inCurrencyIdx) != d_OK) {

       	vdDebug_LogPrintf("inCTOS_SelectMPUHost > LOAD CST ERR");
			
            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

        if (inCPTRead(inHostIndex) != d_OK) {
			
			vdDebug_LogPrintf("inCTOS_SelectMPUHost > LOAD CPT ERR");
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }

		vdDebug_LogPrintf("inCTOS_SelectMPUHost > END");

        return (d_OK);
    }
}

int inCTOS_getCardCVV2(BYTE *baBuf) {
    USHORT usRet;
    USHORT usInputLen;
    USHORT usLens;
    USHORT usMinLen = 3;
    USHORT usMaxLen = 4;
    BYTE bBuf[4 + 1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];

    //CTOS_LCDTClearDisplay();
    vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);

    if (CARD_ENTRY_MANUAL == srTransRec.byEntryMode) {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof (bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr + 3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 6, bDisplayStr);

        usInputLen = 7;
    } else {
        inCTOS_DisplayCardTitle(4, 5);
        usInputLen = 7;
    }

    setLCDPrint(usInputLen, DISPLAY_POSITION_LEFT, "CVV2: ");

    while (1) {
        usRet = shCTOS_GetNum(usInputLen + 1, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 1, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL)
            return (d_EDM_USER_CANCEL);
        if (usRet >= usMinLen && usRet <= usMaxLen) {
            return (d_OK);
        } else if (0 == usRet) {
            return (d_OK);
        }

        baBuf[0] = 0x00;
    }
}

int inCTOS_GetAuthCode(void) {
    USHORT usX = 1, usY = 6;
    BYTE bShowAttr = 0x02;
    USHORT szAuthCodeLen = 6;
    BYTE baPIN[6 + 1];
    BYTE szAuthCode[6 + 1];
    BYTE bRet;

    memset(szAuthCode, 0x00, sizeof (szAuthCode));

    //CTOS_LCDTClearDisplay();
    vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    //    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ENTER AUTH CODE: ");
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ENTER APPR CODE: ");

    while (TRUE) {
        //if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
        if (1) {
            szAuthCodeLen++;
            bRet = InputStringAlphaEx2(1, 8, 0x00, 0x02, szAuthCode, &szAuthCodeLen, 0, d_INPUT_TIMEOUT);
        } else
            bRet = InputStringAlpha(1, 8, 0x00, 0x02, szAuthCode, &szAuthCodeLen, 0, d_INPUT_TIMEOUT);

        if (bRet == d_KBD_CANCEL) {
            //CTOS_LCDTClearDisplay();
            vduiClearBelow(2);
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        if (strlen(szAuthCode) >= 2) {
            strcpy(srTransRec.szAuthCode, szAuthCode);
            break;
        } else {
            memset(szAuthCode, 0x00, sizeof (szAuthCode));
            szAuthCodeLen = 6;
        }
    }

    return ST_SUCCESS;
}

int inCTOS_GetTransRRN(void) {
    USHORT usX = 1, usY = 6;
    BYTE bShowAttr = 0x02;
    USHORT szRRNLen = 12;
    BYTE szRRNCode[12 + 1];
    BYTE bRet;

    memset(szRRNCode, 0x00, sizeof (szRRNCode));

    //CTOS_LCDTClearDisplay();
    vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ENTER RRN: ");

    while (TRUE) {
        //if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
        if (1) {
            szRRNLen++;
            bRet = InputStringAlphaEx2(1, 8, 0x00, 0x02, szRRNCode, &szRRNLen, 0, d_INPUT_TIMEOUT);
        } else
            bRet = InputStringAlpha(1, 8, 0x00, 0x02, szRRNCode, &szRRNLen, 0, d_INPUT_TIMEOUT);

        if (bRet == d_KBD_CANCEL) {
            //CTOS_LCDTClearDisplay();
            vduiClearBelow(2);
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        if (strlen(szRRNCode) >= 2) {
            strcpy(srTransRec.szRRN, szRRNCode);
            break;
        } else {
            memset(szRRNCode, 0x00, sizeof (szRRNCode));
            szRRNLen = 6;
        }
    }

    return ST_SUCCESS;
}

int inCTOS_GetTransOrgDate(void) {
    USHORT usX = 1, usY = 6;
    BYTE bShowAttr = 0x02;
    USHORT szDateLen = 6;
    BYTE szMMDDYY[6 + 1];
    BYTE bRet;

    memset(szMMDDYY, 0x00, sizeof (szMMDDYY));

    //CTOS_LCDTClearDisplay();
    vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "ORIG DATE(MMDDYY): ");

    while (TRUE) {
        //if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
        if (1) {
            szDateLen++;
            bRet = InputStringAlphaEx2(1, 8, 0x00, 0x02, szMMDDYY, &szDateLen, 0, d_INPUT_TIMEOUT);
        } else
            bRet = InputStringAlpha(1, 8, 0x00, 0x02, szMMDDYY, &szDateLen, 0, d_INPUT_TIMEOUT);

        if (bRet == d_KBD_CANCEL) {
            //CTOS_LCDTClearDisplay();
            vduiClearBelow(2);
            vdDisplayErrorMsg(1, 8, "USER CANCEL");
            return (d_EDM_USER_CANCEL);
        }

        if (strlen(szMMDDYY) == 6) {
            wub_str_2_hex(szMMDDYY, srTransRec.szOrgDate, 4);
            //strcpy(srTransRec.szOrgDate, szMMDDYY);
            break;
        } else {
            memset(szMMDDYY, 0x00, sizeof (szMMDDYY));
            szDateLen = 6;
        }
    }

    return ST_SUCCESS;
}

int inCTOS_GetMPUCardFields(void) {
    USHORT EMVtagLen;
    BYTE EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;
    int usResult;

    DebugAddSTR("inCTOS_GetCardFields", "Processing...", 20);

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

SWIPE_AGAIN:

    if (d_OK != inCTOS_ValidFirstIdleKey()) {
        //CTOS_LCDTClearDisplay();
        //vduiClearBelow(2);
        vdDispTransTitle(srTransRec.byTransType);

        inCTOS_DisplayIdleBMP();
    }
    // patrick ECR 20140516 start
    if (strTCT.fECR) // tct
    {
        if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0) {
            char szDisplayBuf[30];
            BYTE szTemp1[30 + 1];

            CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
            memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
            wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
            //gcitra-0728
            //sprintf(szDisplayBuf, "%s %10lu.%02lu", strCST.szCurSymbol,atol(szTemp1)/100, atol(szTemp1)%100);
            //CTOS_LCDTPrintXY(1, 8, szDisplayBuf);	
            CTOS_LCDTPrintXY(1, 8, strCST.szCurSymbol);
            memset(szDisplayBuf, 0x00, sizeof (szDisplayBuf));
            //format amount 10+2
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szDisplayBuf);
            //sprintf(szDisplayBuf,"%10.0f.%02.0f",(atof(szTemp1)/100), (atof(szTemp1)%100));
            //sprintf(szDisplayBuf, "%lu.%02lu", atol(szTemp1)/100, atol(szTemp1)%100);
            setLCDPrint(8, DISPLAY_POSITION_RIGHT, szDisplayBuf);
            //gcitra-0728

        }
    }
    // patrick ECR 20140516 end
    CTOS_TimeOutSet(TIMER_ID_1, GET_CARD_DATA_TIMEOUT_VALUE);

    while (1) {
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
            return READ_CARD_TIMEOUT;

        CTOS_KBDInKey(&byKeyBuf);

        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey())) {
            if (byKeyBuf == d_KBD_CANCEL) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
            if (d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN)) {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }
            vdSetFirstIdleKey(0x00);
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (bySC_status & d_MK_SC_PRESENT) {
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            vdDebug_LogPrintf("--MPU flow----");
            if (d_OK != shCTOS_MPUAppSelectedProcess()) {
                vdDebug_LogPrintf("--MPU Read Failed---- inFallbackToMSR?[%d]", inFallbackToMSR);
                return READ_APPLET_ERR;
                if (inFallbackToMSR == SUCCESS) {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                } else {
                    //vdSetErrorMessage("Get Card Fail C");
                    return READ_APPLET_ERR;
                }
            }
            vdDebug_LogPrintf("--MPU Read succ----");
            //if (srTransRec.byTransType == REFUND)
            vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);
            vdSetMPUCard(TRUE);

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0) {
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

            if (d_OK != inCTOS_CheckEMVFallback()) {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");

                goto SWIPE_AGAIN;

            }

            break;

        }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //Fix for Track2 Len < 35
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if (byMSR_status == d_OK)
            //Fix for Track2 Len < 35
        {
            usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);
            if (usResult != d_OK) {
                CTOS_KBDBufFlush();
                vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                return USER_ABORT;
            }

            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if (d_OK != inCTOS_CheckEMVFallback()) {
#if 0
                /*Void should allow swipe chip card!!!???*/
                if (VS_TRUE == fGetMPUTrans() && VOID == srTransRec.byTransType)
                    break;
                else
#endif
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                    goto SWIPE_AGAIN;
                }
            }

            break;
        }

    }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        EMVtagLen = 0;
        if (EMVtagLen > 0) {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        } else {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    } else if (srTransRec.byEntryMode == CARD_ENTRY_EASY_ICC) {
        if (0 == strcmp("MPU", strIIT.szIssuerLabel)) {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        } else // Pad MPU in front
        {
            strcpy(srTransRec.szCardLable, "MPU-");
            if (0 == strcmp("UNIONPAY", strIIT.szIssuerLabel))
                strcat(srTransRec.szCardLable, "UPI");
            else
                strcat(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    } else {
        strcat(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
    ;
}

#ifdef DISCOUNT_FEATURE
// for Discount function
int inCTOS_WaveGetCardFieldsDisc(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
	ULONG ulAPRtn;
	BYTE temp[64],temp1[64];
	char szTotalAmount[AMT_ASC_SIZE+1];
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[4+1],szCurrCode[4+1];
        char szTemp[12+1];

	/*BYTE szTotalXAmount2 = 0;				
	int inTotalAmount = 0;		
	char szFixedAmount[AMT_ASC_SIZE+1];
	*/

 	vdDebug_LogPrintf("inCTOS_WaveGetCardFieldsDisc..byExtReadCard=[%d].......",strTCT.byExtReadCard);
        
    vdDebug_LogPrintf("ECR [%d]", fGetECRTransactionFlg());

	if(strTCT.byExtReadCard == 1)
	{
		usResult = inCTOS_WaveGetCardFieldsExternal();
		return usResult;
	}
    
    switch (srTransRec.HDTid) {
        case 10://alipay
        case 12: //we chat
        case 16://qq
            return d_OK;
            break;
    }
    
     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
 
SWIPE_AGAIN:
EntryOfStartTrans:

	memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szCurrCode,0x00,sizeof(szCurrCode));
	memset(temp,0x00,sizeof(temp));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));
	
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	vdDebug_LogPrintf("szTotalAmount[%s].strTCT.szCTLSLimit=[%s]........",szTotalAmount,strTCT.szCTLSLimit);

	
	if (NO_CTLS == inCTOSS_GetCtlsMode() || atol(szTotalAmount) > atol(strTCT.szCTLSLimit))
	{
//		CTOS_LCDTPrintXY(1, 3, "Insert/Swipe Card");
//		CTOS_Beep();
		CTOS_Delay(500);
		return (inCTOS_GetCardFields());
	}
	CTOS_LCDTClearDisplay();
	//vduiClearBelow(2);//this function will delay the transaction speed
	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR())
	{

		vdDebug_LogPrintf("XXXXXXXXXXXX");
	
		//CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);
		CTOS_LCDTPrintXY(1, 3, "    Present Card   ");


		wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);		
			
		memset(szTemp, 0x00, sizeof(szTemp));

		
		sprintf(szTemp, "%012.0f", atof(szTotalAmount));
		memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
		strcpy(szTotalAmount, szTemp);
		//format amount 10+2
		memset(temp1,0x00,sizeof(temp1));
		if(memcmp(strCST.szCurSymbol, "MMK", 3) == 0){
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, temp1);
		} else {
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
		}


		//		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
		sprintf(temp," Amount: %s",temp1);
		//sprintf(temp," Amount: %10.0f.%02.0f",(atof(szTotalAmount)/100), (atof(szTotalAmount)%100));
		//sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
		CTOS_LCDTPrintXY(1, 4, temp);
	}
	else{

	vdDebug_LogPrintf("YYYYYYYYYYYY");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);		
		
		vdDebug_LogPrintf("ZZZZZZZZZZZZ [%s]", szTotalAmount);
		}

	if (srTransRec.byTransType == REFUND){
		szTransType[0] = 0x20;	

	}

	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
	strcpy(szCurrCode, szCatgCode);

	//if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR())
	{
		vdDebug_LogPrintf("AAAAAAAAAAAA");
	
		ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
	//	ulAPRtn = EMVCL_InitTransaction(atol(szTotalAmount));
		if(ulAPRtn != d_EMVCL_NO_ERROR)
		{
			vdSetErrorMessage("CTLS InitTrans Fail!");
			return d_NO;
		}
	}

	/*Clear KB bufer*/
	CTOS_KBDBufFlush();
	byKeyBuf = 0x00;
 
    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    
    while (1)
    {
    
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf); //input  
        
        if(strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)){
            CTOS_KBDBufFlush();
            vdSetFirstIdleKey(0x00);
            byKeyBuf = 0x00;
        }
        
        vdDebug_LogPrintf("byKeyBuf [%d], inCTOS_ValidFirstIdleKey [%d]", byKeyBuf, inCTOS_ValidFirstIdleKey());


		// For manual Entry - 
        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();
                
//                CTOS_KBDGet(&byKeyBuf);
                
                
                
                if(byKeyBuf == d_KBD_CANCEL){
                    CTOS_LCDTClearDisplay();
                    CTOS_KBDBufFlush();
//                    vdSetErrorMessage("TXN CANCELLED");
                    return USER_ABORT;
                }

            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();
            
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }
            
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
			vdSetFirstIdleKey(0x00);
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
                        
            //@@IBR ADD 20170202 no manual entry
            if(strCDT.fManEntry == FALSE){
                vduiClearBelow(8);
                setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);
                return USER_ABORT;
            }
            //@@IBR FINISH ADD 20170202 no manual entry
            
            break;
        }

		// For EMV trans
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

            
            vdDebug_LogPrintf("--EMV flow----" );

			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if(srTransRec.byTransType == REFUND)
			{
				vduiClearBelow(7);
                vduiClearBelow(8);
				setLCDPrint(7, DISPLAY_POSITION_CENTER, "PLEASE REMOVE CARD");
                setLCDPrint(8, DISPLAY_POSITION_CENTER, "AND DO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);	
				goto SWIPE_AGAIN;
			}

            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
			
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
			if (srTransRec.byTransType == REFUND)
				vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

			/*for MPU Application*/
			if (VS_TRUE == fGetMPUTrans() && VS_TRUE == fGetMPUCard())
				vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

			if (VS_TRUE == fGetMPUTrans() && 
				(srTransRec.byTransType == VOID ||
				srTransRec.byTransType == MPU_VOID_PREAUTH ||
				srTransRec.byTransType == MPU_VOID_PREAUTH_COMP ||
                                srTransRec.byTransType == VOID_PREAUTH)
				)
				vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

			
			vdDebug_LogPrintf("--EMV Read succ---- srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);
			
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
         	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
				if (1 == chGetIdleEventSC_MSR())
					return USER_ABORT;
                
                goto SWIPE_AGAIN;

             }
                     
             break;
         
         }
        
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
		if(byMSR_status == d_OK )//Fix for Track2 Len < 35
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

			usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 
			if (usResult != d_OK)
			{
                 CTOS_KBDBufFlush();
				 vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                 return USER_ABORT;
             }
		
            if (d_OK != inCTOS_LoadCDTIndex())
			{
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
			}
            
            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
				if (1 == chGetIdleEventSC_MSR())
					return USER_ABORT;
                
                goto SWIPE_AGAIN;

             }
                
            break;
        }

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
		{				
			ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

		// V3 contactless reader
//		EMVCL_StopIdleLEDBehavior(NULL);
//		EMVCL_SetLED(0x0F, 0x08);

// patrick test code 20141230 start
#ifndef d_EMVCL_RC_SEE_PHONE
#define d_EMVCL_RC_SEE_PHONE		0xA00000AF // AMEX
#endif
#define d_EMVCL_RC_SEE_PHONE2		0xA00000E4
// patrick test code 20141230 end		

			if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
			{
				vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
			}
			else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE || d_EMVCL_RC_SEE_PHONE2)
			{
				//CTOS_LCDTClearDisplay();
				vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
				CTOS_Delay(3000);
				goto EntryOfStartTrans;
			}			
			else if(ulAPRtn == d_EMVCL_TX_CANCEL)
			{
				vdDisplayErrorMsg(1, 8, "USER CANCEL");
				return USER_ABORT;
			}
			else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
			{
				if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
	        		inCTOSS_CLMCancelTransaction();

				CTOS_Beep();
			    CTOS_Delay(50);
			    CTOS_Beep();
				CTOS_Delay(50);
				vdDisplayErrorMsg(1, 8, "TIMEOUT");
				return USER_ABORT;
			}
			else if(ulAPRtn != d_EMVCL_PENDING)
			{
				CTOS_Beep();
			    CTOS_Delay(50);
			    CTOS_Beep();
				CTOS_Delay(50);
			    CTOS_Beep();
				vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
				break;
			}
		}
		else
		{
			memset(szTransType,0x00,sizeof(szTransType));
			strcpy(szOtherAmt,"000000000000");
			if (srTransRec.byTransType == REFUND)
				strcpy(szTransType,"20");
			else
				strcpy(szTransType,"00");
                        
            vdDebug_LogPrintf("Ctls V3 Trans szTotalAmount=[%s], szOtherAmt=[%s]", szTotalAmount, szOtherAmt);

			
			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if (srTransRec.byTransType == REFUND){
				//CTOS_LCDTClearDisplay();		
				CTOS_LCDTPrintXY(1, 8, "PLEASE ENTER CARD NO.");
				CTOS_Beep();
				CTOS_Delay(1000);
			}
			else
			{   // original code
				ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode,&stRCDataEx);
				
				CTOS_Beep();
			    CTOS_Delay(50);
			    CTOS_Beep();
				CTOS_Delay(50);
			    CTOS_Beep();
				
				if(ulAPRtn == d_OK)
				{
					vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
					ulAPRtn = d_EMVCL_RC_DATA;
					break;
				}                    
	            if(ulAPRtn == 155){
	                vduiClearBelow(2);
	                vdSetErrorMessage("CARD NOT SUPPORTED");
	                return USER_ABORT;
	            }
				if (ulAPRtn == d_NO)
					return USER_ABORT;

				if (ulAPRtn == d_MORE_PROCESSING)
					return (inCTOS_GetCardFields());
			}
			
		}

	}

	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
	{
		if(ulAPRtn != d_EMVCL_RC_DATA)
		{
			vdCTOSS_WaveCheckRtCode(ulAPRtn);
			if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

			return d_NO;
		}

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
		{
			if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
			{
				inCTOSS_CLMCancelTransaction();
				return d_NO;
			}
		}
		else
		{
			if (d_OK != inCTOSS_V3AnalyzeTransaction(&stRCDataEx))
				return d_NO;
		}

		//Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();
			
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        	inCTOSS_CLMCancelTransaction();

	}

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;
    
    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
}

// for Discount function
int inCTOS_WaveGetCardFieldsDiscPerc(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
	ULONG ulAPRtn;
	BYTE temp[64],temp1[64];
	char szTotalAmount[AMT_ASC_SIZE+1];
	EMVCL_RC_DATA_EX stRCDataEx;
	BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[4+1],szCurrCode[4+1];
        char szTemp[12+1];

	/*BYTE szTotalXAmount2 = 0;				
	int inTotalAmount = 0;		
	char szFixedAmount[AMT_ASC_SIZE+1];
	*/

 	vdDebug_LogPrintf("inCTOS_WaveGetCardFieldsDisc..byExtReadCard=[%d].......",strTCT.byExtReadCard);
        
    vdDebug_LogPrintf("ECR [%d]", fGetECRTransactionFlg());

	if(strTCT.byExtReadCard == 1)
	{
		usResult = inCTOS_WaveGetCardFieldsExternal();
		return usResult;
	}
    
    switch (srTransRec.HDTid) {
        case 10://alipay
        case 12: //we chat
        case 16://qq
            return d_OK;
            break;
    }
    
     if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
 
SWIPE_AGAIN:
EntryOfStartTrans:

	memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
	memset(szOtherAmt,0x00,sizeof(szOtherAmt));
	memset(szTransType,0x00,sizeof(szTransType));
	memset(szCatgCode,0x00,sizeof(szCatgCode));
	memset(szCurrCode,0x00,sizeof(szCurrCode));
	memset(temp,0x00,sizeof(temp));
	memset(szTotalAmount,0x00,sizeof(szTotalAmount));
	
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	vdDebug_LogPrintf("szTotalAmount[%s].strTCT.szCTLSLimit=[%s]........",szTotalAmount,strTCT.szCTLSLimit);

	
	if (NO_CTLS == inCTOSS_GetCtlsMode() || atol(szTotalAmount) > atol(strTCT.szCTLSLimit))
	{
//		CTOS_LCDTPrintXY(1, 3, "Insert/Swipe Card");
//		CTOS_Beep();
		CTOS_Delay(500);
		return (inCTOS_GetCardFields());
	}
	CTOS_LCDTClearDisplay();
	//vduiClearBelow(2);//this function will delay the transaction speed
	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR())
	{

		vdDebug_LogPrintf("XXXXXXXXXXXX");
	
		//CTOS_LCDTClearDisplay();
		vdDispTransTitle(srTransRec.byTransType);
		CTOS_LCDTPrintXY(1, 3, "    Present Card   ");


		wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);		
			
		memset(szTemp, 0x00, sizeof(szTemp));

		
		sprintf(szTemp, "%012.0f", atof(szTotalAmount));
		memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
		strcpy(szTotalAmount, szTemp);
		//format amount 10+2
		memset(temp1,0x00,sizeof(temp1));
		if(memcmp(strCST.szCurSymbol, "MMK", 3) == 0){
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, temp1);
		} else {
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
		}


		//		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
		sprintf(temp," Amount: %s",temp1);
		//sprintf(temp," Amount: %10.0f.%02.0f",(atof(szTotalAmount)/100), (atof(szTotalAmount)%100));
		//sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
		CTOS_LCDTPrintXY(1, 4, temp);
	}
	else{

	vdDebug_LogPrintf("YYYYYYYYYYYY");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);		
		
		vdDebug_LogPrintf("ZZZZZZZZZZZZ [%s]", szTotalAmount);
		}

	if (srTransRec.byTransType == REFUND){
		szTransType[0] = 0x20;	

	}

	sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
	strcpy(szCurrCode, szCatgCode);

	//if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR())
	{
		vdDebug_LogPrintf("AAAAAAAAAAAA");
	
		ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
	//	ulAPRtn = EMVCL_InitTransaction(atol(szTotalAmount));
		if(ulAPRtn != d_EMVCL_NO_ERROR)
		{
			vdSetErrorMessage("CTLS InitTrans Fail!");
			return d_NO;
		}
	}

	/*Clear KB bufer*/
	CTOS_KBDBufFlush();
	byKeyBuf = 0x00;
 
    CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    
    while (1)
    {
    
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf); //input  
        
        if(strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)){
            CTOS_KBDBufFlush();
            vdSetFirstIdleKey(0x00);
            byKeyBuf = 0x00;
        }
        
        vdDebug_LogPrintf("byKeyBuf [%d], inCTOS_ValidFirstIdleKey [%d]", byKeyBuf, inCTOS_ValidFirstIdleKey());


		// For manual Entry - 
        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey()))
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();
                
//                CTOS_KBDGet(&byKeyBuf);
                
                
                
                if(byKeyBuf == d_KBD_CANCEL){
                    CTOS_LCDTClearDisplay();
                    CTOS_KBDBufFlush();
//                    vdSetErrorMessage("TXN CANCELLED");
                    return USER_ABORT;
                }

            memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
            if(d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();
            
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
            {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush ();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }
            
            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
			vdSetFirstIdleKey(0x00);
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
                        
            //@@IBR ADD 20170202 no manual entry
            if(strCDT.fManEntry == FALSE){
                vduiClearBelow(8);
                setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);
                return USER_ABORT;
            }
            //@@IBR FINISH ADD 20170202 no manual entry
            
            break;
        }

		// For EMV trans
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

            
            vdDebug_LogPrintf("--EMV flow----" );

			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if(srTransRec.byTransType == REFUND)
			{
				vduiClearBelow(7);
                vduiClearBelow(8);
				setLCDPrint(7, DISPLAY_POSITION_CENTER, "PLEASE REMOVE CARD");
                setLCDPrint(8, DISPLAY_POSITION_CENTER, "AND DO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);	
				goto SWIPE_AGAIN;
			}

            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
			
            if (d_OK != inCTOS_EMVCardReadProcess ())
            {
                if(inFallbackToMSR == SUCCESS)
                {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                }
                else
                {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----" );
			if (srTransRec.byTransType == REFUND)
				vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

			/*for MPU Application*/
			if (VS_TRUE == fGetMPUTrans() && VS_TRUE == fGetMPUCard())
				vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

			if (VS_TRUE == fGetMPUTrans() && 
				(srTransRec.byTransType == VOID ||
				srTransRec.byTransType == MPU_VOID_PREAUTH ||
				srTransRec.byTransType == MPU_VOID_PREAUTH_COMP ||
                                srTransRec.byTransType == VOID_PREAUTH)
				)
				vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

			
			vdDebug_LogPrintf("--EMV Read succ---- srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);
			
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            
            break;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
         {
         	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

             if (d_OK != inCTOS_LoadCDTIndex())
             {
                 CTOS_KBDBufFlush();
                 //vdSetErrorMessage("Get Card Fail");
                 return USER_ABORT;
             }

             if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
				if (1 == chGetIdleEventSC_MSR())
					return USER_ABORT;
                
                goto SWIPE_AGAIN;

             }
                     
             break;
         
         }
        
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
		if(byMSR_status == d_OK )//Fix for Track2 Len < 35
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

			usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 
			if (usResult != d_OK)
			{
                 CTOS_KBDBufFlush();
				 vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                 return USER_ABORT;
             }
		
            if (d_OK != inCTOS_LoadCDTIndex())
			{
                 CTOS_KBDBufFlush();
                 return USER_ABORT;
			}
            
            if(d_OK != inCTOS_CheckEMVFallback())
             {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
				if (1 == chGetIdleEventSC_MSR())
					return USER_ABORT;
                
                goto SWIPE_AGAIN;

             }
                
            break;
        }

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
		{				
			ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

		// V3 contactless reader
//		EMVCL_StopIdleLEDBehavior(NULL);
//		EMVCL_SetLED(0x0F, 0x08);

// patrick test code 20141230 start
#ifndef d_EMVCL_RC_SEE_PHONE
#define d_EMVCL_RC_SEE_PHONE		0xA00000AF // AMEX
#endif
#define d_EMVCL_RC_SEE_PHONE2		0xA00000E4
// patrick test code 20141230 end		

			if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
			{
				vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
			}
			else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE || d_EMVCL_RC_SEE_PHONE2)
			{
				//CTOS_LCDTClearDisplay();
				vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
				CTOS_Delay(3000);
				goto EntryOfStartTrans;
			}			
			else if(ulAPRtn == d_EMVCL_TX_CANCEL)
			{
				vdDisplayErrorMsg(1, 8, "USER CANCEL");
				return USER_ABORT;
			}
			else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
			{
				if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
	        		inCTOSS_CLMCancelTransaction();

				CTOS_Beep();
			    CTOS_Delay(50);
			    CTOS_Beep();
				CTOS_Delay(50);
				vdDisplayErrorMsg(1, 8, "TIMEOUT");
				return USER_ABORT;
			}
			else if(ulAPRtn != d_EMVCL_PENDING)
			{
				CTOS_Beep();
			    CTOS_Delay(50);
			    CTOS_Beep();
				CTOS_Delay(50);
			    CTOS_Beep();
				vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
				break;
			}
		}
		else
		{
			memset(szTransType,0x00,sizeof(szTransType));
			strcpy(szOtherAmt,"000000000000");
			if (srTransRec.byTransType == REFUND)
				strcpy(szTransType,"20");
			else
				strcpy(szTransType,"00");
                        
            vdDebug_LogPrintf("Ctls V3 Trans szTotalAmount=[%s], szOtherAmt=[%s]", szTotalAmount, szOtherAmt);

			
			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if (srTransRec.byTransType == REFUND){
				//CTOS_LCDTClearDisplay();		
				CTOS_LCDTPrintXY(1, 8, "PLEASE ENTER CARD NO.");
				CTOS_Beep();
				CTOS_Delay(1000);
			}
			else
			{   // original code
				ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode,&stRCDataEx);
				
				CTOS_Beep();
			    CTOS_Delay(50);
			    CTOS_Beep();
				CTOS_Delay(50);
			    CTOS_Beep();
				
				if(ulAPRtn == d_OK)
				{
					vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
					ulAPRtn = d_EMVCL_RC_DATA;
					break;
				}                    
	            if(ulAPRtn == 155){
	                vduiClearBelow(2);
	                vdSetErrorMessage("CARD NOT SUPPORTED");
	                return USER_ABORT;
	            }
				if (ulAPRtn == d_NO)
					return USER_ABORT;

				if (ulAPRtn == d_MORE_PROCESSING)
					return (inCTOS_GetCardFields());
			}
			
		}

	}

	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
	{
		if(ulAPRtn != d_EMVCL_RC_DATA)
		{
			vdCTOSS_WaveCheckRtCode(ulAPRtn);
			if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();

			return d_NO;
		}

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
		{
			if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
			{
				inCTOSS_CLMCancelTransaction();
				return d_NO;
			}
		}
		else
		{
			if (d_OK != inCTOSS_V3AnalyzeTransaction(&stRCDataEx))
				return d_NO;
		}

		//Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
        	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        		inCTOSS_CLMCancelTransaction();
			
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        	inCTOSS_CLMCancelTransaction();

	}

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {    
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;
    
    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
}

#endif

int inCTOS_GetCVV2() {
    BYTE szCVV2Code[CVV2_SIZE + 1];
    int inResult = FAIL;
    BYTE key;
    short shCount = 0;
    DebugAddSTR("inCTOS_GetCVV2", "Processing...", 20);

    memset(srTransRec.szCVV2, 0x00, sizeof (srTransRec.szCVV2));
    memset(szCVV2Code, 0x00, sizeof (szCVV2Code));

    if (CVV2_NONE == strCDT.inCVV_II) {
        return d_OK;
    }

    while (shCount < 3) {

        if (((CVV2_MANUAL == strCDT.inCVV_II) && (CARD_ENTRY_MANUAL == srTransRec.byEntryMode))
                || ((CVV2_MSR == strCDT.inCVV_II) && ((CARD_ENTRY_MSR == srTransRec.byEntryMode)))
                || ((CVV2_MANUAL_MSR == strCDT.inCVV_II) && ((CARD_ENTRY_MANUAL == srTransRec.byEntryMode) || (CARD_ENTRY_MSR == srTransRec.byEntryMode)))
                || ((CVV2_MANUAL_MSR_CHIP == strCDT.inCVV_II) && ((CARD_ENTRY_MANUAL == srTransRec.byEntryMode) || (CARD_ENTRY_MSR == srTransRec.byEntryMode) || (CARD_ENTRY_ICC == srTransRec.byEntryMode)))) {
            CTOS_KBDBufFlush();

            inResult = inCTOS_getCardCVV2(szCVV2Code);
            if (d_OK == inResult) {
                vdDebug_LogPrintf("inCTOS_getCardCVV2[%s]", szCVV2Code);
                strcpy(srTransRec.szCVV2, szCVV2Code);
                if (strlen(srTransRec.szCVV2) <= 0) {
                    if (1 == get_env_int("#SKIPCVV"))
                        return d_OK;
                    else {
                        vdDisplayErrorMsg(1, 8, "CAN NOT SKIP CVV");
                        shCount++;
                    }
                } else
                    return d_OK;
            } else {
                if (d_EDM_USER_CANCEL == inResult) {
                    vdSetErrorMessage("USER CANCEL");
                    return inResult;
                }

                memset(szCVV2Code, 0x00, sizeof (szCVV2Code));
                vdDisplayErrorMsg(1, 8, "INVALID CVV");

                break;
            }
        } else {
            return d_OK;
        }

        shCount++;
    }
    vdSetErrorMessage("Get CVV ERR");
    return FAIL;

}

int inCTOS_EMVSetTransType(BYTE byTransType) {
    switch (byTransType) {
            //for improve transaction speed 
            //case SALE:
            //	ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x00");
            //	break;
        case REFUND:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE, 1, "\x20");
            break;
        case PRE_AUTH:		// for case @2140 - Preauth trx got format error in new switch		
        	ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE, 1, "\x00");
            //ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE, 1, "\x10");
			//ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE, 1, "\x30");
            break;
        case CASH_ADVANCE:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE, 1, "\x01"); //better get it from process first byte
            break;
        default:
            break;
    }
}
//for improve transaction speed 

void vdCTOSS_EMV_SetAmount(void) {
    BYTE szBaseAmount[20];
    BYTE szTipAmount[20];
    BYTE szTotalAmount[20];
    BYTE EMVtagVal[64];
    BYTE szStr[64];
    BYTE byDataTmp1[32];
    BYTE byDataTmp2[32];
    BYTE bPackSendBuf[256];
    USHORT usPackSendLen = 0;
    USHORT ushEMVtagLen;
    ULONG lnTmp;

    memset(byDataTmp1, 0x00, sizeof (byDataTmp1));
    memset(byDataTmp2, 0x00, sizeof (byDataTmp2));
    wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
    lnTmp = atol(byDataTmp1);
    wub_long_2_array(lnTmp, byDataTmp2);

    memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
    bPackSendBuf[usPackSendLen++] = 0x04;
    memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
    usPackSendLen += 4;

    memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szTotalAmount, 6);

    memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x02", 2);
    usPackSendLen += 2;
    bPackSendBuf[usPackSendLen++] = 0x06;
    memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F02, 6);
    usPackSendLen += 6;

    if (atol(szTipAmount) > 0) {
        memcpy(srTransRec.stEMVinfo.T9F03, srTransRec.szTipAmount, 6);
    } else {
        memset(szTipAmount, 0x00, sizeof (szTipAmount));
        memcpy(srTransRec.stEMVinfo.T9F03, szTipAmount, 6);
    }

    memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x03", 2);
    usPackSendLen += 2;
    bPackSendBuf[usPackSendLen++] = 0x06;
    memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F03, 6);
    usPackSendLen += 6;

    vdPCIDebug_HexPrintf("EMV_SetAmount", bPackSendBuf, usPackSendLen);
    usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);

}

int inCTOS_EMVProcessing(void) {
    int inRet;
    BYTE EMVtagVal[64];
    BYTE bySC_status;

    int fVEPflag = 0;
    int fQPSflag = 0;

    vdDebug_LogPrintf("*** inCTOS_EMVProcessing EntryMode = %d START ***", srTransRec.byEntryMode);

    if (CARD_ENTRY_WAVE == srTransRec.byEntryMode)
        inRet = inCTOSS_CheckVEPQPS(0);


    if (CARD_ENTRY_ICC == srTransRec.byEntryMode) {
        inCTOS_EMVSetTransType(srTransRec.byTransType);

        inCTOSS_CheckVEPQPS(0);

        vdCTOSS_EMV_SetAmount(); //for improve transaction speed 

        //if really need control the offline PIN UI.
        //usCTOSS_EMV_SetOfflinePinParam(&srTransRec);

        inRet = usCTOSS_EMV_TxnPerform();
        inCTOS_FirstGenACGetAndSaveEMVData();

        inCTOSS_CheckVEPQPS(1);

        //if('4' ==srTransRec.szPAN[0])
        //inCTOSS_CheckVEPQPS(1);//VISA

        //if('5' ==srTransRec.szPAN[0])
        //inCTOSS_CheckVEPQPS(2);//Mastercard

		// for all offline approval to go online for MC offline issue case #1251
		#ifdef EMV_Y1_DISABLE
		vdDebug_LogPrintf("inCTOS_EMVProcessing byEntryMode[%d] 9F27[%02X]", srTransRec.byEntryMode, srTransRec.stEMVinfo.T9F27);		
		
		if(CARD_ENTRY_ICC == srTransRec.byEntryMode && srTransRec.stEMVinfo.T9F27 == 0x40)
			srTransRec.stEMVinfo.T9F27 = 0x80;
		#endif

        EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

        switch (EMVtagVal[0] & 0xC0) {
            case 0: //Declined --- AAC  
                strcpy(srTransRec.szAuthCode, "Z1");

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                if (!(bySC_status & d_MK_SC_PRESENT))
                    vdSetErrorMessage("CHIP MALFUNCTION");
                else
                    vdSetErrorMessage("EMV Decline");
                /* Issue# 000065 - end -- jzg */

                vdDebug_LogPrintf("1st ACs, card dec");

                return EMV_CRITICAL_ERROR;

            case 0x40: //Approval --- TC
                strcpy(srTransRec.szAuthCode, "Y1");
                srTransRec.shTransResult = TRANS_AUTHORIZED;
                vdDebug_LogPrintf("1nd AC app");
                break;

            case 0x80: //ARQC
                vdDebug_LogPrintf("go online");
                break;

            default:
                strcpy(srTransRec.szAuthCode, "Z1");

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                if (!(bySC_status & d_MK_SC_PRESENT))
                    vdSetErrorMessage("CHIP MALFUNCTION");
                else
                    vdSetErrorMessage("EMV Decline");
                /* Issue# 000065 - end -- jzg */

                return EMV_CRITICAL_ERROR;

        }

        vdDebug_LogPrintf("usCTOSS_EMV_TxnPerform return[%d]", inRet);
        if (inRet != d_OK)
            vdSetErrorMessage("First GenAC ERR");
        return inRet;

    }





    return d_OK;
}

int inCTOS_CheckTipAllowd() {

    if (SALE_TIP == srTransRec.byTransType) {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;
    }

    if (TRUE != strTCT.fTipAllowFlag) {
        if (SALE_TIP == srTransRec.byTransType)
            vdSetErrorMessage("TIP NOT ALLWD");

        return d_NO;
    }

    //1010
    if (srTransRec.byVoided == TRUE) {
        vdSetErrorMessage("TIP NOT ALLWD");
        return d_NO;
    }
    //1010

    return d_OK;
}

SHORT shCTOS_EMVAppSelectedProcess(void) {
    short shResult;
    BYTE SelectedAID[16];
    USHORT SelectedAIDLen = 0;
    BYTE label[32];
    USHORT label_len = 0;
    BYTE PreferAID[128];
    CTOS_RTC SetRTC;
    int inRet = 0;

	vdDebug_LogPrintf("-------shCTOS_EMVAppSelectedProcess-----");
    memset(PreferAID, 0, sizeof (PreferAID));
    memset(label, 0, sizeof (label));

    // patrick testing 20160509
    memcpy(PreferAID, "\xA0\x00\x00\x06\x15", 5);
    shResult = usCTOSS_EMV_TxnAppSelect(PreferAID, 5, SelectedAID, &SelectedAIDLen, label, &label_len);

    if (shResult == EMV_FALLBACK) {
        inRet = shResult;

        shResult = usCTOSS_EMV_TxnAppSelect(PreferAID, 5, SelectedAID, &SelectedAIDLen, label, &label_len);
        if ((shResult) == PP_OK && (inRet == EMV_FALLBACK))
            inRet = shResult;

        if ((inRet == PP_OK) && (shResult == PP_OK))
            return d_OK;
    }

    //    shResult = usCTOSS_EMV_TxnAppSelect(PreferAID, 0, SelectedAID, &SelectedAIDLen, label, &label_len);
    vdDebug_LogPrintf("-shCTOS_EMVAppSelectedProcess=[%d] SelectedAIDLen[%d] label[%s]fback[%d]", shResult, SelectedAIDLen, label, strEMVT.inEMVFallbackAllowed);
    if (d_OK == shResult)
        DebugAddHEX("SelectedAIDLen", SelectedAID, SelectedAIDLen);
    vdDebug_LogPrintf("-------shCTOS_EMVAppSelectedProcess--shResult=[%d]---", shResult);
    if ((shResult != PP_OK) && (shResult != EMV_USER_ABORT)) {
        //EMV: should display "CHIP NOT DETECTED" instead of doing fallback - start -- jzg
        if (shResult == EMV_CHIP_NOT_DETECTED) {
            vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
            return EMV_CHIP_FAILED;
        }
        //EMV: should display "CHIP NOT DETECTED" instead of doing fallback - end -- jzg

        //EMV: If AID not found display "TRANS NOT ALLOWED" - start -- jzg
        if (shResult == EMV_TRANS_NOT_ALLOWED) {
            vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
            return EMV_CHIP_FAILED;
        }
        //EMV: If AID not found display "TRANS NOT ALLOWED" - end -- jzg

        //VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - start -- jzg
        if (shResult == EMV_CARD_BLOCKED) {

			vdDebug_LogPrintf("-------shCTOS_EMVAppSelectedProcess AAA-----");
			
            inFallbackToMSR = FAIL;
            vdDisplayErrorMsg(1, 8, "CARD BLOCKED");
            return EMV_CHIP_FAILED;
        }
        //VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - end -- jzg

        if (EMV_FALLBACK == shResult) {
            //0826
            //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
            vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
            //0826

			vdDebug_LogPrintf("-------shCTOS_EMVAppSelectedProcess--A---");

            CTOS_RTCGet(&SetRTC);
            inFallbackToMSR = SUCCESS;
            sprintf(strTCT.szFallbackTime, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
        } else {
            vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
        }

        return EMV_CHIP_FAILED;
    }

    if (shResult == EMV_USER_ABORT) {


        if (strEMVT.inEMVFallbackAllowed) {
            //0826
            //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
            vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
            //0826

			vdDebug_LogPrintf("-------EMV_TxnAppSelect--B---");

            CTOS_RTCGet(&SetRTC);
            inFallbackToMSR = SUCCESS;
            sprintf(strTCT.szFallbackTime, "%02d%02d%02d", SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);
        } else {
            vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
        }


        return EMV_USER_ABORT;
    }

    return d_OK;

}

short shCTOS_EMVSecondGenAC(BYTE *szIssuerScript, UINT inIssuerScriptlen) {
#define ACT_ONL_APPR 1
#define ACT_ONL_DENY 2
#define ACT_UNAB_ONL 3
#define ACT_ONL_ISSUER_REFERRAL 4           //From Host
#define ACT_ONL_ISSUER_REFERRAL_APPR 4
#define ACT_ONL_ISSUER_REFERRAL_DENY 5

    USHORT usResult;
    EMV_ONLINE_RESPONSE_DATA st2ACResponseData;
    BYTE EMVtagVal[64];
    BYTE bySC_status; //Issue# 000065 -- jzg

    memset(&st2ACResponseData, 0, sizeof (st2ACResponseData));

    //st2ACData.iAction will decide trans approve or not
    if (srTransRec.shTransResult == TRANS_COMM_ERROR)
        st2ACResponseData.bAction = ACT_UNAB_ONL;
    else if (srTransRec.shTransResult == TRANS_REJECTED)
        st2ACResponseData.bAction = ACT_ONL_DENY;
    else if (srTransRec.shTransResult == TRANS_CALL_BANK)
        st2ACResponseData.bAction = ACT_ONL_ISSUER_REFERRAL;
    else if (srTransRec.shTransResult == TRANS_AUTHORIZED)
        st2ACResponseData.bAction = ACT_ONL_APPR;
    //Fix for Issue Script data
    //memset(szIssuerScript,0,sizeof(szIssuerScript));
    st2ACResponseData.pAuthorizationCode = srTransRec.szRespCode;
    st2ACResponseData.pIssuerAuthenticationData = srTransRec.stEMVinfo.T91;
    st2ACResponseData.IssuerAuthenticationDataLen = srTransRec.stEMVinfo.T91Len;
    st2ACResponseData.pIssuerScript = szIssuerScript;
    st2ACResponseData.IssuerScriptLen = inIssuerScriptlen;

    usResult = TRANS_AUTHORIZED;
    DebugAddHEX("shCTOS_EMVSecondGenAC ", st2ACResponseData.pIssuerScript, st2ACResponseData.IssuerScriptLen);
    usResult = usCTOSS_EMV_TxnCompletion(&st2ACResponseData);

    vdDebug_LogPrintf("PP_iCompletion:%d ", usResult);

    inCTOS_SecondGenACGetAndSaveEMVData();

    if (VS_TRUE == strTCT.fDemo) {
        usResult = PP_OK;
        EMVtagVal[0] = 0x40;
    }


    vdDebug_LogPrintf("shCTOS_EMVSecondGenAC::srTransRec.stEMVinfo.T9F27[%02X]", srTransRec.stEMVinfo.T9F27);

	// for testing, to eliminate emv decline and printing of error receipt for JCB TCI Y1 - forced online.
	#if 1
	//if(CARD_ENTRY_ICC == srTransRec.byEntryMode && srTransRec.stEMVinfo.T9F27 == 0x40 && (strncmp(srTransRec.szRespCode, "00", 2) == 0)){
	if(usResult == 255 && srTransRec.stEMVinfo.T9F27 == 0x40 && (strncmp(srTransRec.szRespCode, "00", 2) == 0) && srTransRec.shTransResult == 99){
		usResult = PP_OK;
	}
	#endif

    if (usResult != PP_OK) {
        if (strcmp((char *) srTransRec.szRespCode, "00") == 0)
            vdSetErrorMessage("EMV Decline");

        return EMV_CRITICAL_ERROR;
    }


    EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

    switch (EMVtagVal[0] & 0xC0) {
        case 0: //Declined --- AAC
            if (strcmp((char *) srTransRec.szRespCode, "00") == 0) //approve by host, but not by card
            {

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                if (!(bySC_status & d_MK_SC_PRESENT))
                    vdSetErrorMessage("CHIP MALFUNCTION");
                else
                    vdSetErrorMessage("EMV Decline");
                /* Issue# 000065 - end -- jzg */

                vdDebug_LogPrintf("Host app, card dec");
            } else {
                vdDebug_LogPrintf("Host reject");
            }
            return EMV_CRITICAL_ERROR;

        case 0x40: //Approval --- TC       
            vdDebug_LogPrintf("2nd AC app");
            if (srTransRec.shTransResult != TRANS_AUTHORIZED) //not approve by host, but approve by card
            {

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                if (!(bySC_status & d_MK_SC_PRESENT))
                    vdSetErrorMessage("CHIP MALFUNCTION");
                else
                    vdSetErrorMessage("EMV Decline");
                /* Issue# 000065 - end -- jzg */

                vdDebug_LogPrintf("Host decline, card approve");
                return EMV_CRITICAL_ERROR;
            }
            break;
        case 0x80: //ARQC - fix for JCB VOID PAC
                vdDebug_LogPrintf("go online");
                break;			

        default:
			vdDebug_LogPrintf("shCTOS_EMVSecondGenAC Default!!!");

            /* Issue# 000065 - start -- jzg */
            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if (!(bySC_status & d_MK_SC_PRESENT))
                vdSetErrorMessage("CHIP MALFUNCTION");
            else
                vdSetErrorMessage("EMV Decline");
            /* Issue# 000065 - end -- jzg */

            return EMV_CRITICAL_ERROR;

    }

    vdDebug_LogPrintf("End 2nd GenAC shTransResult=%d iAction=%d", srTransRec.shTransResult, st2ACResponseData.bAction);
    return PP_OK;

}

int inCTOS_EMVTCUpload(void) {
    int inRet;

    vdDebug_LogPrintf("inCTOS_EMVTCUpload=%d", srTransRec.byEntryMode);

    if (strTCT.fSendTCBeforeSettle == TRUE) {
        if (isCheckTerminalMP200() == d_OK)
            return d_OK;
    }

    //0929
    //if( CARD_ENTRY_ICC == srTransRec.byEntryMode) 
    if ((CARD_ENTRY_ICC == srTransRec.byEntryMode) ||
            ((CARD_ENTRY_WAVE == srTransRec.byEntryMode) &&
            ((srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
            (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
            (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
            (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD) ||
            (srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
             (srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY))))
        //0929
    {
    
    vdDebug_LogPrintf("inCTOS_EMVTCUpload HERE***************");
	
        inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
        inRet = inProcessEMVTCUpload(&srTransRec, -1); // TC upload

        vdDebug_LogPrintf("szFileName, %s%02d%02d.rev"
                , strHDT.szHostLabel
                , strHDT.inHostIndex
                , srTransRec.MITid);
        inCTOS_inDisconnect();
    }

    return d_OK;
}

int inCTOS_GetPubKey(const char *filename, unsigned char *modulus, int *mod_len, unsigned char *exponent, int *exp_len) {
    unsigned char tmp[1024];
    int iRead;
    int iMod;
    int iExp;
    FILE *fPubKey;
    UINT uintRet;

    fPubKey = fopen((char*) filename, "rb");
    if (fPubKey == NULL)
        return CTOS_RET_PARAM;

    uintRet = fread(tmp, 1, sizeof (tmp), fPubKey);
    fclose(fPubKey);
    vdDebug_LogPrintf("CAPK=Len[%d]==[%s]", uintRet, tmp);

    if (uintRet > 0) {
        iMod = (int) (tmp[0] - 0x30)*100 + (tmp[1] - 0x30)*10 + (tmp[2] - 0x30);
        vdDebug_LogPrintf("iMod===[%d]", iMod);
        if (iMod % 8 != 0)
            return (CTOS_RET_PARAM);

        if (iMod > CTOS_PED_RSA_MAX)
            return (CTOS_RET_PARAM);

        *mod_len = iMod;
        wub_str_2_hex((tmp + 3), (modulus), iMod * 2);

        vdDebug_LogPrintf("*mod_len===[%d]", *mod_len);
        DebugAddHEX("Module HEX string===", modulus, iMod);


        iExp = (int) tmp[iMod * 2 + 4] - 0x30;
        wub_str_2_hex((&tmp[5 + (iMod * 2)]), (exponent), iExp * 2);

        vdDebug_LogPrintf("iExp===[%d]", iExp);
        DebugAddHEX("Exponent HEX string===", exponent, iExp);

        *exp_len = iExp;
    } else
        return (CTOS_RET_CALC_FAILED);

    return (CTOS_RET_OK);
}

USHORT ushCTOS_EMV_NewDataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue) {
    USHORT usResult;
    USHORT usTagLen;
    static USHORT usGetEMVTimes = 0;

    usResult = usCTOSS_EMV_DataGet(usTag, &usTagLen, pValue);
    *pLen = usTagLen;

    usGetEMVTimes++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewDataGet Times[%d] usTagLen[%d]", usGetEMVTimes, usTagLen);
    return usResult;

}

USHORT ushCTOS_EMV_NewTxnDataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue) {
    USHORT usResult;
    static USHORT usSetEMVTimes = 0;

    usResult = usCTOSS_EMV_DataSet(usTag, usLen, pValue);
    usSetEMVTimes++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewTxnDataSet Times[%d] usResult[%d]", usSetEMVTimes, usResult);

    return usResult;
}

short shCTOS_EMVGetChipDataReady(void) {
    short shResult;
    BYTE byDataTmp1[64];
    BYTE byVal[64];
    USHORT usLen;
    USHORT inIndex;
    BYTE szDataTmp[5];
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];


    USHORT inTagLen = 0;


    memset(szGetEMVData, 0, sizeof (szGetEMVData));
    memset(szOutEMVData, 0, sizeof (szOutEMVData));

    //for improve transaction speed
    //shResult = usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_SELECT_APP, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_SELECT_APP", szOutEMVData, inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_57, byVal, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_57[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, byVal[0], byVal[1], byVal[2]);

    memset(byDataTmp1, 0x00, sizeof (byDataTmp1));
    wub_hex_2_str(byVal, byDataTmp1, usLen);
    memcpy(srTransRec.szTrack2Data, byDataTmp1, (usLen * 2));
    for (inIndex = 0; inIndex < (usLen * 2); inIndex++) {
        if (byDataTmp1[inIndex] == 'F')
            srTransRec.szTrack2Data[inIndex] = 0;
    }
    vdDebug_LogPrintf("szTrack2Data: %s %d", srTransRec.szTrack2Data, inIndex);

    for (inIndex = 0; inIndex < (usLen * 2); inIndex++) {
        if (byDataTmp1[inIndex] != 'D' && byDataTmp1[inIndex] != '=')
            srTransRec.szPAN[inIndex] = byDataTmp1[inIndex];
        else
            break;
    }
    srTransRec.byPanLen = inIndex;
    vdDebug_LogPrintf("PAN: %s %d", srTransRec.szPAN, inIndex);
    inIndex++;
    memset(szDataTmp, 0x00, sizeof (szDataTmp));
    wub_str_2_hex(&byDataTmp1[inIndex], szDataTmp, 4);
    srTransRec.szExpireDate[0] = szDataTmp[0];
    srTransRec.szExpireDate[1] = szDataTmp[1];
    vdDebug_LogPrintf("EMV functions Expiry Date [%02x%02x]", srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
    inIndex = inIndex + 4;
    memcpy(srTransRec.szServiceCode, &byDataTmp1[inIndex], 3);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5A_PAN, srTransRec.stEMVinfo.T5A, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_5A_PAN[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, srTransRec.stEMVinfo.T5A[0], srTransRec.stEMVinfo.T5A[1], srTransRec.stEMVinfo.T5A[2]);


    srTransRec.stEMVinfo.T5A_len = (BYTE) usLen;
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F30_SERVICE_CODE, srTransRec.stEMVinfo.T5F30, &usLen, szOutEMVData, inTagLen);

    memset(byVal, 0x00, sizeof (byVal));
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F34_PAN_IDENTFY_NO, byVal, &usLen, szOutEMVData, inTagLen);
    vdMyEZLib_LogPrintf("5F34: %02x %d", byVal[0], usLen);
    srTransRec.stEMVinfo.T5F34_len = usLen;
    srTransRec.stEMVinfo.T5F34 = byVal[0];

    shResult = usCTOSS_FindTagFromDataPackage(TAG_82_AIP, srTransRec.stEMVinfo.T82, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_82_AIP-[%02x][%02x]-", srTransRec.stEMVinfo.T82[0], srTransRec.stEMVinfo.T82[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_84_DF_NAME, srTransRec.stEMVinfo.T84, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T84_len = (BYTE) usLen;

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F24_EXPIRE_DATE, srTransRec.stEMVinfo.T5F24, &usLen, szOutEMVData, inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F08_IC_VER_NUMBER, szDataTmp, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F08_IC_VER_NUMBER-[%02x][%02x]-", szDataTmp[0], szDataTmp[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F09_TERM_VER_NUMBER-[%02x][%02x]-", srTransRec.stEMVinfo.T9F09[0], srTransRec.stEMVinfo.T9F09[1]);
    if (usLen == 0)
        memcpy(srTransRec.stEMVinfo.T9F09, "\x00\x4C", 2); // can not get value from api like ...,so i hardcode a value from EMV level 2 cert document
    vdDebug_LogPrintf("9F09: %02x%02x %d", srTransRec.stEMVinfo.T9F09[0], srTransRec.stEMVinfo.T9F09[1], usLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F20, srTransRec.szCardholderName, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("5F20,szCardholderName: %s", srTransRec.szCardholderName);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_50[%d] usLen[%d] [%s]--", shResult, usLen, srTransRec.stEMVinfo.szChipLabel);

}

int inCTOS_FirstGenACGetAndSaveEMVData(void) {
    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;
    int ret = 0;
    unsigned char szTransSeqCounter[6 + 1];
    unsigned char szHEXTransSeqCounter[3 + 1];

    BYTE bAppLabel[64] = {0}; // EMV: Get Application Label -- jzg 
    BYTE bAppPrefName[64] = {0}; // EMV: Get Application Preferred Name -- jzg

    //1026	
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    //1026

    BYTE baTCC[3];

    memset(szGetEMVData, 0, sizeof (szGetEMVData));
    memset(szGetEMVData, 0, sizeof (szGetEMVData));
    vdDebug_LogPrintf("inCTOS_FirstGenACGetAndSaveEMVData");

    //for improve transaction speed
    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_1STAC, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataInit();
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC", szOutEMVData, inTagLen);

	vdDebug_LogPrintf("inCTOS_FirstGenACGetAndSaveEMVData::9F27[%02X]", srTransRec.stEMVinfo.T9F27);		
	

    usCTOSS_FindTagFromDataPackage(TAG_5F2A_TRANS_CURRENCY_CODE, srTransRec.stEMVinfo.T5F2A, &usLen, szOutEMVData, inTagLen);



    /* EMV: Get Application Preferred Name - start -- jzg */
    usCTOSS_FindTagFromDataPackage(TAG_9F12, bAppPrefName, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("TAG 9F12 = [%s]", bAppPrefName);

    vdDispAppLabel(bAppPrefName, usLen, srTransRec.stEMVinfo.szChipLabel);
    vdDebug_LogPrintf("TAG 9F12 = [%s]", srTransRec.stEMVinfo.szChipLabel);
    /* EMV: Get Application Preferred Name - end -- jzg */

    vdDebug_LogPrintf("TAG 9F12 = [0x%02X]", srTransRec.stEMVinfo.szChipLabel[0]);

    /* EMV: Get Application Label - start -- jzg */
    if ((!((srTransRec.stEMVinfo.szChipLabel[0] >= 'a') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'z'))) ||
            (!((srTransRec.stEMVinfo.szChipLabel[0] >= 'A') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'Z')))) {
        //memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
        usCTOSS_FindTagFromDataPackage(TAG_50, bAppLabel, &usLen, szOutEMVData, inTagLen);

        if (strlen(bAppLabel) > 0) {
            memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof (srTransRec.stEMVinfo.szChipLabel));
            vdDispAppLabel(bAppLabel, usLen, srTransRec.stEMVinfo.szChipLabel);
            vdDebug_LogPrintf("TAG 50 = [%s]", srTransRec.stEMVinfo.szChipLabel);
        }
    }

    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9A_TRANS_DATE, srTransRec.stEMVinfo.T9A, &usLen, szOutEMVData, inTagLen);


    usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F06, srTransRec.stEMVinfo.T9F06, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F06_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F1A_TERM_COUNTRY_CODE, srTransRec.stEMVinfo.T9F1A, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F1E, srTransRec.stEMVinfo.T9F1E, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F33_TERM_CAB, srTransRec.stEMVinfo.T9F33, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F35_TERM_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9F35), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F36_ATC, srTransRec.stEMVinfo.T9F36, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F36_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F37_UNPREDICT_NUM, srTransRec.stEMVinfo.T9F37, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F42_APP_CURRENCY_CODE, srTransRec.stEMVinfo.T9F42, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F0D, srTransRec.stEMVinfo.T9F0D, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F0E, srTransRec.stEMVinfo.T9F0E, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F0F, srTransRec.stEMVinfo.T9F0F, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F63, srTransRec.stEMVinfo.T9F63, &usLen, szOutEMVData, inTagLen);
	srTransRec.stEMVinfo.T9F63_len = usLen;
    vdDebug_LogPrintf("TAG_9F63 OCIV usLen[%d]", usLen);

#ifdef JCB_LEGACY_FEATURE
    usCTOSS_FindTagFromDataPackage(TAG_9F6E, srTransRec.stEMVinfo.T9F6E, &usLen, szOutEMVData, inTagLen);
	srTransRec.stEMVinfo.T9F6E_len = usLen;
    vdDebug_LogPrintf("TAG_9F6E OCIV usLen[%d]", usLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F7C, srTransRec.stEMVinfo.T9F7C, &usLen, szOutEMVData, inTagLen);
	srTransRec.stEMVinfo.T9F7C_len = usLen;
    vdDebug_LogPrintf("TAG_9F7C OCIV usLen[%d]", usLen);
#endif

    usCTOSS_FindTagFromDataPackage(TAG_9F53, baEMVBackupT9F53, &usLen, szOutEMVData, inTagLen);
    DebugAddHEX("baEMVBackupT9F53", baEMVBackupT9F53, 3);
    //srTransRec.stEMVinfo.T9F53 = baTCC[0];
    vdDebug_LogPrintf("TAG_9F53 baTCC usLen[%d]", usLen);


    ret = inIITRead(srTransRec.IITid);
    vdDebug_LogPrintf("inIITRead[%d]", ret);
    sprintf(szTransSeqCounter, "%06ld", strIIT.ulTransSeqCounter);
    wub_str_2_hex(szTransSeqCounter, (char *) szHEXTransSeqCounter, 6);
    memcpy(srTransRec.stEMVinfo.T9F41, szHEXTransSeqCounter, 3);
    strIIT.ulTransSeqCounter++;
    ret = inIITSave(srTransRec.IITid);
    vdDebug_LogPrintf(" ret[%d] srTransRec.IITid[%d]strIIT.ulTransSeqCounter[%ld]", ret, srTransRec.IITid, strIIT.ulTransSeqCounter);
    //usCTOSS_FindTagFromDataPackage(TAG_9F41, srTransRec.stEMVinfo.T9F41, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F53, (BYTE *)&(srTransRec.stEMVinfo.T9F53), &usLen, szOutEMVData, inTagLen);

    return d_OK;
}

int inCTOS_SecondGenACGetAndSaveEMVData(void) {

    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;

    memset(szGetEMVData, 0, sizeof (szGetEMVData));
    memset(szGetEMVData, 0, sizeof (szGetEMVData));

    //for improve transaction speed
    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_2NDAC, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC", szOutEMVData, inTagLen);

    vdDebug_LogPrintf("inCTOS_SecondGenACGetAndSaveEMVData");

    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9B, srTransRec.stEMVinfo.T9B, &usLen, szOutEMVData, inTagLen); //@@IBR ADD 20161213

    return d_OK;
}

int inCTOS_showEMV_TagLog(void) {
#define TOTAL_TAGS  24
    int i;
    unsigned short tagLen;
    char outp[40];

    typedef struct {
        unsigned short Tags;
        char description[20];
    } print_tag;

    print_tag EMVTag[TOTAL_TAGS] ={
        { TAG_95, "TVR"},
        { TAG_9B, "TSI"},
        { TAG_9F26_EMV_AC, "ARQC"},
        { TAG_9F27, "Crypt Info Data"},
        { TAG_9F10_IAP, "Issuer Appl Data"},
        { TAG_9F37_UNPREDICT_NUM, "Unpredicte number"},
        { TAG_9F36_ATC, "Appl Trans Counter"},
        { TAG_9A_TRANS_DATE, "Trans Date"},
        { TAG_9C_TRANS_TYPE, "Trans Type"},
        { TAG_5A_PAN, "PAN"},
        { TAG_5F34_PAN_IDENTFY_NO, "Sequence Num"},
        { TAG_9F02_AUTH_AMOUNT, "Amount Authorized"},
        { TAG_9F03_OTHER_AMOUNT, "Add Amount"},
        { TAG_5F2A_TRANS_CURRENCY_CODE, "Trans Currency Code"},
        { TAG_82_AIP, "AIP"},
        { TAG_9F1A_TERM_COUNTRY_CODE, "Term Country Code"},
        { TAG_9F34_CVM, "CVR"},
        { TAG_9F10_IAP, "Issuer auth Data"},
        { TAG_9F06, "AID"},
        { TAG_50, "Appl Label Name"},
        { TAG_8F, "CA Public Key Index"},
        { TAG_9F0D, "IAC Default"},
        { TAG_9F0E, "IAC Denial"},
        { TAG_9F0F, "IAC Online"}

    };


    for (i = 0; i < TOTAL_TAGS; i++) {
        memset(outp, 0x00, sizeof (outp));
        ushCTOS_EMV_NewDataGet(EMVTag[i].Tags, &tagLen, outp);
        vdDebug_LogPrintf("----TAG[%s][%x]=====Len[%d]----", EMVTag[i].description, EMVTag[i].Tags, tagLen);
        DebugAddHEX("Value===", outp, tagLen);
    }
    return d_OK;
}

void vdCTOSS_GetAmt(void) {
    memcpy(srTransRec.szBaseAmount, szBaseAmount, 6);
}

void vdCTOSS_SetAmt(BYTE *baAmount) {
    BYTE szTemp[20];

    memset(szTemp, 0x00, sizeof (szTemp));
    //format amount 10+2
    sprintf(szTemp, "%012.0f", atof(baAmount));
    wub_str_2_hex(szTemp, szBaseAmount, 12);
}

int inCTOSS_GetOnlinePIN(void) {
    int inRet = d_NO;
    BYTE EMVtagVal[64];
    USHORT EMVtagLen;

	vdDebug_LogPrintf("inCTOSS_GetOnlinePIN srTransRec.IITid [%d ]srTransRec.HDTid [%d] srTransRec.byEntryMod [%d] fGetMPUCard()", 
		srTransRec.IITid, srTransRec.HDTid, srTransRec.byEntryMode, fGetMPUCard());

	//http://118.201.48.210:8080/redmine/issues/3504.8
	//if(srTransRec.HDTid == 21 || srTransRec.HDTid == 1)
	//if((srTransRec.HDTid == 6 || srTransRec.HDTid == 21) && (srTransRec.IITid == 2 || srTransRec.IITid == 1))
	if((srTransRec.HDTid == 6 || srTransRec.HDTid == 22) && (srTransRec.IITid == 2 || srTransRec.IITid == 1))
	{
		vdDebug_LogPrintf("inCTOSS_GetOnlinePIN HERE");

		 if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		 	return d_OK;
	}

    vdDebug_LogPrintf("inCTOSS_GetOnlinePIN HERE2");

    if (CARD_ENTRY_ICC == srTransRec.byEntryMode)// || srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        return d_OK;

	//if(srTransRec.HDTid == 21 && srTransRec.byEntryMode == CARD_ENTRY_WAVE)
	//	return d_OK;
		

    if (fGetMPUCard() == TRUE){
		vdDebug_LogPrintf("inCTOSS_GetOnlinePIN EXIT HERE");

        return d_OK;
    }



	vdDebug_LogPrintf("inCTOSS_GetOnlinePIN CONTINUE HERE");

	//for JCB CVM
#if 1
	EMVtagLen = 3;
	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
	memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F33, EMVtagLen);//srTransRec.stEMVinfo.T95, EMVtagLen);
    vdDebug_LogPrintf("inCTOSS_GetOnlinePIN T9F33 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);

	if(srTransRec.byEntryMode == CARD_ENTRY_WAVE && (srTransRec.IITid == 8 || srTransRec.IITid == 4))
	{
		if(EMVtagVal[1] == 0x28) //signature(paper), No CVM required, No PIN entry
			return d_OK;
	}	
#endif




    memset(EMVtagVal, 0x00, sizeof (EMVtagVal));
    EMVtagLen = 3;
    memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
    vdDebug_LogPrintf("T9F34 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
	
    //'02' or '42'(CVM Code for 'Online PIN') Byte 1(CVM Performed)
    if (EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42) 
	{
        inInitializePinPad();
#ifdef DUKPT_3DESONLINEPIN
        inRet = GetPIN_With_3DESDUKPT();
#else
        inRet = inGetIPPPin();
#endif
        if (d_OK != inRet) {
            if (d_KMS2_GET_PIN_NULL_PIN == inRet)
                return d_OK;
            else
                return inRet;
        }
    } else
        inRet = d_OK;

    return inRet;
}

int inCUP_GetOnlinePIN(void) {
    int inRet = d_NO;
    BYTE EMVtagVal[64];
    USHORT EMVtagLen;

    vdDebug_LogPrintf("inCUP_GetOnlinePIN EntryMode=[%d]........", srTransRec.byEntryMode);

    if (CARD_ENTRY_ICC == srTransRec.byEntryMode)
        return d_OK;

    inInitializePinPad();
#ifdef DUKPT_3DESONLINEPIN
    inRet = GetPIN_With_3DESDUKPT();
#else
    inRet = inGetIPPPin();
#endif
    if (d_OK != inRet) {
        if (d_KMS2_GET_PIN_NULL_PIN == inRet)
            return d_OK;
        else
            return inRet;
    }

    return inRet;
}


int inCAV_MPU_UPI_GetOnlinePIN(void) {
    int inRet = d_NO;
    BYTE EMVtagVal[64];
    USHORT EMVtagLen;

    vdDebug_LogPrintf("inCAV_MPU_UPI_GetOnlinePIN EntryMode:[%d]HDTid:[%d].IITid[%d.]byTransType.[%d].szTID:[%s].szHostLabel:[%s].....", 
		srTransRec.byEntryMode, srTransRec.HDTid, srTransRec.IITid, srTransRec.byTransType, srTransRec.szTID, strHDT.szHostLabel);


#if 1
    //if (CARD_ENTRY_ICC == srTransRec.byEntryMode)
     //   return d_OK;

	// no pin entry for Visa and Master Contact and ctls
	if(srTransRec.HDTid == 21 || srTransRec.HDTid == 1 || srTransRec.IITid == 1 || srTransRec.IITid == 2)
	{
		 if (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		 	return d_OK;
	}
#else
	//remove condition to resolve redmine case #2181 #2
    //if (CARD_ENTRY_ICC == srTransRec.byEntryMode && srTransRec.HDTid != 7)
     //   return d_OK;
#endif

    vdDebug_LogPrintf("inCAV_MPU_UPI_GetOnlinePIN Before inGetCAVIPPPin");


    inInitializePinPad();

	
/*#ifdef DUKPT_3DESONLINEPIN
    inRet = GetPIN_With_3DESDUKPT();
#else
    inRet = inGetIPPPin();
#endif*/

	#ifdef PIN_CHANGE_ENABLE
	inRet = inGetCAVIPPPin();
	#else
    inRet = inGetIPPPin();
	#endif

	
    if (d_OK != inRet) {
        if (d_KMS2_GET_PIN_NULL_PIN == inRet)
            return d_OK;
        else
            return inRet;
    }

    return inRet;
}

int inMPU_UPI_GetOnlinePIN(void) {
    int inRet = d_NO;
    BYTE EMVtagVal[64];
    USHORT EMVtagLen;

    vdDebug_LogPrintf("inMPU_UPI_GetOnlinePIN EntryMode=[%d]........", srTransRec.byEntryMode);

    if (CARD_ENTRY_ICC == srTransRec.byEntryMode)
        return d_OK;

    inInitializePinPad();
#ifdef DUKPT_3DESONLINEPIN
    inRet = GetPIN_With_3DESDUKPT();
#else
    inRet = inGetIPPPin();
#endif
    if (d_OK != inRet) {
        if (d_KMS2_GET_PIN_NULL_PIN == inRet)
            return d_OK;
        else
            return inRet;
    }

    return inRet;
}


//@@IBR ADD 20161025

int inMPU_GetOnlinePIN(void) {
    int inRet = d_NO;
    BYTE EMVtagVal[64];
    USHORT EMVtagLen;
    BYTE szPinBlock[64 + 1];

    vdDebug_LogPrintf("inMPU_GetOnlinePIN EntryMode:[%d] IITid:[%d] HDTid:[%d]inType[%d]strCST.inCurrencyIndex[%d]strHDT.inCurrencyIdx[%d]........", 
		srTransRec.byEntryMode, srTransRec.IITid, srTransRec.HDTid, strCDT.inType, strCST.inCurrencyIndex, strHDT.inCurrencyIdx);



	// for JCB CTLS CVM
	#if 1
		EMVtagLen = 3;
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F33, EMVtagLen);//srTransRec.stEMVinfo.T95, EMVtagLen);
		vdDebug_LogPrintf("inMPU_GetOnlinePIN T9F33 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);

		if(srTransRec.byEntryMode == CARD_ENTRY_WAVE && (srTransRec.IITid == 8 || srTransRec.IITid == 4))
		{
			if(EMVtagVal[1] == 0x28)//signature(paper), No CVM required, No PIN entry
				return d_OK;
		}
	#endif


	//eliminate multiple pin entry
	#if 1
	memset(EMVtagVal, 0x00, sizeof (EMVtagVal));
	memset(szPinBlock, 0x00, sizeof (szPinBlock));
	EMVtagLen = 3;
	memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
	
	vdDebug_LogPrintf("inMPU_GetOnlinePIN::T9F34 EMVtagVal=[%x][%x][%x]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2]);
	
	//'02' or '42'(CVM Code for 'Online PIN') Byte 1(CVM Performed)
	if (EMVtagVal[0] == 0x02 || EMVtagVal[0] == 0x42) 
	{
		wub_hex_2_str(srTransRec.szPINBlock, szPinBlock, 8);

		vdDebug_LogPrintf("szPinBlock=[%s]......", szPinBlock);

		if (strlen(szPinBlock) > 0)
		return d_OK;
	}
	#endif

	//http://118.201.48.210:8080/redmine/issues/3504.8
	//if(srTransRec.HDTid == 21 || srTransRec.HDTid == 1)
	//Fix for http://118.201.48.214:8080/issues/75 #35.  4)MC/VS card is asking to prompt PIN in this pk. So we would like to get normal production flow.
	// added srTransRec.IITid == 6 so UPI wont ask for pin.
	if((srTransRec.HDTid == 6 || srTransRec.HDTid ==  22 || srTransRec.HDTid == 23)  && (srTransRec.IITid == 2 || srTransRec.IITid == 1 || srTransRec.IITid == 6))
	{
	
		 vdDebug_LogPrintf("inMPU_GetOnlinePIN ((srTransRec.HDTid == 6 || srTransRec.HDTid ==  22)  && (srTransRec.IITid == 2 || srTransRec.IITid == 1 || srTransRec.IITid == 6))");
		 if (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE || srTransRec.byEntryMode == CARD_ENTRY_MANUAL || 
		 	srTransRec.byEntryMode == CARD_ENTRY_FALLBACK || srTransRec.byEntryMode == CARD_ENTRY_MSR)

		 	vdDebug_LogPrintf("inMPU_GetOnlinePIN to EXIT");
		 	return d_OK;
	}

	//f ro testing - wont ask for Online PIN versification if inType is Debit.
	if(strCDT.inType == DEBIT_CARD && srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE )
		return d_OK;

	#if 0
	if (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE){
		if(srTransRec.HDTid == 17 && srTransRec.IITid == 8) //- http://118.201.48.214:8080/issues/98?
		else
		return d_OK;
	}

    if (CARD_ENTRY_ICC == srTransRec.byEntryMode)
        return d_OK;
	#endif

    if ((CARD_ENTRY_MANUAL == srTransRec.byEntryMode) && (srTransRec.HDTid == 19))
        return d_OK;

		#ifdef CB_MPU_NH_MIGRATION
		// for new MPU host application upgrade - based "POS function and  Regulation" doc
        if((CARD_ENTRY_MANUAL == srTransRec.byEntryMode) && (srTransRec.HDTid == 17))
            return d_OK;
		#endif
		
        if((srTransRec.byTransType == MPU_PREAUTH_COMP) && (srTransRec.HDTid == 19))
            return d_OK;

	
	vdDebug_LogPrintf("inMPU_GetOnlinePIN:Before inGetIPPPin:");

	inInitializePinPad();
#ifdef DUKPT_3DESONLINEPIN
    inRet = GetPIN_With_3DESDUKPT();
#else
    inRet = inGetIPPPin();
#endif

	vdDebug_LogPrintf("inMPU_GetOnlinePIN:After inGetIPPPin: inRet [%d]", inRet);


    if (d_OK != inRet) {

		#ifdef CBB_FIN_ROUTING
		//for testing. fix prompt of default currency.	Currenct its changing on pin entry cancellation and after transaction. - 08222022
		inCSTRead(strHDT.inCurrencyIdx);
		#endif

		
        if (d_KMS2_GET_PIN_NULL_PIN == inRet)
            return d_OK;
        else
            return inRet;
    }


    return inRet;
}

/* EMV: Get Application Label - start -- jzg */
void vdDispAppLabel(unsigned char *ucHex, int inLen, char *szOutStr) {
    int i;
    char szBuf[80] = {0};

    for (i = 0; i < inLen; i++)
        szBuf[i] = ucHex[i];
    szBuf[i] = 0x00;

    memcpy(szOutStr, szBuf, inLen);
}

/* EMV: Get Application Label - end -- jzg */

int inCTOSS_TWKRSAFlow(void) {
    CTOS_LCDTClearDisplay();

    vdCTOS_TxnsBeginInit();
    srTransRec.HDTid = strEFT[0].inHDTid;
    int inRetVal = 0;

    inRetVal = inHTLESelectAcqVendorIDByHostId(srTransRec.HDTid);
    if (SUCCESS != inRetVal) {
        return ST_ERROR;
    }

    inCTOSS_TWKRSAFlowProcess();
    inCTOS_inDisconnect();
    vdCTOS_TransEndReset();

    return ST_SUCCESS;
}

void vdChkCashAdvAppMenu(void) {
    int inMenuid = 0;
    int inRet = 0;

    vdDebug_LogPrintf("======vdChkCashAdvAppMenu======");

    inRet = inTCTGetCurrMenuid(1, &inMenuid);
    vdDebug_LogPrintf("inTCTGetCurrMenuid inRet[%d] inMenuid[%d]", inRet, inMenuid);

    //inMenuid = get_env_int("#TXNMENU");
    //vdDebug_LogPrintf("#TXNMENU inMenuid[%d]", inMenuid);

    if (CASH_ADV_APP_MENU == inMenuid)
        vdSetCashAdvAppFlag(TRUE);
    else
        vdSetCashAdvAppFlag(FALSE);

	#ifdef SET_IPP_MENU
	if(inMenuid == IPP_MENU)
		vdSetIPPAppFlag(TRUE);
	else		
		vdSetIPPAppFlag(FALSE);
	#endif
	
}

int inMPU_SelectCurrencyMenu(void) {
    int inRet = d_OK;
    int inCSTCnt = 0;
    int inIdx = 0;

    BYTE bHeaderAttr = 0x01 + 0x04, iCol = 1;
    BYTE x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT CURRENCY";

    BYTE szCurSymbol[10][10];
    BYTE szSymbolMenu[1024];

    inCSTCnt = inCSTNumRecord();

    if (inCSTCnt < 2)
        return 1;

    memset(szCurSymbol, 0x00, sizeof (szCurSymbol));
    memset(szSymbolMenu, 0x00, sizeof (szSymbolMenu));

    for (inIdx = 0; inIdx < inCSTCnt; inIdx++) {
        inRet = inCSTRead(inIdx + 1);
        //		strcpy(&szCurSymbol[inIdx], strCST.szCurSymbol);
        strcpy(&szCurSymbol[inIdx][0], strCST.szCurSymbol);
        if (inIdx < inCSTCnt - 1)
            strcat((char *) &szCurSymbol[inIdx], (char *) " \n");

        //		strcat(szSymbolMenu, &szCurSymbol[inIdx]);
        strcat(szSymbolMenu, &szCurSymbol[inIdx][0]);
    }

    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szSymbolMenu, TRUE);
    vdDebug_LogPrintf("key[%d]", key);

    if (key == 0xFF) {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;
    }

    if (key > 0) {
        if (d_KBD_CANCEL == key)
            return -1;
    }

    vdDebug_LogPrintf("Final key[%d]", key);
    inRet = inCSTRead(key);

    return inRet;
}
#ifdef CB_MPU_NH_MIGRATION
// for MPU new host implementation (Refund txn)
USHORT shCTOS_GetAuthTime(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS)
{
    
    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[20];
    BYTE    bInputFormatStr[20];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));
    
    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);
    
    while(1)
    {
        //vduiLightOn();
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        }
        
        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
        case d_KBD_DOT:
            break;
        case d_KBD_CLEAR:
            if (usInputStrLen)
            {
                usInputStrLen--;
                bInputStrData[usInputStrLen] = 0x00;

                memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));
				#if 0
                if(usInputStrLen >= 2)
                {
                    memcpy(bInputFormatStr, bInputStrData, 2);
                    strcat(bInputFormatStr, "/");
                    if(usInputStrLen > 2)
                        strcat(bInputFormatStr, &bInputStrData[2]);
                }
                else
                {
                    strcpy(bInputFormatStr, bInputStrData);
                }
				#else
				
				strcpy(bInputFormatStr, bInputStrData);
				#endif

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, (usMaxLen+1)*2);
                
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
					CTOS_LCDTPrintAligned(usY,bDisplayStr,d_LCD_ALIGNRIGHT);
                }
                else
                {
                    memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                    //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
					CTOS_LCDTPrintAligned(usY,bDisplayStr,d_LCD_ALIGNLEFT);
                }
            }
            break;
        case d_KBD_CANCEL:
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            if (usInputStrLen < usMaxLen )
            {
                bInputStrData[usInputStrLen++] = bKey;

                memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));

				#if 0
                if(usInputStrLen >= 2)
                {
                    memcpy(bInputFormatStr, bInputStrData, 2);
                    strcat(bInputFormatStr, "/");
                    if(usInputStrLen > 2)
                        strcat(bInputFormatStr, &bInputStrData[2]);
                }
                else
                {
                    strcpy(bInputFormatStr, bInputStrData);
                }
				#else				
					strcpy(bInputFormatStr, bInputStrData);
				#endif

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, (usMaxLen+1)*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
					CTOS_LCDTPrintAligned(usY,bDisplayStr,d_LCD_ALIGNRIGHT);
                }
                else
                {
                    memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                    //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
					CTOS_LCDTPrintAligned(usY,bDisplayStr,d_LCD_ALIGNLEFT);
                }
            }
            break;
        case d_KBD_ENTER:
            if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
            {   
                *usStrLen = usInputStrLen;
                strcpy(baBuf, bInputStrData);
                return *usStrLen;
            }
            break;
        default :
            break;
        }
    }

    return 0;
}
#endif
USHORT shCTOS_GetAuthDate(IN USHORT usY, IN USHORT usLeftRight, OUT BYTE *baBuf, OUT USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS) {

    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];
    BYTE bKey = 0x00;
    BYTE bInputStrData[20];
    BYTE bInputFormatStr[20];
    USHORT usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof (bInputStrData));

    if (usTimeOutMS > 0)
        CTOS_TimeOutSet(TIMER_ID_1, usTimeOutMS);

    while (1) {
        //vduiLightOn();
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL;
        }

        CTOS_KBDHit(&bKey);

        switch (bKey) {
            case d_KBD_DOT:
                break;
            case d_KBD_CLEAR:
                if (usInputStrLen) {
                    usInputStrLen--;
                    bInputStrData[usInputStrLen] = 0x00;

                    memset(bInputFormatStr, 0x00, sizeof (bInputFormatStr));
                    if (usInputStrLen >= 2) {
                        memcpy(bInputFormatStr, bInputStrData, 2);
                        strcat(bInputFormatStr, "/");
                        if (usInputStrLen > 2)
                            strcat(bInputFormatStr, &bInputStrData[2]);
                    } else {
                        strcpy(bInputFormatStr, bInputStrData);
                    }

                    memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
                    memset(bDisplayStr, 0x20, (usMaxLen + 1)*2);

                    if (0x01 == usLeftRight) {
                        strcpy(&bDisplayStr[(usMaxLen + 1 - strlen(bInputFormatStr))*2], bInputFormatStr);
                        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNRIGHT);
                    } else {
                        memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                        //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNLEFT);
                    }
                }
                break;
            case d_KBD_CANCEL:
                *usStrLen = 0;
                baBuf[0] = 0x00;
                return d_KBD_CANCEL;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                if (usInputStrLen < usMaxLen) {
                    bInputStrData[usInputStrLen++] = bKey;

                    memset(bInputFormatStr, 0x00, sizeof (bInputFormatStr));
                    if (usInputStrLen >= 2) {
                        memcpy(bInputFormatStr, bInputStrData, 2);
                        strcat(bInputFormatStr, "/");
                        if (usInputStrLen > 2)
                            strcat(bInputFormatStr, &bInputStrData[2]);
                    } else {
                        strcpy(bInputFormatStr, bInputStrData);
                    }

                    memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
                    memset(bDisplayStr, 0x20, (usMaxLen + 1)*2);
                    if (0x01 == usLeftRight) {
                        strcpy(&bDisplayStr[(usMaxLen + 1 - strlen(bInputFormatStr))*2], bInputFormatStr);
                        //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNRIGHT);
                    } else {
                        memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                        //CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                        CTOS_LCDTPrintAligned(usY, bDisplayStr, d_LCD_ALIGNLEFT);
                    }
                }
                break;
            case d_KBD_ENTER:
                if (usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen) {
                    *usStrLen = usInputStrLen;
                    strcpy(baBuf, bInputStrData);
                    return *usStrLen;
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

// for MPU new host implementation (Refund txn)
int inMPU_GetAuthTime(void)
{
    BYTE    szHours[2];
	BYTE    szMinutes[2]; 
	BYTE    szSeconds[2]; 
	
    USHORT  usRet;
    USHORT  usLens;
    USHORT  usMinLen = 6;
    USHORT  usMaxLen = 6;
    USHORT usInputLine = 8;

	BYTE  baBuf[6+1];
	extern BYTE szRefundTime[3];

	vdDebug_LogPrintf("=====inMPU_GetAuthTime=====");

	memset(szRefundTime, 0x00, sizeof(szRefundTime));
	
	vduiClearBelow(2);
    vdDispTransTitle(srTransRec.byTransType);
	
	setLCDPrint(4, DISPLAY_POSITION_LEFT, "Enter Original");
	setLCDPrint(5, DISPLAY_POSITION_LEFT, "Trans Time(HHMMSS):");
	
    while(1)
    {
        usRet = shCTOS_GetAuthTime(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);

		vdDebug_LogPrintf("baBuf[%s]", baBuf);
		
        memset(szHours, 0x00, sizeof(szHours));
        memcpy(szHours, &baBuf[0], 2);
		vdDebug_LogPrintf("szHours[%s]", szHours);
		
		memset(szMinutes, 0x00, sizeof(szMinutes));
        memcpy(szMinutes, &baBuf[2], 2);		
		vdDebug_LogPrintf("szMinutes[%s]", szMinutes);

        memset(szSeconds, 0x00, sizeof(szSeconds));
        memcpy(szSeconds, &baBuf[4], 2);		
		vdDebug_LogPrintf("szSeconds[%s]", szSeconds);

		
        if(atol(szHours) > 24 || atol(szHours) == 0) //||	atol(szMinutes) > 60 || atol(szSeconds) > 60)
        {
            baBuf[0] = 0x00;
            clearLine(8);
            vdDisplayErrorMsg(1, 8, "INVALID TIME FORMAT");
            clearLine(8);
            continue;
        }
        else
        {
        	//strcpy(szAuthDate, baBuf);
			//wub_str_2_hex(&baBuf[2], szRefundTime, 2);
			//wub_str_2_hex(&baBuf[0], &szRefundTime[1], 2);
			wub_str_2_hex(baBuf, szRefundTime, 6);

			//vdDebug_LogPrintf("baBuf[%s]", baBuf);
			//DebugAddHEX("szRefundTime", szRefundTime, 3);

#if 1 // for new MPU host application upgrade	
			if( srTransRec.HDTid == 17 ||
				srTransRec.HDTid == 18 ||
				srTransRec.HDTid == 19)
			{
				//vdDebug_LogPrintf("XXXXXXX3 - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	

				#if 0
				if (strcmp(szRefundTime, srTransRec.szTime)!=0){
					vdDisplayErrorMsg(1, 8, "INVALID TIME");
					clearLine(8);
					continue;					
				}				
				else
				{
					strcpy(srTransRec.szOrgTime, szRefundTime);
					return (d_OK);
				}
				#else
					strcpy(srTransRec.szOrgTime, szRefundTime);
					return (d_OK);				
				#endif
			

			//vdDebug_LogPrintf("XXXXXXX3.1 - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	
				
			}	
#endif			
			//vdDebug_LogPrintf("XXXXXXX3.2 - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	

            //return (d_OK);
        }
    }
	
	//return d_OK;
}

int inMPU_GetAuthDate(void) {
    BYTE szMonth[3];
    BYTE szDate[3];
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 4;
    USHORT usMaxLen = 4;
    USHORT usInputLine = 8;

    BYTE baBuf[4 + 1];
    extern BYTE szAuthDate[2];

    vdDebug_LogPrintf("=====inMPU_GetAuthDate=====");

	memset(szAuthDate, 0x00, sizeof(szAuthDate));
	
	//vduiClearBelow(2);
    CTOS_LCDTClearDisplay();
	
    vdDispTransTitle(srTransRec.byTransType);
	
#ifdef CB_MPU_NH_MIGRATION
	setLCDPrint(4, DISPLAY_POSITION_LEFT, "Enter Original");			
	setLCDPrint(5, DISPLAY_POSITION_LEFT, "Trans Date(DD/MM):");
#endif	

    while(1)
    {
        usRet = shCTOS_GetAuthDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
        if (usRet == d_KBD_CANCEL)
            return (d_EDM_USER_CANCEL);
        memset(szDate, 0x00, sizeof (szDate));
        memcpy(szDate, baBuf, 2);

        memset(szMonth, 0x00, sizeof (szMonth));
        memcpy(szMonth, &baBuf[2], 2);

        if (atol(szMonth) > 12 || 0 == atol(szMonth) ||
                atol(szDate) > 31) {
            baBuf[0] = 0x00;
            clearLine(8);
            vdDisplayErrorMsg(1, 8, "INVALID DATE FORMAT");
            clearLine(8);
            continue;
        } else {
            //strcpy(szAuthDate, baBuf);
            wub_str_2_hex(&baBuf[2], szAuthDate, 2);
            wub_str_2_hex(&baBuf[0], &szAuthDate[1], 2);

			vdDebug_LogPrintf("baBuf[%s]", baBuf);
			DebugAddHEX("szAuthDate", szAuthDate, 2);

#ifdef CB_MPU_NH_MIGRATION // for new MPU host application upgrade	
			if( srTransRec.HDTid == 17 ||
				srTransRec.HDTid == 18 ||
				srTransRec.HDTid == 19)
			{		
				#if 0
				if (strcmp(szAuthDate, srTransRec.szDate)!=0){
					vdDisplayErrorMsg(1, 8, "INVALID DATE");
					clearLine(8);
					continue;					
				}
				else
				{
					strcpy(srTransRec.szOrgDate, szAuthDate);
					return d_OK;
				}
				#else
					strcpy(srTransRec.szOrgDate, szAuthDate);
					return d_OK;
				#endif
			}	
#endif			
            //return d_OK;
        }
    }

    //return d_OK;
}

//void vdCTOSS_RecoverRAM(void)
//{
//	int limitRAM;
//	ULONG ulUsedDiskSize = 0;
//    ULONG ulTotalDiskSize = 0;
//    ULONG ulUsedRamSize = 0;
//    ULONG ulTotalRamSize = 0;
//	ULONG ulAvailableRamSize = 0;
//    ULONG ulAvailDiskSize = 0;
//	
//    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
//    //vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
//	ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
//    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;
//    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]",ulAvailDiskSize,ulAvailableRamSize);
//
//	limitRAM = get_env_int("LIMITRAM");
//	if (limitRAM < 0)
//		limitRAM = 6000;
//	
//    vdCTOSS_RestoreMemory(limitRAM);
//
//    return ;
//    
//}

int inCTOSS_RecoverRAM(void) {
    int limitRAM;
    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;
    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    usCTOSS_SystemMemoryStatus(&ulUsedDiskSize, &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize);
    //vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]", ulAvailDiskSize, ulAvailableRamSize);

    limitRAM = get_env_int("LIMITRAM");
    vdDebug_LogPrintf("limitRAM [%d]", limitRAM);

    if (limitRAM < 0)
        limitRAM = 6000;



    vdCTOSS_RestoreMemory(limitRAM);
    return d_OK;

}

#ifdef APP_AUTO_TEST
int g_inAutoTestCnt = 0;

int inCTOS_GetAutoTestCnt(void) {
    int inAutoTest = 5;

    //g_inAutoTestCnt = get_env_int("#ATCNT");

    //return g_inAutoTestCnt;
    return inAutoTest;
}

int inCTOS_AutoTestFunc(void) {
    int inLoop = 0;

    //if (g_inAutoTestCnt>0)
    {
        vdDebug_LogPrintf("Auto Test Count [%d]", inLoop);
        inCTOS_WAVE_SALE();
    }

    return d_OK;
}
#endif


//   0    0    0    0    0    0    0    0
//   AE   AL   WE   QQ   VM   CUP  
BYTE g_SuppAppStr[8 + 1];

void vdGetMultipleAppRidding(void) {
    BYTE szAppStr[8 + 1];
    int inLen = 0;

    vdDebug_LogPrintf("=====vdGetMultipleAppRidding=====");

    memset(szAppStr, 0x00, sizeof (szAppStr));

    get_env("#APPSUPSTR", szAppStr, 8);
    vdDebug_LogPrintf("szAppStr[%s]", szAppStr);

    inLen = strlen(szAppStr);
    if (inLen <= 0) {
        vdSetAppRidding(RIDDING_AMEX);
        return;
    }

    /*support App array*/
    strcpy(g_SuppAppStr, szAppStr);

    if (0 == strncmp("1000", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX);
    else if (0 == strncmp("1100", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_ALIPAY);
    else if (0 == strncmp("1010", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_WECHAT);
    else if (0 == strncmp("1110", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_ALIPAY_WECHAT);
    else if (0 == strncmp("0100", szAppStr, 4))
        vdSetAppRidding(RIDDING_ALIPAY);
    else if (0 == strncmp("0010", szAppStr, 4))
        vdSetAppRidding(RIDDING_WECHAT);
    else if (0 == strncmp("0110", szAppStr, 4))
        vdSetAppRidding(RIDDING_ALIPAY_WECHAT);
    else if (0 == strncmp("0001", szAppStr, 4))
        vdSetAppRidding(RIDDING_QQWALLET);
    else if (0 == strncmp("0101", szAppStr, 4))
        vdSetAppRidding(RIDDING_ALIPAY_QQWALLET);
    else if (0 == strncmp("0011", szAppStr, 4))
        vdSetAppRidding(RIDDING_WECHAT_QQWALLET);
    else if (0 == strncmp("0111", szAppStr, 4))
        vdSetAppRidding(RIDDING_ALIPAY_WECHAT_QQWALLET);
    else if (0 == strncmp("1001", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_QQWALLET);
    else if (0 == strncmp("1001", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_ALIPAY_QQWALLET);
    else if (0 == strncmp("1001", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_QQWALLET);
    else if (0 == strncmp("1101", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_ALIPAY_QQWALLET);
    else if (0 == strncmp("1011", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_WECHAT_QQWALLET);
    else if (0 == strncmp("1111", szAppStr, 4))
        vdSetAppRidding(RIDDING_AMEX_ALIPAY_WECHAT_QQWALLET);
    else
        vdSetAppRidding(0);
    //		vdSetAppRidding(RIDDING_AMEX);


    /*check if VM Combined*/
    //if ('1' == szAppStr[VM_IDX])
    //	vdSetVMCombine(1);

}

void vdSetIdleScreenShow(BOOL fSet) {
    fIdleScreen = fSet;
}

BOOL fGetIdleScreenShow(void) {
    return fIdleScreen;
}

int inChkPOSTransRefReq(void) {
    /*check if it is conbined app*/
    if (1 == get_env_int("#ALLINONE"))
        return FALSE;

    if (strTCT.fCancelSignFlag == 1) {
        return TRUE;
    }

    return FALSE;
}

int inCTOS_SelectHostAlipay(void) {
    short shGroupId;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx = 0;
    char szAPName[25];
    int inAPPID, i, inNumOfHDTRecord;
    char szTemp[2 + 1];

    strCDT.HDTid = 21;

    inHostIndex = (short) strCDT.HDTid;

    if (inHDTRead(inHostIndex) != d_OK) {
        vdSetErrorMessage("HOST SELECTION ERR");
        return (d_NO);
    }
    else {
        srTransRec.HDTid = inHostIndex;
        inCurrencyIdx = strHDT.inCurrencyIdx;

        if (inHostIndex == 17)
            inCurrencyIdx = 1;

        if (srTransRec.fIsInstallment == TRUE)
            inCurrencyIdx = 1;

        if (inCSTRead(inCurrencyIdx) != d_OK) {
            vdSetErrorMessage("LOAD CST ERR");
            return (d_NO);
        }

        if (inCPTRead(inHostIndex) != d_OK) {
            vdSetErrorMessage("LOAD CPT ERR");
            return (d_NO);
        }
        return (d_OK);
    }
}

int inCTOS_CheckMPUSignOn(void) {

    if (inGetMPUSignOnStatus() == 1)
        return (d_OK);

    else
        return (d_NO);


}

int inCTOSS_ManualSwitchSIM(void) {
    inCTOSS_ManualSwitchSIMProcess(2);
    return d_OK;
}

int inCTOS_WaveGetCardFieldsCTLS(void) {
    USHORT EMVtagLen;
    BYTE EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;
    int usResult;
    ULONG ulAPRtn;
    BYTE temp[64], temp1[64];
    char szTotalAmount[AMT_ASC_SIZE + 1];
    EMVCL_RC_DATA_EX stRCDataEx;
    BYTE szOtherAmt[12 + 1], szTransType[2 + 1], szCatgCode[4 + 1], szCurrCode[4 + 1];
    char szTemp[12 + 1];
    int ret = 0;

    vdDebug_LogPrintf("inCTOS_WaveGetCardFields..byExtReadCard=[%d].......", strTCT.byExtReadCard);

    vdDebug_LogPrintf("ECR [%d]", fGetECRTransactionFlg());

    //        if(fGetECRTransactionFlg() == TRUE){
    //            vdSetFirstIdleKey(0x00);
    //            CTOS_KBDBufFlush();
    //            byKeyBuf = FALSE;
    //        }

    //        if(strTCT.fEnableAmountIdle == TRUE && (d_OK == inCTOS_ValidFirstIdleKey()))
    //            return d_OK;

    if (strTCT.byExtReadCard == 1) {
        usResult = inCTOS_WaveGetCardFieldsExternal();
        return usResult;
    }

    switch (srTransRec.HDTid) {
        case 10://alipay
        case 12: //we chat
        case 16://qq
            return d_OK;
            break;
    }

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

SWIPE_AGAIN:
    EntryOfStartTrans :

#if 0
            if (d_OK != inCTOS_ValidFirstIdleKey()) {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);

        inCTOS_DisplayIdleBMP();
    }
#endif
    memset(&stRCDataEx, 0x00, sizeof (EMVCL_RC_DATA_EX));
    memset(szOtherAmt, 0x00, sizeof (szOtherAmt));
    memset(szTransType, 0x00, sizeof (szTransType));
    memset(szCatgCode, 0x00, sizeof (szCatgCode));
    memset(szCurrCode, 0x00, sizeof (szCurrCode));
    memset(temp, 0x00, sizeof (temp));
    memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    vdDebug_LogPrintf("szTotalAmount[%s].strTCT.szCTLSLimit=[%s]........", szTotalAmount, strTCT.szCTLSLimit);
    if (NO_CTLS == inCTOSS_GetCtlsMode() || atol(szTotalAmount) > atol(strTCT.szCTLSLimit)) {
        //		CTOS_LCDTPrintXY(1, 3, "Insert/Swipe Card");
        //		CTOS_Beep();
        CTOS_Delay(500);
        return (inCTOS_GetCardFields());
    }
    CTOS_LCDTClearDisplay();
    //vduiClearBelow(2);//this function will delay the transaction speed
    if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR()) {
        //CTOS_LCDTClearDisplay();

		vdDebug_LogPrintf("inCTOS_WaveGetCardFieldsCTLS - PPP !!!");
		
        vdDispTransTitle(srTransRec.byTransType);
        CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
        wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
        memset(szTemp, 0x00, sizeof (szTemp));
        sprintf(szTemp, "%012.0f", atof(szTotalAmount));
        memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
        strcpy(szTotalAmount, szTemp);
        //format amount 10+2
        memset(temp1, 0x00, sizeof (temp1));
        if (memcmp(strCST.szCurSymbol, "MMK", 3) == 0) {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, temp1);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
        }


        //		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, temp1);
        sprintf(temp, " Amount: %s", temp1);
        //sprintf(temp," Amount: %10.0f.%02.0f",(atof(szTotalAmount)/100), (atof(szTotalAmount)%100));
        //sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
        CTOS_LCDTPrintXY(1, 4, temp);
    } else {
        wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    }

    if (srTransRec.byTransType == REFUND)
        szTransType[0] = 0x20;

    sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
    strcpy(szCurrCode, szCatgCode);

    //if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode())
    if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR()) {
        ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount, szOtherAmt, szTransType, szCatgCode, szCurrCode);
        //	ulAPRtn = EMVCL_InitTransaction(atol(szTotalAmount));
        if (ulAPRtn != d_EMVCL_NO_ERROR) {
            vdSetErrorMessage("CTLS InitTrans Fail!");
            return d_NO;
        }
    }

    /*Clear KB bufer*/
    CTOS_KBDBufFlush();
    byKeyBuf = 0x00;

    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);

    while (1) {
#if 0
        if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            return READ_CARD_TIMEOUT;
        }
#endif

        CTOS_KBDInKey(&byKeyBuf); //input  

        if (strTCT.fEnableAmountIdle == TRUE && (inCTOS_ValidFirstIdleKey() == d_OK)) {
            CTOS_KBDBufFlush();
            vdSetFirstIdleKey(0x00);
            byKeyBuf = 0x00;
        }

        vdDebug_LogPrintf("byKeyBuf [%d], inCTOS_ValidFirstIdleKey [%d]", byKeyBuf, inCTOS_ValidFirstIdleKey());

        if ((byKeyBuf) || (d_OK == inCTOS_ValidFirstIdleKey())) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            CTOS_KBDGet(&byKeyBuf);

#if 0
            if (byKeyBuf == d_KBD_CANCEL) {
                CTOS_LCDTClearDisplay();
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("TXN CANCELLED");
                return USER_ABORT;
            }
#endif

            memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
            vdSetFirstIdleKey(0x00);

            if (byKeyBuf == d_KBD_0) {
                ret = inCTOS_PromptPassword();
                if (d_OK != ret) {
                    CTOS_LCDTClearDisplay();
                    return FUNCTION_ABORT;
                }
                in0KeyEvent();
                CTOS_LCDTClearDisplay();
                return FUNCTION_ABORT;
            } else if (byKeyBuf == d_KBD_DOT) {
                vdCTOS_SwitchLanguage();
                vduiClearBelow(2);
                return FUNCTION_ABORT;
            } else if (byKeyBuf == d_KBD_F1) {
                inF1KeyEvent();
                vduiClearBelow(2);
                return FUNCTION_ABORT;
            } else if (byKeyBuf == d_KBD_F2) {
                inF2KeyEvent();
                vduiClearBelow(2);
                return FUNCTION_ABORT;
            } else if (byKeyBuf == d_KBD_F3) {
                inF3KeyEvent();
                vduiClearBelow(2);
                return FUNCTION_ABORT;
            } else if (byKeyBuf == d_KBD_F4) {
                inF4KeyEvent();
                vduiClearBelow(2);
                return FUNCTION_ABORT;
            } else if (byKeyBuf == d_KBD_00) {
                vdCTOS_uiIDLEPowerOff();
                vduiClearBelow(2);
                return FUNCTION_ABORT;
            } else if (byKeyBuf == d_KBD_ENTER) {
                inSetColorMenuMode();
                inEnterKeyEvent();
                vduiClearBelow(2);
                return FUNCTION_ABORT;
            }
            //else
            //return USER_ABORT;
#if 0
            memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
            if (d_OK == inCTOS_ValidFirstIdleKey())
                srTransRec.szPAN[0] = chGetFirstIdleKey();

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            //get the card number and ger Expire Date
            if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN)) {
                vdSetFirstIdleKey(0x00);
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail M");
                return USER_ABORT;
            }

            vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
            vdSetFirstIdleKey(0x00);
            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            //@@IBR ADD 20170202 no manual entry
            if (strCDT.fManEntry == FALSE) {
                vduiClearBelow(8);
                setLCDPrint(8, DISPLAY_POSITION_LEFT, "NO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);
                return USER_ABORT;
            }
            //@@IBR FINISH ADD 20170202 no manual entry
#endif

            continue;
        }

#if 0
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (bySC_status & d_MK_SC_PRESENT) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

            vdDebug_LogPrintf("--EMV flow----");
            if (d_OK != inCTOS_EMVCardReadProcess()) {
                if (inFallbackToMSR == SUCCESS) {
                    vdCTOS_ResetMagstripCardData();
                    vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                    goto SWIPE_AGAIN;
                } else {
                    //vdSetErrorMessage("Get Card Fail C");
                    return USER_ABORT;
                }
            }
            vdDebug_LogPrintf("--EMV Read succ----");
            if (srTransRec.byTransType == REFUND)
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

            /*for MPU Application*/
            if (VS_TRUE == fGetMPUTrans() && VS_TRUE == fGetMPUCard())
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);

            if (VS_TRUE == fGetMPUTrans() &&
                    (srTransRec.byTransType == VOID ||
                    srTransRec.byTransType == MPU_VOID_PREAUTH ||
                    srTransRec.byTransType == MPU_VOID_PREAUTH_COMP ||
                    srTransRec.byTransType == VOID_PREAUTH)
                    )
                vdCTOS_SetTransEntryMode(CARD_ENTRY_EASY_ICC);


            vdDebug_LogPrintf("--EMV Read succ---- srTransRec.byEntryMode[%d]", srTransRec.byEntryMode);

            //Load the CDT table
            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            break;
        }
#endif

#if 0
        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

            if (d_OK != inCTOS_CheckEMVFallback()) {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                if (1 == chGetIdleEventSC_MSR())
                    return USER_ABORT;

                goto SWIPE_AGAIN;

            }

            break;

        }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if (byMSR_status == d_OK)//Fix for Track2 Len < 35
        {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);
            if (usResult != d_OK) {
                CTOS_KBDBufFlush();
                vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                return USER_ABORT;
            }

            if (d_OK != inCTOS_LoadCDTIndex()) {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if (d_OK != inCTOS_CheckEMVFallback()) {
                vdCTOS_ResetMagstripCardData();
                vdDisplayErrorMsg(1, 8, "PLS INSERT CARD");
                if (1 == chGetIdleEventSC_MSR())
                    return USER_ABORT;

                goto SWIPE_AGAIN;

            }

            break;
        }
#endif
        CTOS_SCStatus(d_SC_USER, &bySC_status);
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

        if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode()) {
            ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

            // V3 contactless reader
            //		EMVCL_StopIdleLEDBehavior(NULL);
            //		EMVCL_SetLED(0x0F, 0x08);

            // patrick test code 20141230 start
#ifndef d_EMVCL_RC_SEE_PHONE
#define d_EMVCL_RC_SEE_PHONE		0xA00000AF // AMEX
#endif
#define d_EMVCL_RC_SEE_PHONE2		0xA00000E4
            // patrick test code 20141230 end		

            if (ulAPRtn == d_EMVCL_RC_DEK_SIGNAL) {
                vdDebug_LogPrintf("DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen, stRCDataEx.baChipData);
            } else if (ulAPRtn == d_EMVCL_RC_SEE_PHONE || d_EMVCL_RC_SEE_PHONE2) {
                //CTOS_LCDTClearDisplay();
                vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
                CTOS_Delay(3000);
                goto EntryOfStartTrans;
            }
            else if (ulAPRtn == d_EMVCL_TX_CANCEL) {
                vdDisplayErrorMsg(1, 8, "USER CANCEL");
                return USER_ABORT;
            } else if (ulAPRtn == d_EMVCL_RX_TIMEOUT) {
                if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                    inCTOSS_CLMCancelTransaction();

                CTOS_Beep();
                CTOS_Delay(50);
                CTOS_Beep();
                CTOS_Delay(50);
                vdDisplayErrorMsg(1, 8, "TIMEOUT");
                return USER_ABORT;
            } else if (ulAPRtn != d_EMVCL_PENDING) {
                CTOS_Beep();
                CTOS_Delay(50);
                CTOS_Beep();
                CTOS_Delay(50);
                CTOS_Beep();
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                break;
            }
        } else {
            memset(szTransType, 0x00, sizeof (szTransType));
            strcpy(szOtherAmt, "000000000000");
            if (srTransRec.byTransType == REFUND)
                strcpy(szTransType, "20");
            else
                strcpy(szTransType, "00");

            vdDebug_LogPrintf("Ctls V3 Trans szTotalAmount=[%s], szOtherAmt=[%s]", szTotalAmount, szOtherAmt);
            ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount, szOtherAmt, szTransType, szCatgCode, szCurrCode, &stRCDataEx);

            vdDebug_LogPrintf("1. Ctls V3 Trans szTotalAmount=[%s], szOtherAmt=[%s]", szTotalAmount, szOtherAmt);

            CTOS_Beep();
            CTOS_Delay(50);
            CTOS_Beep();
            CTOS_Delay(50);
            CTOS_Beep();
            if (ulAPRtn == d_OK) {
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                ulAPRtn = d_EMVCL_RC_DATA;
                break;
            }

            if (ulAPRtn == 155) {
                vduiClearBelow(2);
                vdSetErrorMessage("CARD NOT SUPPORTED");
                return USER_ABORT;
            }
            if (ulAPRtn == d_NO)
                return USER_ABORT;

            if (ulAPRtn == d_MORE_PROCESSING)
                return (inCTOS_GetCardFields());
        }

    }

    if (srTransRec.byEntryMode == CARD_ENTRY_WAVE) {
        if (ulAPRtn != d_EMVCL_RC_DATA) {
            vdCTOSS_WaveCheckRtCode(ulAPRtn);
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            return d_NO;
        }

        if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode()) {
            if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx)) {
                inCTOSS_CLMCancelTransaction();
                return d_NO;
            }
        } else {
            if (d_OK != inCTOSS_V3AnalyzeTransaction(&stRCDataEx))
                return d_NO;
        }

        //Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex()) {
            if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

        if (CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
            inCTOSS_CLMCancelTransaction();

    }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        EMVtagLen = 0;
        if (EMVtagLen > 0) {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        } else {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    } else {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
}

#if 0
#define DE48 "00100110020037000030020302100884937373025006840104026002DD03801219389C99DE9C1000040001101004000110301404146006185446"

char *getTagVal(char* szData, char* szTag);

void main() {
    clrscr();
    //printf("Hello world\n");
    printf("Tag 003: Val:%s\n", getTagVal(DE48, "003"));
    printf("Tag 100: Val:%s\n", getTagVal(DE48, "100"));
    printf("Tag 038: Val:%s\n", getTagVal(DE48, "038"));
    printf("Tag 001: Val:%s\n", getTagVal(DE48, "001"));
    getch();
}
#endif

char *getTagVal(char* szData, char* szTag) {
    int inLen = 0, inTagLen = 0;
    int inIndex = 0;
    int inPos = 0;
    char szTempTag[3 + 1], szTempLen[3 + 1], szTempVal[256 + 1], szVal[256 + 1];

    inLen = strlen(szData);
    for (inIndex = 0; inIndex <= inLen; inIndex++) {
        inPos = 0;
        memset(szTempTag, 0, sizeof (szTempTag));
        memcpy(szTempTag, szData + inIndex, 3);
        inPos += 3;

        //get tag len
        memset(szTempLen, 0, sizeof (szTempLen));
        memcpy(szTempLen, szData + inIndex + inPos, 3);
        inPos += 3;

        inTagLen = atoi(szTempLen);
        memset(szTempVal, 0, sizeof (szTempVal));
        memcpy(szTempVal, szData + inIndex + inPos, inTagLen);

        memset(szVal, 0, sizeof (szVal));
        if (memcmp(szData + inIndex, szTag, 3) == 0) {
            //printf("******FOUND******");
            //printf("Tag:%s\n", szTempTag);
            //printf("szTempLen:%s\n", szTempLen, inTagLen);
            //printf("szTempVal:%s\n", szTempVal);
            memcpy(szVal, szTempVal, inTagLen);
            break;
        }

        inPos += inTagLen;
        inIndex += inPos;
        inIndex--;
    }

    return szVal;
}

int inGetPosition(char* szData, char* szValue, int inValueLen, int inLen) {
    int inIndex = 0; //, inLen=0;
    char fFound = 0;

    //inLen=strlen(szData);
    for (inIndex = 0; inIndex <= inLen; inIndex++) {
        if (memcmp(szData + inIndex, szValue, inValueLen) == 0) {
            fFound = 1;
            printf("Found");
            break;
        }
    }

    if (fFound != 1)
        inIndex = (-1);

    return inIndex;
}

/**
Nyan added
 **/
int inCTOS_DisplayCardInfo(USHORT usCardTypeLine, USHORT usPANLine) {

    char szStr[50 + 1];
    USHORT EmvTagLen;
    BYTE EmvTagVal[64];
    BYTE szTempl[30 + 1];
    BYTE bBuffer[4 + 1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE + 1];

    memset(szStr, 0x00, sizeof (szStr));
    memset(EmvTagVal, 0x00, sizeof (EmvTagVal));

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        EmvTagLen = 0;
    }

    if (0 == strlen(szStr)) {
        sprintf(szStr, "%s", strCDT.szCardLabel);
        vdDebug_LogPrintf("Card label: %s", strCDT.szCardLabel);
    }

    if (strIIT.inIssuerNumber == 6){//((strspn(szStr, strIIT.szIssuerAbbrev) > 0) || (strspn(szStr, "CUP") > 0)) { //modify with strIIT with index
        vdDebug_LogPrintf("PANDISPLAY:abbr: %d ", strIIT.inIssuerNumber);
        vdDebug_LogPrintf("PANDISPLAY:abbr: %s ", strIIT.szIssuerAbbrev);
        
        vdDebug_LogPrintf("PANDISPLAY:label: %s contain UPI/CUP", strCDT.szCardLabel);

        memset(szTempl, 0x00, sizeof (szTempl));
        vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTempl);

        if (0 >= usCardTypeLine || 0 >= usPANLine) {
            usCardTypeLine = 3;
            usPANLine = 4;
        }

        CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);
        CTOS_LCDTPrintXY(1, usPANLine, szTempl);

        memset(bDisplayStr, 0x00, sizeof (bDisplayStr));
        memset(bBuffer, 0x00, sizeof (bBuffer));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuffer[0], 1);
        memcpy(bDisplayStr, bBuffer, 2);
        bDisplayStr[2] = '/';
        memset(bBuffer, 0x00, sizeof (bBuffer));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuffer[0], 1);
        memcpy(bDisplayStr + 3, bBuffer, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - strlen(bDisplayStr)*2, 4, bDisplayStr);
    } else {
        vdDebug_LogPrintf("PANDISPLAY:label: %s not contain UPI", strCDT.szCardLabel);
    }




    /*char szExpire[EXPIRY_DATE_BCD_SIZE+1];
    memset(szExpire, 0x00, sizeof(szExpire));
    wub_hex_2_str(srTransRec.szExpireDate, szExpire, EXPIRY_DATE_BCD_SIZE);
    strcpy(srTransRec.szExpireDate, szExpire);
     */
    //CTOS_LCDTPrintXY(1, usPANLine + 1, szExpire);
    //setLCDPrint(usPANLine + 1, DISPLAY_POSITION_RIGHT, szExpire);

    //setLCDPrint(6, DISPLAY_POSITION_LEFT, strCST.szCurSymbol);
    //CTOS_LCDTPrintAligned(6,szStr,d_LCD_ALIGNRIGHT);

    return (d_OK);
    /*
    char szStr[50 + 1]; 
USHORT EMVtagLen;
BYTE   EMVtagVal[64];
BYTE szTemp1[30+1];
memset(szStr,0x00,sizeof(szStr));
memset(EMVtagVal,0x00,sizeof(EMVtagVal));  
if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
{    
    EMVtagLen = 0;
        
    if(EMVtagLen > 0)
    {
        sprintf(szStr, "%s", EMVtagVal);
        vdDebug_LogPrintf("Card label(str): %s",szStr);       
    }
}

if(0 == strlen(szStr))
{
    sprintf(szStr, "%s", strCDT.szCardLabel);
    vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
}
        
memset(szTemp1,0,sizeof(szTemp1));
if (srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == VOID)
    {
            //vdCTOS_FormatPAN("NNNNNNNNNNNNNNNN", srTransRec.szPAN, szTemp1);
		
            vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
    }
    else
    vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);

if(0>= usCardTypeLine || 0 >= usPANLine)
{
    usCardTypeLine = 3;
    usPANLine = 4;
}

	
CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);
CTOS_LCDTPrintXY(1, usPANLine, szTemp1);

	
	
    //nyan add to show expire date 
	
			
    char szExpire[EXPIRY_DATE_BCD_SIZE+1];
    memset(szExpire, 0x00, sizeof(szExpire));
    wub_hex_2_str(srTransRec.szExpireDate, szExpire, EXPIRY_DATE_BCD_SIZE);
    strcpy(srTransRec.szExpireDate, szExpire);
	
    CTOS_LCDTPrintXY(1, usPANLine + 1, szExpire);
	
    
return(d_OK);
     */

    /*
	
    BYTE bBuf[4+1];
    if (srTransRec.byTransType == VOID)
    {
            memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
    memset(bBuf, 0x00, sizeof(bBuf));
    wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
    memcpy(bDisplayStr, bBuf, 2);
    bDisplayStr[2] = '/';
    memset(bBuf, 0x00, sizeof(bBuf));
    wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
    memcpy(bDisplayStr+3, bBuf, 2);
    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 4, bDisplayStr);
    }
	
     */
}





