////////////////////////////////////////////////////////////////////////////////
//              ____      _      _____   _   _          _                     //
//             / ___|    / \    | ____| | \ | |   ___  | |  ___               //
//            | |       / _ \   |  _|   |  \| |  / _ \ | | / __|              //
//            | |___   / ___ \  | |___  | |\  | |  __/ | | \__ \              //
//             \____| /_/   \_\ |_____| |_| \_|  \___| |_| |___/              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// Support for CAEN ELS Easy Driver Power Supplies
//
// Author: M. Gustin
// Date: Oct 15 2015
//
// Supported Devices: CAENels Easy Driver
// Note: this driver works also with the older CAENels SY2604 system
//
// This code was inspired by the driver produced by W. Eric Norum
// for the SY3634 system.
// 
// Configuration options:
// Add a 0x1 as the final argument to enable code
// that measures the time taken to process a transaction.
// 
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2015 CAEN ELS d.o.o.
// This code is distributed subject to a Software License Agreement found
// in file LICENSE that is included with this distribution.
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdio.h>

#include <cantProceed.h>
#include <epicsStdio.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <errlog.h>
#include <iocsh.h>

#include "asynDriver.h"
#include "asynOctetSyncIO.h"
#include "asynOctet.h"
#include "asynInt32.h"
#include "asynFloat64.h"
#include "asynFloat32Array.h"
#include "drvAsynIPPort.h"
#include "devEasyDriver.h"
#include "easyDriverPSinfo.h"
#include <epicsExport.h>

/*
 * Configuration command bits
 */
#define FLAG_DO_TIMING_TESTS            0x1

/*
 * asynFloat64 subaddresses
 */
#define A_SETPOINT_CURRENT          0
#define A_READBACK_CURRENT          1
#define A_Kp                        13
#define A_Ki                        14
#define A_Kd                        15
#define A_READ_BULK_VOLTAGE         40
#define A_READ_FET_TEMPERATURE      41
#define A_READ_SHUNT_TEMPERATURE    42
#define A_READ_OUTPUT_VOLTAGE       43
#define A_READ_GROUND_CURRENT       44

/*
 * asynInt32 subaddresses
 */
#define A_READ_SLEW_MODE            50
//#define A_WRITE_STOP_WAVEFORM       80
//#define A_WRITE_START_WAVEFORM      81
#define A_READ_FORCE_READBACK       99
#define A_WRITE_SUPPLY_ON           100
#define A_WRITE_RESET               101
#define A_WRITE_SLEW_MODE           102
//#define A_WRITE_BULK_ON             104

/*
 * asynFloat32Array subaddress
 */
#define A_WAVEFORM                  0

/*
 * Readback values
 */
typedef struct easyDriverReadback {
    int    status;
    double setpointCurrent;
    double rbCurrent;
} easyDriverReadback;

/*
 * Interposed layer private storage
 */
typedef struct easyDriverPvt {
    asynUser      *pasynUser;      /* To perform lower-interface I/O */

    asynInterface  asynCommon;     /* Our interfaces */
    asynInterface  asynOctet;
    asynInterface  asynInt32;
    void          *asynInt32InterruptPvt;
    asynInterface  asynFloat64;
    void          *asynFloat64InterruptPvt;
    asynInterface  asynFloat32Array;

    char           sendBuf[80];
    char           replyBuf[80];
    size_t         replyLen;

    easyDriverReadback  rb;             	/* Most recent readback values */
    int            		slewMode;       	/* Local variable for Ramp Flag */

    unsigned long  commandCount;    		/* Statistics */
    unsigned long  setpointUpdateCount;
    unsigned long  retryCount;
    unsigned long  noReplyCount;
    unsigned long  badReplyCount;

    int            flagDoTiming;
    double         transMax;
    double         transAvg;

} easyDriverPvt;

/*
 * Report an unexpected reply
 */
static asynStatus
badReply(asynUser *pasynUser, easyDriverPvt *ppvt)
{
    char escBuf[120];

    epicsStrnEscapedFromRaw(escBuf, sizeof escBuf, ppvt->replyBuf, ppvt->replyLen);
    epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                                          "Bad reply string: \"%s\"", escBuf);
    ppvt->badReplyCount++;
    return asynError;
}

/*
 * Send command and get reply
 */
