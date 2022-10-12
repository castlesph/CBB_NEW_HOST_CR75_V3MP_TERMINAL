
#ifndef _POSWAVE_H
#define	_POSWAVE_H

#ifdef	__cplusplus
extern "C" {
#endif

int inCTOS_WaveFlowProcess(void);
int inCTOS_WAVE_SALE(void);
int inCTOS_WAVE_REFUND(void);
int inCTOS_MultiAPReloadWaveData(void);
void vdCTOSS_SetWaveTransType(int type);
int inCTOSS_GetWaveTransType(void);
void vdCTOS_InitWaveData(void);
void vdCTOS_PartialInitWaveData(void);
int inCTOS_ALIPAY_SALE(void);
int inCTOS_AlipayFlowProcess(void);
int inCTOS_QRInquiry_SaleFlowProcess(void);
int inCTOS_ALIPAY_Reversal(void) ;
int inCTOS_AlipayReversalProcess(void) ;


#ifdef	__cplusplus
}
#endif

#endif	/* _POSCTLS_H */

