#ifndef __SAM_H__
#define __SAM_H__


#define FELICA_SAM			"FELICA_SAM"

#define READ_WO_ENC			0
#define READ_ENC			1

#define METHOD_CHARGE		0x00
#define METHOD_PAYMENT		0x01

static unsigned char SYSTEM_CODE[] = {0x00, 0x18};

// Mutual Authentication key
// ######################################################################################
// # WARNING: Key value should not be written in the source code in commercial version. #
// ###################################################################################### 
static const unsigned char AuthNormalKey[16] = {0x12,0x34,0x12,0x34,0x12,0x34,0x12,0x34,0x12,0x34,0x12,0x34,0x12,0x34,0x12,0x34};


// Reader Writer related definition
#define SCARD_S_SUCCESS			0

#define UNKOWN_ERROR			1
#define RCS500_LCLE_ERROR		2
#define RCS500_P1P2_ERROR		3
#define RCS500_INS_ERROR		4
#define RCS500_CLA_ERROR		5
#define RCS500_MAC_ERROR		8


#define APP_SUCCESS		d_OK
#define APP_ERROR		d_NO

#define PrintText		vdDebug_LogPrintf

unsigned char ucTransmit_APDU(unsigned long ulSlotNo, unsigned char *szAPDUDReq, unsigned short txLen, unsigned char *szAPDURes, unsigned short *rxLen);


#define SAM_COMMAND_CODE_POLLING					0x80
#define SAM_RESPONSE_CODE_POLLING					0x81

#define SAM_COMMAND_CODE_MUTUAL_AUTH_RWSAM			0xe2
#define SAM_COMMAND_CODE_MUTUAL_AUTH_V2_RWSAM		0xe4
#define SAM_SUB_COMMAND_CODE_MUTUAL_AUTH_V2_RWSAM	0x80
#define SAM_COMMAND_CODE_MUTUAL_AUTH_V2				0x96
#define SAM_COMMAND_CODE_MUTUAL_AUTH				0x86
#define SAM_SUB_COMMAND_CODE_REQUEST_SERVICE_V2		0x8A
#define SAM_SUB_COMMAND_CODE_REGISTER_AREA_V2		0x02
#define SAM_SUB_COMMAND_CODE_REQUEST_SERVICE_V2_RWSAM	0x8A

#define SAM_COMMAND_CODE_REGISTER_AREA_v2			0xB6
#define SAM_COMMAND_CODE_REGISTER_AREA				0xC2
#define SAM_COMMAND_CODE_REQUEST_SERVICE_V2_EX		0xD2
#define SAM_COMMAND_CODE_REQUEST_SERVICE			0x82
#define SAM_COMMAND_CODE_POLLING					0x80
#define SAM_RESPONSE_CODE_POLLING					0x81

#define SAM_RESPONSE_CODE_READ_WO_ENC				0x99

#define SAM_COMMAND_CODE_WRITE						0x8a
#define SAM_COMMAND_CODE_WRITE_WO_ENC				0x9a
#define SAM_COMMAND_CODE_READ						0x88
#define SAM_COMMAND_CODE_READ_WO_ENC				0x98

#define SAM_COMMAND_CODE_REG_ISSUE_IDEX_v2			0xB6
#define SAM_COMMAND_CODE_REG_ISSUE_IDEX				0xC6
#define SAM_COMMAND_CODE_REG_ISSUE_ID				0xC0
#define SAM_SUB_COMMAND_CODE_REGISTER_SERVICE_V2	0x04
#define SAM_COMMAND_CODE_REGISTER_SERVICE_V2		0xB6
#define SAM_COMMAND_CODE_REGISTER_SERVICE			0xC4

/**
Initialize SAM.
*/
long InitializeSAM(void);

/**
Send request to generate FeliCa Command to SAM.
This function sets 0x00 as Sub command code.  

\param commandCode				Command code for SAM
\param [in] felicaCmdParamsLen	length of "felicaCmdParams"
\param [in] felicaCmdParams		command parameters after Snr of command gereration request packtes to SAM
\param [out] felicaCommandLen	length of "_felica_cmd"
\param [out] felicaCommand		generated FeliCa command

\return  APP_SUCCESS, APP_ERROR
*/
long AskFeliCaCmdToSAM(unsigned char commandCode, 
					   unsigned long felicaCmdParamsLen,
					   unsigned char felicaCmdParams[],
					   unsigned long* felicaCommandLen,
					   unsigned char felicaCommand[]);

