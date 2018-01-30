import tensorflow as tf  
import numpy as np
import random
import csv
import os

train_num = 5000
val_num = 642
learning_rate = 0.01
iterations = 1000*10
batch_size = 5000


train_x=np.zeros((5642, 39))
train_y=np.zeros(5642)
test_x=np.zeros((1000, 39))
test_y=np.zeros(1000)

def load_train_data():
    with open(r'./data/d_train_20180102.csv', encoding="gbk") as f:
        rowidx=-1
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            if rowidx<0:
                rowidx=0
                continue
            train_x[rowidx, 0]= 1.0 if row[1]=='男' else -1.0
            train_x[rowidx, 1]= float(row[2])
            for i in range(4, len(row)-1):
                if row[i]!='':
                    train_x[rowidx, i-2] = float(row[i])
                else:
                    train_x[rowidx, i-2] = 0
    
            train_y[rowidx]=float(row[41])
    
            rowidx+=1

def load_test_data():

    with open(r'./data/d_test_A_20180102.csv', encoding="gbk") as f:
        rowidx=-1
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            if rowidx<0:
                rowidx=0
                continue
            test_x[rowidx, 0]= 1.0 if row[1]=='男' else -1.0
            test_x[rowidx, 1]= float(row[2])
            for i in range(4, len(row)-1):
                if row[i]!='':
                    test_x[rowidx, i-2] = float(row[i])
                else:
                    test_x[rowidx, i-2] = 0
    
            rowidx+=1

def multilayer_perceptron_with_relu_one_layer(x, weights, biases):  
    


    return out_layer  

def get_batch(num):
    idxlist = range(train_num)
    idxsamp = random.sample(idxlist, num)

    batch_x = train_x[idxsamp]
    batch_y = train_y[idxsamp]
    
    noise_num = random.randint(0, 1000)
    for i in range(noise_num):
        x=random.randint(0, 38)
        y=random.randint(0, num-1)
        batch_x[y,x]=0.0

    return batch_x, batch_y.reshape(num, 1)


load_test_data()
load_train_data()

def train_once(loss=1.6):
    
    min_loss = loss
    os.system("rm -rf ./log/*")
      
    X = tf.placeholder(tf.float64, [None, 39])
    Y = tf.placeholder(tf.float64, [None, 1]) 
      
    b0 = tf.Variable(tf.to_double(tf.random_normal([39])), dtype=tf.float64)    #39
    w1 = tf.Variable(tf.to_double(tf.random_normal([39, 1])), dtype=tf.float64) #39
    b1 = tf.Variable(tf.to_double(tf.random_normal([1])), dtype=tf.float64)     #1
    
    input_layer = tf.add(X, b0)
    hidden_layer_one = tf.matmul(input_layer, w1) + b1
    pred = tf.nn.relu(hidden_layer_one)  


    cost = tf.reduce_mean(tf.pow(tf.subtract(Y, pred), 2))  
    #loss = tf.reduce_mean(tf.exp(tf.abs(tf.subtract(Y, pred))))
    
    optimizer=tf.train.AdamOptimizer(learning_rate).minimize(cost)
      
    init = tf.global_variables_initializer()    
    saver= tf.train.Saver()
    sess = tf.Session()  
    sess.run(init)  
    
    for iteration in range(1, iterations):  
          
        batch_X,batch_Y=get_batch(batch_size)
    
        test_cost = sess.run(cost,feed_dict={X:train_x[train_num:], Y:train_y[train_num:].reshape(val_num, 1)})
              
       
        if test_cost < 10.0:
            if min_loss > test_cost:
                min_loss = test_cost
                os.system("rm result.csv")
                os.system("rm ./model/*")
                print("test cost: ", test_cost, "store into model")
                saver.save(sess, "./model/model.ckpt")

                result = sess.run(pred,feed_dict={X:test_x})
                cf = open('result.csv', 'a')
                wcf = csv.writer(cf)
                for i in result:
                    wcf.writerow([("%.6f" % i[0]), ])

        _,batch_cost=sess.run([optimizer,cost],feed_dict={X:batch_X,Y:batch_Y})  
        print("Iteration :","%04d"%(iteration+1),"Train_cost :","{:.9f}".format(batch_cost),"Test_cost :","{:.9f}".format(test_cost))  
    return min_loss




all_mini_loss=10.0

for i in range(10000):
    all_mini_loss = train_once(all_mini_loss)   










