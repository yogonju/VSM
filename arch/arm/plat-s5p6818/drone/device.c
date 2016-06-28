/*
 * (C) Copyright 2009
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/irq.h>
#include <linux/amba/pl022.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>

#if defined(CONFIG_NXP_HDMI_CEC)
#include <mach/nxp-hdmi-cec.h>
#endif


/*------------------------------------------------------------------------------
 * BUS Configure
 */
#if (CFG_BUS_RECONFIG_ENB == 1)
#include <mach/s5p6818_bus.h>

const u16 g_DrexQoS[2] = {
	0x100,		// S0
	0xFFF		// S1, Default value
};

const u8 g_TopBusSI[8] = {
	TOPBUS_SI_SLOT_DMAC0,
	TOPBUS_SI_SLOT_USBOTG,
	TOPBUS_SI_SLOT_USBHOST0,
	TOPBUS_SI_SLOT_DMAC1,
	TOPBUS_SI_SLOT_SDMMC,
	TOPBUS_SI_SLOT_USBOTG,
	TOPBUS_SI_SLOT_USBHOST1,
	TOPBUS_SI_SLOT_USBOTG
};

const u8 g_BottomBusSI[8] = {
	BOTBUS_SI_SLOT_1ST_ARM,
	BOTBUS_SI_SLOT_MALI,
	BOTBUS_SI_SLOT_DEINTERLACE,
	BOTBUS_SI_SLOT_1ST_CODA,
	BOTBUS_SI_SLOT_2ND_ARM,
	BOTBUS_SI_SLOT_SCALER,
	BOTBUS_SI_SLOT_TOP,
	BOTBUS_SI_SLOT_2ND_CODA
};

const u8 g_BottomQoSSI[2] = {
	1,	// Tidemark
	(1<<BOTBUS_SI_SLOT_1ST_ARM) |	// Control
	(1<<BOTBUS_SI_SLOT_2ND_ARM) |
	(1<<BOTBUS_SI_SLOT_MALI) |
	(1<<BOTBUS_SI_SLOT_TOP) |
	(1<<BOTBUS_SI_SLOT_DEINTERLACE) |
	(1<<BOTBUS_SI_SLOT_1ST_CODA)
};

#if 0 //rpdzkj mask for mxliao 2015-10-23
const u8 g_DispBusSI[3] = {
	DISBUS_SI_SLOT_1ST_DISPLAY,
	DISBUS_SI_SLOT_2ND_DISPLAY,
	DISBUS_SI_SLOT_2ND_DISPLAY  //DISBUS_SI_SLOT_GMAC
};
#else
const u8 g_DispBusSI[3] = {
	DISBUS_SI_SLOT_1ST_DISPLAY,
	DISBUS_SI_SLOT_2ND_DISPLAY,
	DISBUS_SI_SLOT_GMAC  //DISBUS_SI_SLOT_GMAC
};

#endif

#endif	/* #if (CFG_BUS_RECONFIG_ENB == 1) */

/*------------------------------------------------------------------------------
 * CPU Frequence
 */
#if defined(CONFIG_ARM_NXP_CPUFREQ)

/*
struct nxp_cpufreq_plat_data dfs_plat_data = {
	.pll_dev	   	= CONFIG_NXP_CPUFREQ_PLLDEV,
	.supply_name	= "vdd_arm_1.3V",
	.supply_delay_us = 0,
};
*/

#define HIGH_PERFORMANCE 0//define by rpdzkj jeff

static unsigned long dfs_freq_table[][2] = {
#if HIGH_PERFORMANCE
	{ 1400000, },
/*		
	{ 1200000, },
	{ 1000000, },
	{  800000, },
	{  700000, },
	{  600000, },
	{  500000, },
	{  400000, },
*/	
#else
	{ 1400000, },
	{ 1200000, },
	{ 1000000, },
	{  800000, },
	{  700000, },
	{  600000, },
	{  500000, },
	{  400000, },
#endif

};

struct nxp_cpufreq_plat_data dfs_plat_data = {
	.pll_dev	   	= CONFIG_NXP_CPUFREQ_PLLDEV,
	.supply_name	= "vdd_arm_1.3V",	//refer to CONFIG_REGULATOR_NXE2000
	.freq_table	   	= dfs_freq_table,
	.table_size	   	= ARRAY_SIZE(dfs_freq_table),
	.max_cpufreq	= 1400*1000,
	.max_retention  =   20*1000,
#if HIGH_PERFORMANCE
	.rest_cpufreq   =  1400*1000,
#else
	.rest_cpufreq   =  400*1000,
#endif
	.rest_retention =    1*1000,
};


static struct platform_device dfs_plat_device = {
	.name			= DEV_NAME_CPUFREQ,
	.dev			= {
		.platform_data	= &dfs_plat_data,
	}
};

#endif

/*------------------------------------------------------------------------------
 * Network DM9000
 */
#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
#include <linux/dm9000.h>

static struct resource dm9000_resource[] = {
	[0] = {
		.start	= CFG_ETHER_EXT_PHY_BASEADDR,
		.end	= CFG_ETHER_EXT_PHY_BASEADDR + 1,		// 1 (8/16 BIT)
		.flags	= IORESOURCE_MEM
	},
	[1] = {
		.start	= CFG_ETHER_EXT_PHY_BASEADDR + 4,		// + 4 (8/16 BIT)
		.end	= CFG_ETHER_EXT_PHY_BASEADDR + 5,		// + 5 (8/16 BIT)
		.flags	= IORESOURCE_MEM
	},
	[2] = {
		.start	= CFG_ETHER_EXT_IRQ_NUM,
		.end	= CFG_ETHER_EXT_IRQ_NUM,
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	}
};

static struct dm9000_plat_data eth_plat_data = {
	.flags		= DM9000_PLATF_8BITONLY,	// DM9000_PLATF_16BITONLY
};

static struct platform_device dm9000_plat_device = {
	.name			= "dm9000",
	.id				= 0,
	.num_resources	= ARRAY_SIZE(dm9000_resource),
	.resource		= dm9000_resource,
	.dev			= {
		.platform_data	= &eth_plat_data,
	}
};
#endif	/* CONFIG_DM9000 || CONFIG_DM9000_MODULE */

/*------------------------------------------------------------------------------
 * DW GMAC board config
 */
#if defined(CONFIG_NXPMAC_ETH)
#include <linux/phy.h>
#include <linux/nxpmac.h>
#include <linux/delay.h>
#include <linux/gpio.h>
int  nxpmac_init(struct platform_device *pdev)
{
    u32 addr;

#if defined (CONFIG_REALTEK_PHY_RTL8201)	// 20140515
	// 100 & 10Base-T
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+ 7, 0 );        // PAD_GPIOE7,     GMAC0_PHY_TXD[0]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+ 8, 0 );        // PAD_GPIOE8,     GMAC0_PHY_TXD[1]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+ 9, 0 );        // PAD_GPIOE9,     GMAC0_PHY_TXD[2]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+10, 0 );        // PAD_GPIOE10,    GMAC0_PHY_TXD[3]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+11, 0 );        // PAD_GPIOE11,    GMAC0_PHY_TXEN
//  nxp_soc_gpio_set_io_drv( PAD_GPIO_E+12, 3 );        // PAD_GPIOE12,    GMAC0_PHY_TXER
//  nxp_soc_gpio_set_io_drv( PAD_GPIO_E+13, 3 );        // PAD_GPIOE13,    GMAC0_PHY_COL
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+14, 2 );        // PAD_GPIOE14,    GMAC0_PHY_RXD[0]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+15, 2 );        // PAD_GPIOE15,    GMAC0_PHY_RXD[1]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+16, 2 );        // PAD_GPIOE16,    GMAC0_PHY_RXD[2]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+17, 2 );        // PAD_GPIOE17,    GMAC0_PHY_RXD[3]
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+18, 3 );        // PAD_GPIOE18,    GMAC0_RX_CLK
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+19, 3);        // PAD_GPIOE19,    GMAC0_PHY_RX_DV
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+20, 3 );        // PAD_GPIOE20,    GMAC0_GMII_MDC
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+21, 3 );        // PAD_GPIOE21,    GMAC0_GMII_MDI
//  nxp_soc_gpio_set_io_drv( PAD_GPIO_E+22, 3 );        // PAD_GPIOE22,    GMAC0_PHY_RXER
//  nxp_soc_gpio_set_io_drv( PAD_GPIO_E+23, 3 );        // PAD_GPIOE23,    GMAC0_PHY_CRS
    nxp_soc_gpio_set_io_drv( PAD_GPIO_E+24, 0 );        // PAD_GPIOE24,    GMAC0_GTX_CLK

