from keras.models import load_model
import numpy as np

model=load_model('/home/prabin/git/BallStick/benchmarks/4641ththth2.h5')
model.summary()

def getTrainableWeights(mdl):
    weights = []
    for layer in mdl.layers:
        for i in range(len(layer.get_weights())):
            for j in layer.get_weights()[i]:
                if(i==0):
                    for k in j:
                        weights.append(k)
                else:
                    weights.append(j)
    return weights

old_wt = getTrainableWeights(model)

def setTrainedWeights(mdl,weights):
    cnt=0
    for i in range(len(mdl.layers)):
        wt = []
        lr_shp = mdl.layers[i].get_weights()
        print lr_shp
        arr = []
        for j in range(len(lr_shp[0])):
            val = []
            for k in range(len(lr_shp[0][j])):
                val.append(weights[cnt])
                cnt+=1
            arr.append(val)
        wt.append(np.array(arr,dtype=np.float32))
        arr = []
        for j in range(len(lr_shp[1])):
            arr.append(weights[cnt])
            cnt+=1
        wt.append(np.array(arr,dtype=np.float32))
        print wt
        mdl.layers[i].set_weights(wt)
    return mdl

new_model = setTrainedWeights(model,old_wt)
new_wt = getTrainableWeights(new_model)

print(model.predict(np.array([[1,1,1,1]])),new_model.predict(np.array([[1,1,1,1]])))