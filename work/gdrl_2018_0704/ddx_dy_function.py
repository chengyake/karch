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
import numpy as np
import tensorflow as tf

mnist=0
if mnist == 1:
    from mnist.data import DATA
else:
    from dydata.data import DATA

train_idx=-1

fbl = 100
class DL:
    batch_size = 1000
    learning_rate = 0.1
    training_iters = batch_size*400
    
    def __init__(self):
        self.data = DATA()
    

    def ddx_net(self, inputs):#in [? 2]    gene[2, 101]
        inputs = tf.round(inputs*fbl)
        self.genes=tf.Variable(tf.random_normal((self.data.num_input, fbl+1,)), trainable=True)




        l = self.genes[:, 0]*tf.cast(tf.equal(inputs, 0), dtype=tf.float32)
        for i in range(1, fbl+1):
            l += self.genes[:, i]*tf.cast(tf.equal(inputs, i), dtype=tf.float32)
        


        #mul = l[:,0]
        #for i in range(1, self.data.num_input):
        #    mul = mul*l[:,i]
    
        #mul = tf.pow(2.71828, tf.reduce_mean(tf.log(tf.abs(l)+0.00001), axis=-1))

        mul = tf.reduce_sum(l, axis=-1)

        return mul


    def train(self):

        if mnist == 1:
            self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
            self.y = tf.placeholder(tf.float32, [None, self.data.num_class])
            pred = self.ddx_net(self.x)
            cost = tf.reduce_mean(tf.abs(pred-tf.cast(tf.argmax(self.y,-1), dtype=tf.float32)))
            correct_pred = tf.equal(tf.round(pred), tf.cast(tf.argmax(self.y,1), dtype=tf.float32))
            acc = tf.reduce_mean(tf.cast(correct_pred, tf.float32))
        else:
            self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
            self.y = tf.placeholder(tf.float32, [None, ])
            pred = self.ddx_net(self.x)
            cost = tf.reduce_mean(tf.abs(pred-self.y))
            acc = cost
        #cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=self.y, logits=pred))
        #correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(self.y,1))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)


        tf.summary.scalar("loss", cost)


        init = tf.global_variables_initializer()
        max_score=10000.0;
        with tf.Session() as sess:
            sess.run(init)
            merge = tf.summary.merge_all()
            train_writer = tf.summary.FileWriter("logs/train", sess.graph)
            test_writer = tf.summary.FileWriter("logs/test")


            step = 1
            #while step * self.batch_size < self.training_iters:
            while True:
                batch_x, batch_y = self.data.get_train_batch(self.batch_size)
                score1, _, m= sess.run([cost, optimizer, merge], feed_dict={self.x: batch_x, self.y: batch_y})
                train_writer.add_summary(m, step)

                batch_x, batch_y = self.data.get_score_batch()
                ac, score2, m, pv= sess.run([acc, cost, merge, pred], feed_dict={self.x: batch_x, self.y: batch_y})
                test_writer.add_summary(m, step)
                
                step += 1
                if score2 < max_score:
                    max_score = score2

                print("yakelog:%.4f\t%.4f\t%.4f\t%.4f" % (score1, score2, max_score, ac))

                
                if max_score<0.000726:
                    print(batch_x.shape, batch_y.shape, pv.shape)

                    print("x=[")
                    for i in range(1000):
                        print(batch_x[i][0],",")
                    print("]")

                    print("y=[")
                    for i in range(1000):
                        print(batch_x[i][1],",")
                    print("]")

                    print("c=[")
                    for i in range(1000):
                        print(batch_y[i],",")
                    print("]")


                    print("z=[")
                    for i in range(1000):
                        print(pv[i],",")
                    print("]")

                    exit()




        tf.reset_default_graph()
        return max_score
    



if __name__ == '__main__':
    
    dl = DL()
    dl.train()


