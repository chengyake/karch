/*
 *  Component: qn9021 driver
 *
 *  driver of ble for xmm2231
 *  Copyright (C) 2014
 *  Contributor(s): 2014-1-26 Yake.Cheng ^.^
 *
 */

#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/ioctl.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/wakelock.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/atomic.h>
#include <asm/unistd.h>
#include <nk/nkern.h>
#include <linux/platform_device.h>
//socket
#include<linux/in.h>
#include<linux/inet.h>
#include<linux/socket.h>
#include<net/sock.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/sched.h>
#include<linux/kthread.h>
#include<linux/err.h>
#include<linux/timer.h>
#include<linux/jiffies.h>
#include <linux/input.h>

#include "qn902x_ble_fu.h"

//test_reset
char test_reset = 0;

#define SENSOR_COMPENSATE

#ifdef SENSOR_COMPENSATE
static int trim_flag;
static struct t_bmc050_mag_dig
{
  signed char dig_x1;
  signed char dig_y1;
  signed char dig_x2;
  signed char dig_y2;
  unsigned short dig_z1;
  signed short dig_z2;
  signed short dig_z3;
  signed short dig_z4;
  unsigned char dig_xy1;
  signed char dig_xy2;
  unsigned short dig_xyz1;
} bmc050_mag_dig_regs;
#endif

//vble_activity_timeout
#define DELAY_TIME 10
#define BLE_ACTIVITY_RETRY_MAX  5
struct timer_list vble_activity_timer;
char timeout_init_ok = 0;
int timeout_reset_count = 0;

//static int sleep;
static short acc_x, acc_y, acc_z;
static short ecomp_x, ecomp_y, ecomp_z;

//static int qn902x_sw;
ble_info vble_info;

struct wake_lock vble_wake_lock;
vble_dev *vble_devp=NULL;

//socket
static struct socket *server_sock = NULL;
static struct socket *client_sock = NULL;
static struct socket *status_server_sock = NULL;
static struct socket *status_client_sock = NULL;
static struct socket *step_server_sock = NULL;
static struct socket *step_client_sock = NULL;
static struct socket *step_client_sock_tmp = NULL;
static unsigned char socket_recvbuf[1024] = {0};

//firmware update
unsigned char ble_mode = NORMAL;

//input
static struct input_dev*	inputdev_acc;
static struct input_dev*	inputdev_comp;

#define G_MAX			2000
/* INPUT_ABS CONSTANTS */
#define FUZZ			0
#define FLAT			0


static int input_acc_register(void )
{

	inputdev_acc = input_allocate_device();
	if (inputdev_acc == NULL) {
		printk(KERN_ERR "not enough memory for input device \n" );
		return -ENOMEM;
	}

    inputdev_acc->name = "acc_event";
    inputdev_acc->id.bustype = BUS_VIRTUAL;

    //acc
    set_bit(EV_ABS, inputdev_acc->evbit);
    input_set_abs_params(inputdev_acc, ABS_X, -G_MAX, G_MAX, FUZZ, FLAT);
    input_set_abs_params(inputdev_acc, ABS_Y, -G_MAX, G_MAX, FUZZ, FLAT);
    input_set_abs_params(inputdev_acc, ABS_Z, -G_MAX, G_MAX, FUZZ, FLAT);

    if (input_register_device(inputdev_acc)) {
        printk(KERN_ERR "cannot register acm input device\n");
        return -EAGAIN;
    }
    return 0;

}

static int input_comp_register(void )
{

	inputdev_comp = input_allocate_device();
	if (inputdev_comp == NULL) {
		printk(KERN_ERR "not enough memory for input device \n" );
		return -ENOMEM;
	}

    inputdev_comp->name = "comp_event";
    inputdev_comp->id.bustype = BUS_VIRTUAL;

    //comp
    set_bit(EV_ABS, inputdev_comp->evbit);
    input_set_abs_params(inputdev_comp, ABS_HAT0X, -2048, 2032, 0, 0);
    input_set_abs_params(inputdev_comp, ABS_HAT0Y, -2048, 2032, 0, 0);
    input_set_abs_params(inputdev_comp, ABS_BRAKE, -2048, 2032, 0, 0);

    if (input_register_device(inputdev_comp)) {
        printk(KERN_ERR "cannot register acm input device\n");
        return -EAGAIN;
    }
    return 0;

}
#ifdef SENSOR_COMPENSATE
#define BMC050_MAG_OVERFLOW_OUTPUT       -32768 // compensated output value returned if sensor had overflow
#define BMC050_MAG_FLIP_OVERFLOW_ADCVAL  -4096 // Flipcore overflow ADC value
#define BMC050_MAG_HALL_OVERFLOW_ADCVAL  -16384 // Hall overflow 1 ADC value
#define BMC050_MAG_NEGATIVE_SATURATION_Z            -32767           // Negative Saturation for Z Axis
#define BMC050_MAG_POSITIVE_SATURATION_Z            32767            //Positive Saturation for Z Axis

short bmc050_mag_compensate_X(short mdata_x, unsigned short data_R)
{
    short inter_retval;
    if (mdata_x != BMC050_MAG_FLIP_OVERFLOW_ADCVAL )  /* no overflow */
    {
        inter_retval = ((short)(((unsigned short)
                        ((((int)bmc050_mag_dig_regs.dig_xyz1) << 14) /
                         (data_R != 0 ? data_R : bmc050_mag_dig_regs.dig_xyz1))) -
                    ((unsigned short)0x4000)));
        inter_retval = ((short)((((int)mdata_x) *
                        ((((((((int)bmc050_mag_dig_regs.dig_xy2) *
                              ((((int)inter_retval) *
                                ((int)inter_retval)) >> 7)) +
                             (((int)inter_retval) *
                              ((int)(((short)bmc050_mag_dig_regs.dig_xy1)
                                  << 7)))) >> 9) +
                           ((int)0x100000)) *
                          ((int)(((short)bmc050_mag_dig_regs.dig_x2) +
                              ((short)0xA0)))) >> 12)) >> 13)) +
            (((short)bmc050_mag_dig_regs.dig_x1) << 3);
    }
    else
    {
        /* overflow */
        inter_retval = BMC050_MAG_OVERFLOW_OUTPUT;
    }
    return inter_retval;
}


short bmc050_mag_compensate_Y(short mdata_y, unsigned short data_R)
{
    short inter_retval;
    if (mdata_y != BMC050_MAG_FLIP_OVERFLOW_ADCVAL   )/* no overflow */
    {
        inter_retval = ((short)(((unsigned short)(((
                                    (int)bmc050_mag_dig_regs.dig_xyz1) << 14) /
                            (data_R != 0 ?
                             data_R : bmc050_mag_dig_regs.dig_xyz1))) -
                    ((unsigned short)0x4000)));
        inter_retval = ((short)((((int)mdata_y) *
                        ((((((((int)
                               bmc050_mag_dig_regs.dig_xy2) *
                              ((((int) inter_retval) *
                                ((int)inter_retval)) >> 7)) +
                             (((int)inter_retval) *
                              ((int)(((short)
                                      bmc050_mag_dig_regs.dig_xy1) << 7)))) >> 9) +
                           ((int)0x100000)) *
                          ((int)(((short)bmc050_mag_dig_regs.dig_y2)
                              + ((short)0xA0))))
                         >> 12)) >> 13)) +
            (((short)bmc050_mag_dig_regs.dig_y1) << 3);
    }
    else
    {
        /* overflow */
        inter_retval = BMC050_MAG_OVERFLOW_OUTPUT;
    }
    return inter_retval;
}


short bmc050_mag_compensate_Z(short mdata_z, unsigned short data_R)
{
    int retval;
    if ((mdata_z != BMC050_MAG_HALL_OVERFLOW_ADCVAL) ) /* no overflow */
    {
        retval = (((((int)(mdata_z - bmc050_mag_dig_regs.dig_z4)) << 15) -
                    ((((int)bmc050_mag_dig_regs.dig_z3) *
                      ((int)(((short)data_R) -
                          ((short)
                           bmc050_mag_dig_regs.dig_xyz1))))>>2)) /
                (bmc050_mag_dig_regs.dig_z2 +
                 ((short)(((((int)
                                 bmc050_mag_dig_regs.dig_z1) *
                             ((((short)data_R) << 1)))+
                         (1<<15))>>16))));
        /* saturate result to +/- 2 mT */
        if (retval > BMC050_MAG_POSITIVE_SATURATION_Z)
        {
            retval =  BMC050_MAG_POSITIVE_SATURATION_Z;
        }
        else
        {
            if (retval < BMC050_MAG_NEGATIVE_SATURATION_Z)
                retval = BMC050_MAG_NEGATIVE_SATURATION_Z;
        }
    }
    else
    {
        /* overflow */
        retval = BMC050_MAG_OVERFLOW_OUTPUT;
    }
    return (short)retval;
}

