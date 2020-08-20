
#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/pwm.h>

/*
 * 50 is flicker fusion threshold. Modulated light will be perceived
 * as steady by our eyes when blinking rate is at least 50.
 */


//LED is connect to PB3 pin (D3)


#define PERIOD (USEC_PER_SEC / 50U)

/* in micro second */
#define FADESTEP	2000

void main(void)
{
	struct device *pwm_dev;
	u32_t pulse_width = 0U;
	u8_t dir = 0U;

	printk("PWM demo app-fade LED\n");

	pwm_dev = device_get_binding("PWM_2");
	if (!pwm_dev) {
		printk("Cannot find %s!\n", "PWM_2");
		return;
	}

int err;
	while (1) {
		if (err = pwm_pin_set_usec(pwm_dev, 2,
					PERIOD, pulse_width, 0)) {
			printk("pwm pin set fails, %x\n", err);
			return;
		}
		if (dir) {
			if (pulse_width < FADESTEP) {
				dir = 0U;
				pulse_width = 0U;
			} else {
				pulse_width -= FADESTEP;
			}
		} else {
			pulse_width += FADESTEP;

			if (pulse_width >= PERIOD) {
				dir = 1U;
				pulse_width = PERIOD;
			}
		}

		k_sleep(K_SECONDS(1));
		printk("pulse_width:  %d \n", pulse_width);
	}
}
