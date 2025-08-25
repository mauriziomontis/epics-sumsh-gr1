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

/* bit order meaning in the rd state mask */
#define PS_RD_NUM_STATES          6
#define EASY_DRIVER_RD_STAT_ONOFF        0      // 1=module on 0=module off
#define EASY_DRIVER_RD_STAT_FAULT        1      // 1=generic fault 0=ok
#define EASY_DRIVER_RD_STAT_DCUNDERVOLT  2      // 1=dc undervoltage 0=ok 
#define EASY_DRIVER_RD_STAT_MOSFETTEMP   3      // 1=mosfet temp. alarm 0=ok 
#define EASY_DRIVER_RD_STAT_SHUNTTEMP    4      // 1=shunt temp. alarm 0=ok 
#define EASY_DRIVER_RD_STAT_EXTINT       5      // 1=external interlock 


/* bit order in the wr state mask */
#define EASY_DRIVER_WR_STAT_RESERVED     0      // reserved, always set to 1
#define EASY_DRIVER_WR_STAT_BULKONOFF    3      // 1=switch bulk on, 0=switch bulk off
#define EASY_DRIVER_WR_STAT_SLEWRATE     4      // 1=enabled slew rate control
#define EASY_DRIVER_WR_STAT_RESET        5      // 1=reset errors
#define EASY_DRIVER_WR_STAT_ONOFF        6      // 1=switch ps on, 0=switch ps off
#define EASY_DRIVER_WR_STAT_IGNORE       7      // 1=ignore status/current setting, trigger readback response

/* EEPROM value indices */
#define EASY_DRIVER_EEPROM_KP_IDX  13
#define EASY_DRIVER_EEPROM_KI_IDX  14
#define EASY_DRIVER_EEPROM_KD_IDX  15

/* Maximum slew rate */
#define EASY_DRIVER_PS_MAX_RAMP_RATIO    20 // 20 Amp/sec (used during switching off procedure)