#else	// 1000Base-T
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E +  7), 3 );     // PAD_GPIOE7,     GMAC0_PHY_TXD[0]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E +  8), 3 );     // PAD_GPIOE8,     GMAC0_PHY_TXD[1]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E +  9), 3 );     // PAD_GPIOE9,     GMAC0_PHY_TXD[2]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 10), 3 );     // PAD_GPIOE10,    GMAC0_PHY_TXD[3]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 11), 3 );     // PAD_GPIOE11,    GMAC0_PHY_TXEN
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 14), 3 );     // PAD_GPIOE14,    GMAC0_PHY_RXD[0]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 15), 3 );     // PAD_GPIOE15,    GMAC0_PHY_RXD[1]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 16), 3 );     // PAD_GPIOE16,    GMAC0_PHY_RXD[2]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 17), 3 );     // PAD_GPIOE17,    GMAC0_PHY_RXD[3]
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 18), 3 );     // PAD_GPIOE18,    GMAC0_RX_CLK
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 19), 3 );     // PAD_GPIOE19,    GMAC0_PHY_RX_DV
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 20), 3 );     // PAD_GPIOE20,    GMAC0_GMII_MDC
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 21), 3 );     // PAD_GPIOE21,    GMAC0_GMII_MDI
	nxp_soc_gpio_set_io_drv( (PAD_GPIO_E + 24), 3 );     // PAD_GPIOE24,    GMAC0_GTX_CLK
#endif


	// Clock control
	NX_CLKGEN_Initialize();
	addr = NX_CLKGEN_GetPhysicalAddress(CLOCKINDEX_OF_DWC_GMAC_MODULE);
	NX_CLKGEN_SetBaseAddress( CLOCKINDEX_OF_DWC_GMAC_MODULE, (void*)IO_ADDRESS(addr) );

	NX_CLKGEN_SetClockSource( CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, 4);     // Sync mode for 100 & 10Base-T : External RX_clk
	NX_CLKGEN_SetClockDivisor( CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, 1);    // Sync mode for 100 & 10Base-T

	NX_CLKGEN_SetClockOutInv( CLOCKINDEX_OF_DWC_GMAC_MODULE, 0, CFALSE);    // TX Clk invert off : 100 & 10Base-T

	NX_CLKGEN_SetClockDivisorEnable( CLOCKINDEX_OF_DWC_GMAC_MODULE, CTRUE);

	// Reset control
	NX_RSTCON_Initialize();
	addr = NX_RSTCON_GetPhysicalAddress();
	NX_RSTCON_SetBaseAddress( (void*)IO_ADDRESS(addr) );
	NX_RSTCON_SetRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_NEGATE);
	udelay(100);
	NX_RSTCON_SetRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_ASSERT);
	udelay(100);
	NX_RSTCON_SetRST(RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i, RSTCON_NEGATE);
	udelay(100);


    gpio_request(CFG_ETHER_GMAC_PHY_RST_NUM,"Ethernet Rst pin");
	gpio_direction_output(CFG_ETHER_GMAC_PHY_RST_NUM, 1 );
	udelay( 100 );
	gpio_set_value(CFG_ETHER_GMAC_PHY_RST_NUM, 0 );
	udelay( 100 );
	gpio_set_value(CFG_ETHER_GMAC_PHY_RST_NUM, 1 );

    gpio_free(CFG_ETHER_GMAC_PHY_RST_NUM);

	//printk("NXP mac init ..................\n");
	return 0;
}

int gmac_phy_reset(void *priv)
{
	// Set GPIO nReset
	gpio_set_value(CFG_ETHER_GMAC_PHY_RST_NUM, 1 );
	udelay( 100 );
	gpio_set_value(CFG_ETHER_GMAC_PHY_RST_NUM, 0 );
	udelay( 100 );
	gpio_set_value(CFG_ETHER_GMAC_PHY_RST_NUM, 1 );
	msleep( 30 );

	//printk("NXP gmac_phy_reset ..................\n");
	return 0;
}

static struct stmmac_mdio_bus_data nxpmac0_mdio_bus = {
	.phy_reset = gmac_phy_reset,
	.phy_mask = 0,
	.probed_phy_irq = CFG_ETHER_GMAC_PHY_IRQ_NUM,
};

static struct plat_stmmacenet_data nxpmac_plat_data = {
#if defined (CONFIG_REALTEK_PHY_RTL8201)
	.phy_addr = 3,
    .clk_csr = 0xe,
    .speed = SPEED_100,
#else
    .phy_addr = 4,
    .clk_csr = 0x4, // PCLK 150~250 Mhz
    .speed = SPEED_1000,	// SPEED_1000
#endif
    .interface = PHY_INTERFACE_MODE_RGMII,
	.autoneg = AUTONEG_ENABLE, //AUTONEG_ENABLE or AUTONEG_DISABLE
	.duplex = DUPLEX_FULL,
	.pbl = 16,          /* burst 16 */
	.has_gmac = 1,      /* GMAC ethernet    */
	.enh_desc = 0,
	.mdio_bus_data = &nxpmac0_mdio_bus,
	.init = &nxpmac_init,
};

/* DWC GMAC Controller registration */

static struct resource nxpmac_resource[] = {
    [0] = DEFINE_RES_MEM(PHY_BASEADDR_GMAC, SZ_8K),
    [1] = DEFINE_RES_IRQ_NAMED(IRQ_PHY_GMAC, "macirq"),
};

static u64 nxpmac_dmamask = DMA_BIT_MASK(32);

struct platform_device nxp_gmac_dev = {
    .name           = "stmmaceth",  //"s5p6818-gmac",
    .id             = -1,
    .num_resources  = ARRAY_SIZE(nxpmac_resource),
    .resource       = nxpmac_resource,
    .dev            = {
        .dma_mask           = &nxpmac_dmamask,
        .coherent_dma_mask  = DMA_BIT_MASK(32),
        .platform_data      = &nxpmac_plat_data,
    }
};
#endif	

/*------------------------------------------------------------------------------
 * DISPLAY (LVDS) / FB
 */
#if defined (CONFIG_FB_NXP)
#if defined (CONFIG_FB0_NXP)
static struct nxp_fb_plat_data fb0_plat_data = {
	.module			= CONFIG_FB0_NXP_DISPOUT,
	.layer			= CFG_DISP_PRI_SCREEN_LAYER,
	.format			= CFG_DISP_PRI_SCREEN_RGB_FORMAT,
	.bgcolor		= CFG_DISP_PRI_BACK_GROUND_COLOR,
	.bitperpixel	= CFG_DISP_PRI_SCREEN_PIXEL_BYTE * 8,
	.x_resol		= CFG_DISP_PRI_RESOL_WIDTH,
	.y_resol		= CFG_DISP_PRI_RESOL_HEIGHT,
	#ifdef CONFIG_ANDROID
	.buffers		= 3,
	.skip_pan_vsync	= 0,
	#else
	.buffers		= 2,
	#endif
	.lcd_with_mm	= CFG_DISP_PRI_LCD_WIDTH_MM,	/* 152.4 */
	.lcd_height_mm	= CFG_DISP_PRI_LCD_HEIGHT_MM,	/* 91.44 */
};

static struct platform_device fb0_device = {
	.name	= DEV_NAME_FB,
	.id		= 0,	/* FB device node num */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb0_plat_data
	},
};
#endif

static struct platform_device *fb_devices[] = {
	#if defined (CONFIG_FB0_NXP)
	&fb0_device,
	#endif
};
#endif /* CONFIG_FB_NXP */

/*------------------------------------------------------------------------------
 * backlight : generic pwm device
 */
#if defined(CONFIG_BACKLIGHT_PWM)
#include <linux/pwm_backlight.h>

static struct platform_pwm_backlight_data bl_plat_data = {
	.pwm_id			= CFG_LCD_PRI_PWM_CH,
	.max_brightness = 255,	/* 255 is 100%, set over 100% */
	.dft_brightness = 255,	/* 50% */
	.pwm_period_ns	= 1000000000/CFG_LCD_PRI_PWM_FREQ,
};

