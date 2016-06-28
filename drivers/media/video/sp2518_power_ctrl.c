#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>		//register_chrdev
#include <linux/gpio.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <mach/platform.h>
#include <mach/soc.h>


#include "sp2518_power_ctrl.h"
#if 0
#define GTP_GPIO_OUTPUT(pin,level)      gpio_direction_output(pin,level)
#define GTP_GPIO_REQUEST(pin, label)    gpio_request(pin, label)
#define GTP_GPIO_FREE(pin)              gpio_free(pin)
#endif
static int sp2518_open(struct inode *inode, struct file *file)
{
	printk("%s-%d\n", __func__, __LINE__);

	#if 1   
		gpio_request(CAMERA_PD0,"CAMERA_PD0");
		gpio_direction_output(CAMERA_PD0, 0);
		mdelay(50);
		gpio_free(CAMERA_PD0);
		//sp2518_write_array(client, sp2518_disable_regs);		
		printk("camera power off\n");
	#endif
	    is_sp2528_power_down = 1;
	return 0;
}

//read(fd,&buf,4)
static ssize_t sp2518_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	printk(KERN_INFO"%s()-%d\n", __func__, __LINE__);

	return 0;
}

//write(fd,&val,4)
static ssize_t sp2518_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	int ret, value;
	printk(KERN_INFO"%s()-%d\n", __func__, __LINE__);

	ret=copy_from_user(&value,buf,count);
	if(ret)
	{
		printk(KERN_ERR"copy data from user failed!\n");
		return ENODATA;
	}

	printk(KERN_INFO"copy data from user: val=%d\n", value);
	

	return ret?0:count;
}


static struct file_operations sp2518_fops = {
		.owner	= THIS_MODULE,	
		.open   = sp2518_open,
		.write  = sp2518_write,
		.read   = sp2518_read,
};

static int __init camera_init(void)
{
	int ret;
	
	dev_major = register_chrdev(0,"camera_module",&sp2518_fops);
	if (dev_major < 0) {
		printk(KERN_ERR "unable to get major\n");
		return -EFAULT;
	}	
	
	sp2518_class = class_create(THIS_MODULE,"sp2518_class");
	if (IS_ERR(sp2518_class)) {
		printk(KERN_ERR "class_create() failed for dev_csp2518_class\n");
		ret = -ENODEV;
		goto err_1;	
	}	
	
	sp2518_device = device_create(sp2518_class,NULL,MKDEV(dev_major,0),NULL,"camera_power");	//  /dev/camera_power
	if (IS_ERR(sp2518_device)) {
		printk(KERN_ERR "device_create failed for sp2518_device\n");
		ret = -ENODEV;
		goto err_2;
	}	

    return 0;
err_1:
	class_destroy(sp2518_class);	
err_2:
	unregister_chrdev(dev_major,"camera_module");
	return ret;
	
}

static void __exit camera_exit(void)
{
	unregister_chrdev(dev_major,"camera_module");
	device_destroy(sp2518_class,MKDEV(dev_major,0));
	class_destroy(sp2518_class);
}

module_init(camera_init);
module_exit(camera_exit);
MODULE_LICENSE("GPL v2");

