#!../../bin/linux-x86_64/motorcontrol

#- SPDX-FileCopyrightText: 2005 Argonne National Laboratory
#-
#- SPDX-License-Identifier: EPICS

#- You may have to change motorcontrol to something else
#- everywhere it appears in this file

< envPaths
epicsEnvSet("STREAM_PROTOCOL_PATH", "../../db")

## Register all support components
dbLoadDatabase "../../dbd/motorcontrol.dbd"
motorcontrol_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("../../db/motorcontrol.db","P=GR1:Ax1:,MOTOR=GR1:Ax1_Mtr")
#drvAsynIPPortConfigure("MOTOR", "172.30.84.151", 0, 0, 0)

iocInit()

## Start any sequence programs
#seq sncmotorcontrol,"user=iocadm"
registrar(motorcalibration)