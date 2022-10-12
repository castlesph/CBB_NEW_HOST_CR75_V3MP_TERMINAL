
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "MultiAptrans.h"
#include "..\Includes\POSTypedef.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSRefund.h"
#include "..\Includes\POSAuth.h"
#include "..\Includes\POSVoid.h"
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSSettlement.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\POSOffline.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\Dmenu.h"
#include "..\Includes\CTOSInput.h"
#include "..\Includes\Showbmp.h"
#include "..\Print\Print.h"
#include "..\UI\Display.h"
#include "..\Debug\Debug.h"
#include "..\FileModule\myFileFunc.h"
#include "..\DataBase\DataBaseFunc.h"
#include "..\powrfail\POSPOWRFAIL.h"
#include "..\Ctls\POSWave.h"
#include "..\TMS\TMS.h"
#include "..\External\External.h"
#include "..\erm\PosErm.h"
#include "..\aptrans\MultiShareEMV.h"
#include "..\Functionslist\POSFunctionsList.h"
#include "..\Includes\POSMPUSale.h"
#include "..\Aptrans\MultiShareECR.h"

// patrick add code 20141205 start
static int inSendECRResponseFlag = 0;
char inSendECRResponseCommandID[2+1];
//int inIPC_KEY_SEND_Parent = 0;

static char gszFontName[50+1]={0};

static int ginReadDBFlag = 0;
static BYTE	gszInvoiceNo[INVOICE_BCD_SIZE+1];

ECR_REQ ECRReq;

// patrick this function need provide to application level and response code
/*================================================================
  Handle IPC function.
  [Return] IPC process status return code
  ================================================================ */
