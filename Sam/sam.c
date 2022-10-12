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

#include <stdio.h>
#include <fcntl.h>

#include <openssl/aes.h>

#include "..\Includes\CTOSInput.h"
#include "..\Includes\POSTypedef.h"
#include "..\ui\Display.h"
#include "..\Debug\Debug.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\POSTrans.h"

#include "sam.h"

#define d_BUFF_SIZE 32
#define VS_ERR -1

// Transaction data to communicate with SAM
unsigned char RwSamNumber[8];
unsigned char Rar[16];
unsigned char Rbr[16];
unsigned char Rcr[4];
unsigned char KYtr[16];	// Transaction Key
unsigned char Snr[4];	// Sequence number

/** AES key for authentication between PC and SAM */
AES_KEY _auth_key;

/** AES key to encrypt transaction packets between PC and SAM */
AES_KEY _tran_key;

/** AES key to generate MAC for transaction between PC and SAM */
AES_KEY _mac_key;


BYTE	bSAMSlot = d_SC_USER;

extern USHORT usSecTimeOut;

extern USHORT usCal_SECUtimeout(UCHAR *bParameter);

int inNETSChangeSAMBaudRate(unsigned long ulSlotNo)
{
	char data_buf[100];
	BYTE bKey = 0x00;
	BYTE baATR[d_BUFF_SIZE], bATRLen, CardType;
	BYTE key;
	BYTE bStatus;

	vdDebug_LogPrintf("inNETSChangeSAMBaudRate");

    CTOS_SCPowerOff(ulSlotNo);

	vdDebug_LogPrintf("CTOS_SCResetISO");

	bATRLen = sizeof (baATR);
	//Power on the ICC and retrun the ATR content metting the ISO-7816 specification //
	if (CTOS_SCResetISO(ulSlotNo, d_SC_5V, baATR, &bATRLen, &CardType) == d_OK) {
		CTOS_LCDTPrintXY(1, 4, "ISO7816 OK");
		return 1;
	} else {
//		CTOS_LCDTPrintXY(1, 4, "ISO7816 Fail");
//		return 0;
	}

	  vdDebug_LogPrintf("CTOS_SCCommonReset");

	  bATRLen = sizeof (baATR);
	//Power on the ICC and retrun the ATR content metting the ISO-7816 specification //
	  if (CTOS_SCCommonReset(ulSlotNo, 0x11, TRUE, TRUE, TRUE, TRUE, d_SC_5V, baATR, &bATRLen, &CardType) == d_OK) {
		CTOS_LCDTPrintXY(1, 4, "RESET OK");
		return 1;
	  } 
//	  else {
//		CTOS_LCDTPrintXY(1, 4, "RESET Fail");
//		  return 0;
//	  }

//	CTOS_Delay(1000);
	bATRLen = sizeof (baATR);
	//Power on the ICC and retrun the ATR contents metting the EMV2000 specification //
	if (CTOS_SCResetEMV(ulSlotNo, d_SC_5V, baATR, &bATRLen, &CardType) == d_OK){
		return 1;
	} else {
//		  return 0;
	}
	  vdDebug_LogPrintf("CTOS_SCResetEMV [%s][%ld][%ld]", baATR, bATRLen, CardType);

	vdDebug_LogPrintf("inNETSChangeSAMBaudRate END");

}

int inNETSStartSAMInit(void)
{
	unsigned char chRetVal;
	int inNFPSAMSlot;
	BYTE szNoKeyBlock;
	unsigned long lnSAMSlotArr[] = {d_SC_SAM1, d_SC_SAM2, d_SC_SAM3, d_SC_SAM4};
	BYTE szAPDUDReq[] = {0x00,0xA4,0x01,0x00,0x02,0x01,0x18};
	BYTE szAPDUEFReq[] = {0x00,0xB0,0x81,0x00,0x24};

	BYTE szAPDURes[200];
	BYTE szAPDUCmd[100];
	unsigned short rxLen,txLen = 0x07;
//	BYTE szProtocol[5];
	int inLen,i;
	BYTE ucChkSum = 0x00;
	BYTE szHex[4];
	char szTemp[10];
	BYTE szArrData[40][4];
//	BYTE szKSI[40];
	BYTE ucSAMStatus;
	char szSAMID[8+1];
	int inResCode = 0;

	vdDebug_LogPrintf("inNETSStartSAMInit");
	
//	memset(szSID,0x00,sizeof(szSID));
//	memset(szOperID,0x00,sizeof(szOperID));
//	memset((BYTE*)&szKSI,0x00,sizeof(Ksi_Rec));
//	memset(szShortName,0x00,sizeof(szShortName));
//	memset((char*)&szArrData,0x00,160);
	
	if(inNETSChangeSAMBaudRate(d_SC_SAM1) != 1)
		return VS_ERR;

	// Select Merchant Card DF
	memset(szAPDURes,0x00,sizeof(szAPDURes));
    memcpy(szAPDUDReq, "\x00\xA4\x01\x00\x02\x01\x18", 7);	
	txLen = 7;
	chRetVal = ucTransmit_APDU(d_SC_SAM1, szAPDUDReq, txLen, szAPDURes, &rxLen);

    // Select Card Information File
	memset(szAPDURes,0x00,sizeof(szAPDURes));
    memcpy(szAPDUDReq, "\x80\xB8\x00\x00\x08", 5);
	txLen = 5;
	chRetVal = ucTransmit_APDU(d_SC_SAM1, szAPDUDReq, txLen, szAPDURes, &rxLen);

    // Read Card Information
	memset(szAPDURes,0x00,sizeof(szAPDURes));
    memcpy(szAPDUDReq, "\x00\xB0\x81\x00\x1C", 5);
	txLen = 5;
	chRetVal = ucTransmit_APDU(d_SC_SAM1, szAPDUDReq, txLen, szAPDURes, &rxLen);



	if(memcmp(szAPDURes,"\x61\x1C",2) != 0)
		return VS_ERR;
	return inResCode;

	vdDebug_LogPrintf("inNETSStartSAMInit END");

}