static struct platform_device bl_plat_device = {
	.name	= "pwm-backlight",
	.id		= -1,
	.dev	= {
		.platform_data	= &bl_plat_data,
	},
};

#endif

/*------------------------------------------------------------------------------
 * NAND device
 */
#if defined(CONFIG_MTD_NAND_NXP)
#include <linux/mtd/partitions.h>
#include <asm-generic/sizes.h>

static struct mtd_partition nxp_nand_parts[] = {
#if 0
	{
		.name           = "root",
		.offset         =   0 * SZ_1M,
	},
#else
	{
		.name		= "system",
		.offset		=  64 * SZ_1M,
		.size		= 512 * SZ_1M,
	}, {
		.name		= "cache",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 256 * SZ_1M,
	}, {
		.name		= "userdata",
		.offset		= MTDPART_OFS_APPEND,
		.size		= MTDPART_SIZ_FULL,
	}
#endif
};

static struct nxp_nand_plat_data nand_plat_data = {
	.parts		= nxp_nand_parts,
	.nr_parts	= ARRAY_SIZE(nxp_nand_parts),
	.chip_delay = 10,
};

static struct platform_device nand_plat_device = {
	.name	= DEV_NAME_NAND,
	.id		= -1,
	.dev	= {
		.platform_data	= &nand_plat_data,
	},
};
#elif defined(CONFIG_NXP_FTL)
static struct resource nand_resource =
{
};

static struct platform_device nand_plat_device = {
	.name	= DEV_NAME_NAND,
	.id		= -1,
	.dev	= {
	},
};
#endif	/* CONFIG_NXP_FTL */

#if defined(CONFIG_TOUCHSCREEN_GSLX680)
#include <linux/i2c.h>
#define	GSLX680_I2C_BUS		(1)

static struct i2c_board_info __initdata gslX680_i2c_bdi = {
	.type	= "gslX680",
	.addr	= (0x40),
    	.irq    = PB_PIO_IRQ(CFG_IO_TOUCH_PENDOWN_DETECT),
};
#endif


/*------------------------------------------------------------------------------
 * Keypad platform device
 */
#if defined(CONFIG_KEYBOARD_NXP_KEY) || defined(CONFIG_KEYBOARD_NXP_KEY_MODULE)

#include <linux/input.h>

static unsigned int  button_gpio[] = CFG_KEYPAD_KEY_BUTTON;
static unsigned int  button_code[] = CFG_KEYPAD_KEY_CODE;

struct nxp_key_plat_data key_plat_data = {
	.bt_count	= ARRAY_SIZE(button_gpio),
	.bt_io		= button_gpio,
	.bt_code	= button_code,
	.bt_repeat	= CFG_KEYPAD_REPEAT,
};

static struct platform_device key_plat_device = {
	.name	= DEV_NAME_KEYPAD,
	.id		= -1,
	.dev    = {
		.platform_data	= &key_plat_data
	},
};
#endif	/* CONFIG_KEYBOARD_NXP_KEY || CONFIG_KEYBOARD_NXP_KEY_MODULE */

/*------------------------------------------------------------------------------
 * ASoC Codec platform device
 */
 //rpdzkj sound for mxliao ,at 2015-10-23
#if defined(CONFIG_SND_CODEC_WM8960) 
#include <linux/i2c.h>

#define	WM8976_I2C_BUS		(0)

/* CODEC */
static struct i2c_board_info __initdata wm8976_i2c_bdi = {
	.type	= "wm8960",			// compatilbe with wm8976
	.addr	= (0x34>>1),		// 0x1A (7BIT), 0x34(8BIT)
};

/* DAI */
struct nxp_snd_dai_plat_data i2s_dai_data = {
	.i2s_ch	= 0,
	.sample_rate	= 48000,
	.pcm_format = SNDRV_PCM_FMTBIT_S16_LE,
	.hp_jack 		= {
		.support    	= 1,
		.detect_level	= 1,
	},
};

static struct platform_device wm8976_dai = {
	.name			= "wm8976-audio",
	.id				= 0,
	.dev			= {
		.platform_data	= &i2s_dai_data,
	}
};
#endif

//rpdzkj TP for mxliao ,at 2015-10-23

#if defined(CONFIG_TOUCHSCREEN_FT5X0X)

#include <linux/i2c.h>
#define	TP_I2C_BUS		(1)
static struct i2c_board_info __initdata ft5x0x_i2c_bdi = {
.type	= "ft5x06_ts",
.addr	= (0x38),
.irq    = PB_PIO_IRQ(CFG_IO_TOUCH_PENDOWN_DETECT),
};
#endif

#if defined(CONFIG_TOUCHSCREEN_GOODIX)
#include <linux/i2c.h>
#define	TP_I2C_BUS		(1)
static struct i2c_board_info __initdata Goodix_i2c_bdi = {
	.type	= "Goodix-TS",
	.addr	= (0x5d),
	.irq    = PB_PIO_IRQ(CFG_IO_TOUCH_PENDOWN_DETECT),
};
//END
#endif

#if defined(CONFIG_SND_SPDIF_TRANSCIEVER) || defined(CONFIG_SND_SPDIF_TRANSCIEVER_MODULE)
static struct platform_device spdif_transciever = {
	.name	= "spdif-dit",
	.id		= -1,
};

struct nxp_snd_dai_plat_data spdif_trans_dai_data = {
	.sample_rate = 48000,
	.pcm_format	 = SNDRV_PCM_FMTBIT_S16_LE,
};

static struct platform_device spdif_trans_dai = {
	.name	= "spdif-transciever",
	.id		= -1,
	.dev	= {
		.platform_data	= &spdif_trans_dai_data,
	}
};
#endif

#if defined(CONFIG_SND_CODEC_ES8316) || defined(CONFIG_SND_CODEC_ES8316_MODULE)
#include <linux/i2c.h>

#define	ES8316_I2C_BUS		(0)

/* CODEC */
static struct i2c_board_info __initdata es8316_i2c_bdi = {
	.type	= "es8316",
	.addr	= (0x22>>1),		// 0x11 (7BIT), 0x22(8BIT)
};

/* DAI */
struct nxp_snd_dai_plat_data i2s_dai_data = {
	.i2s_ch	= 0,
	.sample_rate	= 48000,
	.pcm_format = SNDRV_PCM_FMTBIT_S16_LE,
#if 1
	.hp_jack 		= {
		.support    	= 1,
		.detect_io		= PAD_GPIO_B + 27,
		.detect_level	= 1,
	},
#endif
};

static struct platform_device es8316_dai = {
	.name			= "es8316-audio",
	.id				= 0,
	.dev			= {
		.platform_data	= &i2s_dai_data,
	}
};
#endif


/*------------------------------------------------------------------------------
 * G-Sensor platform device
 */
#if defined(CONFIG_SENSORS_MMA865X) || defined(CONFIG_SENSORS_MMA865X_MODULE)
#include <linux/i2c.h>

#define	MMA865X_I2C_BUS		(2)

/* CODEC */
static struct i2c_board_info __initdata mma865x_i2c_bdi = {
	.type	= "mma8653",
	.addr	= 0x1D//(0x3a),
};

#endif

#if defined(CONFIG_SENSORS_STK831X) || defined(CONFIG_SENSORS_STK831X_MODULE)
#include <linux/i2c.h>

#define	STK831X_I2C_BUS		(2)

/* CODEC */
static struct i2c_board_info __initdata stk831x_i2c_bdi = {
#if   defined CONFIG_SENSORS_STK8312
	.type	= "stk8312",
	.addr	= (0x3d),
#elif defined CONFIG_SENSORS_STK8313
	.type	= "stk8313",
	.addr	= (0x22),
#endif
};

#endif

//added by rpdzkj ivy for bma2x2 2015.11.12
#if defined(CONFIG_SENSORS_BMA2X2)
#define	BMA22X_I2C_BUS		(2)

/* CODEC */
static struct i2c_board_info __initdata bma22x_i2c_bdi = {
	.type	= "bma2x2",
	.addr	= 0x18,//(0x4c),
	.irq	= PB_PIO_IRQ(CFG_IO_BMA250_DETECT),
	
};
#endif
//end 
/*------------------------------------------------------------------------------
 *  * reserve mem
 *   */
