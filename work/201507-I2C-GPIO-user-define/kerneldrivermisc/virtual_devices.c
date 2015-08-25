/*
 * Copyright (C) 2015 Lonshine.
 * 
 * Virtural devices driver:GPIO, irq, i2c-2...
 *
 *  gpio0 = PCL_1    I/O 
 *  gpio1 = PCL_2    I/O 
 *  gpio2 = PCL_14   I/O    EXI3
 *  gpio3 = PCL_15   I/O 
 *  gpio4 = PCL_33   I/O  
 *  gpio5 = PCL_34   I/O 
 *  gpio6 = PCL_42   I/O 
 *  gpio7 = PCL_50   I/O    EXI6
 *  gpio8 = PCL_51   I/O 
 *  gpio9 = PCL_54   I/O    EXI13
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/timer.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/pm_runtime.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/regulator/consumer.h>


#define VDEV_NAME "vdev"
#define MAX_BUFFER_SIZE (512)

#define GPIO_AMOUNT    (10)

static char null_str[] = "default";
static char in_str[] = "input";
static char out_str[] = "output";
static char irq_str[] = "irq";

enum pin_idx {
    PIN_0 = 0,
    PIN_1,
    PIN_2,
    PIN_3,
    PIN_4,
    PIN_5,
    PIN_6,
    PIN_7,
    PIN_8,
    PIN_9,
};

enum cmd_t {
    CMD_REQUEST_PIN = 0,
    CMD_GET_PIN_CONFIG,
    CMD_SET_PIN_LEVEL,
    CMD_GET_PIN_LEVEL,
    CMD_IRQ_TRIGGER,
    CMD_FREE_PIN,
    CMD_READ_I2C,
    CMD_WRITE_I2C,
};

enum pin_type {
    PIN_TYPE_NULL=0,
    PIN_TYPE_IN,
    PIN_TYPE_OUT,
    PIN_TYPE_IRQ,
};

enum irq_trigger_type {
    IRQ_RISING=0,
    IRQ_FALLING,
    IRQ_BOTH,
};

enum respon {
    SUCCESS=0,
    FAILED = -1,
};


static struct pinctrl *pinctrl;
static struct pinctrl_state *pin_in[GPIO_AMOUNT];
static struct pinctrl_state *pin_out[GPIO_AMOUNT];
static struct pinctrl_state *pin_irq1, *pin_irq2, *pin_irq3;

static unsigned int pin[GPIO_AMOUNT];
static unsigned int pin_type[GPIO_AMOUNT];//store pin type
static int irq1, irq2, irq3;
static int trigger1_type, trigger2_type, trigger3_type;

static struct mutex read_mutex;
static int debug_sw;
static spinlock_t irq_lock;


static int framework_event_rsp(int cmd, int pin, int ret);

struct vdev_dev_t {
	struct i2c_client	*client;
	struct miscdevice	vdev_device;
};

struct vdev_dev_t *vdev_dev;


static irqreturn_t vdev_irq1_handler(int irq, void *dev_id) {
    
	unsigned long flags;
	static int i;

	int level = gpio_get_value(pin[2]);

	spin_lock_irqsave(&irq_lock, flags);
    disable_irq_nosync(irq1);
	spin_unlock_irqrestore(&irq_lock, flags);

    if(level != 0 && trigger1_type != IRQ_FALLING) {
        framework_event_rsp(CMD_IRQ_TRIGGER, PIN_2, IRQ_RISING); 
        printk("requesting IRQ1 ^ %d\n", i++);
    }
    if(level == 0 && trigger1_type != IRQ_RISING) {
        framework_event_rsp(CMD_IRQ_TRIGGER, PIN_2, IRQ_FALLING);
        printk("requesting IRQ1 V %d\n", i++);
    }
	spin_lock_irqsave(&irq_lock, flags);
    enable_irq(irq1);
	spin_unlock_irqrestore(&irq_lock, flags);
    return IRQ_HANDLED;
}
static irqreturn_t vdev_irq2_handler(int irq, void *dev_id) {

	unsigned long flags;

	int level = gpio_get_value(pin[7]);

	spin_lock_irqsave(&irq_lock, flags);
    disable_irq_nosync(irq2);
	spin_unlock_irqrestore(&irq_lock, flags);


    if(level != 0 && trigger2_type != IRQ_FALLING) {
        framework_event_rsp(CMD_IRQ_TRIGGER, PIN_7, IRQ_RISING); 
        printk("requesting IRQ2 ^\n");
    } 
    if(level == 0 && trigger2_type != IRQ_RISING) {
        framework_event_rsp(CMD_IRQ_TRIGGER, PIN_7, IRQ_FALLING);
        printk("requesting IRQ3 V\n");
    }
	spin_lock_irqsave(&irq_lock, flags);
    enable_irq(irq2);
	spin_unlock_irqrestore(&irq_lock, flags);
    return IRQ_HANDLED;
}

static irqreturn_t vdev_irq3_handler(int irq, void *dev_id) {

	unsigned long flags;
	int level = gpio_get_value(pin[9]);

	spin_lock_irqsave(&irq_lock, flags);
    disable_irq_nosync(irq3);
	spin_unlock_irqrestore(&irq_lock, flags);


    if(level != 0 && trigger3_type != IRQ_FALLING) {
        framework_event_rsp(CMD_IRQ_TRIGGER, PIN_9, IRQ_RISING); 
        printk("requesting IRQ3 ^\n");
    }
    if(level == 0 && trigger3_type != IRQ_RISING) {
        framework_event_rsp(CMD_IRQ_TRIGGER, PIN_9, IRQ_FALLING);
        printk("requesting IRQ3 V\n");
    }
	spin_lock_irqsave(&irq_lock, flags);
    enable_irq(irq3);
	spin_unlock_irqrestore(&irq_lock, flags);
    return IRQ_HANDLED;
}




static char *get_config_strings(int type) {

    if(type == PIN_TYPE_NULL) {
        return &null_str[0];
    }
    if(type == PIN_TYPE_IN) {
        return &in_str[0];
    }
    if(type == PIN_TYPE_OUT) {
        return &out_str[0];
    }
    if(type == PIN_TYPE_IRQ) {
        return &irq_str[0];
    }

    return "error";
}

static int config_irq(int n, int t) {
    
    int ret;

    if(n == 2) {
        trigger1_type = t;
        ret = pinctrl_select_state(pinctrl, pin_irq1);
        if(ret<0) {
            printk("pinctrl select state pin irq[%d] error \n", n);
        }
        irq1 = gpio_to_irq(pin[n]);
        printk("%s : requesting IRQ1 %d name = %s \n", __func__, irq1, "irq1");
        ret = request_irq(irq1, vdev_irq1_handler,
                IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND , "irq1", vdev_dev);
        if(ret < 0) {
            printk("request irq1 error\n");
            return -1;
        }
    } else if(n == 7) {
        trigger2_type = t;
        ret = pinctrl_select_state(pinctrl, pin_irq2);
        if(ret<0) {
            printk("pinctrl select state pin irq[%d] error \n", n);
        }
        irq2 = gpio_to_irq(pin[n]);
        printk("%s : requesting IRQ2 %d name = %s \n", __func__, irq2, "irq2");
        ret = request_irq(irq2, vdev_irq2_handler,
                IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND , "irq2", vdev_dev);
        if(ret < 0) {
            printk("request irq2 error\n");
            return -1;
        }

    } else if(n == 9) {
        trigger3_type = t;
        ret = pinctrl_select_state(pinctrl, pin_irq3);
        if(ret<0) {
            printk("pinctrl select state pin irq[%d] error \n", n);
        }
        irq3 = gpio_to_irq(pin[n]);
        printk("%s : requesting IRQ3 %d name = %s \n", __func__, irq3, "irq3");
        ret = request_irq(irq3, vdev_irq3_handler,
                IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND , "irq3", vdev_dev);
        if(ret < 0) {
            printk("request irq3 error\n");
            return -1;
        }
    }

    device_init_wakeup(&pn547_dev->client->dev, 1);
    
    return ret;
}


/*
 *  pin:    pin 0-9
 *  type:   NULL/IN/OUT/IRQ
 *  value:  
 *          IN:     value=0
 *          OUT:    value=0/1   reset value
 *          IRQ:    value={emun irq_trigger_type}
 */
