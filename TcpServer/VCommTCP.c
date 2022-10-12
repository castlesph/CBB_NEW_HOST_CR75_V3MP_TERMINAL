
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <ctosapi.h>
	
#include <resolv.h>
	
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sys/socket.h>   //connect,send,recv,setsockopt?
#include <sys/types.h>      
#include <sys/ioctl.h>

#include <netinet/in.h>     // sockaddr_in, "man 7 ip" ,htons
#include <poll.h>             //poll,pollfd
#include <arpa/inet.h>   //inet_addr,inet_aton
#include <unistd.h>        //read,write
#include <netdb.h>         //gethostbyname

#include <error.h>         //perror
#include <stdio.h>
#include <errno.h>         //errno



//#include "../Includes/EMVTypedef.h"
//#include "../Includes/myFileFunc.h"
#include "../debug/debug.h"

#include "VCommTCP.h"

/**********************************************
TCP_StartTCPServer: start tcp server with given port.
TCP_StopTCPServer: stop tcp server.
TCP_CheckServerAccepted: check if client connected and set the connect socket.
TCP_ServerSend: send data with the client socket.
TCP_ServerRecv: receive data with the client socket.
Make sure that the NWIF already net up before call those functions.
**********************************************/

static NetStatus g_NetStatus;
//extern devices_t g_Devices;

static int hServerSocket = -1;
static int iServerListenPort = -1;


static char szRespHttpHead1[] = "HTTP/1.1 200 OK";
static char szRespHttpHead2[] = "Content-type: text/xml";
static char szRespHttpHead3[] = "Content-Length:%d";
static char szRespHttpHead4[] = "Connection: close";


#define KEYF "ppmicros_key.pem"
#define CERTF "ppmicros_cert.pem"

static SSL_CTX* ctx_Server = NULL;
static SSL *ssl_server = NULL;

extern int errno;
static USHORT inEthernetOpen = 0;

typedef struct
{
    BOOL boSave;            // Save all setting to module or end of information retrieve on Ethernet module

    STR    strRemoteIP[24];    // Remote IP address
    USHORT usRemotePort;    // Remote port number

    STR    strLocalIP[24];     // Local IP address

    BYTE strMask[24];       // Network mask
    BYTE strGateway[24];    // Gateway address

    BYTE bDHCP;             // IP configuration
    BYTE bAutoConn;         // Connection mode

    BYTE* pbRxStack;        // The pointer to Received stack

    // For configurations getting
    BYTE strVersion[17];    // The version of Ethernet chip
    BYTE strMac[24];        // MAC address
}CTOS_CM_ETHERNET;

CTOS_CM_ETHERNET    srEthernetRec;

#define IPCONFIG_STATIC 0
#define IPCONFIG_DHCP 1  
#define DIAL_UP_MODE        0
#define ETHERNET_MODE       1
#define GPRS_MODE           2 


static BYTE bUnSingeLongToStr(IN ULONG ulValue, OUT BYTE *baStr)
{
    BYTE    bDigit;
    ULONG    i;

    bDigit = 0;
    i = ulValue;

    while(i)
    {
        i /= 10;
        bDigit++;
    }
    
    if(!bDigit)
        bDigit = 1;
    
    for(i = 0; i < bDigit; i++)
    {
        baStr[bDigit - 1 - i] = (BYTE)(ulValue % 10) + '0';
        ulValue /= 10;        
    }
    
    baStr[bDigit] = '\0';
    
    return bDigit;
}

static int inEthernet_InitEthernet(void)
{   
	USHORT shRet;
	
    shRet = CTOS_EthernetOpenEx();

	inEthernetOpen = 1;

    vdDebug_LogPrintf(". CTOS_EthernetOpenEx Ret=(%04x)",shRet);
    return shRet;    
}
#if 0
int inEthernet_GetConfigFromTable(void)
{
	USHORT shRet;
	
    vdDebug_LogPrintf("**inEthernet_GetConfigFromTable START**");
    memset(&srEthernetRec,0,sizeof(srEthernetRec));
    
    /* Terminal Ip */
	if(strTCP.fDHCPEnable == IPCONFIG_STATIC)
    {
	    memcpy(srEthernetRec.strLocalIP,strTCP.szTerminalIP,strlen(strTCP.szTerminalIP));
	    /* Mask */
	    memcpy(srEthernetRec.strMask,strTCP.szSubNetMask,strlen(strTCP.szSubNetMask));
	    /* Gateway */
	    memcpy(srEthernetRec.strGateway,strTCP.szGetWay,strlen(strTCP.szGetWay));
	    /* DHCP */
	    srEthernetRec.bDHCP = '0';
	    vdDebug_LogPrintf(". srEthernetRec.strLocalIP(%s)",srEthernetRec.strLocalIP);
	    vdDebug_LogPrintf(". srEthernetRec.strMask(%s)",srEthernetRec.strMask);
	    vdDebug_LogPrintf(". srEthernetRec.strGateway(%s)",srEthernetRec.strGateway);
    }

    /* AutoConnect */
    srEthernetRec.bAutoConn = '2';

    return ST_SUCCESS;
}

 /*Set up the configuration of Ethernet */
