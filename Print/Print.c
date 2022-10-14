#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <ctype.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <ctos_qrcode.h>
#include <TextToBmp.h>
#include <epad.h>

#include "../Includes/wub_lib.h"
#include "../Includes/myEZLib.h"
#include "../Includes/msg.h"

#include "Print.h"
#include "../FileModule/myFileFunc.h"
#include "../UI/Display.h"
#include "../Includes/POSTypedef.h"
#include "..\Includes\CTOSInput.h"
#include "../accum/accum.h"
#include "../DataBase/DataBaseFunc.h"
#include "..\debug\debug.h"
#include "..\Includes\Showbmp.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSSetting.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"
//#include "..\Includes\epad.h"
#include "..\Ctls\POSCtls.h"
#include "..\Erm\PosErm.h"
#include "..\Md5\CRCmd5.h"
#include "..\PCI100\COMMS.h"
#include "..\powrfail\POSPOWRFAIL.h"
#include "..\External\External.h"
#include "..\Comm\V5Comm.h"
#include "../Includes/POSIpp.h"
#include "..\Includes\DMenu.h"

#define	d_FONT_TAHOMABOLD_TTF										  "tahomabd.ttf"
#define d_FONT_CONSOLAB_TTF                                           "consolab.ttf"
//MPOS missing txn amount by using this font.
//#define d_FONT_COURBD_TTF                                                                     "courbd.ttf"
#define d_FONT_COURBD_TTF                                             "cousinebd.ttf"

const BYTE baPrinterBufferLogo_Single[]={ //Width=60, Height=49
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                         
                                                                                       
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x60,0x30,0x18,0x18,0x8C,0xC4,0xC6,      
    0x66,0x66,0x22,0x22,0x22,0x22,0x22,0x62,0x66,0x46,0xC4,0x84,0x8C,0x18,0x10,0x30,      
    0x60,0xC0,0xC0,0xE0,0x38,0x0E,0x07,0x1E,0x38,0xE0,0x80,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                         
                                                                                       
    0x00,0x00,0x00,0x00,0xC0,0xF8,0x0E,0x03,0x81,0xF0,0x1C,0x0E,0x03,0xC1,0xF0,0xF8,      
    0x7C,0x3C,0x1C,0x1E,0x1E,0x1E,0xFE,0xFC,0xFC,0xFC,0xF8,0xF0,0xC1,0x83,0xCE,0x7C,      
    0x1C,0x07,0x81,0xE0,0x70,0x1C,0x0E,0x1C,0x70,0xC0,0x83,0x07,0x1C,0x70,0xE0,0x80,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                         
                                                                                      
    0x00,0x00,0x00,0x00,0x0F,0x7F,0xC0,0x00,0x03,0x3F,0xF0,0x80,0x00,0x0F,0x3F,0x7F,  
    0xFF,0xFC,0xFC,0xFC,0xFC,0xFE,0xF3,0xFB,0xFF,0xFF,0x7F,0x3F,0x0F,0x03,0x81,0xE0,  
    0x38,0x0E,0x03,0xC0,0xF0,0x1C,0x1C,0x38,0xE0,0x81,0x03,0x0E,0x38,0x70,0xC0,0x03,  
    0x0E,0x1C,0x70,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,  
    0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x08,0x18,0x38,0x30,0x00,0x00,0x00,0x00,  
    0x80,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x80,0xC0,  
    0xC0,0xC0,0xC0,0x80,0x00,0x00,0xC0,0xFC,0xFC,0xFC,0xC0,0xC0,0x00,0xFC,0xFC,0xFC,  
    0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,  
    0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,  
    0x0C,0xFC,0xFC,0xFC,0x0C,0x0C,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,  
    0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,  
    0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,  
    0xC0,0xC0,0xC0,0xC0,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x80,  
    0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,  
    0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,  
    0xC0,0xC0,0xC0,0x80,0x80,0xC0,0xC0,0xC0,0x00,0x00,0x40,0xC0,0xC0,0x80,0x00,0x00,  
    0x00,0x00,0x80,0xC0,0xC0,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,  
    0xF0,0x78,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x38,0x30,0x00,0x00,0x00,0x00,  
    0x80,0x80,0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0xC0,0xC0,0xFC,0xFC,0xFC,0xC0,0x00,0x00,  
    0x00,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x80,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                     
                                                                                      
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x0E,0x18,0x30,0x61,0x63,0xC6,0x8C,0x88,  
    0x98,0x98,0x11,0x11,0x11,0x11,0x11,0x11,0x90,0x98,0x88,0x8C,0xC6,0x63,0x61,0x30,  
    0x18,0x0C,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x1C,0x70,0xE0,0x81,0x07,  
    0x0E,0x38,0xE0,0xC1,0x07,0x0E,0x38,0x70,0xC0,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,  
    0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x00,0x3F,  
    0xFF,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0xCF,0x8F,  
    0x9E,0x9C,0xFC,0xF8,0x20,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,  
    0x00,0x00,0x3E,0x7F,0xFF,0xCD,0x8C,0x8C,0x8C,0x8C,0xCF,0xCF,0x0E,0x08,0x00,0x03,  
    0xC7,0x8F,0x8E,0x9C,0xFC,0xF9,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3E,0x7F,0xFF,0xCD,0xCC,0x8C,0x8C,0x8C,  
    0xCF,0xCF,0x0F,0x0C,0x00,0x00,0x1C,0x7F,0xFF,0xE3,0xC1,0x80,0x80,0x80,0xC1,0xC1,  
    0x41,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x01,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,  
    0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x01,0xFF,0xFF,0xFF,0x00,0x00,0x08,0x3E,0x7F,0xFF,  
    0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x3E,  
    0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xE3,0x7F,0x3F,0x08,0x00,0x0C,0x7F,0xFF,0xE3,  
    0xC1,0x80,0x80,0x80,0xC1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x03,0x0F,0x3F,0xFC,0xF0,  
    0xF8,0x7E,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,  
    0x7F,0xF0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xE0,0x60,0x00,0x00,0x08,0x3E,  
    0x7F,0xFF,0xC1,0x80,0x80,0x80,0xC1,0xFF,0x7F,0x3E,0x00,0x00,0x80,0xC0,0xC0,0x00,  
    0x00,0x00,0x00,0x00,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0xFF,0xFF,0xFF,0x80,0x80,0x80,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x1C,  
    0x7F,0xFF,0xE1,0xC0,0x80,0x80,0xC0,0xE1,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xC0,0xC0,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                     
                                                                                      
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,  
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,  
    0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,  
    0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x0E,0x0E,  
    0x0D,0x09,0x09,0x08,0x0C,0x0F,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x0C,0x0F,0x0F,  
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,  
    0x00,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
    0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,  
    0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x01,  
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                                     
};

TRANS_TOTAL stBankTotal;
BOOL	fRePrintFlag = FALSE; 
extern int ginSingatureStatus;
extern int ginPinByPass;
	

static CTOS_FONT_ATTRIB stgFONT_ATTRIB;

#define DB_SIGN_BMP "signtest.bmp"
#define DB_SIGN_BMP_GZ "signtest.bmp.gz"


void vdSetGolbFontAttrib(USHORT FontSize, USHORT X_Zoom, USHORT Y_Zoom, USHORT X_Space, USHORT Y_Space)
{
    memset(&stgFONT_ATTRIB, 0x00, sizeof(stgFONT_ATTRIB));
    
    stgFONT_ATTRIB.FontSize = FontSize;      // Font Size = 12x24
	stgFONT_ATTRIB.X_Zoom = X_Zoom;		    // The width magnifies X_Zoom diameters
	stgFONT_ATTRIB.Y_Zoom = Y_Zoom;		    // The height magnifies Y_Zoom diameters

    stgFONT_ATTRIB.X_Space = X_Space;      // The width of the space between the font with next font
    stgFONT_ATTRIB.Y_Space = Y_Space;      // The Height of the space between the font with next font      
    
}

int inGetFontlen(CTOS_FONT_ATTRIB* ATTRIB)
{
	USHORT FontSize;
	USHORT X_Zoom;
	USHORT usCharPerLine = 32;

	FontSize = ATTRIB->FontSize;
	X_Zoom = ATTRIB->X_Zoom;
	switch(FontSize)
	{
		case d_FONT_8x16:
			if (X_Zoom == NORMAL_SIZE)
				usCharPerLine = 49;
			else
				usCharPerLine = 25;
			break;
		case d_FONT_9x18:
			if (X_Zoom == NORMAL_SIZE)
				usCharPerLine = 43;
			else
				usCharPerLine = 21;
			break;
		case d_FONT_12x24:
			if (X_Zoom == NORMAL_SIZE)
				usCharPerLine = 32;
			else
				usCharPerLine = 16;
			break;
		case d_FONT_16x16:
		case d_FONT_16x30:
			if (X_Zoom == NORMAL_SIZE)
				usCharPerLine = 23;
			else
				usCharPerLine = 12;
			break;
		case d_FONT_24x24:
			if (X_Zoom == NORMAL_SIZE)
				usCharPerLine = 16;
			else
				usCharPerLine = 8;
			break;

		default:
			usCharPerLine = 32;
	    	break;
	}

	return usCharPerLine;
}

short printCheckPaper(void)
{
	unsigned short inRet;
	unsigned char key;

	vdDebug_LogPrintf("printCheckPaper");
	while(1)
	{
		inRet = CTOS_PrinterStatus();
		vdDebug_LogPrintf("CTOS_PrinterStatus inRet=[%d]",inRet);
		if (inRet==d_OK)
		{
			CTOS_KBDBufFlush();//cleare key buffer
			return 0;
		}
		else if(inRet==d_PRINTER_PAPER_OUT)
		{
			vduiClearBelow(3);
			vduiWarningSound();
			vduiDisplayStringCenter(3,"PRINTER OUT OF");
			vduiDisplayStringCenter(4,"PAPER, INSERT");
			vduiDisplayStringCenter(5,"PAPER AND PRESS");
			vduiDisplayStringCenter(6,"ANY TO PRINT.");
			vduiDisplayStringCenter(8,"[X] CANCEL PRINT");

			CTOS_KBDGet(&key);
			if(key==d_KBD_CANCEL)
			{
				CTOS_KBDBufFlush();//cleare key buffer
				return -1;
			}	
		}
		else if (inRet==d_PRINTER_NOT_SUPPORT)//for enhance MP200
		{
			if (d_OK == isCheckTerminalMP200())
				return d_OK;
		}
	}	
}


void cardMasking(char *szPan, int style)
{
    int num, inLen=0;
    int i;
	char szTempPAN[19+1];
	
    if (style == PRINT_CARD_MASKING_1)
    {
        num = strlen(szPan) - 10;
        if (num > 0)
        {
            for (i = 0; i < num; i++)
            {
                szPan[6+i] = '*';
            }
        }
    }
    else if (style == PRINT_CARD_MASKING_2)
    {
        num = strlen(szPan) - 12;
        if (num > 0)
        {
            for (i = 0; i < num; i++)
            {
                szPan[12+i] = '*';
            }
        }
    } 
	else if(style == PRINT_CARD_MASKING_3)
    {
        num = strlen(szPan) - 4;
        if (num > 0)
        {
            for (i = 0; i < num; i++)
            {
                szPan[0+i] = '*';
            }
        }
    }
	else if(style == PRINT_CARD_MASKING_4)
	{
		num = 16;
		inLen=strlen(szPan);
		memset(szTempPAN, '*', num);
		memcpy(szTempPAN, szPan, 6);
		memcpy(szTempPAN+12, szPan+(inLen-4), 4);

		memcpy(szPan, szTempPAN, 16);
		szPan[16]=0x00;
	}

}

USHORT printDateTime(void)
{
    char szStr[d_LINE_SIZE*2 + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
  	CTOS_RTC SetRTC;
	char szYear[3];
//1102
	char sMonth[4];
	char szTempMonth[3];
	char szMonthNames[40];

	memset(sMonth,0x00,4);
	memset(szMonthNames,0x00,40);
	strcpy(szMonthNames,"JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
	memset(szTempMonth, 0x00, 3);

//1102
	
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);


	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	vdDebug_LogPrintf("year[%s],date[%02x][%02x]",srTransRec.szYear,srTransRec.szDate[0],srTransRec.szDate[1]);
    wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
    wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
	vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );

	if(atol(szTemp) == 0)
	{
		//sprintf(szTemp ,"%02d%02d",SetRTC.bDay,SetRTC.bMonth);
		sprintf(szTemp ,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);	

	}
	
	if(atol(szTemp1) == 0)
	{
		sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	
	}
	vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
//1102	
    //sprintf(szStr,"DATE/TIME : %02lu/%02lu/%02lu    %02lu:%02lu:%02lu",atol(szTemp)%100,atol(szTemp)/100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	memcpy(szTempMonth,&szTemp[0],2);
	memcpy(sMonth, &szMonthNames[(atoi(szTempMonth) - 1)* 3], 3);
	if (strTCT.inFontFNTMode == 1)
//		sprintf(szStr,"DATE/TIME :%s %02lu, %02lu   %02lu:%02lu:%02lu",sMonth,atol(szTemp)%100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
//            sprintf(szStr,"DATE/TIME :%02lu %s %02lu   %02lu:%02lu:%02lu",atol(szTemp)%100, sMonth,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
            sprintf(szStr,"DATE/TIME : %02lu/%02lu/%02lu    %02lu:%02lu:%02lu",atol(szTemp)%100,atol(szTemp)/100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	else
		//use ttf print
//	sprintf(szStr,"DATE/TIME :%s %02lu, %02lu      %02lu:%02lu:%02lu",sMonth,atol(szTemp)%100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
//            sprintf(szStr,"DATE/TIME :%02lu %s %02lu      %02lu:%02lu:%02lu",atol(szTemp)%100, sMonth, atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
            sprintf(szStr,"DATE/TIME : %02lu/%02lu/%02lu    %02lu:%02lu:%02lu",atol(szTemp)%100,atol(szTemp)/100,atol(srTransRec.szYear),atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
//1102    
    memset (baTemp, 0x00, sizeof(baTemp));		

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 

	//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
    return(result);
     
}

USHORT printTIDMID(void)
{
    char szStr[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];

//6.) To implement TID/MID masking via parameter config similar to PAN masking
#ifdef CBB_FIN_ROUTING	
	char szMaskTID[9+1];
	char szMaskMID[15+1];
	
	memset(szMaskTID,0x00,sizeof(szMaskTID));
	memset(szMaskMID,0x00,sizeof(szMaskMID));

	vdDebug_LogPrintf("printTIDMID CBB_FIN_ROUTING");
    vdDebug_LogPrintf("printTIDMID HDTid [%d] fGetMPUTrans [%d] IITid [%d]", srTransRec.HDTid, fGetMPUTrans(), srTransRec.IITid);
	vdDebug_LogPrintf("printTIDMID szTID[%s]", srTransRec.szTID);		
	vdDebug_LogPrintf("printTIDMID szMID[%s]", srTransRec.szMID);

	//fix for missing TID/ MID printout in Summary and Detail report
	if(srTransRec.IITid == 0)
		srTransRec.IITid = 7;
	
    inIITRead(srTransRec.IITid);
	
	vdCTOSS_PrintFormatTIDMID(srTransRec.szTID,szMaskTID,9,1);
	vdDebug_LogPrintf("printTIDMID szMaskTID[%s]", szMaskTID);	

	memset(szStr, ' ', d_LINE_SIZE);
    sprintf(szStr, "TID: %s", szMaskTID);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	vdCTOSS_PrintFormatTIDMID(srTransRec.szMID,szMaskMID,20,2);
	vdDebug_LogPrintf("printTIDMID szMaskMID[%s]", szMaskMID);		
    memset(szStr, ' ', d_LINE_SIZE);
	
    sprintf(szStr, "MID: %s", szMaskMID);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
	
#else	
	memset(szStr, ' ', d_LINE_SIZE);
    sprintf(szStr, "TID: %s", srTransRec.szTID);
    memset (baTemp, 0x00, sizeof(baTemp));		 
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

    memset(szStr, ' ', d_LINE_SIZE);
    sprintf(szStr, "MID: %s", srTransRec.szMID);
    memset (baTemp, 0x00, sizeof(baTemp));		 
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif
 
    
	vdDebug_LogPrintf("printTIDMID END");
    return (result);

}

USHORT printBatchInvoiceNO(void)
{
    char szStr[d_LINE_SIZE*2 + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    USHORT result;
    BYTE baTemp[PAPER_X_SIZE * 64];
        
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp2, ' ', d_LINE_SIZE);
	
	wub_hex_2_str(srTransRec.szBatchNo,szTemp1,3);
    wub_hex_2_str(srTransRec.szInvoiceNo, szTemp2, INVOICE_BCD_SIZE);
//        sprintf(szTemp2, "%06lu", srTransRec.ulTraceNum);
    vdMyEZLib_LogPrintf("invoice no: %s",szTemp2);
	//CBB use trace number
	if (strTCT.inFontFNTMode == 1)
	{
		#ifdef ENHANCEMENT_1861
    	sprintf(szStr, "BATCH NUM: %s  INV #: %s", szTemp1, szTemp2);		
    	#else
    	//sprintf(szStr, "BATCH NUM: %s   INV#: %s", szTemp1, szTemp2);		
    	sprintf(szStr, "BATCH NUM: %s TRACE#: %s", szTemp1, szTemp2);
		#endif
	}
	else
	{
		#ifdef ENHANCEMENT_1861
    	sprintf(szStr, "BATCH NUM: %s  INV #: %s", szTemp1, szTemp2);				
		#else
		//use ttf print
		//sprintf(szStr, "BATCH NUM: %s    INV#: %s", szTemp1, szTemp2);
		sprintf(szStr, "BATCH NUM: %s  TRACE#:%s", szTemp1, szTemp2);
		#endif
	}
    memset (baTemp, 0x00, sizeof(baTemp));		

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
    return(result);
}

USHORT printBatchNO(void)
{
    char szStr[d_LINE_SIZE + 1];
    char szTemp[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
	
	wub_hex_2_str(srTransRec.szBatchNo,szTemp,3);
    sprintf(szStr, "Batch NO: %s", szTemp);
    memset (baTemp, 0x00, sizeof(baTemp));		

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

	//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    return(result);
}

USHORT printHostLabel(void)
{
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;

    memset(szStr, 0x00, sizeof(szStr));
    sprintf(szStr, "HOST: %s", srTransRec.szHostLabel);
    memset (baTemp, 0x00, sizeof(baTemp));		

    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    //result = CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

	//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    return(result);
}

void vdTTFPrintCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
	
    usCharPerLine = inGetFontlen(&stgFONT_ATTRIB);
	
    //i = strlen(strIn);
	i = CTOSS_strlen(strIn);

	//vdDebug_LogPrintf("vdTTFPrintCenter [%s]", strIn);
	//vdDebug_LogPrintf("CharPerLine[%d=>i=%d]", usCharPerLine,i);
	
	 
	//if (DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
	//	spacestring=(usCharPerLine-i)/2;
	//else

	//start_thandar_adjust allignment
	
	//spacestring=(usCharPerLine-i);//TTF spacestring need 2 spacestring	
	//vdDebug_LogPrintf("spacestring 1 [%d]", spacestring);
	spacestring=(usCharPerLine-i)/2;
	//vdDebug_LogPrintf("spacestring 2 [%d]", spacestring);
	
	//end_thandar_adjust allignment
				
	memset(tucPrint,0x20,sizeof(tucPrint));
	//memcpy(tucPrint+spacestring,strIn,usCharPerLine);
	memcpy(tucPrint+spacestring,strIn,i);
	
	tucPrint[i+spacestring]=0;
    //memset (baTemp, 0x00, sizeof(baTemp));
      
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

	//inCTOSS_CapturePrinterBuffer(tucPrint,&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,1);
}

void vdHeaderPrintCenterTTF(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
    BYTE baTemp[PAPER_X_SIZE * 64];
    
        vdDebug_LogPrintf("vdHeaderPrintCenterTTF [%s]", strIn);

		vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		vdTTFPrintCenter(strIn);
		vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		return;
}

void vdPrintCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
    BYTE baTemp[PAPER_X_SIZE * 64];
    
    vdDebug_LogPrintf("vdPrintCenter [%s]", strIn);

	if (strTCT.inFontFNTMode != 1)
	{
	  	vdTTFPrintCenter(strIn);
		return;
	}

    if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 32;
    else if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 16;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 48;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 24;
    else
        usCharPerLine = 32;
        
    i = strlen(strIn);
	spacestring=(usCharPerLine-i)/2;
				
	memset(tucPrint,0x20,55);
	memcpy(tucPrint+spacestring,strIn,usCharPerLine);	
	
	tucPrint[i+spacestring]=0;
    memset (baTemp, 0x00, sizeof(baTemp));
      
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

	//inCTOSS_CapturePrinterBuffer(tucPrint,&stgFONT_ATTRIB);
    vdDebug_LogPrintf("tucPrint [%s]", tucPrint);
	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,1);
}


void vdPrintTitleCenter(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 16;
    BYTE baTemp[PAPER_X_SIZE * 64];

	if (strTCT.inFontFNTMode == 1)
		vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
    else
		vdSetGolbFontAttrib(d_FONT_12x24, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
    
    i = strlen(strIn);
	if (strTCT.inFontFNTMode == 1)
		spacestring=(usCharPerLine-i)/2;
	else
		spacestring=(usCharPerLine-i)/2;//thandar_font_update _allignment
	    //spacestring=(usCharPerLine-i);
				
	memset(tucPrint,0x20,sizeof(tucPrint));
	
	memcpy(tucPrint+spacestring,strIn,i);	
	
	tucPrint[i+spacestring]=0;
        
    memset (baTemp, 0x00, sizeof(baTemp));	
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//inCTOSS_CapturePrinterBuffer(tucPrint,&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,1);

	if (strTCT.inFontFNTMode == 1)
		vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	else
    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
}

USHORT printDividingLine(int style)
{
    if (style == DIVIDING_LINE_DOUBLE)
    {
		return(inCCTOS_PrinterBufferOutput("===========================================",&stgFONT_ATTRIB,1));
    }
    else
	{
		return(inCCTOS_PrinterBufferOutput("-------------------------------------------",&stgFONT_ATTRIB,1));
    }
}

void vdCTOSS_GetEMVTagsDescription(USHORT usTag, char *szValue)
{
    switch(usTag)
    {
        case TAG_50:
            strcpy(szValue, "Appl Label");
            break;

        case TAG_57:
            strcpy(szValue, "Track2 Data");
            break;

        case TAG_5F2A_TRANS_CURRENCY_CODE:
            strcpy(szValue, "Txn CurrCode");
            break;

        case TAG_5F34_PAN_IDENTFY_NO:
            strcpy(szValue, "PAN Seq Num");
            break;

        case TAG_82_AIP:
            strcpy(szValue, "AIP");
            break;

        case TAG_8A_AUTH_CODE:
            strcpy(szValue, "Auth Code");
            break;

        case TAG_8F:
            strcpy(szValue, "CAPK Index");
            break;

        case TAG_91_ARPC:
            strcpy(szValue, "ARPC");
            break;

        case TAG_95:
            strcpy(szValue, "TVR");
            break;

        case TAG_9A_TRANS_DATE:
            strcpy(szValue, "Txn Date");
            break;

        case TAG_9B:
            strcpy(szValue, "TSI");
            break;

        case TAG_9C_TRANS_TYPE:
            strcpy(szValue, "Txn Type");
            break;

        case TAG_9F02_AUTH_AMOUNT:
            strcpy(szValue, "Txn Amt");
            break;

        case TAG_9F03_OTHER_AMOUNT:
            strcpy(szValue, "Other Amt");
            break;

        case TAG_9F10_IAP:
            strcpy(szValue, "IAP");
            break;

        case TAG_9F12:
            strcpy(szValue, "Appl Pref Name");
            break;

        case TAG_9F1A_TERM_COUNTRY_CODE:
            strcpy(szValue, "Term CurrCode");
            break;

        case TAG_9F1B_TERM_FLOOR_LIMIT:
            strcpy(szValue, "FloorLimit");
            break;

        case TAG_9F26_EMV_AC:
            strcpy(szValue, "AC");
            break;

        case TAG_9F27:
            strcpy(szValue, "CID");
            break;

        case TAG_9F33_TERM_CAB:
            strcpy(szValue, "Term Cab");
            break;

        case TAG_9F34_CVM:
            strcpy(szValue, "CVM");
            break;

        case TAG_9F36_ATC:
            strcpy(szValue, "ATC");
            break;

        case TAG_9F37_UNPREDICT_NUM:
            strcpy(szValue, "Unpredict Num");
            break;

        case TAG_9F5B:
            strcpy(szValue, "Script Result");
            break;

        case TAG_9F63:
            strcpy(szValue, "Card Product ID");
            break;

		case TAG_9F6E:
			strcpy(szValue, "JCB Device information");
            break;

		case TAG_9F7C:
			strcpy(szValue, "JCB Partner Discretion Data");
            break;

        case TAG_71:
            strcpy(szValue, "Issuer Script1");
            break;

        case TAG_72:
            strcpy(szValue, "Issuer Script2");
            break;

        default :
            szValue[0] = 0x00;
            break;
    }
}

void vdPrintEMVTags(void)
{
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    USHORT usPrintTag;
    USHORT usPrintTagLen;
    USHORT usOffset;
    char szStr[512 + 1];
    char szTagDesp[50];
    char szEMVTagList[512];
    char szEMVTagListHex[256];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;
    
    CTOS_PrinterSetHeatLevel(4);
    if( printCheckPaper()==-1)
    	return ;
    //CTOS_LanguagePrinterFontSize(d_FONT_16x16, 0, TRUE);
	if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_9x18, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
	vdCTOSS_PrinterStart(100);
    vdPrintTitleCenter("EMV TAGS DATA");

    memset(szOutEMVData,0x00,sizeof(szOutEMVData));

//    usCTOSS_EMV_MultiDataGet(PRINT_EMV_TAGS_LIST, &inTagLen, szOutEMVData);
//    DebugAddHEX("PRINT_EMV_TAGS_LIST",szOutEMVData,inTagLen);

///////////////////////////////////////////////////////////////
// patrick to print contactless EMV data 20151019
	inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
	vdPCIDebug_HexPrintf("CARD_ENTRY_WAVE", srTransRec.baChipData, srTransRec.usChipDataLen);
////////////////////////////////////////////////////////////////////

    memset(szEMVTagList,0x00,sizeof(szEMVTagList));
    memset(szEMVTagListHex,0x00,sizeof(szEMVTagListHex));
    strcpy(szEMVTagList, PRINT_EMV_TAGS_LIST);
    wub_str_2_hex(szEMVTagList, szEMVTagListHex, strlen(szEMVTagList));

    usOffset = 0;
    while(szEMVTagListHex[usOffset] != 0x00)
    {
        //CTOS_PrinterFline(d_LINE_DOT * 1);
//		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1); //for ERM
        
        memset(szStr, 0x00, sizeof(szStr));
        if ((szEMVTagListHex[usOffset] & 0x1F) == 0x1F)	// If the least 5 bits of the first byte are set, it is a two byte Tag
    	{
    		usPrintTag = szEMVTagListHex[usOffset];
    		usPrintTag = ((usPrintTag << 8) | szEMVTagListHex[usOffset+1]);
    		usPrintTagLen = 2;		
    	}
    	else
    	{
    		usPrintTag = szEMVTagListHex[usOffset];
            usPrintTag = usPrintTag & 0x00FF;
            usPrintTagLen = 1;
    	}
        memcpy(szStr, &szEMVTagList[usOffset*2], usPrintTagLen*2);
        usOffset += usPrintTagLen;

        memset(szTagDesp,0x00,sizeof(szTagDesp));
        vdCTOSS_GetEMVTagsDescription(usPrintTag, szTagDesp);
        vdDebug_LogPrintf("szStr[%s] usPrintTag[%X] szTagDesp[%s]", szStr, usPrintTag, szTagDesp);  
        vdMyEZLib_Printf("%s (%s)", szStr,szTagDesp);
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        EMVtagLen = 0;

//		if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
//        	usCTOSS_FindTagFromDataPackage(usPrintTag, EMVtagVal, &EMVtagLen, srTransRec.baChipData, srTransRec.usChipDataLen);
//		else
			usCTOSS_FindTagFromDataPackage(usPrintTag, EMVtagVal, &EMVtagLen, szOutEMVData, inTagLen);
			
        memset(szStr, ' ', d_LINE_SIZE);
        if(EMVtagLen == 0)
        {
            memcpy(szStr,"No DATA" , 7);
			szStr[7] = 0x00;
        }
        else    
            wub_hex_2_str(EMVtagVal, szStr, EMVtagLen);
        vdMyEZLib_Printf("Len:%d val:%s", EMVtagLen, szStr);
    }
        
    //CTOS_PrinterFline(d_LINE_DOT * 1);
//    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    //vdPrintTitleCenter("*** END OF REPORT ***");
	vdPrintTitleCenter("*END OF REPORT*");
    //CTOS_PrinterFline(d_LINE_DOT * 10);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
  
}


void ReadLine(FILE *fp, char *s_buf)
{
	int i;
	char c_ch;

	if (feof(fp))
	{
		s_buf[0]='\n';
		
		return;
	}

	i = 0;
	while(1)
	{
		if ( feof(fp))
		{
			if (i == 0)
				s_buf[i]='\n';
			
			break;
		}
		
		c_ch = fgetc(fp);
		if (c_ch == '\t') c_ch = ' ';

		if (!feof(fp))
			s_buf[i++] = c_ch;
		
		if (c_ch == '\n' )
		{
			 break;
		}
	}
	
}


void vdCTOSS_GetLIBVersion(void)
{
	FILE *file;
	int i = 0,j;
	int inAPPID;
	char tmpbuf[100];
	int len = 0;
	BYTE szStr[50];
    BYTE baTemp[PAPER_X_SIZE * 64];


	file = fopen ( "/home/ap/pub/version.txt", "r" );
	if ( file != NULL )
	{
		while(!feof(file))
		{
			memset(szStr, 0x00, sizeof(szStr));
			memset(tmpbuf,0x00,sizeof(tmpbuf));
			ReadLine(file, tmpbuf);
			len = strlen(tmpbuf);
			if (len > 2)
				tmpbuf[len-2] = 0;
			vdDebug_LogPrintf("len=[%d],i=[%d],tmpbuf[%s]........",len,i,tmpbuf);
		    sprintf(szStr, "%s", tmpbuf);
		    memset (baTemp, 0x00, sizeof(baTemp));
		    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
		}
	}

	fclose(file);

}


void vdCTOSS_GetAppVersion(char *szVersion)
{
	FILE *file;
	int i,j;
	char CRC[33];
	USHORT inLoop = 0;
	CTOS_stCAPInfo stinfo;
	BYTE byStr[50];
    char szAPName[25];
	int inAPPID;
	int flag = 0;

	#ifdef APP_VERSION
	char szAppVersion[10];
	#endif

	vdDebug_LogPrintf("vdCTOSS_GetAppVersion");
	

	memset(byStr, 0x00, sizeof(byStr));
	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
			memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
			CTOS_APGet(inLoop, &stinfo);
		}
		vdDebug_LogPrintf("szAPName=[%s],baVersion=[%02x%02x],baName=[%s],baExeName=[%s] ", szAPName,stinfo.baVersion[0],stinfo.baVersion[1],stinfo.baName, stinfo.baExeName);
		if (strcmp(szAPName, stinfo.baName)==0)
		{	
			memset(CRC,0x00,sizeof(CRC));
			strcpy(CRC, MOLMD5File(stinfo.baExeName));//Please take note, it is application file name, not application name.
			vdDebug_LogPrintf("MOLMD5File=[%s]",CRC);
			strncpy(byStr,CRC,8);
			for(j = 0; j < 8; j++)
       			byStr[j] = toupper(byStr[j]);
			flag = 1;
			break;
		}
	}

	#ifdef APP_VERSION
	memset(szAppVersion, 0x00, sizeof(szAppVersion));

	get_env("AppVersion",szAppVersion,sizeof(szAppVersion));
	vdDebug_LogPrintf("vdCTOSS_GetAppVersion, szAppVersion [%s]", szAppVersion);
	#endif
	

	if (flag == 1)
	{
		#ifdef APP_VERSION
		//strcpy(szVersion, szAppVersion);
		sprintf(szVersion, "VER: %s", szAppVersion);
		#else
		sprintf(szVersion,"VER:%02x%02x",stinfo.baVersion[0],stinfo.baVersion[1]);
		#endif
		
		strcat(szVersion," ");
		strcat(szVersion,"MD5:");
		strcat(szVersion,byStr);
		strcat(szVersion," ");
	}
	else
	{
		strcpy(szVersion,"VER:0000");
		strcat(szVersion," ");
		strcat(szVersion,"MD5:00000000");
		strcat(szVersion," ");
	}
	vdDebug_LogPrintf("vdCTOSS_GetAppVersion=[%s]",szVersion);	
}


void vdCTOS_GetALLAppID(void)
{
	USHORT inLoop = 0;
	CTOS_stCAPInfo stinfo;
	char szAppname[100+1];
	BYTE processID[100];
	pid_t pid = -1;
	BYTE szStr[50];
    BYTE baTemp[PAPER_X_SIZE * 64];
	
	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
			memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
			CTOS_APGet(inLoop, &stinfo);
		}

		//vdDebug_LogPrintf("baName=[%s],inLoop=[%d] ", stinfo.baName, inLoop);
		if (stinfo.baName[0] != 0x00)
		{
			memset(processID,0x00,sizeof(processID));
			memset(szAppname,0x00,sizeof(szAppname));
			
			strcpy(szAppname, stinfo.baName);
			vdMultiAP_getPID(szAppname,processID);
			pid = atoi(processID);

			vdDebug_LogPrintf("szAppname=[%s],pid=[%d] ",szAppname, pid);
			sprintf(szStr, "[%02d]%s", inLoop+1,szAppname);
		    memset (baTemp, 0x00, sizeof(baTemp));
		    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

			memset(szStr,0x00,sizeof(szStr));
			sprintf(szStr, "    PID:%d, VER:%02x%02x", pid,stinfo.baVersion[0],stinfo.baVersion[1]);
		    memset (baTemp, 0x00, sizeof(baTemp));
		    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
			
		}
	}
}


void vdPrintTerminalConfig(void)
{
    int inResult;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ,i;
    BYTE szStr[50];
    BYTE szBuf[50];
    BYTE baTemp[PAPER_X_SIZE * 64];
	BYTE bLength;
	BYTE szSerialNum[17 + 1];
    int inHostCnt = 0;
    int j = 0;
    int inMinCDT = 0, inMaxCDT = 0;
    char szTemp[MAX_CHAR_PER_LINE+1], szPrint[MAX_CHAR_PER_LINE+1];
    int inDataLen = 0;

    int inAutoSwitchSim=get_env_int("AUTOSWITCHSIM");
	int inManualSwitchSim=get_env_int("MANUALSWITCHSIM");
	
    #ifndef DUAL_SIM_SETTINGS
        inAutoSwitchSim=0; /*disable - not yet complete*/
    #else
        if(inManualSwitchSim == 1)
            inAutoSwitchSim=1;
    #endif

    if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  

	//ushCTOS_PrintHeader(0);
	vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");
//	CTOS_PrinterFline(d_LINE_DOT * 1);
        
//        ushCTOS_PrintHeader(1);
        ushCTOS_TerminalConfigPrintHeader();

	// Get the configuration value of Ethernet //
        
        memset(szSerialNum, 0x00, sizeof(szSerialNum));
        CTOS_GetFactorySN(szSerialNum);
		vdDebug_LogPrintf("RAW szSerialNum[%s]", szSerialNum);
		inDataLen = strlen(szSerialNum);
		
		for (i=0; i<inDataLen; i++)
		{
			if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
			{
				szSerialNum[i] = 0;
				break;
			}
		}
        
        inDataLen = strlen(szSerialNum);
        memset(szTemp, 0x00, sizeof(szTemp));
        memcpy(szTemp, szSerialNum, inDataLen-1);
        
        memset(szSerialNum, 0x00, sizeof(szSerialNum));
        //inFmtPad(szTemp, -inDataLen, '0');
        wub_strpad(szTemp, szSerialNum, '0', 16, 0);
        //strcpy(szSerialNum, szTemp);
        vdDebug_LogPrintf("Fmt szSerialNum[%s]", szSerialNum);
		
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
       // CTOS_PrinterFline(d_LINE_DOT * 1);
        
    memset(szStr, 0x00, sizeof(szStr));
    memset(baTemp, 0x00, sizeof (baTemp));
    sprintf(szStr, "TERMINAL SERIAL NO:");
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    vdPrintCenterTerminalConfig(szStr);

	
    
    memset(szStr, 0x00, sizeof(szStr));
    memset(baTemp, 0x00, sizeof (baTemp));
    strcpy(szStr, szSerialNum);
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    vdPrintCenterTerminalConfig(szStr);

	vdCTOSS_PrinterEnd();

    
    CTOS_PrinterFline(d_LINE_DOT * 1);
        
    memset(szStr, 0x00, sizeof (szStr));
    memset(szBuf, 0x00, sizeof (szBuf));
    vdCTOSS_GetAppVersion(szBuf);
    sprintf(szStr, "%s", szBuf);
    memset(baTemp, 0x00, sizeof (baTemp));
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//	CTOS_GetFactorySN(szBuf);
//	for (i=0; i<strlen(szBuf); i++)
//	{
//		if (szBuf[i] < 0x30 || szBuf[i] > 0x39)
//		{
//			szBuf[i] = 0;
//			break;
//		}
//	}
//    sprintf(szStr, "Factory S/N: %s", szBuf);
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	vdCTOSS_GetLIBVersion();
//
//    CTOS_PrinterFline(d_LINE_DOT * 1);
//	vdCTOS_GetALLAppID();
//
//	// Get the configuration value of Ethernet //
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//	bLength = sizeof(szBuf);
//	CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_IP, szBuf, &bLength);
//    sprintf(szStr, "IP: %s", szBuf);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//	bLength = sizeof(szBuf);
//	CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_MASK, szBuf, &bLength);
//    sprintf(szStr, "MASK: %s", szBuf);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//	bLength = sizeof(szBuf);
//	CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_GATEWAY, szBuf, &bLength);
//    sprintf(szStr, "GATEWAY: %s", szBuf);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//	bLength = sizeof(szBuf);
//	CTOS_EthernetConfigGet(d_ETHERNET_CONFIG_DNSIP, szBuf, &bLength);
//    sprintf(szStr, "DNS: %s", szBuf);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	CTOS_PrinterFline(d_LINE_DOT * 1);
//	inCTOSS_CapturePrinterQRCodeBuffer("\n",0);
//
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//    sprintf(szStr, "APN: %s", strTCP.szAPN);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//    sprintf(szStr, "UserName: %s", strTCP.szUserName);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//    sprintf(szStr, "Password: %s", strTCP.szPassword);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//	CTOS_PrinterFline(d_LINE_DOT * 1);
//	inCTOSS_CapturePrinterQRCodeBuffer("\n",0);
//            
//    memset(szStr, 0x00, sizeof(szStr));
//    memset(szBuf, 0x00, sizeof(szBuf));
//    wub_hex_2_str(strTCT.szInvoiceNo, szBuf, 3);
//    sprintf(szStr, "INVOICE NO: %s", szBuf);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//    memset(szStr, 0x00, sizeof(szStr));
//    sprintf(szStr, "PABX: %s", strTCT.szPabx);                        
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
//
//    memset(szStr, 0x00, sizeof(szStr));
//    if(strTCT.fTipAllowFlag)
//        sprintf(szStr, "TIP ALLOWED: %s", "YES");      
//    else
//        sprintf(szStr, "TIP ALLOWED: %s", "NO");
//    memset (baTemp, 0x00, sizeof(baTemp));
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);
    
    //check host num
//    inNumOfHost = inHDTNumRecord();
        inNumOfHost = inHDTMAX();
    
    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
//            inMMTReadRecord(inNum, 1);
            inCPTRead(inNum);
            
            CTOS_PrinterFline(d_LINE_DOT * 1);
            
//            memset(szStr, 0x00, sizeof(szStr));
//            memset (baTemp, 0x00, sizeof(baTemp));
//            sprintf(szStr, "MERCHANT NAME: %s", strMMT[0].szMerchantName);
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//            
//            memset(szStr, 0x00, sizeof(szStr));
//            memset (baTemp, 0x00, sizeof(baTemp));
//            sprintf(szStr, "MERCHANT ADDRESS:");
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//            
//            memset(szStr, 0x00, sizeof(szStr));
//            memset (baTemp, 0x00, sizeof(baTemp));
//            strcpy(szStr, strMMT[0].szRctHdr1);
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//            
//            memset(szStr, 0x00, sizeof(szStr));
//            memset (baTemp, 0x00, sizeof(baTemp));
//            strcpy(szStr, strMMT[0].szRctHdr2);
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//            
//            memset(szStr, 0x00, sizeof(szStr));
//            memset (baTemp, 0x00, sizeof(baTemp));
//            strcpy(szStr, strMMT[0].szRctHdr3);
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//            
//            memset(szStr, 0x00, sizeof(szStr));
//            memset (baTemp, 0x00, sizeof(baTemp));
//            strcpy(szStr, strMMT[0].szRctHdr4);
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//            
//            memset(szStr, 0x00, sizeof(szStr));
//            memset (baTemp, 0x00, sizeof(baTemp));
//            strcpy(szStr, strMMT[0].szRctHdr5);
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            
//			inCTOSS_CapturePrinterQRCodeBuffer("\n",0);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "HOST: %s", strHDT.szHostLabel);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

            memset(szStr, 0x00, sizeof(szStr));
            memset(szBuf, 0x00, sizeof(szBuf));
            wub_hex_2_str(strHDT.szTPDU, szBuf, 5);
            sprintf(szStr, "TPDU: %s", szBuf);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            memset(szTemp, 0x00, sizeof(szTemp));
            memset (baTemp, 0x00, sizeof(baTemp));
            wub_hex_2_str(strHDT.szNII, szTemp, 2);
            sprintf(szStr, "NII: %s", szTemp);
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
//			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

//            memset(szStr, 0x00, sizeof(szStr));
//            if(strCPT.inCommunicationMode == ETHERNET_MODE)
//            {   
//                sprintf(szStr, "COMM TYPE: %s", "ETHERNET");           
//            }
//            else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
//            {
//                sprintf(szStr, "COMM TYPE: %s", "DIAL_UP");         
//            }       
//            else if(strCPT.inCommunicationMode == GPRS_MODE)
//            {
//                sprintf(szStr, "COMM TYPE: %s", "GPRS");                 
//            }
//            memset (baTemp, 0x00, sizeof(baTemp));
//            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI NUM : %s", strCPT.szPriTxnPhoneNumber);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC NUM : %s", strCPT.szSecTxnPhoneNumber);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//			inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

    #ifdef DUAL_SIM_SETTINGS
			vdDebug_LogPrintf("1. inAutoSwitchSim[%d], strCPT.inCommunicationMode[%d]", inAutoSwitchSim, strCPT.inCommunicationMode);
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE)
			{
                if(strTCP.inSIMSlot == 1)
                {
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "PRI IP	: %s", strCPT.szPriTxnHostIP);						  
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum); 					   
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "SEC IP	: %s", strCPT.szSecTxnHostIP);						  
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum); 					   
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

                }
                else
                {
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "PRI IP  : %s", strCPT.szPriTxnIP_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnPort_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "SEC IP  : %s", strCPT.szSecTxnIP_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnPort_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                }
			}
			else
			{
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "PRI IP	: %s", strCPT.szPriTxnHostIP);						  
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum); 					   
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "SEC IP	: %s", strCPT.szSecTxnHostIP);						  
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum); 					   
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			}
    #else
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI IP	: %s", strCPT.szPriTxnHostIP);						  
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum); 					   
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC IP	: %s", strCPT.szSecTxnHostIP);						  
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum); 					   
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    #endif
			
            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
                if((inResult = inMMTReadRecord(strHDT.inHostIndex,strMMT[inLoop-1].MITid)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else 
                {
                    if(strMMT[0].fMMTEnable)
                    {                        
//                        memset(szStr, 0x00, sizeof(szStr));
//                        sprintf(szStr, "MERCHANT:%s", strMMT[0].szMerchantName);                        
//                        memset (baTemp, 0x00, sizeof(baTemp));
//                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "TID : %s", strMMT[0].szTID);                        
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "MID : %s", strMMT[0].szMID);                        
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

//                        memset(szStr, 0x00, sizeof(szStr));
//                        memset(szBuf, 0x00, sizeof(szBuf));
//                        wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
//                        sprintf(szStr, "BATCH NO: %s", szBuf);                        
//                        memset (baTemp, 0x00, sizeof(baTemp));
//                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

                    }                    

                }
            }
        }
        else
            continue;

    }
    
    
//	inCTOSS_CapturePrinterQRCodeBuffer("\n",1);

//	inCTOSS_PrintEthernetStatus();

//	inCTOSS_CapturePrinterQRCodeBuffer("\n",1);

    CTOS_PrinterFline(d_LINE_DOT * 1);

    memset(szPrint, 0x00, sizeof(szPrint));
    strcpy(szPrint, "--------TABLEPAY DISABLED-------");
    CTOS_PrinterBufferPutString((BYTE *) baTemp, 1, 1, szPrint, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *) baTemp, 3);

//    inHostCnt = inHDTMAX();
    
    for (i = 1; i <= inNumOfHost; i++) {
        inCDTReadbyHost(i, &inMinCDT, &inMaxCDT);
        
        
        vdDebug_LogPrintf("Min = %d, Max = %d", inMinCDT, inMaxCDT);
        j = 0;
        for (j = inMinCDT; j <= inMaxCDT; j++) {
            vdDebug_LogPrintf("j = %d, Min = %d, Max = %d", j, inMinCDT, inMaxCDT);
            if (inCDTReadDisabled(j, i) == d_OK) {
                memset(szTemp, 0x00, sizeof (szTemp));
                memset(szPrint, 0x00, sizeof (szPrint));
                memset(baTemp, 0x00, sizeof (baTemp));
                sprintf(szTemp, "%s-%s", strCDT.szPANLo, strCDT.szPANHi);
                vdDebug_LogPrintf("szTemp [%s]", szTemp);
                sprintf(szPrint, "%-*s%-*s", 42 / 3, strCDT.szCardLabel, 42 / 3, szTemp);
                vdDebug_LogPrintf("szPrint [%s]", szPrint);
                CTOS_PrinterBufferPutString((BYTE *) baTemp, 1, 1, szPrint, &stgFONT_ATTRIB);
                CTOS_PrinterBufferOutput((BYTE *) baTemp, 3);
            }
        }
    }
    
    
    CTOS_PrinterFline(d_LINE_DOT * 5);
    CTOS_PrinterFline(d_LINE_DOT * 5);
    
    return ;
}

void printCardHolderName(void)
{
    USHORT shLen;
    char szStr[d_LINE_SIZE + 1];
    
    shLen = strlen(srTransRec.szCardholderName);
	vdDebug_LogPrintf("szCardholderName=[%d][%s]--", shLen,srTransRec.szCardholderName);

    if(shLen > 0)
    {
    	if (shLen > 44)
			shLen = 44;
        memset(szStr, 0x00, d_LINE_SIZE);
		
        strncpy(szStr, srTransRec.szCardholderName,shLen);

        while(0x20 == szStr[--shLen] && shLen > 0)
            szStr[shLen] = 0x00;

		
		vdDebug_LogPrintf("szCardholderName=[%d][%s]--", shLen,szStr);
		vdPrintCenter(szStr);	
    }
}

USHORT ushCTOS_PrintDemo(void)
{
    BYTE strIn[40];
    unsigned char tucPrint [24*4+1];
    short i,spacestring;
    USHORT usCharPerLine = 16;
    BYTE baTemp[PAPER_X_SIZE * 64];

	if (strTCT.inFontFNTMode == 1)
    	vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
	else
		//use ttf print
	vdSetGolbFontAttrib(d_FONT_12x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);

    memset(strIn, 0x00, sizeof(strIn));
    strcpy(strIn, "DEMO");
    i = strlen(strIn);
	if (strTCT.inFontFNTMode == 1)
    	spacestring=(usCharPerLine-i)/2;
	else
		spacestring=(usCharPerLine-i);
                
    memset(tucPrint,0x20,sizeof(tucPrint));
    
    memcpy(tucPrint+spacestring,strIn,i);   
    
    tucPrint[i+spacestring]=0;

    //CTOS_PrinterFline(d_LINE_DOT * 1);
        
    memset (baTemp, 0x00, sizeof(baTemp));  
    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//    inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,1);
    inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,1);

	//inCTOSS_CapturePrinterBuffer(tucPrint,&stgFONT_ATTRIB);
	//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);

    //CTOS_PrinterFline(d_LINE_DOT * 1);
    //inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);

	if (strTCT.inFontFNTMode == 1)
    	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	else
		//use ttf print
	vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    return d_OK;
    
}

void vdPrintLongMessageCenter(char *szMessage)
{
    int inStrLen = 0, inTotLen = 0, inTempLen = 0;
    char strPrint[42+1];
    char *strTemp;
    char szStr[255+1];
    
    
    //vdDebug_LogPrintf("***vdCTOSS_GetAdvertising START***");
    
    memset(strPrint, 0x00, sizeof(strPrint));
   
    memset(szStr, 0x00, sizeof szStr);
    memcpy(szStr, szMessage, strlen(szMessage));
    
    // strTemp = strtok(srTransRec.szAdvertising, " ");
    strTemp = strtok(szStr, " ");
    while(strTemp != NULL)
    {
        inTempLen = 0;
        inStrLen = strlen(strTemp);
        inTempLen = inTotLen+inStrLen;
        
        if(inTempLen >= 32)
        {
            vdPrintCenter(strPrint);
            memset(strPrint, 0x00, sizeof(strPrint));
            inTotLen = 0;
        }
        
        memcpy(strPrint+inTotLen, strTemp, inStrLen);
        inTotLen += inStrLen;
        memcpy(strPrint+inTotLen, " ", 1);
        inTotLen += 1;
        strTemp = strtok(NULL, " ");
    }
    
    if(strTemp == NULL)
    {
        vdPrintCenter(strPrint);
    }
    
    //vdDebug_LogPrintf("***vdCTOSS_GetAdvertising FINISH***");
}

static void vdPrintLongMessageCenterTerminalConfig(char *szMessage)
{
    int inStrLen = 0, inTotLen = 0, inTempLen = 0;
    char strPrint[42+1];
    char *strTemp;
    char szStr[255+1];
    
    
    //vdDebug_LogPrintf("***vdCTOSS_GetAdvertising START***");
    
    memset(strPrint, 0x00, sizeof(strPrint));
   
    memset(szStr, 0x00, sizeof szStr);
    memcpy(szStr, szMessage, strlen(szMessage));
    
    // strTemp = strtok(srTransRec.szAdvertising, " ");
    strTemp = strtok(szStr, " ");
    while(strTemp != NULL)
    {
        inTempLen = 0;
        inStrLen = strlen(strTemp);
        inTempLen = inTotLen+inStrLen;
        
        if(inTempLen >= 32)
        {
            vdPrintCenterTerminalConfig(strPrint);
            memset(strPrint, 0x00, sizeof(strPrint));
            inTotLen = 0;
        }
        
        memcpy(strPrint+inTotLen, strTemp, inStrLen);
        inTotLen += inStrLen;
        memcpy(strPrint+inTotLen, " ", 1);
        inTotLen += 1;
        strTemp = strtok(NULL, " ");
    }
    
    if(strTemp == NULL)
    {
        vdPrintCenterTerminalConfig(strPrint);
    }
    
    //vdDebug_LogPrintf("***vdCTOSS_GetAdvertising FINISH***");
}

USHORT ushCTOS_PrintHeader(int page)
{	
    int inMerchantAddLen = 0;
    int inDataCnt = 0;
    int i = 0;
    char szStr[32+1];
    
	vdDebug_LogPrintf("start PrinterBMPPic,Tick=[%lu]........",CTOS_TickGet());
	//print Logo	
	//vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");
	if ((isCheckTerminalMP200() != d_OK)&&(isCheckTerminalUPT1000() != d_OK))
	{
		if (strTCT.byERMMode == 4)
		{
			vdCTOSS_PrinterBMPPic(40, 0, "logo.bmp");
		}
		else
		{
	    	vdCTOSS_PrinterBMPPicEx(0, 0, "./fs_data/logo.bmp");
		}
	}

	if(srTransRec.fAlipay == TRUE)
		vdCTOSS_PrinterBMPPicEx(0, 0, "./fs_data/alipay_logo.bmp");
	
	vdDebug_LogPrintf("end PrinterBMPPic,Tick=[%lu]........",CTOS_TickGet());

	if(fRePrintFlag == TRUE)
    {   
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
		if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1);
    }
    
	//merhcant name and address
        if(strlen(strTCT.szMerchantName) > 0)
        	{	if (strTCT.inFontFNTMode != 1)
        	       {

			//		  vdHeaderPrintCenterTTF(strTCT.szMerchantName);
					  vdPrintLongMessageCenter(strTCT.szMerchantName);
        	       }

		        else
				   {
			//		 vdPrintCenter(strTCT.szMerchantName);
				     vdPrintLongMessageCenter(strTCT.szMerchantName);
		        	}
        	
        	}
            //vdPrintCenter(strTCT.szMerchantName);        
               vdPrintLongMessageCenter(strTCT.szMerchantAddress);

			
//        inMerchantAddLen = strlen(strTCT.szMerchantAddress1);
//        inDataCnt = inMerchantAddLen;
//        if(inMerchantAddLen > 32){
//            for(i=0; i < inMerchantAddLen; i++){
//                if(inDataCnt > 32){
//                    memset(szStr, 0x00, sizeof(szStr));
//                    memcpy(szStr, strTCT.szMerchantAddress1+i, 32);
//                    vdPrintCenter(szStr);
//                } else {
//                    memset(szStr, 0x00, sizeof(szStr));
//                    strcpy(szStr, strTCT.szMerchantAddress1+i);
//                    vdPrintCenter(szStr);
//                    break;
//                }
//                
//                inDataCnt--;
//            }
//        } else {
//            memset(szStr, 0x00, sizeof(szStr));
//            strcpy(szStr, strTCT.szMerchantAddress1);
//            vdPrintCenter(szStr);
//        }
        
//        if(strlen(strTCT.szMerchantAddress1) > 0)
//            vdPrintCenter(strTCT.szMerchantAddress1);
//	if(strlen(strTCT.szMerchantAddress2) > 0)
//            vdPrintCenter(strTCT.szMerchantAddress2);
//	if(strlen(strTCT.szMerchantAddress3) > 0)
//            vdPrintCenter(strTCT.szMerchantAddress3);
//	if(strlen(strTCT.szMerchantAddress4) > 0)
//            vdPrintCenter(strTCT.szMerchantAddress4);
//	if(strlen(strTCT.szMerchantAddress5) > 0)
//            vdPrintCenter(strTCT.szMerchantAddress5);
        
//	if(strlen(strMMT[0].szRctHdr1) > 0)
//		vdPrintCenter(strMMT[0].szRctHdr1);
//	if(strlen(strMMT[0].szRctHdr2) > 0)
//		vdPrintCenter(strMMT[0].szRctHdr2);
//	if(strlen(strMMT[0].szRctHdr3) > 0)
//    	vdPrintCenter(strMMT[0].szRctHdr3);
//	if(strlen(strMMT[0].szRctHdr4) > 0)
//    	vdPrintCenter(strMMT[0].szRctHdr4);
//	if(strlen(strMMT[0].szRctHdr5) > 0)
//    	vdPrintCenter(strMMT[0].szRctHdr5);

    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

    if(fRePrintFlag == TRUE)
    {
    	if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
		if (strTCT.inFontFNTMode != 1)
			vdHeaderPrintCenterTTF("DUPLICATE");
		else
			vdPrintCenter("DUPLICATE");
			
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
    
    
	return d_OK;
	
}

USHORT ushCTOS_PrintBodyInstallment(void){
    char szStr[d_LINE_SIZE*2 + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    
    if(srTransRec.fIsInstallment == FALSE)
        return d_OK;
	
//start_thandar_added in for font
		if (strTCT.inFontFNTMode == 1)
        	 vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			 vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

//end_thandar_added in    

    //vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
  
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    inCCTOS_PrinterBufferOutput("--------------------------------",&stgFONT_ATTRIB,1);
    
    memset(szStr, 0x00, sizeof(szStr));
    memset(szTemp,0x00, sizeof(szTemp));
    sprintf(szTemp, "%02d MONTHS", atoi(srTransRec.stIPPinfo.szInstallmentTerms));
    sprintf(szStr, "%-*s%*s", 32/2, "PLAN CODE:", 32/2, szTemp);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szTemp1, 0x00, sizeof(szTemp1));
    memset(szTemp2, 0x00, sizeof(szTemp2));
    memset(szTemp3, 0x00, sizeof(szTemp3));
    memset(szStr, 0x00, sizeof(szStr));
    wub_hex_2_str(srTransRec.szBaseAmount, szTemp, AMT_BCD_SIZE);
    sprintf(szTemp1, "%012.0f", atof(szTemp));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp2);
    
    if(srTransRec.byTransType == SALE){
        sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp2);
    } else if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH) {
        sprintf(szTemp3, "- %s %s", strCST.szCurSymbol, szTemp2);
    }
    
    sprintf(szStr, "%-*s%*s",32/2, "AMOUNT:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szTemp1, 0x00, sizeof(szTemp1));
    memset(szTemp2, 0x00, sizeof(szTemp2));
    memset(szTemp3, 0x00, sizeof(szTemp3));
    memset(szStr, 0x00, sizeof(szStr));
    wub_hex_2_str(srTransRec.stIPPinfo.szMonthlyAmt, szTemp, AMT_BCD_SIZE);
    sprintf(szTemp1, "%012.0f", atof(szTemp));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp2);
    sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp2);
    sprintf(szStr, "%-*s%*s",32/2, "MONTHLY AMT:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

// removed as requested by May (CBB) - 12/18/19
#if 1
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szTemp1, 0x00, sizeof(szTemp1));
    memset(szTemp2, 0x00, sizeof(szTemp2));
    memset(szTemp3, 0x00, sizeof(szTemp3));
    memset(szStr, 0x00, sizeof(szStr));

	#if 1 // original code
    memcpy(szTemp, srTransRec.stIPPinfo.szInterestRate, 12);
    sprintf(szTemp1, "%012.0f", atof(szTemp));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp2);
    sprintf(szTemp3, "%s %s", szTemp2, "%");
    sprintf(szStr, "%-*s%*s", 32/2, "INTEREST RATE:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	#else
    sprintf(szStr, "%-*s%*s", 32/2, "INTEREST RATE:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);	
	#endif
    
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szTemp1, 0x00, sizeof(szTemp1));
    memset(szTemp2, 0x00, sizeof(szTemp2));
    memset(szTemp3, 0x00, sizeof(szTemp3));
    memset(szStr, 0x00, sizeof(szStr));
	#if 1 // original code
    wub_hex_2_str(srTransRec.stIPPinfo.szTotalInterest, szTemp, AMT_BCD_SIZE);
    sprintf(szTemp1, "%012.0f", atof(szTemp));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp2);
    sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp2);
    sprintf(szStr, "%-*s%*s",32/2, "TOTAL INTEREST:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	#else
    sprintf(szStr, "%-*s%*s",32/2, "TOTAL INTEREST:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);	
	#endif
#endif    

    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szTemp1, 0x00, sizeof(szTemp1));
    memset(szTemp2, 0x00, sizeof(szTemp2));
    memset(szTemp3, 0x00, sizeof(szTemp3));
    memset(szStr, 0x00, sizeof(szStr));
    wub_hex_2_str(srTransRec.stIPPinfo.szHandlingFee, szTemp, AMT_BCD_SIZE);
    sprintf(szTemp1, "%012.0f", atof(szTemp));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp2);
    sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp2);
    sprintf(szStr, "%-*s%*s",32/2, "HANDLING FEE:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szTemp1, 0x00, sizeof(szTemp1));
    memset(szTemp2, 0x00, sizeof(szTemp2));
    memset(szTemp3, 0x00, sizeof(szTemp3));
    memset(szStr, 0x00, sizeof(szStr));
//    wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
    wub_hex_2_str(srTransRec.stIPPinfo.szIPPTotalAmount, szTemp, AMT_BCD_SIZE);
    sprintf(szTemp1, "%012.0f", atof(szTemp));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp2);
    sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp2);
    sprintf(szStr, "%-*s%*s",32/2, "TOTAL AMT:", 32/2, szTemp3);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    inCCTOS_PrinterBufferOutput("--------------------------------",&stgFONT_ATTRIB,1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    
    return d_OK;
    
}

/*CBB ask to change the receipt trans title*/
void vdCTOS_ModifyTransTitleString(BYTE *pstr)
{
	vdDebug_LogPrintf("=====vdCTOS_ModifyTransTitleString=====");
	
	if (PRE_AUTH == srTransRec.byTransType)
	{
		if (VISA_CREDIT_HOST_IDX == srTransRec.HDTid ||
			MASTER_CREDIT_HOST_IDX == srTransRec.HDTid)
		{
			//memset(szStr, 0x00, sizeof(szStr));
			//strcpy(pstr, "CARD VERIFY"); // 20180219, CBB ask change back to old .....
		}
	}

	if (PREAUTH_COMP == srTransRec.byTransType)
	{
		if (VISA_CREDIT_HOST_IDX == srTransRec.HDTid ||
			MASTER_CREDIT_HOST_IDX == srTransRec.HDTid)
		{
			//memset(szStr, 0x00, sizeof(szStr));
			//strcpy(pstr, "OFFLINE SALE"); // 20180219, CBB ask change back to old .....
		}
	}

	if (VOID == srTransRec.byTransType && PREAUTH_COMP == srTransRec.byOrgTransType)
	{
		if (VISA_CREDIT_HOST_IDX == srTransRec.HDTid ||
			MASTER_CREDIT_HOST_IDX == srTransRec.HDTid)
		{
			//memset(szStr, 0x00, sizeof(szStr));
			//strcpy(pstr, "OFFLINE VOID"); // 20180219, CBB ask change back to old .....
		}

		if (MPU_CUP_HOST_IDX == srTransRec.HDTid)
		{
			//memset(szStr, 0x00, sizeof(szStr));
			strcpy(pstr, "PRE COMP VOID");
		}
	}
	

	if (VOID == srTransRec.byTransType && PRE_AUTH == srTransRec.byOrgTransType)
	{
		if (VISA_CREDIT_HOST_IDX == srTransRec.HDTid ||
			MASTER_CREDIT_HOST_IDX == srTransRec.HDTid)
		{
			//memset(szStr, 0x00, sizeof(szStr));
			//strcpy(pstr, "CARD VERIFY VOID"); // 20180219, CBB ask change back to old .....
		}

		if (MPU_CUP_HOST_IDX == srTransRec.HDTid)
		{
			//memset(szStr, 0x00, sizeof(szStr));
			strcpy(pstr, "PRE AUTH VOID");
		}
	}

	if (VOID == srTransRec.byTransType && SALE_OFFLINE == srTransRec.byOrgTransType)
	{
		if (VISA_CREDIT_HOST_IDX == srTransRec.HDTid ||
			MASTER_CREDIT_HOST_IDX == srTransRec.HDTid)
		{
			//memset(szStr, 0x00, sizeof(szStr));
			//strcpy(pstr, "OFFLINE VOID"); // 20180219, CBB ask change back to old .....
		}
	}
	
}

USHORT ushCTOS_PrintBody(int page)
{	
    char szStr[d_LINE_SIZE*2 + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
	char szTmpPan[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTempAmt[AMT_ASC_SIZE+1];
    BYTE szPrintingText[40 + 1];

	#ifdef DISCOUNT_FEATURE
		// for Discount feature
		char szStrDisc1[AMT_ASC_SIZE+1];
		char szStrDisc2[AMT_ASC_SIZE+1];
		char szStrDisc3[AMT_ASC_SIZE+1];
		char szStrDisc4[AMT_ASC_SIZE+1];
		char szStrDisc5[AMT_ASC_SIZE+1];
		char szStrDisc6[AMT_ASC_SIZE+1];	
		char szStrDisc7[AMT_ASC_SIZE+1];	
		char szStrDisc8[AMT_ASC_SIZE+1];	
		char szStrDisc9[AMT_ASC_SIZE+1];	
		char szStrDisc10[AMT_ASC_SIZE+1];	
		char szStrDisc11[AMT_ASC_SIZE+1];	
		char szStrDisc12[AMT_ASC_SIZE+1];	
		char szStrDisc13[AMT_ASC_SIZE+1];	
		char szStrDisc14[AMT_ASC_SIZE+1];	
		char szStrDisc15[AMT_ASC_SIZE+1];

		char szFixedAmountDisc[42+1];
		char szOrigAmountDisc[42+1];
		char szPercentageDisc[42+1];	
		int len;
		//
	#endif


    vdDebug_LogPrintf("ushCTOS_PrintBody page:[%d srTransRec.HDTid [%d][%d][%d]]", page, srTransRec.HDTid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex);

	if(d_FIRST_PAGE == page)
	{
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	vdPrintCenter(szTemp1);
		
		printDateTime();

        printTIDMID(); 
		
		printBatchInvoiceNO();

		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    	szGetTransTitle(srTransRec.byTransType, szStr);
        
        if(srTransRec.byOrgTransType == SALE_OFFLINE && srTransRec.byTransType == VOID){
            memset(szStr, 0x00, sizeof(szStr));
            strcpy(szStr, "VOID OFFLINE");
        } else if(srTransRec.byOrgTransType == SALE && srTransRec.byTransType == VOID){
            memset(szStr, 0x00, sizeof(szStr));
			if(srTransRec.HDTid == 6) //http://118.201.48.210:8080/redmine/issues/1525.115
            	strcpy(szStr, "VOID INSTALLMENT");				
			else	
            	strcpy(szStr, "VOID SALE");
        }

		/**/
		vdCTOS_ModifyTransTitleString(szStr);
		
		#if 0
        if(srTransRec.byTransType == VOID_PREAUTH_COMP)
		{
            memset(szStr, 0x00, sizeof(szStr));
        }
		else
        {
            vdPrintTitleCenter(szStr);
        }
        #endif
  		vdPrintTitleCenter(szStr);

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

		sprintf(szStr, "%s", srTransRec.szCardLable);
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(srTransRec.szPAN,szTmpPan,d_LINE_SIZE,page);
		strcpy(szTemp4, szTmpPan);
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szTemp4,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        
		//Exp date and Entry mode
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0, sizeof(szTemp));
		memset(szTemp1, 0, sizeof(szTemp1));
		memset(szTemp4, 0, sizeof(szTemp4));
		memset(szTemp5, 0, sizeof(szTemp5));
		wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
		DebugAddSTR("EXP",szTemp,12);
                memset(szTemp1, 0x00, sizeof(szTemp1));
                
                memcpy(szTemp1, szTemp+2, 2);
                memcpy(szTemp1+2, szTemp, 2);
                
                memset(szTemp, 0x00, sizeof(szTemp));
                strcpy(szTemp, szTemp1);
		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(szTemp,szTmpPan,(EXP_DATE_SIZE + 1),page);

		vdDebug_LogPrintf("ushCTOS_PrintBody [%ld]", srTransRec.byEntryMode);	 

		
		//for (i =0; i<4;i++)
		//	szTemp[i] = '*';
		memcpy(szTemp4,&szTmpPan[0],2);
		memcpy(szTemp5,&szTmpPan[2],2);
                
        memset(szTemp1, 0x00, sizeof(szTemp1));
		if(srTransRec.byEntryMode==CARD_ENTRY_ICC || srTransRec.byEntryMode==CARD_ENTRY_EASY_ICC)
			memcpy(szTemp1,"Chip",4);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
			memcpy(szTemp1,"Manual",6);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"MSR",3);
		else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
			memcpy(szTemp1,"Fallback",8);
		else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
			if(srTransRec.bWaveSID == d_VW_SID_CUP_EMV)
			{
				memcpy(szTemp1,"QuickPass",9);
			}
		}

		vdDebug_LogPrintf("PAT ENTRY MODE [%ld] [%s]", srTransRec.byEntryMode, srTransRec.szPAN);	 
		
		memset (baTemp, 0x00, sizeof(baTemp));
		sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
		
		DebugAddSTR("ENT:",baTemp,12);  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szTemp,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);


//                if(fGetMPUTrans() == FALSE){
                    //Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
		else
			//use ttf print
		sprintf(szStr, "RREF NUM    : %s", srTransRec.szRRN);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
//                }
//		//Reference num
//		memset(szStr, ' ', d_LINE_SIZE);
//		memset (baTemp, 0x00, sizeof(baTemp));					
//		memset(szStr, ' ', d_LINE_SIZE);
//		if (strTCT.inFontFNTMode == 1)
//			sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
//		else
//			//use ttf print
//		sprintf(szStr, "RREF NUM    : %s", srTransRec.szRRN);
//		memset (baTemp, 0x00, sizeof(baTemp));		
//		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
//		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);	
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "APPR CODE : %s", srTransRec.szAuthCode);
		else
		   sprintf(szStr, "APPR CODE   : %s", srTransRec.szAuthCode);
		
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
			/* EMV: Revised EMV details printing - start -- jzg */
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_CUP_EMV)||		
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) || 
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD)) {
			/* EMV: Revised EMV details printing - end -- jzg */
			//AC
			wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "APP CRYPT : %s", szTemp);
			else
				//use ttf print
			sprintf(szStr, "APP CRYPT   : %s", szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			//TVR
			EMVtagLen = 5;
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			else
				//use ttf print
			sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                        DebugAddHEX("*9B*", srTransRec.stEMVinfo.T9B, 2);
                        
            if(strIIT.inIssuerNumber == 6){
                //TSI
                EMVtagLen = 2;
                memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T9B, EMVtagLen);
                DebugAddHEX("*9B*", EMVtagVal, 2);
                memset(szStr, ' ', d_LINE_SIZE);
                if (strTCT.inFontFNTMode == 1)
                        sprintf(szStr, "TSI VALUE : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                else
                        //use ttf print
                sprintf(szStr, "TSI VALUE   : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
            }
			
			//AID
			memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
			vdDebug_LogPrintf("EMVtagLen=[%d]",EMVtagLen);
			
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
			memset(szTemp, ' ', d_LINE_SIZE);
			wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "AID       : %s",szTemp);
			else			
			    sprintf(szStr, "AID         : %s",szTemp);//use ttf print
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			/* EMV: Get Application Label - start -- jzg */
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
			     sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
			else
				sprintf(szStr, "APP LABEL   : %s", srTransRec.stEMVinfo.szChipLabel);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			/* EMV: Get Application Label - end -- jzg */
			
		}

#ifdef MPUPIOnUsFlag	
		vdDebug_LogPrintf("ushCTOS_PrintBody XXXXXX [%s] [%d][%d][%d][%d]",  srTransRec.szHostLabel, srTransRec.HDTid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, srTransRec.byTransType);
		vdDebug_LogPrintf("ushCTOS_PrintBody srTransRec.CDTid =[%d] >> fGetMPUPIOnUs[%d]", srTransRec.CDTid, fGetMPUPIOnUs());		
#else
		vdDebug_LogPrintf("ushCTOS_PrintBody XXXXXX [%s] [%d][%d][%d][%d]",  srTransRec.szHostLabel, srTransRec.HDTid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, srTransRec.byTransType);
		vdDebug_LogPrintf("ushCTOS_PrintBody srTransRec.CDTid =[%d]", srTransRec.CDTid);		
#endif

		#if 0
		//#ifdef CBB_FIN_ROUTING
		if(srTransRec.HDTid == 23)
			inCSTRead(1);
		vdDebug_LogPrintf("ushCTOS_PrintBody strCST.szCurSymbol [%s]", strCST.szCurSymbol);
		
		#endif
		
		//>>>> (4) for MPU-UPI OnUs cards (MPU-UPI-Class, MPU-UPI-Gold, MPU-UPI-Platinum and MPU-UPI).
		//09202022
		#ifdef CBB_FIN_ROUTING		
					//fin-mmk				mpu-bpc 				
			if(srTransRec.HDTid == 23 || srTransRec.HDTid == 17)
			{
				if(srTransRec.CDTid == 58 || srTransRec.CDTid == 62 || srTransRec.CDTid == 63 || srTransRec.CDTid == 64 || srTransRec.CDTid == 71)	// ADDRESSED NOTE #4
					inCSTRead( strHDT.inCurrencyIdx);
				else	
			 		inCSTRead(1);
			}
			else			
				inCSTRead( strHDT.inCurrencyIdx);
			
		#endif

		
		vdDebug_LogPrintf("ushCTOS_PrintBody strCST.inCurrencyIndex =[%d]", strCST.inCurrencyIndex); 	
		
		//thandar_add for Cash Adv printing fee
		if (srTransRec.byTransType == CASH_ADVANCE)
		{
                    if(srTransRec.HDTid == 17)
			{
                        inCSTRead(1);
                    }
                    
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                            sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                            memset(szTemp3, 0x00, sizeof(szTemp3));
                            strcpy(szTemp3, szTempAmt);
                        }


			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                        }
                if (strTCT.inFontFNTMode == 1)        
					sprintf(szStr, "CashAdvFee:%s %s", strCST.szCurSymbol,szTemp5);

				else
					sprintf(szStr, "CashAdvFee  :%s %s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
		}

		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);


		#ifdef DISCOUNT_FEATURE
			// for Discount feature
			// FIxed Rate:
			memset (szStrDisc1, 0x00, sizeof(szStrDisc1));
			memset (szStrDisc2, 0x00, sizeof(szStrDisc2));
			memset (szStrDisc3, 0x00, sizeof(szStrDisc3));
			memset (szStrDisc4, 0x00, sizeof(szStrDisc4));
			memset (szStrDisc5, 0x00, sizeof(szStrDisc5));
			//Original Amount:
			memset (szStrDisc6, 0x00, sizeof(szStrDisc6));
			memset (szStrDisc7, 0x00, sizeof(szStrDisc7));
			memset (szStrDisc8, 0x00, sizeof(szStrDisc8));
			memset (szStrDisc9, 0x00, sizeof(szStrDisc9));
			memset (szStrDisc10, 0x00, sizeof(szStrDisc10));
			// Percentage:
			memset (szStrDisc11, 0x00, sizeof(szStrDisc11));
			memset (szStrDisc12, 0x00, sizeof(szStrDisc12));
			memset (szStrDisc13, 0x00, sizeof(szStrDisc13));
			memset (szStrDisc14, 0x00, sizeof(szStrDisc14));
			memset (szStrDisc15, 0x00, sizeof(szStrDisc15));

			memset(szFixedAmountDisc, 0x00, sizeof(szFixedAmountDisc));
			memset(szOrigAmountDisc, 0x00, sizeof(szOrigAmountDisc));
			memset(szPercentageDisc, 0x00, sizeof(szPercentageDisc));

			// for Discount function
			wub_hex_2_str(srTransRec.szFixedAmount, szStrDisc1, AMT_BCD_SIZE);
			wub_hex_2_str(srTransRec.szOrigAmountDisc, szStrDisc6, AMT_BCD_SIZE);	
			wub_hex_2_str(srTransRec.szPercentage, szStrDisc11, AMT_BCD_SIZE);

			vdDebug_LogPrintf("ushCTOS_PrintBody [%s][%s]", srTransRec.szPercentage, szStrDisc11);			
		#endif
		
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

                if((srTransRec.HDTid == 17) || (strcmp(strCST.szCurSymbol, "MMK") == 0)){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
                    memset(szTemp, 0x00, sizeof(szTemp));
                    strcpy(szTemp, szTempAmt);
                    
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp1)/100);
                    memset(szTemp1, 0x00, sizeof(szTemp1));
                    strcpy(szTemp1, szTempAmt);

					#ifdef DISCOUNT_FEATURE 	
						if(srTransRec.fIsDiscounted == TRUE){
						//Fixed Rate - for Discount function
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1)/100);
						strcpy(szStrDisc3, szStrDisc2);
						//			

						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6)/100);
						strcpy(szStrDisc8, szStrDisc7);
						//	

						//Percentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11)/100);
						strcpy(szStrDisc13, szStrDisc12);
						}
						//						
					#endif					
                }
				else
				{
				#ifdef DISCOUNT_FEATURE 	
					if(srTransRec.fIsDiscounted == TRUE){
					//Fixed Rate - for Discount function
					sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1));
					strcpy(szStrDisc3, szStrDisc2);
					//			

					//Original Amount - for Discount function
					sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6));
					strcpy(szStrDisc8, szStrDisc7);
					//	

					//Percentage  - for Discount function
					sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11));
					strcpy(szStrDisc13, szStrDisc12);
					}
					//						
				#endif
				}
                
		if (strTCT.inFontFNTMode == 1)
			 vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
//       		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
               
		else//use ttf print
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		    //vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		if (srTransRec.byTransType == SALE)
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }

			vdDebug_LogPrintf("VVVVVVVVVVVVVVVVV");			
					
			//Base amount
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			memset(szTemp3, 0x00, sizeof(szTemp3));

			#ifdef DISCOUNT_FEATURE
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){

               	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);

				if(srTransRec.fIsDiscounted == TRUE){
					// for fixed rate
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4); 
					// Original Amount
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9); 	

					// Percent / Amount
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
				}
				
				
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);

				if(srTransRec.fIsDiscounted == TRUE){
					// for fixed rate
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4); 
					// Original Amount
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9); 		
					// Percent / Amount
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14); 
				}
				
            }

			vdDebug_LogPrintf("szStrDisc14 : szTemp5 [%s][%s]", szStrDisc14, szTemp5);

			//if (TRUE ==strTCT.fTipAllowFlag)
			if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
			{
				vdDebug_LogPrintf("AAAAAAAA [%s][%s]", szStrDisc4, szStrDisc9);			
				//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol,szTemp5);		
				sprintf(szTemp3, "  %s %s", strCST.szCurSymbol,szTemp5);		
				 
			}
			else
			{
				vdDebug_LogPrintf("BBBBBBBB [%s][%s]", szStrDisc15, szStrDisc14);
			
            	sprintf(szTemp3, "%s %s", strCST.szCurSymbol,szTemp5);		

				if(srTransRec.fIsDiscounted == TRUE){
					// for fixed rate
	            	sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol,szStrDisc4); 
					// for Original Amount
					sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol,szStrDisc9); 

					// for Percent / amount
					if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
						sprintf(szStrDisc15, "%s", szStrDisc14); 
					else
					{
						len = strlen(szStrDisc15);
						memcpy(szStrDisc15, &szStrDisc14[0], len-2);
					}

				}
								  
			}                  			
			#else			

			vdDebug_LogPrintf("szTemp1 : fTipAllowFlag [%s][%d]", szTemp1, strTCT.fTipAllowFlag);
			
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
            }

			if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)		
				 sprintf(szTemp3, "  %s %s", strCST.szCurSymbol,szTemp5);//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol,szTemp5);
			else
              sprintf(szTemp3, "%s %s", strCST.szCurSymbol,szTemp5);

			#endif                 

			#ifdef DISCOUNT_FEATURE
			vdDebug_LogPrintf("MMMMMMMMMMMMMM");			

			vdDebug_LogPrintf("szStrDisc15:szTemp5 [%s][%s][%s]", szStrDisc15, szTemp5, szTemp3);

			
			//if (TRUE ==strTCT.fTipAllowFlag){					
			if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE){		
				 sprintf(szStr, "AMT:%s", szTemp3);
			}					     
			 else
			 {
				// put discount details here
				//........................
				if(srTransRec.fIsDiscounted == TRUE){		

					strcat(szStrDisc15, " % Disc");
					
					if(srTransRec.fIsDiscountedFixAmt == TRUE)
						sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5); 	//Fixed Amount:					 						
					else													
						sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);		//Percentage Amount:	

						
						//Original Amount:							
						sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10);

						//Total Amount:
						sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);	

				}
			 	else{
					vdDebug_LogPrintf("NNNNNNNNNNNNNNNN");			
					
                	sprintf(szStr, "AMOUNT:  %s", szTemp3);
			 	}
			 }			
			#else
			vdDebug_LogPrintf("ZZZZZZZ::szTemp3:szTemp5:fTipAllowFlag [%s][%s][%d]", szTemp3, szTemp5, strTCT.fTipAllowFlag);

			
					if (TRUE ==strTCT.fTipAllowFlag){					
						 sprintf(szStr, "AMT:%s", szTemp3);	
					}
					 else{
                        sprintf(szStr, "AMOUNT:  %s", szTemp3);
					 }
            #endif
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
						memset (baTemp, 0x00, sizeof(baTemp));
                        
                        if(srTransRec.fIsInstallment == TRUE){
							
							inCSTRead(1); //IPP SHOULD ALWAYS BE MMK //http://118.201.48.210:8080/redmine/issues/1525.115
                            ushCTOS_PrintBodyInstallment();
                        } 
						else
						{
                            //vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							
							#ifdef DISCOUNT_FEATURE
								if(srTransRec.fIsDiscounted == TRUE)
								{								
									vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
									if(srTransRec.fIsDiscountedFixAmt == TRUE)
										inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
									else
										inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
										
									inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1); // for Original amount printout	
									
									inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);									
									inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);		  // For total discounted amount printout
								}
								else
	                            	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

								vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
							
							#else
	                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
							#endif
                        }
                        
			//Tip amount
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr,"%s   %s","TIP",strCST.szCurSymbol);
			memset (baTemp, 0x00, sizeof(baTemp));
			memset (szTemp4, 0x00, sizeof(szTemp4));
			wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTemp4)/100);
                memset(szTemp4, 0x00, sizeof(szTemp4));
                strcpy(szTemp4, szTempAmt);
            }

			DebugAddSTR("TIP:",szTemp4,12);
            //if (TRUE ==strTCT.fTipAllowFlag)
            if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
            {     	
			   	
 				if (strTCT.inFontFNTMode != 1)
			 	  vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	              //vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

				  vdDebug_LogPrintf("KIKO -> szTemp5:[%s] szTemp4:[%s]", szTemp5, szTemp4);

			    if(atol(szTemp4) > 0)
    			{
    				memset(szStr, ' ', d_LINE_SIZE);

					memset(szTemp5,0x00, sizeof(szTemp5));


					vdDebug_LogPrintf("KIKO 111111");
										
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp4, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp4, szTemp5);
                    }
                                            
					//sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TIP:  %s %s", strCST.szCurSymbol,szTemp5);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);					
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);									
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("____________________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
					DebugAddSTR("TOT:",szTemp5,12);
                                        
					//sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TOT:  %s %s", strCST.szCurSymbol,szTemp5);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");				
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("      ____________________",&stgFONT_ATTRIB,1);
    			}
                else
                {

					vdDebug_LogPrintf("KIKO 222222");
                
                    memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp4, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp4, szTemp5);
                    }					
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szTemp5);
					//sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TIP:  %s %s", strCST.szCurSymbol,szTemp5);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);									
					
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("      ____________________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
					
					//sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TOT:  %s %s", strCST.szCurSymbol,szTemp5);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("      ____________________",&stgFONT_ATTRIB,1);
                }
            }
			
			
		}

		else if (srTransRec.byTransType == SALE_TIP)
		{	 			    
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }

			vdDebug_LogPrintf("KIKO 222222.111");
                    
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                memset(szTemp3, 0x00, sizeof(szTemp3));
                strcpy(szTemp3, szTempAmt);
            }

			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
            } else{
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
            }
                        
			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
            }
                        
			sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
					
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
            }
                        
			sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");	
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			
		}

	 else if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH || srTransRec.byTransType == MPU_VOID_PREAUTH_COMP || srTransRec.byTransType == MPU_VOID_PREAUTH)
	 {
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }

					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
					#ifdef DISCOUNT_FEATURE
					//Fixed Rate - for Discount function
					if(srTransRec.fIsDiscounted == TRUE){
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1)/100);
						strcpy(szStrDisc3, szStrDisc2);

						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6)/100);
						strcpy(szStrDisc8, szStrDisc7);
					
						//Perentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11)/100);
						strcpy(szStrDisc13, szStrDisc12);
						
					}					
					#endif
					}
					else
					{
						#ifdef DISCOUNT_FEATURE
						//Fixed Rate - for Discount function
						if(srTransRec.fIsDiscounted == TRUE){
							sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1));
							strcpy(szStrDisc3, szStrDisc2);
						
							//Original Amount - for Discount function
							sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6));
							strcpy(szStrDisc8, szStrDisc7);
						
							//Perentage / Amount - for Discount function
							sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11));
							strcpy(szStrDisc13, szStrDisc12);
							
						}					
						#endif					
					}
                    
//                    CTOS_PrinterSetHeatLevel(4);
				    //old code to show amount only
                    memset(szStr, ' ', d_LINE_SIZE);

				    memset(szTemp5,0x00, sizeof(szTemp5));

				  #ifdef DISCOUNT_FEATURE
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
						
						//Fixed Amount:
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4);
						//Original Amount:
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9);	
						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
						
						
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
						//Fixed Amount:
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4);
						//Original Amount:
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9);
						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14);
						
						
                    }				  
				  #else
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }
                  #endif

				  // removed additional currency symbol #1

				  vdDebug_LogPrintf("V O I D ! ! ! (%d)(%s)", srTransRec.fIsDiscounted, szTemp5);	
				  #if 0
				  if (FALSE == strTCT.fTipAllowFlag)									  
				  {				  
	                    memset(szTemp3,0x00, sizeof(szTemp3));                    
	                    strcpy(szTemp3, "-");
	                    strcat(szTemp3, strCST.szCurSymbol);
	                    strcat(szTemp3, " ");
						//ex:  -MMK 

						vdDebug_LogPrintf("V O I D ! ! ! XXXXX(%s)", szTemp3); 				
						
	                    strcat(szTemp3, szTemp5);

						vdDebug_LogPrintf("V O I D ! ! ! YYYYY(%s)", szTemp3); 				

	                    memset(szTemp5, 0x00, sizeof(szTemp5));
	                    strcpy(szTemp5, szTemp3); // amount +  currency symbol
						vdDebug_LogPrintf("V O I D ! ! ! ZZZZZ(%s)", szTemp5); 				
	                    
					}

					#else
					memset(szTemp3,0x00, sizeof(szTemp3));
                    
                    strcpy(szTemp3, "-");
                    strcat(szTemp3, strCST.szCurSymbol);
                    strcat(szTemp3, " ");
                    strcat(szTemp3, szTemp5);
					#endif

					#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE)
					{
						//Fixed Amount:
						sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol, szStrDisc4);
						//Original Amount:
						sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol, szStrDisc9);	
						// Percent / Amount: 
						//sprintf(szStrDisc15, "%s", szStrDisc14); 

						if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
							sprintf(szStrDisc15, "%s", szStrDisc14); 
						else
						{
							len = strlen(szStrDisc15);
							memcpy(szStrDisc15, &szStrDisc14[0], len-2);
						}																
						
						
					}
					#endif

					vdDebug_LogPrintf("MMMMMMM (%s)", szTemp5);
					

					//if (TRUE ==strTCT.fTipAllowFlag){					
					if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE){		
						 //sprintf(szStr, "AMT:%s", szTemp5);
                        //sprintf(szStr, "AMT:-%s %12s", strCST.szCurSymbol, szTemp5);
                        //sprintf(szStr, "AMT:-%s    %12s", strCST.szCurSymbol, szTemp5);
                        sprintf(szStr, "AMT:  -%s %s", strCST.szCurSymbol, szTemp5);
					}
					else 	
					{
						// put discount details here
						//........................
						#ifdef DISCOUNT_FEATURE
						if(srTransRec.fIsDiscounted == TRUE)
						{		
							strcat(szStrDisc15, " % Disc");
							
							if(srTransRec.fIsDiscountedFixAmt == TRUE){
								//Fixed Amount:							
								sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5);
							}else{
								//Percentage Amount:							
								sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);							
							}

							
							//Original Amount:
							sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10);	
							
							//Total Amount:
							//sprintf(szStrDisc1, "Total    AMT: %s", szTemp5);	
							sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);	
						}
						else{
								//sprintf(szStr, "AMOUNT:   %s", szTemp5); //removed additional currency symbol #2
								sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);
						}
						#else

								//sprintf(szStr, "AMOUNT:   %s", szTemp5); //removed additional currency symbol #2
								sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);
						#endif
					}

					vdDebug_LogPrintf("NNNNNN (%s)", szStr);
                    
                    memset(szPrintingText, 0x00, sizeof(szPrintingText));
                    sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                    put_env_int("#P3LEN", strlen(szPrintingText));
                    put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                    
                    memset (baTemp, 0x00, sizeof(baTemp));
                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
            if(srTransRec.fIsInstallment == TRUE)
            {
            
				inCSTRead(1); //IPP SHOULD ALWAYS BE MMK //http://118.201.48.210:8080/redmine/issues/1525.115
                ushCTOS_PrintBodyInstallment();
            }
			else
			{
                //                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                //vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
                inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

				#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE){		
						
						vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
						if(srTransRec.fIsDiscountedFixAmt == TRUE)
							inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						else
							inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);  // Original Amount	
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);								
						inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);		  // Discounted Amount
					}
					else
						inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	                //vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                #else					
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				#endif

                //if(TRUE ==strTCT.fTipAllowFlag 
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
                {
                    //TIP start here!
                    memset(szTemp3, ' ', d_LINE_SIZE);
                    wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        memset(szTempAmt, 0x00, sizeof(szTempAmt));
                        sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                        memset(szTemp3, 0x00, sizeof(szTemp3));
                        strcpy(szTemp3, szTempAmt);
                    }
                    
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                    {
                         vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                    } 
					else 
					{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                    }
                    
                    //sprintf(szStr, "TIP: %16s", szTemp5);
                    if(atol(szTemp5) > 0)
                    	sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP:-%s    %12s", strCST.szCurSymbol,szTemp5);
					else
						sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP: %s    %12s", strCST.szCurSymbol,szTemp5);
					
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                    //inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
                    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                    
                    //TOTAL start here!					
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else {
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
                    
                    //sprintf(szStr, "TOT: %16s", szTemp5);
                    
                    //sprintf(szStr, "TOT:-%s    %12s", strCST.szCurSymbol,szTemp5);
                    sprintf(szStr, "TOT:  -%s %s", strCST.szCurSymbol,szTemp5);
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                    }
				vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
			}
                    
//                    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
			//closed old amount code!

		} 
	 	else
		{

			vdDebug_LogPrintf("ELSE!!!");			
		
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }

			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        
			#ifdef DISCOUNT_FEATURE 				
			//format amount 10+2
			if(strcmp(strCST.szCurSymbol, "MMK") == 0){
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);			
				// for fixed rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4); 
				// Original Amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9); 	

			// Percent / Amount
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
			
				
				
			} else {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
			
				// for fixed rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4); 
				// Original Amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9);	

			// Percent / Amount
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14);	
				
			}
			#else
				if(strcmp(strCST.szCurSymbol, "MMK") == 0){
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
				} else {
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
				}
			#endif


            memset(szTemp3, 0x00,sizeof(szTemp3));
			//if (TRUE ==strTCT.fTipAllowFlag){
			if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE){
				// sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);
				// fixed for NO amount printed on the receipt.
				//sprintf(szTemp3, "%s %12s", strCST.szCurSymbol, szTemp5);
				sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);
			}
			else
               sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);

			
            if(srTransRec.byTransType == BALANCE_ENQUIRY)
            {
				if(memcmp(srTransRec.szCurrCode1, "104", 3) == 0)
				{
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount1, szTemp3);
					strcpy(strCST.szCurSymbol, "MMK");
				}
				else
				{
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount1, szTemp3);
					strcpy(strCST.szCurSymbol, "USD");
				}

				if(srTransRec.szSign1[0] == 'D') /*D - negative sign*/
				    sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
                else
					sprintf(szStr, "BAL: %s %s", strCST.szCurSymbol, szTemp3);
				
				//vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

                if(strlen(srTransRec.szCurrCode2))
                {
					memset(szTemp3, 0, sizeof(szTemp3));
					memset(szStr, ' ', d_LINE_SIZE);

                    if(memcmp(srTransRec.szCurrCode2, "104", 3) == 0)
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount2, szTemp3);
						strcpy(strCST.szCurSymbol, "MMK");
                    }
                    else
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount2, szTemp3);
						strcpy(strCST.szCurSymbol, "USD");
                    }
                    
					if(srTransRec.szSign2[0] == 'D') /*D - negative sign*/
						sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
					else
						sprintf(szStr, "BAL: %s %s", strCST.szCurSymbol, szTemp3);


                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                }
				vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
            }
            else
			{
			   // put discount details here
			   //........................
			   #ifdef DISCOUNT_FEATURE
			   if(srTransRec.fIsDiscounted == TRUE){	   		   
		   
				   //Fixed Amount:
				   if(srTransRec.fIsDiscountedFixAmt == TRUE)
					   sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5);	   //Fixed Amount:										   
				   else 												   
					   sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);	   //Percentage Amount:    
					   
				   //Original Amount:
				   sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10);			   
				   //Total Amount:
				   sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);  		   
			   }
			   else					   	
               		sprintf(szStr, "AMOUNT: %s", szTemp3);
			   #else
			   		sprintf(szStr, "AMOUNT: %s", szTemp3);
			   #endif
		   	}

				
            memset(szPrintingText, 0x00, sizeof(szPrintingText));
            sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
            put_env_int("#P3LEN", strlen(szPrintingText));
            put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
                        if(srTransRec.fIsInstallment == TRUE)
                        {
                        	
							inCSTRead(1); //IPP SHOULD ALWAYS BE MMK //http://118.201.48.210:8080/redmine/issues/1525.115
                            ushCTOS_PrintBodyInstallment();
                        } 
						else if(srTransRec.byTransType == BALANCE_ENQUIRY)
						{
							
						}
						else
						{
                            //vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

							#ifdef DISCOUNT_FEATURE			
								if(srTransRec.fIsDiscounted == TRUE){
									
									vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
									if(srTransRec.fIsDiscountedFixAmt == TRUE)
										inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
									else
										inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for percentage disc amt  printout
									inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);	

									
									inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
									inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);
								}
								else
	                            	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);	
								vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
							#else
	                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							
	                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);

							#endif
                        }
		}
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
		
	}
//	else if(d_SECOND_PAGE == page)
        else
	{

		vdDebug_LogPrintf("ushCTOS_PrintBody page:[%d srTransRec.HDTid [%d][%d][%d]]", page, srTransRec.HDTid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex);
	
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	vdPrintCenter(szTemp1);
		
		printDateTime();
		inResult = printTIDMID(); 
		
		printBatchInvoiceNO(); // pat confirm hang

		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		memset(szStr, ' ', d_LINE_SIZE);
    	szGetTransTitle(srTransRec.byTransType, szStr);

#if 1
		if(srTransRec.byOrgTransType == SALE_OFFLINE && srTransRec.byTransType == VOID){
			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr, "VOID OFFLINE");
		} else if(srTransRec.byOrgTransType == SALE && srTransRec.byTransType == VOID){
			memset(szStr, 0x00, sizeof(szStr));
			if(srTransRec.HDTid == 6) //http://118.201.48.210:8080/redmine/issues/1525.115
            	strcpy(szStr, "VOID INSTALLMENT");				
			else	
            	strcpy(szStr, "VOID SALE");
		}
#endif		
		

		/**/
		vdCTOS_ModifyTransTitleString(szStr);
		
  		vdPrintTitleCenter(szStr);    

		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
		
		sprintf(szStr, "%s",srTransRec.szCardLable);
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(srTransRec.szPAN,szTmpPan,d_LINE_SIZE,page);
		strcpy(szTemp4, szTmpPan);
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        
		//Exp date and Entry mode
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0, sizeof(szTemp));
		memset(szTemp1, 0, sizeof(szTemp1));
		memset(szTemp4, 0, sizeof(szTemp4));
		memset(szTemp5, 0, sizeof(szTemp5));
		wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
		DebugAddSTR("EXP",szTemp,12);
        memset(szTemp1, 0x00, sizeof(szTemp1));
        memcpy(szTemp1, szTemp+2, 2);
        memcpy(szTemp1+2, szTemp, 2);
        
        memset(szTemp, 0x00, sizeof(szTemp));
        strcpy(szTemp, szTemp1);
		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(szTemp,szTmpPan,(EXP_DATE_SIZE + 1),page);
		
		//for (i =0; i<4;i++)
		//	szTemp[i] = '*';
		memcpy(szTemp4,&szTmpPan[0],2);
		memcpy(szTemp5,&szTmpPan[2],2);

		if(srTransRec.byEntryMode==CARD_ENTRY_ICC || srTransRec.byEntryMode==CARD_ENTRY_EASY_ICC)
			memcpy(szTemp1,"Chip",4);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
			memcpy(szTemp1,"Manual",6);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"MSR",3);
		else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
			memcpy(szTemp1,"Fallback",8);
		else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
            if(srTransRec.bWaveSID == d_VW_SID_CUP_EMV)
            {
                memcpy(szTemp1,"QuickPass",9);
            }
		}
		
		DebugAddSTR("ENT:",szTemp1,12);  
		memset (baTemp, 0x00, sizeof(baTemp));

		memset (baTemp, 0x00, sizeof(baTemp));
		sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
		
		DebugAddSTR("ENT:",baTemp,12);  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);


		//Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
		   sprintf(szStr, "APPR CODE : %s", srTransRec.szAuthCode);
		else
		   sprintf(szStr, "APPR CODE   : %s", srTransRec.szAuthCode);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		if((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
			/* EMV: Revised EMV details printing - start -- jzg */
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD))
			/* EMV: Revised EMV details printing - end -- jzg */
			{
			//AC
			wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "APP CRYPT : %s", szTemp);
			else
				//use ttf print
			sprintf(szStr, "APP CRYPT   : %s", szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			//TVR
			EMVtagLen = 5;
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			else
				//use ttf print
			sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                        
            if(strIIT.inIssuerNumber == 6){
                //TSI
                EMVtagLen = 2;
                memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                memcpy(EMVtagVal, srTransRec.stEMVinfo.T9B, EMVtagLen);
                memset(szStr, ' ', d_LINE_SIZE);
                if (strTCT.inFontFNTMode == 1)
                        sprintf(szStr, "TSI VALUE : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                else
                        //use ttf print
                sprintf(szStr, "TSI VALUE   : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
            }
			
			//AID
			memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
			vdDebug_LogPrintf("EMVtagLen=[%d]",EMVtagLen);
			
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
			memset(szTemp, ' ', d_LINE_SIZE);
			wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "AID       : %s",szTemp);
			else		
			   sprintf(szStr, "AID         : %s",szTemp);//use ttf print
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			/* EMV: Get Application Label - start -- jzg */
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
			  sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
			else
				sprintf(szStr, "APP LABEL   : %s", srTransRec.stEMVinfo.szChipLabel);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			/* EMV: Get Application Label - end -- jzg */
			
		}


			//thandar_add for Cash Adv printing fee
		if (srTransRec.byTransType == CASH_ADVANCE)
		{
            if(srTransRec.HDTid == 17)
			{
                inCSTRead(1);
            }
                    
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
	        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
	            memset(szTempAmt, 0x00, sizeof(szTempAmt));
	            sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
	            memset(szTemp3, 0x00, sizeof(szTemp3));
	            strcpy(szTemp3, szTempAmt);
	        }


			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
            }
            if (strTCT.inFontFNTMode == 1)             
			    sprintf(szStr, "CashAdvFee:%s %s", strCST.szCurSymbol,szTemp5);
			
            else
			    sprintf(szStr, "CashAdvFee  :%s %s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
		}

		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);

// for Discount feature
#ifdef DISCOUNT_FEATURE
		// FIxed Rate:
		memset (szStrDisc1, 0x00, sizeof(szStrDisc1));
		memset (szStrDisc2, 0x00, sizeof(szStrDisc2));
		memset (szStrDisc3, 0x00, sizeof(szStrDisc3));
		memset (szStrDisc4, 0x00, sizeof(szStrDisc4));
		memset (szStrDisc5, 0x00, sizeof(szStrDisc5));
		//Original Amount:
		memset (szStrDisc6, 0x00, sizeof(szStrDisc6));
		memset (szStrDisc7, 0x00, sizeof(szStrDisc7));
		memset (szStrDisc8, 0x00, sizeof(szStrDisc8));
		memset (szStrDisc9, 0x00, sizeof(szStrDisc9));
		memset (szStrDisc10, 0x00, sizeof(szStrDisc10));
		// Percentage:
		memset (szStrDisc11, 0x00, sizeof(szStrDisc11));
		memset (szStrDisc12, 0x00, sizeof(szStrDisc12));
		memset (szStrDisc13, 0x00, sizeof(szStrDisc13));
		memset (szStrDisc14, 0x00, sizeof(szStrDisc14));
		memset (szStrDisc15, 0x00, sizeof(szStrDisc15));	
		

		memset(szFixedAmountDisc, 0x00, sizeof(szFixedAmountDisc));
		memset(szOrigAmountDisc, 0x00, sizeof(szOrigAmountDisc));
		memset(szPercentageDisc, 0x00, sizeof(szPercentageDisc));		
		//

		// for Discount function
		wub_hex_2_str(srTransRec.szFixedAmount, szStrDisc1, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szOrigAmountDisc, szStrDisc6, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szPercentage, szStrDisc11, AMT_BCD_SIZE);
		
#endif

		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
                
                if((srTransRec.HDTid == 17) || (strcmp(strCST.szCurSymbol, "MMK") == 0)){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
                    memset(szTemp, 0x00, sizeof(szTemp));
                    strcpy(szTemp, szTempAmt);
                    
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp1)/100);
                    memset(szTemp1, 0x00, sizeof(szTemp1));
                    strcpy(szTemp1, szTempAmt);

					#ifdef DISCOUNT_FEATURE 				
					if(srTransRec.fIsDiscounted == TRUE){
						//Fixed Rate - for Discount function
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1)/100);
						strcpy(szStrDisc3, szStrDisc2);
						//			
						
						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6)/100);
						strcpy(szStrDisc8, szStrDisc7);
						//						
					
						//Percentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11)/100);
						strcpy(szStrDisc13, szStrDisc12);
					}				
					#endif					
                }
				else
				{
					#ifdef DISCOUNT_FEATURE 				
					if(srTransRec.fIsDiscounted == TRUE){
						//Fixed Rate - for Discount function
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1));
						strcpy(szStrDisc3, szStrDisc2);
						//			
						
						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6));
						strcpy(szStrDisc8, szStrDisc7);
						//						
					
						//Percentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11));
						strcpy(szStrDisc13, szStrDisc12);
					}				
					#endif
				}

		if (strTCT.inFontFNTMode == 1)
			  vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
      //        	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                  
		else	//use ttf print				
		 	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);		    
			//vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

			
        
		if (srTransRec.byTransType == SALE)
		{
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }
			//Base amount
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
            memset(szTemp3, 0x00, sizeof(szTemp3));

			#ifdef DISCOUNT_FEATURE
				//format amount 10+2
				if(strcmp(strCST.szCurSymbol, "MMK") == 0){
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
				//Fixed Rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4);							
				//Original amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9);
				// Percent / Amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
				

				} else {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
				//Fixed Rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4);
				//Original amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9);
				// Percent / Amount
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14); 					
				}

				//if (TRUE ==strTCT.fTipAllowFlag)
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
				{
					 //sprintf(szTemp3, "%s %16s", strCST.szCurSymbol,szTemp5);
					 sprintf(szTemp3, "  %s %s", strCST.szCurSymbol,szTemp5);

					 
					 if(srTransRec.fIsDiscounted == TRUE){
					 // for fixed rate
					 sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol,szStrDisc4); 
					// for Original amount
					sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol,szStrDisc9); 	
					// for Percent / amount
					//sprintf(szStrDisc15, "%s", szStrDisc14); 
					}

					if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
						sprintf(szStrDisc15, "%s", szStrDisc14); 
					else
					{
						len = strlen(szStrDisc15);
						memcpy(szStrDisc15, &szStrDisc14[0], len-2);
					}							
				}
				else
				{
					sprintf(szTemp3, "%s %s", strCST.szCurSymbol,szTemp5);							  
					// for fixed rate
					sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol,szStrDisc4); 
					// for Original Amount
					sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol,szStrDisc9);
					// for Percent / amount
					//sprintf(szStrDisc15, "%s", szStrDisc14); 
					if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
						sprintf(szStrDisc15, "%s", szStrDisc14); 
					else
					{
						len = strlen(szStrDisc15);
						memcpy(szStrDisc15, &szStrDisc14[0], len-2);
					}
					
					
				}			

				//if (TRUE ==strTCT.fTipAllowFlag)
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
					 sprintf(szStr, "AMT:%s", szTemp3);				
				else{
					// put discount details here
					//........................
					if(srTransRec.fIsDiscounted == TRUE){				

						strcat(szStrDisc15, " % Disc");
						//Fixed Amount: 						
						if(srTransRec.fIsDiscountedFixAmt == TRUE)
							sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5); 	//Fixed Amount: 										
						else													
							sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15); 	//Percentage Amount:	
						
						//Original Amount:
						sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10); 				
						//Total Amount:
						sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);				
					}
					else										
						sprintf(szStr, "AMOUNT:  %s", szTemp3);
				}						 
				
			#else
			//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }

					if (TRUE ==strTCT.fTipAllowFlag)
					sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);
					else
                       sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp5);
                        
					if (TRUE ==strTCT.fTipAllowFlag)
						 sprintf(szStr, "AMT:%s", szTemp3);
					else
                        sprintf(szStr, "AMOUNT:  %s", szTemp3);
             #endif
                        
            memset(szPrintingText, 0x00, sizeof(szPrintingText));
            sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
            put_env_int("#P3LEN", strlen(szPrintingText));
            put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        
            if(srTransRec.fIsInstallment == TRUE)
            {
            	
                inCSTRead(1); //IPP SHOULD ALWAYS BE MMK //http://118.201.48.210:8080/redmine/issues/1525.115
                ushCTOS_PrintBodyInstallment();
            } 
            else 
            {
                //vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
                inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

				#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE){
						
						vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
						if(srTransRec.fIsDiscountedFixAmt == TRUE)
							inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						else
							inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);		

						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);								
						inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);
					}
					else
	                	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);				
				#else
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);				
				
				#endif
				
                //if (TRUE ==strTCT.fTipAllowFlag)                
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
                {

					vdDebug_LogPrintf("KIKO 333333");
				
                    //TIP start here!
                    memset(szTemp3, ' ', d_LINE_SIZE);
                    wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                    {
                        memset(szTempAmt, 0x00, sizeof(szTempAmt));
                        sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                        memset(szTemp3, 0x00, sizeof(szTemp3));
                        strcpy(szTemp3, szTempAmt);
                    }
                
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                    } 
                    else 
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                    }
                
                    //sprintf(szStr, "TIP: %16s", szTemp5);
                    //sprintf(szStr, "TIP:%s     %12s", strCST.szCurSymbol,szTemp5);
                    sprintf(szStr, "TIP:  %s %s", strCST.szCurSymbol,szTemp5);
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                    //inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
                    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                    
                    //TOTAL start here!					
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
                    
                    //sprintf(szStr, "TOT: %16s", szTemp5);
                    
                    //sprintf(szStr, "TOT:%s     %12s", strCST.szCurSymbol,szTemp5);
                    sprintf(szStr, "TOT:  %s %s", strCST.szCurSymbol,szTemp5);
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                }
                vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
            }
		}
		else if (srTransRec.byTransType == SALE_TIP)
		{

			vdDebug_LogPrintf("KIKO 444444");
		
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                memset(szTemp3, 0x00, sizeof(szTemp3));
                strcpy(szTemp3, szTempAmt);
            }
                        
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
            }
                        
			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
            }
                        
			sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
            }
                        
			sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			
		} else if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH){ //@@IBR ADD 03102016
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }

                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));


					#ifdef DISCOUNT_FEATURE
					//format amount 10+2
		            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
		                
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
		                
						// for fixed rate
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4); 
						// Original Amount
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9); 	

						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
						
						
		            } else {
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
						// for fixed rate
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4); 
						// Original Amount
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9); 	
						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14);	
						
		            }						
					#else
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }
                    #endif
					
                    memset(szTemp3,0x00, sizeof(szTemp3));
                    
                    strcpy(szTemp3, "-");
                    strcat(szTemp3, strCST.szCurSymbol);
                    strcat(szTemp3, " ");
                    strcat(szTemp3, szTemp5);

					vdDebug_LogPrintf("VOID SECTION!!! (%s)", szTemp3);

					#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE)
					{
						//Fixed Amount:
						sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol, szStrDisc4);
						//Original Amount:
						sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol, szStrDisc9);		
						// Percent / Amount:
						//sprintf(szStrDisc15, "%s", szStrDisc14); 
						if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
							sprintf(szStrDisc15, "%s", szStrDisc14); 
						else
						{
							len = strlen(szStrDisc15);
							memcpy(szStrDisc15, &szStrDisc14[0], len-2);
						}			
						
					}
					#endif

					#ifdef DISCOUNT_FEATURE
					//if (TRUE ==strTCT.fTipAllowFlag)
					if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
					    sprintf(szStr, "AMT:  -%s %s", strCST.szCurSymbol, szTemp5);//sprintf(szStr, "AMT:-%s    %12s", strCST.szCurSymbol, szTemp5);
					else{
						// put discount details here
						//........................
						if(srTransRec.fIsDiscounted == TRUE){		
							strcat(szStrDisc15, " % Disc");

							//Fixed Amount:							
							if(srTransRec.fIsDiscountedFixAmt == TRUE){
								//Fixed Amount:							
								sprintf(szFixedAmountDisc, "Fixed Disc AMT: %s", szStrDisc5);
							}else{
								//Percentage Amount:							
								sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);							
							}

							
							//Original Amount:
							sprintf(szOrigAmountDisc, "Sale     AMT: %s", szStrDisc10);
							//Total Amount:
							sprintf(szStrDisc1, "Total    AMT: %s", szTemp3);
						}
						else						
							sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);
						}
					
					#else
					if (TRUE ==strTCT.fTipAllowFlag){
					//sprintf(szStr, "AMT:%s", szTemp5);
					    sprintf(szStr, "AMT:  -%s %s", strCST.szCurSymbol, szTemp5);//sprintf(szStr, "AMT:-%s    %12s", strCST.szCurSymbol, szTemp5);
					}
					else	
					    sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);

					#endif
					
                    memset(szPrintingText, 0x00, sizeof(szPrintingText));
                    sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                    put_env_int("#P3LEN", strlen(szPrintingText));
                    put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                    
                    memset (baTemp, 0x00, sizeof(baTemp));
                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
					if(srTransRec.fIsInstallment == TRUE)
					{
					
						inCSTRead(1); //IPP SHOULD ALWAYS BE MMK //http://118.201.48.210:8080/redmine/issues/1525.115
						ushCTOS_PrintBodyInstallment();
					}
					else
					{
						//							  vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
						//vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

						#ifdef DISCOUNT_FEATURE
						if(srTransRec.fIsDiscounted == TRUE){
							
							vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
							if(srTransRec.fIsDiscountedFixAmt == TRUE)
								inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
							else
								inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
							inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);	
						
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);								
							inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);
						}
						else
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
						
						#else
						inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
						//vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
						#endif
						
						//if (TRUE ==strTCT.fTipAllowFlag)						
						if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
						{
							//TIP start here!
							memset(szTemp3, ' ', d_LINE_SIZE);
							wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
							if(strcmp(strCST.szCurSymbol, "MMK") == 0){
								memset(szTempAmt, 0x00, sizeof(szTempAmt));
								sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
								memset(szTemp3, 0x00, sizeof(szTemp3));
								strcpy(szTemp3, szTempAmt);
							}
							
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp5,0x00, sizeof(szTemp5));
							//format amount 10+2
							if(strcmp(strCST.szCurSymbol, "MMK") == 0)
							{
								 vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
							} 
							else 
							{
								vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
							}
							
							//sprintf(szStr, "TIP: %16s", szTemp5);
							//sprintf(szStr, "TIP:-%s   %12s", strCST.szCurSymbol,szTemp5);
							if(atol(szTemp5) > 0)
								sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP:-%s    %12s", strCST.szCurSymbol,szTemp5);
							else
								sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP: %s	   %12s", strCST.szCurSymbol,szTemp5);
							
							memset (baTemp, 0x00, sizeof(baTemp));
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							//inCCTOS_PrinterBufferOutput(" 				____________________",&stgFONT_ATTRIB,1);
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							
							//TOTAL start here! 				
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp5,0x00, sizeof(szTemp5));
							//format amount 10+2
							if(strcmp(strCST.szCurSymbol, "MMK") == 0){
							vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
							} else {
							vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
							}
							
							//sprintf(szStr, "TOT: %16s", szTemp5);
							
							//sprintf(szStr, "TOT:-%s    %12s", strCST.szCurSymbol,szTemp5);
							sprintf(szStr, "TOT:  -%s %s", strCST.szCurSymbol,szTemp5);
							memset (baTemp, 0x00, sizeof(baTemp));
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							}
						vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
					}

                }
		else
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                        }
                        
//			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
                        memset(szTemp3, 0x00,sizeof(szTemp3));
						if (TRUE ==strTCT.fTipAllowFlag)
						   sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);
						else							
                           sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);
//                        sprintf(szStr, "AMOUNT: %16s", szTemp3);

						if(srTransRec.byTransType == BALANCE_ENQUIRY)
						{
							if(memcmp(srTransRec.szCurrCode1, "104", 3) == 0)
							{
								vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount1, szTemp3);
								strcpy(strCST.szCurSymbol, "MMK");
							}
							else
							{
								vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount1, szTemp3);
								strcpy(strCST.szCurSymbol, "USD");
							}
						
							if(srTransRec.szSign1[0] == 'D') /*D - negative sign*/
								sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
							else
								sprintf(szStr, "BAL: %s %s",  strCST.szCurSymbol, szTemp3);
							
							//vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
						
							if(strlen(srTransRec.szCurrCode2))
							{
								memset(szTemp3, 0, sizeof(szTemp3));
								memset(szStr, ' ', d_LINE_SIZE);
						
								if(memcmp(srTransRec.szCurrCode2, "104", 3) == 0)
								{
									vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount2, szTemp3);
									strcpy(strCST.szCurSymbol, "MMK");
								}
								else
								{
									vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount2, szTemp3);
									strcpy(strCST.szCurSymbol, "USD");
								}
								
								if(srTransRec.szSign2[0] == 'D') /*D - negative sign*/
									sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
								else
									sprintf(szStr, "BAL: %s %s",  strCST.szCurSymbol, szTemp3);

								inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							}
							vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
						}
                        else
                        {
                            if (TRUE ==strTCT.fTipAllowFlag)
                                sprintf(szStr, "AMOUNT: %s", szTemp3);//sprintf(szStr, "AMT:%s", szTemp3);
                            else						
                                sprintf(szStr, "AMOUNT: %s", szTemp3);
                        }
						
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        
                        if(srTransRec.fIsInstallment == TRUE){
							vdDebug_LogPrintf("1_ushCTOS_PrintBodyInstallment");
							
							inCSTRead(1); //IPP SHOULD ALWAYS BE MMK //http://118.201.48.210:8080/redmine/issues/1525.115
                            ushCTOS_PrintBodyInstallment();
                        } 
						else if(srTransRec.byTransType == BALANCE_ENQUIRY)
						{
							
						}
						else
						{
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                            //vdCTOS_SetFontType(d_FONT_COURBD_TTF);// fix issue case #1906 - Amount is missing in mPOS receipt
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
		}

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
	}
	return d_OK;	
	
}

USHORT ushCTOS_PrintAgreement() {
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        vdSetGolbFontAttrib(d_FONT_9x18, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    if (srTransRec.fIsInstallment == TRUE) {
		if (strTCT.inFontFNTMode == 1)
       		 vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		else
			 vdSetGolbFontAttrib(d_FONT_9x18, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

		
		   
        vdPrintCenter("I Hereby irrevocably and unconditionally");
        vdPrintCenter("authorise the Bank to charge and debit");
        vdPrintCenter("my card account each monthly installment");
        vdPrintCenter("(as stated above). I understand that such");
        vdPrintCenter("use of my credit card shall be governed");
        vdPrintCenter("by the CB Cardmember's Agreement");
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
    } else {
        vdPrintCenter("I AGREE TO PAY THE ABOVE TOTAL AMOUNT");
        vdPrintCenter("ACCORDING TO THE CARD ISSUER AGREEMENT");
    }

    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
}

USHORT ushCTOS_PrintFooter(int page)
{		
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    BYTE szPinBlock[64 + 1];
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szExchangeRate[d_LINE_SIZE + 1];

	
	int inResult=0; 
	BYTE byVEPSQPSPrnted = 0; 
	BYTE byVEPSQPSPrnted2 = 0; // VISA Master  NO SIGNATURE REQUIRED , already printted?
	BYTE byPassSignLine = 0;
	int inEpadResult;

	char szAppVersion[10];
	

#ifdef ENHANCEMENT_1861
	int inJCBSignLine = get_env_int("JCBSIGNLINE");
#endif
        
        if(strTCT.inExchangeRate > 0){
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
//            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
            memset(szStr, 0x00, sizeof(szStr));
            memset(szExchangeRate, 0x00, sizeof(szExchangeRate));
            sprintf(szExchangeRate, "%d", strTCT.inExchangeRate);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szExchangeRate, szStr);
            
            memset(szExchangeRate, 0x00, sizeof(szExchangeRate));
            sprintf(szExchangeRate, "Ex_Rate USD 1 = MMK %s", szStr);
            
//            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
            
            inCCTOS_PrinterBufferOutput(szExchangeRate,&stgFONT_ATTRIB,1);
            vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
            
            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
        }

//start_thandar_added in for Font update
    if (strTCT.inFontFNTMode != 1)
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}

//end_thandar_added in for Font update

	#if 1
	memset(szAppVersion,0x00,sizeof(szAppVersion));
	get_env("AppVersion",szAppVersion,sizeof(szAppVersion));
	vdDebug_LogPrintf("ushCTOS_PrintFooter, szAppVersion [%s]", szAppVersion);	
	#endif
	
	if(page == d_FIRST_PAGE)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
	
		vdDebug_LogPrintf("[A]ushCTOS_PrintFooter byEntryMode[%d]HDTid[%d]byTransType[%d]byOrgTransType[%d]strHDT.inCurrencyIdx[%d]strCST.inCurrencyIndex[%d]",
															srTransRec.byEntryMode, 
															srTransRec.HDTid, 
															srTransRec.byTransType,
															srTransRec.byOrgTransType,
															strHDT.inCurrencyIdx,  
															strCST.inCurrencyIndex); 
		
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{

			vdDebug_LogPrintf("[B]ushCTOS_PrintFooter");  

		    	inResult=inCTOSS_CheckFlagVEPSQPS();

			vdDebug_LogPrintf("[B.1]ushCTOS_PrintFooter inResult [%d]", inResult);
			
		    	EMVtagLen = 3;
            		memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

			vdDebug_LogPrintf("ushCTOS_PrintFooter T9F34 EMVtagVal = [%02x][%02x][%02x][%d]......", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], inResult);

			if(inResult != 0 && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND))
           		 {   
		            		vdDebug_LogPrintf("[C]ushCTOS_PrintFooter - NO SIGNATURE REQUIRED"); 
		            	    	vdDebug_LogPrintf("ushCTOS_PrintFooter fVEPS QPS flag on = 1");               

					#if 0 ////In v1030, with fQPS =1, CL CVM limit = 3001 MMK (for testing purpose), Testing with amount 3150 MMK with MC ctls, 9F34 is 1E0300 but sign line is not present in receipt.
					if(inResult == 4 && EMVtagVal[0] == 0x1E)
					{						
						vdDebug_LogPrintf("[C1]ushCTOS_PrintFooter"); 
						byVEPSQPSPrnted2 = 1;
					}
					else
					{
						vdDebug_LogPrintf("[C2]ushCTOS_PrintFooter"); 
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
					}
					#else
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
					#endif

					//To not include signline on receipt
					byVEPSQPSPrnted = 1;
					
            		}			
			else if ((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E))// || (EMVtagVal[0] == 0x3F)) -- NO CVM
            		{   
				vdDebug_LogPrintf("[D]ushCTOS_PrintFooter"); 
            
    				memset(szPinBlock,0x00,sizeof(szPinBlock));
				wub_hex_2_str(srTransRec.szPINBlock,szPinBlock,8);		
				vdDebug_LogPrintf("[D-0]szPinBlock=[%s]......",szPinBlock);
				#ifdef QUICKPASS
				if (inCTOSS_CheckCVMPrint() == d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)) /*Host 7 - UPI Finexus*/
				{
					vdDebug_LogPrintf("inCTOSS_CheckCVMPrint()== d_OK");
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					
				}
            			else 
				#endif	
				if (strlen(szPinBlock) > 0 && strcmp(szPinBlock,"0000000000000000") == 0)// for pinbypass
            			{
					vdDebug_LogPrintf("[D-1]ushCTOS_PrintFooter"); 
            				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

					// fix for TAIS_QPS_M_001 as per UPI test case
					if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)&& (srTransRec.IITid == 6 || srTransRec.IITid == 9))
					{
						vdDebug_LogPrintf("[D-2]ushCTOS_PrintFooter - NO SIGNATURE REQUIRED"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						

						byPassSignLine = 1;						
					}

					//fix for (2)Some Visa(onus,offus) cards dont ask for digital signature. added to case #1605 04-03-2020
					// to fix http://118.201.48.214:8080/issues/15.7
					#if 0
					if(srTransRec.HDTid == 21){
						vdDebug_LogPrintf("[D-2.2]ushCTOS_PrintFooter"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
						byPassSignLine = 1; 					
						
					}
					#endif

					#if 1
					//http://118.201.48.214:8080/issues/15 For Master, #1) Testing with amount less than CVM limit, sign line is present in e-rec					
					vdDebug_LogPrintf("ushCTOS_PrintFooter EMVtagVal byte 1 = [%02X]",EMVtagVal[0]);
					
					if(srTransRec.HDTid == 21)
					{					
						vdDebug_LogPrintf("[D-2.1]ushCTOS_PrintFooter - NO SIGNATURE REQUIRED"); 
						
						if(EMVtagVal[0] == 0x1F)
						{
							
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
							
							byPassSignLine = 1;
							
						}
					}
					#endif
						
					
            	}
				else
				{
	                //CTOS_PrinterFline(d_LINE_DOT * 1);
	                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
					//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
					//inCTOSS_CapturePrinterBuffer("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB);
					// patrick refund no support online pin
					vdDebug_LogPrintf("[D-3]ushCTOS_PrintFooter"); 
					
					if (((EMVtagVal[0] == 0x02) || (EMVtagVal[0] == 0x42)) && (srTransRec.byTransType == REFUND ))
					{
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					}
					else if (((EMVtagVal[0] == 0x02) || (EMVtagVal[0] == 0x42)) && ((srTransRec.byTransType == VOID ) && (srTransRec.byOrgTransType == REFUND )))
					{
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					}
					else if ((ginPinByPass == 1)|| (EMVtagVal[0] == 0x1F))
					{
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					}
					else
					{
						vdDebug_LogPrintf("[E]ushCTOS_PrintFooter"); 

						//http://118.201.48.214:8080/issues/15 #2) Testing with amount less than CVM limit, "No Signature Required" text is appeared twice in e-receipt 
						if(srTransRec.IITid != 8 && srTransRec.IITid != 4)
						{
						
							vdDebug_LogPrintf("[E.A]ushCTOS_PrintFooter NO SIGNATURE REQUIRED");  
						
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);					
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						

							byPassSignLine = 1;
							
							if (srTransRec.byEntryMode==CARD_ENTRY_ICC && EMVtagVal[0] == 0x01)
							{
								inCCTOS_PrinterBufferOutput("	  (PIN VERIFY SUCCESS)",&stgFONT_ATTRIB,1);
							}
						}
					}
				}
            }
            else
            {

			
				vdDebug_LogPrintf("[E-1]ushCTOS_PrintFooter"); 
                		//CTOS_PrinterFline(d_LINE_DOT * 3);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				// fix for TAIS_QPS_U_007 as per UPI test case
				#ifdef ENHANCEMENT_1861																							//1. With fix for issue http://118.201.48.214:8080/issues/75.72 #3) Pure UPI sign line issue
					if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) && 
						(EMVtagVal[0] == 0x5E || EMVtagVal[0] == 0x1e))
					{
						vdDebug_LogPrintf("[E-2]ushCTOS_PrintFooter - NO SIGNATURE REQUIRED"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
						byPassSignLine = 1; 					
					}
					else
					{				
						vdDebug_LogPrintf("[E-3]ushCTOS_PrintFooter - WITH SIGNATURE"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						byVEPSQPSPrnted = 0;
					}
				#else
					if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) && EMVtagVal[0] == 0x5E){
						vdDebug_LogPrintf("[E-3-0]ushCTOS_PrintFooter  - NO SIGNATURE REQUIRED"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
						byPassSignLine = 1; 					
					}
					else
					{				
						vdDebug_LogPrintf("[E-3]ushCTOS_PrintFooter"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					}				
				#endif
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			vdDebug_LogPrintf("[E-3-1]ushCTOS_PrintFooter"); 
			// on UPI fallback case
			#ifdef ENHANCEMENT_1861																																			//http://118.201.48.214:8080/issues/75.72 -3) Pure UPI sign line issue The current flow is sign line is always present for any amount for any UPISGINLINE value. 
			if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) && 
				(srTransRec.IITid == 6 || srTransRec.IITid == 9))
			{	
			
				vdDebug_LogPrintf("[E-3-1.1]ushCTOS_PrintFooter NO SIGNATURE REQUIRED"); 

				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
				
				byPassSignLine = 1;
			}
			else
			{
				vdDebug_LogPrintf("[E-3-2]ushCTOS_PrintFooter"); 
			
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
			}
			#else
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);			
			#endif
        }
		vdDebug_LogPrintf("[E-4]ushCTOS_PrintFooter:byPassSignLine[%d]byVEPSQPSPrnted[%d]", byPassSignLine, byVEPSQPSPrnted); 
		

		// #1 To remove asking signature for all JCB Tranx ( pure JCB and MPU_JCB ) at MPU New Switch - case #1750
		#ifdef JCB_NO_SIGNLINE
			if((srTransRec.IITid == 8) || (srTransRec.IITid == 4))
			{

				vdDebug_LogPrintf("[E-3-5]ushCTOS_PrintFooter inJCBSignLine:[%d] strHDT.inCurrencyIdx[%d] strCST.inCurrencyIndex [%d]", 
					inJCBSignLine, strHDT.inCurrencyIdx, strCST.inCurrencyIndex); 

				//#4 Add Parameter setting for JCB and UPI Singature line for any amount and Bank set up Amount.
				#ifdef ENHANCEMENT_1861
					if(inJCBSignLine == 1)
					{
						if(fAmountLessThanFloorLimit() == d_OK)
						{
							vdDebug_LogPrintf("ushCTOS_PrintFooter:inJCBSignLine NO SIGNATURE REQUIRED"); 

							//http://118.201.48.214:8080/issues/75.76 #5) To remove extra spaces between "AMOUNT" and "No Signature Required" text for MPU-JCB and JCB
							//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1); for testing
							inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
							
							byPassSignLine = 1;					
						}
						else
						{										
							vdDebug_LogPrintf("ushCTOS_PrintFooter:inJCBSignLine SIGN LINE 2"); 
							byPassSignLine = 0;  //http://118.201.48.214:8080/issues/15 #1) For JCB Testing with amount greater than CVM limit, terminal prompts customer signature but "No Signature Required" with signature in e-receipt 
						
							//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							///inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);	

							
							// if not pin bypass for MPOS virtual signature.
							//fix for missing signature on e-receipt
							//if(byPassSignLine != 1)
							//{
								inEpadResult = ushCTOS_ePadPrintSignature();
								
								//vdDebug_LogPrintf("[E-3-6] 1st page > ushCTOS_PrintFooter inEpadResult NO SIGNATURE REQUIRED"); 
							//}
						}	
					}
					else
					{
						vdDebug_LogPrintf("[E-3-7] 1st page > ushCTOS_PrintFooter inEpadResult [%d]", inEpadResult); 
					
						//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
						
						byPassSignLine = 1; 
					
					}
				#else
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
					byPassSignLine = 1; 				
				#endif
			}
			else
			{
				vdDebug_LogPrintf("[E-3-6]ushCTOS_PrintFooter"); 
				// if not pin bypass for MPOS virtual signature.
				if(byPassSignLine != 1)
					inEpadResult = ushCTOS_ePadPrintSignature();		

				
				vdDebug_LogPrintf("[E-3-6]ushCTOS_PrintFooter inEpadResult [%d]", inEpadResult); 
			}

		#else
				ushCTOS_ePadPrintSignature();		
		#endif


		
		//http://118.201.48.210:8080/redmine/issues/1525.34.2
        //inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
		if(byPassSignLine != 1)
		{
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);		
        	//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		}


		vdDebug_LogPrintf("[F]ushCTOS_PrintFooter [%d][%d][%d][%d]",byVEPSQPSPrnted, byPassSignLine, srTransRec.byTransType, srTransRec.byOrgTransType);  

		if (1 == byVEPSQPSPrnted)
		{
			vdDebug_LogPrintf("[G]ushCTOS_PrintFooter"); 
		
			// print nothing																//In v1030, with fQPS =1, CL CVM limit = 3001 MMK (for testing purpose), Testing with amount 3150 MMK with MC ctls, 9F34 is 1E0300 but sign line is not present in receipt.
			//if ((srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE) || byVEPSQPSPrnted2 == 1)
			if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE)
				inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);
		}
		else
		{
			vdDebug_LogPrintf("[H]ushCTOS_PrintFooter"); 		
		
			if(byPassSignLine != 1)
			{
				inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);				
				printCardHolderName();
			}
		
		}				
		
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
		ushCTOS_PrintAgreement();
		
		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot3);
			

		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** MERCHANT COPY *****  ");
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		if (strTCT.inFontFNTMode == 1)
			inCCTOS_PrinterBufferOutput("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
//                    inCCTOS_PrinterBufferOutput("        ***** COPY *****  ",&stgFONT_ATTRIB,1);
		else
		    inCCTOS_PrinterBufferOutput("      ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
//                    inCCTOS_PrinterBufferOutput("        ***** COPY *****  ",&stgFONT_ATTRIB,1);
//                inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                
                
		
		
	}
	else
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		vdDebug_LogPrintf("ushCTOS_PrintFooter 2ND PAGE!!!!!"); 
		vdDebug_LogPrintf("[11]ushCTOS_PrintFooter [%d][%d][%d][%d]",srTransRec.byEntryMode, 
															srTransRec.HDTid, 
															srTransRec.byTransType,
															srTransRec.byOrgTransType);
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
			inResult=inCTOSS_CheckFlagVEPSQPS();
			
			EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);


			if(inResult != 0 && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND))
            {   
            	    vdDebug_LogPrintf("fVEPS QPS flag on = 1");               
					
					#if 0 ////In v1030, with fQPS =1, CL CVM limit = 3001 MMK (for testing purpose), Testing with amount 3150 MMK with MC ctls, 9F34 is 1E0300 but sign line is not present in receipt.
					if(inResult == 4 && EMVtagVal[0] == 0x1E)
						byVEPSQPSPrnted2 = 1;
					else
					{
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
					}
					#else
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
					#endif
					
				

					byVEPSQPSPrnted = 1;
            }				
			else if ((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E)) //|| (EMVtagVal[0] == 0x3F)) -- NO CVM
            {        
            	memset(szPinBlock,0x00,sizeof(szPinBlock));
				wub_hex_2_str(srTransRec.szPINBlock,szPinBlock,8);		
				vdDebug_LogPrintf("szPinBlock=[%s]......",szPinBlock);
				#ifdef QUICKPASS
                if(inCTOSS_CheckCVMPrint() == d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)) /*Host 7 - UPI Finexus*/
                {
                    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
                    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
                }
				#endif
				if (strlen(szPinBlock) > 0 && strcmp(szPinBlock,"0000000000000000") == 0)// for pinbypass
            	{
            	
            		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

					// fix for TAIS_QPS_M_001 as per UPI test case
					if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)&& (srTransRec.IITid == 6 || srTransRec.IITid == 9)){
						vdDebug_LogPrintf("[22]ushCTOS_PrintFooter"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						

						byPassSignLine = 1;						
					}					

					#if 1
					//http://118.201.48.214:8080/issues/15 For Master, #1) Testing with amount less than CVM limit, sign line is present in e-rec					
					vdDebug_LogPrintf("ushCTOS_PrintFooter EMVtagVal byte 1 = [%02X]",EMVtagVal[0]);
					if(srTransRec.HDTid == 21)
					{					
						vdDebug_LogPrintf("[D-2.1]ushCTOS_PrintFooter"); 
						if(EMVtagVal[0] == 0x1F)
						{
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
							byPassSignLine = 1;
						}
					}
					#endif
					
            	}
				else
				{
					vdDebug_LogPrintf("[33]ushCTOS_PrintFooter"); 
				
                //CTOS_PrinterFline(d_LINE_DOT * 1);
                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
                // patrick refund no support online pin
				if (((EMVtagVal[0] == 0x02) || (EMVtagVal[0] == 0x42)) && (srTransRec.byTransType == REFUND ))
				{
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				}
				else if (((EMVtagVal[0] == 0x02) || (EMVtagVal[0] == 0x42)) && ((srTransRec.byTransType == VOID ) && (srTransRec.byOrgTransType == REFUND )))
				{
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				}
				else if ((ginPinByPass == 1)|| (EMVtagVal[0] == 0x1F))
				{
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				}
				else
				{
					vdDebug_LogPrintf("[44]ushCTOS_PrintFooter"); 

					
					//http://118.201.48.214:8080/issues/15 #2) Testing with amount less than CVM limit, "No Signature Required" text is appeared twice in e-receipt 
					if(srTransRec.IITid != 8 && srTransRec.IITid != 4)
					{
					
					vdDebug_LogPrintf("[44.1]ushCTOS_PrintFooter NO SIGNATURE REQUIRED"); 
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);				
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
						
						byPassSignLine = 1;
						
						if (srTransRec.byEntryMode==CARD_ENTRY_ICC && EMVtagVal[0] == 0x01)
						{
							vdDebug_LogPrintf("[55]ushCTOS_PrintFooter"); 
						
			                //CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						}
					}
				}
				}
            }
            else
            {
				#ifdef ENHANCEMENT_1861
					if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) && (EMVtagVal[0] == 0x5E || EMVtagVal[0] == 0x1e))
					{
						vdDebug_LogPrintf("66]ushCTOS_PrintFooter"); 
						
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
						byPassSignLine = 1; 					
					}
					else
					{				
						vdDebug_LogPrintf("[77]ushCTOS_PrintFooter"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						byVEPSQPSPrnted = 0;
					}
				#else
					if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) && EMVtagVal[0] == 0x5E){
						vdDebug_LogPrintf("[E-2]ushCTOS_PrintFooter"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
						byPassSignLine = 1; 					
					}
					else
					{				
						vdDebug_LogPrintf("[E-3]ushCTOS_PrintFooter"); 
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					}				
				#endif

            }
		}
        else
        {
			vdDebug_LogPrintf("[88]ushCTOS_PrintFooter"); 
			#ifdef ENHANCEMENT_1861
			// on UPI fallback case
			if(inCTOSS_CheckCVMPrint() != d_OK && (srTransRec.HDTid == 7 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) && (srTransRec.IITid == 6 || srTransRec.IITid == 9))
			{	
				vdDebug_LogPrintf("[99]ushCTOS_PrintFooter"); 
			
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
				
				byPassSignLine = 1;
			}
			else
			{		
				vdDebug_LogPrintf("[1010]ushCTOS_PrintFooter"); 
			
	            //CTOS_PrinterFline(d_LINE_DOT * 3);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
			}
			#else
				//CTOS_PrinterFline(d_LINE_DOT * 3);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);			
			#endif
        }

		// #1 To remove asking signature for all JCB Tranx ( pure JCB and MPU_JCB ) at MPU New Switch - case #1750
		#ifdef JCB_NO_SIGNLINE
			if((srTransRec.IITid == 8) || (srTransRec.IITid == 4))
			{
				vdDebug_LogPrintf("[E-2]ushCTOS_PrintFooter:byPassSignLine[%d]", byPassSignLine); 

				//#4 Add Parameter setting for JCB and UPI Singature line for any amount and Bank set up Amount.
				#ifdef ENHANCEMENT_1861
				if(inJCBSignLine == 1)
				{
					if(fAmountLessThanFloorLimit() == d_OK)
					{
						vdDebug_LogPrintf("ushCTOS_PrintFooter:inJCBSignLine NO SIGNATURE REQUIRED"); 

					//http://118.201.48.214:8080/issues/75.76 #5) To remove extra spaces between "AMOUNT" and "No Signature Required" text for MPU-JCB and JCB
						//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1); - for testing
						inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
						
						byPassSignLine = 1; 						
					}
					else
					{				
						vdDebug_LogPrintf("ushCTOS_PrintFooter:inJCBSignLine SIGN LINE 4"); 
						byPassSignLine = 0;//http://118.201.48.214:8080/issues/15 #1) For JCB Testing with amount greater than CVM limit, terminal prompts customer signature but "No Signature Required" with signature in e-receipt  					

						inEpadResult = ushCTOS_ePadPrintSignature();
						
						vdDebug_LogPrintf("[E-3-6] 2nd page > ushCTOS_PrintFooter inEpadResult [%d]", inEpadResult); 
						//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
						//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);	
					}	
				}
				else
				{
					vdDebug_LogPrintf("[E-3-7] 2nd page > ushCTOS_PrintFooter inEpadResult NO SIGNATURE REQUIRED"); 
				
					//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
					byPassSignLine = 1; 
				
				}
				#else
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);						
					
					byPassSignLine = 1; 
				
				#endif
			}
			else
			{
				// if not pin bypass for MPOS virtual signature.
				if(byPassSignLine != 1)			
					ushCTOS_ePadPrintSignature();
			}
		#else
				ushCTOS_ePadPrintSignature();		
		#endif

		//http://118.201.48.210:8080/redmine/issues/1525.34.2
        //inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
		if(byPassSignLine != 1)
		{
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);		
        	//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		}

		vdDebug_LogPrintf("[C10]ushCTOS_PrintFooter [%d][%d][%d]",byVEPSQPSPrnted, srTransRec.byTransType, srTransRec.byOrgTransType);  

        if (1 == byVEPSQPSPrnted)
		{
			// print nothing																////In v1030, with fQPS =1, CL CVM limit = 3001 MMK (for testing purpose), Testing with amount 3150 MMK with MC ctls, 9F34 is 1E0300 but sign line is not present in receipt.
			//if ((srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE) || byVEPSQPSPrnted2 == 1)
			if (srTransRec.byTransType == VOID && srTransRec.byOrgTransType == SALE)
				inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);
		}

		else
		{				
			if(byPassSignLine != 1){
				inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);
				printCardHolderName();
			}

		}

		
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
		ushCTOS_PrintAgreement();

        if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	vdPrintCenter(strMMT[0].szRctFoot3);
		
//		//CTOS_PrinterFline(d_LINE_DOT * 1); 
//		//CTOS_PrinterPutString("   ***** CUSTOMER COPY *****  ");
        //		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        //		if (strTCT.inFontFNTMode == 1)
        ////			inCCTOS_PrinterBufferOutput("   ***** CUSTOMER COPY *****  ",&stgFONT_ATTRIB,1);
        //                    inCCTOS_PrinterBufferOutput("   ***** COPY *****  ",&stgFONT_ATTRIB,1);
        //		else
        //		inCCTOS_PrinterBufferOutput("      ***** COPY *****  ",&stgFONT_ATTRIB,1);
        vdDebug_LogPrintf("this is print page = %d", page);

        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);

        if (page == 1) {
            if (strTCT.inFontFNTMode == 1)
                inCCTOS_PrinterBufferOutput("   ***** MERCHANT COPY *****  ", &stgFONT_ATTRIB, 1);
            else
                inCCTOS_PrinterBufferOutput("   ***** MERCHANT COPY *****  ", &stgFONT_ATTRIB, 1);
        } else if (page == 2) {
            if (strTCT.inFontFNTMode == 1)
                inCCTOS_PrinterBufferOutput("   ***** CUSTOMER COPY *****  ", &stgFONT_ATTRIB, 1);
            else
                inCCTOS_PrinterBufferOutput("   ***** CUSTOMER COPY *****  ", &stgFONT_ATTRIB, 1);
        } else if (page == 3) {
            if (strTCT.inFontFNTMode == 1)
                inCCTOS_PrinterBufferOutput("     ***** BANK COPY *****  ", &stgFONT_ATTRIB, 1);
            else
                inCCTOS_PrinterBufferOutput("     ***** BANK COPY *****  ", &stgFONT_ATTRIB, 1);
        } else {
            if (strTCT.inFontFNTMode == 1)
                inCCTOS_PrinterBufferOutput("        ***** COPY *****  ", &stgFONT_ATTRIB, 1);
            else
                inCCTOS_PrinterBufferOutput("        ***** COPY *****  ", &stgFONT_ATTRIB, 1);
        }
        
//                if(page != 3){
//		if (strTCT.inFontFNTMode == 1)
//                    inCCTOS_PrinterBufferOutput("        ***** COPY *****  ",&stgFONT_ATTRIB,1);
//		else
//                    inCCTOS_PrinterBufferOutput("        ***** COPY *****  ",&stgFONT_ATTRIB,1);
//                } else {
//                    if (strTCT.inFontFNTMode == 1)
//                    inCCTOS_PrinterBufferOutput("     ***** BANK COPY *****  ",&stgFONT_ATTRIB,1);
//		else
//                    inCCTOS_PrinterBufferOutput("     ***** BANK COPY *****  ",&stgFONT_ATTRIB,1);
//                }
	}

	//http://118.201.48.210:8080/redmine/issues/1525
	#if 0
    if(srTransRec.fIsInstallment == TRUE){
        vdPrintCenter("THANK YOU, HAVE A NICE DAY");
        vdPrintCenter("WELCOME AGAIN");
    }
	#endif
	
	//CTOS_PrinterFline(d_LINE_DOT * 6);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();

return d_OK;;	

}



USHORT ushCTOS_printAll(int page)
{	
	int inResult;
    int inParkingFee=0;

	vdDebug_LogPrintf("ushCTOS_printAll AAAAA [%s] [%d][%d][%d][%d]",  srTransRec.szHostLabel, srTransRec.HDTid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, srTransRec.byTransType);
	
    //vdSetPrintThreadStatus(0);
    
	//vdSetPrintThreadStatus(1);
	//vdThreadDisplayPrintBmpEx(0, 32, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);

    if(VS_FALSE == fRePrintFlag)
	    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }

    if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    	return -1;
	
	if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4); 
	
    #ifdef PARKING_FEE
	inParkingFee = get_env_int("PARKFEE");
    #endif
	
	if(srTransRec.fAlipay == TRUE)
	{
		ushCTOS_PrintHeader(page);
		ushCTOS_PrintBodyAlipay(page);
		ushCTOS_PrintFooterAlipay(page);
	}
	else if(inParkingFee == 1)
	{
		ushCTOS_PrintHeaderParkingFee(page);
		ushCTOS_PrintBodyParkingFee(page);
		//ushCTOS_PrintFooter(page);
	}
	else if (srTransRec.HDTid == CBPAY_HOST_INDEX)
	{

		//separate condition for CB pay printing
		ushCTOS_PrintHeader(page);
		ushCTOS_PrintBodyCBPay2(page);	// for Case 1595, 1600, 1603 , 1605
		ushCTOS_PrintFooterCBPay(page);

	}
	else if (srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
	{
		//separate condition for OKD printing
		ushCTOS_PrintHeader(page);
		#ifdef OK_DOLLAR_FEATURE
		ushCTOS_PrintBodyOKD2(page);	
		ushCTOS_PrintFooterOKD(page);
		#endif
	}
	else
	{
		ushCTOS_PrintHeader(page);
		ushCTOS_PrintBody(page);	
		if(srTransRec.byTransType == VOID)
		    ushCTOS_PrintFooterSignature(page);
		else
		    ushCTOS_PrintFooter(page);
	}

#if 0
	CTOS_LCDSelectModeEx(d_LCD_GRAPHIC_320x240_MODE, FALSE);
	CTOS_LCDGShowBMPPic(1, 1, PRINT_BMP);
	CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE);
	CTOS_Delay(5000);
#endif
    //vdSetPrintThreadStatus(0);
    CTOS_LCDTClearDisplay();  


	vdDebug_LogPrintf("ushCTOS_printAll END [%s] [%d][%d][%d][%d]",  srTransRec.szHostLabel, srTransRec.HDTid, strHDT.inCurrencyIdx, 
		strCST.inCurrencyIndex, srTransRec.byTransType);
	
    return d_OK;
 
}

int inCTOS_rePrintReceipt(void)
{
	int   inResult;
	BYTE  szInvNoAsc[6+1];
	BYTE  szInvNoBcd[3];	
	BOOL   needSecond = TRUE;	
	BYTE   key;

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
   // 	return (d_OK);
	CTOS_PrinterSetHeatLevel(4);
	if( printCheckPaper()==-1)
		return d_NO;
	
	inResult = inCTOS_GeneralGetInvoice();
	if(d_OK != inResult)
	{
		return inResult;
	}

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOS_MultiAPBatchSearch(d_IPC_CMD_REPRINT_ANY);
		if(d_OK != inResult)
			return inResult;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inResult = inCTOS_MultiAPGetVoid();
			if(d_OK != inResult)
				return inResult;
		}		
		inResult = inCTOS_BatchSearch();
		if(d_OK != inResult)
			return inResult;
	}

	if(d_OK != inResult)
	{
		return inResult;
	}

	inIITRead(srTransRec.IITid);
	inResult = ushCTOS_printAll(d_FIRST_PAGE);
	if(inResult == d_OK )
	{
		CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  

		vduiDisplayStringCenter(7,"CUSTOMER COPY");
		vduiDisplayStringCenter(8,"NO[X] YES[OK]");

		while(1)
		{ 
			vduiWarningSound();
			
			CTOS_KBDHit(&key);
			if(key == d_KBD_ENTER)
			{
				break;
			}
			else if((key == d_KBD_CANCEL))
			{
				needSecond = FALSE;
				break;
			}
			if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
			{
				break;
			}		
		}
		
		if(needSecond)
		{
			ushCTOS_printAll(d_SECOND_PAGE);
		}
	}

	return d_OK;
}

void vdCTOS_PrintSummaryReport()
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult,inRet;
	int inTranCardType;
	int inReportType;
	int inIITNum , i;
	char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    int inTotalCount = 0;

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return ;
	if( printCheckPaper()==-1)
		return;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;
	strHDT.inHostIndex = shHostIndex;
    
	DebugAddINT("summary host Index",shHostIndex);
	vdDebug_LogPrintf("vdCTOS_PrintSummaryReport [%d]", strHDT.inHostIndex);
	
	inCSTRead(strHDT.inCurrencyIdx);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_SUMMARY_REPORT);
		if(d_OK != inRet)
			return ;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return ;
		}
	}

	inResult = inCTOS_CheckAndSelectMutipleMID();

	vdDebug_LogPrintf("vdCTOS_PrintSummaryReport inResult [%d]", inResult);
	
	DebugAddINT("summary MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return;
	}

    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;    
    }

    if (strTCT.inFontFNTMode == 1)
	{
	    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);
    
	ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("SUMMARY REPORT");
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);
        
    if(strHDT.inHostIndex == 17
#ifdef CBPAY_DV	
	|| strHDT.inHostIndex == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX || strHDT.inHostIndex == 6 || 
	strHDT.inHostIndex == 23
#endif
	)
    {
        inCSTRead(1);
    }

    printHostLabel();
    
	
#ifdef CBPAY_DV
	if(srTransRec.HDTid == CBPAY_HOST_INDEX)
		printTIDMIDCBPay();	
#ifdef OK_DOLLAR_FEATURE
	else if(srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)		
		printTIDMIDOKD();
#endif
	else
		printTIDMID();
#else
	printTIDMID();
#endif

	printDateTime();
    
	printBatchNO();

	//CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);
        
        if(fGetMPUTrans() == VS_TRUE)
            inTranCardType = 1;
        else
            inTranCardType = 0;
        
        if(shHostIndex == 17 || shHostIndex == 18 || shHostIndex == 19 || shHostIndex == 20 || shHostIndex == 22 || shHostIndex == 23)
		{
            inTranCardType = 1;
        } 
		else 
		{
            inTranCardType = 0;
        }
		
		if(TRUE == fGetCashAdvAppFlag())
		{
			inTranCardType = 0;
		}
        
        inTotalCount = inTranCardType + 1;
//	for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
        for(inTranCardType; inTranCardType < inTotalCount; inTranCardType++)
	{
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				//vdDebug_LogPrintf("AAA--Count[%d][%d][%12.0f]", i, (int)srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPayVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usTopupCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRepaymentCount== 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount == 0))
					continue;
				
				vdDebug_LogPrintf("BBB--Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,strIIT.szIssuerLabel);
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount,   
                                (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),
                                
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"TOTAL:");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   
                                (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),
                                
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                        
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),
                        
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
		}
	}
    
	//print space one line
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
	return;	
}

static void vdCTOS_PrintSummaryAfterDetail(void){
    ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult,inRet;
	int inTranCardType;
	int inReportType;
	int inIITNum , i;
	char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    int inTotalPrint = 0;
    
//    vdPrintTitleCenter("TRANSACTION TOTAL");
    vdPrintCenter("TRANSACTION TOTAL");
	vdDebug_LogPrintf("vdCTOS_PrintSummaryAfterDetail [%d]", srTransRec.HDTid);
    
    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
        return;    
    }
    
    if(fGetMPUTrans() == VS_TRUE){
        inTranCardType = 1;
        inTotalPrint = 2;
    } else{
        inTranCardType = 0;
        inTotalPrint = 1;
    }
    
    
//    if(strTCT.fMustAutoSettle == TRUE){
//        if(strHDT.inHostIndex == 17) {
//            inTranCardType = 1;
//            inTotalPrint = 2;
//        } else {
//            inTranCardType = 0;
//            inTotalPrint = 1;
//        }
//        
//        
//    }
    
    if(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23
		#ifdef TOPUP_RELOAD
		|| srTransRec.HDTid == 20
		#endif
		){
        inTranCardType = 1;
        inTotalPrint = 2;
    } else {
        inTranCardType = 0;
        inTotalPrint = 1;
    }
    
    if(strTCT.fMustAutoSettle == TRUE){
        if(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23) {
            inTranCardType = 1;
            inTotalPrint = 2;
        } else {
            inTranCardType = 0;
            inTotalPrint = 1;
        }
    }
	
	if(TRUE == fGetCashAdvAppFlag())
	{
			inTranCardType = 0;
			inTotalPrint = 1;
	}
	
//    for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
    for(inTranCardType; inTranCardType < inTotalPrint ;inTranCardType ++)
	{
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				//vdDebug_LogPrintf("--Count[%d][%d]", i, (int)srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPayVoidSaleCount);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount == 0))
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,strIIT.szIssuerLabel);
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount,   ((double) srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulSaleTotalAmount) - ((double) srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),
								
                  				srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),              

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"TOTAL:");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),
                                
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount), 

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount), 
			

						srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
		}
	}
    
	//print space one line
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
	return;	
}

void vdCTOS_PrintDetailReportForSettleAll(void)
{
	unsigned char chkey;
	short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	int inBatchNumOfRecord;
	int *pinTransDataid;

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return ;
	if( printCheckPaper()==-1)
		return;
		

	if (strTCT.inFontFNTMode == 1)
	{
	    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);

	#ifdef CBPAY_DV
		if (strHDT.inHostIndex == CBPAY_HOST_INDEX)
			ushCTOS_printBatchRecordHeaderCBPay();		
		#ifdef OK_DOLLAR_FEATURE
		else if (strHDT.inHostIndex == OK_DOLLAR_HOST_INDEX)
			ushCTOS_printBatchRecordHeaderOKD();
		#endif
		else
			ushCTOS_printBatchRecordHeader();
	#else
		ushCTOS_printBatchRecordHeader();
	#endif

										// fix for case #1346 122319																//IPP should always be MMK //http://118.201.48.210:8080/redmine/issues/1525.115
        if(strHDT.inHostIndex == 17 || strHDT.inHostIndex == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX || strHDT.inHostIndex == 6
			|| srTransRec.HDTid == 23){
            inCSTRead(1);
    } else {

	if(srTransRec.HDTid == 22)
		strHDT.inCurrencyIdx = 2;
		
        inCSTRead(strHDT.inCurrencyIdx);
        }
	
	pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
	inCount = 0;		
	inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
    for (i = 0; i < inBatchNumOfRecord; i++) {
		inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;
		#ifdef CBPAY_DV
				if (strHDT.inHostIndex == CBPAY_HOST_INDEX)
					ushCTOS_printBatchRecordFooterCBPay();
				#ifdef OK_DOLLAR_FEATURE
				else if(strHDT.inHostIndex == OK_DOLLAR_HOST_INDEX)
					ushCTOS_printBatchRecordFooterOKD();
				#endif
				else
					ushCTOS_printBatchRecordFooter();
		#else
				ushCTOS_printBatchRecordFooter();
		#endif
	}
        
        vdCTOS_PrintSummaryAfterDetail();

    //CTOS_PrinterFline(d_LINE_DOT * 5);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
	vdCTOSS_PrinterEnd();
	free(pinTransDataid);
	
	return;	
}

void vdCTOS_PrintDetailReport(void)
{
	unsigned char chkey;
	short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	int inBatchNumOfRecord;
	int *pinTransDataid;

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return ;
	if( printCheckPaper()==-1)
		return;
		
	//by host and merchant
//	if(PrintDetail==0)
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;

	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);
	vdDebug_LogPrintf("vdCTOS_PrintDetailReport [%d]", strHDT.inHostIndex);

	inCSTRead(strHDT.inCurrencyIdx);

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_DETAIL_REPORT);
		if(d_OK != inRet)
			return ;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return ;
		}
	}

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("summary MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

    inResult = inCTOS_ChkBatchEmpty();
    if(d_OK != inResult)
    {
        #ifdef MINOR_CHANGES
        if(inPreauthNumRecord() > 0) /*check if preauth exist, if exist will print detail report*/
        {
             vdDebug_LogPrintf("Preauth trans is present");
			 vdSetErrorMessage("");
        }
        else
			return;
		#else
		return;
        #endif
        
    }
	
	inBatchNumOfRecord = inBatchNumRecord();
	
	DebugAddINT("batch record",inBatchNumOfRecord);
	if(inBatchNumOfRecord <= 0)
	{
		vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
		return;
	}

	if (strTCT.inFontFNTMode == 1)
	{
	    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);
	//ushCTOS_printBatchRecordHeader();

	#ifdef CBPAY_DV
			if (strHDT.inHostIndex == CBPAY_HOST_INDEX)
				ushCTOS_printBatchRecordHeaderCBPay();
			#ifdef OK_DOLLAR_FEATURE
			else if (strHDT.inHostIndex == OK_DOLLAR_HOST_INDEX)
				ushCTOS_printBatchRecordHeaderOKD();
			#endif
			else
				ushCTOS_printBatchRecordHeader();
	#else
			ushCTOS_printBatchRecordHeader();
	#endif

		
    if(strHDT.inHostIndex == 17
#ifdef CBPAY_DV																					//http://118.201.48.210:8080/redmine/issues/1525.115
	|| strHDT.inHostIndex == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX || strHDT.inHostIndex == 6 ||
	strHDT.inHostIndex == 23
#endif	
	)
    {
        inCSTRead(1);
    } 
    else 
    {
    	if(strHDT.inHostIndex == 22)
		strHDT.inCurrencyIdx = 2;
		
        inCSTRead(strHDT.inCurrencyIdx);
    }
	
	pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
	inCount = 0;		
	inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
    for (i = 0; i < inBatchNumOfRecord; i++) {
		inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;
		#ifdef CBPAY_DV
			if (strHDT.inHostIndex == CBPAY_HOST_INDEX)
				ushCTOS_printBatchRecordFooterCBPay();
			#ifdef OK_DOLLAR_FEATURE
			else if (strHDT.inHostIndex == OK_DOLLAR_HOST_INDEX)
				ushCTOS_printBatchRecordFooterOKD();
			#endif
			else
				ushCTOS_printBatchRecordFooter();
		#else

			ushCTOS_printBatchRecordFooter();

		#endif
	}
        
        vdCTOS_PrintSummaryAfterDetail();

    //CTOS_PrinterFline(d_LINE_DOT * 5);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
	vdCTOSS_PrinterEnd();
	free(pinTransDataid);
	
	return;	
}




int inCTOS_rePrintLastReceipt(void)
{
	int   inResult;
	BYTE  szInvNoAsc[6+1];
	BYTE  szInvNoBcd[3];	
    BOOL   needSecond = TRUE;	
    BYTE   key;

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);
	CTOS_PrinterSetHeatLevel(4);
	if( printCheckPaper()==-1)
		return d_NO;
	
	memcpy(srTransRec.szInvoiceNo,strTCT.szLastInvoiceNo,INVOICE_BCD_SIZE);
	DebugAddHEX("last invoice", strTCT.szLastInvoiceNo,3 );

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOS_MultiAPBatchSearch(d_IPC_CMD_REPRINT_LAST);
		if(d_OK != inResult)
			return inResult;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inResult = inCTOS_MultiAPGetVoid();
			if(d_OK != inResult)
				return inResult;
		}		
		inResult = inCTOS_BatchSearch();
		if(d_OK != inResult)
			return inResult;
	}

	inIITRead(srTransRec.IITid);
	inResult = ushCTOS_printAll(d_FIRST_PAGE);
	if(inResult == d_OK )
	{
		CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);  
		vduiDisplayStringCenter(7,"CUSTOMER COPY");
		vduiDisplayStringCenter(8,"NO[X] YES[OK]");

		while(1)
		{ 
			vduiWarningSound();
			
			CTOS_KBDHit(&key);
			if(key == d_KBD_ENTER)
			{
				break;
			}
			else if((key == d_KBD_CANCEL))
			{
				needSecond = FALSE;
				break;
			}
			if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
			{
				break;
			}		
		}
		
		if(needSecond)
		{
			ushCTOS_printAll(d_SECOND_PAGE);
		}
	}

	return d_OK;
}

USHORT ushCTOS_printReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
    int i;
	int inGetHDTDefCurrVal  = get_env_int("#CURRDEFVAL");

	#ifdef APP_AUTO_TEST
	if (inCTOS_GetAutoTestCnt() > 1)
	{
		vdDebug_LogPrintf("Auto Test ---> Print 2nd Receipt");
		return d_OK;
	}
	#endif

	
	vdDebug_LogPrintf("ushCTOS_printReceipt  [%s] [%d][%d][%d][%d][%d]",  
	srTransRec.szHostLabel, srTransRec.HDTid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, srTransRec.byTransType, inGetHDTDefCurrVal);
	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);

    //if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    //	return -1;

	//vdDisplayAnimateBmp(0,0, "Printer1(320240).bmp", "Printer2(320240).bmp", "Printer3(320240).bmp", NULL, NULL);
    if(srTransRec.byTransType == BALANCE_ENQUIRY)
    {
		ushCTOS_printAll(d_SECOND_PAGE);
		return d_OK;
    }
		
	//inInitializePrinterBufferLib();
	//inSIGPStartCapturePrinterDataLib();
    result = ushCTOS_printAll(d_FIRST_PAGE);
	//inSIGPEndCapturePrinterDataLib();
	inCTOSS_ERM_Form_Receipt(0);

	if (d_OK == isCheckTerminalMP200())
		return d_OK;

	inCTLOS_Updatepowrfail(PFR_PRINT_MERCHANTCOPY);
	CTOS_KBDBufFlush();//cleare key buffer
    if(result == d_OK)
    {
        
        if(strTCT.inDupReceipt > 1) {
            for (i = 1; i < strTCT.inDupReceipt; i++) {
                vdDebug_LogPrintf("@@IBR i = %d", i);
                CTOS_KBDBufFlush();
                key = 0;
                vdDebug_LogPrintf("Page = %d", i + 2);
                //CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
                if ((strTCT.byTerminalType % 2) == 0) {
                    //			vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"CUSTOMER COPY");
                    vduiDisplayStringCenter(V3_STATUS_LINE_ROW, "MORE COPY");
                    vduiDisplayStringCenter(V3_ERROR_LINE_ROW, "NO[X] YES[OK]");
                } else {
                    //			vduiDisplayStringCenter(7,"CUSTOMER COPY");
                    vduiDisplayStringCenter(7, "MORE COPY");
                    vduiDisplayStringCenter(8, "NO[X] YES[OK]");
                }
                vduiWarningSound();

				#if 1
				needSecond=FALSE;
                while(1)
                {
                    key = struiGetchWithTimeOut();
                    if(key == d_KBD_ENTER)
                    {
                        needSecond=TRUE;
                        break;
                    }
                    else if(key == d_KBD_CANCEL)
                    {
                        //needSecond=FALSE;
                        vduiClearBelow(7);
						break;
                    }
                    else
                        vduiWarningSound();
                }
				#else
                //            CTOS_KBDHit(&key);
                CTOS_KBDGet(&key);

				needSecond = FALSE;
                if (key == d_KBD_CANCEL) {
                    //needSecond = FALSE;
                    break;
                }
				else if(key == d_KBD_ENTER)
				{
					needSecond = TRUE;
				}
					
                if (CTOS_TimeOutCheck(TIMER_ID_1) == d_YES) {
                    break;
                }
                #endif
				
                if (needSecond) {
                    //                return (ushCTOS_printAll(i+2));
                    ushCTOS_printAll(i + 1);
                }
				else
					break;
            }
        }
        
//        CTOS_TimeOutSet(TIMER_ID_1, UI_TIMEOUT);
//		if ((strTCT.byTerminalType%2) == 0)
//		{
////			vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"CUSTOMER COPY");
//                    vduiDisplayStringCenter(V3_STATUS_LINE_ROW,"MORE COPY");
//			vduiDisplayStringCenter(V3_ERROR_LINE_ROW,"NO[X] YES[OK]");
//		}
//		else
//		{
////			vduiDisplayStringCenter(7,"CUSTOMER COPY");
//                        vduiDisplayStringCenter(7,"MORE COPY");
//			vduiDisplayStringCenter(8,"NO[X] YES[OK]");
//		}
//        
//        
//        while(1)
//        { 
//			vduiWarningSound();
//			
//            CTOS_KBDHit(&key);
//            if(key == d_KBD_ENTER)
//            {
//                break;
//            }
//            else if((key == d_KBD_CANCEL))
//            {
//                needSecond = FALSE;
//                break;
//            }
//            if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
//            {
//                break;
//            }		
//        }
//		
//        if(needSecond)
//        {
//            return (ushCTOS_printAll(d_SECOND_PAGE));
//        }
    }

	
	vdDebug_LogPrintf("ushCTOS_printReceipt END :  [%s] [%d][%d][%d][%d][%d]",  
		srTransRec.szHostLabel, srTransRec.HDTid, strHDT.inCurrencyIdx, strCST.inCurrencyIndex, srTransRec.byTransType, inGetHDTDefCurrVal);

	// Return default currency code based on inGetHDTDefCurrVal value
	#ifdef CBB_FIN_ROUTING
	if(inGetHDTDefCurrVal == 2 || strHDT.inCurrencyIdx == 2)
		inCSTRead(2);
	else
		inCSTRead(1);		
	#endif

	
    return (d_OK);
}



USHORT ushCTOS_printErrorReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	
	char szStr[d_LINE_SIZE + 1];
   char szTemp[d_LINE_SIZE + 1];
   char szTemp1[d_LINE_SIZE + 1];
   char szTemp3[d_LINE_SIZE + 1];
   char szTemp4[d_LINE_SIZE + 1];
   char szTemp5[d_LINE_SIZE + 1];
   char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
   int inFmtPANSize;
   BYTE baTemp[PAPER_X_SIZE * 64];
   CTOS_FONT_ATTRIB stFONT_ATTRIB;
   int num,i,inResult;
   unsigned char tucPrint [24*4+1];    
   BYTE   EMVtagVal[64];
   USHORT EMVtagLen; 
   short spacestring;

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);

    //if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    //	return -1;

	memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
	
	DebugAddHEX("LastInvoiceNum", strTCT.szLastInvoiceNo,3);
	
    if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    	return -1;

	if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  
	DebugAddSTR("ushCTOS_printAll","print...",20);
		

	ushCTOS_PrintHeader(d_FIRST_PAGE);

	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
	vdPrintCenter(szTemp1);
	
	printDateTime();

    printTIDMID(); 
	
	printBatchInvoiceNO();
	
	szGetTransTitle(srTransRec.byTransType, szStr);     
		vdPrintTitleCenter(szStr);    
    if(srTransRec.byTransType == SETTLE)
    {
        DebugAddSTR("settle","print...",20);
    }
    else
    {
       // vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	if (strTCT.inFontFNTMode == 1)
	{
	    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
   	}
    	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    	EMVtagLen = 0;
    	memset(szStr, ' ', d_LINE_SIZE);
    	vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

    	sprintf(szStr, "%s", srTransRec.szCardLable);
    	vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
    	
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

    	cardMasking(srTransRec.szPAN, PRINT_CARD_MASKING_1);
    	strcpy(szTemp4, srTransRec.szPAN);
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

        //vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        if (strTCT.inFontFNTMode == 1)
		{
		    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		}
		else
		{
	    //use ttf print
		    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	   	}
    	//Exp date and Entry mode
    	memset(szStr, ' ', d_LINE_SIZE);
    	memset(szTemp, 0, sizeof(szTemp));
    	memset(szTemp1, 0, sizeof(szTemp1));
    	memset(szTemp4, 0, sizeof(szTemp4));
    	memset(szTemp5, 0, sizeof(szTemp5));
    	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
    	DebugAddSTR("EXP",szTemp,12);  
    	
    	for (i =0; i<4;i++)
    		szTemp[i] = '*';
    	memcpy(szTemp4,&szTemp[0],2);
    	memcpy(szTemp5,&szTemp[2],2);

    	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
    		memcpy(szTemp1,"Chip",4);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
    		memcpy(szTemp1,"Manual",6);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
    		memcpy(szTemp1,"MSR",3);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
    		memcpy(szTemp1,"Fallback",8);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
            if(srTransRec.bWaveSID == d_VW_SID_CUP_EMV)
            {
                memcpy(szTemp1,"QuickPass",9);
            }
		}
    	memset (baTemp, 0x00, sizeof(baTemp));
    	sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
    	
    	DebugAddSTR("ENT:",baTemp,12);  
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);

    		
     	memset (baTemp, 0x00, sizeof(baTemp));
    	stFONT_ATTRIB.FontSize = 0x1010;
        stFONT_ATTRIB.X_Zoom = DOUBLE_SIZE;       // The width magnifies X_Zoom diameters
        stFONT_ATTRIB.Y_Zoom = DOUBLE_SIZE;       // The height magnifies Y_Zoom diameters

        stFONT_ATTRIB.X_Space = 0;      // The width of the space between the font with next font

    }

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szStr, "%s", "TRANS NOT SUC.");
	
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	
	sprintf(szStr, "%s", "PLS TRY AGAIN");
	
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	//CTOS_PrinterFline(d_LINE_DOT * 6);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
	//inSetTextMode();
    return (d_OK);
}


int inCTOS_REPRINT_ANY()
{
    char szErrMsg[30+1];
    int   inResult;
	int inLastCurrCode = 0;

	vdCTOS_TxnsBeginInit();


#ifdef CBB_FIN_ROUTING
	vdDebug_LogPrintf("inCTOS_REPRINT_ANY BEFORE <inCTOS_rePrintLastReceipt> HDTid[%d] strHDT.inCurrencyIdx[%d] strCST.inCurrencyIndex[%d]", 
		srTransRec.HDTid, strHDT.inCurrencyIdx,  strCST.inCurrencyIndex);
	
	inLastCurrCode =  strCST.inCurrencyIndex;
#endif
	
	fRePrintFlag = TRUE;
    vdCTOS_SetTransType(REPRINT_ANY);
	inResult = inCTOS_rePrintReceipt();

#ifdef CBB_FIN_ROUTING
	vdDebug_LogPrintf("inCTOS_REPRINT_ANY AFTER <inCTOS_rePrintLastReceipt> HDTid[%d] strHDT.inCurrencyIdx[%d] strCST.inCurrencyIndex[%d]inLastCurrCode[%d]", 
		srTransRec.HDTid, strHDT.inCurrencyIdx,  strCST.inCurrencyIndex, inLastCurrCode);

	//For Curr code default value display issue.  Not returning to original default currency. - 08242022
	inCSTRead(inLastCurrCode);
#endif 

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;
	return inResult;
}

int inCTOS_MPU_REPRINT_ANY()
{
    char szErrMsg[30+1];
    int   inResult;
	
    vdSetMPUTrans(TRUE);
	fRePrintFlag = TRUE;
    vdCTOS_SetTransType(REPRINT_ANY);
	inResult = inCTOS_rePrintReceipt();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    vdSetMPUTrans(FALSE);
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;
	return inResult;
}

int inCTOS_REPRINT_LAST()
{
    char szErrMsg[30+1];
	int   inResult;
	int inLastCurrCode = 0;


	#ifdef CBB_FIN_ROUTING
	vdDebug_LogPrintf("inCTOS_REPRINT_LAST BEFORE <inCTOS_rePrintLastReceipt> HDTid[%d] strHDT.inCurrencyIdx[%d] strCST.inCurrencyIndex[%d]", 
		srTransRec.HDTid, strHDT.inCurrencyIdx,  strCST.inCurrencyIndex);
	
	inLastCurrCode =  strCST.inCurrencyIndex;
	#endif
    	
	fRePrintFlag = TRUE;
	inResult = inCTOS_rePrintLastReceipt();	

	#ifdef CBB_FIN_ROUTING
	vdDebug_LogPrintf("inCTOS_REPRINT_LAST AFTER <inCTOS_rePrintLastReceipt> HDTid[%d] strHDT.inCurrencyIdx[%d] strCST.inCurrencyIndex[%d]inLastCurrCode[%d]", 
		srTransRec.HDTid, strHDT.inCurrencyIdx,  strCST.inCurrencyIndex, inLastCurrCode);

	//For Curr code default value display issue.  Not returning to original default currency. - 08242022
	inCSTRead(inLastCurrCode);
	#endif 



    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;
	
	return inResult;
}

int inCTOS_MPU_REPRINT_LAST()
{
    char szErrMsg[30+1];
	int   inResult;


	
	
        vdSetMPUTrans(TRUE);
	fRePrintFlag = TRUE;
	inResult = inCTOS_rePrintLastReceipt();	


    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    vdSetMPUTrans(FALSE);
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
	fRePrintFlag = FALSE;
	
	return inResult;
}

int inCTOS_PRINTF_SUMMARY()
{
    char szErrMsg[30+1];
    
	vdCTOS_PrintSummaryReport();	

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
    
	return d_OK;
	
}

int inCTOS_MPU_PRINTF_SUMMARY(void){ //@@IBR ADD 20170222
    vdSetMPUTrans(TRUE);
    inCTOS_PRINTF_SUMMARY();
    vdSetMPUTrans(FALSE);
}

int inCTOS_PRINTF_DETAIL(void)
{
    char szErrMsg[30+1];
    TRANS_DATA_TABLE srTransTemp;
    
    memset(&srTransTemp, 0x00, sizeof(TRANS_DATA_TABLE));
//    memcpy(&srTransTemp, &srTransRec, sizeof(TRANS_DATA_TABLE));
    srTransTemp = srTransRec;
    
	vdCTOS_PrintDetailReport();
//        vdCTOS_PrintSummaryReport();

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    
    
        memset(&srTransRec, 0x00, sizeof (TRANS_DATA_TABLE));
        CTOS_KBDBufFlush();
        
        if(strTCT.fMustAutoSettle == TRUE){
            vdCTOS_SetTransType(SETTLE);
            srTransRec = srTransTemp;
        }
        
	return d_OK;
	
}

int inCTOS_MPU_PRINTF_DETAIL(void){ //@@IBR ADD 20170222
    
    vdSetMPUTrans(TRUE);
    inCTOS_PRINTF_DETAIL();
    vdSetMPUTrans(FALSE);
}

int inCTOS_REPRINTF_LAST_SETTLEMENT(void)
{
    char szErrMsg[30+1];

	vdDebug_PrintOnPaper("inCTOS_REPRINTF_LAST_SETTLEMENT");

	vdCTOS_TxnsBeginInit();
	
	ushCTOS_ReprintLastSettleReport();
    
    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
    
	return d_OK;
}

int inCTOS_MPU_REPRINTF_LAST_SETTLEMENT(void)
{
    char szErrMsg[30+1];
    
    vdSetMPUTrans(TRUE);
	ushCTOS_ReprintLastSettleReport();
    
    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        vdDisplayErrorMsg(1, 8, szErrMsg);
        vdSetErrorMessage("");
    }
    vdSetMPUTrans(FALSE);
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    CTOS_KBDBufFlush();
    
	return d_OK;
}

static void vdCTOS_PrintSettleIPP(int inReportType, double ulSaleTotalAmount, USHORT usSaleCount, double ulVoidSaleTotalAmount, USHORT usVoidSaleCount) {
    char szStr[d_LINE_SIZE * 2 + 1];
    char szTemp[d_LINE_SIZE + 1];
    char szSaleTotalAmount[16 + 1];
    char szRefundTotalAmount[16 + 1];
    char szVoidSaleTotalAmount[16 + 1];
    char sztmpAmount[30 + 1];
    char szTempBuf1[d_LINE_SIZE + 1];
    char szTempBuf2[d_LINE_SIZE + 1];
    int inTemp1, inTemp2, inTemp3, inTemp4;
    USHORT usCharPerLine = 32;
    char szTempAmt[AMT_ASC_SIZE + 1];
    USHORT usTotalCount;
    double ulTotalAmount;
    char szTotalAmount[16+1];

    memset(szStr, ' ', d_LINE_SIZE * 2);

    if (strTCT.inFontFNTMode == 1)
        strcpy(szStr, "TYPES   CNT CUR              AMT");
    else {
		#if 0
        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        strcpy(szTempBuf1, "TYPES");
        inTemp1 = strlen(szTempBuf1);
        memcpy(szStr, szTempBuf1, inTemp1);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        strcpy(szTempBuf1, "CNT");
        inTemp2 = strlen(szTempBuf1);
        memcpy(&szStr[(11 - inTemp1 - inTemp2)*2 + inTemp1], szTempBuf1, inTemp2);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        strcpy(szTempBuf1, "CUR");
        inTemp3 = strlen(szTempBuf1);
        memcpy(&szStr[(15 - inTemp1 - inTemp2 - inTemp3)*2 + inTemp1 + inTemp2], szTempBuf1, inTemp3);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        strcpy(szTempBuf1, "AMT");
        inTemp4 = strlen(szTempBuf1);
        memcpy(&szStr[(22 - inTemp1 - inTemp2 - inTemp3 - inTemp4)*2 + inTemp1 + inTemp2 + inTemp3], szTempBuf1, inTemp4);
		#endif
		
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr, "TYPES     CNT  CUR         AMT");
        vdDebug_LogPrintf("TYPES,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]", inTemp1, inTemp2, inTemp3, inTemp4);
    }
    inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);

    //sale
    memset(szStr, ' ', d_LINE_SIZE * 2);
    //format amount 10+2
    if (strTCT.inFontFNTMode == 1) {
        sprintf(szSaleTotalAmount, "%.0f", ulSaleTotalAmount);
        memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
            memset(szTempAmt, 0x00, sizeof (szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(szSaleTotalAmount)/100);
            memset(szSaleTotalAmount, 0x00, sizeof (szSaleTotalAmount));
            strcpy(szSaleTotalAmount, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szSaleTotalAmount, sztmpAmount);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szSaleTotalAmount, sztmpAmount);
        }

        sprintf(szStr, "INST.  %4d %s%17s", usSaleCount, strCST.szCurSymbol, sztmpAmount);

    } else {
        sprintf(szSaleTotalAmount, "%.0f", ulSaleTotalAmount);
        memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
            memset(szTempAmt, 0x00, sizeof (szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(szSaleTotalAmount)/100);
            memset(szSaleTotalAmount, 0x00, sizeof (szSaleTotalAmount));
            strcpy(szSaleTotalAmount, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szSaleTotalAmount, sztmpAmount);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szSaleTotalAmount, sztmpAmount);
        }

		#if 0
        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        //strcpy(szTempBuf1, "INSTALLMENT");
		strcpy(szTempBuf1, "INST     ");

        inTemp1 = strlen(szTempBuf1);
        memcpy(szStr, szTempBuf1, inTemp1);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        sprintf(szTempBuf1, "%d", usSaleCount);
        inTemp2 = strlen(szTempBuf1);
        memcpy(&szStr[(11 - inTemp1 - inTemp2)*2 + inTemp1], szTempBuf1, inTemp2);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        sprintf(szTempBuf1, "%s", strCST.szCurSymbol);
        inTemp3 = strlen(szTempBuf1);
        memcpy(&szStr[(15 - inTemp1 - inTemp2 - inTemp3)*2 + inTemp1 + inTemp2], szTempBuf1, inTemp3);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        strcpy(szTempBuf1, sztmpAmount);
        inTemp4 = strlen(szTempBuf1);
        memcpy(&szStr[(25 - inTemp1 - inTemp2 - inTemp3 - inTemp4)*2 + inTemp1 + inTemp2 + inTemp3], szTempBuf1, inTemp4);
		#endif
		
		memset(szStr, 0x00, sizeof(szStr));
        sprintf(szStr, "INST      %03d  %s%12s", usSaleCount, strCST.szCurSymbol, sztmpAmount);
		
        vdDebug_LogPrintf("INSTALLMENT,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]", inTemp1, inTemp2, inTemp3, inTemp4);
    }
    inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, 1);

    //void IPP
    memset(szStr, ' ', d_LINE_SIZE * 2);
    //format amount 10+2
    if (strTCT.inFontFNTMode == 1) {
        sprintf(szVoidSaleTotalAmount, "%.0f", ulVoidSaleTotalAmount);
        memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
            memset(szTempAmt, 0x00, sizeof (szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(szVoidSaleTotalAmount)/100);
            memset(szVoidSaleTotalAmount, 0x00, sizeof (szVoidSaleTotalAmount));
            strcpy(szVoidSaleTotalAmount, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szVoidSaleTotalAmount, sztmpAmount);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szVoidSaleTotalAmount, sztmpAmount);
        }

        sprintf(szStr, "VOIDINST %2d %s%17s", usVoidSaleCount, strCST.szCurSymbol, sztmpAmount);
    } else {
        sprintf(szVoidSaleTotalAmount, "%.0f", ulVoidSaleTotalAmount);
        memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

        if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
            memset(szTempAmt, 0x00, sizeof (szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(szVoidSaleTotalAmount)/100);
            memset(szVoidSaleTotalAmount, 0x00, sizeof (szVoidSaleTotalAmount));
            strcpy(szVoidSaleTotalAmount, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szVoidSaleTotalAmount, sztmpAmount);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szVoidSaleTotalAmount, sztmpAmount);
        }

		#if 0
        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        //strcpy(szTempBuf1, "VOID INSTALLMENT");
		strcpy(szTempBuf1, "VOID INST");

        inTemp1 = strlen(szTempBuf1);
        memcpy(szStr, szTempBuf1, inTemp1);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        sprintf(szTempBuf1, "%d", usVoidSaleCount);
        inTemp2 = strlen(szTempBuf1);
        memcpy(&szStr[(11 - inTemp1 - inTemp2)*2 + inTemp1], szTempBuf1, inTemp2);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        sprintf(szTempBuf1, "%s", strCST.szCurSymbol);
        inTemp3 = strlen(szTempBuf1);
        memcpy(&szStr[(15 - inTemp1 - inTemp2 - inTemp3)*2 + inTemp1 + inTemp2], szTempBuf1, inTemp3);

        memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
        strcpy(szTempBuf1, sztmpAmount);
        inTemp4 = strlen(szTempBuf1);
        memcpy(&szStr[(25 - inTemp1 - inTemp2 - inTemp3 - inTemp4)*2 + inTemp1 + inTemp2 + inTemp3], szTempBuf1, inTemp4);
		#endif
		
		memset(szStr, 0x00, sizeof(szStr));
        sprintf(szStr, "VOID INST %03d  %s%12s", usVoidSaleCount, strCST.szCurSymbol, sztmpAmount);
        vdDebug_LogPrintf("VOID INSTALLMENT,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]", inTemp1, inTemp2, inTemp3, inTemp4);
    }
    inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, 1);

    inCCTOS_PrinterBufferOutput("--------------------------------", &stgFONT_ATTRIB, 1);

    usTotalCount = usSaleCount;
    memset(szStr, ' ', d_LINE_SIZE * 2);
    memset(szTemp, ' ', d_LINE_SIZE);
    if (inReportType == PRINT_HOST_TOTAL) {

        ulTotalAmount = ulSaleTotalAmount ;
        //format amount 10+2
        if (strTCT.inFontFNTMode == 1) {
            sprintf(szTotalAmount, "%.0f", ulTotalAmount);
            memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTempAmt, 0x00, sizeof (szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
                strcpy(szTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
            }

            sprintf(szStr, "TOTALS %03d  %s%12s", usTotalCount, strCST.szCurSymbol, sztmpAmount);
        } else {
            sprintf(szTotalAmount, "%.0f", ulTotalAmount);
            memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTempAmt, 0x00, sizeof (szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
                strcpy(szTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
            }

			#if 0
            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            strcpy(szTempBuf1, "TOTALS");
            inTemp1 = strlen(szTempBuf1);
            memcpy(szStr, szTempBuf1, inTemp1);

            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            sprintf(szTempBuf1, "%d", usTotalCount);
            inTemp2 = strlen(szTempBuf1);
            memcpy(&szStr[(11 - inTemp1 - inTemp2)*2 + inTemp1], szTempBuf1, inTemp2);

            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            sprintf(szTempBuf1, "%s", strCST.szCurSymbol);
            inTemp3 = strlen(szTempBuf1);
            memcpy(&szStr[(15 - inTemp1 - inTemp2 - inTemp3)*2 + inTemp1 + inTemp2], szTempBuf1, inTemp3);

            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            strcpy(szTempBuf1, sztmpAmount);
            inTemp4 = strlen(szTempBuf1);
            memcpy(&szStr[(25 - inTemp1 - inTemp2 - inTemp3 - inTemp4)*2 + inTemp1 + inTemp2 + inTemp3], szTempBuf1, inTemp4);
			#endif
			
			memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "TOTALS    %03d  %s%12s", usTotalCount, strCST.szCurSymbol, sztmpAmount);

			vdDebug_LogPrintf("HOST_TOTAL22,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]", inTemp1, inTemp2, inTemp3, inTemp4);
        }
    } else if (inReportType == PRINT_CARD_TOTAL) {
        ulTotalAmount = ulSaleTotalAmount;
        //format amount 10+2
        if (strTCT.inFontFNTMode == 1) {
            sprintf(szTotalAmount, "%.0f", ulTotalAmount);
            memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTempAmt, 0x00, sizeof (szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
                strcpy(szTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
            }

            sprintf(szStr, "TOTALS %4d %s%17s", usTotalCount, strCST.szCurSymbol, sztmpAmount);
        } else {
            sprintf(szTotalAmount, "%.0f", ulTotalAmount);
            memset(sztmpAmount, 0x00, sizeof (sztmpAmount));

            if (strcmp(strCST.szCurSymbol, "MMK") == 0) {
                memset(szTempAmt, 0x00, sizeof (szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                memset(szTotalAmount, 0x00, sizeof (szTotalAmount));
                strcpy(szTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
            }
			#if 0
            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            strcpy(szTempBuf1, "TOTALS");
            inTemp1 = strlen(szTempBuf1);
            memcpy(szStr, szTempBuf1, inTemp1);

            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            sprintf(szTempBuf1, "%d", usTotalCount);
            inTemp2 = strlen(szTempBuf1);
            memcpy(&szStr[(11 - inTemp1 - inTemp2)*2 + inTemp1], szTempBuf1, inTemp2);

            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            sprintf(szTempBuf1, "%s", strCST.szCurSymbol);
            inTemp3 = strlen(szTempBuf1);
            memcpy(&szStr[(15 - inTemp1 - inTemp2 - inTemp3)*2 + inTemp1 + inTemp2], szTempBuf1, inTemp3);

            memset(szTempBuf1, 0x00, sizeof (szTempBuf1));
            strcpy(szTempBuf1, sztmpAmount);
            inTemp4 = strlen(szTempBuf1);
            memcpy(&szStr[(25 - inTemp1 - inTemp2 - inTemp3 - inTemp4)*2 + inTemp1 + inTemp2 + inTemp3], szTempBuf1, inTemp4);
			#endif
			
			memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "TOTALS    %03d  %s%12s", usTotalCount, strCST.szCurSymbol, sztmpAmount);
	
			vdDebug_LogPrintf("CARD_TOTAL11,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]", inTemp1, inTemp2, inTemp3, inTemp4);
        }
    }
    inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, 1);
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);

}

void vdCTOS_PrintAccumeByHostAndCard (int inReportType,
	USHORT usSaleCount, 
    double ulSaleTotalAmount,    
    USHORT usRefundCount,
    double  ulRefundTotalAmount,    
    USHORT usVoidSaleCount,
    double  ulVoidSaleTotalAmount,
    USHORT usOffSaleCount,
    double  ulOffSaleTotalAmount,
    USHORT usCashAdvCount, 
    double ulCashAdvTotalAmount,
    USHORT usCUPSaleCount, 
    double ulCUPSaleTotalAmount,
    USHORT usCUPPreAuthCount, 
    double ulCUPPreAuthTotalAmount, 
    USHORT usCBPaySaleCount,
    double ulCBPaySaleTotalAmount,
    USHORT usOKDSaleCount,
    double ulOKDSaleTotalAmount,    
    STRUCT_TOTAL Totals)
{
	char szStr[d_LINE_SIZE*2 + 1];
	char szTemp[d_LINE_SIZE + 1];
	USHORT usTotalCount;
	double  ulTotalAmount;
	char szTotalAmount[16+1];
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szSaleTotalAmount[16+1];
	char szRefundTotalAmount[16+1];
	char szVoidSaleTotalAmount[16+1];
	char szOffSaleTotalAmount[16+1];
	char szCashAdvTotalAmount[16+1];
	char szCUPSaleTotalAmount[16+1];
	char szCUPPreAuthTotalAmount[16+1];
	char sztmpAmount[30+1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	int inTemp1,inTemp2,inTemp3,inTemp4;

	USHORT usCharPerLine = 32;
	char szTempAmt[AMT_ASC_SIZE+1];

	USHORT usAlipaySaleCount;
    double ulAlipaySaleTotalAmount;

	char szCBPayTotalAmount[16+1];

#if 0
    USHORT usCBPaySaleCount;
    double ulCBPaySaleTotalAmount;
#endif

    #ifdef TOPUP_RELOAD
    USHORT usTopupCount;
    double ulTopupTotalAmount;
	USHORT usRepaymentCount;
    double ulRepaymentTotalAmount;
	#endif


#if 0
	memset(szStr, ' ', d_LINE_SIZE*2);
	sprintf(szStr, "inHostIndex %d", strHDT.inHostIndex);		
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif

	vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard START strHDT.inHostIndex[%d], fIPPGetSettleFlag[%d]", strHDT.inHostIndex, fIPPGetSettleFlag());

    if(fIPPGetSettleFlag() == TRUE || strHDT.inHostIndex == 17 || strHDT.inHostIndex == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX ||
		strHDT.inHostIndex == 6 || strHDT.inHostIndex == 23){
        inCSTRead(1);
//                    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    }
	else
	{
	// fix  to get correct currency code 
        inHDTRead(strHDT.inHostIndex);
		//FIN-USD host
		if(strHDT.inHostIndex == 22)
			strHDT.inCurrencyIdx = 2;
		
		inCSTRead(strHDT.inCurrencyIdx);	
	}
#if 0
	memset(szStr, ' ', d_LINE_SIZE*2);
	sprintf(szStr, "inCurrencyIdx %d", strHDT.inCurrencyIdx);		
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif

	usCharPerLine = inGetFontlen(&stgFONT_ATTRIB);

		//start_thandar_added in for Font update
  	 if (strTCT.inFontFNTMode != 1)
	{

		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}

//end_thandar_added in for Font update

		
		CTOS_PrinterSetHeatLevel(4);  
		memset (baTemp, 0x00, sizeof(baTemp));
		memset (szSaleTotalAmount, 0x00, sizeof(szSaleTotalAmount));
		memset (szRefundTotalAmount, 0x00, sizeof(szRefundTotalAmount));
		memset (szVoidSaleTotalAmount, 0x00, sizeof(szVoidSaleTotalAmount));
		memset (szOffSaleTotalAmount, 0x00, sizeof(szOffSaleTotalAmount));

		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard-fIPPGetSettleFlag()[%d]", fIPPGetSettleFlag());

		// called upon IPP own settlement function and not main menu Report  function (summary report)		
        if(fIPPGetSettleFlag() == TRUE){
			vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard-fIPPGetSettleFlag() == TRUE");
			
            vdCTOS_PrintSettleIPP(inReportType, ulSaleTotalAmount, usSaleCount, ulVoidSaleTotalAmount, usVoidSaleCount);
            return;
        }
		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard after fIPPGetSettleFlag() == TRUE");

		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard [%d][%d][%12.0f]", srTransRec.HDTid, usVoidSaleCount, ulVoidSaleTotalAmount);
		//vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard CBPay [%d][%12.0f]", usCBPayVoidSaleCount, ulCBPayVoidSaleTotalAmount);
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard CBPay [%d][%12.0f]", usCBPaySaleCount, ulCBPaySaleTotalAmount);
		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard CBPay [%d][%12.0f][%d]", usCBPaySaleCount, ulCBPaySaleTotalAmount, strTCT.inFontFNTMode);
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard OK$ [%d][%12.0f][%d]", usOKDSaleCount, ulOKDSaleTotalAmount, strTCT.inFontFNTMode);
		//types
		memset(szStr, ' ', d_LINE_SIZE*2);
//format amount 10+2
		if (strTCT.inFontFNTMode == 1)
			strcpy(szStr,"TYPES   CNT CUR              AMT");
		else
		{
			//strcpy(szStr,"TYPES      CNT  CUR                           AMT");
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"TYPES");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"CNT");
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);			
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"CUR");
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);
			

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"AMT");
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("TYPES,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr1 [%d][%s]", strlen(szStr), szStr);

		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

	
        /*Alipay Sale*/
		usAlipaySaleCount=Totals.usAlipaySaleCount;
		ulAlipaySaleTotalAmount=Totals.ulAlipaySaleTotalAmount;

		//sale
		memset(szStr, ' ', d_LINE_SIZE*2);
		//format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
			sprintf(szSaleTotalAmount,"%.0f",(ulSaleTotalAmount-ulAlipaySaleTotalAmount));
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szSaleTotalAmount)/100);
                memset(szSaleTotalAmount, 0x00, sizeof(szSaleTotalAmount));
                strcpy(szSaleTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szSaleTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szSaleTotalAmount, sztmpAmount);
            }
            
                sprintf(szStr,"SALES  %4d %s%17s",(usSaleCount-usAlipaySaleCount), strCST.szCurSymbol,sztmpAmount);
		}
		else
		{
			sprintf(szSaleTotalAmount,"%.0f",(ulSaleTotalAmount-ulAlipaySaleTotalAmount));
			vdDebug_LogPrintf("2. strCST.szCurSymbol[%s], szSaleTotalAmount[%s]", strCST.szCurSymbol, szSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szSaleTotalAmount)/100);
                memset(szSaleTotalAmount, 0x00, sizeof(szSaleTotalAmount));
                strcpy(szSaleTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szSaleTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szSaleTotalAmount, sztmpAmount);
            }

                        
			//sprintf(szStr,"SALES  %4d %s%17s",usSaleCount, strCST.szCurSymbol,sztmpAmount);
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"SALES");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",(usSaleCount-usAlipaySaleCount));
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("SALES,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}
		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr2 [%d][%s]", strlen(szStr), szStr);

		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

#if 1                
		//cash adv
		memset(szStr, ' ', d_LINE_SIZE*2);
		//format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
			sprintf(szCashAdvTotalAmount,"%.0f",ulCashAdvTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCashAdvTotalAmount)/100);
                memset(szCashAdvTotalAmount, 0x00, sizeof(szCashAdvTotalAmount));
                strcpy(szCashAdvTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCashAdvTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCashAdvTotalAmount, sztmpAmount);
            }
			
                        
			sprintf(szStr,"CASHADV%4d %s%17s",usCashAdvCount, strCST.szCurSymbol,sztmpAmount);
		}
		else
		{
			sprintf(szCashAdvTotalAmount,"%.0f",ulCashAdvTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCashAdvTotalAmount)/100);
                memset(szCashAdvTotalAmount, 0x00, sizeof(szCashAdvTotalAmount));
                strcpy(szCashAdvTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCashAdvTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCashAdvTotalAmount, sztmpAmount);
            }
                        
			//sprintf(szStr,"SALES  %4d %s%17s",usSaleCount, strCST.szCurSymbol,sztmpAmount);
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"CASH ADV");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usCashAdvCount);
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("CASH ADVS,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}
		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr3 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif		
		//offline sale
		memset(szStr, ' ', d_LINE_SIZE*2);
		//format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
			sprintf(szOffSaleTotalAmount,"%.0f",ulOffSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szOffSaleTotalAmount)/100);
                memset(szOffSaleTotalAmount, 0x00, sizeof(szOffSaleTotalAmount));
                strcpy(szOffSaleTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szOffSaleTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szOffSaleTotalAmount, sztmpAmount);
            }
                        
			
			sprintf(szStr,"OFFLINE%4d %s%17s", usOffSaleCount, strCST.szCurSymbol,sztmpAmount);
		}
		else
		{
			sprintf(szOffSaleTotalAmount,"%.0f",ulOffSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
			if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szOffSaleTotalAmount)/100);
                memset(szOffSaleTotalAmount, 0x00, sizeof(szOffSaleTotalAmount));
                strcpy(szOffSaleTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szOffSaleTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szOffSaleTotalAmount, sztmpAmount);
            }
                        
			//sprintf(szStr,"OFFLINE%4d %s%17s", usOffSaleCount, strCST.szCurSymbol,sztmpAmount);
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"OFFLINE");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usOffSaleCount);
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("OFFLINE,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}
		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);

		//thandar12Apr_remove Offline as pre CB
		//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		//refund
		memset(szStr, ' ', d_LINE_SIZE*2);
////format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
			sprintf(szRefundTotalAmount,"%.0f",ulRefundTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
			if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szRefundTotalAmount)/100);
                memset(szRefundTotalAmount, 0x00, sizeof(szRefundTotalAmount));
                strcpy(szRefundTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szRefundTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szRefundTotalAmount, sztmpAmount);
            }
                        
			sprintf(szStr,"REFUNDS%4d %s%17s", usRefundCount, strCST.szCurSymbol,sztmpAmount);
		}
		else
		{
			sprintf(szRefundTotalAmount,"%.0f",ulRefundTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
                        
			if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szRefundTotalAmount)/100);
                memset(szRefundTotalAmount, 0x00, sizeof(szRefundTotalAmount));
                strcpy(szRefundTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szRefundTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szRefundTotalAmount, sztmpAmount);
            }
                        
			//sprintf(szStr,"REFUNDS%4d %s%17s", usRefundCount, strCST.szCurSymbol,sztmpAmount);
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"REFUNDS");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usRefundCount);
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("REFUNDS,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}
		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr4 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

//CBPAY VOID
#if 0

	if(srTransRec.HDTid == CBPAY_HOST_INDEX)
	{
		memset(szStr, ' ', d_LINE_SIZE*2);
		memset(szVoidSaleTotalAmount,0x00,sizeof(szVoidSaleTotalAmount));
		////format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
			sprintf(szVoidSaleTotalAmount,"%.0f",ulVoidSaleTotalAmount);
			//sprintf(szVoidSaleTotalAmount,"%.0f",ulCBPayVoidSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
			//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szVoidSaleTotalAmount, sztmpAmount);

	        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
	            memset(szTempAmt, 0x00, sizeof(szTempAmt));
	            sprintf(szTempAmt, "%012.0f", atof(szVoidSaleTotalAmount)/100);
	            memset(szVoidSaleTotalAmount, 0x00, sizeof(szVoidSaleTotalAmount));
	            strcpy(szVoidSaleTotalAmount, szTempAmt);
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szVoidSaleTotalAmount, sztmpAmount);
	        } else {
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szVoidSaleTotalAmount, sztmpAmount);
	        }
			
			//sprintf(szStr,"VOID   %4d %s%17s", usCBPayVoidSaleCount,strCST.szCurSymbol, sztmpAmount);
			sprintf(szStr,"VOID   %4d %s%17s", usVoidSaleCount,strCST.szCurSymbol, sztmpAmount);
		}
		else
		{
			//sprintf(szVoidSaleTotalAmount,"%.0f",ulCBPayVoidSaleTotalAmount);
			sprintf(szVoidSaleTotalAmount,"%.0f",ulVoidSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
			//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szVoidSaleTotalAmount, sztmpAmount);


	        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
	            memset(szTempAmt, 0x00, sizeof(szTempAmt));
	            sprintf(szTempAmt, "%012.0f", atof(szVoidSaleTotalAmount)/100);
	            memset(szVoidSaleTotalAmount, 0x00, sizeof(szVoidSaleTotalAmount));
	            strcpy(szVoidSaleTotalAmount, szTempAmt);
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szVoidSaleTotalAmount, sztmpAmount);
	        } else {
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szVoidSaleTotalAmount, sztmpAmount);
	        }

			
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"VOID");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usVoidSaleCount);
			//sprintf(szTempBuf1,"%d",usCBPayVoidSaleCount);
			inTemp2 = strlen(szTempBuf1);
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
			inTemp3 = strlen(szTempBuf1);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			vdDebug_LogPrintf("VOID,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}                
                inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	}
#endif

                
                memset(szStr, ' ', d_LINE_SIZE*2);
//format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
			sprintf(szCUPSaleTotalAmount,"%.0f",ulCUPSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
			
	        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
	            memset(szTempAmt, 0x00, sizeof(szTempAmt));
	            sprintf(szTempAmt, "%012.0f", atof(szCUPSaleTotalAmount)/100);
	            memset(szCUPSaleTotalAmount, 0x00, sizeof(szCUPSaleTotalAmount));
	            strcpy(szCUPSaleTotalAmount, szTempAmt);
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPSaleTotalAmount, sztmpAmount);
	        } else {
	            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPSaleTotalAmount, sztmpAmount);
	        }
                        
			sprintf(szStr,"CUPSALE%4d %s%17s", usCUPSaleCount,strCST.szCurSymbol, sztmpAmount);
		}
		else
		{
			sprintf(szCUPSaleTotalAmount,"%.0f",ulCUPSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
			
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCUPSaleTotalAmount)/100);
                memset(szCUPSaleTotalAmount, 0x00, sizeof(szCUPSaleTotalAmount));
                strcpy(szCUPSaleTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPSaleTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPSaleTotalAmount, sztmpAmount);
            }
			//sprintf(szStr,"VOID   %4d %s%17s", usVoidSaleCount,strCST.szCurSymbol, sztmpAmount);
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"UPI SALE");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usCUPSaleCount);
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("UPI SALE,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}
                
		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		//CTOS_PrinterPutString("----------------------------------------------");
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("--------------------------------",&stgFONT_ATTRIB);

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr5 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                
                memset(szStr, ' ', d_LINE_SIZE*2);
//format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
			sprintf(szCUPPreAuthTotalAmount,"%.0f",ulCUPPreAuthTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
			
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
                memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
                strcpy(szCUPPreAuthTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
            }
                        
			sprintf(szStr,"PREAUTH%4d %s%17s", usCUPPreAuthCount,strCST.szCurSymbol, sztmpAmount);
		}
		else
		{
			sprintf(szCUPPreAuthTotalAmount,"%.0f",ulCUPPreAuthTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
			
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
                memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
                strcpy(szCUPPreAuthTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
            }
			//sprintf(szStr,"VOID   %4d %s%17s", usVoidSaleCount,strCST.szCurSymbol, sztmpAmount);
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"PRE AUTH");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usCUPPreAuthCount);
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
			//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
			//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("PRE AUTH,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}

        /*Alipay Sale*/
		usAlipaySaleCount=Totals.usAlipaySaleCount;
		ulAlipaySaleTotalAmount=Totals.ulAlipaySaleTotalAmount;
		
						memset(szStr, ' ', d_LINE_SIZE*2);
		//format amount 10+2
				if (strTCT.inFontFNTMode == 1)
				{
					sprintf(szCUPPreAuthTotalAmount,"%.0f",ulAlipaySaleTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						memset(szTempAmt, 0x00, sizeof(szTempAmt));
						sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
						memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
						strcpy(szCUPPreAuthTotalAmount, szTempAmt);
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
					} else {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
					}
								
					sprintf(szStr,"ALIPAY %4d %s%17s", usAlipaySaleCount,strCST.szCurSymbol, sztmpAmount);
				}
				else
				{
					sprintf(szCUPPreAuthTotalAmount,"%.0f",ulAlipaySaleTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						memset(szTempAmt, 0x00, sizeof(szTempAmt));
						sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
						memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
						strcpy(szCUPPreAuthTotalAmount, szTempAmt);
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
					} else {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
					}
					//sprintf(szStr,"VOID	%4d %s%17s", usVoidSaleCount,strCST.szCurSymbol, sztmpAmount);
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,"ALIPAY");
					inTemp1 = strlen(szTempBuf1);
					memcpy(szStr,szTempBuf1,inTemp1);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%d",usAlipaySaleCount);
					inTemp2 = strlen(szTempBuf1);
					memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
					//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
					inTemp3 = strlen(szTempBuf1);
					memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
					//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,sztmpAmount);
					inTemp4 = strlen(szTempBuf1);
					memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					
					vdDebug_LogPrintf("PRE AUTH,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
				}

		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		//CTOS_PrinterPutString("----------------------------------------------");
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("--------------------------------",&stgFONT_ATTRIB);

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr6 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);


//CBPAY
#if 1                
		memset(szStr, ' ', d_LINE_SIZE*2);
		memset(szCBPayTotalAmount,0x00,sizeof(szCBPayTotalAmount));
		
		//format amount 10+2
		if (strTCT.inFontFNTMode == 1)
		{
		
			sprintf(szCBPayTotalAmount,"%.0f",ulCBPaySaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));

            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCBPayTotalAmount)/100);
                memset(szCBPayTotalAmount, 0x00, sizeof(szCBPayTotalAmount));
                strcpy(szCBPayTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCBPayTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCBPayTotalAmount, sztmpAmount);
            }
			   // for Case 1595, 1600, 1603 , 1605                 	
			sprintf(szStr,"CBPay %5d %s%17s",usCBPaySaleCount, strCST.szCurSymbol,sztmpAmount);
		}
		else
		{	

	
			sprintf(szCBPayTotalAmount,"%.0f",ulCBPaySaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));

			#if 1 //original
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCBPayTotalAmount)/100);
                memset(szCBPayTotalAmount, 0x00, sizeof(szCBPayTotalAmount));
                strcpy(szCBPayTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCBPayTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCBPayTotalAmount, sztmpAmount);
            }
			#else
			memset(szTempAmt, 0x00, sizeof(szTempAmt));
			sprintf(szTempAmt, "%012.0f", atof(szCBPayTotalAmount)/100);
			memset(szCBPayTotalAmount, 0x00, sizeof(szCBPayTotalAmount));
			strcpy(szCBPayTotalAmount, szTempAmt);
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCBPayTotalAmount, sztmpAmount);			
            #endif
			
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"CBPay");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usCBPaySaleCount);
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol); //- original
			//strcpy(szTempBuf1, "MMK");
			
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("CBPay,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}


		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr7 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif		

#ifdef OK_DOLLAR_FEATURE
		vdDebug_LogPrintf("OK$ SETTLEMENT DETAIL [%d]", strTCT.inFontFNTMode);

		memset(szStr, ' ', d_LINE_SIZE*2);
		memset(szCBPayTotalAmount,0x00,sizeof(szCBPayTotalAmount));
		
		//format amount 10+2
		if(strTCT.inFontFNTMode == 1)
		{
		
			sprintf(szCBPayTotalAmount,"%.0f",ulOKDSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));

            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCBPayTotalAmount)/100);
                memset(szCBPayTotalAmount, 0x00, sizeof(szCBPayTotalAmount));
                strcpy(szCBPayTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCBPayTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCBPayTotalAmount, sztmpAmount);
            }
			                    
			sprintf(szStr,"OK$   %5d %s%17s",usOKDSaleCount, strCST.szCurSymbol,sztmpAmount); // adjust spacing for http://118.201.48.210:8080/redmine/issues/1525.92.1
			
			vdDebug_LogPrintf("OK$ SETTLEMENT DETAIL szStr : [%s]", szStr);
			
		}
		else
		{	

	
			sprintf(szCBPayTotalAmount,"%.0f",ulOKDSaleTotalAmount);
			memset (sztmpAmount, 0x00, sizeof(sztmpAmount));

			#if 1 //original
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szCBPayTotalAmount)/100);
                memset(szCBPayTotalAmount, 0x00, sizeof(szCBPayTotalAmount));
                strcpy(szCBPayTotalAmount, szTempAmt);
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCBPayTotalAmount, sztmpAmount);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCBPayTotalAmount, sztmpAmount);
            }
			#else
			memset(szTempAmt, 0x00, sizeof(szTempAmt));
			sprintf(szTempAmt, "%012.0f", atof(szCBPayTotalAmount)/100);
			memset(szCBPayTotalAmount, 0x00, sizeof(szCBPayTotalAmount));
			strcpy(szCBPayTotalAmount, szTempAmt);
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCBPayTotalAmount, sztmpAmount);			
            #endif
			
			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,"OK$");
			inTemp1 = strlen(szTempBuf1);
			memcpy(szStr,szTempBuf1,inTemp1);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%d",usOKDSaleCount);
			inTemp2 = strlen(szTempBuf1);
			memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			sprintf(szTempBuf1,"%s",strCST.szCurSymbol); //- original
			//strcpy(szTempBuf1, "MMK");
			
			inTemp3 = strlen(szTempBuf1);
			memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);

			memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
			strcpy(szTempBuf1,sztmpAmount);
			inTemp4 = strlen(szTempBuf1);
			memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
			
			vdDebug_LogPrintf("OK$,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
		}

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr8 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif		


#ifdef TOPUP_RELOAD
        /*Topup*/
		usTopupCount=Totals.usTopupCount;
		ulTopupTotalAmount=Totals.ulTopupTotalAmount;
		
						memset(szStr, ' ', d_LINE_SIZE*2);
		//format amount 10+2
				if (strTCT.inFontFNTMode == 1)
				{
					sprintf(szCUPPreAuthTotalAmount,"%.0f",ulTopupTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						memset(szTempAmt, 0x00, sizeof(szTempAmt));
						sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
						memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
						strcpy(szCUPPreAuthTotalAmount, szTempAmt);
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
					} else {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
					}
								
					sprintf(szStr,"TOPUP %5d %s%17s", usTopupCount,strCST.szCurSymbol, sztmpAmount);
				}
				else
				{
					sprintf(szCUPPreAuthTotalAmount,"%.0f",ulTopupTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						memset(szTempAmt, 0x00, sizeof(szTempAmt));
						sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
						memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
						strcpy(szCUPPreAuthTotalAmount, szTempAmt);
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
					} else {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
					}
					//sprintf(szStr,"VOID	%4d %s%17s", usVoidSaleCount,strCST.szCurSymbol, sztmpAmount);
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,"TOPUP");
					inTemp1 = strlen(szTempBuf1);
					memcpy(szStr,szTempBuf1,inTemp1);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%d",usTopupCount);
					inTemp2 = strlen(szTempBuf1);
					memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
					//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
					inTemp3 = strlen(szTempBuf1);
					memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
					//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,sztmpAmount);
					inTemp4 = strlen(szTempBuf1);
					memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					
					vdDebug_LogPrintf("PRE AUTH,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
				}

		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		//CTOS_PrinterPutString("----------------------------------------------");
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("--------------------------------",&stgFONT_ATTRIB);

		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr9 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

        /*Repayment*/
		usRepaymentCount=Totals.usRepaymentCount;
		ulRepaymentTotalAmount=Totals.ulRepaymentTotalAmount;
		
						memset(szStr, ' ', d_LINE_SIZE*2);
		//format amount 10+2
				if (strTCT.inFontFNTMode == 1)
				{
					sprintf(szCUPPreAuthTotalAmount,"%.0f",ulRepaymentTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						memset(szTempAmt, 0x00, sizeof(szTempAmt));
						sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
						memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
						strcpy(szCUPPreAuthTotalAmount, szTempAmt);
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
					} else {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
					}
								
					sprintf(szStr,"REPAYMENT %1d %s%17s", usRepaymentCount,strCST.szCurSymbol, sztmpAmount);
				}
				else
				{
					sprintf(szCUPPreAuthTotalAmount,"%.0f",ulRepaymentTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						memset(szTempAmt, 0x00, sizeof(szTempAmt));
						sprintf(szTempAmt, "%012.0f", atof(szCUPPreAuthTotalAmount)/100);
						memset(szCUPPreAuthTotalAmount, 0x00, sizeof(szCUPPreAuthTotalAmount));
						strcpy(szCUPPreAuthTotalAmount, szTempAmt);
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szCUPPreAuthTotalAmount, sztmpAmount);
					} else {
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szCUPPreAuthTotalAmount, sztmpAmount);
					}
					//sprintf(szStr,"VOID	%4d %s%17s", usVoidSaleCount,strCST.szCurSymbol, sztmpAmount);
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,"REPAYMENT");
					inTemp1 = strlen(szTempBuf1);
					memcpy(szStr,szTempBuf1,inTemp1);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%d",usRepaymentCount);
					inTemp2 = strlen(szTempBuf1);
					memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
					//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
					inTemp3 = strlen(szTempBuf1);
					memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
					//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);
		
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,sztmpAmount);
					inTemp4 = strlen(szTempBuf1);
					memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					
					vdDebug_LogPrintf("REPAYMENT,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
				}

		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
		//CTOS_PrinterPutString("----------------------------------------------");
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("--------------------------------",&stgFONT_ATTRIB);


		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr10 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);		
#endif
		
                
		inCCTOS_PrinterBufferOutput("--------------------------------",&stgFONT_ATTRIB,1);
		
//		usTotalCount = usSaleCount + usOffSaleCount + usRefundCount + usCashAdvCount;
		usTotalCount = usSaleCount + usOffSaleCount + usRefundCount + usCashAdvCount+usCUPSaleCount+usCUPPreAuthCount+usTopupCount+usRepaymentCount+usCBPaySaleCount+usOKDSaleCount;

		memset(szStr, ' ', d_LINE_SIZE*2);
		memset(szTemp, ' ', d_LINE_SIZE);

		if (inReportType == PRINT_HOST_TOTAL)
		{
//			if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount + ulCashAdvTotalAmount))
                        if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount + ulCashAdvTotalAmount+ulCUPSaleTotalAmount+ulTopupTotalAmount+ulRepaymentTotalAmount+ulCBPaySaleTotalAmount+ulOKDSaleTotalAmount))
			{
//				ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount);
                         ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount+ulCUPSaleTotalAmount+ulTopupTotalAmount+ulRepaymentTotalAmount+ulCBPaySaleTotalAmount+ulOKDSaleTotalAmount);
//format amount 10+2
				if (strTCT.inFontFNTMode == 1)
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
					
                                        sprintf(szTemp,"-%s",sztmpAmount);
					sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount,strCST.szCurSymbol, szTemp);
				}
				else
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
                                        
					sprintf(szTemp,"-%s",sztmpAmount);
					//sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount,strCST.szCurSymbol, szTemp);
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,"TOTALS");
					inTemp1 = strlen(szTempBuf1);
					memcpy(szStr,szTempBuf1,inTemp1);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%d",usTotalCount);
					inTemp2 = strlen(szTempBuf1);
					memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
					//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
					inTemp3 = strlen(szTempBuf1);
					memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
					//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,szTemp);
					inTemp4 = strlen(szTempBuf1);
					memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					
					vdDebug_LogPrintf("HOST_TOTAL,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
				}
			}
			else
			{
//				ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount) - ulRefundTotalAmount;
				
               	ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount+ulCUPSaleTotalAmount+ulCUPPreAuthTotalAmount+ulTopupTotalAmount+ulRepaymentTotalAmount+ulCBPaySaleTotalAmount+ulOKDSaleTotalAmount) - ulRefundTotalAmount;
//format amount 10+2
				if (strTCT.inFontFNTMode == 1)
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
                                        
					sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount,strCST.szCurSymbol, sztmpAmount);
				}
				else
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
                                        
					//sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount,strCST.szCurSymbol, sztmpAmount);
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,"TOTALS");
					inTemp1 = strlen(szTempBuf1);
					memcpy(szStr,szTempBuf1,inTemp1);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%d",usTotalCount);
					inTemp2 = strlen(szTempBuf1);
					memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
					//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
					inTemp3 = strlen(szTempBuf1);
					memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
					//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,sztmpAmount);
					inTemp4 = strlen(szTempBuf1);
					memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					
					vdDebug_LogPrintf("HOST_TOTAL22,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
				}
			}
		}
		else if(inReportType == PRINT_CARD_TOTAL)
		{
//			if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount))
                        if(ulRefundTotalAmount > (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount+ulCUPSaleTotalAmount+ulCUPPreAuthTotalAmount+ulTopupTotalAmount+ulRepaymentTotalAmount+ulCBPaySaleTotalAmount+ulOKDSaleTotalAmount))
			{
//				ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount);
                                ulTotalAmount = ulRefundTotalAmount - (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount+ulCUPSaleTotalAmount+ulCUPPreAuthTotalAmount+ulTopupTotalAmount+ulRepaymentTotalAmount+ulCBPaySaleTotalAmount+ulOKDSaleTotalAmount);
//format amount 10+2
				if (strTCT.inFontFNTMode == 1)
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
                                        
					sprintf(szTemp,"-%s",sztmpAmount);
					sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount, strCST.szCurSymbol,szTemp);
				}
				else
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
                                        
					sprintf(szTemp,"-%s",sztmpAmount);
					//sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount, strCST.szCurSymbol,szTemp);
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,"TOTALS");
					inTemp1 = strlen(szTempBuf1);
					memcpy(szStr,szTempBuf1,inTemp1);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%d",usTotalCount);
					inTemp2 = strlen(szTempBuf1);
					memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);					
					//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
					inTemp3 = strlen(szTempBuf1);
					memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
					//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,szTemp);
					inTemp4 = strlen(szTempBuf1);
					memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					
					vdDebug_LogPrintf("CARD_TOTAL,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
				}
			}
			else
			{
//				ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount) - ulRefundTotalAmount;
                ulTotalAmount = (ulSaleTotalAmount+ulOffSaleTotalAmount+ulCashAdvTotalAmount+ulCUPSaleTotalAmount+ulCUPPreAuthTotalAmount+ulTopupTotalAmount+ulRepaymentTotalAmount+ulCBPaySaleTotalAmount+ulOKDSaleTotalAmount) - ulRefundTotalAmount;

//format amount 10+2
				if (strTCT.inFontFNTMode == 1)
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
                                        
					sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount, strCST.szCurSymbol,sztmpAmount);
				}
				else
				{
					sprintf(szTotalAmount,"%.0f",ulTotalAmount);
					memset (sztmpAmount, 0x00, sizeof(sztmpAmount));
					
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTotalAmount)/100);
                    memset(szTotalAmount, 0x00, sizeof(szTotalAmount));
                    strcpy(szTotalAmount, szTempAmt);
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmount, sztmpAmount);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmount, sztmpAmount);
                    }
                                        
					//sprintf(szStr,"TOTALS %4d %s%17s", usTotalCount, strCST.szCurSymbol,sztmpAmount);
					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,"TOTALS");
					inTemp1 = strlen(szTempBuf1);
					memcpy(szStr,szTempBuf1,inTemp1);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%d",usTotalCount);
					inTemp2 = strlen(szTempBuf1);
					memcpy(&szStr[(11-inTemp1-inTemp2)+inTemp1],szTempBuf1,inTemp2);
					//memcpy(&szStr[(11-inTemp1-inTemp2)*2+inTemp1],szTempBuf1,inTemp2);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					sprintf(szTempBuf1,"%s",strCST.szCurSymbol);
					inTemp3 = strlen(szTempBuf1);
					memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)+inTemp1+inTemp2],szTempBuf1,inTemp3);
					//memcpy(&szStr[(15-inTemp1-inTemp2-inTemp3)*2+inTemp1+inTemp2],szTempBuf1,inTemp3);

					memset (szTempBuf1, 0x00, sizeof(szTempBuf1));
					strcpy(szTempBuf1,sztmpAmount);
					inTemp4 = strlen(szTempBuf1);
					memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
					//memcpy(&szStr[(32-inTemp1-inTemp2-inTemp3-inTemp4)*2+inTemp1+inTemp2+inTemp3],szTempBuf1,inTemp4);
						
					vdDebug_LogPrintf("CARD_TOTAL11,inTemp1=[%d],inTemp2=[%d],inTemp3=[%d],inTemp4=[%d]",inTemp1,inTemp2,inTemp3,inTemp4);
				}
			}	 
		}
		//memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);


		
		vdDebug_LogPrintf("vdCTOS_PrintAccumeByHostAndCard szStr11 [%d][%s]", strlen(szStr), szStr);
		szStr[d_LINE_SIZE] = 0x00;// fix for sqa issue #10046 - *MP200* 	Garbage characters on settlement receipt Note: Also existing on Old host 
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
}

int inCTOS_SelectFont(int inFontMode,int inFontSize ,int inFontStyle,char * szFontName)
{
	char sztmpFontName[50+1];
	if(inFontMode == d_FONT_TTF_MODE)
	{
		memset(sztmpFontName,0x00,sizeof(sztmpFontName));
		CTOS_PrinterFontSelectMode(d_FONT_TTF_MODE);	//set the printer with TTF Mode
		//if (strTCT.inThemesType == 0)
//			strcpy(sztmpFontName, d_FONT_DEFAULT_TTF);
		//if (strTCT.inThemesType == 1)
		//	strcpy(sztmpFontName, "tahoma.ttf");
		//if (strTCT.inThemesType == 2)
		//	strcpy(sztmpFontName, "cousinebd.ttf");
		//if (strTCT.inThemesType == 4)
		//	strcpy(sztmpFontName, "tahomabd.ttf");
//		//0-Regular ,1-Italic ,2-Bold ,3-Bold italic
//		CTOS_PrinterTTFSelect(sztmpFontName, inFontStyle);
//		inCTOSS_SetERMFontType(sztmpFontName, inFontStyle);
		CTOS_PrinterTTFSelect(PRNT_FONT_FILE, inFontStyle);//tanJing

	}
	else
	{
	
		CTOS_PrinterFontSelectMode(d_FONT_FNT_MODE);	//set the printer with default Mode
		CTOS_LanguagePrinterFontSize(inFontSize, inFontStyle, TRUE);
	}
	return d_OK;
	
}

int inCTOSS_PrintERMInfo(void)
{
	char pszACCUMFileName[128];
	TRANS_TOTAL_ERM srErmRec;
	ACCUM_REC srAccumRec;
	int inResult;
	int inTranCardType = 0;

	memset(pszACCUMFileName,0x00,sizeof(pszACCUMFileName));
	memset(&srErmRec,0x00,sizeof(TRANS_TOTAL_ERM));
	sprintf(pszACCUMFileName, "ACC%02d%02d.total", strHDT.inHostIndex , srTransRec.MITid);
	
	vdDebug_LogPrintf("--pszACCUMFileName[%s]", pszACCUMFileName);
	usCTOSS_Erm_GetERMInfo(pszACCUMFileName,&srErmRec);

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }        
    vdDebug_LogPrintf("HostTotal SaleCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal OfflCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount);
    vdDebug_LogPrintf("HostTotal RefdCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount);            
    vdDebug_LogPrintf("HostTotal VoidCount[%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount);    
    vdDebug_LogPrintf("HostTotal TipCount [%d] [%12.0f]", srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usTipCount, srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulTipTotalAmount);
 
}


int inCTOS_PrintSettleReport(void)
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult;
	int inTranCardType;
	int inReportType;
	int i;		
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];
        char szExchangeRate[d_LINE_SIZE+1];
        
        int inTotalPrint;

		vdDebug_LogPrintf("inCTOS_PrintSettleReport [%d][%d]", srTransRec.HDTid, fIPPGetSettleFlag());

	inCTLOS_Updatepowrfail(PFR_BATCH_SETTLEMENT_PRINT);
	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);

	if( printCheckPaper()==-1)
		return;

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return;
	}
    
	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }
    
    if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);
	//start capture receipt
    //inInitializePrinterBufferLib();
	//inSIGPStartCapturePrinterDataLib();


	ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("SETTLEMENT");
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

    printHostLabel();

	#ifdef CBPAY_DV
		if(srTransRec.HDTid == CBPAY_HOST_INDEX)
		{
			printTIDMIDCBPay();
						
			// fix for case #1346 122319
			//inCSTRead(1);  
						
		}				
		else if(srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
			printTIDMIDOKD();	
		else
			printTIDMID();
		
	#else
				printTIDMID();
	#endif

	
    printDateTime();

    printBatchNO();
    
    if(srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23
    #ifdef TOPUP_RELOAD
    || srTransRec.HDTid == 20
    #endif
	){
        inTranCardType = 1;
        inTotalPrint = 2;       							
									
        if(srTransRec.HDTid == 17 || srTransRec.HDTid == 23)
            inCSTRead(1);
    } else {
        inTranCardType = 0;
        inTotalPrint = 1;
    }

	
    vdDebug_LogPrintf("inTranCardType[%d]", inTranCardType);

	if(TRUE == fGetCashAdvAppFlag())
	{
		inTranCardType = 0;
		inTotalPrint = 1;
	}

	
    vdDebug_LogPrintf("inTranCardType inTotalPrint A: [%d][%d]", inTranCardType, inTotalPrint);

    if (strTCT.fMustAutoSettle == TRUE) {
        if (srTransRec.HDTid == 17 || srTransRec.HDTid == 18 || srTransRec.HDTid == 19 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23)//srTransRec.HDTid == 13) 
		{
            inTranCardType = 1;
            inTotalPrint = 2;

			if(srTransRec.HDTid == 17 || srTransRec.HDTid == 23)
               inCSTRead(1);
        } else {
            inTranCardType = 0;
            inTotalPrint = 1;
        }
    }

	//IPP currency should always be MMK //http://118.201.48.210:8080/redmine/issues/1525.115
	if(srTransRec.HDTid == 6)
		inCSTRead(1);

    vdDebug_LogPrintf("inTranCardType inTotalPrint B: [%d][%d][%d]", inTranCardType, inTotalPrint, fIPPGetSettleFlag());

    //CTOS_PrinterFline(d_LINE_DOT * 1);
	//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
//	for(inTranCardType = 0; inTranCardType < 1 ;inTranCardType ++)
        for(inTranCardType; inTranCardType < inTotalPrint ;inTranCardType ++)
	{
		inReportType = PRINT_CARD_TOTAL;

		
		vdDebug_LogPrintf("inReportType[%d]", inReportType);
		
		if(inReportType == PRINT_CARD_TOTAL)
		{

			vdDebug_LogPrintf("inReportType[%d]", inReportType);

			for(i= 0; i <20; i ++ )
			{
				//vdDebug_LogPrintf("XXXXX--Count[%d][%d]", i, srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPayVoidSaleCount);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0)
                &&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount == 0))
				{
					if(TRUE == fGetCashAdvAppFlag())
					{
		                 if(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount == 0)					 	
						    continue;
					}
					else
					{
					        continue;
					}

				}


			
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,strIIT.szIssuerLabel);
				
				vdDebug_LogPrintf("szStr[%s]", szStr);
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),
                                
								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),
	
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"TOTAL:");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				
			

				if(TRUE == fGetCashAdvAppFlag())
				{

					vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);	
				}
				else
				{
					vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCashAdvTotalAmount),
                                
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulCBPaySaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOKDSaleTotalAmount),


								srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		

				}
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
		}
	}
        
        if(strTCT.inExchangeRate > 0){
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
//            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
            memset(szStr, 0x00, sizeof(szStr));
            memset(szExchangeRate, 0x00, sizeof(szExchangeRate));
            sprintf(szExchangeRate, "%d", strTCT.inExchangeRate);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szExchangeRate, szStr);
            
            memset(szExchangeRate, 0x00, sizeof(szExchangeRate));
            sprintf(szExchangeRate, "Ex_Rate USD 1 = MMK %s", szStr);
            
//            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
            
            inCCTOS_PrinterBufferOutput(szExchangeRate,&stgFONT_ATTRIB,1);
            vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
            
            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
        }
    
	//print space one line
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//inCTOSS_PrintERMInfo();
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        vdPrintCenter("SETTLEMENT SUCCESSFUL");
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();

//end capture receipt
	//inSIGPEndCapturePrinterDataLib();
	inCTOSS_ERM_Form_Receipt(1);
	
	return d_OK;	
}

USHORT ushCTOS_printBatchRecordHeader(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];

    ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("DETAIL REPORT");
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);

    printHostLabel();
    
	printTIDMID();
    
	printDateTime();
    
	printBatchNO();
    
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);
	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,"CARD NUMBER          CARD NAME", &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "EXP DATE             INVOICE NUM", &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "TRANS TYPE           AMOUNT", &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	//memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, "APPROVAL CODE        ENTRY MODE", &stgFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	if (strTCT.inFontFNTMode == 1)
	{
//		inCCTOS_PrinterBufferOutput("CARD NUMBER          CARD NAME",&stgFONT_ATTRIB,1);
//		inCCTOS_PrinterBufferOutput("EXP DATE             INVOICE NUM",&stgFONT_ATTRIB,1);
//		inCCTOS_PrinterBufferOutput("TRANS TYPE           AMOUNT",&stgFONT_ATTRIB,1);
//		inCCTOS_PrinterBufferOutput("APPROVAL CODE        ENTRY MODE",&stgFONT_ATTRIB,1);
                
                inCCTOS_PrinterBufferOutput("CARD NAME            CARD NUMBER",&stgFONT_ATTRIB,1);
		//inCCTOS_PrinterBufferOutput("EXP DATE             TRACE NO.",&stgFONT_ATTRIB,1);
//                inCCTOS_PrinterBufferOutput("EXP DATE             REF NO.",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("EXP DATE			  INV NO.",&stgFONT_ATTRIB,1);//ENHANCEMENT_1861
		inCCTOS_PrinterBufferOutput("TRANSACTION          AMOUNT",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("APPROVAL CODE        DATE TIME",&stgFONT_ATTRIB,1);
		if (MPU_DEBIT_HOST_IDX == srTransRec.HDTid || MPU_JCBI_HOST_IDX == srTransRec.HDTid || MPU_CUP_HOST_IDX == srTransRec.HDTid)
			inCCTOS_PrinterBufferOutput("REF NO.       ",&stgFONT_ATTRIB,1);
	}
	else
	{
//		inCCTOS_PrinterBufferOutput("CARD NUMBER                    CARD NAME",&stgFONT_ATTRIB,1);
//		inCCTOS_PrinterBufferOutput("EXP DATE                          INVOICE NUM",&stgFONT_ATTRIB,1);
//		inCCTOS_PrinterBufferOutput("TRANS TYPE                      AMOUNT",&stgFONT_ATTRIB,1);
//		inCCTOS_PrinterBufferOutput("APPROVAL CODE                ENTRY MODE",&stgFONT_ATTRIB,1);
            
                inCCTOS_PrinterBufferOutput("CARD NAME            CARD NUMBER",&stgFONT_ATTRIB,1);
//		inCCTOS_PrinterBufferOutput("EXP DATE             TRACE NO.",&stgFONT_ATTRIB,1);
                //inCCTOS_PrinterBufferOutput("EXP DATE             REF NO.",&stgFONT_ATTRIB,1);
                inCCTOS_PrinterBufferOutput("EXP DATE             INV NO.",&stgFONT_ATTRIB,1);//ENHANCEMENT_1861
		inCCTOS_PrinterBufferOutput("TRANSACTION          AMOUNT",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("APPROVAL CODE        DATE TIME",&stgFONT_ATTRIB,1);
		if (MPU_DEBIT_HOST_IDX == srTransRec.HDTid || MPU_JCBI_HOST_IDX == srTransRec.HDTid || MPU_CUP_HOST_IDX == srTransRec.HDTid)
			inCCTOS_PrinterBufferOutput("REF NO.       ",&stgFONT_ATTRIB,1);
	}

	printDividingLine(DIVIDING_LINE_DOUBLE);
	return d_OK;
}



USHORT ushCTOS_printBatchRecordHeaderCBPay(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];

    ushCTOS_PrintHeader(0);	
    
  	  vdPrintTitleCenter("DETAIL REPORT");
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);

    	printHostLabel();
    
	printTIDMIDCBPay();
    
	printDateTime();
    
	printBatchNO();
    

	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);

	if (strTCT.inFontFNTMode == 1)
	{


		// for case http://118.201.48.210:8080/redmine/issues/1525.34.1
 		//inCCTOS_PrinterBufferOutput("TRACE NO.       ",&stgFONT_ATTRIB,1);
 		inCCTOS_PrinterBufferOutput("INV NO.       ",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("TRANSACTION            AMOUNT",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("REF NO",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("DATE TIME",&stgFONT_ATTRIB,1);

	}
	else
	{
		inCCTOS_PrinterBufferOutput("INV NO.       ",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("TRANSACTION            AMOUNT",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("REF NO",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("DATE TIME",&stgFONT_ATTRIB,1);

	}

	printDividingLine(DIVIDING_LINE_DOUBLE);
	return d_OK;
}

USHORT ushCTOS_printBatchRecordFooter(void)
{
	BYTE baTemp[PAPER_X_SIZE * 64];		
	char szStr[d_LINE_SIZE + 1];
	char szPrintBuf[d_LINE_SIZE*2 + 1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	char szTempBuf3[d_LINE_SIZE + 1];

	int i;
	int inTemp1,inTemp2;
	USHORT usCharPerLine = 32;
        int inPosLen = 0;
        char szTempAmt[AMT_ASC_SIZE+1];
        
        
	
    usCharPerLine = inGetFontlen(&stgFONT_ATTRIB);


	DebugAddSTR("ushCTOS_printBatchRecordFooter", srTransRec.szPAN, 10);
	memset(szTempBuf1,0,sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	strcpy(szTempBuf1,srTransRec.szPAN);    
    inIITRead(srTransRec.IITid);
	strcpy(szTempBuf2,strIIT.szIssuerLabel);
//	cardMasking(szTempBuf1, PRINT_CARD_MASKING_1);
        cardMasking(szTempBuf1, PRINT_CARD_MASKING_4);
        memset(szTempBuf3, 0x00, sizeof(szTempBuf3));
//        wub_hex_2_str(srTransRec.szDate, szTempBuf3, DATE_BCD_SIZE);
//        vdDebug_LogPrintf("szDate [%s]", szTempBuf3);
        
        
//        if(srTransRec.inCardType == DEBIT_CARD){
//            strcpy(szTempBuf3, "D");
//        } else if(srTransRec.inCardType == CREDIT_CARD){
//            strcpy(szTempBuf3, "C");
//        }
        
        if(srTransRec.byEntryMode == CARD_ENTRY_EASY_ICC || srTransRec.byEntryMode == CARD_ENTRY_ICC){
            strcpy(szTempBuf3, "C");
        } else if (srTransRec.byEntryMode == CARD_ENTRY_MSR){
            strcpy(szTempBuf3, "S");
        } else if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL) {
            strcpy(szTempBuf3, "M");
        }
        
        memset(szPrintBuf, 0x00, sizeof(szPrintBuf));
        sprintf(szPrintBuf, "%-*s%-*s%-*s", 11, szTempBuf2, 19, szTempBuf1, d_LINE_SIZE/3, szTempBuf3);
        inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
        
        memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        wub_hex_2_str(srTransRec.szInvoiceNo, szTempBuf1, INVOICE_BCD_SIZE);
//        sprintf(szTempBuf1, "%06lu", srTransRec.ulTraceNum);
//        wub_hex_2_str(srTransRec.szRRN, szTempBuf1, RRN_BYTES);
        //sprintf(szTempBuf1, "%s", srTransRec.szRRN);
        sprintf(szPrintBuf, "%-*s%-*s%-*s", 11, "**/**", 19, szTempBuf1, d_LINE_SIZE/3, " ");
        inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
        
        memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        szGetTransTitle(srTransRec.byTransType, szTempBuf1);
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf2, AMT_BCD_SIZE);
        memset(szTempBuf3, 0x00, sizeof(szTempBuf3));
        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
            memset(szTempAmt, 0x00, sizeof(szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(szTempBuf2)/100);
            memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
            strcpy(szTempBuf2, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTempBuf2, szTempBuf3);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTempBuf2, szTempBuf3);
        }
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        
        if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH){
            sprintf(szTempBuf2, "-%s ", strCST.szCurSymbol);
        } else{
            sprintf(szTempBuf2, "%s ", strCST.szCurSymbol);
        }
        
        strcat(szTempBuf2, szTempBuf3);
		
		if(strlen(szTempBuf1) > 10)
			sprintf(szPrintBuf, "%-*s%-*s%-*s", strlen(szTempBuf1)+1, szTempBuf1, 19, szTempBuf2, d_LINE_SIZE/3, " ");
		else
			sprintf(szPrintBuf, "%-*s%-*s%-*s", 11, szTempBuf1, 19, szTempBuf2, d_LINE_SIZE/3, " ");
        inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
        
        memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        memcpy(szTempBuf1, srTransRec.szAuthCode, AUTH_CODE_DIGITS);
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szDate, szTempBuf2, DATE_BCD_SIZE);
		
        memset(szTempBuf3, 0x00, sizeof(szTempBuf3));
        memcpy(szTempBuf3, szTempBuf2+2, 2);                                    inPosLen += 2;
        strcat(szTempBuf3, "/");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                             inPosLen += 2;
        strcat(szTempBuf3, "/");                                                inPosLen += 1;
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szYear, szTempBuf2, 2);
        memcpy(szTempBuf3+inPosLen, szTempBuf2+2, 2);                           inPosLen += 2;
        strcat(szTempBuf3, " ");                                                inPosLen += 1;
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szTime, szTempBuf2, 3);
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                             inPosLen += 2;
        strcat(szTempBuf3, ":");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2+2, 2);                           inPosLen += 2;
        strcat(szTempBuf3, ":");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2+4, 2);                            inPosLen += 2;
        
        sprintf(szPrintBuf, "%-*s%-*s", 11, szTempBuf1, d_LINE_SIZE/3, szTempBuf3);
        inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);

		if (MPU_DEBIT_HOST_IDX == srTransRec.HDTid || MPU_JCBI_HOST_IDX == srTransRec.HDTid || MPU_CUP_HOST_IDX == srTransRec.HDTid)
		{
            memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
            sprintf(szTempBuf1, "%s", srTransRec.szRRN);
            sprintf(szPrintBuf, "%-*s", 10, szTempBuf1);
            inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
		}
        
        inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);
        
        

//	if (strTCT.inFontFNTMode == 1)
//	{
//	    memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
////	    strcpy(szPrintBuf, szTempBuf1);
////	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 21-strlen(szTempBuf1));
////	    strcat(szPrintBuf, szTempBuf2);
//            sprintf(szPrintBuf, "%-*s%-*s%*s", d_LINE_SIZE/3, szTempBuf1, d_LINE_SIZE/3, szTempBuf2, d_LINE_SIZE/3, szTempBuf3);
//	}
//	else
//	{
//	    memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
////		inTemp1 = strlen(szTempBuf1);
////		inTemp2 = strlen(szTempBuf2);
////	    strcpy(szPrintBuf, szTempBuf1);
////	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, (usCharPerLine-inTemp1-inTemp2)*2);
////	    strcat(szPrintBuf, szTempBuf2);
//            
//            sprintf(szPrintBuf, "%-*s%-*s%-*s", d_LINE_SIZE/3, szTempBuf1, d_LINE_SIZE/3, szTempBuf2, d_LINE_SIZE/3, szTempBuf3);
//	}
//	//memset (baTemp, 0x00, sizeof(baTemp));
//	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
//	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
//	
//
//	//Exp date and inv num
//	memset(szTempBuf1, 0, sizeof(szTempBuf1));
//	memset(szTempBuf2, 0, sizeof(szTempBuf2));
//	memset(szTempBuf3, 0, sizeof(szTempBuf3));
//	
//	wub_hex_2_str(srTransRec.szExpireDate, szTempBuf1,EXPIRY_DATE_BCD_SIZE);
//	DebugAddSTR("detail--EXP",szTempBuf1,12);  
//	
//	for (i =0; i<4;i++)
//		szTempBuf1[i] = '*';
//	memcpy(szTempBuf2,&szTempBuf1[0],2);
//	memcpy(szTempBuf3,&szTempBuf1[2],2);
//
//	memset(szStr, 0,sizeof(szStr));
//	wub_hex_2_str(srTransRec.szInvoiceNo, szStr, INVOICE_BCD_SIZE);
//
//	if (strTCT.inFontFNTMode == 1)
//	{
//		memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
//	    strcpy(szPrintBuf, szTempBuf2);
//	    strcat(szPrintBuf, szTempBuf3);
//	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 21-strlen(szTempBuf2)-strlen(szTempBuf3));
//    	strcat(szPrintBuf, szStr);
//	}
//	else
//	{
//		memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
//		inTemp1 = strlen(szTempBuf1);
//		inTemp2 = strlen(szStr);
//	    strcpy(szPrintBuf, szTempBuf1);
//	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, (usCharPerLine-inTemp1-inTemp2)*2);
//	    strcat(szPrintBuf, szStr);
//	}
//	//memset (baTemp, 0x00, sizeof(baTemp));
//	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
//	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
//
//	memset(szStr, 0,sizeof(szStr));
//	memset(szTempBuf1, 0,sizeof(szTempBuf1));
//	memset(szTempBuf2, 0,sizeof(szTempBuf2));
//	szGetTransTitle(srTransRec.byTransType, szStr);
//	wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf1, AMT_BCD_SIZE);
//	//format amount 10+2
//	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTempBuf1, szTempBuf3);
//	//sprintf(szTempBuf2, "%10lu.%02lu", atol(szTempBuf1)/100, atol(szTempBuf1)%100);
//	sprintf(szTempBuf2,"%s",szTempBuf3);
//	if (strTCT.inFontFNTMode == 1)
//	{
//		memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
//	    strcpy(szPrintBuf, szStr);
//	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 14-strlen(szStr));
//	    strcat(szPrintBuf, szTempBuf2);
//	}
//	else
//	{
//		memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
//		inTemp1 = strlen(szTempBuf2);
//		inTemp2 = strlen(szStr);
//	    strcpy(szPrintBuf, szStr);
//	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, (usCharPerLine-inTemp1-inTemp2)*2);
//	    strcat(szPrintBuf, szTempBuf2);
//	}
//	//memset (baTemp, 0x00, sizeof(baTemp));
//	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
//	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
//
//	memset(szTempBuf2, 0,sizeof(szTempBuf2));
//	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
//		memcpy(szTempBuf2,"Chip",4);
//	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
//		memcpy(szTempBuf2,"Manual",6);
//	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
//		memcpy(szTempBuf2,"MSR",3);
//	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
//		memcpy(szTempBuf2,"Fallback",8);
//	else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
//	{
//		if ('4' ==srTransRec.szPAN[0])
//			memcpy(szTempBuf2,"PayWave",7);
//		if ('5' ==srTransRec.szPAN[0])
//			memcpy(szTempBuf2,"PayPass",7);
//		if ('3' ==srTransRec.szPAN[0])
//			memcpy(szTempBuf2,"ExpressPay",10);
//		if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
//			memcpy(szTempBuf2,"J/Speedy",8);
//	}
//	
//	if (strTCT.inFontFNTMode == 1)
//	{
//	    memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
//	    strcpy(szPrintBuf, srTransRec.szAuthCode);
//	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, 21-strlen(srTransRec.szAuthCode));
//	    strcat(szPrintBuf, szTempBuf2);
//	}
//	else
//	{
//		memset (szPrintBuf, 0x00, sizeof(szPrintBuf));
//		inTemp1 = strlen(szTempBuf2);
//		inTemp2 = strlen(srTransRec.szAuthCode);
//	    strcpy(szPrintBuf, srTransRec.szAuthCode);
//	    memset(&szPrintBuf[strlen(szPrintBuf)], 0x20, (usCharPerLine-inTemp1-inTemp2)*2);
//	    strcat(szPrintBuf, szTempBuf2);
//	}
//	//memset (baTemp, 0x00, sizeof(baTemp));
//	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1,szPrintBuf , &stgFONT_ATTRIB);
//	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
//	
//	
//	//CTOS_PrinterFline(d_LINE_DOT * 1);
//	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);

	
}




USHORT ushCTOS_printBatchRecordFooterCBPay(void)
{
	BYTE baTemp[PAPER_X_SIZE * 64];		
	char szStr[d_LINE_SIZE + 1];
	char szPrintBuf[d_LINE_SIZE*2 + 1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	char szTempBuf3[d_LINE_SIZE + 1];

	int i;
	int inTemp1,inTemp2;
	USHORT usCharPerLine = 32;
        int inPosLen = 0;
        char szTempAmt[AMT_ASC_SIZE+1];
        
        
	
    usCharPerLine = inGetFontlen(&stgFONT_ATTRIB);


	DebugAddSTR("ushCTOS_printBatchRecordFooter", srTransRec.szPAN, 10);
	memset(szTempBuf1,0,sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	strcpy(szTempBuf1,srTransRec.szPAN);    
    inIITRead(srTransRec.IITid);
	strcpy(szTempBuf2,strIIT.szIssuerLabel);
//	cardMasking(szTempBuf1, PRINT_CARD_MASKING_1);
        cardMasking(szTempBuf1, PRINT_CARD_MASKING_4);
        memset(szTempBuf3, 0x00, sizeof(szTempBuf3));


 	//print invoice number
        memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        wub_hex_2_str(srTransRec.szInvoiceNo, szTempBuf1, INVOICE_BCD_SIZE);
        sprintf(szPrintBuf, "%s", szTempBuf1); 
        inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
        //print invoice number
        	
 



	//print trans type and amount
	  memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
       		
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf2, AMT_BCD_SIZE);
        memset(szTempBuf3, 0x00, sizeof(szTempBuf3));
        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
            memset(szTempAmt, 0x00, sizeof(szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(szTempBuf2)/100);
            memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
            strcpy(szTempBuf2, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTempBuf2, szTempBuf3);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTempBuf2, szTempBuf3);
        }
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        
        if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH){
            sprintf(szTempBuf2, "-%s ", strCST.szCurSymbol);
        } else{
            sprintf(szTempBuf2, "%s ", strCST.szCurSymbol);
        }



        strcat(szTempBuf2, szTempBuf3);

	#if 0
	if (srTransRec.byTransType == CB_PAY_TRANS)
		sprintf(szPrintBuf, "CBPay SALES     %s", szTempBuf2);
	else if (srTransRec.byTransType == VOID )
		sprintf(szPrintBuf, "VOID CBPay SALES  %s",szTempBuf2 );

	inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
	#else
	if(srTransRec.byTransType == CB_PAY_TRANS)
		strcpy(szPrintBuf, "CBPay SALES");
	else if (srTransRec.byTransType == VOID)
		strcpy(szPrintBuf, "VOID CBPay SALES");
	
		inPrintLeftRight2(szPrintBuf,szTempBuf2,32);
	#endif
	
	//print trans type and amount


	//print cbpay refno
	 memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        memcpy(szTempBuf1, srTransRec.szRefNo, 16);
	inCCTOS_PrinterBufferOutput(szTempBuf1,&stgFONT_ATTRIB,1);
	//print cbpay refno

        //print date and time
         memset(szPrintBuf, 0x00, sizeof(szPrintBuf));
        memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szDate, szTempBuf2, DATE_BCD_SIZE);

	 memset(szTempBuf3, 0x00, sizeof(szTempBuf3));
        memcpy(szTempBuf3, szTempBuf2+2, 2);                                    inPosLen += 2;
        strcat(szTempBuf3, "/");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                             inPosLen += 2;
        strcat(szTempBuf3, "/");                                                inPosLen += 1;
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        //original ---- wub_hex_2_str(srTransRec.szYear, szTempBuf2, 2);
        memcpy(szTempBuf2, srTransRec.szYear, 2);
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                           inPosLen += 2;
		
        strcat(szTempBuf3, " ");                                                inPosLen += 1;
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szTime, szTempBuf2, 3);
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                             inPosLen += 2;
        strcat(szTempBuf3, ":");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2+2, 2);                           inPosLen += 2;
        strcat(szTempBuf3, ":");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2+4, 2);                            inPosLen += 2;

		
        //sprintf(szPrintBuf, "%-*s%-*s", 11, szTempBuf1, d_LINE_SIZE/3, szTempBuf3);
        sprintf(szPrintBuf, "%s", szTempBuf3);
        inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
	//print date and time

        
        inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);
        
        

	
}

#ifdef OK_DOLLAR_FEATURE
USHORT ushCTOS_printBatchRecordHeaderOKD(void)
{
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];

    ushCTOS_PrintHeader(0);	
    
  	  vdPrintTitleCenter("DETAIL REPORT");
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);

    	printHostLabel();
    
	printTIDMIDCBPay();
    
	printDateTime();
    
	printBatchNO();
    

	inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);

	if (strTCT.inFontFNTMode == 1)
	{

        // for case http://118.201.48.210:8080/redmine/issues/1525.34.1        
        //inCCTOS_PrinterBufferOutput("TRACE NO.       ",&stgFONT_ATTRIB,1);
 		inCCTOS_PrinterBufferOutput("INV NO.       ",&stgFONT_ATTRIB,1);		
		inCCTOS_PrinterBufferOutput("TRANSACTION               AMOUNT",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("REF NO",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("DATE TIME",&stgFONT_ATTRIB,1);

	}
	else
	{
		inCCTOS_PrinterBufferOutput("INV NO.       ",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("TRANSACTION               AMOUNT",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("REF NO",&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutput("DATE TIME",&stgFONT_ATTRIB,1);

	}

	printDividingLine(DIVIDING_LINE_DOUBLE);
	return d_OK;
}

USHORT ushCTOS_printBatchRecordFooterOKD(void)
{
	BYTE baTemp[PAPER_X_SIZE * 64];		
	char szStr[d_LINE_SIZE + 1];
	char szPrintBuf[d_LINE_SIZE*2 + 1];
	char szTempBuf1[d_LINE_SIZE + 1];
	char szTempBuf2[d_LINE_SIZE + 1];
	char szTempBuf3[d_LINE_SIZE + 1];

	int i;
	int inTemp1,inTemp2;
	USHORT usCharPerLine = 32;
        int inPosLen = 0;
        char szTempAmt[AMT_ASC_SIZE+1];
        
        
	
    usCharPerLine = inGetFontlen(&stgFONT_ATTRIB);


	DebugAddSTR("ushCTOS_printBatchRecordFooter", srTransRec.szPAN, 10);
	memset(szTempBuf1,0,sizeof(szTempBuf1));
	memset(szTempBuf2, 0, sizeof(szTempBuf2));
	strcpy(szTempBuf1,srTransRec.szPAN);    
    inIITRead(srTransRec.IITid);
	strcpy(szTempBuf2,strIIT.szIssuerLabel);
//	cardMasking(szTempBuf1, PRINT_CARD_MASKING_1);
    cardMasking(szTempBuf1, PRINT_CARD_MASKING_4);
    memset(szTempBuf3, 0x00, sizeof(szTempBuf3));


 	//print invoice number
    memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTempBuf1, INVOICE_BCD_SIZE);
    sprintf(szPrintBuf, "%s", szTempBuf1); 
    inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
    //print invoice number
        	
 



	//print trans type and amount
	  memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
       		
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szTotalAmount, szTempBuf2, AMT_BCD_SIZE);
        memset(szTempBuf3, 0x00, sizeof(szTempBuf3));
        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
            memset(szTempAmt, 0x00, sizeof(szTempAmt));
            sprintf(szTempAmt, "%012.0f", atof(szTempBuf2)/100);
            memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
            strcpy(szTempBuf2, szTempAmt);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTempBuf2, szTempBuf3);
        } else {
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTempBuf2, szTempBuf3);
        }

		vdDebug_LogPrintf("ushCTOS_printBatchRecordFooterOKD szTempBuf3[%s]", szTempBuf3);
		
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        
        if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH){
            sprintf(szTempBuf2, "-%s ", strCST.szCurSymbol);
        } else{
            sprintf(szTempBuf2, "%s ", strCST.szCurSymbol);
        }


				//curr symbol // amount
        strcat(szTempBuf2, szTempBuf3);

	vdDebug_LogPrintf("ushCTOS_printBatchRecordFooterOKD szTempBuf2[%s]", szTempBuf2);


	#if 0
	if (srTransRec.byTransType == OK_DOLLAR_TRANS)
		sprintf(szPrintBuf, "OK$ SALES       %s", szTempBuf2);//"OK$ SALES     %s", szTempBuf2); -- http://118.201.48.210:8080/redmine/issues/1525.50.6
	else if (srTransRec.byTransType == VOID)
		sprintf(szPrintBuf, "VOID OK$ SALES  %s",szTempBuf2 );
	
	inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);	

	#else
	
	if(srTransRec.byTransType == OK_DOLLAR_TRANS)
		strcpy(szPrintBuf, "OK$ SALES");
	else if (srTransRec.byTransType == VOID)
		strcpy(szPrintBuf, "VOID OK$ SALES");
	
		inPrintLeftRight2(szPrintBuf,szTempBuf2,32);
	#endif
	//print trans type and amount
	


		//print OKD refno
		memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        memcpy(szTempBuf1, srTransRec.szOKDRefNo, 16);
		inCCTOS_PrinterBufferOutput(szTempBuf1,&stgFONT_ATTRIB,1);
		//print OKD refno

        //print date and time
        memset(szPrintBuf, 0x00, sizeof(szPrintBuf));
        memset(szTempBuf1, 0x00, sizeof(szTempBuf1));
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szDate, szTempBuf2, DATE_BCD_SIZE);

	 	memset(szTempBuf3, 0x00, sizeof(szTempBuf3));
        memcpy(szTempBuf3, szTempBuf2+2, 2);                                    inPosLen += 2;
        strcat(szTempBuf3, "/");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                             inPosLen += 2;
        strcat(szTempBuf3, "/");                                                inPosLen += 1;
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        //original ---- wub_hex_2_str(srTransRec.szYear, szTempBuf2, 2);
        memcpy(szTempBuf2, srTransRec.szYear, 2);
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                           inPosLen += 2;
		
        strcat(szTempBuf3, " ");                                                inPosLen += 1;
        
        memset(szTempBuf2, 0x00, sizeof(szTempBuf2));
        wub_hex_2_str(srTransRec.szTime, szTempBuf2, 3);
        memcpy(szTempBuf3+inPosLen, szTempBuf2, 2);                             inPosLen += 2;
        strcat(szTempBuf3, ":");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2+2, 2);                           inPosLen += 2;
        strcat(szTempBuf3, ":");                                                inPosLen += 1;
        memcpy(szTempBuf3+inPosLen, szTempBuf2+4, 2);                            inPosLen += 2;

		
        //sprintf(szPrintBuf, "%-*s%-*s", 11, szTempBuf1, d_LINE_SIZE/3, szTempBuf3);
        sprintf(szPrintBuf, "%s", szTempBuf3);
        inCCTOS_PrinterBufferOutput(szPrintBuf,&stgFONT_ATTRIB,1);
		//print date and time

        
        inCCTOS_PrinterBufferOutput("\n",&stgFONT_ATTRIB,1);
        
        

	
}
	
USHORT printTIDMIDOKD(void)
{
	char szStr[d_LINE_SIZE + 1];
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szMid[16] = {0};
	
	memset(szStr, ' ', d_LINE_SIZE);
	sprintf(szStr, "TID: %s", srTransRec.szTID);
	memset (baTemp, 0x00, sizeof(baTemp));		 

	vdDebug_LogPrintf("print [%s]", szStr);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	memset(szStr, ' ', d_LINE_SIZE);
	strcpy(szMid, srTransRec.szMID);
	szMid[16] =  '\0';
	sprintf(szStr, "MID: %s", szMid);
	memset (baTemp, 0x00, sizeof(baTemp));		 

	vdDebug_LogPrintf("print [%s]", szStr);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
	return (result);

}

USHORT ushCTOS_PrintBodyOKD2(int page)
{	
    char szStr[d_LINE_SIZE*2 + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
	char szTmpPan[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTempAmt[AMT_ASC_SIZE+1];
    BYTE szPrintingText[40 + 1];

    //inCBPayRead(1);
    inOKDPayRead(1);

	inCSTRead(1); //MMK is always the currency for CBPAY
    
//    vdDebug_LogPrintf("ushCTOS_PrintBodyOKD2 START szSubMerId [%s]", strCBOKD.szSubMerId);
    
    memset(szTemp1, ' ', d_LINE_SIZE);
    sprintf(szTemp1,"%s",srTransRec.szHostLabel);  
    vdPrintCenter(szTemp1);
    
    printTIDMIDCBPay(); 
    
#if 0 // no sub mid refer to redmine casae #2070 - comment #47.7
    sprintf(szStr, "SUB MID: %s", strCBOKD.szSubMerId);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif
    
    printDateTime();
       
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    
    memset(szStr, ' ', d_LINE_SIZE);
    if(srTransRec.byTransType == OK_DOLLAR_TRANS)
        strcpy(szStr, "OK$ SALES");   
    else if(srTransRec.byOrgTransType == OK_DOLLAR_TRANS && srTransRec.byTransType == VOID)
        strcpy(szStr, "VOID OK$ SALES");
    
    vdCTOS_ModifyTransTitleString(szStr);
    
    vdPrintTitleCenter(szStr);
    
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        //use ttf print
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
    //reference number
    memset(szStr, ' ', d_LINE_SIZE);
    memset (baTemp, 0x00, sizeof(baTemp));					
    memset(szStr, ' ', d_LINE_SIZE);
    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "REF NO      : %s", srTransRec.szOKDRefNo);
    else
    //use ttf print
        sprintf(szStr, "REF NO      : %s", srTransRec.szOKDRefNo);
	
    memset (baTemp, 0x00, sizeof(baTemp));		
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    //print  trace num
    memset(szStr, ' ', d_LINE_SIZE);
    memset (baTemp, 0x00, sizeof(baTemp));					
    memset(szStr, ' ', d_LINE_SIZE);	
    wub_hex_2_str(srTransRec.szInvoiceNo, baTemp, INVOICE_BCD_SIZE);

	
	// for case http://118.201.48.210:8080/redmine/issues/1525.34.1 	   
	//sprintf(szStr, "TRACE NO. : %s", baTemp);

    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "INV NO. : %s", baTemp);
    else
        sprintf(szStr, "INV NO.     : %s", baTemp);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    
    //Auth response code
    memset(szStr, ' ', d_LINE_SIZE);
    memset (baTemp, 0x00, sizeof(baTemp));					
    memset(szStr, ' ', d_LINE_SIZE);	
    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "TRANX ID : %s", srTransRec.szOKDBankTransId);
    else
        sprintf(szStr, "TRANX ID    : %s", srTransRec.szOKDBankTransId);
    
    memset (baTemp, 0x00, sizeof(baTemp));		
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    
    //print CB PAY ACC  -- The value for CB PAY is  intentionally blank.
    memset(szStr, ' ', d_LINE_SIZE);
    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "OK$ A/C  : ");
    else
        sprintf(szStr, "OK$ A/C     : ");
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp3, ' ', d_LINE_SIZE);
    
    wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
    //wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

    memset(szTempAmt, 0x00, sizeof(szTempAmt));
    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
    memset(szTemp, 0x00, sizeof(szTemp));
    strcpy(szTemp, szTempAmt);
					
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    else//use ttf print
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	
    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp1);
    else 
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp1);
    
    if(srTransRec.byTransType == VOID)
        sprintf(szStr, "AMOUNT: -%s %s", strCST.szCurSymbol, szTemp1); // (2) Please need to fix adjust space between currency and amount at CBpay Receipt.  03-31-2020
    else
        sprintf(szStr, "AMOUNT: %s %s", strCST.szCurSymbol, szTemp1); // (2) Please need to fix adjust space between currency and amount at CBpay Receipt. 03-31-2020
    
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);	

	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        //use ttf print
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

	
    vdDebug_LogPrintf("ushCTOS_PrintBodyOKD2 end");
    return d_OK;	
}

USHORT ushCTOS_PrintFooterOKD(int page)
{		
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 

    vdDebug_LogPrintf("ushCTOS_PrintFooterOKD start");

	if(srTransRec.byTransType == VOID)
	    {   
	        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
			if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
			        {
			            vdSetErrorMessage("LOAD MMT ERR");
			            return(d_NO);
			        }
	        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctFoot1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctFoot1);
	    }


	
	if(page == d_FIRST_PAGE)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		vdDebug_LogPrintf("ushCTOS_PrintFooterOKD d_FIRST_PAGE");

		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
		    EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			// patrick add code 20141208
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
            {   
                //CTOS_PrinterFline(d_LINE_DOT * 1);
                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
				if (srTransRec.byEntryMode==CARD_ENTRY_ICC)
				{
                	//CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
					//inCTOSS_CapturePrinterBuffer("     (PIN VERIFY SUCCESS)",&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput("     (PIN VERIFY SUCCESS)",&stgFONT_ATTRIB,1);
				}
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        }

		/*Aplipay no need E signature*/
		//ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//inCTOSS_CapturePrinterBuffer("SIGN:___________________________",&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);
		
		printCardHolderName();
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

		//http://118.201.48.210:8080/redmine/issues/2070.102		
//		ushCTOS_PrintAgreementOKD();

		// for redmine case #2070 - comment 47.8
		#if 0
		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    		vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    		vdPrintCenter(strMMT[0].szRctFoot3);
		#else // http://118.201.48.210:8080/redmine/issues/1525.50.1
		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);

		#endif


    	if(fRePrintFlag == TRUE)
    	{
	    	if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
				inCCTOS_PrinterBufferOutputAligned("* * 重 印 * *", &stgFONT_ATTRIB, d_PRINTER_ALIGNCENTER, 1, 0);
			else
				vdPrintCenter("DUPLICATE");
    	}
			

		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** MERCHANT COPY *****  ");
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		//if (strTCT.inFontFNTMode == 1)
                if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
                {
                        inCCTOS_PrinterBufferOutput("     商戶存根 請妥善保管  ",&stgFONT_ATTRIB,1);
                }
                else
			inCCTOS_PrinterBufferOutput("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
		//else
		//inCCTOS_PrinterBufferOutput("      ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
		
	}
	else
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
			EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			// patrick add code 20141208
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
            {         
                //CTOS_PrinterFline(d_LINE_DOT * 1);
                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,0);
				if (srTransRec.byEntryMode==CARD_ENTRY_ICC)
	                //CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
			//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
        }

		/*Aplipay no need E signature*/
		//ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,0);
		printCardHolderName();
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
		
		//ushCTOS_PrintAgreement();

       		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			vdSetGolbFontAttrib(d_FONT_9x18, NORMAL_SIZE, NORMAL_SIZE, 0, 0);



//http://118.201.48.210:8080/redmine/issues/2070.102
//			ushCTOS_PrintAgreementOKD();		
			
			
			// for redmine case #2070 - comment 47.8
#if 0
			if(strlen(strMMT[0].szRctFoot1) > 0)
				vdPrintCenter(strMMT[0].szRctFoot1);
			if(strlen(strMMT[0].szRctFoot2) > 0)
					vdPrintCenter(strMMT[0].szRctFoot2);
			if(strlen(strMMT[0].szRctFoot3) > 0)
					vdPrintCenter(strMMT[0].szRctFoot3);
#else // http://118.201.48.210:8080/redmine/issues/1525.50.1
			if(strlen(strMMT[0].szRctFoot1) > 0)
				vdPrintCenter(strMMT[0].szRctFoot1);
			
#endif



			vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);


    	if(fRePrintFlag == TRUE)
    	{
	    	if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
				inCCTOS_PrinterBufferOutputAligned("* * 重 印 * *", &stgFONT_ATTRIB, d_PRINTER_ALIGNCENTER, 1, 0);
			else
				vdPrintCenter("DUPLICATE");
    	}
		
		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** CUSTOMER COPY *****  ");
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
		//if (strTCT.inFontFNTMode == 1)
                if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
                {
                        inCCTOS_PrinterBufferOutput("     客戶存根 請妥善保管  ",&stgFONT_ATTRIB,1);
                }
                else
			inCCTOS_PrinterBufferOutput("   ***** CUSTOMER COPY *****  ",&stgFONT_ATTRIB,0);
		//else
		//inCCTOS_PrinterBufferOutput("      ***** CUSTOMER COPY *****  ",&stgFONT_ATTRIB,0);
	}
	
	//CTOS_PrinterFline(d_LINE_DOT * 6);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();

return d_OK;;	
}



USHORT ushCTOS_PrintAgreementOKD(void) 
{
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

    vdPrintCenter("THANK YOU");
    vdPrintCenter("HAVE A NICE DAY");
    vdPrintCenter("NO REFUND");
    
    vdPrintCenter("  ");

    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
}

#endif

USHORT ushCTOS_GetFontInfor(void)
{
	USHORT usASCIIFontID;
	USHORT usFontSize;
	USHORT usFontStyle;

	CTOS_LanguagePrinterGetFontInfo( &usASCIIFontID, &usFontSize, &usFontStyle );   
	vdDebug_LogPrintf("usASCIIFontID[%d]usFontSize[%d]usFontStyle[%d]",usFontSize,usFontStyle );
}

USHORT ushCTOS_ReprintLastSettleReport(void)
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult,inRet;
	int inTranCardType;
	int inReportType;
	int inIITNum , i;
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);
	if( printCheckPaper()==-1)
		return;
		
	//by host and merchant
	shHostIndex = inCTOS_SelectHostSetting();
	if (shHostIndex == -1)
		return;
	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);
    if ((strHDT.inHostIndex == 17)
	#ifdef CBPAY_DV
	 || (strHDT.inHostIndex == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX || strHDT.inHostIndex == 6 || srTransRec.HDTid == 23)
	 #endif
	) {//marco 20170704
        inCSTRead(1);
    } else {
	inCSTRead(strHDT.inCurrencyIdx);
    }

	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inRet = inCTOS_MultiAPSaveData(d_IPC_CMD_REPRINT_LAST_SETT);
		if(d_OK != inRet)
			return inRet;
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return inRet;

			inRet = inCTOS_MultiAPReloadHost();
			if(d_OK != inRet)
				return inRet;
		}
	}

	inResult = inCTOS_CheckAndSelectMutipleMID();
	DebugAddINT("summary MITid",srTransRec.MITid);
	if(d_OK != inResult)
		return;

	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
	inResult = inCTOS_ReadBKAccumTotal(&srAccumRec,strHDT.inHostIndex,srTransRec.MITid);
	if(inResult == ST_ERROR)
	{
	    vdSetErrorMessage("NO RECORD FOUND");
		vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		return ST_ERROR;	
	}		 
	else if(inResult == RC_FILE_READ_OUT_NO_DATA)
	{
	    vdSetErrorMessage("NO RECORD FOUND");
		return;
	}	 
    strcpy(srTransRec.szTID, srAccumRec.szTID);
    strcpy(srTransRec.szMID, srAccumRec.szMID);
    memcpy(srTransRec.szYear, srAccumRec.szYear, DATE_BCD_SIZE);
    memcpy(srTransRec.szDate, srAccumRec.szDate, DATE_BCD_SIZE);
    memcpy(srTransRec.szTime, srAccumRec.szTime, TIME_BCD_SIZE);
    memcpy(srTransRec.szBatchNo, srAccumRec.szBatchNo, BATCH_NO_BCD_SIZE);

    if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);
    
	ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("LAST SETTLE");
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);


	#ifdef CBPAY_DV
		if(srTransRec.HDTid == CBPAY_HOST_INDEX)
			{
				strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
			}

		vdDebug_LogPrintf("srTransRec[%s] ---- strHDT[%s]", srTransRec.szHostLabel, strHDT.szHostLabel);
	#endif

#ifdef OK_DOLLAR_FEATURE
			if(srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
				{
					strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
				}
	
			vdDebug_LogPrintf("srTransRec[%s] ---- strHDT[%s]", srTransRec.szHostLabel, strHDT.szHostLabel);
#endif
	

  	printHostLabel();
    
	#ifdef CBPAY_DV
				if(srTransRec.HDTid == CBPAY_HOST_INDEX)
						printTIDMIDCBPay();
				
				#ifdef OK_DOLLAR_FEATURE
				else if(srTransRec.HDTid == OK_DOLLAR_HOST_INDEX)
					printTIDMIDOKD();
				#endif
				else
					printTIDMID();
	#else
				printTIDMID();
	#endif
    
	printDateTime();
    
	printBatchNO();

    //CTOS_PrinterFline(d_LINE_DOT * 1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        
//        if (srTransRec.inCardType == CREDIT_CARD) {
//        inTranCardType = 0; //save credit card accue total
//    } else {
//        inTranCardType = 1; //save debit card accue total
//    }
        
        if(shHostIndex == MPU_DEBIT_HOST_IDX || shHostIndex == MPU_JCBI_HOST_IDX || shHostIndex == MPU_CUP_HOST_IDX//| srTransRec.HDTid == 13
         #ifdef TOPUP_RELOAD
         || srTransRec.HDTid == 20 || srTransRec.HDTid == 22 || srTransRec.HDTid == 23
         #endif
		 )
         {
            inTranCardType = 1;
         }
        else
            inTranCardType = 0;

		if(TRUE == fGetCashAdvAppFlag())
		{
			inTranCardType = 0;
		}
        
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0))
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,strIIT.szIssuerLabel);
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount,	 (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),
				
								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"TOTAL:");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),
			
						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
		}
//	}
	
	//print space one line
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
	return d_OK;	
}

void vdCTOSS_PrinterBMPPic(unsigned int usX,unsigned int y, const char *path)
{
	char szBmpfile[50+1];
	
	memset(szBmpfile, 0x00, sizeof(szBmpfile));
	sprintf(szBmpfile,"%s%s", LOCAL_PATH, path);
	
	CTOS_PrinterBMPPic(usX, szBmpfile);
	return ;
}

int inCTOSS_QRCode(void)
{
}


int ushCTOS_ePadSignature(void)
{
	BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
	USHORT ret = d_NO;
	DISPLAY_REC szDisplayRec;
	BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];
    char szTemp[AMT_ASC_SIZE+1];
    char szTemp1[AMT_ASC_SIZE+1];

	int inResult, inEpadResult;	

	
	int inJCBSignLine = get_env_int("JCBSIGNLINE");	
	int inUPISignLine = get_env_int("UPISIGNLINE");
	
	long lnTotalAmount = 0;
	char tmpbuf[20];

	int inTemp;
	int inTemp2;

	

     //vdDebug_LogPrintf("ushCTOS_ePadSignature srTransRec.byEntryMode=[%d]",srTransRec.byEntryMode);
	 //vdDebug_LogPrintf("ushCTOS_ePadSignature srTransRec.byTransType=[%d]",srTransRec.byTransType );

     vdDebug_LogPrintf("ushCTOS_ePadSignature srTransRec.byEntryMode=[%d]",srTransRec.byEntryMode);
	 vdDebug_LogPrintf("ushCTOS_ePadSignature srTransRec.byTransType=[%d]fSignatureFlag[%d]byPinPadType[%d]",
	 	srTransRec.byTransType, strTCT.fSignatureFlag, strTCT.byPinPadType);
	 
	if(srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND)
	{
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
			{

			    inResult=inCTOSS_CheckFlagVEPSQPS();
				 vdDebug_LogPrintf("inCTOSS_CheckFlagVEPSQPS inResult=[%d]",inResult );

		
				//if(inResult != 0 && (srTransRec.byTransType == SALE || srTransRec.byTransType == REFUND))
				if(inResult != 0)
				{   
	            	    vdDebug_LogPrintf("ushCTOS_ePadSignature fVEPS QPS flag on = 1");               
						return d_OK;
					
	            }
				
			}

	}

	
    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
	{
    	memset(szTemp, 0x00, sizeof(szTemp));
        wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
        sprintf(szTotalAmt, "%012.0f", atof(szTemp)/100);
    	memset(szStr, 0x00, sizeof(szStr));
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmt, szStr);
    } 
	else 
	{
        memset(szTemp1, 0x00, sizeof(szTemp1));
       wub_hex_2_str(srTransRec.szTotalAmount, szTemp1, AMT_BCD_SIZE);
        memset(szStr, 0x00, sizeof(szStr));
		vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szStr);
    }

	vdDebug_LogPrintf("ushCTOS_ePadSignature=szTotalAmt:[%s]szTemp:[%s]szTemp1:[%s]szStr:[%s]InFloorLimitAmount:[%d]inJCBSignLine:[%d]inUPISignLine:[%d]",
		szTotalAmt,szTemp,szTemp1,szStr,strCDT.InFloorLimitAmount, inJCBSignLine, inUPISignLine);


	//http://118.201.48.210:8080/redmine/issues/1525.97.less than or equal to floorlimit case.
	#if 1
	if(inJCBSignLine == 1)
	{
		if(srTransRec.byTransType != VOID) // always prompt signature on VOID txn.
		{
			if((srTransRec.IITid == 8) || (srTransRec.IITid == 4))
			{	
				
				if(strcmp(strCST.szCurSymbol, "MMK") == 0)
					inTemp = atoi(szTemp);
				else
					inTemp = atoi(szStr);
				
				vdDebug_LogPrintf("ushCTOS_ePadSignature inTemp [%d] strCDT.InFloorLimitAmount [%d]", inTemp, strCDT.InFloorLimitAmount);
				
				if (strCDT.InFloorLimitAmount >= inTemp){
					vdDebug_LogPrintf("(JCB ushCTOS_ePadSignature strCDT.InFloorLimitAmount >= inTemp)");
					return d_OK;
				}
			}
		}
	}
	//http://118.201.48.214:8080/issues/15#change-1001 
	//For UPI, 1) Testing with amount less than CVM limit, terminal prompts customer signature but it's correct in e-receipt (No Signature required).
	if(inUPISignLine == 1)
	{
	
		memset(tmpbuf,0x00,sizeof(tmpbuf));
		inCTOSS_GetEnvDB("CVMAMT",tmpbuf);	

		vdDebug_LogPrintf("CVMAMT:[%s]",tmpbuf);
		
		if(srTransRec.byTransType != VOID) // always prompt signature on VOID txn.
		{		
			if(srTransRec.IITid == 6)
			{
				vdDebug_LogPrintf("ushCTOS_ePadSignature AAAA");
				
				if(strcmp(strCST.szCurSymbol, "MMK") == 0)
					inTemp2 = atoi(szTemp);
				else					
					inTemp2 = atoi(szStr);

				vdDebug_LogPrintf("ushCTOS_ePadSignature inTemp2 [%d] atoi(tmpbuf) [%d]", inTemp2, atoi(tmpbuf));
					
				if (atoi(tmpbuf) >= inTemp2)
				{					
					vdDebug_LogPrintf("UPI ushCTOS_ePadSignature (atoi(tmpbuf) >= inTemp2)");
					return d_OK;				
				}
				
			}
		}		
	}
	#else	
		lnTotalAmount = atol(szTemp);	
		if (lnTotalAmount <= strCDT.InFloorLimitAmount)
			return (d_NO);
	#endif

	vdDebug_LogPrintf("ushCTOS_ePadSignature AFTER CVMAMT/ InFloorLimitAmount CHECKING!!!");


//    memset(szTemp, 0x00, sizeof(szTemp));
////	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);      
//        wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
//        sprintf(szTotalAmt, "%012.0f", atof(szTemp)/100);
//    memset(szStr, 0x00, sizeof(szStr));
//	//format amount 10+2
////	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
//        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTotalAmt, szStr);
	memset(&szDisplayRec,0x00,sizeof(DISPLAY_REC));
	//strcpy(szDisplayRec.szDisplay1,"Sign");
	sprintf(szDisplayRec.szDisplay1,"Sign  %s %s",strCST.szCurSymbol,szStr);
	//strcpy(szDisplayRec.szDisplay5,"Please Sign And");
	//strcpy(szDisplayRec.szDisplay6,"Enter OK Key");

	vdDebug_LogPrintf("EPAD ushCTOS_ePadSignature szDisplayRec.szDisplay1 = Len[%d][%s]", strlen(szDisplayRec.szDisplay1), szDisplayRec.szDisplay1);

	ginSingatureStatus = 1;

	// #2 To remove asking signature for all JCB Tranx ( pure JCB and MPU_JCB ) at MPU New Switch - case #1750
	//#ifdef JCB_NO_SIGNLINE
	#if 0 //http://118.201.48.210:8080/redmine/issues/1525.92.5 (JCB issue)
	if((srTransRec.IITid == 8) || (srTransRec.IITid == 4))
		return d_OK;

	#endif

	if (FALSE ==strTCT.fSignatureFlag)
		return d_OK;

	vdDebug_LogPrintf("ushCTOS_ePadSignature AAAAA");



	if(srTransRec.byTransType != VOID)//thandar_added in_ to not check for VOID as per CB report not signpad below signature 
	{
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
		
		vdDebug_LogPrintf("ushCTOS_ePadSignature BBBBB");

		    EMVtagLen = 3;
	        memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);

			vdDebug_LogPrintf("EMVtagVal=[%02X]",EMVtagVal[0]);
				
			// patrick add code 20141208
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) && (EMVtagVal[0] != 0x02) && // to fix missing signature in e-receipt . http://118.201.48.210:8080/redmine/issues/1525.94.4
				 (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
	        {
				vdDebug_LogPrintf("ushCTOS_ePadSignature CCCCC");
	        
	        	ginSingatureStatus = 2;
				return d_OK;
	        }
		}
	}


	vdDebug_LogPrintf("ushCTOS_ePadSignature DDDDD");
	
	//ePad_SignatureCaptureLib();
	if(strTCT.byPinPadType == 3)
	{
		ret = inCTOSS_EXTSignature();
		vdDebug_LogPrintf("inCTOSS_EXTSignature=[%c][%d]",ret,ret);
		if (strTCT.fCancelSignFlag != TRUE && ret == d_KBD_CANCEL)
		{
			while (1)
			{
				ret = inCTOSS_EXTSignature();
				vdDebug_LogPrintf("inCTOSS_EXTSignature=[%c][%d]",ret,ret);
				if (ret == d_OK)
					break;
			}
		}
		if (ret == d_OK)
			ginSingatureStatus = 0;
		else if (ret == d_KBD_CANCEL)
			ginSingatureStatus = 3;
		else
			ginSingatureStatus = 1;
			
	}
	else
	{
		vdDebug_LogPrintf("EPAD ushCTOS_ePadSignature HERE");
	
		inEpadResult = ePad_SignatureCaptureLibEex(&szDisplayRec);

		
		vdDebug_LogPrintf("EPAD ushCTOS_ePadSignature inEpadResult [%d]", inEpadResult);
	}

	if ((strTCT.byTerminalType%2) == 0)
		setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "Processing...");
	else	
    	setLCDPrint(8, DISPLAY_POSITION_LEFT, "Processing...");

   	inCTLOS_Updatepowrfail(PFR_SIGNATURE_COMPLETE);

	
	vdDebug_LogPrintf("EPAD ushCTOS_ePadSignature END");
	return d_OK;
}

int ushCTOS_ePadPrintSignature(void)
{
    //char szNewFileName[24+1];
    //ULONG ulFileSize;

	vdDebug_LogPrintf("print.c > ushCTOS_ePadPrintSignature");

	if (FALSE ==strTCT.fSignatureFlag)
		return d_NO;

	//if(srTransRec.byTransType == VOID || srTransRec.byTransType == SALE_TIP || srTransRec.byTransType == VOID_PREAUTH)
	if(srTransRec.byTransType == SALE_TIP || srTransRec.byTransType == VOID_PREAUTH)
		return d_NO;

	#if 1
	//ePad_PrintSignatureCaptureLib();
	if(lnGetFileSize(DB_SIGN_BMP) > 0)
	{
		vdCTOSS_PrinterBMPPicEx(0, 0, DB_SIGN_BMP);
	//usCTOSS_ReadLGOfile();
		inCTOSS_ReadERMSignatureFile(VFBMPONAME);
	}
	#else
	ulFileSize=lnGetFileSize(DB_SIGN_BMP);
	vdDebug_LogPrintf("DB_SIGN_BMP:[%s], ulFileSize:[%ld]",DB_SIGN_BMP, ulFileSize);
	
	if(ulFileSize > 0)
	{
		//vdCTOS_PrinterFline(1);
		vdCTOSS_PrinterBMPPicEx(0, 0, DB_SIGN_BMP);
		inCTOSS_ReadERMSignatureFile(VFBMPONAME);
		return d_OK;
	}
	
	#endif
	
	return d_OK;
}


int ushCTOS_ClearePadSignature(void)
{
	if (FALSE ==strTCT.fSignatureFlag)
		return d_OK;
	
	ePad_ClearSignatureCaptureLib();
	return d_OK;
}

int inPrintISOPacket(int fSendPacket, unsigned char *pucMessage, int inLen)
{
    char ucLineBuffer[d_LINE_SIZE];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];
	
	if (inLen <= 0)
	  return(ST_SUCCESS);

    if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
	vdCTOSS_PrinterStart(100);

      if (fSendPacket == 1)
      {
		memset (baTemp, 0x00, sizeof(baTemp));
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"Send ISO Packet\n");
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

      }
      else
      {
		memset (baTemp, 0x00, sizeof(baTemp));	
		memset(szStr, 0x00, sizeof(szStr));
		if (fSendPacket == 0)
			strcpy(szStr,"Receive ISO Packet\n");
		else if (fSendPacket == 2)
			strcpy(szStr,"Receive track2\n");
		else if (fSendPacket == 3)
			strcpy(szStr,"Receive Chip Data\n");
		else if (fSendPacket == 4)
			strcpy(szStr,"Receive Additional Data\n");
		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

      }
      
      //CTOS_PrinterFline(d_LINE_DOT);
	  inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

      
      pucBuff = pucMessage + inLen;
      while (pucBuff > pucMessage)
      {
          memset(ucLineBuffer,0x00, sizeof(ucLineBuffer));
          for (inBuffPtr = 0; (inBuffPtr < 32) && (pucBuff > pucMessage); inBuffPtr += 3)
          {
              sprintf(&ucLineBuffer[inBuffPtr], "%02X ", *pucMessage);
              pucMessage++;
          }
              ucLineBuffer[32] = '\n';
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, ucLineBuffer, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(ucLineBuffer,&stgFONT_ATTRIB,1);

      } 
      //CTOS_PrinterFline(d_LINE_DOT * 2);
	  inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  vdCTOSS_PrinterEnd();


      return (ST_SUCCESS);
}

#define QRCODE_TXRX_BUFFER 			350

static unsigned char uszQRCodeBuffer [(QRCODE_TXRX_BUFFER) + 50];
static int inQRCodeBufferSize = 0;

int inInitializePrinterQRCodeBuffer(void)
{
	memset(uszQRCodeBuffer, 0x00, sizeof(uszQRCodeBuffer));
	strcpy((char *)uszQRCodeBuffer, "");
	inQRCodeBufferSize = 0;
    return(d_OK);	
}

int inCTOSS_CapturePrinterQRCodeBuffer(unsigned char* pchTemp, int endflag)
{
	int inRetSize = 0;
	char tembuf[3+1] = "\n";

	
	inRetSize = strlen((char *)pchTemp);
	
	memcpy(uszQRCodeBuffer + inQRCodeBufferSize, pchTemp,  strlen((char *)pchTemp));
	inQRCodeBufferSize += strlen((char *)pchTemp);

	if (strcmp(pchTemp,"\n") != 0)
	{
		strcpy(uszQRCodeBuffer + inQRCodeBufferSize,tembuf);
		inQRCodeBufferSize += strlen(tembuf);
	}

	//vdDebug_LogPrintf("inCTOSS_CapturePrinterQRCodeBuffer=[%d][%d]........",inRetSize,inQRCodeBufferSize);

	if (inQRCodeBufferSize > QRCODE_TXRX_BUFFER || (endflag == 1))
	{	
		vdDebug_LogPrintf("inCTOSS_CapturePrinterQRCodeBuffer=[%d][%s]........",inQRCodeBufferSize,uszQRCodeBuffer);
		vdCTOSS_Print2QRCodeOneLine(uszQRCodeBuffer);
		inInitializePrinterQRCodeBuffer();
		CTOS_PrinterFline(d_LINE_DOT);
	}
	return inRetSize;
}

void vdCTOSS_DisplayQRCodeOneLine(char *intext)
{
	BYTE key;
	USHORT res;
	CTOS_QRCODE_INFO qrcodeInfo;
        int inSize = 0;

	qrcodeInfo.InfoVersion = QR_INFO_VERSION;
//	qrcodeInfo.Size = 5;
        
        if(strlen(intext) >= 300)
            inSize = 4;
        else
            inSize = 4;
        
    qrcodeInfo.Size = inSize;
        
	qrcodeInfo.Version = QR_VERSION21X21;
	qrcodeInfo.Level = QR_LEVEL_L;
	
	res = CTOS_QRCodeDisplay(&qrcodeInfo, intext, 90, 50);

	return ;
}

void vdCTOSS_Print2QRCodeOneLine(char *intext)
{
	BYTE key;
	USHORT res;
	CTOS_QRCODE_INFO qrcodeInfo;

	qrcodeInfo.InfoVersion = QR_INFO_VERSION;
	qrcodeInfo.Size = 5;
	qrcodeInfo.Version = QR_VERSION21X21;
	qrcodeInfo.Level = QR_LEVEL_L;
	
	res = CTOS_QRCodePrinter(&qrcodeInfo, intext, 1);

	return ;
}


int inPrintISOfield(char *szFieldname,unsigned char *pucMessage, int inLen, int fHexFlag)
{
#define SIZE_24X24	32
#define SIZE_16X16	48

    unsigned char ucLineBuffer[999];
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[50 + 1];
	int inFieldlen;
	int totallen,i;
	char tmpbuf[10+1];

	if (strTCT.fPrintISOMessage != VS_TRUE)
		return ST_SUCCESS;

    //inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
    //vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_16x16,0," ");
    vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

	memset(ucLineBuffer,0x00,sizeof(ucLineBuffer));
	inFieldlen = strlen(szFieldname);
	strcpy(ucLineBuffer,szFieldname);
	strcat(ucLineBuffer,": ");
	if (fHexFlag == 1)
		wub_hex_2_str(pucMessage,&ucLineBuffer[inFieldlen+2],inLen);
	else
		memcpy(&ucLineBuffer[inFieldlen+2],pucMessage,inLen);

	totallen = strlen(ucLineBuffer);	
	vdDebug_LogPrintf("inPrintISOfield=[%d][%s]........",totallen,ucLineBuffer);
	if (totallen > SIZE_16X16)
	{
		inBuffPtr = totallen/SIZE_16X16;
		for (i=0; i<inBuffPtr; i++)
		{
			memset(szStr,0x00,sizeof(szStr));
			memcpy(szStr,&ucLineBuffer[i*SIZE_16X16],SIZE_16X16);

			memset (baTemp, 0x00, sizeof(baTemp));		
			CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		}
		
		inBuffPtr = totallen%SIZE_16X16;
		if (inBuffPtr > 0)
		{
			memset(szStr,0x00,sizeof(szStr));
			memcpy(szStr,&ucLineBuffer[i*SIZE_16X16],inBuffPtr);

			memset (baTemp, 0x00, sizeof(baTemp));		
			CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		}
	}
	else
	{
		memset(szStr,0x00,sizeof(szStr));
		memcpy(szStr,ucLineBuffer,totallen);

		memset (baTemp, 0x00, sizeof(baTemp));		
		CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	}
	
	return (ST_SUCCESS);
}

USHORT usCTOSS_EMV_DownloadFile(char *szLogoURL)
{
    BYTE bInBuf[250];
    BYTE bOutBuf[250];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	usInLen = strlen(szLogoURL);
	strcpy(bInBuf,szLogoURL);

	vdDebug_LogPrintf("usCTOSS_EMVWget usInLen[%d] szLogoURL[%s]",usInLen, szLogoURL);

	usResult = inMultiAP_RunIPCCmdTypes("SHARLS_EMV", d_IPC_CMD_EMV_WGET, bInBuf, usInLen, bOutBuf, &usOutLen);
    
	vdDebug_LogPrintf("usCTOSS_EMVWget usOutLen[%d] bOutBuf[%s]",usOutLen, bOutBuf);
    return usResult;
}

int inDownloadLogoFilePrintLogoFile(void)
{
	int inRetVal = SUCCESS;
	unsigned char uszLogoName[100+1];
	unsigned char uszURLLogoName[100+1];

	inCPTRead(1);

	strcpy((char*)strCPT.szPriTxnHostIP, (char *)"118.201.48.210");
	strCPT.inPriTxnHostPortNum = 80; // web server definitely  is port 80
	strTCT.fShareComEnable = 1;

    if (inCTOS_InitComm(strCPT.inCommunicationMode) != d_OK) 
    {
        vdSetErrorMessage("COMM INIT ERR");
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

    CTOS_PrinterSetHeatLevel(4); 

	memset(uszLogoName, 0x00, sizeof(uszLogoName));
	strcpy(uszLogoName, "logo.bmp");
	vdCTOSS_RemovePrinterBMPPic_Pub(uszLogoName);
	memset(uszURLLogoName, 0x00, sizeof(uszURLLogoName));
	strcpy(uszURLLogoName, "http://118.201.48.210/");
	strcat(uszURLLogoName, uszLogoName);
	CTOS_LCDTPrintXY(1, 8, (unsigned char*)"DOWNLOADING...");						
	usCTOSS_EMV_DownloadFile(uszURLLogoName);	
	CTOS_LCDTPrintXY(1, 8, (unsigned char*)"DOWNLOAD DONE!");																														
	vdCTOSS_PrinterBMPPic_Pub(0, 0, uszLogoName);

	memset(uszLogoName, 0x00, sizeof(uszLogoName));	
	strcpy(uszLogoName, "logo_nets.bmp");
	vdCTOSS_RemovePrinterBMPPic_Pub(uszLogoName);
	memset(uszURLLogoName, 0x00, sizeof(uszURLLogoName));
	strcpy(uszURLLogoName, "http://118.201.48.210/");
	strcat(uszURLLogoName, uszLogoName);
	CTOS_LCDTPrintXY(1, 8, (unsigned char*)"DOWNLOADING...");						
	usCTOSS_EMV_DownloadFile(uszURLLogoName);	
	CTOS_LCDTPrintXY(1, 8, (unsigned char*)"DOWNLOAD DONE!");																														
	vdCTOSS_PrinterBMPPic_Pub(0, 0, uszLogoName);
	
	memset(uszLogoName, 0x00, sizeof(uszLogoName));
	strcpy(uszLogoName, "nfplogo.bmp");
	vdCTOSS_RemovePrinterBMPPic_Pub(uszLogoName);	
	memset(uszURLLogoName, 0x00, sizeof(uszURLLogoName));
	strcpy(uszURLLogoName, "http://118.201.48.210/");
	strcat(uszURLLogoName, uszLogoName);
	CTOS_LCDTPrintXY(1, 8, (unsigned char*)"DOWNLOADING...");						
	usCTOSS_EMV_DownloadFile(uszURLLogoName);	
	CTOS_LCDTPrintXY(1, 8, (unsigned char*)"DOWNLOAD DONE!");																															
	vdCTOSS_PrinterBMPPic_Pub(0, 0, uszLogoName);

	inCTOS_inDisconnect();

	return SUCCESS;
}

void vdCTOSS_RemovePrinterBMPPic_Pub(const char *path)
{
	char szBmpfile[50+1];
	char  szSystemCmdPath[250];
	
	memset(szBmpfile, 0x00, sizeof(szBmpfile));
	sprintf(szBmpfile,"%s%s", PUBLIC_PATH, path);

	memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));
	sprintf(szSystemCmdPath, "rm -f %s", szBmpfile);
	system(szSystemCmdPath);
	
	return ;
}

void vdCTOSS_PrinterBMPPic_Pub(unsigned int usX,unsigned int y, const char *path)
{
	char szBmpfile[50+1];
	
	memset(szBmpfile, 0x00, sizeof(szBmpfile));
	sprintf(szBmpfile,"%s%s", PUBLIC_PATH, path);
	
	CTOS_PrinterBMPPic(usX, szBmpfile);
	return ;
}

void vdCTOSS_DisplayQRCode(void)
{
	BYTE strOut[100];
	BYTE szdatabase[100];
	BYTE sztable[100];
	USHORT usLen;
	USHORT ret;
	char szDispay[50];

	memset(szDispay,0x00,sizeof(szDispay));
	memset(szdatabase,0x00,sizeof(szdatabase));
	memset(sztable,0x00,sizeof(sztable));
	sprintf(szDispay,"QR CODE");
	CTOS_LCDTClearDisplay();
	
	vdDispTitleString(szDispay);			
	CTOS_LCDTPrintXY(1, 3, "ENTER:");
	
	usLen = 100;
	CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
	ret = InputStringAlphaEx(1, 7, 0x00, 0x02, szdatabase, &usLen, 1, d_INPUT_TIMEOUT);
	if (strTCT.inThemesType == 1)
	{
		CTOS_LCDTTFSelect("tahoma.ttf", 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);
	}
	else
	{
		CTOS_LCDTTFSelect(d_FONT_DEFAULT_TTF, 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);
	}

	if (ret == d_KBD_CANCEL || 0 == ret )
	{
		return ;
	}
	if(ret>= 1)
	{
		CTOS_LCDTClearDisplay();
		CTOS_LCDTTFSelect("tahoma.ttf", 0);
		CTOS_LCDTTFSwichDisplayMode(d_TTF_MODE_VIETNAM);

		vdDebug_LogPrintf("szdatabase[%s].usLen=[%d].",szdatabase,usLen);
		vdCTOSS_DisplayQRCodeOneLine(szdatabase);
		WaitKey(60);
	}
	
}

#if 0
int inPrintTesting(void)
{
    char ucLineBuffer[d_LINE_SIZE];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];

	vdDebug_LogPrintf("inPrintTesting..............");
	#if 0
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
#else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
#endif	

	CTOS_PrinterSetHeatLevel(4);

	vdCTOSS_PrinterStart(100);
	vdDebug_LogPrintf("1111111111111.");
	
	 memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"PRINT TESTING START");
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);

	inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);

      {
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"PRINTER_ALIGNLEFT");
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
		inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);

      }
	  vdDebug_LogPrintf("3333333.");
      {
			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNLEFT");
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNCENTER");
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNCENTER");
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNRIGHT");
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNRIGHT");
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
		
      }

	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	  vdDebug_LogPrintf("777777777.");
#if 1
	  memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"ALIGNLEFT");
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,0,0);

	memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"ALIGNCENTER");
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,0,0);

	memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"ALIGNRIGHT");
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT ,1,0);

#endif		

	inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	 memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"PRINT TESTING END");
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
	inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER ,1,0);
			
      vdDebug_LogPrintf("888888888");
	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);
	  inCCTOS_PrinterBufferOutputAligned(ONE_LINE_DOT,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT ,1,0);

	  vdDebug_LogPrintf("99999999");

	  vdCTOSS_PrinterEnd();


      return (ST_SUCCESS);
}


int inCTOSS_CallbackDisplay(void)
{
	unsigned short inRet;
	unsigned char key;

	vdDebug_LogPrintf("vdCTOSS_CallbackDisplay......");
	
	CTOS_LCDTClearDisplay();
	vduiWarningSound();
	vduiDisplayStringCenter(1,"PRINTER STATUS");
	vduiDisplayStringCenter(3,"PRINTER OUT OF");
	vduiDisplayStringCenter(4,"PAPER, INSERT");
	vduiDisplayStringCenter(5,"PAPER AND PRESS");
	vduiDisplayStringCenter(6,"ANY TO PRINT.");
	vduiDisplayStringCenter(8,"[X] CANCEL PRINT");

	CTOS_KBDGet(&key);
	if(key==d_KBD_CANCEL)
	{
		CTOS_KBDBufFlush();//cleare key buffer
		return d_NO;
	}

	return d_OK;
	
}


int inPrintTestingEx(void)
{
    char ucLineBuffer[d_LINE_SIZE];
    unsigned char *pucBuff;
    int inBuffPtr = 0;
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szStr[d_LINE_SIZE + 1];

	vdDebug_LogPrintf("inPrintTesting..............");
	#if 0
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
#else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
#endif	

	CTOS_PrinterSetHeatLevel(4);

	vdCTOSS_PrinterStartStatus(100,inCTOSS_CallbackDisplay);
	vdDebug_LogPrintf("1111111111111.");


	vdCTOSS_PrinterBMPPicExStatus(0, 0, "./fs_data/logo.bmp");
	
	 memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"PRINT TESTING START");
	inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

      {
		memset(szStr, 0x00, sizeof(szStr));
		strcpy(szStr,"PRINTER_ALIGNLEFT");
		inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);

      }
	  vdDebug_LogPrintf("3333333.");
      {
			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNLEFT");
			inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNCENTER");
			inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNCENTER");
			inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNRIGHT");
			inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);

			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr,"PRINTER_ALIGNRIGHT");
			inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);
		
      }

	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  vdDebug_LogPrintf("777777777.");
#if 1
	  memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"ALIGNLEFT");
	inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);

	memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"ALIGNCENTER");
	inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);

	memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"ALIGNRIGHT");
	inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);
#endif		

	inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	 memset(szStr, 0x00, sizeof(szStr));
	strcpy(szStr,"PRINT TESTING END");
	inCCTOS_PrinterBufferOutputStatus(szStr,&stgFONT_ATTRIB,1);
			
      vdDebug_LogPrintf("888888888");
	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	  inCCTOS_PrinterBufferOutputStatus(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

	  vdDebug_LogPrintf("99999999");

	  vdCTOSS_PrinterEndStatus();


      return (ST_SUCCESS);
}

int inPrintTesting1(void)
{
	BYTE bKey;
	

#define WIDTH 384
#define HEIGTH 70
#define TEXT_BMP		"/home/ap/pub/Text_BMP.bmp"

				CTOS_LCDTClearDisplay();
                vdCTOSS_TextBufferToBMP(WIDTH, HEIGTH,1, 1,"SGD 5.69",d_FONT_12x24,TEXT_BMP);
				displaybmpEx(1,1,"Text_BMP.bmp");
				CTOS_Delay(1000);
				vdCTOSS_RotateBMPFile(TEXT_BMP,0);
				//displaybmpEx(1,1,"Text_BMP.bmp");
				//CTOS_Delay(1000);
				CTOS_LCDTClearDisplay();
				vdCTOSS_CombineBMP("/home/ap/pub/SIGNPAD_LEFT.bmp", TEXT_BMP,"/home/ap/pub/SIGNPAD_LEFT_New.bmp",0, 0, HEIGTH-1, WIDTH-1,0, 0);
				displaybmpEx(1,1,"SIGNPAD_LEFT_New.bmp");
				CTOS_KBDGet(&bKey);
            	
				CTOS_LCDTClearDisplay();
                vdCTOSS_TextBufferToBMP(WIDTH, HEIGTH,30, 30,"SGD 8,977.69",d_FONT_16x30,TEXT_BMP);
				displaybmpEx(1,1,"Text_BMP.bmp");
				CTOS_Delay(1000);
				vdCTOSS_RotateBMPFile(TEXT_BMP,0);
				//displaybmpEx(1,1,"Text_BMP.bmp");
				//CTOS_Delay(1000);
				CTOS_LCDTClearDisplay();
				vdCTOSS_CombineBMP("/home/ap/pub/SIGNPAD_LEFT.bmp", TEXT_BMP,"/home/ap/pub/SIGNPAD_LEFT_New.bmp",0, 0, HEIGTH-1, WIDTH-1,0, 0);
				displaybmpEx(1,1,"SIGNPAD_LEFT_New.bmp");
				CTOS_KBDGet(&bKey);
            	
				CTOS_LCDTClearDisplay();
                vdCTOSS_TextBufferToBMP(WIDTH, HEIGTH,50, 20,"SGD 11.79",d_FONT_24x24,TEXT_BMP);
				displaybmpEx(1,1,"Text_BMP.bmp");
				CTOS_Delay(1000);
				vdCTOSS_RotateBMPFile(TEXT_BMP,0);
				//displaybmpEx(1,1,"Text_BMP.bmp");
				//CTOS_Delay(1000);
				CTOS_LCDTClearDisplay();
				vdCTOSS_CombineBMP("/home/ap/pub/SIGNPAD_LEFT.bmp", TEXT_BMP,"/home/ap/pub/SIGNPAD_LEFT_New.bmp",0, 0, HEIGTH-1, WIDTH-1,0, 0);
				displaybmpEx(1,1,"SIGNPAD_LEFT_New.bmp");
				CTOS_KBDGet(&bKey);

}
#endif

void vdCTOS_PrintTerminalConfigNew(void){
    char szPrint[MAX_CHAR_PER_LINE+1];
    char szTemp[MAX_CHAR_PER_LINE+1];
    char szStr[MAX_CHAR_PER_LINE+1];
    int inHostCnt = 0;
    int i = 0, j = 0;
    int inMMTCnt = 0;
    int inMinCDT = 0, inMaxCDT = 0;
    int inRet = 0;
    
    inHostCnt = inHDTMAX();
    inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
    for(i=0; i <= inHostCnt; i++){
        memset(szPrint, 0x00, sizeof(szPrint));
        memset(szTemp, 0x00, sizeof(szTemp));
        strcpy(szTemp, ": ");
        if(inHDTRead(i) == d_OK){
            vdDebug_LogPrintf("CEK Print");
            inCPTRead(strHDT.inHostIndex);
            inMMTReadRecord(strHDT.inHostIndex, 1);
            inCPTRead(strHDT.inHostIndex);
            
//            strcat(szTemp, strHDT.szHostLabel);
//            sprintf(szPrint,"%-*s%-*s", 42/2, "HOST", 42/2, szTemp);
//            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
            sprintf(szPrint, "%s: %s", "HOST", strHDT.szHostLabel);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
//            vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
            
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            strcpy(szTemp, ": ");
            strcat(szTemp, strMMT[0].szTID);
            sprintf(szPrint, "%-*s%-*s", 42/3, "TERMINAL ID", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
            
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            strcpy(szTemp, ": ");
            strcat(szTemp, strMMT[0].szMID);
            sprintf(szPrint, "%-*s%-*s", 42/3, "MERCHANT ID", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
            
            memset(szStr, 0x00, sizeof(szStr));
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            strcpy(szTemp, ": ");
            wub_hex_2_str(strHDT.szTPDU, szStr, TPDU_BCD_SIZE);
            strcat(szTemp, szStr);
            sprintf(szPrint, "%-*s%-*s", 42/3, "TPDU", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
            
            memset(szStr, 0x00, sizeof(szStr));
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            strcpy(szTemp, ": ");
            wub_hex_2_str(strHDT.szNII, szStr, NII_BYTES);
            strcat(szTemp, szStr);
            sprintf(szPrint, "%-*s%-*s", 42/3, "NII", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
            
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            strcpy(szTemp, ": ");
            strcat(szTemp, strCPT.szPriTxnHostIP);
            sprintf(szPrint, "%-*s%-*s", 42/3, "PRI IP", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
            
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            strcpy(szTemp, ": ");
            strcat(szTemp, strCPT.szSecTxnHostIP);
            sprintf(szPrint, "%-*s%-*s", 42/3, "SEC IP", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
            
            memset(szStr, 0x00, sizeof(szStr));
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            sprintf(szTemp, ": %d", strCPT.inPriTxnHostPortNum);
            sprintf(szPrint, "%-*s%-*s", 42/3, "PRI PORT", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
            
            memset(szStr, 0x00, sizeof(szStr));
            memset(szTemp, 0x00, sizeof(szTemp));
            memset(szPrint, 0x00, sizeof(szPrint));
            sprintf(szTemp, ": %d", strCPT.inSecTxnHostPortNum);
            sprintf(szPrint, "%-*s%-*s", 42/3, "SEC PORT", 42/3, szTemp);
            inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
        }
    }
    memset(szPrint, 0x00, sizeof(szPrint));
    strcpy(szPrint, " ");
    inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
    
    memset(szPrint, 0x00, sizeof(szPrint));
    strcpy(szPrint, "------------TABLEPAY DISABLED-----------");
    inCCTOS_PrinterBufferOutput(szPrint,&stgFONT_ATTRIB,1);
    
    for(i = 0; i <= inHostCnt; i++){
        inCDTReadbyHost(i, &inMinCDT, &inMaxCDT);
        for (j = inMinCDT; j <= inMaxCDT; j++) {
            if (inCDTReadDisabled(j, i) == d_OK) {
                memset(szTemp, 0x00, sizeof (szTemp));
                memset(szPrint, 0x00, sizeof (szPrint));
                sprintf(szTemp, "%s-%s", strCDT.szPANLo, strCDT.szPANHi);
                sprintf(szPrint, "%-*s%-*s", 42 / 2, strCDT.szCardLabel, 42 / 2, szTemp);
                inCCTOS_PrinterBufferOutput(szPrint, &stgFONT_ATTRIB, 1);
            }
        }
    }
}

BYTE szErrRespMsg[64+1];
USHORT ushCTOS_MPUPrintErrorReceipt(void)
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	
	char szStr[d_LINE_SIZE + 1];
   char szTemp[d_LINE_SIZE + 1];
   char szTemp1[d_LINE_SIZE + 1];
   char szTemp3[d_LINE_SIZE + 1];
   char szTemp4[d_LINE_SIZE + 1];
   char szTemp5[d_LINE_SIZE + 1];
   char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
   int inFmtPANSize;
   BYTE baTemp[PAPER_X_SIZE * 64];
   CTOS_FONT_ATTRIB stFONT_ATTRIB;
   int num,i,inResult;
   unsigned char tucPrint [24*4+1];    
   BYTE   EMVtagVal[64];
   USHORT EMVtagLen; 
   short spacestring;
   int inRet = 0;

   int inMsgLen = 0;

   
	char szTempMsg[32 + 1];
	memset(szTempMsg, 0x00, sizeof(szTempMsg));
    #if 0
    /*for MPU application only*/
    if (VS_TRUE != fGetMPUTrans())
        return d_OK; 
    #endif   

	#if 0
   	inRet = get_env_int("#ERRRCPT");
   	vdDebug_LogPrintf("ushCTOS_MPUPrintErrorReceipt AAAA %d %d", srTransRec.HDTid, inRet);
	   	if(1 != inRet)
       return d_OK;
 	#endif  

	vdDebug_LogPrintf("ushCTOS_MPUPrintErrorReceipt [%d][ %s][ %d][%s][%d]", srTransRec.HDTid,  srTransRec.szRespCode, strlen(srTransRec.szRespCode), szErrRespMsg, strlen(szErrRespMsg));
   

    // for "[#650] MC/Vs can print error receipt as MPU cards"
    #if 0
    if(srTransRec.HDTid != 6 && srTransRec.HDTid != 7 && srTransRec.HDTid != 17 && srTransRec.HDTid != 18 && srTransRec.HDTid != 19 && srTransRec.HDTid != 20 && srTransRec.HDTid != 21
		&& srTransRec.HDTid != 22 && srTransRec.HDTid != 23)
        return d_OK;
	#endif

	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);

    //if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    //	return -1;

	memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
	
	DebugAddHEX("LastInvoiceNum", strTCT.szLastInvoiceNo,3);
	
    if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    	return -1;

	if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  
	DebugAddSTR("ushCTOS_printAll","print...",20);
		

	ushCTOS_PrintHeader(d_FIRST_PAGE);

	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
	vdPrintCenter(szTemp1);
	
	printDateTime();

    printTIDMID(); 
	
	printBatchInvoiceNO();
	
	szGetTransTitle(srTransRec.byTransType, szStr);     
		vdPrintTitleCenter(szStr);    
    if(srTransRec.byTransType == SETTLE)
    {
        DebugAddSTR("settle","print...",20);
    }
    else
    {
       // vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	if (strTCT.inFontFNTMode == 1)
	{
	    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
   	}
    	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    	EMVtagLen = 0;
    	memset(szStr, ' ', d_LINE_SIZE);
    	vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

    	sprintf(szStr, "%s", srTransRec.szCardLable);
    	vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
    	
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

    	cardMasking(srTransRec.szPAN, PRINT_CARD_MASKING_1);
    	strcpy(szTemp4, srTransRec.szPAN);
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

        //vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        if (strTCT.inFontFNTMode == 1)
		{
		    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		}
		else
		{
	    //use ttf print
		    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	   	}
    	//Exp date and Entry mode
    	memset(szStr, ' ', d_LINE_SIZE);
    	memset(szTemp, 0, sizeof(szTemp));
    	memset(szTemp1, 0, sizeof(szTemp1));
    	memset(szTemp4, 0, sizeof(szTemp4));
    	memset(szTemp5, 0, sizeof(szTemp5));
    	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
    	DebugAddSTR("EXP",szTemp,12);  
    	
    	for (i =0; i<4;i++)
    		szTemp[i] = '*';
    	memcpy(szTemp4,&szTemp[0],2);
    	memcpy(szTemp5,&szTemp[2],2);

    	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
    		memcpy(szTemp1,"Chip",4);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
    		memcpy(szTemp1,"Manual",6);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
    		memcpy(szTemp1,"MSR",3);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
    		memcpy(szTemp1,"Fallback",8);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
            if(srTransRec.bWaveSID == d_VW_SID_CUP_EMV)
            {
                memcpy(szTemp1,"QuickPass",9);
            }
		}
    	memset (baTemp, 0x00, sizeof(baTemp));
    	sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
    	
    	DebugAddSTR("ENT:",baTemp,12);  
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);

    		
     	memset (baTemp, 0x00, sizeof(baTemp));
    	stFONT_ATTRIB.FontSize = 0x1010;
        stFONT_ATTRIB.X_Zoom = DOUBLE_SIZE;       // The width magnifies X_Zoom diameters
        stFONT_ATTRIB.Y_Zoom = DOUBLE_SIZE;       // The height magnifies Y_Zoom diameters

        stFONT_ATTRIB.X_Space = 0;      // The width of the space between the font with next font

    }
                
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 0);

        //TVR
        EMVtagLen = 5;
        memset(EMVtagVal, 0x00, sizeof (EMVtagVal));
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
        memset(szStr, ' ', d_LINE_SIZE);
        if (strTCT.inFontFNTMode == 1)
            sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
        else
            //use ttf print
            sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
        memset(baTemp, 0x00, sizeof (baTemp));
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        //inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        inCCTOS_PrinterBufferOutput(szStr, &stgFONT_ATTRIB, 0);

        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 0);


	//http://118.201.48.214:8080/issues/75.41 #2
	//2)To remove "00" in the angle bracket of RESP CODE if error codes are returned by terminal, not from host
	if(strlen(szErrRespMsg) == 0)	
		memset( srTransRec.szRespCode,0x00,sizeof( srTransRec.szRespCode));

	memset(szStr, 0x00, sizeof(szStr));
	if (srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH)
		sprintf(szStr, "RESP CODE [%s]", "");
	else
		sprintf(szStr, "RESP CODE [%s]", srTransRec.szRespCode);
	vdPrintCenter(szStr);

	memset(szStr, 0x00, sizeof(szStr));
	sprintf(szStr, "[%s]", szErrRespMsg);
	//vdPrintCenter(szStr);

	
	inMsgLen = strlen(szStr);
	memcpy(szTempMsg, szStr, 32);
	if(inMsgLen > 32)
	{
		inCCTOS_PrinterBufferOutput(szTempMsg,&stgFONT_ATTRIB, 0);
			
		memset(szTempMsg, 0x00, sizeof(szTempMsg));
	
		memcpy(szTempMsg, &szStr[32], inMsgLen-32);
		vdPrintCenter(szTempMsg);
	}
	else
	{
		vdPrintCenter(szStr);
	}

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szStr, "%s", "EXPECT  00");
	
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
//        vdPrintTitleCenter("EXPECT 00");

//	memset(szStr, ' ', d_LINE_SIZE);
//	memset(szTemp, ' ', d_LINE_SIZE);
//	memset(szTemp1, ' ', d_LINE_SIZE);
//	
//	sprintf(szStr, "%s", "PLS TRY AGAIN");
//	
//	memset (baTemp, 0x00, sizeof(baTemp));
//	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
//	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	vdPrintTitleCenter("****ERROR*****");

	//CTOS_PrinterFline(d_LINE_DOT * 6);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
	//inSetTextMode();
    return (d_OK);
}

USHORT ushCTOS_CUPPrintErrorReceipt(void) //@@IBR ADD 20170130
{
    USHORT result;
    BYTE   key;
    BOOL   needSecond = TRUE;
	
	char szStr[d_LINE_SIZE + 1];
   char szTemp[d_LINE_SIZE + 1];
   char szTemp1[d_LINE_SIZE + 1];
   char szTemp3[d_LINE_SIZE + 1];
   char szTemp4[d_LINE_SIZE + 1];
   char szTemp5[d_LINE_SIZE + 1];
   char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
   int inFmtPANSize;
   BYTE baTemp[PAPER_X_SIZE * 64];
   CTOS_FONT_ATTRIB stFONT_ATTRIB;
   int num,i,inResult;
   unsigned char tucPrint [24*4+1];    
   BYTE   EMVtagVal[64];
   USHORT EMVtagLen; 
   short spacestring;
   int inRet = 0;

   int inMsgLen = 0;

   
	char szTempMsg[32 + 1];
	memset(szTempMsg, 0x00, sizeof(szTempMsg));

		vdDebug_LogPrintf("ushCTOS_MPUPrintErrorReceipt AAAA %d %d %d", srTransRec.HDTid, inRet, srTransRec.byTransType);

        if(srTransRec.byTransType != CUP_SALE)
            return d_OK;

#if 0   
   inRet = get_env_int("#ERRRCPT");
   if(1 != inRet)
       return d_OK;

#endif

	vdDebug_LogPrintf("ushCTOS_MPUPrintErrorReceipt BBBB %d %d %d", srTransRec.HDTid, inRet, srTransRec.byTransType);


	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);

    //if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    //	return -1;

	memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
	
	if((inResult = inTCTSave(1)) != ST_SUCCESS)
    {
		vdDisplayErrorMsg(1, 8, "Update TCT fail");
    }
	
	DebugAddHEX("LastInvoiceNum", strTCT.szLastInvoiceNo,3);
	
    if( printCheckPaper()==-1)	// Save last invoice first, then check for paper. Otherwise reprint last receipt is wrong
    	return -1;

	if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  
	DebugAddSTR("ushCTOS_printAll","print...",20);
		

	ushCTOS_PrintHeader(d_FIRST_PAGE);

	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
	vdPrintCenter(szTemp1);
	
	printDateTime();

    printTIDMID(); 
	
	printBatchInvoiceNO();
	
	szGetTransTitle(srTransRec.byTransType, szStr);     
		vdPrintTitleCenter(szStr);    
    if(srTransRec.byTransType == SETTLE)
    {
        DebugAddSTR("settle","print...",20);
    }
    else
    {
       // vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	if (strTCT.inFontFNTMode == 1)
	{
	    vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
   	}
    	memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
    	EMVtagLen = 0;
    	memset(szStr, ' ', d_LINE_SIZE);
    	vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

    	sprintf(szStr, "%s", srTransRec.szCardLable);
    	vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
    	
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

    	cardMasking(srTransRec.szPAN, PRINT_CARD_MASKING_1);
    	strcpy(szTemp4, srTransRec.szPAN);
    	memset (baTemp, 0x00, sizeof(baTemp));
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

        //vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        if (strTCT.inFontFNTMode == 1)
		{
		    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		}
		else
		{
	    //use ttf print
		    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	   	}
    	//Exp date and Entry mode
    	memset(szStr, ' ', d_LINE_SIZE);
    	memset(szTemp, 0, sizeof(szTemp));
    	memset(szTemp1, 0, sizeof(szTemp1));
    	memset(szTemp4, 0, sizeof(szTemp4));
    	memset(szTemp5, 0, sizeof(szTemp5));
    	wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
    	DebugAddSTR("EXP",szTemp,12);  
    	
    	for (i =0; i<4;i++)
    		szTemp[i] = '*';
    	memcpy(szTemp4,&szTemp[0],2);
    	memcpy(szTemp5,&szTemp[2],2);

    	if(srTransRec.byEntryMode==CARD_ENTRY_ICC)
    		memcpy(szTemp1,"Chip",4);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
    		memcpy(szTemp1,"Manual",6);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
    		memcpy(szTemp1,"MSR",3);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
    		memcpy(szTemp1,"Fallback",8);
    	else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
            if(srTransRec.bWaveSID == d_VW_SID_CUP_EMV)
            {
                memcpy(szTemp1,"QuickPass",9);
            }
		}
    	memset (baTemp, 0x00, sizeof(baTemp));
    	sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
    	
    	DebugAddSTR("ENT:",baTemp,12);  
    	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
    	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);

    		
     	memset (baTemp, 0x00, sizeof(baTemp));
    	stFONT_ATTRIB.FontSize = 0x1010;
        stFONT_ATTRIB.X_Zoom = DOUBLE_SIZE;       // The width magnifies X_Zoom diameters
        stFONT_ATTRIB.Y_Zoom = DOUBLE_SIZE;       // The height magnifies Y_Zoom diameters

        stFONT_ATTRIB.X_Space = 0;      // The width of the space between the font with next font

    }
                
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        
        //TVR
        EMVtagLen = 5;
        memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
        memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
        memset(szStr, ' ', d_LINE_SIZE);
        if (strTCT.inFontFNTMode == 1)
                sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
        else
                //use ttf print
        sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
        memset (baTemp, 0x00, sizeof(baTemp));		
        //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
        //inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

	//http://118.201.48.214:8080/issues/75.41 #2
	//2)To remove "00" in the angle bracket of RESP CODE if error codes are returned by terminal, not from host
	if(strlen(szErrRespMsg) == 0)	
		memset( srTransRec.szRespCode,0x00,sizeof( srTransRec.szRespCode));

	memset(szStr, 0x00, sizeof(szStr));
	if (srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH)
		sprintf(szStr, "RESP CODE [%s]", "");
	else
		sprintf(szStr, "RESP CODE [%s]", srTransRec.szRespCode);
	vdPrintCenter(szStr);

	memset(szStr, 0x00, sizeof(szStr));
	sprintf(szStr, "[%s]", szErrRespMsg);
	//vdPrintCenter(szStr);

	
	inMsgLen = strlen(szStr);
	memcpy(szTempMsg, szStr, 32);
	if(inMsgLen > 32)
	{
		inCCTOS_PrinterBufferOutput(szTempMsg,&stgFONT_ATTRIB, 0);
			
		memset(szTempMsg, 0x00, sizeof(szTempMsg));
	
		memcpy(szTempMsg, &szStr[32], inMsgLen-32);
		vdPrintCenter(szTempMsg);
	}
	else
	{
		vdPrintCenter(szStr);
	}

	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);
	memset(szTemp1, ' ', d_LINE_SIZE);
	sprintf(szStr, "%s", "EXPECT  00");
	
	memset (baTemp, 0x00, sizeof(baTemp));
	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
//        vdPrintTitleCenter("EXPECT 00");

//	memset(szStr, ' ', d_LINE_SIZE);
//	memset(szTemp, ' ', d_LINE_SIZE);
//	memset(szTemp1, ' ', d_LINE_SIZE);
//	
//	sprintf(szStr, "%s", "PLS TRY AGAIN");
//	
//	memset (baTemp, 0x00, sizeof(baTemp));
//	//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stFONT_ATTRIB);
//	//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	vdPrintTitleCenter("****ERROR*****");

	//CTOS_PrinterFline(d_LINE_DOT * 6);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
	//inSetTextMode();
    return (d_OK);
}


int inMPU_PrintSettleReport(void)
{
	ACCUM_REC srAccumRec;
	unsigned char chkey;
	short shHostIndex;
	int inResult;
	int inTranCardType;
	int inReportType;
	int i;		
	char szStr[d_LINE_SIZE + 1];
	BYTE baTemp[PAPER_X_SIZE * 64];
        char szExchangeRate[d_LINE_SIZE+1];

	inCTLOS_Updatepowrfail(PFR_BATCH_SETTLEMENT_PRINT);
	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return (d_OK);

	if( printCheckPaper()==-1)
		return;

	inResult = inCTOS_ChkBatchEmpty();
	if(d_OK != inResult)
	{
		return;
	}
    
	memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
    if((inResult = inCTOS_ReadAccumTotal(&srAccumRec)) == ST_ERROR)
    {
        vdMyEZLib_LogPrintf("[vdUpdateAmountTotal]---Read Total Rec. error");
		vdSetErrorMessage("Read Accum Error");
        return ST_ERROR;    
    }
    
    if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
	vdCTOSS_PrinterStart(100);
	CTOS_PrinterSetHeatLevel(4);
	//start capture receipt
    //inInitializePrinterBufferLib();
	//inSIGPStartCapturePrinterDataLib();


	ushCTOS_PrintHeader(0);	
    
    vdPrintTitleCenter("SETTLEMENT");
	//CTOS_PrinterFline(d_LINE_DOT * 1);
	//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

    printHostLabel();
    
	printTIDMID();
    
	printDateTime();
    
	printBatchNO();

	
																// fix for case #1346 122319
    if (fGetMPUTrans() == TRUE && (strHDT.inHostIndex == 17) || srTransRec.HDTid == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX 
		|| srTransRec.HDTid == 23) {//marco 20170704        
        inCSTRead(1);
    } else {
        inCSTRead(strHDT.inCurrencyIdx);
    }    

    //CTOS_PrinterFline(d_LINE_DOT * 1);
	//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	for(inTranCardType = 1; inTranCardType < 2 ;inTranCardType ++)
	{
		inReportType = PRINT_CARD_TOTAL;
		
		if(inReportType == PRINT_CARD_TOTAL)
		{
			for(i= 0; i <20; i ++ )
			{
				vdDebug_LogPrintf("--Count[%d]", i);
				if((srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount == 0)
				&&(srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount == 0))
					continue;
				
				vdDebug_LogPrintf("Count[%d]", i); 
				inIITRead(i);
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,strIIT.szIssuerLabel);
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usSaleCount - srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

				
								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),

				srAccumRec.stBankTotal[inTranCardType].stCardTotal[i]);	
			}
			//after print issuer total, then print host toal
			{
			
				memset(szStr, ' ', d_LINE_SIZE);
				memset (baTemp, 0x00, sizeof(baTemp));
				strcpy(szStr,"TOTAL:");
				//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				
				vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                      
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                        
                                srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

								srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),

                                srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
			}
		}
		else
		{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			strcpy(szStr,srTransRec.szHostLabel);
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			
			vdCTOS_PrintAccumeByHostAndCard (inReportType, 
                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usSaleCount - srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount,   (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulSaleTotalAmount) - (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usRefundCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulRefundTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usVoidSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulVoidSaleTotalAmount), 

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.usOffSaleCount, (srAccumRec.stBankTotal[inTranCardType].stHOSTTotal.ulOffSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCashAdvCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCashAdvTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPSaleTotalAmount),
                                
                        srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCUPPreAuthCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCUPPreAuthTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usCBPaySaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulCBPaySaleTotalAmount),

						srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].usOKDSaleCount, (srAccumRec.stBankTotal[inTranCardType].stCardTotal[i].ulOKDSaleTotalAmount),

                        srAccumRec.stBankTotal[inTranCardType].stHOSTTotal);		
		}
	}
        
        if(strTCT.inExchangeRate > 0){
            memset(szStr, 0x00, sizeof(szStr));
            memset(szExchangeRate, 0x00, sizeof(szExchangeRate));
            sprintf(szExchangeRate, "%d", strTCT.inExchangeRate);
            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szExchangeRate, szStr);
            
            memset(szExchangeRate, 0x00, sizeof(szExchangeRate));
            sprintf(szExchangeRate, "MMK %s", szStr);
            
            inCCTOS_PrinterBufferOutput(szExchangeRate,&stgFONT_ATTRIB,1);
        }
    
	//print space one line
	//CTOS_PrinterFline(d_LINE_DOT * 2);		
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//CTOS_PrinterFline(d_LINE_DOT * 2);
	//inCTOSS_PrintERMInfo();
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        vdPrintCenter("SETTLEMENT SUCCESSFUL");
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();

//end capture receipt
	//inSIGPEndCapturePrinterDataLib();
	inCTOSS_ERM_Form_Receipt(1);
	
	return d_OK;	
}

void vdCTOS_SetFontType(char *chFontType) {
    
    inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
    
    CTOS_PrinterTTFSelect(chFontType, 0);
    inCTOSS_SetERMFontType(chFontType, 0);
    CTOS_PrinterTTFSwichDisplayMode(d_TTF_MODE_DEFAULT);

//    inSetLanguageDatabase(chFontType);
//    inCTOSS_SetALLApFont(chFontType);
    
    if(strcmp(chFontType, d_FONT_DEFAULT_TTF) == 0){
        inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
        
}

void vdDebugPrint(BYTE *byPrint, int inPrintLen){
    inCCTOS_PrinterBufferOutput(byPrint,&stgFONT_ATTRIB,1);
}


void vdPrintCenterTerminalConfig(unsigned char *strIn)
{
	unsigned char tucPrint [24*4+1];
	short i,spacestring;
    USHORT usCharPerLine = 32;
    BYTE baTemp[PAPER_X_SIZE * 64];
    
    vdDebug_LogPrintf("vdPrintCenter [%s]", strIn);
	vdDebug_LogPrintf("strTCT.inFontFNTMode [%d]", strTCT.inFontFNTMode);

	if (strTCT.inFontFNTMode != 1)
	{
		vdTTFPrintCenter(strIn);
		return;
	}

	
	vdDebug_LogPrintf("strTCT.inFontFNTMode [%d]", strTCT.inFontFNTMode);

    if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 32;
    else if(d_FONT_24x24 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 16;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && NORMAL_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 48;
    else if(d_FONT_16x16 == stgFONT_ATTRIB.FontSize && DOUBLE_SIZE == stgFONT_ATTRIB.X_Zoom)
        usCharPerLine = 24;
    else
        usCharPerLine = 32;

	
    vdDebug_LogPrintf("usCharPerLine [%d]", usCharPerLine);
        
    i = strlen(strIn);

	
    vdDebug_LogPrintf("strIn length [%d]", i);
	spacestring=(usCharPerLine-i)/2;

	
    vdDebug_LogPrintf("spacestring[%d]", spacestring);
				
	memset(tucPrint,0x20,55);
	memcpy(tucPrint+spacestring,strIn,usCharPerLine);	
	
	tucPrint[i+spacestring]=0;
    memset (baTemp, 0x00, sizeof(baTemp));
      
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, tucPrint, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

	//inCTOSS_CapturePrinterBuffer(tucPrint,&stgFONT_ATTRIB);
//    vdDebug_LogPrintf("tucPrint [%s]", tucPrint);
//	inCCTOS_PrinterBufferOutput(tucPrint,&stgFONT_ATTRIB,1);
}

USHORT ushCTOS_TerminalConfigPrintHeader(void) {

    vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]", srTransRec.HDTid, srTransRec.MITid);
    if (inMMTReadRecord(21, 1) != d_OK) {
		
    vdDebug_LogPrintf("LOAD MMT ERR");
//        vdSetErrorMessage("LOAD MMT ERR");
 //       return (d_NO);
    }

    //merhcant name and address
    
    vdDebug_LogPrintf("sstrlen(strTCT.szMerchantName)[%d]", strlen(strTCT.szMerchantName));
    vdDebug_LogPrintf("(strTCT.szMerchantName)[%s]", (strTCT.szMerchantName));

	if(strlen(strTCT.szMerchantName) > 0)
            vdPrintCenterTerminalConfig(strTCT.szMerchantName);


	
    vdDebug_LogPrintf("sstrlen(strTCT.szMerchantAddress)[%d]", strTCT.szMerchantAddress);
    vdDebug_LogPrintf("(strTCT.szMerchantAddress)[%s]", (strTCT.szMerchantAddress));

	vdPrintLongMessageCenterTerminalConfig(strTCT.szMerchantAddress);
        
//    if (strlen(strTCT.szMerchantAddress1) > 0)
//        vdPrintCenter(strTCT.szMerchantAddress1);
//    if (strlen(strTCT.szMerchantAddress2) > 0)
//        vdPrintCenter(strTCT.szMerchantAddress2);
//    if (strlen(strTCT.szMerchantAddress3) > 0)
//        vdPrintCenter(strTCT.szMerchantAddress3);
//    if (strlen(strTCT.szMerchantAddress4) > 0)
//        vdPrintCenter(strTCT.szMerchantAddress4);
//    if (strlen(strTCT.szMerchantAddress5) > 0)
//        vdPrintCenter(strTCT.szMerchantAddress5);

    return d_OK;

}

USHORT ushCTOS_PrintBodyAlipay(int page)
{	
    char szStr[d_LINE_SIZE*2 + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
	char szTemp2[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
	char szTmpPan[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;

	BYTE	txn_prefix[16 + 1];
	BYTE	txn_title[d_LINE_SIZE*2 + 3];

	if(d_FIRST_PAGE == page)
	{
		#if 0
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	vdPrintCenter(szTemp1);
		#endif
		
		//printDateTime();

        printTIDMID(); 
		printDateTime();
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
		//printBatchInvoiceNO();

		/*add prefix string for print, alipay and wechat */
		/*add prefix string for print, alipay and wechat */
		memset(txn_prefix, 0x00, sizeof(txn_prefix));
		//if (ALIPAY_HOST_INDEX == srTransRec.HDTid)
			//strcpy(txn_prefix, "   ALIPAY ");
		//else if (WPAY_HOST_INDEX == srTransRec.HDTid)
		if (WPAY_HOST_INDEX == srTransRec.HDTid)
			strcpy(txn_prefix, "   WECHAT ");
		else if (QQWALLET_HOST_INDEX == srTransRec.HDTid)
			strcpy(txn_prefix, "   QQ ");
		
    	szGetTransTitle(srTransRec.byTransType, szStr);

		memset(txn_title, 0x00, sizeof(txn_title));
		if (strlen(txn_prefix) > 0)
		{
			strcpy(txn_title, txn_prefix);
			strcat(txn_title, szStr);
		}
		else
		{
			strcpy(txn_title, szStr);
		}
  		vdPrintTitleCenter(txn_title);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
#if 0
		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

		sprintf(szStr, "%s", srTransRec.szCardLable);
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(srTransRec.szPAN,szTmpPan,d_LINE_SIZE,page);
		strcpy(szTemp4, szTmpPan);
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szTemp4,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        
		//Exp date and Entry mode
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0, sizeof(szTemp));
		memset(szTemp1, 0, sizeof(szTemp1));
		memset(szTemp4, 0, sizeof(szTemp4));
		memset(szTemp5, 0, sizeof(szTemp5));
		wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
		DebugAddSTR("EXP",szTemp,12);
		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(szTemp,szTmpPan,(EXP_DATE_SIZE + 1),page);
		
		//for (i =0; i<4;i++)
		//	szTemp[i] = '*';
		memcpy(szTemp4,&szTmpPan[0],2);
		memcpy(szTemp5,&szTmpPan[2],2);

		if(srTransRec.byEntryMode==CARD_ENTRY_ICC || srTransRec.byEntryMode==CARD_ENTRY_EASY_ICC)
			memcpy(szTemp1,"Chip",4);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
			memcpy(szTemp1,"Manual",6);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"MSR",3);
		else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
			memcpy(szTemp1,"Fallback",8);
		else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
		}

		vdDebug_LogPrintf("PAT ENTRY MODE [%ld] [%s]", srTransRec.byEntryMode, srTransRec.szPAN);	 
		
		memset (baTemp, 0x00, sizeof(baTemp));
		sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
		
		DebugAddSTR("ENT:",baTemp,12);  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szTemp,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);
#endif

		if(srTransRec.byTransType == REFUND)
		{
			if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
			{
				memset(szStr, 0x00, d_LINE_SIZE);
				//strcpy(szStr,"原参考号ORI RRN:");
                                strcpy(szStr,"原參考號ORI RRN:");
				//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

				memset(szStr, ' ', d_LINE_SIZE);
				sprintf(szStr, "%s", srTransRec.szCardLable);
				inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
			}
			else
			{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));					
			memset(szStr, ' ', d_LINE_SIZE);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "ORI RRN  : %s", srTransRec.szCardLable);
			//else
				//use ttf print
			//sprintf(szStr, "ORI EFTP RRN    : %s", srTransRec.szCardLable);
			//memset (baTemp, 0x00, sizeof(baTemp));		
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);
			}
		}

		if (srTransRec.byTransType == SALE)
		{	
			#if 0
			if (strTCT.inFontFNTMode == 1)
       			vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
			else
			//use ttf print
	    		vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
			#endif
		}
		
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"参考号  RRN:");
                        strcpy(szStr,"參考號  RRN:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szRRN);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		//Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		//if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "RRN  : %s", srTransRec.szRRN);
		//else
			//use ttf print
		//sprintf(szStr, "EFTP RRN    : %s", srTransRec.szRRN);
		//memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);

		}
		
		if (srTransRec.byTransType == SALE)
		{
			if (strTCT.inFontFNTMode == 1)
        		vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
			else
				//use ttf print
	    	vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		}

		#if 0
		if (srTransRec.byTransType == SALE)
		//CTOS_PrinterCode128Barcode(60, 2, srTransRec.szRRN, strlen(srTransRec.szRRN), 2 ,3, TRUE);
		vdCTOSS_PrinterBufferCode128Barcode(60, 2, srTransRec.szRRN, strlen(srTransRec.szRRN), 2 ,3, TRUE);
		#endif
#if 0
                if (srTransRec.byTransType == VOID)
		{
			if (strTCT.inThemesType == 2)
			{
				memset(szStr, 0x00, d_LINE_SIZE);
				strcpy(szStr,"原交易流水号");
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			}
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
                        memset (szTemp1, 0x00, sizeof(szTemp1));
			memset(szStr, ' ', d_LINE_SIZE);
                        wub_hex_2_str(srTransRec.szInvoiceNo, szTemp1, INVOICE_BCD_SIZE);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "ORI TRACE NO. : %s",szTemp1);
			//else
				//use ttf print
			//sprintf(szStr, "ORI EFTP RRN    : %s", srTransRec.szCardLable);
			memset (baTemp, 0x00, sizeof(baTemp));		
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
				
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			strcpy(szStr,"流水号");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
		//trace num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (szTemp1, 0x00, sizeof(szTemp1));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szTemp1,"%06ld",srTransRec.ulTraceNum);
		//if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "TRACE NO. : %s", szTemp1);
		//else
			//use ttf print
		//sprintf(szStr, "TRACE NO.    : %s", szTemp1);
		memset (baTemp, 0x00, sizeof(baTemp));		
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
#endif

		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"流水号  TRACE NO:");
                        strcpy(szStr,"流水號  TRACE NO:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			memset (szTemp1, 0x00, sizeof(szTemp1));					
			memset(szStr, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szInvoiceNo, szTemp1, INVOICE_BCD_SIZE);
			sprintf(szStr, "%s", szTemp1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		//Invoice num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (szTemp1, 0x00, sizeof(szTemp1));					
		memset(szStr, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szInvoiceNo, szTemp1, INVOICE_BCD_SIZE);
		//if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "TRACE NO. : %s", szTemp1);
		//else
			//use ttf print
		//sprintf(szStr, "TRACE NO.    : %s", szTemp1);
		memset (baTemp, 0x00, sizeof(baTemp));		
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}

		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"授权码  APPR CODE:");
                        strcpy(szStr,"授權碼  APPR CODE:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szAuthCode);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "APPR CODE : %s", srTransRec.szAuthCode);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}

		//Alipay A/C
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			if (srTransRec.HDTid == WPAY_HOST_INDEX)
				//strcpy(szStr,"支付账号WECHAT A/C.:");
                                strcpy(szStr,"支付帳號WECHAT A/C.:");
			else if (srTransRec.HDTid == QQWALLET_HOST_INDEX)
				//strcpy(szStr,"支付账号QQ A/C.:");
                                strcpy(szStr,"支付帳號QQ A/C.:");
			else
				//strcpy(szStr,"支付账号ALIPAY A/C.:");
                                strcpy(szStr,"支付帳號ALIPAY A/C.:");
			
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szTrack1Data);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (srTransRec.HDTid == WPAY_HOST_INDEX)
			sprintf(szStr, "WECHAT A/C.: %s", srTransRec.szTrack1Data);
		else if (srTransRec.HDTid == QQWALLET_HOST_INDEX)
			sprintf(szStr, "QQ A/C.: %s", srTransRec.szTrack1Data);
		else
			sprintf(szStr, "ALIPAY A/C.: %s", srTransRec.szTrack1Data);
		
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}

		//Order No.
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"交易单号ORDER NO.:");
                        strcpy(szStr,"交易單號ORDER NO.:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szTrack2Data);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "ORDER NO.:%s", srTransRec.szTrack2Data);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		
		}

		// POS transaction reference
		if ((inChkPOSTransRefReq() == TRUE) &&		//reuse existing parameter to prevent change in Amex-Alipay
			((srTransRec.byTransType == SALE) || (srTransRec.byTransType == VOID) || (srTransRec.byTransType == REFUND)))
		{
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));					
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "POS TRANS REF: %s", srTransRec.szTrack3Data);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}

		inCCTOS_PrinterBufferOutputAligned(" ", &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
			/* EMV: Revised EMV details printing - start -- jzg */
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD))
			/* EMV: Revised EMV details printing - end -- jzg */
			{
			//AC
			wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "APP CRYPT : %s", szTemp);
			//else
				//use ttf print
			//sprintf(szStr, "APP CRYPT   : %s", szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			//TVR
			EMVtagLen = 5;
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
			memset(szStr, ' ', d_LINE_SIZE);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			//else
				//use ttf print
			//sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			
			//AID
			memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
			vdDebug_LogPrintf("EMVtagLen=[%d]",EMVtagLen);
			
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
			memset(szTemp, ' ', d_LINE_SIZE);
			wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "AID       : %s",szTemp);
			//else
			//use ttf print
			//sprintf(szStr, "AID              : %s",szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			/* EMV: Get Application Label - start -- jzg */
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			/* EMV: Get Application Label - end -- jzg */
			
		}
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);
		memset(szTemp2, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szOrgAmount, szTemp2, AMT_BCD_SIZE);

		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			strcpy(szStr,"交易金額");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
                
		if (strTCT.inFontFNTMode == 1)
       		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		{
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
			//sprintf(szStr, "AMOUNT:%s %13s", strCST.szCurSymbol,szTemp5);
			#if 1
			memset(szStr, 0x00, d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			sprintf(baTemp, "%s %s", strCST.szCurSymbol,szTemp5);
			sprintf(szStr, "AMOUNT:%17s", baTemp);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			#else
			strcpy(szStr, "AMOUNT:");
			memset (baTemp, 0x00, sizeof(baTemp));
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT, 0, 1);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s%s", strCST.szCurSymbol,szTemp5);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT, 1, 1);
			#endif
		}
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
		
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			strcpy(szStr,"實際支付");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
				
		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
		vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		{
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
			//sprintf(szStr, "PAYMENT:%s %12s", strCST.szCurSymbol,szTemp5);
			#if 1
			memset(szStr, 0x00, d_LINE_SIZE);
			memset(baTemp, 0x00, sizeof(baTemp));
			sprintf(baTemp, "%s %s", strCST.szCurSymbol,szTemp5);
			sprintf(szStr, "PAYMENT:%16s", baTemp);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			#else
			strcpy(szStr, "PAYMENT:");
			memset (baTemp, 0x00, sizeof(baTemp));
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT, 0, 1);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s%s", strCST.szCurSymbol,szTemp5);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT, 1, 1);
			#endif
		}
		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
		vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

		/*feed line*/
		inCCTOS_PrinterBufferOutputAligned(" ",&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT, 1, 1);
		if (srTransRec.byTransType == ALIPAY_SALE)
			//CTOS_PrinterCode128Barcode(60, 2, srTransRec.szRRN, strlen(srTransRec.szRRN), 2 ,3, TRUE);
			vdCTOSS_PrinterBufferCode128Barcode(80, 2, srTransRec.szRRN, strlen(srTransRec.szRRN), 2 ,3, TRUE);

	}
	//else if(d_SECOND_PAGE == page)
	else
	{
		#if 0
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	vdPrintCenter(szTemp1);
		#endif
		
		//printDateTime();
		inResult = printTIDMID();
		printDateTime();
		
		//printBatchInvoiceNO(); // pat confirm hang
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
		memset(szStr, ' ', d_LINE_SIZE);
    	szGetTransTitle(srTransRec.byTransType, szStr);   
		/*add prefix string for print, alipay and wechat */
		memset(txn_prefix, 0x00, sizeof(txn_prefix));
		//if (ALIPAY_HOST_INDEX == srTransRec.HDTid)
			//strcpy(txn_prefix, "   ALIPAY ");
		//else if (WPAY_HOST_INDEX == srTransRec.HDTid)
		if (WPAY_HOST_INDEX == srTransRec.HDTid)
			strcpy(txn_prefix, "   WECHAT ");
		else if (QQWALLET_HOST_INDEX == srTransRec.HDTid)
			strcpy(txn_prefix, "   QQ ");
		
    	szGetTransTitle(srTransRec.byTransType, szStr);

		memset(txn_title, 0x00, sizeof(txn_title));
		if (strlen(txn_prefix) > 0)
		{
			strcpy(txn_title, txn_prefix);
			strcat(txn_title, szStr);
		}
		else
		{
			strcpy(txn_title, szStr);
		}
		
  		vdPrintTitleCenter(txn_title);  
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
#if 0
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
		
		sprintf(szStr, "%s",srTransRec.szCardLable);
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,0);

		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(srTransRec.szPAN,szTmpPan,d_LINE_SIZE,page);
		strcpy(szTemp4, szTmpPan);
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,0);

		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        
		//Exp date and Entry mode
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0, sizeof(szTemp));
		memset(szTemp1, 0, sizeof(szTemp1));
		memset(szTemp4, 0, sizeof(szTemp4));
		memset(szTemp5, 0, sizeof(szTemp5));
		wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
		DebugAddSTR("EXP",szTemp,12);
		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(szTemp,szTmpPan,(EXP_DATE_SIZE + 1),page);
		
		//for (i =0; i<4;i++)
		//	szTemp[i] = '*';
		memcpy(szTemp4,&szTmpPan[0],2);
		memcpy(szTemp5,&szTmpPan[2],2);

		if(srTransRec.byEntryMode==CARD_ENTRY_ICC || srTransRec.byEntryMode==CARD_ENTRY_EASY_ICC)
			memcpy(szTemp1,"Chip",4);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
			memcpy(szTemp1,"Manual",6);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"MSR",3);
		else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
			memcpy(szTemp1,"Fallback",8);
		else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
		}
		
		DebugAddSTR("ENT:",szTemp1,12);  
		memset (baTemp, 0x00, sizeof(baTemp));

		memset (baTemp, 0x00, sizeof(baTemp));
		sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
		
		DebugAddSTR("ENT:",baTemp,12);  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,0);
#endif

		if(srTransRec.byTransType == REFUND)
		{
			if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
			{
				memset(szStr, 0x00, d_LINE_SIZE);
				//strcpy(szStr,"原参考号 ORI RRN:");
                                strcpy(szStr,"原参考號 ORI RRN:");
				//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

				memset(szStr, ' ', d_LINE_SIZE);
				sprintf(szStr, "%s", srTransRec.szCardLable);
				inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
			}
			else
			{
		
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));					
			memset(szStr, ' ', d_LINE_SIZE);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "ORI RRN  : %s", srTransRec.szCardLable);
			//else
				//use ttf print
			//sprintf(szStr, "ORI EFTP RRN    : %s", srTransRec.szCardLable);
			//memset (baTemp, 0x00, sizeof(baTemp));		
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);
			}
		}

		if (srTransRec.byTransType == SALE)
		{
			#if 0
			if (strTCT.inFontFNTMode == 1)
       			vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
			else
			//use ttf print
	    		vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
			#endif
		}
		
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"参考号  RRN:");
                        strcpy(szStr,"参考號 RRN:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szRRN);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		//Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		//if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "RRN  : %s", srTransRec.szRRN);
		//else
			//use ttf print
		//sprintf(szStr, "EFTP RRN    : %s", srTransRec.szRRN);
		//memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);

		}


		if (srTransRec.byTransType == SALE)
		{
			if (strTCT.inFontFNTMode == 1)
        		vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
			else
				//use ttf print
	    	vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		}

		#if 0
		if (srTransRec.byTransType == SALE)
		vdCTOSS_PrinterBufferCode128Barcode(60, 2, srTransRec.szRRN, strlen(srTransRec.szRRN), 2 ,3, TRUE);
		#endif
#if 0
		//trace num
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			strcpy(szStr,"流水号");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
		//trace num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (szTemp1, 0x00, sizeof(szTemp1));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szTemp1,"%06ld",srTransRec.ulTraceNum);
		//if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "TRACE NO. : %s", szTemp1);
		//else
			//use ttf print
		//sprintf(szStr, "TRACE NO.    : %s", szTemp1);
		memset (baTemp, 0x00, sizeof(baTemp));		
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			strcpy(szStr,"流水号");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
#endif		
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"流水号  TRACE NO:");
                        strcpy(szStr,"流水號 TRACE NO:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			memset (szTemp1, 0x00, sizeof(szTemp1));					
			memset(szStr, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szInvoiceNo, szTemp1, INVOICE_BCD_SIZE);
			sprintf(szStr, "%s", szTemp1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		//Invoice num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (szTemp1, 0x00, sizeof(szTemp1));					
		memset(szStr, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szInvoiceNo, szTemp1, INVOICE_BCD_SIZE);
		//if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "TRACE NO. : %s", szTemp1);
		//else
			//use ttf print
		//sprintf(szStr, "TRACE NO.    : %s", szTemp1);
		memset (baTemp, 0x00, sizeof(baTemp));		
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}

		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"授权码  APPR CODE:");
                        strcpy(szStr,"授權碼  APPR CODE:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 0, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szAuthCode);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "APPR CODE : %s", srTransRec.szAuthCode);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}

		//Alipay A/C
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			if (srTransRec.HDTid == WPAY_HOST_INDEX)
				//strcpy(szStr,"支付账号WECHAT A/C.:");
                                strcpy(szStr,"支付帳號WECHAT A/C.:");
			else if (srTransRec.HDTid == QQWALLET_HOST_INDEX)
				//strcpy(szStr,"支付账号QQ A/C.:");
                                strcpy(szStr,"支付帳號QQ A/C.:");
			else
				//strcpy(szStr,"支付账号ALIPAY A/C.:");
                                strcpy(szStr,"支付帳號ALIPAY A/C.:");
			
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szTrack1Data);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (srTransRec.HDTid == WPAY_HOST_INDEX)
			sprintf(szStr, "WECHAT A/C.: %s", srTransRec.szTrack1Data);
		else if (srTransRec.HDTid == QQWALLET_HOST_INDEX)
			sprintf(szStr, "QQ A/C.: %s", srTransRec.szTrack1Data);
		else
			sprintf(szStr, "ALIPAY A/C.: %s", srTransRec.szTrack1Data);
		
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}

		//Order No.
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			//strcpy(szStr,"交易单号ORDER NO.:");
                        strcpy(szStr,"交易單號ORDER NO.:");
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNLEFT, 1, 0);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s", srTransRec.szTrack2Data);
			inCCTOS_PrinterBufferOutputAligned(szStr, &stgFONT_ATTRIB, d_PRINTER_ALIGNRIGHT, 1, 0);
		}
		else
		{
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "ORDER NO.:%s", srTransRec.szTrack2Data);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		
		}

		// POS transaction reference
		if ((inChkPOSTransRefReq() == TRUE) &&		//reuse existing parameter to prevent change in Amex-Alipay
			((srTransRec.byTransType == SALE) || (srTransRec.byTransType == VOID) || (srTransRec.byTransType == REFUND)))
		{
			memset(szStr, ' ', d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));					
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "POS TRANS REF: %s", srTransRec.szTrack3Data);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
		
		/*feed line*/
		inCCTOS_PrinterBufferOutputAligned(" ",&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT, 1, 1);
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		if((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
			/* EMV: Revised EMV details printing - start -- jzg */
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_EMV) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_LEGACY2) ||
			(srTransRec.bWaveSID == d_EMVCL_SID_JCB_MSD))
			/* EMV: Revised EMV details printing - end -- jzg */
		{
			//AC
			wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "APP CRYPT : %s", szTemp);
			//else
				//use ttf print
			//sprintf(szStr, "APP CRYPT   : %s", szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,0);

			//TVR
			EMVtagLen = 5;
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
			memset(szStr, ' ', d_LINE_SIZE);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			//else
				//use ttf print
			//sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,0);
			
			//AID
			memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
			vdDebug_LogPrintf("EMVtagLen=[%d]",EMVtagLen);
			
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
			memset(szTemp, ' ', d_LINE_SIZE);
			wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
			//if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "AID       : %s",szTemp);
			//else
			//use ttf print
			//sprintf(szStr, "AID              : %s",szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,0);

			/* EMV: Get Application Label - start -- jzg */
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,0);
			/* EMV: Get Application Label - end -- jzg */
			
		}
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp2, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szOrgAmount, szTemp2, AMT_BCD_SIZE);

                if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			strcpy(szStr,"交易金額");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
                
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
	
		{
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
			//sprintf(szStr, "AMOUNT:%s %13s", strCST.szCurSymbol,szTemp5);
			#if 1
			memset(szStr, 0x00, d_LINE_SIZE);
			memset (baTemp, 0x00, sizeof(baTemp));
			sprintf(baTemp, "%s %s", strCST.szCurSymbol,szTemp5);
			sprintf(szStr, "AMOUNT:%17s", baTemp);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			#else
			strcpy(szStr, "AMOUNT:");
			memset (baTemp, 0x00, sizeof(baTemp));
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT, 0, 1);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s%s", strCST.szCurSymbol,szTemp5);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT, 1, 1);
			#endif
		}

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

		
		if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
		{
			memset(szStr, 0x00, d_LINE_SIZE);
			strcpy(szStr,"實際支付");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		}
				
		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
		vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		{
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
			//sprintf(szStr, "AMOUNT:%s %13s", strCST.szCurSymbol,szTemp5);
			#if 1
			memset(szStr, 0x00, d_LINE_SIZE);
			memset(baTemp, 0x00, sizeof(baTemp));
			sprintf(baTemp, "%s %s", strCST.szCurSymbol,szTemp5);
			sprintf(szStr, "PAYMENT:%16s", baTemp);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			#else
			strcpy(szStr, "PAYMENT:");
			memset (baTemp, 0x00, sizeof(baTemp));
			//inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNLEFT, 0, 1);

			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr, "%s%s", strCST.szCurSymbol,szTemp5);
			inCCTOS_PrinterBufferOutputAligned(szStr,&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT, 1, 1);
			#endif
		}
		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
		vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

		/*feed line*/
		inCCTOS_PrinterBufferOutputAligned(" ",&stgFONT_ATTRIB,d_PRINTER_ALIGNRIGHT, 1, 1);
		/*bar code*/
		if (srTransRec.byTransType == ALIPAY_SALE)
			//CTOS_PrinterCode128Barcode(60, 2, srTransRec.szRRN, strlen(srTransRec.szRRN), 2 ,3, TRUE);
			vdCTOSS_PrinterBufferCode128Barcode(80, 2, srTransRec.szRRN, strlen(srTransRec.szRRN), 2 ,3, TRUE);

	}
	return d_OK;	
	
}

USHORT ushCTOS_PrintFooterAlipay(int page)
{		
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
	
	if(page == d_FIRST_PAGE)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
		    EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			// patrick add code 20141208
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
            {   
                //CTOS_PrinterFline(d_LINE_DOT * 1);
                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
				if (srTransRec.byEntryMode==CARD_ENTRY_ICC)
				{
                	//CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
					//inCTOSS_CapturePrinterBuffer("     (PIN VERIFY SUCCESS)",&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput("     (PIN VERIFY SUCCESS)",&stgFONT_ATTRIB,1);
				}
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        }

		/*Aplipay no need E signature*/
		//ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//inCTOSS_CapturePrinterBuffer("SIGN:___________________________",&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);
		
		printCardHolderName();
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		
		//ushCTOS_PrintAgreement();

		
		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			vdSetGolbFontAttrib(d_FONT_9x18, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		if(strlen(strMMT[0].szRctFoot1) > 0)
			//vdPrintCenter(strMMT[0].szRctFoot1);
			inCCTOS_PrinterBufferOutputAligned(strMMT[0].szRctFoot1,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER, 1, 1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    	//vdPrintCenter(strMMT[0].szRctFoot2);
			inCCTOS_PrinterBufferOutputAligned(strMMT[0].szRctFoot2,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER, 1, 1);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    	//vdPrintCenter(strMMT[0].szRctFoot3);
			inCCTOS_PrinterBufferOutputAligned(strMMT[0].szRctFoot3,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER, 1, 1);
		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);


    	if(fRePrintFlag == TRUE)
    	{
	    	if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
				inCCTOS_PrinterBufferOutputAligned("* * 重 印 * *", &stgFONT_ATTRIB, d_PRINTER_ALIGNCENTER, 1, 0);
			else
				vdPrintCenter("DUPLICATE");
    	}
			

		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** MERCHANT COPY *****  ");
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		//if (strTCT.inFontFNTMode == 1)
                if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
                {
                        inCCTOS_PrinterBufferOutput("     商戶存根 請妥善保管  ",&stgFONT_ATTRIB,1);
                }
                else
			inCCTOS_PrinterBufferOutput("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
		//else
		//inCCTOS_PrinterBufferOutput("      ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
		
	}
	else
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
			EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			// patrick add code 20141208
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
            {         
                //CTOS_PrinterFline(d_LINE_DOT * 1);
                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,0);
				if (srTransRec.byEntryMode==CARD_ENTRY_ICC)
	                //CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
			//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
        }

		/*Aplipay no need E signature*/
		//ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,0);
		printCardHolderName();
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
		
		//ushCTOS_PrintAgreement();

       		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			vdSetGolbFontAttrib(d_FONT_9x18, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		if(strlen(strMMT[0].szRctFoot1) > 0)
			//vdPrintCenter(strMMT[0].szRctFoot1);
			inCCTOS_PrinterBufferOutputAligned(strMMT[0].szRctFoot1,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER, 1, 1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
			//vdPrintCenter(strMMT[0].szRctFoot2);
			inCCTOS_PrinterBufferOutputAligned(strMMT[0].szRctFoot2,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER, 1, 1);
		if(strlen(strMMT[0].szRctFoot3) > 0)
			//vdPrintCenter(strMMT[0].szRctFoot3);
			inCCTOS_PrinterBufferOutputAligned(strMMT[0].szRctFoot3,&stgFONT_ATTRIB,d_PRINTER_ALIGNCENTER, 1, 1);
		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);


    	if(fRePrintFlag == TRUE)
    	{
	    	if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
				inCCTOS_PrinterBufferOutputAligned("* * 重 印 * *", &stgFONT_ATTRIB, d_PRINTER_ALIGNCENTER, 1, 0);
			else
				vdPrintCenter("DUPLICATE");
    	}
		
		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** CUSTOMER COPY *****  ");
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
		//if (strTCT.inFontFNTMode == 1)
                if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
                {
                        inCCTOS_PrinterBufferOutput("     客戶存根 請妥善保管  ",&stgFONT_ATTRIB,1);
                }
                else
			inCCTOS_PrinterBufferOutput("   ***** CUSTOMER COPY *****  ",&stgFONT_ATTRIB,0);
		//else
		//inCCTOS_PrinterBufferOutput("      ***** CUSTOMER COPY *****  ",&stgFONT_ATTRIB,0);
	}
	
	//CTOS_PrinterFline(d_LINE_DOT * 6);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();

return d_OK;;	
}

int inCTOS_MPU_PRINTF_DETAIL_ALL(void)
{
    int inRet = d_NO;
    
    //GetReportTime();
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();
	
	vdSetMPUTrans(TRUE);
    
	if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		vdCTOS_PrintDetailReportAll();	
		inCTOS_MultiAPALLAppEventID(d_IPC_CMD_DETAIL_REPORT);
	}
	else
	{
		if (inMultiAP_CheckSubAPStatus() == d_OK)
		{
			inRet = inCTOS_MultiAPGetData();
			if(d_OK != inRet)
				return ;

			//inRet = inCTOS_MultiAPReloadHost();
			//if(d_OK != inRet)
				//return ;
		}
	}

    vdSetMPUTrans(FALSE);
	
    vdCTOS_TransEndReset();
    
    CTOS_LCDTClearDisplay();
    
    return inRet;
    //vdSetMPUTrans(TRUE);
    //inCTOS_PRINTF_DETAIL();
    //vdSetMPUTrans(FALSE);
}

void vdCTOS_PrintDetailReportAll(void)
{
	unsigned char chkey;
	short shHostIndex;
	int inResult,i,inCount,inRet;
	int inTranCardType;
	int inReportType;
	int inBatchNumOfRecord;
	int *pinTransDataid;

    int inNum=0, inNumOfHost=0;
	//if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
    //	return ;
	if( printCheckPaper()==-1)
		return;
		
	//by host and merchant
//	if(PrintDetail==0)
	//shHostIndex = inCTOS_SelectHostSetting();
	//if (shHostIndex == -1)
		//return;

	strHDT.inHostIndex = shHostIndex;
	DebugAddINT("summary host Index",shHostIndex);

    inNumOfHost = inHDTNumRecord();
    vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            if(strHDT.fHostEnable != TRUE)
                continue;

/*get HDTid, MITid, */
			srTransRec.HDTid=strHDT.inHostIndex;
			
			//inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);
			//memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
			
			//inBatchNumOfRecord = inBatchNumRecord();
/*new*/
            inResult = inCTOS_CheckAndSelectMutipleMID();
            vdDebug_LogPrintf("summary MITid",srTransRec.MITid);
            if(d_OK != inResult)
                return;

            inResult = inCTOS_ChkBatchEmpty();
            if(d_OK != inResult)
            {
				vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
                continue;
            }
            
            inBatchNumOfRecord = inBatchNumRecord();
            
            DebugAddINT("batch record",inBatchNumOfRecord);
            if(inBatchNumOfRecord <= 0)
            {
                vdDisplayErrorMsg(1, 8, "BATCH EMPTY");
                continue;
            }
            
            if (strTCT.inFontFNTMode == 1)
            {
                inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
                vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
            }
            else
            {
                //use ttf print
                inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
                vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
            }
            vdCTOSS_PrinterStart(100);
          //  ushCTOS_printBatchRecordHeader();

	#ifdef CBPAY_DV
		 	if (strHDT.inHostIndex == CBPAY_HOST_INDEX)
				ushCTOS_printBatchRecordHeaderCBPay();
			#ifdef OK_DOLLAR_FEATURE
			else if (strHDT.inHostIndex == OK_DOLLAR_HOST_INDEX)
				ushCTOS_printBatchRecordHeaderOKD();	
			#endif
			else
				ushCTOS_printBatchRecordHeader();
        #else
			ushCTOS_printBatchRecordHeader();
	#endif

										// fix for case #1346 122319															//IPP should always be MMK //http://118.201.48.210:8080/redmine/issues/1525.115
			if(srTransRec.HDTid == 17 || srTransRec.HDTid == CBPAY_HOST_INDEX || srTransRec.HDTid == OK_DOLLAR_HOST_INDEX || strHDT.inHostIndex == 6
				|| strHDT.inHostIndex == 23)		
            {
                inCSTRead(1);
            } 
            else 
            {

		  if(strHDT.inHostIndex == 22)	
		  	strHDT.inCurrencyIdx = 2;
		  
                inCSTRead(strHDT.inCurrencyIdx);
            }
	
            pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
            inCount = 0;		
            inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);

			vdDebug_LogPrintf("srTransRec.HDTid=[%d]-----",srTransRec.HDTid);
			vdDebug_LogPrintf("srTransRec.MITid=[%d]-----",srTransRec.MITid);
			vdDebug_LogPrintf("srTransRec.szBatchNo=[%02x]-----",srTransRec.szBatchNo[2]);
			
            for (i = 0; i < inBatchNumOfRecord; i++) 
            {
                inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
                inCount ++;
                //ushCTOS_printBatchRecordFooter();
                #ifdef CBPAY_DV
				
		                if (strHDT.inHostIndex == CBPAY_HOST_INDEX)
					ushCTOS_printBatchRecordFooterCBPay();						
						#ifdef OK_DOLLAR_FEATURE
						else if (strHDT.inHostIndex == OK_DOLLAR_HOST_INDEX)
							ushCTOS_printBatchRecordFooterOKD();		
						#endif
				else
					ushCTOS_printBatchRecordFooter();
			
			#else
			
				ushCTOS_printBatchRecordFooter();
			
			#endif
            }
        
            vdCTOS_PrintSummaryAfterDetail();
            
            //CTOS_PrinterFline(d_LINE_DOT * 5);
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT, &stgFONT_ATTRIB, 1);
            vdCTOSS_PrinterEnd();
            free(pinTransDataid);
        }
    }
	return;	
}

USHORT ushCTOS_PrintHeaderParkingFee(int page)
{	
    int inMerchantAddLen = 0;
    int inDataCnt = 0;
    int i = 0;
    char szStr[32+1];
    
	vdDebug_LogPrintf("start PrinterBMPPic,Tick=[%lu]........",CTOS_TickGet());

    vdCTOSS_PrinterBMPPicEx(0, 0, "./fs_data/omni_logo.bmp");

	if(fRePrintFlag == TRUE)
    {   
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
		if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
        {
            vdSetErrorMessage("LOAD MMT ERR");
            return(d_NO);
        }
        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctHdr1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctHdr1);
    }

#if 0
	//merhcant name and address
        if(strlen(strTCT.szMerchantName) > 0)
        	{	if (strTCT.inFontFNTMode != 1)
        	       {

			//		  vdHeaderPrintCenterTTF(strTCT.szMerchantName);
					  vdPrintLongMessageCenter(strTCT.szMerchantName);
        	       }

		        else
				   {
			//		 vdPrintCenter(strTCT.szMerchantName);
				     vdPrintLongMessageCenter(strTCT.szMerchantName);
		        	}
        	
        	}
            //vdPrintCenter(strTCT.szMerchantName);        
               vdPrintLongMessageCenter(strTCT.szMerchantAddress);
#endif

    if(VS_TRUE == strTCT.fDemo)
    {
        ushCTOS_PrintDemo();
    }

    if(fRePrintFlag == TRUE)
    {
    	if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, DOUBLE_SIZE, NORMAL_SIZE, 0, 0);
		if (strTCT.inFontFNTMode != 1)
			vdHeaderPrintCenterTTF("DUPLICATE");
		else
			vdPrintCenter("DUPLICATE");
			
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }
    
    
	return d_OK;
	
}

USHORT ushCTOS_PrintBodyParkingFee(int page)
{	
    char szStr[d_LINE_SIZE*2 + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
	char szTmpPan[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTempAmt[AMT_ASC_SIZE+1];
    BYTE szPrintingText[40 + 1];

	if(d_FIRST_PAGE == page)
	{
		#if 0
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	vdPrintCenter(szTemp1);
		#endif
		printDateTime();

        printTIDMID(); 
		
		printBatchInvoiceNO();

		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    	szGetTransTitle(srTransRec.byTransType, szStr);
        
        if(srTransRec.byOrgTransType == SALE_OFFLINE && srTransRec.byTransType == VOID){
            memset(szStr, 0x00, sizeof(szStr));
            strcpy(szStr, "VOID OFFLINE");
        } else if(srTransRec.byOrgTransType == SALE && srTransRec.byTransType == VOID){
            memset(szStr, 0x00, sizeof(szStr));
            strcpy(szStr, "VOID SALE");
        }


		if (strTCT.inFontFNTMode == 1)
            vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);

		sprintf(szStr, "%s", srTransRec.szCardLable);
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(srTransRec.szPAN,szTmpPan,d_LINE_SIZE,page);
		strcpy(szTemp4, szTmpPan);
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szTemp4,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

#if 0        
		//Exp date and Entry mode
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0, sizeof(szTemp));
		memset(szTemp1, 0, sizeof(szTemp1));
		memset(szTemp4, 0, sizeof(szTemp4));
		memset(szTemp5, 0, sizeof(szTemp5));
		wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
		DebugAddSTR("EXP",szTemp,12);
                memset(szTemp1, 0x00, sizeof(szTemp1));
                
                memcpy(szTemp1, szTemp+2, 2);
                memcpy(szTemp1+2, szTemp, 2);
                
                memset(szTemp, 0x00, sizeof(szTemp));
                strcpy(szTemp, szTemp1);
		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(szTemp,szTmpPan,(EXP_DATE_SIZE + 1),page);
		
		//for (i =0; i<4;i++)
		//	szTemp[i] = '*';
		memcpy(szTemp4,&szTmpPan[0],2);
		memcpy(szTemp5,&szTmpPan[2],2);
                
        memset(szTemp1, 0x00, sizeof(szTemp1));
		if(srTransRec.byEntryMode==CARD_ENTRY_ICC || srTransRec.byEntryMode==CARD_ENTRY_EASY_ICC)
			memcpy(szTemp1,"Chip",4);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
			memcpy(szTemp1,"Manual",6);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"MSR",3);
		else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
			memcpy(szTemp1,"Fallback",8);
		else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
		}

		vdDebug_LogPrintf("PAT ENTRY MODE [%ld] [%s]", srTransRec.byEntryMode, srTransRec.szPAN);	 
		
		memset (baTemp, 0x00, sizeof(baTemp));
		sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
		
		DebugAddSTR("ENT:",baTemp,12);  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szTemp,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);


//                if(fGetMPUTrans() == FALSE){
                    //Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
		else
			//use ttf print
		sprintf(szStr, "RREF NUM    : %s", srTransRec.szRRN);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
//                }
//		//Reference num
//		memset(szStr, ' ', d_LINE_SIZE);
//		memset (baTemp, 0x00, sizeof(baTemp));					
//		memset(szStr, ' ', d_LINE_SIZE);
//		if (strTCT.inFontFNTMode == 1)
//			sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
//		else
//			//use ttf print
//		sprintf(szStr, "RREF NUM    : %s", srTransRec.szRRN);
//		memset (baTemp, 0x00, sizeof(baTemp));		
//		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
//		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);	
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "APPR CODE : %s", srTransRec.szAuthCode);
		else
		   sprintf(szStr, "APPR CODE   : %s", srTransRec.szAuthCode);
		
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		if ((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
			/* EMV: Revised EMV details printing - start -- jzg */
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))
			/* EMV: Revised EMV details printing - end -- jzg */
		{
			//AC
			wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "APP CRYPT : %s", szTemp);
			else
				//use ttf print
			sprintf(szStr, "APP CRYPT   : %s", szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			//TVR
			EMVtagLen = 5;
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			else
				//use ttf print
			sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                        DebugAddHEX("*9B*", srTransRec.stEMVinfo.T9B, 2);
                        
                        if(strIIT.inIssuerNumber == 6){
                            //TSI
                            EMVtagLen = 2;
                            memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9B, EMVtagLen);
                            DebugAddHEX("*9B*", EMVtagVal, 2);
                            memset(szStr, ' ', d_LINE_SIZE);
                            if (strTCT.inFontFNTMode == 1)
                                    sprintf(szStr, "TSI VALUE : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                            else
                                    //use ttf print
                            sprintf(szStr, "TSI VALUE   : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                        }
			
			//AID
			memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
			vdDebug_LogPrintf("EMVtagLen=[%d]",EMVtagLen);
			
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
			memset(szTemp, ' ', d_LINE_SIZE);
			wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "AID       : %s",szTemp);
			else			
			    sprintf(szStr, "AID         : %s",szTemp);//use ttf print
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			/* EMV: Get Application Label - start -- jzg */
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
			     sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
			else
				sprintf(szStr, "APP LABEL   : %s", srTransRec.stEMVinfo.szChipLabel);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			/* EMV: Get Application Label - end -- jzg */
			
		}
#endif

		//thandar_add for Cash Adv printing fee
		if (srTransRec.byTransType == CASH_ADVANCE)
		{
                    if(srTransRec.HDTid == 17)
					{
                        inCSTRead(1);
                    }
                    
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                            sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                            memset(szTemp3, 0x00, sizeof(szTemp3));
                            strcpy(szTemp3, szTempAmt);
                        }


			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                        }
                if (strTCT.inFontFNTMode == 1)        
					sprintf(szStr, "CashAdvFee:%s %s", strCST.szCurSymbol,szTemp5);

				else
					sprintf(szStr, "CashAdvFee  :%s %s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
		}

		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
                
                if((srTransRec.HDTid == 17) || (strcmp(strCST.szCurSymbol, "MMK") == 0)){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
                    memset(szTemp, 0x00, sizeof(szTemp));
                    strcpy(szTemp, szTempAmt);
                    
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp1)/100);
                    memset(szTemp1, 0x00, sizeof(szTemp1));
                    strcpy(szTemp1, szTempAmt);
                }
                
		if (strTCT.inFontFNTMode == 1)
			 vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
//       		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
               
		else//use ttf print
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		    //vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		if (srTransRec.byTransType == SALE)
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
			//Base amount
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			memset(szTemp3, 0x00, sizeof(szTemp3));
                        
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                        }

						if (TRUE ==strTCT.fTipAllowFlag)
							 sprintf(szTemp3, "%s %16s", strCST.szCurSymbol,szTemp5);

						else
                          sprintf(szTemp3, "%s %s", strCST.szCurSymbol,szTemp5);
                        
//			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
//                        sprintf(szStr, "AMOUNT:%16s %s", strCST.szCurSymbol,szTemp5);
//                        sprintf(szStr, "AMOUNT: %16s", szTemp3);
					if (TRUE ==strTCT.fTipAllowFlag)						
						 sprintf(szStr, "AMT:%s", szTemp3);
					     
					 else
                        sprintf(szStr, "AMOUNT: %s", szTemp3);
                        
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
                        
                        if(srTransRec.fIsInstallment == TRUE){
                            ushCTOS_PrintBodyInstallment();
                        } else{
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
                        
			//Tip amount
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
			memset (baTemp, 0x00, sizeof(baTemp));
			memset (szTemp4, 0x00, sizeof(szTemp4));
			wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                            sprintf(szTempAmt, "%012.0f", atof(szTemp4)/100);
                            memset(szTemp4, 0x00, sizeof(szTemp4));
                            strcpy(szTemp4, szTempAmt);
                        }

			DebugAddSTR("TIP:",szTemp4,12);
            if (TRUE ==strTCT.fTipAllowFlag)
            {
            	
			   
	
 				if (strTCT.inFontFNTMode != 1)
			 	  vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	              //vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

			    if(atol(szTemp4) > 0)
    			{
    				memset(szStr, ' ', d_LINE_SIZE);

					memset(szTemp5,0x00, sizeof(szTemp5));
                                        
					//format amount 10+2
                                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp4, szTemp5);
                                        } else{
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp4, szTemp5);
                                        }
                                            
					sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
                                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                                        } else{
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                                        }
                                        
					sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");				
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
    			}
                else
                {
                    memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szTemp5);
					//sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TIP:%s", strCST.szCurSymbol);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
					//sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TOT:%s", strCST.szCurSymbol);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
                }
            }
			
			
		}

		else if (srTransRec.byTransType == SALE_TIP)
		{	 			    
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
                    
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                            sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                            memset(szTemp3, 0x00, sizeof(szTemp3));
                            strcpy(szTemp3, szTempAmt);
                        }

			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                        } else{
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                        }
                        
			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                        }
                        
			sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
					
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                        }
                        
			sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");	
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			
		}

	 else if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH || srTransRec.byTransType == MPU_VOID_PREAUTH_COMP || srTransRec.byTransType == MPU_VOID_PREAUTH){ //@@IBR ADD 03102016
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
                    
//                    CTOS_PrinterSetHeatLevel(4);
                    memset(szStr, ' ', d_LINE_SIZE);

				    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }
                    
                    memset(szTemp3,0x00, sizeof(szTemp3));
                    
                    strcpy(szTemp3, "-");
                    strcat(szTemp3, strCST.szCurSymbol);
                    strcat(szTemp3, " ");
                    strcat(szTemp3, szTemp5);
                    
                    memset(szTemp5, 0x00, sizeof(szTemp5));
                    strcpy(szTemp5, szTemp3);
//                    sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
//                    sprintf(szStr, "AMOUNT: %16s", szTemp5);
					if (TRUE ==strTCT.fTipAllowFlag)
						 sprintf(szStr, "AMT:%s", szTemp5);

					else 	
                         sprintf(szStr, "AMOUNT: %s", szTemp5);
                    
                    memset(szPrintingText, 0x00, sizeof(szPrintingText));
                    sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                    put_env_int("#P3LEN", strlen(szPrintingText));
                    put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                    
                    memset (baTemp, 0x00, sizeof(baTemp));
                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    if(srTransRec.fIsInstallment == TRUE){
                        ushCTOS_PrintBodyInstallment();
                    } else{
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                        vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
                    
//                    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
                }
		else
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                        }
                        
//			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
                        memset(szTemp3, 0x00,sizeof(szTemp3));
						if (TRUE ==strTCT.fTipAllowFlag)
							sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);

						else
                           sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp5);
//                        sprintf(szStr, "AMOUNT: %16s", szTemp3);
						if (TRUE ==strTCT.fTipAllowFlag)
						   sprintf(szStr, "AMT:%s", szTemp3);

					   else						
                           sprintf(szStr, "AMOUNT: %s", szTemp3);
                        
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
                        if(srTransRec.fIsInstallment == TRUE){
                            ushCTOS_PrintBodyInstallment();
                        } else{
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
		}
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
		
	}
//	else if(d_SECOND_PAGE == page)
        else
	{
		#if 0
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	vdPrintCenter(szTemp1);
		#endif
		
		printDateTime();
		inResult = printTIDMID(); 
		
		printBatchInvoiceNO(); // pat confirm hang

		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		memset(szStr, ' ', d_LINE_SIZE);
    	szGetTransTitle(srTransRec.byTransType, szStr);

		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_9x18, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
		
		sprintf(szStr, "%s",srTransRec.szCardLable);
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(srTransRec.szPAN,szTmpPan,d_LINE_SIZE,page);
		strcpy(szTemp4, szTmpPan);
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
 #if 0       
		//Exp date and Entry mode
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0, sizeof(szTemp));
		memset(szTemp1, 0, sizeof(szTemp1));
		memset(szTemp4, 0, sizeof(szTemp4));
		memset(szTemp5, 0, sizeof(szTemp5));
		wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
		DebugAddSTR("EXP",szTemp,12);
                memset(szTemp1, 0x00, sizeof(szTemp1));
                memcpy(szTemp1, szTemp+2, 2);
                memcpy(szTemp1+2, szTemp, 2);
                
                memset(szTemp, 0x00, sizeof(szTemp));
                strcpy(szTemp, szTemp1);
		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(szTemp,szTmpPan,(EXP_DATE_SIZE + 1),page);
		
		//for (i =0; i<4;i++)
		//	szTemp[i] = '*';
		memcpy(szTemp4,&szTmpPan[0],2);
		memcpy(szTemp5,&szTmpPan[2],2);

		if(srTransRec.byEntryMode==CARD_ENTRY_ICC || srTransRec.byEntryMode==CARD_ENTRY_EASY_ICC)
			memcpy(szTemp1,"Chip",4);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
			memcpy(szTemp1,"Manual",6);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"MSR",3);
		else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
			memcpy(szTemp1,"Fallback",8);
		else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
		}
		
		DebugAddSTR("ENT:",szTemp1,12);  
		memset (baTemp, 0x00, sizeof(baTemp));

		memset (baTemp, 0x00, sizeof(baTemp));
		sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
		
		DebugAddSTR("ENT:",baTemp,12);  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);


		//Reference num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		sprintf(szStr, "RREF NUM  : %s", srTransRec.szRRN);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
		   sprintf(szStr, "APPR CODE : %s", srTransRec.szAuthCode);
		else
		   sprintf(szStr, "APPR CODE   : %s", srTransRec.szAuthCode);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		if((srTransRec.byEntryMode == CARD_ENTRY_ICC) ||
			/* EMV: Revised EMV details printing - start -- jzg */
			(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
			(srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
			(srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
			(srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))
			/* EMV: Revised EMV details printing - end -- jzg */
		{
			//AC
			wub_hex_2_str(srTransRec.stEMVinfo.T9F26, szTemp, 8);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "APP CRYPT : %s", szTemp);
			else
				//use ttf print
			sprintf(szStr, "APP CRYPT   : %s", szTemp);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			//TVR
			EMVtagLen = 5;
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T95, EMVtagLen);
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "TVR VALUE : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			else
				//use ttf print
			sprintf(szStr, "TVR VALUE   : %02x%02x%02x%02x%02x", EMVtagVal[0], EMVtagVal[1], EMVtagVal[2], EMVtagVal[3], EMVtagVal[4]);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                        
                        if(strIIT.inIssuerNumber == 6){
                            //TSI
                            EMVtagLen = 2;
                            memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
                            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9B, EMVtagLen);
                            memset(szStr, ' ', d_LINE_SIZE);
                            if (strTCT.inFontFNTMode == 1)
                                    sprintf(szStr, "TSI VALUE : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                            else
                                    //use ttf print
                            sprintf(szStr, "TSI VALUE   : %02x%02x", EMVtagVal[0], EMVtagVal[1]);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                        }
			
			//AID
			memset(szStr, ' ', d_LINE_SIZE);
            EMVtagLen = srTransRec.stEMVinfo.T84_len;
			vdDebug_LogPrintf("EMVtagLen=[%d]",EMVtagLen);
			
			memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
			memcpy(EMVtagVal, srTransRec.stEMVinfo.T84, EMVtagLen);
			memset(szTemp, ' ', d_LINE_SIZE);
			wub_hex_2_str(EMVtagVal, szTemp, EMVtagLen);
			if (strTCT.inFontFNTMode == 1)
				sprintf(szStr, "AID       : %s",szTemp);
			else		
			   sprintf(szStr, "AID         : %s",szTemp);//use ttf print
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

			/* EMV: Get Application Label - start -- jzg */
			memset(szStr, ' ', d_LINE_SIZE);
			if (strTCT.inFontFNTMode == 1)
			  sprintf(szStr, "APP LABEL : %s", srTransRec.stEMVinfo.szChipLabel);
			else
				sprintf(szStr, "APP LABEL   : %s", srTransRec.stEMVinfo.szChipLabel);
			memset (baTemp, 0x00, sizeof(baTemp));		
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			/* EMV: Get Application Label - end -- jzg */
			
		}

#endif
			//thandar_add for Cash Adv printing fee
		if (srTransRec.byTransType == CASH_ADVANCE)
		{
                    if(srTransRec.HDTid == 17)
					{
                        inCSTRead(1);
                    }
                    
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                            sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                            memset(szTemp3, 0x00, sizeof(szTemp3));
                            strcpy(szTemp3, szTempAmt);
                        }


			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                        }
            if (strTCT.inFontFNTMode == 1)             
			    sprintf(szStr, "CashAdvFee:%s %s", strCST.szCurSymbol,szTemp5);
			
            else
			    sprintf(szStr, "CashAdvFee  :%s %s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
		}

		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
                
                if((srTransRec.HDTid == 17) || (strcmp(strCST.szCurSymbol, "MMK") == 0)){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
                    memset(szTemp, 0x00, sizeof(szTemp));
                    strcpy(szTemp, szTempAmt);
                    
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp1)/100);
                    memset(szTemp1, 0x00, sizeof(szTemp1));
                    strcpy(szTemp1, szTempAmt);
                }

		if (strTCT.inFontFNTMode == 1)
			  vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
      //        	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                  
		else	//use ttf print				
		 	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);		    
			//vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

			
        
		if (srTransRec.byTransType == SALE)
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
			//Base amount
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
                        memset(szTemp3, 0x00, sizeof(szTemp3));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                        }

						if (TRUE ==strTCT.fTipAllowFlag)
						sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);

						else
                           sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp5);
                        
//                        if(atoi(szTemp1) == 0)
//                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
//                        else
//                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                        
//			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
//                        sprintf(szStr, "AMOUNT:%16s %s", strCST.szCurSymbol,szTemp5);
//                        sprintf(szStr, "AMOUNT: %16s", szTemp3);
					if (TRUE ==strTCT.fTipAllowFlag)
						 sprintf(szStr, "AMT:%s", szTemp3);

					else
                        sprintf(szStr, "AMOUNT: %s", szTemp3);
                        
                        
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        
                        if(srTransRec.fIsInstallment == TRUE){
                            ushCTOS_PrintBodyInstallment();
                        } else {
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
                            
			//Tip amount
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr,"%s    %s","TIP",strCST.szCurSymbol);
			memset (baTemp, 0x00, sizeof(baTemp));
			memset (szTemp4, 0x00, sizeof(szTemp4));
			wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                            sprintf(szTempAmt, "%012.0f", atof(szTemp4)/100);
                            memset(szTemp4, 0x00, sizeof(szTemp4));
                            strcpy(szTemp4, szTempAmt);
                        }

			DebugAddSTR("TIP:",szTemp4,12);
            if (TRUE ==strTCT.fTipAllowFlag)
            {

			   		    
				if (strTCT.inFontFNTMode != 1)
			 	  vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	              //vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

		  
			    if(atol(szTemp4) > 0)
    			{
    				memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
                                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp4, szTemp5);
                                        } else {
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp4, szTemp5);
                                        }
                                        
					sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
        			//CTOS_PrinterPutString("                 _______________");
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("                 _______________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
                                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                                        } else {
                                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                                        }
                                        
					sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
        			//CTOS_PrinterPutString("                 _______________");
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("                 _______________",&stgFONT_ATTRIB,1);
    			}
                else
                {
                    memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szTemp5);
					//sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TIP:%s", strCST.szCurSymbol);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
        			//CTOS_PrinterPutString("                 _______________");
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("                 _______________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
					//sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TOT:%s", strCST.szCurSymbol);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3); 
        			//CTOS_PrinterPutString("                 _______________");
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput("                 _______________",&stgFONT_ATTRIB,1);
                }
            }
			
			
		}
		else if (srTransRec.byTransType == SALE_TIP)
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            memset(szTempAmt, 0x00, sizeof(szTempAmt));
                            sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                            memset(szTemp3, 0x00, sizeof(szTemp3));
                            strcpy(szTemp3, szTempAmt);
                        }
                        
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                        }
                        
			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                        }
                        
			sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                        }
                        
			sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			
		} else if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH){ //@@IBR ADD 03102016
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }
                    
                    memset(szTemp3,0x00, sizeof(szTemp3));
                    
                    strcpy(szTemp3, "-");
                    strcat(szTemp3, strCST.szCurSymbol);
                    strcat(szTemp3, " ");
                    strcat(szTemp3, szTemp5);
                    
                    memset(szTemp5, 0x00, sizeof(szTemp5));
                    strcpy(szTemp5, szTemp3);
//                    sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
//                    sprintf(szStr, "AMOUNT: %16s",szTemp5);
					if (TRUE ==strTCT.fTipAllowFlag)
						sprintf(szStr, "AMT:%s", szTemp5);
					else
                    	sprintf(szStr, "AMOUNT: %s",szTemp5);
                    
                    memset(szPrintingText, 0x00, sizeof(szPrintingText));
                    sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                    put_env_int("#P3LEN", strlen(szPrintingText));
                    put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                    
                    memset (baTemp, 0x00, sizeof(baTemp));
                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    if(srTransRec.fIsInstallment == TRUE){
                        ushCTOS_PrintBodyInstallment();
                    } else{
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                        vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
                }
		else
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                        }
                        
//			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
                        memset(szTemp3, 0x00,sizeof(szTemp3));
						if (TRUE ==strTCT.fTipAllowFlag)
						   sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);
						else							
                           sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp5);
//                        sprintf(szStr, "AMOUNT: %16s", szTemp3);
						if (TRUE ==strTCT.fTipAllowFlag)
								sprintf(szStr, "AMT:%s", szTemp3);
					    else						
                        		sprintf(szStr, "AMOUNT: %s", szTemp3);
                        
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        
                        if(srTransRec.fIsInstallment == TRUE){
							vdDebug_LogPrintf("1_ushCTOS_PrintBodyInstallment");
                            ushCTOS_PrintBodyInstallment();
                        } else{
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
		}

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
	}

	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
	return d_OK;	
}

void vdPrintTerminalConfigNew(void)
{
    int inResult;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ,i;
    BYTE szStr[50];
    BYTE szBuf[50];
    BYTE baTemp[PAPER_X_SIZE * 64];
	BYTE bLength;
	BYTE szSerialNum[17 + 1];
    int inHostCnt = 0;
    int j = 0;
    int inMinCDT = 0, inMaxCDT = 0;
    char szTemp[MAX_CHAR_PER_LINE+1], szPrint[MAX_CHAR_PER_LINE+1];
    int inDataLen = 0;
    char szCardLabel[16+1];
	int inCDTID[400];
	int inCount=0;
	
    int inAutoSwitchSim=get_env_int("AUTOSWITCHSIM");
	int inManualSwitchSim=get_env_int("MANUALSWITCHSIM");
	
    #ifndef DUAL_SIM_SETTINGS
        inAutoSwitchSim=0; /*disable - not yet complete*/
    #else
        if(inManualSwitchSim == 1)
            inAutoSwitchSim=1;
    #endif

    if (strTCT.inFontFNTMode == 1)
	{
	   	inCTOS_SelectFont(d_FONT_FNT_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
	}
	else
	{
    //use ttf print
		inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
   	}
    vdCTOSS_PrinterStart(100);
    CTOS_PrinterSetHeatLevel(4);  

	//ushCTOS_PrintHeader(0);
	vdCTOSS_PrinterBMPPic(0, 0, "logo.bmp");
//	CTOS_PrinterFline(d_LINE_DOT * 1);
        
//        ushCTOS_PrintHeader(1);
        ushCTOS_TerminalConfigPrintHeader();

	// Get the configuration value of Ethernet //
        
        memset(szSerialNum, 0x00, sizeof(szSerialNum));
        CTOS_GetFactorySN(szSerialNum);
		vdDebug_LogPrintf("RAW szSerialNum[%s]", szSerialNum);
		inDataLen = strlen(szSerialNum);
		
		for (i=0; i<inDataLen; i++)
		{
			if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
			{
				szSerialNum[i] = 0;
				break;
			}
		}
        
        inDataLen = strlen(szSerialNum);
        memset(szTemp, 0x00, sizeof(szTemp));
        memcpy(szTemp, szSerialNum, inDataLen-1);
        
        memset(szSerialNum, 0x00, sizeof(szSerialNum));
        //inFmtPad(szTemp, -inDataLen, '0');
        wub_strpad(szTemp, szSerialNum, '0', 16, 0);
        //strcpy(szSerialNum, szTemp);
        vdDebug_LogPrintf("Fmt szSerialNum[%s]", szSerialNum);
		
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
       // CTOS_PrinterFline(d_LINE_DOT * 1);
        
    memset(szStr, 0x00, sizeof(szStr));
    memset(baTemp, 0x00, sizeof (baTemp));
    sprintf(szStr, "TERMINAL SERIAL NO:");
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    vdPrintCenterTerminalConfig(szStr);

	
    
    memset(szStr, 0x00, sizeof(szStr));
    memset(baTemp, 0x00, sizeof (baTemp));
    strcpy(szStr, szSerialNum);
//    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    vdPrintCenterTerminalConfig(szStr);

	vdCTOSS_PrinterEnd();

    
    CTOS_PrinterFline(d_LINE_DOT * 1);
        
    memset(szStr, 0x00, sizeof (szStr));
    memset(szBuf, 0x00, sizeof (szBuf));
    vdCTOSS_GetAppVersion(szBuf);
    sprintf(szStr, "%s", szBuf);
    memset(baTemp, 0x00, sizeof (baTemp));
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

    inNumOfHost = inHDTMAX();
    
    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            inCPTRead(inNum);
            CTOS_PrinterFline(d_LINE_DOT * 1);
           
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "HOST: %s (%02d)", strHDT.szHostLabel, strHDT.inHostIndex);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

            memset(szStr, 0x00, sizeof(szStr));
            memset(szBuf, 0x00, sizeof(szBuf));
            wub_hex_2_str(strHDT.szTPDU, szBuf, 5);
            sprintf(szStr, "TPDU: %s", szBuf);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            memset(szTemp, 0x00, sizeof(szTemp));
            memset (baTemp, 0x00, sizeof(baTemp));
            wub_hex_2_str(strHDT.szNII, szTemp, 2);
            sprintf(szStr, "NII: %s", szTemp);
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI NUM : %s", strCPT.szPriTxnPhoneNumber);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC NUM : %s", strCPT.szSecTxnPhoneNumber);                        
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

    #ifdef DUAL_SIM_SETTINGS
			vdDebug_LogPrintf("1. inAutoSwitchSim[%d], strCPT.inCommunicationMode[%d]", inAutoSwitchSim, strCPT.inCommunicationMode);
			if(inAutoSwitchSim == 1 && strCPT.inCommunicationMode == GPRS_MODE)
			{
                if(strTCP.inSIMSlot == 1)
                {
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "PRI IP	: %s", strCPT.szPriTxnHostIP);						  
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum); 					   
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "SEC IP	: %s", strCPT.szSecTxnHostIP);						  
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					
					memset(szStr, 0x00, sizeof(szStr));
					sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum); 					   
					memset (baTemp, 0x00, sizeof(baTemp));
					CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
					CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);

                }
                else
                {
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "PRI IP  : %s", strCPT.szPriTxnIP_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnPort_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "SEC IP  : %s", strCPT.szSecTxnIP_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
                    memset(szStr, 0x00, sizeof(szStr));
                    sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnPort_2);                        
                    memset (baTemp, 0x00, sizeof(baTemp));
                    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                }
			}
			else
			{
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "PRI IP	: %s", strCPT.szPriTxnHostIP);						  
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum); 					   
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "SEC IP	: %s", strCPT.szSecTxnHostIP);						  
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
				
				memset(szStr, 0x00, sizeof(szStr));
				sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum); 					   
				memset (baTemp, 0x00, sizeof(baTemp));
				CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
				CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			}
    #else
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI IP	: %s", strCPT.szPriTxnHostIP);						  
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "PRI PORT: %d", strCPT.inPriTxnHostPortNum); 					   
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC IP	: %s", strCPT.szSecTxnHostIP);						  
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
            
            memset(szStr, 0x00, sizeof(szStr));
            sprintf(szStr, "SEC PORT: %d", strCPT.inSecTxnHostPortNum); 					   
            memset (baTemp, 0x00, sizeof(baTemp));
            CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
            CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
    #endif
			
            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
                if((inResult = inMMTReadRecord(strHDT.inHostIndex,strMMT[inLoop-1].MITid)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MITid,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else 
                {
                    if(strMMT[0].fMMTEnable)
                    {                        
//                        memset(szStr, 0x00, sizeof(szStr));
//                        sprintf(szStr, "MERCHANT:%s", strMMT[0].szMerchantName);                        
//                        memset (baTemp, 0x00, sizeof(baTemp));
//                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "TID : %s", strMMT[0].szTID);                        
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

                        memset(szStr, 0x00, sizeof(szStr));
                        sprintf(szStr, "MID : %s", strMMT[0].szMID);                        
                        memset (baTemp, 0x00, sizeof(baTemp));
                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

//                        memset(szStr, 0x00, sizeof(szStr));
//                        memset(szBuf, 0x00, sizeof(szBuf));
//                        wub_hex_2_str(strMMT[0].szBatchNo, szBuf, 3);
//                        sprintf(szStr, "BATCH NO: %s", szBuf);                        
//                        memset (baTemp, 0x00, sizeof(baTemp));
//                        CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
//                        CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
//						inCTOSS_CapturePrinterQRCodeBuffer(szStr,0);

                    }                    

                }
            }
        }
        else
            continue;

    }
    
    CTOS_PrinterFline(d_LINE_DOT * 1);

    memset(szPrint, 0x00, sizeof(szPrint));
    strcpy(szPrint, "--------TABLEPAY ENABLED-------");
    CTOS_PrinterBufferPutString((BYTE *) baTemp, 1, 1, szPrint, &stgFONT_ATTRIB);
    CTOS_PrinterBufferOutput((BYTE *) baTemp, 3);

//    inHostCnt = inHDTMAX();
    
    //for (i = 1; i <= inNumOfHost; i++) {
        //inCDTReadbyHost(i, &inMinCDT, &inMaxCDT);
        memset((char*)inCDTID, 0x00, sizeof(inCDTID));
        inCDTReadCDTid(inCDTID);
		
        inMinCDT=0;
        inMaxCDT=inCDTMAX();
        j = 0;
		vdDebug_LogPrintf("Min = %d, Max = %d", inMinCDT, inMaxCDT);

		memset(szCardLabel, 0, sizeof(szCardLabel));
        for (j = inMinCDT; j <= inMaxCDT; j++) {
            vdDebug_LogPrintf("j = %d, Min = %d, Max = %d", j, inMinCDT, inMaxCDT);
			
            //if (inCDTReadEnabled(j, i) == d_OK) {
            if (inCDTRead(inCDTID[j]) == d_OK) {
                if(strlen(strCDT.szPANLo) <= 8)
                    continue;
				
                memset(szTemp, 0x00, sizeof (szTemp));
                memset(szPrint, 0x00, sizeof (szPrint));
                memset(baTemp, 0x00, sizeof (baTemp));
				if(strcmp(szCardLabel, strCDT.szCardLabel) != 0)
				{
					memset(szCardLabel, 0, sizeof(szCardLabel));
					strcpy(szCardLabel, strCDT.szCardLabel);
					
                    sprintf(szPrint, "%s",strCDT.szCardLabel);
                    CTOS_PrinterBufferPutString((BYTE *) baTemp, 1, 1, szPrint, &stgFONT_ATTRIB);
                    CTOS_PrinterBufferOutput((BYTE *) baTemp, 3);
					inCount=1;
				}

                memset(szPrint, 0x00, sizeof (szPrint));
                memset(baTemp, 0x00, sizeof (baTemp));
                sprintf(szTemp, "%02d %9.9s-%9.9s H%02d T%d", inCount, strCDT.szPANLo, strCDT.szPANHi, strCDT.HDTid, strCDT.inType);
				inCount++;
                vdDebug_LogPrintf("szTemp [%s]", szTemp);
                //sprintf(szPrint, "%-*s%-*s", 42 / 3, strCDT.szCardLabel, 42 / 3, szTemp);
                sprintf(szPrint, "%32s", szTemp);
                vdDebug_LogPrintf("szPrint [%s]", szPrint);
                CTOS_PrinterBufferPutString((BYTE *) baTemp, 1, 1, szPrint, &stgFONT_ATTRIB);
                CTOS_PrinterBufferOutput((BYTE *) baTemp, 3);
            }
        //}
    }
    
    CTOS_PrinterFline(d_LINE_DOT * 10);
    
    return ;
}


// TODO: perform cleanup on this function
USHORT ushCTOS_PrintBodyCBPay(int page)
{	
    char szStr[d_LINE_SIZE*2 + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
	char szTmpPan[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTempAmt[AMT_ASC_SIZE+1];
    BYTE szPrintingText[40 + 1];

	#ifdef DISCOUNT_FEATURE
		// for Discount feature
		char szStrDisc1[AMT_ASC_SIZE+1];
		char szStrDisc2[AMT_ASC_SIZE+1];
		char szStrDisc3[AMT_ASC_SIZE+1];
		char szStrDisc4[AMT_ASC_SIZE+1];
		char szStrDisc5[AMT_ASC_SIZE+1];
		char szStrDisc6[AMT_ASC_SIZE+1];	
		char szStrDisc7[AMT_ASC_SIZE+1];	
		char szStrDisc8[AMT_ASC_SIZE+1];	
		char szStrDisc9[AMT_ASC_SIZE+1];	
		char szStrDisc10[AMT_ASC_SIZE+1];	
		char szStrDisc11[AMT_ASC_SIZE+1];	
		char szStrDisc12[AMT_ASC_SIZE+1];	
		char szStrDisc13[AMT_ASC_SIZE+1];	
		char szStrDisc14[AMT_ASC_SIZE+1];	
		char szStrDisc15[AMT_ASC_SIZE+1];

		char szFixedAmountDisc[42+1];
		char szOrigAmountDisc[42+1];
		char szPercentageDisc[42+1];	
		int len;
		//
	#endif


	inCBPayRead(1);

	inCSTRead(1); //MMK is always the currency for CBPAY

	vdDebug_LogPrintf("ushCTOS_PrintBodyCBPay START ");
		

	if(d_FIRST_PAGE == page)
	{
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);  
    	//sprintf(szTemp1,"%s",strHDT.szHostLabel);
    	vdPrintCenter(szTemp1);
		
	printTIDMIDCBPay(); 

	sprintf(szStr, "SUB MID: %s", strCBPay.szSubMerId);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
	printDateTime();

	//printBatchInvoiceNO();

	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

	//szGetTransTitle(srTransRec.byTransType, szStr);
	memset(szStr, ' ', d_LINE_SIZE);
	strcpy(szStr, "CBPay SALES");
        
        if(srTransRec.byOrgTransType == SALE_OFFLINE && srTransRec.byTransType == VOID){
            memset(szStr, 0x00, sizeof(szStr));
            strcpy(szStr, "VOID OFFLINE");
        } else if(srTransRec.byOrgTransType == CB_PAY_TRANS && srTransRec.byTransType == VOID){//else if(srTransRec.byOrgTransType == SALE && srTransRec.byTransType == VOID){
            memset(szStr, 0x00, sizeof(szStr));
            strcpy(szStr, "VOID CBPay SALES");
        }

		/**/
		vdCTOS_ModifyTransTitleString(szStr);
		
		#if 0
        if(srTransRec.byTransType == VOID_PREAUTH_COMP)
		{
            memset(szStr, 0x00, sizeof(szStr));
        }
		else
        {
            vdPrintTitleCenter(szStr);
        }
        #endif
  		vdPrintTitleCenter(szStr);




		if (strTCT.inFontFNTMode == 1)
     		  	 vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
		   	 vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);



		//reference number
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "REF NO  : %s", srTransRec.szRefNo);
		else
			//use ttf print
		sprintf(szStr, "REF NO      :  %s", srTransRec.szRefNo);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);


		//print  trace num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);	
		    wub_hex_2_str(srTransRec.szInvoiceNo, baTemp, INVOICE_BCD_SIZE);

		// for case http://118.201.48.210:8080/redmine/issues/1525.34.1 	   
		//sprintf(szStr, "TRACE NO. : %s", baTemp);
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "INV NO. : %s", baTemp);
		else
		   sprintf(szStr, "INV NO.   :  %s", baTemp);

		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);


		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);	
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "TRANX ID : %s", srTransRec.szBankTransId);
		else
		   sprintf(szStr, "TRANX ID    :  %s", srTransRec.szBankTransId);
		
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);


		//print CB PAY ACC  -- The value for CB PAY is  intentionally blank.
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "CBPay A/C  : ");
		else
		   sprintf(szStr, "CBPay A/C   : ");

		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		
		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);


		
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);


		#ifdef DISCOUNT_FEATURE
			// for Discount feature
			// FIxed Rate:
			memset (szStrDisc1, 0x00, sizeof(szStrDisc1));
			memset (szStrDisc2, 0x00, sizeof(szStrDisc2));
			memset (szStrDisc3, 0x00, sizeof(szStrDisc3));
			memset (szStrDisc4, 0x00, sizeof(szStrDisc4));
			memset (szStrDisc5, 0x00, sizeof(szStrDisc5));
			//Original Amount:
			memset (szStrDisc6, 0x00, sizeof(szStrDisc6));
			memset (szStrDisc7, 0x00, sizeof(szStrDisc7));
			memset (szStrDisc8, 0x00, sizeof(szStrDisc8));
			memset (szStrDisc9, 0x00, sizeof(szStrDisc9));
			memset (szStrDisc10, 0x00, sizeof(szStrDisc10));
			// Percentage:
			memset (szStrDisc11, 0x00, sizeof(szStrDisc11));
			memset (szStrDisc12, 0x00, sizeof(szStrDisc12));
			memset (szStrDisc13, 0x00, sizeof(szStrDisc13));
			memset (szStrDisc14, 0x00, sizeof(szStrDisc14));
			memset (szStrDisc15, 0x00, sizeof(szStrDisc15));

			memset(szFixedAmountDisc, 0x00, sizeof(szFixedAmountDisc));
			memset(szOrigAmountDisc, 0x00, sizeof(szOrigAmountDisc));
			memset(szPercentageDisc, 0x00, sizeof(szPercentageDisc));

			// for Discount function
			wub_hex_2_str(srTransRec.szFixedAmount, szStrDisc1, AMT_BCD_SIZE);
			wub_hex_2_str(srTransRec.szOrigAmountDisc, szStrDisc6, AMT_BCD_SIZE);	
			wub_hex_2_str(srTransRec.szPercentage, szStrDisc11, AMT_BCD_SIZE);

			vdDebug_LogPrintf("ushCTOS_PrintBody [%s][%s]", srTransRec.szPercentage, szStrDisc11);			
		#endif
		
		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

                if((srTransRec.HDTid == 17) || (strcmp(strCST.szCurSymbol, "MMK") == 0)){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
                    memset(szTemp, 0x00, sizeof(szTemp));
                    strcpy(szTemp, szTempAmt);
                    
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp1)/100);
                    memset(szTemp1, 0x00, sizeof(szTemp1));
                    strcpy(szTemp1, szTempAmt);

					#ifdef DISCOUNT_FEATURE 	
						if(srTransRec.fIsDiscounted == TRUE){
						//Fixed Rate - for Discount function
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1)/100);
						strcpy(szStrDisc3, szStrDisc2);
						//			

						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6)/100);
						strcpy(szStrDisc8, szStrDisc7);
						//	

						//Percentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11)/100);
						strcpy(szStrDisc13, szStrDisc12);
						}
						//						
					#endif					
                }
				else
				{
				#ifdef DISCOUNT_FEATURE 	
					if(srTransRec.fIsDiscounted == TRUE){
					//Fixed Rate - for Discount function
					sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1));
					strcpy(szStrDisc3, szStrDisc2);
					//			

					//Original Amount - for Discount function
					sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6));
					strcpy(szStrDisc8, szStrDisc7);
					//	

					//Percentage  - for Discount function
					sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11));
					strcpy(szStrDisc13, szStrDisc12);
					}
					//						
				#endif
				}
                
		if (strTCT.inFontFNTMode == 1)
			 vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
//       		vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
               
		else//use ttf print
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		    //vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
		if (srTransRec.byTransType == SALE)
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }

			vdDebug_LogPrintf("VVVVVVVVVVVVVVVVV");			
					
			//Base amount
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			memset(szTemp3, 0x00, sizeof(szTemp3));

			#ifdef DISCOUNT_FEATURE
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){

               	vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);

				if(srTransRec.fIsDiscounted == TRUE){
					// for fixed rate
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4); 
					// Original Amount
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9); 	

					// Percent / Amount
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
				}
				
				
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);

				if(srTransRec.fIsDiscounted == TRUE){
					// for fixed rate
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4); 
					// Original Amount
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9); 		
					// Percent / Amount
	                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14); 
				}
				
            }

			vdDebug_LogPrintf("szStrDisc14 : szTemp5 [%s][%s]", szStrDisc14, szTemp5);

			//if (TRUE ==strTCT.fTipAllowFlag)
			if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
			{
				vdDebug_LogPrintf("AAAAAAAA [%s][%s]", szStrDisc4, szStrDisc9);			
				//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol,szTemp5);		
				sprintf(szTemp3, "  %s %s", strCST.szCurSymbol,szTemp5);		
				 
			}
			else
			{
				vdDebug_LogPrintf("BBBBBBBB [%s][%s]", szStrDisc15, szStrDisc14);
			
            	sprintf(szTemp3, "%s %s", strCST.szCurSymbol,szTemp5);		

				if(srTransRec.fIsDiscounted == TRUE){
					// for fixed rate
	            	sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol,szStrDisc4); 
					// for Original Amount
					sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol,szStrDisc9); 

					// for Percent / amount
					if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
						sprintf(szStrDisc15, "%s", szStrDisc14); 
					else
					{
						len = strlen(szStrDisc15);
						memcpy(szStrDisc15, &szStrDisc14[0], len-2);
					}

				}
								  
			}                  			
			#else			

			vdDebug_LogPrintf("szTemp1 : fTipAllowFlag [%s][%d]", szTemp1, strTCT.fTipAllowFlag);
			
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
            }

			if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)		
				 sprintf(szTemp3, "  %s %s", strCST.szCurSymbol,szTemp5);//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol,szTemp5);
			else
              sprintf(szTemp3, "%s %s", strCST.szCurSymbol,szTemp5);

			#endif                 

			#ifdef DISCOUNT_FEATURE
			vdDebug_LogPrintf("MMMMMMMMMMMMMM");			

			vdDebug_LogPrintf("szStrDisc15:szTemp5 [%s][%s][%s]", szStrDisc15, szTemp5, szTemp3);

			
			//if (TRUE ==strTCT.fTipAllowFlag){					
			if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE){		
				 sprintf(szStr, "AMT:%s", szTemp3);
			}					     
			 else
			 {
				// put discount details here
				//........................
				if(srTransRec.fIsDiscounted == TRUE){		

					strcat(szStrDisc15, " % Disc");
					
					if(srTransRec.fIsDiscountedFixAmt == TRUE)
						sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5); 	//Fixed Amount:					 						
					else													
						sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);		//Percentage Amount:	

						
						//Original Amount:							
						sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10);

						//Total Amount:
						sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);	

				}
			 	else
                	sprintf(szStr, "AMOUNT:  %s", szTemp3);
			 }			
			#else
			vdDebug_LogPrintf("ZZZZZZZ::szTemp3:szTemp5:fTipAllowFlag [%s][%s][%d]", szTemp3, szTemp5, strTCT.fTipAllowFlag);

			
					if (TRUE ==strTCT.fTipAllowFlag){					
						 sprintf(szStr, "AMT:%s", szTemp3);	
					}
					 else{
                        sprintf(szStr, "AMOUNT:  %s", szTemp3);
					 }
            #endif
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
						memset (baTemp, 0x00, sizeof(baTemp));
                        
                        if(srTransRec.fIsInstallment == TRUE){
                            ushCTOS_PrintBodyInstallment();
                        } 
						else
						{
                            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							
							#ifdef DISCOUNT_FEATURE
								if(srTransRec.fIsDiscounted == TRUE)
								{								
									vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
									if(srTransRec.fIsDiscountedFixAmt == TRUE)
										inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
									else
										inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
										
									inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1); // for Original amount printout	
									
									inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);									
									inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);		  // For total discounted amount printout
								}
								else
	                            	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

								vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
							
							#else
	                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
							#endif
                        }
                        
			//Tip amount
			memset(szStr, ' ', d_LINE_SIZE);
			sprintf(szStr,"%s   %s","TIP",strCST.szCurSymbol);
			memset (baTemp, 0x00, sizeof(baTemp));
			memset (szTemp4, 0x00, sizeof(szTemp4));
			wub_hex_2_str(srTransRec.szTipAmount, szTemp4, AMT_BCD_SIZE);
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTemp4)/100);
                memset(szTemp4, 0x00, sizeof(szTemp4));
                strcpy(szTemp4, szTempAmt);
            }

			DebugAddSTR("TIP:",szTemp4,12);
            //if (TRUE ==strTCT.fTipAllowFlag)
            if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
            {     	
			   	
 				if (strTCT.inFontFNTMode != 1)
			 	  vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
	              //vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

				  vdDebug_LogPrintf("KIKO -> szTemp5:[%s] szTemp4:[%s]", szTemp5, szTemp4);

			    if(atol(szTemp4) > 0)
    			{
    				memset(szStr, ' ', d_LINE_SIZE);

					memset(szTemp5,0x00, sizeof(szTemp5));


					vdDebug_LogPrintf("KIKO 111111");
										
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp4, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp4, szTemp5);
                    }
                                            
					//sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TIP:  %s %s", strCST.szCurSymbol,szTemp5);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);					
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);									
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("____________________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
					DebugAddSTR("TOT:",szTemp5,12);
                                        
					//sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TOT:  %s %s", strCST.szCurSymbol,szTemp5);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");				
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("      ____________________",&stgFONT_ATTRIB,1);
    			}
                else
                {

					vdDebug_LogPrintf("KIKO 222222");
                
                    memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp4, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp4, szTemp5);
                    }					
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", "0", szTemp5);
					//sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TIP:  %s %s", strCST.szCurSymbol,szTemp5);

                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);									
					
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("      ____________________",&stgFONT_ATTRIB,1);
        			
        			//Total amount
        			memset(szStr, ' ', d_LINE_SIZE);
					memset(szTemp5,0x00, sizeof(szTemp5));
					//format amount 10+2
					//vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
					//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
					
					//sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
					sprintf(szStr, "TOT:  %s %s", strCST.szCurSymbol,szTemp5);
        			memset (baTemp, 0x00, sizeof(baTemp));
        			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
        			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
					//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
        			//CTOS_PrinterPutString("                 ____________________");
					//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
					//inCCTOS_PrinterBufferOutput("      ____________________",&stgFONT_ATTRIB,1);
                }
            }
			
			
		}

		#if 0 // TODO: Remove -- not needed for CBPAY receipt
		else if (srTransRec.byTransType == SALE_TIP)
		{	 			    
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }

			vdDebug_LogPrintf("KIKO 222222.111");
                    
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                memset(szTemp3, 0x00, sizeof(szTemp3));
                strcpy(szTemp3, szTempAmt);
            }

			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
            } else{
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
            }
                        
			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
            }
                        
			sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
					
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
            }
                        
			sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");	
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
        	//inCTOSS_CapturePrinterBuffer("                 ____________________",&stgFONT_ATTRIB);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			
		}
		#endif
		
	 else if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH || srTransRec.byTransType == MPU_VOID_PREAUTH_COMP || srTransRec.byTransType == MPU_VOID_PREAUTH)
	 {
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }

					if(strcmp(strCST.szCurSymbol, "MMK") == 0){
					#ifdef DISCOUNT_FEATURE
					//Fixed Rate - for Discount function
					if(srTransRec.fIsDiscounted == TRUE){
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1)/100);
						strcpy(szStrDisc3, szStrDisc2);

						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6)/100);
						strcpy(szStrDisc8, szStrDisc7);
					
						//Perentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11)/100);
						strcpy(szStrDisc13, szStrDisc12);
						
					}					
					#endif
					}
					else
					{
						#ifdef DISCOUNT_FEATURE
						//Fixed Rate - for Discount function
						if(srTransRec.fIsDiscounted == TRUE){
							sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1));
							strcpy(szStrDisc3, szStrDisc2);
						
							//Original Amount - for Discount function
							sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6));
							strcpy(szStrDisc8, szStrDisc7);
						
							//Perentage / Amount - for Discount function
							sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11));
							strcpy(szStrDisc13, szStrDisc12);
							
						}					
						#endif					
					}
                    
//                    CTOS_PrinterSetHeatLevel(4);
				    //old code to show amount only
                    memset(szStr, ' ', d_LINE_SIZE);

				    memset(szTemp5,0x00, sizeof(szTemp5));

				  #ifdef DISCOUNT_FEATURE
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
						
						//Fixed Amount:
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4);
						//Original Amount:
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9);	
						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
						
						
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
						//Fixed Amount:
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4);
						//Original Amount:
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9);
						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14);
						
						
                    }				  
				  #else
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }
                  #endif

				  // removed additional currency symbol #1

				  vdDebug_LogPrintf("V O I D ! ! ! (%d)(%s)", srTransRec.fIsDiscounted, szTemp5);	
				  #if 0
				  if (FALSE == strTCT.fTipAllowFlag)									  
				  {				  
	                    memset(szTemp3,0x00, sizeof(szTemp3));                    
	                    strcpy(szTemp3, "-");
	                    strcat(szTemp3, strCST.szCurSymbol);
	                    strcat(szTemp3, " ");
						//ex:  -MMK 

						vdDebug_LogPrintf("V O I D ! ! ! XXXXX(%s)", szTemp3); 				
						
	                    strcat(szTemp3, szTemp5);

						vdDebug_LogPrintf("V O I D ! ! ! YYYYY(%s)", szTemp3); 				

	                    memset(szTemp5, 0x00, sizeof(szTemp5));
	                    strcpy(szTemp5, szTemp3); // amount +  currency symbol
						vdDebug_LogPrintf("V O I D ! ! ! ZZZZZ(%s)", szTemp5); 				
	                    
					}

					#else
					memset(szTemp3,0x00, sizeof(szTemp3));
                    
                    strcpy(szTemp3, "-");
                    strcat(szTemp3, strCST.szCurSymbol);
                    strcat(szTemp3, " ");
                    strcat(szTemp3, szTemp5);
					#endif

					#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE)
					{
						//Fixed Amount:
						sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol, szStrDisc4);
						//Original Amount:
						sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol, szStrDisc9);	
						// Percent / Amount: 
						//sprintf(szStrDisc15, "%s", szStrDisc14); 

						if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
							sprintf(szStrDisc15, "%s", szStrDisc14); 
						else
						{
							len = strlen(szStrDisc15);
							memcpy(szStrDisc15, &szStrDisc14[0], len-2);
						}																
						
						
					}
					#endif

					vdDebug_LogPrintf("MMMMMMM (%s)", szTemp5);
					

					//if (TRUE ==strTCT.fTipAllowFlag){					
					if (TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE){		
						 //sprintf(szStr, "AMT:%s", szTemp5);
                        //sprintf(szStr, "AMT:-%s %12s", strCST.szCurSymbol, szTemp5);
                        //sprintf(szStr, "AMT:-%s    %12s", strCST.szCurSymbol, szTemp5);
                        sprintf(szStr, "AMT:  -%s %s", strCST.szCurSymbol, szTemp5);
					}
					else 	
					{
						// put discount details here
						//........................
						#ifdef DISCOUNT_FEATURE
						if(srTransRec.fIsDiscounted == TRUE)
						{		
							strcat(szStrDisc15, " % Disc");
							
							if(srTransRec.fIsDiscountedFixAmt == TRUE){
								//Fixed Amount:							
								sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5);
							}else{
								//Percentage Amount:							
								sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);							
							}

							
							//Original Amount:
							sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10);	
							
							//Total Amount:
							//sprintf(szStrDisc1, "Total    AMT: %s", szTemp5);	
							sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);	
						}
						else{
								//sprintf(szStr, "AMOUNT:   %s", szTemp5); //removed additional currency symbol #2
								sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);
						}
						#else

								//sprintf(szStr, "AMOUNT:   %s", szTemp5); //removed additional currency symbol #2
								sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);
						#endif
					}

					vdDebug_LogPrintf("NNNNNN (%s)", szStr);
                    
                    memset(szPrintingText, 0x00, sizeof(szPrintingText));
                    sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                    put_env_int("#P3LEN", strlen(szPrintingText));
                    put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                    
                    memset (baTemp, 0x00, sizeof(baTemp));
                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
            if(srTransRec.fIsInstallment == TRUE)
            {
                ushCTOS_PrintBodyInstallment();
            }
			else
			{
                //                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

				#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE){		
						
						vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
						if(srTransRec.fIsDiscountedFixAmt == TRUE)
							inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						else
							inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);  // Original Amount	
					
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);								
						inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);		  // Discounted Amount
					}
					else
						inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	                //vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                #else					
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
				#endif

                //if(TRUE ==strTCT.fTipAllowFlag 
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
                {
                    //TIP start here!
                    memset(szTemp3, ' ', d_LINE_SIZE);
                    wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        memset(szTempAmt, 0x00, sizeof(szTempAmt));
                        sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                        memset(szTemp3, 0x00, sizeof(szTemp3));
                        strcpy(szTemp3, szTempAmt);
                    }
                    
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                    {
                         vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                    } 
					else 
					{
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                    }
                    
                    //sprintf(szStr, "TIP: %16s", szTemp5);
                    if(atol(szTemp5) > 0)
                    	sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP:-%s    %12s", strCST.szCurSymbol,szTemp5);
					else
						sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP: %s    %12s", strCST.szCurSymbol,szTemp5);
					
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                    //inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
                    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                    
                    //TOTAL start here!					
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else {
                    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
                    
                    //sprintf(szStr, "TOT: %16s", szTemp5);
                    
                    //sprintf(szStr, "TOT:-%s    %12s", strCST.szCurSymbol,szTemp5);
                    sprintf(szStr, "TOT:  -%s %s", strCST.szCurSymbol,szTemp5);
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                    }
				vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
			}
                    
//                    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
			//closed old amount code!

		} 
	 	else
		{

			vdDebug_LogPrintf("ELSE!!!");			
		
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }

			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        
			#ifdef DISCOUNT_FEATURE 				
			//format amount 10+2
			if(strcmp(strCST.szCurSymbol, "MMK") == 0){
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);			
				// for fixed rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4); 
				// Original Amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9); 	

			// Percent / Amount
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
			
				
				
			} else {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
			
				// for fixed rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4); 
				// Original Amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9);	

			// Percent / Amount
			vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14);	
				
			}
			#else
				if(strcmp(strCST.szCurSymbol, "MMK") == 0){
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
				} else {
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
				}
			#endif


            memset(szTemp3, 0x00,sizeof(szTemp3));
			//if (TRUE ==strTCT.fTipAllowFlag){
			if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE){
				// sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);
				// fixed for NO amount printed on the receipt.
				//sprintf(szTemp3, "%s %12s", strCST.szCurSymbol, szTemp5);
				sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);
			}
			else
               sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);

			
            if(srTransRec.byTransType == BALANCE_ENQUIRY)
            {
				if(memcmp(srTransRec.szCurrCode1, "104", 3) == 0)
				{
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount1, szTemp3);
					strcpy(strCST.szCurSymbol, "MMK");
				}
				else
				{
					vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount1, szTemp3);
					strcpy(strCST.szCurSymbol, "USD");
				}

				if(srTransRec.szSign1[0] == 'D') /*D - negative sign*/
				    sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
                else
					sprintf(szStr, "BAL: %s %s", strCST.szCurSymbol, szTemp3);
				
				vdCTOS_SetFontType(d_FONT_COURBD_TTF);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

                if(strlen(srTransRec.szCurrCode2))
                {
					memset(szTemp3, 0, sizeof(szTemp3));
					memset(szStr, ' ', d_LINE_SIZE);

                    if(memcmp(srTransRec.szCurrCode2, "104", 3) == 0)
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount2, szTemp3);
						strcpy(strCST.szCurSymbol, "MMK");
                    }
                    else
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount2, szTemp3);
						strcpy(strCST.szCurSymbol, "USD");
                    }
                    
					if(srTransRec.szSign2[0] == 'D') /*D - negative sign*/
						sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
					else
						sprintf(szStr, "BAL: %s %s", strCST.szCurSymbol, szTemp3);


                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                }
				vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
            }
            else
			{
			   // put discount details here
			   //........................
			   #ifdef DISCOUNT_FEATURE
			   if(srTransRec.fIsDiscounted == TRUE){	   		   
		   
				   //Fixed Amount:
				   if(srTransRec.fIsDiscountedFixAmt == TRUE)
					   sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5);	   //Fixed Amount:										   
				   else 												   
					   sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);	   //Percentage Amount:    
					   
				   //Original Amount:
				   sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10);			   
				   //Total Amount:
				   sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);  		   
			   }
			   else					   	
               		sprintf(szStr, "AMOUNT: %s", szTemp3);
			   #else
			   		sprintf(szStr, "AMOUNT: %s", szTemp3);
			   #endif
		   	}

				
            memset(szPrintingText, 0x00, sizeof(szPrintingText));
            sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
            put_env_int("#P3LEN", strlen(szPrintingText));
            put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
                        if(srTransRec.fIsInstallment == TRUE)
                        {
                            ushCTOS_PrintBodyInstallment();
                        } 
						else if(srTransRec.byTransType == BALANCE_ENQUIRY)
						{
							
						}
						else
						{
                            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

							#ifdef DISCOUNT_FEATURE			
								if(srTransRec.fIsDiscounted == TRUE){
									
									vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
									if(srTransRec.fIsDiscountedFixAmt == TRUE)
										inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
									else
										inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for percentage disc amt  printout
									inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);	

									
									inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
									inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);
								}
								else
	                            	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);							
                        
							#else
	                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							
	                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);

							#endif
                        }
		}
		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
		
	}
	else if(d_SECOND_PAGE == page)
 //       else
	{
    	memset(szTemp1, ' ', d_LINE_SIZE);
    	sprintf(szTemp1,"%s",srTransRec.szHostLabel);
    	//sprintf(szTemp1,"%s",strHDT.szHostLabel);
    	vdPrintCenter(szTemp1);
		
		
		inResult = printTIDMIDCBPay(); 

		sprintf(szStr, "SUB MID: %s", strCBPay.szSubMerId);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
		
		printDateTime();
		//printBatchInvoiceNO(); // pat confirm hang

		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		memset(szStr, ' ', d_LINE_SIZE);
    		//szGetTransTitle(srTransRec.byTransType, szStr);

		memset(szStr, ' ', d_LINE_SIZE);
		strcpy(szStr, "CBPay SALES");

#if 1
		if(srTransRec.byOrgTransType == SALE_OFFLINE && srTransRec.byTransType == VOID){
			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr, "VOID OFFLINE");
		} else if(srTransRec.byOrgTransType == CB_PAY_TRANS && srTransRec.byTransType == VOID){
			memset(szStr, 0x00, sizeof(szStr));
			strcpy(szStr, "VOID CBPay SALES");
		}
#endif		
		

		/**/
		vdCTOS_ModifyTransTitleString(szStr);
		
  		vdPrintTitleCenter(szStr);    



if (strTCT.inFontFNTMode == 1)
		 vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
else
	//use ttf print
	 vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);

		

#if 	0  //REMOVE THIS
	    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		memset(szStr, ' ', d_LINE_SIZE);
		vdMyEZLib_LogPrintf("CDT index: %d",srTransRec.CDTid);
		
		sprintf(szStr, "%s",srTransRec.szCardLable);
		vdMyEZLib_LogPrintf("Card label: %s",srTransRec.szCardLable);
		
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(srTransRec.szPAN,szTmpPan,d_LINE_SIZE,page);
		strcpy(szTemp4, szTmpPan);
		memset (baTemp, 0x00, sizeof(baTemp));
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp4, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp4,&stgFONT_ATTRIB,1);

		if (strTCT.inFontFNTMode == 1)
        	vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
        
		//Exp date and Entry mode
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, 0, sizeof(szTemp));
		memset(szTemp1, 0, sizeof(szTemp1));
		memset(szTemp4, 0, sizeof(szTemp4));
		memset(szTemp5, 0, sizeof(szTemp5));
		wub_hex_2_str(srTransRec.szExpireDate, szTemp,EXPIRY_DATE_BCD_SIZE);
		DebugAddSTR("EXP",szTemp,12);
        memset(szTemp1, 0x00, sizeof(szTemp1));
        memcpy(szTemp1, szTemp+2, 2);
        memcpy(szTemp1+2, szTemp, 2);
        
        memset(szTemp, 0x00, sizeof(szTemp));
        strcpy(szTemp, szTemp1);
		memset (szTmpPan, 0x00, sizeof(szTmpPan));
		//start base on issuer IIT and mask card pan and expire data
		vdCTOSS_PrintFormatPAN(szTemp,szTmpPan,(EXP_DATE_SIZE + 1),page);
		
		//for (i =0; i<4;i++)
		//	szTemp[i] = '*';
		memcpy(szTemp4,&szTmpPan[0],2);
		memcpy(szTemp5,&szTmpPan[2],2);

		if(srTransRec.byEntryMode==CARD_ENTRY_ICC || srTransRec.byEntryMode==CARD_ENTRY_EASY_ICC)
			memcpy(szTemp1,"Chip",4);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MANUAL)
			memcpy(szTemp1,"Manual",6);
		else if(srTransRec.byEntryMode==CARD_ENTRY_MSR)
			memcpy(szTemp1,"MSR",3);
		else if(srTransRec.byEntryMode==CARD_ENTRY_FALLBACK)
			memcpy(szTemp1,"Fallback",8);
		else if(srTransRec.byEntryMode==CARD_ENTRY_WAVE)
		{
			if ('4' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayWave",7);
			if ('5' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"PayPass",7);
			if ('3' ==srTransRec.szPAN[0])
				memcpy(szTemp1,"ExpressPay",10);
			if (('3' ==srTransRec.szPAN[0])&&('5' ==srTransRec.szPAN[1]))
				memcpy(szTemp1,"J/Speedy",8);
            if(srTransRec.bWaveSID == d_VW_SID_CUP_EMV)
            {
                memcpy(szTemp1,"QuickPass",9);
            }
		}
		
		DebugAddSTR("ENT:",szTemp1,12);  
		memset (baTemp, 0x00, sizeof(baTemp));

		memset (baTemp, 0x00, sizeof(baTemp));
		sprintf(szTemp,"%s%s/%s          %s%s","EXP: ",szTemp4,szTemp5,"ENT:",szTemp1);
		
		DebugAddSTR("ENT:",baTemp,12);  
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szTemp, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		inCCTOS_PrinterBufferOutput(szTemp,&stgFONT_ATTRIB,1);



#endif


		
		//reference number
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
			sprintf(szStr, "REF NO  : %s", srTransRec.szRefNo);
		else
			//use ttf print
		sprintf(szStr, "REF NO      :   %s", srTransRec.szRefNo);
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);


		//print  trace num
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);	
		    wub_hex_2_str(srTransRec.szInvoiceNo, baTemp, INVOICE_BCD_SIZE);

		// for case http://118.201.48.210:8080/redmine/issues/1525.34.1 	   
		//sprintf(szStr, "TRACE NO. : %s", baTemp);				
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "INV NO. : %s", baTemp);
		else
		   sprintf(szStr, "INV NO.   :   %s", baTemp);

		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);


		//Auth response code
		memset(szStr, ' ', d_LINE_SIZE);
		memset (baTemp, 0x00, sizeof(baTemp));					
		memset(szStr, ' ', d_LINE_SIZE);	
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "TRANX ID : %s", srTransRec.szBankTransId);
		else
		   sprintf(szStr, "TRANX ID    :   %s", srTransRec.szBankTransId);
		
		memset (baTemp, 0x00, sizeof(baTemp));		
		//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
		//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
		//inCTOSS_CapturePrinterBuffer(szStr,&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);


		//print CB PAY ACC  -- The value for CB PAY is  intentionally blank.
		memset(szStr, ' ', d_LINE_SIZE);
		if (strTCT.inFontFNTMode == 1)
		   	sprintf(szStr, "CBPay A/C  : ");
		else
		   sprintf(szStr, "CBPay A/C   : ");

		inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	
	
		memset(szStr, ' ', d_LINE_SIZE);
		memset(szTemp, ' ', d_LINE_SIZE);
		memset(szTemp1, ' ', d_LINE_SIZE);
		memset(szTemp3, ' ', d_LINE_SIZE);

// for Discount feature
#ifdef DISCOUNT_FEATURE
		// FIxed Rate:
		memset (szStrDisc1, 0x00, sizeof(szStrDisc1));
		memset (szStrDisc2, 0x00, sizeof(szStrDisc2));
		memset (szStrDisc3, 0x00, sizeof(szStrDisc3));
		memset (szStrDisc4, 0x00, sizeof(szStrDisc4));
		memset (szStrDisc5, 0x00, sizeof(szStrDisc5));
		//Original Amount:
		memset (szStrDisc6, 0x00, sizeof(szStrDisc6));
		memset (szStrDisc7, 0x00, sizeof(szStrDisc7));
		memset (szStrDisc8, 0x00, sizeof(szStrDisc8));
		memset (szStrDisc9, 0x00, sizeof(szStrDisc9));
		memset (szStrDisc10, 0x00, sizeof(szStrDisc10));
		// Percentage:
		memset (szStrDisc11, 0x00, sizeof(szStrDisc11));
		memset (szStrDisc12, 0x00, sizeof(szStrDisc12));
		memset (szStrDisc13, 0x00, sizeof(szStrDisc13));
		memset (szStrDisc14, 0x00, sizeof(szStrDisc14));
		memset (szStrDisc15, 0x00, sizeof(szStrDisc15));	
		

		memset(szFixedAmountDisc, 0x00, sizeof(szFixedAmountDisc));
		memset(szOrigAmountDisc, 0x00, sizeof(szOrigAmountDisc));
		memset(szPercentageDisc, 0x00, sizeof(szPercentageDisc));		
		//

		// for Discount function
		wub_hex_2_str(srTransRec.szFixedAmount, szStrDisc1, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szOrigAmountDisc, szStrDisc6, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szPercentage, szStrDisc11, AMT_BCD_SIZE);
		
#endif

		wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
                
                if((srTransRec.HDTid == 17) || (strcmp(strCST.szCurSymbol, "MMK") == 0)){
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
                    memset(szTemp, 0x00, sizeof(szTemp));
                    strcpy(szTemp, szTempAmt);
                    
                    memset(szTempAmt, 0x00, sizeof(szTempAmt));
                    sprintf(szTempAmt, "%012.0f", atof(szTemp1)/100);
                    memset(szTemp1, 0x00, sizeof(szTemp1));
                    strcpy(szTemp1, szTempAmt);

					#ifdef DISCOUNT_FEATURE 				
					if(srTransRec.fIsDiscounted == TRUE){
						//Fixed Rate - for Discount function
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1)/100);
						strcpy(szStrDisc3, szStrDisc2);
						//			
						
						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6)/100);
						strcpy(szStrDisc8, szStrDisc7);
						//						
					
						//Percentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11)/100);
						strcpy(szStrDisc13, szStrDisc12);
					}				
					#endif					
                }
				else
				{
					#ifdef DISCOUNT_FEATURE 				
					if(srTransRec.fIsDiscounted == TRUE){
						//Fixed Rate - for Discount function
						sprintf(szStrDisc2, "%012.0f", atof(szStrDisc1));
						strcpy(szStrDisc3, szStrDisc2);
						//			
						
						//Original Amount - for Discount function
						sprintf(szStrDisc7, "%012.0f", atof(szStrDisc6));
						strcpy(szStrDisc8, szStrDisc7);
						//						
					
						//Percentage / Amount - for Discount function
						sprintf(szStrDisc12, "%012.0f", atof(szStrDisc11));
						strcpy(szStrDisc13, szStrDisc12);
					}				
					#endif
				}

		if (strTCT.inFontFNTMode == 1)
			  vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
      //        	vdSetGolbFontAttrib(d_FONT_16x16, DOUBLE_SIZE, DOUBLE_SIZE, 0, 0);
                  
		else	//use ttf print				
		 	vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);		    
			//vdSetGolbFontAttrib(d_FONT_16x30, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

			
        
		if (srTransRec.byTransType == SALE)
		{

			vdDebug_LogPrintf("SALE Print ... ");
		
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }
			//Base amount
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
            memset(szTemp3, 0x00, sizeof(szTemp3));

			#ifdef DISCOUNT_FEATURE
				//format amount 10+2
				if(strcmp(strCST.szCurSymbol, "MMK") == 0){
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
				//Fixed Rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4);							
				//Original amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9);
				// Percent / Amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
				

				} else {
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
				//Fixed Rate
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4);
				//Original amount
				vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9);
				// Percent / Amount
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14); 					
				}

				//if (TRUE ==strTCT.fTipAllowFlag)
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
				{
					 //sprintf(szTemp3, "%s %16s", strCST.szCurSymbol,szTemp5);
					 sprintf(szTemp3, "  %s %s", strCST.szCurSymbol,szTemp5);

					 
					 if(srTransRec.fIsDiscounted == TRUE){
					 // for fixed rate
					 sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol,szStrDisc4); 
					// for Original amount
					sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol,szStrDisc9); 	
					// for Percent / amount
					//sprintf(szStrDisc15, "%s", szStrDisc14); 
					}

					if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
						sprintf(szStrDisc15, "%s", szStrDisc14); 
					else
					{
						len = strlen(szStrDisc15);
						memcpy(szStrDisc15, &szStrDisc14[0], len-2);
					}							
				}
				else
				{
					sprintf(szTemp3, "%s %s", strCST.szCurSymbol,szTemp5);							  
					// for fixed rate
					sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol,szStrDisc4); 
					// for Original Amount
					sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol,szStrDisc9);
					// for Percent / amount
					//sprintf(szStrDisc15, "%s", szStrDisc14); 
					if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
						sprintf(szStrDisc15, "%s", szStrDisc14); 
					else
					{
						len = strlen(szStrDisc15);
						memcpy(szStrDisc15, &szStrDisc14[0], len-2);
					}
					
					
				}			

				//if (TRUE ==strTCT.fTipAllowFlag)
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
					 sprintf(szStr, "AMT:%s", szTemp3);				
				else{
					// put discount details here
					//........................
					if(srTransRec.fIsDiscounted == TRUE){				

						strcat(szStrDisc15, " % Disc");
						//Fixed Amount: 						
						if(srTransRec.fIsDiscountedFixAmt == TRUE)
							sprintf(szFixedAmountDisc, "Fixed Dis AMT: %s", szStrDisc5); 	//Fixed Amount: 										
						else													
							sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15); 	//Percentage Amount:	
						
						//Original Amount:
						sprintf(szOrigAmountDisc, "Sale      AMT: %s", szStrDisc10); 				
						//Total Amount:
						sprintf(szStrDisc1, "Total     AMT: %s", szTemp3);				
					}
					else										
						sprintf(szStr, "AMOUNT:  %s", szTemp3);
				}						 
				
			#else
			//format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }

					if (TRUE ==strTCT.fTipAllowFlag)
					sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);
					else
                       sprintf(szTemp3, "%s %s", strCST.szCurSymbol, szTemp5);
                        
					if (TRUE ==strTCT.fTipAllowFlag)
						 sprintf(szStr, "AMT:%s", szTemp3);
					else
                        sprintf(szStr, "AMOUNT:  %s", szTemp3);
             #endif
                        
            memset(szPrintingText, 0x00, sizeof(szPrintingText));
            sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
            put_env_int("#P3LEN", strlen(szPrintingText));
            put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        
            if(srTransRec.fIsInstallment == TRUE)
            {
                ushCTOS_PrintBodyInstallment();
            } 
            else 
            {
                vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

				#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE){
						
						vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
						if(srTransRec.fIsDiscountedFixAmt == TRUE)
							inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						else
							inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
						inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);		

						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);								
						inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);
					}
					else
	                	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);				
				#else
					inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);				
				
				#endif
				
                //if (TRUE ==strTCT.fTipAllowFlag)                
				if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
                {

					vdDebug_LogPrintf("KIKO 333333");
				
                    //TIP start here!
                    memset(szTemp3, ' ', d_LINE_SIZE);
                    wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                    {
                        memset(szTempAmt, 0x00, sizeof(szTempAmt));
                        sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                        memset(szTemp3, 0x00, sizeof(szTemp3));
                        strcpy(szTemp3, szTempAmt);
                    }
                
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
                    } 
                    else 
                    {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
                    }
                
                    //sprintf(szStr, "TIP: %16s", szTemp5);
                    //sprintf(szStr, "TIP:%s     %12s", strCST.szCurSymbol,szTemp5);
                    sprintf(szStr, "TIP:  %s %s", strCST.szCurSymbol,szTemp5);
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                    //inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
                    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                    
                    //TOTAL start here!					
                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                    }
                    
                    //sprintf(szStr, "TOT: %16s", szTemp5);
                    
                    //sprintf(szStr, "TOT:%s     %12s", strCST.szCurSymbol,szTemp5);
                    sprintf(szStr, "TOT:  %s %s", strCST.szCurSymbol,szTemp5);
                    memset (baTemp, 0x00, sizeof(baTemp));
                    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                }
                vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
            }
		}
		else if (srTransRec.byTransType == SALE_TIP)
		{

			vdDebug_LogPrintf("KIKO 444444");
		
            if(srTransRec.HDTid == 17){
                inCSTRead(1);
            }
			memset(szTemp3, ' ', d_LINE_SIZE);
			wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
                        
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                memset(szTempAmt, 0x00, sizeof(szTempAmt));
                sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
                memset(szTemp3, 0x00, sizeof(szTemp3));
                strcpy(szTemp3, szTempAmt);
            }
                        
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
            }
                        
			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
            }
                        
			sprintf(szStr, "TIP:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
            } else {
                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
            }
                        
			sprintf(szStr, "TOT:%s %16s", strCST.szCurSymbol,szTemp5);
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
			//CTOS_PrinterPutString("                 ____________________");
			inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput("                 ____________________",&stgFONT_ATTRIB,1);
			
		} else if(srTransRec.byTransType == VOID || srTransRec.byTransType == VOID_PREAUTH){ //@@IBR ADD 03102016
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }

                    memset(szStr, ' ', d_LINE_SIZE);
                    memset(szTemp5,0x00, sizeof(szTemp5));


					#ifdef DISCOUNT_FEATURE
					//format amount 10+2
		            if(strcmp(strCST.szCurSymbol, "MMK") == 0){
		                
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
		                
						// for fixed rate
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc3, szStrDisc4); 
						// Original Amount
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc8, szStrDisc9); 	

						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szStrDisc13, szStrDisc14); 
						
						
		            } else {
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
						// for fixed rate
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc3, szStrDisc4); 
						// Original Amount
		                vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc8, szStrDisc9); 	
						// Percent / Amount
						vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szStrDisc13, szStrDisc14);	
						
		            }						
					#else
                    //format amount 10+2
                    if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp1, szTemp5);
                    } else {
                        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp1, szTemp5);
                    }
                    #endif
					
                    memset(szTemp3,0x00, sizeof(szTemp3));
                    
                    strcpy(szTemp3, "-");
                    strcat(szTemp3, strCST.szCurSymbol);
                    strcat(szTemp3, " ");
                    strcat(szTemp3, szTemp5);

					vdDebug_LogPrintf("VOID SECTION!!! (%s)", szTemp3);

					#ifdef DISCOUNT_FEATURE
					if(srTransRec.fIsDiscounted == TRUE)
					{
						//Fixed Amount:
						sprintf(szStrDisc5, "%s %s", strCST.szCurSymbol, szStrDisc4);
						//Original Amount:
						sprintf(szStrDisc10, "%s %s", strCST.szCurSymbol, szStrDisc9);		
						// Percent / Amount:
						//sprintf(szStrDisc15, "%s", szStrDisc14); 
						if(strcmp(strCST.szCurSymbol, "MMK") == 0)		
							sprintf(szStrDisc15, "%s", szStrDisc14); 
						else
						{
							len = strlen(szStrDisc15);
							memcpy(szStrDisc15, &szStrDisc14[0], len-2);
						}			
						
					}
					#endif

					#ifdef DISCOUNT_FEATURE
					//if (TRUE ==strTCT.fTipAllowFlag)
					if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
					    sprintf(szStr, "AMT:  -%s %s", strCST.szCurSymbol, szTemp5);//sprintf(szStr, "AMT:-%s    %12s", strCST.szCurSymbol, szTemp5);
					else{
						// put discount details here
						//........................
						if(srTransRec.fIsDiscounted == TRUE){		
							strcat(szStrDisc15, " % Disc");

							//Fixed Amount:							
							if(srTransRec.fIsDiscountedFixAmt == TRUE){
								//Fixed Amount:							
								sprintf(szFixedAmountDisc, "Fixed Disc AMT: %s", szStrDisc5);
							}else{
								//Percentage Amount:							
								sprintf(szPercentageDisc, "PERCENTAGE  : %s", szStrDisc15);							
							}

							
							//Original Amount:
							sprintf(szOrigAmountDisc, "Sale     AMT: %s", szStrDisc10);
							//Total Amount:
							sprintf(szStrDisc1, "Total    AMT: %s", szTemp3);
						}
						else						
							sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);
						}
					
					#else
					if (TRUE ==strTCT.fTipAllowFlag){
					//sprintf(szStr, "AMT:%s", szTemp5);
					    sprintf(szStr, "AMT:  -%s %s", strCST.szCurSymbol, szTemp5);//sprintf(szStr, "AMT:-%s    %12s", strCST.szCurSymbol, szTemp5);
					}
					else	
					    sprintf(szStr, "AMOUNT:-%s %s", strCST.szCurSymbol, szTemp5);

					#endif
					
                    memset(szPrintingText, 0x00, sizeof(szPrintingText));
                    sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                    put_env_int("#P3LEN", strlen(szPrintingText));
                    put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                    
                    memset (baTemp, 0x00, sizeof(baTemp));
                    //CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
                    //CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                    
					if(srTransRec.fIsInstallment == TRUE)
					{
						ushCTOS_PrintBodyInstallment();
					}
					else
					{
						//							  vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
						vdCTOS_SetFontType(d_FONT_COURBD_TTF);
						inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);

						#ifdef DISCOUNT_FEATURE
						if(srTransRec.fIsDiscounted == TRUE){
							
							vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
							if(srTransRec.fIsDiscountedFixAmt == TRUE)
								inCCTOS_PrinterBufferOutput(szFixedAmountDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
							else
								inCCTOS_PrinterBufferOutput(szPercentageDisc,&stgFONT_ATTRIB,1); // for fixed amount printout
							inCCTOS_PrinterBufferOutput(szOrigAmountDisc,&stgFONT_ATTRIB,1);	
						
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);								
							inCCTOS_PrinterBufferOutput(szStrDisc1,&stgFONT_ATTRIB,1);
						}
						else
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
						
						#else
						inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
						//vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
						#endif
						
						//if (TRUE ==strTCT.fTipAllowFlag)						
						if(TRUE ==strTCT.fTipAllowFlag && srTransRec.fIsDiscounted != TRUE)
						{
							//TIP start here!
							memset(szTemp3, ' ', d_LINE_SIZE);
							wub_hex_2_str(srTransRec.szTipAmount, szTemp3, AMT_BCD_SIZE);
							if(strcmp(strCST.szCurSymbol, "MMK") == 0){
								memset(szTempAmt, 0x00, sizeof(szTempAmt));
								sprintf(szTempAmt, "%012.0f", atof(szTemp3)/100);
								memset(szTemp3, 0x00, sizeof(szTemp3));
								strcpy(szTemp3, szTempAmt);
							}
							
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp5,0x00, sizeof(szTemp5));
							//format amount 10+2
							if(strcmp(strCST.szCurSymbol, "MMK") == 0)
							{
								 vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp3, szTemp5);
							} 
							else 
							{
								vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp3, szTemp5);
							}
							
							//sprintf(szStr, "TIP: %16s", szTemp5);
							//sprintf(szStr, "TIP:-%s   %12s", strCST.szCurSymbol,szTemp5);
							if(atol(szTemp5) > 0)
								sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP:-%s    %12s", strCST.szCurSymbol,szTemp5);
							else
								sprintf(szStr, "TIP:  -%s %s", strCST.szCurSymbol,szTemp5);//sprintf(szStr, "TIP: %s	   %12s", strCST.szCurSymbol,szTemp5);
							
							memset (baTemp, 0x00, sizeof(baTemp));
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							//inCCTOS_PrinterBufferOutput(" 				____________________",&stgFONT_ATTRIB,1);
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							
							//TOTAL start here! 				
							memset(szStr, ' ', d_LINE_SIZE);
							memset(szTemp5,0x00, sizeof(szTemp5));
							//format amount 10+2
							if(strcmp(strCST.szCurSymbol, "MMK") == 0){
							vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
							} else {
							vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
							}
							
							//sprintf(szStr, "TOT: %16s", szTemp5);
							
							//sprintf(szStr, "TOT:-%s    %12s", strCST.szCurSymbol,szTemp5);
							sprintf(szStr, "TOT:  -%s %s", strCST.szCurSymbol,szTemp5);
							memset (baTemp, 0x00, sizeof(baTemp));
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							}
						vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
					}

                }
		else
		{
                    if(srTransRec.HDTid == 17){
                        inCSTRead(1);
                    }
			memset(szStr, ' ', d_LINE_SIZE);
			memset(szTemp5,0x00, sizeof(szTemp5));
			//format amount 10+2
                        if(strcmp(strCST.szCurSymbol, "MMK") == 0){
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp5);
                        } else {
                            vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp5);
                        }
                        
//			sprintf(szStr, "AMT:%s %16s", strCST.szCurSymbol,szTemp5);
                        memset(szTemp3, 0x00,sizeof(szTemp3));
						if (TRUE ==strTCT.fTipAllowFlag)
						   sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);//sprintf(szTemp3, "%s %16s", strCST.szCurSymbol, szTemp5);
						else							
                           sprintf(szTemp3, "  %s %s", strCST.szCurSymbol, szTemp5);
//                        sprintf(szStr, "AMOUNT: %16s", szTemp3);

						if(srTransRec.byTransType == BALANCE_ENQUIRY)
						{
							if(memcmp(srTransRec.szCurrCode1, "104", 3) == 0)
							{
								vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount1, szTemp3);
								strcpy(strCST.szCurSymbol, "MMK");
							}
							else
							{
								vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount1, szTemp3);
								strcpy(strCST.szCurSymbol, "USD");
							}
						
							if(srTransRec.szSign1[0] == 'D') /*D - negative sign*/
								sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
							else
								sprintf(szStr, "BAL: %s %s",  strCST.szCurSymbol, szTemp3);
							
							vdCTOS_SetFontType(d_FONT_COURBD_TTF);
							inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
							inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
						
							if(strlen(srTransRec.szCurrCode2))
							{
								memset(szTemp3, 0, sizeof(szTemp3));
								memset(szStr, ' ', d_LINE_SIZE);
						
								if(memcmp(srTransRec.szCurrCode2, "104", 3) == 0)
								{
									vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", srTransRec.szBalAmount2, szTemp3);
									strcpy(strCST.szCurSymbol, "MMK");
								}
								else
								{
									vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", srTransRec.szBalAmount2, szTemp3);
									strcpy(strCST.szCurSymbol, "USD");
								}
								
								if(srTransRec.szSign2[0] == 'D') /*D - negative sign*/
									sprintf(szStr, "BAL: %s%s %s", "-", strCST.szCurSymbol, szTemp3);
								else
									sprintf(szStr, "BAL: %s %s",  strCST.szCurSymbol, szTemp3);

								inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
							}
							vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
						}
                        else
                        {
                            if (TRUE ==strTCT.fTipAllowFlag)
                                sprintf(szStr, "AMOUNT: %s", szTemp3);//sprintf(szStr, "AMT:%s", szTemp3);
                            else						
                                sprintf(szStr, "AMOUNT: %s", szTemp3);
                        }
						
                        memset(szPrintingText, 0x00, sizeof(szPrintingText));
                        sprintf(szPrintingText, "TOTAL AMOUNT: %16s", szTemp3);
                        put_env_int("#P3LEN", strlen(szPrintingText));
                        put_env("#P3TEXT", szPrintingText, strlen(szPrintingText));
                        
			memset (baTemp, 0x00, sizeof(baTemp));
			//CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
			//CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
                        
                        if(srTransRec.fIsInstallment == TRUE){
							vdDebug_LogPrintf("1_ushCTOS_PrintBodyInstallment");
                            ushCTOS_PrintBodyInstallment();
                        } 
						else if(srTransRec.byTransType == BALANCE_ENQUIRY)
						{
							
						}
						else
						{
//                            vdCTOS_SetFontType(d_FONT_TAHOMABOLD_TTF);
                            vdCTOS_SetFontType(d_FONT_COURBD_TTF);
                            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
                            inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
                            vdCTOS_SetFontType(d_FONT_DEFAULT_TTF);
                        }
		}

		if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			//use ttf print
	    vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		
	}
	return d_OK;	
	
}




USHORT ushCTOS_PrintFooterCBPay(int page)
{		
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 


	if(srTransRec.byTransType == VOID)
	    {   
	        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d]",srTransRec.HDTid, srTransRec.MITid);
			if ( inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid) != d_OK)
			        {
			            vdSetErrorMessage("LOAD MMT ERR");
			            return(d_NO);
			        }
	        vdDebug_LogPrintf("srTransRec.HDTid[%d] srTransRec.MITid[%d] strMMT[0].szRctFoot1[%s]",srTransRec.HDTid, srTransRec.MITid, strMMT[0].szRctFoot1);
	    }


	
	if(page == d_FIRST_PAGE)
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
		    EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			// patrick add code 20141208
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
            {   
                //CTOS_PrinterFline(d_LINE_DOT * 1);
                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,1);
				if (srTransRec.byEntryMode==CARD_ENTRY_ICC)
				{
                	//CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
					//inCTOSS_CapturePrinterBuffer("     (PIN VERIFY SUCCESS)",&stgFONT_ATTRIB);
					inCCTOS_PrinterBufferOutput("     (PIN VERIFY SUCCESS)",&stgFONT_ATTRIB,1);
				}
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
			//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        }

		/*Aplipay no need E signature*/
		//ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//inCTOSS_CapturePrinterBuffer("SIGN:___________________________",&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);
		
		printCardHolderName();
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);


		//http://118.201.48.210:8080/redmine/issues/2070.102
//		ushCTOS_PrintAgreementCBPay();

		//http://118.201.48.210:8080/redmine/issues/1525.34 and http://118.201.48.210:8080/redmine/issues/1525.50 
		#if 0
		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		if(strlen(strMMT[0].szRctFoot2) > 0)
	    		vdPrintCenter(strMMT[0].szRctFoot2);
		if(strlen(strMMT[0].szRctFoot3) > 0)
	    		vdPrintCenter(strMMT[0].szRctFoot3);
		#else
		if(strlen(strMMT[0].szRctFoot1) > 0)
			vdPrintCenter(strMMT[0].szRctFoot1);
		
		#endif



    	if(fRePrintFlag == TRUE)
    	{
	    	if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
				inCCTOS_PrinterBufferOutputAligned("* * 重 印 * *", &stgFONT_ATTRIB, d_PRINTER_ALIGNCENTER, 1, 0);
			else
				vdPrintCenter("DUPLICATE");
    	}
			

		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** MERCHANT COPY *****  ");
		//inCTOSS_CapturePrinterBuffer("\n",&stgFONT_ATTRIB);
		//inCTOSS_CapturePrinterBuffer("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB);
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		//if (strTCT.inFontFNTMode == 1)
                if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
                {
                        inCCTOS_PrinterBufferOutput("     商戶存根 請妥善保管  ",&stgFONT_ATTRIB,1);
                }
                else
			inCCTOS_PrinterBufferOutput("   ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
		//else
		//inCCTOS_PrinterBufferOutput("      ***** MERCHANT COPY *****  ",&stgFONT_ATTRIB,1);
		
	}
	else
	{
		memset(EMVtagVal, 0x00, sizeof(EMVtagVal));
		EMVtagLen = 0;
		
		DebugAddINT("ushCTOS_PrintFooter,mode",srTransRec.byEntryMode);  
		// patrick fix signature line 20140823
		if ((srTransRec.byEntryMode==CARD_ENTRY_ICC)||(srTransRec.byEntryMode==CARD_ENTRY_WAVE))
		{
			EMVtagLen = 3;
            memcpy(EMVtagVal, srTransRec.stEMVinfo.T9F34, EMVtagLen);
			// patrick add code 20141208
			if (((EMVtagVal[0] != 0x03) && (EMVtagVal[0] != 0x05) &&
				 (EMVtagVal[0] != 0x1E) &&
				 (EMVtagVal[0] != 0x5E)) || (EMVtagVal[0] == 0x3F))
            {         
                //CTOS_PrinterFline(d_LINE_DOT * 1);
                //CTOS_PrinterPutString("*****NO SIGNATURE REQUIRED*****");
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
				inCCTOS_PrinterBufferOutput("*****NO SIGNATURE REQUIRED*****",&stgFONT_ATTRIB,0);
				if (srTransRec.byEntryMode==CARD_ENTRY_ICC)
	                //CTOS_PrinterPutString("     (PIN VERIFY SUCCESS)");
					inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
            }
            else
            {
                //CTOS_PrinterFline(d_LINE_DOT * 3);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
				inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
            }
		}
        else
        {
            //CTOS_PrinterFline(d_LINE_DOT * 3);
			//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
			inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
        }

		/*Aplipay no need E signature*/
		//ushCTOS_ePadPrintSignature();
		
		//CTOS_PrinterPutString("SIGN:_______________________________________");
		//inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,0);
		printCardHolderName();
		//CTOS_PrinterFline(d_LINE_DOT * 1);
		//inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
		
		//ushCTOS_PrintAgreement();

       		if (strTCT.inFontFNTMode == 1)
			vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
		else
			vdSetGolbFontAttrib(d_FONT_9x18, NORMAL_SIZE, NORMAL_SIZE, 0, 0);


//http://118.201.48.210:8080/redmine/issues/2070.102
//			ushCTOS_PrintAgreementCBPay();
			
			
			
			//http://118.201.48.210:8080/redmine/issues/1525.34 and http://118.201.48.210:8080/redmine/issues/1525.50 
#if 0
			if(strlen(strMMT[0].szRctFoot1) > 0)
				vdPrintCenter(strMMT[0].szRctFoot1);
			if(strlen(strMMT[0].szRctFoot2) > 0)
					vdPrintCenter(strMMT[0].szRctFoot2);
			if(strlen(strMMT[0].szRctFoot3) > 0)
					vdPrintCenter(strMMT[0].szRctFoot3);
#else
			if(strlen(strMMT[0].szRctFoot1) > 0)
				vdPrintCenter(strMMT[0].szRctFoot1);
			
#endif




			vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);


    	if(fRePrintFlag == TRUE)
    	{
	    	if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
				inCCTOS_PrinterBufferOutputAligned("* * 重 印 * *", &stgFONT_ATTRIB, d_PRINTER_ALIGNCENTER, 1, 0);
			else
				vdPrintCenter("DUPLICATE");
    	}
		
		//CTOS_PrinterFline(d_LINE_DOT * 1); 
		//CTOS_PrinterPutString("   ***** CUSTOMER COPY *****  ");
		inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,0);
		//if (strTCT.inFontFNTMode == 1)
                if (strTCT.inThemesType == 2 || strTCT.inThemesType == 3)
                {
                        inCCTOS_PrinterBufferOutput("     客戶存根 請妥善保管  ",&stgFONT_ATTRIB,1);
                }
                else
			inCCTOS_PrinterBufferOutput("   ***** CUSTOMER COPY *****  ",&stgFONT_ATTRIB,0);
		//else
		//inCCTOS_PrinterBufferOutput("      ***** CUSTOMER COPY *****  ",&stgFONT_ATTRIB,0);
	}
	
	//CTOS_PrinterFline(d_LINE_DOT * 6);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();

return d_OK;;	
}



USHORT ushCTOS_PrintAgreementCBPay(void) 
{
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);


	vdPrintCenter("THANK YOU");
	vdPrintCenter("HAVE A NICE DAY");
	vdPrintCenter("NO REFUND");


	vdPrintCenter("  ");
	// for Case 1595, 1600, 1603 , 1605
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
}
	
USHORT printTIDMIDCBPay(void)
{
	char szStr[d_LINE_SIZE + 1];
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
	char szMid[16] = {0};
	
	memset(szStr, ' ', d_LINE_SIZE);
	sprintf(szStr, "TID: %s", srTransRec.szTID);
	memset (baTemp, 0x00, sizeof(baTemp));		 

	vdDebug_LogPrintf("print [%s]", szStr);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);

	memset(szStr, ' ', d_LINE_SIZE);
	strcpy(szMid, srTransRec.szMID);
	szMid[16] =  '\0';
	sprintf(szStr, "MID: %s", szMid);
	memset (baTemp, 0x00, sizeof(baTemp));		 

	vdDebug_LogPrintf("print [%s]", szStr);
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	
	return (result);

}

//footer for VOID transaction
USHORT ushCTOS_PrintFooterSignature(int page)
{		
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen;
    BYTE szPinBlock[64 + 1];
    char szStr[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    char szExchangeRate[d_LINE_SIZE + 1];

	
	int inResult=0; 
	BYTE byVEPSQPSPrnted = 0; // VISA Master  NO SIGNATURE REQUIRED , already printted?
	BYTE byPassSignLine = 0;

	vdDebug_LogPrintf("ushCTOS_PrintFooterSignature");

    if (strTCT.inFontFNTMode != 1)
    {
        //use ttf print
        inCTOS_SelectFont(d_FONT_TTF_MODE,d_FONT_24x24,0," ");
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    }

    if(strTCT.fSignatureFlag == 1)
    {
		if(fRePrintFlag == FALSE)
            ushCTOS_ePadPrintSignature();
		else
		{
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
            inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
		}
    }
	else
	{
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
        inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	}

    inCCTOS_PrinterBufferOutput("SIGN:_______________________________________",&stgFONT_ATTRIB,1);				
    printCardHolderName();
		
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	
	ushCTOS_PrintAgreement();

	if(strlen(strMMT[0].szRctFoot1) > 0)
		vdPrintCenter(strMMT[0].szRctFoot1);
	if(strlen(strMMT[0].szRctFoot2) > 0)
		vdPrintCenter(strMMT[0].szRctFoot2);
	if(strlen(strMMT[0].szRctFoot3) > 0)
		vdPrintCenter(strMMT[0].szRctFoot3);

    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	
    if(page == d_FIRST_PAGE)
    {
        if (strTCT.inFontFNTMode == 1)
            inCCTOS_PrinterBufferOutput("    ***** MERCHANT COPY *****    ",&stgFONT_ATTRIB,1);
        else
            inCCTOS_PrinterBufferOutput("    ***** MERCHANT COPY *****    ",&stgFONT_ATTRIB,1);
    }
    else
    {
        if (strTCT.inFontFNTMode == 1)
            inCCTOS_PrinterBufferOutput("    ***** CUSTOMER COPY *****    ",&stgFONT_ATTRIB,1);
        else
            inCCTOS_PrinterBufferOutput("    ***** CUSTOMER COPY *****    ",&stgFONT_ATTRIB,1);
    }

	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	vdCTOSS_PrinterEnd();
	
    return d_OK;
}
// for Case 1595, 1600, 1603 , 1605
USHORT ushCTOS_PrintBodyCBPay2(int page)
{	
    char szStr[d_LINE_SIZE*2 + 3];
    char szTemp[d_LINE_SIZE + 1];
    char szTemp1[d_LINE_SIZE + 1];
    char szTemp3[d_LINE_SIZE + 1];
    char szTemp4[d_LINE_SIZE + 1];
    char szTemp5[d_LINE_SIZE + 1];
    char szFormatPAN[strlen(srTransRec.szPAN) + 7];//to account for space as in PAN format and to include additional PAN no. 7 has been added -Meena 08/01/2012
    int inFmtPANSize;
	char szTmpPan[d_LINE_SIZE + 1];
    BYTE baTemp[PAPER_X_SIZE * 64];
    USHORT result;
    int num,i,inResult;
    unsigned char tucPrint [24*4+1];	
    BYTE   EMVtagVal[64];
    USHORT EMVtagLen; 
    short spacestring;
    BYTE   key;
    char szTempAmt[AMT_ASC_SIZE+1];
    BYTE szPrintingText[40 + 1];

    inCBPayRead(1);
    inCSTRead(1); //MMK is always the currency for CBPAY
    
    vdDebug_LogPrintf("ushCTOS_PrintBodyCBPay START ");
    
    memset(szTemp1, ' ', d_LINE_SIZE);
    sprintf(szTemp1,"%s",srTransRec.szHostLabel);  
    vdPrintCenter(szTemp1);
    
    printTIDMIDCBPay(); 
    
    sprintf(szStr, "SUB MID: %s", strCBPay.szSubMerId);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    printDateTime();
       
    inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
    
    memset(szStr, ' ', d_LINE_SIZE);
    if(srTransRec.byTransType == CB_PAY_TRANS)
        strcpy(szStr, "CBPay SALES");   
    else if(srTransRec.byOrgTransType == CB_PAY_TRANS && srTransRec.byTransType == VOID)
        strcpy(szStr, "VOID CBPay SALES");
    
    vdCTOS_ModifyTransTitleString(szStr);
    
    vdPrintTitleCenter(szStr);
    
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        //use ttf print
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
    //reference number
    memset(szStr, ' ', d_LINE_SIZE);
    memset (baTemp, 0x00, sizeof(baTemp));					
    memset(szStr, ' ', d_LINE_SIZE);
    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "REF NO  : %s", srTransRec.szRefNo);
    else
    //use ttf print
        sprintf(szStr, "REF NO      :  %s", srTransRec.szRefNo);
	
    memset (baTemp, 0x00, sizeof(baTemp));		
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    //print  trace num
    memset(szStr, ' ', d_LINE_SIZE);
    memset (baTemp, 0x00, sizeof(baTemp));					
    memset(szStr, ' ', d_LINE_SIZE);	
    wub_hex_2_str(srTransRec.szInvoiceNo, baTemp, INVOICE_BCD_SIZE);

	// for case http://118.201.48.210:8080/redmine/issues/1525.34.1 	   
	//sprintf(szStr, "TRACE NO. : %s", baTemp);
    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "INV NO. : %s", baTemp);
    else
        sprintf(szStr, "INV NO.   :  %s", baTemp);
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    
    //Auth response code
    memset(szStr, ' ', d_LINE_SIZE);
    memset (baTemp, 0x00, sizeof(baTemp));					
    memset(szStr, ' ', d_LINE_SIZE);	
    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "TRANX ID : %s", srTransRec.szBankTransId);
    else
        sprintf(szStr, "TRANX ID    :  %s", srTransRec.szBankTransId);
    
    memset (baTemp, 0x00, sizeof(baTemp));		
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    
    //print CB PAY ACC  -- The value for CB PAY is  intentionally blank.
    memset(szStr, ' ', d_LINE_SIZE);
    if (strTCT.inFontFNTMode == 1)
        sprintf(szStr, "CBPay A/C  : ");
    else
        sprintf(szStr, "CBPay A/C   : ");
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
    
    memset(szStr, ' ', d_LINE_SIZE);
    memset(szTemp, ' ', d_LINE_SIZE);
    memset(szTemp1, ' ', d_LINE_SIZE);
    memset(szTemp3, ' ', d_LINE_SIZE);
    
    wub_hex_2_str(srTransRec.szTotalAmount, szTemp, AMT_BCD_SIZE);
    //wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);

    memset(szTempAmt, 0x00, sizeof(szTempAmt));
    sprintf(szTempAmt, "%012.0f", atof(szTemp)/100);
    memset(szTemp, 0x00, sizeof(szTemp));
    strcpy(szTemp, szTempAmt);
					
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);
    else//use ttf print
        vdSetGolbFontAttrib(d_FONT_16x16, NORMAL_SIZE, DOUBLE_SIZE, 0, 0);

	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	
    if(strcmp(strCST.szCurSymbol, "MMK") == 0)
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn", szTemp, szTemp1);
    else 
        vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp, szTemp1);
    
    if(srTransRec.byTransType == VOID)
        sprintf(szStr, "AMOUNT: -%s %s", strCST.szCurSymbol, szTemp1); // (2) Please need to fix adjust space between currency and amount at CBpay Receipt.  03-31-2020
    else
        sprintf(szStr, "AMOUNT: %s %s", strCST.szCurSymbol, szTemp1); // (2) Please need to fix adjust space between currency and amount at CBpay Receipt. 03-31-2020
    
    inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);	

	inCCTOS_PrinterBufferOutput(ONE_LINE_DOT,&stgFONT_ATTRIB,1);
	
    if (strTCT.inFontFNTMode == 1)
        vdSetGolbFontAttrib(d_FONT_24x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    else
        //use ttf print
        vdSetGolbFontAttrib(d_FONT_12x24, NORMAL_SIZE, NORMAL_SIZE, 0, 0);
    
    return d_OK;	
}



int inPrint(unsigned char *strPrint) 
{
	char szStr[500 + 1] = {0}; 
	BYTE baTemp[PAPER_X_SIZE * 64] = {0};

	vdDebug_LogPrintf("inPrint | strPrint[%s]", strPrint);
	
	printCheckPaper();
	
	memset(szStr, 0x00, sizeof(szStr));
	memcpy(szStr, strPrint, strlen(strPrint));

	vdDebug_LogPrintf("inPrint | szStr[%s]", szStr);

	#if 0
	memset (baTemp, 0x00, sizeof(baTemp));
	CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
	return CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);  
	#else
	inCCTOS_PrinterBufferOutput(szStr,&stgFONT_ATTRIB,1);
	#endif
	
	return(d_OK);
}

int inPrintLeftRight2(unsigned char *strLeft, unsigned char *strRight, int inMode) 
{
	char szStr[48+1]; 
    int inLength=0, inSize=0;
    BYTE baTemp[PAPER_X_SIZE * 64];

	vdDebug_LogPrintf("inPrintLeftRight2 | strLeft[%s] strRight[%s]", strLeft, strRight);
	
	//if(inMode == LENGTH_24)
        //inLength=LENGTH_24;
	//else if(inMode == LENGTH_32)
		//inLength=LENGTH_32;
	//else
	    //inLength=LENGTH_41;
	inLength=inMode;
	
	memset(szStr, 0x20, sizeof(szStr));
	inSize=strlen(strRight);

	vdDebug_LogPrintf("inPrintLeftRight2 | inSize.strRight[%d]", inSize);
	
    memcpy(&szStr[inLength-inSize], strRight, inSize);
	inSize=strlen(strLeft);

	vdDebug_LogPrintf("inPrintLeftRight2 | inSize.strLeft[%d]", inSize);
	
    memcpy(szStr, strLeft, strlen(strLeft));

	#if 0
    memset (baTemp, 0x00, sizeof(baTemp));
    CTOS_PrinterBufferPutString((BYTE *)baTemp, 1, 1, szStr, &stgFONT_ATTRIB);
    return CTOS_PrinterBufferOutput((BYTE *)baTemp, 3);
	#else
	inPrint(szStr);
	return(d_OK);
	#endif
}