static int config_pin(int n, enum pin_type type, int value) {

    int ret=-1;

    if(n<0 || n > 9 || type < 0 || type > 3 || value < 0 || value > 2) {
        printk("config pin params error\n");
        return -1;
    }

    printk("config pin n=%d type=%d value=%d\n", n, type, value);
    if(type == PIN_TYPE_NULL) {

        if(pin_type[n] == PIN_TYPE_IRQ) {
            if(n == 2) {
                free_irq(irq1, vdev_dev);
            } else if(n == 7) {
                free_irq(irq2, vdev_dev);
            } else if (n == 90) {
                free_irq(irq3, vdev_dev);
            }
        }
        ret = pinctrl_select_state(pinctrl, pin_in[n]);
        if(ret<0) {
            printk("pinctrl select state pin null[%d] error \n", n);
            return -1;
        }
        gpio_direction_input(pin[n]);
        pin_type[n] = PIN_TYPE_NULL;
        return 0;
    }

    if(type ==  PIN_TYPE_IN) {
        ret = pinctrl_select_state(pinctrl, pin_in[n]);
        if(ret<0) {
            printk("pinctrl select state pin in[%d] error\n", n);
            return -1;
        }
        gpio_direction_input(pin[n]);
        pin_type[n] = PIN_TYPE_IN;
        return 0;
    }

    if(type ==  PIN_TYPE_OUT) {
        ret = pinctrl_select_state(pinctrl, pin_out[n]);
        if(ret<0) {
            printk("pinctrl select state pin out[%d] error \n", n);
            return -1;
        }
        gpio_direction_output(pin[n], value>0?1:0);
        pin_type[n] = PIN_TYPE_OUT;
        return 0;
    }

    if(type ==  PIN_TYPE_IRQ && (n == 2 || n == 7 || n == 9)) {

        config_irq(n, value);
        pin_type[n] = PIN_TYPE_IRQ;
        return 0;
    }
    
    return ret;
}



