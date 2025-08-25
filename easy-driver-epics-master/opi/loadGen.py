#
# Generate synthetic load for Caen Els Easy Driver power supply timing tests
#

from epics import caget, caput
import time
import argparse

parser = argparse.ArgumentParser(description='Generate setpoint changes for CEAN ELS Easy Driver power supply.')
parser.add_argument('-r', '--rate', type=float, default=1, help='Setpoint update rate (Hz)')
parser.add_argument('pvPrefix', nargs='?', default='EasyDriver:1:')
args = parser.parse_args()

pvName = args.pvPrefix + 'Setpoint'
interval = 1.0 / args.rate

while 1:
    caput(pvName, 0.1)
    time.sleep(interval)
    caput(pvName, 0.2)
    time.sleep(interval)
