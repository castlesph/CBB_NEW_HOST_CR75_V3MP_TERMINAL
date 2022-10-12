

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





int inCTOSS_EXTSignatureEXT(void)
{
	char szV3PSendBuf[SIGP_SEND_SIZE+1];
	int inOffSet = 0;
	int status;
	char szEnc[SIGP_SEND_SIZE + 1];
	char  szSystemCmdPath[250];
	int inEnclen;
	char szEnclen[4+1];

	char szreadbuf[SIGP_SEND_SIZE + 1];
	memset(szEnc, 0x00, sizeof(szEnc));
	memset(szEnclen,0x00,sizeof(szEnclen));
	
	ePad_SignatureCaptureLibEx();

	CTOS_LCDTClearDisplay();
	status = lnGetFileSize(DB_SIGN_BMP);
	vdDebug_LogPrintf("inCTOSS_EXTSignatureEXT size[%d]", status);
	//memset(szreadbuf, 0x00, sizeof(szreadbuf));
	//sprintf(szreadbuf,"size[%d]",status);
	//vdDisplayErrorMsg(1,8,szreadbuf);

	if(status > 0)
	{
		inDeleteGzipFile(DB_SIGN_BMP);
		
		memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
		sprintf(szSystemCmdPath, "gzip -c %s > %s.gz", DB_SIGN_BMP,DB_SIGN_BMP);
		system(szSystemCmdPath);	
		CTOS_Delay(50);
		
		inReadGzipFile(DB_SIGN_BMP, szEnc, &inEnclen);

		sprintf(szEnclen,"%04d",inEnclen);

		//memset(szreadbuf, 0x00, sizeof(szreadbuf));
		//sprintf(szreadbuf,"inEnclen[%d]",inEnclen);
		//vdDisplayErrorMsg(1,13,szreadbuf);
	}
	else
	{
		szEnc[0] = d_KBD_CANCEL;
		inEnclen = 1;
		sprintf(szEnclen,"%04d",inEnclen);
	}
	
	vdCTOSS_SetV3PRS232Port(strTCT.byRS232ECRPort);
	inCTOSS_V3PRS232Open(strTCT.inPPBaudRate, 'N', 8, 1);
	
	//memset(szreadbuf, 0x00, sizeof(szreadbuf));
	//sprintf(szreadbuf,"open[%d]",strTCT.byRS232ECRPort);
	//vdDisplayErrorMsg(1,13,szreadbuf);

	// send STX INJECT_KEY ETX LRC
	memset(szV3PSendBuf, 0x00, sizeof(szV3PSendBuf));
	inOffSet = 0;
	szV3PSendBuf[inOffSet] = STX;
	inOffSet += 1;
	memcpy(&szV3PSendBuf[inOffSet], "EXT_SIGNATURE", strlen("EXT_SIGNATURE"));
	inOffSet += strlen("EXT_SIGNATURE");
	szV3PSendBuf[inOffSet] = '|';
	inOffSet += 1;				
	//////////////////////////////////////////////////////////////////////////
	memcpy(&szV3PSendBuf[inOffSet], szEnclen, 4);//data length
	inOffSet += 4;
	memcpy(&szV3PSendBuf[inOffSet], szEnc, inEnclen);//data buff
	inOffSet += inEnclen;
	///////////////////////////////////////////////////////////////////////
	szV3PSendBuf[inOffSet] = ETX;
	inOffSet += 1;			
	szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
	inOffSet += 1;

	//memset(szreadbuf, 0x00, sizeof(szreadbuf));
	//sprintf(szreadbuf,"inOffSet[%d]",inOffSet);
	//vdDisplayErrorMsg(1,13,szreadbuf);

	inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);

	inCTOSS_V3PRS232Close();

	inDeleteGzipFile(DB_SIGN_BMP);
	inDeleteFile(DB_SIGN_BMP);

}




