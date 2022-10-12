
#ifndef __COMMCEHEAD___
#define __COMMCEHEAD___


#ifndef byte
#define byte	unsigned char
#endif

#define DISCONNECT_TIMEOUT		15
#define LOGOUT_TIMEOUT			30  // ��10��Ϊ20�룬ԭ��10����ڵȲ���event����� 01.04.09.icbc1
#define INIT_TIMEOUT			10
#define SEND_TIMEOUT			40
#define RECV_TIMEOUT			20

#define IDLE_TIME				10

#define ENV_PARAM_NAME			16
#define ENV_PARAM_VALUE			128

#define VX680_NW_HANDLE			3

#define CE_NWIF_MAX				10
#define CE_SUCCESS				0

#define MAX_DATA_LEN			8192
#define PHONE_LEN				32
#define APN_LEN					32
#define USER_LEN					32
//#define PWD_LEN					32
#define IPADDR_LEN				64
#define MACADDR_LEN				16	// for bluetooth base, [1.04.06.a]


#define COMM_TYPE_ETHERNET			31
#define COMM_TYPE_WIFI				32
#define COMM_TYPE_BTETH				33
#define COMM_TYPE_BTSDLC			34
#define COMM_TYPE_GPRS				35
#define COMM_TYPE_CDMA				36


#define COMM_OK  0


#define COMM_FMT_NOFMT 0
#define COMM_FMT_STX_HEXLEN_DATA_LRC_ETX    1
#define COMM_FMT_HEXLEN_DATA 2
#define COMM_FMT_BCDLEN_DATA 3


#define STX 		0x02
#define ETX 		0x03
#define VS_SUCCESS            0             /* General purpose error code */
#define VS_ERR              (-1)



#define COMM_ERR_MODE_WRONG  1001
#define COMM_ERR_HANDLE_INVALID  1002
#define COMM_ERR_CREATE_SOCKET_FAILED  1003
#define COMM_ERR_BIND_SOCKET_FAILED  1004
#define COMM_ERR_SERVER_LISTEN_FAILED  1005
#define COMM_ERR_SERVER_NOT_START  1006
#define COMM_ERR_NO_CLIENT_CONNECTED  1007
#define COMM_ERR_NOT_CONNECT  1008
#define COMM_ERR_PARAM_INVALID  1009
#define COMM_ERR_SEND_PARTIAL  1010
#define COMM_ERR_RECV_FAILED  1011
#define COMM_ERR_RECV_TIMEOUT  1012
#define COMM_ERR_HOST_CLOSED  1013

#define SSL_ERR_CTX_NEW  1014
#define SSL_ERR_CTX_USE_CERT  1015
#define SSL_ERR_CTX_USE_KEY  1016
#define SSL_ERR_CTX_CHECK_KEY  1017
#define SSL_ERR_CREATE_SSL  1018
#define SSL_ERR_ACCEPT_SSL  1019



typedef enum{
	ENV_STRING = 0,
	ENV_INT
}ENV_TYPE;

typedef enum{
	MDM_ASYNC	=0,
	MDM_SDLC
} SYNC_MODE;

typedef enum{
	MDM_INTERNAL =0,
	MDM_DONGLE	
} MDM_MODE;

typedef enum{
	CEEVENT_GOT_ERROR = -1,
	CEEVENT_GOT_NOEVENT,	
	CEEVENT_GOT_EVENT,
	CEEVENT_GOT_CANCELEVENT,
	CEEVENT_GOT_DOCKCONNECTION_DROP_EVENT,	//���ӵ����¼�(680), 1.04.05.c, add by jiangnan 2012/9/13 15:19:58
	CEEVENT_GOT_NETOUT,		// Added by @laikey_20130113
}GET_EVENT;

typedef enum{
	COMM_CNTS_NOT_CONNECT = 0,
	COMM_CNTS_CONNECTED,
	COMM_CNTS_PROCESSING,
} CONNECTSTAUTS;

// GPRS��Ӧ���л��������, Add start by Simon, 2012-9-18
typedef enum{
	COMM_PARAM_FULLSAME = 0,
	COMM_PARAM_HALFSAME,
	COMM_PARAM_DIFFERENT,
	COMM_PARAM_DIFPHONE,	// 01.05.01.a by Simon
} PARAM_COMPARED_TYPE;
// GPRS��Ӧ���л��������, Add end   by Simon, 2012-9-18

typedef struct {
	int hModem;
	int iDialState;		// state of the Dial connection
	int iRegState;		// network state
	int	hSocket;
	int hServer;	// for be server mode, Simon, [1.04.06.a]
}NetStatus;



int TCP_Send(void *SndData, int SndLen,int Fmt);
int TCP_Recv(void *RecvData, int maxRevSize, int *RcvLen,int Fmt,int TimeOutSec);
int TCP_Recv_TCD(void *RecvData, int maxRevSize, int *RcvLen,int Fmt,int TimeOutSec);	// Added by @laikey_20130113


//////////////////////////////////////////////////////////////////////////
//
//				TCP SERVER FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////
int TCP_StartTCPServer(int CommType, int iListenPort);
int TCP_StopTCPServer(void);
int TCP_CheckServerAccepted(char *sRemoteClientIp,int *iRemoteClientPort);
int TCP_ServerSend( void *SndData, int SndLen, int Fmt );
int TCP_ServerRecv( void *RecvData, int maxRevSize, int *RcvLen, int Fmt, int TimeOutSec );
int TCP_ServerCloseSocket( void );

int	TCP_FromAndSendHTTP( void *SndData, int SndLen);

void InitOpenSSLServer(void);
int setup_Server_ctx(void);
int Set_SSL_server_new(void);

void vdTCP_TestServer(void);


#endif

