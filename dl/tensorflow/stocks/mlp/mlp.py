from __future__ import print_function
from keras.datasets import cifar10
from keras.preprocessing.image import ImageDataGenerator
from keras.utils import np_utils
from keras.callbacks import ReduceLROnPlateau, CSVLogger, EarlyStopping

import numpy as np
import mlpnet


nb_epoch = 32

batch_size = 1
rows, cols = 32, 1024
datw, dath = 32, 1024
channels = 4

output_num = cols

num_train_samples = 2539
num_test_samples = 2539

lr_reducer = ReduceLROnPlateau(factor=np.sqrt(0.1), cooldown=0, patience=5, min_lr=0.5e-6)
early_stopper = EarlyStopping(min_delta=0.001, patience=10)
csv_logger = CSVLogger('mlp_simple.csv')




def load_data():


    #x_train = np.zeros((num_train_samples, datw, dath, channels), dtype='float16')
    #y_train = np.zeros((num_train_samples,), dtype='float16')
    #x_test = np.zeros((num_test_samples , datw, dath, channels), dtype='float16')
    #y_test = np.zeros((num_test_samples,), dtype='float16')

    x_train = np.fromfile('./x_train.bin', dtype=np.float16)
    y_train = np.fromfile('./y_train.bin', dtype=np.float16)
    x_test = np.fromfile('./x_test.bin', dtype=np.float16)
    y_test = np.fromfile('./y_test.bin', dtype=np.float16)

    x_train = x_train.reshape((num_train_samples, datw, dath, channels))
    y_train = y_train.reshape((num_train_samples, dath))
    x_test = x_test.reshape((num_test_samples, datw, dath, channels))
    y_test = y_test.reshape((num_test_samples, dath))

    x_train = x_train[:,:,:cols,:]
    y_train = y_train[:,:cols]
    x_test = x_test[:,:,:cols,:]
    y_test = y_test[:,:cols]

    return (x_train, y_train), (x_test, y_test)





(X_train, Y_train), (X_test, Y_test) = load_data()


X_train = X_train.astype('float32')
X_test = X_test.astype('float32')
Y_train = Y_train.astype('float32')
Y_test = Y_test.astype('float32')


model = mlpnet.Builder.mlp_simple((rows, cols, channels), output_num)
model.compile(loss='binary_crossentropy',
              optimizer='sgd',
              metrics=['MSE'])

model.fit(X_train, Y_train[:, 51],
          batch_size=batch_size,
          epochs=nb_epoch,
          validation_data=(X_test[:128], Y_test[:128, 51]),
          shuffle=True,
          callbacks=[lr_reducer, early_stopper, csv_logger])


model.save('logits_binary_liner_mse.h5')

