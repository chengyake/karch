import numpy as np
import tensorflow as tf
import cv2

from tensorflow.python.ops import control_flow_ops  
from tensorflow.python.training import moving_averages  
from tensorflow.python.framework import graph_util

from data import DATA




learning_rate = 0.01
training_iters = 2400000000
batch_size = 32
display_step = 10
save_model_step=100
HW=256
is_training=True
#is_training=False
lambda1 = 0.001

x = tf.placeholder(tf.float32, [None, HW,HW,1])
y255 = tf.placeholder(tf.float32, [None, HW,HW,1])
y = tf.multiply(y255, 0.003921569)#1/255



def bn_layer(inputs,is_training,name='BatchNorm',moving_decay=0.9,eps=1e-5):
    shape = inputs.shape
    assert len(shape) in [2,4]

    param_shape = shape[-1]

    gamma = tf.Variable(tf.ones(param_shape), name='gamma')
    beta  = tf.Variable(tf.zeros(param_shape), name='beta')
    mean  = tf.Variable(tf.ones(param_shape), trainable=False, name='mean')
    var   = tf.Variable(tf.ones(param_shape), trainable=False, name='var')

    tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(gamma)) 
    tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(beta)) 
    tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(mean)) 
    tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(var)) 



    if is_training == True:
        batch_mean, batch_var = tf.nn.moments(inputs,[0,1,2],name='moments')
        mean = tf.assign(mean, batch_mean)
        var = tf.assign(var, batch_var)
        return tf.nn.batch_normalization(inputs,batch_mean+mean*1e-10,batch_var+var*1e-10,gamma,beta,eps)
    else:
        return tf.nn.batch_normalization(inputs,mean,var,gamma,beta,eps)

def leaky_relu(inputs):
    return tf.maximum(inputs, 0.2*inputs)


def conv2d_1(name, inputs, shape, strides=1):

    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(W)) 
        x1 = tf.nn.conv2d(inputs, W, strides=[1, strides, strides, 1], padding='SAME', name="conv1")
        if name=='layerM21' and is_training == True:
            tf.summary.scalar('w_mean',tf.reduce_mean(W))
            tf.summary.scalar('w_max',tf.reduce_max(W))
            tf.summary.scalar('w_min',tf.reduce_min(W))


    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(B)) 
        x2 = tf.nn.bias_add(x1, B, name="bias1")
    
    with tf.name_scope(name+"_BN"):
        x3 = bn_layer(x2, is_training, name=name)

    with tf.name_scope(name+"_relu"):
        c1_out=leaky_relu(x3)
        #c1_out=tf.nn.leaky_relu(x3)

    return c1_out



def conv2d_2(name, inputs, shape, strides=2):
    
    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(W)) 
        x1 = tf.nn.conv2d(inputs, W, strides=[1, strides, strides, 1], padding='SAME', name="conv2")

    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        tf.add_to_collection('l2_losses', tf.contrib.layers.l2_regularizer(lambda1)(B)) 
        x2 = tf.nn.bias_add(x1, B, name="bias2")
    
    with tf.name_scope(name+"_BN"):
        x3 = bn_layer(x2, is_training, name=name)

    with tf.name_scope(name+"_relu"):
        c2_out=leaky_relu(x3)
        #c2_out=tf.nn.leaky_relu(x3)

    return c2_out


def conv_add(name, a1, a2):
    with tf.name_scope(name):
        add = tf.add(a1, a2, 'add')
    return add


def upscale2(name, inputs, filters):
    with tf.name_scope(name):
        x1 = tf.layers.conv2d_transpose(inputs, filters, 3, 2, "SAME", trainable = is_training)
        #deconv = tf.contrib.layers.conv2d_transpose(inputs, filters, 3, 2, "VALID", trainable=is_training)
    with tf.name_scope(name+"_BN"):
        x2 = bn_layer(x1, is_training, name=name)
        return x2

def dropout(name, inputs, keep_prob):
    with tf.name_scope(name):
        return tf.nn.dropout(inputs, keep_prob)
    

def res_net(name, inputs, res_num, i_n, o_n):
    idx=0

    ml = conv2d_2(name+"_%d"%idx,  inputs,  [3,3, i_n,o_n])
    idx+=1
    for i in range(res_num):
        ml0 = conv2d_1(name+"_%d"%idx,  ml,  [1,1,o_n, i_n]) 
        idx+=1
        ml1 = conv2d_1(name+"_%d"%idx,  ml0, [3,3,i_n, o_n]) 
        idx+=1
        ml  = conv_add(name+"_%d"%idx,  ml1,  ml)
        idx+=1
    return ml


def res_net1(name, inputs, res_num, i_n, o_n):
    idx=0
    ml = inputs
    for i in range(res_num):
        ml0 = conv2d_1(name+"_%d"%idx,  ml,  [1,1,o_n, i_n]) 
        idx+=1
        ml1 = conv2d_1(name+"_%d"%idx,  ml0, [3,3,i_n, o_n]) 
        idx+=1
        ml  = conv_add(name+"_%d"%idx,  ml1,  ml)
        idx+=1
    return ml

