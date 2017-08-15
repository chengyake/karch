#ifndef libusb_api_h
#define libusb_api_h



#define PID 0x8613
#define VID 0x04b4  



libusb_device_handle *usb_open(int vid, int pid);


int usb_control_transfer(libusb_device_handle *dev_handle,
	uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	unsigned char *data, uint16_t wLength, unsigned int timeout);


int usb_bulk_transfer(libusb_device_handle *dev_handle,
	unsigned char endpoint, unsigned char *data, int length,
	int *actual_length, unsigned int timeout);


int usb_close(libusb_device_handle *dev_handle);






#endif
