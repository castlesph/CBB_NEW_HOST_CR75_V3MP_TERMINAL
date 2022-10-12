/** 
**    A Template for developing new terminal shared application
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <sqlite3.h>

#include "../Includes/myEZLib.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"

#include "../Includes/Trans.h"

#include "DatabaseFunc.h"
#include "../FileModule/myFileFunc.h"
#include "../Debug/Debug.h"
#include "../Database/DatabaseFunc.h"
#include "../Includes/POSSetting.h"

sqlite3 * db;
sqlite3_stmt *stmt;
int inStmtSeq = 0;

void vdSetJournalModeOff(void)
{
	int result;
	char *sql2 = "PRAGMA journal_mode=off";
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
	//vdDebug_LogPrintf("sqlite3_prepare_v2 sql2=[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return;
	}

	do
	{
		result = sqlite3_step(stmt);
		//vdDebug_LogPrintf("sqlite3_step sql2=[%d]",result);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			//vdDebug_LogPrintf("sqlite3_step sql2=[%s]",sqlite3_column_text(stmt,inStmtSeq));
		}
	}while (result == SQLITE_ROW);
	
	result = sqlite3_finalize(stmt);
	//vdDebug_LogPrintf("sqlite3_finalize sql2=[%d]",result);
}

int inHDTCheckHostName(char szHostName[][100], int inCPTID[], int inType)
{    
	int result;
	char *sql = "SELECT HDTid, szHostLabel FROM HDT WHERE HDTid IN (SELECT DISTINCT HDTid FROM CDT WHERE inType = ? and fCDTEnable = 1) AND fHostEnable = 1";
	int inCount = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return -1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return -2;
	}

	
	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inType);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			/* HDTid */
			inCPTID[inCount] = sqlite3_column_int(stmt,inStmtSeq);
                        
                        strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq += 1));
			inCount++;
		}
	} while (result == SQLITE_ROW);

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	vdDebug_LogPrintf("inDBResult[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return inCount;
}

int inHDTReadHostName(char szHostName[][100], int inCPTID[])
{    
	int result;
	char *sql = "SELECT HDTid, szHostLabel FROM HDT WHERE fHostEnable = ?";
	int inCount = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			/* HDTid */
			inCPTID[inCount] = sqlite3_column_int(stmt,inStmtSeq);

            /* szHostName */
			strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			inCount++;
		}
	} while (result == SQLITE_ROW);

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	vdDebug_LogPrintf("inDBResult[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}



int inHDTReadHDTid(int inHDTID[])
{    
	int result;
	char *sql = "SELECT HDTid FROM HDT";
	int inCount = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	//sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			/* HDTid */
			inHDTID[inCount] = sqlite3_column_int(stmt,inStmtSeq);
			vdDebug_LogPrintf("inHDTID[%d]",inHDTID[inCount]);

            /* szHostName */
			//strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			inCount++;
		}
	} while (result == SQLITE_ROW);

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	vdDebug_LogPrintf("inDBResult[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

//for COM&USB communication
int inCPTRead(int inSeekCnt)
{     
	int result;
	//char *sql = "SELECT HDTid, szHostName,inCommunicationMode, inSecCommunicationMode, szPriTxnPhoneNumber, szSecTxnPhoneNumber, szPriSettlePhoneNumber, szSecSettlePhoneNumber, fFastConnect, fDialMode, inDialHandShake, szATCommand, inMCarrierTimeOut, inMRespTimeOut, fPreDial, szPriTxnHostIP, szSecTxnHostIP, szPriSettlementHostIP, szSecSettlementHostIP, inPriTxnHostPortNum, inSecTxnHostPortNum, inPriSettlementHostPort, inSecSettlementHostPort, fTCPFallbackDial, fSSLEnable, inTCPConnectTimeout, inTCPResponseTimeout, inPriTxnComBaudRate, inSecTxnComBaudRate, inCountryCode, inHandShake, inParaMode, inIPHeader FROM CPT WHERE CPTid = ?";
	char *sql = "SELECT HDTid, szHostName,inCommunicationMode, inSecCommunicationMode, szPriTxnPhoneNumber, szSecTxnPhoneNumber, szPriSettlePhoneNumber, szSecSettlePhoneNumber, fFastConnect, fDialMode, inDialHandShake, szATCommand, inMCarrierTimeOut, inMRespTimeOut, fPreDial, szPriTxnHostIP, szSecTxnHostIP, szPriSettlementHostIP, szSecSettlementHostIP, inPriTxnHostPortNum, inSecTxnHostPortNum, inPriSettlementHostPort, inSecSettlementHostPort, fTCPFallbackDial, fSSLEnable, inTCPConnectTimeout, inTCPResponseTimeout, inPriTxnComBaudRate, inSecTxnComBaudRate, inCountryCode, inHandShake, inParaMode, inIPHeader, szPriTxnIP_2, szSecTxnIP_2, szPriSettleIP_2, szSecSettleIP_2, inPriTxnPort_2, inSecTxnPort_2, inPriSettlePort_2, inSecSettlePort_2 FROM CPT WHERE CPTid = ?";
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	//lnGetFileSize(DB_TERMINAL_JOURNAL);
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	result = sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* HDTid */
			strCPT.HDTid = sqlite3_column_int(stmt, inStmtSeq );   

			/* szHostName*/
			strcpy((char*)strCPT.szHostName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inCommunicationMode */
			strCPT.inCommunicationMode = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inSecCommunicationMode */
			strCPT.inSecCommunicationMode = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* szPriTxnPhoneNumber; */
			strcpy((char*)strCPT.szPriTxnPhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szSecTxnPhoneNumber*/
			strcpy((char*)strCPT.szSecTxnPhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szPriSettlePhoneNumber*/			 //5
			strcpy((char*)strCPT.szPriSettlePhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


			/* szSecSettlePhoneNumber*/
			strcpy((char*)strCPT.szSecSettlePhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fFastConnect*/
			strCPT.fFastConnect = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));   

			/* fDialMode*/
			strCPT.fDialMode = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
                        
			/* inDialHandShake*/
			strCPT.inDialHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* szATCommand*/
			strcpy((char*)strCPT.szATCommand, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inMCarrierTimeOut*/
			strCPT.inMCarrierTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* inMRespTimeOut*/
			strCPT.inMRespTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        
            /* fPreDial*/
			strCPT.fPreDial = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

			/* szPriTxnHostIP*/
			strcpy((char*)strCPT.szPriTxnHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
						
                        /* szSecTxnHostIP*/
			strcpy((char*)strCPT.szSecTxnHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
                        /* szPriSettlementHostIP*/
			strcpy((char*)strCPT.szPriSettlementHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
	                
                        /* szSecSettlementHostIP*/
			strcpy((char*)strCPT.szSecSettlementHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		        
                        /* inPriTxnHostPortNum*/
			strCPT.inPriTxnHostPortNum = sqlite3_column_int(stmt, inStmtSeq +=1 );
                          
                        /* inSecTxnHostPortNum*/
			strCPT.inSecTxnHostPortNum = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        
                        /* inPriSettlementHostPort*/
			strCPT.inPriSettlementHostPort = sqlite3_column_int(stmt, inStmtSeq +=1 );
                         
                        /* inSecSettlementHostPort*/
			strCPT.inSecSettlementHostPort = sqlite3_column_int(stmt, inStmtSeq +=1 );
                        
			/*fTCPFallbackDial*/
			strCPT.fTCPFallbackDial = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        			
                        /*fSSLEnable*/
			strCPT.fSSLEnable =fGetBoolean((BYTE *) sqlite3_column_text(stmt,inStmtSeq +=1 ));
                       
                        /*inTCPConnectTimeout*/
			strCPT.inTCPConnectTimeout = sqlite3_column_int(stmt, inStmtSeq +=1 );
                            
                        /*inTCPResponseTimeout*/
			strCPT.inTCPResponseTimeout = sqlite3_column_int(stmt, inStmtSeq +=1 );
						/*inPriTxnComBaudRate*/
			strCPT.inPriTxnComBaudRate = sqlite3_column_int(stmt, inStmtSeq +=1 );
                            
                        /*inSecTxnComBaudRate*/
			strCPT.inSecTxnComBaudRate = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/*inCountryCode*/
			strCPT.inCountryCode = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/*inHandShake*/
			strCPT.inHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/*inParaMode*/
			strCPT.inParaMode = sqlite3_column_int(stmt, inStmtSeq +=1 );

            /*inIPHeader*/
			strCPT.inIPHeader = sqlite3_column_int(stmt, inStmtSeq +=1 );   

            /*for SIM 2*/
            /* szPriTxnIP_2*/
            strcpy((char*)strCPT.szPriTxnIP_2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szSecTxnIP_2*/
            strcpy((char*)strCPT.szSecTxnIP_2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szPriSettleIP_2*/
            strcpy((char*)strCPT.szPriSettleIP_2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szSecSettleIP_2*/
            strcpy((char*)strCPT.szSecSettleIP_2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* inPriTxnPort_2*/
            strCPT.inPriTxnPort_2 = sqlite3_column_int(stmt, inStmtSeq +=1 );
            /* inSecTxnPort_2*/
            strCPT.inSecTxnPort_2 = sqlite3_column_int(stmt, inStmtSeq +=1 );
            /* inPriSettlePort_2*/
            strCPT.inPriSettlePort_2 = sqlite3_column_int(stmt, inStmtSeq +=1 );
            /* inSecSettlePort_2*/
            strCPT.inSecSettlePort_2 = sqlite3_column_int(stmt, inStmtSeq +=1 );
		}
	} while (result == SQLITE_ROW);

    //lnGetFileSize(DB_TERMINAL_JOURNAL);
	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}
//for COM&USB communication
int inCPTSave(int inSeekCnt)
{
	int result;
	//char *sql = "UPDATE CPT SET HDTid = ?, szHostName = ?,inCommunicationMode = ?, inSecCommunicationMode = ?, szPriTxnPhoneNumber = ?, szSecTxnPhoneNumber = ?, szPriSettlePhoneNumber = ?, szSecSettlePhoneNumber = ?, fFastConnect = ?, fDialMode = ?, inDialHandShake = ?, szATCommand = ?, inMCarrierTimeOut = ?, inMRespTimeOut = ?, fPreDial = ?, szPriTxnHostIP = ?, szSecTxnHostIP = ?, szPriSettlementHostIP = ?, szSecSettlementHostIP = ?, inPriTxnHostPortNum = ?, inSecTxnHostPortNum = ?, inPriSettlementHostPort = ?, inSecSettlementHostPort = ?, fTCPFallbackDial = ?, fSSLEnable = ?, inTCPConnectTimeout = ?, inTCPResponseTimeout = ?, inPriTxnComBaudRate = ?, inSecTxnComBaudRate = ?, inCountryCode = ?, inHandShake = ?, inParaMode = ?, inIPHeader= ? WHERE  CPTid = ?";
	char *sql = "UPDATE CPT SET HDTid = ?, szHostName = ?,inCommunicationMode = ?, inSecCommunicationMode = ?, szPriTxnPhoneNumber = ?, szSecTxnPhoneNumber = ?, szPriSettlePhoneNumber = ?, szSecSettlePhoneNumber = ?, fFastConnect = ?, fDialMode = ?, inDialHandShake = ?, szATCommand = ?, inMCarrierTimeOut = ?, inMRespTimeOut = ?, fPreDial = ?, szPriTxnHostIP = ?, szSecTxnHostIP = ?, szPriSettlementHostIP = ?, szSecSettlementHostIP = ?, inPriTxnHostPortNum = ?, inSecTxnHostPortNum = ?, inPriSettlementHostPort = ?, inSecSettlementHostPort = ?, fTCPFallbackDial = ?, fSSLEnable = ?, inTCPConnectTimeout = ?, inTCPResponseTimeout = ?, inPriTxnComBaudRate = ?, inSecTxnComBaudRate = ?, inCountryCode = ?, inHandShake = ?, inParaMode = ?, inIPHeader= ?, szPriTxnIP_2 = ?, szSecTxnIP_2 = ?, szPriSettleIP_2 = ?, szSecSettleIP_2 = ?, inPriTxnPort_2 = ?, inSecTxnPort_2 = ?, inPriSettlePort_2 = ?, inSecSettlePort_2 = ? WHERE  CPTid = ?";
	
 	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	//lnGetFileSize(DB_TERMINAL_JOURNAL);
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	inStmtSeq = 0;
        
        /* HDTid*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.HDTid);
	/* szHostName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szHostName, strlen((char*)strCPT.szHostName), SQLITE_STATIC);
	/* inCommunicationMode*/	
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inCommunicationMode);
	/* inSecCommunicationMode*/	
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecCommunicationMode);
	/* szPriTxnPhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriTxnPhoneNumber, strlen((char*)strCPT.szPriTxnPhoneNumber), SQLITE_STATIC);
        /* szSecTxnPhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecTxnPhoneNumber, strlen((char*)strCPT.szSecTxnPhoneNumber), SQLITE_STATIC);
        /* szPriSettlePhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriSettlePhoneNumber, strlen((char*)strCPT.szPriSettlePhoneNumber), SQLITE_STATIC);
        /* szSecSettlePhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecSettlePhoneNumber, strlen((char*)strCPT.szSecSettlePhoneNumber), SQLITE_STATIC);
        
        /* fFastConnect*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fFastConnect);
        
        /* fDialMode*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fDialMode);
         /* inDialHandShake*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inDialHandShake);       
        
        /* szATCommand*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szATCommand, strlen((char*)strCPT.szATCommand), SQLITE_STATIC);   
        
        /* inMCarrierTimeOut*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inMCarrierTimeOut);       
               
        /* inMRespTimeOut*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inMRespTimeOut);    
   
        /* fPreDial*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fPreDial);
        
	/* szPriTxnHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriTxnHostIP, strlen((char*)strCPT.szPriTxnHostIP), SQLITE_STATIC);
	/* szSecTxnHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecTxnHostIP, strlen((char*)strCPT.szSecTxnHostIP), SQLITE_STATIC);
	/*szPriSettlementHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriSettlementHostIP, strlen((char*)strCPT.szPriSettlementHostIP), SQLITE_STATIC);
	/*szSecSettlementHostIP*/	
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecSettlementHostIP, strlen((char*)strCPT.szSecSettlementHostIP), SQLITE_STATIC);
	/* inPriTxnHostPortNum*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriTxnHostPortNum);
       /* inSecTxnHostPortNum*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecTxnHostPortNum); 
        /* inPriSettlementHostPort*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriSettlementHostPort);          
        /* inSecSettlementHostPort*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecSettlementHostPort);   
        /* fTCPFallbackDial*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fTCPFallbackDial);        
        /* fSSLEnable*/
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.fSSLEnable);           
          /* inTCPConnectTimeout*/	  
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inTCPConnectTimeout);	
        /* inTCPResponseTimeout*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inTCPResponseTimeout);
		/* inPriTxnComBaudRate*/	  
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriTxnComBaudRate);	
        /* inSecTxnComBaudRate*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecTxnComBaudRate);
		/* inCountryCode*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inCountryCode);
		/* inHandShake*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inHandShake);
		/* inParaMode*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inParaMode);
         /* inIPHeader*/
		result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inIPHeader);	 

    /*for SIM 2*/
    /* szPriTxnIP_2*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriTxnIP_2, strlen((char*)strCPT.szPriTxnIP_2), SQLITE_STATIC);
    /* szSecTxnIP_2*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecTxnIP_2, strlen((char*)strCPT.szSecTxnIP_2), SQLITE_STATIC);
    /*szPriSettleIP_2*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szPriSettleIP_2, strlen((char*)strCPT.szPriSettleIP_2), SQLITE_STATIC);
    /*szSecSettleIP_2*/	
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCPT.szSecSettleIP_2, strlen((char*)strCPT.szSecSettleIP_2), SQLITE_STATIC);
    /* inPriTxnPort_2*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriTxnPort_2);
    /* inSecTxnPort_2*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecTxnPort_2); 
    /* inSecTxnPort_2*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inPriSettlePort_2);          
    /* inSecSettlePort_2*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCPT.inSecSettlePort_2);   

	//szPriTxnIP_2 = ?, szSecTxnIP_2 = ?, szPriSettleIP_2 = ?, szSecSettleIP_2 = ?, inPriTxnPort_2 = ?, inSecTxnPort_2 = ?, inPriSettlePort_2 = ?, inSecSettlePort_2 = ?
		
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    //lnGetFileSize(DB_TERMINAL_JOURNAL);
	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(d_OK);
        
}



int inHDTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM HDT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inTCTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TCT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inCDTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM CDT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inIITNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM IIT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inMMTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM MMT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inPITNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM PIT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inRDTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM RDT";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inEMVNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM EMV";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inAIDNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM AID";
	int inCount = 0;
	
	/* open the database */
	//result = sqlite3_open(DB_TERMINAL,&db);
	result = sqlite3_open(DB_EMV,&db);
	
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inMSGNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM MSG";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inHDTRead(int inSeekCnt)
{
	int len;
	CTOS_RTC SetRTC;     
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inHostIndex, szHostLabel, szTPDU, szNII, fReversalEnable, fHostEnable, szTraceNo, fSignOn, ulLastTransSavedIndex, inCurrencyIdx, szAPName, inFailedREV, inDeleteREV, inNumAdv, fTPDUOnOff FROM HDT WHERE HDTid = ? AND fHostEnable = ?";

    vdDebug_LogPrintf("inHDTRead = [%d]", inSeekCnt);
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inHostIndex*/
			strHDT.inHostIndex = sqlite3_column_int(stmt, inStmtSeq );

            /* szHostLabel */
			strcpy((char*)strHDT.szHostLabel, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
            
			/*szTPDU*/
			memcpy(strHDT.szTPDU, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);

			/* szNII*/
			memcpy(strHDT.szNII, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);

			/* fReversalEnable*/
			strHDT.fReversalEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fHostEnable*/
			strHDT.fHostEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

	
			/* szTraceNo*/
			memcpy(strHDT.szTraceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

			/* fSignOn */
			strHDT.fSignOn = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*ulLastTransSavedIndex*/
			strHDT.ulLastTransSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );

 			/* inCurrencyIdx */	
			strHDT.inCurrencyIdx = sqlite3_column_int(stmt, inStmtSeq +=1 );
            
			/* szAPName */			
			strcpy((char*)strHDT.szAPName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inFailedREV */
			strHDT.inFailedREV = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* inDeleteREV */
			strHDT.inDeleteREV = sqlite3_column_int(stmt,inStmtSeq +=1);

            /* inNumAdv */
			strHDT.inNumAdv = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* fTPDUOnOff // MPU NH migration*/
			strHDT.fTPDUOnOff = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			

		}
	} while (result == SQLITE_ROW);

	vdDebug_LogPrintf("strHDT.inHostIndex[%d]",strHDT.inHostIndex);
	vdDebug_LogPrintf("strHDT.szHostLabel[%s]",strHDT.szHostLabel);
	vdDebug_LogPrintf("strHDT.inCurrencyIdx[%d]",strHDT.inCurrencyIdx);
	vdDebug_LogPrintf("strHDT.fHostEnable[%d]",strHDT.fHostEnable);
	vdDebug_LogPrintf("strHDT.szTPDU[%s]",strHDT.szTPDU);
	vdDebug_LogPrintf("strHDT.szNII[%s]",strHDT.szNII);
	vdDebug_LogPrintf("strHDT.fTPDUOnOff[%d]",strHDT.fTPDUOnOff);	

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inHDTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE HDT SET inHostIndex = ? , szTPDU = ? ,szNII = ? ,fHostEnable = ? ,szTraceNo = ? ,fSignOn = ?,inFailedREV = ?,inNumAdv = ?, inCurrencyIdx = ? WHERE  HDTid = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	
    vdDebug_LogPrintf("inHDTSave = [%d]", inSeekCnt);

	inStmtSeq = 0;
    /*inHostIndex*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.inHostIndex);
	/*szTPDU*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strHDT.szTPDU, 5, SQLITE_STATIC);
	/* szNII*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strHDT.szNII, 2, SQLITE_STATIC);
	/* fHostEnable*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.fHostEnable);

	/* szTraceNo*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strHDT.szTraceNo, 3, SQLITE_STATIC);
	/* fSignOn */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.fSignOn);

	/*inFailedREV*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.inFailedREV);

    /*inNumAdv*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.inNumAdv);
    /* inCurrencyIdx */	
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strHDT.inCurrencyIdx);
		
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

int inHDTUpdateTraceNum(void) { //@@IBR ADD 20170214
    int result;
    char *sql = "UPDATE HDT SET szTraceNo = ?";


//	vdPCIDebug_HexPrintf("inHDTUpdateTraceNum 1111111");

    /* open the database */
    result = sqlite3_open(DB_TERMINAL, &db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    sqlite3_exec(db, "begin", 0, 0, NULL);
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

    inStmtSeq = 0;

//	vdPCIDebug_HexPrintf("inHDTUpdateTraceNum 22222");
	vdPCIDebug_HexPrintf("strHDT.szTraceNo11",strHDT.szTraceNo,3);
    /*szTraceNo*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strHDT.szTraceNo, 3, SQLITE_STATIC);

//    result = sqlite3_bind_int(stmt, inStmtSeq += 1, inSeekCnt);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        sqlite3_close(db);
        return 3;
    }

    sqlite3_exec(db, "commit;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

//	vdPCIDebug_HexPrintf("inHDTUpdateTraceNum 33333");
    return (d_OK);
}

int inCDTReadMulti(char *szPAN, int *inFindRecordNum)
{
	int result;
	int inResult = d_NO;
	
	char *sql;// = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE CAST(? AS double) BETWEEN CAST(szPANLo AS double) AND CAST(szPANHi AS double) AND fCDTEnable = ?) AND fCDTEnable = ?";
	//char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE CAST(? AS INTEGER) BETWEEN CAST(szPANLo AS INTEGER) AND CAST(szPANHi AS INTEGER) AND fCDTEnable = ?) AND fCDTEnable = ?";
//	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE ? BETWEEN cast(szPANLo as integer) AND cast(szPANHi as integer) AND fCDTEnable = ?";
//	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE CDTid = (SELECT MIN(CDTid) FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE ? BETWEEN CAST(szPANLo AS INTEGER) AND CAST(szPANHi AS INTEGER) AND fCDTEnable = ?))";
	int inDBResult = 0;
	char szTemp[16];
	int inCount=0;
	unsigned long inPAN = 0;
	char szTempSQL[1024+1];

    memset(szTempSQL, 0, sizeof(szTempSQL));
	sql=&szTempSQL;

	vdDebug_LogPrintf("inCDTReadMulti srTransRec.byTransType[%d]", srTransRec.byTransType);
	
    if(srTransRec.byTransType == BALANCE_ENQUIRY || srTransRec.byTransType == TOPUP_RELOAD_TRANS)
	    strcpy(szTempSQL, "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid, fBTR, inBTR, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE CAST(? AS double) BETWEEN CAST(szPANLo AS double) AND CAST(szPANHi AS double) AND fBTR = ?) AND fBTR = ?");
    else
	    strcpy(szTempSQL, "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid, fBTR, inBTR, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE CAST(? AS double) BETWEEN CAST(szPANLo AS double) AND CAST(szPANHi AS double) AND fCDTEnable = ?) AND fCDTEnable = ?");

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTReadMulti,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	//memcpy(szTemp, szPAN, 8);
	//szTemp[8]=0;
	//inPAN = atol(szTemp);
	//vdDebug_LogPrintf("inPAN=%ld",inPAN);

	memcpy(szTemp, szPAN, 12);
	szTemp[12]=0;
	
	
	inStmtSeq = 0;
	//sqlite3_bind_int(stmt, inStmtSeq +=1, inPAN);
	sqlite3_bind_text(stmt, inStmtSeq += 1, (char *)szTemp, strlen(szTemp), SQLITE_STATIC);
	
//	sqlite3_bind_double(stmt, inStmtSeq +=1, inPAN);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strMCDT[inCount].szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strMCDT[inCount].szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strMCDT[inCount].szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strMCDT[inCount].inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strMCDT[inCount].inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strMCDT[inCount].inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strMCDT[inCount].inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strMCDT[inCount].InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strMCDT[inCount].fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strMCDT[inCount].fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strMCDT[inCount].fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strMCDT[inCount].fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strMCDT[inCount].fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strMCDT[inCount].fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strMCDT[inCount].fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strMCDT[inCount].IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strMCDT[inCount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			strMCDT[inCount].CDTid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*Balance/Topup/Reload flag*/
            strMCDT[inCount].fBTR= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*Balance/Topup/Reload value*/
			strMCDT[inCount].inBTR= sqlite3_column_int(stmt,inStmtSeq +=1);


			/* fMagSwipeEnable */
			strMCDT[inCount].fMagSwipeEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fPreAuthAllowed */
			strMCDT[inCount].fPreAuthAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fRefundAllowed*/
			strMCDT[inCount].fRefundAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fCTLSAllowed*/
			strMCDT[inCount].fCTLSAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fAddRefRRNAPP*/
			strMCDT[inCount].fAddRefRRNAPP = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			inCount ++;
			if(inCount >=10)
				break;
		}
	} while (result == SQLITE_ROW);
	
  
  *inFindRecordNum = inCount;
  vdDebug_LogPrintf("inCDTReadMulti Record=%d",*inFindRecordNum);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inCDTReadMultiByType(char *szPAN, int *inFindRecordNum, int inType)
{
	int result;
	int inResult = d_NO;
	
	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE ? BETWEEN cast(szPANLo as integer) AND cast(szPANHi as integer) AND fCDTEnable = ? AND inType = ? limit 0, 1";
	//char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE CDTid = (SELECT MIN(CDTid) FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE ? BETWEEN CAST(szPANLo AS INTEGER) AND CAST(szPANHi AS INTEGER) AND fCDTEnable = ? AND inType = ?))";
	int inDBResult = 0;
	char szTemp[16];
	int inCount=0;
	unsigned long inPAN = 0;
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTRead,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
//	memcpy(szTemp, szPAN, 8);
        memcpy(szTemp, szPAN, 12);
//	szTemp[8]=0;
        szTemp[12]=0;
	inPAN = atol(szTemp);
	vdDebug_LogPrintf("inPAN=%ld",inPAN);
	inStmtSeq = 0;
	//sqlite3_bind_int(stmt, inStmtSeq +=1, inPAN);
	sqlite3_bind_double(stmt, inStmtSeq +=1, inPAN);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1
	sqlite3_bind_int(stmt, inStmtSeq +=1, inType);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strMCDT[inCount].szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strMCDT[inCount].szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strMCDT[inCount].szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strMCDT[inCount].inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strMCDT[inCount].inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strMCDT[inCount].inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strMCDT[inCount].inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strMCDT[inCount].InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strMCDT[inCount].fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strMCDT[inCount].fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strMCDT[inCount].fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strMCDT[inCount].fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strMCDT[inCount].fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strMCDT[inCount].fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strMCDT[inCount].fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strMCDT[inCount].IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strMCDT[inCount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			strMCDT[inCount].CDTid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* fMagSwipeEnable */
			strMCDT[inCount].fMagSwipeEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			

			/* fPreAuthAllowed */
			strMCDT[inCount].fPreAuthAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fRefundAllowed*/
			strMCDT[inCount].fRefundAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fCTLSAllowed*/
			strMCDT[inCount].fCTLSAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fAddRefRRNAPP*/
			strMCDT[inCount].fAddRefRRNAPP = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


			inCount ++;
			if(inCount >=10)
				break;
		}
	} while (result == SQLITE_ROW);
  
  *inFindRecordNum = inCount;
  vdDebug_LogPrintf("inCDTReadMulti Record=%d",*inFindRecordNum);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

/*Get Credit card type only*/
int inCDTReadMultiForIPP(char *szPAN, int *inFindRecordNum)
{
	int result;
	int inResult = d_NO;
	
	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE CAST(? AS double) BETWEEN CAST(szPANLo AS double) AND CAST(szPANHi AS double) AND fCDTEnable = ?) AND fCDTEnable = ? AND inType = ?";
	//char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE CAST(? AS INTEGER) BETWEEN CAST(szPANLo AS INTEGER) AND CAST(szPANHi AS INTEGER) AND fCDTEnable = ?) AND fCDTEnable = ?";
//	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE ? BETWEEN cast(szPANLo as integer) AND cast(szPANHi as integer) AND fCDTEnable = ?";
//	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, CDTid FROM CDT WHERE CDTid = (SELECT MIN(CDTid) FROM CDT WHERE szPANHi = (SELECT MIN(szPANHi) FROM CDT WHERE ? BETWEEN CAST(szPANLo AS INTEGER) AND CAST(szPANHi AS INTEGER) AND fCDTEnable = ?))";
	int inDBResult = 0;
	char szTemp[16];
	int inCount=0;
	unsigned long inPAN = 0;
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTReadMultiForIPP,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	//memcpy(szTemp, szPAN, 8);
	//szTemp[8]=0;
	//inPAN = atol(szTemp);
	//vdDebug_LogPrintf("inPAN=%ld",inPAN);	
	memcpy(szTemp, szPAN, 12);
	szTemp[12]=0;
	
	vdDebug_LogPrintf("szPAN --- szTemp[%s]",szTemp);
	
	inStmtSeq = 0;
	//sqlite3_bind_int(stmt, inStmtSeq +=1, inPAN);
	sqlite3_bind_text(stmt, inStmtSeq += 1, (char *)szTemp, strlen(szTemp), SQLITE_STATIC);
	
//	sqlite3_bind_double(stmt, inStmtSeq +=1, inPAN);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1
	sqlite3_bind_int(stmt, inStmtSeq +=1, 0);//inType = 0

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strMCDT[inCount].szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strMCDT[inCount].szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strMCDT[inCount].szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strMCDT[inCount].inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strMCDT[inCount].inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strMCDT[inCount].inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strMCDT[inCount].inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strMCDT[inCount].InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strMCDT[inCount].fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strMCDT[inCount].fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strMCDT[inCount].fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strMCDT[inCount].fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strMCDT[inCount].fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strMCDT[inCount].fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strMCDT[inCount].fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strMCDT[inCount].IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strMCDT[inCount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			strMCDT[inCount].CDTid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* fMagSwipeEnable */
			strMCDT[inCount].fMagSwipeEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/* fPreAuthAllowed */
			strMCDT[inCount].fPreAuthAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fRefundAllowed*/
			strMCDT[inCount].fRefundAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fCTLSAllowed*/
			strMCDT[inCount].fCTLSAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*fAddRefRRNAPP*/
			strMCDT[inCount].fAddRefRRNAPP = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


			inCount ++;
			if(inCount >=10)
				break;
		}
	} while (result == SQLITE_ROW);
  
  *inFindRecordNum = inCount;
  vdDebug_LogPrintf("inCDTReadMulti Record=%d",*inFindRecordNum);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


int inCDTRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql; // = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, fBTR, inBTR FROM CDT WHERE CDTid = ? AND fCDTEnable = ?";
    char szTempSQL[1024+1];

    memset(szTempSQL, 0, sizeof(szTempSQL));
	sql=&szTempSQL;

	vdDebug_LogPrintf("inCDTReadMulti srTransRec.byTransType[%d] inSeekCnt [%d]", srTransRec.byTransType, inSeekCnt);
	
    if(srTransRec.byTransType == BALANCE_ENQUIRY || srTransRec.byTransType == TOPUP_RELOAD_TRANS)
	    strcpy(szTempSQL, "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, fBTR, inBTR, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE CDTid = ? AND fBTR = ?");
    else
	    strcpy(szTempSQL, "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, fBTR, inBTR, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE CDTid = ? AND fCDTEnable = ?");
	
	int inDBResult = 0;	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTRead,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strCDT.szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strCDT.szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strCDT.szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strCDT.inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strCDT.inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strCDT.inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strCDT.inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strCDT.InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strCDT.fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strCDT.fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strCDT.fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strCDT.fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strCDT.fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strCDT.fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strCDT.fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strCDT.IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strCDT.HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);	

			/*Balance/Topup/Reload flag*/
            strCDT.fBTR= fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*Balance/Topup/Reload value*/
			strCDT.inBTR= sqlite3_column_int(stmt,inStmtSeq +=1);

		/* fMagSwipeEnable */
		strCDT.fMagSwipeEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			

		/* fPreAuthAllowed */
		strCDT.fPreAuthAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fRefundAllowed*/
		strCDT.fRefundAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fCTLSAllowed*/
		strCDT.fCTLSAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fAddRefRRNAPP*/
		strCDT.fAddRefRRNAPP = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


		}
	} while (result == SQLITE_ROW);
   
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inCDTMAX(void)
{
	int result;	
	char *sql = "SELECT MAX(CDTid) FROM CDT";
	int inMaxValue = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			inMaxValue = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);
    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("inCDTRead,inDBResult[%d]result[%d]",inDBResult,result);
	inDBResult = sqlite3_finalize(stmt);
	
	vdDebug_LogPrintf("inCDTRead,sqlite3_finalize[%d]",inDBResult);
	inDBResult = sqlite3_close(db);
	vdDebug_LogPrintf("inCDTRead,sqlite3_close[%d]",inDBResult);

	return(inMaxValue);
}

int inIITRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
//	char *sql = "SELECT inIssuerNumber, szIssuerAbbrev, szIssuerLabel, szPANFormat, szMaskMerchantCopy, szMaskCustomerCopy, szMaskExpireDate, szMaskDisplay, fMerchExpDate, fCustExpDate, fMerchPANFormat, inCheckHost, ulTransSeqCounter FROM IIT WHERE IITid = ?";
        char *sql = "SELECT inIssuerNumber, szIssuerAbbrev, szIssuerLabel, szPANFormat, szMaskMerchantCopy, szMaskCustomerCopy, szMaskExpireDate, szMaskDisplay, szMaskTID, szMaskMID, fMerchExpDate, fCustExpDate, fMerchPANFormat, inCheckHost, ulTransSeqCounter, fCardVerEnable, fPreAuthEnable, fMPUCard, fUPICard FROM IIT WHERE IITid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("IIT read result[%d]", result); 
		vdDebug_LogPrintf("SQLITE_ROW[%d]", SQLITE_ROW); 
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/*inIssuerNumber*/
			strIIT.inIssuerNumber = sqlite3_column_int(stmt, inStmtSeq );
			vdDebug_LogPrintf("inIssuerNumber [%d]", strIIT.inIssuerNumber);
			/* szIssuerAbbrev */			
			strcpy((char*)strIIT.szIssuerAbbrev, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
				
			/* szIssuerLabel */			
			strcpy((char*)strIIT.szIssuerLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szPANFormat*/
			strcpy((char*)strIIT.szPANFormat, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskMerchantCopy*/
			strcpy((char*)strIIT.szMaskMerchantCopy, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskCustomerCopy*/
			strcpy((char*)strIIT.szMaskCustomerCopy, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskExpireDate*/
			strcpy((char*)strIIT.szMaskExpireDate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMaskDisplay*/
			strcpy((char*)strIIT.szMaskDisplay, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			strcpy((char*)strIIT.szMaskTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			strcpy((char*)strIIT.szMaskMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fMerchExpDate */
			strIIT.fMerchExpDate = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCustExpDate //5*/
			strIIT.fCustExpDate = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fMerchPANFormat*/
			strIIT.fMerchPANFormat = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	

            /*inCheckHost*/
		    strIIT.inCheckHost = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*ulTransSeqCounter*/
			strIIT.ulTransSeqCounter = sqlite3_column_double(stmt,inStmtSeq +=1);
                        
                        /* fCardVerEnable*/
			strIIT.fCardVerEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        
                        /* fPreAuthEnable*/
			strIIT.fPreAuthEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        
                        /* fMPUCard */
			strIIT.fMPUCard = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        
                        /* fUPICard */
			strIIT.fUPICard = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


			//strcpy((char*)strIIT.szMaskMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


						
        }
	} while (result == SQLITE_ROW);


	vdDebug_LogPrintf("inIITRead strIIT.szMaskTID [%s] ", strIIT.szMaskTID);
	vdDebug_LogPrintf("inIITRead strIIT.szMaskMID [%s]", strIIT.szMaskMID);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inIITSave(int inSeekCnt)
{
	int result;
//	char *sql = "UPDATE IIT SET szIssuerAbbrev = ?, szIssuerLabel = ?, szPANFormat = ?, szMaskMerchantCopy = ?, szMaskCustomerCopy = ?, szMaskExpireDate = ?, szMaskDisplay = ?, fMerchExpDate = ?, fCustExpDate = ?, fMerchPANFormat = ?, ulTransSeqCounter = ? WHERE  IITid = ?";
        char *sql = "UPDATE IIT SET szIssuerAbbrev = ?, szIssuerLabel = ?, szPANFormat = ?, szMaskMerchantCopy = ?, szMaskCustomerCopy = ?, szMaskExpireDate = ?, szMaskDisplay = ?, szMaskTID = ?, szMaskMID = ?, fMerchExpDate = ?, fCustExpDate = ?, fMerchPANFormat = ?, ulTransSeqCounter = ?, fCardVerEnable = ?, fPreAuthEnable = ?, fMPUCard = ?, fUPICard = ? WHERE  IITid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	/* szIssuerAbbrev */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szIssuerAbbrev, strlen((char*)strIIT.szIssuerAbbrev), SQLITE_STATIC);
	/* szIssuerLabel */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szIssuerLabel, strlen((char*)strIIT.szIssuerLabel), SQLITE_STATIC);
	/* szPANFormat*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szPANFormat, strlen((char*)strIIT.szPANFormat), SQLITE_STATIC);
	/* szMaskMerchantCopy*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskMerchantCopy, strlen((char*)strIIT.szMaskMerchantCopy), SQLITE_STATIC);
	/* szMaskCustomerCopy*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskCustomerCopy, strlen((char*)strIIT.szMaskCustomerCopy), SQLITE_STATIC);
	/* szMaskExpireDate*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskExpireDate, strlen((char*)strIIT.szMaskExpireDate), SQLITE_STATIC);
	/* szMaskDisplay*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskDisplay, strlen((char*)strIIT.szMaskDisplay), SQLITE_STATIC);

	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskTID, strlen((char*)strIIT.szMaskTID), SQLITE_STATIC);

	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskMID, strlen((char*)strIIT.szMaskTID), SQLITE_STATIC);
	
	/*fMerchExpDate*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fMerchExpDate);
	/* fCustExpDate */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fCustExpDate);
	/* fMerchPANFormat */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fMerchPANFormat);
	/* ulTransSeqCounter */
    if(strIIT.ulTransSeqCounter >= 0xFFFFFE)
       strIIT.ulTransSeqCounter = 1;
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, strIIT.ulTransSeqCounter);
    vdDebug_LogPrintf(" strIIT.ulTransSeqCounter[%d] result[%d]",strIIT.ulTransSeqCounter, result);
    
        /* fCardVerEnable */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fCardVerEnable);
        
        /* fPreAuthEnable */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fPreAuthEnable);
        
        /* fMPUCard */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fMPUCard);
        
        /* fUPICard */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strIIT.fUPICard);


//	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskMID, strlen((char*)strIIT.szMaskMID), SQLITE_STATIC);

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


int inCSTNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM CST";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}


int inCSTRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inCurrencyIndex, szCurSymbol, szCurCode FROM CST WHERE CSTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);

	//vdDebug_LogPrintf("inCSTRead result  = %d		inSeekCnt = %d", result, inSeekCnt);
	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	//vdDebug_LogPrintf("inCSTRead 2");
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	//vdDebug_LogPrintf("inCSTRead 3 result [%d]", result);

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		//vdDebug_LogPrintf("inCSTRead 4 result [%d]", result);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			//vdDebug_LogPrintf("inCSTRead 5");
			
			inStmtSeq = 0;

			/* inCurrencyIndex */
			strCST.inCurrencyIndex = sqlite3_column_int(stmt,inStmtSeq);
				
			/* szCurSymbol */			
			strcpy((char*)strCST.szCurSymbol, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szCurCode*/
			strcpy((char*)strCST.szCurCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);


	vdDebug_LogPrintf("inCSTRead  inCurrencyIndex [%d]", strCST.inCurrencyIndex);
	vdDebug_LogPrintf("inCSTRead  inCurrencyIndex [%s]", strCST.szCurSymbol);
	vdDebug_LogPrintf("inCSTRead  inCurrencyIndex [%s]", strCST.szCurCode);
	vdDebug_LogPrintf("inCSTRead  inResult [%d]", inResult);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


int inCSTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE CST SET inCurrencyIndex = ?, szCurSymbol = ?, szCurCode = ? WHERE  CSTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	/* inCurrencyIndex */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCST.inCurrencyIndex);
	/* szCurSymbol */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCST.szCurSymbol, strlen((char*)strCST.szCurSymbol), SQLITE_STATIC);
	/* szCurCode */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCST.szCurCode, strlen((char*)strCST.szCurCode), SQLITE_STATIC);

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



int inMMTReadRecord(int inHDTid,int inMITid)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT MMTid, szHostName, HDTid, szMerchantName, MITid, szTID, szMID, szATCMD1, szATCMD2, szATCMD3, szATCMD4, szATCMD5, fMustSettFlag, fEnablePSWD, szPassWord, szBatchNo, szRctHdr1,szRctHdr2, szRctHdr3, szRctHdr4, szRctHdr5, szRctFoot1, szRctFoot2, szRctFoot3, fMMTEnable FROM MMT WHERE HDTid = ? AND MITid = ? AND fMMTEnable = ? ORDER BY MMTid";
	int incount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

