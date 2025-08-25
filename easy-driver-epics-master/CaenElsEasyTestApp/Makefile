#////////////////////////////////////////////////////////////////////////////////
#//              ____      _      _____   _   _          _                     //
#//             / ___|    / \    | ____| | \ | |   ___  | |  ___               //
#//            | |       / _ \   |  _|   |  \| |  / _ \ | | / __|              //
#//            | |___   / ___ \  | |___  | |\  | |  __/ | | \__ \              //
#//             \____| /_/   \_\ |_____| |_| \_|  \___| |_| |___/              //
#//                                                                            //
#////////////////////////////////////////////////////////////////////////////////
TOP = ..
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *src*))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *Src*))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *db*))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *Db*))
include $(TOP)/configure/RULES_DIRS