unsigned char ucTransmit_APDU(unsigned long ulSlotNo, unsigned char *szAPDUDReq, unsigned short txLen, unsigned char *szAPDURes, unsigned short *rxLen) 
{
	unsigned char usRetVal;
    unsigned short pusRespLen = 0;

	vdDebug_LogPrintf("ucTransmit_APDU");

    pusRespLen = 256;
    usRetVal = CTOS_SCSendAPDU(ulSlotNo, szAPDUDReq, txLen, szAPDURes, &pusRespLen);
	vdPCIDebug_HexPrintf("CTOS_SCSendAPDU", szAPDUDReq , txLen);
	vdPCIDebug_HexPrintf("CTOS_SCSendAPDU", szAPDURes , pusRespLen);
	vdPCIDebug_HexPrintf("CTOS_SCSendAPDU", szAPDURes , 10);
	*rxLen = pusRespLen;

	vdDebug_LogPrintf("ucTransmit_APDU END");

    return (usRetVal);
}

void PrintHexArray(BYTE * title, USHORT len, BYTE * hex)
{
	DebugAddHEX(title, hex, len);
}


void vdSetFelicaSAMSlot(BYTE bSlot)
{
	bSAMSlot = bSlot;
	put_env_int(FELICA_SAM, bSAMSlot);
}

BYTE chGetFelicaSAMSlot(void)
{
	bSAMSlot = get_env_int(FELICA_SAM);
	if (bSAMSlot <= 0)
		bSAMSlot = d_SC_USER;
	
	return bSAMSlot;
}
void vdFelica_SAMSlotConfig(void)
{
	BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0;
    int inRet = 0;

	BYTE bSlot = 0;

    vdDebug_LogPrintf("=====vdFelica_SAMSlotConfig=====");

	bSlot = chGetFelicaSAMSlot();

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
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "Felica SAM Slot");
        if(bSlot == 1)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "SAM1");
        else if(bSlot == 2)
            setLCDPrint(4, DISPLAY_POSITION_LEFT, "SAM2"); 
		else
		{
			setLCDPrint(4, DISPLAY_POSITION_LEFT, "SC USER");
			bSlot = 0;
			vdSetFelicaSAMSlot(bSlot);
		}
        
        CTOS_LCDTPrintXY(1, 5, "0-SC USER   1-SAM1");
        CTOS_LCDTPrintXY(1, 6, "2-SAM2");
        
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
            if (strOut[0]==0x30 || strOut[0]==0x31 || strOut[0]==0x32)
            {
                 if(strOut[0] == 0x30)
                 {
					bSlot = 0;
                 }
				 if(strOut[0] == 0x31)
                 {
					bSlot = 1;
                 }
                 if(strOut[0] == 0x32)
                 {
					bSlot = 2;
                 }
                
                 vdSetFelicaSAMSlot(bSlot);
                 
                 break;
             }
             else
             {
                vduiWarningSound();
                vduiDisplayStringCenter(6,"PLEASE SELECT");
                vduiDisplayStringCenter(7,"A VALID");
                vduiDisplayStringCenter(8,"SAM SLOT");
                CTOS_Delay(2000);       
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
       
    return ;
}

USHORT usSAM_ChkCardPresent(void)
{
    USHORT	usRet = d_OK;
	BYTE	bStatus = 0;
	BYTE	bSlotID = bSAMSlot; // change the slot id for SAM card.

	bSlotID = chGetFelicaSAMSlot();

	vdDebug_LogPrintf("====usChkSAMCardPresent==== Slot[%d]", bSAMSlot);
	
    usRet = CTOS_SCStatus(bSlotID, &bStatus);
	
	vdDebug_LogPrintf("CTOS_SCStatususRet[%04X]", usRet);
    if (bStatus & d_MK_SC_PRESENT)
		usRet = 1;
	
	vdDebug_LogPrintf("CTOS_SCStatususRet return[%d]", usRet);
	return usRet;
}

USHORT usSAM_CardSlotInit(void)
{
    char data_buf[100];
    BYTE bKey = 0x00;
    BYTE baATR[d_BUFF_SIZE], bATRLen, CardType;

    bATRLen = sizeof (baATR);

	vdDebug_LogPrintf("=====usSAM_CardSlotInit=====");
	
    //Power on the ICC and retrun the ATR contents metting the EMV2000 specification //
    if (CTOS_SCResetEMV(bSAMSlot, d_SC_3V, baATR, &bATRLen, &CardType) == d_OK)
	{
		vdDebug_LogPrintf("CTOS_SCResetEMV OK");
		return d_OK;
    }
	else
    {
    	vdDebug_LogPrintf("CTOS_SCResetEMV fail");
    }

    CTOS_Delay(500);

    bATRLen = sizeof (baATR);
    //Power on the ICC and retrun the ATR content metting the ISO-7816 specification //
    if (CTOS_SCResetISO(bSAMSlot, d_SC_3V, baATR, &bATRLen, &CardType) == d_OK) {
		CTOS_LCDTPrintXY(1, 4, "ISO7816 OK");
		vdDebug_LogPrintf("ISO7816 OK");
		return d_OK;
    } else {
     	CTOS_LCDTPrintXY(1, 4, "ISO7816 Fail");
		vdDebug_LogPrintf("ISO7816 fail");
        return d_NO;
    }

	return d_NO;
}