//    vdDebug_LogPrintf("inMMTReadRecord inHDTid[%d]inMITid[%d]", inHDTid, inMITid);
    
	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMITid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);
	
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/* MMTid */
			strMMT[incount].MMTid = sqlite3_column_int(stmt, inStmtSeq);
			
			/*szHostName*/
			strcpy((char*)strMMT[incount].szHostName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
            
			/* HDTid */
			strMMT[incount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
				
			/* szMerchantName */			
			strcpy((char*)strMMT[incount].szMerchantName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* MITid */
			strMMT[incount].MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*szTID*/
			strcpy((char*)strMMT[incount].szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMID*/
			strcpy((char*)strMMT[incount].szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
			/*szATCMD1*/
			strcpy((char*)strMMT[incount].szATCMD1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD2*/
			strcpy((char*)strMMT[incount].szATCMD2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD3*/
			strcpy((char*)strMMT[incount].szATCMD3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD4*/
			strcpy((char*)strMMT[incount].szATCMD4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD5*/
			strcpy((char*)strMMT[incount].szATCMD5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
	
            /* fMustSettFlag */
			strMMT[incount].fMustSettFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
            /* fEnablePSWD */
			strMMT[incount].fEnablePSWD = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/*szPassWord*/
			strcpy((char*)strMMT[incount].szPassWord, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szBatchNo*/
			memcpy(strMMT[incount].szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

			/* szRctHdr1 */			
			strcpy((char*)strMMT[incount].szRctHdr1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr2 */			
			strcpy((char*)strMMT[incount].szRctHdr2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr3 */ 		
			strcpy((char*)strMMT[incount].szRctHdr3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr4 */ 		
			strcpy((char*)strMMT[incount].szRctHdr4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctHdr5 */ 		
			strcpy((char*)strMMT[incount].szRctHdr5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1  ));

			/* szRctFoot1 */ 		
			strcpy((char*)strMMT[incount].szRctFoot1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1  ));

			/* szRctFoot2 */		
			strcpy((char*)strMMT[incount].szRctFoot2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szRctFoot3 */		
			strcpy((char*)strMMT[incount].szRctFoot3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
            /* fMMTEnable */
			strMMT[incount].fMMTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	// patrick add code 20141207
	memcpy(&strMMT[0],&strMMT[incount],sizeof(STRUCT_MMT));
	
	srTransRec.MITid = strMMT[incount].MITid;
	strcpy(srTransRec.szTID, strMMT[incount].szTID);
	strcpy(srTransRec.szMID, strMMT[incount].szMID);
	memcpy(srTransRec.szBatchNo, strMMT[incount].szBatchNo, 4);
	strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
    
    return(inResult);
}


int inMMTReadNumofRecords(int inSeekCnt,int *inFindRecordNum)
{
	int result;
	char *sql = "SELECT MMTid, szHostName, HDTid, szMerchantName, MITid, szTID, szMID, szATCMD1, szATCMD2, szATCMD3, szATCMD4, szATCMD5, fMustSettFlag, fEnablePSWD, szPassWord, szBatchNo, szRctHdr1,szRctHdr2, szRctHdr3, szRctHdr4, szRctHdr5, szRctFoot1, szRctFoot2, szRctFoot3, fMMTEnable FROM MMT WHERE HDTid = ? AND fMMTEnable = ? ORDER BY MMTid";
	int incount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inMMTReadNumofRecords=[%d]",result);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* MMTid */
			strMMT[incount].MMTid = sqlite3_column_int(stmt, inStmtSeq);

			/*szHostName*/
			strcpy((char*)strMMT[incount].szHostName, (char *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
            
			/* HDTid */
			strMMT[incount].HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
				
			/* szMerchantName */			
			strcpy((char*)strMMT[incount].szMerchantName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* MITid */
			strMMT[incount].MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );

			/*szTID*/
			strcpy((char*)strMMT[incount].szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMID*/
			strcpy((char*)strMMT[incount].szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szATCMD1*/
			strcpy((char*)strMMT[incount].szATCMD1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD2*/
			strcpy((char*)strMMT[incount].szATCMD2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD3*/
			strcpy((char*)strMMT[incount].szATCMD3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD4*/
			strcpy((char*)strMMT[incount].szATCMD4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szATCMD5*/
			strcpy((char*)strMMT[incount].szATCMD5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
	
            /* fMustSettFlag */
			strMMT[incount].fMustSettFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
            /* fEnablePSWD */
			strMMT[incount].fEnablePSWD = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/*szPassWord*/
			strcpy((char*)strMMT[incount].szPassWord, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

            /*szBatchNo*/
            memcpy(strMMT[incount].szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

			/*szRctHdr1*/
			strcpy((char*)strMMT[incount].szRctHdr1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			
			/*szRctHdr2*/
			strcpy((char*)strMMT[incount].szRctHdr2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctHdr3*/
			strcpy((char*)strMMT[incount].szRctHdr3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctHdr4*/
			strcpy((char*)strMMT[incount].szRctHdr4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctHdr5*/
			strcpy((char*)strMMT[incount].szRctHdr5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctFoot1*/
			strcpy((char*)strMMT[incount].szRctFoot1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctFoot2*/
			strcpy((char*)strMMT[incount].szRctFoot2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szRctFoot3*/
			strcpy((char*)strMMT[incount].szRctFoot3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			
            /* fMMTEnable */
			strMMT[incount].fMMTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			incount ++;

		}
	} while (result == SQLITE_ROW);

	   *inFindRecordNum = incount;

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    
    return(d_OK);
}

int inMMTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE MMT SET MMTid = ? , szHostName = ? ,HDTid = ? ,szMerchantName = ? ,MITid = ? ,szTID = ? ,szMID = ? , szATCMD1 = ? , szATCMD2 = ? , szATCMD3 = ? , szATCMD4 = ? , szATCMD5 = ?, fMustSettFlag = ?, fEnablePSWD = ?, szPassWord = ?, szBatchNo = ?, szRctHdr1 = ?, szRctHdr2 = ?, szRctHdr3 = ?, szRctHdr4 = ?, szRctHdr5 = ?, szRctFoot1 =?, szRctFoot2 =?, szRctFoot3 =?, fMMTEnable =? WHERE  MMTid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
    /* MMMTid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MMTid);
    /* szHostName */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szHostName, strlen((char*)strMMT[0].szHostName), SQLITE_STATIC);
	/* HDTid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].HDTid);
	/* szMerchantName */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szMerchantName, strlen((char*)strMMT[0].szMerchantName), SQLITE_STATIC);
    /* MITid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MITid);
    /* szTID*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szTID, strlen((char*)strMMT[0].szTID), SQLITE_STATIC);
	/* szMID*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szMID, strlen((char*)strMMT[0].szMID), SQLITE_STATIC);
	/* szATCMD1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD1, strlen((char*)strMMT[0].szATCMD1), SQLITE_STATIC);
	/* szATCMD2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD2, strlen((char*)strMMT[0].szATCMD2), SQLITE_STATIC);
	/* szATCMD3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD3, strlen((char*)strMMT[0].szATCMD3), SQLITE_STATIC);
	/* szATCMD4 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD4, strlen((char*)strMMT[0].szATCMD4), SQLITE_STATIC);
	/* szATCMD5 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szATCMD5, strlen((char*)strMMT[0].szATCMD5), SQLITE_STATIC);   
    /* fMustSettFlag */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].fMustSettFlag);
	
    /* fEnablePSWD */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].fEnablePSWD);
	/* szPassWord */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szPassWord, strlen((char*)strMMT[0].szPassWord), SQLITE_STATIC);   

    /* szBatchNo*/
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strMMT[0].szBatchNo, 3, SQLITE_STATIC);

	/* szRctHdr1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr1, strlen((char*)strMMT[0].szRctHdr1), SQLITE_STATIC);   

	/* szRctHdr2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr2, strlen((char*)strMMT[0].szRctHdr2), SQLITE_STATIC);   

	/* szRctHdr3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr3, strlen((char*)strMMT[0].szRctHdr3), SQLITE_STATIC);   

	/* szRctHdr4 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr4, strlen((char*)strMMT[0].szRctHdr4), SQLITE_STATIC);   

	/* szRctHdr5 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctHdr5, strlen((char*)strMMT[0].szRctHdr5), SQLITE_STATIC);   

	/* szRctFoot1 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctFoot1, strlen((char*)strMMT[0].szRctFoot1), SQLITE_STATIC);   

	/* szRctFoot2 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctFoot2, strlen((char*)strMMT[0].szRctFoot2), SQLITE_STATIC);   

	/* szRctFoot3 */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMMT[0].szRctFoot3, strlen((char*)strMMT[0].szRctFoot3), SQLITE_STATIC);
	/* fMMTEnable */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].fMMTEnable);

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

int inPITRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT szTransactionTypeName, inTxnTypeID, fTxnEnable, inPasswordLevel FROM PIT WHERE inTxnTypeID = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/*szTransactionTypeName*/
			strcpy((char*)strPIT.szTransactionTypeName, (char *)sqlite3_column_text(stmt,inStmtSeq));
            
			/* inTxnTypeID */
			strPIT.inTxnTypeID = sqlite3_column_int(stmt,inStmtSeq +=1 );
		    
            /* fTxnEnable */
			strPIT.fTxnEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* inPasswordLevel */
			strPIT.inPasswordLevel = sqlite3_column_int(stmt,inStmtSeq +=1 );
		}
	} while (result == SQLITE_ROW);
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	   
    return(inResult);
}

int inPITSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE PIT SET szTransactionTypeName = ? ,inTxnTypeID = ? ,fTxnEnable = ? ,inPasswordLevel = ? WHERE  PITid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
    /* szTransactionTypeName */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strPIT.szTransactionTypeName, strlen((char*)strPIT.szTransactionTypeName), SQLITE_STATIC);
	/* inTxnTypeID */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPIT.inTxnTypeID);
	/* fTxnEnable*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPIT.fTxnEnable);
    /* inPasswordLevel */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strPIT.inPasswordLevel);

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

int inEMVRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inSchemeReference, inIssuerReference, inTRMDataPresent, lnEMVFloorLimit, lnEMVRSThreshold, inEMVTargetRSPercent, inEMVMaxTargetRSPercent, inMerchantForcedOnlineFlag, inBlackListedCardSupportFlag, szEMVTACDefault, szEMVTACDenial, szEMVTACOnline, szDefaultTDOL, szDefaultDDOL, inEMVFallbackAllowed, inNextRecord, ulEMVCounter, inEMVAutoSelectAppln, szEMVTermCountryCode, szEMVTermCurrencyCode, inEMVTermCurExp, szEMVTermCapabilities, szEMVTermAddCapabilities, szEMVTermType, szEMVMerchantCategoryCode, szEMVTerminalCategoryCode, inModifyCandListFlag, shRFU1, shRFU2, shRFU3, szRFU1, szRFU2, szRFU3 FROM EMV WHERE EMVid = ?";

	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
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

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		//Debug_LogPrintf("EMV read[%d]",result);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inSchemeReference;*/
			strEMVT.inSchemeReference = sqlite3_column_int(stmt,inStmtSeq);
    
            /*inIssuerReference*/
			strEMVT.inIssuerReference = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inTRMDataPresent*/
			strEMVT.inTRMDataPresent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                            
            /*lnEMVFloorLimit*/
			strEMVT.lnEMVFloorLimit = sqlite3_column_double(stmt,inStmtSeq +=1 );
                
            /*lnEMVRSThreshold*/            //5
			strEMVT.lnEMVRSThreshold = sqlite3_column_double(stmt,inStmtSeq +=1 );
                
            /*inEMVTargetRSPercent*/
			strEMVT.inEMVTargetRSPercent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inEMVMaxTargetRSPercent*/
			strEMVT.inEMVMaxTargetRSPercent = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inMerchantForcedOnlineFlag*/
			strEMVT.inMerchantForcedOnlineFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*inBlackListedCardSupportFlag*/
			strEMVT.inBlackListedCardSupportFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
            
            /* szEMVTACDefault */            //10
			memcpy(strEMVT.szEMVTACDefault, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
           
            /* szEMVTACDenial */            
			memcpy(strEMVT.szEMVTACDenial, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
            
            /* szEMVTACOnline */
			memcpy(strEMVT.szEMVTACOnline, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);
            
            /* szDefaultTDOL */
			strcpy((char*)strEMVT.szDefaultTDOL, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szDefaultDDOL */
			strcpy((char*)strEMVT.szDefaultDDOL, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*inEMVFallbackAllowed*/        //15
			strEMVT.inEMVFallbackAllowed = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /*inNextRecord*/
			strEMVT.inNextRecord = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /*ulEMVCounter*/
			strEMVT.ulEMVCounter = sqlite3_column_double(stmt, inStmtSeq +=1 );

            /*inEMVAutoSelectAppln*/
			strEMVT.inEMVAutoSelectAppln = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szEMVTermCountryCode */
			memcpy(strEMVT.szEMVTermCountryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			
            /* szEMVTermCurrencyCode */        //20
			memcpy(strEMVT.szEMVTermCurrencyCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            
            /*inEMVTermCurExp*/
			strEMVT.inEMVTermCurExp = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szEMVTermCapabilities*/
			memcpy(strEMVT.szEMVTermCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
               
            /* szEMVTermAddCapabilities */
			memcpy(strEMVT.szEMVTermAddCapabilities, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);
    
            /* szEMVTermType */
			memcpy(strEMVT.szEMVTermType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            
            /* szEMVMerchantCategoryCode */        //25
			memcpy(strEMVT.szEMVMerchantCategoryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);

            /* szEMVTerminalCategoryCode */
			memcpy(strEMVT.szEMVTerminalCategoryCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

            /*inModifyCandListFlag*/
			strEMVT.inModifyCandListFlag = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU1*/
			strEMVT.shRFU1 = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU2*/
			strEMVT.shRFU2 = sqlite3_column_int(stmt,inStmtSeq +=1 );
                
            /*shRFU3*/                            //30
			strEMVT.shRFU3 = sqlite3_column_int(stmt,inStmtSeq +=1 );
    
            /* szRFU1*/
			strcpy((char*)strEMVT.szRFU1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
    
            /* szRFU2 */
			strcpy((char*)strEMVT.szRFU2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
            /* szRFU3 */
			strcpy((char*)strEMVT.szRFU3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inEMVUpdateCurrency(void) {
    int result;
    char *sql = "UPDATE EMV SET szEMVTermCountryCode = ? ,szEMVTermCurrencyCode = ?";

    /* open the database */
    result = sqlite3_open(DB_EMV, &db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    sqlite3_exec(db, "begin", 0, 0, NULL);
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

    inStmtSeq = 0;

    /*szEMVTermCountryCode*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strEMVT.szEMVTermCountryCode, 2, SQLITE_STATIC);

    /*szEMVTermCurrencyCode*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strEMVT.szEMVTermCurrencyCode, 2, SQLITE_STATIC);

//    result = sqlite3_bind_int(stmt, inStmtSeq += 1, inSeekCnt);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        sqlite3_close(db);
        return 3;
    }

    sqlite3_exec(db, "commit;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (d_OK);
}

int inTCTRead(int inSeekCnt) {
    int result;
    int len = 0;
    int inResult = -1;
    // 	char *sql = "SELECT inMainLine, fHandsetPresent, fDemo, fCVVEnable, szSuperPW, szSystemPW, szEngineerPW, szFunKeyPW, szPMpassword, byRS232DebugPort, byTerminalType, byPinPadType, byPinPadPort,inPrinterHeatLevel, inWaitTime, inThemesType, byCtlsMode, byERMMode, fTipAllowFlag, fSignatureFlag, fDebitFlag, szFallbackTime, inFallbackTimeGap, fManualEntryInv, szInvoiceNo, szPabx, szLastInvoiceNo, fECR, fDebugOpenedFlag,fShareComEnable, inReversalCount, inTipAdjustCount, fPrintISOMessage, fNSR, fNSRCustCopy, szNSRLimit, szCTLSLimit, usTMSGap, inTMSComMode, usTMSRemotePort, szTMSRemoteIP, szTMSRemotePhone, szTMSRemoteID, szTMSRemotePW, fFirstInit, byPinPadMode, byRS232ECRPort, inPPBaudRate, inFontFNTMode, fCancelSignFlag, inERMMaximum, byERMInit, byQP3000SPort , byExtReadCard FROM TCT WHERE TCTid = ?";
    char *sql = "SELECT inMainLine, fHandsetPresent, fDemo, fCVVEnable, szSuperPW, szSystemPW, szEngineerPW, szFunKeyPW, szPMpassword, byRS232DebugPort, "
            "byTerminalType, byPinPadType, byPinPadPort,inPrinterHeatLevel, inWaitTime, inThemesType, byCtlsMode, byERMMode, fTipAllowFlag, fSignatureFlag, "
            "fDebitFlag, szFallbackTime, inFallbackTimeGap, fManualEntryInv, szInvoiceNo, szPabx, szLastInvoiceNo, fECR, fDebugOpenedFlag,fShareComEnable, "
            "inReversalCount, inTipAdjustCount, fPrintISOMessage, fQPS, fQPSCustCopy, szQPSUSDLimit, szQPSLimit, fVEPS, fVEPSCustCopy, szVEPSUSDLimit, szVEPSLimit, fNSR, fNSRCustCopy, szNSRLimit, szCTLSLimit, usTMSGap, inTMSComMode, usTMSRemotePort, "
            "szTMSRemoteIP, szTMSRemotePhone, szTMSRemoteID, szTMSRemotePW, fFirstInit, byPinPadMode, byRS232ECRPort, inPPBaudRate, inFontFNTMode, "
            "fCancelSignFlag, inERMMaximum, byERMInit, byQP3000SPort , byExtReadCard, chNextSettleTime, fMustAutoSettle, szAutoSettleIntervalHour, "
            "inExchangeRate, inECRPrintMode, inDupReceipt, fEnableAmountIdle, szMerchantName, szMerchantAddress, szApplicationPW, fEnableIPPMenu, "
            "fEnableAlipayMenu, inRcptVia, fEnablePreAuthMenu, fEnableUPISignONMenu, fUploadReceiptIdle, inIdleTimeout, fSendTCBeforeSettle, usTMSRemotePort2, szTMSRemoteIP2, fUploadReceiptonSettle, fEnableDiscountMenu,  fDisplayIDLEOption, fOKDollarPreConnect FROM TCT WHERE TCTid = ?";

    /* open the database */
    result = sqlite3_open(DB_TERMINAL, &db);
    //vdDebug_LogPrintf("sqlite3_open =[%d],",result);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    vdSetJournalModeOff();

    sqlite3_exec(db, "begin", 0, 0, NULL);
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    //vdDebug_LogPrintf("sqlite3_prepare_v2 =[%d],",result);
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


            /* inMainLine */
            strTCT.inMainLine = sqlite3_column_int(stmt, inStmtSeq);

            /* fHandsetPresent */
            strTCT.fHandsetPresent = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* fDemo //5*/
            strTCT.fDemo = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* fCVVEnable */
            strTCT.fCVVEnable = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szSuperPW*/
            strcpy((char*) strTCT.szSuperPW, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szSystemPW */
            strcpy((char*) strTCT.szSystemPW, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szEngineerPW */
            strcpy((char*) strTCT.szEngineerPW, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* szFunKeyPW */
            strcpy((char*) strTCT.szFunKeyPW, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* szPMpassword */
            strcpy((char*) strTCT.szPMpassword, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /*byRS232DebugPort*/
            strTCT.byRS232DebugPort = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*byTerminalType*/
            strTCT.byTerminalType = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*byPinPadType*/
            strTCT.byPinPadType = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*byPinPadPort*/
            strTCT.byPinPadPort = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*inPrinterHeatLevel*/
            strTCT.inPrinterHeatLevel = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*inWaitTime*/
            strTCT.inWaitTime = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*inThemesType*/
            strTCT.inThemesType = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*byCtlsMode*/
            strTCT.byCtlsMode = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*byERMMode*/
            strTCT.byERMMode = sqlite3_column_int(stmt, inStmtSeq += 1);

            /* fTipAllowFlag */
            strTCT.fTipAllowFlag = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* fSignatureFlag */
            strTCT.fSignatureFlag = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* fDebitFlag */
            strTCT.fDebitFlag = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szFallbackTime*/
            strcpy((char*) strTCT.szFallbackTime, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* inFallbackTimeGap */
            strTCT.inFallbackTimeGap = sqlite3_column_int(stmt, inStmtSeq += 1);

            /* fManualEntryInv */
            strTCT.fManualEntryInv = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szInvoiceNo*/
            memcpy(strTCT.szInvoiceNo, sqlite3_column_blob(stmt, inStmtSeq += 1), 3);

            /* szPabx */
            strcpy((char*) strTCT.szPabx, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            DelCharInStr(strTCT.szPabx, ',', 2);
            len = strlen((char*) strTCT.szPabx);
            if (len > 0)
                strTCT.szPabx[len] = ',';

            /* szLastInvoiceNo */
            memcpy(strTCT.szLastInvoiceNo, sqlite3_column_blob(stmt, inStmtSeq += 1), 3);

            /* fECR */
            strTCT.fECR = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* fDebugOpenedFlag */
            strTCT.fDebugOpenedFlag = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* fShareComEnable */
            strTCT.fShareComEnable = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* inReversalCount */
            strTCT.inReversalCount = sqlite3_column_int(stmt, inStmtSeq += 1);

            /* inTipAdjustCount */
            strTCT.inTipAdjustCount = sqlite3_column_int(stmt, inStmtSeq += 1);

            /*fPrintISOMessage*/
            strTCT.fPrintISOMessage = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
			
            /*fQPS*/
            strTCT.fQPS = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /*fQPSCustCopy*/
            strTCT.fQPSCustCopy = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szQPSUSDLimit*/
            strcpy((char*) strTCT.szQPSUSDLimit, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szQPSLimit */
	    	strcpy((char*) strTCT.szQPSLimit, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /*fVEPS*/
            strTCT.fVEPS = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /*fVEPSCustCopy*/
            strTCT.fVEPSCustCopy = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szVEPSUSDLimit */
            strcpy((char*) strTCT.szVEPSUSDLimit, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szVEPSLimit*/
	    	strcpy((char*) strTCT.szVEPSLimit, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

			/*fNSR*/
            strTCT.fNSR = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /*fNSRCustCopy*/
            strTCT.fNSRCustCopy = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* szNSRLimit*/
            strcpy((char*) strTCT.szNSRLimit, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* szCTLSLimit*/
            strcpy((char*) strTCT.szCTLSLimit, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));

            /* usTMSGap */
            strTCT.usTMSGap = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* inTMSComMode */
            strTCT.inTMSComMode = sqlite3_column_int(stmt, inStmtSeq += 1);

            /* usTMSRemotePort */
            strTCT.usTMSRemotePort = sqlite3_column_int(stmt, inStmtSeq += 1);

            /* szTMSRemoteIP */
            memcpy(strTCT.szTMSRemoteIP, sqlite3_column_blob(stmt, inStmtSeq += 1), 30);

            /* szTMSRemotePhone */
            memcpy(strTCT.szTMSRemotePhone, sqlite3_column_blob(stmt, inStmtSeq += 1), 30);
            /* szTMSRemoteID */
            memcpy(strTCT.szTMSRemoteID, sqlite3_column_blob(stmt, inStmtSeq += 1), 32);
            /* szTMSRemotePW */
            memcpy(strTCT.szTMSRemotePW, sqlite3_column_blob(stmt, inStmtSeq += 1), 32);

            /* fFirstInit */
            strTCT.fFirstInit = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* byPinPadMode */
            strTCT.byPinPadMode = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* byRS232ECRPort */
            strTCT.byRS232ECRPort = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* inPPBaudRate */
            strTCT.inPPBaudRate = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* inFontFNTMode */
            strTCT.inFontFNTMode = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* fCancelSignFlag */
            strTCT.fCancelSignFlag = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* inERMMaximum */
            strTCT.inERMMaximum = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* byERMInit */
            strTCT.byERMInit = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*byQP3000SPort*/
            strTCT.byQP3000SPort = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*byExtReadCard*/
            strTCT.byExtReadCard = sqlite3_column_int(stmt, inStmtSeq += 1);

            /*chNextSettleTime*/
            strcpy((char*) strTCT.chNextSettleTime, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /*fMustAutoSettle*/
            strTCT.fMustAutoSettle = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* szAutoSettleIntervalHour */
            //			strTCT.inAutoSettleIntervalHour = sqlite3_column_int(stmt,inStmtSeq +=1 );
            strcpy((char*) strTCT.szAutoSettleIntervalHour, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* inExchangeRate */
            strTCT.inExchangeRate = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* inECRPrintMode */
            strTCT.inECRPrintMode = sqlite3_column_int(stmt, inStmtSeq += 1);
            /* inDupReceipt */
            strTCT.inDupReceipt = sqlite3_column_int(stmt, inStmtSeq += 1);
            /*fEnableAmountIdle*/
            strTCT.fEnableAmountIdle = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* szMerchantName */
            strcpy((char*) strTCT.szMerchantName, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* szMerchantAddress */
            strcpy((char*) strTCT.szMerchantAddress, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* szApplicationPW */
            strcpy((char*) strTCT.szApplicationPW, (char *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* fEnableIPPMenu */
            strTCT.fEnableIPPMenu = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /* fEnableAlipayMenu */
            strTCT.fEnableAlipayMenu = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
            /*inRcptVia*/
            strTCT.inRcptVia = sqlite3_column_int(stmt, inStmtSeq += 1);
			/* fEnablePreAuthMenu */
            strTCT.fEnablePreAuthMenu = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));	
			/* fEnableUPISignONMenu */
            strTCT.fEnableUPISignONMenu = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

            strTCT.fUploadReceiptIdle = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));
			strTCT.inIdleTimeout = sqlite3_column_int(stmt, inStmtSeq += 1);

            /*fSendTCBeforeSettle*/
			strTCT.fSendTCBeforeSettle = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

			/*for SIM2 - usTMSRemotePort2 */
            strTCT.usTMSRemotePort2 = sqlite3_column_int(stmt, inStmtSeq += 1);

            /*for SIM2 - szTMSRemoteIP */
            memcpy(strTCT.szTMSRemoteIP2, sqlite3_column_blob(stmt, inStmtSeq += 1), 30);

			strTCT.fUploadReceiptonSettle = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));

		/* fEnableDiscountMenu */
		strTCT.fEnableDiscountMenu = fGetBoolean((BYTE *) sqlite3_column_text(stmt, inStmtSeq += 1));						

		/* fEnableDiscountMenu */
		strTCT.fDisplayIDLEOption = sqlite3_column_int(stmt, inStmtSeq += 1);

		/*fOKDollarPreConnect*/
		strTCT.fOKDollarPreConnect = sqlite3_column_int(stmt, inStmtSeq += 1);
		
				
        }
    } while (result == SQLITE_ROW);

    sqlite3_exec(db, "commit;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (inResult);
}

int inTCTSave(int inSeekCnt) {
    int result;
    //	char *sql = "UPDATE TCT SET inMainLine = ?, fHandsetPresent = ?, fDemo = ?, fCVVEnable = ?, szSuperPW = ?, szSystemPW = ?, szEngineerPW = ?, szFunKeyPW = ?, szPMpassword = ?, byRS232DebugPort = ?, byTerminalType = ?, byPinPadType = ?, byPinPadPort = ?, inPrinterHeatLevel = ?, inWaitTime = ?, inThemesType = ?, byCtlsMode = ?, byERMMode = ?, fTipAllowFlag = ?, fSignatureFlag = ?, fDebitFlag = ?, inFallbackTimeGap = ?, fManualEntryInv = ?, szInvoiceNo = ?, szPabx = ?, szLastInvoiceNo = ?, fDebugOpenedFlag = ?, fShareComEnable = ?, inReversalCount = ?, inTipAdjustCount = ?, fPrintISOMessage = ?, fNSR = ?, fNSRCustCopy = ?, szNSRLimit = ?, szCTLSLimit = ?, usTMSGap = ?, inTMSComMode = ?, usTMSRemotePort = ?, szTMSRemoteIP = ?, szTMSRemotePhone = ?, szTMSRemoteID = ?, szTMSRemotePW = ?, fFirstInit = ?, byPinPadMode = ?, byRS232ECRPort = ?, inPPBaudRate = ?, inFontFNTMode = ?, fCancelSignFlag = ?, inERMMaximum = ?, byERMInit = ? , byExtReadCard = ? WHERE  TCTid = ?";
    char *sql = "UPDATE TCT SET inMainLine = ?, fHandsetPresent = ?, fDemo = ?, fCVVEnable = ?, szSuperPW = ?, szSystemPW = ?, szEngineerPW = ?, "
                "szFunKeyPW = ?, szPMpassword = ?, byRS232DebugPort = ?, byTerminalType = ?, byPinPadType = ?, byPinPadPort = ?, inPrinterHeatLevel = ?, "
                "inWaitTime = ?, inThemesType = ?, byCtlsMode = ?, byERMMode = ?, fTipAllowFlag = ?, fSignatureFlag = ?, fDebitFlag = ?, "
                "inFallbackTimeGap = ?, fManualEntryInv = ?, szInvoiceNo = ?, szPabx = ?, szLastInvoiceNo = ?, fDebugOpenedFlag = ?, fShareComEnable = ?, "
                "inReversalCount = ?, inTipAdjustCount = ?, fPrintISOMessage = ?, fQPS = ?, fQPSCustCopy = ?, szQPSUSDLimit = ?, szQPSLimit = ?, fVEPS = ?, fVEPSCustCopy = ?, szVEPSUSDLimit = ?, szVEPSLimit = ?, fNSR = ?, fNSRCustCopy = ?, szNSRLimit = ?, szCTLSLimit = ?, usTMSGap = ?, "
                "inTMSComMode = ?, usTMSRemotePort = ?, szTMSRemoteIP = ?, szTMSRemotePhone = ?, szTMSRemoteID = ?, szTMSRemotePW = ?, fFirstInit = ?, "
                "byPinPadMode = ?, byRS232ECRPort = ?, inPPBaudRate = ?, inFontFNTMode = ?, fCancelSignFlag = ?, inERMMaximum = ?, byERMInit = ? , "
                "byExtReadCard = ?, chNextSettleTime = ?, fMustAutoSettle = ?, szAutoSettleIntervalHour = ?, inExchangeRate = ?, inDupReceipt = ?, "
                "fEnableAmountIdle = ?, szMerchantName = ?, szMerchantAddress = ?, szApplicationPW = ?, fEnableIPPMenu = ?, fEnableAlipayMenu = ?, inRcptVia = ?, fEnablePreAuthMenu = ?, fEnableUPISignONMenu = ?, fEnableDiscountMenu = ?, fDisplayIDLEOption = ?, fOKDollarPreConnect = ? WHERE  TCTid = ?";

    vdDebug_LogPrintf("[inTCTSave]-start ");
    /* open the database */
    result = sqlite3_open(DB_TERMINAL, &db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    vdSetJournalModeOff();

    vdDebug_LogPrintf("[inTCTSave]-fDemo[%d]", strTCT.fDemo);
    sqlite3_exec(db, "begin", 0, 0, NULL);
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    vdDebug_LogPrintf("[inTCTSave]-sqlite3_prepare_v2[%d]", result);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

    inStmtSeq = 0;

    /* inMainLine */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inMainLine);
    
    /* fHandsetPresent */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fHandsetPresent);
    
    /* fDemo */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fDemo);
    
    /* fCVVEnable */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fCVVEnable);
    
    /* szSuperPW */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szSuperPW, strlen((char*) strTCT.szSuperPW), SQLITE_STATIC);
    
    /* szSystemPW */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szSystemPW, strlen((char*) strTCT.szSystemPW), SQLITE_STATIC);
    
    /* szEngineerPW */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szEngineerPW, strlen((char*) strTCT.szEngineerPW), SQLITE_STATIC);

    /* szFunKeyPW */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szFunKeyPW, strlen((char*) strTCT.szFunKeyPW), SQLITE_STATIC);
    
    /* szPMpassword */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szPMpassword, strlen((char*) strTCT.szPMpassword), SQLITE_STATIC);
    
    /* byRS232DebugPort */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byRS232DebugPort);
    
    /* byTerminalType */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byTerminalType);
    
    /* byPinPadType */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byPinPadType);
    
    /* byPinPadPort */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byPinPadPort);
    
    /* inPrinterHeatLevel */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inPrinterHeatLevel);
    
    /* inWaitTime */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inWaitTime);
    
    /* inThemesType */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inThemesType);
    
    /* byCtlsMode */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byCtlsMode);
    
    /* byERMMode */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byERMMode);

    /* fTipAllowFlag */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fTipAllowFlag);
    
    /* fSignatureFlag */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fSignatureFlag);

    /* fDebitFlag */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fDebitFlag);

    /*inFallbackTimeGap*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inFallbackTimeGap);

    /* fManualEntryInv */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fManualEntryInv);

    /* szInvoiceNo*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strTCT.szInvoiceNo, 3, SQLITE_STATIC);

    /* szPabx*/
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szPabx, strlen((char*) strTCT.szPabx), SQLITE_STATIC);
    
    /* szLastInvoiceNo*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strTCT.szLastInvoiceNo, 3, SQLITE_STATIC);

    /* fDebugOpenedFlag*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fDebugOpenedFlag);

    /* fShareComEnable*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fShareComEnable);

    /*inReversalCount*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inReversalCount);

    /*inTipAdjustCount*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inTipAdjustCount);

    /*fPrintISOMessage*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fPrintISOMessage);
    
    /*fQPS*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fQPS);
    
    /*fQPSCustCopy*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fQPSCustCopy);
    
    /* szQPSUSDLimit*/
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szQPSUSDLimit, strlen((char*) strTCT.szQPSUSDLimit), SQLITE_STATIC);

   /* szQPSLimit */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szQPSLimit, strlen((char*) strTCT.szQPSLimit), SQLITE_STATIC);

    /*fVEPS*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fVEPS);
    
   /*fVEPSCustCopy*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fVEPSCustCopy);
    
  /* szVEPSUSDLimit */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szVEPSUSDLimit, strlen((char*) strTCT.szVEPSUSDLimit), SQLITE_STATIC);

   /* szVEPSLimit*/
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szVEPSLimit, strlen((char*) strTCT.szVEPSLimit), SQLITE_STATIC);
   
	/*fNSR*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fNSR);
    
    /*fNSRCustCopy*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fNSRCustCopy);
    
    /* szNSRLimit */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szNSRLimit, strlen((char*) strTCT.szNSRLimit), SQLITE_STATIC);
    
    /* szCTLSLimit */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szCTLSLimit, strlen((char*) strTCT.szCTLSLimit), SQLITE_STATIC);

    /*usTMSGap*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.usTMSGap);
    
    /*inTMSComMode*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inTMSComMode);

    /*usTMSRemotePort*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.usTMSRemotePort);

    /* szTMSRemoteIP*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strTCT.szTMSRemoteIP, 30, SQLITE_STATIC);

    /* szTMSRemotePhone*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strTCT.szTMSRemotePhone, 30, SQLITE_STATIC);
    
    /* szTMSRemoteID*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strTCT.szTMSRemoteID, 32, SQLITE_STATIC);
    
    /* szTMSRemotePW*/
    result = sqlite3_bind_blob(stmt, inStmtSeq += 1, strTCT.szTMSRemotePW, 32, SQLITE_STATIC);

    /* fFirstInit */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fFirstInit);
    
    /*byPinPadMode*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byPinPadMode);
    
    /*byRS232ECRPort*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byRS232ECRPort);
    
    /*inPPBaudRate*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inPPBaudRate);
    
    /*inFontFNTMode*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inFontFNTMode);
    
    /* fCancelSignFlag */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fCancelSignFlag);
    
    /*inERMMaximum*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inERMMaximum);
    
    /*byERMInit*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byERMInit);
    
    /*byExtReadCard*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.byExtReadCard);
    
    /*chNextSettleTime*/
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.chNextSettleTime, strlen((char*) strTCT.chNextSettleTime), SQLITE_STATIC);
    
    /*fMustAutoSettle*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fMustAutoSettle);
    
    /*szAutoSettleIntervalHour*/
    //        result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCT.inAutoSettleIntervalHour);
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szAutoSettleIntervalHour, strlen((char*) strTCT.szAutoSettleIntervalHour), SQLITE_STATIC);
    
    /*inExchangeRate*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inExchangeRate);
    
    /*inDupReceipt*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inDupReceipt);
    
    /*fEnableAmountIdle*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fEnableAmountIdle);
    
    /* szMerchantName */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szMerchantName, strlen((char*) strTCT.szMerchantName), SQLITE_STATIC);
    
    /* szMerchantAddress */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szMerchantAddress, strlen((char*) strTCT.szMerchantAddress), SQLITE_STATIC);
    
    /* szApplicationPW */
    result = sqlite3_bind_text(stmt, inStmtSeq += 1, (char*) strTCT.szApplicationPW, strlen((char*) strTCT.szApplicationPW), SQLITE_STATIC);
    
    /* fEnableIPPMenu*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fEnableIPPMenu);
    
    /* fEnableAlipayMenu*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fEnableAlipayMenu);

	/*inRcptVia*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.inRcptVia);
	
	/* fEnablePreAuthMenu*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fEnablePreAuthMenu);
	
	/* fEnableUPISignONMenu*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fEnableUPISignONMenu);

    /* fEnableDiscountMenu*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fEnableDiscountMenu);


    /* fDisplayIDLEOption*/
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fDisplayIDLEOption);

    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCT.fOKDollarPreConnect);	
	
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, inSeekCnt);
    result = sqlite3_step(stmt);
    vdDebug_LogPrintf("sqlite3_step =[%d]", result);
    if (result != SQLITE_DONE) {
        sqlite3_close(db);
        return 3;
    }

    sqlite3_exec(db, "commit;", NULL, NULL, NULL);

    result = sqlite3_finalize(stmt);
    vdDebug_LogPrintf("sqlite3_finalize =[%d]", result);
    sqlite3_close(db);
    vdDebug_LogPrintf("[inTCTSave]-end ");

    return (d_OK);
}

int inTCPRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
//	char *sql = "SELECT szTerminalIP, szGetWay, szSubNetMask, szHostDNS1, szHostDNS2, fDHCPEnable, szAPN, szUserName, szPassword FROM TCP WHERE TCPid = ?";
	char *sql = "SELECT szTerminalIP, szGetWay, szSubNetMask, szHostDNS1, szHostDNS2, fDHCPEnable, szAPN, szUserName, szPassword, inSIMSlot FROM TCP WHERE TCPid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/*szTerminalIP*/
			strcpy((char*)strTCP.szTerminalIP, (char *)sqlite3_column_text(stmt,inStmtSeq));
            				
			/* szGetWay */			
			strcpy((char*)strTCP.szGetWay, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szSubNetMask*/
			strcpy((char*)strTCP.szSubNetMask, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szHostDNS1*/
			strcpy((char*)strTCP.szHostDNS1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szHostDNS2*/
			strcpy((char*)strTCP.szHostDNS2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* fDHCPEnable */
			strTCP.fDHCPEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szAPN*/
			strcpy((char*)strTCP.szAPN, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szUserName*/
			strcpy((char*)strTCP.szUserName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szPassword*/
			strcpy((char*)strTCP.szPassword, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inSIMSlot */
            strTCP.inSIMSlot= sqlite3_column_int(stmt, inStmtSeq += 1);

		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inTCPSave(int inSeekCnt)
{
	int result;
//	char *sql = "UPDATE TCP SET szTerminalIP = ? ,szGetWay = ? ,szSubNetMask = ? ,szHostDNS1 = ? ,szHostDNS2 = ? ,fDHCPEnable = ? ,szAPN = ? ,szUserName = ? ,szPassword = ? WHERE  TCPid = ?";
	char *sql = "UPDATE TCP SET szTerminalIP = ? ,szGetWay = ? ,szSubNetMask = ? ,szHostDNS1 = ? ,szHostDNS2 = ? ,fDHCPEnable = ? ,szAPN = ? ,szUserName = ? ,szPassword = ?, inSIMSlot = ? WHERE  TCPid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
    /* szTerminalIP */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szTerminalIP, strlen((char*)strTCP.szTerminalIP), SQLITE_STATIC);
	/* szGetWay */
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szGetWay, strlen((char*)strTCP.szGetWay), SQLITE_STATIC);
    /* szSubNetMask*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szSubNetMask, strlen((char*)strTCP.szSubNetMask), SQLITE_STATIC);
	/* szHostDNS1*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szHostDNS1, strlen((char*)strTCP.szHostDNS1), SQLITE_STATIC);
	/* szHostDNS2*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szHostDNS2, strlen((char*)strTCP.szHostDNS2), SQLITE_STATIC);
    /* fDHCPEnable*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strTCP.fDHCPEnable);
    /* szAPN*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szAPN, strlen((char*)strTCP.szAPN), SQLITE_STATIC);
    /* szUserName*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szUserName, strlen((char*)strTCP.szUserName), SQLITE_STATIC);
    /* szPassword*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strTCP.szPassword, strlen((char*)strTCP.szPassword), SQLITE_STATIC);
	/* inSIMSlot */
    result = sqlite3_bind_int(stmt, inStmtSeq += 1, strTCP.inSIMSlot);

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



int inDatabase_BatchDeleteHDTidMITid(void)
{
	int result;
	char *sql = "DELETE FROM TransData WHERE HDTid = ? AND MITid = ?";	
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].HDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MITid);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delete,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_BatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransData WHERE (szHostLabel = ? AND szBatchNo = ? AND MITid = ?)";	
	int inDBResult = 0;
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)srTransRec.szHostLabel, strlen((char*)srTransRec.szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, srTransRec.szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, srTransRec.MITid);


	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch dele,sqlite3_exec[%d]MITid[%d]",inDBResult,srTransRec.MITid);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_BatchRead(TRANS_DATA_TABLE *transData, int inSeekCnt)
{
	int result;
	int inResult = d_NO;
//	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM TransData WHERE ulSavedIndex = ?";
        char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt, szTransRef, szRefNo, szBankTransId, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len, szOKDTransRef, szOKDRefNo, szOKDBankTransId FROM TransData WHERE ulSavedIndex = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szPINBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            transData->fIsInstallment = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szInstallmentTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szMonthlyAmt, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szInterestRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szTotalInterest, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            memcpy(transData->stIPPinfo.szHandlingFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szIPPSchemeID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szFreqInstallment, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szTransCurrency, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szIPPTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fAlipay = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fIsDiscounted = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			memcpy(transData->szFixedAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
			memcpy(transData->szOrigAmountDisc, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fIsDiscountedFixAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			memcpy(transData->szPercentage, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fIsDiscountedPercAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		//For CB Pay
		strcpy((char*)transData->szTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));  
		strcpy((char*)transData->szBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));  

		memcpy(transData->stEMVinfo.T9F63, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
		transData->stEMVinfo.T9F63_len = sqlite3_column_int(stmt,inStmtSeq +=1 );


		memcpy(transData->stEMVinfo.T9F6E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
		transData->stEMVinfo.T9F63_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F7C, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
		transData->stEMVinfo.T9F7C_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		//For OK$
		strcpy((char*)transData->szOKDTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szOKDRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));  
		strcpy((char*)transData->szOKDBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));  

			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

int inDatabase_BatchReadByHostidAndMITid(TRANS_DATA_TABLE *transData,int inHDTid,int inMITid)
{
	int result;
	int inResult = d_NO;
//	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM MMT WHERE HDTid = ? AND MITid = ?";
        char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt, szTransRef, szRefNo, szBankTransId, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len, szOKDTransRef, szOKDRefNo, szOKDBankTransId FROM MMT WHERE HDTid = ? AND MITid = ?";

	int incount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMITid);
	
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {		
		result = sqlite3_step(stmt);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szPINBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);			
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);		
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            transData->fIsInstallment = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szInstallmentTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szMonthlyAmt, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szInterestRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szTotalInterest, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            memcpy(transData->stIPPinfo.szHandlingFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szIPPSchemeID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szFreqInstallment, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szTransCurrency, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szIPPTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fAlipay = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
            transData->fIsDiscounted = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));										
			memcpy(transData->szFixedAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
			memcpy(transData->szOrigAmountDisc, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fIsDiscountedFixAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));										
			memcpy(transData->szPercentage, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fIsDiscountedPercAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));										


		//For CB Pay  
		strcpy((char*)transData->szTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		
		memcpy(transData->stEMVinfo.T9F63, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);		
		transData->stEMVinfo.T9F63_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F6E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);		
		transData->stEMVinfo.T9F6E_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
		
		memcpy(transData->stEMVinfo.T9F7C, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);		
		transData->stEMVinfo.T9F7C_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		//For OK$  
		strcpy((char*)transData->szOKDTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szOKDRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szOKDBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    
    return(inResult);
}


int inDatabase_BatchInsert(TRANS_DATA_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM TransData";
// 	char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt, szTransRef, szRefNo, szBankTransId, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len, szOKDTransRef, szOKDRefNo, szOKDBankTransId) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

	vdDebug_LogPrintf("inDatabase_BatchInsert");

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPINBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsInstallment);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szInstallmentTerms, strlen((char*)transData->stIPPinfo.szInstallmentTerms), SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szMonthlyAmt, 7, SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szInterestRate, strlen((char*)transData->stIPPinfo.szInterestRate), SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szTotalInterest, 7, SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szHandlingFee, 7, SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szIPPSchemeID, strlen((char*)transData->stIPPinfo.szIPPSchemeID), SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szFreqInstallment, strlen((char*)transData->stIPPinfo.szFreqInstallment), SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szTransCurrency, strlen((char*)transData->stIPPinfo.szTransCurrency), SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szIPPTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fAlipay);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscounted);	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szFixedAmount, 7, SQLITE_STATIC);	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrigAmountDisc, 7, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscountedFixAmt);	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPercentage, 7, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscountedPercAmt);	

	//For CB Pay
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTransRef, strlen((char*)transData->szTransRef), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szRefNo, strlen((char*)transData->szRefNo), SQLITE_STATIC);   
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBankTransId, strlen((char*)transData->szRefNo), SQLITE_STATIC);   

	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63_len);

	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E_len);
	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C, 32, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C_len);

	//For OK$
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szOKDTransRef, strlen((char*)transData->szOKDTransRef), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szOKDRefNo, strlen((char*)transData->szOKDRefNo), SQLITE_STATIC);   
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szOKDBankTransId, strlen((char*)transData->szOKDBankTransId), SQLITE_STATIC);   


	vdDebug_LogPrintf("transData->HDTid[%d]",transData->HDTid);
	vdDebug_LogPrintf("transData->szHostLabel[%s]",transData->szHostLabel);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_BatchUpdate(TRANS_DATA_TABLE *transData)
{
	int result;
//	char *sql = "UPDATE TransData SET HDTid = ?, MITid = ?, CDTid = ?, IITid = ?, szHostLabel = ?, szBatchNo = ?, byTransType = ?, byPanLen = ?, szExpireDate = ?, byEntryMode = ?, szTotalAmount = ?, szBaseAmount = ?, szTipAmount = ?, byOrgTransType = ?, szMacBlock = ?, szPINBlock = ?, szYear = ?, szDate = ?, szTime = ?, szOrgDate = ?, szOrgTime = ?, szAuthCode = ?, szRRN = ?, szInvoiceNo = ?, szOrgInvoiceNo = ?, byPrintType = ?, byVoided = ?, byAdjusted = ?, byUploaded = ?, byTCuploaded = ?, szCardholderName = ?, szzAMEX4DBC = ?, szStoreID = ?, szRespCode = ?, szServiceCode = ?, byContinueTrans = ?, byOffline = ?, byReversal = ?, byEMVFallBack = ?, shTransResult = ?, szTpdu = ?, szIsoField03 = ?, szMassageType = ?, szPAN = ?, szCardLable = ?, usTrack1Len = ?, usTrack2Len = ?, usTrack3Len = ?, szTrack1Data = ?, szTrack2Data = ?, szTrack3Data = ?, usChipDataLen = ?, baChipData = ?, usAdditionalDataLen = ?, baAdditionalData = ?, bWaveSID = ?,usWaveSTransResult = ?,bWaveSCVMAnalysis = ?, ulTraceNum = ?, ulOrgTraceNum = ?, usTerminalCommunicationMode = ?, ulSavedIndex = ?, byPINEntryCapability = ?, byPackType = ?, szOrgAmount = ?, szCVV2 = ?, inCardType = ?, byTCFailUpCnt = ?, byCardTypeNum = ?, byEMVTransStatus = ?, T5A_len = ?, T5A = ?, T5F2A = ?, T5F30 = ?, T5F34 = ?, T5F34_len = ?, T82 = ?, T84_len = ?, T84 = ?, T8A = ?, T91 = ?, T91Len = ?, T95 = ?, T9A = ?, T9C = ?, T9F02 = ?, T9F03 = ?, T9F09 = ?, T9F10_len = ?, T9F10 = ?, T9F1A = ?, T9F26 = ?, T9F27 = ?, T9F33 = ?, T9F34 = ?, T9F35 = ?, T9F36_len = ?, T9F36 = ?, T9F37 = ?, T9F41 = ?, T9F53 = ?, ISR = ?, ISRLen = ?, T9B = ?, T5F24 = ?, T71Len = ?, T71 = ?, T72Len = ?, T72 = ?, T9F06 = ?, T9F1E = ?, T9F28 = ?, T9F29 = ?, szChipLabel = ?, szTID = ?, szMID = ? WHERE ulSavedIndex = ?";
        char *sql = "UPDATE TransData SET HDTid = ?, MITid = ?, CDTid = ?, IITid = ?, szHostLabel = ?, szBatchNo = ?, byTransType = ?, byPanLen = ?, szExpireDate = ?, byEntryMode = ?, szTotalAmount = ?, szBaseAmount = ?, szTipAmount = ?, byOrgTransType = ?, szMacBlock = ?, szPINBlock = ?, szYear = ?, szDate = ?, szTime = ?, szOrgDate = ?, szOrgTime = ?, szAuthCode = ?, szRRN = ?, szInvoiceNo = ?, szOrgInvoiceNo = ?, byPrintType = ?, byVoided = ?, byAdjusted = ?, byUploaded = ?, byTCuploaded = ?, szCardholderName = ?, szzAMEX4DBC = ?, szStoreID = ?, szRespCode = ?, szServiceCode = ?, byContinueTrans = ?, byOffline = ?, byReversal = ?, byEMVFallBack = ?, shTransResult = ?, szTpdu = ?, szIsoField03 = ?, szMassageType = ?, szPAN = ?, szCardLable = ?, usTrack1Len = ?, usTrack2Len = ?, usTrack3Len = ?, szTrack1Data = ?, szTrack2Data = ?, szTrack3Data = ?, usChipDataLen = ?, baChipData = ?, usAdditionalDataLen = ?, baAdditionalData = ?, bWaveSID = ?,usWaveSTransResult = ?,bWaveSCVMAnalysis = ?, ulTraceNum = ?, ulOrgTraceNum = ?, usTerminalCommunicationMode = ?, ulSavedIndex = ?, byPINEntryCapability = ?, byPackType = ?, szOrgAmount = ?, szCVV2 = ?, inCardType = ?, byTCFailUpCnt = ?, byCardTypeNum = ?, byEMVTransStatus = ?, T5A_len = ?, T5A = ?, T5F2A = ?, T5F30 = ?, T5F34 = ?, T5F34_len = ?, T82 = ?, T84_len = ?, T84 = ?, T8A = ?, T91 = ?, T91Len = ?, T95 = ?, T9A = ?, T9C = ?, T9F02 = ?, T9F03 = ?, T9F09 = ?, T9F10_len = ?, T9F10 = ?, T9F1A = ?, T9F26 = ?, T9F27 = ?, T9F33 = ?, T9F34 = ?, T9F35 = ?, T9F36_len = ?, T9F36 = ?, T9F37 = ?, T9F41 = ?, T9F53 = ?, ISR = ?, ISRLen = ?, T9B = ?, T5F24 = ?, T71Len = ?, T71 = ?, T72Len = ?, T72 = ?, T9F06 = ?, T9F1E = ?, T9F28 = ?, T9F29 = ?, szChipLabel = ?, szTID = ?, szMID = ?, fIsInstallment = ?, szInstallmentTerms = ?, szMonthlyAmt = ?, szInterestRate = ?, szTotalInterest = ?, szHandlingFee = ?, szIPPSchemeID = ?, szFreqInstallment = ?, szTransCurrency = ?, szIPPTotalAmount = ?, fAlipay = ?, fIsDiscounted = ?, szFixedAmount = ?, szOrigAmountDisc = ?, fIsDiscountedFixAmt = ?, szPercentage = ?, fIsDiscountedPercAmt = ?, szTransRef = ?, szRefNo = ?, szBankTransId=?, szOKDTransRef = ?, szOKDRefNo = ?, szOKDBankTransId = ? WHERE ulSavedIndex = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPINBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsInstallment);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szInstallmentTerms, strlen((char*)transData->stIPPinfo.szInstallmentTerms), SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szMonthlyAmt, 7, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szInterestRate, strlen((char*)transData->stIPPinfo.szInterestRate), SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szTotalInterest, 7, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szHandlingFee, 7, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szIPPSchemeID, strlen((char*)transData->stIPPinfo.szIPPSchemeID), SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szFreqInstallment, strlen((char*)transData->stIPPinfo.szFreqInstallment), SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szTransCurrency, strlen((char*)transData->stIPPinfo.szTransCurrency), SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szIPPTotalAmount, 7, SQLITE_STATIC);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fAlipay);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscounted);	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szFixedAmount, 7, SQLITE_STATIC);	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrigAmountDisc, 7, SQLITE_STATIC);		
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscountedFixAmt);	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPercentage, 7, SQLITE_STATIC);	
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscountedPercAmt);


		//FOR CBPAY
		result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTransRef, strlen((char*)transData->szTransRef), SQLITE_STATIC); 
		result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szRefNo, strlen((char*)transData->szRefNo), SQLITE_STATIC);  
		result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szBankTransId, strlen((char*)transData->szRefNo), SQLITE_STATIC);

	//FOR OK$
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szOKDTransRef, strlen((char*)transData->szOKDTransRef), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szOKDRefNo, strlen((char*)transData->szOKDRefNo), SQLITE_STATIC);  
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szOKDBankTransId, strlen((char*)transData->szOKDBankTransId), SQLITE_STATIC);
	
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->ulSavedIndex);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	vdDebug_LogPrintf("inDatabase_BatchUpdate Inv Num:[%02X%02X%02X]",transData->szInvoiceNo[0], transData->szInvoiceNo[1], transData->szInvoiceNo[2]);	
	vdDebug_LogPrintf("inDatabase_BatchUpdate transData->szOKDTransRef[%s]", transData->szOKDTransRef);
	vdDebug_LogPrintf("inDatabase_BatchUpdate transData->szOKDRefNo[%s]", transData->szOKDRefNo);
	vdDebug_LogPrintf("inDatabase_BatchUpdate transData->szOKDBankTransId[%s]", transData->szOKDBankTransId);
	vdDebug_LogPrintf("inDatabase_BatchUpdate transData->byVoided [%d]",transData->byVoided);
	vdDebug_LogPrintf("inDatabase_BatchUpdate transData->byUploaded [%d]",transData->byUploaded);
	vdDebug_LogPrintf("inDatabase_BatchUpdate transData->byTCuploaded [%d]",transData->byTCuploaded);
	
   
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType)
{
	int inResult;

	vdDebug_LogPrintf("inDatabase_BatchSave inStoredType= %d, transData->byTransType=%d", inStoredType, transData->byTransType);
	

    if (inStoredType == DF_BATCH_APPEND)
    {
		if(transData->byTransType == PRE_AUTH)
		{
			inResult=inDatabase_PreauthBatchInsert(transData);
			if (inResult != ST_SUCCESS)
			{
				return ST_ERROR;
			}
		}	 
		#if 0
		else if(transData->byTransType == PREAUTH_COMP)
		{
			//vdDebug_LogPrintf("transData->szPreAuthCode=%s", transData->szPreAuthCode);
			inResult=inDatabase_PreauthBatchDeletePerAuthCode(transData->szPreAuthCode);
			if (inResult != ST_SUCCESS)
			{
				return ST_ERROR;
			}			
		}
        #endif
		inResult = inDatabase_BatchInsert(transData);

		vdDebug_LogPrintf("inDatabase_BatchSave::inDatabase_BatchInsert inResult [%d]", inResult);
		
		if (inResult != ST_SUCCESS)
		{
			return ST_ERROR;
		}
    }
    else if(inStoredType == DF_BATCH_UPDATE)//default for tip or adjust txn
    {
		if(transData->byTransType == VOID && transData->byOrgTransType == PRE_AUTH)
		{
            inResult = inDatabase_BatchUpdatePreauth(transData);
            if (inResult != ST_SUCCESS)
            {
                return ST_ERROR;
            }
		}
		
		inResult = inDatabase_BatchUpdate(transData);

		vdDebug_LogPrintf("inDatabase_BatchSave::inDatabase_BatchUpdate inResult [%d]", inResult);
		
		if (inResult != ST_SUCCESS)
		{
			return ST_ERROR;
		}
    }
    else
    {
        return ST_ERROR;
    }

    strHDT.ulLastTransSavedIndex = transData->ulSavedIndex;

	vdDebug_LogPrintf("inDatabase_BatchSave END");

    return ST_SUCCESS;
}

