#!../../bin/linux-x86_64/mgfield

#- SPDX-FileCopyrightText: 2005 Argonne National Laboratory
#-
#- SPDX-License-Identifier: EPICS

#- You may have to change mgfield to something else
#- everywhere it appears in this file

#< envPaths

## Register all support components
dbLoadDatabase "../../dbd/mgfield.dbd"
mgfield_registerRecordDeviceDriver(pdbbase) 

## Load record instances
#dbLoadRecords("../../db/mgfield.db","user=iocadm")

iocInit()

## Start any sequence programs
#seq sncmgfield,"user=iocadm"