#ifdef CONFIG_CMA
#include <linux/cma.h>
extern void nxp_cma_region_reserve(struct cma_region *, const char *);

void __init nxp_reserve_mem(void)
{
    static struct cma_region regions[] = {
        {
            .name = "ion",
#ifdef CONFIG_ION_NXP_CONTIGHEAP_SIZE
            .size = CONFIG_ION_NXP_CONTIGHEAP_SIZE * SZ_1K,
#else
			.size = 0,
#endif
            {
                .alignment = PAGE_SIZE,
            }
        },
        {
            .size = 0
        }
    };

    static const char map[] __initconst =
        "ion-nxp=ion;"
        "nx_vpu=ion;";

#ifdef CONFIG_ION_NXP_CONTIGHEAP_SIZE
    printk("%s: reserve CMA: size %d\n", __func__, CONFIG_ION_NXP_CONTIGHEAP_SIZE * SZ_1K);
#endif
    nxp_cma_region_reserve(regions, map);
}
#endif

#if defined(CONFIG_I2C_NXP) || defined (CONFIG_I2C_SLSI)
#define I2CUDELAY(x)	1000000/x
/* gpio i2c 3 */
#define	I2C3_SCL	PAD_GPIO_C + 15
#define	I2C3_SDA	PAD_GPIO_C + 16

static struct i2c_gpio_platform_data nxp_i2c_gpio_port3 = {
	.sda_pin	= I2C3_SDA,
	.scl_pin	= I2C3_SCL,
	.udelay		= I2CUDELAY(CFG_I2C3_CLK),				/* Gpio_mode CLK Rate = 1/( udelay*2) * 1000000 */

	.timeout	= 10,
};


static struct platform_device i2c_device_ch3 = {
	.name	= "i2c-gpio",
	.id		= 3,
	.dev    = {
		.platform_data	= &nxp_i2c_gpio_port3,
	},
};

static struct platform_device *i2c_devices[] = {
	&i2c_device_ch3,
};
#endif /* CONFIG_I2C_NXP || CONFIG_I2C_SLSI */

/*------------------------------------------------------------------------------
 * v4l2 platform device
 */
#if defined(CONFIG_V4L2_NXP) || defined(CONFIG_V4L2_NXP_MODULE)
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <mach/nxp-v4l2-platformdata.h>
#include <mach/soc.h>

static int is_camera_set_clock = 0;
static int camera_common_set_clock(ulong clk_rate)
{
    printk(KERN_ALERT "#####camera set clock");
    if(is_camera_set_clock)
		return 0;

    PM_DBGOUT("%s: %d\n", __func__, (int)clk_rate);
    if (clk_rate > 0)
        nxp_soc_pwm_set_frequency(1, clk_rate, 50);
    else
        nxp_soc_pwm_set_frequency(1, 0, 0);
    msleep(1);
	is_camera_set_clock = clk_rate > 0 ? 1 : 0;
	
    return 0;
}

static bool is_camera_port_configured = false;
static void camera_common_vin_setup_io(int module, bool force)
{
    if (!force && is_camera_port_configured)
        return;
    else {
        u_int *pad;
        int i, len;
        u_int io, fn;

        /* VIP0:0 = VCLK, VID0 ~ 7 */
        const u_int port[][2] = {
//changed 0 to 1 by rpdzkj ivy for camera 2015.11.6
#if 1
            /* VCLK, HSYNC, VSYNC */
            { PAD_GPIO_E +  4, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_E +  5, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_E +  6, NX_GPIO_PADFUNC_1 },
            /* DATA */
            { PAD_GPIO_D + 28, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_D + 29, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_D + 30, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_D + 31, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_E +  0, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_E +  1, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_E +  2, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_E +  3, NX_GPIO_PADFUNC_1 },

            { PAD_GPIO_A + 30, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_B +  0, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_B +  2, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_B +  4, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_B +  6, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_B +  8, NX_GPIO_PADFUNC_1 },
            { PAD_GPIO_B +  9, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_B + 10, NX_GPIO_PADFUNC_1 },
#endif

        };

        printk("## camera port config in device.c: %s\n", __func__);

        pad = (u_int *)port;
        len = sizeof(port)/sizeof(port[0]);

        for (i = 0; i < len; i++) {
            io = *pad++;
            fn = *pad++;
            nxp_soc_gpio_set_io_dir(io, 0);
            nxp_soc_gpio_set_io_func(io, fn);
        }

        is_camera_port_configured = true;
    }
}

static bool camera_power_enabled = false;
// fix for dronel
static void camera_power_control(int enable)
{
    printk(KERN_ALERT "## enter camera power_control func in device.c\n");
    camera_power_enabled = enable ? true : false;
}

static bool is_mipi_camera_enabled = false;
static bool is_mipi_camera_power_state_changed = false;
static bool is_dvp_camera_enabled = false;
static bool is_dvp_camera_power_state_changed = false;

//static int front_camera_power_enable(bool on);
static int mipi_camera_power_enable(bool on)
{
    unsigned int io = CFG_IO_CAMERA_BACK_POWER_DOWN;
    unsigned int reset_io = CFG_IO_CAMERA_RESET;
    printk(KERN_ALERT "######enter camera power_enable func");
    PM_DBGOUT("%s: is_mipi_camera_enabled %d, on %d\n", __func__, is_,mipi_camera_enabled, on);

    return 0;
}

static bool mipi_camera_power_state_changed(void)
{
    return is_mipi_camera_power_state_changed;
}

// modified by yang and hoping
static int dvp_camera_power_enable(bool on)
{
    printk(KERN_ALERT "## enter dvp power_enable func in device.c\n");
    PM_DBGOUT("%s: is_dvp_camera_enabled %d, on %d\n", __func__, is_dvp_camera_enabled, on);

    struct regulator *camera_power_1p5V = NULL;
    struct regulator *camera_power_2p8V = NULL;

    camera_power_2p8V = regulator_get(NULL,"vcam1_2.8V");
    camera_power_1p5V = regulator_get(NULL,"vcam1_1.5V");

    gpio_request(CAMERA_PD0,"CAMERA_PD0");
    gpio_request(CAMERA_RST,"CAMERA_RST");

    if (on) {
        if (!is_dvp_camera_enabled) {

            gpio_direction_output(CAMERA_PD0, 1);
            mdelay(5);

            regulator_set_voltage(camera_power_2p8V, 2800000, 2800000);
            regulator_set_voltage(camera_power_1p5V, 1500000, 1500000);
            // power up
            regulator_enable(camera_power_2p8V);
            mdelay(5);
            regulator_enable(camera_power_1p5V);
            // reset
            mdelay(10);
            gpio_direction_output(CAMERA_PD0, 0);
            // set CLK
            nxp_soc_pwm_set_frequency(1,24000000,50); //24MHz,  50% duty cycle
            mdelay(5);
            gpio_direction_output(CAMERA_RST, 1);
            mdelay(30);

            is_dvp_camera_enabled = true;
            is_dvp_camera_power_state_changed = true;
        } else {
            is_dvp_camera_power_state_changed = false;
        }
    } else {
        if (is_dvp_camera_enabled) {
            is_dvp_camera_enabled = false;
            is_dvp_camera_power_state_changed = true;
        } else {
            is_dvp_camera_power_state_changed = false;
        }
    }
	gpio_free(CAMERA_PD0);
    gpio_free(CAMERA_RST);
    regulator_put(camera_power_2p8V);
    regulator_put(camera_power_1p5V);

    return 0;
}

// modified by yang and hoping

static bool dvp_camera_power_state_changed(void)
{
    return is_dvp_camera_power_state_changed;
}

static struct i2c_board_info dvp_i2c_camera = {
        I2C_BOARD_INFO("OV5640_DVP", 0x78>>1),
};

// added by yang 2016.4
#define MODULE_NAME "OV5640_MIPI"
#include <linux/i2c.h>
static struct i2c_board_info mipi_i2c_camera = {

        I2C_BOARD_INFO(MODULE_NAME,0X78>>1),

};

static struct nxp_v4l2_i2c_board_info sensor[] = {
    //   modified by yang and hoping
    {
         .board_info = &mipi_i2c_camera,
         .i2c_adapter_id = 2,
    },
    {
        .board_info = &dvp_i2c_camera,
        .i2c_adapter_id = 0,
    },

};

