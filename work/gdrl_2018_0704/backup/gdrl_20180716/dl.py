#! /usr/bin/python3
'''
layer_1          ooo
                
                w1+b1

layer_2      nnnnnnnnnnnn

                w2+b2

layer_3        iiiiiii

'''
import time
import random
import argparse
import numpy as np
import tensorflow as tf

from mnist.data import DATA


#best score 0.9631 in full-connnection mode after 5 mins
#best score        in GDRL mode

class DL:

    batch_size = 128
    learning_rate = 0.1
    training_iters = batch_size*1000

    num_nodes = [784, 128, 10] #must > =3
    num_con_s = [(num_nodes[0],num_nodes[1]),
                 (num_nodes[1],num_nodes[2])]
    num_con_h = [512, 512, 512, 512, 512]


#    num_nodes = [784, 256, 256, 256, 256, 10] #must > =3
#    num_con_s = [(num_nodes[0],num_nodes[1]),
#                 (num_nodes[1],num_nodes[2]),
#                 (num_nodes[2],num_nodes[3]),
#                 (num_nodes[3],num_nodes[4]),
#                 (num_nodes[4],num_nodes[5])]
#    num_con_h = [512, 512, 512, 512, 512]


    num_layer = len(num_nodes)
    
    def __init__(self):
        self.data = DATA()

  
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
    
 
    def init_soft_connections(self, soft_genesw, soft_genesb):
        self.soft_conw = {
            'in': tf.Variable(soft_genesw[0], trainable=True),
            'out':tf.Variable(soft_genesw[-1], trainable=True)
        }
        for i in range(1, self.num_layer-2):
            self.soft_conw[i]=tf.Variable(soft_genesw[i], trainable=True)

        self.soft_conb = {
            'in': tf.Variable(soft_genesb[0], trainable=True),
            'out':tf.Variable(soft_genesb[-1], trainable=True)
        }
        for i in range(1, self.num_layer-2):
            self.soft_conb[i]=tf.Variable(soft_genesb[i], trainable=True)
        return


    def init_hard_connections(self, hard_genesw, hard_genesb):

        self.hard_conw = {
            'in': tf.Variable(hard_genesw[0], trainable=False),
            'out':tf.Variable(hard_genesw[-1], trainable=False)
        }
        for i in range(1, self.num_layer-2):
            self.hard_conw[i]=tf.Variable(hard_genesw[i], trainable=False)

        self.hard_conb = {
            'in': tf.Variable(hard_genesb[0], trainable=False),
            'out':tf.Variable(hard_genesb[-1], trainable=False)
        }
        for i in range(1, self.num_layer-2):
            self.hard_conb[i]=tf.Variable(hard_genesb[i], trainable=False)
        return

    def alex_net(self, inputs):
        
        layer = self.fc_layer("layer_input", inputs, self.soft_conw['in']*self.hard_conw["in"], self.soft_conb['in']*self.hard_conb["in"])
        for i in range(1, self.num_layer-2):
            layer = self.fc_layer("layer_mid%d"%i,layer,self.soft_conw[i]*self.hard_conw[i],self.soft_conb[i]*self.hard_conb[i])
        out = self.fc_layer("layer_output", layer, self.soft_conw['out']*self.hard_conw["out"], self.soft_conb['out']*self.hard_conb["out"])

        return out



    def random_test(self):
        
        batch_x, batch_y = self.data.get_val_batch()
        pre = np.random.random((batch_y.shape[0], 2))
        acc = np.mean(np.equal(np.argmax(pre ,-1), np.argmax(batch_y, -1)))
        print("Random Test accuracy:", acc)

    def test(self, genes):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        pred = self.alex_net(self.x)
        cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=self.z, logits=pred))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)
        correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(self.y,1))
        accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))
        
        init = tf.global_variables_initializer()
        max_score=0.0;
        step = 0;
        with tf.Session() as sess:
            max_score=-100000.0
            sess.run(init)
            while step * self.batch_size < self.training_iters*10:
                step+=1
                batch_x, batch_y = self.data.get_train_batch(self.batch_size)
                score1, _ = sess.run([accuracy, optimizer], feed_dict={self.x: batch_x, self.y: batch_y})

                batch_x, batch_y = self.data.get_score_batch()
                score2 = sess.run(accuracy, feed_dict={self.x: batch_x, self.y: batch_y})
                if score2 > max_score:
                    max_score = score2
                print("yakelog:%.4f\t%.4f" % (acc, max_score))


        tf.reset_default_graph()
        print("My gdrl accuracy:", acc)

    def train(self, hard_genes_w, hard_genes_b, soft_genes_w, soft_genes_b):
        self.x = tf.placeholder(tf.float32, [None, self.data.num_input])
        self.y = tf.placeholder(tf.float32, [None, self.data.num_class])

        self.init_hard_connections(hard_genes_w, hard_genes_b)
        self.init_soft_connections(soft_genes_w, soft_genes_b)


        pred = self.alex_net(self.x)
        cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(labels=self.y, logits=pred))
        optimizer = tf.train.AdamOptimizer(learning_rate=self.learning_rate).minimize(cost)
        correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(self.y,1))
        accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))

        tf.summary.scalar("loss", cost)

        
        init = tf.global_variables_initializer()
        max_score=0.0;
        with tf.Session() as sess:
            sess.run(init)
            merge = tf.summary.merge_all()
            train_writer = tf.summary.FileWriter("logs/train", sess.graph)
            test_writer = tf.summary.FileWriter("logs/test")


            step = 1
            #while step * self.batch_size < self.training_iters:
            while True:
                batch_x, batch_y = self.data.get_train_batch(self.batch_size)
                score1, _, m= sess.run([accuracy, optimizer, merge], feed_dict={self.x: batch_x, self.y: batch_y})
                train_writer.add_summary(m, step)

                batch_x, batch_y = self.data.get_score_batch()
                score2, m = sess.run([accuracy, merge], feed_dict={self.x: batch_x, self.y: batch_y})
                test_writer.add_summary(m, step)
                step += 1
                if score2 > max_score:
                    max_score = score2
                print("yakelog:%.4f\t%.4f\t%.4f" % (score1, score2, max_score))
                

                if score2 >=0.96 :
                    print(self.soft_conw["in"].eval())
                    print(self.soft_conb["in"].eval())
                    print(self.soft_conw["out"].eval())
                    print(self.soft_conb["out"].eval())
                    exit()


        tf.reset_default_graph()

        return max_score
    



if __name__ == '__main__':
    
    dl = DL()
    test_mode=1
    if test_mode == 1:
        hw=[]
        hb=[]
        sw=[]
        sb=[]
        for i in range(dl.num_layer-1):
            hw.append( np.random.random(dl.num_con_s[i]).astype(np.float32) )
            hb.append( np.random.random(dl.num_con_s[i][1]).astype(np.float32) )
            sw.append( np.ones((dl.num_con_s[i]), dtype=np.float32) )
            sb.append( np.ones((dl.num_con_s[i][1]), dtype=np.float32) )
        dl.train(hw,hb,sw,sb)
    elif test_mode == 2:
        dl.random_test()