void init_trim_register(void)
{

    trim_flag=1;
    bmc050_mag_dig_regs.dig_x1 = (signed char)vble_devp->fb_info->vring->buffer[3];
    bmc050_mag_dig_regs.dig_y1 = (signed char)vble_devp->fb_info->vring->buffer[4];
    bmc050_mag_dig_regs.dig_x2 = (signed char)vble_devp->fb_info->vring->buffer[10];
    bmc050_mag_dig_regs.dig_y2 = (signed char)vble_devp->fb_info->vring->buffer[11];
    bmc050_mag_dig_regs.dig_z1 = (unsigned short)(((unsigned short)vble_devp->fb_info->vring->buffer[17] << 8 )
            | vble_devp->fb_info->vring->buffer[16]);
    bmc050_mag_dig_regs.dig_z2 = (short)(((short) ((signed char)vble_devp->fb_info->vring->buffer[15]) << 8 )
            | vble_devp->fb_info->vring->buffer[14]);
    bmc050_mag_dig_regs.dig_z3 = (short)(((short)((signed char)vble_devp->fb_info->vring->buffer[21])<<8 )
            | vble_devp->fb_info->vring->buffer[20]);
    bmc050_mag_dig_regs.dig_z4 = (short)(((short)((signed char)vble_devp->fb_info->vring->buffer[9]) <<8)
            | vble_devp->fb_info->vring->buffer[8]);
    bmc050_mag_dig_regs.dig_xy1 =(unsigned char)vble_devp->fb_info->vring->buffer[23];
    bmc050_mag_dig_regs.dig_xy2 = (signed char)vble_devp->fb_info->vring->buffer[22];
    bmc050_mag_dig_regs.dig_xyz1 =(unsigned short)(((unsigned short)vble_devp->fb_info->vring->buffer[19] <<8 )
            | vble_devp->fb_info->vring->buffer[18]);

    bmc050_mag_dig_regs.dig_xyz1 = bmc050_mag_dig_regs.dig_xyz1 & 0x7FFF;

}

#endif



void input_report_acc(unsigned char *data)
{
    short raw_x, raw_y, raw_z;
    raw_x = (((data[0] & 0xc0) >> 6) | (data[1] << 2)) & 0x03FF;
    raw_y = (((data[2] & 0xc0) >> 6) | (data[3] << 2)) & 0x03FF;
    raw_z = (((data[4] & 0xc0) >> 6) | (data[5] << 2)) & 0x03FF;

    raw_x = raw_x << 6;
    acc_x = raw_x >> 6;
    raw_y = raw_y << 6;
    acc_y = raw_y >> 6;
    raw_z = raw_z << 6;
    acc_z = raw_z >> 6;

    //printk("vble: g-sensor %d, %d, %d\n", acc_x, acc_y, acc_z);

}

void input_report_ecomp(unsigned char *data)
{
    short raw_dataX, raw_dataY, raw_dataZ;
    unsigned short raw_dataR, raw_tmp;

    raw_tmp = data[1] << 8;
    raw_tmp |= data[0] & 0xf8;
    raw_dataX = (((short)raw_tmp) >> 3);

    raw_tmp = data[3] << 8;
    raw_tmp |= data[2] & 0xf8;
    raw_dataY = (((short)raw_tmp) >> 3);

    raw_tmp = data[5] << 8;
    raw_tmp |= data[4] & 0xfe;
    raw_dataZ = (((short)raw_tmp) >> 1);

    raw_tmp = data[7] << 8;
    raw_tmp |= data[6] & 0xfc;
    raw_dataR = (((unsigned short)raw_tmp) >> 2);

#ifndef SENSOR_COMPENSATE
    ecomp_x = raw_dataX;
    ecomp_y = raw_dataY;
    ecomp_z = raw_dataZ;
#else
    if(trim_flag==1) {
        ecomp_x = bmc050_mag_compensate_X(raw_dataX, raw_dataR);
        ecomp_y = bmc050_mag_compensate_Y(raw_dataY, raw_dataR);
        ecomp_z = bmc050_mag_compensate_Z(raw_dataZ, raw_dataR);
    } else {
        ecomp_x = raw_dataX;
        ecomp_y = raw_dataY;
        ecomp_z = raw_dataZ;
    }
#endif
	//printk("vble: e sensor %d, %d, %d\n", ecomp_x, ecomp_y, ecomp_z);
}