int inEthernet_SetConnectConfig(void)
{
	USHORT shRet;
    BYTE hPort[5];
    unsigned char const manual_connect[] = "2";
    BYTE babuff[d_BUFF_SIZE];
    
    memset(babuff, 0, sizeof(babuff));
    vdDebug_LogPrintf("**inEthernet_SetConnectConfig START**");
    vdDebug_LogPrintf("IP CONFIG = %d",strTCP.fDHCPEnable);
    if (strTCP.fDHCPEnable == IPCONFIG_DHCP)
    {
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, "1", 1);
        srEthernetRec.bDHCP = '1';
        vdDebug_LogPrintf(". Set DHCP (%c)",srEthernetRec.bDHCP);
    }
	else
    {
  		shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DHCP, "0", 1);
        srEthernetRec.bDHCP = '0';
        vdDebug_LogPrintf(". Set DHCP (%c)",srEthernetRec.bDHCP);
    }

	if(shRet != d_OK)
	{
	  	vdDebug_LogPrintf(". Set DHCP Not OK(%04x)",shRet);
	  	return ST_ERROR;
	}
	 
   
    if (strTCP.fDHCPEnable == IPCONFIG_STATIC)//is static
    {
        //Set IP Address
        vdDebug_LogPrintf(". Set IP (%s)",srEthernetRec.strLocalIP);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_IP, srEthernetRec.strLocalIP, strlen(srEthernetRec.strLocalIP));
		
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set IP Not OK(%04x)",shRet);
            return ST_ERROR;
        }

        // Set Mask //
        vdDebug_LogPrintf(". Set Mask (%s)",srEthernetRec.strMask);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_MASK, srEthernetRec.strMask, strlen(srEthernetRec.strMask));
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set Mask Not OK(%04x)",shRet);
            return ST_ERROR;
        }

        // Set Geteway IP //
        vdDebug_LogPrintf(". Set Gateway (%s)",srEthernetRec.strGateway);
        shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_GATEWAY, srEthernetRec.strGateway, strlen(srEthernetRec.strGateway));
        if(shRet != d_OK)
        {
            vdDebug_LogPrintf(". Set Gateway Not OK(%04x)",shRet);
            return ST_ERROR;
        }

        // Set DNS IP //
        vdDebug_LogPrintf(". Set DNSIP (%s)",strTCP.szHostDNS1);
        if (wub_strlen(strTCP.szHostDNS1)>0)//only set DNS if value assign, it is due to older firmware doesn't support
        {
            shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_DNSIP, strTCP.szHostDNS1, strlen(strTCP.szHostDNS1));
            if(shRet != d_OK)
            {
                vdDebug_LogPrintf(". Set DNSIP Not OK(%04x)",shRet);
                return ST_ERROR;
            }
        }
    }
    
    vdDebug_LogPrintf(". Set AutoConn (%02x)",srEthernetRec.bAutoConn);

    shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_AUTOCON,&srEthernetRec.bAutoConn, 1);
    if(shRet != d_OK)
    {
        vdDebug_LogPrintf(". Set AutoConn Not OK(%04x)",shRet);
        return ST_ERROR;
    }
 
    shRet = CTOS_EthernetConfigSet(d_ETHERNET_CONFIG_UPDATE_EXIT, babuff, 0);
    if(shRet != d_OK)
    {
        vdDebug_LogPrintf(". save Exit Not OK(%04x)",shRet);
        return ST_ERROR;
    }
     
    vdDebug_LogPrintf("**inEthernet_SetConnectConfig END**");
    return ST_SUCCESS;
}

 
int inEthernet_ConnectHost(void)
{
 BYTE byPortLen,byIpLen;
 BYTE baPort[10],bytemp[10];
 BYTE bkey;
 USHORT usRealLen;
 BYTE babuff[d_BUFF_SIZE];
 USHORT shRet;
 char szPriTxnHostIP[50]="118.201.48.210";
 USHORT inPriTxnHostPortNum = 5010;

 DebugAddSTR("inEthernet_ConnectHost","START**...",100); 
  
 usRealLen = 0;
 /* Host Ip */	 
 byIpLen = strlen(szPriTxnHostIP);
 memcpy(srEthernetRec.strRemoteIP,szPriTxnHostIP,byIpLen);
 vdDebug_LogPrintf(". srEthernetRec.strRemoteIP =(%s) byIpLen =(%d)",srEthernetRec.strRemoteIP,byIpLen);

 /* Host Port */
 memset(baPort,0,sizeof(baPort));
 srEthernetRec.usRemotePort = inPriTxnHostPortNum;
 byPortLen = bUnSingeLongToStr(srEthernetRec.usRemotePort,baPort); 
 vdDebug_LogPrintf(". Set Port (%d)  baPort =(%s) byPortLen =(%d)",inPriTxnHostPortNum,baPort,byPortLen);

 shRet = CTOS_EthernetConnectEx(srEthernetRec.strRemoteIP, byIpLen, baPort, byPortLen);
 
 CTOS_LCDTPrintXY(1, 8, "Connecting...	 ");
 if (shRet != d_OK)
 {
	 vdDebug_LogPrintf(". CTOS_EthernetConnectEx Err=(%04x)",shRet);

 }
 
 vdDebug_LogPrintf(". CTOS_EthernetConnectEx Ret=(%04x)",shRet);

 return shRet;
}

 
int inEthernet_SendData(void)
{
	USHORT shRet;

 ULONG ulRultSendLen;
 BYTE bySendTemp[1048];
 int ulSendLen = 232;
 byte uszSendData[2048] = "600004000002003020078020C0020400000000000000011100000900500002000400355264710003795113D190722100000668000F303930303030313134303130303038383838383030303101465F2A0207025F340102820238008407A0000000041010950500000080009A031501279C01009F02060000000001119F03060000000000009F090200029F10120110A08003220000E01D00000000000000FF9F1A0207029F1E0831323334353637389F26082CFD08188B5964639F2701809F3303E0B0C89F34031E03009F3501229F360201CC9F3704D7422F239F41030000010006303030303038";
 
 //gcitra
	 unsigned char szTmp[25];
	 unsigned char szHex[25];
	 
 
 /*
	 {
		 vdMyEZLib_LogPrintf(". Ethernet Send Len%d",ulSendLen);
		 bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
		 bySendTemp[1] = (ulSendLen & 0x000000FF);
		 vdMyEZLib_LogPrintf((". *DataHead == H*"));
		 vdMyEZLib_LogPrintf((". %02x%02x"),bySendTemp[0],bySendTemp[1]);
	 }
   */ 
	 //if(strCPT.inIPHeader == 1)
		 sprintf((char *) szTmp, "%04X", (unsigned int)ulSendLen);
	 //else
	//	 sprintf((char *) szTmp, "%04d", (int)ulSendLen);
	 
		 wub_str_2_hex(szTmp,szHex,4);
		 memcpy(bySendTemp, szHex, 2);
 //gcitra  
 
 ulRultSendLen = ulSendLen;
 
 //memcpy(&bySendTemp[2], uszSendData, ulSendLen);
 wub_str_2_hex(uszSendData,&bySendTemp[2],ulSendLen*2);
 
 ulRultSendLen += 2;
 
 
 shRet = CTOS_EthernetFlushRxData();
 
 CTOS_TimeOutSet(TIMER_ID_1, 200);
 while(1)
 {
#ifdef _TRANS_CONNECT_USE_RS232_   
	 if(CTOS_RS232TxReady(DBG_PORT) == d_OK)
	 {
		shRet = CTOS_RS232TxData(DBG_PORT, uszSendData, ulSendLen);
		vdMyEZLib_LogPrintf(". CTOS_RS232TxData Ret=(%04x)",shRet);
		 if (shRet == d_OK)
			 break;
	 }
#else        
	 shRet = CTOS_EthernetTxReady(); // Wait Ethernet ready to transmit data to remote
	 if (shRet == d_OK)
	 {
		 shRet = CTOS_EthernetTxData(bySendTemp, ulRultSendLen); // Ethernet transmit data to remote
		  //vdMyEZLib_LogPrintf(". CTOS_EthernetTxData Ret=(%04x)",shRet);
		 if (shRet == d_OK)
			 break;
	 }
#endif
  
	 if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
	 {
		 CTOS_LCDTPrintXY(1, 8, "Tx Data Fail	 ");
		 CTOS_Delay(1000);
		 shRet = ST_ERROR;

		 break;
	 }
	 
 }
 vdPCIDebug_HexPrintf("SendData",bySendTemp,ulRultSendLen);
 vdDebug_LogPrintf("**inEthernet_SendData END**");
 return shRet;
}

 
 int inEthernet_ReceiveData(void)
 {
	 DWORD dwStatus;
	 int inRealLen;
	 USHORT usMaxRecLen;
	 BYTE usTempRecData[1024];
	 BYTE uszRecData[2048];
	 USHORT shRet;
	 
	 vdDebug_LogPrintf("**inEthernet_ReceiveData START**");
	 usMaxRecLen = 0;
	 inRealLen = 0;
	 dwStatus = 0;
	 short inLen = 0;
	 
	 memset(usTempRecData,0,sizeof(usTempRecData));
	 CTOS_TimeOutSet(TIMER_ID_1, 6000); //------ 1500 to 6000 -Meena 07/03/13
	 
	 while(1)
	 {	  
		 /* Check if sockect disconnect or LAN disconnect, ROOTFS38 ok, ROOTFS35 nok */ 
		 shRet = CTOS_EthernetStatus(&dwStatus);
		 vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d) 1dwStatus=[%X]",shRet, dwStatus);
		 if(((d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE) & dwStatus) != (d_STATUS_ETHERNET_CONNECTED | d_STATUS_ETHERNET_PHYSICAL_ONLINE))
		 {
			 inRealLen = ST_ERROR;
			 break;
		 }
		 
		 /* Check if the data is currently availabl in Ethernet */
		 shRet = CTOS_EthernetRxReady(&usMaxRecLen);
 
		 //vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d)",shRet);
		 if (shRet == d_OK && usMaxRecLen>=2){
			 
			 //vdMyEZLib_LogPrintf(". Bef RxData usMaxRecLen(%d)",usMaxRecLen);
			 /* Receive data via Ethernet channel */
			 shRet = CTOS_EthernetRxData(&usTempRecData[inRealLen],&usMaxRecLen);
			 
			 //vdMyEZLib_LogPrintf(". Aft RxData usMaxRecLen(%d)",usMaxRecLen);
 
			 vdDebug_LogPrintf("CTOS_EthernetRxReady=(%d) usMaxRecLen[%d]",shRet, usMaxRecLen);
			 if(shRet != d_OK)
			 {
				 inRealLen = ST_ERROR;
				 //vdMyEZLib_LogPrintf(". CTOS_EthernetRxData shRet=(%04x)",shRet);
				 usMaxRecLen =0;
			 }
			 else if(usMaxRecLen>=2)
			 {
				 //vdMyEZLib_LogPrintf(". Aft RxData usMaxRecLen(%d)",usMaxRecLen);
				 inRealLen =  inRealLen+usMaxRecLen;
				 //vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData length received=(%d)",inRealLen);
				 if(inLen == 0)
				 {
					 inLen = ((usTempRecData[0] / 16 * 10) + usTempRecData[0] % 16) * 100;
					 inLen += (usTempRecData[1] / 16 * 10) + usTempRecData[1] % 16;
				 }
				 
				 
				 //vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData length send by host=(%d)",inLen);
				 if(inRealLen >= inLen)
				 {
				 memcpy(uszRecData,&usTempRecData[2],inRealLen);
				 //shRet = CTOS_EthernetFlushRxData();
				 //vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
				 break;
				 }
			 }
 
		 }
 
 /*
		 CTOS_KBDHit(&key);
		 if (key == d_KBD_CANCEL){
			 usRealLen = ST_ERROR;
			 vdMyEZLib_LogPrintf(". Keb Return");
			 shRet = CTOS_EthernetFlushRxData();
			 vdMyEZLib_LogPrintf(". CTOS_EthernetFlushRxData shRet=(%04x)",shRet);
			 vdDisplayErrorMsg(1, 8, "Cancel");
			 return ST_ERROR;
		 }
 */ 	   
		 
		 if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
		 {
			 CTOS_EthernetFlushRxData();
			 vdDebug_LogPrintf("Ethernet Connect TimeOut");
			 //vdDisplayErrorMsg(1, 8, "No Response");
			 //CTOS_Delay(1000);
			 return ST_ERROR;
		 }
		 
 
	 }

	 vdPCIDebug_HexPrintf("ReceiveData",uszRecData,inRealLen);
	 vdDebug_LogPrintf("**inEthernet_ReceiveData END**");
	 return inRealLen;
 }


 
 int inEthernet_Disconnection(void)
{
	USHORT shRet;
	vdDebug_LogPrintf("Disconnect,inEthernetOpen=[%d]",inEthernetOpen);

	if (1 ==inEthernetOpen)
	 {
		 shRet = CTOS_EthernetDisconnect();
		 //inEthernetOpen= 0;
	 }
	 
	 vdDebug_LogPrintf(". CTOS_EthernetDisconnect Ret=(%04x)",shRet);
	 return d_OK;
}