static asynStatus
xfer(asynUser *pasynUser, easyDriverPvt *ppvt, size_t nSend)
{
    size_t nSent;
    int eom;
    asynStatus status;
    int retry = 0;
    epicsTimeStamp ts[2];
    double t;

    ppvt->commandCount++;
    for (;;) {
        if (ppvt->flagDoTiming) epicsTimeGetCurrent(&ts[0]);
        status = pasynOctetSyncIO->writeRead(ppvt->pasynUser,
                                ppvt->sendBuf, nSend,
                                ppvt->replyBuf, sizeof ppvt->replyBuf - 1, 0.1,
                                &nSent, &ppvt->replyLen, &eom);
        if (ppvt->flagDoTiming) epicsTimeGetCurrent(&ts[1]);
        if (status == asynSuccess)
            break;
        if (++retry > 10) {
            epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                                        "%s", ppvt->pasynUser->errorMessage);
            ppvt->noReplyCount++;
            return status;
        }
        ppvt->retryCount++;
    }
    ppvt->replyBuf[ppvt->replyLen] = '\0';
    if (ppvt->flagDoTiming) {
        t = epicsTimeDiffInSeconds(&ts[1], &ts[0]);
        if (t > ppvt->transMax) ppvt->transMax = t;
        ppvt->transAvg = ppvt->transAvg ? ppvt->transAvg * 0.998 + t * 0.002 : t;
    }
    return asynSuccess;
}

/*
 * Format and send a command string
 */
static asynStatus
xferf(asynUser *pasynUser, easyDriverPvt *ppvt, const char *fmt, ...)
                                                        EPICS_PRINTF_STYLE(3,4);
static asynStatus
xferf(asynUser *pasynUser, easyDriverPvt *ppvt, const char *fmt, ...)
{
    size_t nSend;
    va_list args;
    asynStatus status;

    va_start(args, fmt);
    nSend = epicsVsnprintf(ppvt->sendBuf, sizeof ppvt->sendBuf, fmt, args);
    va_end(args);
    if (nSend >= sizeof ppvt->sendBuf) {
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                                        "Formatted message too long to send.");
        return asynError;
    }
    status = xfer(pasynUser, ppvt, nSend);
    if (status != asynSuccess)
        return status;
    if (strcmp(ppvt->replyBuf, "#AK") != 0)
        return badReply(pasynUser, ppvt);
    return asynSuccess;
}

/*
 * Read a floating point value
 */
static asynStatus
read64f(asynUser *pasynUser, easyDriverPvt *ppvt, epicsFloat64 *value,
                                                    const char *fmt, ...)
                                                        EPICS_PRINTF_STYLE(4,5);
static asynStatus
read64f(asynUser *pasynUser, easyDriverPvt *ppvt, epicsFloat64 *value,
                                                    const char *fmt, ...)
{
    size_t nSend;
    va_list args;
    asynStatus status;
    double x;
    const char *cp;

    va_start(args, fmt);
    nSend = epicsVsnprintf(ppvt->sendBuf, sizeof ppvt->sendBuf, fmt, args);
    va_end(args);
    if (nSend >= sizeof ppvt->sendBuf) {
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                                        "Formatted message too long to send.");
        return asynError;
    }
    status = xfer(pasynUser, ppvt, nSend);
    if (status != asynSuccess)
        return status;
    if ((cp = strchr(ppvt->replyBuf, ':')) == NULL)
        cp = ppvt->replyBuf;
    else
        cp++;
    if (sscanf(cp, "%lg", &x) != 1)
        return badReply(pasynUser, ppvt);
    *value = x;
    return asynSuccess;
}

/*
 * Process a status reply
 */
void
processStatusReply(easyDriverPvt *ppvt)
{
    ELLLIST *pclientList;
    interruptNode *pnode;
    int addr;

    pasynManager->interruptStart(ppvt->asynInt32InterruptPvt, &pclientList);
    pnode = (interruptNode *)ellFirst(pclientList);
    while (pnode) {
        asynInt32Interrupt *int32Interrupt = pnode->drvPvt;
        addr = int32Interrupt->addr;
        if (addr <= 31) {
            int32Interrupt->callback(int32Interrupt->userPvt,
                                     int32Interrupt->pasynUser,
                                     ((ppvt->rb.status & (1 << addr)) != 0));
        }
        pnode = (interruptNode *)ellNext(&pnode->node);
    }
    pasynManager->interruptStart(ppvt->asynFloat64InterruptPvt, &pclientList);
    pnode = (interruptNode *)ellFirst(pclientList);
    while (pnode) {
        asynFloat64Interrupt *float64Interrupt = pnode->drvPvt;
        double value;
        addr = float64Interrupt->addr;
        switch(addr) {
        case A_SETPOINT_CURRENT: value = ppvt->rb.setpointCurrent; break;
        case A_READBACK_CURRENT: value = ppvt->rb.rbCurrent;       break;
        default:                 addr = -1;                        break;
        }
        if (addr >= 0) {
            float64Interrupt->callback(float64Interrupt->userPvt,
                                       float64Interrupt->pasynUser,
                                       value);
        }
        pnode = (interruptNode *)ellNext(&pnode->node);
    }
    pasynManager->interruptEnd(ppvt->asynFloat64InterruptPvt);
}