int vble_socket_get(int length)
{

    int ret=0;
    struct kvec vec;
    struct msghdr msg;

    memset(&msg,0,sizeof(msg));
    vec.iov_base=socket_recvbuf;
    vec.iov_len=1024;

    ret=kernel_recvmsg(client_sock,&msg,&vec,1,length,0); /*receive message*/

    return ret;

}
int vble_socket_get_send(unsigned char* data , int length)
{
    wake_lock_timeout(&vble_wake_lock, 2 * HZ);
    //DEBUG("server: receive len=%d message: %s\n",ret,socket_recvbuf);
    DEBUG("vble_socket receive: len=%d \n",length);
    //send ble cmd
    mutex_lock(&vble_devp->lock);
    vble_devp->fb_info->vring->length = length+3;
    vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
    vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_QPPS_SEND_DATA;
    vble_devp->fb_info->vring->buffer[2]= length; //length MAX = 20
    memcpy(&vble_devp->fb_info->vring->buffer[3], data, length);
    vble_send_cmd_mex_resp(CMD_SEND_DATA);
    mutex_unlock(&vble_devp->lock);

    return 0;
}
int vble_socket_task(void*data){

    struct sockaddr_in s_addr;
    unsigned short portnum=8888;
    int ret=0;
    //struct kvec vec;
    //struct msghdr msg;

    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(portnum);
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*create a socket*/
    ret=sock_create_kern(AF_INET, SOCK_STREAM,0,&server_sock);
    if(ret){
        ERROR("vble server:socket_create error = %d\n",ret);
        return -1;
    }
    DEBUG("vble server: socket_create ok!\n");

    /*bind the socket*/
	ret=kernel_bind(server_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if (ret< 0) {
        ERROR("vble server: bind error = %d\n",ret);
        goto err0;
    }
    DEBUG("vble server: bind ok!\n");

    /*listen*/
	ret=kernel_listen(server_sock, 5);
    if(ret<0){
        ERROR("vble server: listen error = %d\n",ret);
        goto err0;
    }
    DEBUG("vble server: listen ok!\n");

	/*accept*/
	for(;;)
	{
        ret=kernel_accept(server_sock, &client_sock, 0);
        if (ret<0) {
            ERROR("vble server: accept error = %d\n",ret);
            goto err0;
        }
        DEBUG("vble server: accept ok!\n");

        /*receive message from client*/
        for(;;)
        {
            int ex_break = 0;
            int len_send_max = 20;
            int len_send = 0;
            int len_get = 0;
            int len_packet = 0;
            unsigned char buf_send[len_send_max];
            memset(&buf_send,0,sizeof(buf_send));

            //memset(&msg,0,sizeof(msg));
            //vec.iov_base=socket_recvbuf;
            //vec.iov_len=1024;
            //ret=kernel_recvmsg(client_sock,&msg,&vec,1,2,0); /*receive message*/
            ret = vble_socket_get(2);
            if(ret<=0){
                ERROR("vble server: receive message error = %d\n",ret);	/*Connection is broken*/
                break;
            }
            memcpy(buf_send+len_send,socket_recvbuf,ret);
            len_send += ret;

            if(socket_recvbuf[0] ==0x66)
            {
                if(socket_recvbuf[1] ==0x0C)
                {
                    //memset(&msg,0,sizeof(msg));
                    //vec.iov_base=socket_recvbuf;
                    //vec.iov_len=1024;
                    //ret=kernel_recvmsg(client_sock,&msg,&vec,1,2,0); /*receive message*/
                    ret = vble_socket_get(2);
                    if(ret<=0){
                        ERROR("vble server: receive message error = %d\n",ret); /*Connection is broken*/
                        break;
                    }
                    memcpy(buf_send+len_send,socket_recvbuf,ret);
                    len_send += ret;
                    len_packet = (buf_send[2]<<8) + buf_send[3];
                    len_get = len_send_max - len_send;
                }
                else
                {
                    //memset(&msg,0,sizeof(msg));
                    //vec.iov_base=socket_recvbuf;
                    //vec.iov_len=1024;
                    //ret=kernel_recvmsg(client_sock,&msg,&vec,1,1,0); /*receive message*/
                    ret = vble_socket_get(1);
                    if(ret<=0){
                        ERROR("vble server: receive message error = %d\n",ret); /*Connection is broken*/
                        break;
                    }
                    memcpy(buf_send+len_send,socket_recvbuf,ret);
                    len_send += ret;
                    len_packet = buf_send[2];
                    len_get = len_send_max - len_send;
                }

                while(len_packet > len_get)
                {
                    //memset(&msg,0,sizeof(msg));
                    //vec.iov_base=socket_recvbuf;
                    //vec.iov_len=1024;
                    //ret=kernel_recvmsg(client_sock,&msg,&vec,1,len_get,0); /*receive message*/
                    ret = vble_socket_get(len_get);
                    if(ret<=0){
                        ERROR("vble server: receive message error = %d\n",ret); /*Connection is broken*/
                        ex_break = 1;
                        break;
                    }
                    memcpy(buf_send+len_send,socket_recvbuf,ret);
                    len_send += ret;
                    len_packet -= ret;
                    len_get = len_send_max;

                    vble_socket_get_send(buf_send,len_send);
                    len_send = 0;

                }
                if(ex_break == 1)
                {
                    ex_break = 0;
                    break;
                }
                if(len_packet > 0)
                {
                    len_get = len_packet;

                    //memset(&msg,0,sizeof(msg));
                    //vec.iov_base=socket_recvbuf;
                    //vec.iov_len=1024;
                    //ret=kernel_recvmsg(client_sock,&msg,&vec,1,len_get,0); /*receive message*/
                    ret = vble_socket_get(len_get);
                    if(ret<=0){
                        ERROR("vble server: receive message error = %d\n",ret); /*Connection is broken*/
                        break;
                    }
                    memcpy(buf_send+len_send,socket_recvbuf,ret);
                    len_send += ret;

                    vble_socket_get_send(buf_send,len_send);
                    len_send = 0;
                }
                else if(len_packet == 0)
                {
                    vble_socket_get_send(buf_send,len_send);
                    len_send = 0;
                }
            }//if
        }//for
	}

	/*release socket*/
	//sock_release(client_sock);
err0:   sock_release(server_sock);

    return ret;
}

int status_socket_task(void*data){

    struct sockaddr_in s_addr;
    unsigned short portnum=8890;
    int ret=0;
    struct kvec vec;
    struct msghdr msg;
    unsigned char status_socket_recvbuf[256]={0};

    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(portnum);
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*create a socket*/
    ret=sock_create_kern(AF_INET, SOCK_STREAM,0,&status_server_sock);
    if(ret){
        ERROR("status server:socket_create error = %d\n",ret);
        return -1;
    }
    DEBUG("status server: socket_create ok!\n");

    /*bind the socket*/
	ret=kernel_bind(status_server_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if (ret< 0) {
        ERROR("status server: bind error = %d\n",ret);
        goto err0;
    }
    DEBUG("status server: bind ok!\n");

    /*listen*/
	ret=kernel_listen(status_server_sock, 5);
    if(ret<0){
        ERROR("status server: listen error = %d\n",ret);
        goto err0;
    }
    DEBUG("status server: listen ok!\n");

	/*accept*/
	for(;;)
	{
        ret=kernel_accept(status_server_sock, &status_client_sock, 0);
        if (ret<0) {
            ERROR("status server: accept error = %d\n",ret);
            goto err0;
        }
        DEBUG("status server: accept ok!\n");

        /*receive message from client*/
        for(;;)
        {
           memset(&msg,0,sizeof(msg));
           vec.iov_base=status_socket_recvbuf;
           vec.iov_len=1024;

           ret=kernel_recvmsg(status_client_sock,&msg,&vec,1,20,0); /*receive message*/
           if(ret<=0){
               ERROR("status server: receive message error = %d\n",ret); /*Connection is broken*/
               break;
           }
           DEBUG("status server: receive message %d\n",ret);
        }

	}

	/*release socket*/
err0:   sock_release(status_server_sock);

    return ret;
}

int step_socket_task(void*data) {

    struct sockaddr_in s_addr;
    unsigned short portnum=6666;
    int ret=0;

    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(portnum);
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*create a socket*/
    ret=sock_create_kern(AF_INET, SOCK_STREAM,0,&step_server_sock);
    if(ret){
        ERROR("step server:socket_create error = %d\n",ret);
        return -1;
    }
    DEBUG("step server: socket_create ok!\n");

    /*bind the socket*/
    ret=kernel_bind(step_server_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (ret< 0) {
        ERROR("step server: bind error = %d\n",ret);
        goto err0;
    }
    DEBUG("step server: bind ok!\n");

    /*listen*/
    ret=kernel_listen(step_server_sock, 5);
    if(ret<0){
        ERROR("step server: listen error = %d\n",ret);
        goto err0;
    }
    DEBUG("step server: listen ok!\n");

    /*accept*/
    for(;;)
    {
        ret=kernel_accept(step_server_sock, &step_client_sock_tmp, 0);
        if (ret<0) {
            ERROR("step server: accept error = %d\n",ret);
            goto err0;
        }
        DEBUG("step server: accept ok!\n");
        step_client_sock=step_client_sock_tmp;
    }
    return ret;

err0:   sock_release(step_server_sock);

        return ret;
}

static int vble_socket_send(char* data ,int len)
{
	int length = 0;
	struct kvec vec;
	struct msghdr msg;

	memset(&msg,0,sizeof(msg));
	vec.iov_base=data;
	vec.iov_len=len;

    if(client_sock == NULL)
    {
        return -3;
    }

	if(client_sock->state != SS_CONNECTED)
	{
	    ERROR("vble server: send not accept\n");
	    return -1;
	}

	length=kernel_sendmsg(client_sock,&msg,&vec,1,len); /*send message */
	if(length<0){
	    ERROR("vble server: kernel_sendmsg error = %d\n",length);
	    return -2;
	}
	//DEBUG("server: send len=%d message[0]:%d\n",length,data[0]);

	return 0;
}
int status_socket_send(char* data ,int len)
{
	int length = 0;
	struct kvec vec;
	struct msghdr msg;

	memset(&msg,0,sizeof(msg));
	vec.iov_base=data;
	vec.iov_len=len;

    if(status_client_sock == NULL)
    {
        return -3;
    }

	if(status_client_sock->state != SS_CONNECTED)
	{
	    ERROR("status server: send not accept\n");
	    return -1;
	}

	length=kernel_sendmsg(status_client_sock,&msg,&vec,1,len); /*send message */
	if(length<0){
	    ERROR("status server: kernel_sendmsg error = %d\n",length);
	    return -2;
	}
    //DEBUG("status socket_send: %d byte \n",len);

	return 0;
}

static int step_socket_send(char* data ,int len)
{
	int length = 0;
	struct kvec vec;
	struct msghdr msg;

	memset(&msg,0,sizeof(msg));
	vec.iov_base=data;
	vec.iov_len=len;

    if(step_client_sock == NULL)
    {
        return -3;
    }

	if(step_client_sock->state != SS_CONNECTED)
	{
	    ERROR("setp server: send not accept\n");
	    return -1;
	}

	length=kernel_sendmsg(step_client_sock,&msg,&vec,1,len); /*send message */
	if(length<0){
	    ERROR("step server: kernel_sendmsg error = %d\n",length);
	    return -2;
	}
	printk("vble step server: send len=%d message[0]:%d\n",length,data[0]);

	return 0;
}
static void vble_activity_timeout(unsigned long data)
{
    //DEBUG("vble_activity_timeout up\n");
    up(&vble_devp->timer_sem);
}
void vble_timer_reset(void)
{

    ERROR("vble_timer_task IOCTL_CMD_RESET\n");
    mutex_lock(&vble_devp->lock);
    vble_send_cmd_mex(CMD_RESET);
    mutex_unlock(&vble_devp->lock);
    vble_info.ble_status = SLEEP;
    vble_info.ble_bond = 0;
    vble_info.ancc_status = 0;
    vble_info.reset_status = 1;
    memset(vble_info.con_addr,0,6);
    status_socket_send((char*)&vble_info,sizeof(vble_info));
    //status_socket_send((char*)&vble_info,3);
    vble_info.reset_status = 0;

    timeout_reset_count++;
}
int vble_timer_task(void*data)
{
    int ret;
    int i;
    int ble_retry_num;
    init_timer(&vble_activity_timer);
    vble_activity_timer.expires = jiffies + (DELAY_TIME*HZ);
    vble_activity_timer.data = (unsigned long) "vble_activity_timer";
    vble_activity_timer.function = &vble_activity_timeout; /* timer handler */
    add_timer(&vble_activity_timer);
    DEBUG("vble_timer_task\n");
    timeout_init_ok = 1;

    for(;;)
	{
        down(&vble_devp->timer_sem);
        if(ble_mode == NORMAL)
        {
            ble_retry_num = 0;
            for(i=0;i<BLE_ACTIVITY_RETRY_MAX;i++)
            {
                mutex_lock(&vble_devp->lock);
                //DEBUG("vble_timer_task IOCTL_CMD_GET_DEVNAME\n");
                vble_devp->fb_info->vring->length = 3;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_GET_DEVNAME;
                vble_devp->fb_info->vring->buffer[2]= 0x00; //length
                ret = vble_send_cmd_mex_resp(CMD_SEND_DATA);
                mutex_unlock(&vble_devp->lock);
                if(test_reset == 1)
                {
                    ERROR("vble_timer_task test_reset = 1\n");
                    vble_timer_reset();
                    test_reset = 0;
                    break;
                }
                if(ret != 0)
                {
                    ble_retry_num++;
                    ERROR("vble_timer_task ble_retry_num = %d\n",ble_retry_num);
                    if(ble_retry_num >= BLE_ACTIVITY_RETRY_MAX)
                    {
                        vble_timer_reset();
                        break;
                    }
                }
                else
                {
                    break;
                }
                msleep(500);
            }//for
            mod_timer(&vble_activity_timer,jiffies + (DELAY_TIME*HZ));
        }// if
    }
}

void vble_receive_normal_process(eaci_cmd * rec_buf)
{
    if(rec_buf->type == EACI_MSG_TYPE_EVT)
    {

        switch(rec_buf->cmd)
        {
            case EACI_MSG_EVT_ADV :
                if(rec_buf->data[0] == 1)  //1:start 0:end
                {
                    vble_info.ble_status = ADVERTISING;
                    DEBUG("ble_status = ADVERTISING \n");
                }
                else if(rec_buf->data[0] == 0)
                {
                    vble_info.ble_status = SLEEP;
                    vble_info.ble_bond = 0;
                    vble_info.ancc_status = 0;
                    memset(vble_info.con_addr,0,6);
                    DEBUG("ble_status = SLEEP \n");
                }
                vble_retry_set(0,0,0);
                status_socket_send((char*)&vble_info,sizeof(vble_info));
                break;

            case EACI_MSG_EVT_CONN:
                if(rec_buf->data[0] == 0)  //0:success
                {
                    vble_info.ble_status = CONNECT;
                    DEBUG("ble_status = CONNECT \n");
                    memcpy(vble_info.con_addr,&rec_buf->data[1],6);
                    vble_retry_set(EACI_MSG_TYPE_CMD,EACI_MSG_CMD_PER_UPDATE_PARAM,BLE_RETRY_DELAY_TIME_5);
                    vble_devp->ancc_next_type = EACI_MSG_TYPE_CMD;
                    vble_devp->ancc_next_cmd = EACI_MSG_CMD_PER_UPDATE_PARAM;
                    up(&vble_devp->ancc_sem);
                }
                status_socket_send((char*)&vble_info,sizeof(vble_info));
                break;

            case EACI_MSG_EVT_CONN_QPPS:
                if(rec_buf->data[0] == 0)  //0:success
                {
                    vble_info.ble_status = CONNECT_QPPS;
                    DEBUG("ble_status = CONNECT_QPPS \n");
                }
                else
                {
                    vble_info.ble_status = CONNECT;
                    DEBUG("ble_status = CONNECT_QPPS --> CONNECT \n");
                }
                status_socket_send((char*)&vble_info,sizeof(vble_info));
                break;

            case EACI_MSG_EVT_BOND:
                if(rec_buf->data[0] == 0)  //0:success
                {
                    vble_info.ble_bond = 1;
                    DEBUG("ble_status = BOND \n");
                    vble_retry_set(EACI_MSG_TYPE_DATA_REQ,CMD_ANCC_ENABLE,BLE_RETRY_DELAY_TIME_5);
                    vble_devp->ancc_next_type = EACI_MSG_TYPE_DATA_REQ;
                    vble_devp->ancc_next_cmd = CMD_ANCC_ENABLE;
                    up(&vble_devp->ancc_sem);
                    status_socket_send((char*)&vble_info,sizeof(vble_info));
                }
                else
                {
                    DEBUG("ble_status = BOND FAILED\n");
                    vble_info.ble_bond = 2;
                    status_socket_send((char*)&vble_info,sizeof(vble_info));
                    vble_info.ble_bond = 0;
                }

                break;

            case EACI_MSG_EVT_SMP_SEC:
                if(rec_buf->data[0] == 0)  //0:success
                {
                    vble_info.ble_bond = 1;
                    DEBUG("ble_status = already BOND \n");
                }
                status_socket_send((char*)&vble_info,sizeof(vble_info));
                break;

            case EACI_MSG_EVT_DISC:
                vble_info.ble_status = SLEEP;
                vble_info.ble_bond = 0;
                vble_info.ancc_status = 0;
                memset(vble_info.con_addr,0,6);

                DEBUG("ble_status = DISC: 0x%x \n",rec_buf->data[0]);
                status_socket_send((char*)&vble_info,sizeof(vble_info));
                break;

            case EACI_MSG_EVT_CEN_UPDATE_PARAM:
                DEBUG("EACI_MSG_EVT_CEN_UPDATE_PARAM OK\n");
                break;

            case EACI_MSG_EVT_UPDATE_PARAM:
                if(rec_buf->data[0] == 0)  //0:success
                {
                    DEBUG("EACI_MSG_EVT_UPDATE_PARAM OK\n");

                    if(vble_info.ble_bond == 1)
                    {
                        vble_retry_set(EACI_MSG_TYPE_DATA_REQ,CMD_ANCC_ENABLE,BLE_RETRY_DELAY_TIME_5);
                        vble_devp->ancc_next_type = EACI_MSG_TYPE_DATA_REQ;
                        vble_devp->ancc_next_cmd = CMD_ANCC_ENABLE;
                        up(&vble_devp->ancc_sem);
                    }
                    else if(vble_info.ble_bond == 0)
                    {
                        vble_retry_set(EACI_MSG_TYPE_CMD,EACI_MSG_CMD_BOND,BLE_RETRY_DELAY_TIME_30);
                        vble_devp->ancc_next_type = EACI_MSG_TYPE_CMD;
                        vble_devp->ancc_next_cmd = EACI_MSG_CMD_BOND;
                        up(&vble_devp->ancc_sem);
                    }

                }
                break;

            case EACI_MSG_EVT_SET_DEVNAME:
                break;

            case EACI_MSG_EVT_GET_MAC:
            case EACI_MSG_EVT_GET_DEVNAME:
            case EACI_MSG_EVT_GET_VERSION:
            case EACI_MSG_EVT_QPPS_SEND_DATA:
            case EACI_MSG_EVT_SENSOR_TRIM:
                vble_devp->fb_info->vring->length = vble_devp->fb_info->vring->irq_length;
                memcpy(vble_devp->fb_info->vring->buffer,vble_devp->fb_info->vring->irq_buffer,vble_devp->fb_info->vring->irq_length);
                vble_devp->fb_info->vring->cmd = CMD_NONE;
                wake_up_interruptible(&vble_devp->wait_cmd);
                break;

            case EACI_MSG_EVT_QPPS_RECV_DATA:
                DEBUG("vble_socket_send: %d byte \n",rec_buf->length);
                vble_socket_send(rec_buf->data,rec_buf->length);
                break;

            case EACI_MSG_EVT_GSENSOR_DATA:
                input_report_acc(&rec_buf->data[0]);
                break;

            case EACI_MSG_EVT_ESENSOR_DATA:
                input_report_ecomp(&rec_buf->data[0]);
                break;

            case EACI_MSG_EVT_STEP_DATA:
                step_socket_send(rec_buf->data, rec_buf->length);
                break;


            default:
                DEBUG("default \n");

        }//switch
    }
    else if(rec_buf->type == EACI_MSG_TYPE_DATA_IND && (rec_buf->data[1]<<8)+rec_buf->data[0] == ANCS_UUID)
    {
        //DEBUG("ancc rec data:  ");
        vble_ancc_receive(rec_buf);
    }


}


int vble_receive_task(void*data)
{
	int ret, len;
	for(;;)
	{

		down(&vble_devp->irq_sem);
        wake_lock_timeout(&vble_wake_lock, 2 * HZ);
        if(timeout_init_ok == 1)
        {
            mod_timer(&vble_activity_timer,jiffies + (DELAY_TIME*HZ));
        }

        //DEBUG("vble_receive_task \n");
        if(ble_mode == NORMAL)
        {
            len=0;
            while(len < vble_devp->fb_info->vring->irq_length && len<BUFFER_SIZE){
                vble_receive_normal_process((eaci_cmd *)&vble_devp->fb_info->vring->irq_buffer[len]);
                len+=(vble_devp->fb_info->vring->irq_buffer[len+2]+3);
            }
        }
        else if(ble_mode == INIT_UPDATE)
        {
            mutex_lock(&vble_devp->lock);
            DEBUG("INIT firmware update \n");
            //msleep(1000);
            ret = vble_send_cmd_mex(CMD_RESET);
            //msleep(1000);
            ble_firmwate_update();
            //msleep(1000);
            ret = vble_send_cmd_mex(CMD_RESET);

            ble_mode = NORMAL;
            mutex_unlock(&vble_devp->lock);

        }

	}
	return 0;
}

static int vble_fasync(int fd, struct file *filp, int mode)
{
    FUNC_ENTRY();
    return fasync_helper(fd, filp, mode, &vble_devp->async);
}

unsigned int vble_send_cmd_mex(mex_cmd cmd)
{
    //FUNC_ENTRY();
    //DEBUG("cmd_mex=%d %d\n", cmd,vble_devp->fb_info->vring->buffer[1]);

    vble_devp->fb_info->vring->cmd = cmd;
    vble_devp->fb_info->vring->result = SUCCESS;
    nkops.nk_xirq_trigger(vble_devp->fb_info->s_xirq, vble_devp->fb_info->vlink->s_id);
    msleep(500);
    return 0;
}

unsigned int vble_send_cmd_mex_resp(mex_cmd cmd)
{
    int ret;

    //FUNC_ENTRY();
    //DEBUG("cmd_mex_resp=%d %d\n", cmd,vble_devp->fb_info->vring->buffer[1]);

    vble_devp->fb_info->vring->cmd = cmd;
    vble_devp->fb_info->vring->result = SUCCESS;
    nkops.nk_xirq_trigger(vble_devp->fb_info->s_xirq, vble_devp->fb_info->vlink->s_id);

    ret = wait_event_interruptible_timeout(vble_devp->wait_cmd,
            (vble_devp->fb_info->vring->cmd == CMD_NONE), 100);

    if(ret <= 0)
    {
        //mutex_unlock(&vble_devp->lock);
        //memset(vble_devp->fb_info->vring, 0, sizeof(vble_devp->fb_info->vring));
        printk("vble mex_cmd_ret=%d\n", ret);
        return ERROR_TIMEOUT;
    }
    else if(ret > 0)
    {
        return 0;
    }
    return 0;
}

static int vble_open(struct inode *inode, struct file *file)
{
    int ret = 0;
    FUNC_ENTRY();
/*
    if(qn902x_sw == 0) {
        qn902x_sw = 1;
        if(ble_mode == NORMAL)
        {
            ret = vble_send_cmd_mex(CMD_RESET);
        }
    }
*/
    return ret;
}

static int vble_close(struct inode *inodp, struct file *filp)
{
    int ret = 0;

    FUNC_EXIT();
    return ret;
}

static long vble_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = SUCCESS;
    unsigned char __user *p = (unsigned char __user *)arg;
    //FUNC_ENTRY();

    if(cmd == IOCTL_CMD_QUERY_STATUS)
    {
        vble_devp->fb_info->vring->buffer[0] = 9;
        vble_devp->fb_info->vring->buffer[1] = vble_info.ble_status;
        vble_devp->fb_info->vring->buffer[2] = vble_info.ble_bond;
        vble_devp->fb_info->vring->buffer[3] = vble_info.ancc_status;
        vble_devp->fb_info->vring->buffer[4] = vble_info.reset_status;
        memcpy(&vble_devp->fb_info->vring->buffer[5] ,&vble_info.con_addr[0],6);

        DEBUG("ble_status : %d %d %d\n",vble_devp->fb_info->vring->buffer[1],vble_devp->fb_info->vring->buffer[2],vble_devp->fb_info->vring->buffer[3]);
        ret = copy_to_user(p, &vble_devp->fb_info->vring->buffer[0], vble_devp->fb_info->vring->buffer[0]+1);
        return ret;
    }

    //DEBUG("vble in ioctl lock mutex cmd=%d\n",cmd);
    mutex_lock(&vble_devp->lock);
    //DEBUG("vble in ioctl lock mutex cmd=(%d)\n",cmd);

    wake_lock_timeout(&vble_wake_lock, 2 * HZ);

    if(ble_mode == INIT_UPDATE)
    {
        ret = -EINVAL;
    }
    else
    {

        switch(cmd) {
        case IOCTL_CMD_RESET://clear all status && make everything restar
            DEBUG("IOCTL_CMD_RESET\n");
            ret = vble_send_cmd_mex(CMD_RESET);
            vble_info.ble_status = SLEEP;
            vble_info.ble_bond = 0;
            vble_info.ancc_status = 0;
            vble_info.reset_status = 0;
            memset(vble_info.con_addr,0,6);
            break;

        case IOCTL_CMD_READ_MAC:
            DEBUG("IOCTL_CMD_READ_MAC\n");
            vble_devp->fb_info->vring->length = 3;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_GET_MAC;
            vble_devp->fb_info->vring->buffer[2]= 0x00; //length
            ret = vble_send_cmd_mex_resp(CMD_SEND_DATA);
            if(ret != 0) {
            	break;
            }
            ret = copy_to_user(p, &vble_devp->fb_info->vring->buffer[3], vble_devp->fb_info->vring->buffer[2]);
            break;

        case IOCTL_CMD_GET_DEVNAME:
            DEBUG("IOCTL_CMD_GET_DEVNAME\n");
            vble_devp->fb_info->vring->length = 3;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_GET_DEVNAME;
            vble_devp->fb_info->vring->buffer[2]= 0x00; //length
            ret = vble_send_cmd_mex_resp(CMD_SEND_DATA);
            if(ret != 0) {
            	break;
            }
            ret = copy_to_user(p, &vble_devp->fb_info->vring->buffer[2], vble_devp->fb_info->vring->buffer[2]+1);
            break;

        case IOCTL_CMD_SET_DEVNAME:
            DEBUG("IOCTL_CMD_SET_DEVNAME\n");
            if(*p <= 0) {
                ret = -EINVAL;
            } else {
                ret = copy_from_user(&vble_devp->fb_info->vring->buffer[2], p, (*p)+1);
                if(ret != 0) {
                    break;
                }
                vble_devp->fb_info->vring->length = 3 + *p;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_SET_DEVNAME;
                //vble_devp->fb_info->vring->buffer[2]= *p; //length
                ret = vble_send_cmd_mex(CMD_SEND_DATA);
            }
            break;
/*
        case IOCTL_CMD_QUERY_STATUS:
            //ret = vble_send_cmd_mex(CMD_QUERY_STATUS);
            //if(ret != 0) {
            //	break;
            //}
            if(vble_info.ble_status == SLEEP)
            {
                vble_info.ble_bond = 0;
                vble_info.ancc_status = 0;
                memset(vble_info.con_addr,0,6);
            }

            vble_devp->fb_info->vring->buffer[0] = 9;
            vble_devp->fb_info->vring->buffer[1] = vble_info.ble_status;
            vble_devp->fb_info->vring->buffer[2] = vble_info.ble_bond;
            vble_devp->fb_info->vring->buffer[3] = vble_info.ancc_status;
            memcpy(&vble_devp->fb_info->vring->buffer[4] ,&vble_info.con_addr[0],6);

            DEBUG("ble_status : %d %d %d\n",vble_devp->fb_info->vring->buffer[1],vble_devp->fb_info->vring->buffer[2],vble_devp->fb_info->vring->buffer[3]);
            ret = copy_to_user(p, &vble_devp->fb_info->vring->buffer[0], vble_devp->fb_info->vring->buffer[0]+1);
            break;
*/
        case IOCTL_CMD_TEST:
            DEBUG("IOCTL_CMD_TEST\n");
            test_reset = 1;
            up(&vble_devp->timer_sem);
            break;

        case IOCTL_CMD_ENABLE_BLE:
            DEBUG("IOCTL_CMD_ENABLE_BLE\n");
            vble_info.ble_status = SLEEP;
            vble_info.ble_bond = 0;
            vble_info.ancc_status = 0;
            memset(vble_info.con_addr,0,6);
            status_socket_send((char*)&vble_info,sizeof(vble_info));
            //status_socket_send((char*)&vble_info,3);
            ret = vble_send_cmd_mex(CMD_ENABLE_BLE);
            break;
        case IOCTL_CMD_DISABLE_BLE:
            DEBUG("IOCTL_CMD_DISABLE_BLE\n");
            ret = vble_send_cmd_mex(CMD_DISABLE_BLE);
            break;

        case IOCTL_CMD_GET_VERSION:
            DEBUG("IOCTL_CMD_GET_VERSION\n");
            vble_devp->fb_info->vring->length = 3;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_GET_VERSION;
            vble_devp->fb_info->vring->buffer[2]= 0x00; //length
            ret = vble_send_cmd_mex_resp(CMD_SEND_DATA);
            if(ret != 0) {
            	break;
            }
            ret = copy_to_user(p, &vble_devp->fb_info->vring->buffer[2], vble_devp->fb_info->vring->buffer[2]+1);
            break;


        //ls qpps
		case IOCTL_CMD_ADV_START:
            DEBUG("IOCTL_CMD_ADV_START\n");
            vble_retry_set(EACI_MSG_TYPE_CMD,EACI_MSG_CMD_ADV,BLE_RETRY_DELAY_TIME_5);
            vble_devp->fb_info->vring->length = 8;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_ADV;
            vble_devp->fb_info->vring->buffer[2]= 0x05; //length
            vble_devp->fb_info->vring->buffer[3]= 0x01; //start
            vble_devp->fb_info->vring->buffer[4]= 0x80; //min_adv_intv recommended value: 30 to 60 ms; N * 0.625
            vble_devp->fb_info->vring->buffer[5]= 0x02; //0x280*0.625=400ms
            vble_devp->fb_info->vring->buffer[6]= 0x20;
            vble_devp->fb_info->vring->buffer[7]= 0x03; //0x320*0.625=500ms
			ret = vble_send_cmd_mex(CMD_SEND_DATA);
            break;

		case IOCTL_CMD_ADV_END:
            DEBUG("IOCTL_CMD_ADV_END\n");
            vble_devp->fb_info->vring->length = 8;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_ADV;
            vble_devp->fb_info->vring->buffer[2]= 0x05; //length
            vble_devp->fb_info->vring->buffer[3]= 0x00; //end
            vble_devp->fb_info->vring->buffer[4]= 0x00;
            vble_devp->fb_info->vring->buffer[5]= 0x00;
            vble_devp->fb_info->vring->buffer[6]= 0x00;
            vble_devp->fb_info->vring->buffer[7]= 0x00;
			ret = vble_send_cmd_mex(CMD_SEND_DATA);
            break;
        case IOCTL_CMD_BOND:
            DEBUG("IOCTL_CMD_BOND\n");
            vble_devp->fb_info->vring->length = 9;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_BOND;
            vble_devp->fb_info->vring->buffer[2]= 0x06; //length
            memcpy(&vble_devp->fb_info->vring->buffer[3],vble_info.con_addr,6);
            ret = vble_send_cmd_mex(CMD_SEND_DATA);
            break;

		case IOCTL_CMD_DISCONNECT:
            DEBUG("IOCTL_CMD_DISCONNECT\n");
            vble_devp->fb_info->vring->length = 9;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_DISC;
            vble_devp->fb_info->vring->buffer[2]= 0x06; //length
            memcpy(&vble_devp->fb_info->vring->buffer[3],vble_info.con_addr,6);
			ret = vble_send_cmd_mex(CMD_SEND_DATA);
            break;

        case IOCTL_CMD_FW_UPDATE:
            DEBUG("IOCTL_CMD_FW_UPDATE\n");
            ble_mode = INIT_UPDATE;
            up(&vble_devp->irq_sem);
            break;

        case IOCTL_CMD_ANCC_ENABLE:
            DEBUG("IOCTL_CMD_ANCC_ENABLE\n");
            ret = vble_ancc_enable();
            break;

        case IOCTL_CMD_ENABLE_G:
            printk("vble opt g sensor %d\n", *p);
            if(*p>0) {
                vble_devp->fb_info->vring->length = 3;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_ENABLE_G;
                vble_devp->fb_info->vring->buffer[2]= 0x00; //length
                ret = vble_send_cmd_mex(CMD_SEND_DATA);
            } else {
                vble_devp->fb_info->vring->length = 3;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_DISABLE_G;
                vble_devp->fb_info->vring->buffer[2]= 0x00; //length
                ret = vble_send_cmd_mex(CMD_SEND_DATA);
            }
            break;

        case IOCTL_CMD_ENABLE_E:
            printk("vble opt e sensor %d\n", *p);
            if(*p>0) {
                vble_devp->fb_info->vring->length = 3;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_ENABLE_E;
                vble_devp->fb_info->vring->buffer[2]= 0x00; //length
#ifdef SENSOR_COMPENSATE
                ret = vble_send_cmd_mex_resp(CMD_SEND_DATA);
                if(ret==0 && vble_devp->fb_info->vring->buffer[2]!=0) {
                    printk("vble init trim register\n");
                    init_trim_register();
                }
#else
                ret = vble_send_cmd_mex(CMD_SEND_DATA);
#endif
            } else {
                vble_devp->fb_info->vring->length = 3;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_DISABLE_E;
                vble_devp->fb_info->vring->buffer[2]= 0x00; //length
                ret = vble_send_cmd_mex(CMD_SEND_DATA);
            }
            break;

        case IOCTL_CMD_ENABLE_STEP_DETECT:
            printk("vble opt setp sensor\n");
            vble_devp->fb_info->vring->length = 3;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_ENABLE_STEP_DETECT;
            vble_devp->fb_info->vring->buffer[2]= 0x00; //length
            ret = vble_send_cmd_mex(CMD_SEND_DATA);
            break;

        case IOCTL_CMD_DISABLE_STEP_DETECT:
            printk("vble opt dis setp sensor\n");
            vble_devp->fb_info->vring->length = 3;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_DISABLE_STEP_DETECT;
            vble_devp->fb_info->vring->buffer[2]= 0x00; //length
            ret = vble_send_cmd_mex(CMD_SEND_DATA);
            break;

        case IOCTL_CMD_ENABLE_STEP_STORE:
            printk("vble opt store step sensor\n");
            vble_devp->fb_info->vring->length = 3;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_ENABLE_STEP_STORE;
            vble_devp->fb_info->vring->buffer[2]= 0x00; //length
            ret = vble_send_cmd_mex(CMD_SEND_DATA);

            up(&vble_devp->timer_sem);
            break;

        case IOCTL_CMD_DISABLE_STEP_STORE:
            printk("vble opt dis store setp sensor\n");
            vble_devp->fb_info->vring->length = 3;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
            vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_DISABLE_STEP_STORE;
            vble_devp->fb_info->vring->buffer[2]= 0x00; //length
            ret = vble_send_cmd_mex(CMD_SEND_DATA);
            break;

        case IOCTL_CMD_SET_VBLE_TIME:
            printk("vble set vble time\n");
            if(*p <= 0) {
                ret = -EINVAL;
            } else {
                ret = copy_from_user(&vble_devp->fb_info->vring->buffer[3], p, 6);
                if(ret != 0) {
                    break;
                }
                vble_devp->fb_info->vring->length = 9;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_SET_SYS_TIME;
                vble_devp->fb_info->vring->buffer[2]= 0x06; //length
                //memcpy(&vble_devp->fb_info->vring->buffer[3], p, 6);
                ret = vble_send_cmd_mex(CMD_SEND_DATA);
            }
            break;

        default:
            ret = -EINVAL;
        }//switch
    }


    //printk("vble in ioctl unlock mutex cmd=%d\n",cmd);
    mutex_unlock(&vble_devp->lock);

    //FUNC_EXIT();
    return ret;
}



int vble_firmware_ioctl(int cmd ,unsigned char * data,int* length)
{
    int ret;
    switch(cmd)
    {
        case CMD_FU_VERS:
            memset(&vble_devp->fb_info->vring->buffer[0],0,sizeof(vble_devp->fb_info->vring->buffer));
            ret = vble_send_cmd_mex_resp(CMD_FU_VERS);
            *length = vble_devp->fb_info->vring->length;
            memcpy(data, &vble_devp->fb_info->vring->buffer[0], *length);
            break;

        case CMD_FU_BAUD:
            vble_devp->fb_info->vring->length = *length;
            memcpy(&vble_devp->fb_info->vring->buffer[0], data, *length);
            ret = vble_send_cmd_mex_resp(CMD_FU_BAUD);
            break;

        case CMD_FU_BURN:
            vble_devp->fb_info->vring->length = *length;
            memcpy(&vble_devp->fb_info->vring->buffer[0], data, *length);
            ret = vble_send_cmd_mex_resp(CMD_FU_BURN);
            *length = vble_devp->fb_info->vring->length;
            memcpy(data, &vble_devp->fb_info->vring->buffer[0], *length);
            break;

        default:
            ret = -EINVAL;
    }
    return ret;
}

static struct file_operations dev_fops = {
    .owner          = THIS_MODULE,
    .open           = vble_open,
    .fasync         = vble_fasync,
    .release        = vble_close,
    .unlocked_ioctl = vble_unlocked_ioctl,
};

struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,
};
/*
void vble_irq_handler(void)
{
    FUNC_ENTRY();
    if (vble_devp->async)
    {
        kill_fasync(&vble_devp->async, SIGIO, POLL_IN);
    }
}
*/
static void vble_xirq_hdl(void *cookie, NkXIrq xirq)
{
    //FUNC_ENTRY();
    if(vble_devp->fb_info->vring->cmd == CMD_DONE)
    {

        //mutex_unlock(&vble_devp->lock);
        //ERROR("vble ----------------Command complete call back cmd status %d\n",vble_devp->fb_info->vring->result);
        vble_devp->fb_info->vring->cmd = CMD_NONE;  //only firmware update
        wake_up_interruptible(&vble_devp->wait_cmd);
    }
    if(vble_devp->fb_info->vring->irq == CMD_QUERY_IRQ)
    {
        //ERROR("vble -----------cmd_irq---------------\n");
        //wake_lock_timeout(&vble_wake_lock, 1 * HZ);
        vble_devp->fb_info->vring->irq = CMD_NONE;
        up(&vble_devp->irq_sem);
    }
    //FUNC_EXIT();
}

static void vble_sysconf(void *cookie, NkXIrq xirq)
{
    vble_dev *dev = (vble_dev *) cookie;
    volatile int* my_state = &dev->fb_info->vlink->c_state;
    int peer_state = dev->fb_info->vlink->s_state;

    DEBUG("Received sys conf cross interrupt, Updating the state of the fe-be driver");

    switch(*my_state)
    {
        case NK_DEV_VLINK_OFF:
            if(peer_state != NK_DEV_VLINK_ON)
            {
                *my_state = NK_DEV_VLINK_RESET;
                nkops.nk_xirq_trigger(NK_XIRQ_SYSCONF,  dev->fb_info->vlink->s_id);
            }
            break;
        case NK_DEV_VLINK_RESET:
            if(peer_state != NK_DEV_VLINK_OFF)
            {
                *my_state = NK_DEV_VLINK_ON;
                nkops.nk_xirq_trigger(NK_XIRQ_SYSCONF,  dev->fb_info->vlink->s_id);
            }
            break;
        case NK_DEV_VLINK_ON:
            if (peer_state == NK_DEV_VLINK_OFF)
            {
                *my_state = NK_DEV_VLINK_RESET;
                nkops.nk_xirq_trigger(NK_XIRQ_SYSCONF,  dev->fb_info->vlink->s_id);
            }
            break;
    }

    DEBUG("Sending sysconf OS#%d(%d)->OS#%d(%d)\n",
            dev->fb_info->vlink->c_id,
            dev->fb_info->vlink->c_state,
            dev->fb_info->vlink->s_id,
            dev->fb_info->vlink->s_state);


    //return (*my_state == NK_DEV_VLINK_ON) && (peer_state == NK_DEV_VLINK_ON);
}

static ssize_t ecomp_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    //FUNC_ENTRY();

    return sprintf(buf, "%d %d %d\n", ecomp_x, ecomp_y, ecomp_z);
}

