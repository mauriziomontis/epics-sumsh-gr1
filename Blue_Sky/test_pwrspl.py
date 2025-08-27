#!/usr/bin/env python3

from bluesky import RunEngine
from bluesky.plans import scan
from bluesky.callbacks import LiveTable
from blue_pwrspl import pwrspl

def callback(status):
    print("Done:", status)

# Instantiate with the new PV prefix
psu = pwrspl("", name="psu")

# Turn on and set to 4 A
status = psu.set(4)
status.add_callback(callback)
status.wait()  # block until complete

# Create and configure the RunEngine
RE = RunEngine()

# Subscribe a LiveTable to show the current readback
RE.subscribe(LiveTable([psu.current_RB]))

# Perform a scan moving the PSU setpoint from -7 A to 7 A in 15 steps
RE(scan([], psu, -7, 7, 15))

psu.toff()
