#////////////////////////////////////////////////////////////////////////////////
#//              ____      _      _____   _   _          _                     //
#//             / ___|    / \    | ____| | \ | |   ___  | |  ___               //
#//            | |       / _ \   |  _|   |  \| |  / _ \ | | / __|              //
#//            | |___   / ___ \  | |___  | |\  | |  __/ | | \__ \              //
#//             \____| /_/   \_\ |_____| |_| \_|  \___| |_| |___/              //
#//                                                                            //
#////////////////////////////////////////////////////////////////////////////////
TOP=..
include $(TOP)/configure/CONFIG
#=======================================

# Build devEasyDriver as a library for an IOC:
LIBRARY_IOC += devEasyDriver

# Library Source files
devEasyDriver_SRCS += devEasyDriver.c

# Link with the asyn and base libraries
devEasyDriver_LIBS += asyn
devEasyDriver_LIBS += $(EPICS_BASE_IOC_LIBS)

# Install .dbd and .db files
DBD += devEasyDriver.dbd
#DB_INSTALLS += devEasyDriver.db
DB += devEasyDriver.db

#=======================================
include $(TOP)/configure/RULES
