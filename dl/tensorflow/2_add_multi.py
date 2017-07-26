# -*- coding: utf-8 -*-

import tensorflow as tf

input1 = tf.constant(2.0)
input2 = tf.constant(3.0)
input3 = tf.constant(5.0)

intermd = tf.add(input1, input2)
mul = tf.multiply(input2, input3)

with tf.Session() as sess:
    result = sess.run([mul, intermd])  # 一次执行多个op
    print result
    print type(result)
    print type(result[0])  