USHORT inMultiAP_HandleIPC(BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int inRet = d_NO;
	USHORT len;
	DMENU dmMenuListtmp[40] = {0};

	len = 0;

    //ECR txn no need this, make it fast
    //if(0 == fGetECRTransactionFlg())
    {
        //if main APP change the parameter, sub need read again
    	inTCTRead(1);
    	//inCPTRead(1);//for multiap trans speed
    	//inCSTRead(1);//for multiap trans speed
    	//inTCPRead(1);//for multiap trans speed
    }
		// patrick add code 20141205 start
		inSendECRResponseCommandID[0] = inbuf[0];

	vdCTOSS_MultiAPAnalysis(inbuf+1, inlen-1);
	
	vdCTOSS_MultiAPAnalysisECR(inbuf+1, inlen-1);
	/* Process IPC command code action */
	switch (inbuf[0])
	{

		case d_IPC_CMD_APP_SUB_MENU:
			//inSetColorMenuMode();
                    vdDebug_LogPrintf("SUB APP");
			
			inRet = inDSubMenuRead(&inbuf[1], dmMenuListtmp);	
			if (inRet==d_OK)
			{
				//inSetColorMenuMode();
				inRet = inDynamicMenu(dmMenuListtmp);
			}
			outbuf[len++] = d_IPC_CMD_APP_SUB_MENU;
			if (inRet == d_OK)
				outbuf[len++] = d_SUCCESS;
			else
				outbuf[len++] = d_FAIL;
			outbuf[len] = 0x00;
			
			break;
		
		case d_IPC_CMD_APP_MENU_FUNC:
			//inSetColorMenuMode();
			
			inRet = inCTOSS_ExeFunction(&inbuf[1]);
			outbuf[len++] = d_IPC_CMD_APP_MENU_FUNC;
			if (inRet == d_OK)
				outbuf[len++] = d_SUCCESS;
			else
				outbuf[len++] = d_FAIL;
			outbuf[len] = 0x00;
			
			break;
			
		// patrick add code 20141205 start
	//inSendECRResponseCommandID[0] = inbuf[0];
	case d_IPC_CMD_ONLINES_SALE:
//		inRet = inCTOS_SALE();
                inRet = inCTOS_MPU_SALE();
		outbuf[len++] = d_IPC_CMD_ONLINES_SALE;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;
//            case d_IPC_CMD_WALLET_SALE:
//                inRet = inCTOS_WalletSALE();
//		outbuf[len++] = d_IPC_CMD_WALLET_SALE;
//		if (inRet == d_OK)
//			outbuf[len++] = d_SUCCESS;
//		else
//			outbuf[len++] = d_FAIL;
//		outbuf[len] = 0x00;
//                break;
	case d_IPC_CMD_WAVE_SALE:
            vdDebug_LogPrintf("SALE APTRANS");
//			inRet = inCTOS_WAVE_SALE();
            inRet = inCTOS_MPU_SALE();
			outbuf[len++] = d_IPC_CMD_WAVE_SALE;
			if (inRet == d_OK)
				outbuf[len++] = d_SUCCESS;
			else
				outbuf[len++] = d_FAIL;
			outbuf[len] = 0x00;
                        DebugAddHEX("outbuf", outbuf, len);
			break;

	case d_IPC_CMD_WAVE_REFUND:
		inRet = inCTOS_WAVE_REFUND();
		outbuf[len++] = d_IPC_CMD_WAVE_REFUND;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;

	case d_IPC_CMD_OFFLINE_SALE:
		inRet = inCTOS_SALE_OFFLINE();
		outbuf[len++] = d_IPC_CMD_OFFLINE_SALE;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_PRE_AUTH:
		inRet = inCTOS_PREAUTH();
		outbuf[len++] = d_IPC_CMD_PRE_AUTH;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
 		break;
	
	case d_IPC_CMD_REFUND:
		inRet = inCTOS_REFUND();
		outbuf[len++] = d_IPC_CMD_REFUND;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_POWER_FAIL:
		inRet = inCTLOS_Getpowrfail();
		outbuf[len++] = d_IPC_CMD_POWER_FAIL;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;
		
	case d_IPC_CMD_GET_PID:
//		inMultiAP_APforkMainroutine();
		outbuf[len++] = d_IPC_CMD_GET_PID;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_VOID_SALE:
		inRet = inCTOS_VOID();
		outbuf[len++] = d_IPC_CMD_VOID_SALE;
		
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;

	case d_IPC_CMD_TIP_ADJUST:
		inRet = inCTOS_TIPADJUST();
		outbuf[len++] = d_IPC_CMD_TIP_ADJUST;
		
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;

		break;

	case d_IPC_CMD_SETTLEMENT:
            vdDebug_LogPrintf("SETTLE APTRANS");
		inCTOS_SETTLEMENT();
		outbuf[len++] = d_IPC_CMD_SETTLEMENT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_SETTLE_ALL:
		inRet = inCTOS_SETTLE_ALL();
		outbuf[len++] = d_IPC_CMD_SETTLE_ALL;
		if (inRet == d_OK)
			outbuf[len++] = d_OK;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

    case d_IPC_CMD_CHK_BATCH_EMPTY:
		inRet = inCTOSS_TMSChkBatchEmpty();
		outbuf[len++] = d_IPC_CMD_CHK_BATCH_EMPTY;
		if (inRet == d_OK)
			outbuf[len++] = d_OK;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BATCH_REVIEW:
		inCTOS_BATCH_REVIEW();
		outbuf[len++] = d_IPC_CMD_BATCH_REVIEW;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_BATCH_TOTAL:
		inCTOS_BATCH_TOTAL();
		outbuf[len++] = d_IPC_CMD_BATCH_TOTAL;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_REPRINT_LAST:
		vdDebug_PrintOnPaper("d_IPC_CMD_REPRINT_LAST");
		inRet = inCTOS_REPRINT_LAST();
		outbuf[len++] = d_IPC_CMD_REPRINT_LAST;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_DELETE_BATCH:
		vdCTOS_DeleteBatch();
		outbuf[len++] = d_IPC_CMD_DELETE_BATCH;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
    case d_IPC_CMD_DELETE_REVERSAL:
		vdCTOS_DeleteReversal();
		outbuf[len++] = d_IPC_CMD_DELETE_REVERSAL;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_REPRINT_ANY:
		inRet = inCTOS_REPRINT_ANY();
		outbuf[len++] = d_IPC_CMD_REPRINT_ANY;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else if (inRet == d_NOT_RECORD)
			outbuf[len++] = d_NOT_RECORD;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_REPRINT_LAST_SETT:
		vdDebug_PrintOnPaper("d_IPC_CMD_REPRINT_LAST_SETT");
		inCTOS_REPRINTF_LAST_SETTLEMENT();
		outbuf[len++] = d_IPC_CMD_REPRINT_LAST_SETT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_SUMMARY_REPORT:
		inCTOS_PRINTF_SUMMARY();
		outbuf[len++] = d_IPC_CMD_SUMMARY_REPORT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_DETAIL_REPORT:
		inCTOS_PRINTF_DETAIL();
		outbuf[len++] = d_IPC_CMD_DETAIL_REPORT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

    case d_IPC_CMD_CHECK_DUP_INV:
		inCTOSS_BatchCheckDuplicateInvoice();
		outbuf[len++] = d_IPC_CMD_CHECK_DUP_INV;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;

	case d_IPC_CMD_FUN_KEY1:
		inF1KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY1;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY2:
		inF2KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY2;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY3:
		inF3KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY3;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_FUN_KEY4:
		inF4KeyEvent();
		outbuf[len++] = d_IPC_CMD_FUN_KEY4;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_SETFONT:
		inSetTextModeEx(inbuf+1, inlen-1);
		outbuf[len++] = d_IPC_CMD_SETFONT;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	/////////////////////////////////////////////////////////////////////////////
	//for external pinpad
	case d_IPC_CMD_CTLS_EXT_TRANS:		
		usCTOSS_CtlsV3TransEXT(inbuf+1, inlen-1);
		outbuf[len++] = d_IPC_CMD_CTLS_EXT_TRANS;
		if (inRet == d_OK)
			outbuf[len++] = d_SUCCESS;
		else
			outbuf[len++] = d_FAIL;
		outbuf[len] = 0x00;
		break;	

///////////////////////////////////////
	case d_IPC_CMD_EXT_SIGNATURE:
		inCTOSS_EXTSignatureEXT();
		outbuf[len++] = d_IPC_CMD_EXT_SIGNATURE;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_EXT_READCARD:
		inCTOS_WaveGetCardFieldsEXT(inbuf+1, inlen-1);
		outbuf[len++] = d_IPC_CMD_EXT_READCARD;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_EXT_READEMVCARD:
		inCTOS_EMVGetCardEXT(inbuf+1, inlen-1);
		outbuf[len++] = d_IPC_CMD_EXT_READEMVCARD;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_EXT_LOADTMK:
		inCTOSS_PinPadLoad_3DES_TMK_Plaintext(inbuf+1, inlen-1);
		outbuf[len++] = d_IPC_CMD_EXT_LOADTMK;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_EXT_GET_ONLINEPIN:
		inCTOSS_PinPadGetIPPPin(inbuf+1, inlen-1);
		outbuf[len++] = d_IPC_CMD_EXT_GET_ONLINEPIN;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_ERM_INIT:
		inCTOSS_ERMInit();
		outbuf[len++] = d_IPC_CMD_ERM_INIT;
		outbuf[len++] = d_OK;
		outbuf[len] = 0x00;
		break;
	case d_IPC_CMD_PASSDATA:
		inCTOSS_PassData();
		outbuf[len++] = d_IPC_CMD_PASSDATA;
		outbuf[len++] = d_SUCCESS;
		outbuf[len] = 0x00;
		break;
        case d_IPC_CMD_ECHO:
                inCTOS_ECR_ECHO();
                outbuf[len++] = d_IPC_CMD_ECHO;
                outbuf[len++] = d_SUCCESS;
                outbuf[len] = 0x00;
                break;
        case d_IPC_CMD_TRX_RETRIEVAL:
                inCTOS_ECR_RETRIEVAL();
                outbuf[len++] = d_IPC_CMD_TRX_RETRIEVAL;
                outbuf[len++] = d_SUCCESS;
                outbuf[len] = 0x00;
                break;

    #ifdef ECR_PREAUTH_AND_COMP
        case d_IPC_CMD_PRE_AUTH_COMP:
			vdDebug_PrintOnPaper("d_IPC_CMD_PRE_AUTH_COMP");
            inRet = inCTOS_PREAUTH_COMPLETION_ECR();
            outbuf[len++] = d_IPC_CMD_PRE_AUTH_COMP;
            if (inRet == d_OK)
                outbuf[len++] = d_SUCCESS;
            else
                outbuf[len++] = d_FAIL;
            outbuf[len] = 0x00;
		break;
    #endif

        default:
            outbuf[len++] = 0xFF;
        break;	
	}

	inTCTSave(1);
	
	*outlen = len;
	return len;
}

