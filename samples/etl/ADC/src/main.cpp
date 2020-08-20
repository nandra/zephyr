/*
 * Copyright (c) 2019 Vestas Wind Systems A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Potentiometer -> pin A0
 * 
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/adc.h>
#include <stdio.h>
#include <sys/printk.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(main);
#define RTD_NOMINAL_RESISTANCE 100

uint16_t temp_data[8];

void main(void)
{
	struct device *adc1;
	s32_t buffer;
	int err, ret;
	static const struct adc_channel_cfg ch_cfg = {
		.gain = ADC_GAIN_1,
		.reference = ADC_REF_INTERNAL,
		.channel_id = 0,
	};
	const struct adc_sequence seq = {
        .channels    = BIT(0),
        .buffer      = temp_data,
        .buffer_size = sizeof(temp_data),
        .resolution  = 12,
	};

	adc1 = device_get_binding("ADC_1");
	if (!adc1) {
		printk("err %p", adc1);
		return;
	}

	err = adc_channel_setup(adc1, &ch_cfg);
	if (err) {
		printk("failed to setup ADC channel (err %d)", err);
		return;
	}



	while (1) {
                ret = adc_read(adc1, &seq);
                if (ret != 0) {
                        printk("adc_read() failed with code %d", ret);
                } else {
                        printk("adc_read() value %d\n", temp_data[0]);
                }
		k_sleep(K_MSEC(1000));
        }
}