long TransmitDataToSAM(unsigned char samCmdBuf[], unsigned long samCmdLen, unsigned char samResBuf[], unsigned long* samResLen)
{
	long _ret = 0;
	unsigned char _send_buf[262];
	unsigned long _send_len;
	unsigned char tmpBuf[262];
	unsigned short tmpBufLen;
	unsigned char sw1, sw2;
	unsigned int i;

	USHORT usRet = 0;
	extern USHORT usSecTimeOut;

	_send_buf[0] = 0xA0; //CLA
	_send_buf[1] = 0x00; //INS
	_send_buf[2] = 0x00; //P1
	_send_buf[3] = 0x00; //P2
	_send_buf[4] = (unsigned char)samCmdLen; //Lc
	for( i=0; i<samCmdLen; i++) {
		_send_buf[i+5] = samCmdBuf[i];
	}
	_send_buf[samCmdLen +5] = 0x00; //Le
	_send_len = samCmdLen + 6;
	tmpBufLen = *samResLen;

	PrintHexArray("PC->SAM: ", _send_len, _send_buf);

	#if 0
	_ret = SCardTransmit(hCardSAM,
		NULL,
		_send_buf,
		_send_len,
		NULL,  
		tmpBuf,
		&tmpBufLen);
	#endif
	usRet = CTOS_SCSendAPDU(bSAMSlot, _send_buf, _send_len, tmpBuf, &tmpBufLen);

	if( usRet != d_OK )
	{
		vdDebug_LogPrintf("SCardTransmit Error\n");
		return APP_ERROR;
	}

	_ret = usRet;

	PrintHexArray("SAM->PC: ", tmpBufLen, tmpBuf);

	// Calc the Sec Polling timeout
	usSecTimeOut = usCal_SECUtimeout(tmpBuf);
	vdDebug_LogPrintf("cal_SECUtimeout usTimeOut[%d]", usSecTimeOut);

	sw1 = tmpBuf[tmpBufLen - 2];
	sw2 = tmpBuf[tmpBufLen - 1];

	if( (sw1 == 0x67) && (sw2 == 0x00) ){
		return RCS500_LCLE_ERROR;
	}
	if( (sw1 == 0x6A) && (sw2 == 0x86) ){
		return RCS500_P1P2_ERROR;
	}
	if( (sw1 == 0x6D) && (sw2 == 0x00) ){
		return RCS500_INS_ERROR;
	}
	if( (sw1 == 0x6E) && (sw2 == 0x00) ){
		return RCS500_CLA_ERROR;
	}

	// Remove SW1/SW2
	*samResLen = tmpBufLen-2;
	for(i=0; i<*samResLen; i++ ) {
		samResBuf[i] = tmpBuf[i];
	}

	if( (sw1 == 0x90) && (sw2 == 0x00) ){
		return SCARD_S_SUCCESS;
	}

	return UNKOWN_ERROR;
}


long _send_SetNormalMode(unsigned char samResBuf[], unsigned long* samResLen);
long _send_Attention(unsigned char samResBuf[], unsigned long* samResLen);
long _send_Auth1(unsigned char samResBuf[], unsigned long* samResLen);
long _check_Auth1_result(unsigned char samResBuf[], unsigned long samResLen);
long _send_Auth2(unsigned char samResBuf[], unsigned long* samResLen);
long _check_Auth2_result(unsigned char samResBuf[], unsigned long samResLen);

void _calc_mac(	unsigned char key[16], unsigned char msg[], unsigned long size, unsigned char mac[16]);
void encrypt_payload (unsigned char commandCode, unsigned char subCommandCode, 
						  unsigned long felicaCmdParamsLen, unsigned char felicaCmdParams[],
						  unsigned char payload[], unsigned char mac[8]);
long _decrypt_sam_response(unsigned char samResponse[], unsigned int samResLen, unsigned char plainPackets[]);

void _get_error_reason(unsigned char samResBuf[], unsigned long* samResLen);



//========================================
// Public functions
//========================================


long InitializeSAM(void)
{
	long			_ret = 0;
	unsigned char	_sam_res[262];
	unsigned long	_sam_res_len;

	PrintText("Initialize SAM");

	//Check SAM card slot
	_ret = usSAM_ChkCardPresent();
	if( _ret != 1 )
	{
		PrintText("SAM Not Present");
		vdDisplayErrorMsg(0, 8, "SAM Not Present");
		return APP_ERROR;
	}

	//Power on card
	_ret = usSAM_CardSlotInit();
	if( _ret != APP_SUCCESS )
	{
		vdDisplayErrorMsg(0, 8, "SAM Init Error");
		return APP_ERROR;
	}

	//Send Set RWSAM Mode command to RC-S500
	PrintText("Send RWSAM Mode");
	_ret = _send_SetNormalMode(_sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS ){
		PrintText(" -> Error");
		return APP_ERROR;
	}

	//Send Attention command to RC-S500
	PrintText("Send Attention");
	_ret = _send_Attention(_sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS ){
		PrintText(" -> Error");
		return APP_ERROR;
	}

	//Set RW SAM ID
	memcpy(RwSamNumber, &_sam_res[4], 8);

	//Send Authentication 1 to RC-S500
	PrintText("Send Auth1");
	_ret = _send_Auth1(_sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS ) {
		PrintText(" -> Error");
		return APP_ERROR;
	}

	//Check Authentication 1 response
	_ret = _check_Auth1_result(_sam_res, _sam_res_len);
	if( _ret != APP_SUCCESS ){
		PrintText(" -> Error");
		return APP_ERROR;
	}

	//Send Authentication 2 to RC-S500
	PrintText("Send Auth2");
	_ret = _send_Auth2(_sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS ) {
		PrintText(" -> Error");
		return APP_ERROR;
	}

	//Check Authentication 2 response
	_ret = _check_Auth2_result(_sam_res, _sam_res_len);
	if( _ret != APP_SUCCESS ) {
		PrintText(" -> Error");
		return APP_ERROR;
	}

	return APP_SUCCESS;
}

