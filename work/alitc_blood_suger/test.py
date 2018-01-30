import tensorflow as tf  
import numpy as np
import random
import csv
import os

train_num = 5000
val_num = 642
learning_rate = 1
iterations = 1000

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

def load_param():
    


os.system("rm result.csv")
load_test_data()
load_train_data()


weights=np.zeros(39)
biases =np.zeros(39)













































l,_=train_x.shape
batch_times=int(l/batch_size)

  
X = tf.placeholder("float", [None, 39])  # 28*28  
Y = tf.placeholder("float", [None, 1])  # 1 float
  

  
weights = {"W_layer_one":tf.Variable(tf.random_normal([39, hidden_one])),  
         "W_layer_two":tf.Variable(tf.random_normal([hidden_one, hidden_two])),  
         "W_layer_thr":tf.Variable(tf.random_normal([hidden_two, hidden_thr])),  
         "W_out_layer":tf.Variable(tf.random_normal([hidden_thr, 1]))}  
biases = {"b_layer_one":tf.Variable(tf.random_normal([hidden_one])),  
        "b_layer_two":tf.Variable(tf.random_normal([hidden_two])),  
        "b_layer_thr":tf.Variable(tf.random_normal([hidden_thr])),  
        "b_out_layer":tf.Variable(tf.random_normal([1]))}   


   
  
pred = multilayer_perceptron(X, weights, biases)  
  
cost = tf.reduce_mean(tf.pow(tf.subtract(Y, pred), 2))  
lose = tf.reduce_mean(tf.exp(tf.abs(tf.subtract(Y, pred))))

optimizer=tf.train.AdamOptimizer(learning_rate).minimize(lose)  
  
init = tf.global_variables_initializer()     
sess = tf.Session()  
sess.run(init)  
 
loss_summ = tf.summary.scalar('loss', lose)
summary_op = tf.summary.merge([loss_summ])
train_writer = tf.summary.FileWriter("./log/train", sess.graph)
test_writer = tf.summary.FileWriter("./log/test")

for iteration in range(iterations):  
      
    train_cost=0
    for i in range(batch_times):  
         
        batch_X,batch_Y=get_batch(batch_size)

        _,batch_cost, summary_out=sess.run([optimizer,cost, summary_op],feed_dict={X:batch_X,Y:batch_Y})  
        train_writer.add_summary(summary_out, iteration)
        test_cost, test_out = sess.run([cost, summary_op],feed_dict={X:train_x[train_num:], Y:train_y[train_num:].reshape(val_num, 1)})
        test_writer.add_summary(test_out, iteration)



        train_cost+=batch_cost/batch_times  
          
    if iteration % display_step==0:  
          
        print("Iteration :","%04d"%(iteration+1),"Train_cost :","{:.9f}".format(train_cost))  
   


print("Train_cost :",sess.run(cost,feed_dict={X:train_x[:train_num], Y:train_y[:train_num].reshape(train_num, 1)}))  
print("Test_cost :",sess.run(cost,feed_dict={X:train_x[train_num:], Y:train_y[train_num:].reshape(val_num, 1)}))  
  
result = sess.run(pred,feed_dict={X:test_x})
cf = open('result.csv', 'a')
wcf = csv.writer(cf)
for i in result:
    wcf.writerow([("%.6f" % i[0]), ])



print(result[:20])

