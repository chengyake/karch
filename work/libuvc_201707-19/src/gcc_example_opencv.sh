gcc ctrl.c  ctrl-gen.c  device.c  diag.c  test.c  frame.c  frame-mjpeg.c  init.c  misc.c  stream.c  -I./include -I./include/libuvc -I/home/yake/project/libusb_install/include -L/home/yake/project/libusb_install/lib `pkg-config --cflags --libs opencv` -lusb-1.0 -lpthread -ljpeg -lm   -o test 



#install opencv_2.4.9