int inEthernet_CutOffConnection(void)
{
	USHORT shRet;
	vdDebug_LogPrintf("inEthernet_CutOffConnection,inEthernetOpen=[%d]",inEthernetOpen);

   if (1 ==inEthernetOpen)
   	{
		shRet = CTOS_EthernetClose();
		inEthernetOpen= 0;
   	}
    
    vdDebug_LogPrintf(". CTOS_EthernetClose Ret=(%04x)",shRet);
    vdDebug_LogPrintf("**inEthernet_CutOffConnection END**");
    return shRet;
}

int inTestTrans()
{
	inEthernet_InitEthernet();

	//inEthernet_GetConfigFromTable();

	//inEthernet_SetConnectConfig();

	inEthernet_ConnectHost();

	inEthernet_SendData();

	inEthernet_ReceiveData();

	inEthernet_Disconnection();

	//inEthernet_CutOffConnection();
}




int inEthernet_GetTerminalIP(char *szTerminalIP)
{
	BYTE bLength;
	int result;
	BYTE tmpbabuff[d_BUFF_SIZE];

	memset(tmpbabuff,0x00,sizeof(tmpbabuff));
	bLength = sizeof(tmpbabuff);
	result = CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, tmpbabuff, &bLength);
    vdDebug_LogPrintf("inEthernet_CheckComm =[%d][%s]",result,tmpbabuff);
    strcpy(szTerminalIP, tmpbabuff);
	return ST_SUCCESS;

}
#endif

int inEthernet_InitTerminal(void)
{
	inEthernet_InitEthernet();
	return d_OK;
}


/* TCP_Close_with_Socket
�ϲ�֮ǰ��close socket����
*/
int TCP_Close_with_Socket( int* pSocket )
{
    if( pSocket == NULL )
    {
        vdDebug_LogPrintf("Invalid socket handle");
        return COMM_ERR_PARAM_INVALID;
    }
    vdDebug_LogPrintf("Try to closesocket[handle=%d]....", (*pSocket));
    if( (*pSocket) <= 0)
        return COMM_ERR_NOT_CONNECT ;

    errno = 0;
    //socketclose ((*pSocket));
	shutdown((*pSocket),2);
    vdDebug_LogPrintf("closesocket[handle=%d] ...[errno=%d]", (*pSocket), errno);
    (*pSocket) = 0;
    return COMM_OK;
}


