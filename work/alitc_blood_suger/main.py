import tensorflow as tf  
import numpy as np
import random
import csv
import os

train_num = 5000
val_num = 642
learning_rate = 0.01
iterations = 1000*200
batch_size = 4500
display_step = 1  

hidden_one = 5
hidden_two = 5
hidden_thr = 5

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

    with open(r'./data/d_test_B_20180128.csv', encoding="gbk") as f:
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
    
    input_layer = tf.divide(x, 100)
    hidden_layer_one = tf.matmul(input_layer, weights["W_layer_one"]) + biases["b_layer_one"]  
    hidden_layer_one = tf.nn.relu(hidden_layer_one)  


    out_layer = tf.matmul(hidden_layer_one, weights["W_out_layer"]) + biases["b_out_layer"]
    out_layer = tf.nn.relu(out_layer)
    return out_layer  

def multilayer_perceptron_with_relu(x, weights, biases):  
    
    input_layer = tf.divide(x, 100)
    hidden_layer_one = tf.matmul(input_layer, weights["W_layer_one"]) + biases["b_layer_one"]  
    hidden_layer_one = tf.nn.relu(hidden_layer_one)  

    hidden_layer_two = tf.matmul(hidden_layer_one, weights["W_layer_two"]) + biases["b_layer_two"]  
    hidden_layer_two = tf.nn.relu(hidden_layer_two)  
     
    hidden_layer_thr = tf.matmul(hidden_layer_two, weights["W_layer_thr"]) + biases["b_layer_thr"]  
    hidden_layer_thr = tf.nn.relu(hidden_layer_thr)  

    out_layer = tf.matmul(hidden_layer_thr, weights["W_out_layer"]) + biases["b_out_layer"]
    out_layer = tf.nn.relu(out_layer)
    return out_layer  

def multilayer_perceptron(x, weights, biases):  
    
    input_layer = tf.divide(x, 100)
    hidden_layer_one = tf.matmul(input_layer, weights["W_layer_one"]) + biases["b_layer_one"]  
    hidden_layer_one = tf.nn.sigmoid(hidden_layer_one)  

    hidden_layer_two = tf.matmul(hidden_layer_one, weights["W_layer_two"]) + biases["b_layer_two"]  
    hidden_layer_two = tf.nn.sigmoid(hidden_layer_two)  
     
    hidden_layer_thr = tf.matmul(hidden_layer_two, weights["W_layer_thr"]) + biases["b_layer_thr"]  
    hidden_layer_thr = tf.nn.sigmoid(hidden_layer_thr)  

    out_layer = tf.matmul(hidden_layer_thr, weights["W_out_layer"]) + biases["b_out_layer"]
    out_layer = tf.nn.sigmoid(out_layer)
    out_layer = tf.multiply(out_layer, 10)
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




os.system("rm result.csv")
os.system("rm -rf ./log/*")
load_test_data()
load_train_data()


l,_=train_x.shape
batch_times=int(l/batch_size)

  
X = tf.placeholder(tf.float64, [None, 39])  # 28*28  
Y = tf.placeholder(tf.float64, [None, 1])  # 1 float
  

  
weights = {"W_layer_one":tf.Variable(tf.to_double(tf.random_normal([39, hidden_one])), dtype=tf.float64),  
         "W_layer_two":tf.Variable(tf.to_double(tf.random_normal([hidden_one, hidden_two])), dtype=tf.float64),  
         "W_layer_thr":tf.Variable(tf.to_double(tf.random_normal([hidden_two, hidden_thr])), dtype=tf.float64),  
         "W_out_layer":tf.Variable(tf.to_double(tf.random_normal([hidden_thr, 1])), dtype=tf.float64)}  
biases = {"b_layer_one":tf.Variable(tf.to_double(tf.random_normal([hidden_one])), dtype=tf.float64),  
        "b_layer_two":tf.Variable(tf.to_double(tf.random_normal([hidden_two])), dtype=tf.float64),  
        "b_layer_thr":tf.Variable(tf.to_double(tf.random_normal([hidden_thr])), dtype=tf.float64),  
        "b_out_layer":tf.Variable(tf.to_double(tf.random_normal([1])),dtype=tf.float64)}   


   
  
pred = multilayer_perceptron_with_relu(X, weights, biases)  
  
cost = tf.reduce_mean(tf.pow(tf.subtract(Y, pred), 2))  
loss = tf.reduce_mean(tf.exp(tf.abs(tf.subtract(Y, pred))))

#optimizer=tf.train.AdamOptimizer(learning_rate).minimize(tf.add(tf.multiply(cost, 10), loss))  
#optimizer=tf.train.AdamOptimizer(learning_rate).minimize(loss)  
optimizer=tf.train.AdamOptimizer(learning_rate).minimize(cost)  
#optimizer=tf.train.AdamOptimizer(learning_rate).minimize(tf.add(loss, tf.multiply(cost, 100)))  
  
init = tf.global_variables_initializer()    
saver= tf.train.Saver()
sess = tf.Session()  
sess.run(init)  
 
loss_summ = tf.summary.scalar('loss', loss)
summary_op = tf.summary.merge([loss_summ])
train_writer = tf.summary.FileWriter("./log/train", sess.graph)
test_writer = tf.summary.FileWriter("./log/test")

if os.path.exists("./model/model.ckpt.meta"):
    print("restore default model.ckpt-------------xxxxxxxxxxxx---------xxxxxxx")
    saver.restore(sess, "./model/model.ckpt")
mini_cost = 100.0
for iteration in range(1, iterations):  
      
    train_cost=0

    for i in range(batch_times):  
         
        batch_X,batch_Y=get_batch(batch_size)

        _,batch_cost, summary_out=sess.run([optimizer,cost, summary_op],feed_dict={X:batch_X,Y:batch_Y})  
        train_writer.add_summary(summary_out, iteration)
        test_cost, test_out = sess.run([cost, summary_op],feed_dict={X:train_x[train_num:], Y:train_y[train_num:].reshape(val_num, 1)})
        test_writer.add_summary(test_out, iteration)

        #train_cost+=batch_cost/batch_times  
          
    if iteration % display_step==0:  
          
        print("Iteration :","%04d"%(iteration+1),"Train_cost :","{:.9f}".format(batch_cost),"Test_cost :","{:.9f}".format(test_cost))  
   
    if iteration % 2000==0:
        print("xxstore default model.ckpt")
        saver.save(sess, "./model/model.ckpt")
    
    if test_cost < mini_cost and test_cost < 2.2:
        os.system("rm result.csv")
        print("----------------we get mini test cost: ", test_cost)
        mini_cost = test_cost
        result = sess.run(pred,feed_dict={X:test_x})
        cf = open('result.csv', 'a')
        wcf = csv.writer(cf)
        for i in result:
            wcf.writerow([("%.6f" % i[0]), ])



print("Train_cost :",sess.run(cost,feed_dict={X:train_x[:train_num], Y:train_y[:train_num].reshape(train_num, 1)}))  
print("Test_cost :",sess.run(cost,feed_dict={X:train_x[train_num:], Y:train_y[train_num:].reshape(val_num, 1)}))  
  


