#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=arm-brcm-linux-gnueabi-gcc
CCC=arm-brcm-linux-gnueabi-g++
CXX=arm-brcm-linux-gnueabi-g++
FC=g77.exe
AS=as

# Macros
CND_PLATFORM=Gnueabi-Windows
CND_DLIB_EXT=dll
CND_CONF=Device
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include NbMakefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/FeliCa/FeliCa.o \
	${OBJECTDIR}/DCC/PosDCC.o \
	${OBJECTDIR}/Main/POSMain.o \
	${OBJECTDIR}/QR_Payment/POSQR_Payment.o \
	${OBJECTDIR}/Trans/POSSale.o \
	${OBJECTDIR}/Ftps/ftpsget.o \
	${OBJECTDIR}/DCC/ShareDCCFile.o \
	${OBJECTDIR}/PCI100/COMMS.o \
	${OBJECTDIR}/Database/pas.o \
	${OBJECTDIR}/mifare/mifare.o \
	${OBJECTDIR}/Database/dct.o \
	${OBJECTDIR}/Trans/POSMPUSignOn.o \
	${OBJECTDIR}/Aptrans/MultiShareECR.o \
	${OBJECTDIR}/Ctls/POSWave.o \
	${OBJECTDIR}/Trans/POSMPUSale.o \
	${OBJECTDIR}/Ctls/POSCtls.o \
	${OBJECTDIR}/Trans/POSMPUVoid.o \
	${OBJECTDIR}/Trans/POSBalanceEnquiry.o \
	${OBJECTDIR}/Trans/POSAlipay.o \
	${OBJECTDIR}/Trans/POSCUPPreAuth.o \
	${OBJECTDIR}/POWRFAIL/POSPOWRFAIL.o \
	${OBJECTDIR}/Trans/POSOffline.o \
	${OBJECTDIR}/Md5/CRCmd5.o \
	${OBJECTDIR}/UTILS/wub_lib.o \
	${OBJECTDIR}/Des/dessrc.o \
	${OBJECTDIR}/Trans/POSTipAdjust.o \
	${OBJECTDIR}/Iso8583/V5IsoFunc.o \
	${OBJECTDIR}/External/ExtCard.o \
	${OBJECTDIR}/Database/DatabaseFunc.o \
	${OBJECTDIR}/UTILS/myEZLib.o \
	${OBJECTDIR}/UI/Display.o \
	${OBJECTDIR}/Trans/POSCUPSale.o \
	${OBJECTDIR}/Trans/POSVoid.o \
	${OBJECTDIR}/ACCUM/accum.o \
	${OBJECTDIR}/Trans/POSMPUSettle.o \
	${OBJECTDIR}/TMS/TMS.o \
	${OBJECTDIR}/Ftps/ftpget.o \
	${OBJECTDIR}/Aptrans/MultiAptrans.o \
	${OBJECTDIR}/Trans/POSRefund.o \
	${OBJECTDIR}/Aptrans/MultiShareEMV.o \
	${OBJECTDIR}/Trans/POSCUPSaleAdjust.o \
	${OBJECTDIR}/External/ExtSignature.o \
	${OBJECTDIR}/FeliCa/Reader_PC.o \
	${OBJECTDIR}/Trans/CardUtil.o \
	${OBJECTDIR}/batch/POSbatch.o \
	${OBJECTDIR}/Trans/POSCUPLogon.o \
	${OBJECTDIR}/print/Print.o \
	${OBJECTDIR}/PCI100/PCI100.o \
	${OBJECTDIR}/Functionslist/POSFunctionsList.o \
	${OBJECTDIR}/Trans/POSTrans.o \
	${OBJECTDIR}/Htle/htlesrc.o \
	${OBJECTDIR}/PinPad/pinpad.o \
	${OBJECTDIR}/External/ExtCtls.o \
	${OBJECTDIR}/Trans/POSHost.o \
	${OBJECTDIR}/Aptrans/MultiShareCOM.o \
	${OBJECTDIR}/Trans/POSTopupReload.o \
	${OBJECTDIR}/FeliCa/FeliCa_Trans.o \
	${OBJECTDIR}/PCI100/PCI100des.o \
	${OBJECTDIR}/Trans/POSSettlement.o \
	${OBJECTDIR}/Trans/POSMPUPreAuth.o \
	${OBJECTDIR}/SmartCard/MPUCard.o \
	${OBJECTDIR}/Ftps/ftpput.o \
	${OBJECTDIR}/Erm/PosErm.o \
	${OBJECTDIR}/External/ExtOnlinePin.o \
	${OBJECTDIR}/Sam/sam.o \
	${OBJECTDIR}/FileModule/myFileFunc.o \
	${OBJECTDIR}/Comm/V5Comm.o \
	${OBJECTDIR}/Trans/POSAuth.o \
	${OBJECTDIR}/Setting/POSSetting.o \
	${OBJECTDIR}/UI/showbmp.o \
	${OBJECTDIR}/Des/rsasrc.o \
	${OBJECTDIR}/TMSDYNMenu/TMSDYNMenu.o \
	${OBJECTDIR}/Ftps/ftpsput.o \
	${OBJECTDIR}/Trans/POSIpp.o \
	${OBJECTDIR}/PCI100/USBComms.o \
	${OBJECTDIR}/DEBUG/debug.o


