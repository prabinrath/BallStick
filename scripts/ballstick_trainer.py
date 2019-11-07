#!/usr/bin/env python
import numpy as np
import keras
from keras.models import Sequential,load_model
from keras.layers import Dense
from keras.optimizers import Adam
import socket
import sys
import time
import sys
print sys.argv[1]
if sys.argv[1] == '1':
	train = True
else:
	train = False

'''
4531ththth
4641ththth
'''

if not train:
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
            print buff
            if(buff=='CONN_CHCK'):
                nsent = s.send('ALL_SET')
            else:
                data = buff.split(',')
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
else:
	f = open('../datasets/dataset_new.txt','r+')
	trainx=[];trainy=[];testx=[];testy=[];
	while True:
		samp = f.readline()
		if(samp==''):
			break
		samp=samp.split(',')
		samp = [samp[0],samp[1],samp[2],samp[3],samp[6]]
		for i in range(0,len(samp)):
			samp[i] = float(samp[i])
		if np.random.choice([True, False], p=[0.8, 0.2]):
			trainx.append(samp[0:len(samp)-1])
			trainy.append(samp[len(samp)-1])
		else:
			testx.append(samp[0:len(samp)-1])
			testy.append(samp[len(samp)-1])
	f.close()

	trainx = np.array(trainx,np.float64)
	trainy = np.array(trainy,np.float64)
	testx = np.array(testx,np.float64)
	testy = np.array(testy,np.float64)

	max_v=[1,1,1,1];
	min_v=[0,0,0,0];
	max_rv=[30,30,300,300]
	min_rv=[-30,-30,0,0]
	for i in range(4):
		trainx[:,i]=min_v[i]+(((trainx[:,i]-min_rv[i])/(max_rv[i]-min_rv[i]))*max_v[i])
	for i in range(4):
		testx[:,i]=min_v[i]+(((testx[:,i]-min_rv[i])/(max_rv[i]-min_rv[i]))*max_v[i])
		
	trainy=0+(((trainy-(-90))/((90)-(-90)))*1)

	model=Sequential([Dense(6,input_shape=(4,),activation='tanh'),Dense(4,activation='tanh'),Dense(1,activation='tanh')])
	model.summary()
	model.compile(Adam(lr=0.001,decay=0.00001),loss='mean_squared_error')
	model.fit(trainx,trainy,validation_split=0.1,batch_size=10,epochs=50,shuffle=True,verbose=2)

	pred = model.predict(testx,batch_size=4,verbose=2)
	for i in range(0,len(pred)):
		print testy[i],' ',-90+pred[i]*(90-(-90))
	model.save('../weights/ballstick_new.h5')