int inMultiAP_RunIPCCmdTypes(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	pid_t wpid;	
	BYTE processID[100];
	pid_t pid = -1;
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];
	int begin;
	int timeoutSecond = 10;

	strcpy(szAppname, Appname);

	memset(processID,0x00,sizeof(processID));
	vdMultiAP_getPID(szAppname,processID);
	pid = atoi(processID);

	vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes =[%d][%s],[%x]",pid, szAppname,IPC_EVENT_ID);
        
        if(strcmp(szAppname, "V5S_Alipay") == 0){
            vdDebug_LogPrintf("Masuk");
            inCSTRead(3);
        }
            
        
	if (pid <= 0)
	{
		inCTOS_ReForkSubAP(szAppname);
		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);
		//here need delay, wait sub AP initialization 
		if (pid > 0 && d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
			CTOS_Delay(3000);
		
		vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypes22 =[%d][%s]",pid, szAppname);
	}

	if (pid > 0)
	{
//		inIPC_KEY_SEND_Parent = pid;
		IPC_IN_LEN = 0;
		memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
		memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
		
		IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
		if (inlen > 0)
		{
			memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
			IPC_IN_LEN = IPC_IN_LEN + inlen;
		}
		
		do
		{
			//beacuse the unforkAP must have IPC EVENT ID.
			//if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
			//{
			//	vdDebug_LogPrintf("Multi UnForkAPName=[%s] ....", szAppname);
			//	break;
			//}
			
			memset(IPC_IN_BUF_STR, 0x00, sizeof(IPC_IN_BUF_STR));
			inlen = wub_hex_2_str(IPC_IN_BUF ,IPC_IN_BUF_STR ,IPC_IN_LEN);
			IPC_OUT_LEN = inMultiAP_IPCCmdParent(IPC_IN_BUF_STR, IPC_OUT_BUF_STR, pid);
			wub_str_2_hex(IPC_OUT_BUF_STR, IPC_OUT_BUF, strlen(IPC_OUT_BUF_STR));
			
			vdDebug_LogPrintf("CloseAllDrvHandle IPC_OUT_LEN=[%d][%2X][%d]",IPC_OUT_LEN, IPC_OUT_BUF[0],IPC_OUT_BUF[0]);

			if (IPC_OUT_BUF[0] == d_SUCCESS)
			{
				break;
			}

			insendloop++;
			if (insendloop > 1)//now it's blocking mode, no need wait too long.
			{
				vdDebug_LogPrintf("inMultiAP_IPCCmdParent TIMEOUT.!!!");
				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					vdDebug_LogPrintf("inCTOS_Kill unfork SubAP = [%s]",szAppname);
					inCTOS_KillASubAP(szAppname);
				}
				return d_NO;
			}
		} while (1);

		vdDebug_LogPrintf("inWaitTime =[%d]",strTCT.inWaitTime);
		if (strTCT.inWaitTime <= 0)
			strTCT.inWaitTime = 100;
		
		begin=time(NULL);
		memset(IPC_OUT_BUF_STR, 0x00, sizeof(IPC_OUT_BUF_STR));
		while ( (wpid=waitpid(pid, &status, WNOHANG)) != pid )
		{	
#if 0		
			memset(processID,0x00,sizeof(processID));
			vdMultiAP_getPID(szAppname,processID);
			if (atoi(processID) <= 0)
			{
				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					vdDebug_LogPrintf("Multi UnForkAPName11=[%s] ....", szAppname);
					return d_OK;
				}
				
				inCTOS_ReForkSubAP(szAppname);
				return d_NO;
			}
#endif

			vdDebug_LogPrintf("inMultiAP_IPCCmdParent DDD");

			if (d_IPC_CMD_EMV_WGET == IPC_EVENT_ID)
			{
				if (time(NULL)-begin > timeoutSecond)
				{
					inCTOS_ReForkSubAP(szAppname);
					vdDebug_LogPrintf("inMultiAP_IPCCmdParent  EEE");
					
					return d_NO;
				}
			}
						
			CTOS_Delay(strTCT.inWaitTime);

			ipc_len = d_MAX_IPC_BUFFER;
			// patrick need hendle check status if transaction fail or success.
			if (inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK)
			{
				ipc_len = wub_str_2_hex(IPC_OUT_BUF_STR, outbuf, ipc_len);
				*outlen = ipc_len;

				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					inCTOS_KillASubAP(szAppname);
				}

				vdDebug_LogPrintf("inMultiAP_IPCCmdParent  FFF");
				
				return d_OK;
			}
			
		}
	}
	else
	{
		vdDebug_LogPrintf("inMultiAP_IPCCmdParent  GGG");
	
		return d_NO;
	}
}


int inMultiAP_RunIPCCmdTypesEx(char *Appname, int IPC_EVENT_ID, BYTE *inbuf, USHORT inlen, BYTE *outbuf, USHORT *outlen)
{
	int insendloop = 0;
	int status;
	int ipc_len;
	pid_t wpid;	
	BYTE processID[100];
	pid_t pid = -1;
	USHORT IPC_IN_LEN, IPC_OUT_LEN;
	BYTE IPC_IN_BUF[d_MAX_IPC_BUFFER], IPC_OUT_BUF[d_MAX_IPC_BUFFER];
	BYTE IPC_IN_BUF_STR[d_MAX_IPC_BUFFER], IPC_OUT_BUF_STR[d_MAX_IPC_BUFFER];
	char szAppname[100+1];

	strcpy(szAppname, Appname);

	memset(processID,0x00,sizeof(processID));
	vdMultiAP_getPID(szAppname,processID);
	pid = atoi(processID);

	vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypesEx =[%d][%s],[%x]",pid, szAppname,IPC_EVENT_ID);
	if (pid <= 0)
	{
		inCTOS_ReForkSubAP(szAppname);
		memset(processID,0x00,sizeof(processID));
		vdMultiAP_getPID(szAppname,processID);
		pid = atoi(processID);
		//here need delay, wait sub AP initialization 
		if (pid > 0 && d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname))
			CTOS_Delay(3000);
		
		vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypesEx22 =[%d][%s]",pid, szAppname);
	}

	if (pid > 0)
	{
//		inIPC_KEY_SEND_Parent = pid;
		IPC_IN_LEN = 0;
		memset(IPC_IN_BUF_STR,0x00,sizeof(IPC_IN_BUF_STR));
		memset(IPC_OUT_BUF_STR,0x00,sizeof(IPC_OUT_BUF_STR));
		
		IPC_IN_BUF[IPC_IN_LEN ++] = IPC_EVENT_ID;
		if (inlen > 0)
		{
			memcpy(&IPC_IN_BUF[IPC_IN_LEN], inbuf, inlen);
			IPC_IN_LEN = IPC_IN_LEN + inlen;
		}
		
		do
		{
			memset(IPC_IN_BUF_STR, 0x00, sizeof(IPC_IN_BUF_STR));
			inlen = wub_hex_2_str(IPC_IN_BUF ,IPC_IN_BUF_STR ,IPC_IN_LEN);
			IPC_OUT_LEN = inMultiAP_IPCCmdParentEx(IPC_IN_BUF_STR, IPC_OUT_BUF_STR, pid);
			wub_str_2_hex(IPC_OUT_BUF_STR, IPC_OUT_BUF, strlen(IPC_OUT_BUF_STR));
			
			//vdDebug_LogPrintf("CloseAllDrvHandle IPC_OUT_LEN=[%d][%2X][%d]",IPC_OUT_LEN, IPC_OUT_BUF[0],IPC_IN_LEN);

			if (IPC_OUT_BUF[0] == d_SUCCESS)
			{
				break;
			}

			insendloop++;
			if (insendloop > 0)
			{
				vdDebug_LogPrintf("inMultiAP_IPCCmdParentEx TIMEOUT.!!!");
				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					vdDebug_LogPrintf("inCTOS_Kill unfork SubAP = [%s]",szAppname);
					inCTOS_KillASubAP(szAppname);
				}
            	return d_OK;
			}
		} while (1);

		vdDebug_LogPrintf("inWaitTime =[%d]",strTCT.inWaitTime);
		if (strTCT.inWaitTime <= 0)
			strTCT.inWaitTime = 100;
		
		 CTOS_TimeOutSet (TIMER_ID_1 , 200);
		//CTOS_TimeOutSet (TIMER_ID_1 , 60*100);//thandar_changed to 60 sec as ECR app time out
		
		memset(IPC_OUT_BUF_STR, 0x00, sizeof(IPC_OUT_BUF_STR));
		while ( (wpid=waitpid(pid, &status, WNOHANG)) != pid )
		{
			CTOS_Delay(strTCT.inWaitTime);

			if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
			{
				vdDebug_LogPrintf("inMultiAP_RunIPCCmdTypesEx TIMEOUT.!!!");
            	return d_OK;
			}

			ipc_len = d_MAX_IPC_BUFFER;
			// patrick need hendle check status if transaction fail or success.
			if (inMultiAP_IPCGetParent(IPC_OUT_BUF_STR, &ipc_len) == d_OK)
			{
				ipc_len = wub_str_2_hex(IPC_OUT_BUF_STR, outbuf, ipc_len);
				*outlen = ipc_len;

				if (d_OK == inCTOSS_MultiCheckUnForkAPName(szAppname)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(szAppname))
				{
					inCTOS_KillASubAP(szAppname);
				}
				
				return d_OK;
			}
		}
	}
	else
	{
		return d_NO;
	}
}


