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

#from fdata.data import DATA
from dydata.data import DATA
#from mnist.data import DATA

train_idx=-1

fbl = 100
class DL:
    batch_size = 1000
    learning_rate = 0.1
    training_iters = batch_size*400
    
    def __init__(self):
        self.data = DATA()
    

    def random_genes(self):
        tmp=[]
        for i in range(self.data.num_input):
            if i==train_idx or train_idx==-1:
                tmp.append(tf.Variable(tf.random_normal((1, fbl+1)), trainable=True))
            else:
                tmp.append(tf.Variable(tf.random_normal((1, fbl+1)), trainable=False))
        self.genes=tf.concat([v for v in tmp], axis=0)
        print("Random gene success")
        return


    def load_genes(self):
        if os.path.exists("gene.npz"):
            tmp=[]
            genes_v = np.load("gene.npz")["gene"]
            for i in range(self.data.num_input):
                if i==train_idx or train_idx==-1:
                    tmp.append(tf.expand_dims(tf.Variable(genes_v[i], trainable=True), axis=0))
                else:
                    tmp.append(tf.expand_dims(tf.Variable(genes_v[i], trainable=False), axis=0))
            self.genes=tf.concat([v for v in tmp], axis=0)
            print("Load gene success")
            return True
        return False

    def save_genes(self):
        np.savez("gene.npz", gene=self.genes.eval())
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
#
#    def fc_net(self, inputs):
#
#        mid_num1=1000
#        mid_num2=1000
#
#        w1 = tf.Variable(tf.random_normal([784, mid_num1]))
#        b1 = tf.Variable(tf.zeros([mid_num1]) + 0.000001)
#        x1 = self.fc_layer("layer1", inputs, w1, b1)
#
#        w2 = tf.Variable(tf.random_normal([mid_num1, mid_num2]))
#        b2 = tf.Variable(tf.zeros([mid_num2]) + 0.000001)
#        x2 = self.fc_layer("layer2", x1, w2, b2)
#
#        w3 = tf.Variable(tf.random_normal([mid_num2, 1]))
#        b3 = tf.Variable(tf.zeros([1]) + 0.000001)
#        x3 = self.fc_layer("layer3", x2, w3, b3)
#        return x3


    def ddx_layer(self, inputs):
        li = tf.round(inputs[:,0]*fbl)


        self.genes=tf.Variable(tf.random_normal((fbl+1,)), trainable=True)


        l = self.genes[0]*tf.cast(tf.equal(li, 0), dtype=tf.float32)
        for i in range(1, fbl+1):
            l += self.genes[i]*tf.cast(tf.equal(li, i), dtype=tf.float32)

        return l



    def _ddx_layer(self, inputs):
        li = tf.round(inputs*fbl)


        self.genes=tf.Variable(tf.random_normal((2, fbl+1)), trainable=True)


        l = self.genes[:,0]*tf.cast(tf.equal(li, 0), dtype=tf.float32)
        for i in range(1, fbl+1):
            l += self.genes[:,i]*tf.cast(tf.equal(li, i), dtype=tf.float32)

        print(l.shape)
        layer = l[:, 0]
        #for i in range(1, self.data.num_input):
        #    layer = layer*l[:,i]
        print(layer.shape)

        return layer


    def ddx_net(self, inputs):#inputs must be 0-1
        
        layer = self.ddx_layer(inputs)
        #li = tf.round(tf.nn.sigmoid(layer)*fbl)
        #arg=tf.Variable(tf.random_normal((1, fbl+1)), trainable=True)
        #l = arg[:, 0]*tf.cast(tf.equal(li, 0), dtype=tf.float32)
        #for i in range(1, fbl+1):
        #    l += arg[:,i]*tf.cast(tf.equal(li, i), dtype=tf.float32)


        #w3 = tf.Variable(tf.random_normal([784, 1]))
        #b3 = tf.Variable(tf.zeros([1]) + 0.0001)
        #x3 = self.fc_layer("layer3", layer, w3, b3)
        
        return layer



    def train(self):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        #if self.load_genes()==False:
        #    self.random_genes()
        pred = self.ddx_net(self.x)
        #cost = tf.reduce_mean(tf.abs(pred-tf.cast(tf.argmax(self.y,-1), dtype=tf.float32)))
        cost = tf.reduce_mean(tf.abs(pred-self.y))
        #cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=self.y, logits=pred))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)
        #correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(self.y,1))
        #correct_pred = tf.equal(tf.round(pred), tf.cast(tf.argmax(self.y,1), dtype=tf.float32))
        #acc = tf.reduce_mean(tf.cast(correct_pred, tf.float32))



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
                score2, m, pv= sess.run([cost, merge, pred], feed_dict={self.x: batch_x, self.y: batch_y})
                test_writer.add_summary(m, step)



                if max_score < 0.082 and False:
                    print("x=[")
                    for i in range(1000):
                        print(batch_x[i][0],",")
                    print("]")

                    print("y=[")
                    for i in range(1000):
                        print(batch_x[i][1],",")
                    print("]")

                    print("z=[")
                    for i in range(1000):
                        print(pv[i],",")
                    print("]")

                    print("c=[")
                    for i in range(1000):
                        print(batch_y[i],",")
                    print("]")
                    exit()




                step += 1
                if score2 < max_score:
                    max_score = score2
                    self.save_genes()

                print("yakelog:%.4f\t%.4f\t%.4f" % (score1, score2, max_score))

        tf.reset_default_graph()
        return max_score
    



if __name__ == '__main__':
    
    s=1000.0
    dl = DL()
    test_mode=2
    if test_mode == 1: 
        train_idx=-1
        dl.train()
        while s > 0.001:
            for i in range(0, dl.data.num_input):
                train_idx=i
                s=dl.train()
                print(i, "yakelog:train success", s)

    elif test_mode == 2:
        train_idx=-1
        dl.train()



    elif test_mode == 9:
        dl.random_test()

  

    #dl.save_genes()