static struct nxp_mipi_csi_platformdata csi = {
    .module = 0,
    .clk_rate = 56000000,
    .lanes = 2,
    .alignment = 0, // not set
    .hs_settle = 0, // not set
    .width = 1280,
    .height = 960,
    .fixed_phy_vdd = true,
    .irq = 0, // not set
    .base = NULL, //not set
    .phy_enable = NULL, // not set
};

static struct nxp_capture_platformdata capture_plat_data[] = {

    #if 1
    {
        // dvp_camera 656 interface 
        .module = 0,
        .sensor = &sensor[1],  //  sensor[0]:mipi  sensor[1] : dvp;
        .type = NXP_CAPTURE_INF_PARALLEL,
        .parallel = {
            .is_mipi        = false,
            .external_sync  = false,    //if external_sync is used(this means that value is true), 601 format else 656 format
            .h_active       = 640,  //1280
            .h_frontporch   = 0,
            .h_syncwidth    = 0,
            .h_backporch    = 2,
            .v_active       = 480,  //960
            .v_frontporch   = 0,
            .v_syncwidth    = 0,
            .v_backporch    = 13,
            .clock_invert   = false, 
            .port           = 0,
            .data_order     = NXP_VIN_Y0CBY1CR,//NXP_VIN_CRY1CBY0,//NXP_VIN_CBY0CRY1,
            .interlace      = false,
            .clk_rate       = 24000000,
            .late_power_down = true,
            .power_enable   = dvp_camera_power_enable,// 
            .power_state_changed = dvp_camera_power_state_changed,//
            .set_clock      = camera_common_set_clock,
            .setup_io       = camera_common_vin_setup_io,
        },
        .deci = {
            .start_delay_ms = 0,
            .stop_delay_ms  = 0,
        },
    },
    #endif

    #if 1
    {
        // dvp_camera 656 interface 
        .module = 1,
        .sensor = &sensor[0],  //  sensor[0]:mipi  sensor[1] : dvp;
        .type = NXP_CAPTURE_INF_PARALLEL,
        .parallel = {
            .is_mipi        = false,
            .external_sync  = false,    //if external_sync is used(this means that value is true), 601 format else 656 format
            .h_active       = 640,  //1280
            .h_frontporch   = 0,
            .h_syncwidth    = 0,
            .h_backporch    = 2,
            .v_active       = 480,  //960
            .v_frontporch   = 0,
            .v_syncwidth    = 0,
            .v_backporch    = 13,
            .clock_invert   = false, 
            .port           = 0,
            .data_order     = NXP_VIN_Y0CBY1CR,//NXP_VIN_CRY1CBY0,//NXP_VIN_CBY0CRY1,
            .interlace      = false,
            .clk_rate       = 24000000,
            .late_power_down = true,
            .power_enable   = dvp_camera_power_enable,// 
            .power_state_changed = dvp_camera_power_state_changed,//
            .set_clock      = camera_common_set_clock,
            .setup_io       = camera_common_vin_setup_io,
        },
        .deci = {
            .start_delay_ms = 0,
            .stop_delay_ms  = 0,
        },
    },
    #endif

    #if 0
    {

        /* mipi_camera 656 interface */
        .module = 1, 
        .sensor = &sensor[0], // sensor[0]:mipi; sensor[1]:dvp 
        .type = NXP_CAPTURE_INF_CSI,
        .parallel = {
            /* for 656 */
            .is_mipi        = true,
            .external_sync  = false,
            .h_active       = 640,
            .h_frontporch   = 1,
            .h_syncwidth    = 1,
            .h_backporch    = 0,
            .v_active       = 480,
            .v_frontporch   = 0,
            .v_syncwidth    = 1,
            .v_backporch    = 0,
            .clock_invert   = false,
            .port           = 0,
            .data_order     =  NXP_VIN_CBY0CRY1,
            .interlace      = false,
            .clk_rate       = 24000000,
            .late_power_down = true,
            .power_enable   = mipi_camera_power_enable,
            .power_state_changed =mipi_camera_power_state_changed, 
            .set_clock      = camera_common_set_clock,
            .setup_io       = camera_common_vin_setup_io,
        },
        .deci = {
            .start_delay_ms = 0,
            .stop_delay_ms  = 0,
        },
        .csi = &csi,
    },
    #endif

    { 0, NULL, 0, },

};



#if 1
//TK
/*------------------------------------------------------------------------------
 * DISPLAY MIPI device
 */
#if defined (CONFIG_NXP_DISPLAY_MIPI)
#include <linux/delay.h>

#define	MIPI_BITRATE_1G

#ifdef MIPI_BITRATE_1G
#define	PLLPMS		0x33E8
#define	BANDCTL		0xF
#elif defined(MIPI_BITRATE_900M)
#define	PLLPMS		0x2258
#define	BANDCTL		0xE
#elif defined(MIPI_BITRATE_840M)
#define	PLLPMS		0x2230
#define	BANDCTL		0xD
#elif defined(MIPI_BITRATE_750M)
#define	PLLPMS		0x43E8
#define	BANDCTL		0xC
#elif defined(MIPI_BITRATE_660M)
#define	PLLPMS		0x21B8
#define	BANDCTL		0xB
#elif defined(MIPI_BITRATE_600M)
#define	PLLPMS		0x2190
#define	BANDCTL		0xA
#elif defined(MIPI_BITRATE_540M)
#define	PLLPMS		0x2168
#define	BANDCTL		0x9
#elif defined(MIPI_BITRATE_512M)
#define	PLLPMS		0x3200
#define	BANDCTL		0x9
#elif defined(MIPI_BITRATE_480M)
#define	PLLPMS		0x2281
#define	BANDCTL		0x8
#elif defined(MIPI_BITRATE_420M)
#define	PLLPMS		0x2231
#define	BANDCTL		0x7
#elif defined(MIPI_BITRATE_402M)
#define	PLLPMS		0x2219
#define	BANDCTL		0x7
#elif defined(MIPI_BITRATE_210M)
#define	PLLPMS		0x2232
#define	BANDCTL		0x4
#endif

#define	PLLCTL		0
#define	DPHYCTL		0

#define MIPI_DELAY 0xFF
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct data_val{
	u8 data[48];
};

struct mipi_reg_val{
	u32 cmd;
	u32 addr;
	u32 cnt;
	struct data_val data;
};


static struct mipi_reg_val mipi_init_data[]= 
{


	//sleep out display on 
	{0x05, 0,  1,{0x11,}},
	{0xff, 120,	  0, {0,}},
	{0x05, 0,  1,{0x29,}},
	{0xff, 50,	  0, {0,}},
	{0,    0,	  0, {0,}},



};


static void  mipilcd_dcs_long_write(U32 cmd, U32 ByteCount, U8* pByteData )
{
	U32 DataCount32 = (ByteCount+3)/4;
	int i = 0;
	U32 index = 0;
	volatile NX_MIPI_RegisterSet* pmipi = (volatile NX_MIPI_RegisterSet*)IO_ADDRESS(NX_MIPI_GetPhysicalAddress(index));

	NX_ASSERT( 512 >= DataCount32 );

#if 1
	printk("0x%02x  : ", cmd);
	for(i=0; i< ByteCount; i++)
		printk("0x%02x,", pByteData[i]);
	printk("\n");
#endif

	for( i=0; i<DataCount32; i++ )
	{
		pmipi->DSIM_PAYLOAD = (pByteData[3]<<24)|(pByteData[2]<<16)|(pByteData[1]<<8)|pByteData[0];
		pByteData += 4;
	}
	pmipi->DSIM_PKTHDR  = (cmd & 0xff) | (ByteCount<<8);
}

static void mipilcd_dcs_write( unsigned int id, unsigned int data0, unsigned int data1 )
{
	U32 index = 0;
	volatile NX_MIPI_RegisterSet* pmipi = (volatile NX_MIPI_RegisterSet*)IO_ADDRESS(NX_MIPI_GetPhysicalAddress(index));

#if 0
	printk("0x%02x  : ", id);
	switch(id)
	{
		case 0x05:
			printk("0x%02x,\n", data0);
			break;

		case 0x15:
			printk("0x%02x,0x%02x,\n", data0, data1);
			break;
	}
#endif

	pmipi->DSIM_PKTHDR = id | (data0<<8) | (data1<<16);
}