# C Compiler Flags
CFLAGS="-I${SDKV5SINC}" -fsigned-char -Wundef -Wstrict-prototypes -Wno-trigraphs -Wimplicit -Wformat 

# CC Compiler Flags
CCFLAGS="-I${SDKV5SINC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 
CXXFLAGS="-I${SDKV5SINC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lcakms -lcalcd -lcamodem -lcapmodem -lcaprt -lcartc -lcauart -lcauldpm -lcausbh -lcagsm -lcabarcode -lpthread -ldl -lcaclvw -lcatls -lctosapi -lz -lssl -lcrypto -lcurl -lfreetype -lxml2 -lcaethernet -lv5smultiap -lv5sinput -lv5scfgexpress -lv5sISOEnginee -lv5sEFT -lcasqlite -lcaxml ../VEGA_Library/NETBEANS/DMenu/Ver0005/Lib/V5S_LibDMenu.a -lv3_libepadso -lcaqrcode -lv5sEFTNAC -lbluetooth "DESFire.o" "aes.o" -lbmp -lcaclentry -lcaclmdl -lcafont -lcafs -lv5spbm  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/V5S/${PRONAME}/10V5S_App/${APPNAME}.exe

dist/V5S/${PRONAME}/10V5S_App/${APPNAME}.exe: ../VEGA_Library/NETBEANS/DMenu/Ver0005/Lib/V5S_LibDMenu.a

dist/V5S/${PRONAME}/10V5S_App/${APPNAME}.exe: ${OBJECTFILES}
	${MKDIR} -p dist/V5S/${PRONAME}/10V5S_App
	arm-brcm-linux-gnueabi-g++ -L . "-L${SDKV5SLIB}" "-L${SDKV5SLIBN}" -o dist/V5S/${PRONAME}/10V5S_App/${APPNAME}  ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/FeliCa/FeliCa.o: FeliCa/FeliCa.c 
	${MKDIR} -p ${OBJECTDIR}/FeliCa
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/FeliCa/FeliCa.o FeliCa/FeliCa.c

${OBJECTDIR}/DCC/PosDCC.o: DCC/PosDCC.c 
	${MKDIR} -p ${OBJECTDIR}/DCC
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/DCC/PosDCC.o DCC/PosDCC.c

${OBJECTDIR}/Main/POSMain.o: Main/POSMain.c 
	${MKDIR} -p ${OBJECTDIR}/Main
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Main/POSMain.o Main/POSMain.c

${OBJECTDIR}/QR_Payment/POSQR_Payment.o: QR_Payment/POSQR_Payment.c 
	${MKDIR} -p ${OBJECTDIR}/QR_Payment
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/QR_Payment/POSQR_Payment.o QR_Payment/POSQR_Payment.c

${OBJECTDIR}/Trans/POSSale.o: Trans/POSSale.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSSale.o Trans/POSSale.c

${OBJECTDIR}/Ftps/ftpsget.o: Ftps/ftpsget.c 
	${MKDIR} -p ${OBJECTDIR}/Ftps
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Ftps/ftpsget.o Ftps/ftpsget.c

${OBJECTDIR}/DCC/ShareDCCFile.o: DCC/ShareDCCFile.c 
	${MKDIR} -p ${OBJECTDIR}/DCC
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/DCC/ShareDCCFile.o DCC/ShareDCCFile.c

${OBJECTDIR}/PCI100/COMMS.o: PCI100/COMMS.c 
	${MKDIR} -p ${OBJECTDIR}/PCI100
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/PCI100/COMMS.o PCI100/COMMS.c

${OBJECTDIR}/Database/pas.o: Database/pas.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Database/pas.o Database/pas.c

