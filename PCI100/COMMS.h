

#ifndef _COMMS_H
#define	_COMMS_H

#ifdef	__cplusplus
extern "C" {
#endif


#ifndef BYTE
#define BYTE	unsigned char
#endif

#ifndef ULONG
#define ULONG	unsigned long		// 4byte
#endif



#define     STX                            0x02
#define     ETX                            0x03
#define     ACK                            0x06
#define     NAK                            0x15
#define     EOT                            0x04
#define     CAN                            0x18
#define     SI                             0x0f
#define     SO                             0x0e
#define     DLE                            0x10
#define     CIPHERBIT                      0x20


#define 	ERR_COM_OPEN                    (-10) /* port open failed                   */
#define 	ERR_COM_INIT                    (-11) /* Error setting port parameters      */
#define 	ERR_COM_CLOSE                   (-12) /* port close failed                  */
#define 	ERR_COM_SEND                    (-13) /* Write to Com port failed           */
#define 	ERR_COM_NAK                     (-14) /* Did not receive ACK for last write */
#define 	ERR_COM_READ                    (-15) /* Error setting port parameters      */
#define 	ERR_COM_WRITE                   (-16) /* Error setting port parameters      */
#define 	ERR_COM_TIMEOUT                 (-17) /* Error setting port parameters      */
#define 	ERR_COM_RECV                    (-18) /* Error setting port parameters      */


int inCTOSS_USBOpen(void);
int inCTOSS_USBSendBuf(char *szSnedBuf,int inlen);
int inCTOSS_USBRecvBuf(char *szRecvBuf,int *inlen);
int inCTOSS_USBClose(void);
int inCTOSS_USBRecvBufByte(char *szRecvBuf,int inlen);


int inCTOSS_RS232Open(ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits);
int inCTOSS_RS232SendBuf(char *szSnedBuf,int inlen);
int inCTOSS_RS232RecvBuf(char *szRecvBuf,int *inlen,int timeout);
int inCTOSS_RS232Close(void);
int inCTOSS_RS232RecvByte(char *szRecvBuf, int inlen);

int inCTOSS_V3PRS232RecvACK(char *szRecvBuf,int *inlen,int timeout);


void vdCTOSS_SetRS232Port(int CommPort);

void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg);
void vdCTOSS_RNG(BYTE *baResult);

void vdCTOSS_SetV3PRS232Port(int CommPort);
int inCTOSS_V3PRS232Open(ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits);
int inCTOSS_V3PRS232SendBuf(unsigned char *szSnedBuf,int inlen);
int inCTOSS_V3PRS232RecvBuf(unsigned char *szRecvBuf,int *inlen,int timeout);
int inCTOSS_USBRecvByte(char *szRecvBuf, int inlen);
int inCTOSS_V3PRS232RecvByte(unsigned char *szRecvBuf, int inlen);
int inCTOSS_V3PRS232Close(void);


int inCTOSS_USBHostOpenEx(ULONG ulBaudRate, BYTE bParity, BYTE bDataBits, BYTE bStopBits);
int inCTOSS_USBHostSendBufEx(unsigned char *szSnedBuf,int inlen);
int inCTOSS_USBHostRecvACKBufEx(unsigned char *szRecvBuf,int *inlen, int inTimeOut);
int inCTOSS_USBHostRecvBufEx(unsigned char *szRecvBuf,int *inlen, int inTimeOut);
int inCTOSS_USBHostRecvACKEx(char *szRecvBuf, int *inlen, int inTimeOut);
int inCTOSS_USBHostCloseEx(void);
int inCTOSS_USBHostFlushEx(void);

int openCOM(char *strComDevice);
int closeCOM(int fdcom);
int writeToCOM(int fdcom, unsigned char *wbuf, int length);
int readFromCOM(int fdcom, unsigned char *rbuf, int length);


#ifdef	__cplusplus
}
#endif

#endif	/* _COMMS_H */