int inMultiAP_SendChild(BYTE *inbuf, USHORT inlen)
{
	BYTE bret;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	int out_len = 0;
	memset(outbuf,0x00,sizeof(outbuf));
	
	out_len = wub_hex_2_str(inbuf ,outbuf ,inlen);

	bret = inMultiAP_IPCSendChild(outbuf , out_len);
	
	//vdDebug_LogPrintf("inMultiAP_SendChild=[%s] [%ld] [%d],[%d]",outbuf, out_len,inlen,bret);

	return bret;
}

// patrick add code 20141205 start
int inMultiAP_ECRSendSuccessResponse(void)
{
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
        BYTE szPrintingText[40 + 1];
        
        CTOS_RTC SetRTC;
	char szYear[3];
	char sMonth[4];
	char szTempMonth[3];
	char szMonthNames[40];
        char szTemp[d_LINE_SIZE + 1];
        char szTemp1[d_LINE_SIZE + 1];

	#ifdef APP_AUTO_TEST
	if (inCTOS_GetAutoTestCnt() > 1)
	{
		return d_OK;
	}
    #endif
    
	if (strTCT.fECR)
	{
            memset(sMonth,0x00,4);
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szTemp1, 0x00, sizeof(szTemp));

            memset(szMonthNames,0x00,40);
            strcpy(szMonthNames,"JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
            memset(szTempMonth, 0x00, 3);

            CTOS_RTCGet(&SetRTC);
            sprintf(szYear ,"%02d",SetRTC.bYear);
            memcpy(srTransRec.szYear,szYear,2);

            wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
            wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
            vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );
            
            if(atol(szTemp) == 0)
            {
                sprintf(szTemp ,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);	
            }
	
            if(atol(szTemp1) == 0)
            {
				sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	
            }
            vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
            memcpy(szTempMonth,&szTemp[0],2);
            memcpy(sMonth, &szMonthNames[(atoi(szTempMonth) - 1)* 3], 3);
            
            // Update Printing Text Flag, Currently only set manually.
            // It is possible to only turn on several parts, caused ECR only print several parts only.
            put_env_int("#P1FLAG", 1);
            put_env_int("#P2FLAG", 1);
            put_env_int("#P3FLAG", 1);
            put_env_int("#P4FLAG", 1);
           
            // Printing Part 1
            memset(szPrintingText, 0x00, sizeof(szPrintingText));
            sprintf(szPrintingText, "TID: %s   MID: %s", srTransRec.szTID, srTransRec.szMID);
            put_env_int("#P1LEN", strlen(szPrintingText));
            put_env("#P1TEXT", szPrintingText, strlen(szPrintingText));
            
            // Printing Part 2
            memset(szPrintingText, 0x00, sizeof(szPrintingText));
            sprintf(szPrintingText, "DATE: %02lu %s 20%02lu   TIME: %02lu:%02lu:%02lu",atol(szTemp)%100, sMonth, atol(srTransRec.szYear), atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
            put_env_int("#P2LEN", strlen(szPrintingText));
            put_env("#P2TEXT", szPrintingText, strlen(szPrintingText));
            
            // Printing Part 4
            memset(szPrintingText, 0x00, sizeof(szPrintingText));
            strcpy(szPrintingText, "APPROVED");
            put_env_int("#P4LEN", strlen(szPrintingText));
            put_env("#P4TEXT", szPrintingText, strlen(szPrintingText));
            
            vdDebug_LogPrintf("resp code = %s", srTransRec.szRespCode);
            
            if((strcmp(srTransRec.szRespCode, "00") != 0) || (srTransRec.byTransType == ECR_ECHO)){
                put_env("#P1TEXT", " ", 1);
                put_env("#P2TEXT", " ", 1);
                put_env("#P3TEXT", " ", 1);
                put_env("#P4TEXT", " ", 1);
                strcpy(srTransRec.szRespCode, "00");
            }
            
            
            // int put_env(char *tag, char *value, int len)
            
				bret= inMultiAP_Database_BatchDelete();
				vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse: inMultiAP_Database_BatchDelete,bret=[%d]", bret);
				if(d_OK != bret)
				{
					vdSetErrorMessage("MultiAP BatchDelete ERR");
				}
					vdDebug_LogPrintf("Resp PAN:%s",srTransRec.szPAN);
					vdDebug_LogPrintf("Resp DE39:%s",srTransRec.szRespCode);
					vdDebug_LogPrintf("Resp RREF:%s",srTransRec.szRRN);
					vdDebug_LogPrintf("Resp ExpDate:[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
					vdDebug_LogPrintf("Resp MID:%s",srTransRec.szTID);
					vdDebug_LogPrintf("Resp TID:%s",srTransRec.szMID);
					vdDebug_LogPrintf("Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
					vdDebug_LogPrintf("T9F63 =[%d]-----",srTransRec.stEMVinfo.T9F63);
					vdDebug_LogPrintf("T9F6E =[%d]-----",srTransRec.stEMVinfo.T9F6E);
				
				bret = inMultiAP_Database_BatchInsert(&srTransRec);
				vdDebug_LogPrintf("inMultiAP_ECRSendSuccessResponse: inMultiAP_Database_BatchInsert=[%d]", bret);
				if(d_OK != bret)
				{
					vdDebug_LogPrintf("inMultiAP_Database_BatchInsert (d_OK != bret)");
				
					vdSetErrorMessage("MultiAP BatchInsert ERR");
				}
		
						vdSetECRTransactionFlg(0);
						//memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

				outbuf[out_len++] = inSendECRResponseCommandID[0];//d_IPC_CMD_WAVE_SALE;
				outbuf[out_len++] = d_SUCCESS;

				inSendECRResponseFlag = 1;
				put_env_int("ECRRESP",1);
				//bret = inMultiAP_SendChild(outbuf,out_len);
				//hubing enhance ECR
				bret = inMultiAP_SendAP_IPCCmd("SHARLS_ECR", outbuf,out_len);
				if (bret != d_OK)
				{
					return d_OK;
				}
			}

	return d_OK;		
}
// patrick add code 20141205 end

// patrick move to postrans?
int inMultiAP_ECRGetMainroutine(void)
{
	BYTE ipc[d_MAX_IPC_BUFFER] , str[d_MAX_IPC_BUFFER];
	int ipc_len;
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	pid_t my_pid;
		
	ipc_len = sizeof(ipc);
	memset(str,0x00,sizeof(str));
	memset(outbuf,0x00,sizeof(outbuf));
//for improve transaction speed
	if (inMultiAP_IPCGetChildEx(str, &ipc_len) == d_OK)
	{
		outbuf[out_len++] = d_SUCCESS;
		outbuf[out_len] = 0x00;
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
			return d_OK;
                
        vdSetECRTransactionFlg(1);
        
		memset(outbuf,0x00,sizeof(outbuf));
		memset(ipc,0x00,sizeof(ipc));
		out_len = 0;
		ipc_len = wub_str_2_hex(str , ipc , ipc_len);

		
		// patrick add code 20141205 start		
		inSendECRResponseFlag = 0;
		put_env_int("ECRRESP",0);//hubing enhance ECR
                //initialize idle variable
//                vdSetFirstIdleKey(0x00);
//                CTOS_KBDBufFlush();
		ipc_len = inMultiAP_HandleIPC(ipc , ipc_len, outbuf, &out_len);//Do IPC request

		// patrick add code 20141205 start
		inSendECRResponseFlag = get_env_int("ECRRESP");//hubing enhance ECR
		if (inSendECRResponseFlag == 1)	
			return d_OK;
		// patrick add code 20141205 end

		if (out_len == 0)
		{
			out_len = 0;
			outbuf[out_len++] = d_FAIL;
			outbuf[out_len] = 0x00;
		}

		inTCTSave(1);
		
		bret= inMultiAP_Database_BatchDelete();
		vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchDelete ERR");
		}
		vdDebug_LogPrintf("Resp PAN:%s",srTransRec.szPAN);
    	vdDebug_LogPrintf("Resp DE39:%s",srTransRec.szRespCode);
    	vdDebug_LogPrintf("Resp RREF:%s",srTransRec.szRRN);
    	vdDebug_LogPrintf("Resp ExpDate:[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
    	vdDebug_LogPrintf("Resp MID:%s",srTransRec.szTID);
    	vdDebug_LogPrintf("Resp TID:%s",srTransRec.szMID);
    	vdDebug_LogPrintf("Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
    
		bret = inMultiAP_Database_BatchInsert(&srTransRec);
		vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchInsert ERR");
		}

        vdSetECRTransactionFlg(0);
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
        
		//bret = inMultiAP_SendChild(outbuf,out_len);
		//hubing enhance ECR
		bret = inMultiAP_SendAP_IPCCmd("SHARLS_ECR", outbuf,out_len);
//		bret = d_NO;
		if (bret != d_OK)
		{
			// patrick add code 20141205 start		
			//exit(1);
			/*
			my_pid = getpid();
			memset(outbuf,0x00,sizeof(outbuf));
			sprintf(outbuf,"kill -9 %d",my_pid);
			vdDebug_LogPrintf("kill,[%d][%s]",my_pid,outbuf);
			system(outbuf);
			*/
			return d_OK;
		}
		// patrick add code 20141205 start
		return d_OK;
	}
}

//for External pinpad&CTLS&Signature
int inMultiAP_EXTGetMainroutine(void)
{
	BYTE ipc[d_MAX_IPC_BUFFER] , str[d_MAX_IPC_BUFFER];
	int ipc_len;
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	pid_t my_pid;
		
	ipc_len = sizeof(ipc);
	memset(str,0x00,sizeof(str));
	memset(outbuf,0x00,sizeof(outbuf));

	if (inMultiAP_IPCGetChildEx(str, &ipc_len) == d_OK)
	{
		outbuf[out_len++] = d_SUCCESS;
		outbuf[out_len] = 0x00;
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
			return d_OK;
                
        vdSetECRTransactionFlg(1);
        
		memset(outbuf,0x00,sizeof(outbuf));
		memset(ipc,0x00,sizeof(ipc));
		out_len = 0;
		ipc_len = wub_str_2_hex(str , ipc , ipc_len);
		ipc_len = inMultiAP_HandleIPC(ipc , ipc_len, outbuf, &out_len);//Do IPC request

		if (out_len == 0)
		{
			out_len = 0;
			outbuf[out_len++] = d_FAIL;
			outbuf[out_len] = 0x00;
		}

		inTCTSave(1);
		
		bret= inMultiAP_Database_BatchDelete();
		vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchDelete ERR");
		}
		vdDebug_LogPrintf("Resp PAN:%s",srTransRec.szPAN);
    	vdDebug_LogPrintf("Resp DE39:%s",srTransRec.szRespCode);
    	vdDebug_LogPrintf("Resp RREF:%s",srTransRec.szRRN);
    	vdDebug_LogPrintf("Resp ExpDate:[%02X%02X]",srTransRec.szExpireDate[0], srTransRec.szExpireDate[1]);
    	vdDebug_LogPrintf("Resp MID:%s",srTransRec.szTID);
    	vdDebug_LogPrintf("Resp TID:%s",srTransRec.szMID);
    	vdDebug_LogPrintf("Inv Num:[%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);
    
		bret = inMultiAP_Database_BatchInsert(&srTransRec);
		vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
		if(d_OK != bret)
		{
			vdSetErrorMessage("MultiAP BatchInsert ERR");
		}

        vdSetECRTransactionFlg(0);
        memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
        
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
		{
			exit(1);
			return d_OK;
		}
	}
}


// patrick move to postrans?
int inMultiAP_GetMainroutine(void)
{
	BYTE ipc[d_MAX_IPC_BUFFER] , str[d_MAX_IPC_BUFFER];
	int ipc_len;
	BYTE bret = d_NO;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	pid_t my_pid;
		
	ipc_len = sizeof(ipc);
	memset(str,0x00,sizeof(str));
	memset(outbuf,0x00,sizeof(outbuf));

	if (inMultiAP_IPCGetChild(str, &ipc_len) == d_OK)
	{
		outbuf[out_len++] = d_SUCCESS;
		outbuf[out_len] = 0x00;
		bret = inMultiAP_SendChild(outbuf,out_len);
		if (bret != d_OK)
			return d_OK;

		memset(outbuf,0x00,sizeof(outbuf));
		memset(ipc,0x00,sizeof(ipc));
		out_len = 0;
		ipc_len = wub_str_2_hex(str , ipc , ipc_len);
		// patrick add code 20141205 start		
		inSendECRResponseFlag = 0;		
		ipc_len = inMultiAP_HandleIPC(ipc , ipc_len, outbuf, &out_len);//Do IPC request

		// patrick add code 20141205 start
		if (inSendECRResponseFlag == 1)	
			return d_OK;
		// patrick add code 20141205 end

		if (out_len == 0)
		{
			out_len = 0;
			outbuf[out_len++] = d_FAIL;
			outbuf[out_len] = 0x00;
		}
		
		bret = inMultiAP_SendChild(outbuf,out_len);
//		bret = d_NO;
		if (bret != d_OK)
		{
			exit(1);
			/*
			my_pid = getpid();
			memset(outbuf,0x00,sizeof(outbuf));
			sprintf(outbuf,"kill -9 %d",my_pid);
			vdDebug_LogPrintf("kill,[%d][%s]",my_pid,outbuf);
			system(outbuf);
			*/
			return d_OK;
		}
	}
}

int inCTOS_MultiAPALLAppEventID(int IPC_EVENT_ID)
{
	int inResult = d_NO, inRetVal = d_NO;
//	int inLoop = 0;
	USHORT inLoop = 0;
	CTOS_stCAPInfo stinfo;
	BYTE byStr[25];
	USHORT Outlen; 
    char szAPName[25];
	int inAPPID;
	BYTE bret;
	BYTE szTempDebug [100+1];

	BYTE bInBuf[512];
	BYTE bIntmpBuf[512];
	USHORT usInLen = 0,ustmpInLen = 0;

	usInLen = 0;
	ustmpInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    memset(bIntmpBuf, 0x00, sizeof(bIntmpBuf));
	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	inTCTSave(1);

	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		memset(byStr, 0x00, sizeof(byStr));
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
			memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
			CTOS_APGet(inLoop, &stinfo);
		}

//		CTOS_Delay(50);
//		sprintf(szTempDebug, "inLoop=[%d", inLoop);
//		CTOS_LCDTPrintXY(1, 6, szTempDebug);

		vdDebug_LogPrintf("baName=[%s],inLoop=[%d] ", stinfo.baName, inLoop);

		if (strcmp(szAPName, stinfo.baName)!=0)
		{	
			if (stinfo.baName[0] != 0x00)
			{
				if (memcmp(stinfo.baName, "SHARLS_", 7)==0)
					continue;
#if 0
				if (d_OK == inCTOSS_MultiCheckUnForkAPName(stinfo.baName)
					|| d_OK == inCTOSS_MultiCheckUnForkSharlsAPName(stinfo.baName))
				{
					vdDebug_LogPrintf("Multi UnForkAPName22=[%s] ....", stinfo.baName);
					continue;
				}
#endif
//				CTOS_LCDTPrintXY(1, 7, stinfo.baName);

				//inTCTSave(1);//for multiap trans speed
				if (d_IPC_CMD_VOID_SALE == IPC_EVENT_ID)
				{
					usInLen = 0;
					ustmpInLen = 0;
				    memset(bInBuf, 0x00, sizeof(bInBuf));
				    memset(bIntmpBuf, 0x00, sizeof(bIntmpBuf));
				    usInLen += usCTOSS_PackTagLenValue(bInBuf, MULTI_AP_READ_DB_FLAG, 1, "1");
					memcpy(bIntmpBuf,bInBuf,usInLen);
					ustmpInLen = usInLen;

				    memset(bInBuf, 0x00, sizeof(bInBuf));
					usInLen = 0;
				    usInLen += usCTOSS_PackTagLenValue(bInBuf, MULTI_AP_INVOICE, INVOICE_BCD_SIZE, srTransRec.szInvoiceNo);
					memcpy(&bIntmpBuf[ustmpInLen],bInBuf,usInLen);
					ustmpInLen += usInLen;

					vdPCIDebug_HexPrintf("bIntmpBuf",bIntmpBuf,ustmpInLen);
				}
				else
				{
					bret= inMultiAP_Database_BatchDelete();
					vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
					if(d_OK != bret)
				    	return bret;

	//				CTOS_LCDTPrintXY(1, 3, "BatchDelete");

					bret = inMultiAP_Database_BatchInsert(&srTransRec);
					vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
					if(d_OK != bret)
				    	return bret;
				}
//				CTOS_LCDTPrintXY(1, 4, "BatchInsert");
				memset(szTempDebug,0x00,sizeof(szTempDebug));
				sprintf(szTempDebug,"Checking %s",stinfo.baName);
				CTOS_LCDTPrintXY(1, 8, "                                        ");
				CTOS_LCDTPrintXY(1, 8, szTempDebug);
				//inRetVal = inMultiAP_RunIPCCmdTypes(stinfo.baName, IPC_EVENT_ID, srTransRec.szInvoiceNo,INVOICE_BCD_SIZE,byStr,&Outlen);
				inRetVal = inMultiAP_RunIPCCmdTypes(stinfo.baName, IPC_EVENT_ID, bIntmpBuf,ustmpInLen,byStr,&Outlen);
//				CTOS_LCDTPrintXY(1, 8, "PFR");

				//inTCTRead(1);

				vdDebug_LogPrintf("baName[%x][%d] ",byStr[1],byStr[1]);
				if ((byStr[1] == d_SUCCESS)||(byStr[1] == d_FAIL))
				{
					inTCTRead(1);//for multiap trans speed
					return byStr[1];
				}
				else
					continue;
			}
		}
	}

	inTCTRead(1);//for multiap trans speed
	return d_NOT_RECORD;
}


