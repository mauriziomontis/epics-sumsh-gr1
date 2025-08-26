////////////////////////////////////////////////////////////////////////////////
//              ____      _      _____   _   _          _                     //
//             / ___|    / \    | ____| | \ | |   ___  | |  ___               //
//            | |       / _ \   |  _|   |  \| |  / _ \ | | / __|              //
//            | |___   / ___ \  | |___  | |\  | |  __/ | | \__ \              //
//             \____| /_/   \_\ |_____| |_| \_|  \___| |_| |___/              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2015 CAEN ELS d.o.o.
// This code is distributed subject to a Software License Agreement found
// in file LICENSE that is included with this distribution.
////////////////////////////////////////////////////////////////////////////////

#ifndef asynInterposeCom_H
#define asynInterposeCom_H

#include <shareLib.h>
#include <epicsExport.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

epicsShareFunc int devEasyDriverConfigure(const char *portName, const char *hostInfo, int flags, int priority);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* asynInterposeCom_H */
