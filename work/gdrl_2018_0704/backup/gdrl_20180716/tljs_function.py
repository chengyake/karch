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

from fdata.data import DATA

#
#   c*sin(ax+b)
#


jishu = 100
class DL:

    batch_size = 10000
    learning_rate = 0.001
    training_iters = batch_size*1000
    arg={}    
    
    def __init__(self):
        self.data = DATA()


    def tl_net(self, inputs):
        for i in range(jishu):
            if i < 8:
                self.arg[i]=tf.Variable(tf.random_normal((self.data.num_input,2)), trainable=True)
            else:
                self.arg[i]=tf.Variable(tf.zeros((self.data.num_input,2)), trainable=False)


        layer = self.arg[0][:,0]*tf.sin(0.1*inputs+self.arg[0][:,1])
        for i in range(1, jishu):
            layer = tf.add(layer, self.arg[i][:,0]*tf.sin((0.1*i+0.1)*inputs+self.arg[i][:,1]))
        return layer


        #out = tf.reduce_sum(tf.log(tf.maximum(0.00000001, l9))-tf.log(tf.maximum(0.00000001, -l9)), axis=-1)

        #ll = tf.nn.sigmoid(layer)
        #out = tf.reduce_sum(tf.log(ll), axis=-1)
        #out = tf.log(ll)
        #out = ll

        #out = tf.reduce_sum(l9, axis=-1)


    def train(self):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        pred = self.tl_net(self.x)
        cost = tf.reduce_mean(tf.pow(2.7, tf.abs(self.y-pred)))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)

        tf.summary.scalar("loss", cost)
        saver = tf.train.Saver(tf.global_variables())


        init = tf.global_variables_initializer()
        min_score=10000.0;
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
                score1, _, m= sess.run([cost, optimizer, merge], feed_dict={self.x: batch_x, self.y: batch_y})
                train_writer.add_summary(m, step)

                batch_x, batch_y = self.data.get_score_batch()
                score2, m, pv= sess.run([cost, merge, pred], feed_dict={self.x: batch_x, self.y: batch_y})
                test_writer.add_summary(m, step)

                step += 1
                if score2 < min_score:
                    min_score = score2
                    saver.save(sess, "save/model.ckpt", global_step=step)
                    
                if False and step > 100:
                #if True and step > 100:
                    print(batch_x.shape)
                    for i in range(len(batch_x)):
                        print(batch_x[i][0], pv[i][0], batch_y[i][0])
                    exit()


                print("yakelog:%.4f\t%.4f\t%.4f" % (score1, score2, min_score))
                #print("yakelog:",self.arg[0].eval())
                #print("yakelog:",self.arg[1].eval())
                #print("yakelog:",self.arg[2].eval())
                #print("yakelog:",self.arg[3].eval())


        tf.reset_default_graph()

        return min_score
    



if __name__ == '__main__':
    os.system("rm logs -rf")
    dl = DL()
    test_mode=1
    if test_mode == 1:
        dl.train()

    elif test_mode == 2:
        dl.random_test()