/*
 * Send command and get reply
 */
static asynStatus
cmd(asynUser *pasynUser, easyDriverPvt *ppvt, int command, double setpoint)
{
    size_t nSend;
    asynStatus status;
    extern volatile int interruptAccept;

    nSend = sprintf(ppvt->sendBuf, "FDB:%2.2X:%.4f\r",
                                    command | (1 << EASY_DRIVER_WR_STAT_RESERVED),
                                    setpoint);
    status = xfer(pasynUser, ppvt, nSend);
    if (status != asynSuccess)
        return status;
    if (sscanf(ppvt->replyBuf, "#FDB:%X:%lf:%lf", &ppvt->rb.status,
                                            &ppvt->rb.setpointCurrent,
                                            &ppvt->rb.rbCurrent) != 3) {
        return badReply(pasynUser, ppvt);;
    }
    if (interruptAccept)
        processStatusReply(ppvt);
    return asynSuccess;
}

/*
 * asynCommon methods
 */
static void
report(void *pvt, FILE *fp, int details)
{
    easyDriverPvt *ppvt = (easyDriverPvt *)pvt;

    if (details >= 1) {
        if (ppvt->flagDoTiming) {
            fprintf(fp, "Transaction time avg:%.3g max:%.3g\n", ppvt->transAvg, ppvt->transMax);
            if (details >= 2) {
                ppvt->transMax = 0;
                ppvt->transAvg = 0;
            }
        }
        fprintf(fp, "         Command count: %lu\n", ppvt->commandCount);
        fprintf(fp, " Setpoint update count: %lu\n", ppvt->setpointUpdateCount);
        fprintf(fp, "           Retry count: %lu\n", ppvt->retryCount);
        fprintf(fp, "        No reply count: %lu\n", ppvt->noReplyCount);
        fprintf(fp, "       Bad reply count: %lu\n", ppvt->badReplyCount);
    }
}

static asynStatus
connect(void *pvt, asynUser *pasynUser)
{
    return pasynManager->exceptionConnect(pasynUser);
}

static asynStatus
disconnect(void *pvt, asynUser *pasynUser)
{
    return pasynManager->exceptionDisconnect(pasynUser);
}
static asynCommon commonMethods = { report, connect, disconnect };

/*
 * asynOctet method
 */
static asynStatus
octetRead(void *pvt, asynUser *pasynUser, char *data,
          size_t maxchars, size_t *nbytesTransfered, int *eomReason)
{
    easyDriverPvt *ppvt = (easyDriverPvt *)pvt;
    asynStatus status;
    int address;
    size_t nSend;

    if ((status = pasynManager->getAddr(pasynUser, &address)) != asynSuccess)
        return status;
    if (address != 0) {
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                          "Invalid asynOctet read address %d", address);
        return asynError;
    }
    nSend = sprintf(ppvt->sendBuf, "MVER\r");
    status = xfer(pasynUser, ppvt, nSend);
    if (status != asynSuccess)
        return status;
    if (strncmp(ppvt->replyBuf, "#MVER:", 6) != 0)
        return badReply(pasynUser, ppvt);
    nSend = ppvt->replyLen - 6;
    if (maxchars <= nSend) {
        *eomReason = ASYN_EOM_CNT;
        *nbytesTransfered = maxchars;
    }
    else {
        *eomReason = ASYN_EOM_EOS;
        *nbytesTransfered = ppvt->replyLen - 6;
    }
    strncpy(data, ppvt->replyBuf + 6, *nbytesTransfered);
    return asynSuccess;
}
static asynOctet octetMethods = { NULL, octetRead };

/*
 * asynInt32 methods
 */
