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

from fdata.data import DATA


jishu = 4
class DL:
    batch_size = 10000
    learning_rate = 0.1
    training_iters = batch_size*500
    
    genes=[]
    
    def __init__(self):
        self.data = DATA()
    

    def random_genes(self):
        self.genes=[]
        for i in range(jishu):
           self.genes.append(tf.Variable(tf.random_normal((self.data.num_input,)), trainable=True))
        print("Random genes success")
        return

    def update_genes(self, genes):
        self.genes=[]
        for i in range(jishu):
            self.genes.append(tf.Variable(genes[i], trainable=True))
        print("Load genes success")
        return True




    def load_genes(self):
        if os.path.exists("genes.npz"):
            genes_v = np.load("genes.npz")["genes"]
            self.genes=[]
            for i in range(jishu):
                self.genes.append(tf.Variable(genes_v[i], trainable=True))
            print("Load genes success")
            return True
        return False


    def save_genes(self):
        genes_v=[]
        for i in range(jishu):
            genes_v.append(self.genes[i].eval())
        np.savez("genes.npz", genes=genes_v)
        print("Store genes success")
        return



    def tl_net(self, inputs):

        layer = tf.add(self.genes[0], tf.multiply(self.genes[1], inputs))
        for i in range(2, jishu):
            layer = tf.add(layer, tf.multiply(self.genes[i], tf.pow(inputs, i)))


        #out = tf.reduce_sum(tf.log(tf.maximum(0.00000001, l9))-tf.log(tf.maximum(0.00000001, -l9)), axis=-1)

        ll = tf.nn.sigmoid(layer)
        #out = tf.reduce_sum(tf.log(ll), axis=-1)
        #out = tf.log(ll)
        out = ll

        #out = tf.reduce_sum(l9, axis=-1)

        return out

    def train(self):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        #if self.load_genes()==False:
        #    self.random_genes()
        pred = self.tl_net(self.x)
        cost = tf.reduce_mean(tf.abs(self.y-pred))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)

        tf.summary.scalar("loss", cost)


        init = tf.global_variables_initializer()
        min_score=100.0;
        with tf.Session() as sess:
            sess.run(init)
            merge = tf.summary.merge_all()
            train_writer = tf.summary.FileWriter("logs/train", sess.graph)
            test_writer = tf.summary.FileWriter("logs/test")


            step = 1
            while step * self.batch_size < self.training_iters:
            #while True:
                batch_x, batch_y = self.data.get_train_batch(self.batch_size)
                score1, _, m= sess.run([cost, optimizer, merge], feed_dict={self.x: batch_x, self.y: batch_y})
                train_writer.add_summary(m, step)

                batch_x, batch_y = self.data.get_score_batch()
                score2, m, pv= sess.run([cost, merge, pred], feed_dict={self.x: batch_x, self.y: batch_y})
                test_writer.add_summary(m, step)

                step += 1
                if score2 < min_score:
                    min_score = score2
                    


                print("yakelog:%.4f\t%.4f\t%.4f" % (score1, score2, min_score))


        tf.reset_default_graph()
        print("-----------------_")
        return min_score
    



if __name__ == '__main__':
    
    div=4
    dl = DL()
    #test_mode=1
    #if test_mode == 1:
    #    dl.train()

    #elif test_mode == 2:
    #    dl.random_test()

    min_s = 1000.0
    ss = 0

    for i in range(-100,101, 50):
        for j in range(-100,101, 50):
            for k in range(-100,101, 50):
                for l in range(-100,101, 50):
                    genes=[np.array(float(i)/100, dtype=np.float32),
                           np.array(float(j)/10 , dtype=np.float32),
                           np.array(float(k)/1  , dtype=np.float32),
                           np.array(float(l)/0.1, dtype=np.float32)]

                    dl.update_genes(genes)         
                    ss = dl.train()
                    if ss < min_s:
                        min_s=ss
                    print("asdfasdf",i,j,k,l,ss,min_s)


    #dl.save_genes()