static int vdev_dev_open(struct inode *inode, struct file *filp) {

	struct vdev_dev_t *vdev_dev = container_of(filp->private_data,
						struct vdev_dev_t,
						vdev_device);

	filp->private_data = vdev_dev;

	printk("%s success: imajor:%d, iminor%d\n", __func__, imajor(inode), iminor(inode));
	return 0;
}
static ssize_t vdev_dev_read(struct file *filp, char __user *buf,
		size_t count, loff_t *offset) {

    
	return 0;
}

static ssize_t vdev_dev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *offset) {

	return 0;
}


static int vdev_dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	return 0;
}
static const struct file_operations vdev_dev_fops = {
	.owner	= THIS_MODULE,
	.llseek	= no_llseek,
	.read	= vdev_dev_read,
	.write	= vdev_dev_write,
	.open	= vdev_dev_open,
	.unlocked_ioctl  = vdev_dev_ioctl,
};

//debug
static ssize_t show_debug(struct device *dev,
				   struct device_attribute *attr, char *buf) {

    return snprintf(buf, PAGE_SIZE, "debug switch %s\n", debug_sw>0 ? "on" : "off");
}

static ssize_t store_debug(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t size) {

    char onoff = buf[0];
    if(onoff == '0') {
        debug_sw = 0;
        printk("switch debug off\n");
    } else {
        debug_sw = 1;
        printk("switch debug on\n");
    }

	return size;
}

