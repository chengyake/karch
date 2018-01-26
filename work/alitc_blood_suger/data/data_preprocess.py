import csv
import numpy as np

train_x=np.zeros((5642, 39))
train_y=np.zeros(5642)

test_x=np.zeros((1000, 39))

def load_train_data():

    with open(r'd_train_20180102.csv', encoding="gbk") as f:
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

    with open(r'd_test_A_20180102.csv', encoding="gbk") as f:
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





load_test_data()
load_train_data()

print(train_y)
print(test_x[999])
