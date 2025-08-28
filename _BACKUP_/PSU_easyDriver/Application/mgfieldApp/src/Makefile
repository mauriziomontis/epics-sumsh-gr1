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

PROD_IOC = mgfield
# mgfield.dbd will be created and installed
DBD += mgfield.dbd

# mgfield.dbd will be made up from these files:
mgfield_DBD += base.dbd

# Include dbd files from all support applications:
#mgfield_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#mgfield_LIBS += xxx

# mgfield_registerRecordDeviceDriver.cpp derives from mgfield.dbd
mgfield_SRCS += mgfield_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
mgfield_SRCS_DEFAULT += mgfieldMain.cpp
mgfield_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#mgfield_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
mgfield_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

