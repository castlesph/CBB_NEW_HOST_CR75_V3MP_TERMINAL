/* 
 * File:   typedef.h
 * Author: sam
 *
 * Created on 2012年8月15日, 下午 11:19
 */

#ifndef TYPEDEF_H
#define    TYPEDEF_H

#ifdef    __cplusplus
extern "C" {
#endif

//#define TLE 1
//#define CVV  //CVV enable changed to TCT.txt

#define PUBLIC_PATH 				"/home/ap/pub/"
#define LOCAL_PATH 					"./fs_data/"
#define UNFORK_AP_FILE			"unforkap.ini"

#define IN
#define OUT
#define INOUT

#define char    char
#define UCHAR   unsigned char
#define BYTE    unsigned char

#ifndef STR
    #define STR        unsigned char    
#endif


#define SHORT   short                // 2byte
#define USHORT  unsigned short       // 2byte
#define WORD    unsigned short       // 2byte
#define int     int                  // 4byte    
#define UINT    unsigned int         // 4byte
#define DWORD   unsigned int         // 4byte
#define BOOL    unsigned char        // 1byte
#define LONG    long                 // 4byte
#define ULONG   unsigned long        // 4byte
#define DOUBLE   double        		 // 8byte

//#define NMX_LIB

//#define DUKPT_3DESONLINEPIN


#define TRUE     1
#define FALSE    0

#ifndef NULL
#define NULL    0
#endif

#ifndef CTOSNULL
#define CTOSNULL    0
#endif

#define RUN_SAHRE_EMV   1


#define V3_STATUS_LINE_ROW   13
#define V3_ERROR_LINE_ROW   14


//#define MULTIAPLIB   1


#define PRNT_FONT_FILE			"cousinebd.ttf"
#define THEMESTYPE_7_FONT       "myanmar.ttf"
#define ZAWGYI_FONT             "Zawgyi-One-20060620.ttf"


typedef unsigned char VS_BOOL;

#define MAX_AMOUNT	999999999999

#define K_STATUS        BYTE

#define SUCCESS     0
#define FAIL       -1

#define VS_FALSE           ((VS_BOOL) 0)
#define VS_TRUE            ((VS_BOOL) 1)

#define MAX_CHAR_PER_LINE       40


/*for CBB define*/
#define CBB_COMM_KEYSET		0xC000

#define CBB_TMK_KEYSET		CBB_COMM_KEYSET
#define CBB_TMK_KEYIDX		0x0004

#define CBB_TPK_KEYSET		CBB_COMM_KEYSET
#define CBB_TPK_KEYIDX		0x0005

#define CBB_TAK_KEYSET		CBB_COMM_KEYSET
#define CBB_TAK_KEYIDX		0x0006

#define CBB_WEK_KEYSET		CBB_COMM_KEYSET
#define CBB_WEK_KEYIDX		0x0007

//For OK$ AES
#define CBB_AES_KEYSET		0xC002 //0xC000 - not good
#define CBB_AES_KEYIDX		0x0004//0x0004, [0xC000 & 0x0006  - GOOD]


#define MPU_TMK_KEYSET		CBB_COMM_KEYSET
#define MPU_TMK_KEYIDX		0x0014

#define MPU_PIK_KEYSET		CBB_COMM_KEYSET
#define MPU_PIK_KEYIDX		0x0015

#define MPU_MAK_KEYSET		CBB_COMM_KEYSET
#define MPU_MAK_KEYIDX		0x0016

#define MPU_MAC_KEYSET      CBB_COMM_KEYSET
#define MPU_MAC_KEYIDX      0x0017

#define IPP_TMK_KEYSET		0xC001
#define IPP_TMK_KEYIDX		0x0004

#define IPP_TPK_KEYSET		0xC001
#define IPP_TPK_KEYIDX		0x0005


//MMK DCT
#if 0 //TESTED WORKING with Key Injection, Sign On and Pin Entry
#define MMK_FIN_MAC_KEYSET CBB_COMM_KEYSET
#define MMK_FIN_MAC_KEYIDX  0x0015	// 21
#define MMK_FIN_MAC_KEYIDX2 0x0016	// 22 - refer to ushMMK_WriteEncMAK();

#define MMK_FIN_TMK_KEYSET  0xC003	
#define MMK_FIN_TMK_KEYIDX  0x0011


#else
#define MMK_FIN_MAC_KEYSET 0xC003		// CBB_COMM_KEYSET
#define MMK_FIN_MAC_KEYIDX  0x0015		// 21
#define MMK_FIN_MAC_KEYIDX2 0x0016	// 22 - refer to ushMMK_WriteEncMAK();
//MMK DCT

//for MMK TMK injection
#define MMK_FIN_TMK_KEYSET  0xC003	
#define MMK_FIN_TMK_KEYIDX  0x0011
//for MMK TMK injection


//  USD DCT
#define USD_FIN_MAC_KEYSET  0xC003		//CBB_COMM_KEYSET
#define USD_FIN_MAC_KEYIDX  0x0016		//Working - 0x0015
#define USD_FIN_MAC_KEYIDX2  0x0017		// Working - 0x0016
// USD DCT

//FOR USD TMK injection
#define USD_FIN_TMK_KEYSET  0xC003
#define USD_FIN_TMK_KEYIDX  0x0012
//FOR USD TMK injection

#endif




//#define RETAIL_APP_MENU				3
#define IPP_MENU					3
#define CASH_ADV_APP_MENU			4
#define NEW_ENHANCEMENT_MENU        5


#define VISA_CREDIT_HOST_IDX	1
#define MASTER_CREDIT_HOST_IDX	21

#define UNIONPAY_HOST_IDX		7
#define MPU_CREDIT_HOST_IDX		16
#define MPU_DEBIT_HOST_IDX		17
#define MPU_JCBI_HOST_IDX		18
#define MPU_CUP_HOST_IDX		19


/*Tags definition*/

#define TAG_4F_AID                    0x004F //Application Identifier (AID)
#define TAG_50                        0x0050 //Application Label
#define TAG_52                        0x0052 //Command To Perform
#define TAG_55                        0x0055 //TSignature CVM
#define TAG_57                        0x0057 //Track 2 Equivalent Data
#define TAG_5A_PAN                    0x005A //Application Primary Account Number (PAN)
#define TAG_61                        0x0061 //Application Template
#define TAG_6F                        0x006F //FCI Template
#define TAG_70                        0x0070 //Application Elementary File (AEF) Data Template
#define TAG_71                        0x0071 //Issuer Script Template 1
#define TAG_72                        0x0072 //Issuer Script Template 2
#define TAG_73                        0x0073 //Directory Discretionary Template
#define TAG_77                        0x0077 //Response Message Template Format 2
#define TAG_80                        0x0080 //Response Message Template Format 1
#define TAG_81_BIN_AMOUNT             0x0081 //Amount, Authorised (Binary)
#define TAG_82_AIP                    0x0082 //Application Interchange Profile (AIP)
#define TAG_83                        0x0083 //Command Template
#define TAG_84_DF_NAME                0x0084 //Dedicated File (DF) Name
#define TAG_86                        0x0086 //Issuer Script Command
#define TAG_87                        0x0087 //Application Priority Indicator
#define TAG_88                        0x0088 //Short File Identifier (SFI)
#define TAG_8A_AUTH_CODE              0x008A //Authorisation Response Code
#define TAG_8C                        0x008C //Card Risk Management Data 1 (CDOL1)
#define TAG_8D                        0x008D //Card Risk Management Data 2 (CDOL2)
#define TAG_8E                        0x008E //Cardholder Verification Method (CVM) List
#define TAG_8F                        0x008F //Certification Authority Public Key Index (ICC)
#define TAG_90                        0x0090 //Issuer Public Key Certificate
#define TAG_91_ARPC                   0x0091 //Issuer Authentication Data
#define TAG_92                        0x0092 //Issuer Public Key Remainder
#define TAG_93                        0x0093 //Signed Application Data
#define TAG_94                        0x0094 //Application File Locator (AFL)
#define TAG_95                        0x0095 //Terminal Verification Results
#define TAG_97_TDOL                   0x0097 //Transaction Certificate Data Object List (TDOL)
#define TAG_98                        0x0098 //Transaction Certificate (TC) Hash Value
#define TAG_99                        0x0099 //Transaction PIN Data
#define TAG_9A_TRANS_DATE             0x009A //Transaction Date
#define TAG_9B                        0x009B //Transaction Status Information
#define TAG_9C_TRANS_TYPE             0x009C //Transaction Type
#define TAG_9D                        0x009D //Directory Definition File (DDF) Name
                                      
                                      
#define TAG_5F20                      0x5F20 //Cardholder Name
#define TAG_5F24_EXPIRE_DATE          0x5F24 //Application Expiration Date
#define TAG_5F25                      0x5F25 //Application Effective Date
#define TAG_5F28                      0x5F28 //Issuer Country Code
#define TAG_5F2A_TRANS_CURRENCY_CODE  0x5F2A //Transaction Currency Code
#define TAG_5F2D                      0x5F2D //Language Preference
#define TAG_5F30_SERVICE_CODE         0x5F30 //Service Code
#define TAG_5F34_PAN_IDENTFY_NO       0x5F34 //Application PAN Sequence Number
#define TAG_5F36                      0x5F36 //Transaction Currency Exponent
                                      
                                      
#define TAG_9F01                      0x9F01 //Acquirer Identifier
#define TAG_9F02_AUTH_AMOUNT          0x9F02 //Amount, Authorised (Numeric)
#define TAG_9F03_OTHER_AMOUNT         0x9F03 //Amount, Other (Numeric)
#define TAG_9F04                      0x9F04 //Amount, Other (Binary)
#define TAG_9F05                      0x9F05 //Application Discretionary Data
#define TAG_9F06                      0x9F06 //Application Identifier (AID) (Terminal)
#define TAG_9F07                      0x9F07 //Application Usage Control
#define TAG_9F08_IC_VER_NUMBER        0x9F08 //Application Version Number
#define TAG_9F09_TERM_VER_NUMBER      0x9F09 //Application Version Number (Terminal)
#define TAG_9F0B                      0x9F0B //Cardholder Name Extended
#define TAG_9F0D                      0x9F0D //Issuer Action Code - Default
#define TAG_9F0E                      0x9F0E //Issuer Action Code - Denial
#define TAG_9F0F                      0x9F0F //Issuer Action Code - Online

#define TAG_9F10_IAP                  0x9F10 //Issuer Application Data
#define TAG_9F11                      0x9F11 //Issuer Code Table Index
#define TAG_9F12                      0x9F12 //Application Preferred Name
#define TAG_9F13                      0x9F13 //Last Online ATC Register
#define TAG_9F14                      0x9F14 //Lower Consecutive Offline Limit (EMV Generic)
#define TAG_9F15                      0x9F15 //Merchant Category Code
#define TAG_9F16                      0x9F16 //Merchant Identifier
#define TAG_9F17                      0x9F17 //PIN Try Counter
#define TAG_9F18                      0x9F18 //Issuer Script Identifier
#define TAG_9F1A_TERM_COUNTRY_CODE    0x9F1A //Terminal Country Code
#define TAG_9F1B_TERM_FLOOR_LIMIT     0x9F1B //Terminal Floor Limit
#define TAG_9F1C_TID                  0x9F1C //Terminal Identification
#define TAG_9F1D                      0x9F1D //Terminal Risk Management Data
#define TAG_9F1E                      0x9F1E //Interface Device Serial Number
#define TAG_9F1F                      0x9F1F //Track 1 Discretionary Data
                                      
#define TAG_9F20                      0x9F20 //Track 2 Discretionary Data
#define TAG_9F21_TRANS_TIME           0x9F21 //Transaction Time
#define TAG_9F22                      0x9F22 //Certification Authority Public Key Index (Terminal)
#define TAG_9F23                      0x9F23 //Upper Consecutive Offline Limit (EMV Generic)
#define TAG_9F26_EMV_AC               0x9F26 //Application Cryptogram (AC)
#define TAG_9F27                      0x9F27 //Cryptogram Information Data (CID)
#define TAG_9F2D                      0x9F2D //ICC PIN Encipherment Public Key Certificate
#define TAG_9F2E                      0x9F2E //ICC PIN Encipherment Public Key Exponent
#define TAG_9F2F                      0x9F2F //ICC PIN Encipherment Public Key Remainder
                                      
#define TAG_9F32                      0x9F32 //Issuer Public Key Exponent
#define TAG_9F33_TERM_CAB             0x9F33 //Terminal Capabilities
#define TAG_9F34_CVM                  0x9F34 //Cardholder Verification Method (CVM) Results
#define TAG_9F35_TERM_TYPE            0x9F35 //Terminal Type
#define TAG_9F36_ATC                  0x9F36 //Application Transaction Counter (ATC)
#define TAG_9F37_UNPREDICT_NUM        0x9F37 //Unpredictable Number
#define TAG_9F38                      0x9F38 //Processing Options Data Object List (PDOL)
#define TAG_9F39                      0x9F39 //POS Entry Mode
#define TAG_9F3A_AMOUNT_REF_CURRENCY  0x9F3A //Amount, Reference Currency
#define TAG_9F3B                      0x9F3B //Application Reference Currency
#define TAG_9F3C_TRANS_REF_CURRENCY   0x9F3C //Transaction Reference Currency Code
#define TAG_9F3D                      0x9F3D //Transaction Reference Currency Exponent
                                      
#define TAG_9F40_ADD_TERM_CAB         0x9F40 //Additional Terminal Capabilities
#define TAG_9F41                      0x9F41 //Transaction Sequence Counter
#define TAG_9F42_APP_CURRENCY_CODE    0x9F42 //Application Currency Code
#define TAG_9F43                      0x9F43 //Application Reference Currency Exponent
#define TAG_9F44                      0x9F44 //Application Currency Exponent
#define TAG_9F45                      0x9F45 //Data Authentication Code
#define TAG_9F46                      0x9F46 //ICC Public Key Certificate
#define TAG_9F47                      0x9F47 //ICC Public Key Exponent
#define TAG_9F48                      0x9F48 //ICC Public Key Remainder
#define TAG_9F49_DDOL                 0x9F49 //Dynamic Data Authentication Data (DDOL)
#define TAG_9F4A                      0x9F4A //Static Data Authentication Tag List
#define TAG_9F4B                      0x9F4B //Signed Dynamic Application Data
#define TAG_9F4C                      0x9F4C //ICC Dynamic Number
                                      
#define TAG_9F52                      0x9F52 //Application Default Action
#define TAG_9F53                      0x9F53 //Transaction Category Code
#define TAG_9F58                      0x9F58 //Lower Consecutive Offline Limit (Proprietary)
#define TAG_9F59                      0x9F59 //Upper Consecutive Offline Limit (Proprietary)
#define TAG_9F5B                      0x9F5B //POS_ISS_SCRIPT_RESULT

#define TAG_9F63                      0x9F63 //Offline counter
#define TAG_9F6E                  	  0x9F6E //JCB Device information
#define TAG_9F7C                      0x9F7C //JCB Partner Discretion Data


#define TAG_9F72                      0x9F72 //Consecutive Transaction Limit (international - country)
#define TAG_9F73                      0x9F73 //Currency Conversion Factor
#define TAG_9F75                      0x9F75 //Cumulative Total Transaction Amount Limit - Dual Currency
#define TAG_9F76                      0x9F76 //Secondary Application Currency Code
#define TAG_9F7F                      0x9F7F //Card Product Life Cycle History File Identifiers

#define TAG_DF31_ISR                  0xDF31//issuer script result    

/*definition of EMV status*/
#define EMV_TRANS_FAILED     101
#define EMV_CHIP_FAILED      102
#define EMV_USER_ABORT       103
#define EMV_CRITICAL_ERROR   104
#define EMV_NO_AP_FOUND      105
#define EMV_POOL_FULL        106
#define EMV_FALLBACK         107

#define EMV_CARD_BLOCKED 108 //VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback -- jzg
#define EMV_CHIP_NOT_DETECTED 109 //EMV - should display "CHIP NOT DETECTED" instead of doing fallback -- jzg
#define EMV_TRANS_NOT_ALLOWED 110 //EMV - If AID not found display "TRANS NOT ALLOWED" -- jzg


/*EMV chip trans result*/
#define EMV_OFFLINE_APPROVED_TC    0x0001
#define EMV_OFFLINE_DECLINE_AAC    0x0002
#define EMV_ONLINE_ARQC            0x0003
#define EMV_ONLINE_AAR             0x0004

    
#define AID_MAX                    22

#define EMV_VISA_CARD              1
#define EMV_MASTER_CARD            2
#define EMV_JCB_CARD               3
#define EMV_AMEX_CARD              4    



/* Transaction Code */
#define SALE         101
#define PRE_AUTH     102
#define PRE_COMP     103
#define TC_UPLOAD    104
#define SEND_ADVICE  105
#define REVERSAL     106
#define REFUND       107
#define VOID         108
#define SALE_TIP     109
#define SALE_OFFLINE 110
#define SETTLE       111
#define CLS_BATCH    112
#define BATCH_UPLOAD 113
#define SALE_ADJUST  114
#define SIGN_ON      115
#define EPP_SALE      116
#define REPRINT_ANY      117
#define BATCH_REVIEW    118
#define BATCH_TOTAL     119
#define VOID_REFUND         120
#define OFFLINE_VOID        121
#define OFFLINE_REFUND      122
#define VOID_REVERSAL       123
#define REFUND_REVERSAL     124
#define PREAUTH_REVERSAL    125
#define VOIDREFUND_REVERSAL 126

#define PURCHASE_CASHBACK	127
#define PURCHASE_ADVICE		128
#define CASH_ADVANCE		129

#define CUP_LOGON			130
#define CUP_SALE			131
#define CUP_PRE_AUTH		132
#define CUP_PREAUTH_COMP	133
#define CUP_VOID_PREAUTH	134
#define CUP_SALE_ADJUST		135

#define MPU_SIGNON			140
#define MPU_SIGNOFF			141
#define MPU_SETTLE                      142

#define MPU_PREAUTH                     143
#define MPU_PREAUTH_COMP                144
#define MPU_PREAUTH_COMP_ADV            145
#define MPU_VOID_PREAUTH                146
#define MPU_VOID_PREAUTH_COMP           147

#define EFTSEC_TWK 			150
#define EFTSEC_TMK 			151

#define EFTSEC_TWK_RSA 		152
#define EFTSEC_TMK_RSA 		153

#define REVERSAL_CASH_ADV       154

#define MPU_PREAUTH_REV					155
#define MPU_PAC_REV						156
#define MPU_PAC_ADV_REV					157
#define MPU_VOID_PREAUTH_REV			158
#define MPU_VOID_PAC_REV				159

#define AUTO_REVERSAL  		160

#define VOID_PREAUTH            161
#define VOID_PREAUTH_REV        162
#define IPP_SIGN_ON             163

#define PREAUTH_COMP            164
#define VOID_PREAUTH_COMP       165

#define QR_INQUIRY                  170
#define ALIPAY_SALE                 171
#define ALIPAY_REVERSAL             172
#define ALIPAY_VOID_REVERSAL        173
#define ALIPAY_VOID                 174
#define ALIPAY_SALE_BATCH_UPLOAD    175

#define TOPUP 180
#define RELOAD 181
#define BALANCE_ENQUIRY 182
#define TOPUP_RELOAD_REVERSAL 183
#define TOPUP_RELOAD_TRANS 184
#define TOPUP_RELOAD_LOGON 185

#define CB_PAY_TRANS 186

#define TRANS_ENQUIRY 187
#define CHANGE_PIN			188
#define OK_DOLLAR_TRANS 190
#define SIGNON_ALL 191


#define ECR_ECHO                200
#define DEMO_MODE                201

#define SETUP				15 //aaa issue#87 No password protected for setup menu

/* To determine the type of results */
#define CN_FALSE   0
#define CN_TRUE    1

/* Results of the state */
#define ST_SUCCESS          0
#define ST_ERROR            (-1)
#define ST_BUILD_DATD_ERR   (-2)
#define ST_SEND_DATA_ERR    (-3)
#define ST_UNPACK_DATA_ERR  (-4)
#define ST_RESP_MATCH_ERR  (-5)
#define ST_CLS_BATCH_ERR (-6)



/* Values for chip status flag */
#define NOT_USING_CHIP       0
#define EMV_CARD             1
#define EMV_EASY_ENTRY_CARD  2
#define EMV_TABLE_NOT_USED  -1

/* The status of the transaction */
#define TRANS_AUTHORIZED    99
#define TRANS_COMM_ERROR    98
#define TRANS_CALL_BANK     97
#define TRANS_CANCELLED     96
#define TRANS_REJECTED      95
#define TRANS_TERMINATE     94

/* Commuction mode */
#define DIAL_UP_MODE        0
#define ETHERNET_MODE       1
#define GPRS_MODE           2
#define MDM_PPP_MODE		3
#define WIFI_MODE			4
#define USB_MODE           	5
#define COM1_MODE           6//for COM&USB communication
#define COM2_MODE           7
#define NULL_MODE           9






#define STORE_ID_DIGITS            18
#define TRACK_I_BYTES              85	//85
#define TRACK_II_BYTES             41	//41
#define TRACK_III_BYTES            200	//64
#define CVV2_BYTES                 6
#define CARD_HOLD_NAME_DIGITS      30
#define AUTH_CODE_DIGITS           6
#define PAN_SIZE                   19
#define RRN_BYTES                  12
#define RESP_CODE_SIZE             2
#define TERMINAL_ID_BYTES          8
#define MERCHANT_ID_BYTES          15
#define CHIP_DATA_LEN              1024
#define ADD_DATA_LEN             1024
#define CARD_ACCEPTOR_NAME_BYTES          40


/*kobe added >>>*/
#define TRACE_NO_ASC_SIZE   6
#define TRACE_NO_BCD_SIZE   ((TRACE_NO_ASC_SIZE+1)/2)

#define BATCH_NO_ASC_SIZE   6
#define BATCH_NO_BCD_SIZE   ((BATCH_NO_ASC_SIZE+1)/2)

#define	EMV_TAC_SIZE            6
#define	EMV_MAX_TDOL_SIZE	    65
#define	EMV_MAX_DDOL_SIZE	    65
#define	EMV_COUNTRY_CODE_SIZE	3
#define	EMV_CURRENCY_CODE_SIZE	3
#define	EMV_TERM_CAPABILITIES_BCD_SIZE	4
#define EMV_ADD_TERM_CAPABILITIES_BCD_SIZE  6
#define	EMV_TERM_TYPE_SIZE	3
#define	EMV_MERCH_CAT_CODE_SIZE	5
#define EMV_TERM_CAT_CODE_SIZE	3
#define EMV_STRING_SIZE	25
#define TEL_DIGITS 		24				/* BCD   */
#define PABX_BYTES		4
#define PABX_DIGITS		8
#define NII_BYTES               4   //Meena 3 incresed to 4 as strNII size is 4
#define TPDU_BYTES              10   //Meena 3 incresed to 4 as strNII size is 4

#define DF_BATCH_APPEND 0
#define DF_BATCH_UPDATE 1


#define TPDU_ASC_SIZE              10
#define TPDU_BCD_SIZE           ((TPDU_ASC_SIZE+1)/2)

#define PRO_CODE_ASC_SIZE       6
#define PRO_CODE_BCD_SIZE       ((PRO_CODE_ASC_SIZE+1)/2)

#define MTI_ASC_SIZE            4
#define MTI_BCD_SIZE            ((MTI_ASC_SIZE+1)/2)

#define INVOICE_ASC_SIZE        6
#define INVOICE_BCD_SIZE        ((INVOICE_ASC_SIZE+1)/2)

#define DATE_ASC_SIZE                     4
#define DATE_BCD_SIZE                      ((DATE_ASC_SIZE+1)/2)

#define TIME_ASC_SIZE                     6
#define TIME_BCD_SIZE                      ((TIME_ASC_SIZE+1)/2)

#define EXPIRY_DATE_ASC_SIZE              4
#define EXPIRY_DATE_BCD_SIZE               ((EXPIRY_DATE_ASC_SIZE+1)/2)

#define AMT_ASC_SIZE                      12/* BCD - includes cents */
#define AMT_BCD_SIZE                       ((AMT_ASC_SIZE+1)/2)

#define GET_CARD_DATA_TIMEOUT_VALUE  60*100 //60 seconds
#define UI_TIMEOUT                   30*100

#define CARD_ENTRY_MSR                  1
#define CARD_ENTRY_MANUAL               2
#define CARD_ENTRY_ICC                  3
#define CARD_ENTRY_FALLBACK             4
#define CARD_ENTRY_WAVE                 5
#define CARD_ENTRY_EASY_ICC         	6

    
#define READ_CARD_TIMEOUT              -1001
#define USER_ABORT                     -1002
#define INVALID_CARD                   -1003
#define PLS_INSERT_CARD                -1004
#define MSR_NOT_ALLOW                  -1005
#define MANUAL_NOT_ALLOW               -1006
#define HOST_NOT_OPEN                  -1007
#define RECORD_INVALID                 -1008
#define CARD_EXPIRED                   -1009
#define TIME_OUT                       -1010
#define MODEM_FAILED                   -1011
#define READ_APPLET_ERR			   	   -1012
#define FUNCTION_ABORT                 -1013


#define CDT_AMEX                    1
#define CDT_VISA                    2
#define CDT_MASTER                  3
#define CDT_VISA1                   4
#define CDT_MASTER1                 5
#define CDT_VISA2                   6  // To include multiple host for same card type - Meena 03/01/13 
#define CDT_MASTER2                 7  // To include multiple host for same card type - Meena 03/01/13
    
#define SIGN_ON_HOST                0
#define MAIN_HOST                   1
#define MOTO_HOST                   3

#define PWDTYPE_TERMINAL	1
#define PWDTYPE_SETTLEMENT	2
#define PWDTYPE_VOID		3
#define PWDTYPE_TLE		4
    
#define d_MEGSTRIPE_SIZE   128
    
#define INTERACTTIMEOUT 3000 
#define DECIMAL_POINT_MODERATOR 10000    
    
#define GPRS_TIMEOUT_DISABLE	0
#define GPRS_TIMEOUT_ENABLE_VALUE  1500    


typedef struct
{
    BYTE byCardTypeNum;
    BYTE byEMVTransStatus;
    BYTE T5A_len;    // PAN_len
    BYTE T5A[10];    // PAN    
    BYTE T5F24[3];        // Application Expiration Date
    BYTE T5F2A[2];    // Currency Code
    BYTE T5F30[3];        // Service code.
    BYTE T5F34;        // PAN SEQ No.
    BYTE T5F34_len;        // PAN SEQ No len.
    BYTE T82[2];        // Application Interchange Profile (AIP)
    BYTE T84_len;    // AID Len
    BYTE T84[16];    // AID
    BYTE T8A[2];
    BYTE T91[16];    // Issuer Application data (IAD)
    BYTE T91Len;     //IAD length
    BYTE T95[5];        // Terminal Verification Results
    BYTE T9A[3];        // Transaction Date
    BYTE T9C;        // Trans Type
    BYTE T9F02[6];    // Amount, Authorised (Numeric)
    BYTE T9F03[6];    // Amount, others (Numeric)
    BYTE T9F06[16];    // Application Identifier (AID) ?C terminal
    BYTE T9F06_len;
    BYTE T9F09[2];    // ICC Application Version No.
    BYTE T9F0D[5];    // Issuer Action Code ?C Default
    BYTE T9F0E[5];    // IIssuer Action Code ?C Denial
    BYTE T9F0F[5];    // Issuer Action Code ?C Online
    BYTE T9F10_len;
    BYTE T9F10[32];    // Issuer Application data
    BYTE T9F1A[2];    // Terminal Country Code
    BYTE T9F26[8];    // Application Cryptogram (AC)
    BYTE T9F27;        // Cryptogram Information Data (CID)
    BYTE T9F33[3];    // Terminal Capabilities
    BYTE T9F34[3];    // Cardholder Verification Method (CVM) Results
    BYTE T9F35;        // Terminal Type
    BYTE T9F36_len;
    BYTE T9F36[2];    // Application Transaction Counter (ATC)
    BYTE T9F37[4];    // Unpredictable Number, (random number)
    BYTE T9F41[3];    // Original Trace No (Field 11)
    BYTE T9F42[2];    // applciation currency code
    BYTE T9F53;        // TCC,
    BYTE T9F63[16];     /*Card Product Identification Information*/    
    BYTE T9F63_len;    
    BYTE ISR[16];        // Issuer Script Result.
    BYTE ISRLen;        // Issuer Script Result.
    BYTE T9B[2];        // Transaction Status Information
    BYTE T9F6E[4];     /*Device Information*/    
    BYTE T9F6E_len;    
    BYTE T9F7C[32];     /*Partner Discretionary Data*/    
    BYTE T9F7C_len;          
    BYTE T71Len;    // Application Interchange Profile (AIP)
    BYTE T71[258];    // Application Interchange Profile (AIP)
    BYTE T72Len;    // Application Interchange Profile (AIP)
    BYTE T72[258];    // Application Interchange Profile (AIP)
    BYTE T9F1E[8];  // terminal hardware serial number
    BYTE T9F28[8];  // ARQC
    BYTE T9F29[8];  // TC
    BYTE szChipLabel[32];  // AP label    
}emvinfo_t;

typedef struct{
    char szInstallmentTerms[3+1];
    BYTE szMonthlyAmt[AMT_BCD_SIZE+1];
    char szInterestRate[12+1];
    BYTE szTotalInterest[AMT_BCD_SIZE+1];
    BYTE szHandlingFee[AMT_BCD_SIZE+1];
    char szIPPSchemeID[6+1];
    char szFreqInstallment[1+1];
    char szTransCurrency[3+1];
    BYTE szIPPTotalAmount[AMT_BCD_SIZE+1];
}ipp_t;


typedef struct
{
    BYTE        byTransType;
    BYTE        byPanLen;
    BYTE        szExpireDate[EXPIRY_DATE_BCD_SIZE+1];    /* BCD YYMM        */
    BYTE        byEntryMode;
    BYTE        szTotalAmount[AMT_BCD_SIZE+1];   /* BCD total   amount    */
    BYTE        szBaseAmount[AMT_BCD_SIZE+1];
    BYTE        szTipAmount[AMT_BCD_SIZE+1];
    BYTE        szTID[TERMINAL_ID_BYTES+1];
    BYTE        szMID[MERCHANT_ID_BYTES+1];
    BYTE        szHostLabel[16];
    BYTE        szBatchNo[BATCH_NO_BCD_SIZE+1];
    BYTE        byOrgTransType ;
    BYTE        szMacBlock[8] ;
    BYTE        szYear[2];                 /* int 1997=97,2003=103    */
    BYTE        szDate[DATE_BCD_SIZE+1];     /* BCD MMDD        */
    BYTE        szTime[TIME_BCD_SIZE+1];     /* BCD hhmmss        */
    BYTE        szOrgDate[DATE_BCD_SIZE+1];     /* BCD MMDD        */
    BYTE        szOrgTime[TIME_BCD_SIZE+1];     /* BCD hhmmss        */
    BYTE        szAuthCode[AUTH_CODE_DIGITS+1];
    BYTE        szRRN[RRN_BYTES+1];
    BYTE        szInvoiceNo[INVOICE_BCD_SIZE+1];
    BYTE        szOrgInvoiceNo[INVOICE_BCD_SIZE+1];
    BYTE        byPrintType;//for trans comms dailbakup
    BYTE        byVoided;
    BYTE        byAdjusted;
    BYTE        byUploaded;
    BYTE        byTCuploaded;    /* For upload TC*/
    BYTE        szCardholderName[CARD_HOLD_NAME_DIGITS+1];
    BYTE        szzAMEX4DBC[4+1];
    BYTE        szStoreID[STORE_ID_DIGITS+1]; //use this one to store how much amount fill up in DE4 for VOID
    BYTE        szRespCode[RESP_CODE_SIZE+1];
    BYTE        szServiceCode[3+1];
    BYTE        byContinueTrans;
    BYTE        byOffline;
    BYTE        byReversal;
    BYTE        byEMVFallBack;
    SHORT       shTransResult;
    BYTE        szTpdu[TPDU_ASC_SIZE+1];
    BYTE        szIsoField03[PRO_CODE_BCD_SIZE+1];
    BYTE        szMassageType[MTI_BCD_SIZE+1];
    BYTE        szPAN[PAN_SIZE+1];
    BYTE        szCardLable[20+1];
    USHORT      usTrack1Len;
    USHORT      usTrack2Len;
    USHORT      usTrack3Len;
    BYTE        szTrack1Data[TRACK_I_BYTES+1];    
    BYTE        szTrack2Data[TRACK_II_BYTES+1];
    BYTE        szTrack3Data[TRACK_III_BYTES+1];
	USHORT      usChipDataLen;		//Chip Data
	BYTE        baChipData[CHIP_DATA_LEN+1];
	USHORT      usAdditionalDataLen;	//Additional Data
	BYTE        baAdditionalData[ADD_DATA_LEN+1];
	BYTE		bWaveSID;
	USHORT		usWaveSTransResult;	// Transction Result
	BYTE		bWaveSCVMAnalysis;
    ULONG       ulTraceNum; 
    ULONG       ulOrgTraceNum;
    USHORT      usTerminalCommunicationMode;
    BYTE        IITid;
    BYTE        HDTid; //kobe added
    ULONG       ulSavedIndex;// kobea added  
    BYTE        byPINEntryCapability;
    BYTE        byPackType;
	int  		inCardType;
	UINT 		MITid;
    BYTE        szOrgAmount[AMT_BCD_SIZE+1];
    BYTE        szCVV2[CVV2_BYTES+1];
    BYTE        byTCFailUpCnt;//20121204
    BYTE		szPINBlock[8+1];
	BYTE		szNewPINBlock[8+1];			
	BYTE		szVerifyNewPINBlock[8+1];			
	BYTE		szKSN[20+1];
    BYTE        szFixedAmount[AMT_BCD_SIZE+1];	
	BYTE        szPercentage[AMT_BCD_SIZE+1];		
    BYTE        szOrigAmountDisc[AMT_BCD_SIZE+1];		
	BOOL 		fIsDiscounted;	
	BOOL 		fIsDiscountedFixAmt;
	BOOL 		fIsDiscountedPercAmt;	
    SHORT  		CDTid;
    emvinfo_t   stEMVinfo;
    
    //  ECR
    
    BOOL fIsInstallment;
    
    ipp_t stIPPinfo;
    BYTE byQRData[ADD_DATA_LEN+1];
    int inQRDataLen;
	BOOL fAlipay;
	BOOL fSendAlipayAutoReversal;
	BYTE szPreAuthCode[AUTH_CODE_DIGITS+1];

	BYTE szCurrCode1[3+1];
	BYTE szCurrCode2[3+1];
	BYTE szSign1[1+1];
	BYTE szSign2[1+1];
	BYTE szBalAmount1[12+1];
	BYTE szBalAmount2[12+1];
	BYTE szKCV[6+1];


	//For CBPAY
	BYTE szTransRef[16+1];
	BYTE szRefNo[16+1];
	BYTE szBankTransId[32+1]; 
	BOOL fECRPreauth;
	BYTE byECRPreauthComp;

	//For OK$
	BYTE szOKDTransRef[32+1];
	BYTE szOKDRefNo[16+1];//[32+1];
	BYTE szOKDBankTransId[32+1]; 
	BYTE szOKDCustomerNum[30+1];
	BYTE szOKDCustomerNumPword[50+1];
	
}TRANS_DATA_TABLE;

TRANS_DATA_TABLE srTransRec;

BYTE szMacData[ADD_DATA_LEN+1];
int inMacDataCnt;
BYTE szTMK[16+1];

/*use for EMV tag 9F53*/
#define baEMVBackupT9F53 srTransRec.szzAMEX4DBC

#define RGB(r,g,b) (unsigned long)( ((DWORD)(BYTE)r&0xff)|((DWORD)((BYTE)g&0xff)<<8)|((DWORD)((BYTE)b&0xff)<<16) )

/* <<< end*/

//#define _TRANS_CONNECT_USE_RS232_

//=============================================================================================================================
//
// Virtual Functions
// for V3 touch panel
//
//=============================================================================================================================
/*
typedef struct
{
    USHORT LeftTopX;
    USHORT LeftTopY;
    USHORT RightBottomX;
    USHORT RightBottomY;
}CTOS_VIRTUAL_FUNCTION_KEY_SET_PARA;

USHORT CTOS_VirtualFunctionKeySet(IN CTOS_VIRTUAL_FUNCTION_KEY_SET_PARA* pPara, IN BYTE FuncKeyCount);
*/

/*==================================================*
 * 	Modem PPP Definition Table(MPT) Struct          *
 *==================================================*/
typedef struct
{
	int		HDTid ;
	BYTE	szHostName[50];	/*  */
	int		inCommunicationMode;
	BYTE	szPriISPPhoneNumber[30];	/* ASCII string */
	BYTE	szSecISPPhoneNumber[30];	/* ASCII string */
	
	int 	inHandShake;
	int 	inParaMode;
	int 	inCountryCode;
	
	int		inDialHandShake;	/* ASCII string */
	int		inMCarrierTimeOut;	/* ASCII string */
    int   	inMRespTimeOut;

	BYTE	szUserName[30] ;
    BYTE	szPassword[30] ;
	int 	inTxRxBlkSize;
	
} STRUCT_MPT;
STRUCT_MPT      strMPT;

typedef struct{
    BYTE szCardType[2+1];
    ULONG ulTraceNum;
    BYTE szPAN[20+1];
    BYTE szAmount[AMT_BCD_SIZE+1];
} BATCH_UP_DETAIL;


typedef struct{
    int inBatchCount;
    
    BATCH_UP_DETAIL stBatchDetail[8];
} BATCH_UP_DATA;

BATCH_UP_DATA strBatchUp;

#define CBC_MODE        1

//#define APP_AUTO_TEST

#define ALIPAY_HOST_INDEX   	21
#define WPAY_HOST_INDEX   		12
#define QQWALLET_HOST_INDEX 	11


#define CBPAY_HOST_INDEX 	13
#define CBPAY_DV 	//0 // for CB Pay development

#define RIDDING_AMEX				1
#define RIDDING_AMEX_ALIPAY			2
#define RIDDING_AMEX_WECHAT			3
#define RIDDING_AMEX_ALIPAY_WECHAT	4
#define RIDDING_ALIPAY				5
#define RIDDING_WECHAT				6
#define RIDDING_ALIPAY_WECHAT		7
#define RIDDING_QQWALLET			8
#define RIDDING_ALIPAY_QQWALLET		9
#define RIDDING_WECHAT_QQWALLET		10
#define RIDDING_ALIPAY_WECHAT_QQWALLET		11
#define RIDDING_AMEX_QQWALLET				12
#define RIDDING_AMEX_ALIPAY_QQWALLET		13
#define RIDDING_AMEX_WECHAT_QQWALLET		14
#define RIDDING_AMEX_ALIPAY_WECHAT_QQWALLET	15
#define RIDDING_AE_VM						16

#define POSITION_LEFT 1
#define POSITION_RIGHT 2

#define DUAL_SIM_SETTINGS
#define PARKING_FEE

#define TOPUP_RELOAD
//#define CONVERT_ISO_TO_STRING

#define DISCOUNT_FEATURE

#define QUICKPASS

#define EMV_Y1_DISABLE

#define PIN_CHANGE_ENABLE
#define ENTER_DEFAULT_PIN 0
#define ENTER_NEW_PIN  	  1
#define ENTER_VERIFY_PIN  2
#define ENTER_PIN         3
#define PIN_SIZE		  8


//#define DEBUG_PRINT_ON_PAPER

// change request, all UPI quickpass (CTLS) txn should be routed to Finexus host. case #1468
//#define QUICKPASS_FNX_HOST 

#define MINOR_CHANGES

#define TAG_001 "001"
#define TAG_002 "002"
#define TAG_003 "003"
#define TAG_021 "021"
#define TAG_025 "025"
#define TAG_026 "026"
#define TAG_038 "026"
#define TAG_090 "090"
#define TAG_100 "100"
#define TAG_101 "101"
#define TAG_103 "103"

#define d_VW_SID_CUP_EMV					0x91

//#define d_VW_SID_JCB_WAVE_2				0x64
//#define d_VW_SID_JCB_WAVE_QVSDC			0x65

#define d_EMVCL_SID_JCB_EMV 0x63
#define d_EMVCL_SID_JCB_MSD 0x64
#define d_EMVCL_SID_JCB_LEGACY 0x65
#define d_EMVCL_SID_JCB_LEGACY2 0x99


#define ECR_PREAUTH_AND_COMP
#define CB_MPU_NH_MIGRATION
#define ERM_ERROR_RECEIPT_FEATURE
#define CASH_ADV_NEW_FEATURE
#define JCB_NO_SIGNLINE
#define JCB_LEGACY_FEATURE

#define ENHANCEMENT_1861
#define MPU_CARD_TC_UPLOAD_ENABLE // enble tc upload for mpu host


#define OK_DOLLAR_FEATURE
#define OK_DOLLAR_HOST_INDEX 	14
#define IS_SPACE 0x20
#define IS_ZERO 0x30
#define MPU_PREAUTH_ENABLE //7,8 AND 9 MPU HOSTS
#define OK_DOLLAR_FALLBACK_URL

#define NEW_MPOS_ISSUE // for CBB_OK$_Test_Result bug tracker

#define APP_VERSION
#define CTMS_IP_TO_URL_MODE

#define SET_IPP_MENU // to allign menuid 3 (IPP menu)

//#define IPP_TRANS //for Retail + IPP setup (MENUID 5)

//FOR cr http://118.201.48.214:8080/issues/75
#define CBB_FIN_ROUTING

//http://118.201.48.214:8080/issues/75 >>  #75.2 Change Request_Change on Refund 
#define CBB_CR_REFUND
//#define MPUPIOnUsFlag
#define CBB_TUP_ROUTING
#define CBB_CAV_ROUTING


#ifdef    __cplusplus
}
#endif

#endif    /* TYPEDEF_H */

