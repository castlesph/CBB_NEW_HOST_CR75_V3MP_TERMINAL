ON NEW HOST VERSION 2004

DONE:
1. TERMINAL.S3DB 
 -the diff:   CDT (fMagSwipeEnable -Done) HDT(fTPDUOnOff - Done), TCT (DONE 03/23/2020)
2. DYNAMICMENU05 AND DYNAMICMENU04 (DONE 03/03/2020)
3. ISOENGINEE17, ISOENGINEE18, ISOENGINEE19 (DONE COPIED FROM NH TO OH 03/23/2020)
4. V5S_CBB.S3DB (Done - no changes for both - 03/23/2020)
5. TPDU - (DONE 03/23/2020)
7. ERM_ERROR_RECEIPT_FEATURE (NH) - DONE, ADDED TO OH APPLICATION (03-24-2020)
B. (DONe - 03-24-2020)
With iERS values for Auth and RRN error receipt.
	// RRN 
	strcpy(srTransRec.szRRN, "000000000000"); 
	
	// AuthCode 
	memset (szStr, 0x00, sizeof(szStr));	
	sprintf(szStr, "RC[%s]", srTransRec.szRespCode);		
	strcpy(srTransRec.szAuthCode, szStr); 

C.Passed CB/ MPU testing for change request below:
1. Send error receipt to iERS. (Done - 03-24-2020)

==================================================================================================


6. POS function and  Regulation, POS Function latest DOCS
REFER TO: C:\CastlesProject\CBB_NEW_HOST\DOCUMENTS PATH. (NH)

8. for MPU_NPS_TESTCASE_POS_DIRECT test case POSIC014 (NH)

9. CODE COMPARISON RESULT (FUNCTIONS TO CHECK)

 - POSBATCH.C (Done 03-24-2020 - for issue 562 	MK Merchant issue)
 - POSERM.C, POSERM.H (Done 03-24-2020)
 - POSFUNCTIONSLIST.C (Done 03-24-2020)
 - INCLUDES (POSMAIN.H) - (Done 03-24-2020)
 - INCLUDES (POSSETTING.H) - (Done 03-24-2020)
 - INCLUDES (POSTRANS.H) (Done 03-24-2020 (9F63))
 - INCLUDES (POSTYPEDEF.H) - (Done 03-24-2020)
===================================================================


 - POSAUTH.C (Done 03-24-2020)
differences FROM New Host:

inCTOS_PreAuthVoidFlowProcess
.....
	inRet = inCTOS_WaveGetCardFields();
	if(d_OK != inRet)
		return inRet;


inCTOS_PreAuthCompletionFlowProcess
.......

    vdDebug_LogPrintf("XXXXXXX - byTransType[%d]",srTransRec.byTransType);	

	// new adjustment for MPU new host application - 05/06/19 - based "POS function and  Regulation" doc
	if( srTransRec.HDTid == 17 ||
		srTransRec.HDTid == 18 ||
		srTransRec.HDTid == 19)
	{
		inRet = inCTOS_GeneralGetRRN();
		if(d_OK != inRet)
			return inRet;

		#if 0
		// to fix incorrect transtype after checking for RRN
    	vdCTOS_SetTransType(PREAUTH_COMP);
        vdDispTransTitle(PREAUTH_COMP);		
		#endif
	}

	vdDebug_LogPrintf("XXXXXXX2 - Resp RREF [%s]  AuthCode [%s]",srTransRec.szRRN, srTransRec.szAuthCode);	
	// new adjustment for MPU new host application - 05/06/19
	inRet = inMPU_GetAuthTime();
	if (d_OK != inRet)
    	return inRet;

 - POSTRANS.C - (Done 03-24-2020)
inCTOS_WaveGetCardFields
.....
line 2613
			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if(srTransRec.byTransType == REFUND)
			{
				vduiClearBelow(7);
                vduiClearBelow(8);
				setLCDPrint(7, DISPLAY_POSITION_CENTER, "PLEASE REMOVE CARD");
                setLCDPrint(8, DISPLAY_POSITION_CENTER, "AND DO MANUAL ENTRY");
                vduiWarningSound();
                CTOS_Delay(1500);	
				goto SWIPE_AGAIN;
			}


line 2800
			//for MPU new host flow implementation 05/07/2019 - based "POS function and  Regulation" doc
			if (srTransRec.byTransType == REFUND){
				//CTOS_LCDTClearDisplay();		
				CTOS_LCDTPrintXY(1, 8, "PLEASE ENTER CARD NO.");
				CTOS_Beep();
				CTOS_Delay(1000);
			}

line 6406
		// for new MPU host application upgrade - based "POS function and  Regulation" doc
        if((CARD_ENTRY_MANUAL == srTransRec.byEntryMode) && (srTransRec.HDTid == 17))
            return d_OK;


Added:
inMPU_GetAuthTime()

inMPU_GetAuthDate
...
	//vduiClearBelow(2);
    CTOS_LCDTClearDisplay();
	
    vdDispTransTitle(srTransRec.byTransType);

	setLCDPrint(4, DISPLAY_POSITION_LEFT, "Enter Original");			
	setLCDPrint(5, DISPLAY_POSITION_LEFT, "Trans Date(DD/MM):");


#if 1 // for new MPU host application upgrade	
			if( srTransRec.HDTid == 17 ||
				srTransRec.HDTid == 18 ||
				srTransRec.HDTid == 19)
			{		
				#if 0
				if (strcmp(szAuthDate, srTransRec.szDate)!=0){
					vdDisplayErrorMsg(1, 8, "INVALID DATE");
					clearLine(8);
					continue;					
				}
				else
				{
					strcpy(srTransRec.szOrgDate, szAuthDate);
					return d_OK;
				}
				#else
					strcpy(srTransRec.szOrgDate, szAuthDate);
					return d_OK;
				#endif
			}	
#endif


 - POSMPUVOID.C
Added to inCTOS_MPUVoidFlowProcess()

