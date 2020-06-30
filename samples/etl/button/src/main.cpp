/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 * 
 * led is conect on pin (D13)
 * 
 * boton is conect on pin 6 (D12)
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#define SLEEP_TIME_MS	1000

#ifndef DT_ALIAS_SW0_GPIOS_FLAGS
#define DT_ALIAS_SW0_GPIOS_FLAGS 0
#endif

#ifndef DT_ALIAS_LED0_GPIOS_FLAGS
#define DT_ALIAS_LED0_GPIOS_FLAGS 0
#endif

int i=0;

void button_pressed(struct device *dev, struct gpio_callback *cb,
		    u32_t pins)
{
	printk("Button pressed \n",i);
	i++;
	if (i==3)
	{
		i=0;
	}
	
}

static struct gpio_callback button_cb_data;

void main(void)
{
	struct device *dev_button;
	int ret;
	int delays[3] = {1000, 500, 250};


	dev_button = device_get_binding("GPIOA");
	if (dev_button == NULL) {
		printk("Error: didn't find %s device\n",
			DT_ALIAS_SW0_GPIOS_CONTROLLER);
		return;
	}

	ret = gpio_pin_configure(dev_button, 6,
				 DT_ALIAS_SW0_GPIOS_FLAGS | GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d '%s'\n",
			ret, DT_ALIAS_SW0_GPIOS_PIN, DT_ALIAS_SW0_LABEL);
		return;
	}

	ret = gpio_pin_interrupt_configure(dev_button, 6,
					   GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on pin %d '%s'\n",
			ret, DT_ALIAS_SW0_GPIOS_PIN, DT_ALIAS_SW0_LABEL);
		return;
	}

	gpio_init_callback(&button_cb_data, button_pressed,
			   BIT(6));
	gpio_add_callback(dev_button, &button_cb_data);

#ifdef DT_ALIAS_LED0_GPIOS_CONTROLLER
	struct device *dev_led;

	dev_led = device_get_binding(DT_ALIAS_LED0_GPIOS_CONTROLLER);
	if (dev_led == NULL) {
		printk("Error: didn't find %s device\n",
			DT_ALIAS_LED0_GPIOS_CONTROLLER);
		return;
	}

	ret = gpio_pin_configure(dev_led, DT_ALIAS_LED0_GPIOS_PIN,
				 DT_ALIAS_LED0_GPIOS_FLAGS | GPIO_OUTPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d '%s'\n",
			ret, DT_ALIAS_LED0_GPIOS_PIN, DT_ALIAS_LED0_LABEL);
		return;
	}
#endif
	printk("Press %s on the board\n", DT_ALIAS_SW0_LABEL);

	while (1) {

		bool val;
		val=!val;
		gpio_pin_set(dev_led, DT_ALIAS_LED0_GPIOS_PIN, val);
		k_msleep(delays[i]);

	}
}
