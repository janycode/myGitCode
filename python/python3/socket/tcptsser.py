#!/usr/bin/env python3

from twisted.internet import protocol, reactor
from time import ctime

PORT = 21567

class TSServProtocol(protocol.Protocol):
   def connectionMade(self):
       clnt = self.clnt = self.transport.getPeer().host
       print ('...connected from: ',clnt)
   def dataReceived(self,data):
       self.transport.write(('[%s] %s' %(ctime(), data)).encode('utf-8'))

factory = protocol.Factory()
factory.protocol = TSServProtocol
print ('waiting for connection...')
reactor.listenTCP(PORT, factory)
reactor.run()