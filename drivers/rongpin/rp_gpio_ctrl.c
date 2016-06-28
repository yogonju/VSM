/*
 * (C) Copyright 2010
 * rpdzkj
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

//TK
#include <asm/gpio.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
//#include <plat/gpio-cfg.h>


//#include <mach/gpio.h>
#include <mach/platform.h>
#include <mach/soc.h>

#include <linux/proc_fs.h>


//以下pr_debug写法可行，只要定义RP_DEBUG就可以打印信息

#undef pr_debug
//#define RP_DEBUG

#ifdef RP_DEBUG
	#define pr_debug(fmt...) printk(fmt)
#else
	#define pr_debug(fmt...)
#endif

//如果J60_NUM == 0，那么驱动控制j60中所以引脚，如果J60_NUM == 4，那么驱动控制j60-5 ~ j60-8
#define J60_NUM 4


extern struct proc_dir_entry proc_root;
static struct proc_dir_entry *root_entry;
static struct proc_dir_entry *entry;
static struct proc_dir_entry *root_entry_485;
static struct proc_dir_entry *entry_485;


static unsigned long gpio[] = {
	PAD_GPIO_C + 29,
	PAD_GPIO_C + 30,
	PAD_GPIO_C + 31,
	PAD_GPIO_D + 0,
	PAD_GPIO_C + 9,
	PAD_GPIO_C + 10,
	PAD_GPIO_C + 12,
	PAD_GPIO_C + 11,
	0,
	PAD_GPIO_ALV + 5,
	PAD_GPIO_B + 30,
	PAD_GPIO_B + 31,
	0,
	PAD_GPIO_ALV + 0,
	//j60后6个引脚无法操作
/*	0,	
	0,
	0,
	0,
	0,
	0,
*/
};

//for choosing gpio or spi function 前四个引脚被上下左右按键占用，如要使用按键功能，请匆操作这四个gpio
static unsigned long gpio_spi[][2] = {
		{PAD_GPIO_C, 29},
		{PAD_GPIO_C, 30},
		{PAD_GPIO_C, 29},
		{PAD_GPIO_D, 0},
		{PAD_GPIO_C, 9},
		{PAD_GPIO_C, 10},
		{PAD_GPIO_C, 12},
		{PAD_GPIO_C, 11},
};


static int gpio_setup(void)
{
	int num, i;
	num = ARRAY_SIZE(gpio);
		
	if(NULL == gpio){
		printk("========  gpio can not find ======\n");
		return -1;
	}

	for(i = J60_NUM; i < 8; i++){
		if(i == 8 || i == 12)
			continue;
		gpio_request(gpio[i], "gpio_j60");
		//gpio_direction_output(gpio[i], 0);
		//gpio_free(gpio[i]);
		//s3c_gpio_cfgpin(gpio[i], S3C_GPIO_SFN(1));//S3C_GPIO_OUTPUT
	}

	return 0;
}
static int gpio_proc_write(struct file * file,const char * buffer,unsigned long count,void * data)
{
	int value;
	int i;
	value = 0; 
	sscanf(buffer, "%d", &value);
	pr_debug("value = %d\n",value);
#if 0
	if (value == 1)
		{	
			gpio_direction_output(gpio[0], 1);
		}
	else if (value == 2)
		{
			gpio_direction_output(gpio[0], 0);
		}
	else if (value == 3)
		{
			gpio_direction_output(gpio[1], 1);
		}
	else if (value == 4)
		{
			gpio_direction_output(gpio[1], 0);
		}
	else if (value == 5)
		{
			//gpio_request(gpio[2], "rp_gpio");
			gpio_direction_output(gpio[2], 1);
			//gpio_free(gpio[2]);
			//msleep(50);
		}
	else if (value == 6)
		{
			gpio_direction_output(gpio[2], 0);
		}
	else if (value == 7)
		{
			gpio_direction_output(gpio[3], 1);
		}
	else if (value == 8)
		{
			gpio_direction_output(gpio[3], 0);
		}
	else
#endif
	 if (value == 9)
		{
			gpio_direction_output(gpio[4], 1);
		}
	else if (value == 10)
		{
			gpio_direction_output(gpio[4], 0);
		}
	else if (value == 11)
		{
			gpio_direction_output(gpio[5], 1);
		}
	else if (value == 12)
		{
			gpio_direction_output(gpio[5], 0);
		}
	else if (value == 13)
		{
			gpio_direction_output(gpio[6], 1);
		}
	else if (value == 14)
		{
			gpio_direction_output(gpio[6], 0);
		}
	else if (value == 15)
		{
			//gpio_request(gpio[7], "rp_gpio");
			gpio_direction_output(gpio[7], 1);
			//gpio_free(gpio[7]);
			//msleep(50);
		}
	else if (value == 16)
		{
			gpio_direction_output(gpio[7], 0);
		}
	//gpio 功能(设置8个引脚，j60:1-8)
	else if (value == 18)
		{	//						gpio组 具体引脚  功能(参考cfg_gpio.h)
			//NX_GPIO_SetPadFunction(index, bit, func);
			for(i = J60_NUM; i < 8; i++){
				if(i < 4)
					NX_GPIO_SetPadFunction(PAD_GET_GROUP(gpio_spi[i][0]), gpio_spi[i][1],NX_GPIO_PADFUNC_0);
				else
					NX_GPIO_SetPadFunction(PAD_GET_GROUP(gpio_spi[i][0]), gpio_spi[i][1],NX_GPIO_PADFUNC_1);		
			}
			pr_debug("setup gpio successfully\n");
			
			
			
		}
	//spi 功能
	else if (value == 17)
		{
			for(i = J60_NUM; i < 8; i++){
				if(i < 4)
					NX_GPIO_SetPadFunction(PAD_GET_GROUP(gpio_spi[i][0]), gpio_spi[i][1],NX_GPIO_PADFUNC_1);
				else
					NX_GPIO_SetPadFunction(PAD_GET_GROUP(gpio_spi[i][0]), gpio_spi[i][1],NX_GPIO_PADFUNC_2);		
			}
			pr_debug("setup spi successfully\n");
			//nxp_key_probe_for_gpio_ctrl();
			
			
		}
	
	return count;
}

