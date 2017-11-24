import data
import argparse
from model import EDSR
parser = argparse.ArgumentParser()
parser.add_argument("--dataset",default="images_register")
parser.add_argument("--imgsize",default=320,type=int)
parser.add_argument("--scale",default=2,type=int)
parser.add_argument("--layers",default=16,type=int)
parser.add_argument("--featuresize",default=128,type=int)
parser.add_argument("--batchsize",default=10,type=int)
parser.add_argument("--savedir",default='saved_models')
parser.add_argument("--iterations",default=400,type=int)
args = parser.parse_args()
data.load_dataset(args.dataset)
down_size = args.imgsize//args.scale
network = EDSR(down_size,args.layers,args.featuresize,args.scale, output_channels=1)
network.set_data_fn(data.get_batch,(args.batchsize,args.imgsize,down_size),data.get_test_set,(args.imgsize,down_size))
network.train(args.iterations,args.savedir)
