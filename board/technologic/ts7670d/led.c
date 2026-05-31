#include <asm/io.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <asm/arch/iomux-mx28.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <config.h>
#include <linux/types.h>
#ifdef CONFIG_LED_STATUS
#include <status_led.h>
#endif

static unsigned int saved_state[4] = {CONFIG_LED_STATUS_OFF,
	CONFIG_LED_STATUS_OFF, CONFIG_LED_STATUS_OFF, CONFIG_LED_STATUS_OFF};

iomux_cfg_t const led_pads[] = {
	CONFIG_RED_LED, // Red
	CONFIG_GREEN_LED, // Green
	CONFIG_YEL_LED, // Yellow
	CONFIG_BLUE_LED, // Blue
};

void coloured_LED_init(void)
{
	mxs_iomux_setup_multiple_pads(led_pads, ARRAY_SIZE(led_pads));
}

void red_led_on(void)
{
	gpio_direction_output(led_pads[0], 0);
	saved_state[STATUS_LED_RED] = CONFIG_LED_STATUS_ON;
}

void red_led_off(void)
{
	gpio_direction_output(led_pads[0], 1);
	saved_state[STATUS_LED_RED] = CONFIG_LED_STATUS_OFF;
}

void green_led_on(void)
{
	gpio_direction_output(led_pads[1], 0);
	saved_state[STATUS_LED_GREEN] = CONFIG_LED_STATUS_ON;
}

void yellow_led_off(void)
{
	gpio_direction_output(led_pads[2], 1);
	saved_state[STATUS_LED_YELLOW] = CONFIG_LED_STATUS_OFF;
}

void yellow_led_on(void)
{
	gpio_direction_output(led_pads[2], 0);
	saved_state[STATUS_LED_YELLOW] = CONFIG_LED_STATUS_ON;
}

void blue_led_off(void)
{
	gpio_direction_output(led_pads[3], 0);
	saved_state[STATUS_LED_BLUE] = CONFIG_LED_STATUS_OFF;
}

void blue_led_on(void)
{
	gpio_direction_output(led_pads[3], 1);
	saved_state[STATUS_LED_BLUE] = CONFIG_LED_STATUS_ON;
}

void green_led_off(void)
{
	gpio_direction_output(led_pads[1], 1);
	saved_state[STATUS_LED_GREEN] = CONFIG_LED_STATUS_OFF;
}

void __led_init(led_id_t mask, int state)
{
	__led_set(mask, state);
}

void __led_toggle(led_id_t mask)
{
	if (STATUS_LED_RED == mask) {
		if (CONFIG_LED_STATUS_ON == saved_state[STATUS_LED_RED])
			red_led_off();
		else
			red_led_on();
	} else if (STATUS_LED_GREEN == mask) {
		if (CONFIG_LED_STATUS_ON == saved_state[STATUS_LED_GREEN])
			green_led_off();
		else
			green_led_on();
	} else if (STATUS_LED_YELLOW == mask) {
		if (CONFIG_LED_STATUS_ON == saved_state[STATUS_LED_YELLOW])
			yellow_led_off();
		else
			yellow_led_on();
	} else if (STATUS_LED_BLUE == mask) {
		if (CONFIG_LED_STATUS_ON == saved_state[STATUS_LED_BLUE])
			blue_led_off();
		else
			blue_led_on();
	}
}

void __led_set(led_id_t mask, int state)
{
	switch(mask) {
	  case STATUS_LED_RED:
		if(CONFIG_LED_STATUS_ON == state) {
			red_led_on();
		} else {
			red_led_off();
		}
		break;
	  case STATUS_LED_GREEN:
		if(CONFIG_LED_STATUS_ON == state) {
			green_led_on();
		} else {
			green_led_off();
		}
		break;
	  case STATUS_LED_YELLOW:
		if(CONFIG_LED_STATUS_ON == state) {
			yellow_led_on();
		} else {
			yellow_led_off();
		}
		break;
	  case STATUS_LED_BLUE:
		if(CONFIG_LED_STATUS_ON == state) {
			blue_led_on();
		} else {
			blue_led_off();
		}
		break;
	}
}
