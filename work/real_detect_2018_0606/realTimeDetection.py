import numpy as np
import tensorflow as tf
import cv2

from tensorflow.python.framework import graph_util

from StoreImageToMem import Memimages


learning_rate = 0.01
training_iters = 100000000
batch_size = 70
display_step = 10
save_model_step=100

x = tf.placeholder(tf.float32, [None, 240,240,3])
y = tf.placeholder(tf.float32, [None, 60,60,22])


def conv2d_1(name, inputs, shape, strides=1):

    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        x1 = tf.nn.conv2d(inputs, W, strides=[1, strides, strides, 1], padding='SAME', name="conv")
        if name=='layer45':
            mean = tf.reduce_mean(W)
            tf.summary.scalar('w_mean',mean)
            tf.summary.scalar('w_val',W[0,0,0,0])
            tf.summary.scalar('w_max',tf.reduce_max(W))
            tf.summary.scalar('w_min',tf.reduce_min(W))

    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        x2 = tf.nn.bias_add(x1, B, name="bias1")
    
    with tf.name_scope(name+"_BN"):
        #x3 = tf.layers.batch_normalization(inputs=x2, name=name+"_bn1")
        scale = tf.Variable(tf.ones([shape[-1]]))
        offset = tf.Variable(tf.zeros([shape[-1]]))
        mean, var = tf.nn.moments(x2, [0,1,2])
        x3 = tf.nn.batch_normalization(x2, mean, var, scale, offset, 0.000001)

    with tf.name_scope(name+"_relu"):
        #c1_out=tf.nn.relu(x3, name="relu1")
        c1_out=tf.nn.leaky_relu(x3, name="relu1")
        #c1_out=tf.sigmoid(x3, name="relu1")

    return c1_out



def conv2d_2(name, inputs, shape, strides=2):
    
    with tf.name_scope(name+"_pad"):
        x0=tf.pad(inputs, [[0,0], [1,0], [1,0], [0,0]])

    with tf.name_scope(name+"_conv"):
        W = tf.Variable(tf.random_normal(shape))
        x1 = tf.nn.conv2d(x0, W, strides=[1, strides, strides, 1], padding='VALID', name="deconv")

    with tf.name_scope(name+"_bias"):
        B = tf.Variable(tf.random_normal([shape[-1]]))
        x2 = tf.nn.bias_add(x1, B, name="bias2")
    
    with tf.name_scope(name+"_BN"):
        #x3 = tf.layers.batch_normalization(inputs=x2, name=name+"_bn2")
        scale = tf.Variable(tf.ones([shape[-1]]))
        offset = tf.Variable(tf.zeros([shape[-1]]))
        mean, var = tf.nn.moments(x2, [0,1,2])
        x3 = tf.nn.batch_normalization(x2, mean, var, scale, offset, 0.000001)

    with tf.name_scope(name+"_relu"):
        #c2_out=tf.nn.relu(x3, name="relu2")
        c2_out=tf.nn.leaky_relu(x3, name="relu2")
        #c2_out=tf.sigmoid(x3, name="relu2")

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

def alex_net(inputs):

    #with tf.name_scope("input"):
    conv0 = tf.reshape(inputs, shape=[-1, 240, 240, 3], name="reshape")  #N,240,240,3
    conv1 = conv2d_1('layer1',  conv0, [3,3,  3, 32]) #n,240,240,32

    conv2 = conv2d_2('layer2',  conv1,  [3,3, 32, 64]) #n,120,120
    conv3 = conv2d_1('layer3',  conv2,  [1,1, 64, 32]) 
    conv4 = conv2d_1('layer4',  conv3,  [3,3, 32, 64]) 
    ladd5 = conv_add('layer5',  conv2,  conv4)


    conv6 = conv2d_2('layer6',  ladd5,  [3,3, 64,128]) #n,60,60
    conv7 = conv2d_1('layer7',  conv6,  [1,1,128, 64]) 
    conv8 = conv2d_1('layer8',  conv7,  [3,3, 64,128]) 
    ladd9 = conv_add('layer9',  conv6,  conv8)
    conv10= conv2d_1('layer10', ladd9,  [1,1,128, 64]) 
    conv11= conv2d_1('layer11', conv10, [3,3, 64,128]) 
    ladd12= conv_add('layer12', conv11, ladd9)
    conv13= conv2d_1('layer13', ladd12, [1,1,128, 64]) 
    conv14= conv2d_1('layer14', conv13, [3,3, 64,128]) 
    ladd15= conv_add('layer15', conv14, ladd12)
    conv16= conv2d_1('layer16', ladd15, [1,1,128, 64]) 
    conv17= conv2d_1('layer17', conv16, [3,3, 64,128]) 
    ladd18= conv_add('layer18', conv17, ladd15)

    conv19= conv2d_2('layer19', ladd18, [3,3,128,256]) #n,30,30
    conv20= conv2d_1('layer20', conv19, [1,1,256,128]) 
    conv21= conv2d_1('layer21', conv20, [3,3,128,256]) 
    ladd22= conv_add('layer22', conv21, conv19)
    conv23= conv2d_1('layer23', ladd22, [1,1,256,128]) 
    conv24= conv2d_1('layer24', conv23, [3,3,128,256]) 
    ladd25= conv_add('layer25', conv24, ladd22)
    conv26= conv2d_1('layer26', ladd25, [1,1,256,128]) 
    conv27= conv2d_1('layer27', conv26, [3,3,128,256]) 
    ladd28= conv_add('layer28', conv27, ladd25)
    conv29= conv2d_1('layer29', ladd28, [1,1,256,128]) 
    conv30= conv2d_1('layer30', conv29, [3,3,128,256]) 
    ladd31= conv_add('layer31', conv30, ladd28)

    conv32= conv2d_2('layer32', ladd31, [3,3,256,512]) #n,15,15
    conv33= conv2d_1('layer33', conv32, [1,1,512,256]) 
    conv34= conv2d_1('layer34', conv33, [3,3,256,512]) 
    ladd35= conv_add('layer35', conv34, conv32)
    conv36= conv2d_1('layer36', ladd35, [1,1,512,256]) 
    conv37= conv2d_1('layer37', conv36, [3,3,256,512]) 
    laddA1= conv_add('layerA1', conv37, ladd35)
    convA2= conv2d_1('layerA2', laddA1, [1,1,512,256]) 
    convA3= conv2d_1('layerA3', convA2, [3,3,256,512]) 
    laddB1= conv_add('layerB1', convA3, laddA1)
    convB2= conv2d_1('layerB2', laddB1, [1,1,512,256]) 
    convB3= conv2d_1('layerB3', convB2, [3,3,256,512]) 
    laddB4= conv_add('layerB4', convB3, laddB1)

    upll39= upscale2('layer39', laddB4, 512, 3)#n, 30, 30
    conv40= conv2d_1('layer40', upll39, [1,1,512,256]) 
    conv41= conv2d_1('layer41', conv40, [3,3,256,512]) 

    upll42= upscale2('layer42', conv41, 256, 3)#n, 60, 60
    conv43= conv2d_1('layer43', upll42, [1,1,256,128]) 
    conv44= conv2d_1('layer44', conv43, [3,3,128,256]) 

    conv45= conv2d_1('layer45', conv44, [3,3,256, 22]) 

    #with tf.name_scope("output"):
    out = tf.sigmoid(conv45, "sigmoid")
    #out = tf.nn.relu(conv45, name = "sigmoid")
    return out
    #60 60 20