long AskFeliCaCmdToSAMSC(unsigned char commandCode, 
					   unsigned char subCommandCode, 
					   unsigned long felicaCmdParamsLen,
					   unsigned char felicaCmdParams[],
					   unsigned long* felicaCommandLen,
					   unsigned char felicaCommand[])
{
	long			_ret;
	unsigned char	_payload[262], _mac[8], _send_buf[262], _sam_res[262];
	unsigned long	_send_len, _sam_res_len;

	// Encrypt command payload data
	encrypt_payload(commandCode, subCommandCode, felicaCmdParamsLen, felicaCmdParams, _payload, _mac);

	//construct command packet sent to SAM
	_send_buf[0] = 0x00;			// Dispatcher
	_send_buf[1] = 0x00;			// Reserved
	_send_buf[2] = 0x00;			// Reserved
	_send_buf[3] = commandCode;		// Command Code
	_send_buf[4] = subCommandCode;	// Sub Command Code
	_send_buf[5] = 0x00;			// Reserved
	_send_buf[6] = 0x00;			// Reserved
	_send_buf[7] = 0x00;			// Reserved
	memcpy(&_send_buf[8], Snr, 4);	// Snr
	memcpy(&_send_buf[8+4], _payload, felicaCmdParamsLen);	// encrypted data
	memcpy(&_send_buf[8+4+felicaCmdParamsLen], _mac, 8);	// encrypted MAC
	_send_len = 8 + 4 + felicaCmdParamsLen + 8;
	_sam_res_len = 0xFF;

	// Send packets to SAM
	_ret = TransmitDataToSAM(_send_buf, _send_len, _sam_res, &_sam_res_len);
	if(_ret != APP_SUCCESS){
		return APP_ERROR;
	}

	// Extract FeliCa command packets from SAM response
	*felicaCommandLen = _sam_res_len - 3;
	memcpy(felicaCommand, &_sam_res[3], *felicaCommandLen);

	return APP_SUCCESS;
}


long AskFeliCaCmdToSAM(unsigned char commandCode, 
					   unsigned long felicaCmdParamsLen,
					   unsigned char felicaCmdParams[],
					   unsigned long* felicaCommandLen,
					   unsigned char felicaCommand[])
{
	return AskFeliCaCmdToSAMSC(commandCode,
							 0x00,
							 felicaCmdParamsLen,
							 felicaCmdParams,
							 felicaCommandLen,
							 felicaCommand );
}


long SendRWSAMCmd( unsigned char commandCode,
				   unsigned char subcommndCode,
				   unsigned long felicaCmdParamsLen,
				   unsigned char felicaCmdParams[],
				   unsigned long* felicaCommandLen,
				   unsigned char felicaCommand[])
{
	unsigned char	_sam_res[262];
	unsigned long	_sam_res_len;
	long _ret;

	_ret = AskFeliCaCmdToSAMSC(commandCode, subcommndCode, felicaCmdParamsLen, felicaCmdParams, &_sam_res_len, _sam_res);
	if( _ret != APP_SUCCESS )
	{
		return _ret;
	}

	// decrypt and verify SAM response
	_ret = _decrypt_sam_response(_sam_res, _sam_res_len,felicaCommand );
	*felicaCommandLen = _sam_res_len - (1+1+3+4) - 8;

	return _ret;	

}

long SendPollingResToSAM(unsigned long felicaResLen,
						 unsigned char felicaResponse[] )
{
	long			_ret = 0;
	unsigned char	_send_buf[262], _sam_res[262];
	unsigned long	_send_len, _sam_res_len;
	unsigned char	_result[256];

	//Send back the response from FeliCa Card to RW-SAM(RC-S500)
	_send_buf[0] =			0x01;						// Dispatcher
	_send_buf[1] =			0x00;						// Reserved
	_send_buf[2] =			0x00;						// Reserved
	_send_buf[3] =			0x01;						// Reserved
	_send_buf[4] =			0x01;						// Number of Target 1<=n<=4
	memcpy(&_send_buf[5],	&felicaResponse[1], 8);		// IDm
	memcpy(&_send_buf[5+8],	&felicaResponse[1+8], 8);	// PMm
	_send_len = 5 + 8 + 8;
	_sam_res_len = 0xFF;
	
	vdDebug_LogPrintf("======SendPollingResToSAM=====");

	// Send packets to SAM
	_ret = TransmitDataToSAM(_send_buf, _send_len, _sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS )
		return _ret;

	//Check Result
	_ret = _decrypt_sam_response(&_sam_res[1+2], _sam_res_len-(1+2), _result);
	vdDebug_LogPrintf("_decrypt_sam_response _ret[%d]", _ret);

	return _ret;
}

