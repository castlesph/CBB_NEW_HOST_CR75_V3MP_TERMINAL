
#ifndef _DMENU__H
#define	_DMENU__H

#ifdef	__cplusplus
extern "C" {
#endif

#include ".\POSTypedef.h"

#define DB_DYNAMICMENU		"./fs_data/DYNAMICMENU01.S3DB"

typedef int (*DMENU_FUN)(void)  ;

//////////////////////////////////////////////////////////////////////////////////////
/*==================================================*
* 	Menu Definition Struct               *
*==================================================*/
/*
    
typedef struct tagCOMMAND
{
	short m_ButtonID;
	char m_ButtonItemImage[100];
	int usImageX1;
	int usImageY1;
	int usImageX2;
	int usImageY2;	
	char m_ButtonItemLabel[100];
    char m_ButtonTitleLabel[100];	
	int (*ButtonItemFunc)(void);
	unsigned char m_COMMANDMenu[100];	
} COMMAND;
*/ 
//////////////////////////////////////////////////////////////////////////////////////

typedef struct tagDMENU
{
	unsigned short usButtonID;
	unsigned char szButtonItemImage[100];
	unsigned char szButtonItemLabel[200];
	unsigned char szButtonTitleLabel[100];	
	unsigned char szButtonItemFunc[100];	
	unsigned char szSubMenuName[100];	
	unsigned char szCurrentMenuName[100];
	unsigned char szMenuAppName[50];
	unsigned char szForeBackGndColor[30];
} DMENU;

#define MAXMENUS 	1024

typedef struct tagMENU
{
	unsigned char m_COMMANDMenu[100];	
	DMENU *cmMENU;
} MENU;

typedef struct tagTouch
{
	char szImageKey[100];
	int usImageX1;
	int usImageY1;
	int usImageX2;
	int usImageY2;
	int usImageX0;
	int usImageY0;
} TOUCH;

#define BUTTON_IMG_BAR		"_B_"   //menu list with horizontal stripe image

int inF1KeyEvent(void);
int inF2KeyEvent(void);
int inF3KeyEvent(void);
int inF4KeyEvent(void);
int inEnterKeyEvent(void);
int in0KeyEvent(void);
int in1KeyEvent(void);
int in2KeyEvent(void);
int in3KeyEvent(void);
int in4KeyEvent(void);
int in5KeyEvent(void);
int in6KeyEvent(void);
int in7KeyEvent(void);
int in8KeyEvent(void);
int in9KeyEvent(void);


//int inFindCOMMANDMenu(unsigned char *m_COMMANDMenu);
int inDynamicMenu(DMENU* cmMENU_COMMNAD);
//int inDemoEvent(void);
//int inEnterKey1Event(void);
int inDSubMenuRead(char *szSubMenuName, DMENU *dmSubMenuList);

int inDSubMenuUpdateFlag(char *szSubMenuName, int fButtonIDEnable, int usButtonID);

BYTE bGMenuDisplay(BYTE *sHeaderString, BYTE iHeaderStrLen, BYTE bHeaderAttr, BYTE iCol, BYTE center_x, BYTE *sItemsString, BYTE isReCount);
int inSetLanguageDatabase(char* szLanguangeDatabaseName);
int inDMLanguageDatabaseRead(char *szButtonItemLabel, char *szButtonItemLabelTranslate);
int inChkRunOtherAppMenu(BYTE key, DMENU* cmMENU_COMMAND);
int inChkRunSubMenuFromMain(BYTE *szSubMenuName, BYTE *szFuncName);

#ifdef	__cplusplus
}
#endif

#endif	/* _DMENU__H */