static asynStatus
int32Write(void *pvt, asynUser *pasynUser, epicsInt32 value)
{
    easyDriverPvt *ppvt = (easyDriverPvt *)pvt;
    asynStatus status;
    int address;

    if ((status = pasynManager->getAddr(pasynUser, &address)) != asynSuccess)
        return status;
    switch(address) {
    //case A_WRITE_BULK_ON:
    case A_WRITE_SUPPLY_ON:
        /* See how things now stand */
        status = cmd(pasynUser, ppvt, 1 << EASY_DRIVER_WR_STAT_IGNORE, 0);
        if (status != asynSuccess) return status;
        if (value) {																			// Bulk Enable or Module Enable
            /* If supply is off, turn it on */
            if ((address == A_WRITE_SUPPLY_ON)													// if CMD is "Module ON" and the Module is OFF
             && ((ppvt->rb.status & (1 << EASY_DRIVER_RD_STAT_ONOFF)) == 0)){					

				return cmd(pasynUser, ppvt, (1 << EASY_DRIVER_WR_STAT_ONOFF), 0.0);				// Put the module on with the setpoint 0.0
			}
        }

        else {																					// Bulk Disable or Module Disable
            if ((ppvt->rb.status & (1 << EASY_DRIVER_RD_STAT_ONOFF)) != 0) {					// If the module is not OFF
                 if (ppvt->rb.setpointCurrent != 0) {
                    double pause = ppvt->rb.setpointCurrent / EASY_DRIVER_PS_MAX_RAMP_RATIO;	// todo: verify this parameter
                    /* Ramp down before turning off */
                    status = cmd(pasynUser, ppvt,
                                            (1 << EASY_DRIVER_WR_STAT_ONOFF) |					// Leave module ON
                                            (1 << EASY_DRIVER_WR_STAT_SLEWRATE),  0.0);			// Perform a ramp to 0.0
                    if (status != asynSuccess) return status;
                    epicsThreadSleep(pause);
                }

                status = cmd(pasynUser, ppvt, 0, 0.0);											// Set the Module OFF
                if (status != asynSuccess) return status;
            }
        }
        break;

    case A_WRITE_RESET:
        return cmd(pasynUser, ppvt, (1 << EASY_DRIVER_WR_STAT_RESET), 0.0);

    case A_WRITE_SLEW_MODE:
        ppvt->slewMode = value ? EASY_DRIVER_WR_STAT_SLEWRATE : 0;
        break;

    default:
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                              "Invalid asynInt32 write address %d", address);
        return asynError;
    }
    return asynSuccess;
}

static asynStatus
int32Read(void *pvt, asynUser *pasynUser, epicsInt32 *value)
{
    easyDriverPvt *ppvt = (easyDriverPvt *)pvt;
    asynStatus status;
    int address;

    if ((status = pasynManager->getAddr(pasynUser, &address)) != asynSuccess)
        return status;
    switch(address) {
    case A_READ_SLEW_MODE:
        *value = (ppvt->slewMode != 0);
        break;

    case A_READ_FORCE_READBACK:
        status = cmd(pasynUser, ppvt, (1 << EASY_DRIVER_WR_STAT_IGNORE), 0.0);
        *value = status;
        return status;

    default:
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                          "Invalid asynInt32 read address %d", address);
        return asynError;
    }
    return asynSuccess;
}

static asynInt32 int32Methods = { int32Write, int32Read };

/*
 * asynFloat64 methods
 */
static asynStatus
float64Write(void *pvt, asynUser *pasynUser, epicsFloat64 value)
{
    easyDriverPvt *ppvt = (easyDriverPvt *)pvt;
    asynStatus status;
    int address;

    if ((status = pasynManager->getAddr(pasynUser, &address)) != asynSuccess)
        return status;
    switch(address) {
    case A_Kp: case A_Ki: case A_Kd:
        status = cmd(pasynUser, ppvt, 1 << EASY_DRIVER_WR_STAT_IGNORE, 0);
        if (status != asynSuccess)
            return status;
        if ((ppvt->rb.status & (1 << EASY_DRIVER_RD_STAT_ONOFF)) != 0) {
            epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                                  "Can't set controller gain when ON");
            return asynError;
        }
        status = xferf(pasynUser, ppvt, "MWG:%d:%.4e\r", address, value);
        if (status != asynSuccess)
            return status;
        epicsThreadSleep(0.2);
        status = xferf(pasynUser, ppvt, "MUP\r");
        if (status != asynSuccess)
            return status;
        epicsThreadSleep(0.2);
        status = xferf(pasynUser, ppvt, "PTP\r");
        if (status != asynSuccess)
            return status;
        break;

    case A_SETPOINT_CURRENT:
        ppvt->setpointUpdateCount++;
        status = cmd(pasynUser, ppvt, (1 << EASY_DRIVER_WR_STAT_ONOFF) |
                                      ppvt->slewMode, value);				// Use the ramp flag in the SlewMode variable
        return status;

    default:
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                              "Invalid asynFloat64 write address %d", address);
        return asynError;
    }
    return asynSuccess;
}

