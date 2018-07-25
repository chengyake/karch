#! /usr/bin/python3
'''
layer_1          ooo
                
                w1+b1

layer_2      nnnnnnnnnnnn

                w2+b2

layer_3        iiiiiii

'''
import os
import time
import random
import argparse
import numpy as np
import tensorflow as tf

from mnist.data import DATA

fbl=10.0
jishu = 8
class DL:

    batch_size = 640
    learning_rate = 0.1
    training_iters = batch_size*1000
    
    
    def __init__(self):
        self.data = DATA()

#    def tl_net(self, inputs):
#        arg={}
#        for i in range(jishu):
#            arg[i]=tf.Variable(tf.random_normal((784,)), trainable=True)
#            
#        layer = tf.add(arg[0], arg[1]*tf.pow(inputs, 1))
#        for i in range(2, jishu):
#            layer = tf.add(layer, arg[i]*tf.pow(inputs, i))
#
#        layer = tf.nn.sigmoid(layer)
#
#        out = tf.reduce_sum(-tf.log(layer), axis=-1)
#
#
#        return out

    def tl_net(self, inputs):
        
        a0=tf.Variable(tf.random_normal((784,)), trainable=True)
        a1=tf.Variable(tf.random_normal((784,)), trainable=True)
        a2=tf.Variable(tf.random_normal((784,)), trainable=True)
        a3=tf.Variable(tf.random_normal((784,)), trainable=True)
        a4=tf.Variable(tf.random_normal((784,)), trainable=True)
        a5=tf.Variable(tf.random_normal((784,)), trainable=True)
        a6=tf.Variable(tf.random_normal((784,)), trainable=True)
        a7=tf.Variable(tf.random_normal((784,)), trainable=True)
        a8=tf.Variable(tf.random_normal((784,)), trainable=True)
        a9=tf.Variable(tf.random_normal((784,)), trainable=True)

        l1 = tf.add(a0, tf.multiply(a1, inputs))
        l2 = tf.add(l1, tf.multiply(a2, tf.pow(inputs, 2)))
        l3 = tf.add(l2, tf.multiply(a3, tf.pow(inputs, 3)))
        l4 = tf.add(l3, tf.multiply(a4, tf.pow(inputs, 4)))
        l5 = tf.add(l4, tf.multiply(a5, tf.pow(inputs, 5)))
        l6 = tf.add(l5, tf.multiply(a6, tf.pow(inputs, 6)))
        l7 = tf.add(l6, tf.multiply(a7, tf.pow(inputs, 7)))
        l8 = tf.add(l7, tf.multiply(a8, tf.pow(inputs, 8)))
        l9 = tf.add(l8, tf.multiply(a9, tf.pow(inputs, 9)))
    
        #out = tf.reduce_sum(tf.log(tf.maximum(0.00000001, l9))-tf.log(tf.maximum(0.00000001, -l9)), axis=-1)

        ll = tf.nn.sigmoid(l9)+0.001
        out = tf.reduce_sum(-tf.log(ll), axis=-1)
        #out = tf.reduce_sum(l9, axis=-1)

        return out

    def bn_layer(self, inputs, is_training=True, name='BatchNorm', moving_decay=0.9, eps=1e-5):
        shape = inputs.shape
        assert len(shape) in [2,4]
        param_shape = shape[-1]

        gamma = tf.Variable(tf.ones(param_shape), name=name+'gamma')
        beta  = tf.Variable(tf.zeros(param_shape), name=name+'beta')
        mean  = tf.Variable(tf.ones(param_shape), trainable=False, name=name+'mean')
        var   = tf.Variable(tf.ones(param_shape), trainable=False, name=name+'var')

        if is_training == True:
            batch_mean, batch_var = tf.nn.moments(inputs,[0],name='moments')
            mean = tf.assign(mean, batch_mean)
            var = tf.assign(var, batch_var)
            return tf.nn.batch_normalization(inputs,batch_mean+mean*1e-8,batch_var+var*1e-8,gamma,beta,eps, name=name+"bn")
        else:
            return tf.nn.batch_normalization(inputs,mean,var,gamma,beta,eps)

    def fc_layer(self,name,x, w, b):
        x1=tf.matmul(x, w, name=name) + b
        x2=self.bn_layer(x1, name=name)
        return tf.nn.leaky_relu(x2, name=name)

    def fc_net(self, inputs):

        mid_num1=1000
        mid_num2=1000

        w1 = tf.Variable(tf.random_normal([784, mid_num1]))
        b1 = tf.Variable(tf.zeros([mid_num1]) + 0.000001)
        x1 = self.fc_layer("layer1", inputs, w1, b1)

        w2 = tf.Variable(tf.random_normal([mid_num1, mid_num2]))
        b2 = tf.Variable(tf.zeros([mid_num2]) + 0.000001)
        x2 = self.fc_layer("layer2", x1, w2, b2)

        w3 = tf.Variable(tf.random_normal([mid_num2, 1]))
        b3 = tf.Variable(tf.zeros([1]) + 0.000001)
        x3 = self.fc_layer("layer3", x2, w3, b3)
        return x3




    def fly_net(self, inputs):
        genes={}
        for i in range(100):
            genes[i]=tf.Variable(tf.random_normal((self.data.num_input,2))*10.0, trainable=True)

        layer = genes[0][:,0]*tf.sin(0.1*inputs+genes[0][:,1])
        for i in range(1, 100):
            layer = tf.add(layer, genes[i][:,0]*tf.sin(0.1*i+0.1*inputs+genes[i][:,1]))

        w3 = tf.Variable(tf.random_normal([self.data.num_input, 1]))
        b3 = tf.Variable(tf.zeros([1]) + 0.000001)
        x3 = self.fc_layer("layer3", layer, w3, b3)


        #layer = tf.nn.sigmoid(layer)+0.000001
        #out = tf.reduce_sum(-tf.log(layer), axis=-1)

        #out = tf.reduce_sum(layer, axis=-1)

        return x3

    
    def ddx_net(self, inputs):

        arg=tf.Variable(tf.random_normal((self.data.num_input,int(fbl+1))), trainable=True)
        li = tf.round(inputs*fbl)
        l = arg[:,0]*tf.cast(tf.equal(li, 0), dtype=tf.float32)
        for i in range(1, int(fbl+1)):
            l += arg[:,i]*tf.cast(tf.equal(li, i), dtype=tf.float32)


        layer = tf.nn.sigmoid(l)+0.01
        out = tf.reduce_sum(-tf.log(layer), axis=-1)

        #out = tf.reduce_sum(l, axis=-1)

        return out






    def train(self):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        #pred = self.tl_net(self.x-0.5)
        #pred = self.fc_net(self.x-0.5)
        #pred = self.fly_net(self.x-0.5)
        pred = self.ddx_net(self.x)
        cost = tf.reduce_mean(tf.abs(pred-tf.cast(tf.argmax(self.y,-1), dtype=tf.float32)))
        #cost = tf.reduce_mean(tf.pow(2.7, tf.abs(pred-tf.cast(tf.argmax(self.y,-1), dtype=tf.float32))))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)
        acc_ = tf.equal(tf.cast(tf.round(pred), dtype=tf.int64), tf.argmax(self.y, -1, output_type=tf.int64))
        acc = tf.reduce_mean(tf.cast(acc_, dtype=tf.float32))

        tf.summary.scalar("cost", cost)
        tf.summary.scalar("loss", acc)
        saver = tf.train.Saver(tf.global_variables())


        init = tf.global_variables_initializer()
        max_score=0.0;
        with tf.Session() as sess:
            sess.run(init)
            merge = tf.summary.merge_all()
            train_writer = tf.summary.FileWriter("logs/train", sess.graph)
            test_writer = tf.summary.FileWriter("logs/test")

            ckpt = tf.train.get_checkpoint_state("./save")
            if ckpt and ckpt.model_checkpoint_path:
                print(ckpt.model_checkpoint_path)
                saver.restore(sess, ckpt.model_checkpoint_path)


            step = 1
            #while step * self.batch_size < self.training_iters:
            while True:
                batch_x, batch_y = self.data.get_train_batch(self.batch_size)
                score1, _, m= sess.run([acc, optimizer, merge], feed_dict={self.x: batch_x, self.y: batch_y})
                train_writer.add_summary(m, step)

                batch_x, batch_y = self.data.get_score_batch()
                score2, m, pv= sess.run([acc, merge, pred], feed_dict={self.x: batch_x, self.y: batch_y})
                test_writer.add_summary(m, step)
                #print(np.argmax(batch_y[:10], axis=-1))
                #print(np.round(pv[:10]).astype(np.int))
                #print(np.sum(np.equal(np.argmax(batch_y, axis=-1), np.round(pv).astype(np.int)))/batch_y.shape[0])

                step += 1
                if score2 > max_score:
                    max_score = score2
                    saver.save(sess, "save/model.ckpt", global_step=step)
                print("yakelog:%.4f\t%.4f\t%.4f" % (score1, score2, max_score))


        tf.reset_default_graph()

        return max_score
    



if __name__ == '__main__':
    os.system("rm logs -rf")
    dl = DL()
    test_mode=1
    if test_mode == 1:
        dl.train()

    elif test_mode == 2:
        dl.random_test()



