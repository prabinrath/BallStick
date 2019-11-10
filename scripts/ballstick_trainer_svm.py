#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVR
#import joblib
from sklearn.externals import joblib
import socket
import sys
import time
import sys

if sys.argv[1] == '1':
	train = True
else:
	train = False

if not train:
    server_addr = ('localhost', 2300)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    if s < 0:
        print("Error creating socket")
    try:
        s.connect(server_addr)
        print("Connected to %s" % repr(server_addr))
    except:
        print("ERROR: Connection to %s refused" % repr(server_addr))
        sys.exit(1)
    try:
    	model = joblib.load('../weights/ballstick_svm.h5')
        while True:
            buff = s.recv(1024)
            print(buff)
            if(buff=='CONN_CHCK'):
                nsent = s.send('ALL_SET')
            else:
                data = buff.split(',')
                for i in range(0,len(data)):
                	data[i] = float(data[i])
                data = np.array([data],np.float64)
                pred = model.predict(data)
                nsent = s.send(str(pred[0]))
    finally:
        print("Closing socket")
        s.close()
else:
	f = open('../benchmarks/datasetB.txt','r+')
	data_x=[];data_y=[];
	while True:
		samp = f.readline()
		if(samp==''):
			break
		samp=samp.split(',')
		samp = [samp[0],samp[1],samp[2],samp[3],samp[6]]
		for i in range(0,len(samp)):
			samp[i] = float(samp[i])
			data_x.append(samp[0:len(samp)-1])
			data_y.append([samp[len(samp)-1]])
	f.close()

	data_x = np.array(data_x,np.float64)
	data_y = np.array(data_y,np.float64)
	data_x.reshape(1,-1)
	data_y.reshape(-1,1)

	sc_x = StandardScaler()
	sc_y = StandardScaler()
	data_x = sc_x.fit_transform(data_x)
	data_y = sc_y.fit_transform(data_y)

	trainx=[];trainy=[];testx=[];testy=[];
	for i in range(len(data_x)):
		if np.random.choice([True, False], p=[0.8, 0.2]):
			trainx.append(data_x[i])
			trainy.append(data_y[i])
		else:
			testx.append(data_x[i])
			testy.append(data_y[i])

	trainx = np.array(trainx,np.float64)
	trainy = np.array(trainy,np.float64)
	testx = np.array(testx,np.float64)
	testy = np.array(testy,np.float64)
	
	model = SVR(kernel='rbf')
	model.fit(trainx,trainy)
	print(model.score(testx,testy))
	joblib.dump(model, '../weights/ballstick_svm.h5')
	# model = joblib.load('../weights/ballstick_svm.h5')
	# pred = model.predict(testx[0:1000])
	# X_grid = np.arange(0, len(testy[0:1000]), 1)
	# X_grid = X_grid.reshape((len(X_grid), 1))
	# plt.plot(X_grid, sc_y.inverse_transform(testy[0:1000]), color = 'red')
	# plt.plot(X_grid, sc_y.inverse_transform(pred[0:1000]), color = 'blue')
	# plt.title('Result')
	# plt.show()
	# for i in range(0,len(testy)):
	# 	pred = model.predict([testx[i]])
	# 	print(sc_y.inverse_transform([testy[i]]),sc_y.inverse_transform([pred]))