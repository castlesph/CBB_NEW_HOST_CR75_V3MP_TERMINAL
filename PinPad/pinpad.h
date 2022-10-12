
#ifndef ___PINPAD_H___
#define	___PINPAD_H___

#ifdef	__cplusplus
extern "C" {
#endif

    int inInitializePinPad(void);
    void TEST_Write3DES_Plaintext(void);
    void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine);
    void OnGetPINDigit(BYTE NoDigits);
    void OnGetPINCancel(void);
    void OnGetPINBackspace(BYTE NoDigits);
    int inGetIPPPin(void);
    int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC);
    int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC);
    void vdCTOS_PinEntryPleaseWaitDisplay(void);
    int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex);
    int GetPIN_With_3DESDUKPT(void);
    void vdCTOS_MPUInjectKey(void); //@@IBR ADD 06102016
    void vdHardCodeCUP_FinexusUATKey(void);

    int inCheckStringMAC(BYTE *szDataIn, int inLengthIn, BYTE *szDataOut, int *inOutLen);
    void vdEncrypt3DES(BYTE Mode, BYTE *szMsg, int inMsgLen, BYTE *szKey, BYTE *DESResult, BYTE* szVektor);

    USHORT ushMPU_WriteEncPIK(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen);
    USHORT ushCBB_WriteEncTPK(BYTE *baTPK, BYTE byKeyLen);
    USHORT ushMPU_WriteEncMAK(BYTE *baMAK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen);
    void vdHardCodeCUP_UATKey(void);	

	USHORT ushIPP_WriteEncTPK(BYTE *baTPK, BYTE byKeyLen);
    void vdCTOS_IPPInjectKey(void);
    void vdCTOS_UPI_Finexus_InjectKey(void);
	
    void vdCTOSS_GetKEYInfo(USHORT ushKeySet, USHORT ushKeyIdx);
    
    USHORT ushCAV_WriteEncPIK(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen);

	#ifdef PIN_CHANGE_ENABLE
	USHORT ushCAV_WriteEncPIK2(BYTE *baPIK, BYTE byKeyLen, BYTE *baKCV, BYTE byKCVLen);
	USHORT ushCAVPCVISAMCCBB_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen);
	void vdHardCodeVISAMC_FinexusUATKey(void);
	void vdHardCodeCUP_FinexusUATKey2(void);
	USHORT ushCAVPCCBB_WriteEncTPK(BYTE *baTPK, BYTE byKeyLen);
	USHORT ushCAVPCCBB_WriteTMKPlain(BYTE *baTMK, BYTE byKeyLen);
	void vdCAVPCHardCodeCUP_UATKey(void);
	int inGetCAVIPPPin(void);
	int inCTOS_inGetDefaultIPPPin(void);
	int inCTOS_GetNewPIN(void);
	int inCTOS_GetVerifyPIN(void);
	int inCTOS_ValidatePINEntry(void);
	void vdCTOS_PinEntryScreenDisplay(BOOL fPinSequence);	
	int inCTOS_PinPadTypePinEntry(BOOL fNewPin);
	int inCTOS_inGetNewIPPPin(void);
	int inCTOS_inGetVerifyIPPPin(void);
	#endif

	#ifdef OK_DOLLAR_FEATURE	
	void vdHardCodedOKD_UATKey(void);	
	#endif

#ifdef	__cplusplus
}
#endif

#endif	/* ___PINPAD_H___ */

