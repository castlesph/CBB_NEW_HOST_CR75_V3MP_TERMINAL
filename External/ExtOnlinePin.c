

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

#include "../Includes/wub_lib.h"
#include "../Includes/Encryption.h"
#include "../Includes/msg.h"
#include "../Includes/myEZLib.h"

#include "../Includes/V5IsoFunc.h"
#include "../Includes/POSTypedef.h"
#include "../Comm/V5Comm.h"
#include "../FileModule/myFileFunc.h"
#include "../UI/Display.h"
#include "../Includes/Trans.h"
#include "../UI/Display.h"
#include "../Accum/Accum.h"
#include "../POWRFAIL/POSPOWRFAIL.h"
#include "../DataBase/DataBaseFunc.h"
#include "../Includes/POSTrans.h"
#include "..\Debug\Debug.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\Showbmp.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\ISOEnginee.h"
#include "..\Includes\EFTSec.h"
#include "..\Print\Print.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\epad.h"
#include "..\PCI100\COMMS.h"
#include "..\Ctls\POSCtls.h"
#include "..\Erm\PosErm.h"
#include "..\pinpad\Pinpad.h"


#include "External.h"

static USHORT gusKeySet;
static USHORT gusKeyIndex;



int CTOSS_Load_3DES_TMK_Plaintext(char *szKeyData,int HDTid)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	
	//CTOS_LCDTClearDisplay();
	vduiClearBelow(2);
	inDCTRead(HDTid);

	if ((strTCT.byPinPadMode == 0) && (strTCT.byPinPadType == 3))
	{
		inCTOSS_EXTLoad_3DES_TMK_Plaintext(szKeyData,HDTid);
	}
	//---------------------------------------------------------------------
	// Write 3DES Key in plaintext	
	KeySet = gusKeySet;
	KeyIndex = gusKeyIndex;
	
	vdDebug_LogPrintf("**CTOSS_Load_3DES_TMK_Plaintext**");
        
	sprintf(str, "KeySet = %04X", KeySet);
	CTOS_LCDTPrintXY(1, 3, str);
	sprintf(str, "KeyIndex = %04X", KeyIndex);
	CTOS_LCDTPrintXY(1, 4, str);
	
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = szKeyData;
	para.Value.KeyLength = 16;

	vdPCIDebug_HexPrintf("CTOSS_Load_3DES_TMK_Plaintext", szKeyData, 16);

	ret = CTOS_KMS2KeyWrite(&para);
    
	vdDebug_LogPrintf("**TMK CTOS_KMS2KeyWrite*ret[%d]*",ret );
	if(ret != d_OK)
	{
		sprintf(str, "ret = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
        return d_NO;
	}
    else
        return d_OK;

}


int inCTOSS_PinPadLoad_3DES_TMK_Plaintext(BYTE *inbuf, USHORT inlen)
{
	OLPIN_PARAM	stOLPinParam;
	char szV3PSendBuf[4096+1];
	int inOffSet = 0;
	int status;
	char szRecvBuf[300+1];
	int inRecvlen = 0;
	unsigned char *pszPtr;
	
	memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
	memcpy(&stOLPinParam, inbuf,  sizeof(stOLPinParam));
	gusKeySet = stOLPinParam.ushKeySet;
	gusKeyIndex = stOLPinParam.ushKeyIdx;	

	CTOSS_Load_3DES_TMK_Plaintext(stOLPinParam.szPINKey,stOLPinParam.HDTid);

	vdCTOSS_SetV3PRS232Port(strTCT.byRS232ECRPort);
	inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

	// send STX INJECT_KEY ETX LRC
	memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
	inOffSet = 0;
	szV3PSendBuf[inOffSet] = STX;
	inOffSet += 1;
	memcpy(&szV3PSendBuf[inOffSet], "LOAD_3DES_TMK_EXT", strlen("LOAD_3DES_TMK_EXT"));
	inOffSet += strlen("LOAD_3DES_TMK_EXT");
	szV3PSendBuf[inOffSet] = '|';
	inOffSet += 1;				
	//////////////////////////////////////////////////////////////////////////
	memcpy(&szV3PSendBuf[inOffSet], &stOLPinParam, sizeof(OLPIN_PARAM));
	inOffSet += sizeof(OLPIN_PARAM);
	///////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = ETX;
	inOffSet += 1;			
	szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
	inOffSet += 1;
	inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);

	inCTOSS_V3PRS232Close();
	
}