LINE 116

	#ifdef CB_MPU_NH_MIGRATION
		// new adjustment for MPU new host application - 05/10/19 - based "POS function and  Regulation" doc
		if( srTransRec.HDTid == 17 ||
			srTransRec.HDTid == 18 ||
			srTransRec.HDTid == 19)
		{

				inRet = inCTOS_BatchSearch();
				if (d_OK != inRet)
					return inRet;

				vdDebug_LogPrintf("inCTOS_MPUVoidFlowProcess2 - byTransType [%d][%d][%d][%d]", 
									srTransRec.byTransType, 
									srTransRec.byOrgTransType, 
									fGetMPUTrans(), 
									fGetMPUCard());

				
				if(srTransRec.byTransType == PRE_AUTH)
				{			
					memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
					inRet = inCTOS_WaveGetCardFields();
					if (d_OK != inRet)
						return inRet;		

					if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
						inRet = inMPU_GetOnlinePIN();
						if (d_OK != inRet)
							return inRet;
					}		

					//POSIC023 - F12 in the pre-auth cancel request message need to be same as pre-auth message. #1
					inRet = inDatabase_BatchSearchByInvNo(&srTransRec, srTransRec.szInvoiceNo);
					vdDebug_LogPrintf("inDatabase_BatchSearchByInvNo Result:[%d]", inRet);
					
					if (d_OK != inRet)
						return inRet;		

					DebugAddHEX("inCTOS_MPUVoidFlowProcess3.srTransRec.szTime", srTransRec.szTime, 3);	
					 
					
				}

		}
	#endif

Added to LINE 204
		#ifdef CB_MPU_NH_MIGRATION
			#if 0
	        memset(srTransRec.szPAN, 0x00, sizeof (srTransRec.szPAN));
	        inRet = inCTOS_WaveGetCardFields();
	        if (d_OK != inRet)
	            return inRet;

	        if (fGetMPUCard() == TRUE || fGetMPUTrans() == TRUE) {
	            inRet = inMPU_GetOnlinePIN();
	            if (d_OK != inRet)
	                return inRet;
	        }		
			#endif
		#endif
===================================================================================


TO DO:::
=====================================


 
======================================


AND ALSO:
A. 
With fix for:
1. void time issue - raised by MPU (SIT)
 - void time (de12) should be the original sale time.

2. With Error receipt upload to ERM (change request case #1426).
3. Void sale de22 should be 0012 (manual entry) - change on requirement (MPU reported)


D. With fix for de22 issue (rc68 on sale)

- Fallback should be 0801


CHANGES ALREADY MADE ON OLD HOST APP:

ADDED 03-23-2020
1. fTPDUOnOff - in HDT table and databasefunc.c and in V5isofunc.c.
2. Copied ISOENGINEE17, ISOENGINEE18, ISOENGINEE19 from New Host
3. copied 21Table_data from New host
4. inUnPackIsoFunc62 for SIGN_ON
5. SIGN_ON - WORKING
6. fMagSwipeEnable - for enhancement for scenario #1 of redmine case #1583 - "Need to check MPU bin to disable magstripe' - Working


ADDED 03-24-2020
1. ERM_ERROR_RECEIPT_FEATURE added in old host application.
2.
 - POSBATCH.C (Done 03-24-2020 - for issue 562 	MK Merchant issue)
 - POSERM.C, POSERM.H (Done 03-24-2020)
 - POSFUNCTIONSLIST.C (Done 03-24-2020)
 - INCLUDES (POSMAIN.H) - (Done 03-24-2020)
 - INCLUDES (POSSETTING.H) - (Done 03-24-2020)
 - INCLUDES (POSTRANS.H) (Done 03-24-2020 (9F63))
 - INCLUDES (POSTYPEDEF.H) - (Done 03-24-2020)
3. 
POSAUTH.C
POSTRANS.C
POSMPUVOID.C

 - WORKING PREAUTH, PREAUTHVOID, PREAUTH COMPLETION AND PAC VOID TXNS AND MENU BASED ON 
POS function and  Regulation, POS Function latest DOCS

ISSUE: 03-24-2020
=================
 - pre auth and pre auth void(de12) not the same,  preauth comp and preauth compl void(de12) also the same.

the FIX: 03-25-2020
===================
1. V5ISOFUNC.C >>> inPackIsoFunc12()
 a. added:
		vdDebug_LogPrintf("DE12 MPU and Credit host combine checking [%d][%d]", srTransPara->byTransType, srTransPara->byOrgTransType);
		if (srTransPara->byTransType == VOID && PRE_AUTH == srTransPara->byOrgTransType)	
			memcpy((char *) &uszSendData[inDataCnt], srTransPara->szTime, 3); //POSIC023 - F12 in the pre-auth cancel request message need to be same as pre-auth message. #2
			//memcpy((char *) &uszSendData[inDataCnt], &baTransDT[2], 3);

		// Fix for PreAuthCompletion VOID/ cancellation.  Send Original de12 (time) of Preauth Completion. - based "POS function and  Regulation" doc
		if (srTransPara->byTransType == VOID && PREAUTH_COMP == srTransPara->byOrgTransType)
		{
			memset(uszSendData, 0x00, sizeof(uszSendData));
			vdDebug_LogPrintf("WWWWWWWWWWWWWWWWWWWW");
			//memcpy((char *) &uszSendData[inDataCnt], &baTransDT[2], 3);			
			memcpy((char *)&uszSendData[inDataCnt], srTransPara->szOrgTime, 3);
			//memcpy((char *)&uszSendData[inDataCnt], srTransRec.szOrgTime, 3);
		}

 b. inSnedReversalToHost()
 c. inSaveReversalFile()
- || (srTransPara->byTransType == PREAUTH_COMP))

 d. vdModifyBitMapFunc()
 d-1 // for New Host upgrade - remove sending of de36 02062019
 d-1.1 		if(srTransPara->byTransType != VOID)
        vdCTOSS_SetBitMapOn(inBitMap, 52);

 d-2 // for New Host upgrade - remove sending of de26 02062019
 d-3 //vdCTOSS_SetBitMapOn(inBitMap, 38); - for testing. as per new host specs 03/11/2019
 d-4 // for New Host upgrade 02092019
 d-5 //removed de35 based on MPU uat result  07/12/2019
 d-6 // for New Host upgrade 02092019 - no need to send on request
 d-7 /*for MPU-UPI card BIN rang (Credit app) no need send DE55*/
 d-8 for New Host upgrade remove de23 02092019 
 d-9 // for MPU new host implementation. no need to send de23 on void request
 d-10 if(srTransPara->byTransType != REFUND) // for MPU new host implementation
 d-11 || srTransPara->byTransType == REFUND 	  // FOR new host application
 d-12 // old code, for testing (PASSED). to fix rc68 on sale - 03122020
 d-13 // For PreAuthComp Void - New Host implementation - based "POS function and  Regulation" doc
 d-14 #else // for MPU new host implementation
