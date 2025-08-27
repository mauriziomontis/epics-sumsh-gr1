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

PROD_IOC = PSU_control_2
# PSU_control_2.dbd will be created and installed
DBD += PSU_control_2.dbd

# PSU_control_2.dbd will be made up from these files:
PSU_control_2_DBD += base.dbd

# Include dbd files from all support applications:
PSU_control_2_DBD += stream-base.dbd
PSU_control_2_DBD += asyn.dbd
PSU_control_2_DBD += drvAsynIPPort.dbd

# Add all the support libraries needed by this IOC
PSU_control_2_LIBS += stream
PSU_control_2_LIBS += asyn

# PSU_control_2_registerRecordDeviceDriver.cpp derives from PSU_control_2.dbd
PSU_control_2_SRCS += PSU_control_2_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
PSU_control_2_SRCS_DEFAULT += PSU_control_2Main.cpp
PSU_control_2_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#PSU_control_2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
PSU_control_2_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
USR_INCLUDES += -I$(ASYN)/include -I$(STREAM)/include
