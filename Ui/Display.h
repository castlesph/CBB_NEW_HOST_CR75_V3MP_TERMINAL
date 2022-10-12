/* 
 * File:   display.h
 * Author: wade
 *
 * Created on 2012年8月11日, 下午 6:08
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DISPLAY_POSITION_LEFT 0
#define DISPLAY_POSITION_CENTER 1
#define DISPLAY_POSITION_RIGHT 2
#define DISPLAY_LINE_SIZE 16

void vdDispErrMsg(IN BYTE *szMsg);
void   vdDispTransTitle(BYTE byTransType);
void vdDispTitleString(BYTE *szTitle);
int    vdDispTransactionInfo(void);
void   setLCDPrint(int line,int position, char *pbBuf);
void   showAmount(IN  BYTE bY, BYTE bStrLen, BYTE *baBuf);
void   szGetTransTitle(BYTE byTransType, BYTE *szTitle);
void   vdDisplayTxnFinishUI(void);
USHORT showBatchRecord(TRANS_DATA_TABLE *strTransData);
void vduiLightOn(void);
void vduiKeyboardBackLight(BOOL fKeyBoardLight);
void vduiPowerOff(void);
void vduiClearBelow(int line);
void vduiDisplayStringCenter(unsigned char  y,unsigned char *sBuf);
void vduiWarningSound(void);
short vduiAskConfirmContinue(void);
BYTE struiGetchWithTimeOut(void); 
void vduiDisplayInvalidTLE(void);
short inuiAskSettlement(void);
void vduiDisplaySignalStrengthBatteryCapacity(void);
USHORT clearLine(int line);
void setLCDPrint27(int line,int position, char *pbBuf);


void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg);
void vdSetErrorMessage(char *szMessage);
int inGetErrorMessage(char *szMessage);
void vdCTOS_DispStatusMessage(char* szMsg);
void vdDisplayErrorMsg2(int inColumn, int inRow,  char *msg, char *msg2);
void inDisplayLeftRight(int inLine, unsigned char *strLeft, unsigned char *strRight, int inMode);


#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

