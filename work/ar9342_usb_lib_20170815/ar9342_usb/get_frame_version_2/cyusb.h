#ifndef CYUSB_H
#define CYUSB_H



//#define DEBUG_AR9342
#ifdef  DEBUG_AR9342
#define DEBUG(format, ...) printf (format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif



#define PID 0x8613
#define VID 0x04b4

//fixed frame size, can't redefine
#define FRAME_SIZE  (512*256*2)




int open_usb(int vid, int pid);
void close_usb();

int download_firmware();
int init_afe();
int init_fpga();
int init_params();




int set_freq_index(int index);
int set_depth(int index);
int set_encode_index(int index);
int set_dtgc_hold_time(unsigned char time);
int set_beam_start_dot(unsigned short dot);
int set_rx_data_start_dot(unsigned short dot);
int set_run(int run);



int get_depth(int index);
double get_freq(int index);

/*
 * 1. if(data size !=  FRAME_SIZE) return -invalid_argument(-22)
 * 2. if(loop_times > 2000) return -timeout(-62)
 */
int get_frame(unsigned char *data, int size);









#endif