int inPackIsoFunc25(TRANS_DATA_TABLE *srTransPara, unsigned char* uszSendData) {
 d-15
 d-16 inPackIsoFunc60() 
 d-17 inPackIsoFunc63() - // old host implemention for DE63
			#if 0
 d-18 inMPU_PorcessTransUpLoad
 -	//enabled to support final recon (960000) - for MPU new host enhancement
	#if 1
 - V5ISOFUNC.C - DONE 03-25-2020



2. POSREFUND.C
Added to OLD HOST app:
- inCTOS_RefundFlowProcess()
a.1 >>> // for new MPU host application upgrade - based "POS function and  Regulation" doc
#if 0
    inRet = inCTOS_GetCVV2();
    if (d_OK != inRet)
        return inRet;
#endif


a.2 >>> #if 1 // for new MPU host application upgrade	
>>> Working refund process flow based on "POS function and  Regulation" document.


3. POSSETTINGS.C (Done with no changes or nothing to compare)


DATE 03-26-2020
===============
1. PRINT.C (Done) - nothing to add on OLD host application.

2. POSHOST.C (Done)
added to 
	2.1 > inCTOS_GetOffApproveNO() in old host application.
 - 			setLCDPrint(5, DISPLAY_POSITION_LEFT, "Enter Approval Code: ");
 - 			#ifdef CB_MPU_NH_MIGRATION //for NEW HOST implementation. to get RRn for PreAuth Comp 

	2.2 > inCTOS_GeneralGetInvoice
//setLCDPrint(5, DISPLAY_POSITION_LEFT, "TRACE NO: "); -- original MPU change request

	2.3 > inCTOS_GeneralGetRRN
		- usY = 8;
		-     setLCDPrint(4, DISPLAY_POSITION_LEFT, "Enter Original");
    			setLCDPrint(5, DISPLAY_POSITION_LEFT, "Reference Number:");
		- 	clearLine(8);
		- 	// Check for existing record in batch - for NEW HOST implementation. to get RRn for PreAuth Comp		


3. POSMPUSALE.C > nothing to change. (Done)
4. DATABASEFUNC.H > nothing to change. (Done)

5. DATABASEFUNC.C (Done)

Working with all fxn and txn's. crossed check with New host application version 2005



NEW CHANGES FROM ALBERT TO CHECK WITH FIX FOR CASES: (Done 03-30-2020)
====================================================
- POSERM.C (done)
- PRINT.C	(done)
- PRINT.H	(done)
- POSQR_PAYMENT.C	(done)

Fix for the following cases:    
1. CBB Redmine Case 1595 - CBPay receipt slip with blank in MPOS.
2. CBB Redmine Case 1600 - CBPay ERM footer incorrect format.
3. CBB Redmine Case 1603 - CBPay Sale not uploading ERM receipt after transaction.
4. CBB Redmine Case 1605 - CBPay ERM Settlement receipt count and total amount not aligned.


03-31-2020 (added to redmine case #1605 by Yee) - Done
===============================================
(1) CBPay ERM Settlement Receipt count type is missing at CBPay Settlement.
the fix: Deferred, not an issue. Issuer total should not be printed on settlement for CBPay transaction.


(2) Please need to fix adjust space between currency and amount at CBpay Receipt.
the fix:
check ushCTOS_PrintBodyCBPay2
    if(srTransRec.byTransType == VOID)
        sprintf(szStr, "AMOUNT: -%s %s", strCST.szCurSymbol, szTemp1); // (2) Please need to fix adjust space between currency and amount at CBpay Receipt.  03-31-2020
    else
        sprintf(szStr, "AMOUNT: %s %s", strCST.szCurSymbol, szTemp1); // (2) Please need to fix adjust space between currency and amount at CBpay Receipt. 03-31-2020


04-01-2020
==========
1. APPROVED TRASACTIONS:
=========================
SIGN ON, 
SALE, 
VOID SALE,  
SALE REVERSAL, 
VOID SALE REVERSAL, 
SETTLEMENT , SETTLEMENT WITH BATCH UPLOAD (04-13-20)
PREAUTH, 
PRE-AUTH REVERSAL, 
REFUND AND REFUND REVERSAL (04-13-20)
PREAUTH VOID - APPROVED (04-15-20)
PREAUTH VOID REVERSAL - APPROVED (04-15-20)
===========================================




inject keys:

977C 7585 5D89 37B0 
C145 6285 1968 43A8
KCV - B28463

F82C 1551 A73D 8585 
A1FB 978C 2337 3289
KCV - CACFCD

Final KCV - C99992

Host details:
IP/Port 
43.242.135.24 8561 for SSL
43.242.135.24 8562 for non SSL

MID='000000010010010'		
TID='00100001'

Card details for Manual entry
3571 0830 5426 7629 
pin - 123456
exp date - 202601


04-02-2020 Added
==========
In 0584(CBPay)MPOS version which you provided, we found issues to fix.

(1)All asked pin number card types(MPU-UPI co-brand card onus/offus,MPU onus/offus and Pure UPI onus/offus)are masking asterisk(*) at inputing PIN Number.So we can't see how much pin digits in terminal UI.
the fix: pinpad.c >> inGetIPPPin

//(1)All asked pin number card types(MPU-UPI co-brand card onus/offus,MPU onus/offus and Pure UPI onus/offus)are masking asterisk(*)
//at inputing PIN Number.So we can't see how much pin digits in terminal UI.
//04-02-2020
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);



04-03-2020 - Done - release patch to Yee - for validation.
(2)Some Visa(onus,offus) cards not asked customer signature.
the fix:
 in ushCTOS_PrintFooter refer to:
//fix for (2)Some Visa(onus,offus) cards dont ask for digital signature. added to case #1605 04-03-2020



04-13-2020
==========
1. Tested REFUND with CB HOST (working).
1.1 REFUND REVERSAL - working.

2. PREAUTH VOID - GOT RC56 - NO CARD RECORD REPLY FROM HOST.
3. PREAUTH COMPLETION - COMM ERROR.
4. PREAUTH COMPLETION REVERSAL APPROVED (FOR RETEST)
5. CASH ADVANCE - COMM ERROR.
6. CASH ADVANCE REVERSAL - COMM ERROR.
7. SETTLEMENT WITH BATCH UPLOAD - WORKING


04-15-2020
==========
1. PREAUTH VOID - APPROVED
2. PREAUTH VOID REVERSAL - APPROVED
3. PREAUTH COMPLETION - RC 12 - INVALID TRANSACTION
4. CASH ADVANCE - COMM ERROR
5. CASH ADVANCE REVERSAL - COMM ERROR


04-22-2020
release package to Yee V0586
- with fix for invisible card 
    //CTOS_KBDBufFlush();//cleare key buffer -commented to resolve 04-02-2020 issue below?

04-23-2020
1. CASH ADVANCE - approved, ignore cvv2 entry
2  Cash Advance void - approved.
3. Cash Advnace void reversal - approved.
2. CASH ADVANCE REVERSAL - approved.
- also tested apporved by Yee.



04-27-2020
1. preuth - approved
2. preauth reversal - approved
3. preauth comp - comm error
3. preauth comp  reversal approved

04-28-2020
Investigate issue raised by KHine/ Khaing via redmine.
And I found these issues: 
1) MPU error receipt is not printed 
-fixed refer to inBuildAndSendIsoData

