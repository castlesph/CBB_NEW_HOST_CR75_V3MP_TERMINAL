/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

#include <unistd.h>
#include <pwd.h>

#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include <stdio.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "..\Includes\CfgExpress.h"
#include "..\Debug\Debug.h"

#include "..\FileModule\myFileFunc.h"
#include "..\Comm\V5Comm.h"
#include "..\Database\DatabaseFunc.h"

int inFtpsPut(unsigned char *uszFileName, unsigned char *uszFtpURL, unsigned char *uszFtpUserName, unsigned char *uszFtpPassword)
{
  CURL *curl;
  CURLcode res;
  struct stat file_info;
  double speed_upload, total_time;
  FILE *fd;
  unsigned char uszCURLFileName[300+1];
	  
  unsigned char uszCURLURL[1000+1];
  unsigned char uszCURLAccount[500+1];

	vdDebug_LogPrintf("inFtpsPut");
  
  inCPTRead(1);
  
  strcpy((char*)strCPT.szPriTxnHostIP, (char *)uszFtpURL);
  strCPT.inPriTxnHostPortNum = 21; // ftp server definitely  is port 21
  strTCT.fShareComEnable = 1;
  
//  strCPT.inCommunicationMode = GPRS_MODE;
  
  if (inCTOS_InitComm(strCPT.inCommunicationMode) != d_OK) 
  {
	  return(d_NO);
  }
  
  inCTOS_CheckInitComm(strCPT.inCommunicationMode); 
  if (srCommFuncPoint.inCheckComm(&srTransRec) != d_OK)
  {
	  inCTOS_inDisconnect();
	  return(d_NO);
  }
  
  if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
  {
	  inCTOS_inDisconnect();
	  return ST_ERROR;
  }

  memset(uszCURLFileName, 0x00, sizeof(uszCURLFileName));
  strcpy(uszCURLFileName, PUBLIC_PATH);
  strcat(uszCURLFileName, uszFileName);

  memset(uszCURLURL, 0x00, sizeof(uszCURLURL));
  strcpy(uszCURLURL, "ftp://");
  strcat(uszCURLURL, uszFtpURL);
  strcat(uszCURLURL, "/");
  strcat(uszCURLURL, uszFileName);

  memset(uszCURLAccount, 0x00, sizeof(uszCURLAccount));
  strcpy(uszCURLAccount, uszFtpUserName);
  strcat(uszCURLAccount, ":");
  strcat(uszCURLAccount, uszFtpPassword);

  vdDebug_LogPrintf("uszCURLAccount=[%s]", uszCURLAccount);

  fd = fopen(uszCURLFileName, "rb"); /* open file to upload */
  if(!fd) {

    return 1; /* can't continue */
  }

  /* to get the file size */
  if(fstat(fileno(fd), &file_info) != 0) {

    return 1; /* can't continue */
  }

  curl = curl_easy_init();
  if(curl) {
    /* upload to this place */
    curl_easy_setopt(curl, CURLOPT_URL,uszCURLURL);
	curl_easy_setopt(curl, CURLOPT_USERPWD, uszCURLAccount); 

    /* tell it to "upload" to the URL */
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* set where to read from (on Windows you need to use READFUNCTION too) */
    curl_easy_setopt(curl, CURLOPT_READDATA, fd);

    /* and give the size of the upload (optional) */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)file_info.st_size);

    /* We activate SSL and we require it for both control and data */
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    /* enable verbose for easier tracing */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    }
    else {
      /* now extract transfer info */
      curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed_upload);
      curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);

      fprintf(stderr, "Speed: %.3f bytes/sec during %.3f seconds\n",
              speed_upload, total_time);

    }
    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  vdDebug_LogPrintf("inFtpsPut END");

  inCTOS_inDisconnect();
  
  return 0;
}

