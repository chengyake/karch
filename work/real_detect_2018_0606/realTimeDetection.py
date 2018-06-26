import numpy as np
import tensorflow as tf
import cv2

from tensorflow.python.ops import control_flow_ops  
from tensorflow.python.training import moving_averages  
from tensorflow.python.framework import graph_util

from StoreImageToMem import Memimages




learning_rate = 0.001
training_iters = 2400000000
batch_size = 30
display_step = 10
save_model_step=100
HW=287
is_training=True
#is_training=False

x = tf.placeholder(tf.float32, [None, HW,HW,3])
y = tf.placeholder(tf.float32, [None, HW,HW,4])
#k = tf.placeholder(tf.float32)
#c = tf.placeholder(tf.uint8, [1, HW,HW,3])



def bn_layer(inputs,is_training,name='BatchNorm',moving_decay=0.9,eps=1e-5):
    shape = inputs.shape
    assert len(shape) in [2,4]

    param_shape = shape[-1]

    gamma = tf.Variable(tf.ones(param_shape), name='gamma')
    beta  = tf.Variable(tf.zeros(param_shape), name='beta')
    mean  = tf.Variable(tf.ones(param_shape), trainable=False, name='mean')
    var   = tf.Variable(tf.ones(param_shape), trainable=False, name='var')

    if is_training == True:
        batch_mean, batch_var = tf.nn.moments(inputs,[0,1,2],name='moments')
        mean = tf.assign(mean, batch_mean)
        var = tf.assign(var, batch_var)
        return tf.nn.batch_normalization(inputs,batch_mean+mean*1e-8,batch_var+var*1e-8,gamma,beta,eps)
    else:
        return tf.nn.batch_normalization(inputs,mean,var,gamma,beta,eps)

def leaky_relu(inputs):
    return tf.maximum(inputs, 0.2*inputs)


def conv2d_1(name, inputs, shape, strides=1):

    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        x1 = tf.nn.conv2d(inputs, W, strides=[1, strides, strides, 1], padding='SAME', name="conv1")

        if name=='layerM21' and is_training == True:
            tf.summary.scalar('w_mean',tf.reduce_mean(W))
            tf.summary.scalar('w_max',tf.reduce_max(W))
            tf.summary.scalar('w_min',tf.reduce_min(W))


    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        x2 = tf.nn.bias_add(x1, B, name="bias1")
    
    with tf.name_scope(name+"_BN"):
        x3 = bn_layer(x2, is_training, name=name)

    with tf.name_scope(name+"_relu"):
        c1_out=leaky_relu(x3)

    return c1_out



def conv2d_2(name, inputs, shape, strides=2):
    
    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        x1 = tf.nn.conv2d(inputs, W, strides=[1, strides, strides, 1], padding='VALID', name="conv2") #same ???

    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        x2 = tf.nn.bias_add(x1, B, name="bias2")
    
    with tf.name_scope(name+"_BN"):
        x3 = bn_layer(x2, is_training, name=name)

    with tf.name_scope(name+"_relu"):
        c2_out=leaky_relu(x3)

    return c2_out


def conv_add(name, a1, a2):
    with tf.name_scope(name):
        add = tf.add(a1, a2, 'add')
    return add


def upscale2(name, inputs, filters, output_shape):
    with tf.name_scope(name):
        deconv = tf.layers.conv2d_transpose(inputs, filters, 3, 2, "VALID")
        #deconv = tf.contrib.layers.conv2d_transpose(inputs, filters, 3, 2, "VALID", trainable=is_training)
        #print(inputs.shape)
        #print(inputs.shape[1]*2+1)
        #W = tf.Variable(tf.random_normal([3, 3, filters, inputs.shape[-1]]))
        #deconv = tf.nn.conv2d_transpose(inputs, filter=W, output_shape=output_shape, strides=[1,2,2,1], padding='VALID')
    return deconv

def dropout(name, inputs, keep_prob):
    with tf.name_scope(name):
        return tf.nn.dropout(inputs, keep_prob)
    