def yake_net(inputs):

    conv0 = tf.reshape(inputs, shape=[-1, HW, HW, 1], name="reshape")  #N,HW,HW,1
    with tf.name_scope("div"):
        convd = tf.multiply(conv0, 0.003921569)#1/255
    block1 = conv2d_1('block1',  convd, [3,3, 1, 32]) #n,HW,HW,32

    block2 = res_net("block2", block1, 1,  32,  64)     #n,128,128, 64
    block3 = res_net("block3", block2, 2,  64, 128)     #n, 64, 64, 128
    block4 = res_net("block4", block3, 6, 128, 128)     #n, 32, 32, 256
    block5 = res_net("block5", block4, 6, 128, 128)     #n, 16, 16, 512
    block6 = res_net("block6", block5, 3, 128, 128)     #n,  8,  8, 1024

    block7 = upscale2("block7", block6, 128)             #n, 16, 16, 512
    block8 = res_net1("block8", block7,2, 128, 128)
    block9 = upscale2("block9", block8, 128)             #n, 32, 32, 256
    blockA = res_net1("blockA", block9,2, 128, 128)
    blockB = upscale2("blockB", blockA, 128)             #n, 64, 64, 128
    blockC = res_net1("blockC", blockB,2, 64, 128)
    blockD = upscale2("blockD", blockC,  64)             #n,128,128, 64
    blockE = res_net1("blockE", blockD,2, 32,  64)
    blockF = upscale2("blockF", blockE,  32)             #n,256,256, 32
    blockG = res_net1("blockG", blockF,2, 16,  32)


    blockZ = conv2d_1('blockZ', blockG, [1, 1, 32,1]) 

    out = tf.sigmoid(blockZ, "sigmoid")

    tf.summary.image("input0", inputs[:1,:,:,:])
    tf.summary.image("output" , (y[:1,:,:,:]+out[:1,:,:,:])/2.0*255)
    return out


data = DATA()
pred = yake_net(x)



cast = tf.reduce_mean(tf.abs((pred-y)*(y+0.004)))
optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cast)
l2 = tf.add_n(tf.get_collection('l2_losses'))


tf.summary.scalar('loss',cast)
tf.summary.scalar('l2',l2)



init = tf.global_variables_initializer()

saver = tf.train.Saver(tf.global_variables())

def write_photo(batch_x, lab, name):
    img=batch_x[0]+lab[0]*255
    img[img>255]=255
    cv2.imwrite(name, img) 



with tf.Session() as sess:
    sess.run(init)

    merged = tf.summary.merge_all()
    train_writer = tf.summary.FileWriter("logs/train", sess.graph)
    test_writer = tf.summary.FileWriter("logs/test")

    
    #write graph and model to save
    tf.train.write_graph(sess.graph_def, "save", "graph.pbtxt")
    ckpt = tf.train.get_checkpoint_state("./save")
    if ckpt and ckpt.model_checkpoint_path:
        print(ckpt.model_checkpoint_path)
        saver.restore(sess, ckpt.model_checkpoint_path)
    

    if is_training == False:
        batch_x, batch_y = data.get_val_branch(batch_size)
        p=sess.run(pred, feed_dict={x: batch_x})
        print(p[0,:,:,1])
        write_photo(batch_x, p, 'val.png')
        constant_graph = graph_util.convert_variables_to_constants(sess, sess.graph_def, ['sigmoid'])
        with tf.gfile.FastGFile('./save/model_yake.pb' , mode='wb') as f:
            f.write(constant_graph.SerializeToString())
            exit()


    step = 0
    while step * batch_size < training_iters:
        step += 1
        batch_x, batch_y = data.get_train_batch(batch_size)
        train_loss, _ =sess.run([cast, optimizer], feed_dict={x: batch_x, y255: batch_y})
        
        if step % display_step==0:
            #calc train loss
            train_loss, p, rs=sess.run([cast, pred, merged], feed_dict={x: batch_x, y255: batch_y})
            train_writer.add_summary(rs, step)
            write_photo(batch_x, p, "train.png")
            #calc val loss
            batch_x, batch_y = data.get_val_batch(batch_size)
            val_loss, p, rs=sess.run([cast, pred, merged], feed_dict={x: batch_x, y255: batch_y})
            test_writer.add_summary(rs, step)
            write_photo(batch_x, p, 'val.png')
            print("Iter " + str(step*batch_size) + ", Train Loss= "+"{:.6f}".format(train_loss)+", Val Loss= "+"{:.6f}".format(val_loss))
        else:
            print("Iter " + str(step*batch_size) + ", Train Loss= "+"{:.6f}".format(train_loss))


        #save model
        if step % save_model_step == 0:
            saver.save(sess, "save/model.ckpt", global_step=step)
            print("save model.ckpt success")

    print ("Optimization Finished!")


    




