long SendAuth1V2ResultToSAM(unsigned long felicaResLen, 
						 unsigned char felicaResponse[], 
					   unsigned long* auth2V2CommandLen,
					   unsigned char auth2V2Command[])
{
	long			_ret = 0;
	unsigned char	_send_buf[262], _sam_res[262];
	unsigned long	_send_len, _sam_res_len;

	//Send back the response from FeliCa Card to RW-SAM(RC-S500)
	_send_buf[0] =			0x01;	// Dispatcher
	_send_buf[1] =			0x00;	// Reserved
	_send_buf[2] =			0x00;	// Reserved
	memcpy(&_send_buf[3],	felicaResponse, felicaResLen); //response of FeliCa Card
	_send_len = 3 + felicaResLen;
	_sam_res_len = 0xFF;

	// Send packets to SAM
	_ret = TransmitDataToSAM(_send_buf, _send_len, _sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS )
	{
		PrintText("Card result Error\n");
		return APP_ERROR;
	}

	*auth2V2CommandLen = _sam_res_len - 3;
	memcpy(auth2V2Command, &_sam_res[3], *auth2V2CommandLen);

	return _ret;
}


long SendRegistertResultToSAM(unsigned long felicaResLen, 
								   unsigned char felicaResponse[], 
								   unsigned long* chgsysblkV2CommandLen,
								   unsigned char chgsysblkV2Command[])
{
	long			_ret = 0;
	unsigned char	_send_buf[262], _sam_res[262];
	unsigned long	_send_len, _sam_res_len;

	//Send back the response from FeliCa Card to RW-SAM(RC-S500)
	_send_buf[0] =			0x01;	// Dispatcher
	_send_buf[1] =			0x00;	// Reserved
	_send_buf[2] =			0x00;	// Reserved
	memcpy(&_send_buf[3],	felicaResponse, felicaResLen); //response of FeliCa Card
	_send_len = 3 + felicaResLen;
	_sam_res_len = 0xFF;

	// Send packets to SAM
	_ret = TransmitDataToSAM(_send_buf, _send_len, _sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS )
	{
		PrintText("Card result Error\n");
		return APP_ERROR;
	}

	*chgsysblkV2CommandLen = _sam_res_len - 3;
	memcpy(chgsysblkV2Command, &_sam_res[3], *chgsysblkV2Command);

	return _ret;
}

long SendCardResultToSAM(unsigned long felicaResLen, 
						 unsigned char felicaResponse[], 
						 unsigned long* resultLen, 
						 unsigned char result[])
{
	long			_ret = 0;
	unsigned char	_send_buf[262], _sam_res[262];
	unsigned long	_send_len, _sam_res_len;

	//Send back the response from FeliCa Card to RW-SAM(RC-S500)
	_send_buf[0] =			0x01; // Dispatcher
	_send_buf[1] =			0x00; // Reserved
	_send_buf[2] =			0x00; // Reserved
	memcpy(&_send_buf[3],	felicaResponse, felicaResLen);	//response of FeliCa Card
	_send_len = 3 + felicaResLen;
	_sam_res_len = 0xFF;

	// Send packets to SAM
	_ret = TransmitDataToSAM(_send_buf, _send_len, _sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS )
	{
		PrintText("Card result Error\n");
		return APP_ERROR;
	}

	// decrypt and verify SAM response
	_ret = _decrypt_sam_response(&_sam_res[1+2], _sam_res_len-(1+2), result);
	*resultLen = _sam_res_len - (1+2+1+1+3+4) - 8;

	return _ret;
}

long SendRegisterResultToSAM(unsigned long felicaResLen, 
								   unsigned char felicaResponse[], 
								   unsigned long* chgsysblkV2CommandLen,
								   unsigned char chgsysblkV2Command[])
{
	long			_ret = 0;
	unsigned char	_send_buf[262], _sam_res[262];
	unsigned long	_send_len, _sam_res_len;

	//Send back the response from FeliCa Card to RW-SAM(RC-S500)
	_send_buf[0] =			0x01;	// Dispatcher
	_send_buf[1] =			0x00;	// Reserved
	_send_buf[2] =			0x00;	// Reserved
	memcpy(&_send_buf[3],	felicaResponse, felicaResLen); //response of FeliCa Card
	_send_len = 3 + felicaResLen;
	_sam_res_len = 0xFF;

	// Send packets to SAM
	_ret = TransmitDataToSAM(_send_buf, _send_len, _sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS )
	{
		PrintText("Card result Error\n");
		return APP_ERROR;
	}

	*chgsysblkV2CommandLen = _sam_res_len - 3;
	memcpy(chgsysblkV2Command, &_sam_res[3], *chgsysblkV2Command);

	return _ret;
}

long SendCardErrorToSAM( unsigned long* resultLen, 
						 unsigned char result[])
{
	long			_ret = 0;
	unsigned char	_send_buf[262], _sam_res[262];
	unsigned long	_send_len, _sam_res_len;

	// "card No Response packet"
	_send_buf[0] = 0x01; // Dispatcher
	_send_buf[1] = 0x00; // Reserved
	_send_buf[2] = 0x00; // Reserved

	_send_len = 3;
	_sam_res_len = 0xFF;

	// Send packets to SAM
	_ret = TransmitDataToSAM(_send_buf, _send_len, _sam_res, &_sam_res_len);
	if( _ret != APP_SUCCESS )
	{
		return APP_ERROR;
	}

	return APP_SUCCESS;
}




//========================================
// private functions
//========================================


/**
Send SET MODE command to SAM.

\return PC/SC API Return Values
*/
long _send_SetNormalMode(unsigned char samResBuf[], unsigned long* samResLen)
{
	long			_ret = 0;
	unsigned char	_send_buf[262];
	unsigned long	_send_len;

	//Send Set RWSAM Mode command
	_send_buf[0] = 0x00; // Dispatcher
	_send_buf[1] = 0x00; // Reserved
	_send_buf[2] = 0x00; // Reserved
	_send_buf[3] = 0xE6; // Command code
	_send_buf[4] = 0x02; // Sub command code
	_send_buf[5] = 0x02; // RW SAM mode(0x02: Normal Mode)

	_send_len = 6;
	*samResLen = 0xFF; // Expect the length of response (Data + SW1 +SW2)

	_ret = TransmitDataToSAM(_send_buf, _send_len, samResBuf, samResLen);

	return _ret;
}

