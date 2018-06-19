import numpy as np
import tensorflow as tf
import cv2

from tensorflow.python.ops import control_flow_ops  
from tensorflow.python.training import moving_averages  
from tensorflow.python.framework import graph_util

from StoreImageToMem import Memimages




learning_rate = 0.001
training_iters = 2400000000
batch_size = 60
display_step = 10
save_model_step=10
HW=287
#is_training=True
is_training=False

x = tf.placeholder(tf.float32, [None, HW,HW,3])
y = tf.placeholder(tf.float32, [None, 8,8,6])
#k = tf.placeholder(tf.float32)
#c = tf.placeholder(tf.uint8, [1, HW,HW,3])



def bn_layer(x,is_training,name='BatchNorm',moving_decay=0.9,eps=1e-5):
    shape = x.shape
    assert len(shape) in [2,4]

    param_shape = shape[-1]
    with tf.variable_scope(name):

        gamma = tf.Variable(tf.ones(param_shape), name='gamma')
        beta  = tf.Variable(tf.zeros(param_shape), name='beta')
        mean  = tf.Variable(tf.ones(param_shape), trainable=False, name='mean')
        var   = tf.Variable(tf.ones(param_shape), trainable=False, name='var')

        if is_training == True:
            axes = list(range(len(shape)-1))
            batch_mean, batch_var = tf.nn.moments(x,axes,name='moments')
            mean = tf.assign(mean, batch_mean)
            var  = tf.assign(var, batch_var)


        if name=='layerM20' and is_training == True:
            mean = tf.reduce_mean(var)
            tf.summary.scalar('w_mean',mean)
            tf.summary.scalar('w_val',var[0])
            tf.summary.scalar('w_max',tf.reduce_max(var))
            tf.summary.scalar('w_min',tf.reduce_min(var))



        return tf.nn.batch_normalization(x,mean,var,beta,gamma,eps)

def leaky_relu(x):
    return tf.maximum(x, 0.2*x)



def conv2d_1(name, inputs, shape, strides=1):

    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        x1 = tf.nn.conv2d(inputs, W, strides=[1, strides, strides, 1], padding='SAME', name="conv")

    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        x2 = tf.nn.bias_add(x1, B, name="bias1")
    
    with tf.name_scope(name+"_BN"):
        x3 = bn_layer(x2, is_training, name=name)

    with tf.name_scope(name+"_relu"):
        #c1_out=tf.nn.relu(x3, name="relu1")
        #c1_out=tf.nn.leaky_relu(x3, name="relu1")
        #c1_out=tf.sigmoid(x3, name="relu1")
        #c1_out=tf.tanh(x3, name="relu1")
        c1_out=leaky_relu(x3)

    return c1_out



def conv2d_2(name, inputs, shape, strides=2):
    
    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        x1 = tf.nn.conv2d(inputs, W, strides=[1, strides, strides, 1], padding='VALID', name="deconv") #same ???

    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        x2 = tf.nn.bias_add(x1, B, name="bias2")
    
    with tf.name_scope(name+"_BN"):
        x3 = bn_layer(x2, is_training, name=name)

    with tf.name_scope(name+"_relu"):
        #c2_out=tf.nn.relu(x3, name="relu2")
        #c2_out=tf.nn.leaky_relu(x3, name="relu2")
        #c2_out=tf.sigmoid(x3, name="relu2")
        #c2_out=tf.tanh(x3, name="relu2")
        c2_out=leaky_relu(x3)

    return c2_out


def conv_add(name, a1, a2):
    with tf.name_scope(name):
        add = tf.add(a1, a2, 'add')
    return add


def upscale2(name, inputs, filters, kernel_size):
    with tf.name_scope(name):
        #deconv = tf.contrib.layers.conv2d_transpose(inputs, filters, kernel_size, 2, "SAME")
        deconv = tf.layers.conv2d_transpose(inputs, filters, kernel_size, 2, "SAME")
    return deconv

def dropout(name, inputs, keep_prob):
    with tf.name_scope(name):
        return tf.nn.dropout(inputs, keep_prob)
    


