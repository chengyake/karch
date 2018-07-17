import numpy as np
import random
from tensorflow.examples.tutorials.mnist import input_data


class DATA:
    
    num_input=784
    num_class=10

    def __init__(self):
        self.mnist = input_data.read_data_sets("/home/yake/project/karch/work/gdrl_2018_0704/gdrl_20180716/mnist/data", one_hot=True)
        print("Train Dataset:\t",self.mnist.train.images.shape, "\t",self.mnist.train.labels.shape)
        print("Test  Dataset:\t",self.mnist.test.images.shape, "\t",self.mnist.test.labels.shape)
        print("Valid Dataset:\t",self.mnist.validation.images.shape,"\t", self.mnist.validation.labels.shape)
    
    def get_train_batch(self, size):
        idx = random.sample(range(self.mnist.train.images.shape[0]), size)
        return self.mnist.train.images[idx], self.mnist.train.labels[idx]

    def get_score_batch(self, size=10000):
        idx = random.sample(range(self.mnist.test.images.shape[0]), size)
        return self.mnist.test.images[idx], self.mnist.test.labels[idx]

    def get_val_batch(self, size=5000):
        idx = random.sample(range(self.mnist.validation.images.shape[0]), size)
        return self.mnist.validation.images[idx], self.mnist.validation.labels[idx]


if __name__ == '__main__':
    
    data = DATA()
    x, y = data.get_train_batch(55000)
    print(x.shape, y.shape)
    x, y = data.get_score_batch()
    print(x.shape, y.shape)
    x, y = data.get_val_batch()
    print(x.shape, y.shape)
