from ophyd import Device, Component, EpicsSignal, EpicsSignalRO
from ophyd.status import Status


class Motor(Device):
	setpoint = Component(EpicsSignal, ':Ax1_Mtr')
	readback = Component(EpicsSignal, ':Ax1_Mtr', kind="hinted")
	
	def __init__(self, prefix, *, name,settle_time=5, **kwargs):

	    self.settle_time = settle_time
	    
	    super().__init__(prefix,name=name, **kwargs)
	
	
	def set(self, position:int):
	
		sta = Status(settle_time=self.settle_time, timeout=30)
		
		self.setpoint.set(int(position)*100)

		sta.set_finished()
		
		return sta
	