//config
static ssize_t show_pins_config(struct device *dev,
        struct device_attribute *attr, char *buf) {

    int i;
    char strings[256]={0};
    char str[128];
    if(debug_sw) {
        for(i=0; i<10; i++) {
            memset(str, 0, sizeof(str));
            sprintf(str, "pin[%d]:%s\n", i, get_config_strings(pin_type[i]));
            strcat(strings, str);
        }
        return snprintf(buf, PAGE_SIZE, "%s", strings);
    } else {
        return snprintf(buf, PAGE_SIZE, "debug switch %s\n", debug_sw>0 ? "on" : "off");
    }
}

static ssize_t store_pins_config(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t size) {

    int pin = buf[0]-48;
    int config = buf[1]-48;
    int value = buf[2]-48;

    printk("pin %d\nconfig %d\nvalue %d\n", pin, config, value);
    if(pin >=0 && pin <=9 && config >= 0 && config <= 3 && debug_sw > 0) {
        config_pin(pin, config, value);
        return size;
    }

    printk("pins config error\n");
    return size;
}

//value
static ssize_t show_pins_value(struct device *dev,
				   struct device_attribute *attr, char *buf) {

    int i;
    char strings[128]={0};
    char str[128]={0};
    if(debug_sw) {
        for(i=0; i<10; i++) {
            if(pin_type[i] == PIN_TYPE_IN) {
                memset(str, 0, sizeof(str));
                sprintf(str, "pin[%d]:%d\n", i, gpio_get_value(pin[i]));
                strcat(strings, str);
            }
        }
        return snprintf(buf, PAGE_SIZE, "%s\n", strings);
    } else {
        return snprintf(buf, PAGE_SIZE, "debug switch %s\n", debug_sw>0 ? "on" : "off");
    }
}

static ssize_t store_pins_value(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t size) {
    int n = buf[0]-48;
    int v = buf[1]-48;

    if(n >=0 && n <=9 && v >= 0 && v <= 1 && debug_sw > 0) {
        if(pin_type[n] == PIN_TYPE_OUT) {
            gpio_set_value(pin[n], v);
            return size;
        }
    }

    printk("pins set value error\n");
    return size;
}



static DEVICE_ATTR(debug, S_IWUSR | S_IRUGO,
		   show_debug, store_debug);
static DEVICE_ATTR(pconfig, S_IWUSR | S_IRUGO,
		   show_pins_config, store_pins_config);
static DEVICE_ATTR(pvalue, S_IWUSR | S_IRUGO,
		   show_pins_value, store_pins_value);


static struct socket *server_sock = NULL;
static struct socket *client_sock = NULL;
static int vdev_socket_recv(unsigned char *bf, int len) {

    int ret=0;
    struct kvec vec;
    struct msghdr msg;

    memset(&msg,0,sizeof(msg));
    vec.iov_base=bf;
    vec.iov_len=len;

    ret=kernel_recvmsg(client_sock,&msg,&vec,1,len,0); 

    return ret;

}

static int vdev_socket_send(unsigned char* data ,int len) {
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
	    printk("vdev server: send not accept\n");
	    return -1;
	}

	length=kernel_sendmsg(client_sock,&msg,&vec,1,len); /*send message */
	if(length<0){
	    printk("vdev server: kernel_sendmsg error = %d\n",length);
	    return -2;
	}

	return 0;
}

static int framework_event_rsp(int cmd, int pin, int ret) {
    unsigned char resp[4]={0};
    printk("rsp: cmd=%d pin=%d ret=%d\n", cmd, pin, ret);
    resp[0] = cmd;
    resp[1] = pin;
    resp[2] = ret;
    return vdev_socket_send(&resp[0], 3);
}

