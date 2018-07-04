'''
A Convolutional Network implementation example using TensorFlow library.
This example is using the MNIST database of handwritten digits
'''

import tensorflow as tf

from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("./data", one_hot=True)

learning_rate = 0.001
training_iters = 200000
batch_size = 128
display_step = 1

n_inputs = 784
n_classes = 10 

layer_num=4
layer_nodes=1024



x = tf.placeholder(tf.float32, [None, n_inputs])
y = tf.placeholder(tf.float32, [None, n_classes])

def bn_layer(inputs,is_training=True,name='BatchNorm',moving_decay=0.9,eps=1e-5):
    shape = inputs.shape
    assert len(shape) in [2,4]

    param_shape = shape[-1]

    gamma = tf.Variable(tf.ones(param_shape), name='gamma')
    beta  = tf.Variable(tf.zeros(param_shape), name='beta')
    mean  = tf.Variable(tf.ones(param_shape), trainable=False, name='mean')
    var   = tf.Variable(tf.ones(param_shape), trainable=False, name='var')

    if is_training == True:
        batch_mean, batch_var = tf.nn.moments(inputs,[0],name='moments')
        mean = tf.assign(mean, batch_mean)
        var = tf.assign(var, batch_var)
        return tf.nn.batch_normalization(inputs,batch_mean+mean*1e-8,batch_var+var*1e-8,gamma,beta,eps)
    else:
        return tf.nn.batch_normalization(inputs,mean,var,gamma,beta,eps)

def full_layer(name,x, w, b):
    x1=tf.matmul(x, w) + b
    x2=bn_layer(x1)
    return tf.nn.leaky_relu(x2)





weights = {
    'w1': tf.Variable(tf.random_normal([n_inputs, layer_nodes])),
    'w2': tf.Variable(tf.random_normal([layer_nodes, layer_nodes])),
    'w3': tf.Variable(tf.random_normal([layer_nodes, layer_nodes])),
    'w4': tf.Variable(tf.random_normal([layer_nodes, n_classes])),
}
biases = {
    'b1': tf.Variable(tf.random_normal([layer_nodes])),
    'b2': tf.Variable(tf.random_normal([layer_nodes])),
    'b3': tf.Variable(tf.random_normal([layer_nodes])),
    'b4': tf.Variable(tf.random_normal([n_classes])),
  }

def alex_net(inputs):

    layer1 = full_layer("layer1", inputs, weights['w1'], biases['b1'])
    layer2 = full_layer("layer2", layer1, weights['w2'], biases['b2'])
    layer3 = full_layer("layer3", layer2, weights['w3'], biases['b3'])
    layer4 = full_layer("layer4", layer3, weights['w4'], biases['b4'])

    return layer4



pred = alex_net(x)

cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y, logits=pred))
optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cost)

correct_pred = tf.equal(tf.argmax(pred,1), tf.argmax(y,1))
accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))

init = tf.global_variables_initializer()

with tf.Session() as sess:
    sess.run(init)
    step = 1
    while step * batch_size < training_iters:
        batch_x, batch_y = mnist.train.next_batch(batch_size)
        sess.run(optimizer, feed_dict={x: batch_x, y: batch_y})
        if step % display_step == 0:
            loss,acc = sess.run([cost,accuracy], feed_dict={x: batch_x, y: batch_y})
            print ("Iter " + str(step*batch_size) + ", Minibatch Loss= " + "{:.6f}".format(loss) + ", Training Accuracy= " + "{:.5f}".format(acc))
        step += 1
    print ("Optimization Finished!")
    print ("Testing Accuracy:",
           sess.run(accuracy, feed_dict={x: mnist.test.images[:256],
                                         y: mnist.test.labels[:256]}))



