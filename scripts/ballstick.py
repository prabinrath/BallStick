import numpy as np
import keras
from keras.models import Sequential,load_model
from keras.layers import Dense,Activation
from keras.optimizers import Adam,SGD
from keras.metrics import categorical_crossentropy
from keras.utils.generic_utils import get_custom_objects
import matplotlib.pyplot as plt
from sklearn.metrics import accuracy_score

f = open('dataset.txt','r+')
trainx=[];trainy=[];testx=[];testy=[];
while True:
	samp = f.readline()
	if(samp==''):
		break
	samp=samp.split(',')
	for i in range(0,len(samp)):
		samp[i] = float(samp[i])
	if np.random.choice([True, False], p=[0.7, 0.3]):
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
max_rv=[200,300,90,90]
min_rv=[0,0,-90,-90]
for i in range(4):
    trainx[:,i]=min_v[i]+(((trainx[:,i]-min_rv[i])/(max_rv[i]-min_rv[i]))*max_v[i])
for i in range(4):
    testx[:,i]=min_v[i]+(((testx[:,i]-min_rv[i])/(max_rv[i]-min_rv[i]))*max_v[i])
    
trainy=0+(((trainy-(-90))/((90)-(-90)))*1)

model=Sequential([Dense(5,input_shape=(4,),activation='sigmoid'),Dense(1,activation='sigmoid')])
model.summary()
model.compile(Adam(lr=0.0001,decay=0.00001),loss='mean_squared_error')
model.fit(trainx,trainy,validation_split=0.1,batch_size=100,epochs=1000,shuffle=True,verbose=2)

pred = model.predict(testx,batch_size=4,verbose=2)
for i in range(0,len(pred)):
	print testy[i],' ',-90+pred[i]*(90-(-90))
