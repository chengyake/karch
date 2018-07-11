#! /usr/bin/python3

import random
import argparse
import numpy as np
import tensorflow as tf

from tensorflow.examples.tutorials.mnist import input_data


class DL:

    layer_num = 4  #include io layer
    layer_nodes = 512 #must > n_inputs

    n_inputs = int(784/2)
    n_classes = 10 
    
    def __init__(self):
        self.mnist = input_data.read_data_sets("./data", one_hot=True)
        print(self.layer_num, self.layer_nodes)

    def fc_layer(self,name,x, w):
        x1=tf.matmul(x, w)
        return tf.nn.leaky_relu(x1)
    
  
    def alex_net(self, inputs, genes):
        
        layer = self.fc_layer("layeri", inputs, genes[0,:self.n_inputs,:])
        for i in range(self.layer_num-2):
            layer = self.fc_layer("layer%d"%i, layer, genes[i+1])
    
        out = self.fc_layer("layero", layer, genes[i+2,:,:self.n_classes])
        return out


    def train(self, genes):
        self.x = tf.placeholder(tf.float32, [None, self.n_inputs])
        self.y = tf.placeholder(tf.float32, [None, self.n_classes])

        pred = self.alex_net(self.x, genes)
        cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=self.y, logits=pred))

        correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(self.y,1))
        accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))
        
        init = tf.global_variables_initializer()
        with tf.Session() as sess:
            sess.run(init)
            score = sess.run(accuracy, feed_dict={self.x: self.mnist.test.images[:,::2], self.y: self.mnist.test.labels})

        tf.reset_default_graph()
        return score
    


if __name__ == '__main__':
    
    gdrl = DL()
    from main import genetic_algo
    ga = genetic_algo()
    print("------------train--------------")
    s = gdrl.train(ga.genes[0])
    print(s)







    