int inCTOS_MultiAPALLAppEventIDEx(int IPC_EVENT_ID)
{
	int inResult = d_NO, inRetVal = d_NO;
	USHORT inLoop = 0;
	CTOS_stCAPInfo stinfo;
	BYTE byStr[25];
	USHORT Outlen;
	BYTE inStr[100];
	USHORT inlen = 0;
    char szAPName[25];
	int inAPPID;
	BYTE bret;
	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	memset(inStr,0x00,sizeof(inStr));
	inlen = strlen(gszFontName);
	if (inlen > 0)
		strcpy(inStr,gszFontName);

	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		memset(byStr, 0x00, sizeof(byStr));
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
			memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
			CTOS_APGet(inLoop, &stinfo);
		}
		
		
		vdDebug_LogPrintf("baName=[%s],inLoop=[%d] ", stinfo.baName, inLoop);
	   //hu bing fix
		if (d_IPC_CMD_SETFONT == IPC_EVENT_ID)
		{
			if (strcmp("SHARLS_BT", stinfo.baName)==0)
				continue;
		}
		
		if (strcmp(szAPName, stinfo.baName)!=0)
		{	
			if (stinfo.baName[0] != 0x00)
			{
				inRetVal = inMultiAP_RunIPCCmdTypesEx(stinfo.baName, IPC_EVENT_ID, inStr,inlen,byStr,&Outlen);    

				vdDebug_LogPrintf("baName[%x][%x] ",byStr[0],byStr[1]);

			}
		}
	}
	return d_OK;
}


