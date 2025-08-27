#!../../bin/linux-x86_64/PSU_control_2

#- SPDX-FileCopyrightText: 2005 Argonne National Laboratory
#-
#- SPDX-License-Identifier: EPICS

#- You may have to change PSU_control_2 to something else
#- everywhere it appears in this file

< envPaths
epicsEnvSet("STREAM_PROTOCOL_PATH", "../../db")

## Register all support components
dbLoadDatabase "../../dbd/PSU_control_2.dbd"
PSU_control_2_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("../../db/pwrspl.db","user=iocadm")

drvAsynIPPortConfigure("PWRSPL", "172.30.84.111:10001", 0, 0, 0)

iocInit()

## Start any sequence programs
#seq sncPSU_control_2,"user=iocadm"
