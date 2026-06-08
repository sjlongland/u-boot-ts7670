/*
 * Technologic TS-7670 rev D Single-board Computer
 *
 * (C) Copyright 2015 Technologic Systems
 * Based on work by:
 * Stuart Longland of VRT Systems <stuartl@vrt.com.au>
 *
 * Based on mx28evk.c:
 * Freescale MX28EVK board
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux-mx28.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <asm/spl.h>
#include <config.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <linux/types.h>
#include <miiphy.h>
#include <netdev.h>
#include <env.h>
#include <errno.h>
#include <spi.h>
#include <fpga.h>
#include <lattice.h>
#include <watchdog.h>
#include <i2c.h>
#include <fuse.h>

#define TS7670D_V2_EN_SDPWR     MX28_PAD_PWM3__GPIO_3_28
#define TS7670D_V2_POWER_FAIL	MX28_PAD_SSP0_DETECT__GPIO_2_9

DECLARE_GLOBAL_DATA_PTR;
int random_mac = 0;

void mx28_adjust_mac(int dev_id, unsigned char *mac)
{
	mac[0] = 0x00;
	mac[1] = 0xD0;
	mac[2] = 0x69;
}

int board_early_init_f(void)
{
	/* IO0 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK0, 480000);
	/* IO1 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK1, 480000);

	/* SSP clocks at 52MHz */
	mxs_set_sspclk(MXC_SSPCLK0, 52000, 0);
	mxs_set_sspclk(MXC_SSPCLK1, 52000, 0);
	mxs_set_sspclk(MXC_SSPCLK2, 52000, 0);

	/* Power-cycle eMMC */
	mxs_iomux_setup_pad(TS7670D_V2_EN_SDPWR);
	gpio_direction_output(TS7670D_V2_EN_SDPWR, 1); // EN_SD_POWER#
	udelay(1000);
	gpio_direction_output(TS7670D_V2_EN_SDPWR, 0);

	/* Wait a little bit for the card to wake up fully */
	udelay(1000000);

#if defined(CONFIG_CMD_NET)
	/*
	 * Initialise the Ethernet clock.  This function used to be
	 * called in board_eth_init, but that function is now removed and is no
	 * longer called.  TODO: find out the fate of cpu_eth_init.
	 */
	int ret = cpu_eth_init(NULL);
	if (ret)
		return ret;
#endif

	return 0;
}

int dram_init(void)
{
	return mxs_dram_init();
}

int misc_init_r(void)
{
	env_set("model", "7670D");

	return 0;
}

void board_boot_order(u32 *spl_boot_list)
{
#ifdef CONFIG_ENV_IS_IN_EXT4
	const char env_device[] = {CONFIG_ENV_EXT4_DEVICE_AND_PART};
	switch (env_device[0]) {
	case '0':
		spl_boot_list[0] = BOOT_DEVICE_MMC1;
		break;
	case '1':
		spl_boot_list[0] = BOOT_DEVICE_MMC2;
		break;
	}
#elif CONFIG_ENV_IS_IN_MMC
	switch (CONFIG_ENV_MMC_DEVICE_INDEX) {
	case 0:
		spl_boot_list[0] = BOOT_DEVICE_MMC1;
		break;
	case 1:
		spl_boot_list[0] = BOOT_DEVICE_MMC2;
		break;
	}
#endif
}

int board_init(void)
{
	/* Adress of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	/* Watchdog initialisation */
#if defined(CONFIG_HW_WATCHDOG)
	hw_watchdog_init();
#endif

	return 0;
}