int inCTOS_MultiAPBatchSearch(int IPC_EVENT_ID)
{
	int inResult = d_NO;

    vdDebug_PrintOnPaper("MultiAPBatchSearch");
	
	vdDebug_PrintOnPaper("MultiAPBatchSearch [%02X%02X%02X]",srTransRec.szInvoiceNo[0], srTransRec.szInvoiceNo[1], srTransRec.szInvoiceNo[2]);

	inResult = inDatabase_BatchSearch(&srTransRec, srTransRec.szInvoiceNo);

	DebugAddSTR("inCTOS_BatchSearch","Processing...",20);
	
	DebugAddINT("inCTOS_BatchSearch", inResult);

	if(inResult != d_OK)
    {
		vdDebug_PrintOnPaper("1. NO RECORD FOUND");
		inResult = inCTOS_MultiAPALLAppEventID(IPC_EVENT_ID);
		if ((inResult == d_SUCCESS)||(inResult == d_FAIL))
			return d_NO;

        if(IPC_EVENT_ID == d_IPC_CMD_VOID_SALE) 
        {
            inResult = inCTOS_BatchSearchByInvNo(); /*Void of Preauth*/
            if (inResult != d_OK)
            {
                vdSetErrorMessage("NO RECORD FOUND");
                return d_NO;
            }
        }
        else
        {
			vdDebug_PrintOnPaper("2. NO RECORD FOUND");
            vdSetErrorMessage("NO RECORD FOUND");
            return d_NO;
        }
    }

	vdDebug_PrintOnPaper("RECORD FOUND");
	
    memcpy(srTransRec.szOrgDate, srTransRec.szDate, 2);
    memcpy(srTransRec.szOrgTime, srTransRec.szTime, 3);
	
	return inResult;
}