static asynStatus
float64Read(void *pvt, asynUser *pasynUser, epicsFloat64 *value)
{
    easyDriverPvt *ppvt = (easyDriverPvt *)pvt;
    asynStatus status;
    int address;

    if ((status = pasynManager->getAddr(pasynUser, &address)) != asynSuccess)
        return status;

    switch (address) {
    case A_Kp:
    case A_Ki:
    case A_Kd:
        return read64f(pasynUser, ppvt, value, "MRG:%d\r", address);

    case A_READ_BULK_VOLTAGE:
        return read64f(pasynUser, ppvt, value, "MRP\r");

    case A_READ_FET_TEMPERATURE:
        return read64f(pasynUser, ppvt, value, "MRT\r");

    case A_READ_SHUNT_TEMPERATURE:
        return read64f(pasynUser, ppvt, value, "MRTS\r");

    case A_READ_OUTPUT_VOLTAGE:
        return read64f(pasynUser, ppvt, value, "MRV\r");

    case A_SETPOINT_CURRENT:
        status = cmd(pasynUser, ppvt, 1 << EASY_DRIVER_WR_STAT_IGNORE, 0);
        if (status != asynSuccess)
            return status;
        *value = ppvt->rb.setpointCurrent;
        break;

    default:
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                              "Invalid asynFloat64 read address %d", address);
        return asynError;
    }
    return asynSuccess;
}

static asynFloat64 float64Methods = { float64Write, float64Read };

/*
 * asynFloat32Array methods
 */
static asynStatus
float32ArrayWrite(void *pvt, asynUser *pasynUser, epicsFloat32 *value, size_t nelements)
{
    easyDriverPvt *ppvt = (easyDriverPvt *)pvt;
    asynStatus status;
    int address;
    unsigned int i;

    if ((status = pasynManager->getAddr(pasynUser, &address)) != asynSuccess)
        return status;
    if (address != A_WAVEFORM) {
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                          "Invalid asynFloat32Array write address %d", address);
        return asynError;
    }
    status = xferf(pasynUser, ppvt, "MWAVEP:%u\r", (unsigned int)nelements);
    if (status != asynSuccess)
        return status;
    for (i = 0 ; i < nelements ; i++) {
        status = xferf(pasynUser, ppvt, "MWAVE:%u:%g\r", i, value[i]);
        if (status != asynSuccess)
            return status;
    }
    return asynSuccess;
}

static asynFloat32Array float32ArrayMethods = { float32ArrayWrite };

