#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/mman.h>
#include <errno.h>


int check_frame_sum(unsigned char*data, unsigned int num) {
    
    unsigned int i;
    unsigned char sum=0;
    
    if(data == NULL) {
        printf("check frame sum error:\n");
    }

    for(i=0; i<num-1; i++) {
        sum ^= data[i];   
    }

    if(sum == data[num-1])  {
        printf("check frame sum pass!!\n");
    } else {
        printf("check frame sum error: we get sum %02x\n", sum);

    }

}


int main() {

    int i;
    unsigned char check1[]  = { 0xAA, 0x55, 0x3A, 0x11, 0x11, 0x11, 0x11, 0x50, 0x5F,
        0x11, 0x11, 0x6A, 0x1D, 0x09, 0x00, 0x11, 0x11, 0x03,
        0x11, 0x33, 0x35, 0x32, 0x31, 0x2E, 0x31, 0x38, 0x37,
        0x32, 0x31, 0x31, 0x31, 0x31, 0x30, 0x2E, 0x39, 0x38,
        0x35, 0x37, 0x9C };

    unsigned char check2[]  = { 0xAA, 0x55, 0x3A, 0x11, 0x11, 0x11, 0x11, 0x50, 0x5F, 0x11, 
        0x11, 0x5C, 0x1C, 0x09, 0x00, 0x11, 0x11, 0x03, 0x11, 0x33, 
        0x35, 0x32, 0x31, 0x2E, 0x33, 0x31, 0x30, 0x31, 0x31, 0x31, 
        0x31, 0x31, 0x31, 0x2E, 0x30, 0x38, 0x33, 0x32, 0xAF};


    check_frame_sum(&check2[0], 39);

    return 0;

}




