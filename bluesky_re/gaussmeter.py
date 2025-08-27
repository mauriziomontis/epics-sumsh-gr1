from ophyd import Device, Component, EpicsSignal, EpicsSignalRO

class Gaussmeter(Device):
	MField = Component(EpicsSignalRO, 'GM:MField')
	On_Off = Component(EpicsSignal, 'GM:On_Off')
	On_Off_RB = Component(EpicsSignalRO, 'GM:On_Off_RB')
	ACDC = Component(EpicsSignal, 'GM:ACDC')
	ACDC_RB = Component(EpicsSignalRO, 'GM:ACDC_RB')
	#group5:GM:Mfield
	#group5:GM:On_Off
	#group5:GM:On_Off_RB
	#group5:GM:ACDC
	#group5:GM:ACDC_RB
	

