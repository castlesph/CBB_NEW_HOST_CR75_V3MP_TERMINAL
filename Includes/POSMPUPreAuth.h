/* 
 * File:   POSMPUPreAuth.h
 * Author: Computer
 *
 * Created on October 25, 2016, 3:04 PM
 */

#ifndef POSMPUPREAUTH_H
#define	POSMPUPREAUTH_H

#ifdef	__cplusplus
extern "C" {
#endif

int inCTOS_MPU_PREAUTH(void);
int inCTOS_MPU_PREAUTH_COMP(void);
int inCTOS_MPU_PREAUTH_COMP_ADVICE(void);
int inCTOS_MPU_VOID_PREAUTH(void);
int inCTOS_MPU_VOID_PREAUTH_COMP(void);
#ifdef	__cplusplus
}
#endif

#endif	/* POSMPUPREAUTH_H */