int process_framework_event(int cmd) {
    int ret = 0;
    unsigned char data[256]={0};

    switch(cmd) {

        case CMD_REQUEST_PIN:
            ret = vdev_socket_recv(data, 3);
            if(ret < 3) {
                printk("vdev socket recv error\n");
                ret = FAILED;
                break;
            }
            ret = config_pin(data[0], data[1], data[2]);
            break;

        case CMD_GET_PIN_CONFIG:
            ret = vdev_socket_recv(data, 1);
            if(ret < 1) {
                printk("vdev socket recv error\n");
                ret = FAILED;
                break;
            }
            if(data[0] < 0 || data[0] > 9) {
                printk("vdev get pin config param error\n");
                ret = FAILED;
            }
            ret = pin_type[data[0]];
            break;

        case CMD_SET_PIN_LEVEL:
            ret = vdev_socket_recv(data, 2);
            if(ret < 2) {
                printk("vdev socket recv error\n");
                ret = FAILED;
                break;
            }
            if(data[0] < 0 || data[0] > 9) {
                printk("vdev get pin config param error\n");
                ret = FAILED;
                break;
            }
            if(pin_type[data[0]] != PIN_TYPE_OUT) {
                ret = FAILED;
                break;
            }
            printk("pin %d = %d\n", data[0], data[1]>0?1:0);
            gpio_set_value(pin[data[0]], data[1]>0?1:0);
            ret = SUCCESS;
            break;

        case CMD_GET_PIN_LEVEL:
            ret = vdev_socket_recv(data, 1);
            if(ret < 1) {
                printk("vdev socket recv error\n");
                ret = FAILED;
                break;
            }
            if(data[0] < 0 || data[0] > 9) {
                printk("vdev get pin config param error\n");
                ret = FAILED;
                break;
            }
            if(pin_type[data[0]] != PIN_TYPE_IN) {
                ret = FAILED;
                break;
            }
            ret = gpio_get_value(pin[data[0]]);
            break;

        case CMD_FREE_PIN:
            ret = vdev_socket_recv(data, 1);
            if(ret < 1) {
                printk("vdev socket recv error\n");
                ret = FAILED;
                break;
            }
            if(data[0] < 0 || data[0] > 9) {
                printk("vdev get pin config param error\n");
                ret = FAILED;
                break;
            }
            ret = config_pin(data[0], PIN_TYPE_NULL, 0);
            if(ret <0) {
                ret = FAILED;
            } else {
                ret = SUCCESS;
            }
            break;

        case CMD_READ_I2C:
        case CMD_WRITE_I2C:
        default:
            ret = FAILED;
            break;

    }

    return framework_event_rsp(cmd, data[0], ret);
}



