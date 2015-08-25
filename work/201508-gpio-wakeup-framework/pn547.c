/*
 * Copyright (C) 2010 Trusted Logic S.A.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/spinlock.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/pinctrl/consumer.h>
#include <linux/poll.h>

#include <linux/kthread.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/socket.h>
#include <net/sock.h>

#include "pn547.h"

#define MAX_BUFFER_SIZE	512
#define PN547_DEBUG

struct pn547_dev *pn547_dev;

static int pn547_socket_send(unsigned char* data ,int len);

static void pn547_disable_irq(struct pn547_dev *pn547_dev) {
	unsigned long flags;

	spin_lock_irqsave(&pn547_dev->irq_enabled_lock, flags);
	if (pn547_dev->irq_enabled) {
		//disable_irq_nosync(pn547_dev->client->irq);
		pn547_dev->irq_enabled = false;
	}
	spin_unlock_irqrestore(&pn547_dev->irq_enabled_lock, flags);
}

static irqreturn_t pn547_dev_irq_handler(int irq, void *dev_id) {
    
    unsigned char wake[]={3, 2, 1, 0};
	struct pn547_dev *pn547_dev = dev_id;
    printk("nfc pn547 in irq --------------> \n");
	if (!gpio_get_value(pn547_dev->irq_gpio)) {
		return IRQ_HANDLED;
	}

	pn547_disable_irq(pn547_dev);

    
    pn547_socket_send(&wake[0], 3);

	/* Wake up waiting readers */
	wake_up(&pn547_dev->read_wq);

	return IRQ_HANDLED;
}

static ssize_t pn547_dev_read(struct file *filp, char __user *buf,
		size_t count, loff_t *offset) {
	struct pn547_dev *pn547_dev = filp->private_data;
	char tmp[MAX_BUFFER_SIZE];
	int ret,i;
    
	if (count > MAX_BUFFER_SIZE) {
		count = MAX_BUFFER_SIZE;
    }

	//printk("%s : reading %zu bytes.\n", __func__, count);

	mutex_lock(&pn547_dev->read_mutex);

	if (!gpio_get_value(pn547_dev->irq_gpio)) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			printk("nfc read failed\n");
			goto fail;
		}

		pn547_dev->irq_enabled = true;
		//enable_irq(pn547_dev->client->irq);
		ret = wait_event_interruptible(pn547_dev->read_wq,
				gpio_get_value(pn547_dev->irq_gpio));

		pn547_disable_irq(pn547_dev);

		if (ret) {
			goto fail;
        }
	}

	/* Read data */
	//pn547_dev->client->flags |= 0x40; //XGOLD_I2C_M_B100 100KHz;
	ret = i2c_master_recv(pn547_dev->client, tmp, count);
	mutex_unlock(&pn547_dev->read_mutex);

	if (ret < 0) {
		printk("%s: i2c_master_recv returned %d\n", __func__, ret);
		return ret;
	}
	if (ret > count) {
		printk("%s: received too many bytes from i2c (%d)\n",
			__func__, ret);
		return -EIO;
	}
	if (copy_to_user(buf, tmp, ret)) {
		printk("%s : failed to copy to user space\n", __func__);
		return -EFAULT;
	}
#ifdef PN547_DEBUG	
	printk("IFD->PC:");
	for(i = 0; i < ret; i++) {
		printk(" %02X", tmp[i]);
	}
	printk("\n");
#endif
	return ret;

fail:
	printk("pn547 read failed");
	mutex_unlock(&pn547_dev->read_mutex);
	return ret;
}

static ssize_t pn547_dev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *offset)
{
	struct pn547_dev  *pn547_dev;
	char tmp[MAX_BUFFER_SIZE];
	int ret,i;

	pn547_dev = filp->private_data;

	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	if (copy_from_user(tmp, buf, count)) {
		printk("%s : failed to copy from user space\n", __func__);
		return -EFAULT;
	}

	//printk("%s : writing %zu bytes.\n", __func__, count);
	/* Write data */
	//pn547_dev->client->flags |= 0x40; //XGOLD_I2C_M_B100;
	ret = i2c_master_send(pn547_dev->client, tmp, count);
	if (ret != count) {
		printk("%s : i2c_master_send returned %d\n", __func__, ret);
		ret = -EIO;
		goto exit;
	}

#ifdef PN547_DEBUG	
	printk("PC->IFD:");
	for(i = 0; i < count; i++) {
		printk(" %02X", tmp[i]);
	}
	printk("\n");
#endif

exit:
	return ret;
}

static int pn547_dev_open(struct inode *inode, struct file *filp)
{
	struct pn547_dev *pn547_dev = container_of(filp->private_data,
						struct pn547_dev,
						pn547_device);

	filp->private_data = pn547_dev;

	printk("%s : imajor:%d, iminor%d\n", __func__, imajor(inode), iminor(inode));
	return 0;
}

