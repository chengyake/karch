#include <unistd.h>  
#include <stdio.h>  
#include <libusb-1.0/libusb.h>  
#include "libusb_api.h"



#define VID 0x04b4  
#define PID 0x8613




int main() {

    libusb_device_handle *handle = usb_open(VID, PID);
    
    usb_close(handle);
}


