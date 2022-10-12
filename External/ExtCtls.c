

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


#include "External.h"

extern BOOL g_isForcedOnl;


USHORT usCTOSS_CtlsV3TransEXT(BYTE *inbuf, USHORT inlen)
{
	CTLS_Trans strCTLSTrans;
	CTLS_TransData strCTLSTransData;
	ULONG ulAPRtn;
	EMVCL_RC_DATA_EX stRCDataEx;
	
	char szV3PSendBuf[4096+1];
	int inOffSet = 0;
	int status;
	char szRecvBuf[300+1];
	int inRecvlen = 0;
	unsigned char *pszPtr;

	//vdDisplayErrorMsg(1,13,"CTOSS_CtlsV3TransEXT");
	
	memcpy(&strCTLSTrans, inbuf, sizeof(CTLS_Trans));
	//g_isForcedOnl = strCTLSTrans.inForcedOnl;
	ulAPRtn = usCTOSS_CtlsV3Trans(strCTLSTrans.szAmount, strCTLSTrans.szOtherAmt, strCTLSTrans.szTransType, strCTLSTrans.szCatgCode, strCTLSTrans.szCurrCode, &stRCDataEx);
	strCTLSTransData.status = ulAPRtn;

	if (ulAPRtn == d_OK)
	{
		strCTLSTransData.usTransResult = stRCDataAnalyze.usTransResult;
		strCTLSTransData.bCVMAnalysis = stRCDataAnalyze.bCVMAnalysis;
		memcpy(strCTLSTransData.baCVMResults, stRCDataAnalyze.baCVMResults,sizeof(stRCDataAnalyze.baCVMResults));
			
		strCTLSTransData.bSID = stRCDataEx.bSID;
		strcpy(strCTLSTransData.baDateTime, stRCDataEx.baDateTime);
		//pack track1
		if (stRCDataEx.bTrack1Len > 0)
		{
			strCTLSTransData.bTrack1Len = stRCDataEx.bTrack1Len;
			memcpy(strCTLSTransData.baTrack1Data, stRCDataEx.baTrack1Data, strCTLSTransData.bTrack1Len);
		}
		//pack track2
		if (stRCDataEx.bTrack2Len > 0)
		{
			strCTLSTransData.bTrack2Len = stRCDataEx.bTrack2Len;
			memcpy(strCTLSTransData.baTrack2Data, stRCDataEx.baTrack2Data, strCTLSTransData.bTrack2Len);
		}
		//pack ChipData
		if (stRCDataEx.usChipDataLen > 0)
		{
			strCTLSTransData.usChipDataLen = stRCDataEx.usChipDataLen;
			memcpy(strCTLSTransData.baChipData, stRCDataEx.baChipData, strCTLSTransData.usChipDataLen);
		}
		//pack AdditionalData
		if (stRCDataEx.usAdditionalDataLen > 0)
		{
			strCTLSTransData.usAdditionalDataLen = stRCDataEx.usAdditionalDataLen;
			memcpy(strCTLSTransData.baAdditionalData, stRCDataEx.baAdditionalData, strCTLSTransData.usAdditionalDataLen);
		}

		
		DebugAddINT("RC Analyze, Trans Result", strCTLSTransData.usTransResult);
		DebugAddINT("RC Analyze, CVM Analysis", strCTLSTransData.bCVMAnalysis);
		DebugAddHEX("RC Analyze, CVM Result", strCTLSTransData.baCVMResults, sizeof(strCTLSTransData.baCVMResults));
		DebugAddINT("RC Analyze, Visa AOSA Present", strCTLSTransData.bVisaAOSAPresent);
		DebugAddHEX("RC Analyze, Visa AOSA", strCTLSTransData.baVisaAOSA, sizeof(strCTLSTransData.baVisaAOSA));
		DebugAddINT("RC Analyze, ODA Fail", strCTLSTransData.bODAFail);

		DebugAddHEX("SCDataEx DateTime", strCTLSTransData.baDateTime, sizeof(strCTLSTransData.baDateTime));
		DebugAddHEX("SCDataEx Track1", strCTLSTransData.baTrack1Data, strCTLSTransData.bTrack1Len);
		DebugAddHEX("SCDataEx Track2", strCTLSTransData.baTrack2Data, strCTLSTransData.bTrack2Len);
		DebugAddHEX("SCDataEx Chip", strCTLSTransData.baChipData, strCTLSTransData.usChipDataLen);
		DebugAddHEX("SCDataEx Additional", strCTLSTransData.baAdditionalData, strCTLSTransData.usAdditionalDataLen);			
	}

	
	vdCTOSS_SetV3PRS232Port(strTCT.byRS232ECRPort);
	inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);

	// send STX INJECT_KEY ETX LRC
	memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
	inOffSet = 0;
	szV3PSendBuf[inOffSet] = STX;
	inOffSet += 1;
	memcpy(&szV3PSendBuf[inOffSet], "CTLS_TRANS", strlen("CTLS_TRANS"));
	inOffSet += strlen("CTLS_TRANS");
	szV3PSendBuf[inOffSet] = '|';
	inOffSet += 1;				
	//////////////////////////////////////////////////////////////////////////
	memcpy(&szV3PSendBuf[inOffSet], &strCTLSTransData, sizeof(CTLS_TransData));
	inOffSet += sizeof(CTLS_TransData);
	///////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = ETX;
	inOffSet += 1;			
	szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
	inOffSet += 1;
	inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);

	inCTOSS_V3PRS232Close();

	return d_OK;
}



