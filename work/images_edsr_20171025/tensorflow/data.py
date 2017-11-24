import scipy.misc
import numpy as np
import random
import os

mu1_set = []
trs_set = []



"""
Load set of images in a directory.
This will automatically allocate a 
random 20% of the images as a test set

data_dir: path to directory containing images
"""
def load_dataset(data_dir):
    global mu1_set, trs_set
    mu1_set = get_file_list('.bmp', "./images_register/mu1_register")
    trs_set = get_file_list('.bmp', "./images_register/trs_register")
    mu1_set.sort()
    trs_set.sort()

    return

"""
Get test set from the loaded dataset

size (optional): if this argument is chosen,
each element of the test set will be cropped
to the first (size x size) pixels in the image.

returns the test set of your data
"""
def get_test_set(original_size,shrunk_size):
    global mu1_set, trs_set
    y_imgs = []
    x_imgs = []
    for i in range(80, 90) :
	    img = scipy.misc.imread(mu1_set[i])
	    img = crop_center(img,original_size,original_size)
	    x_img = scipy.misc.imresize(img,(shrunk_size,shrunk_size))
	    x_imgs.append(x_img)

	    img = scipy.misc.imread(trs_set[i])
	    img = crop_center(img,original_size,original_size)
	    y_imgs.append(img)

    x_imgs = np.array(x_imgs)
    x_imgs = x_imgs.reshape(x_imgs.shape+(1,))
    y_imgs = np.array(y_imgs)
    y_imgs = y_imgs.reshape(y_imgs.shape+(1,))
    return x_imgs,y_imgs

"""
Get a batch of images from the training
set of images.

batch_size: size of the batch
original_size: size for target images
shrunk_size: size for shrunk images

returns x,y where:
	-x is the input set of shape [-1,shrunk_size,shrunk_size,channels]
	-y is the target set of shape [-1,original_size,original_size,channels]
"""
def get_batch(batch_size,original_size,shrunk_size):
	x =[]
	y =[]
	global mu1_set, trs_set
	img_indices = random.sample(range(80),batch_size)
	for i in range(len(img_indices)):
		index = img_indices[i]
		img = scipy.misc.imread(mu1_set[index])
		img = crop_center(img,original_size,original_size)
		x_img = scipy.misc.imresize(img,(shrunk_size,shrunk_size))
		x.append(x_img)

		img = scipy.misc.imread(trs_set[index])
		img = crop_center(img,original_size,original_size)
		y.append(img)
	x = np.array(x)
	x = x.reshape(x.shape+(1,))
	y = np.array(y)
	y = y.reshape(y.shape+(1,))
	return x,y

"""
Simple method to crop center of image

img: image to crop
cropx: width of crop
cropy: height of crop
returns cropped image
"""
def crop_center(img,cropx,cropy):
	y,x = img.shape
	startx = x//2-(cropx//2)
	starty = y//2-(cropy//2)    
	return img[starty:starty+cropy,startx:startx+cropx]


def get_file_list(fmt, d):
    l=[]
    if not (os.path.exists(d)):
        print "folder %s is missing" % d
        return
    for path, dirname, files in os.walk(d):
        for filename in files:
            if filename.find(fmt)!=-1:
                l.append(path+"/"+filename)

    return l


