#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <sqlite3.h>

#include "../Includes/myEZLib.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"

#include "../Includes/Trans.h"

#include "../Database/DatabaseFunc.h"
#include "../FileModule/myFileFunc.h"
#include "../Debug/Debug.h"
#include "../Database/DatabaseFunc.h"
#include "../Includes/POSSetting.h"
#include "../Debug/Debug.h"



#include "ShareDCCFile.h"

//DCC_CFG		strDCCCfg;

DCC_SHARE_DATA	stDCCShareData;


#if 0
/*if already defined in current application, extern*/
extern sqlite3 * db;
extern sqlite3_stmt *stmt;
extern int inStmtSeq;



int inDCCCFGRead(int inSeekCnt)
{
	int result;
	int len = 0;
	int inResult = -1;
	int incount = 0;
 	char *sql = "SELECT fDCCFlag, inCommMode, szHostIP, ulHostPort, szSecHostIP, ulSecHostPort, szPriPhoneNo, szSecPhoneNo, inHDTIndex, szTPDU, inIPHeader, szReserved1, szReserved2, szReserved3 FROM DCCCFG WHERE CFGid = ?";

	vdDebug_LogPrintf("inDCCCFGRead[%d]", inSeekCnt);


	/* open the database */
	result = sqlite3_open(DB_DCCCFG,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	vdDebug_LogPrintf("sqlite3_open[%d]", inSeekCnt);
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
			
			/* fDCCFlag */
			strDCCCfg.fDCCFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq));  
			vdDebug_LogPrintf("fDCCFlag [%d]", strDCCCfg.fDCCFlag);
			/*inCommMode*/
			strDCCCfg.inCommMode = sqlite3_column_int(stmt,inStmtSeq +=1 );			

			/* szHostIP*/
			strcpy((char*)strDCCCfg.szHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*ulHostPort*/
			strDCCCfg.ulHostPort = sqlite3_column_int(stmt,inStmtSeq +=1 );			

			/* szSecHostIP*/
			strcpy((char*)strDCCCfg.szSecHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*ulSecHostPort*/
			strDCCCfg.ulSecHostPort = sqlite3_column_int(stmt,inStmtSeq +=1 );			

			/* szPriPhoneNo*/
			strcpy((char*)strDCCCfg.szPriPhoneNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szSecPhoneNo*/
			strcpy((char*)strDCCCfg.szSecPhoneNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));			

			/*inHDTIndex*/
			strDCCCfg.inHDTIndex = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/* szTPDU */
			memcpy(strDCCCfg.szTPDU, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);

			/*inIPHeader*/
			strDCCCfg.inIPHeader = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/* szReserved1 */
			strcpy((char*)strDCCCfg.szReserved1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szReserved2 */
			strcpy((char*)strDCCCfg.szReserved2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szReserved3 */
			strcpy((char*)strDCCCfg.szReserved3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			vdDebug_LogPrintf("szReserved3 [%s]", strDCCCfg.szReserved3);
        }
	} while (result == SQLITE_ROW);	

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	vdDebug_LogPrintf("inDCCCFGRead inResult [%d]", inResult);
    return(inResult);
}


int inDCCCFGSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE DCCCFG SET fDCCFlag = ?, inCommMode = ?, szHostIP = ?, ulHostPort = ?, szSecHostIP = ?, ulSecHostPort = ?, szPriPhoneNo = ?, szSecPhoneNo = ?, inHDTIndex = ?, szTPDU = ?, inIPHeader = ? WHERE CFGid = ?";    		

	/* open the database */
	result = sqlite3_open(DB_DCCCFG,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);   
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;

	
	/* fERMFlag */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCCCfg.fDCCFlag);

	/* inCommMode */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCCCfg.inCommMode);	

	/* szHostIP */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strDCCCfg.szHostIP, strlen((char*)strDCCCfg.szHostIP), SQLITE_STATIC);

	/* ulHostPort */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCCCfg.ulHostPort);

	/* szSecHostIP */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strDCCCfg.szSecHostIP, strlen((char*)strDCCCfg.szSecHostIP), SQLITE_STATIC);

	/* ulSecHostPort */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCCCfg.ulSecHostPort);	

	/* szPriPhoneNo */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strDCCCfg.szPriPhoneNo, strlen((char*)strDCCCfg.szPriPhoneNo), SQLITE_STATIC);

	/* szSecPhoneNo */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strDCCCfg.szSecPhoneNo, strlen((char*)strDCCCfg.szSecPhoneNo), SQLITE_STATIC);

	/* inHDTIndex */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCCCfg.inHDTIndex);	

	/* inIPHeader */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strDCCCfg.inIPHeader);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}
