/* drivers/misc/qn902x_ble_fu.c
 *
 * Copyright (C) 2014 lonsine
 *
 * Component: qn9021 driver ancc
 *
 */
#include <linux/wakelock.h>
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

#include "qn902x_ble_fu.h"

/// IOS Connection Parameter
/// Connection interval minimum
#define IOS_CONN_INTV_MIN                              0x0018//0x0008
/// Connection interval maximum
#define IOS_CONN_INTV_MAX                              0x0028//0x0010
/// Latency
#define IOS_SLAVE_LATENCY                              0x0000
/// Supervision timeout, Time = N * 10 msec
#define IOS_STO_MULT                                   0x012c

//// IOS default
// Interval:  0x18
// Latency:  0
// Supervision Timeout:  0x48
// Clock Accuracy:  0x05

int ancs_init_ok = 0;
vble_retry vble_retry_info;

//socket
static struct socket *ancc0_server_sock = NULL;
static struct socket *ancc0_client_sock = NULL;
static unsigned char ancc0_socket_recvbuf[1024] = {0};

static struct socket *ancc1_server_sock = NULL;
static struct socket *ancc1_client_sock = NULL;
static unsigned char ancc1_socket_recvbuf[1024] = {0};


int vble_ancc0_socket_task(void*data){

    struct sockaddr_in s_addr;
    unsigned short portnum=8889;
    int ret=0;
    struct kvec vec;
    struct msghdr msg;

    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(portnum);
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*create a socket*/
    ret=sock_create_kern(AF_INET, SOCK_STREAM,0,&ancc0_server_sock);
    if(ret){
        ERROR("ancc0 server:socket_create error = %d\n",ret);
        return -1;
    }
    DEBUG("ancc0 server: socket_create ok!\n");

    /*bind the socket*/
	ret=kernel_bind(ancc0_server_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if (ret< 0) {
        ERROR("ancc0 server: bind error = %d\n",ret);
        goto err0;
    }
    DEBUG("ancc0 server: bind ok!\n");

    /*listen*/
	ret=kernel_listen(ancc0_server_sock, 5);
    if(ret<0){
        ERROR("ancc0 server: listen error = %d\n",ret);
        goto err0;
    }
    DEBUG("ancc0 server: listen ok!\n");

	/*accept*/
	for(;;)
	{
        ret=kernel_accept(ancc0_server_sock, &ancc0_client_sock, 0);
        if (ret<0) {
            ERROR("ancc0 server: accept error = %d\n",ret);
            goto err0;
        }
        DEBUG("ancc0 server: accept ok!\n");

        /*receive message from client*/
        for(;;)
        {
            memset(&msg,0,sizeof(msg));
            vec.iov_base=ancc0_socket_recvbuf;
            vec.iov_len=1024;

            ret=kernel_recvmsg(ancc0_client_sock,&msg,&vec,1,1024,0); /*receive message*/
            if(ret<=0){
                ERROR("ancc0 server: receive message error = %d\n",ret);	/*Connection is broken*/
                break;
            }
            wake_lock_timeout(&vble_wake_lock, 2 * HZ);
            //DEBUG("server: receive len=%d message: %s\n",ret,recvbuf);
            //send ble cmd
            mutex_lock(&vble_devp->lock);
            DEBUG("ancc0 socket receive : %d bytes \n",ret);

            vble_devp->fb_info->vring->length = ret+5+6;
            vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_DATA_REQ;
            vble_devp->fb_info->vring->buffer[1]= CMD_ANCC_GET_DATA;
            vble_devp->fb_info->vring->buffer[2]= ret+2+6; //length
            vble_devp->fb_info->vring->buffer[3]= (unsigned char)(ANCS_UUID&0xff);
            vble_devp->fb_info->vring->buffer[4]= (unsigned char)((ANCS_UUID>>8)&0xff);
            memcpy(&vble_devp->fb_info->vring->buffer[5],ancc0_socket_recvbuf, ret);
            memcpy(&vble_devp->fb_info->vring->buffer[5+ret],vble_info.con_addr,6);
            //printf_string(vble_devp->fb_info->vring->length,&vble_devp->fb_info->vring->buffer[0]);
            ret = vble_send_cmd_mex(CMD_SEND_DATA);
            //DEBUG("ancc socket receive send ble ret=%d buffer[0]=%d\n",ret,vble_devp->fb_info->vring->buffer[0]);
            //printf_string(vble_devp->fb_info->vring->buffer[0]+1,&vble_devp->fb_info->vring->buffer[0]);
            mutex_unlock(&vble_devp->lock);
        }

	}

	/*release socket*/
	//sock_release(client_sock);
err0:   sock_release(ancc0_server_sock);

    return ret;
}
int vble_ancc0_socket_send(char* data ,int len)
{
	int length = 0;
	struct kvec vec;
	struct msghdr msg;

	memset(&msg,0,sizeof(msg));
	vec.iov_base=data;
	vec.iov_len=len;

    if(ancc0_client_sock == NULL)
    {
        return -3;
    }

	if(ancc0_client_sock->state != SS_CONNECTED)
	{
	    ERROR("ancc0 server: send not accept\n");
	    return -1;
	}

	length=kernel_sendmsg(ancc0_client_sock,&msg,&vec,1,len); /*send message */
	if(length<0){
	    ERROR("ancc0 server: kernel_sendmsg error = %d\n",length);
	    return -2;
	}
	//DEBUG("server: send len=%d message[0]:%d\n",length,data[0]);

	return 0;
}


int vble_ancc1_socket_task(void*data){

    struct sockaddr_in s_addr;
    unsigned short portnum=8886;
    int ret=0;
    struct kvec vec;
    struct msghdr msg;

    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(portnum);
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*create a socket*/
    ret=sock_create_kern(AF_INET, SOCK_STREAM,0,&ancc1_server_sock);
    if(ret){
        ERROR("ancc1 server:socket_create error = %d\n",ret);
        return -1;
    }
    DEBUG("server: socket_create ok!\n");

    /*bind the socket*/
	ret=kernel_bind(ancc1_server_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if (ret< 0) {
        ERROR("ancc1 server: bind error = %d\n",ret);
        goto err0;
    }
    DEBUG("ancc1 server: bind ok!\n");

    /*listen*/
	ret=kernel_listen(ancc1_server_sock, 5);
    if(ret<0){
        ERROR("ancc1 server: listen error = %d\n",ret);
        goto err0;
    }
    DEBUG("ancc1 server: listen ok!\n");

	/*accept*/
	for(;;)
	{
        ret=kernel_accept(ancc1_server_sock, &ancc1_client_sock, 0);
        if (ret<0) {
            ERROR("ancc1 server: accept error = %d\n",ret);
            goto err0;
        }
        DEBUG("ancc1 server: accept ok!\n");

        /*receive message from client*/

        for(;;)
        {

            memset(&msg,0,sizeof(msg));
            vec.iov_base=ancc1_socket_recvbuf;
            vec.iov_len=1024;

            ret=kernel_recvmsg(ancc1_client_sock,&msg,&vec,1,1024,0); //receive message//
            if(ret<=0){
                ERROR("ancc1 server: receive message error = %d\n",ret);	//Connection is broken//
                break;
            }
            DEBUG("ancc1 server: receive len=%d message: %s\n",ret,ancc1_socket_recvbuf);
            //send ble cmd
            //memcpy(&vble_devp->fb_info->vring->buffer[0], ancc0_socket_recvbuf, (*ancc0_socket_recvbuf)+1);
            //ret = vble_send_cmd_mex(CMD_SEND_DATA);
            //DEBUG("server: send ble cmd ret=%d buffer[0]=%d\n",ret,vble_devp->fb_info->vring->buffer[0]);

        }


	}

	/*release socket*/
	//sock_release(client_sock);
err0:   sock_release(ancc1_server_sock);

    return ret;
}
int vble_ancc1_socket_send(char* data ,int len)
{
	int length = 0;
	struct kvec vec;
	struct msghdr msg;

	memset(&msg,0,sizeof(msg));
	vec.iov_base=data;
	vec.iov_len=len;

    if(ancc1_client_sock == NULL)
    {
        return -3;
    }

	if(ancc1_client_sock->state != SS_CONNECTED)
	{
	    ERROR("ancc1 server: send not accept\n");
	    return -1;
	}

	length=kernel_sendmsg(ancc1_client_sock,&msg,&vec,1,len); /*send message */
	if(length<0){
	    ERROR("ancc1 server: kernel_sendmsg error = %d\n",length);
	    return -2;
	}
	//DEBUG("server: send len=%d message[0]:%d\n",length,data[0]);

	return 0;
}

int vble_ancc_cmd_task(void*data)
{

    for(;;)
	{
		down(&vble_devp->ancc_sem);
        //DEBUG("vble_ancc_cmd_task\n");

        mutex_lock(&vble_devp->lock);
        if(vble_devp->ancc_next_type == EACI_MSG_TYPE_CMD)
        {
            if(vble_devp->ancc_next_cmd == EACI_MSG_CMD_ADV)
            {
                DEBUG("EACI_MSG_CMD_ADV\n");
                vble_devp->fb_info->vring->length = 8;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_ADV;
                vble_devp->fb_info->vring->buffer[2]= 0x05; //length
                vble_devp->fb_info->vring->buffer[3]= 0x01; //start
                vble_devp->fb_info->vring->buffer[4]= 0x80; //min_adv_intv recommended value: 30 to 60 ms; N * 0.625
                vble_devp->fb_info->vring->buffer[5]= 0x02; //0x280*0.625=400ms
                vble_devp->fb_info->vring->buffer[6]= 0x20;
                vble_devp->fb_info->vring->buffer[7]= 0x03; //0x320*0.625=500ms
                vble_send_cmd_mex(CMD_SEND_DATA);
            }
            else if(vble_devp->ancc_next_cmd == EACI_MSG_CMD_PER_UPDATE_PARAM)
            {
                DEBUG("EACI_MSG_CMD_PER_UPDATE_PARAM\n");
                vble_devp->fb_info->vring->length = 11;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_PER_UPDATE_PARAM;
                vble_devp->fb_info->vring->buffer[2]= 0x08; //length
                vble_devp->fb_info->vring->buffer[3]= (unsigned char)IOS_CONN_INTV_MIN;
                vble_devp->fb_info->vring->buffer[4]= (unsigned char)(IOS_CONN_INTV_MIN >> 8);
                vble_devp->fb_info->vring->buffer[5]= (unsigned char)IOS_CONN_INTV_MAX;
                vble_devp->fb_info->vring->buffer[6]= (unsigned char)(IOS_CONN_INTV_MAX >> 8);
                vble_devp->fb_info->vring->buffer[7]= (unsigned char)IOS_SLAVE_LATENCY;
                vble_devp->fb_info->vring->buffer[8]= (unsigned char)(IOS_SLAVE_LATENCY >> 8);
                vble_devp->fb_info->vring->buffer[9]= (unsigned char)IOS_STO_MULT;
                vble_devp->fb_info->vring->buffer[10]= (unsigned char)(IOS_STO_MULT >> 8);
                vble_send_cmd_mex(CMD_SEND_DATA);
            }
            else if(vble_devp->ancc_next_cmd == EACI_MSG_CMD_BOND)
            {
                DEBUG("EACI_MSG_CMD_BOND\n");
                vble_devp->fb_info->vring->length = 9;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_CMD;
                vble_devp->fb_info->vring->buffer[1]= EACI_MSG_CMD_BOND;
                vble_devp->fb_info->vring->buffer[2]= 0x06; //length
                memcpy(&vble_devp->fb_info->vring->buffer[3],vble_info.con_addr,6);
                vble_send_cmd_mex(CMD_SEND_DATA);
            }
        }
        else if(vble_devp->ancc_next_type == EACI_MSG_TYPE_DATA_REQ)
        {
            if(vble_devp->ancc_next_cmd == CMD_ANCC_ENABLE)
            {
                msleep(100);
                DEBUG("CMD_ANCC_ENABLE\n");
                vble_devp->fb_info->vring->length = 11;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_DATA_REQ;
                vble_devp->fb_info->vring->buffer[1]= CMD_ANCC_ENABLE;
                vble_devp->fb_info->vring->buffer[2]= 0x08; //length
                vble_devp->fb_info->vring->buffer[3]= (unsigned char)(ANCS_UUID&0xff);
                vble_devp->fb_info->vring->buffer[4]= (unsigned char)((ANCS_UUID>>8)&0xff);
                memcpy(&vble_devp->fb_info->vring->buffer[5],vble_info.con_addr,6);
                vble_send_cmd_mex(CMD_SEND_DATA);
            }
            else if(vble_devp->ancc_next_cmd == CMD_ANCC_SET_NTFN)
            {
                DEBUG("CMD_ANCC_SET_NTFN\n");
                vble_devp->fb_info->vring->length = 13;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_DATA_REQ;
                vble_devp->fb_info->vring->buffer[1]= CMD_ANCC_SET_NTFN;
                vble_devp->fb_info->vring->buffer[2]= 0x0A; //length
                vble_devp->fb_info->vring->buffer[3]= (unsigned char)(ANCS_UUID&0xff);
                vble_devp->fb_info->vring->buffer[4]= (unsigned char)((ANCS_UUID>>8)&0xff);
                vble_devp->fb_info->vring->buffer[5]= 0x01;
                vble_devp->fb_info->vring->buffer[6]= 0x00;
                memcpy(&vble_devp->fb_info->vring->buffer[7],vble_info.con_addr,6);
                vble_send_cmd_mex(CMD_SEND_DATA);
            }
            else if(vble_devp->ancc_next_cmd == CMD_ANCC_SET_NTFD)
            {
                DEBUG("CMD_ANCC_SET_NTFD\n");
                vble_devp->fb_info->vring->length = 13;
                vble_devp->fb_info->vring->buffer[0]= EACI_MSG_TYPE_DATA_REQ;
                vble_devp->fb_info->vring->buffer[1]= CMD_ANCC_SET_NTFD;
                vble_devp->fb_info->vring->buffer[2]= 0x0A; //length
                vble_devp->fb_info->vring->buffer[3]= (unsigned char)(ANCS_UUID&0xff);
                vble_devp->fb_info->vring->buffer[4]= (unsigned char)((ANCS_UUID>>8)&0xff);
                vble_devp->fb_info->vring->buffer[5]= 0x01;
                vble_devp->fb_info->vring->buffer[6]= 0x00;
                memcpy(&vble_devp->fb_info->vring->buffer[7],vble_info.con_addr,6);
                vble_send_cmd_mex(CMD_SEND_DATA);
            }
        }
        mutex_unlock(&vble_devp->lock);
    }//for
    return 0;
}

void vble_ancc_receive(eaci_cmd* rec_buf)
{
    switch(rec_buf->cmd)
    {

        case CMD_ANCC_ENABLE:
            if(rec_buf->data[2] == 0x83)//no ancs
            {
                ERROR("CMD_ANCC_ENABLE no ancs server\n");
            }
            else if(rec_buf->data[2] == 0)
            {
                DEBUG("CMD_ANCC_ENABLE OK\n");
                vble_retry_set(EACI_MSG_TYPE_DATA_REQ,CMD_ANCC_SET_NTFD,BLE_RETRY_DELAY_TIME_5);
                vble_devp->ancc_next_type = EACI_MSG_TYPE_DATA_REQ;
                vble_devp->ancc_next_cmd = CMD_ANCC_SET_NTFD;
                up(&vble_devp->ancc_sem);
            }
            else
            {
                ERROR("CMD_ANCC_ENABLE ERROR\n");
            }
            break;

        case CMD_ANCC_SET_NTFN:
            if(rec_buf->data[2] == 0x05)//no permission
            {
                ERROR("CMD_ANCC_SET_NTFN no permission\n");
            }
            else if(rec_buf->data[2] == 0)
            {
                DEBUG("CMD_ANCC_SET_NTFN OK\n");
                vble_devp->ancc_next_cmd = 0;
                vble_info.ancc_status = 1;
                status_socket_send((char*)&vble_info,sizeof(vble_info));
                vble_retry_set(0,0,0);
            }
            else
            {
                ERROR("CMD_ANCC_SET_NTFN ERROR\n");
            }
            break;

        case CMD_ANCC_SET_NTFD:
            if(rec_buf->data[2] == 0x05)//no permission
            {
                ERROR("CMD_ANCC_SET_NTFD no permission\n");
            }
            else if(rec_buf->data[2] == 0)
            {
                DEBUG("CMD_ANCC_SET_NTFD OK\n");
                vble_retry_set(EACI_MSG_TYPE_DATA_REQ,CMD_ANCC_SET_NTFN,BLE_RETRY_DELAY_TIME_5);
                vble_devp->ancc_next_type = EACI_MSG_TYPE_DATA_REQ;
                vble_devp->ancc_next_cmd = CMD_ANCC_SET_NTFN;
                up(&vble_devp->ancc_sem);
            }
            else
            {
                ERROR("CMD_ANCC_SET_NTFD ERROR\n");
            }
            break;

        case CMD_ANCC_GET_MESG:
            DEBUG("ANCC IRQ: vble_ancc0_socket_send %d byte\n",rec_buf->length-2);
            vble_ancc0_socket_send(&rec_buf->data[2] ,rec_buf->length-2);
            break;
        case CMD_ANCC_GET_DATA:
            DEBUG("ANCC IRQ: vble_ancc1_socket_send %d byte\n",rec_buf->length-2);
            vble_ancc1_socket_send(&rec_buf->data[2] ,rec_buf->length-2);
            break;

        default:
            DEBUG("default \n");
    }
}

static void vble_retry_timeout(unsigned long data)
{
    //DEBUG("vble_retry_timeout \n");
    if(ancs_init_ok != 1)
    {
        return;
    }

    if(vble_retry_info.delay == 0)
    {
        //DEBUG("vble_retry_timeout delete type=0x%x cmd=%d\n",vble_retry_info.type,vble_retry_info.cmd);
        return;
    }
    if(vble_retry_info.num >= BLE_RETRY_MAX )
    {
        //ERROR("vble_retry_timeout over max type=0x%x cmd=%d\n",vble_retry_info.type,vble_retry_info.cmd);
        return;
    }
    vble_devp->ancc_next_type = vble_retry_info.type;
    vble_devp->ancc_next_cmd = vble_retry_info.cmd;
    up(&vble_devp->ancc_sem);
    mod_timer(&vble_retry_info.timer,jiffies + (vble_retry_info.delay*HZ));
    vble_retry_info.num++;

}
void vble_retry_set(unsigned char type,unsigned char cmd,unsigned char delay)
{
    //DEBUG("vble_retry_set\n");
    if(ancs_init_ok != 1)
    {
        ERROR("ancs_init_ok != 1\n");
        return;
    }

    //debug
    if(delay == 0)
    {
        //DEBUG("delay == 0\n");
        vble_retry_info.delay = delay;
        mod_timer(&vble_retry_info.timer,jiffies + (delay*HZ));
        return;
    }

    vble_retry_info.type = type;
    vble_retry_info.cmd = cmd;
    vble_retry_info.num = 0;
    vble_retry_info.delay = delay;
    mod_timer(&vble_retry_info.timer,jiffies + (delay*HZ));

}



int vble_ancc_enable(void)
{
    vble_devp->ancc_next_cmd = CMD_ANCC_ENABLE;
    up(&vble_devp->ancc_sem);
    return 0;
}

int vble_ancc_init(void)
{
    int ret;
    static struct task_struct *vble_ancc_cmd;
    static struct task_struct *vble_ancc0_socket;
    static struct task_struct *vble_ancc1_socket;

    vble_ancc_cmd = kthread_create(vble_ancc_cmd_task, NULL, "vble_ancc_cmd_task");
    if(IS_ERR(vble_ancc_cmd)){
        printk("server: unable to start vble_ancc_cmd_task thread.\n");
        ret = PTR_ERR(vble_ancc_cmd);
        vble_ancc_cmd = NULL;
        return ret;
    }
    init_MUTEX(&vble_devp->ancc_sem);
    wake_up_process(vble_ancc_cmd);

    vble_ancc0_socket = kthread_create(vble_ancc0_socket_task, NULL, "vble_ancc0_socket_task");
    if(IS_ERR(vble_ancc0_socket)){
        printk("server: unable to start vble_ancc0_socket_task thread.\n");
        ret = PTR_ERR(vble_ancc0_socket);
        vble_ancc0_socket = NULL;
        return ret;
    }
    wake_up_process(vble_ancc0_socket);

    vble_ancc1_socket = kthread_create(vble_ancc1_socket_task, NULL, "vble_ancc1_socket_task");
    if(IS_ERR(vble_ancc1_socket)){
        printk("server: unable to start vble_ancc1_socket_task thread.\n");
        ret = PTR_ERR(vble_ancc1_socket);
        vble_ancc1_socket = NULL;
        return ret;
    }
    wake_up_process(vble_ancc1_socket);

    init_timer(&vble_retry_info.timer);
    vble_retry_info.timer.expires = jiffies + (1*HZ);
    vble_retry_info.timer.data = (unsigned long) "vble_retry_timer";
    vble_retry_info.timer.function = &vble_retry_timeout;
    vble_retry_info.delay = 0;
    add_timer(&vble_retry_info.timer);

    ancs_init_ok = 1;

    DEBUG("vble_ancc_init OK \n");
    return 0;
}

void printf_string(int len, unsigned char* str)
{
	int i;
	for(i=0;i<len;i++)
	{
		DEBUG("str[%d] = 0x%x\n",i,*(str+i));
	}

}


MODULE_AUTHOR("lonshinetech");
MODULE_LICENSE("GPL");
