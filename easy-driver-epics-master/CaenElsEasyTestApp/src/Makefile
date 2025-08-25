#////////////////////////////////////////////////////////////////////////////////
#//              ____      _      _____   _   _          _                     //
#//             / ___|    / \    | ____| | \ | |   ___  | |  ___               //
#//            | |       / _ \   |  _|   |  \| |  / _ \ | | / __|              //
#//            | |___   / ___ \  | |___  | |\  | |  __/ | | \__ \              //
#//             \____| /_/   \_\ |_____| |_| \_|  \___| |_| |___/              //
#//                                                                            //
#////////////////////////////////////////////////////////////////////////////////
TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#////////////////////////////////////////////////////////////////////////////////
#//              ____      _      _____   _   _          _                     //
#//             / ___|    / \    | ____| | \ | |   ___  | |  ___               //
#//            | |       / _ \   |  _|   |  \| |  / _ \ | | / __|              //
#//            | |___   / ___ \  | |___  | |\  | |  __/ | | \__ \              //
#//             \____| /_/   \_\ |_____| |_| \_|  \___| |_| |___/              //
#//                                                                            //
#////////////////////////////////////////////////////////////////////////////////
#=============================
# Build the IOC application

PROD_IOC = EasyDriverTest
# EasyDriverTest.dbd will be created and installed
DBD += EasyDriverTest.dbd

# EasyDriverTest.dbd will be made up from these files:
EasyDriverTest_DBD += base.dbd

# Include dbd files from all support applications:
EasyDriverTest_DBD += devEasyDriver.dbd

# Add all the support libraries needed by this IOC
EasyDriverTest_LIBS += devEasyDriver
EasyDriverTest_LIBS += asyn

# EasyDriverTest_registerRecordDeviceDriver.cpp derives from EasyDriverTest.dbd
EasyDriverTest_SRCS += EasyDriverTest_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
EasyDriverTest_SRCS_DEFAULT += EasyDriverTestMain.cpp
EasyDriverTest_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#EasyDriverTest_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
EasyDriverTest_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

