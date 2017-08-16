#include <unistd.h>  
#include <stdio.h>  
#include <libusb-1.0/libusb.h>  

libusb_context *ctx = NULL;   

libusb_device_handle *usb_open(int vid, int pid) {

    libusb_device **devs;   
    libusb_device_handle *dev_handle;   
    int r;   
    ssize_t cnt;  

    if(ctx != NULL) {
        perror("Repeat init Error");  
        return NULL;
    }

    r = libusb_init(&ctx);  
    if(r < 0) {  
        perror("Init Error");  
        return NULL;  
    }  
    libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);  

    cnt = libusb_get_device_list(ctx, &devs);  
    if(cnt < 0) {  
        perror("Get Device Error\n"); 
        return NULL;  
    }  

    dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid); 
    if(dev_handle == NULL)  {
        perror("Cannot open device\n");  
        libusb_exit(ctx);
        return NULL;  
    }

    libusb_free_device_list(devs, 1);

    if(libusb_kernel_driver_active(dev_handle, 0) == 1) {
        perror("Kernel Driver Active\n");  
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0)
            perror("Kernel Driver Detached!\n");  
    }  
    r = libusb_claim_interface(dev_handle, 0);
    if(r < 0) {  
        perror("Cannot Claim Interface\n");  
        libusb_close(dev_handle); 
        libusb_exit(ctx);
        return NULL;  
    }  

    return dev_handle;
}

int usb_control_transfer(libusb_device_handle *dev_handle,
	uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	unsigned char *data, uint16_t wLength, unsigned int timeout) {
	
	return libusb_control_transfer(dev_handle, request_type, bRequest, wValue, wIndex, data, wLength, timeout);
}

int usb_bulk_transfer(libusb_device_handle *dev_handle,
	unsigned char endpoint, unsigned char *data, int length,
	int *actual_length, unsigned int timeout) {

    return usb_bulk_transfer(dev_handle, endpoint, data, length, actual_length, timeout);
}



int usb_close(libusb_device_handle *dev_handle) {

    libusb_close(dev_handle);
    libusb_exit(ctx);
    ctx=NULL;
}


 
