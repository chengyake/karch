#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>
#include <libusb-1.0/libusb.h>  
#include <pthread.h>
#include <time.h>
#include<malloc.h>  
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<memory.h>

// First, use "lsusb" see vid and pid.  
// there is my printer(hp deskjet 1010) vid and pid.  


#define PID 0x8613
#define VID 0x04b4  



char chip_file[17][21] = {
    {"./CHIP/W_CHIP_1.txt"}, 
    {"./CHIP/W_CHIP_2.txt"},
    {"./CHIP/W_CHIP_3.txt"},
    {"./CHIP/W_CHIP_4.txt"},
    {"./CHIP/W_CHIP_5.txt"},
    {"./CHIP/W_CHIP_6.txt"},
    {"./CHIP/W_CHIP_7.txt"},
    {"./CHIP/W_CHIP_8.txt"},
    {"./CHIP/W_CHIP_9.txt"},
    {"./CHIP/W_CHIP_10.txt"},
    {"./CHIP/W_CHIP_11.txt"}, 
    {"./CHIP/W_CHIP_12.txt"},
    {"./CHIP/W_CHIP_13.txt"},
    {"./CHIP/W_CHIP_14.txt"},
    {"./CHIP/W_CHIP_15.txt"},
    {"./CHIP/W_CHIP_16.txt"},
    {"./CHIP/W_CHIP_17.txt"}
};

char fpga_file[10][21] = {
    {"./FPGA/W_FPGA_2.txt"}, 
    {"./FPGA/W_FPGA_4.txt"},
    {"./FPGA/W_FPGA_6.txt"},
    {"./FPGA/W_FPGA_8.txt"},
    {"./FPGA/W_FPGA_10.txt"}, 
    {"./FPGA/W_FPGA_12.txt"},
    {"./FPGA/W_FPGA_14.txt"},
    {"./FPGA/W_FPGA_16.txt"},
    {"./FPGA/W_FPGA_18.txt"},
    {"./FPGA/W_FPGA_22.txt"},
};

libusb_context *ctx = NULL;
libusb_device_handle *dev_handle;

int usb_open(int vid, int pid) {


    libusb_device **devs;   
    int r;  
    ssize_t cnt;  
    r = libusb_init(&ctx);   
    if(r < 0) {  
        perror("Init Error");  
        return 1;  
    }  
    libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);  

    cnt = libusb_get_device_list(ctx, &devs);  
    if(cnt < 0) {  
        perror("Get Device Error\n");  
        return 1;  
    }  
    printf("%lu Devices in list.\n", cnt);  

    dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid);  
    if(dev_handle == NULL)  {
        perror("Cannot open device\n");  
        libusb_exit(ctx);  
        return 0;  
    } else { 
        printf("Device Opened\n");  
    }
    libusb_free_device_list(devs, 1);  

    if(libusb_kernel_driver_active(dev_handle, 0) == 1) { 
        printf("Kernel Driver Active\n");  
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0)  
            printf("Kernel Driver Detached!\n");  
    }  
    r = libusb_claim_interface(dev_handle, 0);  
    if(r < 0) {  
        perror("Cannot Claim Interface\n");  
        return 1;  
    }  
    printf("Claimed Interface\n");  
    
    return 0;
}

int usb_close() {

    libusb_close(dev_handle); //close the device we opened  
    libusb_exit(ctx); //needs to be called to end the  

}

static int usb_cmd_write(libusb_device_handle *hd, unsigned char rtype, unsigned char req/*setupdata[1]*/,
                        unsigned short value/*setupdata[2-3] 2L 3H*/,
                        unsigned short index/*setupdata[4-5] 4L 5H*/, unsigned char *data, unsigned short len, unsigned int rsp) {  
    
    int interface = 0, i;  
    libusb_control_transfer(hd, rtype,  
            req,  
            value,  
            index,  
            data, len, 5000);  

    printf("CTL:\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
            rtype, req, value&0x00FF, (value&0xFF00)>>8,  index&0x00FF, (index&0xFF00)>>8, len&0x00FF, (len&0xFF00)>>8);  
    if(rsp > 0) {
        (rtype&0x80) ? printf("IN :\t") : printf("CUT:\t");
        for(i=0; i<rsp; i++) {
            printf("%02x ", data[i]);  
        }
        printf("\n");
    }
    return 0;  
}