static int pn547_dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct pn547_dev *pn547_dev = filp->private_data;

	switch (cmd) {
	case PN547_SET_PWR:
		if (arg == 2) {
			/* power on with firmware download (requires hw reset)
			 */
			printk("%s power on with firmware\n", __func__);
			gpio_set_value(pn547_dev->ven_gpio, 1);
			gpio_set_value(pn547_dev->firm_gpio, 1);
			msleep(10);
			gpio_set_value(pn547_dev->ven_gpio, 0);
			msleep(50);
			gpio_set_value(pn547_dev->ven_gpio, 1);
			msleep(10);
		} else if (arg == 1) {
			/* power on */
			printk("%s power on\n", __func__);
			gpio_set_value(pn547_dev->firm_gpio, 0);
			gpio_set_value(pn547_dev->ven_gpio, 1);
			msleep(10);
		} else  if (arg == 0) {
			/* power off */
			printk("%s power off\n", __func__);
			gpio_set_value(pn547_dev->firm_gpio, 0);
			gpio_set_value(pn547_dev->ven_gpio, 0);
			msleep(10);
		} else {
			printk("%s bad arg %lu\n", __func__, arg);
			return -EINVAL;
		}
		break;
	default:
		printk("%s bad ioctl %u\n", __func__, cmd);
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations pn547_dev_fops = {
	.owner	= THIS_MODULE,
	.llseek	= no_llseek,
	.read	= pn547_dev_read,
	.write	= pn547_dev_write,
	.open	= pn547_dev_open,
	.unlocked_ioctl  = pn547_dev_ioctl,
};

#ifdef PN547_DEBUG
static ssize_t show_ldo_on(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	int ret;
/*
    ret = regulator_enable(pn547_dev->vsim2);
    if(ret) {
        printk("vsim2 pmu enable error\n");
    }

    ret = regulator_set_voltage(pn547_dev->vsim2, 1700000, 1800000);
    if(ret<0) {
        printk("vsim2 pmu set voltage error\n");
    }
    return snprintf(buf, PAGE_SIZE, "enable ldo\n");
 */

    return snprintf(buf, PAGE_SIZE, "just for test");
}

static ssize_t store_ldo_off(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t size)
{
/*	int ret;
    ret = regulator_disable(pn547_dev->vsim2);
    if(ret) {
        printk("vsim2 pmu disable error\n");
    }
    */
	return size;
}

static DEVICE_ATTR(ldo, S_IWUSR | S_IRUGO,
		   show_ldo_on, store_ldo_off);

#endif


static struct socket *server_sock = NULL;
static struct socket *client_sock = NULL;
static int pn547_socket_send(unsigned char* data ,int len) {
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
	    printk("pn547 server: send not accept\n");
	    return -1;
	}

	length=kernel_sendmsg(client_sock,&msg,&vec,1,len); /*send message */
	if(length<0){
	    printk("pn547 server: kernel_sendmsg error = %d\n",length);
	    return -2;
	}

	return 0;
}

int pn547_socket_task(void*data) {

    struct sockaddr_in s_addr;
    unsigned short portnum=8892;
    int ret=0;

    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(portnum);
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    /*create a socket*/
    ret=sock_create_kern(AF_INET, SOCK_STREAM, 0, &server_sock);
    if(ret) {
        printk("pn547 server:socket_create error = %d\n",ret);
        return -1;
    }
    printk("pn547 server: socket_create ok!\n");

    /*bind the socket*/
	ret=kernel_bind(server_sock, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if (ret < 0) {
        printk("pn547 server: bind error = %d\n",ret);
        sock_release(server_sock);
        return -1;
    }
    printk("pn547 server: bind ok!\n");

    /*listen*/
	ret=kernel_listen(server_sock, 5);
    if(ret < 0) {
        printk("pn547 server: listen error = %d\n",ret);
        sock_release(server_sock);
        return -1;
    }
    printk("pn547 server: listen ok!\n");

	/*accept*/
	//for(;;)
	{
        ret=kernel_accept(server_sock, &client_sock, 0);
        if (ret<0) {
            printk("pn547 server: accept error = %d\n",ret);
            sock_release(server_sock);
            return -1;
        }
        printk("pn547 server: accept ok!\n");

    }

}







static int pn547_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	
	int ret;
    struct task_struct *pn547_socket;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk("%s : need I2C_FUNC_I2C\n", __func__);
		return  -ENODEV;
	}

	pn547_dev = kzalloc(sizeof(*pn547_dev), GFP_KERNEL);
	if (pn547_dev == NULL) {
		dev_err(&client->dev,
				"failed to allocate memory for module data\n");
		ret = -ENOMEM;
		goto err_exit;
	}

    /* get dts prop */
    pn547_dev->ven_gpio = of_get_named_gpio_flags(client->dev.of_node, "intel,gpio_ven", 0, NULL);
    pn547_dev->irq_gpio = of_get_named_gpio_flags(client->dev.of_node, "intel,gpio_irq", 0, NULL);
    pn547_dev->firm_gpio= of_get_named_gpio_flags(client->dev.of_node, "intel,gpio_firm", 0, NULL);
	pn547_dev->client   = client;

	/* init mutex and queues */
	init_waitqueue_head(&pn547_dev->read_wq);
	mutex_init(&pn547_dev->read_mutex);
	spin_lock_init(&pn547_dev->irq_enabled_lock);

	pn547_dev->pn547_device.minor = MISC_DYNAMIC_MINOR;
	pn547_dev->pn547_device.name = PN547_NAME;
	pn547_dev->pn547_device.fops = &pn547_dev_fops;
    
    /* registe misc device */
	ret = misc_register(&pn547_dev->pn547_device);
	if (ret) {
		printk("%s : misc_register failed\n", __FILE__);
		goto err_misc_register;
	}

    /* PINS config */
    if(gpio_request(pn547_dev->ven_gpio,"nfc_ven") != 0) {
	  printk("PN547: gpio_ven_request error\n");
	  goto err_ven;
	}
    if(gpio_request(pn547_dev->irq_gpio,"nfc_irq") != 0) {
	  printk("PN547: gpio_irq_request error\n");
	  goto err_irq;
	}
    if(gpio_request(pn547_dev->firm_gpio,"nfc_firm") != 0) {
	  printk("PN547: gpio_firm_request error\n");
	  goto err_firm;
	}
 
	gpio_direction_output(pn547_dev->firm_gpio, 0);
	gpio_direction_output(pn547_dev->ven_gpio, 1);
	gpio_direction_input(pn547_dev->irq_gpio);

    //nfc irq
	client->irq = gpio_to_irq(pn547_dev->irq_gpio);
	printk("%s : requesting IRQ %d name = %s \n", __func__, client->irq, client->name);
	pn547_dev->irq_enabled = true;
	ret = request_irq(client->irq, pn547_dev_irq_handler,
			    IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND, client->name, pn547_dev);
	if (ret) {
		printk("request_irq failed\n");
		goto err_request_irq_failed;
	}
	pn547_disable_irq(pn547_dev);

	i2c_set_clientdata(client, pn547_dev);

