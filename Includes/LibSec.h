/* 
 * File:   Libsec.h
 * Author: 
 *
 * Created on 2015
 */

#ifndef ___CTOS_LIBSEC_H___
#define	___CTOS_LIBSEC_H___

#ifdef	__cplusplus
extern "C" {
#endif

/*Defines*/
typedef struct tagSecPinParam
{
	BYTE chPINMinLen;
	BYTE chPINMaxLen;
	BYTE chPINBlkType;
	BYTE chPINCipherType;
	
	BYTE chPINBypass;
	BYTE chTimeOut;

	BYTE szPAN[20+1];
	BYTE chPANLen;

	void (*OnGetPINDigit)(BYTE NoDigits);
	void (*OnGetPINCancel)(void);
	void (*OnGetPINBackspace)(BYTE NoDigits);
	void (*OnGetPINOtherKeys)(BYTE NoDigits);
}SEC_PIN_PAPAM;


/*APIs*/
USHORT usSEC_KeyWrite(USHORT usKeySet, USHORT usKeyIndex, USHORT usKeyLen, BYTE *pKeyVal);
USHORT usSEC_KeyCheck(USHORT usKeySet, USHORT usKeyIndex);
USHORT usSEC_KeyDelete(USHORT usKeySet, USHORT usKeyIndex);
USHORT usSEC_DES_ECB(BYTE bEncDec, USHORT usKeySet, USHORT usKeyIndex, BYTE bKeyLen, BYTE *baData, USHORT usDataLen, BYTE *baResult);
USHORT usSEC_DES_CBC(BYTE bEncDec, USHORT usKeySet, USHORT usKeyIndex, BYTE bKeyLen, BYTE *baICV, BYTE *baData, USHORT usDataLen, BYTE *baResult);
USHORT usSEC_MAC(USHORT usKeySet, USHORT usKeyIndex, BYTE bKeyLen, BYTE *baICV, BYTE *baData, USHORT usDataLen, BYTE *baMAC);
USHORT usSEC_GetPIN(USHORT usKeySet, USHORT usKeyIndex, SEC_PIN_PAPAM stPinParam, BYTE *pOUT_PinBlock);
USHORT usSEC_KeyWriteByKPK(USHORT usKPKSet, USHORT usKPKIndex, USHORT usKeySet, USHORT usKeyIndex, USHORT usKeyLen, BYTE *pKeyVal, BYTE *pIN_KCV);

USHORT usSEC_MAC_ECB(USHORT usKeySet, USHORT usKeyIndex, BYTE bKeyLen, BYTE *baData, USHORT usDataLen, BYTE *baMAC);
USHORT usSEC_GetPINNoPAN(USHORT usKeySet, USHORT usKeyIndex, SEC_PIN_PAPAM stPinParam, BYTE *pOUT_PinBlock);
USHORT ushSyncKSN(BYTE *CurrentKSN, BYTE *NewKSN, USHORT usKeySet, USHORT usKeyIndex);

#ifdef	__cplusplus
}
#endif

#endif	/* ___CTOS_LIBSEC_H___ */