2) Instead of Fallback entry mode, MSR entry mode is printed on receipt and DE22 is 0021 instead of 0801.
- fixed bacause of the the flag fMagSwipeEnable

3) Magstripe is enable in MPU-JCB cards starting with 3571 and 3565 bins.
- fixed bacause of the the flag fMagSwipeEnable

4) Input PIN screen is not appeared in manual entry. But it is appeared when using chip.
 - PIN by passed for manual card entry.

04-29-2020
1)"Reversal Error" for the PreAuth Complete Transactions.
the fix: refer to inSaveReversalFile - case#1583


04-30-2020
1. User Input Amount and Printing Amount in Receipt Mismatch for Preauth Complete Transaction - case #1688
the fix:
refer to inDatabase_BatchSearchByAuthCode

2. If preauth comp amount > preauth amount terminal should prompt invalid amount
the fix: 
refer to inPackIsoFunc25 (issue related to case #1688
if(srTransRec.HDTid == 7 || srTransRec.HDTid == 21) // fix for if preauth comp exceed 15%. host still accept.



04-05-2020
1. WITH ENHANCED CAV FEATURE
REFER TO CASH_ADV_NEW_FEATURE
2. added 	if (fGetCashAdvAppFlag() == FALSE) 
to not include cbapy if menuid is 4 (Finexus CAV menu)



06252020

1) "Trace number" in every receipt and terminal UI, need to be replaced by "Invoice Number".
- done

4) Add Parameter setting for JCB and UPI Singature line for any amount and Bank set up Amount.
	a. Added flag UPISIGNLINE and JCBSIGNLINE if disbled > no checking of 9f34 (CVM) and default is no sign line.
	b. REcycled flag for any amount checking of sign line.
		> fAmountLessThanFloorLimit for JCB scheme, while CVMAMT flag for UPI.
- Done


7) Add 6244 1903 0000 to 6244 1903 9999 as Pure UPI.


9) Mgtstripe Sales Tranx will not allowed for Pure MPU,Pure UPI, MPU-JCB Co-brand, MPU-UPI Co-Brand.
But Mgt Stripe with Fallback will be allowed.

- For the Pure JCB, Mgt Stripe will allow in both Sales and Fallback.
 - We want to enable mgt stripe transaction by default for Pure JCB.


10) Disable/Enable Refund Menu on Terminal UI like Alipay.

08/05/2020

ADDED
Added to v5IsoFunc.c
// for cacse 2028 - Fallback scenario DE22 value for UPI and JCB on MPU new switch #2
if(srTransPara->IITid == 4 || srTransPara->IITid == 6 || srTransPara->IITid == 8 || srTransPara->IITid == 9)

added to inProcessEMVTCUpload
// fix for NFC Payment - ERROR 094 - Dupl Transmission case #2020


August 12
1. with @2071 Unmasking PreAuth recipt of merchant copy for Hotel Merchant
2. Eliminate crash due to header error on HOST SIM.  Not sending tpdu on request.


August 25
1. Added debug logs to inBuildAndSendIsoData, inBcdAddOne to fix duplicate trace number issue.

Aug 26
1. Enable tc upload for JCB CTLS card/ MPU host.
refer to MPU_CARD_TC_UPLOAD_ENABLE

2. added 

#define d_EMVCL_SID_JCB_EMV 0x63
#define d_EMVCL_SID_JCB_LEGACY2 0x99

to support other JCB CTLS cards and perform tc upload

Aug 28
With fixes for

1. Incorrect JCB CTS DE22 value, result 0911, expected 0071
2. Incorrect value of 5f2a on de55 if currency is USD.

Sept 2
With fix for the following:

(1)We tried with JCB card CTLS trx and got RC-05. MPU mentioned Tag 9A and DE 55 are missing.

(2)And tried JCB card trx with USD currency. Now void trx DE 49 value got 104.

(3) With fix for duplicate trace number.
scenario: sale, tc upload then void.  Void use the trace number of tc upload.

Sep 4
With fix for:

1. Settlement with batch upload error for all and any option.
2. VOID reversal sending USD country code.

sept 14
With fix for redmine case #2257


12112020
1. with fix for cash advance reversal error
the fix - refer to inProcessReversal()


10-22-21 (V3 (1015) and MPOS (3015))
1. Latest release With Corrected JCB PROD CapIndex
MP200
https://mega.nz/file/ZiBTzShQ#3gmsEjxQ5G2uM6VHgakXOBP52gvxXUEO8ohvFQpHugc

V3
https://mega.nz/file/Q6J13ayB#5FN1DGiL6mx7nUjzy1HWVaUYz9O0e0mrCOp-74Hq2EQ



