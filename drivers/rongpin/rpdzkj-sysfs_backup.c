//[*]--------------------------------------------------------------------------------------------------[*]
//
//
// 
//  rp4412 Board : rpdzkj sysfs driver (charles.park)
//  2012.01.17
// 
///sys/devices/platform/rpdzkj-sysfs/
//[*]--------------------------------------------------------------------------------------------------[*]
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <mach/gpio.h>
#include <linux/gpio.h>

#include <mach/platform.h>

static  unsigned char   HdmiBootArgs[5]="1080";
static  unsigned char   VOutArgs[5]="dvi";

static	ssize_t show_resolution(struct device *dev, struct device_attribute *attr, char *buf);
static	ssize_t show_vout_mode(struct device *dev, struct device_attribute *attr, char *buf);
static 	ssize_t set_poweroff_trigger(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
static	DEVICE_ATTR(hdmi_resolution,	S_IRWXUGO, show_resolution, NULL);
static	DEVICE_ATTR(vout_mode,			S_IRWXUGO, show_vout_mode, NULL);
static	DEVICE_ATTR(poweroff_trigger,	S_IRWXUGO, NULL, set_poweroff_trigger);
static struct device_attribute power_supply_attrs[];

static struct attribute *rpdzkj_sysfs_entries[] = {
	&dev_attr_poweroff_trigger.attr,
	&dev_attr_hdmi_resolution.attr,
	&dev_attr_vout_mode.attr,
	NULL,
};
static struct attribute_group rpdzkj_sysfs_attr_group = {
	.name   = NULL,
	.attrs  = rpdzkj_sysfs_entries,
};

int rpdzkj_get_hdmi_resolution  (void)
{
    // Bootarg setup 1080p
    if(!strncmp("1080", HdmiBootArgs, 4)) 
    	  return   3;
    else if(!strncmp("720", HdmiBootArgs, 4)) 
    	  return   2;
    else if(!strncmp("480", HdmiBootArgs, 4)) 
    	  return   1;
    return   3;
}

int odroid_get_vout_mode  (void)
{
    // Bootarg setup dvi
    if(!strncmp("dvi", VOutArgs, 3))  
    	 	return   1;
	  else 		
	  		return   0;
	  	return   0;		
}
static	ssize_t show_resolution (struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n", rpdzkj_get_hdmi_resolution());
}
static	ssize_t show_vout_mode (struct device *dev, struct device_attribute *attr, char *buf)
{
	return	sprintf(buf, "%d\n", odroid_get_vout_mode() ? 1 : 0);
}


static 	ssize_t set_poweroff_trigger(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    //unsigned int	val;	//没有使用 注释 by rpdzkj ivy 2015.10.29
   gpio_request(CFG_IO_USB_HUB_RESET, "HUB_INT_IRQ");
   gpio_direction_output(CFG_IO_USB_HUB_RESET,1);

  
    return 	count;
}

static int	rpdzkj_sysfs_resume(struct platform_device *dev)
{
   
    return  0;
}

static int	rpdzkj_sysfs_suspend(struct platform_device *dev, pm_message_t state)
{

    return  0;
}

static	int rpdzkj_sysfs_probe(struct platform_device *pdev)	
{
   gpio_request(CFG_IO_USB_HUB_RESET, "HUB_INT_IRQ");
   
   gpio_direction_output(CFG_IO_USB_HUB_RESET,1);
  printk("rpdzkj----------------rpdzkj_sysfs_probe\r\n");
  
		return sysfs_create_group(&pdev->dev.kobj, &rpdzkj_sysfs_attr_group);
	
}

static	int	rpdzkj_sysfs_remove		(struct platform_device *pdev)	
{
	
	 sysfs_remove_group(&pdev->dev.kobj, &rpdzkj_sysfs_attr_group);
    
    return	0;
}

//[*]--------------------------------------------------------------------------------------------------[*]
static struct platform_driver rpdzkj_sysfs_driver = {
	.probe 		= rpdzkj_sysfs_probe,
	.remove 	= __devexit_p(rpdzkj_sysfs_remove),
	.suspend	= rpdzkj_sysfs_suspend,
	.resume		= rpdzkj_sysfs_resume,
	.driver = {
		.name = "rpdzkj-sysfs",
		.owner = THIS_MODULE,
	},
};
static struct platform_device rpdzkj_device_button = {
	.name	= "rpdzkj-sysfs",
	.id		= -1,
};
static int __init rpdzkj_sysfs_init(void)
{	
	  //int value=0; //没有使用 注释 by rpdzkj ivy 2015.10.29
	  platform_device_register(&rpdzkj_device_button);
   return platform_driver_register(&rpdzkj_sysfs_driver);
  
}

static void __exit rpdzkj_sysfs_exit(void)
{
    platform_driver_unregister(&rpdzkj_sysfs_driver);
    platform_device_unregister(&rpdzkj_device_button);
}

late_initcall(rpdzkj_sysfs_init);
module_exit(rpdzkj_sysfs_exit);

//[*]--------------------------------------------------------------------------------------------------[*]
MODULE_DESCRIPTION("SYSFS driver for rpdzkj board");
MODULE_AUTHOR("rpdzkj-kernel");
MODULE_LICENSE("GPL");