def yake_net(inputs):

    conv0 = tf.reshape(inputs, shape=[-1, HW, HW, 3], name="reshape")  #N,HW,HW,3
    conv1 = conv2d_1('layer1',  conv0,  [3,3,  3, 32]) #n,HW,HW,32

    conv2 = conv2d_2('layer2',  conv1,  [3,3, 32, 64]) #n,143,143
    conv3 = conv2d_1('layer3',  conv2,  [1,1, 64, 32]) # 1
    conv4 = conv2d_1('layer4',  conv3,  [3,3, 32, 64]) 
    ladd5 = conv_add('layer5',  conv2,  conv4)


    conv6 = conv2d_2('layer6',  ladd5,  [3,3, 64,128]) #n,71,71
    conv7 = conv2d_1('layer7',  conv6,  [1,1,128, 64]) #1
    conv8 = conv2d_1('layer8',  conv7,  [3,3, 64,128]) 
    ladd9 = conv_add('layer9',  conv6,  conv8)
    conv10= conv2d_1('layer10', ladd9,  [1,1,128, 64]) #2
    conv11= conv2d_1('layer11', conv10, [3,3, 64,128]) 
    ladd12= conv_add('layer12', conv11, ladd9)


    conv19= conv2d_2('layer19', ladd12, [3,3,128,128]) #n,35,35
    conv20= conv2d_1('layer20', conv19, [1,1,128,64]) #1
    conv21= conv2d_1('layer21', conv20, [3,3,64,128]) 
    ladd22= conv_add('layer22', conv21, conv19)
    conv23= conv2d_1('layer23', ladd22, [1,1,128,64]) #2
    conv24= conv2d_1('layer24', conv23, [3,3,64,128]) 
    ladd25= conv_add('layer25', conv24, ladd22)
    conv26= conv2d_1('layer26', ladd25, [1,1,128,64]) #3
    conv27= conv2d_1('layer27', conv26, [3,3,64,128]) 
    ladd28= conv_add('layer28', conv27, ladd25)
    conv29= conv2d_1('layer29', ladd28, [1,1,128,64]) #4
    conv30= conv2d_1('layer30', conv29, [3,3,64,128]) 
    ladd31= conv_add('layer31', conv30, ladd28)



    conv32= conv2d_2('layer32', ladd31, [3,3,128,128]) #n,17,17
    conv33= conv2d_1('layer33', conv32, [1,1,128,64]) 
    conv34= conv2d_1('layer34', conv33, [3,3,64,128]) 
    ladd35= conv_add('layer35', conv34, conv32)
    conv36= conv2d_1('layer36', ladd35, [1,1,128,64]) 
    conv37= conv2d_1('layer37', conv36, [3,3,64,128]) 
    laddA1= conv_add('layerA1', conv37, ladd35)
    convA2= conv2d_1('layerA2', laddA1, [1,1,128,64]) 
    convA3= conv2d_1('layerA3', convA2, [3,3,64,128]) 
    laddB1= conv_add('layerB1', convA3, laddA1)
    convB2= conv2d_1('layerB2', laddB1, [1,1,128,64]) 
    convB3= conv2d_1('layerB3', convB2, [3,3,64,128]) 
    laddB4= conv_add('layerB4', convB3, laddB1)

    

    convM19= conv2d_2('layerM19', laddB4 , [3,3,128, 128]) #n,8,8
    convM20= conv2d_1('layerM20', convM19, [1,1,128, 64])
    convM21= conv2d_1('layerM21', convM20, [3,3,64, 128]) 
    laddM22= conv_add('layerM22', convM21, convM19)
    convM23= conv2d_1('layerM23', laddM22, [1,1,128, 64])
    convM24= conv2d_1('layerM24', convM23, [3,3,64, 128]) 
    laddM25= conv_add('layerM25', convM24, laddM22)

    deconF2= upscale2("deconF2", laddM25, 128, 3)
    #with tf.name_scope('deconF2'):
    #    W1 = tf.Variable(tf.random_normal([3, 3, 128, 128]))
    #    deconF2 = tf.nn.conv2d_transpose(laddM25, filter=W1, output_shape=[batch_size, 17,17,128], strides=[1,2,2,1], padding='VALID')
    convF3 = conv2d_1('layerF3', deconF2, [1,1,128,64]) 
    convF4 = conv2d_1('layerF4', convF3,  [3,3,64,128]) 
    laddF5 = conv_add('layerF5', convF4,  deconF2)
    convF6 = conv2d_1('layerF6', laddF5,  [1,1,128,64]) 
    convF7 = conv2d_1('layerF7', convF6,  [3,3,64,128]) 
    laddF8 = conv_add('layerF8', convF7,  laddF5)


    deconG2= upscale2("deconG2", laddF8, 128, 3)
    #with tf.name_scope('deconG2'):
    #    W2 = tf.Variable(tf.random_normal([3, 3, 128, 128]))
    #    deconG2 = tf.nn.conv2d_transpose(laddF8, filter=W2, output_shape=[batch_size, 35,35,128], strides=[1,2,2,1], padding='VALID')
    convG3 = conv2d_1('layerG3', deconG2, [1,1,128,64]) 
    convG4 = conv2d_1('layerG4', convG3,  [3,3,64,128]) 
    laddG5 = conv_add('layerG5', convG4,  deconG2)
    convG6 = conv2d_1('layerG6', laddG5,  [1,1,128,64]) 
    convG7 = conv2d_1('layerG7', convG6,  [3,3,64,128]) 
    laddG8 = conv_add('layerG8', convG7,  laddG5)

    deconH2= upscale2("deconH2", laddG8, 64, 3)
    #with tf.name_scope('deconH2'):
    #    W3 = tf.Variable(tf.random_normal([3, 3, 64, 128]))
    #    deconH2 = tf.nn.conv2d_transpose(laddG8, filter=W3, output_shape=[batch_size, 71,71,64], strides=[1,2,2,1], padding='VALID')
    convH3 = conv2d_1('layerH3', deconH2, [1,1,64,32]) 
    convH4 = conv2d_1('layerH4', convH3,  [3,3,32,64]) 
    laddH5 = conv_add('layerH5', convH4,  deconH2)
    convH6 = conv2d_1('layerH6', laddH5,  [1,1,64,32]) 
    convH7 = conv2d_1('layerH7', convH6,  [3,3,32,64]) 
    laddH8 = conv_add('layerH8', convH7,  laddH5)

    deconK2= upscale2("deconK2", laddH8, 32, 3)
    #with tf.name_scope('deconK2'):
    #    W4 = tf.Variable(tf.random_normal([3, 3, 32, 64]))
    #    deconK2 = tf.nn.conv2d_transpose(laddH8, filter=W4, output_shape=[batch_size, 143,143,32], strides=[1,2,2,1], padding='VALID')
    convK3 = conv2d_1('layerK3', deconK2, [1,1,32,16]) 
    convK4 = conv2d_1('layerK4', convK3,  [3,3,16,32]) 
    laddK5 = conv_add('layerK5', convK4,  deconK2)
    convK6 = conv2d_1('layerK6', laddK5,  [1,1,32,16]) 
    convK7 = conv2d_1('layerK7', convK6,  [3,3,16,32]) 
    laddK8 = conv_add('layerK8', convK7,  laddK5)

    deconZ2= upscale2("deconZ2", laddK8, 16, 3)
    #with tf.name_scope('deconZ2'):
    #    W5 = tf.Variable(tf.random_normal([3, 3, 16, 32]))
    #    deconZ2 = tf.nn.conv2d_transpose(laddK8, filter=W5, output_shape=[batch_size, 287,287,16], strides=[1,2,2,1], padding='VALID')
    convZ3 = conv2d_1('layerZ3', deconZ2, [1,1,16,8]) 
    convZ4 = conv2d_1('layerZ4', convZ3,  [3,3,8,16]) 
    laddZ5 = conv_add('layerZ5', convZ4,  deconZ2)
    convZ6 = conv2d_1('layerZ6', laddZ5,  [1,1,16,8]) 
    convZ7 = conv2d_1('layerZ7', convZ6,  [3,3,8,16]) 
    laddZ8 = conv_add('layerZ8', convZ7,  laddZ5)

    convZ9 = conv2d_1('layerZ9', laddZ8,  [1,1,16,4]) 

    #with tf.name_scope("output"):
    out = tf.sigmoid(convZ9, "sigmoid")
    #out = tf.nn.relu(conv45, name = "sigmoid")
    tf.summary.image("input", inputs[:1,:,:,:]*255)
    tf.summary.image("output1", out[:1,:,:,:1]*255)
    tf.summary.image("output2", out[:1,:,:,1:2]*255)
    return out


