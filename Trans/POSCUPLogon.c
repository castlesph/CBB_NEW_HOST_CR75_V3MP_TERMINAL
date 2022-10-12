/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

#include "..\Includes\POSTypedef.h"
#include "..\Debug\Debug.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Database\DatabaseFunc.h"
#include "..\erm\Poserm.h"


int inCUPCheckMK(void)
{
	return d_OK;
}

int inCTOS_CUPLogonFlowProcess(void)
{
    int inRet = d_NO;
	
    vdCTOS_SetTransType(CUP_LOGON);
    
    //display title
    vdDispTransTitle(CUP_LOGON);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectCUPHost();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	/*Display OK*/
	vduiDisplayStringCenter(8, "LOGON SUCCESS");
    vduiWarningSound();
	WaitKey(10);

    return ST_SUCCESS;
}

int inCTOS_CUP_LOGON(void)
{
    int inRet = d_NO;

    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCTOS_CUPLogonFlowProcess();

    vdCTOS_TransEndReset();

    return inRet;
}


int inCBBLogonFlowProcess(void)
{
    int inRet = d_NO;	

    vdCTOS_SetTransType(CUP_LOGON);
    
    //display title
    vdDispTransTitle(CUP_LOGON);

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SelectCBBHost();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;


    inRet = inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet; 	

    inRet = inCTOS_inDisconnect();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	/*Display OK*/
	vduiDisplayStringCenter(8, "LOGON SUCCESS");
    vduiWarningSound();
	WaitKey(10);
	

    return ST_SUCCESS;
}


int inCTOS_CBB_LOGON(void)
{
    int inRet = d_NO;	
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
    
    inRet = inCBBLogonFlowProcess();

    vdCTOS_TransEndReset();

    return inRet;
}



