import numpy as np


l=[]
data=np.zeros((1287, 3278))

f = open("2005_2018.bin", 'r') 


for i in range(1287):
    line = f.readline()
    l.append(line[:11])
    for j in range(3278):
        line = f.readline()
        
        if len(line)<=12:
            data[i,j]=float(1.0)
        else:
            data[i,j]=float(line[11:])
        print(data[i,j])

f.close()

rate = (data[:,1:]-data[:,:-1])/data[:,:-1]

np.savez("closs_rate.npz", closs_rate=rate)

print(l[1286], rate[1286,3276])