int TCP_ServerCloseSocket( void )
{
#ifdef UNUSESSL 
    if (1 == shGetUseSSL())
    {
        if (ssl_server != NULL)
        {
            //cannot SSL_shutdown, it will cannot connect second time.
//          SSL_shutdown(ssl_server);
            SSL_free(ssl_server);
            ssl_server = NULL;
        }
    }
#endif
    return TCP_Close_with_Socket(&( g_NetStatus.hServer )); // �ϲ�����, 01.04.08.c
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: TCP_Send_with_Socket.	// ��ĺ�����ϲ�send����, 01.04.08.a
* DESCRIPTION:  ����Socket���
* PARAMETERS:
*               sock        ָ��ʹ�õ�socket
*               szData		�������
*				SendLen		��ݳ���
*				SndDataFmt	��ݸ�ʽ
* RETURN:       COMM_OK -	����Socket��ݳɹ�
*               ����    -  �ο������붨��
* NOTES:
 * ------------------------------------------------------------------------ */
int TCP_Send_with_Socket( int sock, void *SndData, int SndLen, int Fmt )
{
    short nBytes;
    byte sbuf[ MAX_DATA_LEN + 5 ];
    byte Lrc = 0;
    int i , slen ;
    unsigned char szTmp[25];
    unsigned char szHex[25];

    memset(szTmp, 0x00, sizeof(szTmp));
    memset(szHex, 0x00, sizeof(szHex));
    vdDebug_LogPrintf( "TCP_Send [Socket Handle=%d] ....", sock );

    if(sock <= 0)
        return COMM_ERR_NOT_CONNECT ;

    if(SndData == NULL || SndLen <= 0 )
        return COMM_ERR_PARAM_INVALID;

    memset( sbuf, 0 , sizeof( sbuf ) );
    slen = 0 ;
    switch( Fmt )
    {
    case COMM_FMT_STX_HEXLEN_DATA_LRC_ETX:
        sbuf[slen++] = STX ;
        sbuf[slen++] = SndLen / 256;
        sbuf[slen++] = SndLen % 256;
        memcpy( sbuf + slen, SndData, SndLen );
        slen += SndLen;
        for( i = 0, Lrc = 0; i < SndLen; i++ )
            Lrc ^= *(( byte * )SndData + i );
        sbuf[slen++] = Lrc;
        sbuf[slen++] = ETX;
        break ;
    case COMM_FMT_HEXLEN_DATA:
        sbuf[slen++] = SndLen / 256;
        sbuf[slen++] = SndLen % 256;
        memcpy( sbuf + slen , SndData , SndLen );
        slen += SndLen ;
        break ;
    case COMM_FMT_BCDLEN_DATA:
        sprintf((char *) szTmp, "%04d", SndLen);
        //SVC_DSP_2_HEX((char *) szTmp ,(char *) szHex , 2);
		wub_str_2_hex((char *)szTmp ,(char *)szHex, 4);
        memcpy(sbuf, szHex, 2);
        slen=slen+2;
        
        memcpy( sbuf + slen , SndData , SndLen );
        slen += SndLen ;
        break ;
    default:
        memcpy( sbuf + slen , SndData , SndLen );
        slen += SndLen ;
        break ;
    }
    errno = 0;
#ifdef UNUSESSL	
    if (1 == shGetUseSSL())
        nBytes = SSL_write(ssl_server, (char *)sbuf, slen); 
    else
#endif		
        nBytes = send(sock,(char *)sbuf,slen,0);
    vdDebug_LogPrintf( " socket send retval=%d ...", nBytes);

    if(nBytes != slen)
    {
        vdDebug_LogPrintf( "Socket Send failed![ret=%d errno=%d]",nBytes,errno);
        return COMM_ERR_SEND_PARTIAL ;
    }
    // TCP do NOT trace HEX(so many trace will make process slowly and *PCAP also can output trace), Simon, 20130829
    // LOG_PRINTF_HEX("Send data", (char *)sbuf, slen);
    return COMM_OK ;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: TCP_ServerSend.
* DESCRIPTION:  ����Socket���(Server)
* PARAMETERS:
*               szData		�������
*				SendLen		��ݳ���
*				SndDataFmt	��ݸ�ʽ
* RETURN:       COMM_OK -	����Socket��ݳɹ�
*               ����    -  �ο������붨��
* NOTES:
 * ------------------------------------------------------------------------ */
int TCP_ServerSend( void *SndData, int SndLen, int Fmt )
{
    return TCP_Send_with_Socket( g_NetStatus.hServer, SndData, SndLen, Fmt );
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: TCP_Send.
* DESCRIPTION:  ����Socket���(Client)
* PARAMETERS:
*               szData		�������
*				SendLen		��ݳ���
*				SndDataFmt	��ݸ�ʽ
* RETURN:       COMM_OK -	����Socket��ݳɹ�
*               ����    -  �ο������붨��

* NOTES:
 * ------------------------------------------------------------------------ */
int TCP_Send( void *SndData, int SndLen, int Fmt )
{
    return TCP_Send_with_Socket( g_NetStatus.hSocket, SndData, SndLen, Fmt );
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: TCP_Recv_with_Socket.// ��ĺ�����ϲ�recv����, 01.04.08.a
 * DESCRIPTION:  ����Socket���
 * PARAMETERS:
 *              sock        ָ��ʹ�õ�socket
 *              RecvData    �������
 *              maxLen      �ɽ���������������
 *              RcvLen      ʵ�ʽ�����ݳ���
 *              Fmt         ��ݸ�ʽ
 *              TimeOutSec  ���ճ�ʱ����
 *              isTCD       �Ƿ���ʾ����ʱ��0����ʾ������ֵ��ʾ
 * RETURN:      COMM_OK -  ����Socket��ݳɹ�
 *              ����    -  �ο������붨��
 * NOTES:
 * ------------------------------------------------------------------------ */
int TCP_Recv_with_Socket( int sock, void *RecvData, int maxRevSize, int *RcvLen,int Fmt,int TimeOutSec, int isTCD)
{
    int retVal;
	struct timeval timeout;
    // struct timeval timeout;	// �޸�Vx675����ʡ��ģʽ��*EE=1(*PM2)�����շ��ص����⡣
    int MaxRevLen = 0;
    int xlen = 0;
    int i;
    unsigned long trytimer;
    int iTryTimes;
    byte Lrc ;
    unsigned char szTmp[25];
    unsigned char szHex[25];
    unsigned char tmp[MAX_DATA_LEN];
    long ls, lt;	// For TCD
    *RcvLen = 0 ;
    if(sock <= 0)
        return COMM_ERR_NOT_CONNECT ;

vdDebug_LogPrintf("RecvData=[%s],maxRevSize=[%d],Fmt=[%d],TimeOutSec=[%d]",RecvData,maxRevSize,Fmt,TimeOutSec);
    if(RecvData == NULL || maxRevSize < 0)
        return COMM_ERR_PARAM_INVALID;

	timeout.tv_sec = TimeOutSec;
	timeout.tv_usec = 100;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    // �޸�ʽ���գ�ֱ�ӷ��أ������ó�ʱ
    if (Fmt == COMM_FMT_NOFMT)
    {
        // check and set MAX size, Simon
        MaxRevLen = maxRevSize < MAX_DATA_LEN ? maxRevSize : MAX_DATA_LEN;

        //�յ����  // MAX_DATA_LEN -> MaxRevLen , by simon
      //  retVal = recv(sock, (char *)tmp, MaxRevLen, MSG_DONTWAIT);
#ifdef UNUSESSL      
        if (1 == shGetUseSSL())
            retVal = SSL_read(ssl_server, (char *)tmp, MaxRevLen - 1);
        else
#endif			
            retVal = recv(sock, (char *)tmp, MaxRevLen, 0);    
       if(retVal <= 0)
       {
              vdDebug_LogPrintf("Error Reading from SSL");
              return COMM_ERR_PARAM_INVALID;
       }
        if (retVal > 0)
        {
            vdDebug_LogPrintf("recv()ed [ret=%d errno=%d ]", retVal, errno);
            *RcvLen = retVal ;
            memcpy(RecvData, tmp, *RcvLen);
            return COMM_OK ;
        }
        //����Ҷ�
        else if (retVal==0)
        {
            vdDebug_LogPrintf("Host closed [ret=%d errno=%d ]", retVal, errno);
            return COMM_ERR_HOST_CLOSED;
        }
        else
        {
            //����ݣ����
            if (errno == EWOULDBLOCK)
            {
                vdDebug_LogPrintf("recv() no data [ret=%d errno=%d ]", retVal, errno);
                return COMM_OK;
            }
            else
            {
                vdDebug_LogPrintf("recv() failed [ret=%d errno=%d ]", retVal, errno);
                return COMM_ERR_RECV_FAILED;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    MaxRevLen = maxRevSize ;
    if ((Fmt == COMM_FMT_HEXLEN_DATA) || (Fmt == COMM_FMT_BCDLEN_DATA))
        MaxRevLen += 2;
    else if(Fmt == COMM_FMT_STX_HEXLEN_DATA_LRC_ETX)
        MaxRevLen += 5;
    else    // add else, 01.04.08.b
        return COMM_ERR_PARAM_INVALID;
    vdDebug_LogPrintf( "recv data fmt=[%d], MaxRevLen=[%d], timer=[%d]", Fmt, MaxRevLen, TimeOutSec);

	CTOS_TimeOutSet (TIMER_ID_1 , TimeOutSec*100);
    // ʹ��ticket����timeout 01.04.09.c, by Simon, 2013-5-10
    // trytimer = read_ticks()+TimeOutSec*TICKS_PER_SEC; //01.04.09.c����
    //trytimer = set_itimeout(NO_CLOCK_HANDLE TimeOutSec, TM_SECONDS);
    vdDebug_LogPrintf( "set_itimeout with[%d] return[%ul]", TimeOutSec, trytimer );
    xlen = 0 ;
    iTryTimes = 0;
    //ls = read_ticks( );	// For TCD
    lt = 0;	// For TCD
    
    do
    {
        retVal = recv(sock,(char *)((char *)tmp+xlen), MaxRevLen-xlen,0);

        // return when the remote socket closed
        if ( (retVal < 0 && errno >= 701 && errno <= 704) || (retVal == 0))
        {
            vdDebug_LogPrintf("Socket failed retval[ret=%d errno=%d ]",retVal,errno);
            return COMM_ERR_RECV_FAILED;
        }

        vdDebug_LogPrintf( "xlen=[%d]",xlen);
        vdDebug_LogPrintf( "Socket recv retval=[%d]... errno=[%d]",retVal,errno);
        if (retVal > 0)
        {
            xlen += retVal;

            // first time get the data, calculate the length by header
            if ((iTryTimes == 0) && (xlen>3))
            {
                iTryTimes++;
                if (Fmt == COMM_FMT_HEXLEN_DATA)
                {
                    vdDebug_LogPrintf("Get Length buf[0x%02x 0x%02x]",tmp[0],tmp[1]);
                    MaxRevLen = tmp[0]*256 + tmp[1] + 2;
                }
                else if (Fmt == COMM_FMT_BCDLEN_DATA)
                {
                    memcpy(szHex,tmp,2);
                    //SVC_HEX_2_DSP((char *) szHex ,(char *) szTmp , 2);
					wub_hex_2_str((char *) szHex ,(char *) szTmp , 2);
                    MaxRevLen = atoi((char *)szTmp);
                    vdDebug_LogPrintf( "len=[%d] Get Length buf[0x%02x 0x%02x],MaxRevLen=[%d]", retVal, tmp[0], tmp[1],MaxRevLen);
 
                }
                else if (Fmt == COMM_FMT_STX_HEXLEN_DATA_LRC_ETX)
                {
                    vdDebug_LogPrintf("Get Length buf[0x%02x 0x%02x]",tmp[1],tmp[2]);
                    MaxRevLen = tmp[1]*256 + tmp[2] + 5;
                }

                MaxRevLen = (MaxRevLen<MAX_DATA_LEN)?MaxRevLen:MAX_DATA_LEN;
            }
        }

        if( retVal <= 0  && xlen >= 0 && Fmt == COMM_FMT_NOFMT && iTryTimes++ > 1 )     // WiFi test, Modify by Simon, 2012-9-12
        {
            vdDebug_LogPrintf("Socket Recv retval[ret=%d trytime=%d errno=%d ]",retVal,iTryTimes,errno);
            break;
        }

        if(xlen >= MaxRevLen)
            break ;
      //  if ( isTCD ) VCommUtils_DisplayTimeCountDown( TimeOutSec, ls, &lt );	// show TCD
        CTOS_Delay(10);// ��Ϊȡ����recv��1���ӳ�ʱʱ�䣬�����ʵ��ӳ��ȴ�ʱ��// ���ʱ��̫����Ӱ���������׵�ʱ��

		if(xlen == 0 && CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            return COMM_ERR_RECV_TIMEOUT;
    }
    // ʹ��ticket����timeout    // 01.04.09.c, Modify by Simon, 2013-5-10
    while(1);  //ʹ�ú궨�����09.c���룬 01.04.09.d, Modify by Simon, 2013-5-14

    //LOG_PRINTFHEX(("READ DATA FROM TCP:",(char *)RecvData,xlen));
    // if(xlen == 0 && !CHK_TIMEOUT(-1,trytimer))
    //if(xlen == 0 && !CHECK_TIMEOUT(trytimer)) //ʹ�ú궨�����09.c���룬 01.04.09.d, Modify by Simon, 2013-5-14
    //{
    //    return COMM_ERR_RECV_TIMEOUT;
    //}

    retVal = xlen;

    switch(Fmt)
    {
    case COMM_FMT_HEXLEN_DATA:
        MaxRevLen = tmp[0]*256 + tmp[1];
        vdDebug_LogPrintf( "len=[%d] Get Length buf[0x%02x 0x%02x],MaxRevLen=[%d]", retVal, tmp[0], tmp[1],MaxRevLen);

        if(MaxRevLen != retVal-2)
            return COMM_ERR_RECV_FAILED;

        *RcvLen = retVal-2 ;
        memcpy(RecvData, tmp+2, *RcvLen);

        break ;
    case COMM_FMT_BCDLEN_DATA:
        memcpy(szHex,tmp,2);
        //SVC_HEX_2_DSP((char *) szHex ,(char *) szTmp , 2);
		wub_hex_2_str((char *) szHex ,(char *) szTmp , 2);
        MaxRevLen = atoi((char *)szTmp);
        vdDebug_LogPrintf( "len=[%d] Get Length buf[0x%02x 0x%02x],MaxRevLen=[%d]", retVal, tmp[0], tmp[1],MaxRevLen);
        if(MaxRevLen != retVal-2)
            return COMM_ERR_RECV_FAILED;

        *RcvLen = retVal-2 ;
        memcpy(RecvData, tmp+2, *RcvLen);

        break ;
    case COMM_FMT_STX_HEXLEN_DATA_LRC_ETX:
        if(tmp[0] != STX)
        {
            vdDebug_LogPrintf( "Socket Recv failed![STX=0x%02X]", tmp[0]);
            return COMM_ERR_RECV_FAILED;
        }
        vdDebug_LogPrintf( "Get Length buf[0x%02x 0x%02x]",tmp[1],tmp[2]);
        MaxRevLen = tmp[1]*256 + tmp[2];

        if(tmp[MaxRevLen+4] != ETX)
        {
            vdDebug_LogPrintf( "Socket Recv failed![ETX=0x%02X]", tmp[MaxRevLen+4]);
            return COMM_ERR_RECV_FAILED;
        }
        for ( i = 0 ,Lrc = 0; i < MaxRevLen ; i++)
        {
            Lrc ^= *((byte *)tmp+3+i);
        }
        if(tmp[MaxRevLen+3] != Lrc)
        {
            vdDebug_LogPrintf( "Socket Recv failed![calc LRC=0x%02X][Rcv LRC=0x%02X]", Lrc,tmp[MaxRevLen+3]);
            return COMM_ERR_RECV_FAILED;
        }
        *RcvLen = retVal-5;
        memcpy(RecvData, tmp+3, *RcvLen);
        break ;
    default:
        *RcvLen = retVal ;
        memcpy(RecvData, tmp, *RcvLen);
        break ;
    }
    // TCP do NOT trace HEX(so many trace will make process slowly and *PCAP also can output trace), Simon, 20130829
    // LOG_PRINTF_HEX("Recv data", (char *)RecvData, *RcvLen);
    return COMM_OK ;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: TCP_ServerRecv.
 * DESCRIPTION:  ����Socket���(Server)
 * PARAMETERS:
 *              RecvData    �������
 *              maxLen      �ɽ���������������
 *              RcvLen      ʵ�ʽ�����ݳ���
 *              Fmt         ��ݸ�ʽ
 *              TimeOutSec  ���ճ�ʱ����
 * RETURN:       COMM_OK -  ����Socket��ݳɹ�
 *               ����    -  �ο������붨��
 * NOTES:
* ------------------------------------------------------------------------ */
int TCP_ServerRecv( void *RecvData, int maxRevSize, int *RcvLen, int Fmt, int TimeOutSec )
{
    return TCP_Recv_with_Socket( g_NetStatus.hServer, RecvData, maxRevSize, RcvLen, Fmt, TimeOutSec, 0);
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: TCP_Recv.
 * DESCRIPTION:  ����Socket���(Client)
 * PARAMETERS:

 *              RecvData    �������
 *              maxLen      �ɽ���������������
 *              RcvLen      ʵ�ʽ�����ݳ���
 *              Fmt         ��ݸ�ʽ
 *              TimeOutSec  ���ճ�ʱ����
 * RETURN:       COMM_OK -  ����Socket��ݳɹ�
 *                ����    -  �ο������붨��
 * NOTES:
* ------------------------------------------------------------------------ */
int TCP_Recv( void *RecvData, int maxRevSize, int *RcvLen, int Fmt, int TimeOutSec )
{
    return TCP_Recv_with_Socket( g_NetStatus.hSocket, RecvData, maxRevSize, RcvLen, Fmt, TimeOutSec, 0 );
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: TCP_Recv_TCD.
 * DESCRIPTION:  ����Socket���, ֧�ֽ��յȴ���ʾ����ʱ
 * PARAMETERS:   RecvData   �������
 *              maxLen      �ɽ���������������
 *              RcvLen      ʵ�ʽ�����ݳ���
 *              Fmt         ��ݸ�ʽ
 *              TimeOutSec  ���ճ�ʱ����
 * RETURN:       COMM_OK -  ����Socket��ݳɹ�
*                ����    -  �ο������붨��
 * NOTES:
 *	Added by @laikey_20130113
 * ------------------------------------------------------------------------ */
int TCP_Recv_TCD( void *RecvData, int maxRevSize, int *RcvLen, int Fmt, int TimeOutSec )
{
    return TCP_Recv_with_Socket( g_NetStatus.hSocket, RecvData, maxRevSize, RcvLen, Fmt, TimeOutSec, 1 );
}
//////////////////////////////////////////////////////////////////////////
//
//				TCP SERVER FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////

/* --------------------------------------------------------------------------
* FUNCTION NAME: TCP_StartTCPServer.
* DESCRIPTION:  ����TCP��SERVER
* PARAMETERS:   CommType	 ͨѶ���ͣ�Ŀǰֻ֧��ETHERNET��GPRS
*				iListenPort  ����˿�
* RETURN:       COMM_OK						-	�����ɹ�
*				COMM_ERR_CREATE_SOCKET_FAILED-	Socket����ʧ��
* NOTES:
* ------------------------------------------------------------------------ */
int TCP_StartTCPServer(int CommType, int iListenPort)
{
    int ret = COMM_OK;
    int hSocket=-1;
    unsigned int iBufLen=0;
    char sLocalIp[50]= {0};
    struct sockaddr_in	sockHost;
    struct linger	t_linger;
    int flags;

    vdDebug_LogPrintf( "Socket--StartTcpServer" );
    if(iServerListenPort == iListenPort && hServerSocket > 0)
        return COMM_OK ;
	
#ifdef UNUSESSL
    if (1 == shGetUseSSL())
    {
        InitOpenSSLServer();

        ret = setup_Server_ctx();
        if (VS_SUCCESS != ret)
            return ret;
    }
#endif

	//inEthernet_GetTerminalIP(sLocalIp);
    vdDebug_LogPrintf("sLocalIp[%s], iListenPort[%d]", sLocalIp, iListenPort);

    hSocket = socket (AF_INET, SOCK_STREAM, 0);
    if (hSocket < 0)
    {
        //ret = socketerrno(SOCKET_ERROR);
        vdDebug_LogPrintf( "Socket creation FAILED: handle[%d], errno[%d],[%d]", hSocket, errno, ret);
        return COMM_ERR_CREATE_SOCKET_FAILED;
    }
    else
    {
        vdDebug_LogPrintf("Socket created: [%d]", hSocket);
    }

    t_linger.l_onoff = 1;
    t_linger.l_linger = 5;
    setsockopt(hSocket, SOL_SOCKET, SO_LINGER, (char*) &t_linger, sizeof(struct linger) );

    memset (&sockHost, 0, sizeof (struct sockaddr_in));
    sockHost.sin_family = AF_INET;
    //sockHost.sin_addr.s_addr = htonl (inet_addr (sLocalIp));
	//sockHost.sin_addr.s_addr = inet_addr (sLocalIp);
	sockHost.sin_addr.s_addr = htonl(INADDR_ANY);
    sockHost.sin_port = htons (iListenPort);

	vdDebug_LogPrintf("INADDR_ANY: [%s]", INADDR_ANY);

    ret = bind(hSocket, (struct sockaddr *)&sockHost, sizeof(struct sockaddr_in));
    vdDebug_LogPrintf("bind [%s] to socket[%d], ret=%d",(char*)sLocalIp,hSocket,ret);
    if(ret < 0)
    {
        //ret = socketerrno(SOCKET_ERROR);
        vdDebug_LogPrintf( "Socket bind FAILED: handle[%d],LocalIp[%s] Port[%d] errno[%d],[%d]", hSocket, sLocalIp,iListenPort,errno, ret);
        //socketclose(hSocket);
		shutdown(hSocket,2);
        return COMM_ERR_BIND_SOCKET_FAILED;
    }

    ret = listen(hSocket,1);
    vdDebug_LogPrintf("listen [port:%d] at socket[%d] = %d",iListenPort,hSocket,ret);
    if( ret <0 )
    {
        //ret = socketerrno(SOCKET_ERROR);
        vdDebug_LogPrintf( "Socket listen FAILED: handle[%d],LocalIp[%s] Port[%d] errno[%d],[%d]", hSocket, sLocalIp,iListenPort,errno, ret);
        //socketclose(hSocket);
		shutdown(hSocket,2);
        return COMM_ERR_SERVER_LISTEN_FAILED;
    }
//need the block mode
#if 0	
    //set non-blocking
    flags = 1;
    //socketioctl(hSocket, FIONBIO, &flags);
	ioctl(hSocket, FIONBIO, &flags);
#endif
    hServerSocket = hSocket;
    iServerListenPort = iListenPort ;
    return COMM_OK ;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: TCP_StopTCPServer.
* DESCRIPTION:  ֹͣTCP��SERVER
* PARAMETERS:
* RETURN:       COMM_OK						-	ֹͣ�ɹ�
* NOTES:
* ------------------------------------------------------------------------ */
int TCP_StopTCPServer(void)
{
    int ret ;
    int Retry = 0;

    if(hServerSocket > 0)
    {
        do
        {
            //ret = socketclose(hServerSocket);
			ret = shutdown(hServerSocket,2);
            vdDebug_LogPrintf("closesocket[%d] ret[%d]", hServerSocket, ret);
            if( ret < 0 )
            {
                //ret = socketerrno(SOCKET_ERROR);
                vdDebug_LogPrintf( "closesocket FAILED: handle[%d],errno[%d],[%d]", hServerSocket,errno, ret);
            }
        }
        while (ret < 0 && Retry ++ < 10);
    }

    hServerSocket = -1;
    iServerListenPort = -1;
    return COMM_OK ;
}

int TCP_CheckServerAccepted(char *sRemoteClientIp,int *iRemoteClientPort)
{
    struct sockaddr_in cliAddr;
    int			ClientSocket=-1;
    socklen_t	cliAddrLen=sizeof(struct sockaddr_in);
	int iRet;
	int c_iBufLen = 512;  
    char recvBuf[512 + 1];

    //vdDebug_LogPrintf("CheckServerAccepted[%d]", hServerSocket);

    if(hServerSocket <= 0)
        return COMM_ERR_SERVER_NOT_START;

    ClientSocket = accept (hServerSocket, (struct sockaddr*) &cliAddr, (socklen_t *)&cliAddrLen );
	vdDebug_LogPrintf("Accept ClientSocket[%d] ....", ClientSocket);
    if(ClientSocket < 0)
        return COMM_ERR_NO_CLIENT_CONNECTED;

    vdDebug_LogPrintf("accept at server socket[%d], get new socket[%d]", hServerSocket, ClientSocket);

    g_NetStatus.hServer = ClientSocket; //�շ��������client��     // (Wifi) server handle, by Simon, 2012-9-11

    sprintf(sRemoteClientIp,"%s",inet_ntoa(cliAddr.sin_addr));
    *iRemoteClientPort = cliAddr.sin_port ;

    vdDebug_LogPrintf("Accept a Client IP[%s] Port[%d] Request....", sRemoteClientIp, cliAddr.sin_port);
	for (;;)  
    {
    	memset(recvBuf,0x00,sizeof(recvBuf));
        iRet = recv(ClientSocket, recvBuf, c_iBufLen, 0);
		vdDebug_LogPrintf("recv[%d] ....", iRet);
        if(iRet == 0)
        {  
            TCP_ServerCloseSocket();
			return d_NO;
        }  
        else if(-1 == iRet)  
        {  
            TCP_ServerCloseSocket();
			return d_NO;  
        }  
		if (iRet > 0)
		{
	        recvBuf[iRet] = '\0';  
	        vdDebug_LogPrintf("c_iBufLen[%d] recvBuf=[%s]....", c_iBufLen,recvBuf);
			break;
		}
    } 
	
    return COMM_OK ;
}

int	TCP_FromAndSendHTTP( void *SndData, int SndLen)
{
    char 	szSendBuff[3048],sztmpBuff[1000];
    char	*szSendPtr;
    int		inSendLen=0;
    int		inSendtmpLen=0;
    int inResult;
    int fmtflag=0;


    memset ( szSendBuff, 0, sizeof( szSendBuff ) );
    szSendPtr = szSendBuff;

    inSendtmpLen = strlen(szRespHttpHead1);
    memcpy ( szSendPtr, szRespHttpHead1, inSendtmpLen);
    szSendPtr += inSendtmpLen;
	memcpy ( szSendPtr, "\x0d\x0a", 2 );
	szSendPtr += 2;
	inSendLen += (inSendtmpLen+2);

    inSendtmpLen = strlen(szRespHttpHead2);
    memcpy ( szSendPtr, szRespHttpHead2, inSendtmpLen);
    szSendPtr += inSendtmpLen;
	memcpy ( szSendPtr, "\x0d\x0a", 2 );
	szSendPtr += 2;
	inSendLen += (inSendtmpLen+2);


    memset ( sztmpBuff, 0, sizeof( sztmpBuff ) );
    sprintf(sztmpBuff,szRespHttpHead3,strlen(SndData));
    vdDebug_LogPrintf("szRespHttpHead3 = [%s][%s]",szRespHttpHead3,sztmpBuff);
    inSendtmpLen = strlen(sztmpBuff);
    memcpy ( szSendPtr, sztmpBuff, inSendtmpLen);
    szSendPtr += inSendtmpLen;
	memcpy ( szSendPtr, "\x0d\x0a", 2 );
	szSendPtr += 2;
	inSendLen += (inSendtmpLen+2);

    inSendtmpLen = strlen(szRespHttpHead4);
    memcpy ( szSendPtr, szRespHttpHead4, inSendtmpLen);
    szSendPtr += inSendtmpLen;
	memcpy ( szSendPtr, "\x0d\x0a\x0d\x0a", 4 );
	szSendPtr += 4;
	inSendLen += (inSendtmpLen+2);

    memcpy (  szSendPtr,  SndData,  strlen(SndData) );
	inSendLen += SndLen;

    vdDebug_LogPrintf("inSendLen=[%d]szSendBuff=[%s]",strlen(szSendBuff),szSendBuff);
    inResult = TCP_ServerSend(szSendBuff,strlen(szSendBuff),fmtflag);
    vdDebug_LogPrintf("TCP_ServerSend = [%d]",inResult);

    return inResult;
}

void InitOpenSSLServer(void)
{
    if(!SSL_library_init())
    {
        vdDebug_LogPrintf("Init OpenSSL Failed");
        return;
    }

    OpenSSL_add_all_algorithms(); 

    SSL_load_error_strings();
}

int setup_Server_ctx(void)
{
    ctx_Server = SSL_CTX_new(SSLv23_server_method()); //??SSL???, ??SSL?????
    if(ctx_Server == NULL)
    {
        vdDebug_LogPrintf("Server context is invalid");
        return SSL_ERR_CTX_NEW;
    }

    vdDebug_LogPrintf("SSL_CTX_new %X", ctx_Server);


    SSL_CTX_set_verify(ctx_Server, SSL_VERIFY_PEER, NULL); //????,SSL_VERIFY_PEER???????

    if(SSL_CTX_use_certificate_file(ctx_Server, CERTF, SSL_FILETYPE_PEM) <= 0) //??????
    {    
        vdDebug_LogPrintf("Error loading certificate from file");
        return SSL_ERR_CTX_USE_CERT;
    }
    if(SSL_CTX_use_PrivateKey_file(ctx_Server, KEYF, SSL_FILETYPE_PEM) <= 0) //??????
    {
        vdDebug_LogPrintf("Error loading private key from file");
        return SSL_ERR_CTX_USE_KEY;
    }
    if(!SSL_CTX_check_private_key(ctx_Server)) //???????????
    {
        vdDebug_LogPrintf("Private key does not match the certificate public key");
        return SSL_ERR_CTX_CHECK_KEY;
    }
//    SSL_CTX_set_cipher_list(ctx_Server, "RC4-MD5");    

    vdDebug_LogPrintf("setup_client_ctx SUCCESSFULL");

    return COMM_OK;
}

int Set_SSL_server_new()
{
    int err;
    X509* client_cert;
    char* str;

    ssl_server = SSL_new(ctx_Server);
    vdDebug_LogPrintf("Set_SSL_server_new=[%x],[%x]",ssl_server,ctx_Server);
    if(ssl_server == NULL)
    {
        vdDebug_LogPrintf("Error Creating an SSL");
        return SSL_ERR_CREATE_SSL;
    }

    SSL_set_fd(ssl_server, g_NetStatus.hServer);
    err = SSL_accept(ssl_server);
    if(err <= 0)
    {
        SSL_free(ssl_server);
        ssl_server = NULL;
        vdDebug_LogPrintf("Error Accepting SSl Object");
        return SSL_ERR_ACCEPT_SSL;
    }

    vdDebug_LogPrintf("SSL_accept finished");
    vdDebug_LogPrintf("SSL connection using %s", SSL_get_cipher(ssl_server));
    client_cert = SSL_get_peer_certificate(ssl_server);
    if(client_cert != NULL)
    {
        vdDebug_LogPrintf("Client certificate:");
        str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
        vdDebug_LogPrintf("  subject: %s", str);
        str = NULL;
        str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
        vdDebug_LogPrintf("  issuer: %s", str);
        X509_free(client_cert);
    }
    else
    {
        vdDebug_LogPrintf("Client does not have certificate");
        return COMM_OK;
    }

    return COMM_OK;
}

void vdTCP_TestServer(void)
{
    int inResult = VS_ERR;
    char sRemoteClientIp[50];
    char iRemoteClientPort[10];
    char szRecvData[1000];
    int maxRevSize;
    int fmtflag;
	unsigned char key;
        
    memset(sRemoteClientIp,0x00,sizeof(sRemoteClientIp));
    memset(iRemoteClientPort,0x00,sizeof(iRemoteClientPort));
    memset(szRecvData,0x00,sizeof(szRecvData));

    CTOS_KBDBufFlush();
	
	//inTestTrans();

	inEthernet_InitTerminal();
    
    inResult = TCP_StartTCPServer(15,8800);
    vdDebug_LogPrintf("StartTCPServer = [%d]",inResult);

    while (1)
    {
        inResult = TCP_CheckServerAccepted(sRemoteClientIp,iRemoteClientPort);

        if (VS_SUCCESS == inResult)
        {
#ifdef UNUSESSL        
            if (1 == shGetUseSSL())
            {
                if (VS_SUCCESS != Set_SSL_server_new())
                {  
                    TCP_ServerCloseSocket();
                    return VS_SUCCESS;
                }
            }
#endif            
            memset(szRecvData,0x00,sizeof(szRecvData));
            fmtflag = 1;
            inResult = TCP_ServerRecv(szRecvData,1000,&maxRevSize,0,30);
            vdDebug_LogPrintf("TCP_ServerRecv = [%d],maxRevSize=[%d]",inResult,maxRevSize);
            vdDebug_LogPrintf("szRecvData=[%s]",szRecvData);

			//inTestTrans();
			
            
            TCP_FromAndSendHTTP("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?><TransactionResponse><SequenceNo>000001</SequenceNo><TransType>05</TransType><PAN>5411800000003447</PAN><ExpiryDate>1701</ExpiryDate><CardholderName>MC HKD MCS</CardholderName><EntryMode>01</EntryMode><IssuerID>02</IssuerID><RespCode>00</RespCode><RespText>APPROVAL      003350</RespText><RRN>412803367242</RRN><AuthCode>003350</AuthCode><OfflineFlag>N</OfflineFlag><ConversionIndicator>X</ConversionIndicator><DCCIndicator>0</DCCIndicator></TransactionResponse>",522);
            CTOS_Delay(1000);
            TCP_ServerCloseSocket();
        }

		//CTOS_KBDGet(&key);
        //if(key==d_KBD_CANCEL)
        //{
        //    TCP_ServerCloseSocket();
		//	TCP_StopTCPServer();
        //    return;
        //}
    }

	TCP_StopTCPServer();
	
	return;
}


int inCTTOS_TCPIPConnectAP(unsigned long ulPort, unsigned char *hostip,unsigned long ulistenPort)
{
    int sockType;
    int retVal;
	USHORT	usRtn;
	int sd, ret, type;
	struct sockaddr_in  sockHost;
	int 		sockHandle = -1;
	char szBuf[512+1];
	
	vdDebug_LogPrintf("inCTTOS_TCPIPConnectAP....");
	usRtn = CTOS_EthernetOpenEx();
    vdDebug_LogPrintf(". CTOS_EthernetOpenEx Ret=(%04x)",usRtn);
	
    vdDebug_LogPrintf("=====inTCPIP_ConnectHost=====");
    vdDebug_LogPrintf("inTCPIP_ConnectHost HOST IP = %s    PORT = %d,ulistenPort=[%d]", hostip, ulPort,ulistenPort);
    

    memset (&sockHost, 0, sizeof (struct sockaddr_in));

    sockHost.sin_family = AF_INET;
    memset(sockHost.sin_zero,0,8);
    sockHost.sin_addr.s_addr = inet_addr((char *)hostip);
    sockHost.sin_port = htons(ulPort);

    sockType = SOCK_STREAM;
    sockHandle = socket(AF_INET, sockType, 0);
	
    if (sockHandle < 0) {
		vdDebug_LogPrintf("Socket error is: %d", sockHandle);
        return d_NO;
    }
	
    vdDebug_LogPrintf("Socket handle is: %d", sockHandle);
    
    retVal = connect(sockHandle, (struct sockaddr*)&sockHost, sizeof(struct sockaddr_in));
	vdDebug_LogPrintf("Socket connect is: [%d]", retVal);

	memset(szBuf,0x00,sizeof(szBuf));
	sprintf(szBuf,"%ld",ulistenPort);
	retVal = send(sockHandle, szBuf, strlen(szBuf), 0);
	vdDebug_LogPrintf("send is: [%d],szBuf=[%s]", retVal,szBuf);

	if(retVal < 0)
	{
		shutdown(sockHandle, 2);
		return d_NO;
	}

	shutdown(sockHandle, 2);

	return d_OK;

}


