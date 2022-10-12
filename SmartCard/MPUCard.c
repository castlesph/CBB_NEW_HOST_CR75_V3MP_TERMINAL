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
#include "..\Htle\htlesrc.h"


#include "MPUCard.h"

typedef struct AID_SUPPORTED {
#define		EMV_MAX_AID_SIZE			 		33      // ASCII rep. of 16 bytes binary

    char szAidSuported[EMV_MAX_AID_SIZE];
} AID_SUPPORTED;


int inMPUInitCardData(void)
{
    char szMessage[29 + 1];
    //unsigned long ulTimerCount;
    int inResult = VS_ERR;
    BYTE bKey = 0x00;

    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 4, "INSERT CARD");

    do {
		CTOS_KBDHit(&bKey);
		
        if (inVXEMVAPCardPresent() == 1) {
            CTOS_LCDTPrintXY(1, 4, "WAITING..   ");
            inResult = shCTOS_MPUAppSelectedProcess();
        } else if (bKey == d_KBD_CANCEL) {
            inResult = VS_ESCAPE;
            return VS_ESCAPE;
        }
    } while (inResult == VS_ERR);

    if (inResult == ERR_SELECT_AID) {
        return (VS_ERR);
    }

    return (VS_SUCCESS);
}

SHORT shCTOS_MPUAppSelectedProcess(void)
{
    short shResult;
    BYTE SelectedAID[16]; 
    USHORT SelectedAIDLen = 0;
    BYTE label[32];
    USHORT label_len = 0;
    BYTE PreferAID[128];
    CTOS_RTC SetRTC;
    int inResult = 0;
	USHORT usResult;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES*2], szTk3Buf[TRACK_III_BYTES];
    USHORT usTk1Len, usTk2Len, usTk3Len;

    unsigned char lc = 0;
    int loop = 0;
    unsigned short lReturn = 0;
    unsigned char szSendBuff[16 + 1], szReceiveBuff[256 + 1];
    char szTempBuff[16 + 1];
    int inHostNum;

    AID_SUPPORTED stAid[11];
    
    vdDebug_LogPrintf("-------MPU_TxnAppSelect-----"); 
    memset(PreferAID,0,sizeof(PreferAID));
    memset(label,0,sizeof(label));

    lc = 0;
    memset(szSendBuff, 0x00, 16);
    memset(szTempBuff, 0x00, 16);

    for (loop = 0; loop <= 10; loop++) {
        memset(stAid[loop].szAidSuported, 0x00, sizeof (stAid[loop].szAidSuported));
    }
    strcpy(stAid[1].szAidSuported, "A0000004820000000000");
    strcpy(stAid[2].szAidSuported, "");
    strcpy(stAid[3].szAidSuported, "");
    strcpy(stAid[4].szAidSuported, "");
    strcpy(stAid[5].szAidSuported, "");
    strcpy(stAid[6].szAidSuported, "");
    strcpy(stAid[7].szAidSuported, "");
    strcpy(stAid[8].szAidSuported, "");
    strcpy(stAid[9].szAidSuported, "");
    strcpy(stAid[10].szAidSuported, "");

	vdDebug_LogPrintf("Card Init"); 
    inResult = inVXEMVAPCardInit();

	if (inResult!=1)
        return ERR_SELECT_AID;

	vdDebug_LogPrintf("Select APP"); 
    for (loop = 1; loop <= 10; loop++) {
        if (strcmp(stAid[loop].szAidSuported, "") != 0) {
            lc = strlen(stAid[loop].szAidSuported) / 2;
            wub_str_2_hex((char *) stAid[loop].szAidSuported, (char*) szSendBuff, strlen(stAid[loop].szAidSuported));
            lReturn = inReadRecordJavaCard(CLA_SELECT, INS_SELECT, P1_SELECT, 0x00, lc, szSendBuff, 0x00, szReceiveBuff);
            if (lReturn == 0x9000) {
                break;
            }
        }
    }

	// fix de55 tags null value for special MPU-UPI gold card (6244190010317982)
