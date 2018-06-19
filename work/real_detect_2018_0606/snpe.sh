opt_dlc() {
source /opt/snpe-1.15.0/bin/envsetup.sh -t /usr/local/lib/python2.7/dist-packages/tensorflow
python /usr/local/lib/python2.7/dist-packages/tensorflow/python/tools/optimize_for_inference.py --input $1 --output tmp_opt.pb --input_names reshape --output_names sigmoid
/opt/snpe-1.15.0/bin/x86_64-linux-clang/snpe-tensorflow-to-dlc --graph tmp_opt.pb --input_dim reshape 240,240,3 --out_node sigmoid  --dlc $1.dlc  --allow_unconsumed_nodes
/opt/snpe-1.15.0/bin/x86_64-linux-clang/snpe-dlc-quantize --input_dlc model_yake.pb.dlc --input_list raw_list.txt --output_dlc model_yake_quantized.dlc
}

#/opt/snpe-1.15.0/bin/x86_64-linux-clang/snpe-net-run -container model_yake.pb.dlc --input_list raw_list.txt
#/opt/snpe-1.15.0/bin/x86_64-linux-clang/snpe-dlc-info  -i model_yake.pb.dlc 
