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

struct FtpFile {
  const char filename[300+1];
  FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb,
                        void *stream)
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) {
    /* open file for writing */
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1; /* failure, can't open file to write */
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

int inFtpsGet(unsigned char *uszFileName, unsigned char *uszFtpURL, unsigned char *uszFtpUserName, unsigned char *uszFtpPassword)
{
  CURL *curl;
  CURLcode res;
  struct FtpFile ftpfile;
  /*
	struct FtpFile ftpfile={
	  NULL, // name to store the file as if successful 
	  NULL
	};
*/		  
  unsigned char uszCURLURL[1000+1];
  unsigned char uszCURLAccount[500+1];

	vdDebug_LogPrintf("inFtpsGet");
	memset(&ftpfile,0x00,sizeof(ftpfile));

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
  
  memset((char *)ftpfile.filename, 0x00, sizeof((char *)ftpfile.filename));
  strcpy((char *)ftpfile.filename, (char *)PUBLIC_PATH);
  strcat((char *)ftpfile.filename, (char *)uszFileName);

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

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if(curl) {
    /*
     * You better replace the URL with one that works! Note that we use an
     * FTP:// URL with standard explicit FTPS. You can also do FTPS:// URLs if
     * you want to do the rarer kind of transfers: implicit.
     */
    curl_easy_setopt(curl, CURLOPT_URL, uszCURLURL);
	curl_easy_setopt(curl, CURLOPT_USERPWD, uszCURLAccount); 
	
    /* Define our callback to get called when there's data to be written */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

    /* We activate SSL and we require it for both control and data */
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    /* Switch on full protocol/debug output */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    if(CURLE_OK != res) {
      /* we failed */
      fprintf(stderr, "curl told us %d\n", res);
    }
  }

  if(ftpfile.stream)
    fclose(ftpfile.stream); /* close the local file */

  curl_global_cleanup();

  vdDebug_LogPrintf("inFtpsGet END");

  inCTOS_inDisconnect();

  return 0;
}

