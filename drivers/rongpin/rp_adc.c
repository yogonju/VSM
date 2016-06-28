/*
 * 	rpdzkj
 *
 */

//#define DEBUG

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/suspend.h>
#include <linux/notifier.h>

#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>
#if 0
#include "../iio.h"
#include "../sysfs.h"
#include "../machine.h"

#include "nxp_adc.h"
#endif

#include <linux/proc_fs.h>
#include <linux/workqueue.h>


#define ADC_CHANNEL (0)
#define ADC_SAMPLE_CNT (16)

extern struct proc_dir_entry proc_root;
static struct proc_dir_entry *adc_root_entry;
static struct proc_dir_entry *adc_entry;

struct delayed_work adc_event_work;
struct workqueue_struct *adc_workqueue;

static void adc_detect(unsigned long data);

struct timer_list adc_timer = TIMER_INITIALIZER(adc_detect,0,0);

static int adc_raw;

static struct mutex adc_mutex;
//for rp_adc_read (global)
//static struct nxp_adc_info *adc_global = NULL;

static int adc_get_value(int channel);
extern int rp_adc_read(int channel);


static int adc_proc_write(struct file *file, const char *buffer, unsigned long count, void *data) 
{      
	printk("\n adc_proc_write\n");

	return count; 
} 

static int adc_proc_read(char *page, char **start, off_t off, 
			  int count, int *eof, void *data)
{
	int i, value, len = 0;
	
	mutex_lock(&adc_mutex);
	for (i=0; i<1; i++)  {
		value = adc_get_value(0);   // to-do
		len += sprintf(page+len, " %d,", value);
		mdelay(10);
	}
		//value = s_adc_percent;
	len = sprintf(page, "%d", value);
		
	mutex_unlock(&adc_mutex);

	return len;

}
static int adc_get_value(int channel)
{
	int adc_value = 0;
	int retry_cnt = 0;
#if 0
	//Î´Ê¹ÓÃclient
	if (IS_ERR(client)) {
		return -1;  
	}
#endif
	do {
		//iio_st_read_channel_raw
		//adc_value = s3c_adc_read(client,channel);
		//adc_value = nxp_read_raw(struct iio_dev * indio_dev,struct iio_chan_spec const * chan,int * val,int * val2,long mask)
		adc_value = rp_adc_read(channel);
		if (adc_value < 0) {
			pr_info("%s: adc read(%d), retry(%d)", __func__, adc_value, retry_cnt++);
			msleep(10);
		}
	} while (((adc_value < 0) && (retry_cnt <= 5)));

	if(retry_cnt > 5 ) 
		return -1;

	return adc_value;
}

static void adc_get_precise_value(int channel)
{
   	 int count, sum, i, avg;
   	 int buffer[ADC_SAMPLE_CNT];
	
	//int percent,tab_lenth;

	//static int timeout = 0;

	 sum = 0;
   	 count = 0;

	
   	 adc_get_value(channel);	
   	 for(i = 0; i < ADC_SAMPLE_CNT; i++)
   	 {
	        buffer[i] = adc_get_value(channel);
			
			if(sum += buffer[i] != 0)
			{
		    		sum += buffer[i];
				count++;
			}
   	 }
	

	if (count == 0)
	{
		return;  
	}

	avg = sum / count;

    sum = 0;
    count = 0;
    for(i = 0; i < ADC_SAMPLE_CNT; i++)
    {
	        if(abs(buffer[i] - avg) < 200)
	        {
	            sum += buffer[i];
	            count++;
	        }
    }

	if (count == 0)  {
		return;
	}
		
	adc_raw = sum / count;
}

static void adc_work(struct work_struct *work)
{
#if 0
	struct nxp_adc_info *adc = container_of(work, struct nxp_adc_info, monitoring_work.work);
	struct iio_chan_spec const *chan;
	int val = 0;
	int val2 = 0;
	
	chan = &nxp_adc_iio_channels[0];
	nxp_read_raw(adc->iio, chan, &val, &val2, 0);

	return val;
#endif
	adc_get_precise_value(ADC_CHANNEL);

}

static void adc_detect(unsigned long data)
{
	if (!delayed_work_pending(&adc_event_work)) 
	{	
		queue_delayed_work(adc_workqueue, &adc_event_work, 1*HZ);
		//queue_delayed_work(adc->monitoring_wqueue, &adc->monitoring_work, 1*HZ);
	}
	mod_timer(&adc_timer, jiffies + 2*HZ);
}

static int __init rp_adc_init(void)
{
	
	adc_root_entry = proc_mkdir("adc",&proc_root);
	if(adc_root_entry){
		adc_entry = create_proc_entry("adc_ctrl", 0666,adc_root_entry);
		if(adc_entry){
			adc_entry->read_proc 	= adc_proc_read;
			adc_entry->write_proc	= adc_proc_write;
			adc_entry->data			= (void *)0;
		}
	}

	INIT_DELAYED_WORK(&adc_event_work,adc_work);
	adc_workqueue = create_singlethread_workqueue("adc");

	mutex_init(&adc_mutex);

	adc_detect(0);
	
	printk("rp_adc_init success\n");

	return 0;
}

static void __exit rp_adc_exit(void)
{
	printk("rp_adc_exit success\n");
}

module_init(rp_adc_init);
module_exit(rp_adc_exit);
MODULE_AUTHOR("rpdzkj");
MODULE_DESCRIPTION("ADC driver for rp4418");
MODULE_LICENSE("GPL");
