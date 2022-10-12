/* 
 * File:   POSIpp.h
 * Author: Computer
 *
 * Created on July 24, 2017, 4:48 PM
 */

#ifndef POSIPP_H
#define	POSIPP_H

#ifdef	__cplusplus
extern "C" {
#endif
    
void vdGetIPPSchemeID(char *szSchemeID);
void vdSetSettleIPPFlag(BOOL flag);
BOOL fIPPGetSettleFlag(void);

int inCTOS_IPP_INSTALLMENT(void);
int inCTOS_IPP_VOID(void);
int inCTOS_IPP_SETTLEMENT(void);
int inCTOS_IPP_SIGNON(void);

#ifdef	__cplusplus
}
#endif

#endif	/* POSIPP_H */