#ifdef PN547_DEBUG
	ret = device_create_file(&pn547_dev->client->dev, &dev_attr_ldo);
	if(ret<0) {
        printk("nfc debugfs create error\n");
    }
#endif

    pn547_socket = kthread_create(pn547_socket_task, NULL, "pn547_socket_task");
    if(IS_ERR(pn547_socket)){
        printk("server: unable to start kernel thread.\n");
        ret = PTR_ERR(pn547_socket);
        pn547_socket = NULL;
        return ret;
    }
    wake_up_process(pn547_socket);

    device_init_wakeup(&pn547_dev->client->dev, 1);
	printk("nfc probe successful\n");
	return 0;


err_request_irq_failed:

err_firm:
	gpio_free(pn547_dev->firm_gpio);
err_irq:
	gpio_free(pn547_dev->irq_gpio);
err_ven:
    gpio_free(pn547_dev->ven_gpio);
	misc_deregister(&pn547_dev->pn547_device);
err_misc_register:
	mutex_destroy(&pn547_dev->read_mutex);
	kfree(pn547_dev);
err_exit:
	return ret;
}

static int pn547_remove(struct i2c_client *client)
{
	struct pn547_dev *pn547_dev;

	pn547_dev = i2c_get_clientdata(client);
	free_irq(client->irq, pn547_dev);
	misc_deregister(&pn547_dev->pn547_device);
	mutex_destroy(&pn547_dev->read_mutex);
	gpio_free(pn547_dev->irq_gpio);
	gpio_free(pn547_dev->ven_gpio);
	gpio_free(pn547_dev->firm_gpio);
	kfree(pn547_dev);

	return 0;
}

//power manager suspend/resume
static int pn547_suspend(struct device *dev)
{   
	struct i2c_client *client = to_i2c_client(dev);
    printk("pn547 --> suspend\n");

	if (device_may_wakeup(dev))
        enable_irq_wake(client->irq);
    return 0;
}
static int pn547_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
    printk("pn547 <-- resume\n");

	if (device_may_wakeup(dev))
        disable_irq_wake(client->irq);
	return 0;
}
const struct dev_pm_ops pn547_pm = {
	.suspend = pn547_suspend,
	.resume = pn547_resume,
};



static const struct i2c_device_id pn547_id[] = {
	{ PN547_NAME, 0 },
	{ }
};

static struct i2c_driver pn547_driver = {
	.id_table	= pn547_id,
	.probe		= pn547_probe,
	.remove		= pn547_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= PN547_NAME,
		.pm = &pn547_pm,
	},
};

/*
 * module load/unload record keeping
 */

static int __init pn547_dev_init(void)
{
	pr_info("Loading pn547 driver\n");
	return i2c_add_driver(&pn547_driver);
}
module_init(pn547_dev_init);

static void __exit pn547_dev_exit(void)
{
	pr_info("Unloading pn547 driver\n");
	i2c_del_driver(&pn547_driver);
}
module_exit(pn547_dev_exit);

MODULE_DESCRIPTION("NFC PN547 driver");
MODULE_LICENSE("GPL");