${OBJECTDIR}/mifare/mifare.o: mifare/mifare.c 
	${MKDIR} -p ${OBJECTDIR}/mifare
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/mifare/mifare.o mifare/mifare.c

${OBJECTDIR}/Database/dct.o: Database/dct.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Database/dct.o Database/dct.c

${OBJECTDIR}/Trans/POSMPUSignOn.o: Trans/POSMPUSignOn.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSMPUSignOn.o Trans/POSMPUSignOn.c

${OBJECTDIR}/Aptrans/MultiShareECR.o: Aptrans/MultiShareECR.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Aptrans/MultiShareECR.o Aptrans/MultiShareECR.c

${OBJECTDIR}/Ctls/POSWave.o: Ctls/POSWave.c 
	${MKDIR} -p ${OBJECTDIR}/Ctls
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Ctls/POSWave.o Ctls/POSWave.c

${OBJECTDIR}/Trans/POSMPUSale.o: Trans/POSMPUSale.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSMPUSale.o Trans/POSMPUSale.c

${OBJECTDIR}/Ctls/POSCtls.o: Ctls/POSCtls.c 
	${MKDIR} -p ${OBJECTDIR}/Ctls
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Ctls/POSCtls.o Ctls/POSCtls.c

${OBJECTDIR}/Trans/POSMPUVoid.o: Trans/POSMPUVoid.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSMPUVoid.o Trans/POSMPUVoid.c

${OBJECTDIR}/Trans/POSBalanceEnquiry.o: Trans/POSBalanceEnquiry.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSBalanceEnquiry.o Trans/POSBalanceEnquiry.c

${OBJECTDIR}/Trans/POSAlipay.o: Trans/POSAlipay.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSAlipay.o Trans/POSAlipay.c

${OBJECTDIR}/Trans/POSCUPPreAuth.o: Trans/POSCUPPreAuth.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSCUPPreAuth.o Trans/POSCUPPreAuth.c

${OBJECTDIR}/POWRFAIL/POSPOWRFAIL.o: POWRFAIL/POSPOWRFAIL.c 
	${MKDIR} -p ${OBJECTDIR}/POWRFAIL
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/POWRFAIL/POSPOWRFAIL.o POWRFAIL/POSPOWRFAIL.c

${OBJECTDIR}/Trans/POSOffline.o: Trans/POSOffline.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSOffline.o Trans/POSOffline.c

${OBJECTDIR}/Md5/CRCmd5.o: Md5/CRCmd5.cpp 
	${MKDIR} -p ${OBJECTDIR}/Md5
	$(COMPILE.cc) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Md5/CRCmd5.o Md5/CRCmd5.cpp

${OBJECTDIR}/UTILS/wub_lib.o: UTILS/wub_lib.c 
	${MKDIR} -p ${OBJECTDIR}/UTILS
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/UTILS/wub_lib.o UTILS/wub_lib.c

${OBJECTDIR}/Des/dessrc.o: Des/dessrc.c 
	${MKDIR} -p ${OBJECTDIR}/Des
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Des/dessrc.o Des/dessrc.c

${OBJECTDIR}/Trans/POSTipAdjust.o: Trans/POSTipAdjust.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSTipAdjust.o Trans/POSTipAdjust.c

${OBJECTDIR}/Iso8583/V5IsoFunc.o: Iso8583/V5IsoFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Iso8583
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Iso8583/V5IsoFunc.o Iso8583/V5IsoFunc.c

${OBJECTDIR}/External/ExtCard.o: External/ExtCard.c 
	${MKDIR} -p ${OBJECTDIR}/External
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/External/ExtCard.o External/ExtCard.c

${OBJECTDIR}/Database/DatabaseFunc.o: Database/DatabaseFunc.c 
	${MKDIR} -p ${OBJECTDIR}/Database
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Database/DatabaseFunc.o Database/DatabaseFunc.c

${OBJECTDIR}/UTILS/myEZLib.o: UTILS/myEZLib.c 
	${MKDIR} -p ${OBJECTDIR}/UTILS
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/UTILS/myEZLib.o UTILS/myEZLib.c

${OBJECTDIR}/UI/Display.o: UI/Display.c 
	${MKDIR} -p ${OBJECTDIR}/UI
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/UI/Display.o UI/Display.c

${OBJECTDIR}/Trans/POSCUPSale.o: Trans/POSCUPSale.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSCUPSale.o Trans/POSCUPSale.c

${OBJECTDIR}/Trans/POSVoid.o: Trans/POSVoid.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSVoid.o Trans/POSVoid.c

