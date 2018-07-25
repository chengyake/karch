import os
import random
import numpy as np

#2005-01-01   2018-07-01
#1287 s
#3278 days 3277 close rate

class DATA:
    num_input=1340 #1287 + 53
    num_class=2
    
    num_train=2800
    num_test=400
    num_val=77
    offset = 0.0

    def __init__(self):
        self.get_jq_data()

    def get_jq_data(self):
        if os.path.exists('/home/yake/project/karch/work/gdrl_2018_0704/gdrl_20180716/sdata/closs_rate.npz'):
            self.jqdata = np.load("/home/yake/project/karch/work/gdrl_2018_0704/gdrl_20180716/sdata/closs_rate.npz")['closs_rate']
            #self.jqdata = self.jqdata[::2]
            print("Load close rate data. shape:", self.jqdata.shape)
        else:
            print("No close_rate.npz exists")
            exit()
        return

    def get_train_batch(self, size):
        choice = np.array(random.sample(range(53, self.num_train), size))
        x = self.jqdata[:,choice]
        _x = np.zeros((size, 53))
        for i in range(size):
            _x[i] = self.jqdata[0, choice[i]-53:choice[i]]

        y = self.jqdata[0,choice+1]
        y1= np.where(y>=self.offset, 1.0, 0.0)
        y2= np.where(y<self.offset, 1.0, 0.0)

        return np.hstack((np.transpose(x), _x)),  y, np.transpose(np.vstack((y2,y1)))


    def get_score_batch(self, a=num_train,b=num_train+num_test):
        choice = range(a,b)
        size = b-a
        x = self.jqdata[:,choice]
        _x = np.zeros((size, 53))
        for i in range(size):
            _x[i] = self.jqdata[0, choice[i]-53:choice[i]]

        y = self.jqdata[0,range(a+1, b+1)]
        y1= np.where(y>=self.offset, 1.0, 0.0)
        y2= np.where(y<self.offset, 1.0, 0.0)

        return np.hstack((np.transpose(x), _x)), y, np.transpose(np.vstack((y2,y1)))


    def get_val_batch(self, a=num_train+num_test,b=num_train+num_test+num_val-1):
        choice = range(a,b)
        size = b-a
        x = self.jqdata[:,choice]
        _x = np.zeros((size, 53))
        for i in range(size):
            _x[i] = self.jqdata[0, choice[i]-53:choice[i]]

        y = self.jqdata[0,range(a+1, b+1)]
        y1= np.where(y>=self.offset, 1.0, 0.0)
        y2= np.where(y<self.offset, 1.0, 0.0)

        return np.hstack((np.transpose(x), _x)), y, np.transpose(np.vstack((y2,y1)))




if __name__ == '__main__':
    
    data = DATA()
    batch_x, batch_y, batch_z = data.get_train_batch(2800-53)
    print(batch_x.shape, batch_y.shape, batch_z.shape)
    batch_x, batch_y, batch_z = data.get_score_batch()
    print(batch_x.shape, batch_y.shape, batch_z.shape)
    batch_x, batch_y, batch_z = data.get_val_batch()
    print(batch_x.shape, batch_y.shape, batch_z.shape)