/**
Send request to generate FeliCa Command to SAM

\param commandCode				Command code for SAM
\param subCommandCode			Sub command code for SAM
\param [in] felicaCmdParamsLen	length of "felicaCmdParams"
\param [in] felicaCmdParams		command parameters after Snr of command gereration request packtes to SAM
\param [out] felicaCommandLen	length of "felicaCommand"
\param [out] felicaCommand		generated FeliCa command packets(starts with command code)

\return  APP_SUCCESS, APP_ERROR
*/
long AskFeliCaCmdToSAMSC(unsigned char commandCode, 
						unsigned char subCommandCode, 
						unsigned long felicaCmdParamsLen,
						unsigned char felicaCmdParams[],
						unsigned long* felicaCommandLen,
						unsigned char felicaCommand[]);



/**
Send Polling response of FeliCa Card to SAM.
SAM analyzes FeliCa response packets, and returns 

\param [in] felicaResLen	length of "felicaResponse"
\param [in] felicaResponse	response packets from FeliCa Card(packets data starts with response code)

\return  APP_SUCCESS, APP_ERROR
*/
long SendPollingResToSAM(unsigned long felicaResLen, 
						 unsigned char felicaResponse[] );

/**
Send Authentication1V2 response of FeliCa Card to SAM.

\param [in] felicaResLen	length of "felicaResponse"
\param [in] felicaResponse	response packets from FeliCa Card(packets data starts with response code)
\param [out] auth2V2CommandLen	length of "auth2V2Command"
\param [out] auth2V2Command		Authentication V2 command packets to be sent to FeliCa card(packets data starts with command code)

\return  APP_SUCCESS, APP_ERROR
*/
long SendAuth1V2ResultToSAM(unsigned long felicaResLen, 
							unsigned char felicaResponse[], 
							unsigned long* auth2V2CommandLen,
							unsigned char auth2V2Command[]);


/**
Send RC-S500 command to SAM

\param commandCode				Command code for SAM
\param subCommandCode			Sub command code for SAM
\param [in] felicaCmdParamsLen	length of "felicaCmdParams"
\param [in] felicaCmdParams		command parameters after Snr of command gereration request packtes to SAM
\param [out] felicaCommandLen	length of "felicaCommand"
\param [out] felicaCommand		generated FeliCa command packets(starts with command code)

\return  APP_SUCCESS, APP_ERROR
*/
long SendRWSAMCmd( unsigned char commandCode,
				   unsigned char subcommandCode,
				   unsigned long felicaCmdParamsLen,
				   unsigned char felicaCmdParams[],
				   unsigned long* felicaCommandLen,
				   unsigned char felicaCommand[]);


/**
Send Register related(Area/Service/Issue...) response of FeliCa Card to SAM.

\param [in] felicaResLen	length of "felicaResponse"
\param [in] felicaResponse	response packets from FeliCa Card(packets data starts with response code)
\param [out] auth2V2CommandLen	length of "auth2V2Command"
\param [out] auth2V2Command		Authentication V2 command packets to be sent to FeliCa card(packets data starts with command code)

\return  APP_SUCCESS, APP_ERROR
*/
long SendRegisterResultToSAM(unsigned long felicaResLen, 
							 unsigned char felicaResponse[], 
							 unsigned long* chgsysblkV2CommandLen,
							 unsigned char chgsysblkV2Command[]);

/**
Send response of FeliCa Card to SAM.
SAM analyzes FeliCa response packets, and returns some parameters
extracted from FeliCa response.

\param [in] felicaResLen	length of "felicaResponse"
\param [in] felicaResponse	response packets from FeliCa Card(packets data starts with response code)
\param [out] resultLen		length of "result"
\param [out] result			response packets data from SAM (packets after Snr)

\return  APP_SUCCESS, APP_ERROR
*/
long SendCardResultToSAM(unsigned long felicaResLen, 
						 unsigned char felicaResponse[], 
						 unsigned long* resultLen, 
						 unsigned char result[]);

/**
Report FeliCa Card Error to SAM.

\param [out] resultLen		length of "result"
\param [out] result			response packets data from SAM (packets after Snr)

\return  APP_SUCCESS, APP_ERROR
*/
long SendCardErrorToSAM( unsigned long* resultLen, 
						 unsigned char result[]);


void vdFelica_SAMSlotConfig(void);


#endif //end __SAM_H__