int vdev_socket_task(void*data) {

    struct sockaddr_in s_addr;
    unsigned short portnum=8891;
    int ret=0;

    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(portnum);
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*create a socket*/
    ret=sock_create_kern(AF_INET, SOCK_STREAM, 0, &server_sock);
    if(ret) {
        printk("vdev server:socket_create error = %d\n",ret);
        return -1;
    }
    printk("vdev server: socket_create ok!\n");

    /*bind the socket*/
	ret=kernel_bind(server_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if (ret < 0) {
        printk("vdev server: bind error = %d\n",ret);
        sock_release(server_sock);
        return -1;
    }
    printk("vdev server: bind ok!\n");

    /*listen*/
	ret=kernel_listen(server_sock, 5);
    if(ret < 0) {
        printk("vdev server: listen error = %d\n",ret);
        sock_release(server_sock);
        return -1;
    }
    printk("vdev server: listen ok!\n");

	/*accept*/
	for(;;)
	{
        ret=kernel_accept(server_sock, &client_sock, 0);
        if (ret<0) {
            printk("vdev server: accept error = %d\n",ret);
            sock_release(server_sock);
            return -1;
        }
        printk("vdev server: accept ok!\n");

        /*receive message from client*/
        for(;;)
        {
            char cmd=-1;
            ret = vdev_socket_recv(&cmd, 1);
            if(ret < 0) {
                printk("vdev socket recv error\n");
                break;
            }
            ret = process_framework_event(cmd);
            if(ret < 0) {
                printk("vdev socket process error\n");
                break;
            }
        }//for
	}

}

static int get_pins_from_dts(void) {

	pinctrl = devm_pinctrl_get(&vdev_dev->client->dev);

	pin_in[0] = pinctrl_lookup_state(pinctrl, "gpio0_in");
	pin_in[1] = pinctrl_lookup_state(pinctrl, "gpio1_in");
	pin_in[2] = pinctrl_lookup_state(pinctrl, "gpio2_in");
	pin_in[3] = pinctrl_lookup_state(pinctrl, "gpio3_in");
	pin_in[4] = pinctrl_lookup_state(pinctrl, "gpio4_in");
	pin_in[5] = pinctrl_lookup_state(pinctrl, "gpio5_in");
	pin_in[6] = pinctrl_lookup_state(pinctrl, "gpio6_in");
	pin_in[7] = pinctrl_lookup_state(pinctrl, "gpio7_in");
	pin_in[8] = pinctrl_lookup_state(pinctrl, "gpio8_in");
	pin_in[9] = pinctrl_lookup_state(pinctrl, "gpio9_in");
	pin_out[0] = pinctrl_lookup_state(pinctrl, "gpio0_out");
	pin_out[1] = pinctrl_lookup_state(pinctrl, "gpio1_out");
	pin_out[2] = pinctrl_lookup_state(pinctrl, "gpio2_out");
	pin_out[3] = pinctrl_lookup_state(pinctrl, "gpio3_out");
	pin_out[4] = pinctrl_lookup_state(pinctrl, "gpio4_out");
	pin_out[5] = pinctrl_lookup_state(pinctrl, "gpio5_out");
	pin_out[6] = pinctrl_lookup_state(pinctrl, "gpio6_out");
	pin_out[7] = pinctrl_lookup_state(pinctrl, "gpio7_out");
	pin_out[8] = pinctrl_lookup_state(pinctrl, "gpio8_out");
	pin_out[9] = pinctrl_lookup_state(pinctrl, "gpio9_out");
	pin_irq1 = pinctrl_lookup_state(pinctrl, "irq1");
	pin_irq2 = pinctrl_lookup_state(pinctrl, "irq2");
	pin_irq3 = pinctrl_lookup_state(pinctrl, "irq3");
  
    pin[0] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio0", 0, NULL);
    pin[1] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio1", 0, NULL);
    pin[2] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio2", 0, NULL);
    pin[3] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio3", 0, NULL);
    pin[4] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio4", 0, NULL);
    pin[5] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio5", 0, NULL);
    pin[6] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio6", 0, NULL);
    pin[7] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio7", 0, NULL);
    pin[8] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio8", 0, NULL);
    pin[9] = of_get_named_gpio_flags(vdev_dev->client->dev.of_node, "intel,gpio9", 0, NULL);

    if(gpio_request(pin[0],"pin0") != 0) {
	  printk("vdev: pin0 request error\n");
	}
    if(gpio_request(pin[1],"pin1") != 0) {
	  printk("vdev: pin1 request error\n");
	}
    if(gpio_request(pin[2],"pin2") != 0) {
	  printk("vdev: pin2 request error\n");
	}
    if(gpio_request(pin[3],"pin3") != 0) {
	  printk("vdev: pin3 request error\n");
	}
    if(gpio_request(pin[4],"pin4") != 0) {
	  printk("vdev: pin4 request error\n");
	}
    if(gpio_request(pin[5],"pin5") != 0) {
	  printk("vdev: pin5 request error\n");
	}
    if(gpio_request(pin[6],"pin6") != 0) {
	  printk("vdev: pin6 request error\n");
	}
    if(gpio_request(pin[7],"pin7") != 0) {
	  printk("vdev: pin7 request error\n");
	}
    if(gpio_request(pin[8],"pin8") != 0) {
	  printk("vdev: pin8 request error\n");
	}
    if(gpio_request(pin[9],"pin9") != 0) {
	  printk("vdev: pin9 request error\n");
	}
	gpio_direction_input(pin[0]);
	gpio_direction_input(pin[1]);
	gpio_direction_input(pin[2]);
	gpio_direction_input(pin[3]);
	gpio_direction_input(pin[4]);
	gpio_direction_input(pin[5]);
	gpio_direction_input(pin[6]);
	gpio_direction_input(pin[7]);
	gpio_direction_input(pin[8]);
	gpio_direction_input(pin[9]);

    return 0;

}



static int vdev_probe(struct i2c_client *client,
		const struct i2c_device_id *id) {
	
	int ret;
    struct task_struct *vdev_socket;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk("%s : need I2C_FUNC_I2C\n", __func__);
		return  -ENODEV;
	}

	vdev_dev = kzalloc(sizeof(*vdev_dev), GFP_KERNEL);
	if (vdev_dev == NULL) {
		dev_err(&client->dev,
				"failed to allocate memory for module data\n");
		ret = -ENOMEM;
		goto err_exit;
	}

 	vdev_dev->client   = client;
	vdev_dev->vdev_device.minor = MISC_DYNAMIC_MINOR;
	vdev_dev->vdev_device.name = VDEV_NAME;
	vdev_dev->vdev_device.fops = &vdev_dev_fops;
    
    /* registe misc device */
	ret = misc_register(&vdev_dev->vdev_device);
	if (ret) {
		printk("%s : misc_register failed\n", __FILE__);
		goto err_misc_register;
	}

	i2c_set_clientdata(client, vdev_dev);

	mutex_init(&read_mutex);
	spin_lock_init(&irq_lock);
    
    //debug interface
	ret = device_create_file(&vdev_dev->client->dev, &dev_attr_debug);
	if(ret<0) {
        printk("Lonshine virtual devices debugfs create in error\n");
        goto err_all;
    }
	ret = device_create_file(&vdev_dev->client->dev, &dev_attr_pconfig);
	if(ret<0) {
        printk("Lonshine virtual devices debugfs create out error\n");
        goto err_all;
    }
	ret = device_create_file(&vdev_dev->client->dev, &dev_attr_pvalue);
	if(ret<0) {
        printk("Lonshine virtual devices debugfs create out error\n");
        goto err_all;
    }

    get_pins_from_dts();


    //socket
    vdev_socket = kthread_create(vdev_socket_task, NULL, "vdev_socket_task");
    if(IS_ERR(vdev_socket)){
        printk("server: unable to start kernel thread.\n");
        ret = PTR_ERR(vdev_socket);
        vdev_socket = NULL;
        return ret;
    }
    wake_up_process(vdev_socket);

	printk("Lonshine virtual devices probe successful\n");
	return 0;


err_all:
	mutex_destroy(&read_mutex);
	misc_deregister(&vdev_dev->vdev_device);
err_misc_register:
	kfree(vdev_dev);
err_exit:
	return ret;
}

