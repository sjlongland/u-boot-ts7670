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
#define TS7670D_V2_SDBOOT_JP    MX28_PAD_LCD_D12__GPIO_1_12
#define TS7670D_V2_POWER_FAIL	MX28_PAD_SSP0_DETECT__GPIO_2_9

DECLARE_GLOBAL_DATA_PTR;
int random_mac = 0;
static int ts7670d_get_sdboot_jp(void);

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

	/* SSP clocks at 96MHz */
	mxs_set_sspclk(MXC_SSPCLK0, 96000, 0);
	/* SSP clocks at 96MHz */
	mxs_set_sspclk(MXC_SSPCLK1, 96000, 0);
	/* SSP2 clock at 160MHz */
	mxs_set_sspclk(MXC_SSPCLK2, 160000, 0);

	/* Power-cycle eMMC */
	mxs_iomux_setup_pad(TS7670D_V2_EN_SDPWR);
	gpio_direction_output(TS7670D_V2_EN_SDPWR, 1); // EN_SD_POWER#
	udelay(1000);
	gpio_direction_output(TS7670D_V2_EN_SDPWR, 0);

	/* Wait a little bit for the card to wake up fully */
	udelay(1000000);

	return 0;
}

int dram_init(void)
{
	return mxs_dram_init();
}

static int ts7670d_get_sdboot_jp(void)
{
	gpio_direction_input(TS7670D_V2_SDBOOT_JP);
	return gpio_get_value(TS7670D_V2_SDBOOT_JP);
}

int misc_init_r(void)
{
	int sdboot = 0;

	env_set("model", "7670D");

	sdboot = ts7670d_get_sdboot_jp();
	if(sdboot) env_set("jpsdboot", "off");
	else env_set("jpsdboot", "on");

	/* Work-around non-working Ethernet */
	printf("Reading Ethernet address from fuses\n");
	uint8_t enetaddr[6];
	u32 fuse_data;
	int res = fuse_read(0, 0, &fuse_data);
	if (res < 0) {
		printf("Failed to read fuse 0/0: %d\n", res);
		return res;
	}

	/* Set the OUI */
	mx28_adjust_mac(0, enetaddr);

	/* Set device MAC from fuses */
	enetaddr[3] = (fuse_data & 0x00ff0000) >> 16;
	enetaddr[4] = (fuse_data & 0x0000ff00) >> 8;
	enetaddr[5] = (fuse_data & 0x000000ff);
	if (eth_env_set_enetaddr("ethaddr", enetaddr)) {
		printf("Failed to set ethernet address\n");
	}

	struct mxs_clkctrl_regs *clkctrl_regs =
		(struct mxs_clkctrl_regs *)MXS_CLKCTRL_BASE;
	struct udevice *dev;

	/* Try to set RMII clock */
	writel(CLKCTRL_ENET_TIME_SEL_RMII_CLK | CLKCTRL_ENET_CLK_OUT_EN,
	       &clkctrl_regs->hw_clkctrl_enet);
	return 0;
}

void board_boot_order(u32 *spl_boot_list)
{
	if (ts7670d_get_sdboot_jp() > 0) {
		spl_boot_list[0] = BOOT_DEVICE_MMC1;
	} else {
		spl_boot_list[0] = BOOT_DEVICE_MMC2;
	}
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

static int ts7670D_mmc_cd(int id) {
	return 1;
}

int board_mmc_init(struct bd_info *bis)
{
	int ret;

	/* SD card */
	ret = mxsmmc_initialize(bis, 0, NULL, ts7670D_mmc_cd);
	if(ret != 0) {
		printf("SD controller initialized with %d\n", ret);
	}

	/* eMMC */
	ret = mxsmmc_initialize(bis, 1, NULL, ts7670D_mmc_cd);
	if(ret != 0) {
		printf("eMMC controller initialized with %d\n", ret);
	}

	return 0;
}
