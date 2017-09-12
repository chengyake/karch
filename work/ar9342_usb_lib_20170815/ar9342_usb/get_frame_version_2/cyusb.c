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
#include"cyparams.h"
#ifdef  DEBUG_AR9342
#include <opencv/highgui.h>
#endif


/*
       image = 0       
controlparam = 1       
   tabledata = 2
   framehead = 3       
   probecode = 4       
       port5 = 5       
     chip_rw = 6
       port7 = 7       
*/



#define SIZE_BULK_PKG			(512)
#define SIZE_ERROR_BYTE		    (1026)	//每启动一次读数据命令时,返回的数据中前面的{1026}个是错误的,此常数是测试所得
#define ADDR_EP0				(0xE000)//FX2 的RAM 起始地址(因为其地址空间 0xe000~0xe1ff 共512字节)
#define SIZE_EP0_PKG			(510)	//ep0 共512字节,首2字节作标志,故有效数据长度510
#define FPGADOWNLOAD_INIT		(0xB5)	//fpga下载初始化
#define FPGADOWNLOAD_CONFIG	    (0xB4)	//配置fpga
#define WRITE_ENABLE_COMMAND	(0xB2)	// 写使能
#define READ_ENABLE_COMMAND	    (0xB3)  // 读使能

const unsigned short depth[] = {30, 40, 50, 60, 70, 80};	
const double probe_freq[] = {6.0, 7.0, 8.0, 10.0};	

struct ctrl_s {

    unsigned char reg;
    unsigned char val;
};
int dev_state=-ENODEV;
int depth_index  = 2;
int encode_index = 0;

libusb_context *ctx = NULL;
libusb_device_handle *dev_handle;

