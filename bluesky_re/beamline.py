# Workarounds/patches
from ophyd.signal import EpicsSignalBase
EpicsSignalBase.set_defaults(connection_timeout= 5, timeout=200)

from ophyd import EpicsMotor

# standard magics
from bluesky.magics import BlueskyMagics
get_ipython().register_magics(BlueskyMagics)

# simulated devices
from ophyd.sim import det1, det2, det3, det4, motor1, motor2, motor, noisy_det   # two simulated detectors

from beamlinetools.devices.gaussmeter import Gaussmeter
from beamlinetools.devices.motor import Motor
from beamlinetools.devices.PSU_ophyd import PowerSupply

FB_stage = Motor('Gr5', name='FB_stage')
Gauss = Gaussmeter('group5:', name='Gauss')
PS = PowerSupply('GROUP5:PS:', name='PowerSupply')
Gauss.wait_for_connection()
FB_stage.wait_for_connection()
PS.wait_for_connection()
