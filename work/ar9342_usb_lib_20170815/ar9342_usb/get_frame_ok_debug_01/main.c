#include<stdio.h>  
#include<stdlib.h>

#include "cyusb.h"



int main() {

    int i;
    int ret;
    unsigned char frame[512*256*2];

    ret = open_usb(VID, PID);
    if(ret < 0) {return ret;}

    ret = download_firmware();
    if(ret < 0) {return ret;}

    ret = init_afe();
    if(ret < 0) {return ret;}

    ret = init_fpga();
    if(ret < 0) {return ret;}

    ret = init_params();
    if(ret < 0) {return ret;}
#ifdef  DEBUG_AR9342
    init_display();
#endif
    set_run(1);
    
    printf("pull frame 100...\n");
    for(i=0; i<100; i++) {
        get_frame(&frame[0], FRAME_SIZE);
#ifdef  DEBUG_AR9342
        show_frame(&frame[0]);
        //printf("chengyake show frame\n");
#endif
    }
    printf("pull frame over...\n");

    close_usb();
#ifdef DEBUG_AR9342
    close_display();
#endif
}