int inDatabase_BatchSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo)
{
	int result,i = 0;
	int inResult = d_NO;
//	char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM TransData WHERE szInvoiceNo = ?";
        char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt, szTransRef, szRefNo, szBankTransId, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len, szOKDTransRef, szOKDRefNo, szOKDBankTransId FROM TransData WHERE szInvoiceNo = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, hexInvoiceNo, 4, SQLITE_STATIC);

	DebugAddINT("inDatabase_BatchSearch sqlite3_bind_blob", result);
	DebugAddHEX("hexInvoiceNo", hexInvoiceNo, 3);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		//DebugAddINT("if(100)=Find", result);
		//DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq);
			transData->MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq+=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szPINBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);						
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
            strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            transData->fIsInstallment = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szInstallmentTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szMonthlyAmt, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szInterestRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szTotalInterest, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            memcpy(transData->stIPPinfo.szHandlingFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szIPPSchemeID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szFreqInstallment, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szTransCurrency, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szIPPTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fAlipay = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
            transData->fIsDiscounted = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		
			memcpy(transData->szFixedAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
			memcpy(transData->szOrigAmountDisc, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);			
            transData->fIsDiscountedFixAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		
			memcpy(transData->szPercentage, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fIsDiscountedPercAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		


			//For CBPAY 
			strcpy((char*)transData->szTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));  
			strcpy((char*)transData->szBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    memcpy(transData->stEMVinfo.T9F63, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T9F63_len = sqlite3_column_int(stmt,inStmtSeq +=1 );


		memcpy(transData->stEMVinfo.T9F6E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
		transData->stEMVinfo.T9F6E_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
						

		memcpy(transData->stEMVinfo.T9F7C, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
		transData->stEMVinfo.T9F7C_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		//For OK$ 
		strcpy((char*)transData->szOKDTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szOKDRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));  
		strcpy((char*)transData->szOKDBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		}
	} while (result == SQLITE_ROW);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	
	/*vdDebug_LogPrintf("transData->szOKDTransRef[%s]", transData->szOKDTransRef);
	vdDebug_LogPrintf("transData->szOKDRefNo[%s]", transData->szOKDRefNo);
	vdDebug_LogPrintf("transData->szOKDBankTransId[%s]", transData->szOKDBankTransId);*/
	vdDebug_LogPrintf("transData->byVoided [%d]",transData->byVoided);
	vdDebug_LogPrintf("transData->szRRN[%s]", transData->szRRN);
	vdDebug_LogPrintf("transData->byUploaded[%d]", transData->byUploaded);
	vdDebug_LogPrintf("transData->byTCuploaded[%d]", transData->byTCuploaded);
	vdDebug_LogPrintf("transData->byAdjusted[%d]", transData->byAdjusted);
	vdDebug_LogPrintf("transData->shTransResult[%d]", transData->shTransResult);	

	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

int inDatabase_BatchCheckDuplicateInvoice(char *hexInvoiceNo)
{
	int result;
	char *sql = "SELECT * FROM TransData WHERE szInvoiceNo = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, hexInvoiceNo, 4, SQLITE_STATIC);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			
			sqlite3_exec(db,"commit;",NULL,NULL,NULL);

			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return 0x0080;
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inDatabase_BatchSearchByRRN(TRANS_DATA_TABLE *transData, char *strRRN)
{
	int result,i = 0;
	int inResult = d_NO;
//	char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM TransData WHERE szRRN = ?";
    char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt FROM TransData WHERE szRRN = ?";

	vdDebug_LogPrintf("=====inDatabase_BatchSearchByRRN=====");
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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

	vdDebug_LogPrintf("strRRN[%s]", strRRN);
	DebugAddHEX("strRRN", strRRN, 13);

	inStmtSeq = 0;
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strRRN, 13, SQLITE_STATIC);

	DebugAddINT("inDatabase_BatchSearchByRRN sqlite3_bind_blob", result);
	DebugAddHEX("strRRN", strRRN, 12);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		DebugAddINT("if(100)=Find", result);
		DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq);
			transData->MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq+=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szPINBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);						
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
            strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
            transData->fIsInstallment = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szInstallmentTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szMonthlyAmt, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szInterestRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szTotalInterest, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            memcpy(transData->stIPPinfo.szHandlingFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            strcpy((char*)transData->stIPPinfo.szIPPSchemeID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szFreqInstallment, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->stIPPinfo.szTransCurrency, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->stIPPinfo.szIPPTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fAlipay = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
            transData->fIsDiscounted = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			memcpy(transData->szFixedAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
			memcpy(transData->szOrigAmountDisc, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);			
            transData->fIsDiscountedFixAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			memcpy(transData->szPercentage, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fIsDiscountedPercAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			
		}
	} while (result == SQLITE_ROW);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}


int inDatabase_BatchSearchByAuthCode(TRANS_DATA_TABLE *transData, char *strAuthCode)
{
	int result,i = 0;
	int inResult = d_NO;
//	char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM TransData WHERE szRRN = ?";
    //char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay FROM TransData WHERE szAuthCode = ?";


    //char *sql = "SELECT szYear, szDate, szTime, szAuthCode, szPAN, ulSavedIndex, szTotalAmount FROM PreauthData WHERE szAuthCode = ?";
    // fix for User Input Amount and Printing Amount in Receipt Mismatch for Preauth Complete Transaction - #1688
    //removed on select original preauth szTotalAmount.
	char *sql = "SELECT szYear, szDate, szTime, szAuthCode, szPAN, ulSavedIndex FROM PreauthData WHERE szAuthCode = ?";


	vdDebug_LogPrintf("=====inDatabase_BatchSearchByAuthCode=====");
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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

	vdDebug_LogPrintf("strAuthCode[%s]", strAuthCode);
	DebugAddHEX("strAuthCode", strAuthCode, 13);

	inStmtSeq = 0;
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strAuthCode, 7, SQLITE_STATIC);

	DebugAddINT("inDatabase_BatchSearchByAuthCode sqlite3_bind_blob", result);
	DebugAddHEX("strAuthCode", strAuthCode, 6);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		//DebugAddINT("if(100)=Find", result);
		//DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );	
			//memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);	
		}
	} while (result == SQLITE_ROW);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}


int inMSGResponseCodeRead(char* szMsg, int inMsgIndex, int inHostIndex)
{
	int result;
  	char *sql = "SELECT szMsg FROM MSG WHERE usMsgIndex = ? AND inHostIndex = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMsgIndex);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndex);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */		
			inStmtSeq = 0;

			 /* szMsg */
			 strcpy((char*)szMsg, (char *)sqlite3_column_text(stmt,inStmtSeq ));
   		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inMSGResponseCodeReadByActionCode(char* szMsg, int inActionCode, int inHostIndex)
{
	int result;
  	char *sql = "SELECT szMsg FROM MSG WHERE usActionCode = ? AND inHostIndex = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inActionCode);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndex);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */		
			inStmtSeq = 0;

			 /* szMsg */
			 strcpy((char*)szMsg, (char *)sqlite3_column_text(stmt,inStmtSeq ));
   		}
	} while (result == SQLITE_ROW);	   

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}



int inMPUBatchNumRecord(int inHostIndex, int inMerchIndex, char *szBatchNo)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TransData WHERE HDTid = ? AND MITid = ? AND szBatchNo = ?";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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

	sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndex);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMerchIndex);
	sqlite3_bind_blob(stmt, inStmtSeq +=1, szBatchNo, 3, SQLITE_STATIC);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);


	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);


    vdDebug_LogPrintf("inBatchNumRecord inHostIndex[%d]", inHostIndex);
    vdDebug_LogPrintf("inBatchNumRecord inMerchIndex[%d]", inMerchIndex);
	vdDebug_LogPrintf("inBatchNumRecord srTransRec.szBatchNo=[%02x]-----",szBatchNo[2]);
    vdDebug_LogPrintf("inBatchNumRecord inCount[%d]", inCount);


	return(inCount);
}

int inBatchNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TransData WHERE HDTid = ? AND MITid = ? AND szBatchNo = ?";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].HDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, strMMT[0].MITid);
	sqlite3_bind_blob(stmt, inStmtSeq +=1, strMMT[0].szBatchNo, 3, SQLITE_STATIC);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);


	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);


    vdDebug_LogPrintf("inBatchNumRecord inCount[%d]", inCount);

	return(inCount);
}



int inBatchByMerchandHost(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID)
{
	int result;
  	char *sql = "SELECT TransDataid FROM TransData WHERE HDTid = ? AND MITid = ? AND szBatchNo = ?";
	int count = 0;

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inHostIndex);
	sqlite3_bind_int(stmt, inStmtSeq +=1, inMerchIndex);
	sqlite3_bind_blob(stmt, inStmtSeq +=1, szBatchNo, 3, SQLITE_STATIC);

	inStmtSeq = 0;
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */		

			 inTranID[count] = sqlite3_column_int(stmt,inStmtSeq);
             DebugAddINT("inTranID", inTranID[count]);
			 count++;

   		}
	} while (result == SQLITE_ROW);	   

    
    vdDebug_LogPrintf("inBatchByMerchandHost inHostIndex[%d]", inHostIndex);
    vdDebug_LogPrintf("inBatchByMerchandHost inMerchIndex[%d]", inMerchIndex);
	vdDebug_LogPrintf("inBatchByMerchandHost srTransRec.szBatchNo=[%02x]-----",szBatchNo[2]);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}

int inDatabase_BatchReadByTransId(TRANS_DATA_TABLE *transData, int inTransDataid)
{
	int result;
	int inResult = d_NO;
//	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo  FROM TransData WHERE TransDataid = ?";
        char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt, szTransRef, szRefNo, szBankTransId, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len, szOKDTransRef, szOKDRefNo, szOKDBankTransId FROM TransData WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inTransDataid);

	inStmtSeq = 0;
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szPINBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);			
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
                        transData->fIsInstallment = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->stIPPinfo.szInstallmentTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->stIPPinfo.szMonthlyAmt, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
                        strcpy((char*)transData->stIPPinfo.szInterestRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->stIPPinfo.szTotalInterest, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
                        memcpy(transData->stIPPinfo.szHandlingFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
                        strcpy((char*)transData->stIPPinfo.szIPPSchemeID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->stIPPinfo.szFreqInstallment, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->stIPPinfo.szTransCurrency, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->stIPPinfo.szIPPTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fAlipay = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            transData->fIsDiscounted = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			memcpy(transData->szFixedAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
			memcpy(transData->szOrigAmountDisc, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);			
            transData->fIsDiscountedFixAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			memcpy(transData->szPercentage, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fIsDiscountedPercAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	


			//For CBPay 
			strcpy((char*)transData->szTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)transData->szBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		memcpy(transData->stEMVinfo.T9F63, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
		transData->stEMVinfo.T9F63_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F6E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
		transData->stEMVinfo.T9F6E_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F7C, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
		transData->stEMVinfo.T9F7C_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		//For OK$ 
		strcpy((char*)transData->szOKDTransRef, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szOKDRefNo, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		strcpy((char*)transData->szOKDBankTransId, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
		}
	} while (result == SQLITE_ROW);


	
	vdDebug_LogPrintf("inDatabase_BatchReadByTransId:transData->HDTid:[%d]", transData->HDTid);
	vdDebug_LogPrintf("inDatabase_BatchReadByTransId:ulTraceNum->[%d]",transData->ulTraceNum);

	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

int inMultiAP_Database_BatchRead(TRANS_DATA_TABLE *transData)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT MITid, HDTid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len FROM TransData WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->MITid = sqlite3_column_int(stmt,inStmtSeq);
			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szPINBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);		
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
			strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			memcpy(transData->stEMVinfo.T9F63, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T9F63_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F6E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
		transData->stEMVinfo.T9F6E_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F7C, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
		transData->stEMVinfo.T9F7C_len = sqlite3_column_int(stmt,inStmtSeq +=1 );			
			
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
        sqlite3_errmsg(db);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

int inMultiAP_Database_BatchUpdate(TRANS_DATA_TABLE *transData)
{
	int result;
	char *sql = "UPDATE TransData SET HDTid = ?, MITid = ?, CDTid = ?, IITid = ?, szHostLabel = ?, szBatchNo = ?, byTransType = ?, byPanLen = ?, szExpireDate = ?, byEntryMode = ?, szTotalAmount = ?, szBaseAmount = ?, szTipAmount = ?, byOrgTransType = ?, szMacBlock = ?, szPINBlock = ?, szYear = ?, szDate = ?, szTime = ?, szOrgDate = ?, szOrgTime = ?, szAuthCode = ?, szRRN = ?, szInvoiceNo = ?, szOrgInvoiceNo = ?, byPrintType = ?, byVoided = ?, byAdjusted = ?, byUploaded = ?, byTCuploaded = ?, szCardholderName = ?, szzAMEX4DBC = ?, szStoreID = ?, szRespCode = ?, szServiceCode = ?, byContinueTrans = ?, byOffline = ?, byReversal = ?, byEMVFallBack = ?, shTransResult = ?, szTpdu = ?, szIsoField03 = ?, szMassageType = ?, szPAN = ?, szCardLable = ?, usTrack1Len = ?, usTrack2Len = ?, usTrack3Len = ?, szTrack1Data = ?, szTrack2Data = ?, szTrack3Data = ?, usChipDataLen = ?, baChipData = ?, usAdditionalDataLen = ?, baAdditionalData = ?, bWaveSID = ?,usWaveSTransResult = ?,bWaveSCVMAnalysis = ?, ulTraceNum = ?, ulOrgTraceNum = ?, usTerminalCommunicationMode = ?, ulSavedIndex = ?, byPINEntryCapability = ?, byPackType = ?, szOrgAmount = ?, szCVV2 = ?, inCardType = ?, byTCFailUpCnt = ?, byCardTypeNum = ?, byEMVTransStatus = ?, T5A_len = ?, T5A = ?, T5F2A = ?, T5F30 = ?, T5F34 = ?, T5F34_len = ?, T82 = ?, T84_len = ?, T84 = ?, T8A = ?, T91 = ?, T91Len = ?, T95 = ?, T9A = ?, T9C = ?, T9F02 = ?, T9F03 = ?, T9F09 = ?, T9F10_len = ?, T9F10 = ?, T9F1A = ?, T9F26 = ?, T9F27 = ?, T9F33 = ?, T9F34 = ?, T9F35 = ?, T9F36_len = ?, T9F36 = ?, T9F37 = ?, T9F41 = ?, T9F53 = ?, ISR = ?, ISRLen = ?, T9B = ?, T5F24 = ?, T71Len = ?, T71 = ?, T72Len = ?, T72 = ?, T9F06 = ?, T9F1E = ?, T9F28 = ?, T9F29 = ?, szChipLabel = ?, szTID = ?, szMID = ?, T9F63 = ?, T9F63_LEN = ?, T9F6E = ?, T9F6E_len = ?, T9F7C = ?, T9F7C_len = ? WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
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
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPINBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63_len);

	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E_len);
	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C, 32, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C_len);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inMultiAP_Database_BatchInsert(TRANS_DATA_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM TransData";
 	char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len) VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	   
	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
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
	//vdCTOSS_GetMemoryStatus("bagin Database");

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPINBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63_len);

	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E_len);

	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C, 32, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C_len);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inMultiAP_Database_BatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransData";	
	int inDBResult =0 ;
	
	//1205
	remove(DB_MULTIAP_JOURNAL);
	//1205

	/* open the database */
	result = sqlite3_open(DB_MULTIAP,&db);
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

//	inStmtSeq = 0;
//	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)srTransRec.szHostLabel, strlen((char*)srTransRec.szHostLabel), SQLITE_STATIC); 
//	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, srTransRec.szBatchNo, 3, SQLITE_STATIC);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inMultiAP_Database_EMVOpenDatabase(void)
{
	int result;
	
	/* open the database */
	result = sqlite3_open(DB_EMV,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	return d_OK;
}

int inMultiAP_Database_EMVCloseDatabase(void)
{
	int result;
	
	if (db != NULL) sqlite3_close(db);
	db = NULL;

	return d_OK;
}


int inMultiAP_Database_EMVTransferDataInit(void)
{
    int inSeekCnt = 1;
    USHORT usDataLen = 0;
    BYTE bEMVData[4];
    int result;
    char *sql = "UPDATE ETD SET inEMVDataLen = ? ,szEMVTransferData = ? WHERE  ETDid = ?";          
   
    /* open the database */
    result = sqlite3_open(DB_EMV,&db);
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

    memset(bEMVData, 0x00, sizeof(bEMVData));
    inStmtSeq = 0;

    /* inEMVDataLen */
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, usDataLen);
    
    /* szEMVTransferData */    
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, bEMVData, 1, SQLITE_STATIC);


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



int inMultiAP_Database_EMVTransferDataWrite(USHORT usDataLen, BYTE *bEMVData)
{
    int inSeekCnt = 1;
    int result;
    char *sql = "UPDATE ETD SET inEMVDataLen = ? ,szEMVTransferData = ? WHERE  ETDid = ?";          
  
    /* open the database */
    result = sqlite3_open(DB_EMV,&db);
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

    /* inEMVDataLen */
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, usDataLen);
    
    /* szEMVTransferData */    
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, bEMVData, usDataLen, SQLITE_STATIC);


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


int inMultiAP_Database_EMVTransferDataRead(USHORT *usDataLen, BYTE *bEMVData)
{
    int inSeekCnt = 1;
    int result;
    int inResult = d_NO;
    char *sql = "SELECT inEMVDataLen, szEMVTransferData FROM ETD WHERE ETDid = ?";
        
    /* open the database */
    result = sqlite3_open(DB_EMV,&db);
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

    sqlite3_bind_int(stmt, 1, inSeekCnt);

    /* loop reading each row until step returns anything other than SQLITE_ROW */
    do {
        result = sqlite3_step(stmt);
        if (result == SQLITE_ROW) { /* can read data */
            inResult = d_OK;
            inStmtSeq = 0;

            /*inEMVDataLen*/
            *usDataLen = sqlite3_column_int(stmt,inStmtSeq  );
            
            /*szEMVTransferData*/
            memcpy(bEMVData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), *usDataLen);

        }
    } while (result == SQLITE_ROW);

    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return(inResult);
}

//for Sharls_COM modem
#if 0
int inMultiAP_Database_COM_Read(void)
{
    int result;
    int inSeekCnt = 1;
    int len = 0;
    int inResult = -1;
    char *sql = "SELECT inCommMode, inHeaderFormat,bSSLFlag, szCAFileName, szClientFileName, szClientKeyFileName, szPriPhoneNum, szSecPhoneNum, szPriHostIP, ulPriHostPort, szSecHostIP, ulSecHostPort, bDialBackUp, bPredialFlag, inParaMode, inHandShake, inCountryCode, szATCMD1, szATCMD2, szATCMD3, szATCMD4, szATCMD5, inConnectionTimeOut, inReceiveTimeOut, inGPRSSingal, inSendLen, szSendData, inReceiveLen, szReceiveData FROM COM WHERE COMid = ?";

    /* open the database */
    result = sqlite3_open(DB_COM,&db);
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
    vdDebug_LogPrintf("**inMultiAP_Database_COM_Read**");

    sqlite3_bind_int(stmt, 1, inSeekCnt);

    /* loop reading each row until step returns anything other than SQLITE_ROW */
    do {
        result = sqlite3_step(stmt);
        if (result == SQLITE_ROW) { /* can read data */
            inResult = d_OK;
            inStmtSeq = 0;

           /* inCommMode */
           strCOM.inCommMode = sqlite3_column_int(stmt, inStmtSeq );

           
           /* inCommMode */
           strCOM.inHeaderFormat = sqlite3_column_int(stmt, inStmtSeq +=1 );

           /* fDialMode*/
           strCOM.bSSLFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

           /* szCAFileName*/
			strcpy((char*)strCOM.szCAFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szClientFileName*/
             strcpy((char*)strCOM.szClientFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szClientKeyFileName*/
             strcpy((char*)strCOM.szClientKeyFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szPriPhoneNum*/
             strcpy((char*)strCOM.szPriPhoneNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szSecPhoneNum*/
             strcpy((char*)strCOM.szSecPhoneNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /* szPriHostIP*/
             strcpy((char*)strCOM.szPriHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szPriHostIP*/
			strCOM.ulPriHostPort = sqlite3_column_double(stmt,inStmtSeq +=1);

            /* szSecHostIP*/
             strcpy((char*)strCOM.szSecHostIP, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            /* szPriHostIP*/
			strCOM.ulSecHostPort = sqlite3_column_double(stmt,inStmtSeq +=1);
            vdDebug_LogPrintf("**inMultiAP_Database_COM_Read inSendLen**");

			/* bDialBackUp*/
			strCOM.bDialBackUp = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
			/* bPredialFlag*/
			strCOM.bPredialFlag = fGetBoolean((BYTE *)sqlite3_column_text(stmt, inStmtSeq +=1 ));
			/* inParaMode */
			strCOM.inParaMode = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inHandShake */
			strCOM.inHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inCountryCode */
			strCOM.inCountryCode = sqlite3_column_int(stmt, inStmtSeq +=1 );		
			/* szATCMD1*/
			strcpy((char*)strCOM.szATCMD1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD2*/
			strcpy((char*)strCOM.szATCMD2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD3*/
			strcpy((char*)strCOM.szATCMD3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD4*/
			strcpy((char*)strCOM.szATCMD4, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* szATCMD5*/
			strcpy((char*)strCOM.szATCMD5, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			/* inConnectionTimeOut */
			strCOM.inConnectionTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inReceiveTimeOut */
			strCOM.inReceiveTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/* inGPRSSingal */
			strCOM.inGPRSSingal = sqlite3_column_int(stmt, inStmtSeq +=1 );

            /* inSendLen*/
            strCOM.inSendLen = sqlite3_column_int(stmt, inStmtSeq +=1 );
            vdDebug_LogPrintf("**inMultiAP_Database_COM_Read inSendLen[%d]**",strCOM.inSendLen);
            /*szSendData*/
			memcpy(strCOM.szSendData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), strCOM.inSendLen);
            vdDebug_LogPrintf("**inMultiAP_Database_COM_Read data inSendLen[%d]**",strCOM.inSendLen);

            /* inReceiveLen*/
            strCOM.inReceiveLen = sqlite3_column_int(stmt, inStmtSeq +=1 );
			/*szReceiveData*/
			memcpy(strCOM.szReceiveData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), strCOM.inReceiveLen);

        }
    } while (result == SQLITE_ROW); 

    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);
    //???sqlite????
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    vdDebug_LogPrintf((". *COM read"));

    return(inResult);
}


int inMultiAP_Database_COM_Save(void)
{
    int inSeekCnt = 1;
    int result;
    char *sql = "UPDATE COM SET inCommMode = ? ,inHeaderFormat = ? ,bSSLFlag = ? ,szCAFileName = ? ,szClientFileName = ? ,szClientKeyFileName = ? ,szPriPhoneNum = ? ,szSecPhoneNum = ? ,szPriHostIP = ? ,ulPriHostPort = ? ,szSecHostIP = ? ,ulSecHostPort = ? ,bDialBackUp = ? ,bPredialFlag = ? ,inParaMode = ? ,inHandShake = ? ,inCountryCode = ? ,szATCMD1 = ? ,szATCMD2 = ? ,szATCMD3 = ? ,szATCMD4 = ? ,szATCMD5 = ? ,inConnectionTimeOut = ? ,inReceiveTimeOut = ? ,inGPRSSingal = ? ,inSendLen = ? ,szSendData = ? ,inReceiveLen = ?,szReceiveData = ? WHERE  COMid = ?";          
 
    /* open the database */
    result = sqlite3_open(DB_COM,&db);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
    vdDebug_LogPrintf("**inMultiAP_Database_COM_Save **");
    sqlite3_exec( db, "begin", 0, 0, NULL );
    /* prepare the sql, leave stmt ready for loop */
    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        sqlite3_close(db);
        return 2;
    }

	inStmtSeq = 0;
        
    /* inCommMode*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCommMode);
    /* inHeaderFormat*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHeaderFormat);
    /* bSSLFlag*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bSSLFlag);
    /* szCAFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szCAFileName, strlen((char*)strCOM.szCAFileName), SQLITE_STATIC);
    /* szClientFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientFileName, strlen((char*)strCOM.szClientFileName), SQLITE_STATIC);
    /* szClientKeyFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientKeyFileName, strlen((char*)strCOM.szClientKeyFileName), SQLITE_STATIC);
    /* szPriPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriPhoneNum, strlen((char*)strCOM.szPriPhoneNum), SQLITE_STATIC);
    /* szSecPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecPhoneNum, strlen((char*)strCOM.szSecPhoneNum), SQLITE_STATIC);
    /* szPriHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriHostIP, strlen((char*)strCOM.szPriHostIP), SQLITE_STATIC);
    /* ulPriHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulPriHostPort);
    /* szSecHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecHostIP, strlen((char*)strCOM.szSecHostIP), SQLITE_STATIC);
    /* ulSecHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulSecHostPort);
	/* bDialBackUp*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bDialBackUp);
	/* bPredialFlag*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bPredialFlag);
	/* inParaMode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inParaMode);
	/* inHandShake*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHandShake);
	/* inCountryCode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCountryCode);	
    /* szATCMD1*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD1, strlen((char*)strCOM.szATCMD1), SQLITE_STATIC);	
    /* szATCMD2*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD2, strlen((char*)strCOM.szATCMD2), SQLITE_STATIC);
	/* szATCMD3*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD3, strlen((char*)strCOM.szATCMD3), SQLITE_STATIC);
	/* szATCMD4*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD4, strlen((char*)strCOM.szATCMD4), SQLITE_STATIC);
    /* szATCMD5*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD5, strlen((char*)strCOM.szATCMD5), SQLITE_STATIC);
	/* inConnectionTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inConnectionTimeOut);
	/* inReceiveTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveTimeOut);
	/* inGPRSSingal*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inGPRSSingal);


    /* inSendLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inSendLen);
	/* szSendData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szSendData, strCOM.inSendLen, SQLITE_STATIC);

    /* inReceiveLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveLen);
	/* szReceiveData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szReceiveData, strCOM.inReceiveLen, SQLITE_STATIC);


    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    result = sqlite3_step(stmt);
    if( result != SQLITE_DONE ){
        sqlite3_close(db);
        return 3;
    }
    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);
    //???sqlite????
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return(d_OK);
}




int inMultiAP_Database_COM_Clear(void)
{
    int inSeekCnt = 1;
    int result;
    //char *sql = "UPDATE COM SET inCommMode = ? ,inHeaderFormat = ? ,bSSLFlag = ? ,szCAFileName = ? ,szClientFileName = ? ,szClientKeyFileName = ? ,szPriPhoneNum = ? ,szSecPhoneNum = ? ,szPriHostIP = ? ,ulPriHostPort = ? ,szSecHostIP = ? ,ulSecHostPort = ? ,inSendLen = ? ,szSendData = ? ,inReceiveLen = ?,szReceiveData = ? WHERE  COMid = ?";          
 	char *sql = "UPDATE COM SET inCommMode = ? ,inHeaderFormat = ? ,bSSLFlag = ? ,szCAFileName = ? ,szClientFileName = ? ,szClientKeyFileName = ? ,szPriPhoneNum = ? ,szSecPhoneNum = ? ,szPriHostIP = ? ,ulPriHostPort = ? ,szSecHostIP = ? ,ulSecHostPort = ? ,bDialBackUp = ? ,bPredialFlag = ? ,inParaMode = ? ,inHandShake = ? ,inCountryCode = ? ,szATCMD1 = ? ,szATCMD2 = ? ,szATCMD3 = ? ,szATCMD4 = ? ,szATCMD5 = ? ,inConnectionTimeOut = ? ,inReceiveTimeOut = ? ,inGPRSSingal = ? ,inSendLen = ? ,szSendData = ? ,inReceiveLen = ?,szReceiveData = ? WHERE  COMid = ?";		  

	/* open the database */
    result = sqlite3_open(DB_COM,&db);
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

    memset(&strCOM,0,sizeof(strCOM));
    
    /* inCommMode*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCommMode);
    /* inHeaderFormat*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHeaderFormat);
    /* bSSLFlag*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bSSLFlag);
    /* szCAFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szCAFileName, strlen((char*)strCOM.szCAFileName), SQLITE_STATIC);
    /* szClientFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientFileName, strlen((char*)strCOM.szClientFileName), SQLITE_STATIC);
    /* szClientKeyFileName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szClientKeyFileName, strlen((char*)strCOM.szClientKeyFileName), SQLITE_STATIC);
    /* szPriPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriPhoneNum, strlen((char*)strCOM.szPriPhoneNum), SQLITE_STATIC);
    /* szSecPhoneNum*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecPhoneNum, strlen((char*)strCOM.szSecPhoneNum), SQLITE_STATIC);
    /* szPriHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szPriHostIP, strlen((char*)strCOM.szPriHostIP), SQLITE_STATIC);
    /* ulPriHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulPriHostPort);
    /* szSecHostIP*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szSecHostIP, strlen((char*)strCOM.szSecHostIP), SQLITE_STATIC);
    /* ulSecHostPort*/
    result = sqlite3_bind_double(stmt, inStmtSeq +=1, strCOM.ulSecHostPort);
	/* bDialBackUp*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bDialBackUp);
	/* bPredialFlag*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.bPredialFlag);
	/* inParaMode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inParaMode);
	/* inHandShake*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inHandShake);
	/* inCountryCode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inCountryCode);	
    /* szATCMD1*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD1, strlen((char*)strCOM.szATCMD1), SQLITE_STATIC);	
    /* szATCMD2*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD2, strlen((char*)strCOM.szATCMD2), SQLITE_STATIC);
	/* szATCMD3*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD3, strlen((char*)strCOM.szATCMD3), SQLITE_STATIC);
	/* szATCMD4*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD4, strlen((char*)strCOM.szATCMD4), SQLITE_STATIC);
    /* szATCMD5*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCOM.szATCMD5, strlen((char*)strCOM.szATCMD5), SQLITE_STATIC);
	/* inConnectionTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inConnectionTimeOut);
	/* inReceiveTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveTimeOut);
	/* inGPRSSingal*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inGPRSSingal);

    /* inSendLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inSendLen);
	/* szSendData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szSendData, strCOM.inSendLen, SQLITE_STATIC);

    /* inReceiveLen*/
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCOM.inReceiveLen);
	/* szReceiveData*/
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCOM.szReceiveData, strCOM.inReceiveLen, SQLITE_STATIC);


    result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
    result = sqlite3_step(stmt);
    if( result != SQLITE_DONE ){
        sqlite3_close(db);
        return 3;
    }
    
    sqlite3_exec(db,"commit;",NULL,NULL,NULL);
    //???sqlite????
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    vdDebug_LogPrintf("---usCTOSS_COMM_SAVE");

    return(d_OK);
}
//end for Sharls_COM modem
#endif

int inERMAP_Database_BatchDelete(void)
{
	int result;
	char *sql = "DELETE FROM TransData";	
	int inDBResult =0 ;

	/* open the database */
	result = sqlite3_open(DB_ERM,&db);
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

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inERMAP_Database_BatchRead(ERM_TransData *strERMTransData)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3 FROM TransData WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_ERM,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			strERMTransData->shType = sqlite3_column_int(stmt,inStmtSeq);
			strcpy((char*)strERMTransData->szTPDU, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szPAN, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szAmt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szSTAN, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szTime, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szDate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szExpDate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szRefNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szApprCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szTerminalSerialNO, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));			
			strcpy((char*)strERMTransData->szBankCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szMerchantCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szStoreCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szPaymentType, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szPaymentMedia, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szBatchNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szReceiptImageFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szInvNum, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szLogoFileName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
			strcpy((char*)strERMTransData->szSingatureStatus, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szRSAKEKVersion, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szReceiptLogoIndex, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fGzipReceiptImage = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fSettle = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fReserved1 = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fReserved2 = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strERMTransData->fReserved3 = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));			
			strcpy((char*)strERMTransData->szReserved1, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szReserved2, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            strcpy((char*)strERMTransData->szReserved3, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

//INSERT INTO TransData (TransDataid, shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3) VALUES (1, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1)
int inERMAP_Database_BatchInsert(ERM_TransData *strERMTransData)
{
	int result;	
 	char *sql = "INSERT INTO TransData (TransDataid, shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3) VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	   
	/* open the database */
	result = sqlite3_open(DB_ERM,&db);
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
	//vdCTOSS_GetMemoryStatus("bagin Database");

	inStmtSeq = 0;
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->shType);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTPDU, strlen((char*)strERMTransData->szTPDU), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szPAN, strlen((char*)strERMTransData->szPAN), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szAmt, strlen((char*)strERMTransData->szAmt), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szSTAN, strlen((char*)strERMTransData->szSTAN), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTime, strlen((char*)strERMTransData->szTime), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szDate, strlen((char*)strERMTransData->szDate), SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szExpDate, strlen((char*)strERMTransData->szExpDate), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szRefNum, strlen((char*)strERMTransData->szRefNum), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szApprCode, strlen((char*)strERMTransData->szApprCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTID, strlen((char*)strERMTransData->szTID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szMID, strlen((char*)strERMTransData->szMID), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szTerminalSerialNO, strlen((char*)strERMTransData->szTerminalSerialNO), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szBankCode, strlen((char*)strERMTransData->szBankCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szMerchantCode, strlen((char*)strERMTransData->szMerchantCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szStoreCode, strlen((char*)strERMTransData->szStoreCode), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szPaymentType, strlen((char*)strERMTransData->szPaymentType), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szPaymentMedia, strlen((char*)strERMTransData->szPaymentMedia), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szBatchNum, strlen((char*)strERMTransData->szBatchNum), SQLITE_STATIC);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReceiptImageFileName, strlen((char*)strERMTransData->szReceiptImageFileName), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szInvNum, strlen((char*)strERMTransData->szInvNum), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szLogoFileName, strlen((char*)strERMTransData->szLogoFileName), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szSingatureStatus, strlen((char*)strERMTransData->szSingatureStatus), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szRSAKEKVersion, strlen((char*)strERMTransData->szRSAKEKVersion), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReceiptLogoIndex, strlen((char*)strERMTransData->szReceiptLogoIndex), SQLITE_STATIC); 
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fGzipReceiptImage);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fSettle);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fReserved1);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fReserved3);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strERMTransData->fReserved3);
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReserved1, strlen((char*)strERMTransData->szReserved1), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReserved2, strlen((char*)strERMTransData->szReserved2), SQLITE_STATIC); 
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strERMTransData->szReserved3, strlen((char*)strERMTransData->szReserved3), SQLITE_STATIC); 
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inHDTReadByApname(char *szAPName)
{
	int len;
	CTOS_RTC SetRTC;     
	int result;
	int inResult = d_NO;
	int inHostIndex = 0;
	char *sql = "SELECT inHostIndex FROM HDT WHERE szAPName = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)szAPName, strlen(szAPName), SQLITE_STATIC);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inHostIndex*/
			inHostIndex = sqlite3_column_int(stmt, inStmtSeq );

		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inHostIndex);
}


int inMultiAP_Database_CTLS_Delete(void)
{
	int result;
	char *sql = "DELETE FROM TransData";	
	int inDBResult =0 ;

	/* open the database */
	result = sqlite3_open(DB_CTLS,&db);
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

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch delet,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inMultiAP_Database_CTLS_Read(CTLS_TransData *strCTLSTransData)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT szAmount, szOtherAmt, szTransType, szCatgCode, szCurrCode, status, bSID, baDateTime, bTrack1Len, baTrack1Data, bTrack2Len, baTrack2Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, usTransResult, bCVMAnalysis, baCVMResults, bVisaAOSAPresent, baVisaAOSA, bODAFail, inReserved1, inReserved2, inReserved3, szReserved1, szReserved2, szReserved3 FROM TransData WHERE TransDataid = ?";

	/* open the database */
	result = sqlite3_open(DB_CTLS,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			strcpy((char*)strCTLSTransData->szAmount, (char *)sqlite3_column_text(stmt,inStmtSeq));
			strcpy((char*)strCTLSTransData->szOtherAmt, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)strCTLSTransData->szTransType, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 )); 
			strcpy((char*)strCTLSTransData->szCatgCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strcpy((char*)strCTLSTransData->szCurrCode, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strCTLSTransData->status = sqlite3_column_int(stmt,inStmtSeq +=1 );
			strCTLSTransData->bSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			strcpy((char*)strCTLSTransData->baDateTime, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			strCTLSTransData->bTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(strCTLSTransData->baTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 100);
			strCTLSTransData->bTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(strCTLSTransData->baTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 100);
			strCTLSTransData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(strCTLSTransData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			strCTLSTransData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(strCTLSTransData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			strCTLSTransData->usTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			strCTLSTransData->bCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(strCTLSTransData->baCVMResults, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
			strCTLSTransData->bVisaAOSAPresent = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(strCTLSTransData->baVisaAOSA, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			strCTLSTransData->bODAFail = sqlite3_column_int(stmt,inStmtSeq +=1 );
			strCTLSTransData->inReserved1 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			strCTLSTransData->inReserved2 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			strCTLSTransData->inReserved3 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(strCTLSTransData->szReserved1, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 100);
			memcpy(strCTLSTransData->szReserved2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 100);
			memcpy(strCTLSTransData->szReserved3, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 100);
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}


int inMultiAP_Database_CTLS_Insert(CTLS_TransData *strCTLSTransData)
{
	int result; 
	char *sql = "INSERT INTO TransData (TransDataid, szAmount, szOtherAmt, szTransType, szCatgCode, szCurrCode, status, bSID, baDateTime, bTrack1Len, baTrack1Data, bTrack2Len, baTrack2Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, usTransResult, bCVMAnalysis, baCVMResults, bVisaAOSAPresent, baVisaAOSA, bODAFail, inReserved1, inReserved2, inReserved3, szReserved1, szReserved2, szReserved3) VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	   
	/* open the database */
	result = sqlite3_open(DB_CTLS,&db);
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
	//vdCTOSS_GetMemoryStatus("bagin Database");

	inStmtSeq = 0;
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCTLSTransData->szAmount, strlen((char*)strCTLSTransData->szAmount), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCTLSTransData->szOtherAmt, strlen((char*)strCTLSTransData->szOtherAmt), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCTLSTransData->szTransType, strlen((char*)strCTLSTransData->szTransType), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCTLSTransData->szCatgCode, strlen((char*)strCTLSTransData->szCatgCode), SQLITE_STATIC); 
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCTLSTransData->szCurrCode, strlen((char*)strCTLSTransData->szCurrCode), SQLITE_STATIC); 
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->status);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->bSID);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strCTLSTransData->baDateTime, strlen((char*)strCTLSTransData->baDateTime), SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->bTrack1Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->baTrack1Data, 100, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->bTrack2Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->baTrack2Data, 100, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->baAdditionalData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->usTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->bCVMAnalysis);	
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->baCVMResults, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->bVisaAOSAPresent);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->baVisaAOSA, 6, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->bODAFail);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->inReserved1);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->inReserved2);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strCTLSTransData->inReserved3);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->szReserved1, 100, SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->szReserved2, 100, SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strCTLSTransData->szReserved3, 100, SQLITE_STATIC); 
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inEFTReadNumofRecordsByHostId(int *inFindRecordNum, int inHostId)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT HDTid, inEFTEnable, inKeySet, inKeyIndex, inSHA1MAC, szEncryptMode, szEncrypBitmap, szAcquirerID, szVendorID, szEFTNII, szEFTVersion FROM EFT WHERE inEFTEnable = 1 AND HDTid = ? GROUP BY szAcquirerID,szVendorID";
	int incount = 0;
		
	/* open the database */
	result = sqlite3_open(DB_EFT_LIB,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /* inHDTid */
			strEFT[incount].inHDTid = sqlite3_column_int(stmt,inStmtSeq  );

            /* inEFTEnable */
			strEFT[incount].inEFTEnable = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* inKeySet */
			strEFT[incount].inKeySet = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* inKeyIndex */
			strEFT[incount].inKeyIndex = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* inSHA1MAC */
			strEFT[incount].inSHA1MAC = sqlite3_column_int(stmt,inStmtSeq +=1 );
            
			/* szEncryptMode */			
			strcpy((char*)strEFT[incount].szEncryptMode, (char *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

            /*szEncrypBitmap*/
			strcpy((char*)strEFT[incount].szEncrypBitmap, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szAcquirerID*/
			strcpy((char*)strEFT[incount].szAcquirerID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szVendorID*/
			strcpy((char*)strEFT[incount].szVendorID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szEFTNII*/
			strcpy((char*)strEFT[incount].szEFTNII, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szEFTVersion*/
			strcpy((char*)strEFT[incount].szEFTVersion, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    incount ++;
			
		}
	} while (result == SQLITE_ROW);

	*inFindRecordNum = incount;
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inEFTReadNumofRecords(int *inFindRecordNum)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT HDTid, inEFTEnable, inKeySet, inKeyIndex, inSHA1MAC, szEncryptMode, szEncrypBitmap, szAcquirerID, szVendorID, szEFTNII, szEFTVersion FROM EFT WHERE inEFTEnable = 0 GROUP BY szAcquirerID,szVendorID";
	int incount = 0;
		
	/* open the database */
	result = sqlite3_open(DB_EFT_LIB,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /* inHDTid */
			strEFT[incount].inHDTid = sqlite3_column_int(stmt,inStmtSeq  );

            /* inEFTEnable */
			strEFT[incount].inEFTEnable = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* inKeySet */
			strEFT[incount].inKeySet = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* inKeyIndex */
			strEFT[incount].inKeyIndex = sqlite3_column_int(stmt,inStmtSeq +=1 );

            /* inSHA1MAC */
			strEFT[incount].inSHA1MAC = sqlite3_column_int(stmt,inStmtSeq +=1 );
            
			/* szEncryptMode */			
			strcpy((char*)strEFT[incount].szEncryptMode, (char *)sqlite3_column_text(stmt, inStmtSeq +=1 ));

            /*szEncrypBitmap*/
			strcpy((char*)strEFT[incount].szEncrypBitmap, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szAcquirerID*/
			strcpy((char*)strEFT[incount].szAcquirerID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szVendorID*/
			strcpy((char*)strEFT[incount].szVendorID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

            /*szEFTNII*/
			strcpy((char*)strEFT[incount].szEFTNII, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szEFTVersion*/
			strcpy((char*)strEFT[incount].szEFTVersion, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    incount ++;
			
		}
	} while (result == SQLITE_ROW);

	*inFindRecordNum = incount;
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


/*For TMS, read all MMT records but only need few data*/
STRUCT_MMT      strMMTRec;

int inMMTReadSelectedData(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT MITid, HDTid, szHostName, szMerchantName, szTID, szMID, szBatchNo, fMMTEnable FROM MMT WHERE MMTid = ?";
	int incount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	
	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
			
            /* MITid */
			strMMTRec.MITid = sqlite3_column_int(stmt,inStmtSeq);
			            
			/* HDTid */
			strMMTRec.HDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			
			/*szHostName*/
			strcpy((char*)strMMTRec.szHostName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/* szMerchantName */			
			strcpy((char*)strMMTRec.szMerchantName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szTID*/
			strcpy((char*)strMMTRec.szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*szMID*/
			strcpy((char*)strMMTRec.szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
            
			/*szBatchNo*/
			memcpy(strMMTRec.szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);

            /* fMMTEnable */
			strMMTRec.fMMTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			//incount++;
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);
    
    return(inResult);
}

/*add for modem PPP*/
int inMPTRead(int inSeekCnt)
{     
	int result;
	char *sql = "SELECT HDTid, szHostName, inCommunicationMode, szPriISPPhoneNumber, szSecISPPhoneNumber, inHandShake, inParaMode, inCountryCode, inDialHandShake, inMCarrierTimeOut, inMRespTimeOut, szUserName, szPassword, inTxRxBlkSize FROM MPT WHERE MPTid = ?";
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	result = sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* HDTid */
			strMPT.HDTid = sqlite3_column_int(stmt, inStmtSeq );   
                        
			/* szHostName*/
			strcpy((char*)strMPT.szHostName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
           
			/* inCommunicationMode */
			strMPT.inCommunicationMode = sqlite3_column_int(stmt, inStmtSeq +=1 );   

			/* szPriISPPhoneNumber; */
			strcpy((char*)strMPT.szPriISPPhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szSecISPPhoneNumber*/
			strcpy((char*)strMPT.szSecISPPhoneNumber, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/*inHandShake*/
			strMPT.inHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );
			
			/*inParaMode*/
			strMPT.inParaMode = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/*inCountryCode*/
			strMPT.inCountryCode = sqlite3_column_int(stmt, inStmtSeq +=1 );
			
			/* inDialHandShake*/
			strMPT.inDialHandShake = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* inMCarrierTimeOut*/
			strMPT.inMCarrierTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* inMRespTimeOut*/
			strMPT.inMRespTimeOut = sqlite3_column_int(stmt, inStmtSeq +=1 );

			/* szUserName; */
			strcpy((char*)strMPT.szUserName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* szPassword*/
			strcpy((char*)strMPT.szPassword, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* inTxRxBlkSize*/
			strMPT.inTxRxBlkSize = sqlite3_column_int(stmt, inStmtSeq +=1 );    
   
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}


int inMPTSave(int inSeekCnt)
{
	int result;
	char *sql = "UPDATE MPT SET HDTid = ?, szHostName = ?, inCommunicationMode = ?, szPriISPPhoneNumber = ?, szSecISPPhoneNumber = ?, inHandShake = ?, inParaMode = ?, inCountryCode = ?, inDialHandShake = ?, inMCarrierTimeOut = ?, inMRespTimeOut = ?, szUserName = ?, szPassword = ?, inTxRxBlkSize = ? WHERE  MPTid = ?";

 	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
        
	/* HDTid*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.HDTid);
	
	/* szHostName*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMPT.szHostName, strlen((char*)strMPT.szHostName), SQLITE_STATIC);

	/* inCommunicationMode*/	
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inCommunicationMode);

	/* szPriISPPhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMPT.szPriISPPhoneNumber, strlen((char*)strMPT.szPriISPPhoneNumber), SQLITE_STATIC);

	/* szSecISPPhoneNumber*/
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMPT.szSecISPPhoneNumber, strlen((char*)strMPT.szPriISPPhoneNumber), SQLITE_STATIC);

	/* inHandShake*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inHandShake);
		
	/* inParaMode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inParaMode);

	/* inCountryCode*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inCountryCode);
	
	/* inDialHandShake*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inDialHandShake);       
       
	/* inMCarrierTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inMCarrierTimeOut);       
               
	/* inMRespTimeOut*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inMRespTimeOut);
    
    /* szUserName*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMPT.szUserName, strlen((char*)strMPT.szUserName), SQLITE_STATIC);

	/* szPassword*/
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strMPT.szPassword, strlen((char*)strMPT.szPassword), SQLITE_STATIC);

	/* inTxRxBlkSize*/
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, strMPT.inTxRxBlkSize);

	
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

int inDatabase_WaveDelete(char *szTableName, char *szTag)
{
	int result;
	//char *sql = "DELETE FROM TransData WHERE (szHostLabel = ? AND szBatchNo = ? AND MITid = ?)";	
	int inDBResult = 0;
	char sql[128];






	
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "DELETE FROM %s WHERE szTag = '%s'",szTableName,szTag);
	vdDebug_LogPrintf("inDatabase_WaveDelete,sql=[%s]",sql);
	/* open the database */
	result = sqlite3_open(DB_WAVE,&db);
	vdDebug_LogPrintf("sqlite3_open,result=[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	vdDebug_LogPrintf("sqlite3_prepare_v2,result=[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;

	result = sqlite3_step(stmt);
	vdDebug_LogPrintf("sqlite3_step,result=[%d]",result);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch dele,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}


int inDatabase_WaveUpdate(char *szTableName, char *szTag, char *szTagValue)
{
	int result;	
 	//char *sql = "INSERT INTO TransData (TransDataid, shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3) VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	char sql[128];

	
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "UPDATE %s SET szValue = '%s' WHERE szTag = '%s'",szTableName, szTagValue, szTag);
	vdDebug_LogPrintf("inDatabase_WaveUpdate,sql=[%s]",sql);
	
	/* open the database */
	result = sqlite3_open(DB_WAVE,&db);
	vdDebug_LogPrintf("sqlite3_open,result=[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	vdDebug_LogPrintf("sqlite3_prepare_v2,result=[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	//vdCTOSS_GetMemoryStatus("bagin Database");

	inStmtSeq = 0;

	result = sqlite3_step(stmt);
	vdDebug_LogPrintf("sqlite3_step,result=[%d]",result);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}


int inDatabase_WaveInsert(char *szTableName, char *szTag, char *szTagValue)
{
	int result;	
 	//char *sql = "INSERT INTO TransData (TransDataid, shType, szTPDU, szPAN, szAmt, szSTAN, szTime, szDate, szExpDate, szRefNum, szApprCode, szTID, szMID, szTerminalSerialNO, szBankCode, szMerchantCode, szStoreCode, szPaymentType, szPaymentMedia, szBatchNum, szReceiptImageFileName, szInvNum, szLogoFileName, szSingatureStatus, szRSAKEKVersion, szReceiptLogoIndex, fGzipReceiptImage, fSettle, fReserved1, fReserved2, fReserved3, szReserved1, szReserved2, szReserved3) VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	char sql[128];

	
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "INSERT INTO %s (szTag,szValue) VALUES ('%s','%s')",szTableName,szTag,szTagValue);
	vdDebug_LogPrintf("inDatabase_WaveInsert,sql=[%s]",sql);
	
	/* open the database */
	result = sqlite3_open(DB_WAVE,&db);
	vdDebug_LogPrintf("sqlite3_open,result=[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	vdDebug_LogPrintf("sqlite3_prepare_v2,result=[%d]",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	//vdCTOSS_GetMemoryStatus("bagin Database");

	inStmtSeq = 0;

	result = sqlite3_step(stmt);
	vdDebug_LogPrintf("sqlite3_step,result=[%d]",result);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}



int inTCTUpdateMenuid(int inSeekCnt, int inMenuid)
{
	int result;
	char *sql = "UPDATE TCT SET inMenuid = ? WHERE TCTid = ?";

	vdDebug_LogPrintf("[inTCTUpdateMenuid]-start ");
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);	
	vdDebug_LogPrintf("[inTCTUpdateMenuid]result[%d]", result);
	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}


	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);   
	vdDebug_LogPrintf("[inTCTUpdateMenuid]result[%d]", result);

	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;

	/* inMenuid */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inMenuid);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	result = sqlite3_step(stmt);

	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	
	vdDebug_LogPrintf("[inTCTUpdateMenuid]result[%d]", result);
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	result = sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("[inTCTUpdateMenuid]result[%d]", result);

    return(d_OK);
}


int inTCTGetCurrMenuid(int inSeekCnt, int *inMenuID)
{
	int result;
	int len = 0;
	int inResult = -1;
	
 	char *sql = "SELECT inMenuid FROM TCT WHERE TCTid = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	//vdDebug_LogPrintf("sqlite3_open =[%d],",result);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	//vdDebug_LogPrintf("sqlite3_prepare_v2 =[%d],",result);
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
			
            /* inMenuid */
			*inMenuID = sqlite3_column_int(stmt,inStmtSeq);
        }
	} while (result == SQLITE_ROW);	

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

//@@IBR ADD 05102016
int inHDTMAX(void)
{
	int result;	
	char *sql = "SELECT MAX(HDTid) FROM HDT WHERE fHostEnable = 1";
	int inMaxValue = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			inMaxValue = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);
    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("inCDTRead,inDBResult[%d]result[%d]",inDBResult,result);
	inDBResult = sqlite3_finalize(stmt);
	
	vdDebug_LogPrintf("inCDTRead,sqlite3_finalize[%d]",inDBResult);
	inDBResult = sqlite3_close(db);
	vdDebug_LogPrintf("inCDTRead,sqlite3_close[%d]",inDBResult);

	return(inMaxValue);
}

//@@IBR ADD 10102016
int inCDTReadbyHost(int inSeekCnt, int *inMinCnt, int *inMaxCnt){
    int result;	
	char *sql = "SELECT MIN(CDTid),MAX(CDTid) FROM CDT WHERE HDTid = ?";
        int inMinValue = 0;
	int inMaxValue = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
                        inMinValue = sqlite3_column_int(stmt,inStmtSeq);
			inMaxValue = sqlite3_column_int(stmt,inStmtSeq += 1);
		}
	} while (result == SQLITE_ROW);
    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("inCDTRead,inDBResult[%d]result[%d]",inDBResult,result);
	inDBResult = sqlite3_finalize(stmt);
	
	vdDebug_LogPrintf("inCDTRead,sqlite3_finalize[%d]",inDBResult);
	inDBResult = sqlite3_close(db);
	vdDebug_LogPrintf("inCDTRead,sqlite3_close[%d]",inDBResult);
        
        *inMinCnt = inMinValue;
        *inMaxCnt = inMaxValue;
        
	return inDBResult;
    
}

int inCDTReadDisabled(int inSeekCnt, int inHDTid)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE CDTid = ? AND HDTid = ? AND fCDTEnable = ?";

	int inDBResult = 0;	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTRead,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
        sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 0);//fCDTEnable = 0

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strCDT.szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strCDT.szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strCDT.szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strCDT.inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strCDT.inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strCDT.inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strCDT.inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strCDT.InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strCDT.fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strCDT.fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strCDT.fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strCDT.fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strCDT.fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strCDT.fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strCDT.fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strCDT.IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strCDT.HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);	

		/* fMagSwipeEnable */
		strCDT.fMagSwipeEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/* fPreAuthAllowed */
			strCDT.fPreAuthAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fRefundAllowed*/
		strCDT.fRefundAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


		/*fCTLSAllowed*/
		strCDT.fCTLSAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fAddRefRRNAPP*/
		strCDT.fAddRefRRNAPP = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		}
	} while (result == SQLITE_ROW);
   
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


int inHDTCurrencyUpdate(int inCurrency)
{
	int result;
	char *sql = "UPDATE HDT SET inCurrencyIdx = ?";

	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}

	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
	inStmtSeq = 0;
		
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inCurrency);

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




/*for muliple application menu management*/
int inMAPMRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inDefAppMenuID, inSubAppMenuID FROM MAPM WHERE MAPMid = ?";
	
	vdDebug_LogPrintf("=====inMAPMRead=====");
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/*Apps Describletion*/
			//strcpy((char*)strMAPM.szAppsDes, (char *)sqlite3_column_text(stmt,inStmtSeq));
			
			/* Defualt App Menu ID */
			strMAPM.inDefAppMenuID = sqlite3_column_int(stmt,inStmtSeq);

			/* Sub App Menu ID */
			strMAPM.inSubAppMenuID = sqlite3_column_int(stmt,inStmtSeq + 1);
			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	vdDebug_LogPrintf("strMAPM.inDefAppMenuID[%d]", strMAPM.inDefAppMenuID);
	
    return(inResult);
}


int inIPSRead(int inSeekCnt)
{
	int len;
	CTOS_RTC SetRTC;     
	int result;
	int inResult = d_NO;
	char *sql = "SELECT inInstallmentPlan, szSchemeID FROM IPS WHERE IPSid = ? AND inInstallmentPlan > 0";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

            /*inInstallmentPlan*/
            strIPS.inInstallmentPlan = sqlite3_column_int(stmt, inStmtSeq );

            /* szSchemeID */
            strcpy((char*)strIPS.szSchemeID, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));

		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inIPSMAX(void)
{
	int result;	
	char *sql = "SELECT MAX(IPSid) FROM IPS WHERE inInstallmentPlan > 0";
	int inMaxValue = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			inMaxValue = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);
    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("inCDTRead,inDBResult[%d]result[%d]",inDBResult,result);
	inDBResult = sqlite3_finalize(stmt);
	
	vdDebug_LogPrintf("inCDTRead,sqlite3_finalize[%d]",inDBResult);
	inDBResult = sqlite3_close(db);
	vdDebug_LogPrintf("inCDTRead,sqlite3_close[%d]",inDBResult);

	return(inMaxValue);
}

int inHDTCheckByHostName(char *szHostName)
{
	int result;
	int inResult = d_NO;
        char *sql = "SELECT inHostIndex FROM HDT WHERE szHostLabel = ? AND fHostEnable = ?";
	int inDBResult = 0;
	char szHostLabel[50];
        int inHostIndex = 0;
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTRead,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}
        
        memset(szHostLabel, 0x00, sizeof(szHostLabel));
        strcpy(szHostLabel, szHostName);
	inStmtSeq = 0;
        
        sqlite3_bind_text(stmt, inStmtSeq += 1, (char *)szHostLabel, strlen(szHostLabel), SQLITE_STATIC);
        sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
                        
                        inHostIndex = sqlite3_column_int(stmt, inStmtSeq );
		}
	} while (result == SQLITE_ROW);
        
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inHostIndex);
}

int inFLGRead(int inSeekCnt)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT fRetryTranStatEnq, fConfirmQRCode FROM FLG WHERE FLGid = ?";
		
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			strFLG.fRetryTranStatEnq = sqlite3_column_int(stmt,inStmtSeq);
			strFLG.fConfirmQRCode = sqlite3_column_int(stmt,inStmtSeq+1);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inDB_DeleteConfRecOne(char *szDBName, char *szTabName, int inRecNum)

{
	int result;
	char sql[512];	
	int inDBResult = 0;

	char szCondStr[512];

	if (NULL == szDBName || NULL == szTabName)
		return -1;

	memset(sql, 0x00, sizeof(sql));
	strcpy(sql, "DELETE FROM ");

	/*from which table*/
	strcat(sql, szTabName);

	memset(szCondStr, 0x00, sizeof(szCondStr));
	sprintf(szCondStr, " WHERE %sid = ?", szTabName);

	/*final SQL*/
	strcat(sql, szCondStr);
	
	/* open the database */
	result = sqlite3_open(szDBName,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, inRecNum);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("table record delete,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);


	return(ST_SUCCESS);
}


int inDB_GetTableAllRecid(char *pszDBName, char *pszTblName, int inRecID[])
{
	int inRet;	
	char szSQL[512];
	int inRow = 0;
    int inColumn = 0;
	char **azResult;
	char *zErrMsg = NULL;

	int inTotalNum = 0;

	//vdDebug_LogPrintf("=====inDB_GetTableAllRecid=====");

	if (NULL == pszDBName || NULL == pszTblName)
		return -1;

	memset(szSQL, 0x00, sizeof(szSQL));
	sprintf(szSQL, "SELECT %sid FROM %s", pszTblName, pszTblName);
	
	/* open the database */
	inRet = sqlite3_open(pszDBName,&db);
	if (inRet != SQLITE_OK) {
		sqlite3_close(db);
		return -1;
	}

	//vdDebug_LogPrintf("szSQL[%s]", szSQL);
	
	sqlite3_exec( db, "begin;", 0, 0, NULL );

	inRet = sqlite3_get_table(db, szSQL, &azResult, &inRow, &inColumn, &zErrMsg);
	
	if (inRet != SQLITE_OK)
	{
		//vdDebug_LogPrintf("zErrMsg = %s \n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return -1;
	}

	int i = 0;
    //vdDebug_LogPrintf("inRow:%d inColumn=%d \n", inRow, inColumn);
    if ((inRow == 0) && (inColumn == 0))
	{
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
        return -1;
	}

	for( i=0 ; i<( inRow + 1 ) * inColumn ; i++ )
	{
  		//vdDebug_LogPrintf( "azResult[%d] [%s]", i, azResult[i]); 
		//strcpy(value, azResult[i]);
		if (i>0)
		{
			inRecID[i-1] = atoi(azResult[i]);
		}
	}

	/*Total must skip the first result which is string XXXid */
	inTotalNum = i-1;

 	sqlite3_free_table(azResult);
	
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_free(zErrMsg);
	sqlite3_close(db);

	return(inTotalNum);
}


int inDB_CreateTable(char *pszTabName, int inStructSize)
{
	int result;
	char sql[512];	
	int inDBResult = 0;

	vdDebug_LogPrintf("inDB_CreateTable[%s][%d]", pszTabName, inStructSize);

	if (strlen(pszTabName) <= 0 || inStructSize <= 0)
		return d_NO;
	
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "CREATE TABLE %s ([%sid] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, [stDataBuf] BLOB(%d) NOT NULL);", pszTabName, pszTabName, inStructSize);

	
	vdDebug_LogPrintf("sql[%s]", sql);
	
	/* open the database */
	result = sqlite3_open(DB_TMLTMP,&db);
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

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("table create,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);


	return(d_OK);
	
}


int inDB_ReadTableRecByRecid(char *pszTabName, int inSeekCnt, char *pstBuf, int inStructSize)
{
	int result;
	int inResult = d_NO;

	char sql[512];


	if (strlen(pszTabName) <= 0 || inSeekCnt < 0)
		return d_NO;
	
	memset(sql, 0x00, sizeof(sql));

	sprintf(sql, "SELECT stDataBuf FROM %s WHERE %sid = ?", pszTabName, pszTabName);
	/* open the database */
	result = sqlite3_open(DB_TMLTMP,&db);
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

	sqlite3_bind_int(stmt, 1, inSeekCnt);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			/* structure data buffer */
			memcpy(pstBuf, sqlite3_column_blob(stmt,inStmtSeq), inStructSize);

		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inDB_ReadTableRecBySeq(char *pszTabName, int inSeqNum, char *pstBuf, int inStructSize)
{
	int inNumRec = 0;
	
	int inTabRecID[500];
	int inSelectRecid = 0;

	int inRet = d_OK;

	
	vdDebug_LogPrintf("=====inDB_ReadTableRecBySeq=====[%s][%d]", pszTabName, inSeqNum);

	memset(inTabRecID, 0x00, sizeof(inTabRecID));

	/*get all records*/
	inNumRec = inDB_GetTableAllRecid(DB_TMLTMP, pszTabName, inTabRecID);
	if (inNumRec <= 0)
		return d_NO;

	/*get the frist records*/
	inSelectRecid = inTabRecID[inSeqNum-1];

	/*Read from table*/
	inRet = inDB_ReadTableRecByRecid(pszTabName, inSelectRecid, pstBuf, inStructSize);

	return d_OK;
}


int inDB_SaveTableRecByRecid(char *pszTabName, int inSeekCnt, char *pstBuf, int inStructSize)
{
	int result;

	char sql[512];

	memset(sql, 0x00, sizeof(sql));

	sprintf(sql, "UPDATE %s SET stDataBuf = ? WHERE %sid = ?;", pszTabName, pszTabName);

	/* open the database */
	result = sqlite3_open(DB_TMLTMP,&db);
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
	
	/* stDataBuf */
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, pstBuf, inStructSize, SQLITE_STATIC); 

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


int inDB_AppendTableRec(char *pszTabName, char *pstBuf, int inStructSize)
{
	int result;	
 	//char *sql = "INSERT INTO AQ01 (AQ01id, lnAQ01TotalTrans, lnAQ01TransCode, inAQ01CheckSum, szAQ01TID, szAQ01BatchNum, szAQ01CAN, szAQ01TransType, szAQ01TransHeader, szAQ01SignCert, szAQ01CounterData, szAQ01TRP, szAQ01LastBalance, szAQ01AfterBalance, szAQ01PurchFeeAmt, szAQ01MAC, szAQ01TransCntIndicator, szAQ01LastATUStatus, szAQ01DebitOption, szAQ01LastCreditTRP, szAQ01LastCreditHeader, szAQ01LastTxnTRP, szAQ01LastTxnRecord, szAQ01LastDebitOption, szAQ01LastTxnSignCert, szAQ01LastCntData, szAQ01LastBadDbtCnt, szAQ01AcqSamID, szAQ01BinIssuer) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	char sql[512];

	vdDebug_LogPrintf("=====inDB_AppendTableRec=====[%s] [%d]", pszTabName, inStructSize);

	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "INSERT INTO %s (%sid, stDatabuf) VALUES (NULL, ?)", pszTabName, pszTabName);
	
	/* open the database */
	result = sqlite3_open(DB_TMLTMP,&db);
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
	

	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, pstBuf, inStructSize, SQLITE_STATIC); 
		
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}


int inDB_SaveTableRecBySeq(char *pszTabName, int inSeqNum, char *pstBuf, int inStructSize)
{
	int inNumRec = 0;
	
	int inTabRecID[500];
	int inSelectRecid = 0;

	int inRet = d_NO;

	memset(inTabRecID, 0x00, sizeof(inTabRecID));

	vdDebug_LogPrintf("=====inDB_SaveTableRecBySeq=====[%s][%d]", pszTabName, inSeqNum);


	/*get all records*/
	inNumRec = inDB_GetTableAllRecid(DB_TMLTMP, pszTabName, inTabRecID);
	vdDebug_LogPrintf("=====inDB_GetTableAllRecid=====inNumRec[%d]", inNumRec);
	if (inNumRec <= 0)
	{
		inRet = inDB_AppendTableRec(pszTabName, pstBuf, inStructSize);
		return inRet;
	}

	/*get the frist records*/
	inSelectRecid = inTabRecID[inSeqNum-1];
	vdDebug_LogPrintf("inSelectRecid[%d]", inSelectRecid);
	if (inSelectRecid > 0)
	/*Read from table*/
	inRet = inDB_SaveTableRecByRecid(pszTabName, inSelectRecid, pstBuf, inStructSize);
	else
		inDB_AppendTableRec(pszTabName, pstBuf, inStructSize);

	return d_OK;
}


int inDB_DeleteTableRecByRecid(char *pszTabName, int inSeekCnt)
{
	int inRet = d_OK;
	
	inRet = inDB_DeleteConfRecOne(DB_TMLTMP, pszTabName, inSeekCnt);

	return inRet;
}


int inDB_DeleteTableRecByReq(char *pszTabName, int inSeqNum)
{
	int inNumRec = 0;
	
	int inTabRecID[500];
	int inSelectRecid = 0;

	int inRet = d_OK;

	memset(inTabRecID, 0x00, sizeof(inTabRecID));

	vdDebug_LogPrintf("=====inDB_DeleteTableRecByReq=====");

	/*get all records*/
	inNumRec = inDB_GetTableAllRecid(DB_TMLTMP, pszTabName, inTabRecID);
	if (inNumRec <= 0)
	{
		return d_OK;
	}

	/*get the frist records*/
	inSelectRecid = inTabRecID[inSeqNum];

	/*delete one rec from table*/
	inRet = inDB_DeleteTableRecByRecid(pszTabName, inSelectRecid);

	return d_OK;
}

int inDB_RemoveTable(char *pszTabName)
{
	int result;
	char sql[512];	
	int inDBResult = 0;

	vdDebug_LogPrintf("=====inDB_RemoveTable=====[%s]", pszTabName);

	if (strlen(pszTabName) <= 0)
		return d_NO;
	
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "DROP TABLE %s",pszTabName);
	
	
	/* open the database */
	result = sqlite3_open(DB_TMLTMP,&db);
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

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("table delete,sqlite3_exec[%d]",inDBResult);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
	
}



int inDB_CheckTableExist(char *pszTabName)
{

	int len;
	int result;
	char sql[512]; // from table
	int inCount = 0;

	vdDebug_LogPrintf("=====inDB_CheckTableExist=====[%s]", pszTabName);

	if (NULL == pszTabName)
		return 0;

	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='%s';", pszTabName);

	/* open the database */
	result = sqlite3_open(DB_TMLTMP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return -1;
	}
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return -2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return(inCount);

}


int inDB_GetTableTotalRecNum(char *pszTabName)

{
	int len;
	int result;
	//char *sql = "SELECT COUNT(*) FROM CBT";
	char sql[512];
	int inCount = 0;

	
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "SELECT COUNT(*) FROM %s;", pszTabName);
	
	/* open the database */
	result = sqlite3_open(DB_TMLTMP,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return -1;
	}
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return -2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return(inCount);
}

#if 1
int inDatabase_PreauthBatchInsert(TRANS_DATA_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM PreauthData";
// 	char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    char *sql = "INSERT INTO PreauthData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt, szAuthDate, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	BYTE szTempAuthDate[10+1];
	CTOS_RTC SetRTC;

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

    CTOS_RTCGet(&SetRTC);
	
    memset(szTempAuthDate, 0, sizeof(szTempAuthDate));
	sprintf(szTempAuthDate, "%04d-%02d-%02d", SetRTC.bYear+2000, SetRTC.bMonth, SetRTC.bDay);
	szTempAuthDate[10]=0;

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->HDTid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->MITid);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->CDTid);
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->IITid);
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTransType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPanLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szExpireDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEntryMode);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBaseAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTipAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOrgTransType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMacBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPINBlock, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRRN, 13, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgInvoiceNo, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPrintType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byAdjusted);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byUploaded);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCuploaded);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardholderName, 31, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szzAMEX4DBC, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szStoreID, 19, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szRespCode, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szServiceCode, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byContinueTrans);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byOffline);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byReversal);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byEMVFallBack);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->shTransResult);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTpdu, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szIsoField03, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szMassageType, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCardLable, 20, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack1Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack2Len);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTrack3Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack1Data, 86, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack2Data, 42, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTrack3Data, 65, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usChipDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baChipData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usAdditionalDataLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->baAdditionalData, 1024, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSID);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usWaveSTransResult);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->bWaveSCVMAnalysis);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulTraceNum);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulOrgTraceNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->usTerminalCommunicationMode);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPINEntryCapability);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byPackType);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrgAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szCVV2, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->inCardType);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byTCFailUpCnt);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byCardTypeNum);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.byEMVTransStatus);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5A, 10, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F2A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F30, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F34_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T82, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T84_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T84, 16, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T8A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T91, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T91Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T95, 5, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9A, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9C);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F02, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F03, 6, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F09, 2, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F10, 32, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1A, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F26, 8, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F27);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F33, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F34, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F35);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36_len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F36, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F37, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F41, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F53);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.ISR, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.ISRLen);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9B, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T5F24, 3, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T71Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T71, 258, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T72Len);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T72, 258, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F06, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F1E, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F28, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F29, 8, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.szChipLabel, 32, SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szTID, strlen((char*)transData->szTID), SQLITE_STATIC); 
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szMID, strlen((char*)transData->szMID), SQLITE_STATIC); 
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->szHostLabel, strlen((char*)transData->szHostLabel), SQLITE_STATIC); 
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szBatchNo, 3, SQLITE_STATIC);
        result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsInstallment);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szInstallmentTerms, strlen((char*)transData->stIPPinfo.szInstallmentTerms), SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szMonthlyAmt, 7, SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szInterestRate, strlen((char*)transData->stIPPinfo.szInterestRate), SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szTotalInterest, 7, SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szHandlingFee, 7, SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szIPPSchemeID, strlen((char*)transData->stIPPinfo.szIPPSchemeID), SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szFreqInstallment, strlen((char*)transData->stIPPinfo.szFreqInstallment), SQLITE_STATIC);
        result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)transData->stIPPinfo.szTransCurrency, strlen((char*)transData->stIPPinfo.szTransCurrency), SQLITE_STATIC);
        result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stIPPinfo.szIPPTotalAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fAlipay);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscounted);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szFixedAmount, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szOrigAmountDisc, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscountedFixAmt);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPercentage, 7, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->fIsDiscountedPercAmt);
	

    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)szTempAuthDate, strlen((char*)szTempAuthDate), SQLITE_STATIC); 
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63, 16, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F63_len);


	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E, 4, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F6E_len);


	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C, 32, SQLITE_STATIC);
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->stEMVinfo.T9F7C_len);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    vdDebug_LogPrintf("inDatabase_PreauthBatchInsert XXXXXXX - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	
    vdDebug_LogPrintf("inDatabase_PreauthBatchInsert szTempAuthDate: %s", szTempAuthDate);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

#else
int inDatabase_PreauthBatchInsert(TRANS_DATA_TABLE *transData)
{
	int result;	
	char *sql1 = "SELECT MAX(TransDataid) FROM PreauthData";
// 	char *sql = "INSERT INTO TransData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    //char *sql = "INSERT INTO PreauthData (TransDataid, HDTid, MITid, CDTid, IITid, szHostLabel, szBatchNo, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis, ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    char *sql = "INSERT INTO PreauthData (TransDataid, szYear, szDate, szTime, szAuthCode, szPAN, ulSavedIndex, szAuthDate, szTotalAmount, szInvoiceNo) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	BYTE szTempAuthDate[10+1];
	CTOS_RTC SetRTC;
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	sqlite3_exec( db, "begin", 0, 0, NULL );

	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

    CTOS_RTCGet(&SetRTC);
	
    memset(szTempAuthDate, 0, sizeof(szTempAuthDate));
	sprintf(szTempAuthDate, "%04d-%02d-%02d", SetRTC.bYear+2000, SetRTC.bMonth, SetRTC.bDay);
	szTempAuthDate[10]=0;

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			transData->ulSavedIndex = sqlite3_column_int(stmt,inStmtSeq);
			transData->ulSavedIndex += 1;
		}
	} while (result == SQLITE_ROW);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szYear, 2, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szDate, 3, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTime, 4, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szAuthCode, 7, SQLITE_STATIC);
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szPAN, 20, SQLITE_STATIC);
	result = sqlite3_bind_double(stmt, inStmtSeq +=1, transData->ulSavedIndex);

	vdDebug_LogPrintf("transData->szPAN: %s", transData->szPAN);
	vdDebug_LogPrintf("szTempAuthDate: %s", szTempAuthDate);
	
    result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)szTempAuthDate, strlen((char*)szTempAuthDate), SQLITE_STATIC); 
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szTotalAmount, 7, SQLITE_STATIC);

	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, transData->szInvoiceNo, 4, SQLITE_STATIC);
	
	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}
#endif
int inDatabase_PreauthBatchDeletePerAuthCode(char *strAuthCode)
{
	int result;
	char *sql = "DELETE FROM PreauthData WHERE szAuthCode = ?";	
	int inDBResult = 0;
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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

	vdDebug_LogPrintf("strAuthCode[%s]", strAuthCode);
	DebugAddHEX("strAuthCode", strAuthCode, 13);

	inStmtSeq = 0;
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strAuthCode, 7, SQLITE_STATIC);
	//sqlite3_bind_text(stmt, inStmtSeq += 1, (char *)strAuthCode, strlen(strAuthCode), SQLITE_STATIC);

	//inStmtSeq = 0;
	//result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)srTransRec.szHostLabel, strlen((char*)srTransRec.szHostLabel), SQLITE_STATIC); 
	//result = sqlite3_bind_blob(stmt, inStmtSeq +=1, srTransRec.szBatchNo, 3, SQLITE_STATIC);
	//result = sqlite3_bind_int(stmt, inStmtSeq +=1, srTransRec.MITid);


	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch dele,sqlite3_exec[%d]MITid[%d]",inDBResult,srTransRec.MITid);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inDatabase_PreauthBatchDeleteExpired(void)
{
	int result;
	int dategap = 0;
	char *sql;// = "DELETE FROM PreauthData WHERE (((strftime('%s','now') - strftime('%s',szAuthDate))/86400) >= 30)";	
	int inDBResult = 0;
	char szbuf1[100];
	char szbuf2[100];

	dategap = get_env_int("PREDELTGAP");
	if (dategap <= 0)
		dategap = 30;

	memset(szbuf1,0x00,sizeof(szbuf1));
	memset(szbuf2,0x00,sizeof(szbuf2));
	sprintf(szbuf2,"%d)",dategap);
	strcpy(szbuf1,"DELETE FROM PreauthData WHERE (((strftime('%s','now') - strftime('%s',szAuthDate))/86400) >= ");
	strcat(szbuf1,szbuf2);
	vdDebug_LogPrintf("inDatabase_PreauthBatchDeleteExpired=[%s]",szbuf1);
	sql = szbuf1;
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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

	//vdDebug_LogPrintf("strAuthCode[%s]", strAuthCode);
	//DebugAddHEX("strAuthCode", strAuthCode, 13);

	//inStmtSeq = 0;
	//result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strAuthCode, 7, SQLITE_STATIC);
	//sqlite3_bind_text(stmt, inStmtSeq += 1, (char *)strAuthCode, strlen(strAuthCode), SQLITE_STATIC);

	//inStmtSeq = 0;
	//result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)srTransRec.szHostLabel, strlen((char*)srTransRec.szHostLabel), SQLITE_STATIC); 
	//result = sqlite3_bind_blob(stmt, inStmtSeq +=1, srTransRec.szBatchNo, 3, SQLITE_STATIC);
	//result = sqlite3_bind_int(stmt, inStmtSeq +=1, srTransRec.MITid);


	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
	vdDebug_LogPrintf("Batch dele,sqlite3_exec[%d]MITid[%d]",inDBResult,srTransRec.MITid);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

#if 1
int inDatabase_BatchSearchByInvNo(TRANS_DATA_TABLE *transData, char *hexInvoiceNo)
{
	int result,i = 0;
	int inResult = d_NO;
//	char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM TransData WHERE szInvoiceNo = ?";
    char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay, fIsDiscounted, szFixedAmount, szOrigAmountDisc, fIsDiscountedFixAmt, szPercentage, fIsDiscountedPercAmt, T9F63, T9F63_LEN, T9F6E, T9F6E_len, T9F7C, T9F7C_len FROM PreauthData WHERE szInvoiceNo = ?";

	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, hexInvoiceNo, 4, SQLITE_STATIC);

	DebugAddINT("inDatabase_BatchSearchByInvNo sqlite3_bind_blob", result);
	DebugAddHEX("hexInvoiceNo", hexInvoiceNo, 3);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		DebugAddINT("if(100)=Find", result);
		DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			transData->HDTid = sqlite3_column_int(stmt,inStmtSeq);
			transData->MITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->CDTid = sqlite3_column_int(stmt,inStmtSeq +=1 );
            transData->IITid = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTransType = sqlite3_column_int(stmt,inStmtSeq+=1 );
			transData->byPanLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szExpireDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->byEntryMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szBaseAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szTipAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			transData->byOrgTransType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szMacBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szPINBlock, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);
			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szOrgTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szRRN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 13);						
			memcpy(transData->szInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szOrgInvoiceNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byPrintType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byVoided = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byAdjusted = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byUploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCuploaded = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szCardholderName, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 31);						
			memcpy(transData->szzAMEX4DBC, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->szStoreID, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 19);						
			memcpy(transData->szRespCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szServiceCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			transData->byContinueTrans = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byOffline = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byReversal = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byEMVFallBack = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->shTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTpdu, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szIsoField03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szMassageType, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
            memcpy(transData->szCardLable, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);
			transData->usTrack1Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack2Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usTrack3Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szTrack1Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 86);						
			memcpy(transData->szTrack2Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 42);						
			memcpy(transData->szTrack3Data, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 65);						
			transData->usChipDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baChipData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);
			transData->usAdditionalDataLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->baAdditionalData, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 1024);			
			transData->bWaveSID = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->usWaveSTransResult = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->bWaveSCVMAnalysis = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );
			transData->ulOrgTraceNum = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->usTerminalCommunicationMode = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );			
			transData->byPINEntryCapability = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byPackType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->szOrgAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szCVV2, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);
			transData->inCardType = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->byTCFailUpCnt = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byCardTypeNum = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.byEMVTransStatus = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5A_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T5A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 10);						
			memcpy(transData->stEMVinfo.T5F2A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F30, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T5F34 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T5F34_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T82, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T84_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T84, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			memcpy(transData->stEMVinfo.T8A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T91, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T91Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T95, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 5);						
			memcpy(transData->stEMVinfo.T9A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9C = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F02, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F03, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 6);						
			memcpy(transData->stEMVinfo.T9F09, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			transData->stEMVinfo.T9F10_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F10, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
			memcpy(transData->stEMVinfo.T9F1A, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F26, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			transData->stEMVinfo.T9F27 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F33, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F34, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F35 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			transData->stEMVinfo.T9F36_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9F36, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T9F37, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->stEMVinfo.T9F41, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T9F53 = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.ISR, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.ISRLen = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T9B, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 2);						
			memcpy(transData->stEMVinfo.T5F24, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			transData->stEMVinfo.T71Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T71, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			transData->stEMVinfo.T72Len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			memcpy(transData->stEMVinfo.T72, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 258);						
			memcpy(transData->stEMVinfo.T9F06, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->stEMVinfo.T9F1E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F28, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.T9F29, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 8);						
			memcpy(transData->stEMVinfo.szChipLabel, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);
                        strcpy((char*)transData->szTID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->szMID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->szHostLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->szBatchNo, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);
                        transData->fIsInstallment = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->stIPPinfo.szInstallmentTerms, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->stIPPinfo.szMonthlyAmt, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
                        strcpy((char*)transData->stIPPinfo.szInterestRate, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->stIPPinfo.szTotalInterest, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
                        memcpy(transData->stIPPinfo.szHandlingFee, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
                        strcpy((char*)transData->stIPPinfo.szIPPSchemeID, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->stIPPinfo.szFreqInstallment, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        strcpy((char*)transData->stIPPinfo.szTransCurrency, (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
                        memcpy(transData->stIPPinfo.szIPPTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);
            transData->fAlipay = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));	
            transData->fIsDiscounted = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		
			memcpy(transData->szFixedAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);			
			memcpy(transData->szOrigAmountDisc, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
            transData->fIsDiscountedFixAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		
			memcpy(transData->szPercentage, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);			
            transData->fIsDiscountedPercAmt = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		
			memcpy(transData->stEMVinfo.T9F63, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 16);						
			transData->stEMVinfo.T9F63_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F6E, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
		transData->stEMVinfo.T9F6E_len = sqlite3_column_int(stmt,inStmtSeq +=1 );

		memcpy(transData->stEMVinfo.T9F7C, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 32);						
		transData->stEMVinfo.T9F7C_len = sqlite3_column_int(stmt,inStmtSeq +=1 );
			
		}
	} while (result == SQLITE_ROW);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}

#else
int inDatabase_BatchSearchByInvNo(TRANS_DATA_TABLE *transData, char *hexInvoiceNo)
{
	int result,i = 0;
	int inResult = d_NO;
//	char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo FROM TransData WHERE szRRN = ?";
    //char *sql = "SELECT HDTid, MITid, CDTid, IITid, byTransType, byPanLen, szExpireDate, byEntryMode, szTotalAmount, szBaseAmount, szTipAmount, byOrgTransType, szMacBlock, szPINBlock, szYear, szDate, szTime, szOrgDate, szOrgTime, szAuthCode, szRRN, szInvoiceNo, szOrgInvoiceNo, byPrintType, byVoided, byAdjusted, byUploaded, byTCuploaded, szCardholderName, szzAMEX4DBC, szStoreID, szRespCode, szServiceCode, byContinueTrans, byOffline, byReversal, byEMVFallBack, shTransResult, szTpdu, szIsoField03, szMassageType, szPAN, szCardLable, usTrack1Len, usTrack2Len, usTrack3Len, szTrack1Data, szTrack2Data, szTrack3Data, usChipDataLen, baChipData, usAdditionalDataLen, baAdditionalData, bWaveSID,usWaveSTransResult,bWaveSCVMAnalysis,ulTraceNum, ulOrgTraceNum, usTerminalCommunicationMode, ulSavedIndex, byPINEntryCapability, byPackType, szOrgAmount, szCVV2, inCardType, byTCFailUpCnt, byCardTypeNum, byEMVTransStatus, T5A_len, T5A, T5F2A, T5F30, T5F34, T5F34_len, T82, T84_len, T84, T8A, T91, T91Len, T95, T9A, T9C, T9F02, T9F03, T9F09, T9F10_len, T9F10, T9F1A, T9F26, T9F27, T9F33, T9F34, T9F35, T9F36_len, T9F36, T9F37, T9F41, T9F53, ISR, ISRLen, T9B, T5F24, T71Len, T71, T72Len, T72, T9F06, T9F1E, T9F28, T9F29, szChipLabel, szTID, szMID, szHostLabel, szBatchNo, fIsInstallment, szInstallmentTerms, szMonthlyAmt, szInterestRate, szTotalInterest, szHandlingFee, szIPPSchemeID, szFreqInstallment, szTransCurrency, szIPPTotalAmount, fAlipay FROM TransData WHERE szAuthCode = ?";
    char *sql = "SELECT szYear, szDate, szTime, szAuthCode, szPAN, ulSavedIndex, szTotalAmount FROM PreauthData WHERE szInvoiceNo = ?";

	vdDebug_LogPrintf("=====inDatabase_BatchSearchByInvNo=====");
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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

#if 1
    inStmtSeq = 0;
    result = sqlite3_bind_blob(stmt, inStmtSeq +=1, hexInvoiceNo, 4, SQLITE_STATIC);
	DebugAddHEX("hexInvoiceNo", hexInvoiceNo, 13);
#else
	vdDebug_LogPrintf("strAuthCode[%s]", strAuthCode);
	DebugAddHEX("strAuthCode", strAuthCode, 13);

	inStmtSeq = 0;
	result = sqlite3_bind_blob(stmt, inStmtSeq +=1, strAuthCode, 7, SQLITE_STATIC);

	DebugAddINT("inDatabase_BatchSearchByAuthCode sqlite3_bind_blob", result);
	DebugAddHEX("strAuthCode", strAuthCode, 6);
#endif

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		i++;
		//DebugAddINT("if(100)=Find", result);
		//DebugAddINT("Loop Count", i);

		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

			memcpy(transData->szYear, sqlite3_column_blob(stmt,inStmtSeq), 2);						
			memcpy(transData->szDate, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 3);						
			memcpy(transData->szTime, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 4);						
			memcpy(transData->szAuthCode, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);						
			memcpy(transData->szPAN, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 20);	
			transData->ulSavedIndex = sqlite3_column_double(stmt, inStmtSeq +=1 );	
			memcpy(transData->szTotalAmount, sqlite3_column_blob(stmt,inStmtSeq +=1 ), 7);	
		}
	} while (result == SQLITE_ROW);

    DebugAddSTR("batch serPAN", transData->szPAN, 10);
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inResult);
}
#endif

int inDatabase_BatchUpdatePreauth(TRANS_DATA_TABLE *transData)
{
    int result;
    char *sql = "UPDATE PreauthData SET byVoided = ? WHERE ulSavedIndex = ?";

    vdDebug_LogPrintf("=====inDatabase_BatchUpdatePreauth=====");
	
	/* open the database */
	result = sqlite3_open(DB_BATCH,&db);
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
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->byVoided);
    result = sqlite3_bind_int(stmt, inStmtSeq +=1, transData->ulSavedIndex);

	result = sqlite3_step(stmt);
	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(ST_SUCCESS);
}

