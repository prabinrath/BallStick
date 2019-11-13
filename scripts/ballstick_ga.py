#!/usr/bin/env python
import numpy as np
from keras.models import load_model
import socket
import sys
import time
import sys
from ga import Population

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

def setTrainedWeights(mdl,weights):
    cnt=0
    for i in range(len(mdl.layers)):
		wt = []
		lr_shp = mdl.layers[i].get_weights()
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
		mdl.layers[i].set_weights(wt)
    return mdl

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
	pop_size = 10
	mutate_prob = 0.05
	retain = 0.1
	random_retain = 0.05
	pop = Population(pop_size=pop_size, mutate_prob=mutate_prob, retain=retain, random_retain=random_retain)
	generation = 1

	buff = s.recv(1024)
	if(buff=='CONN_CHCK'):
		pass
	else:
		print 'Connection issue!!!'
		exit()

	while True:
		fitness_vals = []
		fitness_sum = 0
		max_score = 0
		max_index = 0
		for ind in range(len(pop.individuals)):
			model = setTrainedWeights(model,pop.individuals[ind].numbers)
			nsent = s.send('ALL_SET')
			while True:
				buff = s.recv(1024)
				data = buff.split(',')
				if len(data)==2:
					print('Generation: {}, Fitness: {}'.format(generation,data[0]))
					if float(data[1])>max_score:
						max_score = float(data[1])
						max_index = ind
					fitness_vals.append(data[0])
					fitness_sum+=float(data[0])
					break
				else:
					for i in range(0,len(data)):
					    data[i] = float(data[i])
					data = np.array([data],np.float64)
					max_v=[1,1,1,1];
					min_v=[0,0,0,0];
					max_rv=[40,60,80,200]
					min_rv=[-40,-40,0,0]
					for i in range(4):
					    data[:,i]=min_v[i]+(((data[:,i]-min_rv[i])/(max_rv[i]-min_rv[i]))*(max_v[i]-min_v[i]))
					pred = model.predict(data,batch_size=4,verbose=2)
					pred = 90*pred[0]
					nsent = s.send(str(pred[0]))
		pop.grade(fitness_sum,generation)
		print sorted(fitness_vals,reverse=True)[:5]
		print('Generation: {} ----------------- Max Score: {}'.format(generation,max_score))
		if pop.done:
		    print("Finished at generation:", x, ", Population fitness:", pop.fitness_history[-1])
		    model = setTrainedWeights(model,pop.individuals[max_index].numbers)
		    model.save('/home/prabin/git/BallStick/weights/ballstick_ga_final.h5')
		    break
		if generation%20 == 0:
			bench_individuals = [x for _,x in sorted(zip(fitness_vals,pop.individuals),reverse=True)]
			model = setTrainedWeights(model,bench_individuals[0].numbers)
			model.save('/home/prabin/git/BallStick/weights/ballstick_ga'+str(generation)+'.h5')
			print "Benchmark model weights saved !!!"
		pop.evolve(fitness_vals)
		generation = generation+1
		
finally:
	print "Closing socket"
s.close()
