/*
 * wm8960.h  --  WM8960 Soc Audio driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _WM8960_H
#define _WM8960_H

/* WM8960 register space */
 
#define WM8960_CACHEREGNUM 	56

#if 1
/*
 * wm8960 register cache
 * We can't read the WM8960 register space when we are
 * using 2 wire for device control, so we cache them instead.
 */
 u16 wm8960_reg[WM8960_CACHEREGNUM] = {
#if 1
	0x010f, 0x010f, 0x017f, 0x017f,//0x00-0x03
	0x0000, 0x0008, 0x0040, 0x0002,  //0x04--0x07
	0x01c4, 0x0040, 0x01ff, 0x01ff,  //0x08--0x0b
	0x0000, 0x0000, 0x0000, 0x0000,  //0x0c--0x0f
	0x007f, 0x007b, 0x0100, 0x0032,  //0x10--0x13
	0x0000, 0x01ff, 0x01ff, 0x01c0,   //0x14--0x17
	0x0000, 0x017e, 0x01e1, 0x0000,  //0x18--0x1b
	0x0000, 0x0000, 0x0000, 0x0000,  //0x1c--0x1f
	0x0138, 0x0100, 0x0150, 0x0050,  //0x20--0x23
	0x0050, 0x0150, 0x0000, 0x0000,  //0x24--0x27
	0x017f, 0x017f, 0x0040, 0x0000,  //0x28---0x2b
	0x0000, 0x0050, 0x0050, 0x003c,  //0x2c---0x2f
	0x0043, 0x0037, 0x004d, 0x0080,  //0x30---0x33
	0x0037, 0x0021, 0x0161, 0x0026,   //0x34--0x37
#else

	0x0108,0x0108,0x017f,0x017f,
	0x0000,0x0008,0x0000,0x0002,
	0x01c0,0x0040,0x01ff,0x01ff,
	0x0000,0x0000,0x0000,0x0000,
	0x007f,0x007b,0x0100,0x0032,
	0x0000,0x01ff,0x01ff,0x01c0,
	0x0000,0x017e,0x01e1,0x0000, //. . 0x0001  //. 0x17e
	0x0008,0x0000,0x0000,0x0000,
	0x0138,0x0100,0x0150,0x0050,
	0x0050,0x0150,0x0000,0x0120,
	0x017f,0x017f,0x0040,0x0000,
	0x0000,0x0050,0x0050,0x003c,
	0x0003,0x00f7,0x004d,0x0080,
	0x0037,0x0021,0x0161,0x0026,
	
#endif

};
#endif


#define WM8960_LINVOL		0x0
#define WM8960_RINVOL		0x1
#define WM8960_LOUT1		0x2
#define WM8960_ROUT1		0x3
#define WM8960_CLOCK1		0x4
#define WM8960_DACCTL1		0x5
#define WM8960_DACCTL2		0x6
#define WM8960_IFACE1		0x7
#define WM8960_CLOCK2		0x8
#define WM8960_IFACE2		0x9
#define WM8960_LDAC		0xa
#define WM8960_RDAC		0xb

#define WM8960_RESET		0xf
#define WM8960_3D		0x10
#define WM8960_ALC1		0x11
#define WM8960_ALC2		0x12
#define WM8960_ALC3		0x13
#define WM8960_NOISEG		0x14
#define WM8960_LADC		0x15
#define WM8960_RADC		0x16
#define WM8960_ADDCTL1		0x17
#define WM8960_ADDCTL2		0x18
#define WM8960_POWER1		0x19
#define WM8960_POWER2		0x1a
#define WM8960_ADDCTL3		0x1b
#define WM8960_APOP1		0x1c
#define WM8960_APOP2		0x1d

#define WM8960_LINPATH		0x20
#define WM8960_RINPATH		0x21
#define WM8960_LOUTMIX		0x22

#define WM8960_ROUTMIX		0x25
#define WM8960_MONOMIX1		0x26
#define WM8960_MONOMIX2		0x27
#define WM8960_LOUT2		0x28
#define WM8960_ROUT2		0x29
#define WM8960_MONO		0x2a
#define WM8960_INBMIX1		0x2b
#define WM8960_INBMIX2		0x2c
#define WM8960_BYPASS1		0x2d
#define WM8960_BYPASS2		0x2e
#define WM8960_POWER3		0x2f
#define WM8960_ADDCTL4		0x30
#define WM8960_CLASSD1		0x31

#define WM8960_CLASSD3		0x33
#define WM8960_PLL1		0x34
#define WM8960_PLL2		0x35
#define WM8960_PLL3		0x36
#define WM8960_PLL4		0x37


/*
 * WM8960 Clock dividers
 */
#define WM8960_SYSCLKDIV 		0
#define WM8960_DACDIV			1
#define WM8960_OPCLKDIV			2
#define WM8960_DCLKDIV			3
#define WM8960_TOCLKSEL			4
#define WM8960_ADCDIV			  5
#define WM8960_BCLKDIV      6

#define WM8960_SYSCLK_DIV_1		(0 << 1)
#define WM8960_SYSCLK_DIV_2		(2 << 1)

#define WM8960_SYSCLK_MCLK		(0 << 0)
#define WM8960_SYSCLK_PLL		(1 << 0)

#define WM8960_DAC_DIV_1		(0 << 3)
#define WM8960_DAC_DIV_1_5		(1 << 3)
#define WM8960_DAC_DIV_2		(2 << 3)
#define WM8960_DAC_DIV_3		(3 << 3)
#define WM8960_DAC_DIV_4		(4 << 3)
#define WM8960_DAC_DIV_5_5		(5 << 3)
#define WM8960_DAC_DIV_6		(6 << 3)

#define WM8960_DCLK_DIV_1_5		(0 << 6)
#define WM8960_DCLK_DIV_2		(1 << 6)
#define WM8960_DCLK_DIV_3		(2 << 6)
#define WM8960_DCLK_DIV_4		(3 << 6)
#define WM8960_DCLK_DIV_6		(4 << 6)
#define WM8960_DCLK_DIV_8		(5 << 6)
#define WM8960_DCLK_DIV_12		(6 << 6)
#define WM8960_DCLK_DIV_16		(7 << 6)

#define WM8960_TOCLK_F19		(0 << 1)
#define WM8960_TOCLK_F21		(1 << 1)

#define WM8960_OPCLK_DIV_1		(0 << 0)
#define WM8960_OPCLK_DIV_2		(1 << 0)
#define WM8960_OPCLK_DIV_3		(2 << 0)
#define WM8960_OPCLK_DIV_4		(3 << 0)
#define WM8960_OPCLK_DIV_5_5		(4 << 0)
#define WM8960_OPCLK_DIV_6		(5 << 0)

#endif
