#!/usr/bin/env python30

from ophyd import Component, Device, EpicsSignal, EpicsSignalRO#, Status
from ophyd.status import Status

class PowerSupply(Device):

	voltage_RB = Component(EpicsSignalRO, "VOLTAGE_RB", kind="hinted")
	current_RB = Component(EpicsSignalRO, "CURRENT_RB", kind="hinted")
	#current_RB = Component(EpicsSignalRO, "xxxExample")
	skewrate_RB = Component(EpicsSignalRO, "SKEW_RATE_RB")
	
	current_SP = Component(EpicsSignal, "CURRENT_SP")
	#current_SP = Component(EpicsSignal, "subExample")
	skewrate_SP = Component(EpicsSignal, "SKEW_RATE_SP")
	
	onoff_switch = Component(EpicsSignal, "ONOFF.PROC")
	onoff_status = Component(EpicsSignal, "STATUSREGISTER.B0")
	
	def set(self, setpoint):
	
		stat = Status(timeout=30)
		
		#Check if PSU-output is on, if not turn it on
		if self.onoff_status.get() == 0:
			self.onoff_switch.set(1)
	
		#Set setpoint to change current
		self.current_SP.put(setpoint)
		
		#Wait for actual current (RB) to reach setpoint (SP)
		while(True):
			if abs(self.current_RB.get() - self.current_SP.get()) < 0.2:
				
				stat.set_finished()
				break
		
		return stat
	