static int gpio_proc_read(char * page,char * * start,off_t off,int count,int * eof,void * data)
{
	int value = 0;
	int i, num;
	int len = 0;
#if 1
	num = ARRAY_SIZE(gpio);
	for(i = J60_NUM; i < 8; i++){
		if(i == 8 || i == 12)
			continue;
		value = gpio_get_value(gpio[i]);
		pr_debug("j60-%d:%d\n",i+1, value);
		if(i < 7)
			len += sprintf(page+len, "%d,", value);
		else
			len += sprintf(page+len, "%d", value);
		
	}
#else

	value = gpio_get_value(gpio[0]);
	len = sprintf(page+len, " %d,", value);
	
	value = gpio_get_value(gpio[1]);
	len += sprintf(page+len, " %d,", value);
	
#endif
	
	return len;
}

#if 1
static int RS485_proc_write(struct file *file, const char *buffer, 
                           unsigned long count, void *data) 
{
	int value; 
	value = 0; 
	sscanf(buffer, "%d", &value);
	printk("value = %d\n",value);
	if (value == 0)//485 1 rx
		{
				gpio_direction_output(PAD_GPIO_ALV + 5, 0);
		}
	else if (value == 1) //485 1 tx
		{
				gpio_direction_output(PAD_GPIO_ALV + 5, 1);
		}

    return count; 
}
#endif 

static int rp_gpio_init(void)
{
	

	root_entry = proc_mkdir("gpio_ctrl", &proc_root);
	if(root_entry){
		entry = create_proc_entry("rp_gpio_ctrl", 0666, root_entry);
		if(entry){
			entry->read_proc  = gpio_proc_read;
			entry->write_proc = gpio_proc_write;
			entry->data		 = (void *)0;
		}else{
			printk("^^^^^^^   create_proc_entry entry failed   ^^^^^^^^\n");
		}
	}else{
		printk("^^^^^^^   proc_mkdir root_entry failed   ^^^^^^^^\n");
	}
#if 1
	
	root_entry_485 = proc_mkdir("serial_ctrl", &proc_root);
	
	if(root_entry_485){
		entry_485 = create_proc_entry("485_ctrl" ,0666, root_entry_485);
		if(entry_485)
		{
			entry_485->write_proc = RS485_proc_write;
			entry_485->read_proc =  NULL;
			entry_485->data = (void*)0;
		}
	}
#endif
	gpio_setup();
	
	return 0;

}

static void __exit rp_gpio_exit(void)
{
	printk("rp_gpio_exit success!!!\n");
}


module_init(rp_gpio_init);
module_exit(rp_gpio_exit);

MODULE_AUTHOR("rpdzkj");
MODULE_DESCRIPTION("gpio j60 driver for rpdzkj");
MODULE_LICENSE("GPL");