epicsShareFunc int 
devEasyDriverConfigure(const char *portName, const char *hostInfo, int flags, int priority)
{
    easyDriverPvt *ppvt;
    char *lowerName, *host;
    asynStatus status;

    /*
     * Create our private data area
     */
    ppvt = callocMustSucceed(1, sizeof(easyDriverPvt), "devEasyDriverConfigure");
    ppvt->flagDoTiming = ((flags & FLAG_DO_TIMING_TESTS) != 0);
    if (priority == 0) priority = epicsThreadPriorityMedium;

    /*
     * Create the port that we'll use for I/O.
     * Configure it with our priority, autoconnect, process EOS.
     * No output EOS, "\r" inputEOS.
     * We have to create this port since we are multi-address and the
     * IP port is single-address.
     */
    lowerName = callocMustSucceed(1, strlen(portName)+5, "devEasyDriverConfigure");
    sprintf(lowerName, "%s_TCP", portName);
    host = callocMustSucceed(1, strlen(hostInfo)+5, "devEasyDriverConfigure");
    sprintf(host, "%s TCP", hostInfo);
    drvAsynIPPortConfigure(lowerName, host, priority, 0, 0);
    status = pasynOctetSyncIO->connect(lowerName, -1, &ppvt->pasynUser, NULL);
    if (status != asynSuccess) {
        printf("Can't connect to \"%s\"\n", lowerName);
        return -1;
    }
    free(host);
    free(lowerName);
    status = pasynOctetSyncIO->setInputEos(ppvt->pasynUser, "\r", 1);
    if (status != asynSuccess) {
        printf("Can't set input end-of-string: %s.\n", ppvt->pasynUser->errorMessage);
        return -1;
    }

    /*
     * Create our port
     */
    status = pasynManager->registerPort(portName,
                                        ASYN_MULTIDEVICE | ASYN_CANBLOCK,
                                        1,         /*  autoconnect */
                                        priority,  /* priority (for now) */
                                        0);        /* default stack size */
    if (status != asynSuccess) {
        printf("Can't register port %s", portName);
        return -1;
    }

    /*
     * Advertise our interfaces
     */
    ppvt->asynCommon.interfaceType = asynCommonType;
    ppvt->asynCommon.pinterface  = &commonMethods;
    ppvt->asynCommon.drvPvt = ppvt;
    status = pasynManager->registerInterface(portName, &ppvt->asynCommon);
    if (status != asynSuccess) {
        printf("Can't register asynCommon support.\n");
        return -1;
    }
    ppvt->asynOctet.interfaceType = asynOctetType;
    ppvt->asynOctet.pinterface = &octetMethods;
    ppvt->asynOctet.drvPvt = ppvt;
    status = pasynOctetBase->initialize(portName, &ppvt->asynOctet, 0, 0, 0);
    if (status != asynSuccess) {
        printf("Can't register asynOctet support.\n");
        return -1;
    }
    ppvt->asynInt32.interfaceType = asynInt32Type;
    ppvt->asynInt32.pinterface = &int32Methods;
    ppvt->asynInt32.drvPvt = ppvt;
    status = pasynInt32Base->initialize(portName, &ppvt->asynInt32);
    if (status != asynSuccess) {
        printf("Can't register asynInt32 support.\n");
        return -1;
    }
    pasynManager->registerInterruptSource(portName, &ppvt->asynInt32,
                                                &ppvt->asynInt32InterruptPvt);
    ppvt->asynFloat64.interfaceType = asynFloat64Type;
    ppvt->asynFloat64.pinterface = &float64Methods;
    ppvt->asynFloat64.drvPvt = ppvt;
    status = pasynFloat64Base->initialize(portName, &ppvt->asynFloat64);
    if (status != asynSuccess) {
        printf("Can't register asynFloat64 support.\n");
        return -1;
    }
    pasynManager->registerInterruptSource(portName, &ppvt->asynFloat64,
                                                &ppvt->asynFloat64InterruptPvt);
    ppvt->asynFloat32Array.interfaceType = asynFloat32ArrayType;
    ppvt->asynFloat32Array.pinterface = &float32ArrayMethods;
    ppvt->asynFloat32Array.drvPvt = ppvt;
    status = pasynFloat32ArrayBase->initialize(portName, &ppvt->asynFloat32Array);
    if (status != asynSuccess) {
        printf("Can't register asynFloat32Array support.\n");
        return -1;
    }
    return 0;
}

/*
 * IOC shell command
 */
static const iocshArg devEasyDriverConfigureArg0 = { "port name",iocshArgString};
static const iocshArg devEasyDriverConfigureArg1 = { "host:port",iocshArgString};
static const iocshArg devEasyDriverConfigureArg2 = { "flags",iocshArgInt};
static const iocshArg devEasyDriverConfigureArg3 = { "priority",iocshArgInt};
static const iocshArg *devEasyDriverConfigureArgs[] = {
                    &devEasyDriverConfigureArg0, &devEasyDriverConfigureArg1,
                    &devEasyDriverConfigureArg2, &devEasyDriverConfigureArg3 };
static const iocshFuncDef devEasyDriverConfigureFuncDef =
                      {"devEasyDriverConfigure",4,devEasyDriverConfigureArgs};
static void devEasyDriverConfigureCallFunc(const iocshArgBuf *args)
{
    devEasyDriverConfigure(args[0].sval, args[1].sval, args[2].ival, args[3].ival);
}

static void
devEasyDriverConfigure_RegisterCommands(void)
{
    iocshRegister(&devEasyDriverConfigureFuncDef,devEasyDriverConfigureCallFunc);
}
epicsExportRegistrar(devEasyDriverConfigure_RegisterCommands);
