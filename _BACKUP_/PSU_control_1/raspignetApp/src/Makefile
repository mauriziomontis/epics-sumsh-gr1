# SPDX-FileCopyrightText: 2003 Argonne National Laboratory
#
# SPDX-License-Identifier: EPICS

TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = raspignet
# raspignet.dbd will be created and installed
DBD += raspignet.dbd

# raspignet.dbd will be made up from these files:
raspignet_DBD += base.dbd

# Include dbd files from all support applications:
raspignet_DBD += stream-base.dbd
raspignet_DBD += asyn.dbd
raspignet_DBD += drvAsynIPPort.dbd

# Add all the support libraries needed by this IOC
raspignet_LIBS += stream
raspignet_LIBS += asyn

# raspignet_registerRecordDeviceDriver.cpp derives from raspignet.dbd
raspignet_SRCS += raspignet_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
raspignet_SRCS_DEFAULT += raspignetMain.cpp
raspignet_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#raspignet_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
raspignet_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

