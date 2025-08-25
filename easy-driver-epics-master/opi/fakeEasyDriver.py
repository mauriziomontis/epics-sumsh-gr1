##############################################################################
# Copyright (c) 2015 CAEN ELS
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
##############################################################################

#
# Very simple-minded simulation of a CAEN ELS Easy Driver power supply
#
import SocketServer
import string
import random

class DummySupply(SocketServer.BaseRequestHandler):
    status = 0
    current = 0
    def handle(self):
        line = self.request[0].strip()
        if(len(line) != 0):
            args = string.split(line, ':')
            command = int(args[1],16)
            if ((command & 0x80) == 0):
                if (command & 0x40):
                    DummySupply.status |= 0x1
                    DummySupply.current = float(args[2])
                else:
                    DummySupply.status &= ~0x1
            feedback = DummySupply.current*(1.0-random.random()/100.0)
            self.request[1].sendto("#FDB:%2.2X: +%.4f: +%.4f\r" % \
                                (DummySupply.status, DummySupply.current, \
                                 feedback), self.client_address)

server = SocketServer.UDPServer(('', 10001), DummySupply)
server.serve_forever()
