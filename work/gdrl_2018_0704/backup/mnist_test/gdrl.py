#! /usr/bin/python3
'''
A Convolutional Network implementation example using TensorFlow library.
This example is using the MNIST database of handwritten digits
'''
import random
import argparse
import numpy as np
import tensorflow as tf

from tensorflow.examples.tutorials.mnist import input_data


class DL:

    learning_rate = 0.001
    batch_size = 4096
    layer_num = 4  #include io layer
    layer_nodes = 512 #must > n_inputs
    con_num = 2048

    n_inputs = int(784/2)
    n_classes = 10 
    training_iters = 1000*1000
    
    def __init__(self):
        self.mnist = input_data.read_data_sets("./data", one_hot=True)
        print(self.learning_rate, self.batch_size, self.layer_num, self.layer_nodes)

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
        
        init = tf.global_variables_initializer()
        with tf.Session() as sess:
            sess.run(init)
            step = 1
            while step * self.batch_size < self.training_iters:
                batch_x, batch_y = self.mnist.train.next_batch(self.batch_size)
                sess.run(optimizer, feed_dict={self.x: batch_x[:,::2], self.y: batch_y})
                step += 1
            score = sess.run(accuracy, feed_dict={self.x: self.mnist.test.images[:,::2], self.y: self.mnist.test.labels})

        tf.reset_default_graph()
        return score
    


if __name__ == '__main__':
    
    gdrl = DL()
    from main import genetic_algo
    ga = genetic_algo()

    #genes = np.random.randint(1, (gdrl.layer_num*gdrl.layer_nodes*gdrl.layer_nodes)/gdrl.con_num, size=(gdrl.layer_num, gdrl.layer_nodes, gdrl.layer_nodes), dtype="int8")
    #genes[genes>1]=0
    #print(np.sum(genes))
    #ga.adjust_connections(genes, ga.con_num)
    
    genes = ga.get_best_genes()

    print(np.sum(genes))

    score = gdrl.train(genes)
    print ("yakelog1:", score)

    score = gdrl.train(genes)
    print ("yakelog2:", score)

    score = gdrl.train(genes)
    print ("yakelog3:", score)