${OBJECTDIR}/ACCUM/accum.o: ACCUM/accum.c 
	${MKDIR} -p ${OBJECTDIR}/ACCUM
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/ACCUM/accum.o ACCUM/accum.c

${OBJECTDIR}/Trans/POSMPUSettle.o: Trans/POSMPUSettle.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSMPUSettle.o Trans/POSMPUSettle.c

${OBJECTDIR}/TMS/TMS.o: TMS/TMS.c 
	${MKDIR} -p ${OBJECTDIR}/TMS
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/TMS/TMS.o TMS/TMS.c

${OBJECTDIR}/Ftps/ftpget.o: Ftps/ftpget.c 
	${MKDIR} -p ${OBJECTDIR}/Ftps
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Ftps/ftpget.o Ftps/ftpget.c

${OBJECTDIR}/Aptrans/MultiAptrans.o: Aptrans/MultiAptrans.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Aptrans/MultiAptrans.o Aptrans/MultiAptrans.c

${OBJECTDIR}/Trans/POSRefund.o: Trans/POSRefund.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSRefund.o Trans/POSRefund.c

${OBJECTDIR}/Aptrans/MultiShareEMV.o: Aptrans/MultiShareEMV.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Aptrans/MultiShareEMV.o Aptrans/MultiShareEMV.c

${OBJECTDIR}/Trans/POSCUPSaleAdjust.o: Trans/POSCUPSaleAdjust.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSCUPSaleAdjust.o Trans/POSCUPSaleAdjust.c

${OBJECTDIR}/External/ExtSignature.o: External/ExtSignature.c 
	${MKDIR} -p ${OBJECTDIR}/External
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/External/ExtSignature.o External/ExtSignature.c

${OBJECTDIR}/FeliCa/Reader_PC.o: FeliCa/Reader_PC.c 
	${MKDIR} -p ${OBJECTDIR}/FeliCa
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/FeliCa/Reader_PC.o FeliCa/Reader_PC.c

${OBJECTDIR}/Trans/CardUtil.o: Trans/CardUtil.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/CardUtil.o Trans/CardUtil.c

${OBJECTDIR}/batch/POSbatch.o: batch/POSbatch.c 
	${MKDIR} -p ${OBJECTDIR}/batch
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/batch/POSbatch.o batch/POSbatch.c

${OBJECTDIR}/Trans/POSCUPLogon.o: Trans/POSCUPLogon.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSCUPLogon.o Trans/POSCUPLogon.c

${OBJECTDIR}/print/Print.o: print/Print.c 
	${MKDIR} -p ${OBJECTDIR}/print
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/print/Print.o print/Print.c

${OBJECTDIR}/PCI100/PCI100.o: PCI100/PCI100.c 
	${MKDIR} -p ${OBJECTDIR}/PCI100
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/PCI100/PCI100.o PCI100/PCI100.c

${OBJECTDIR}/Functionslist/POSFunctionsList.o: Functionslist/POSFunctionsList.c 
	${MKDIR} -p ${OBJECTDIR}/Functionslist
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Functionslist/POSFunctionsList.o Functionslist/POSFunctionsList.c

${OBJECTDIR}/Trans/POSTrans.o: Trans/POSTrans.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSTrans.o Trans/POSTrans.c

${OBJECTDIR}/Htle/htlesrc.o: Htle/htlesrc.c 
	${MKDIR} -p ${OBJECTDIR}/Htle
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Htle/htlesrc.o Htle/htlesrc.c

${OBJECTDIR}/PinPad/pinpad.o: PinPad/pinpad.c 
	${MKDIR} -p ${OBJECTDIR}/PinPad
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/PinPad/pinpad.o PinPad/pinpad.c

${OBJECTDIR}/External/ExtCtls.o: External/ExtCtls.c 
	${MKDIR} -p ${OBJECTDIR}/External
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/External/ExtCtls.o External/ExtCtls.c

${OBJECTDIR}/Trans/POSHost.o: Trans/POSHost.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSHost.o Trans/POSHost.c

${OBJECTDIR}/Aptrans/MultiShareCOM.o: Aptrans/MultiShareCOM.c 
	${MKDIR} -p ${OBJECTDIR}/Aptrans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Aptrans/MultiShareCOM.o Aptrans/MultiShareCOM.c

${OBJECTDIR}/Trans/POSTopupReload.o: Trans/POSTopupReload.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSTopupReload.o Trans/POSTopupReload.c