static ssize_t ecomp_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    FUNC_ENTRY();
    //vble_send_cmd_mex(CMD_GET_SENSOR);

    return count;
}
static ssize_t accel_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    //FUNC_ENTRY();

    return sprintf(buf, "%d %d %d\n", acc_x, acc_y, acc_z);
}

static ssize_t accel_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    FUNC_ENTRY();
    //ble_mode = INIT_UPDATE;
    //up(&vble_devp->irq_sem);

    return count;
}
static ssize_t vble_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    //FUNC_ENTRY();

    return sprintf(buf, "M0 reset count = %d \n", timeout_reset_count);
}

static ssize_t vble_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    FUNC_ENTRY();

    return count;
}


static DEVICE_ATTR(value_mag, S_IRUGO | S_IWUGO, ecomp_show, ecomp_set);
static DEVICE_ATTR(value_accel, S_IRUGO | S_IWUGO, accel_show, accel_set);
static DEVICE_ATTR(value_vble, S_IRUGO | S_IWUGO, vble_show, vble_set);


static struct attribute *bmc050_attributes[] = {
	&dev_attr_value_accel.attr,
	&dev_attr_value_mag.attr,
	&dev_attr_value_vble.attr,
	NULL
};
//static const struct attribute *vble_attributes = &dev_attr_value_vble.attr;