04-20-2022
=========
Latest package is now available for download on links below with enhancements to remove OFFLINE and IP Address menu for all MENUID option.
 
V3 (Version 1029)
https://mega.nz/file/9rBFHKJY#CMO5sHHJEWK2BdIJQn938u2kjEQX0j6zFzQfwSpDAWc
 
MP200 (Version 3029)
https://mega.nz/file/h7xnzTBY#7bt7ViapzYgcf839ivQRW2pK12fmGMuYw7KBDiRVa8M

04-21-2022
==========
MP200 (V_3030)
https://mega.nz/file/92ozTLKA#WOcx1Ik8x_Dtrqw5Lp431dh0Vsni8ltzCvlvvPnATuw
 
V3 (V_1030)
https://mega.nz/file/1nQFiA5B#1F4VAWZp1sBvDlKxw-QnjA_orRVXZVeQo20C1jm-eA4

04-26-2022
With fix for:
For JCB,
1) Testing with amount greater than CVM limit, terminal prompts customer signature but "No Signature Required" with signature in e-receipt
2) Testing with amount less than CVM limit, "No Signature Required" text is appeared twice in e-receipt
For Master,
1) Testing with amount less than CVM limit, sign line is present in e-receipt.
For UPI,
1) Testing with amount less than CVM limit, terminal prompts customer signature but it's correct in e-receipt (No Signature required).


05-17-2022
Latest package are stored on links below for download with fix for the ff:
1.	JCB and UPI CTLS/ insert, shouldn’t prompt signature panel for transaction amount equal to CVMAMT/ floor limit on MP200 terminal.
2.	For V3/ Mp200, NO Signature line present/printed on receipt if JCB/ UPI transaction amount is equal to CVMAMT/ Floor limit.
 
MP200 – V3034:
https://mega.nz/file/1rpHVYTa#kbXkEIr7c32LwEV613Fnr3ywLZfGOyeX_nwJV33GRV0
 
V3 - V1034:
https://mega.nz/file/djIF0JZa#UhacPCllCRYy-1haVIaaoJtjwQvsgg9EQYzt4lIndOY


05/25/2022
1. with enhancement for 
"CBB would like to get a flag that can disable/enable uploading error e-receipts to iERS portal in MPOS terminal.
Please kindly implement this as the highest priority cause MPU old switch will be going to shutdown sooner or later."
 > released patch to SQA for validation. 


05/31/2022
Latest package are located on links below with enhancements for raised issue
 
“When error receipt flag = 0,
1) failed trnx any amount -> no need to prompt ph no./email -> not upload e-receipt
 
When error receipt flag = 1,
1) failed trnx greater than CVM limit -> prompt ph no./email -> upload error e-receipt
2) failed trnx less than CVM limit -> no need to prompt ph no./email -> upload error e-receipt
 
 
Whatever error receipt flag is 0 or 1, 
1) approved trnx above CVM limit -> prompt ph no./email -> upload e-receipt
2) approved trnx less than CVM limit -> upload e-receipt”
 
For V3 (1036)
https://mega.nz/file/sjIXCL4A#z4bOa902KkE7GJuJBeweTLQu5_lTZZ4SaNwkbQ48PMg
 
For MP200 (3036)
https://mega.nz/file/w2xwHbwZ#1U7U95bI8ioEz_P_a88y2NTDmDRoI7NARmmTTHKHYpY


06/21/2022
With CR for

2) To add fRefund flag for each BIN
• If card no. input is not supported to process terminal will 
prompt to “TRANSACTION NOT ALLOWED”

4) MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus should be 
always MMK.


5) Terminal should follow the existing flow of VISA/MASTER/UPI when 
processing MPU and JCB card transactions for Precomp

8) Create a flag which can allow/deny ctls trnx for MPU-UPI OffUs cards.

Others:
=======
Reported 06202022 - by Yamin
1) When error receipt flag is 1, terminal should not prompt ph no./email for any amount if trnx is not approved. (for all entry modes)
2) Sign line is not present for any amount when InFloorLimit = 0 and JCBSIGNLINE = 1 for JCB and MPU-JCB cards.


06/22/2022
With SIGN ON - with host display, working...

07/11/2022
With working SIGN ON ALL , in F2 menu and main menu.  

07/12/2022
With working SIGN ON ALL , in F2 menu and main menu.  
 - if F2 was press, SIGNON ALL option will appear.
 - if SIGNON menu was press on IDLE/ Main, SIGNON will not appear as option.

07/13/2022
1. Allign MENU for SIGN ON and SIGN ON ALL (EDIT DYNAMICMENU05)
2. DISABLE FIN-USD HOST IF DEFAULT CURRENCY IS MMK.  ALSO IF CURRENCY WAS CHANGE TO MKK OR VISE VERSA.  
 - FOR SIGN ON ALL FEATURE.  IF DEFAUTL IS MMK, FIN USD SHOULD BE DISABLED, IF USD IS THE DEFAULT, FIN-MMK SHOULD BE DISABLED.
3. CHANGE SIGN ON TRANSACTION DISPLAY TO SIGN ON ALL IF TRANSACTION IS A SIGN ON ALL.


07/14/2022
 - Add function for SignOn and Encrytions.
1. added ushUSD_WriteEncPIK2, ushMMK_WriteEncPIK2 in inUnPackIsoFunc57
2. added ushUSD_WriteEncPIK, ushMMK_WriteEncPIK, ushUSD_WriteEncMAK and ushMMK_WriteEncMAK in inUnPackIsoFunc62
3. added

//MMK
#define MMK_FIN_TMK_KEYSET  0xC003
#define MMK_FIN_TMK_KEYIDX  0x0011

//USD
#define USD_FIN_TMK_KEYSET  0xC003
#define USD_FIN_TMK_KEYIDX  0x0012
- in POSTypedef.h

4. added vdHardCodeMMK_FinexusUATKey and vdHardCodeUSD_FinexusUATKey in POSSetting.c
5. Removed BPC record in HDT, MMT, CPT, since MPU(17) will still in use.