int inCTOS_MultiAPSaveData(int IPC_EVENT_ID)
{
	BYTE bret;
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	char szAPName[25];
	int inAPPID;
	
	memset(outbuf,0x00,sizeof(outbuf));
	memset(szAPName,0x00,sizeof(szAPName));

	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("szAPName[%s]=[%s]", strHDT.szAPName,szAPName);

	if (strlen(strHDT.szAPName) == 0)
		return d_NO;
	
	if (strcmp (strHDT.szAPName,szAPName) == 0)
		return d_OK;

	inTCTSave(1);

	bret= inMultiAP_Database_BatchDelete();
	vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("MultiAP BatchDelete ERR");
    	return bret;
	}
	
	bret = inMultiAP_Database_BatchInsert(&srTransRec);
	vdDebug_LogPrintf("inMultiAP_Database_BatchInsert=[%d]", bret);
	if(d_OK != bret)
	{
		vdSetErrorMessage("MultiAP BatchInsert ERR");
    	return bret;
	}


	vdDebug_LogPrintf("szAPName[%s],bret=[%d]", strHDT.szAPName,bret);
	
	inMultiAP_RunIPCCmdTypes(strHDT.szAPName,IPC_EVENT_ID,"",0, outbuf,&out_len);

	inTCTRead(1);
	
	if (outbuf[0] != IPC_EVENT_ID)
	{
		//vdSetErrorMessage("MultiAP Type ERR");
		return d_NO;
	}

	if (outbuf[1] != d_SUCCESS)
	{
		//vdSetErrorMessage("MultiAP ERR");
		return d_NO;
	}
	
	return d_ON;
}

int inCTOS_MultiAPCheckAllowd(void)
{
	char szAPName[25];
	int inAPPID;
	
	memset(szAPName,0x00,sizeof(szAPName));

	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("szAPName[%s]=[%s]", strHDT.szAPName,szAPName);

	if (strcmp (strHDT.szAPName,szAPName) == 0)
		return d_OK;

	vdDisplayErrorMsg(1, 8, "FAIL FIND AP");
	return d_ON;
}

int inCTOS_MultiAPGetData(void)
{
	BYTE bret;
	
	vdDebug_LogPrintf("inCTOS_MultiAPGetData-----");
	//CTOS_LCDTPrintXY(1, 8, "LOADING TABLE... ");
	
	bret = inMultiAP_Database_BatchRead(&srTransRec);
	vdDebug_LogPrintf("inMultiAP_Database_BatchReadbret=[%d]", bret);
    
    DebugAddHEX("ECR AMOUNT", srTransRec.szBaseAmount, 6);
	if(d_OK != bret)
	{
		//vdSetErrorMessage("MultiAP BatchRead ERR");
    	return bret;
	}

	bret= inMultiAP_Database_BatchDelete();
	vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
	if(d_OK != bret)
	{
		//vdSetErrorMessage("MultiAP Batch Delete ERR");
    	return bret;
	}

	return d_OK;
	
}

int inCTOS_MultiAPReloadHost(void)
{
	short shGroupId ;
    int inHostIndex;
    short shCommLink;
	int inCurrencyIdx=0;
	
    inHostIndex = (short) srTransRec.HDTid;

	vdDebug_LogPrintf("inCTOS_MultiAPReloadTable,HDTid=[%d]",inHostIndex);
	
	if ( inHDTRead(inHostIndex) != d_OK)
	{
		vdSetErrorMessage("HOST SELECTION ERR");
		return(d_NO);
    } 
    else 
	{
		strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

		inCurrencyIdx = strHDT.inCurrencyIdx;
		
		if (inCSTRead(inCurrencyIdx) != d_OK)
		{	
			vdSetErrorMessage("LOAD CST ERR");
	        return(d_NO);
        }

		if ( inCPTRead(inHostIndex) != d_OK)
		{
			vdSetErrorMessage("LOAD CST ERR");
			return(d_NO);
	    }
    }

	return (d_OK);
	
}


int inCTOS_MultiAPReloadTable(void)
{
	short shGroupId ;
    int inHostIndex;
    short shCommLink;
	int inCurrencyIdx=0;
	
	vdDebug_LogPrintf("inCTOS_MultiAPReloadTable-CDTid[%d]--IITid[%d]--",srTransRec.CDTid,srTransRec.IITid);
	if (srTransRec.CDTid > 0)
	{
		if ( inCDTRead(srTransRec.CDTid) != d_OK)
		{
		    vdDebug_LogPrintf("LOAD CDT ERROR [%d]", srTransRec.CDTid);
			vdSetErrorMessage("LOAD CDT ERROR");
			return(d_NO);
	    }
	}

 	if (srTransRec.IITid > 0) 
    	inGetIssuerRecord(srTransRec.IITid);

    inHostIndex = (short) strCDT.HDTid;
	
	vdDebug_LogPrintf("inCTOS_SelectHost =[%d],HDTid=[%d]",inHostIndex,strCDT.HDTid); 
	if ( inHDTRead(inHostIndex) != d_OK)
	{
		vdDisplayErrorMsg(1, 8, "HOST SELECTION ERR");
		return(d_NO);
    } 
    else 
	{
		srTransRec.HDTid = inHostIndex;
		inCurrencyIdx = strHDT.inCurrencyIdx;

		if (inCSTRead(inCurrencyIdx) != d_OK)
		{	
	        vdDisplayErrorMsg(1, 8, "CST ERR");  
	        return(d_NO);
        }

		if ( inCPTRead(inHostIndex) != d_OK)
		{
	        vdDisplayErrorMsg(1, 8, "CPT ERR");
			return(d_NO);
	    }
    }

	return (d_OK);
	
}

int inCTOS_MultiAPGetpowrfail(void)
{
	int inResult = d_NO;
	
	inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_POWER_FAIL);
	if ((inResult == d_SUCCESS)||(inResult == d_FAIL))
			return d_NO;
}