#endif

/*set share DCC config param, user may modify this function to pass DCC param*/
void vdSetShareDCCCfgParam(VS_BOOL fSetFlag)
{
	/*prepare the share DCC params frist*/
	//inDCCCFGRead(1);

	/*no need set*/
	if (VS_FALSE == fSetFlag)
	{
		stDCCShareData.fSetCfg = VS_FALSE;
		memset(&stDCCShareData.stDCCCfg, 0x00, sizeof(DCC_CFG));
		return;
	}

	stDCCShareData.fSetCfg = VS_TRUE;
	
	stDCCShareData.stDCCCfg.fDCCFlag = 1;

	stDCCShareData.stDCCCfg.inCommMode = strCPT.inCommunicationMode;

	strcpy(stDCCShareData.stDCCCfg.szHostIP, strCPT.szPriTxnHostIP);
	stDCCShareData.stDCCCfg.ulHostPort = strCPT.inPriTxnHostPortNum;

	strcpy(stDCCShareData.stDCCCfg.szSecHostIP, strCPT.szSecTxnHostIP);
	stDCCShareData.stDCCCfg.ulSecHostPort = strCPT.inSecTxnHostPortNum;

	//strDCCCfg.inHDTIndex = strHDT.inHostIndex;

	memcpy(stDCCShareData.stDCCCfg.szTPDU, strHDT.szTPDU, 5);


	/*hard code here for testing only*/
	stDCCShareData.stDCCCfg.inCommMode = 1;
	strcpy(stDCCShareData.stDCCCfg.szHostIP, "192.168.0.101");
	stDCCShareData.stDCCCfg.ulHostPort = 5011;
	stDCCShareData.stDCCCfg.inHDTIndex = 0;
	memcpy(stDCCShareData.stDCCCfg.szTPDU, "\x60\x00\x02\x00\x00", 5);
	stDCCShareData.stDCCCfg.inIPHeader = 1;
	
	
	//inDCCCFGSave(1);
}

/*set share DCC trans data, user may modify this function to pass DCC param*/
void vdSetShareDCCTransData(VS_BOOL fSetFlag)
{
	/*no need set*/
	if (VS_FALSE == fSetFlag)
	{
		stDCCShareData.fSetTxn = VS_FALSE;
		memset(&stDCCShareData.stTxnData, 0x00, sizeof(DCC_STD));
		return;
	}

	/*hard code here for testing only*/
	stDCCShareData.fSetTxn = VS_TRUE;
	
	memcpy(stDCCShareData.stTxnData.szTime, "\x12\x00\x00", 3);
	memcpy(stDCCShareData.stTxnData.szDate, "\x02\x12", 2);
	strcpy(stDCCShareData.stTxnData.szCardNum, "42106800000");
	strcpy(stDCCShareData.stTxnData.szCardType, "DCC");
}


void vdSetDCCShareData(BYTE *pszData, USHORT *inLen)
{
	vdSetShareDCCCfgParam(VS_TRUE);
	
	vdSetShareDCCTransData(VS_TRUE);

	memcpy(pszData, &stDCCShareData, DCC_SHARE_DATA_SIZE);
	*inLen = DCC_SHARE_DATA_SIZE;
	
}

