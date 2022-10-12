/* 
 * Author: Administrator
 *
 * Created on 18 ??????? 2558, 10:29 ?.
 */
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <sstream>
#include <pthread.h>
#include <signal.h>
#include <locale>

#include "../FileModule/myFileFunc.h"
#include "../Includes/MultiAplib.h"
#include "../Includes/PosTrans.h"
#include "..\Database\DatabaseFunc.h"
#include "../Ui/Display.h"
#include "..\debug\debug.h"
#include "../Includes/wub_lib.h"
#include "unforkAP.h"

using namespace std;

#ifdef	__cplusplus
extern "C"
{
#endif
    
extern USHORT CTOS_PrinterPutString(UCHAR* baBuf);
void vdPrintunfork()
{
    ifstream unfork;
    stringstream str;
    string line,strh="disable";
    int i = 1;
    
    unfork.open(UNFORKAPP , ios::in );
    assert( unfork.is_open() );
    CTOS_PrinterPutString( (UCHAR *)(strh.c_str()) ); 
    while( std::getline(unfork , line) )
    {
        str  << i++ << '.';
        str << line;
        
        CTOS_PrinterPutString( reinterpret_cast<unsigned char *>((char *)str.str().c_str()) ); 
        
        str.str(string());
    }
    
   
    unfork.close();

}

void vdNotLoadUnusedApps()
{
	using namespace std;

    ofstream unfork ;
    vector<string> apps;
    int i= 1  , inAPPID = 0 ;
    char szAPName[25];
    
    unfork.open( UNFORKAPP  , ios::binary);
    assert( unfork.is_open()  );
    
    memset( szAPName , 0 , sizeof(szAPName)) ;
    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);
    apps.push_back(szAPName); //main ap;
    
 //   unfork  << "TESTc";
    
    //phase 1
    while (inHDTRead(i) )
    {        
        if( std::find( apps.begin() , apps.end() , strHDT.szAPName ) == apps.end()   && strHDT.fHostEnable == 1)
        {
            
             unfork << "\r\n" << strHDT.szAPName;            
        }
		apps.push_back(strHDT.szAPName);
        ++i;
    }
//    if( strTCT.byERMMode == 0)
    unfork << "\r\n" << "SHARLS_ERM"; //<--- not use yet
 
    unfork << "\r\n" << "SHARLS_CLOSEJOB" << "\r\n"; //<--- not use yet
    unfork.close();
    
    vdPrintunfork();
}

 vector<string> apps;
 vector <string> apps2;
char CheckUnloadApp( char * apname )
{
//    ifstream unfork;
    string line;
    
    int i = 1;
    if( apps.empty() )
    {
        while (!inHDTRead(i) )
        {
            if (strHDT.fHostEnable == 0)
            {
                if( !(std::find( apps2.begin() , apps2.end() , strHDT.szAPName ) != apps2.end () ) )
                        apps.push_back(strHDT.szAPName);
               
            }
            else
                apps2.push_back(strHDT.szAPName);
            ++i;
        }
        
          std::sort(apps.begin(), apps.end());
          apps.erase(std::unique(apps.begin(), apps.end()), apps.end());
          
          
          apps2.clear();
      //    for( i = 0  ; i<apps.size() ; i++)
        //      vdDebug_LogPrintf("disable %s" , apps[i].c_str());
    }
    
    if( std::find( apps.begin() , apps.end() , apname ) != apps.end ()  )
        return 1;
    else
        return 0;
}

#ifdef	__cplusplus
}
}
#endif