${OBJECTDIR}/FeliCa/FeliCa_Trans.o: FeliCa/FeliCa_Trans.c 
	${MKDIR} -p ${OBJECTDIR}/FeliCa
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/FeliCa/FeliCa_Trans.o FeliCa/FeliCa_Trans.c

${OBJECTDIR}/PCI100/PCI100des.o: PCI100/PCI100des.c 
	${MKDIR} -p ${OBJECTDIR}/PCI100
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/PCI100/PCI100des.o PCI100/PCI100des.c

${OBJECTDIR}/Trans/POSSettlement.o: Trans/POSSettlement.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSSettlement.o Trans/POSSettlement.c

${OBJECTDIR}/Trans/POSMPUPreAuth.o: Trans/POSMPUPreAuth.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSMPUPreAuth.o Trans/POSMPUPreAuth.c

${OBJECTDIR}/SmartCard/MPUCard.o: SmartCard/MPUCard.c 
	${MKDIR} -p ${OBJECTDIR}/SmartCard
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/SmartCard/MPUCard.o SmartCard/MPUCard.c

${OBJECTDIR}/Ftps/ftpput.o: Ftps/ftpput.c 
	${MKDIR} -p ${OBJECTDIR}/Ftps
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Ftps/ftpput.o Ftps/ftpput.c

${OBJECTDIR}/Erm/PosErm.o: Erm/PosErm.c 
	${MKDIR} -p ${OBJECTDIR}/Erm
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Erm/PosErm.o Erm/PosErm.c

${OBJECTDIR}/External/ExtOnlinePin.o: External/ExtOnlinePin.c 
	${MKDIR} -p ${OBJECTDIR}/External
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/External/ExtOnlinePin.o External/ExtOnlinePin.c

${OBJECTDIR}/Sam/sam.o: Sam/sam.c 
	${MKDIR} -p ${OBJECTDIR}/Sam
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Sam/sam.o Sam/sam.c

${OBJECTDIR}/FileModule/myFileFunc.o: FileModule/myFileFunc.c 
	${MKDIR} -p ${OBJECTDIR}/FileModule
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/FileModule/myFileFunc.o FileModule/myFileFunc.c

${OBJECTDIR}/Comm/V5Comm.o: Comm/V5Comm.c 
	${MKDIR} -p ${OBJECTDIR}/Comm
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Comm/V5Comm.o Comm/V5Comm.c

${OBJECTDIR}/Trans/POSAuth.o: Trans/POSAuth.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSAuth.o Trans/POSAuth.c

${OBJECTDIR}/Setting/POSSetting.o: Setting/POSSetting.c 
	${MKDIR} -p ${OBJECTDIR}/Setting
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Setting/POSSetting.o Setting/POSSetting.c

${OBJECTDIR}/UI/showbmp.o: UI/showbmp.c 
	${MKDIR} -p ${OBJECTDIR}/UI
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/UI/showbmp.o UI/showbmp.c

${OBJECTDIR}/Des/rsasrc.o: Des/rsasrc.c 
	${MKDIR} -p ${OBJECTDIR}/Des
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Des/rsasrc.o Des/rsasrc.c

${OBJECTDIR}/TMSDYNMenu/TMSDYNMenu.o: TMSDYNMenu/TMSDYNMenu.c 
	${MKDIR} -p ${OBJECTDIR}/TMSDYNMenu
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/TMSDYNMenu/TMSDYNMenu.o TMSDYNMenu/TMSDYNMenu.c

${OBJECTDIR}/Ftps/ftpsput.o: Ftps/ftpsput.c 
	${MKDIR} -p ${OBJECTDIR}/Ftps
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Ftps/ftpsput.o Ftps/ftpsput.c

${OBJECTDIR}/Trans/POSIpp.o: Trans/POSIpp.c 
	${MKDIR} -p ${OBJECTDIR}/Trans
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/Trans/POSIpp.o Trans/POSIpp.c

${OBJECTDIR}/PCI100/USBComms.o: PCI100/USBComms.c 
	${MKDIR} -p ${OBJECTDIR}/PCI100
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/PCI100/USBComms.o PCI100/USBComms.c

${OBJECTDIR}/DEBUG/debug.o: DEBUG/debug.c 
	${MKDIR} -p ${OBJECTDIR}/DEBUG
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA5000S/include/curl -o ${OBJECTDIR}/DEBUG/debug.o DEBUG/debug.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} dist/V5S/${PRONAME}/10V5S_App/${APPNAME}.exe

# Subprojects
.clean-subprojects:
