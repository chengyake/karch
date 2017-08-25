#include<unistd.h>  
#include<stdio.h>  
#include<stdlib.h>
#include<time.h>
#include<fcntl.h>
#include<malloc.h>  
#include<memory.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<libusb-1.0/libusb.h>  
#include<error.h>
#include<errno.h>

#include"cyusb.h"

#ifdef  DEBUG_AR9342
#include <opencv/highgui.h>
#endif


// First, use "lsusb" see vid and pid.  
// there is my printer(hp deskjet 1010) vid and pid.  


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

#ifdef  DEBUG_AR9342
IplImage *img;
#endif
static int fill_buffer_with_file(unsigned char *file, unsigned char *data) {

    int fd, idx=0;
    unsigned char str[1024*120]={0};

    fd=open(file, O_RDONLY);
    if(fd < 0)  {
        DEBUG("No such file\n");
        return -ENOENT;
    }

    lseek(fd, 0, SEEK_SET);
    read(fd, &str[0], 1024*120);
    close(fd);

    char delims[] = "\r\n";
    char *seg = NULL;
    seg = strtok(str, delims);
    while( seg != NULL ) {
        data[idx++] = atoi(seg);
        seg = strtok( NULL, delims );
    }     
    
    return idx;
}

#ifdef  DEBUG_AR9342
int init_display() {

    img=cvCreateImage(cvSize(512,256), IPL_DEPTH_16S,1);
    cvNamedWindow("Image:",1);
    return 0;
}

int show_frame(unsigned char *data) {

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
#endif

int open_usb(int vid, int pid) {


    libusb_device **devs;   
    int r;  
    ssize_t cnt;  
    r = libusb_init(&ctx);   
    if(r < 0) {  
        DEBUG("Init Error\n");  
        return r;  
    }  
    libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);  

    cnt = libusb_get_device_list(ctx, &devs);  
    if(cnt < 0) {  
        DEBUG("Get Device Error\n");  
        return cnt;  
    }  
    DEBUG("%lu Devices in list.\n", cnt);  

    dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid);  
    if(dev_handle == NULL)  {
        DEBUG("Cannot open device\n");  
        libusb_exit(ctx);  
        return -ENODEV;  
    }
    DEBUG("Device Opened\n");  
    libusb_free_device_list(devs, 1);  

    if(libusb_kernel_driver_active(dev_handle, 0) == 1) { 
        DEBUG("Kernel Driver Active\n");  
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0)  
            DEBUG("Kernel Driver Detached!\n");  
    }
    r = libusb_claim_interface(dev_handle, 0);  
    if(r < 0) {  
        DEBUG("Cannot Claim Interface\n");  
        return r;  
    }  
    DEBUG("Claimed Interface\n");  
    
    return 0;
}

void close_usb() {

    libusb_close(dev_handle); //close the device we opened  
    libusb_exit(ctx); //needs to be called to end the  
}

static int usb_cmd_write(libusb_device_handle *hd, unsigned char rtype, unsigned char req/*setupdata[1]*/,
                        unsigned short value/*setupdata[2-3] 2L 3H*/,
                        unsigned short index/*setupdata[4-5] 4L 5H*/, unsigned char *data, unsigned short len, unsigned int rsp) {  
    int i, ret;
    int interface = 0;  
    ret = libusb_control_transfer(hd, rtype,  
            req,  
            value,  
            index,  
            data, len, 5000);  

#ifdef  DEBUG_AR9342
    DEBUG("CTL:\t%02x %02x %02x %02x %02x %02x %02x %02x\n",
            rtype, req, value&0x00FF, (value&0xFF00)>>8,  index&0x00FF, (index&0xFF00)>>8, len&0x00FF, (len&0xFF00)>>8);  

    if(rsp > 0) {
        (rtype&0x80) ? printf("IN :\t") : printf("CUT:\t");
        for(i=0; i<rsp; i++) {
            printf("%02x ", data[i]);  
        }
        printf("\n");
    }
#endif
    return ret;  
}

static int usb_bulk_write(libusb_device_handle *hd, unsigned char ep, unsigned char *data, int size) {
    
    int i, ret;
    int len=-1;
    ret = libusb_bulk_transfer(hd, ep,
                         data, size, &len, 10000);
    if(ret < 0) {
        DEBUG("usb bulk write failed\n");
        return -1;
    }
#ifdef  DEBUG_AR9342
    printf("BUT:\t");
    for(i=0; i<len&&i<16; i++) {
        //if(i%16==0) DEBUG("\t%d\n", i);
        printf("%02X ", data[i]);
    }
    printf("\t%d\n", len);
#endif
    return len;
}

static int usb_bulk_read(libusb_device_handle *hd, unsigned char ep, unsigned char *data, int size) {
    
    int i, ret;
    int len=-1;
    ret = libusb_bulk_transfer(hd, ep,
                         data, size, &len, 10000);
    if(ret < 0) {
        DEBUG("usb bulk read failed\n");
        return -1;
    }
#ifdef  DEBUG_AR9342
    DEBUG("BIN:\t");
    for(i=0; i<len; i++) {
        if(i%16==0) DEBUG("\t%d\n", i);
        DEBUG("%02x ", data[i]);
    }
    DEBUG("\n");
#endif
    return len;
}



