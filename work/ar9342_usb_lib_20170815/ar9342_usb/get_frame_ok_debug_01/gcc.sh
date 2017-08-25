gcc main.c cyusb.c -I./ -I./libusb_install/include -L./libusb_install/lib -lusb-1.0 
#gcc main.c cyusb.c -I./ -I./libusb_install/include -L./libusb_install/lib -lusb-1.0 `pkg-config --cflags --libs opencv`
#gcc get_frame.c -I./ -I./libusb_install/include -L./libusb_install/lib -lusb-1.0 `pkg-config --cflags --libs opencv`