int inCTOSS_EXTLoad_3DES_TMK_Plaintext(char *szKeyData,int HDTid)
{
	USHORT ret = d_OK;
	char  szSystemCmdPath[250];
	char szEnc[SIGP_SEND_SIZE + 1];
	int inEnclen;
	char szEnclen[4+1];
	OLPIN_PARAM	stOLPinParam;

	vdDebug_LogPrintf("inCTOSS_EXTLoad_3DES_TMK_Plaintext byPinPadMode[%d] byPinPadType[%d],byPinPadPort=[%d]", strTCT.byPinPadMode, strTCT.byPinPadType,strTCT.byPinPadPort);

	memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
	memcpy(stOLPinParam.szPINKey,szKeyData,16);
	stOLPinParam.HDTid = HDTid;
	stOLPinParam.ushKeySet = strDCT.usKeySet;
	stOLPinParam.ushKeyIdx = strDCT.usKeyIndex;
	
	if(strTCT.byPinPadMode == 0)
	{
		if(strTCT.byPinPadType == 3)
		{
			char szV3PSendBuf[1024+1];
			char sztmpBuf[1024+1];
			int inOffSet = 0;
			int status;
			char szRecvBuf[10000+1];
			int inRecvlen = 0;
			char chResp = ACK;
			unsigned char *pszPtr;
			
			vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
			status = inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);
			if (status != d_OK)
			{
				vdDisplayErrorMsg(1, 8, "Open COM Error");
				return status;
			}

			// send STX INJECT_KEY ETX LRC
			memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
			inOffSet = 0;
			szV3PSendBuf[inOffSet] = STX;
			inOffSet += 1;
			memcpy(&szV3PSendBuf[inOffSet], "LOAD_3DES_TMK_EXT", strlen("LOAD_3DES_TMK_EXT"));
			inOffSet += strlen("LOAD_3DES_TMK_EXT");
			///////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = '|';
			inOffSet += 1;
			memcpy(&szV3PSendBuf[inOffSet], &stOLPinParam, sizeof(OLPIN_PARAM));
			inOffSet += sizeof(OLPIN_PARAM);			
			//////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = ETX;
			inOffSet += 1;			
		    szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
			inOffSet += 1;

			inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);	
			vdDebug_LogPrintf("end inCTOSS_V3PRS232SendBuf");
			memset(szRecvBuf,0x00,sizeof(szRecvBuf));
			inRecvlen = 10000;
			status = inCTOSS_V3PRS232RecvBuf(szRecvBuf, &inRecvlen, 60000);
			//inPrintISOPacket(VS_FALSE, szRecvBuf, inRecvlen);
            //DebugAddHEX("inCTOSS_V3PRS232RecvBuf", szRecvBuf, inRecvlen);

			inCTOSS_V3PRS232Close();
			if (status == d_NO)
				return d_NO;				
			if (szRecvBuf[0]==0x15)
				return d_NO;

		}
	}

	return d_OK;
}



