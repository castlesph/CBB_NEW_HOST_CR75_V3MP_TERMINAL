#ifndef __POSDCC_H__
#define	__POSDCC_H__

typedef enum
{
    d_IPC_CMD_DCC_RATE_REQUEST		= 0x01,
	d_IPC_CMD_DCC_TRANS_LOGGING		= 0x02,

}IPC_DCC_CMD_TYPES;


USHORT usCTOSS_DCC_RateRequest(void);
USHORT usCTOSS_DCC_TransLogging(void);


#endif