memimg = Memimages()
pred = alex_net(x)
pred_a=pred[:,1:-1,1:-1,:20]
pred_b=pred[:,1:-1,1:-1,20:]
y_a=y[:,1:-1,1:-1,:20]
y_b=y[:,1:-1,1:-1,20:]

cast1=tf.reduce_mean(tf.square(pred_a-y_a))
cast2=tf.reduce_mean(tf.square((pred_a-y_a)*y_a))
cast3=tf.reduce_mean(tf.square(pred_b-y_b))
cast=cast1+10*cast2+20*cast3 #100*cast1+10*cast2+cast3
tf.summary.scalar('loss',cast)

optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cast)

#correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(y,1))
#accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))

init = tf.global_variables_initializer()

saver = tf.train.Saver()

with tf.Session() as sess:
    sess.run(init)

    merged = tf.summary.merge_all()
    train_writer = tf.summary.FileWriter("logs/train", sess.graph)
    test_writer = tf.summary.FileWriter("logs/test")

    
    constant_graph = graph_util.convert_variables_to_constants(sess, sess.graph_def, ['sigmoid'])
    #write graph and model to save
    tf.train.write_graph(sess.graph_def, "save", "graph.pbtxt")
    ckpt = tf.train.get_checkpoint_state("./save")
    if ckpt and ckpt.model_checkpoint_path:
        print(ckpt.model_checkpoint_path)
        saver.restore(sess, ckpt.model_checkpoint_path)
    #

    step = 0
    while step * batch_size < training_iters:
        step += 1
        batch_x, batch_y = memimg.get_batch(batch_size)
        train_loss, _ =sess.run([cast, optimizer], feed_dict={x: batch_x, y: batch_y})
        
        #calc loss of train and val
        if step % display_step == 0:
            train_loss, rs=sess.run([cast, merged], feed_dict={x: batch_x, y: batch_y})
            train_writer.add_summary(rs, step)

            batch_x, batch_y = memimg.get_val(batch_size)
            val_loss, p, rs=sess.run([cast, pred, merged], feed_dict={x: batch_x, y: batch_y})
            test_writer.add_summary(rs, step)
            img=batch_x[0].copy()
            img*=255
            cv2.imwrite("batch_x_org.png", img)
            for i in range(1,59):
                for j in range(1,59):
                    if np.max(p[0,i,j,:20])>=0.9:
                        h = p[0,i,j,20]*240
                        w = p[0,i,j,21]*240
                        index = np.where(p[0,i,j,:20]==np.max(p[0,i,j,:20]))
                        cv2.rectangle(img, (int(j*4-w), int(i*4-h)), (int(j*4+w), int(i*4+h)), (0,255,0), 1)
                        cv2.putText(img,memimg.classes[index[0][0]],(int(j*4-w),int(i*4-h)),cv2.FONT_HERSHEY_COMPLEX_SMALL,0.8,(0,255,0), 1)
            cv2.imwrite("batch_x_rect.png", img)
            for i in range(20):
                cv2.imwrite("batch_%s.png" % memimg.classes[i], (np.hstack((np.array(p)[0,1:-1,1:-1,i],np.array(batch_y)[0,1:-1,1:-1,i]))*255).astype("uint8"))
          
            print("Iter " + str(step*batch_size) + ", Train Loss= "+"{:.6f}".format(train_loss)+", Val Loss= "+"{:.6f}".format(val_loss))
        else:
            print("Iter " + str(step*batch_size) + ", Train Loss= "+"{:.6f}".format(train_loss))
        #save model
        if step % save_model_step == 0:
            saver.save(sess, "save/model.ckpt", global_step=step)
            print("save model.ckpt success")

        if step % save_model_step*10 == 0:
            with tf.gfile.FastGFile('./save/model_%08d.pb' % step, mode='wb') as f:
                f.write(constant_graph.SerializeToString())

    print ("Optimization Finished!")


