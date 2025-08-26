#!/usr/bin/env python3

"""
Hall Sensor I2C GPIO - Device

Author: Maurizio Montis
Email: maurizio.montis@lnl.infn.it
Description:
    Script to interface a Hall Sensor device connected via I2C GPIO over TCP/IP.
    Based on the code provided by EPICS Community for USPAS Training.


"""

__author__ = "Maurizio Montis"
__email__ = "maurizio.montis@lnl.infn.it"
__version__ = "1.0.0"
__license__ = "MIT"


# Standard library imports
import socketserver
import os
import pprint
import random
import sys

import time
import argparse

import board
import busio
from adafruit_ads1x15.ads1115 import ADS1115
from adafruit_ads1x15.ads1x15 import Mode
from adafruit_ads1x15.analog_in import AnalogIn


# ========================== Configuration ==========================
I2C_ADDR       = 0x48     # 0x48 if ADDR=GND
GAIN           = 1        # PGA: {2/3,1,2,4,8,16}. 1 => ±4.096V FS (ADS)
DATA_RATE_SPS  = 128      # 8..860 samples/s
READ_INTERVAL  = 0.20     # secs between readings (MODE continuous)

# Note: (top=10k, bottom=20k) => V_adc = V_sens * 2/3  => V_sens = V_adc * 1.5
DIVIDER_RATIO  = 1      # ratio used to find the real sensor voltage

# Hall Sensor Calibration ( A1302 ~2.5 mV/G).
V0             = 1.5     # Volt for Zero Magnet Field  
SENS_V_PER_G   = 0.0014   # Sensibility in V/G (1.4 mV/G). (Remember: 1 Tesla = 10_000 Gauss :D )
# ====================================================================

status = {}
verbose = "--verbose" in sys.argv


## Additional Methos for I2C GPIO
def setup_adc():
    """I2C and ADS1115 initialization (single mode, channel AIN0)."""
    i2c = busio.I2C(board.SCL, board.SDA)   # bus I2C-1 sul Raspberry Pi
    ads = ADS1115(i2c, address=I2C_ADDR)
    ads.mode = Mode.SINGLE              
    ads.data_rate = DATA_RATE_SPS
    ads.gain = GAIN
    ch0 = AnalogIn(ads, 0)     
    return ads, ch0

def read_sample(chn):
    """Output (v_adc, v_sens, B_gauss)."""
    v_adc = chn.voltage                     
    v_sens = v_adc * DIVIDER_RATIO          
    delta_v = v_sens - V0
    B_gauss = - delta_v / SENS_V_PER_G        # G = V / (V/G)
    #return v_adc, v_sens, B_gauss
    return v_adc, delta_v, B_gauss


class HallSensor(socketserver.StreamRequestHandler):
    def handle(self):
        global status
        global verbose
        client = self.client_address[0]
        if client in status:
            #voltage = status[client]['volts']
            #mgfield = status[client]['mgfield']
            on = status[client]['on']
        else:
            #voltage = 0
            #mgfield = 0
            on = False
            status[client] = {}
            #status[client]['volts'] = voltage
            #status[client]['mgfield'] = mgfield
            status[client]['on'] = on

        # setup ADC
        ads, ch0 = setup_adc()
        if verbose:
            print("Hall Sensor initialized.")
            print(f"Addr=0x{I2C_ADDR:02X}, gain={ads.gain}, rate={ads.data_rate} SPS")
            print(f"Divider ratio={DIVIDER_RATIO}, V0={V0} V, Sens={SENS_V_PER_G*1e3:.2f} mV/G")

        while True:
            line = self.rfile.readline().strip()
            
            if not line:
                break
            
            line = line.decode('utf-8')  # Decode bytes to string
            args = line.split()
            reply = None
            
            if verbose:
                print("--> " + line)
            
            # IND Command
            if line == '*IDN?':
                reply = 'RASPY 4B HALL SENSOR (ADC1115 + Hall Sensor SE014)'
            
            # Enable Command
            elif line == 'ENABLE?':
                reply = 'ON '  + ('1' if on else '0')

            # Magnetic Field Reading Command
            elif line == 'MGFLD?':               
                if on:
                    _, _, B_g, = read_sample(ch0)
                    reply = f'MGFLD: {B_g:.4f}'
                else:
                    reply = 'MGFLD: 0.0000'

            # Sensor Voltage Reading Command
            elif line == 'VOLT?':
                if on:
                    v_ads, v_sens, B_g = read_sample(ch0)
                    reply = f'VOLT: {v_sens:.4f}'
                else:
                    reply = 'VOLT: 0.0000' 
                       
            elif len(args) > 1:
                try:
                    val = float(args[1])
                    if args[0] == 'ON':
                        if args[1] == '1':
                            on = True
                            reply = 'ON 1'
                        elif args[1] == '0':
                            on = False
                            reply = 'ON 0'
                    status[client]['on'] = on
                
                except Exception:
                    pass
            
            if reply:
                self.wfile.write((reply + '\r\n').encode('utf-8'))  # Encode string to bytes
                if verbose:
                    print("<-- " + reply)

class Server(socketserver.ThreadingMixIn, socketserver.TCPServer):
    daemon_threads = True
    allow_reuse_address = True
    def __init__(self, server_address, RequestHandlerClass):
        super().__init__(server_address, RequestHandlerClass)

server = Server(('0.0.0.0', 10000), HallSensor)
print("Serving on TCP 10000")
print("Terminate with Ctrl-C")
try:
    server.serve_forever()
except KeyboardInterrupt:
    sys.exit(0)