/**
Send Attention command to SAM.

\return PC/SC API Return Values
*/
long _send_Attention(unsigned char samResBuf[], unsigned long* samResLen)
{
	long			_ret = 0;
	unsigned char	_send_buf[262];
	unsigned long	_send_len;

	//Send Attention Command
	_send_buf[0] = 0x00; // Dispatcher
	_send_buf[1] = 0x00; // Reserved
	_send_buf[2] = 0x00; // Reserved
	_send_buf[3] = 0x00; // Command code
	_send_buf[4] = 0x00; // Reserved
	_send_buf[5] = 0x00; // Reserved

	_send_len = 6;
	*samResLen = 24; // Response Data + SW1 +SW2

	_ret = TransmitDataToSAM(_send_buf, _send_len, samResBuf, samResLen);

	return _ret;
}

/**
Send Authentication 1 command to RC-S500.

\return PC/SC API Return Values
*/
long _send_Auth1(unsigned char samResBuf[], unsigned long* samResLen)
{
	long			_ret = 0;
	unsigned char	_send_buf[262];
	unsigned long	_send_len;

	//Send Authentication1 command
	_send_buf[0] = 0x00; // Dispatcher
	_send_buf[1] = 0x00; // Reserved
	_send_buf[2] = 0x00; // Reserved
	_send_buf[3] = 0x02; // Command code
	_send_buf[4] = 0x00; // Reserved
	_send_buf[5] = 0x00; // Reserved
	
	//Set RW SAM ID
	memcpy(&_send_buf[6], RwSamNumber, 8);

	//Generate Rar
	//GenerateRandom(Rar,16);
	CTOS_RNG(Rar);
	CTOS_RNG(&Rar[8]);

	//Set Rar
	memcpy(&_send_buf[6+8], Rar, 16);

	_send_len = 6 + 8 + 16;
	*samResLen = 1+2+1+32+4 + 2; // Response Data(1+2+1+32+4) + SW1 SW2

	_ret = TransmitDataToSAM(_send_buf, _send_len, samResBuf, samResLen);

	return _ret;
}

/**
Check Authentication 1 response of SAM

\return  APP_SUCCESS, APP_ERROR
*/
long _check_Auth1_result(unsigned char samResBuf[], unsigned long samResLen)
{
	unsigned char	_Kab[16];
	unsigned char	_received_Rar[16];
	unsigned char	_encrypted_M2r[32];
	unsigned char	_decrypted_M2r[32];
	unsigned char	_iv[16];
	int i = 0;
	
	//extract M2r(encrypted)
	memcpy(_encrypted_M2r, &samResBuf[4], 32);

	//extract Rcr
	memcpy(Rcr, &samResBuf[4+32], 4);

	// generate Kab
	for(i=0; i<4; i++)
		_Kab[i] = AuthNormalKey[i] ^ Rcr[i];
	for(i=4; i<16; i++)
		_Kab[i] = AuthNormalKey[i];

	//decrypt M2r
	AES_set_decrypt_key(_Kab, 128, &_auth_key);
	memset (_iv, 0, sizeof (_iv));
    memset (_decrypted_M2r, 0, 32);
    AES_cbc_encrypt (_encrypted_M2r, _decrypted_M2r, 32, &_auth_key, _iv, AES_DECRYPT);

	//extract Rar, Rbr
	memcpy(Rbr, _decrypted_M2r, 16);				// Rbr
	memcpy(_received_Rar, &_decrypted_M2r[16], 16);	// Rar

	//compare Rar
	for (i=0; i<16; i++){
		if(Rar[i] != _received_Rar[i]){
			return APP_ERROR;
		}
	}

	return APP_SUCCESS;
}


/**
Send Authentication 2 command to SAM.

\return PC/SC API Return Values
*/
long _send_Auth2(unsigned char samResBuf[], unsigned long* samResLen)
{
 	long			_ret = 0;
	unsigned char	_buf[32];
	unsigned char	_Kab[16];
	unsigned char	_iv[16];
	unsigned char	_M3r[32];
	unsigned char	_send_buf[262];
	unsigned long	_send_buf_len;
	int i = 0;

	// concatinate Rar, Rbr
	memcpy(&_buf[0], Rar, 16);
	memcpy(&_buf[16], Rbr, 16);

	// generate Kab
	for(i=0; i<4; i++)
		_Kab[i] = AuthNormalKey[i] ^ Rcr[i];
	for(i=4; i<16; i++)
		_Kab[i] = AuthNormalKey[i];
	
	//encrypt M3r
	AES_set_encrypt_key(_Kab, 128, &_auth_key);
	memset (_iv, 0, sizeof (_iv));
    memset (_M3r, 0, 32);
    AES_cbc_encrypt (_buf, _M3r, 32, &_auth_key, _iv, AES_ENCRYPT);

	//Send Authentication2 command
	_send_buf[0] =			0x00;				// Dispatcher
	_send_buf[1] =			0x00;				// Reserved
	_send_buf[2] =			0x00;				// Reserved
	_send_buf[3] =			0x04;				// Command code
	_send_buf[4] =			0x00;				// Reserved
	_send_buf[5] =			0x00;				// Reserved
	memcpy(&_send_buf[6],	RwSamNumber, 8);	// RW SAM ID
	memcpy(&_send_buf[6+8], _M3r, 32);			// M3r

	_send_buf_len = 6 + 8 + 32;
	*samResLen = 1+2+1+1 + 2; //Response Data(1+2+1+1) + SW1 SW2

	_ret = TransmitDataToSAM(_send_buf, _send_buf_len, samResBuf, samResLen);

	return _ret;
}

