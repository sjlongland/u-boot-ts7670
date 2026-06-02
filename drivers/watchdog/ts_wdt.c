/*
 * i2c watchdog.
 * Copyright (C) 2015 Mark Featherston <mark@embeddedarm.com>
 * Technologic Systems
 *
 * Adapted for use in U-Boot by Stuart Longland <stuartl@vrt.com.au>
 * VRT Systems
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <config.h>
#include <dm.h>
#include <i2c.h>
#include <log.h>
#include <wdt.h>
#include <linux/printk.h>

struct ts_wdt_priv {
	/* Timeout in 0.1 second units */
	uint16_t timeout;
};

/* The WDT expects 3 values:
 * 0 (always; register address)
 * and two bytes for the feed length in deciseconds
 * 1 <MSB>
 * 2 <LSB>
 * there are also 3 special values if they are specified
 * in the LSB with a 0 MSB:
 * 0 - 200ms
 * 1 - 2s
 * 2 - 4s
 * 3 - 10s
 * 4 - disable watchdog
 */

#define TS_WDT_CMD_TIMEOUT_0S2 (0x0000)
#define TS_WDT_CMD_TIMEOUT_2S (0x0001)
#define TS_WDT_CMD_TIMEOUT_4S (0x0002)
#define TS_WDT_CMD_TIMEOUT_10S (0x0003)
#define TS_WDT_CMD_STOP (0x0004)

static int ts_wdt_write(struct udevice *dev, u16 cmd)
{
	/*
	 * The initial 0 is taken care of, we only need to send the duration
	 * which is given in big-endian format.
	 */
	u8 out[2];

	out[0] = (cmd & 0xff00) >> 8;
	out[1] = cmd & 0xff;

	return dm_i2c_write(dev, 0, out, 2);
}

static int ts_wdt_stop(struct udevice *dev)
{
	return ts_wdt_write(dev, TS_WDT_CMD_STOP);
}

static int ts_wdt_reset(struct udevice *dev)
{
	const struct ts_wdt_priv* priv = dev_get_uclass_priv(dev);
	return ts_wdt_write(dev, priv->timeout);
}

static int ts_wdt_start(struct udevice *dev, u64 timeout, ulong flags)
{
	struct ts_wdt_priv* priv = dev_get_uclass_priv(dev);
	(void)flags;

	/* Units of 0.1 sec */
	if (timeout < 100) {
		/* Too small */
		timeout = 100;
	}

	priv->timeout = timeout / 100;
	pr_info("Starting watchdog with delay=%u\n", priv->timeout);
	return ts_wdt_reset(dev);
}

static int ts_wdt_expire_now(struct udevice *dev, ulong flags)
{
	pr_info("EXPIRE now\n");
	return ts_wdt_write(dev, TS_WDT_CMD_TIMEOUT_0S2);
}

static const struct wdt_ops ts_wdt_ops = {
	.start = ts_wdt_start,
	.stop = ts_wdt_stop,
	.reset = ts_wdt_reset,
	.expire_now = ts_wdt_expire_now,
};

static const struct udevice_id ts_wdt_ids[] = {
	{ .compatible = "technologic,ts-wdt", },
	{}
};

U_BOOT_DRIVER(ts_wdt) = {
	.name = "ts-wdt",
	.id = UCLASS_WDT,
	.of_match = ts_wdt_ids,
	.ops = &ts_wdt_ops,
	.per_child_auto = sizeof(struct ts_wdt_priv),
};
