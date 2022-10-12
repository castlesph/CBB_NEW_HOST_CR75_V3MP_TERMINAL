/* 
 * File:   V5IsoFunc.h
 * Author: Administrator
 *
 * Created on 2012年8月6日, 下午 9:23
 */

#ifndef V5ISOFUNC_H
#define    V5ISOFUNC_H

#ifdef    __cplusplus
extern "C" {
#endif

#include "../Includes/POSTypedef.h"
#include "../Includes/ISOEnginee.h"


#define BIT_MAP_MAX_ARRAY    	30

#define BIT_MAP_SIZE        	8

#define ISO_SEND_SIZE       	2048
#define ISO_REC_SIZE        	2048


void vdDispTextMsg(char *szTempMsg);
void vdDecideWhetherConnection(TRANS_DATA_TABLE *srTransPara);
int inBuildAndSendIsoData(void);
int inSnedReversalToHost(TRANS_DATA_TABLE *srTransPara, int inTransCode);
int inSaveReversalFile(TRANS_DATA_TABLE *srTransPara, int inTransCode);
int inProcessReversal(TRANS_DATA_TABLE *srTransPara);
int inProcessAdviceTrans(TRANS_DATA_TABLE *srTransPara, int inAdvCnt);
int inProcessEMVTCUpload(TRANS_DATA_TABLE *srTransPara, int inAdvCnt);
int inPackSendAndUnPackData(TRANS_DATA_TABLE *srTransPara, int inTransCode);
int inBuildOnlineMsg(TRANS_DATA_TABLE *srTransPara);
int inSetBitMapCode(TRANS_DATA_TABLE *srTransPara, int inTransCode);
int inPackMessageIdData(int inTransCode, unsigned char *uszPackData, char *szMTI);
int inPackPCodeData(int inTransCode, unsigned char *uszPackData, char *szPCode);
void vdModifyBitMapFunc(int inTransCode, int *inBitMap);
int inCheckIsoHeaderData(char *szSendISOHeader, char *szReceISOHeader);
int inSendAndReceiveFormComm(TRANS_DATA_TABLE* srTransPara,
                                unsigned char* uszSendData,
                                 int inSendLen,
                                unsigned char* uszReceData);
int inProcessOfflineTrans(TRANS_DATA_TABLE *srTransPara);
int inAnalyseIsoData(TRANS_DATA_TABLE *srTransPara);

int inCheckHostRespCode(TRANS_DATA_TABLE *srTransPara);
int inCheckTransAuthCode(TRANS_DATA_TABLE *srTransPara);

int inAnalyseChipData(TRANS_DATA_TABLE *srTransPara);
int inAnalyseNonChipData(TRANS_DATA_TABLE *srTransPara);

int inAnalyseReceiveData(void);
int inAnalyseAdviceData(int inPackType);
void vdDiapTransFormatDebug(unsigned char *bSendBuf, int inSendLen);
int inPorcessTransUpLoad(TRANS_DATA_TABLE *srTransPara);

int inPackIsoFunc02(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc03(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc04(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc07(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData); //@@IBR ADD 20161019
int inPackIsoFunc11(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc14(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc15(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc18(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc22(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc23(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc24(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc25(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc26(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData); //@@IBR ADD 20161020
int inPackIsoFunc32(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc35(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc36(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData); //@@IBR ADD 20161025
int inPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc41(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc42(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc43(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc45(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc48(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc49(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData); //@@IBR ADD 20161020
int inPackIsoFunc52(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc54(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc56(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc60(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc61(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc62(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);
int inPackIsoFunc64(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData);

int inUnPackIsoFunc02(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc07(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc11(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc12(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc13(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc14(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc15(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc32(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc33(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc37(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc38(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc39(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc41(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc43(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc44(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc48(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc49(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc54(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc55(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc57(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc60(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf); //@@IBR ADD 20161125
int inUnPackIsoFunc62(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inUnPackIsoFunc63(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inPackISOEMVData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inPackISOPayWaveData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inPackISOJCBPayWaveData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inPackISOPayPassData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);
int inCTOS_PackDemoResonse(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData);
int inPackISOExpressPayData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);

void vdInitialISOFunction(ISO_FUNC_TABLE *srPackFunc);
int inBaseRespValidation(TRANS_DATA_TABLE *srOrgTransPara,TRANS_DATA_TABLE *srTransPara);
int inCheckReversalTrans(int byPackType);
int inMPU_GenMACBuffer(TRANS_DATA_TABLE *srTransPara, BYTE *baMACBuffer, int inSendRecv);
int inMPU_PackDE60Data(TRANS_DATA_TABLE *srTransPara, BYTE *pszMsg);

void vdPackISO_SetDateTimeForTrans(TRANS_DATA_TABLE *srTransPara);
int inMPU_PackISOEMVData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);

int inMPU_ProcessBatchUpload(void);

int inMPU_GenMACBuffer(TRANS_DATA_TABLE *srTransPara, BYTE *baMACBuffer, int inSendRecv);
int inProcessReversalEx(TRANS_DATA_TABLE *srTransPara);
int inSendAndReceiveAlipayFormComm(TRANS_DATA_TABLE* srTransPara,
        unsigned char* uszSendData,
        int inSendLen,
        unsigned char* uszReceData);

int inBuildStringISO(char *uzSrc, int inSrcLen, char *uzDest, int inPackType);
int inParseStringISO(char *uzSrc, char *uzDest, int inPackType);

void vdClearISO_DataElement(void);
int inPackISOQuickpassData(TRANS_DATA_TABLE *srTransPara, unsigned char *uszUnPackBuf);


#ifdef    __cplusplus
}
#endif

#endif    /* V5ISOFUNC_H */

