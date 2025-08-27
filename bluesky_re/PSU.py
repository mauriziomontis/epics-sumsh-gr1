#!/usr/bin/env python3

from ophyd import Component, Device, EpicsSignal, EpicsSignalRO

class PowerSupply(Device):

	voltage_RB = Component(EpicsSignalRO, "VOLTAGE_RB")
	current_RB = Component(EpicsSignalRO, "CURRENT_RB")
	skewrate_RB = Component(EpicsSignalRO, "SKEW_RATE_RB")
	
	current_SP = Component(EpicsSignal, "CURRENT_SP")
	skewrate_SP = Component(EpicsSignal, "SKEW_RATE_SP")
	
	onoff_switch = Component(EpicsSignal, "ONOFF.PROC")
	