#ifdef PIN_CHANGE_ENABLE
	if (fGetCashAdvAppFlag() == TRUE)
	{
		if(srTransRec.byTransType == CASH_ADVANCE || srTransRec.byTransType == CHANGE_PIN)	
			return ERR_SELECT_AID;
	}
#endif

    if (lReturn != 0x9000) {
        vdHTLEDisplayMSG(lReturn);
        return ERR_SELECT_AID;
    }

	vdDebug_LogPrintf("Active Card");
	memset(szSendBuff, 0x00, sizeof(szSendBuff));
	memset(szReceiveBuff, 0x00, sizeof(szReceiveBuff));
	lReturn = inReadRecordJavaCard(0x80, 0xA0, 0x00, 0x00, 0x00, szSendBuff, 0x04, szReceiveBuff);
    if (lReturn != 0x9000) {
        vdHTLEDisplayMSG(lReturn);
        return ERR_SELECT_AID;
    }

	
	vdDebug_LogPrintf("Read Card szTk1Buf");
	memset(szSendBuff, 0x00, sizeof(szSendBuff));
	memset(szReceiveBuff, 0x00, sizeof(szReceiveBuff));
	lReturn = inReadRecordJavaCard(0x80, 0x0C, 0x01, 0x00, 0x00, szSendBuff, 0x80, szTk1Buf);
	if (lReturn != 0x9000) {
		vdHTLEDisplayMSG(lReturn);
	}

	usTk1Len = strlen(szTk1Buf);
	vdPCIDebug_HexPrintf("szTk1Buf", szTk1Buf , usTk1Len);

	
	vdDebug_LogPrintf("Read Card szTk2Buf");
	memset(szSendBuff, 0x00, sizeof(szSendBuff));
	memset(szReceiveBuff, 0x00, sizeof(szReceiveBuff));
	lReturn = inReadRecordJavaCard(0x80, 0x0C, 0x02, 0x00, 0x00, szSendBuff, 0x80, szTk2Buf);
	if (lReturn != 0x9000) {
		vdHTLEDisplayMSG(lReturn);
	}

	usTk2Len = strlen(szTk2Buf);
	vdPCIDebug_HexPrintf("szTk2Buf", szTk2Buf , usTk2Len);

	
	vdDebug_LogPrintf("Read Card szTk3Buf");
	memset(szSendBuff, 0x00, sizeof(szSendBuff));
	memset(szReceiveBuff, 0x00, sizeof(szReceiveBuff));
	lReturn = inReadRecordJavaCard(0x80, 0x0C, 0x03, 0x00, 0x00, szSendBuff, 0x80, szTk3Buf);
	if (lReturn != 0x9000) {
		vdHTLEDisplayMSG(lReturn);
	}

	usTk3Len = strlen(szTk3Buf);
	vdPCIDebug_HexPrintf("szTk3Buf", szTk3Buf , usTk3Len);


	vdCTOS_ResetMagstripCardData();
	usResult = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len); 

    return d_OK;
        
}

int inCTOS_MPUCardReadProcess (void)
{
    short shResult = 0;
    USHORT usMsgFailedResult = 0;
    
	shResult = shCTOS_EMVAppSelectedProcess();
    
    vdDebug_LogPrintf("-------inCTOS_MPUCardReadProcess---[%d]--",shResult); 

    if(shResult == EMV_CHIP_FAILED)
    {
        usMsgFailedResult = MSG_TRANS_ERROR;
        return usMsgFailedResult;
    }
    else if(shResult == EMV_USER_ABORT)
    {
        usMsgFailedResult = MSG_USER_CANCEL;
        return usMsgFailedResult;
    }
    shCTOS_EMVGetChipDataReady();

    return (d_OK);
}

