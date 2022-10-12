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


#include "Posdcc.h"
#include "ShareDCCFile.h"


/*DCC transaction flow*/
USHORT usCTOSS_DCC_RateRequest(void)
{
    BYTE bInBuf[512];
    BYTE bOutBuf[512];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	/*set DCC Param, you may also set it during terminal start up */
	vdSetDCCShareData(bInBuf, &usInLen);
	
	/*Call Share DCC application*/
	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_DCC", d_IPC_CMD_DCC_RATE_REQUEST, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_DCC_RateRequest usOutLen[%d]", usOutLen);
    return usResult;
}

USHORT usCTOSS_DCC_TransLogging(void)
{
    BYTE bInBuf[512];
    BYTE bOutBuf[512];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	/*set DCC Param, you may also set it during terminal start up */
	vdSetDCCShareData(bInBuf, &usInLen);
	
	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_DCC", d_IPC_CMD_DCC_TRANS_LOGGING, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_DCC_TransLogging usOutLen[%d] bOutBuf[%s]", usOutLen, bOutBuf);
    return usResult;
}