/**
Check Authentication 2 response of SAM.

\return  APP_SUCCESS, APP_ERROR
*/
long _check_Auth2_result(unsigned char samResBuf[], unsigned long samResLen)
{
	if(samResBuf[4] != 0x00 ){
		return APP_ERROR;
	}

	// Initialize Snr (Initial value for RC-S500 driver is 1 )
	Snr[0] = 0x01;
	Snr[1] = 0x00;
	Snr[2] = 0x00;
	Snr[3] = 0x00;

	// Set KYtr
	memcpy(KYtr, Rbr, 16);

	return APP_SUCCESS;
}


/**
Calculate CBC-MAC of message.

\param [in] key		MAC key
\param [in] msg		message data to calculate MAC
\param [in] msgLen	length of "msg" 
\param [out] MAC	calculated MAC
*/
void _calc_mac(	unsigned char key[16], unsigned char msg[], unsigned long msgLen, unsigned char MAC[16])
{
	unsigned long	_enc_size;
	unsigned char	_last_block[16];
	unsigned char	_encypted_msg[16] = {0};
	AES_KEY			_aes_key;
	unsigned long i;
		
	AES_set_encrypt_key(key, 128, &_aes_key);

	// Copy last block data for Zero Padding
	if(msgLen == 0){
		return;
	}else if(msgLen % 16 == 0){
		_enc_size = msgLen - 16;
		memcpy(_last_block, &msg[_enc_size], 16);
	}else{
		_enc_size = (msgLen / 16) * 16;
		memset(_last_block, 0x00, 16);
		memcpy(_last_block, &msg[_enc_size], msgLen % 16);
	}
	
	// CBC encryption
	for(i = 0; i < _enc_size; i += 16){
		AES_cbc_encrypt(&msg[i], _encypted_msg, 16, &_aes_key, _encypted_msg, AES_ENCRYPT);
	}
	AES_cbc_encrypt(_last_block, MAC, 16, &_aes_key, _encypted_msg, AES_ENCRYPT);
	
	return;
}


/**
Encrypt command payload to SAM.

\param commandCode				Command code for SAM
\param subCommandCode			Sub command code for SAM
\param [in] felicaCmdParamsLen	length of "felicaCmdParams"
\param [in] felicaCmdParams		command parameters after Snr of command gereration request packtes to SAM
\param [out] payload			encrypted payload to SAM
\param [out] mac				encrypted MAC to SAM

\return  APP_SUCCESS, APP_ERROR
*/
void encrypt_payload (unsigned char commandCode, 
						  unsigned char subCommandCode, 
						  unsigned long felicaCmdParamsLen,
						  unsigned char felicaCmdParams[],
						  unsigned char payload[],
						  unsigned char	mac[8])
{
	unsigned char	_b0[16], _b1[16], _raw_mac[16], _ctr_block[16], _iv[16], _work_buf[256];
	AES_KEY			_aes_key;
	unsigned int	_num;

//#ifdef DEBUG
	PrintHexArray("PC->SAM [Raw Payload]: ", felicaCmdParamsLen, felicaCmdParams);
//#endif

	// Create B0
	_b0[0] =			0x59;		// Flags
	memcpy(&_b0[1],		Snr, 4);	// Snr
	memcpy(&_b0[1+4],	Rcr, 4);	// Rcr
	memcpy(&_b0[1+4+4], Rar, 5);	// Rar
	_b0[14] = (unsigned char)(felicaCmdParamsLen / 0x0100);	// Packet data size
	_b0[15] = (unsigned char)(felicaCmdParamsLen % 0x0100);	// Packet data size

	// Create B1
    _b1[0] =				0x00;			// Associated Data Size
    _b1[1] =				0x09;			// Associated Data Size
	_b1[2] =				commandCode;	// Command Code
	_b1[2+1] =				subCommandCode;	// Sub Command Code
	_b1[2+1+1] =			0x00;			// Reserved
	_b1[2+1+1+1] =			0x00;			// Reserved
	_b1[2+1+1+2] =			0x00;			// Reserved
	memcpy(&_b1[2+1+1+3],	Snr, 4);		// Snr
	memset(&_b1[2+1+1+3+4],	0x00, 5);		// All Zero	

	// Generate Ctr1
	_ctr_block[0] =				0x01;		// Flags
	memcpy(&_ctr_block[1],		Snr, 4);	// Snr
	memcpy(&_ctr_block[1+4],	Rcr, 4);	// Rcr
	memcpy(&_ctr_block[1+4+4],	Rar, 5);	// Rar
	_ctr_block[14] =			0x00;		// i
    _ctr_block[15] =			0x01;		// i

	// Encrypt packet data
	memset(_iv, 0x00, 16);
	memset(payload, 0x00, 256);
	_num = 0;
	AES_set_encrypt_key(KYtr, 128, &_aes_key);
	AES_ctr128_encrypt(felicaCmdParams, payload, felicaCmdParamsLen, &_aes_key, _ctr_block, _iv, &_num);

    // Calc CBC-MAC
	memset(_work_buf, 0x00, 256);
	memcpy(&_work_buf[0],		_b0, 16);
	memcpy(&_work_buf[16],		_b1, 16);
    memcpy(&_work_buf[16+16],	felicaCmdParams, felicaCmdParamsLen);
	_calc_mac(KYtr, _work_buf, 16+16+felicaCmdParamsLen, _raw_mac);
	
	// Encrypt MAC
    _ctr_block[14] = 0x00;
    _ctr_block[15] = 0x00;
	_num = 0;
	AES_ctr128_encrypt(_raw_mac, mac, 8, &_aes_key, _ctr_block ,_iv, &_num);
}

