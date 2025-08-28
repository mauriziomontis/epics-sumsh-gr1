#!../../bin/linux-x86_64/EasyDriverTest

###############################################################################
# Set up environment
epicsEnvSet("P","$(P=easyDriver:)")
epicsEnvSet("EASY_DRIVER_91","$(EASY_DRIVER_91=192.168.0.10:10001)")
< envPaths
cd "${TOP}"

###############################################################################
# Register all support components
dbLoadDatabase "dbd/EasyDriverTest.dbd"
EasyDriverTest_registerRecordDeviceDriver pdbbase

###############################################################################
# Set up ASYN ports
devEasyDriverConfigure("L1","$(EASY_DRIVER_91)",0x1)
asynSetTraceIOMask("L1_TCP",-1,0x2)
#asynSetTraceMask("L1_TCP",-1,0x9)

###############################################################################
# Load record instances
dbLoadRecords "db/asynRecord.db" "P=$(P),R=asyn,PORT=L1_TCP,ADDR=0,OMAX=0,IMAX=0"
dbLoadRecords "db/devEasyDriver.db" "P=$(P),R=91:,PORT=L1,RANGE=5,NELM=10000"

###############################################################################
# Start IOC
cd ${TOP}/iocBoot/${IOC}
iocInit
