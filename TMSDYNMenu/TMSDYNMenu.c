/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include "sqlite3.h"
#include "..\FileModule\myFileFunc.h"
#include "..\Includes\Dmenu.h"
#include "..\Includes\CTOSInput.h"

sqlite3 *dbDMENU;
sqlite3_stmt *stmt;
int inStmtSeq;

typedef struct tagDMENU_TMS
{
	unsigned short usButtonID;
	unsigned char szButtonItemImage[100];
	unsigned char szButtonItemLabel[100];
	unsigned char szButtonTitleLabel[100];	
	unsigned char szButtonItemData[100];	
	unsigned char szSubMenuName[100];	
} DMENU_TMS;

int inDSubTMSMenuRead(unsigned char *uszTMSDYNMenuDatabase, unsigned char *szSubMenuName, DMENU_TMS *dmSubMenuList)
{
	int inResult = d_NO;
	int result = d_NO;
	int inRecordCount = 0;
	char szDB_DYNAMICMENU[100];
	char sql[1024];
	sprintf(sql, "SELECT usButtonID, szButtonItemImage, szButtonItemLabel, szButtonTitleLabel, szButtonItemData, szSubMenuName FROM %s WHERE fButtonIDEnable = 1 ORDER BY usButtonID ASC LIMIT 40", szSubMenuName);
	
 	/* open the database */
	sprintf(szDB_DYNAMICMENU, "%s%s", "./fs_data/", uszTMSDYNMenuDatabase);	
	result = sqlite3_open(szDB_DYNAMICMENU, &dbDMENU);
	if (result != SQLITE_OK) {
		sqlite3_close(dbDMENU);
		return 1;
	}
	sqlite3_exec( dbDMENU, "begin", 0, 0, NULL );
	
	/* prepare the sql, leave stmt ready for loop */
	result = sqlite3_prepare_v2(dbDMENU, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
		sqlite3_close(dbDMENU);
		return 2;
	}

	/* loop reading each row until step returns anything other than SQLITE_ROW */
	do {
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW) { /* can read data */
			inResult = d_OK;
			inStmtSeq = 0;		

			/* usButtonID */
			dmSubMenuList[inRecordCount].usButtonID = sqlite3_column_int(stmt,inStmtSeq);

			if ((strTCT.byTerminalType%2) == 0)
			{
				/*szButtonItemImage*/
				strcpy((char *)dmSubMenuList[inRecordCount].szButtonItemImage, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));
			}
			else
			{
				memset((char *)dmSubMenuList[inRecordCount].szButtonItemImage, 0x00, sizeof((char *)dmSubMenuList[inRecordCount].szButtonItemImage));
				inStmtSeq +=1;
			}

			/*szButtonItemLabel*/
			strcpy((char *)dmSubMenuList[inRecordCount].szButtonItemLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szButtonTitleLabel*/
			strcpy((char *)dmSubMenuList[inRecordCount].szButtonTitleLabel, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szButtonItemData*/
			strcpy((char *)dmSubMenuList[inRecordCount].szButtonItemData, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			/*szSubMenuName*/
			strcpy((char *)dmSubMenuList[inRecordCount].szSubMenuName, (char *)sqlite3_column_text(stmt,inStmtSeq +=1));

			inRecordCount++;
		}
	} while (result == SQLITE_ROW);

	sqlite3_exec(dbDMENU,"commit;",NULL,NULL,NULL);
	//???sqlite????
	sqlite3_finalize(stmt);
	sqlite3_close(dbDMENU);

    return(inResult);
}

int inTMSDynamicMenu(unsigned char *uszTMSDYNMenuDatabase, unsigned char *uszTMSDYNMenuTable, DMENU_TMS* cmTMSMENU_COMMAND)
{	
	BYTE *sHeaderString, iHeaderStrLen;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
	BYTE  x = 1;
	BYTE *psItemsString;
	USHORT len = 0;
	BYTE key;
	BYTE szMenuList[2048+1];
	int inLoop = 0;
	int inRetVal = 0;
	int inResult = 0;
	DMENU_TMS dmTMSMenuListtmp[40] = {0};
	unsigned char szButtonItemDataValue[200+1];

	if (cmTMSMENU_COMMAND[0].szButtonItemLabel[0] != 0)
	{
		memset(szMenuList, 0x00, sizeof(szMenuList));
		for (inLoop = 0; inLoop <= MAXMENUS; inLoop++)
		{
			strcat((char *)szMenuList, (char *)cmTMSMENU_COMMAND[inLoop].szButtonItemLabel);
			if (cmTMSMENU_COMMAND[inLoop+1].szButtonItemLabel[0] == 0)
				break;
                        strcat((char *)szMenuList, (char *)"\n");
		}
	}

	if (cmTMSMENU_COMMAND[0].szButtonItemImage[0] != 0)
	{
		memset(szMenuList, 0x00, sizeof(szMenuList));
		for (inLoop = 0; inLoop <= MAXMENUS; inLoop++)
		{	
			strcat((char *)szMenuList, (char *)cmTMSMENU_COMMAND[inLoop].szButtonItemImage);
			if (cmTMSMENU_COMMAND[inLoop+1].szButtonItemImage[0] == 0)
				break;
		
			strcat((char *)szMenuList, (char *)" \n");
		}
	}
	
	sHeaderString = cmTMSMENU_COMMAND[0].szButtonTitleLabel;
	iHeaderStrLen = strlen((char *)sHeaderString);
	iCol = 1;
	psItemsString = szMenuList;
	if (cmTMSMENU_COMMAND[0].szButtonItemImage[0] != 0)
		key = bGMenuDisplay(sHeaderString, iHeaderStrLen, bHeaderAttr, iCol, x, psItemsString, TRUE);
	else	
		key = MenuDisplay(sHeaderString, iHeaderStrLen, bHeaderAttr, iCol, x, psItemsString, TRUE);

	if (key == 0xFF) 
	{
		CTOS_LCDTClearDisplay();
		CTOS_LCDTPrintXY(1, 1, "WRONG INPUT!!!");
		CTOS_Beep();
		CTOS_Delay(300);
		CTOS_Beep();

		return -1;	
	}
	
//	vduiLightOn();
	CTOS_BackLightSet (d_BKLIT_LCD, d_ON);
	
	if(key == d_KBD_CANCEL)
	{
	   return 0;;
	}
	else if(key == d_KBD_F1 || key == d_KBD_F2 || key == d_KBD_F3 || key == d_KBD_F4)
	{
			if (cmTMSMENU_COMMAND[key-1].szButtonItemData[0]!=0x00)
			{
				strcpy(szButtonItemDataValue, cmTMSMENU_COMMAND[key-1].szButtonItemData);
				CTOS_LCDTPrintAligned(6, szButtonItemDataValue, d_LCD_ALIGNCENTER);	
				CTOS_Delay(500);
			}
							
			if (cmTMSMENU_COMMAND[key-1].szSubMenuName[0]!=0x00)
			{
				memset(dmTMSMenuListtmp, 0x00, sizeof(dmTMSMenuListtmp));
				inRetVal = inDSubTMSMenuRead(uszTMSDYNMenuDatabase, cmTMSMENU_COMMAND[key-1].szSubMenuName, dmTMSMenuListtmp);	
				if (inRetVal==d_OK) 		
					inRetVal = inTMSDynamicMenu(uszTMSDYNMenuDatabase, cmTMSMENU_COMMAND[key-1].szSubMenuName, dmTMSMenuListtmp);
			}
	}
	else if(0 <key && key < 99 )
	{
		if (cmTMSMENU_COMMAND[key-1].szButtonItemData[0]!=0x00)
		{
			strcpy(szButtonItemDataValue, cmTMSMENU_COMMAND[key-1].szButtonItemData);
			CTOS_LCDTPrintAligned(6, szButtonItemDataValue, d_LCD_ALIGNCENTER); 
			CTOS_Delay(500);
		}
						
		if (cmTMSMENU_COMMAND[key-1].szSubMenuName[0]!=0x00)
		{
			memset(dmTMSMenuListtmp, 0x00, sizeof(dmTMSMenuListtmp));
			inRetVal = inDSubTMSMenuRead(uszTMSDYNMenuDatabase, cmTMSMENU_COMMAND[key-1].szSubMenuName, dmTMSMenuListtmp);	
			if (inRetVal==d_OK) 		
				inRetVal = inTMSDynamicMenu(uszTMSDYNMenuDatabase, cmTMSMENU_COMMAND[key-1].szSubMenuName, dmTMSMenuListtmp);
		}
	}
	else
	{
		CTOS_LCDTClearDisplay();
		CTOS_LCDTPrintXY(1, 1, "WRONG INPUT!!!");
		CTOS_Beep();
		CTOS_Delay(300);
		CTOS_Beep();
	}
	return d_OK;
}

int inDemoTMSMenu(void)
{
	int inRetVal = d_OK;
	unsigned char *uszTMSDYNMenuDatabase = "TMSDYNMENU01.S3DB";
	unsigned char *uszTMSDYNMenuTable = "DMMenu1";
	DMENU_TMS dmTMSMenuListtmp[40] = {0};

	memset(dmTMSMenuListtmp, 0x00, sizeof(dmTMSMenuListtmp));
	inRetVal = inDSubTMSMenuRead(uszTMSDYNMenuDatabase, uszTMSDYNMenuTable, dmTMSMenuListtmp);	

	inRetVal = inTMSDynamicMenu(uszTMSDYNMenuDatabase, uszTMSDYNMenuTable, dmTMSMenuListtmp);
	return inRetVal;
}