def yake_net(inputs):

    #with tf.name_scope("input"):
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
#
    convE1= conv2d_1('layerE1', ladd31, [1,1,128,64]) #5
    convE2= conv2d_1('layerE2', convE1, [3,3,64,128]) 
    laddE3= conv_add('layerE3', convE2, ladd31)
    convE4= conv2d_1('layerE4', laddE3, [1,1,128,64]) #6
    convE5= conv2d_1('layerE5', convE4, [3,3,64,128]) 
    laddE6= conv_add('layerE6', convE5, laddE3)
    convF1= conv2d_1('layerF1', laddE6, [1,1,128,64]) #7
    convF2= conv2d_1('layerF2', convF1, [3,3,64,128]) 
    laddF3= conv_add('layerF3', convF2, laddE6)
    convF4= conv2d_1('layerF4', laddF3, [1,1,128,64]) #8
    convF5= conv2d_1('layerF5', convF4, [3,3,64,128]) 
    laddF6= conv_add('layerF6', convF5, laddF3)



    conv32= conv2d_2('layer32', laddF6, [3,3,128,256]) #n,17,17
    conv33= conv2d_1('layer33', conv32, [1,1,256,128]) 
    conv34= conv2d_1('layer34', conv33, [3,3,128,256]) 
    ladd35= conv_add('layer35', conv34, conv32)
    conv36= conv2d_1('layer36', ladd35, [1,1,256,128]) 
    conv37= conv2d_1('layer37', conv36, [3,3,128,256]) 
    laddA1= conv_add('layerA1', conv37, ladd35)
    convA2= conv2d_1('layerA2', laddA1, [1,1,256,128]) 
    convA3= conv2d_1('layerA3', convA2, [3,3,128,256]) 
    laddB1= conv_add('layerB1', convA3, laddA1)
    convB2= conv2d_1('layerB2', laddB1, [1,1,256,128]) 
    convB3= conv2d_1('layerB3', convB2, [3,3,128,256]) 
    laddB4= conv_add('layerB4', convB3, laddB1)
#
    convC1= conv2d_1('layerC1', laddB4, [1,1,256,128]) 
    convC2= conv2d_1('layerC2', convC1, [3,3,128,256]) 
    laddC3= conv_add('layerC3', convC2, laddB4)
    convD1= conv2d_1('layerD1', laddC3, [1,1,256,128]) 
    convD2= conv2d_1('layerD2', convD1, [3,3,128,256]) 
    laddD3= conv_add('layerD3', convD2, laddC3)
    convG1= conv2d_1('layerG1', laddD3, [1,1,256,128]) #7
    convG2= conv2d_1('layerG2', convG1, [3,3,128,256]) 
    laddG3= conv_add('layerG3', convG2, laddD3)
    convG4= conv2d_1('layerG4', laddG3, [1,1,256,128]) #8
    convG5= conv2d_1('layerG5', convG4, [3,3,128,256]) 
    laddG6= conv_add('layerG6', convG5, laddG3)


    convM19= conv2d_2('layerM19', laddG6 , [3,3,256, 512]) #n,8,8
    #dropO1 = dropout ("layerO1" , convM19, keep_prob)
    convM20= conv2d_1('layerM20', convM19, [1,1,512,  128])
    #dropO2 = dropout ("layerO2" , convM20, keep_prob)
    convM21= conv2d_1('layerM21', convM20, [1,1,128,  6]) 


    #with tf.name_scope("output"):
    out = tf.sigmoid(convM21, "sigmoid")
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

pred_b=pred[:,:,:,2:4]
y_b=y[:,:,:,2:4]

pred_c=pred[:,:,:,4:6]
y_c=y[:,:,:,4:6]


#class_loss
cast1 = tf.reduce_mean(tf.pow(2.7, tf.abs(y_a-pred_a))-1)
#center_loss
cast2 = tf.reduce_mean(tf.pow(3.2, tf.abs(y_b-pred_b))-1)
#box loss
cast3 = tf.reduce_mean(tf.pow(6.0, tf.abs(y_c-pred_c))-1)

cast = cast1+cast2+cast3



tf.summary.scalar('loss1',cast1)
tf.summary.scalar('loss2',cast2)
tf.summary.scalar('loss3',cast3)
tf.summary.scalar('loss',cast)
#tf.summary.image("image", c)

optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cast)


init = tf.global_variables_initializer()

saver = tf.train.Saver(tf.global_variables())

def write_photo(batch_x, p, name):
    display_mode=1

    img=batch_x[0].copy()
    img*=255

    rate = p[0,:,:,:2]/np.expand_dims(np.sum(p[0,:,:,:2], axis=-1), axis=-1)
    ij = np.where(rate[:-1,:,:2]==np.max(rate[:-1,:,:2]))
    #print(p[0,:,:,:])
    #print(ij)
    i=ij[0][0]
    j=ij[1][0]
    index=ij[2][0]
    if np.sum(p[0,i,j,:2]) >=0.01:
        xc = (p[0,i,j,2]+j)/8.0
        yc = (p[0,i,j,3]+i)/8.0
        xw = p[0,i,j,4]
        yh = p[0,i,j,5]
        percent = rate[i,j,index]
        cv2.rectangle(img, (int((xc-xw)*HW), int((yc-yh)*HW)), (int((xc+xw)*HW), int((yc+yh)*HW)), (0,255,0), 1)
        cv2.putText(img,memimg.classes[index]+"%0.3f" % percent, (int(xc*HW), int(yc*HW)),cv2.FONT_HERSHEY_COMPLEX_SMALL, 0.5,(0,255,0), 1)


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
    #print('layer11_BN/layer11/mean:0', (sess.run('layer11_BN/layer11/mean:0')))
    #print('layer11_BN/layer11/var:0', (sess.run('layer11_BN/layer11/var:0')))

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


    




















