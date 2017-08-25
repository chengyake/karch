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

#include <opencv/highgui.h>

// First, use "lsusb" see vid and pid.  
// there is my printer(hp deskjet 1010) vid and pid.  


#define PID 0x8613
#define VID 0x04b4  
#define SIZE_BULK_PKG			(512)
#define SIZE_ERROR_BYTE		    (1026)	//每启动一次读数据命令时,返回的数据中前面的{1026}个是错误的,此常数是测试所得
#define ADDR_EP0				(0xE000)//FX2 的RAM 起始地址(因为其地址空间 0xe000~0xe1ff 共512字节)
#define SIZE_EP0_PKG			(510)	//ep0 共512字节,首2字节作标志,故有效数据长度510
#define FPGADOWNLOAD_INIT		(0xB5)	//fpga下载初始化
#define FPGADOWNLOAD_CONFIG	    (0xB4)	//配置fpga
#define WRITE_ENABLE_COMMAND	(0xB2)	// 写使能
#define READ_ENABLE_COMMAND	    (0xB3)  // 读使能

/*
       image = 0       // usb port 6
controlparam = 1       // usb port 0
   tabledata = 2
   framehead = 3       // usb port 6
   probecode = 4       // NC
       port5 = 5       // NC
     chip_rw = 6
       port7 = 7       // NC
*/


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

IplImage *img;