int inCTOSS_EXTSignature(void)
{
	USHORT ret = d_OK;
	char  szSystemCmdPath[250];
	char szEnc[SIGP_SEND_SIZE + 1];
	int inEnclen;
	char szEnclen[4+1];

	vdDebug_LogPrintf("inCTOSS_EXTSignature byPinPadMode[%d] byPinPadType[%d],byPinPadPort=[%d]", strTCT.byPinPadMode, strTCT.byPinPadType,strTCT.byPinPadPort);

	if(strTCT.byPinPadMode == 0)
	{
		if(strTCT.byPinPadType == 3)
		{
			char szV3PSendBuf[1024+1];
			int inOffSet = 0;
			int status;
			char szRecvBuf[10000+1];
			int inRecvlen = 0;
			unsigned char *pszPtr;

			CTOS_LCDTPrintAligned(8,"PLEASE SIGNATURE",d_LCD_ALIGNLEFT);
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
			memcpy(&szV3PSendBuf[inOffSet], "EXT_SIGNATURE", strlen("EXT_SIGNATURE"));
			inOffSet += strlen("EXT_SIGNATURE");
			///////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = '|';
			inOffSet += 1;
			szV3PSendBuf[inOffSet] = 1;
			inOffSet += 1;;			
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

			//inCTOSS_USBHostCloseEx();
			inCTOSS_V3PRS232Close();
			if (status == d_NO)
				return d_NO;				
			if (szRecvBuf[0]==0x15)
				return d_NO;

			memset(szEnc, 0x00, sizeof(szEnc));
			memset(szEnclen,0x00,sizeof(szEnclen));
			
			pszPtr = (char*)memchr(szRecvBuf, '|', inRecvlen); // check STX
			memcpy(szEnclen, &pszPtr[1], 4);
			inEnclen = atoi(szEnclen);
			if (inEnclen > 0)
				memcpy(szEnc,&pszPtr[5],inEnclen);

			inDeleteFile(DB_SIGN_BMP);
			inDeleteFile(DB_SIGN_BMP_GZ);

			vdDebug_LogPrintf("inEnclen=[%d],szEnc[0]=[%c]",inEnclen,szEnc[0]);
			if (inEnclen == 1)
			{
				return szEnc[0];
			}
			
			if (inEnclen > 0)
			{
				inWriteFile(DB_SIGN_BMP_GZ,szEnc,inEnclen);

				memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
				sprintf(szSystemCmdPath, "gunzip -c %s.gz > %s", DB_SIGN_BMP,DB_SIGN_BMP);
				system(szSystemCmdPath);	
				CTOS_Delay(50);

				lnGetFileSize(DB_SIGN_BMP);

				vdCTOSS_SaveBMPForERM(DB_SIGN_BMP);

				CTOS_LCDTClearDisplay();
				CTOS_LCDSelectModeEx(d_LCD_GRAPHIC_320x240_MODE, FALSE);
			    CTOS_LCDGShowBMPPic(1, 1, DB_SIGN_BMP);
			    CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
				CTOS_Delay(1000);
			}
			return ret;
		}
	}

	return d_OK;
}



int inCTOSS_EXTOnLinePin(void)
{
	USHORT ret = d_OK;
	char  szSystemCmdPath[250];
	char szEnc[SIGP_SEND_SIZE + 1];
	int inEnclen;
	char szEnclen[4+1];

	vdDebug_LogPrintf("inCTOSS_EXTSignature byPinPadMode[%d] byPinPadType[%d],byPinPadPort=[%d]", strTCT.byPinPadMode, strTCT.byPinPadType,strTCT.byPinPadPort);

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
			memcpy(&szV3PSendBuf[inOffSet], "EXT_SIGNATURE", strlen("EXT_SIGNATURE"));
			inOffSet += strlen("EXT_SIGNATURE");
			///////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = '|';
			inOffSet += 1;
			szV3PSendBuf[inOffSet] = 1;
			inOffSet += 1;;			
			//////////////////////////////////////////////
			szV3PSendBuf[inOffSet] = ETX;
			inOffSet += 1;			
		    szV3PSendBuf[inOffSet] = (char) wub_lrc(&(szV3PSendBuf[1]), inOffSet-1);
			inOffSet += 1;

			memset(sztmpBuf,0x00,sizeof(sztmpBuf));
			memset(szV3PSendBuf,0x00,sizeof(szV3PSendBuf));
			strcpy(sztmpBuf,"02038736303030303030303030303139393030301C343000123030303030303030303230301C464603400000000000000000D0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000D000000000000000D0D00000000000000000000000000000000000000000000000000000000000000000000000000000000000D0D031000000000000000000000000000000000000000000000000000000000000000000000000000000000000344637364135433632423230303135360000000000000000000000000000000000000038353444343143383943363145433643000000000000000000000000000000000000003000003131313100000034463736413543363242323030313536000000007050AA307041DC18704760637041DC4000000002363231303234303031303039363135390000000000000000000000000000000000000000000000001C4645000230311C033F");
			inOffSet = strlen(sztmpBuf);
			wub_str_2_hex(sztmpBuf,szV3PSendBuf,inOffSet);
			inOffSet = inOffSet/2;
			
			inCTOSS_V3PRS232SendBuf(szV3PSendBuf, inOffSet);	
			vdDebug_LogPrintf("end inCTOSS_V3PRS232SendBuf");
			memset(szRecvBuf,0x00,sizeof(szRecvBuf));
			inRecvlen = 10000;
			status = inCTOSS_V3PRS232RecvBuf(szRecvBuf, &inRecvlen, 60000);
			//inPrintISOPacket(VS_FALSE, szRecvBuf, inRecvlen);
            //DebugAddHEX("inCTOSS_V3PRS232RecvBuf", szRecvBuf, inRecvlen);

			//inCTOSS_USBHostCloseEx();
			inCTOSS_V3PRS232SendBuf(&chResp, 1);
			inCTOSS_V3PRS232Close();
			if (status == d_NO)
				return d_NO;				
			if (szRecvBuf[0]==0x15)
				return d_NO;

		}
	}

	return d_OK;
}