static int MIPI_LCD_INIT(int width, int height, void *data)
{
	int i=0;
	U32 index = 0;
	U32 value = 0;
	u8 pByteData[48];
	int size=ARRAY_SIZE(mipi_init_data);
	//struct mipi_reg_val *lcd_init_data = &mipi_init_data;

	volatile NX_MIPI_RegisterSet* pmipi = (volatile NX_MIPI_RegisterSet*)IO_ADDRESS(NX_MIPI_GetPhysicalAddress(index));
	value = pmipi->DSIM_ESCMODE;
	pmipi->DSIM_ESCMODE = value|(3 << 6);
	value = pmipi->DSIM_ESCMODE;

	mdelay(10);

	for(i=0; i<size; i++)
	{

		switch(mipi_init_data[i].cmd)
		{
#if 1

#if 0 // all long packet
			case 0x05:
				//pByteData[0] = mipi_init_data[i].addr;
				//memcpy(&pByteData[1], &mipi_init_data[i].data.data[0], 7);
				mipilcd_dcs_long_write(0x39, mipi_init_data[i].cnt, &mipi_init_data[i].data.data[0]);
				break;
			case 0x15:
				pByteData[0] = mipi_init_data[i].addr;
				memcpy(&pByteData[1], &mipi_init_data[i].data.data[0], 7);
				mipilcd_dcs_long_write(0x39, mipi_init_data[i].cnt+1, &pByteData);
				break;
#else
			case 0x05:
				mipilcd_dcs_write(mipi_init_data[i].cmd, mipi_init_data[i].data.data[0], 0x00);
				break;
			case 0x15:
				mipilcd_dcs_write(mipi_init_data[i].cmd, mipi_init_data[i].addr, mipi_init_data[i].data.data[0]);
				break;
#endif
			case 0x29:
			case 0x39:
				pByteData[0] = mipi_init_data[i].addr;
				memcpy(&pByteData[1], &mipi_init_data[i].data.data[0], 48);
				mipilcd_dcs_long_write(mipi_init_data[i].cmd, mipi_init_data[i].cnt+1, &pByteData[0]);
				break;
			case MIPI_DELAY:
				printk("delay : %dms\n", mipi_init_data[i].addr);
				mdelay(mipi_init_data[i].addr);
				break;
		}
#endif
	}

	value = pmipi->DSIM_ESCMODE;
	pmipi->DSIM_ESCMODE = value&(~(3 << 6));
	value = pmipi->DSIM_ESCMODE;
	mdelay(10);
	return 0;
}

static struct disp_vsync_info mipi_vsync_param = {
	.h_active_len	= CFG_DISP_PRI_RESOL_WIDTH,
	.h_sync_width	= CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
	.h_back_porch	= CFG_DISP_PRI_HSYNC_BACK_PORCH,
	.h_front_porch	= CFG_DISP_PRI_HSYNC_FRONT_PORCH,
	.h_sync_invert	= CFG_DISP_PRI_HSYNC_ACTIVE_HIGH,
	.v_active_len	= CFG_DISP_PRI_RESOL_HEIGHT,
	.v_sync_width	= CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
	.v_back_porch	= CFG_DISP_PRI_VSYNC_BACK_PORCH,
	.v_front_porch	= CFG_DISP_PRI_VSYNC_FRONT_PORCH,
	.v_sync_invert	= CFG_DISP_PRI_VSYNC_ACTIVE_HIGH,
	.pixel_clock_hz	= CFG_DISP_PRI_PIXEL_CLOCK,
	.clk_src_lv0	= CFG_DISP_PRI_CLKGEN0_SOURCE,
	.clk_div_lv0	= CFG_DISP_PRI_CLKGEN0_DIV,
	.clk_src_lv1	= CFG_DISP_PRI_CLKGEN1_SOURCE,
	.clk_div_lv1	= CFG_DISP_PRI_CLKGEN1_DIV,
};

static struct disp_syncgen_par mipi_syncgen_param = {
#if 1
	.delay_mask			= DISP_SYNCGEN_DELAY_RGB_PVD |
						  DISP_SYNCGEN_DELAY_HSYNC_CP1 |
					 	  DISP_SYNCGEN_DELAY_VSYNC_FRAM |
					 	  DISP_SYNCGEN_DELAY_DE_CP,
	.d_rgb_pvd			= 0,
	.d_hsync_cp1		= 0,
	.d_vsync_fram		= 0,
	.d_de_cp2			= 7,
	.vs_start_offset 	= CFG_DISP_PRI_HSYNC_FRONT_PORCH +
						  CFG_DISP_PRI_HSYNC_SYNC_WIDTH +
						  CFG_DISP_PRI_HSYNC_BACK_PORCH +
						  CFG_DISP_PRI_RESOL_WIDTH - 1,
	.ev_start_offset 	= CFG_DISP_PRI_HSYNC_FRONT_PORCH +
						  CFG_DISP_PRI_HSYNC_SYNC_WIDTH +
						  CFG_DISP_PRI_HSYNC_BACK_PORCH +
						  CFG_DISP_PRI_RESOL_WIDTH - 1,
	.vs_end_offset 		= 0,
	.ev_end_offset 		= 0,
#else
	.interlace 		= CFG_DISP_PRI_MLC_INTERLACE,
	.out_format		= CFG_DISP_PRI_OUT_FORMAT,
	.invert_field 	= CFG_DISP_PRI_OUT_INVERT_FIELD,
	.swap_RB		= CFG_DISP_PRI_OUT_SWAPRB,
	.yc_order		= CFG_DISP_PRI_OUT_YCORDER,
	.delay_mask		= DISP_SYNCGEN_DELAY_RGB_PVD|DISP_SYNCGEN_DELAY_HSYNC_CP1|DISP_SYNCGEN_DELAY_VSYNC_FRAM|DISP_SYNCGEN_DELAY_DE_CP,
	.d_rgb_pvd		= 0,
	.d_hsync_cp1	= 0,
	.d_vsync_fram	= 0,
	.d_de_cp2		= 7,
	.vs_start_offset = (CFG_DISP_PRI_HSYNC_FRONT_PORCH + CFG_DISP_PRI_HSYNC_SYNC_WIDTH + CFG_DISP_PRI_HSYNC_BACK_PORCH + CFG_DISP_PRI_RESOL_WIDTH - 1),
	.vs_end_offset	= 0,
	.ev_start_offset = (CFG_DISP_PRI_HSYNC_FRONT_PORCH + CFG_DISP_PRI_HSYNC_SYNC_WIDTH + CFG_DISP_PRI_HSYNC_BACK_PORCH + CFG_DISP_PRI_RESOL_WIDTH - 1),
	.ev_end_offset	= 0,
	.vclk_select	= CFG_DISP_PRI_PADCLKSEL,
	.clk_delay_lv0	= CFG_DISP_PRI_CLKGEN0_DELAY,
	.clk_inv_lv0	= CFG_DISP_PRI_CLKGEN0_INVERT,
	.clk_delay_lv1	= CFG_DISP_PRI_CLKGEN1_DELAY,
	.clk_inv_lv1	= CFG_DISP_PRI_CLKGEN1_INVERT,
	.clk_sel_div1	= CFG_DISP_PRI_CLKSEL1_SELECT,
#endif
};

static struct disp_mipi_param mipi_param = {
	.pllpms 	= PLLPMS,
	.bandctl	= BANDCTL,
	.pllctl		= PLLCTL,
	.phyctl		= DPHYCTL,
	.lcd_init	= MIPI_LCD_INIT
};
#endif
//END
#endif

/* out platformdata */
static struct i2c_board_info hdmi_edid_i2c_boardinfo = {
    I2C_BOARD_INFO("nxp_edid", 0xA0>>1),
};

static struct nxp_v4l2_i2c_board_info edid = {
    .board_info = &hdmi_edid_i2c_boardinfo,
    .i2c_adapter_id = 0,
};

static struct i2c_board_info hdmi_hdcp_i2c_boardinfo = {
    I2C_BOARD_INFO("nxp_hdcp", 0x74>>1),
};