static int usb_bulk_write(libusb_device_handle *hd, unsigned char ep, unsigned char *data, int size) {
    
    int i;
    int len=-1;
    libusb_bulk_transfer(hd, ep,
                         data, size, &len, 1000);
    printf("BUT:\t");
    for(i=0; i<len&&i<30; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
    return len;
}

static int usb_bulk_read(libusb_device_handle *hd, unsigned char ep, unsigned char *data, int size) {
    
    int i;
    int len=-1;
    libusb_bulk_transfer(hd, ep,
                         data, size, &len, 1000);
    printf("BIN:\t");
    for(i=0; i<len; i++) {
        if(i%16==0) printf("\n");
        printf("%02x ", data[i]);
    }
    printf("\n");
    return len;
}

static int download_firmware() {

    int i,l, len, ret;
    int fd,size;
    unsigned char data[512]={0};

    fd=open("./MU_40M.bin",O_RDONLY);


    len = lseek(fd, 0, SEEK_END);
    if(len <=0 ) {
        printf("lseek file error\n");
        return -1;
    }
   
    lseek(fd, 0, SEEK_SET);
    usb_cmd_write(dev_handle, 0xc0, 0xb5, 0x0000, 0x0000, &data[0], 0x0002, 2);
    for(l=0; l<len; l+=510) {
        data[0]=0xaa; data[1]=0xaa;
        size=read(fd, &data[2], 510);
        usb_cmd_write(dev_handle, 0x40, 0xa0, 0xe000,  0x0000, &data[0], 0x0200, 512);

        usb_cmd_write(dev_handle, 0xc0, 0xb4, 0x0000,  0x0000, &data[0], 0x0002, 2);
        for(i=0; i<1000; i++) {
            usb_cmd_write(dev_handle, 0xc0, 0xa0, 0xe000,  0x0000, &data[0], 0x0002, 2);
            if(data[0]==0xbb && data[1] == 0xbb) break;
            if(data[0]==0xdd && data[1] == 0xdd) return -1;
            if(data[0]==0xcc && data[1] == 0xcc) return 0;
        }

    }

    close(fd);
}



int fill_buffer_with_file(unsigned char *file, unsigned char *data) {

    int i=0;
    int fd;
    unsigned char str[1024*120];

    fd=open(file, O_RDONLY);
    if(fd < 0)  {
        perror("No such file\n");
        return -1;
    }

    lseek(fd, 0, SEEK_SET);
    read(fd, &str[0], 1024*120);
    close(fd);

    char delims[] = "\r\n";
    char *seg = NULL;
    seg = strtok(str, delims);
    while( seg != NULL ) {
        data[i++] = atoi(seg);
        seg = strtok( NULL, delims );
    }     
    
    return i;
}




















int main() {
    int i, l;
    unsigned char data[1024*60]; 
    
    usb_open(VID, PID);
    
    printf("start ...");



    download_firmware();



// init AFE
    for(i=0; i<17; i++) {
        usb_cmd_write(dev_handle, 0xc0, 0xb2, 0x0001, 0x0000, &data[0], 0x0002, 2);

        printf("%s\n", chip_file[i]);
        usb_cmd_write(dev_handle, 0xc0, 0xb2, 0x0006, 0x0000, &data[0], 0x0002, 2);
        l = fill_buffer_with_file(chip_file[i], &data[0]);
        if(l <= 0) return -1;

        usb_bulk_write(dev_handle, 0x02, &data[0], l);
    }


//init FPGA
    for(i=0; i<=9; i++) {
        usb_cmd_write(dev_handle, 0xc0, 0xb2, 0x0001, 0x0000, &data[0], 0x0002, 2);
        data[0] = 0x06;
        data[1] = i*2+2;
        if(i == 9) data[1] = 22;
        usb_bulk_write(dev_handle, 0x02, &data[0], 2);

        usb_cmd_write(dev_handle, 0xc0, 0xb2, 0x0002, 0x0000, &data[0], 0x0002, 2);
        printf("%s\n", fpga_file[i]);
        l = fill_buffer_with_file(fpga_file[i], &data[0]);
        if(l <= 0) return -1;

        usb_bulk_write(dev_handle, 0x02, &data[0], l);
    }




//RUN Frizze

    usb_cmd_write(dev_handle, 0xc0, 0xb2, 0x0001, 0x0000, &data[0], 0x0002, 2);
    data[0] = 0x07;
    data[1] = 0x00;
    usb_bulk_write(dev_handle, 0x02, &data[0], 2);



//get data
    for(i=0; i<2000; i++) {
        data[0] = 0x00;
        data[1] = 0x00;
        usb_cmd_write(dev_handle, 0xc0, 0xb3, 0x0003, 0x0000, &data[0], 0x0002, 2);
        printf("------get frame %d--------p?%d\n", l, data[4]);
        l = usb_bulk_read(dev_handle, 0x86, &data[0], 1536);


        usb_cmd_write(dev_handle, 0xc0, 0xb3, 0x0000, 0x0000, &data[0], 0x0002, 2);
        printf("------get frame %d--------\n", l);
        l = usb_bulk_read(dev_handle, 0x86, &data[0], 1536);
        printf("------get frame %d--------\n", l);
        l = usb_bulk_read(dev_handle, 0x86, &data[0], 51184+16);
        printf("------get frame %d--------\n", l);
        l = usb_bulk_read(dev_handle, 0x86, &data[0], 496+16);

    }

    usb_close();

}