int inHDTReadEnabledHostID(int inHDTid[])
{    
	int result;	
	char *sql = "SELECT HDTid FROM HDT WHERE fHostEnable = 1";
	int inCount = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
    //sqlite3_bind_text(stmt, inStmtSeq +=1, szHostName, strlen(szHostName), SQLITE_STATIC);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			/* HDTid */
			inHDTid[inCount] = sqlite3_column_int(stmt,inStmtSeq);

            /* szHostName */
			//strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			//vdDebug_LogPrintf("szHostName[%s] %d",szHostName[inCount], inCount);

			inCount++;
		}
	} while (result == SQLITE_ROW);

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	vdDebug_LogPrintf("inDBResult[%d]",inDBResult);
	vdDebug_LogPrintf("inCount[%d]",inCount);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inCount);
}

int inHDTReadMaxTraceNo(void)
{
	int len;
	CTOS_RTC SetRTC;     
	int result;
	int inResult = d_NO;
	//char *sql = "SELECT inHostIndex, szHostLabel, szTPDU, szNII, fReversalEnable, fHostEnable, szTraceNo, fSignOn, ulLastTransSavedIndex, inCurrencyIdx, szAPName, inFailedREV, inDeleteREV, inNumAdv FROM HDT WHERE HDTid = ? AND fHostEnable = ?";
    char *sql = "SELECT szTraceNo, MAX(szTraceNo) FROM HDT";
    
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	//sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	//sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;
	
			/* szTraceNo*/
			memcpy(strHDT.szTraceNo, sqlite3_column_blob(stmt,inStmtSeq), 3);
		}
	} while (result == SQLITE_ROW);

	//vdDebug_LogPrintf("strHDT.inHostIndex[%d]",strHDT.inHostIndex);
	//vdDebug_LogPrintf("strHDT.inCurrencyIdx[%d]",strHDT.inCurrencyIdx);
	//vdDebug_LogPrintf("strHDT.fHostEnable[%d]",strHDT.fHostEnable);

	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}

int inCDTReadEnabled(int inSeekCnt, int inHDTid)
{
	int result;
	int inResult = d_NO;
	char *sql = "SELECT szPANLo, szPANHi, szCardLabel, inType, inMinPANDigit, inMaxPANDigit, inCVV_II, InFloorLimitAmount, fExpDtReqd,  fPinpadRequired, fManEntry, fCardPresent, fChkServiceCode, fluhnCheck, fCDTEnable, IITid, HDTid, fMagSwipeEnable, fPreAuthAllowed, fRefundAllowed, fCTLSAllowed, fAddRefRRNAPP FROM CDT WHERE CDTid = ? AND HDTid = ? AND fCDTEnable = ?";

	int inDBResult = 0;	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}
	inDBResult = sqlite3_exec( db, "begin", 0, 0, NULL );
	vdDebug_LogPrintf("inCDTRead,sqlite3_exec[%d]",inDBResult);
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;
	sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
        sqlite3_bind_int(stmt, inStmtSeq +=1, inHDTid);
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fCDTEnable = 0

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		vdDebug_LogPrintf("inCDTRead,result[%d]SQLITE_ROW[%d]",result,SQLITE_ROW);
		
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;

		    /*szPANLo*/
		     strcpy(strCDT.szPANLo, sqlite3_column_text(stmt,inStmtSeq));

		    /*szPANHi*/
		     strcpy(strCDT.szPANHi, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*szCardLabel*/
		    strcpy(strCDT.szCardLabel, sqlite3_column_text(stmt,inStmtSeq +=1));

		    /*inType*/
			strCDT.inType = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMinPANDigit*/
		    strCDT.inMinPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);
			
			/*inMaxPANDigit*/
		    strCDT.inMaxPANDigit = sqlite3_column_int(stmt,inStmtSeq +=1);

			/*inCVV_II*/
		    strCDT.inCVV_II = sqlite3_column_int(stmt,inStmtSeq +=1);

            
			/*InFloorLimitAmount*/
			strCDT.InFloorLimitAmount = sqlite3_column_double(stmt,inStmtSeq +=1 );

			
			/*fExpDtReqd*/
		    strCDT.fExpDtReqd = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
			
			/*fPinpadRequired*/
		    strCDT.fPinpadRequired = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

				
			/*fManEntry*/
		    strCDT.fManEntry = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		    /*fCardPresent*/
		    strCDT.fCardPresent = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			
		    /*fChkServiceCode*/
		    strCDT.fChkServiceCode =fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fluhnCheck */
			strCDT.fluhnCheck = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

			/* fCDTEnable*/
			strCDT.fCDTEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));		

            /*IITid*/
		    strCDT.IITid = sqlite3_column_int(stmt,inStmtSeq +=1);

			/* HDTid*/
			strCDT.HDTid = sqlite3_column_int(stmt,inStmtSeq +=1);	

		/* fMagSwipeEnable */
		strCDT.fMagSwipeEnable = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			
			/* fPreAuthAllowed */
			strCDT.fPreAuthAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fRefundAllowed*/
		strCDT.fRefundAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fCTLSAllowed*/
		strCDT.fCTLSAllowed = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

		/*fAddRefRRNAPP*/
		strCDT.fAddRefRRNAPP = fGetBoolean((BYTE *)sqlite3_column_text(stmt,inStmtSeq +=1 ));


		}
	} while (result == SQLITE_ROW);
   
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(inResult);
}


//SELECT CDTid, szCardLabel FROM CDT where fCDTEnable=1 order by szCardLabel

//SELECT HDTid, szHostLabel FROM HDT WHERE fHostEnable = ? order by inSequence

int inCDTReadCDTid(int inCDTID[])
{    
	int result;	
	char *sql = "SELECT CDTid FROM CDT where fCDTEnable = ? order by szCardLabel";
	int inCount = 0;
	int inDBResult = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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
	sqlite3_bind_int(stmt, inStmtSeq +=1, 1);//fHostEnable = 1

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;
			/* HDTid */
			inCDTID[inCount] = sqlite3_column_int(stmt,inStmtSeq);

            /* szHostName */
			//strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));
			//vdDebug_LogPrintf("szHostName[%s] %d",szHostName[inCount], inCount);

			inCount++;
		}
	} while (result == SQLITE_ROW);

    
	inDBResult = sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	vdDebug_LogPrintf("inDBResult[%d]",inDBResult);
	vdDebug_LogPrintf("inCount[%d]",inCount);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

    return(d_OK);
}



int inCBPayRead(int inSeekCnt)
	{
		int len;
		CTOS_RTC SetRTC;	 
		int result;
		int inResult = d_NO;
		char *sql = "SELECT GenerateURL, TranStatusURL, VoidURL, CancelURL, AuthenToken, SubMerId, inQP_CurlTimeout, inQP_CurlRetries, fQP_SSLEnable FROM QPT WHERE QPTid=?";
		//


		/* open the database */
		result = sqlite3_open(DB_TERMINAL,&db);
		if (result != SQLITE_OK) {
			sqlite3_close(db);
			return 1;
		}
	
		vdDebug_LogPrintf("inCBPayRead BEGIN");
	
		vdSetJournalModeOff();
		
		sqlite3_exec( db, "begin", 0, 0, NULL );
		/* prepare the sql, leave stmt ready for loop */
		result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
		if (result != SQLITE_OK) {
			sqlite3_close(db);
			return 2;
		}
	
		sqlite3_bind_int(stmt, 1, inSeekCnt);
	
		//vdDebug_LogPrintf("inBDOPayRead BEFORE DO");
	
		/* loop reading each row until step returns anything other than SQLITE_ROW */
		do {
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW) { /* can read data */
				inResult = d_OK;
				inStmtSeq = 0;

				
				strcpy((char*)strCBPay.szGenerateURL, (char *)sqlite3_column_text(stmt, inStmtSeq));
				
				strcpy((char*)strCBPay.szTranStatusURL, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));	
				
				strcpy((char*)strCBPay.szVoidURL, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));	

				strcpy((char*)strCBPay.szCancelURL, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));	
				


				strcpy((char*)strCBPay.szAuthenToken, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
				
				strcpy((char*)strCBPay.szSubMerId, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));	
				
				

				strCBPay.inQP_CurlTimeout = sqlite3_column_int(stmt,inStmtSeq +=1 );
	
				strCBPay.inQP_CurlRetries = sqlite3_column_int(stmt,inStmtSeq +=1 );
				
				strCBPay.fQP_SSLEnable = sqlite3_column_int(stmt,inStmtSeq +=1 );
			
			}
		} while (result == SQLITE_ROW);
	  
		sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
		sqlite3_finalize(stmt);
		sqlite3_close(db);

		//srTransRec.fDisclaimer = strCBPay.fDisclaimer;

		return(inResult);
	}



int inOKDPayRead(int inSeekCnt)
	{
		int len;
		CTOS_RTC SetRTC;	 
		int result;
		int inResult = d_NO;
		//char *sql = "SELECT TranStatusURL, TranStatusSecURL, VoidURL, VoidSecURL, AuthenToken, SubMerId, inQP_CurlTimeout, inQP_CurlRetries, fQP_SSLEnable FROM OKD WHERE OKDid=?";
		//

		//char *sql = "SELECT TranStatusURL, TranStatusSecURL, VoidURL, VoidSecURL, TranStatusURLSimTwo, TranStatusSecURLSimTwo, VoidURLSimTwo, VoidSecURLSimTwo, inQP_CurlTimeout, inQP_CurlRetries, fOKDollarSimOne FROM OKD WHERE OKDid=?";
		char *sql = "SELECT TranStatusURL, TranStatusSecURL, VoidURL, VoidSecURL, TranStatusURLSimTwo, TranStatusSecURLSimTwo, VoidURLSimTwo, VoidSecURLSimTwo, inQP_CurlTimeout, inQP_CurlRetries, fOKDollarSimOne FROM OKD WHERE OKDid=?";

		/* open the database */
		result = sqlite3_open(DB_TERMINAL,&db);
		if (result != SQLITE_OK) {
			sqlite3_close(db);
			return 1;
		}
	
		vdDebug_LogPrintf("inOKDPayRead BEGIN");
	
		vdSetJournalModeOff();
		
		sqlite3_exec( db, "begin", 0, 0, NULL );
		/* prepare the sql, leave stmt ready for loop */
		result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

		vdDebug_LogPrintf("inOKDPayRead result [%d]", result);
		
		if (result != SQLITE_OK) {
			sqlite3_close(db);
			return 2;
		}

	
		sqlite3_bind_int(stmt, 1, inSeekCnt);
	
		//vdDebug_LogPrintf("inBDOPayRead BEFORE DO");
	
		/* loop reading each row until step returns anything other than SQLITE_ROW */
		do {
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW) { /* can read data */
				inResult = d_OK;
				inStmtSeq = 0;

				//sim 1 primary and secondary URL
				strcpy((char*)strCBOKD.szTranStatusURL, (char *)sqlite3_column_text(stmt, inStmtSeq));				
				strcpy((char*)strCBOKD.szTranStatusSecURL, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));					
				strcpy((char*)strCBOKD.szVoidURL, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));	
				strcpy((char*)strCBOKD.szVoidSecURL, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));	
				

				//sim 2 primary and secondary URL
				strcpy((char*)strCBOKD.szTranStatusURLSimTwo, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));				
				strcpy((char*)strCBOKD.szTranStatusSecURLSimTwo, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));	
				strcpy((char*)strCBOKD.szVoidURLSimTwo, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
				strcpy((char*)strCBOKD.szVoidSecURLSimTwo, (char *)sqlite3_column_text(stmt, inStmtSeq +=1));
				
				

				strCBOKD.inQP_CurlTimeout = sqlite3_column_int(stmt,inStmtSeq +=1 );	
				strCBOKD.inQP_CurlRetries = sqlite3_column_int(stmt,inStmtSeq +=1 );				
				strCBOKD.fOKDollarSimOne = sqlite3_column_int(stmt,inStmtSeq +=1 );
			
			}
		} while (result == SQLITE_ROW);
	  
		sqlite3_exec(db,"commit;",NULL,NULL,NULL);
	
		sqlite3_finalize(stmt);
		sqlite3_close(db);

		//srTransRec.fDisclaimer = strCBPay.fDisclaimer;
		vdDebug_LogPrintf("inOKDPayRead inResult [%d] END", inResult);
		
		vdDebug_LogPrintf("szTranStatusURL [%s]", strCBOKD.szTranStatusURL);
		vdDebug_LogPrintf("TranStatusSecURL [%s]", strCBOKD.szTranStatusSecURL);
		
		vdDebug_LogPrintf("VoidURL [%s]", strCBOKD.szVoidURL);
		vdDebug_LogPrintf("VoidSecURL [%s]", strCBOKD.szVoidSecURL);

		vdDebug_LogPrintf("TranStatusURLSimTwo [%s]", strCBOKD.szTranStatusURLSimTwo);
		vdDebug_LogPrintf("TranStatusSecURLSimTwo [%s]", strCBOKD.szTranStatusSecURLSimTwo);		

		vdDebug_LogPrintf("szVoidURLSimTwo [%s]", strCBOKD.szVoidURLSimTwo);
		vdDebug_LogPrintf("szVoidSecURLSimTwo [%s]", strCBOKD.szVoidSecURLSimTwo);

		
		vdDebug_LogPrintf("inQP_CurlTimeout [%d]", strCBOKD.inQP_CurlTimeout);
		vdDebug_LogPrintf("inQP_CurlRetries [%d]", strCBOKD.inQP_CurlRetries);
		
		vdDebug_LogPrintf("fOKDollarSimOne [%d]", strCBOKD.fOKDollarSimOne);

		
		return(inResult);
	}



int inHDTReadOrderBySequence2(char szHostName[][100], int inCPTID[])
{
	int len;
	int result;
		
	char *sql = "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 and inHostSettingOn = 1 order by inSequence";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	//sqlite3_bind_int(stmt, 1, inMITid);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

            /*inHostIndex*/
            inCPTID[inCount] = sqlite3_column_int(stmt, inStmtSeq );


            /* szHostName */
			strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

						
			/* count */
			//inCount = sqlite3_column_int(stmt,inStmtSeq);
			inCount++;
			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	vdDebug_LogPrintf("inHDTReadOrderBySequence2");

	return(inCount);
}


int inHDTReadOrderBySequence(char szHostName[][100], int inCPTID[])
{
	int len;
	int result;
		
	char *sql = "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 order by inSequence";
	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	//sqlite3_bind_int(stmt, 1, inMITid);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

            /*inHostIndex*/
            inCPTID[inCount] = sqlite3_column_int(stmt, inStmtSeq );


            /* szHostName */
			strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

						
			/* count */
			//inCount = sqlite3_column_int(stmt,inStmtSeq);
			inCount++;
			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}




#ifdef MINOR_CHANGES
int inPreauthNumRecord(void)
{
	int len;
	int result;
	char *sql = "SELECT COUNT(*) FROM TransData where byTransType=102;";
	int inCount = 0;
	
	/* open the database */
	//result = sqlite3_open(DB_TERMINAL,&db);
	result = sqlite3_open(DB_BATCH,&db);
	
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

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

			/* count */
			inCount = sqlite3_column_int(stmt,inStmtSeq);
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}	
#endif


int inHDTUpdateHostEnable(int inSeekCnt, int fHostEnable)
{
	int result;
	char *sql = "UPDATE HDT SET fHostEnable = ? WHERE HDTid = ?";

	vdDebug_LogPrintf("[inHDTUpdateHostEnable]-start ");
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);	
	vdDebug_LogPrintf("[inHDTUpdateHostEnable]result[%d]", result);
	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}


	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);   
	vdDebug_LogPrintf("[inHDTUpdateHostEnable]result[%d]", result);

	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;

	/* fHostEnable */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, fHostEnable);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	result = sqlite3_step(stmt);

	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	
	vdDebug_LogPrintf("[inHDTUpdateHostEnable]result[%d]", result);
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	result = sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("[inHDTUpdateHostEnable]result[%d]", result);

    return(d_OK);
}


#ifdef CBB_FIN_ROUTING



int inHDTUpdateHostCurr(int inSeekCnt, int inCurrencyIdx)
{
	int result;
	char *sql = "UPDATE HDT SET inCurrencyIdx = ? WHERE HDTid = ?";

	vdDebug_LogPrintf("[inHDTUpdateHostCurr]-start ");
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);	
	vdDebug_LogPrintf("[inHDTUpdateHostCurr]result[%d]", result);
	
	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 1;
	}


	vdSetJournalModeOff();
	
	sqlite3_exec( db, "begin", 0, 0, NULL );
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);   
	vdDebug_LogPrintf("[inHDTUpdateHostCurr]result[%d]", result);

	if (result != SQLITE_OK) {
		sqlite3_close(db);
		return 2;
	}

	inStmtSeq = 0;

	/* inCurrencyIdx */
	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inCurrencyIdx);

	result = sqlite3_bind_int(stmt, inStmtSeq +=1, inSeekCnt);
	result = sqlite3_step(stmt);

	if( result != SQLITE_DONE ){
		sqlite3_close(db);
		return 3;
	}

	
	vdDebug_LogPrintf("[inHDTUpdateHostCurr]result[%d]", result);
    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	result = sqlite3_finalize(stmt);
	sqlite3_close(db);
	vdDebug_LogPrintf("[inHDTUpdateHostCurr]result[%d]", result);

    return(d_OK);
}


int inHDTReadOrderBySequenceSignOnALL(char szHostName[][100], int inCPTID[], int inCurrIndx)
{
	int len;
	int result;
	BYTE szSignOnHost[1024+1];
	char *sql;
	sql=&szSignOnHost;		

	memset(szSignOnHost,0x00,sizeof(szSignOnHost));
		
	//char *sql = "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 order by inSequence";

	
	vdDebug_LogPrintf("[inHDTReadOrderBySequenceSignOnALL inCurrIndx [%d]", inCurrIndx);
	strcpy(szSignOnHost, "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 AND inCurrIndxSignon != 0 order by inSequence");
		

	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	//sqlite3_bind_int(stmt, 1, inMITid);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

            /*inHostIndex*/
            inCPTID[inCount] = sqlite3_column_int(stmt, inStmtSeq );


            /* szHostName */
			strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

						
			/* count */
			//inCount = sqlite3_column_int(stmt,inStmtSeq);
			inCount++;
			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}

int inHDTReadOrderBySequenceSignOn(char szHostName[][100], int inCPTID[], int inCurrIndx)
{
	int len;
	int result;
	BYTE szSignOnHost[1024+1];
	char *sql;
	sql=&szSignOnHost;		

	memset(szSignOnHost,0x00,sizeof(szSignOnHost));
		
	//char *sql = "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 order by inSequence";

	
	vdDebug_LogPrintf("[inHDTReadOrderBySequenceSignOn inCurrIndx [%d]", inCurrIndx);

	#if 1
	//Default Currency is MMK
	if(inCurrIndx == 1)		
		strcpy(szSignOnHost, "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 AND inCurrIndxSignon = 1 order by inSequence");
	else
		strcpy(szSignOnHost, "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 AND inCurrIndxSignon != 0 order by inSequence");
	#else // show all regardless of default currency.
	strcpy(szSignOnHost, "SELECT HDTid , szHostLabel  FROM HDT WHERE fHostEnable=1 AND inCurrIndxSignon != 0 order by inSequence");
	
	#endif	

	int inCount = 0;
	
	/* open the database */
	result = sqlite3_open(DB_TERMINAL,&db);
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

	//sqlite3_bind_int(stmt, 1, inMITid);

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inStmtSeq = 0;

            /*inHostIndex*/
            inCPTID[inCount] = sqlite3_column_int(stmt, inStmtSeq );


            /* szHostName */
			strcpy((char*)szHostName[inCount], (char *)sqlite3_column_text(stmt,inStmtSeq +=1 ));

						
			/* count */
			//inCount = sqlite3_column_int(stmt,inStmtSeq);
			inCount++;
			
		}
	} while (result == SQLITE_ROW);

    
	sqlite3_exec(db,"commit;",NULL,NULL,NULL);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(inCount);
}
#endif