int open_usb(int vid, int pid) {

    libusb_device **devs;   
    int ret;  
    ssize_t cnt;  
    ret = libusb_init(&ctx);   
    if(ret < 0) {  
        DEBUG("Init Error\n");  
        return ret;  
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
    ret = libusb_claim_interface(dev_handle, 0);  
    if(ret < 0) {
        DEBUG("Cannot Claim Interface\n");  
        return ret;  
    } 
    DEBUG("Claimed Interface\n");  
    dev_state=0;
    
    return 0;
}

void close_usb() {

    dev_state=-ENODEV;
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

int download_firmware(char *fw) {

    int i,j, len, ret;
    int fd;
    unsigned char data[512]={0};
    
    if(fw == NULL) {
        return -EINVAL;
    }
    if(dev_state<0) {return dev_state;}

    fd=open(fw, O_RDONLY);
    if(fd < 0) {
        return -ENOENT;
    }

    len = lseek(fd, 0, SEEK_END);
    if(len <= 0 ) {
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


int m_usb_Write(unsigned char port, unsigned char *data, unsigned int size) {
    
    unsigned int ret;
    ret = write_cmd(WRITE_ENABLE_COMMAND, port);
    if(ret < 0) return ret;

    ret = write_data(data, size);
    return ret;
}

int write_ctrl_1(struct ctrl_s *ctrl) {
	return m_usb_Write(1, (unsigned char *)ctrl, 2);	
}


int set_data_table(unsigned char reg_value, unsigned char* pData, unsigned int ulSize)
{   
    int ret;
	struct ctrl_s ctrl;
	ctrl.reg = 0x06;
	ctrl.val = reg_value;
	ret = write_ctrl_1(&ctrl);
	if(ret < 0) return ret;

	return m_usb_Write(2, pData, ulSize);

}

int write_ctrl_2(int Addr, unsigned char *pcbData, int uSize) {
    
    int index=0;
	struct ctrl_s ctrl;
	memset(&ctrl, 0, sizeof(ctrl));

	for (index=0; index<uSize; index++)
	{
		ctrl.reg = Addr + index;
		ctrl.val = pcbData[index];
		write_ctrl_1(&ctrl);
	}
	return 0;
}

int set_run(int run) {

    int ret;
	unsigned char cbRun = run ? 0 : 1;

    if(dev_state<0) {return dev_state;}

	ret = write_ctrl_2(0x07, (unsigned char*)&cbRun, sizeof(cbRun));
	if(ret < 0) return ret;

	dev_state = cbRun;
}

int is_run() {
    return dev_state;
}


double get_freq(int index) {
	if (index < 0 || index > 4) return probe_freq[0];
	return probe_freq[index];
}

int set_freq_index(int index) {

	unsigned char cbFreqIndex = 0;

    if(dev_state<0) {return dev_state;}
	if (index < 0 || index > 4) return -1;

	if (index==0)
	{ 
		cbFreqIndex=8;
	}
	else if (index==1)
	{
		cbFreqIndex=10;
	}
	else if (index==2)
	{
		cbFreqIndex=12;
	}
	else
	{
		cbFreqIndex=13;
	}
 	return write_ctrl_2(0x0D, (unsigned char*)&cbFreqIndex, sizeof(cbFreqIndex));
}

int get_depth(int index) {
	if (index < 0 || index >= 6) return depth[0];
	return depth[index];
}

int set_depth(int index) {   
    int ret;
	unsigned char ratio = 5;
    if(dev_state<0) {return dev_state;}
	return write_ctrl_2(0x0E, (unsigned char*)&ratio, sizeof(ratio));
}

int set_encode_index(int index) {
	unsigned char idx = index;
    if(dev_state<0) {return dev_state;}
	return write_ctrl_2(0x0C, (unsigned char*)&idx, sizeof(idx));
}

int set_dtgc_hold_time(unsigned char time) {
    if(dev_state<0) {return dev_state;}
	return write_ctrl_2(0x0F, (unsigned char*)&time, sizeof(time));
}

int set_beam_start_dot(unsigned short dot) {
    if(dev_state<0) {return dev_state;}
    return write_ctrl_2(0x10, (unsigned char*)&dot, sizeof(dot));
}

int set_rx_data_start_dot(unsigned short dot) {
    if(dev_state<0) {return dev_state;}
	return write_ctrl_2(0x12, (unsigned char*)&dot, sizeof(dot));
}


int init_afe() {

    if(dev_state<0) {return dev_state;}
	unsigned char DATA_CHIP_1[4] = {1,0,0,207};
    unsigned char DATA_CHIP_2[4] = {16,0,1,207};
	unsigned char DATA_CHIP_3[4] = {64,1,209,207};
	unsigned char DATA_CHIP_4[4] = {1,0,218,207};
	unsigned char DATA_CHIP_5[4] = {32,0,225,207};
	unsigned char DATA_CHIP_6[4] = {128,0,2,207};
	unsigned char DATA_CHIP_7[4] = {0,0,1,207};
	unsigned char DATA_CHIP_8[4] = {227,0,22,207};
	unsigned char DATA_CHIP_9[4] = {0,0,15,207};
	unsigned char DATA_CHIP_10[4] = {68,68,18,207};
	unsigned char DATA_CHIP_11[4] = {255,0,20,207};
	unsigned char DATA_CHIP_12[4] = {0,0,37,207};
	unsigned char DATA_CHIP_13[4] = {0,0,42,207};
	unsigned char DATA_CHIP_14[4] = {0,0,43,207};
	unsigned char DATA_CHIP_15[4] = {197,128,66,207};
	unsigned char DATA_CHIP_16[4] = {0,0,69,207};
	unsigned char DATA_CHIP_17[4] = {28,162,70,207};
	m_usb_Write(6, DATA_CHIP_1, 4);	
	m_usb_Write(6, DATA_CHIP_2, 4);
	m_usb_Write(6, DATA_CHIP_3, 4);
	m_usb_Write(6, DATA_CHIP_4, 4);
	m_usb_Write(6, DATA_CHIP_5, 4);
	m_usb_Write(6, DATA_CHIP_6, 4);
	m_usb_Write(6, DATA_CHIP_7, 4);
	m_usb_Write(6, DATA_CHIP_8, 4);
	m_usb_Write(6, DATA_CHIP_9, 4);
	m_usb_Write(6, DATA_CHIP_10, 4);
	m_usb_Write(6, DATA_CHIP_11, 4);
	m_usb_Write(6, DATA_CHIP_12, 4);
	m_usb_Write(6, DATA_CHIP_13, 4);
	m_usb_Write(6, DATA_CHIP_14, 4);
	m_usb_Write(6, DATA_CHIP_15, 4);
	m_usb_Write(6, DATA_CHIP_16, 4);
	m_usb_Write(6, DATA_CHIP_17, 4);

	return 0;
}


int init_fpga() {   

    if(dev_state<0) {return dev_state;}
	set_data_table(2, FPGA_2, 256*6);
	set_data_table(6, FPGA_6, 256);
	set_data_table(18, FPGA_18, 256);
	set_data_table(8, FPGA_8, 16);
	set_data_table(4, FPGA_4, 128*128);
	set_data_table(14, FPGA_TRASMIT, 16);
	set_data_table(16, FPGA_RECEIVE, 24576);
	set_data_table(22, FPGA_RXWEIGHT, 24576);
    set_data_table(10, FPGA_10, 4096);
	set_data_table(12, FPGA_12, 22000);
	return 0;
}

int init_params() {

    if(dev_state<0) {return dev_state;}
	set_encode_index(0);
	set_depth(2);
	set_freq_index(0);
	set_dtgc_hold_time(20);
	set_beam_start_dot(80);
	set_rx_data_start_dot(270);
	return 0;
}

int get_frame(unsigned char *data, int size) {

    int i, ret;
    int ret_time=-ETIME;
    unsigned char tmp[512*3]; 
     
    if(dev_state<1) {return -1;}
    if(data == NULL || size < FRAME_SIZE) {return -EINVAL;}

    DEBUG("start loop frame data p1\n");
    for(i=0; i<2000; i++) {

        ret = write_cmd(READ_ENABLE_COMMAND, 0x0003);
        if(ret < 0) {return ret;}

        ret = read_data(&tmp[0], 512*3);
        if(ret < 0) {return ret;}
        DEBUG("------get frame %d--------p%d  %d\n", ret, tmp[1026+4], i);

        if(tmp[1026+4] == 1) {
            ret = write_cmd(READ_ENABLE_COMMAND, 0x0000);

            ret |= read_data(&tmp[0], 512*3);
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