int inCTOS_MultiAPGetVoid(void)
{
	BYTE bret;
	TRANS_DATA_TABLE srTransRectmp;
	
	vdDebug_LogPrintf("inCTOS_MultiAPGetVoid-----ginReadDBFlag=[%d]",ginReadDBFlag);
	if (ginReadDBFlag == 1)
		memcpy(srTransRec.szInvoiceNo,gszInvoiceNo,3);
	else
	{
		bret = inMultiAP_Database_BatchRead(&srTransRectmp);
		vdDebug_LogPrintf("inMultiAP_Database_BatchReadbret=[%d]", bret);
		if(d_OK != bret)
	    	return bret;

		bret= inMultiAP_Database_BatchDelete();
		vdDebug_LogPrintf("inMultiAP_Database_BatchDelete,bret=[%d]", bret);
		if(d_OK != bret)
	    	return bret;

		memcpy(srTransRec.szInvoiceNo,srTransRectmp.szInvoiceNo,3);
	}		
	
	vdDebug_LogPrintf("inCTOS_MultiAPGetVoid =[%02x%02x%02x]",srTransRec.szInvoiceNo[0],srTransRec.szInvoiceNo[1],srTransRec.szInvoiceNo[2]);

	return d_OK;
}

void vdCTOSS_DislplayMianAPMenu(int IPC_EVENT_ID)
{
	//CTOS_LCDTClearDisplay();
		
	switch(IPC_EVENT_ID)
    {
        case d_IPC_CMD_FUN_KEY1:
            inF1KeyEvent();
            break;
        case d_IPC_CMD_FUN_KEY2:
            inF2KeyEvent();
            break;
        case d_IPC_CMD_FUN_KEY3:
            inF3KeyEvent();
            break;
		case d_IPC_CMD_FUN_KEY4:
            inF4KeyEvent();
            break;
		default:
        	break;
    }

	return;
}

int inCTOSS_MultiDislplayMenu(int IPC_EVENT_ID)
{
	CTOS_stCAPInfo stinfo;
	int inLoop = 0;
    char szHostMenu[1024];
	int inHostindex = 0;
    char szHostName[d_MAX_APP+1][25];
	BYTE outbuf[d_MAX_IPC_BUFFER];
	USHORT out_len = 0;
	char szAPName[25];
	int inAPPID;

	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
	char szHeaderString[50] = "SELECT AP";

	
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
	
	vdDebug_LogPrintf("begain inCTOSS_MultiDislplayMenu............");
	memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));
	
  	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
 			CTOS_APGet(inLoop, &stinfo);
		}
		
 		vdDebug_LogPrintf("baName[%s] ", stinfo.baName);
 
		if (memcmp(stinfo.baName, "SHARLS_", 7) == 0)
			continue;

		if (stinfo.baName[0] != (char)0x00)
		{
			strncpy(szHostName[inHostindex++],stinfo.baName,25);
		}
	}

	for (inLoop = 0; inLoop < d_MAX_APP; inLoop++)
    {
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");      
        }
        else
            break;
    }

	vdDebug_LogPrintf("inHostindex=[%d]..........", inHostindex);
	if (inHostindex == 1)//only one app 
	{
		vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
		return d_OK;
	}
	
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;  
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;
        
        vdDebug_LogPrintf("key=[%d],szHostName =[%s]", key, szHostName[key-1]);

		if (strcmp(szAPName, szHostName[key-1])==0)
		{
			vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
			return d_OK;
		}
		inTCTSave(1);
		
		inMultiAP_RunIPCCmdTypes(szHostName[key-1],IPC_EVENT_ID,"",0, outbuf,&out_len);

		inTCTRead(1);

		vdDebug_LogPrintf("end inCTOSS_MultiDislplayMenu............");
		
		if (outbuf[0] != IPC_EVENT_ID)
		{
			return d_NO;
		}

		if (outbuf[1] != d_SUCCESS)
		{
			return d_NO;
		}
	
	}
	
 	return d_OK;
}

int inCTOSS_SetALLApFont(char *font)
{
	memset(gszFontName,0x00,sizeof(gszFontName));
	if (strlen(font) > 0)
		strcpy(gszFontName,font);

	vdDebug_LogPrintf("inCTOSS_SetALLApFont=[%s]............",gszFontName);
	inCTOS_MultiAPALLAppEventIDEx(d_IPC_CMD_SETFONT);

	return d_OK;
}



void vdCTOSS_MultiAPAnalysis(BYTE *inbuf, USHORT inlen)
{
	USHORT intmplen = 0;
	BYTE tmpbuf[50+1];

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	memset(gszInvoiceNo,0x00,sizeof(gszInvoiceNo));
	ginReadDBFlag = 0;
	
	usCTOSS_GetTagLenValue(inbuf, inlen, MULTI_AP_READ_DB_FLAG, &intmplen, tmpbuf);
	ginReadDBFlag = atoi(tmpbuf);
	vdDebug_LogPrintf("ginReadDBFlag =[%d],intmplen=[%d],tmpbuf=[%s]", ginReadDBFlag,intmplen,tmpbuf);
	
	usCTOSS_GetTagLenValue(inbuf, inlen, MULTI_AP_INVOICE, &intmplen, gszInvoiceNo);
	vdPCIDebug_HexPrintf("gszInvoiceNo",gszInvoiceNo,intmplen);
}


void vdCTOSS_MultiAPAnalysisECR(BYTE *inbuf, USHORT inlen)
{
	//ECR_REQ ECRReq; 
	//vdDebug_LogPrintf("vdCTOSS_MultiAPAnalysisECR check ECR on?[%d]fGetECRTransactionFlgEx(%d)", strTCT.fECR,fGetECRTransactionFlgEx());
	vdDebug_LogPrintf("vdCTOSS_MultiAPAnalysisECR check ECR on?[%d]", strTCT.fECR);
	if (strTCT.fECR == 1 /*&& (fGetECRTransactionFlgEx() == TRUE)*/) // tct
	{
		memset(&ECRReq,0x00,sizeof(ECRReq));
		memcpy(&ECRReq, inbuf,sizeof(ECR_REQ));

        vdDebug_LogPrintf("req_resp=[%s]",ECRReq.req_resp);
		
		vdDebug_LogPrintf("sizeof(ECR_REQ)=[%d]",sizeof(ECR_REQ));
		vdDebug_LogPrintf("txn_code=[%s]",ECRReq.txn_code);
		vdDebug_LogPrintf("amount=[%s]",ECRReq.amount);
		vdDebug_LogPrintf("inv_no=[%s]",ECRReq.inv_no);
		vdDebug_LogPrintf("Pan=[%s]",ECRReq.Pan);

		vdDebug_LogPrintf("szECRREF=[%s]",ECRReq.szECRREF);
		vdDebug_LogPrintf("szAuthCode=[%s]",ECRReq.szAuthCode);
		vdDebug_LogPrintf("szMID=[%s]",ECRReq.szMID);
		
        vdDebug_LogPrintf("szTipAmount=[%s]",ECRReq.szTipAmount);
	}
}