static int vdev_remove(struct i2c_client *client) {
	struct vdev_dev_t *vdev_dev;

	vdev_dev = i2c_get_clientdata(client);
	misc_deregister(&vdev_dev->vdev_device);
	kfree(vdev_dev);

	return 0;
}

//power manager suspend/resume
static int vdev_suspend(struct device *dev)
{   
	struct i2c_client *client = to_i2c_client(dev);
    printk("vdev --> suspend\n");

	if (device_may_wakeup(dev))
        enable_irq_wake(client->irq);
    return 0;
}
static int vdev_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
    printk("vdev --> resume\n");

	if (device_may_wakeup(dev))
        disable_irq_wake(client->irq);
	return 0;
}
const struct dev_pm_ops vdev_pm = {
	.suspend = vdev_suspend,
	.resume = vdev_resume,
};

static const struct i2c_device_id vdev_id[] = {
	{ VDEV_NAME, 0 },
	{ }
};

static struct i2c_driver vdev_driver = {
	.id_table	= vdev_id,
	.probe		= vdev_probe,
	.remove		= vdev_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= VDEV_NAME,
		.pm = &vdev_pm,
	},
};


//driver module attach/de-attach
static int __init vdev_dev_init(void) {
	pr_info("Loading vdev driver\n");
	return i2c_add_driver(&vdev_driver);
}
module_init(vdev_dev_init);

static void __exit vdev_dev_exit(void) {
	pr_info("Unloading vdev driver\n");
	i2c_del_driver(&vdev_driver);
}
module_exit(vdev_dev_exit);

MODULE_DESCRIPTION("Lonshine virtural devices driver");
MODULE_LICENSE("GPL");



