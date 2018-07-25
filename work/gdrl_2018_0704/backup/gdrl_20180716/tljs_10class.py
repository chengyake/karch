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


jishu = 16
class DL:

    batch_size = 640*2
    learning_rate = 0.01
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

    def _tl_net(self, inputs):
        
        self.a0=tf.Variable(tf.random_normal((784,)), trainable=True)
        a1=tf.Variable(tf.random_normal((784,)), trainable=True)
        a2=tf.Variable(tf.random_normal((784,)), trainable=True)
        a3=tf.Variable(tf.random_normal((784,)), trainable=True)
        a4=tf.Variable(tf.random_normal((784,)), trainable=True)
        a5=tf.Variable(tf.random_normal((784,)), trainable=True)
        a6=tf.Variable(tf.random_normal((784,)), trainable=True)
        a7=tf.Variable(tf.random_normal((784,)), trainable=True)
        a8=tf.Variable(tf.random_normal((784,)), trainable=True)
        a9=tf.Variable(tf.random_normal((784,)), trainable=True)

        l1 = tf.add(self.a0, tf.multiply(a1, inputs))
        l2 = tf.add(l1, tf.multiply(a2, tf.pow(inputs, 2)))
        l3 = tf.add(l2, tf.multiply(a3, tf.pow(inputs, 3)))
        l4 = tf.add(l3, tf.multiply(a4, tf.pow(inputs, 4)))
        l5 = tf.add(l4, tf.multiply(a5, tf.pow(inputs, 5)))
        l6 = tf.add(l5, tf.multiply(a6, tf.pow(inputs, 6)))
        l7 = tf.add(l6, tf.multiply(a7, tf.pow(inputs, 7)))
        l8 = tf.add(l7, tf.multiply(a8, tf.pow(inputs, 8)))
        l9 = tf.add(l8, tf.multiply(a9, tf.pow(inputs, 9)))
    

        ll = tf.nn.sigmoid(l9)
        out = tf.reduce_sum(tf.log(ll), axis=-1)
        out = tf.expand_dims(out, axis=-1)
        return out
    
    def tl_net(self, inputs):
        tl=[]
        for i in range(10):
            tl.append(self._tl_net(inputs))
        
        ret = tf.concat(tl, axis=1)
        return ret 

    def train(self):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        pred = self.tl_net(self.x-0.5)
        cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=self.y, logits=pred))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)
        acc = tf.reduce_mean(tf.cast(tf.equal(tf.argmax(pred, -1), tf.argmax(self.y,-1)), dtype=tf.float32))

        tf.summary.scalar("loss", cost)
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
                print(np.argmax(batch_y[:10], axis=-1))
                print(np.argmax(pv[:10], axis=-1))

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



