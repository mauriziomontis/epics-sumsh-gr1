#!../../bin/linux-x86_64/raspignet

#- SPDX-FileCopyrightText: 2005 Argonne National Laboratory
#-
#- SPDX-License-Identifier: EPICS

#- You may have to change raspignet to something else
#- everywhere it appears in this file

< envPaths
epicsEnvSet("STREAM_PROTOCOL_PATH", "../../db")

## Register all support components
dbLoadDatabase "../../dbd/raspignet.dbd"
raspignet_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords "../../db/gsmtr.db","user=iocadm"

#drvAsynIPPortConfigure("RASPY1", "172.30.84.235:10000", 0, 0, 0)
drvAsynIPPortConfigure("RASPY1", "192.168.1.69:10000", 0, 0, 0)

iocInit()

## Start any sequence programs
#seq sncraspignet,"user=iocadm"