/**
Decrypt SAM response packets and verify MAC value.

\param [in] samResponse		SAM response packets (starts with response code, sub response code, ...) 
\param [in] samResLen		length of "samResponse" 
\param [out] plainPackets	decrypted response data (packets after Snr)\n
							Lenght of palinPackets is samResLen -1-1-3-4(RC to Snr) - 8(MAC) = samResLen - 17

\return  APP_SUCCESS, APP_ERROR
*/
long _decrypt_sam_response(unsigned char samResponse[], unsigned int samResLen, unsigned char plainPackets[]) {
	unsigned char	_received_snr[4];
	unsigned int	_snr_value, _received_snr_value;
	unsigned int	_enc_data_len;

	unsigned char	_b0[16], _b1[16],  _ctr_block[16], _iv[16], _work_buf[256];
	unsigned char	_mac[16], _received_mac[8];
	unsigned int	_num;
	AES_KEY			_aes_key;
	
	// Extract Snr
	memcpy(_received_snr, &samResponse[1+1+3], 4);

	// Calc encrypted packtes data length
	_enc_data_len = samResLen - (1+1+3+4) - 8;

	// Generate Ctr1
	_ctr_block[0] =				0x01;				// Flags
	memcpy(&_ctr_block[1],		_received_snr, 4);	// Snr
	memcpy(&_ctr_block[1+4],	Rcr, 4);			// Rcr
	memcpy(&_ctr_block[1+4+4],	Rar, 5);			// Rar
	_ctr_block[14] =			0x00;				// i
    _ctr_block[15] =			0x01;				// i

	// Decrypt packet data
	memset(_iv, 0x00, 16);
	memset(_work_buf, 0x00, 256);
	_num = 0;
	AES_set_encrypt_key(KYtr, 128, &_aes_key);
	AES_ctr128_encrypt(&samResponse[1+1+3+4], plainPackets, _enc_data_len, &_aes_key, _ctr_block, _iv, &_num);

#ifdef DEBUG
	PrintHexArray("SAM->PC [Raw Payload]: ", _enc_data_len, plainPackets);
#endif

	// Decrypt MAC
	memset(_iv, 0x00, 16);
	_ctr_block[14] = 0x00;
    _ctr_block[15] = 0x00;
	_num = 0;
	AES_ctr128_encrypt(&samResponse[samResLen-8], _received_mac, 8, &_aes_key, _ctr_block, _iv, &_num);

	// Create B0
	_b0[0] =			0x59;							// Flags
	memcpy(&_b0[1],		_received_snr, 4);				// Snr
	memcpy(&_b0[1+4],	Rcr, 4);						// Rcr
	memcpy(&_b0[1+4+4],	Rar, 5);						// Rar
	_b0[14] = (unsigned char)(_enc_data_len / 0x0100);	// Packet data size
	_b0[15] = (unsigned char)(_enc_data_len % 0x0100);	// Packet data size

	// Create B1
    _b1[0] =				0x00;					// Associated Data Size
    _b1[1] =				0x09;					// Associated Data Size
	memcpy(&_b1[2],			samResponse, 1+1+3+4);	// Response Code, Sub Response Code, Reserved(3), Snr(4)
	memset(&_b1[2+1+1+3+4],	0x00, 5);				// All Zero	

    // Calc CBC-MAC
	memset(_work_buf, 0x00, 256);
	memcpy(&_work_buf[0],		_b0, 16);
	memcpy(&_work_buf[16],		_b1, 16);
    memcpy(&_work_buf[16+16],	plainPackets, _enc_data_len);
	_calc_mac(KYtr, _work_buf, 16+16+_enc_data_len, _mac);

	// Verify Snr
	_received_snr_value = CharArrayToIntLE(_received_snr, 4);
	_snr_value = CharArrayToIntLE(Snr, 4);
	if ( _received_snr_value != _snr_value + 1 )
		return APP_ERROR;

	// Update Snr
	_snr_value += 2;
	IntToCharArrayLE(_snr_value, Snr);

    // Verify MAC
	if ( memcmp(_received_mac, _mac, 8) == 0 )
		return APP_SUCCESS;
	else
		return APP_ERROR;
}



//////////////DEBUG purpose function. Not in use
void _get_error_reason(unsigned char samResBuf[], unsigned long* samResLen)
{
	long _ret;
	unsigned char _send_buf[262];
	unsigned long _send_len;

	_send_buf[0] = 0x00; //Dispatcher
	_send_buf[1] = 0x00; //Reserved
	_send_buf[2] = 0x00; //Reserved
	_send_buf[3] = 0x28; //Command code
	_send_buf[4] = 0x00; //Reserved
	_send_buf[5] = 0x00; //Reserved
	_send_len = 6;
	*samResLen = 8; //Response Data + SW1 +SW2

	_ret = TransmitDataToSAM(_send_buf, _send_len, samResBuf, samResLen);
	if( _ret != APP_SUCCESS )
	{
		return;
	}
}


void vdSAMTest(void)
{
	USHORT usRet = d_OK;
	
	usRet = usSAM_ChkCardPresent();
	if( usRet != 1 )
	{
		return;
	}
	
	usRet = usSAM_CardSlotInit();
	if( usRet != APP_SUCCESS )
	{
		return;
	}

	usRet = InitializeSAM();
	if( usRet != APP_SUCCESS )
	{
		return;
	}
}


