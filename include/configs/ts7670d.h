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

/* The rest of the configuration is shared */
#include <configs/mxs.h>

#endif /* __CONFIGS_TS7670D_H__ */