int inCTOSS_PinPadGetIPPPin(BYTE *inbuf, USHORT inlen)
{
	USHORT ret;
	char szV3PSendBuf[4096+1];
	int inOffSet = 0;
	int status;
	char szRecvBuf[300+1];
	int inRecvlen = 0;
	unsigned char *pszPtr;
	
	memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
	memcpy(&stOLPinParam, inbuf,  sizeof(stOLPinParam));

	srTransRec.HDTid = stOLPinParam.HDTid;
	strcpy(srTransRec.szPAN,stOLPinParam.szPAN);

	inDCTRead(srTransRec.HDTid);
	strDCT.usKeySet = stOLPinParam.ushKeySet;
	strDCT.usKeyIndex = stOLPinParam.ushKeyIdx;
	memcpy(strDCT.szPINKey,stOLPinParam.szPINKey,16);
	inDCTSave(srTransRec.HDTid);

#ifdef DUKPT_3DESONLINEPIN
		ret = GetPIN_With_3DESDUKPT();
#else		
	ret = inGetIPPPin();
#endif
	memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
	if(ret != d_OK)
	{
		if(ret == d_KMS2_GET_PIN_NULL_PIN)
		{
			stOLPinParam.inPINEntered = 1;
		}
		else
		{
			stOLPinParam.inPINEntered = 0;
		}
	}
	else	
		stOLPinParam.inPINEntered = 2;
	
	stOLPinParam.ushKeySet = ret;
	memcpy(stOLPinParam.szPINBlock,srTransRec.szPINBlock,8);
	
	vdCTOSS_SetV3PRS232Port(strTCT.byRS232ECRPort);
	inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

	// send STX INJECT_KEY ETX LRC
	memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
	inOffSet = 0;
	szV3PSendBuf[inOffSet] = STX;
	inOffSet += 1;
	memcpy(&szV3PSendBuf[inOffSet], "GET_ONLINEPIN_EXT", strlen("GET_ONLINEPIN_EXT"));
	inOffSet += strlen("GET_ONLINEPIN_EXT");
	szV3PSendBuf[inOffSet] = '|';
	inOffSet += 1;				
	//////////////////////////////////////////////////////////////////////////
	memcpy(&szV3PSendBuf[inOffSet], &stOLPinParam, sizeof(OLPIN_PARAM));
	inOffSet += sizeof(OLPIN_PARAM);
	///////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = ETX;
	inOffSet += 1;			
	szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
	inOffSet += 1;
	inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);

	inCTOSS_V3PRS232Close();
	
}


int inCTOSS_EXTGetIPPPin(void)
{
	USHORT ret = d_OK;
	char  szSystemCmdPath[250];
	char szEnc[SIGP_SEND_SIZE + 1];
	int inEnclen;
	char szEnclen[4+1];

	vdDebug_LogPrintf("inCTOSS_EXTGetIPPPin byPinPadMode[%d] byPinPadType[%d],byPinPadPort=[%d]", strTCT.byPinPadMode, strTCT.byPinPadType,strTCT.byPinPadPort);
	
	if(strTCT.byPinPadMode == 0)
	{
		if(strTCT.byPinPadType == 3)
		{
			char szV3PSendBuf[1024+1];
			char sztmpBuf[1024+1];
			int inOffSet = 0;
			int status;
			char szRecvBuf[10000+1];
			int inRecvlen = 0;
			char chResp = ACK;
			unsigned char *pszPtr;
			
			vdCTOSS_SetV3PRS232Port(strTCT.byPinPadPort);
			status = inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);
			if (status != d_OK)
			{
				vdDisplayErrorMsg(1, 8, "Open COM Error");
				return status;
			}

			// send STX INJECT_KEY ETX LRC
			memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
			inOffSet = 0;
			szV3PSendBuf[inOffSet] = STX;
			inOffSet += 1;
			memcpy(&szV3PSendBuf[inOffSet], "GET_ONLINEPIN_EXT", strlen("GET_ONLINEPIN_EXT"));
			inOffSet += strlen("GET_ONLINEPIN_EXT");
			///////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = '|';
			inOffSet += 1;
			memcpy(&szV3PSendBuf[inOffSet], &stOLPinParam, sizeof(OLPIN_PARAM));
			inOffSet += sizeof(OLPIN_PARAM);			
			//////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = ETX;
			inOffSet += 1;			
			szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
			inOffSet += 1;

			inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);	
			vdDebug_LogPrintf("end inCTOSS_V3PRS232SendBuf");
			memset(szRecvBuf,0x00,sizeof(szRecvBuf));
			inRecvlen = 10000;
			status = inCTOSS_V3PRS232RecvBuf(szRecvBuf, &inRecvlen, 60000);
			//inPrintISOPacket(VS_FALSE, szRecvBuf, inRecvlen);
			//DebugAddHEX("inCTOSS_V3PRS232RecvBuf", szRecvBuf, inRecvlen);

			inCTOSS_V3PRS232Close();
			if (status == d_NO)
				return d_NO;				
			if (szRecvBuf[0]==0x15)
				return d_NO;

			pszPtr = (char*)memchr(szRecvBuf, '|', inRecvlen); // check STX
			memset(&stOLPinParam, 0x00, sizeof(stOLPinParam));
			memcpy(&stOLPinParam, pszPtr+1,  sizeof(stOLPinParam));

			vdDebug_LogPrintf("ushKeySet=[%d],inPINEntered=[%d.....]",stOLPinParam.ushKeySet,stOLPinParam.inPINEntered);

		}
	}

	return d_OK;
}