static struct attribute_group bmc050_attribute_group = {
	.attrs = bmc050_attributes
};

#if 0
static int vble_suspend(struct platform_device *dev, pm_message_t state)
{
    FUNC_ENTRY();
    /*
    if(sleep == 0){
        sleep = 1;
        mutex_lock(&vble_devp->lock);
        vble_send_cmd_mex(CMD_SUSPEND);
        mutex_unlock(&vble_devp->lock);
    }
    */
    return 0;
}

static int vble_resume(struct platform_device *dev)
{

    FUNC_ENTRY();
    /*
    if(timeout_init_ok == 1)
    {
        mod_timer(&vble_activity_timer,jiffies + (0*HZ));
    }
    */
    //up(&vble_devp->timer_sem);
    /*
    if(sleep == 1){
        sleep = 0;
        mutex_lock(&vble_devp->lock);
        vble_send_cmd_mex(CMD_RESUME);
        mutex_unlock(&vble_devp->lock);
    }
    */
    return 0;
}

static struct platform_device vble_device = {
    .name    = "ble_driver",
};

static int vble_probe(struct platform_device *dev)
 {
     int ret;
     ret = sysfs_create_file(&vble_device.dev.kobj, vble_attributes);
     return 0;
}

static struct platform_driver vble_driver = {
    .suspend    = vble_suspend,
    .resume     = vble_resume,
    .probe      = vble_probe,
    .driver     = {
        .name   = "ble_driver",
        .owner = THIS_MODULE,
    },
};
#endif

