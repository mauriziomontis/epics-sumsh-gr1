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

PROD_IOC = motorcontrol
# motorcontrol.dbd will be created and installed
DBD += motorcontrol.dbd

# motorcontrol.dbd will be made up from these files:
motorcontrol_DBD += base.dbd

# Include dbd files from all support applications:
motorcontrol_DBD += stream-base.dbd
motorcontrol_DBD += asyn.dbd
motorcontrol_DBD += drvAsynIPPort.dbd

# Add all the support libraries needed by this IOC
motorcontrol_LIBS += stream
motorcontrol_LIBS += asyn

# motorcontrol_registerRecordDeviceDriver.cpp derives from motorcontrol.dbd
motorcontrol_SRCS += motorcontrol_registerRecordDeviceDriver.cpp
motorcontrol_SRCS += motorcalibration.c

# Build the main IOC entry point on workstation OSs.
motorcontrol_SRCS_DEFAULT += motorcontrolMain.cpp
motorcontrol_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#motorcontrol_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
motorcontrol_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

