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

train_s=0
train_e=4

jishu = 100
class DL:
    batch_size = 10000
    learning_rate = 0.001
    training_iters = batch_size*1000
    
    genes={}  #tf.Var
    def __init__(self):
        self.data = DATA()
    

    def random_genes(self):
        for i in range(jishu):
            if i in range(train_s, train_e):
                self.genes[i]=tf.Variable(tf.zeros((self.data.num_input,2)), trainable=True)
            else:
                self.genes[i]=tf.Variable(tf.zeros((self.data.num_input,2)), trainable=False)
        print("Random gene success")
        return

    def update_genes(self, g):
        for i in range(jishu):
            self.genes[i] = tf.Variable(g[i], trainable=True)
        print("update gene success")
        return 

    def load_genes(self):
        if os.path.exists("gene.npz"):
            genes_v = np.load("gene.npz")["gene"]
            for i in range(jishu):
                if i in range(train_s, train_e):
                    self.genes[i]=tf.Variable(genes_v[i], trainable=True)
                else:
                    self.genes[i]=tf.Variable(genes_v[i], trainable=False)

            print("Load gene success")
            return True
        return False

    def save_genes(self):
        genes_v=[]
        for i in range(jishu):
            genes_v.append(self.genes[i].eval())
        np.savez("gene.npz", gene=genes_v)
        print("Store genes success")
        return


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







    def tl_net(self, inputs):

        mid_num1=100
        mid_num2=100
        
        w1 = tf.Variable(tf.random_normal([1, mid_num1]))
        b1 = tf.Variable(tf.zeros([mid_num1]) + 0.000001)
        x1 = self.fc_layer("layer1", inputs, w1, b1)

        w2 = tf.Variable(tf.random_normal([mid_num1, mid_num2]))
        b2 = tf.Variable(tf.zeros([mid_num2]) + 0.000001)
        x2 = self.fc_layer("layer2", x1, w2, b2)

        w3 = tf.Variable(tf.random_normal([mid_num2, 1]))
        b3 = tf.Variable(tf.zeros([1]) + 0.000001)
        x3 = self.fc_layer("layer3", x2, w3, b3)



        return x2

    def train(self):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        pred = self.tl_net(self.x)
        cost = tf.reduce_mean(tf.pow(2.7, tf.abs(self.y-pred)))
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
                    

                if False:
                #if True and step > 1:
                    for i in range(batch_x.shape[0]):
                        print(batch_x[i][0], pv[i][0], batch_y[i][0])
                    exit()



                print("yakelog:%.4f\t%.4f\t%.4f" % (score1, score2, min_score))


        tf.reset_default_graph()
        return min_score
    



if __name__ == '__main__':
    
    div=4
    dl = DL()
    test_mode=2
    if test_mode == 1:
        for i in range(0,jishu,4):
            train_s = i
            train_e = i+4
            dl.train()

    elif test_mode == 2:
        dl.train()



    elif test_mode == 9:
        dl.random_test()

  

    #dl.save_genes()
