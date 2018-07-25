#! /usr/bin/python3
'''
A Convolutional Network implementation example using TensorFlow library.
This example is using the MNIST database of handwritten digits
'''
import time
import random
import argparse
import numpy as np
import tensorflow as tf
from jqdatasdk import *


class DL:

    learning_rate = 0.0001
    batch_size = 640
    layer_num = 6  #include io layer
    layer_nodes = 2048 #must > n_inputs
    con_num = 32000

    n_inputs = (1287+53)
    n_classes = 3   #first pingan bank
    training_iters = 1000*64*4
    
    def __init__(self):
        self.get_jq_data()

    def get_jq_data(self):
        #2005-01-01   2018-07-01
        #1287 stocks
        #3278 days 3277 close rate
        #3000 for train; 276 for test
        self.num_train=3000
        self.jqdata = np.load("closs_rate.npz")['closs_rate']
        return
    def next_batch(self, size):
        choice = np.random.randint(53, self.num_train, size=size)
        x = self.jqdata[:,choice]
        _x = np.zeros((size, 53))
        for i in range(size):
            _x[i] = self.jqdata[0, choice[i]-53:choice[i]]

        y = self.jqdata[0,choice+1]
        y = np.expand_dims(y, axis=0)
        y1= np.where(y>0.008, 1.0, 0.0)
        y21= np.where(y>-0.008, 1.0, 0.0)
        y22= np.where(y<0.008, 1.0, 0.0)
        y2 = y21*y22
        y3= np.where(y<-0.008, 1.0, 0.0)

        return np.hstack((np.transpose(x), _x)), np.transpose(np.vstack((y1,y2,y3)))

    def val_batch(self):
        x = self.jqdata[:,self.num_train:-1]
        size = x.shape[1]
        _x = np.zeros((size, 53))
        for i in range(size):
            _x[i] = self.jqdata[0, self.num_train+i-53:self.num_train+i]
        y = self.jqdata[0,self.num_train+1:]
        y = np.expand_dims(y, axis=0)
        y1= np.where(y>0.008, 1.0, 0.0)
        y21= np.where(y>-0.008, 1.0, 0.0)
        y22= np.where(y<0.008, 1.0, 0.0)
        y2 = y21*y22
        y3= np.where(y<-0.008, 1.0, 0.0)


        return np.hstack((np.transpose(x), _x)), np.transpose(np.vstack((y1,y2,y3)))


    def bn_layer(self, inputs, is_training=True, name='BatchNorm', moving_decay=0.9, eps=1e-5):
        shape = inputs.shape
        assert len(shape) in [2,4]
        param_shape = shape[-1]
    
        gamma = tf.Variable(tf.ones(param_shape), name='gamma')
        beta  = tf.Variable(tf.zeros(param_shape), name='beta')
        mean  = tf.Variable(tf.ones(param_shape), trainable=False, name='mean')
        var   = tf.Variable(tf.ones(param_shape), trainable=False, name='var')
    
        if is_training == True:
            batch_mean, batch_var = tf.nn.moments(inputs,[0],name='moments')
            mean = tf.assign(mean, batch_mean)
            var = tf.assign(var, batch_var)
            return tf.nn.batch_normalization(inputs,batch_mean+mean*1e-8,batch_var+var*1e-8,gamma,beta,eps)
        else:
            return tf.nn.batch_normalization(inputs,mean,var,gamma,beta,eps)
    

    def fc_layer(self,name,x, w, b):
        x1=tf.matmul(x, w) + b
        x2=self.bn_layer(x1)
        return tf.nn.leaky_relu(x2)
    
    def _init_weights(self):
        self.weights = {
            'in': tf.Variable(tf.random_normal([self.n_inputs, self.layer_nodes])),
            'out':tf.Variable(tf.random_normal([self.layer_nodes, self.n_classes])),
        }
        for i in range(self.layer_num-2):
            self.weights[i]=tf.Variable(tf.random_normal([self.layer_nodes, self.layer_nodes]))

        self.biases = {
            'in': tf.Variable(tf.random_normal([self.layer_nodes])),
            'out':tf.Variable(tf.random_normal([self.n_classes])),
        }
        for i in range(self.layer_num-2):
            self.biases[i]=tf.Variable(tf.random_normal([self.layer_nodes]))


    def alex_net(self, inputs, genes):
        
        self._init_weights()
        layer = self.fc_layer("layeri", inputs, self.weights['in']*genes[0,:self.n_inputs,:], self.biases['in'])
        for i in range(self.layer_num-2):
            layer = self.fc_layer("layer%d"%i, layer, self.weights[i]*genes[i+1], self.biases[i])
    
        out = self.fc_layer("layero", layer, self.weights['out']*genes[i+2,:,:self.n_classes],self.biases['out'])
        return out


    def train(self, genes):
        self.x = tf.placeholder(tf.float32, [None, self.n_inputs])
        self.y = tf.placeholder(tf.float32, [None, self.n_classes])

        pred = self.alex_net(self.x, genes)
        cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=self.y, logits=pred))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)
        correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(self.y,1))
        accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))

        #tf.summary.scalar("loss", cost)

        #pred = self.alex_net(self.x, genes)
        ##cost = tf.reduce_mean(tf.abs(self.y-pred)/(tf.abs(self.y)+0.001))
        #cost = tf.reduce_mean(tf.abs(self.y-pred))
        #tf.summary.scalar("loss", cost)
        #optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)
        
        init = tf.global_variables_initializer()
        max_score=0;
        with tf.Session() as sess:
            sess.run(init)
            #merge = tf.summary.merge_all()
            #train_writer = tf.summary.FileWriter("logs/train", sess.graph)
            #test_writer = tf.summary.FileWriter("logs/test")


            step = 1
            while step * self.batch_size < self.training_iters:
            #while True:
                batch_x, batch_y = self.next_batch(self.batch_size)
                score1, _= sess.run([accuracy, optimizer], feed_dict={self.x: batch_x, self.y: batch_y})
                #train_writer.add_summary(m, step)
                batch_x, batch_y = self.val_batch()
                score2 = sess.run(accuracy, feed_dict={self.x: batch_x, self.y: batch_y})
                #test_writer.add_summary(m, step)
                step += 1
                if score2 > max_score:
                    max_score = score2
                #print("yakelog:\t%.4f\t%.4f\t%.4f" % (score1, score2, max_score))

        tf.reset_default_graph()

        return max_score
    


if __name__ == '__main__':
    
    gdrl = DL()
    from main import genetic_algo
    ga = genetic_algo()
    #gdata = np.ones_like(ga.genes[0])
    #gdata = np.random.randint(1, 800, size=ga.genes[0].shape)
    #gdata[gdata>1]=0
    #print(gdata.sum())
    gdrl.train(ga.genes[0])
