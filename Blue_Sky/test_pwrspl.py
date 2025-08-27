#!/usr/bin/env python3

from bluesky import RunEngine
from bluesky.plans import count, scan, list_scan
from bluesky.callbacks import LiveTable
from PSU_ophyd import pwrspl


def callback(status):
	print("Done: ", status)
	
psu = pwrspl("G1:Power:", name="psu")
status = psu.set(4)
print(status)

RE = RunEngine()
token = RE.subscribe(LiveTable([psu.current_RB]))
RE(scan([], psu, 0, 5, 6))