07/19/2022
1. Rename MPU to MPU-BPC host label
2. added condition for MPU-jcb
		if(strCST.inCurrencyIndex == 1)//MMK currency
		{ 
			if(srTransRec.IITid == 8) //MPU-JCB - //for 3) MPU-JCB OnUs cards (3571 0851, 3571 0951) should be able to route either Finexus host or MPU host.
				inHostIndex = 17;	//BPC/HDTID 17
			else				
				inHostIndex = 23; 	//FIN-MMK
		}
		else
			inHostIndex = 22;	//FIN-USD

3. Added host 22 and 23 for Accumulator and for settle/ detail report.
4. Working batch review and batch total fxns
5. fixed missing TID/ MID from Settlement/ detail report.

07/21/2022
1. Assigned new MACRO values for MMK and USD
MMK
#define MMK_FIN_MAC_KEYSET 0xC003// CBB_COMM_KEYSET
#define MMK_FIN_MAC_KEYIDX  0x0015	// 21
#define MMK_FIN_MAC_KEYIDX2 0x0016	// 22 - refer to ushMMK_WriteEncMAK();

#define MMK_FIN_TMK_KEYSET  0xC003	
#define MMK_FIN_TMK_KEYIDX  0x0011
DCT TABLE:
HDTID 23 > 49155 > 21

USD
#define USD_FIN_MAC_KEYSET  0xC003//CBB_COMM_KEYSET
#define USD_FIN_MAC_KEYIDX  0x0015
#define USD_FIN_MAC_KEYIDX2  0x0016

#define USD_FIN_TMK_KEYSET  0xC003 //Orig - 0xC003
#define USD_FIN_TMK_KEYIDX  0x0012

07/22/2022
1. Assigned new values for USD Encryption
//  USD DCT
#define USD_FIN_MAC_KEYSET  0xC003		//CBB_COMM_KEYSET
#define USD_FIN_MAC_KEYIDX  0x0016		//Working - 0x0015
#define USD_FIN_MAC_KEYIDX2  0x0017		// Working - 0x0016

#define USD_FIN_TMK_KEYSET  0xC003
#define USD_FIN_TMK_KEYIDX  0x0012

DCT Table:
HDTID 22 > 49155 > 22

2. Tested working USD, MMK and MPU-BPC Key Injection, SignOn and PIN Entry.


08/03/2022
1. Added de57 in ISOENGINEE23 and ISOENGINEE22.
2. added condition to inPackIsoFunc57() for #FINUSDTMKREF and #FINMMKTMKREF ref num.
3. Created HardCoded key injection for MMK - vdHardCodeMMK_FinexusUATKey().
4. Created HardCoded key injection for USD - vdHardCodeUSD_FinexusUATKey().
5. Able to connect thru CBB host for MMK/USD SignOn feature.


08/04/2022
1. working signon, sale, void settlement.
2. 	added code to vdModifyBitMapFunc

#if 1 // for testing...08/04/2022
	if(srTransRec.HDTid == 22 || srTransRec.HDTid == 23 || srTransRec.HDTid == 17 )		
	{
		vdCTOSS_SetBitMapOn(inBitMap, 62);		
		vdCTOSS_SetBitMapOff(inBitMap, 49);
	}
	
	vdDebug_LogPrintf("vdModifyBitMapFunc >>>>vdCTOSS_SetBitMapOn(inBitMap, 62)");
#endif 

08/05/2022
1. added condition for sending de48 for reversal, batch upload and void.
 - refer to vdModifyBitMapFunc

2. remove sending of de35 for sale reversal.

3. working Preauth, pac, pac rev, preauth rev. 


08/09/2022
1. added condition to inGetCAVIPPPin, inCTOS_inGetDefaultIPPPin, inCTOS_inGetNewIPPPin and inCTOS_inGetVerifyIPPPin for CAV and PIN change.

