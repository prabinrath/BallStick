#!/usr/bin/env python
import numpy as np
from keras.models import load_model
import socket
import sys
import time
import sys

server_addr = ('localhost', 2300)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
if s < 0:
    print "Error creating socket"
try:
    s.connect(server_addr)
    print "Connected to %s" % repr(server_addr)
except:
    print "ERROR: Connection to %s refused" % repr(server_addr)
    sys.exit(1)
try:
	model=load_model('/home/prabin/git/BallStick/benchmarks/4641ththth2.h5')
	while True:
		buff = s.recv(1024)
		#print buff
		if(buff=='CONN_CHCK'):
			nsent = s.send('ALL_SET')
		else:
			data = buff.split(',')
			if len(data)==1:
				print(data[0])
				nsent = s.send('ALL_SET')
			else:
			    for i in range(0,len(data)):
			        data[i] = float(data[i])
			    data = np.array([data],np.float64)
			    max_v=[1,1,1,1];
			    min_v=[0,0,0,0];
			    max_rv=[30,30,300,300]
			    min_rv=[-30,-30,0,0]
			    for i in range(4):
			        data[:,i]=min_v[i]+(((data[:,i]-min_rv[i])/(max_rv[i]-min_rv[i]))*max_v[i])
			    pred = model.predict(data,batch_size=4,verbose=2)
			    pred=-90+pred[0]*(90-(-90))
			    nsent = s.send(str(pred[0]))
finally:
	print "Closing socket"
s.close()