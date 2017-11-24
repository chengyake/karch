from model import EDSR
import scipy.misc
import argparse
import data
import os
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("--dataset",default="data/General-100")
parser.add_argument("--imgsize",default=320,type=int)
parser.add_argument("--scale",default=2,type=int)
parser.add_argument("--layers",default=16,type=int)
parser.add_argument("--featuresize",default=128,type=int)
parser.add_argument("--batchsize",default=10,type=int)
parser.add_argument("--savedir",default="saved_models")
parser.add_argument("--iterations",default=400,type=int)
parser.add_argument("--numimgs",default=5,type=int)
parser.add_argument("--outdir",default="out")
parser.add_argument("--image")
args = parser.parse_args()
if not os.path.exists(args.outdir):
	os.mkdir(args.outdir)
data.load_dataset(args.dataset)
down_size = args.imgsize//args.scale
network = EDSR(down_size,args.layers,args.featuresize,scale=args.scale)
network.resume(args.savedir)
if args.image:
	x = scipy.misc.imread(args.image)
else:
	print("No image argument given")
inputs = x
x = np.array(x)
x = x.reshape(x.shape+(1,))
outputs = network.predict(x)
shape = outputs.shape
outputs = outputs.reshape((shape[0], shape[1]))
if args.image:
	scipy.misc.imsave(args.outdir+"/input_"+args.image,inputs)
	scipy.misc.imsave(args.outdir+"/output_"+args.image,outputs)