int fill_buffer_with_file(unsigned char *file, unsigned char *data) {

    int i=0;
    int fd;
    unsigned char str[1024*120]={0};

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

int init_display() {

    img=cvCreateImage(cvSize(512,256), IPL_DEPTH_16S,1);
    cvNamedWindow("Image:",1);
    return 0;
}

int display(char *data) {

    short *dp = (short *)(img->imageData);
    memcpy(img->imageData, (char *)data, 512*256*2);
    cvShowImage("Image:",img);
    cvWaitKey(10);
    return 0;
}

void close_display() {

    cvDestroyWindow("Image:");
    cvReleaseImage(&img);
}


int open_usb(int vid, int pid) {


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
        exit(0);  
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

int close_usb() {

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
                         data, size, &len, 10000);
    printf("BUT:\t");
    for(i=0; i<len&&i<16; i++) {
        //if(i%16==0) printf("\t%d\n", i);
        printf("%02X ", data[i]);
    }
    printf("\t%d\n", len);
    return len;
}

static int usb_bulk_read(libusb_device_handle *hd, unsigned char ep, unsigned char *data, int size) {
    
    int i;
    int len=-1;
    libusb_bulk_transfer(hd, ep,
                         data, size, &len, 10000);
    printf("BIN:\t");
    for(i=0; i<len; i++) {
        if(i%16==0) printf("\t%d\n", i);
        printf("%02x ", data[i]);
    }
    printf("\n");
    return len;
}



static int write_cmd(unsigned char cmd /*setupdata[1]*/, unsigned short port /*setupdata[2-3] 2L 3H*/) {
    unsigned char buff[16];
    return usb_cmd_write(dev_handle, 0xc0, cmd, port, 0x00,  buff, 16, 0x02);
}

static int write_data(unsigned char *data, int size) {

    return usb_bulk_write(dev_handle, 0x02, data, size);
}
static int read_data(unsigned char *data, int size) {

    return usb_bulk_read(dev_handle, 0x86, data, size);
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
    write_cmd(FPGADOWNLOAD_INIT, 0x0000);
    for(l=0; l<len; l+=510) {
        data[0]=0xaa; data[1]=0xaa;
        size=read(fd, &data[2], 510);
        usb_cmd_write(dev_handle, 0x40, 0xa0, ADDR_EP0,  0x0000, &data[0], 0x0200, 512);// latest frame size problem

        write_cmd(FPGADOWNLOAD_CONFIG, 0x0000);
        for(i=0; i<1000; i++) {
            usb_cmd_write(dev_handle, 0xc0, 0xa0, ADDR_EP0,  0x0000, &data[0], 0x0002, 2);
            if(data[0]==0xbb && data[1] == 0xbb) break;
            if(data[0]==0xdd && data[1] == 0xdd) return -1;
            if(data[0]==0xcc && data[1] == 0xcc) return 0;
        }

    }

    close(fd);
}

int init_afe() {

    int i, l;
    unsigned char data[512]; 

    for(i=0; i<17; i++) {

        printf("%s\n", chip_file[i]);
        write_cmd(WRITE_ENABLE_COMMAND, 0x0006);
        l = fill_buffer_with_file(chip_file[i], &data[0]);
        if(l <= 0) return -1;

        write_data(&data[0], l);
    }

    return 0;
}


int init_fpga() {
    int i, l, j;
    unsigned char data[1024*60]; 
    unsigned char data1[1024*60]; 
    for(i=0; i<=9; i++) {
        printf("%s\n", fpga_file[i]);
        write_cmd(WRITE_ENABLE_COMMAND, 0x0001);
        data[0] = 0x06;
        data[1] = i*2+2;
        if(i == 9) data[1] = 22;
        write_data(&data[0], 2);

        write_cmd(WRITE_ENABLE_COMMAND, 0x0002);
        l = fill_buffer_with_file(fpga_file[i], &data[0]);
        if(l <= 0) return -1;
        write_data(&data[0], l);
#if 0
        //compare
        write_cmd(WRITE_ENABLE_COMMAND, 0x0001);
        data[0] = 0x06;
        data[1] = i*2+2;
        if(i == 9) data[1] = 22;
        write_data(&data[0], 2);


        write_cmd(READ_ENABLE_COMMAND, 0x0002);     //3
        read_data(&data1[0], 1026+(l/512)*512+510);                   //4
        l = fill_buffer_with_file(fpga_file[i], &data[0]);
        for(j=0; j<l; j++) {
            if(data[j] != data1[1026+j]) {
                printf("write %d != read %d idx = %d\n", data[j], data1[1026+j], j);
                exit(-1);
            }
        }
#endif
    }
    return 0;
}




int set_params(unsigned char reg, unsigned char value) {

    unsigned char data[512]; 
    write_cmd(WRITE_ENABLE_COMMAND, 0x0001);
    data[0] = reg;
    data[1] = value;
    write_data(&data[0], 2);

    return 0;
}


int init_params() {
    
    set_params(0x0C, 1);
    set_params(0x0D, 4);
    set_params(0x0E, 9);
    set_params(0x0F, 20);
    set_params(0x10, 30);
    set_params(0x12, 100);

    return 0;
}

#if 1
int set_run(int run) {

    unsigned char data[512*3]; 
    write_cmd(WRITE_ENABLE_COMMAND, 0x0001);
    data[0] = 0x07;
    data[1] = (run==0 ? 1 : 0); //0 run: 1 frizze
    write_data(&data[0], 2);

    return 0;
}

#else 
int set_run(int run) {
    
    int i;

    unsigned char data[1024*60]; 
    write_cmd(WRITE_ENABLE_COMMAND, 0x0001);

    data[0] = '1';
    data[1] = '2';
    data[2] = '3';
    data[3] = '6';
    data[4] = '5';
    data[5] = '4';
    data[6] = '7';
    data[7] = '8';
    data[8] = '0';
    data[9] = '9';

    write_data(&data[0], 10);



    write_cmd(READ_ENABLE_COMMAND, 0x0001);     //3
    read_data(&data[0], 512*3);                 //4
    printf("mode is %d\n", data[1025]);         
  
    return 0;
}
#endif

int get_data() {
    int i, l;
    unsigned char tmp[51200]; 
    unsigned char data[512*256*2]={0}; 
    printf("start loop frame data p1\n");
    for(i=0; i<20000; i++) {
        write_cmd(READ_ENABLE_COMMAND, 0x0003);
        l = read_data(&tmp[0], 512*3);
        printf("------get frame %d--------p%d  %d\n", l, tmp[1026+4], i);

        write_cmd(READ_ENABLE_COMMAND, 0x0000);

        if(tmp[1026+4] == 1) {
            l = read_data(&tmp[0], 512*3);
            printf("------get frame %d--------\n", l);
            memcpy(data, &tmp[1026], 510);

            l = read_data(&data[510], 512*511);
            printf("------get frame %d--------\n", l);

            l = read_data(&tmp[0], 512);
            printf("------get frame %d--------\n", l);
            data[512*512-2] = tmp[0];
            data[512*512-1] = tmp[1];

            display(&data[0]);
            //printf("---display--one--frame---\n");
        }
    }
}

int main() {

    
    open_usb(VID, PID);
    
    printf("start ...\n");

    download_firmware();

    init_afe();

    init_fpga();
    
    init_params();

    init_display();

    set_run(1);

    get_data();

    close_usb();
    close_display();
}
