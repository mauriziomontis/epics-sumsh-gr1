#KEK!/usr/bin/env python3

"""
Hall Sensor I2C GPIO - Device

Author: Maurizio Montis
Email: maurizio.montis@lnl.infn.it
Description:
    Script to interface a Hall Sensor device connected via I2C GPIO over TCP/IP.
    Based on the code provided by EPICS Community for USPAS Training.

172.30.0.0 / 16   
"""

__author__ = "Maurizio Montis"
__email__ = "maurizio.montis@lnl.infn.it"
__version__ = "1.0.0"
__license__ = "MIT"


# Standard library imports
import socketserver
import sys


# ========================== Configuration ==========================
I2C_ADDR       = 0x48     # 0x48 if ADDR=GND
GAIN           = 1        # PGA: {2/3,1,2,4,8,16}. 1 => ±4.096V FS (ADS)
DATA_RATE_SPS  = 128      # 8..860 samples/s
READ_INTERVAL  = 0.20     # secs between readings (MODE continuous)

# Note: (top=10k, bottom=20k) => V_adc = V_sens * 2/3  => V_sens = V_adc * 1.5
DIVIDER_RATIO  = 1      # ratio used to find the real sensor voltage

# Hall Sensor Calibration (tipica per A1302 ~2.5 mV/G).
V0             = 2.50     # Volt for Zero Magnet Field  
SENS_V_PER_G   = 0.0025   # Sensibility in V/G (2.5 mV/G). (Remember: 1 Tesla = 10_000 Gauss :D )
# ====================================================================

status = {}
verbose = "--verbose" in sys.argv

class HallSensorDummy(socketserver.StreamRequestHandler):
    def handle(self):
        global status
        global verbose
        client = self.client_address[0]
        if client in status:
            on = status[client]['on']
        else:
            on = False
            status[client] = {}
            status[client]['on'] = on

        if verbose:
            print("Hall Sensor initialized.")
            
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
                    #_, _, B_g, = read_sample(ch0)
                    reply = f'MGFLD: 69'
                else:
                    reply = 'MGFLD: 0.0000'

            # Sensor Voltage Reading Command
            elif line == 'VOLT?':
                if on:
                    #v_ads, v_sens, B_g = read_sample(ch0)
                    reply = f'VOLT: 69'
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

# address: 172.30.84.235

server = Server(('0.0.0.0', 10000), HallSensorDummy)
print("Serving on TCP 10000")
print("Terminate with Ctrl-C")
try:
    server.serve_forever()
except KeyboardInterrupt:
    sys.exit(0)