memimg = Memimages()
#pred = yake_net(x, k)
pred = yake_net(x)

pred_a=pred[:,:,:,:2]
y_a=y[:,:,:,:2]

pred_b=pred[:,:,:,2:]
y_b=y[:,:,:,2:]


cast1_a = tf.reduce_sum(tf.abs(y_a-pred_a), axis=-1)
cast1_b = tf.reduce_sum(y_a, axis=-1)-0.0019
cast1 = tf.reduce_mean(cast1_a*cast1_b)*10000


cast2_a = tf.reduce_sum(tf.abs(y_b-pred_b), axis=-1)
cast2_b = tf.reduce_sum(y_a, axis=-1)-0.002
cast2 = tf.reduce_mean(cast2_a*cast2_b)*10000




#class_loss
#cast1 = tf.reduce_mean(tf.pow(2.7, tf.abs(y_a-pred_a)*y_a)-1)
#box loss
#cast2 = tf.reduce_mean(tf.reduce_mean(tf.pow(4.0, tf.abs(y_b-pred_b))-1, axis=-1) * (tf.reduce_sum(y_a, axis=-1)-0.002))

cast = cast1+cast2



tf.summary.scalar('loss1',cast1)
tf.summary.scalar('loss2',cast2)
tf.summary.scalar('loss',cast)
#tf.summary.image("image", c)

optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cast)


init = tf.global_variables_initializer()

saver = tf.train.Saver(tf.global_variables())

def write_photo(batch_x, lab, name):

    img=batch_x[0].copy()
    img*=255

    rate = lab[0,:,:,:2]/np.expand_dims(np.sum(lab[0,:,:,:2], axis=-1), axis=-1)
    ij = np.where(rate[:,:,:2]==np.max(rate[:,:,:2]))
    i=ij[0][0]
    j=ij[1][0]
    index=ij[2][0]

    if np.sum(lab[0,i,j,:2]) >=0.9:
        w = int(lab[0,i,j,2]*HW) 
        h = int(lab[0,i,j,3]*HW)
        percent = rate[i,j,index]
        cv2.rectangle(img, (j-w, i-h), (j+w, i+h), (0,255,0), 1)
        cv2.putText(img,memimg.classes[index]+"%0.3f" % percent, (j,i),cv2.FONT_HERSHEY_COMPLEX_SMALL,0.8,(0,255,0), 1)
    cv2.imwrite(name, img)
    return np.expand_dims(img, 0)



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
    
    #names = [v.name for v in tf.global_variables()]
    #print(names)
    print('layer11_BN/layer11/mean:0', (sess.run('layer11_BN/mean:0')))
    print('layer11_BN/layer11/var:0', (sess.run('layer11_BN/var:0')))

    if is_training == False:
        batch_x, batch_y = memimg.get_val(batch_size)
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
        batch_x, batch_y = memimg.get_batch(batch_size)
        train_loss, _ =sess.run([cast, optimizer], feed_dict={x: batch_x, y: batch_y})
        
        if step % display_step==0:
            #calc train loss
            train_loss, p, rs=sess.run([cast, pred, merged], feed_dict={x: batch_x, y: batch_y})
            train_writer.add_summary(rs, step)
            train_writer.flush()
            write_photo(batch_x, p, "train.png")
            #calc val loss
            batch_x, batch_y = memimg.get_val(batch_size)
            val_loss, p, rs=sess.run([cast, pred, merged], feed_dict={x: batch_x, y: batch_y})
            test_writer.add_summary(rs, step)
            test_writer.flush()
            write_photo(batch_x, p, 'val.png')
            print("Iter " + str(step*batch_size) + ", Train Loss= "+"{:.6f}".format(train_loss)+", Val Loss= "+"{:.6f}".format(val_loss))
        else:
            print("Iter " + str(step*batch_size) + ", Train Loss= "+"{:.6f}".format(train_loss))


        #save model
        if step % save_model_step == 0:
            saver.save(sess, "save/model.ckpt", global_step=step)
            print("save model.ckpt success")

    print ("Optimization Finished!")


    




