static struct nxp_v4l2_i2c_board_info hdcp = {
    .board_info = &hdmi_hdcp_i2c_boardinfo,
    .i2c_adapter_id = 0,
};


static void hdmi_set_int_external(int gpio)
{
    nxp_soc_gpio_set_int_enable(gpio, 0);
    nxp_soc_gpio_set_int_mode(gpio, 1); /* high level */
    nxp_soc_gpio_set_int_enable(gpio, 1);
    nxp_soc_gpio_clr_int_pend(gpio);
}

static void hdmi_set_int_internal(int gpio)
{
    nxp_soc_gpio_set_int_enable(gpio, 0);
    nxp_soc_gpio_set_int_mode(gpio, 0); /* low level */
    nxp_soc_gpio_set_int_enable(gpio, 1);
    nxp_soc_gpio_clr_int_pend(gpio);
}

static int hdmi_read_hpd_gpio(int gpio)
{
    return nxp_soc_gpio_get_in_value(gpio);
}

static struct nxp_out_platformdata out_plat_data = {
    .hdmi = {
        .internal_irq = 0,
        .external_irq = 0,//PAD_GPIO_A + 19,
        .set_int_external = hdmi_set_int_external,
        .set_int_internal = hdmi_set_int_internal,
        .read_hpd_gpio = hdmi_read_hpd_gpio,
        .edid = &edid,
        .hdcp = &hdcp,
    },
};

static struct nxp_v4l2_platformdata v4l2_plat_data = {
    .captures = &capture_plat_data[0],
    .out = &out_plat_data,
};

static struct platform_device nxp_v4l2_dev = {
    .name       = NXP_V4L2_DEV_NAME,
    .id         = 0,
    .dev        = {
        .platform_data = &v4l2_plat_data,
    },
};
#endif /* CONFIG_V4L2_NXP || CONFIG_V4L2_NXP_MODULE */

/*------------------------------------------------------------------------------
 * SSP/SPI
 */
#if defined(CONFIG_SPI_SPIDEV) || defined(CONFIG_SPI_SPIDEV_MODULE)
#include <linux/spi/spi.h>
static void spi0_cs(u32 chipselect)
{
#if (CFG_SPI0_CS_GPIO_MODE)
	if(nxp_soc_gpio_get_io_func( CFG_SPI0_CS )!= nxp_soc_gpio_get_altnum( CFG_SPI0_CS))
		nxp_soc_gpio_set_io_func( CFG_SPI0_CS, nxp_soc_gpio_get_altnum( CFG_SPI0_CS));

	nxp_soc_gpio_set_io_dir( CFG_SPI0_CS,1);
	nxp_soc_gpio_set_out_value(	 CFG_SPI0_CS , chipselect);
#else
	;
#endif
}
struct pl022_config_chip spi0_info = {
    /* available POLLING_TRANSFER, INTERRUPT_TRANSFER, DMA_TRANSFER */
    .com_mode = CFG_SPI0_COM_MODE,
    .iface = SSP_INTERFACE_MOTOROLA_SPI,
    /* We can only act as master but SSP_SLAVE is possible in theory */
    .hierarchy = SSP_MASTER,
    /* 0 = drive TX even as slave, 1 = do not drive TX as slave */
    .slave_tx_disable = 1,
    .rx_lev_trig = SSP_RX_4_OR_MORE_ELEM,
    .tx_lev_trig = SSP_TX_4_OR_MORE_EMPTY_LOC,
    .ctrl_len = SSP_BITS_8,
    .wait_state = SSP_MWIRE_WAIT_ZERO,
    .duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX,
    /*
     * This is where you insert a call to a function to enable CS
     * (usually GPIO) for a certain chip.
     */
#if (CFG_SPI0_CS_GPIO_MODE)
    .cs_control = spi0_cs,
#endif
	.clkdelay = SSP_FEEDBACK_CLK_DELAY_1T,

};

static struct spi_board_info spi_plat_board[] __initdata = {
    [0] = {
        .modalias        = "spidev",    /* fixup */
        .max_speed_hz    = 3125000,     /* max spi clock (SCK) speed in HZ */
        .bus_num         = 0,           /* Note> set bus num, must be smaller than ARRAY_SIZE(spi_plat_device) */
        .chip_select     = 0,           /* Note> set chip select num, must be smaller than spi cs_num */
        .controller_data = &spi0_info,
        .mode            = SPI_MODE_3 | SPI_CPOL | SPI_CPHA,
    },
};

#endif
/*------------------------------------------------------------------------------
 * DW MMC board config
 */
#if defined(CONFIG_MMC_DW)
static int _dwmci_ext_cd_init(void (*notify_func)(struct platform_device *, int state))
{
	return 0;
}

static int _dwmci_ext_cd_cleanup(void (*notify_func)(struct platform_device *, int state))
{
	return 0;
}

static int _dwmci_get_ro(u32 slot_id)
{
	return 0;
}


static int _dwmci0_init(u32 slot_id, irq_handler_t handler, void *data)
{
	struct dw_mci *host = (struct dw_mci *)data;
	int io  = CFG_SDMMC0_DETECT_IO;
	int irq = IRQ_GPIO_START + io;
	int id  = 0, ret = 0;

	printk("dw_mmc dw_mmc.%d: Using external card detect irq %3d (io %2d)\n", id, irq, io);

	ret  = request_irq(irq, handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				DEV_NAME_SDHC "0", (void*)host->slot[slot_id]);
	if (0 > ret)
		pr_err("dw_mmc dw_mmc.%d: fail request interrupt %d ...\n", id, irq);
	return 0;
}
static int _dwmci0_get_cd(u32 slot_id)
{
	int io = CFG_SDMMC0_DETECT_IO;
	return nxp_soc_gpio_get_in_value(io);
}

static int _dwmci1_init(u32 slot_id, irq_handler_t handler, void *data)
{
	struct dw_mci *host = (struct dw_mci *)data;
	int io  = CFG_SDMMC0_DETECT_IO;
	int irq = IRQ_GPIO_START + io;
	int id  = 1, ret = 0;

	printk("dw_mmc dw_mmc.%d: Using external card detect irq %3d (io %2d)\n", id, irq, io);

	ret  = request_irq(irq, handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				DEV_NAME_SDHC "1", (void*)host->slot[slot_id]);
	if (0 > ret)
		pr_err("dw_mmc dw_mmc.%d: fail request interrupt %d ...\n", id, irq);
	return 0;
}
static int _dwmci1_get_cd(u32 slot_id)
{
	int io = CFG_SDMMC0_DETECT_IO;
	return nxp_soc_gpio_get_in_value(io);
}

#ifdef CONFIG_MMC_NXP_CH1
static struct dw_mci_board _dwmci1_data = {
	.quirks			= DW_MCI_QUIRK_HIGHSPEED,
	.bus_hz			= 100 * 1000 * 1000,
	.caps			= MMC_CAP_CMD23,
	.detect_delay_ms= 200,
	.cd_type		= DW_MCI_CD_EXTERNAL,
	.clk_dly        = DW_MMC_DRIVE_DELAY(0) | DW_MMC_SAMPLE_DELAY(0) | DW_MMC_DRIVE_PHASE(2) | DW_MMC_SAMPLE_PHASE(1),
	.init			= _dwmci1_init,
	.get_ro         = _dwmci_get_ro,
	.get_cd			= _dwmci1_get_cd,
	.ext_cd_init	= _dwmci_ext_cd_init,
	.ext_cd_cleanup	= _dwmci_ext_cd_cleanup,
#if defined (CONFIG_MMC_DW_IDMAC) && defined (CONFIG_MMC_NXP_CH0_USE_DMA)
	.mode			= DMA_MODE,
#else
	.mode 			= PIO_MODE,
#endif
};
#endif

#ifdef CONFIG_MMC_NXP_CH0
static struct dw_mci_board _dwmci0_data = {
	.quirks			= DW_MCI_QUIRK_HIGHSPEED,
	.bus_hz			= 100 * 1000 * 1000,
	.caps = MMC_CAP_CMD23|MMC_CAP_NONREMOVABLE,
	.detect_delay_ms= 200,
	.cd_type 		= DW_MCI_CD_NONE,
	.pm_caps        = MMC_PM_KEEP_POWER | MMC_PM_IGNORE_PM_NOTIFY,
	.clk_dly        = DW_MMC_DRIVE_DELAY(0) | DW_MMC_SAMPLE_DELAY(0) | DW_MMC_DRIVE_PHASE(0) | DW_MMC_SAMPLE_PHASE(0),
#if defined (CONFIG_MMC_DW_IDMAC) && defined (CONFIG_MMC_NXP_CH1_USE_DMA)
	.mode			= DMA_MODE,
#else
	.mode 			= PIO_MODE,
#endif
};
#endif