static int __init vble_init(void)
{
    int ret = 0;
    NkPhAddr plink = 0;
    NkPhAddr pdata = 0;
    NkDevVlink* vlink = NULL;
    NkXIrqId  vsysconf_id = NULL;
    NkOsId my_id = nkops.nk_id_get();
    static struct task_struct *vble_socket;
    static struct task_struct *vble_receive;
    static struct task_struct *status_socket;
    static struct task_struct *step_socket;
    static struct task_struct *vble_timer;

    ERROR("vble : vble_init\n");

    FUNC_ENTRY();
    //platform_device_register(&vble_device);
    //platform_driver_register(&vble_driver);
    //registe misc device
    ret = misc_register(&misc);
    if(ret < 0)
    {
        ERROR("can't registe misc device");
        ret =  - ENODEV;
        goto fail_exit;
    }
    //misc.this_device->class->suspend = vble_suspend;
    //misc.this_device->class->resume = vble_resume;

    ERROR("vble : vble_misc register\n");

    /*ret = device_create_file(misc.this_device, &dev_attr_vble);
    if(ret < 0)
    {
        ERROR("failed to create sys file");
        ret =  - ENODEV;
        goto fail_create_sys;
    }*/

    //get vble_devp as plat_data ; hold everything...
    vble_devp = (vble_dev*)kzalloc(sizeof(vble_dev), GFP_KERNEL);
    if (!vble_devp)
    {
        ERROR("failed to alloc vble_devp");
        ret =  - ENOMEM;
        goto fail_alloc_vble;
    }

    init_waitqueue_head(&vble_devp->wait_cmd);
    wake_lock_init(&vble_wake_lock, WAKE_LOCK_SUSPEND, "vble_irq");
    mutex_init(&vble_devp->lock);


    ERROR("vble : vble alloc vble_dev\n");

    vble_devp->fb_info = (fb_info*)kzalloc(sizeof(fb_info), GFP_KERNEL);
    if (!vble_devp->fb_info)
    {
        ERROR("failed to alloc fb_info_devp");
        ret =  - ENOMEM;
        goto fail_alloc_fb_info;
    }

    ERROR("vble : vble alloc fb_info\n");

    vble_devp->async = (struct fasync_struct *)kzalloc(sizeof(struct fasync_struct), GFP_KERNEL);
    if (!vble_devp->async)
    {
        ERROR("failed to alloc fb_aync");
        ret =  - ENOMEM;
        goto fail_alloc_async;
    }
    //ok
    //search for backend driver link ; we use VFMR for BLE
    while ((plink = nkops.nk_vlink_lookup("VFMR", plink)))
    {
        vlink = nkops.nk_ptov(plink);
        if(vlink!=NULL)
        {
            if ((vlink->c_id == my_id))
            {
                DEBUG("FMR: found vlink");
                break;
            }
        }
        else
        {
            ERROR("Unable to get vlink ");
            ret = -ENOMEM;
            goto fail_lookup_vfmr;
        }
    }
    if (plink ==0 || vlink == NULL)
    {
        ERROR("Unable to allocate plink or vlink");
        ret = -ENODEV;
        goto fail_lookup_vfmr;
    }

    //update link pointers
    vble_devp->fb_info->vlink =vlink;
    vble_devp->fb_info->plink =plink;

    ERROR("vble : vble find ...links\n");

    //Perform persistent shared memory allocation
    pdata = nkops.nk_pmem_alloc(plink, 0, sizeof(VRing));
    if(!pdata)
    {
        ERROR("FMR: Unable to allocate persistent shared memory");
        ret = -ENOMEM;
        goto fail_lookup_vfmr;
    }
    vble_devp->fb_info->vring = (VRing*) nkops.nk_ptov(pdata);


    ERROR("vble : vble vring\n");

    //Perform persistent cross interrupts allocation/attachment
    vble_devp->fb_info->s_xirq = nkops.nk_pxirq_alloc(plink,1,vlink->s_id,1);
    if(!vble_devp->fb_info->s_xirq)
    {
        ERROR("Could not allocate xirq.s_ID=%d != MyOs_ID=%d\n",vlink->s_id,my_id);
        ret = -ENOMEM;
        goto fail_lookup_vfmr;
    }

    vble_devp->fb_info->c_xirq = nkops.nk_pxirq_alloc(plink,0,vlink->c_id,1);
    if(!vble_devp->fb_info->c_xirq)
    {
        ERROR("Could not allocate xirq.C_ID=%d != MyOs_ID=%d\n",vlink->c_id,my_id);
        ret = -ENOMEM;
        goto fail_lookup_vfmr;
    }

    ERROR("vble : vble before attach\n");

    //attach cross interrupt handler
    vble_devp->fb_info->xid = nkops.nk_xirq_attach(vble_devp->fb_info->c_xirq,vble_xirq_hdl, vble_devp);
    if(vble_devp->fb_info->xid == 0)
    {
        ERROR("Could not attach handler, xirq.C_ID=%d != MyOs_ID=%d\n",vlink->c_id,my_id);
        ret = -EINVAL;
        goto fail_lookup_vfmr;
    }

//ok
    //attach sysconf handler
    vsysconf_id =nkops.nk_xirq_attach(NK_XIRQ_SYSCONF, vble_sysconf, vble_devp);
    if(vsysconf_id==NULL)
    {
        ERROR("Unable to attach Sys conf handler");
        ret = -EINVAL;
        goto fail_lookup_vfmr;
    }

    vble_devp->fb_info->vring->irq=CMD_NONE;
    vble_devp->fb_info->vring->cmd=CMD_NONE;
    //error

    //irq
    vble_receive = kthread_create(vble_receive_task, NULL, "vble_receive_task");
    if(IS_ERR(vble_receive)){
        printk("server: unable to start kernel thread irq.\n");
        ret = PTR_ERR(vble_receive);
        vble_receive = NULL;
        return ret;
    }
    init_MUTEX(&vble_devp->irq_sem);
    wake_up_process(vble_receive);

    //firmware update
    DEBUG("init firmware update \n");
    //vble_send_cmd_mex(CMD_RESET);
    ble_firmwate_update();
    vble_send_cmd_mex(CMD_RESET);

    //socket
    vble_socket = kthread_create(vble_socket_task, NULL, "vble_socket_task");
    if(IS_ERR(vble_socket)){
        printk("server: unable to start kernel thread.\n");
        ret = PTR_ERR(vble_socket);
        vble_socket = NULL;
        return ret;
    }
    wake_up_process(vble_socket);

    //status socket
    status_socket = kthread_create(status_socket_task, NULL, "status_socket_task");
    if(IS_ERR(status_socket)){
        printk("server: unable to start kernel status_socket thread.\n");
        ret = PTR_ERR(status_socket);
        status_socket = NULL;
        return ret;
    }
    wake_up_process(status_socket);

    //step socket
    step_socket = kthread_create(step_socket_task, NULL, "step_socket_task");
    if(IS_ERR(step_socket)){
        printk("server: unable to start kernel step thread.\n");
        ret = PTR_ERR(step_socket);
        step_socket = NULL;
        return ret;
    }
    wake_up_process(step_socket);

    //socket ancc
    vble_ancc_init();

    input_acc_register();
    input_comp_register();

    ret = sysfs_create_group(&inputdev_acc->dev.kobj,&bmc050_attribute_group);
    if(ret < 0)
    {
        ERROR("failed to create sys file");
    }

    //timer
    vble_timer = kthread_create(vble_timer_task, NULL, "vble_timer_task");
    if(IS_ERR(vble_timer)){
        printk("server: unable to start vble timer thread.\n");
        ret = PTR_ERR(vble_timer);
        vble_timer = NULL;
        return ret;
    }
    init_MUTEX(&vble_devp->timer_sem);
    wake_up_process(vble_timer);


    ERROR("vble : vble ok\n");
    return ret;

fail_lookup_vfmr: kfree(vble_devp->async);
fail_alloc_async: kfree(vble_devp->fb_info);
fail_alloc_fb_info: kfree(vble_devp);
fail_alloc_vble: //device_remove_file(misc.this_device, &dev_attr_vble);
//fail_create_sys:
                 misc_deregister(&misc);
fail_exit:
     FUNC_EXIT();
     return ret;
}

static void __exit vble_exit(void)
{
    FUNC_ENTRY();

    wake_lock_destroy(&vble_wake_lock);
    misc_deregister(&misc);
    kfree(vble_devp->fb_info);
    kfree(vble_devp);
    //device_remove_file(misc.this_device, &dev_attr_vble);

    FUNC_EXIT();

}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yake.Cheng");

module_init(vble_init);
module_exit(vble_exit);

