# start of automatically prepended lines
from beamlinetools.BEAMLINE_CONFIG import *
# end of automatically prepended lines
#from bluesky.plans import scan, FB_stage
from bluesky.plan_stubs import mv, sleep


def init_exp():
	#yield from mv(FB_stage.setpoint, 1)
	status = FB_stage.set(1)
	yield from sleep(5)
	yield from mv(Gauss.RESET, 1)
	yield from sleep(20)


def exp1(current, start, stop, steps):

	status = FB_stage.set(start)
	yield from sleep(5)
	status = PS.set(current)
	yield from scan([Gauss.MField], FB_stage, start, stop, steps)
	#yield from sleep(2)
	#yield from scan([Gauss.MField, PS.voltage_RB], FB_stage, start, stop, steps)
	
	
def exp2(position, start, stop, steps):
	
	
	status = FB_stage.set(position)
	status = PS.set(start)
	yield from sleep(5)
	#yield from scan([Gauss.MField, PS.current_RB], PS, start, stop, steps)
	yield from scan([Gauss.MField], PS, start, stop, steps)
	#yield from scan([PS.current_RB], PS, start, stop, steps)
	
	
	