#ifdef CONFIG_MMC_NXP_CH2
/*
static struct dw_mci_board _dwmci2_data = {
    .quirks			= DW_MCI_QUIRK_BROKEN_CARD_DETECTION |
				  	  DW_MCI_QUIRK_HIGHSPEED |
				  	  DW_MMC_QUIRK_HW_RESET_PW |
				      DW_MCI_QUIRK_NO_DETECT_EBIT,
	.bus_hz			= 100*1000 * 1000,
	.caps			= MMC_CAP_UHS_DDR50 | MMC_CAP_1_8V_DDR |
					  MMC_CAP_NONREMOVABLE |
			 	  	  MMC_CAP_4_BIT_DATA | MMC_CAP_CMD23|
				  	  MMC_CAP_ERASE | MMC_CAP_HW_RESET,
	.clk_dly        = DW_MMC_DRIVE_DELAY(0x0) | DW_MMC_SAMPLE_DELAY(0) | DW_MMC_DRIVE_PHASE(0) | DW_MMC_SAMPLE_PHASE(0),
//	.clk_dly        = DW_MMC_DRIVE_DELAY(0) | DW_MMC_SAMPLE_DELAY(0x1c) | DW_MMC_DRIVE_PHASE(2) | DW_MMC_SAMPLE_PHASE(1),

	.desc_sz		= 4,
	.detect_delay_ms= 200,
	.sdr_timing		= 0x01010001,
	.ddr_timing		= 0x03030002,
#if defined (CONFIG_MMC_DW_IDMAC) && defined (CONFIG_MMC_NXP_CH2_USE_DMA)
	.mode			= DMA_MODE,
#else
	.mode 			= PIO_MODE,
#endif
};
*/
static struct dw_mci_board _dwmci2_data = {
    .quirks			= DW_MCI_QUIRK_BROKEN_CARD_DETECTION |
				  	  DW_MCI_QUIRK_HIGHSPEED |
				  	  DW_MMC_QUIRK_HW_RESET_PW |
				      DW_MCI_QUIRK_NO_DETECT_EBIT,
	.bus_hz			= 100 * 1000 * 1000,
	.caps			= MMC_CAP_UHS_DDR50 |
					  MMC_CAP_NONREMOVABLE |
			 	  	  MMC_CAP_4_BIT_DATA | MMC_CAP_CMD23 |
				  	  MMC_CAP_ERASE | MMC_CAP_HW_RESET,
	.clk_dly        = DW_MMC_DRIVE_DELAY(0) | DW_MMC_SAMPLE_DELAY(0x1c) | DW_MMC_DRIVE_PHASE(2) | DW_MMC_SAMPLE_PHASE(1),

	.desc_sz		= 4,
	.detect_delay_ms= 500,
	.sdr_timing		= 0x01010001,
	.ddr_timing		= 0x03030002,
#if defined (CONFIG_MMC_DW_IDMAC) && defined (CONFIG_MMC_NXP_CH2_USE_DMA)
	.mode       	= DMA_MODE,
#else
	.mode       	= PIO_MODE,
#endif

};
#endif

#endif /* CONFIG_MMC_DW */

/*------------------------------------------------------------------------------
 * RFKILL driver
 */
//#if defined(CONFIG_NXP_RFKILL) changed to RFKILL_NXP by rpdzkj ivy 2015.11.10
#if defined(CONFIG_RFKILL_NXP)


struct rfkill_dev_data  rfkill_dev_data =
{
	.supply_name 	= "vgps_3.3V",	// vwifi_3.3V, vgps_3.3V
	.module_name 	= "wlan",
	.initval		= RFKILL_INIT_SET | RFKILL_INIT_OFF,
    .delay_time_off	= 1000,
};

struct nxp_rfkill_plat_data rfkill_plat_data = {
	.name		= "WiFi-Rfkill",
	.type		= RFKILL_TYPE_WLAN,
	.rf_dev		= &rfkill_dev_data,
    .rf_dev_num	= 1,
};

static struct platform_device rfkill_device = {
	.name			= DEV_NAME_RFKILL,
	.dev			= {
		.platform_data	= &rfkill_plat_data,
	}
};
#endif	/* CONFIG_RFKILL_NXP */

/*------------------------------------------------------------------------------
 * USB HSIC power control.
 */
int nxp_hsic_phy_pwr_on(struct platform_device *pdev, bool on)
{
	return 0;
}
EXPORT_SYMBOL(nxp_hsic_phy_pwr_on);

/*------------------------------------------------------------------------------
 * HDMI CEC driver
 */
#if defined(CONFIG_NXP_HDMI_CEC)
static struct platform_device hdmi_cec_device = {
	.name			= NXP_HDMI_CEC_DRV_NAME,
};
#endif /* CONFIG_NXP_HDMI_CEC */

/*------------------------------------------------------------------------------
 * SLsiAP Thermal Unit
 */
#if defined(CONFIG_SENSORS_NXP_TMU)

struct nxp_tmu_trigger tmu_triggers[] = {
	{
		.trig_degree	=  85,	// 160
		.trig_duration	=  100,
		.trig_cpufreq	=  800*1000,	/* Khz */
	},
};

static struct nxp_tmu_platdata tmu_data = {
	.channel  = 0,
	.triggers = tmu_triggers,
	.trigger_size = ARRAY_SIZE(tmu_triggers),
	.poll_duration = 100,
};

static struct platform_device tmu_device = {
	.name			= "nxp-tmu",
	.dev			= {
		.platform_data	= &tmu_data,
	}
};
#endif

/*------------------------------------------------------------------------------
 * register board platform devices
 */
void __init nxp_board_devs_register(void)
{
	printk("[Register board platform devices]\n");

#if defined(CONFIG_ARM_NXP_CPUFREQ)
	printk("plat: add dynamic frequency (pll.%d)\n", dfs_plat_data.pll_dev);
	platform_device_register(&dfs_plat_device);
#endif

#if defined(CONFIG_MMC_DW)
	#ifdef CONFIG_MMC_NXP_CH0
	nxp_mmc_add_device(0, &_dwmci0_data);
	#endif
    #ifdef CONFIG_MMC_NXP_CH2
	nxp_mmc_add_device(2, &_dwmci2_data);
	#endif
    #ifdef CONFIG_MMC_NXP_CH1
	nxp_mmc_add_device(1, &_dwmci1_data);
	#endif
#endif

//rpdzkj eth for mxliao ,at 2015-10-23
#if defined(CONFIG_NXPMAC_ETH) 
    printk("plat: add device nxp-gmac\n"); 
    platform_device_register(&nxp_gmac_dev); 
#endif
//end

#if defined(CONFIG_SENSORS_NXP_TMU)
	printk("plat: add device TMU\n");
	platform_device_register(&tmu_device);
#endif

#if defined (CONFIG_FB_NXP)
	printk("plat: add framebuffer\n");
	platform_add_devices(fb_devices, ARRAY_SIZE(fb_devices));
#endif
//TK MIPI	
//END

#if defined(CONFIG_MTD_NAND_NXP) || defined(CONFIG_NXP_FTL)
	platform_device_register(&nand_plat_device);
#endif


#if defined(CONFIG_I2C_NXP) || defined (CONFIG_I2C_SLSI)
    platform_add_devices(i2c_devices, ARRAY_SIZE(i2c_devices));
#endif


#if defined(CONFIG_V4L2_NXP) || defined(CONFIG_V4L2_NXP_MODULE)
    printk(KERN_ALERT "#####: plat: add device nxp-v4l2\n");
    platform_device_register(&nxp_v4l2_dev);
#endif

#if defined(CONFIG_SPI_SPIDEV) || defined(CONFIG_SPI_SPIDEV_MODULE)
    spi_register_board_info(spi_plat_board, ARRAY_SIZE(spi_plat_board));
    printk("plat: register spidev\n");
#endif


	/* END */
	printk("\n");
}