static int write_cmd(unsigned char cmd, unsigned short port) {
    unsigned char buff[16];
    return usb_cmd_write(dev_handle, 0xc0, cmd, port, 0x00,  buff, 16, 0x02);
}

static int write_data(unsigned char *data, int size) {

    return usb_bulk_write(dev_handle, 0x02, data, size);
}
static int read_data(unsigned char *data, int size) {

    return usb_bulk_read(dev_handle, 0x86, data, size);
}




int download_firmware() {

    int i,j, len, ret;
    int fd;
    unsigned char data[512]={0};

    fd=open("./MU_40M.bin",O_RDONLY);
    if(fd < 0) {
        return fd;
    }

    len = lseek(fd, 0, SEEK_END);
    if(len < 0 ) {
        DEBUG("lseek file error\n");
        return len;
    }
   
    lseek(fd, 0, SEEK_SET);
    ret = write_cmd(FPGADOWNLOAD_INIT, 0x0000);
    if(ret < 0) {return ret;}

    for(j=0; j<len; j+=510) {
        data[0]=0xaa; data[1]=0xaa;

        if(len - j >= 510) { 
            read(fd, &data[2], 510);
            usb_cmd_write(dev_handle, 0x40, 0xa0, ADDR_EP0,  0x0000, &data[0], 0x0200, 512);
        } else {
            read(fd, &data[2], len-j);
            usb_cmd_write(dev_handle, 0x40, 0xa0, ADDR_EP0,  0x0000, &data[0], 0x0200, len-j+2);
        }

        ret = write_cmd(FPGADOWNLOAD_CONFIG, 0x0000);
        if(ret < 0) {return ret;}
        for(i=0; i<1000; i++) {
            usb_cmd_write(dev_handle, 0xc0, 0xa0, ADDR_EP0,  0x0000, &data[0], 0x0002, 2);
            if(data[0]==0xbb && data[1] == 0xbb) break;
            if(data[0]==0xdd && data[1] == 0xdd) return -EIO;
            if(data[0]==0xcc && data[1] == 0xcc) return 0;
        }

    }

    close(fd);
    return 0;
}

int init_afe() {

    int i, ret;
    unsigned char data[512]; 

    for(i=0; i<17; i++) {
        DEBUG("%s\n", chip_file[i]);
        write_cmd(WRITE_ENABLE_COMMAND, 0x0006);

        ret = fill_buffer_with_file(chip_file[i], &data[0]);
        if(ret < 0) return ret;

        write_data(&data[0], ret);
    }

    return 0;
}


int init_fpga() {
    int i, j, ret;
    unsigned char data[1024*60]; 
    //unsigned char data1[1024*60]; 
    for(i=0; i<=9; i++) {
        DEBUG("%s\n", fpga_file[i]);
        ret = write_cmd(WRITE_ENABLE_COMMAND, 0x0001);
        if(ret < 0) {return ret;}
        data[0] = 0x06;
        data[1] = i*2+2;
        if(i == 9) data[1] = 22;
        write_data(&data[0], 2);

        write_cmd(WRITE_ENABLE_COMMAND, 0x0002);
        ret = fill_buffer_with_file(fpga_file[i], &data[0]);
        if(ret < 0) return ret;
        write_data(&data[0], ret);
#if 0
        //compare // bug: first power up, check fail

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
                DEBUG("write %d != read %d idx = %d\n", data[j], data1[1026+j], j);
                exit(-1);
            }
        }
#endif
    }
    return 0;
}




static int set_params(unsigned char reg, unsigned char value) {
    
    int ret;
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

int set_run(int run) {
    
    unsigned int ret;
    unsigned char data[512*3]; 
    write_cmd(WRITE_ENABLE_COMMAND, 0x0001);

    data[0] = 0x07;
    data[1] = (run==0 ? 1 : 0); //0 run: 1 frizze
    write_data(&data[0], 2);

    return 0;
}
int get_frame(unsigned char *data, int size) {

    int i, ret;
    int ret_time=-62;
    unsigned char tmp[512*3]; 
     
    if(data == NULL) {return -2;}
    if(size < FRAME_SIZE){return -3;}

    DEBUG("start loop frame data p1\n");
    for(i=0; i<2000; i++) {

        write_cmd(READ_ENABLE_COMMAND, 0x0003);

        ret = read_data(&tmp[0], 512*3);
        if(ret < 0) {return ret;}
        DEBUG("------get frame %d--------p%d  %d\n", ret, tmp[1026+4], i);

        if(tmp[1026+4] == 1) {
            write_cmd(READ_ENABLE_COMMAND, 0x0000);

            read_data(&tmp[0], 512*3);
            memcpy(data, &tmp[1026], 510);

            read_data(&data[510], 512*511);

            read_data(&tmp[0], 512);
            data[512*512-2] = tmp[0];
            data[512*512-1] = tmp[1];

            ret_time = 0;
            break;
        }

    }
    return ret_time;
}


