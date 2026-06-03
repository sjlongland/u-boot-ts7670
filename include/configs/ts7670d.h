/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2026 Stuart Longland <me@vk4msl.com>
 *
 * (C) Copyright 2011 Freescale Semiconductor, Inc.
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
 *
 * Based on m28evk.h:
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 */
#ifndef __CONFIGS_TS7670D_H__
#define __CONFIGS_TS7670D_H__

/* Memory configuration */
#define PHYS_SDRAM_1			0x40000000	/* Base address */
#define PHYS_SDRAM_1_SIZE		0x40000000	/* Max 1 GB RAM */
#define CFG_SYS_SDRAM_BASE		PHYS_SDRAM_1

#define CONFIG_AUTOBOOT_KEYED          1
#define CONFIG_AUTOBOOT_PROMPT         "Press Ctrl+C to abort autoboot in %d second(s)\n"
#define CTRL(c) ((c)&0x1F)     
#define CONFIG_AUTOBOOT_STOP_STR       (char []){CTRL('C'), 0}

#define CONFIG_PREBOOT \
		" setenv bootdelay 5; "

/* Extra Environment */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"autoload=no\0" \
	"nfsroot=/nfsroot/\0" \
        "kernel=/zImage\0" \
	"fdtimage=/imx28-ts7670d.dtb\0" \
	"nfsip=192.168.0.1\0" \
	"fdtaddr=0x41000000\0" \
	"cmdline_append=rw rootwait console=ttyAMA0,115200\0" \
	"boot_fdt=yes\0" \
	"ip_dyn=yes\0" \
	"clearenv=if sf probe; then " \
		"sf erase 0x100000 0x2000 && " \
		"echo restored environment to factory default ; fi\0" \
	"update-uboot=echo Updating u-boot from /u-boot.sb; " \
		"if test ${jpsdboot} = 'on' ; " \
			"then if load mmc 0:2 ${loadaddr} /u-boot.sb; " \
				"then sf probe; " \
				"sf erase 0 80000; " \
				"sf write ${loadaddr} 0 ${filesize}; " \
			"fi;" \
		"else " \
			"if load mmc 1:2 ${loadaddr} /u-boot.sb; " \
				"then sf probe; " \
				"sf erase 0 80000; " \
				"sf write ${loadaddr} 0 ${filesize}; " \
			"fi; " \
		"fi;\0" \
	"emmcroot=/dev/mmcblk2p3\0" \
	"emmcboot=echo Booting from the onboard eMMC  ...; " \
		"if load mmc 1:2 ${loadaddr} /boot.ub; " \
			"then echo Booting from custom /boot.ub; " \
			"source ${loadaddr}; " \
		"fi; " \
		"load mmc 1:2 ${loadaddr} ${kernel}; " \
		"load mmc 1:2 ${fdtaddr} ${fdtimage}; " \
		"setenv bootargs root=${emmcroot} ${cmdline_append}; " \
		"bootz ${loadaddr} - ${fdtaddr}; \0"\
	"sdroot=/dev/mmcblk0p3\0"\
	"sdboot=echo Booting from the SD Card ...; " \
		"if load mmc 0:2 ${loadaddr} /boot.ub; " \
			"then echo Booting from custom /boot.ub; " \
			"source ${loadaddr}; " \
		"fi; " \
		"load mmc 0:2 ${loadaddr} ${kernel}; " \
		"load mmc 0:2 ${fdtaddr} ${fdtimage}; " \
		"setenv bootargs root=${sdroot} ${cmdline_append}; " \
		"bootz ${loadaddr} - ${fdtaddr}; \0"\
	"usbprod=usb start; " \
		"if usb storage; " \
			"then echo Checking USB storage for updates; " \
			"if load usb 0:1 ${loadaddr} /tsinit.ub; " \
				"then led green on;" \
				"source ${loadaddr}; " \
				"led red off; " \
				"exit; " \
			"fi; " \
		"fi; \0" \
	"nfsboot=echo Booting from NFS ...; " \
		"dhcp; " \
		"env set serverip ${nfsip}; " \
		"nfs ${loadaddr} ${nfsroot}/zImage; " \
		"setenv bootargs root=/dev/nfs ip=dhcp " \
		  "nfsroot=${serverip}:${nfsroot},vers=2,nolock ${cmdline_append}; " \
		"nfs ${fdtaddr} ${nfsroot}/${fdtimage}; " \
		"bootz ${loadaddr} - ${fdtaddr};\0"\

/* The rest of the configuration is shared */
#include <configs/mxs.h>

#endif /* __CONFIGS_TS7670D_H__ */