08/10/2022
1. Added condition for MMK and USD currency
> if currency defauld index is MMK, all transaction should go to FIN-MMK host, regardless if BIN HDTID is 22.
/*
NOTES:
>>>> (1) MPU-JCB OnUs cards (3571 0851, 3571 0951) should be able to route either Finexus host or MPU host.  Host routing rides with the Current Terminal Currency index
>>>> (2) MPU OnUs/OffUs, MPU-JCB OnUs/OffUs, MPU-UPI Offus should be always MMK.
>>>> (3) IF Default Currency IS MMK all BIN regardless of HDTIDx, transcation will be routed to FIN-MMK (HDTID 23)
*
#ifdef CBB_FIN_ROUTING	
		vdDebug_LogPrintf("inCTOS_SelectHost CBB_FIN_ROUTING");

		//MMK currency
		if(strCST.inCurrencyIndex == 1)
		{ 
			if(inHostIndex == 17 && srTransRec.IITid == 8) // Addressed NOTE #1.
				inHostIndex = 17;
			else				
				inHostIndex = 23; 	// Addressed NOTES #2 and #3 above
		}
		else // USD CURRENCY
		{	
			//Addressed NOTE #1 and #2
			if((inHostIndex == 23) && (srTransRec.IITid == 7 || srTransRec.IITid == 8 || srTransRec.IITid == 9))
				inHostIndex = 23;
			else if(inHostIndex == 17 && srTransRec.IITid == 8)	//Addressed NOTE #1.
				inHostIndex = 17;
			else
				inHostIndex = 22;
		}

		vdDebug_LogPrintf("2. inCTOS_SelectHost =[%d]", inHostIndex);
#endif

2. EDIT CDT TABLE > WITH HDTID ASSIGNMENT > BASED ON BIN Ranges in CBB EDC Terminal (3) FILE.


08122022
=========
With fix for:
1. DE35 (MPU card with 95 BIN to remove padded '0' before card no.)
2. error receipt hdtid(22, 23) host. - not printing error receipt for FIN host.
3. Pin Entry for VISA/MC cards - Terminal should not prompt pin entry.
4. When we do MPU BPC sign on, it's getting "Comm Error" remove De62 on SIGNON


08152022
==========
1. With fix for LOAD CST Err.
2. With fix for host 22, IIT 4 and 2. Entrymode fallback and manual entry terminal prompts pin verification.
3. Added condition for Online pin verification case if intype is Debit - inMPU_GetOnlinePIN()

08162022
=======
1. With for Settlement MMK currency and no amount displayed on SALE, TOTAL field.
- added condition to inCTOS_DisplayBatchTotalEx(), 
	else if(srTransRec.HDTid == 22)		
		inCSTRead(2);
2. //to Exclude CBPAY and OK$ host during SIGNON -ALL
 - added condition to inCTOS_SIGNONAllHosts().

3.)When we perform Sign On All menu, it is working for all host sign on(CBPay,OK$,MPU BPC,Finexus-MMK) like that we opened all
hosts.
It should be performed Sign On All for only both MPU BPC sign on and Finexus-MMK sign on on MMK Currency setup terminal.
 - same with #2

4)When we perform Preauth void trxs, it should be displayed "Preauth Void " in terminal UI after typing Invoice number(Trace
number).

5)After we did Preauth void trx, it is displayed with both Preauth and Preauth Void trxs in Batch Review. It should be displayed
only "Preauth void" in it. Am I right??

6)MC/VS card is asking to prompt PIN in this pk. So we would like to get normal production flow.

08182022
========
1, with fix for 
http://118.201.48.214:8080/issues/75 #39
Please let me update issue lists on new finexus routing pk 1039 currently.
In USD setup terminal, should be included three Menus :
1) MPU BPC sign on
2)Finexus-MMK
3)Finexus-USD

We got missing Finexus-USD menu in USD setup terminal even we open Hostid 22 for USD.

When we choose Currency Select menu to USD in terminal manually, once it is changed to Finexus-USD menu and included in it.
So Please kindly help to check the attached video file and fix it as urgent case.
2. with fix for sudden currency change on idle.


08 22 2022 - 08 23 2022
=========   ========
1. With fix for MPU-upi platinun ON-us. Currency was change on amount entry and total display
MPU-UPI-Class
MPU-UPI-Gold
MPU-UPI-Platinum
MPU-UPI 

2. 	//for testing. fix prompt of default currency.  Currency its changing on pin entry cancellation and after transaction. - 08222022
	inCSTRead(strHDT.inCurrencyIdx);
- added code to inMPU_GetOnlinePIN()

for http://118.201.48.214:8080/issues/75 #41
3.)onus MPU-UPI currency issue is okey with latest patch file. But we are getting another issue is Pure MPU cards 9503 should be MMK on USD currency setup terminal. Please kindly compare it on S1F2 latest version.
4.)To remove "00" in the angle bracket of RESP CODE if error codes are returned by terminal, not from host
5.)We can't test Same MID/ Different TID case and got "No Record found" with another terminal for Preauth functions.
Please kindly help to check and fix it.



1. Removed condition on inCTOS_SelectHost to address ">>>> (1) MPU-JCB OnUs cards (3571 0851, 3571 0951, 3571 0975 and 3571 0925) should be able to route either Finexus host or MPU host.  Host routing rides with the Current Terminal Currency index.
"
			if(inHostIndex == 17 && srTransRec.IITid == 8) // Addressed NOTE #1.
				inHostIndex = 17;

2. added 2 MPU-JCB DEBIT BINS in CDT table
>>> 3571 0975
>>> 3571 0925


NOTE: With released package version 1040 and 3040

08242022
========
1. with fix for the ff case:
> USD setup > MPU card (hdtid 23, IITid 7 (Routed to FIN-MMK host)) > SALE RECEIPT - OK.  Merch / Bank copy with USD currency printed should be MMK.
> REprint ANY/ LAST > Printed Currency was USD should be MMK.
> After reprint receipt, back to IDLE.  Currency Default value was MMK should be USD.

2. with fix for (http://118.201.48.214:8080/issues/75.53)
> 1) We are getting currency issue with MPU-UPI offus card on USD setup terminal . MPU onus/offus, MPU-JCB onus/off, MPU-UPI offus should be always MMK.
> 2) Our merchant team would like to get TID/MID masking case like this format 
TID *xxxx
MID xxxx****xxxx.

>>>>> Released APP to SQA with version 1041 and 3041.

08252022

 Reported issues:
1)We would like to test MMK currency. But it was displayed with USD in terminal UI although we did do setup MMK currency.
And When I checked it in HDT 23, inCurrencyIdx value is 2. It should be 1 for MMK, am I correct?
>>>>Unable to replicate

2)No need spacing for TID/MID format and would like to get like that eg;
TID : xxxx0022
MID : 0000xxxxxxx0179

3)We would like to get CB Logo like the latest andriod version in receipts.

4)Please kindly also provide fTPDU onoff flag=0 as default value in HDTid 17.
So please kindly help to check it.

Internal Issue:
1.	Display of default currency on IDLE.
USD setup > on Idle > Insert card (MPU) > on Amount entry (MMK) cancel the transaction > back to idle > currency remain MMK should be USD.

>>>>> Released APP to SQA with version 1042 and 3042.

08312022
========

1.”DE03 of the batch upload refund simple has to follow the original 0200 transactions'.
2. Kindly remove DE 23 & 35 in void iso pkt. To add DE14.”
3. Added CURRDEFVAL at ENV VAR to replace HDTID 17 for currency default index.

>>>>> Released APP to SQA with version 1043 and 3043.


09012022
========
1. WITH ADDed fix for currency code did not return to default value.
	// Return default currency code based on inGetHDTDefCurrVal value
	#ifdef CBB_FIN_ROUTING
	if(inGetHDTDefCurrVal == 2 || strHDT.inCurrencyIdx == 2)
		inCSTRead(2);
	else
		inCSTRead(1);		
	#endif
>>> refer to ushCTOS_printReceipt()


09052022
1. with fix preauth void mti.
> added VOID_PREAUTH to 22 and 23 isoengine.

2. with fix for JCB bin cdt 58, doesnt change currency from usd to mmk (amount entry, receipt etc) if hdtid is 23

09062022
1.  REturn preauth flow
2. Fix currency index issue during amount entry before card entry.
refer to >>> inCTOS_GetCardFields

latest versions
V3_1045, Mpos_3045


09082022
1. with fix for preuth void d25 value. should be 06 not 00.

09092022
1. removed de37 from refund transaction.
- http://118.201.48.214:8080/issues/75



09142022
1. WITH FIX FOR jcb VOID PAC issue.
> chip malfunction
> fix: added 
        case 0x80: //ARQC - fix for JCB VOID PAC
                vdDebug_LogPrintf("go online");
                break;
to shCTOS_EMVSecondGenAC

With fixes for the following issues:
1.	PreAuth void de25 value. should be 06 not 00.
2.	Removed de37 from refund transaction.
3.	JCB VOID PAC issue.
4.	Add DE37 for PreAuthComp cancel/Preauth cancel trxs on New Finexus Routing.

> Latest release package V3 only 1046

09152022
===
1.with fix for vissa/mc prompt pin entry for MSR entrymode.
> refer to inMPU_GetOnlinePIN


09162022
1. WITH Master kEy entry for MMK/ USd on key injection menu

09192022
1. With Enhanced MMK/USD key injection pin entry.  
>> 2 component entry


09202022
1. with fix for
>>> MPU-UPI OnUs Void, Preauth Void, Preauth Comp Void -> currency changed to MMK (original trnx is performed with USD)
The expected flow is currency should be the same as original trnx currency.
Please refer to attached logs.


09212022
1. with fix for 
//fix for uble to void Preauth transaction after settlement
//http://118.201.48.214:8080/issues/75#change-4195.63 #3
//3) MPU, MPU-JCB, MPU-UPI OffUs Preauth Void after settlement -> No Record Found The expected flow is Preauth trnx should be able to void before Preauth record date.


09222022
1. with fix for 
http://118.201.48.214:8080/issues/75.63 #4
>>> 4) After Settlement All, auto Sign On All should be performed one time.


09232022
1. With fix for 
1.1	PAC VOID incorrect MTI.  Should be 0220.  Current is 0200.
1.2	Incorrect De25 value for PAC (JCB card). Current is 00, should be 06.
Released latest package with version 1047


2. with fix for JCB CDTID 58 offus card. should carry  strCST.inCurrencyIndex
> added to condition srTransRec.CDTid == 58

https://mega.nz/file/BjhwSSwC#jIhKY6fwQirjSfw6KX9D2rxdAzQs7APwL41AUkTgIk0

09262022
1. with fix for Preauth Void de25. should be 06.
2. Rearrange menu id 4. added reprint and topup/ repayment to DYNAMICMENU04 dbase.
>>> edit vdCheckMenuID4 function to support new menuid 4.

09272022
1. with REfund CR
refer to CBB_CR_REFUND
2. added fAddRefRRNAPP to cdt table for refund bin validation.


09302022
1. REmoved de 12 and 13 from refund request.

10032022
1. With fix for issue http://118.201.48.214:8080/issues/75.72 #3 -  Pure UPI sign line issue
2. UPI ask for pin entry.
3. fix appr code entry for refund. default minimum input is 6 before its 2.
4. With fix for issue http://118.201.48.214:8080/issues/75.72 #4 - Currency is incorrect in Customer Copy and Bank Copy (merchant copy is correct) for MPU-JCB cards which should be always MMK.
5. With fix for http://118.201.48.214:8080/issues/75.72 #2
> The expected flow for all entry modes is ->
when JCBSIGNLINE = 1 and InFloorLimit is USD 50, then sign line should be present starting from USD 50.01,
6. with fix for http://118.201.48.214:8080/issues/75.72 #1
>  Visa/MC sign line issue
The current flow is whatever fVEPS, fQPS values are 0 or 1, sign line is 
present for any amount.
7. Multiple printout of �NO SIGNATURE REQUIRED� for JCB CTLS card.

CBB Change Request: #75 MPU cards routing change to Finexus host (V3_1050)
Link:
https://mega.nz/file/AvolzRjK#ARz-YeQoBzdukRoWpcWLzJ7HuxrcajbaDWBCJjreKj8


CBB Change Request: #75 MPU cards routing change to Finexus host (V3_1050)
Link:
https://mega.nz/file/AvolzRjK#ARz-YeQoBzdukRoWpcWLzJ7HuxrcajbaDWBCJjreKj8


10052022
1. Fixed PIN entry prompt for VISA/MC on Cash Advance


10062022
1. Added CAVFINFLAG to support old routing for CAV function.


10072022
1. added TUPFINFLAG for BTR/ toptup transaction, same flow with CAV old routing.
2. Added inHostSettingOn flag to HDT table for Menuid 4 [LOGON] menu, to support 
host display. Example remove Topup&Repayment in select host.
> release pacckage to SQA with version 1051 (v3 only)


10112022
1. with fix for MMT load error for CAV transaction.
2. With firx for HOST SELECTION ERR during TOPUP_RELOAD_LOGON.


10122022
1. with fix for incorrect MID format (szMaskMID) during VISA / MC transaction.
The fix: 
From:
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskMID, strlen((char*)strIIT.szMaskTID), SQLITE_STATIC);
To:
	result = sqlite3_bind_text(stmt, inStmtSeq +=1, (char*)strIIT.szMaskMID, strlen((char*)strIIT.szMaskMID), SQLITE_STATIC);

2. With fix for PIN CHANGE host selection error"


10132022
 With fix for the following:
2) Pure JCB via magstripe and manual entry prompts PIN.

3) To add these 2 OnUs new BINs:
- 3571 0950 0000 to 3571 0950 9999 (MPU-JCB)
- 9512 0800 0000 to 9512 0899 9999 (MPU Prepaid )

To modify PAN Hi value for the following BIN:
   - 9505 0800 0000 to 9505 0899 9999  (MPU Credit, CDTid 43)

   
4) To fix CPTid 20 host name in db

5) To remove extra spaces between "AMOUNT" and "No Signature Required" text 
for MPU-JCB and JCB


6) To remove UPLOAD RECEIPT (from V3) and SIGN OFF menus (both V3/MPOS)

7) To add J/Speedy prm (floorlimit, CTLS trnx limit, CL CVM required limit) 
in the prm file
0=WAVE\JCB\szValue\JCBid\7=000100000000; JCB Reader CL Trans limit (Default)(12 bytes)
0=WAVE\JCB\szValue\JCBid\8=000000030001; JCB Reader CL CVM   limit (Default)(12 bytes)
0=WAVE\JCB\szValue\JCBid\9=000000020001; JCB Reader CL Floor limit (Default)(12 bytes)
