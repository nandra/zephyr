
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>
#include <drivers/pwm.h>

#include <etl/list.h>
#include <etl/container.h>

#define led 6
#define but 7
int state=1; 




void main()
{
  struct device *dev = device_get_binding("GPIOA");
  if (dev == NULL) {
    return;
  }
  int ret = gpio_pin_configure(dev, led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
	  return;
  }
  
  while(1) {
    gpio_pin_set(dev, led, state);
    k_msleep(1000);
    state = !state;

  } 



/*
  struct device *button = device_get_binding("GPIOA");
  struct device *LD = device_get_binding("GPIOA");
  gpio_pin_configure(button, but, GPIO_INPUT | GPIO_PULL_DOWN);
  gpio_pin_configure(LD, led, GPIO_OUTPUT_ACTIVE);
  while(1){
  int hodnota = gpio_pin_get(button, but);
  if( hodnota == 1 ){
    gpio_pin_set(LD, led, 1 );
  }
  else
  {
    gpio_pin_set(LD, led, 0 );
  }
  }
*/



}
