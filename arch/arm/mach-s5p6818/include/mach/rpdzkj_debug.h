#ifndef __RPDZKJ_DEBUG_H__
#define __RPDZKJ_DEBUG_H__


//#undef RPDZKJ_DEBUG
//#define RPDZKJ_DEBUG
#ifdef CONFIG_RPDZKJ_DEBUG
	#define rp_debug(fmt...) printk(fmt)
#else
	#define rp_debug(fmt...)
#endif



#endif 