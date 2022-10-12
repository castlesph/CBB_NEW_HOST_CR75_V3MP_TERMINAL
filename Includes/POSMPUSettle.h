/* 
 * File:   POSMPUSettle.h
 * Author: Computer
 *
 * Created on November 4, 2016, 9:33 AM
 */

#ifndef POSMPUSETTLE_H
#define	POSMPUSETTLE_H

#ifdef	__cplusplus
extern "C" {
#endif

int inCTOS_MPU_SETTLEMENT(void);
int inCTOS_MPU_BatchUpload(void);
int inCTOS_MPU_SETTLE_ALL(void);

int inMPU_CheckBatchUploadFlag(TRANS_DATA_TABLE *srTransPara);
void vdMPU_SetBatchUploadFlag(TRANS_DATA_TABLE *srTransPara, int inFlag);

#ifdef	__cplusplus
}
#endif

#endif	/* POSMPUSETTLE_H */

