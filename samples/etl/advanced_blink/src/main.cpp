//***********************************************************************************
// A version of the Blink demo, but with delays stored in two instances of etl::list
//***********************************************************************************

#undef min
#undef max

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#include <etl/list.h>
#include <etl/container.h>

#define led 6

void iterate(const etl::ilist<int>& delays, struct device* dev)
{
    if (dev == NULL)
      return;
    etl::ilist<int>::const_iterator itr;

    // Iterate through the list.
    itr = delays.begin();

    while (itr != delays.end())
    {
	    gpio_pin_set(dev, led, 1);
	    k_msleep(100);
	    gpio_pin_set(dev, led, 0);
	    k_msleep(*itr++);
    }
}

void main()
{
  int delay_times1[] = { 900, 800, 700, 600, 500, 400, 300, 200, 100 };
  int delay_times2[] = { 400, 300, 200, 100 };

  // Fill the first delay list, then reverse it.
  // Notice how we don't have to know the size of the array!
  const size_t size1 = sizeof(etl::array_size(delay_times1));
  etl::list<int, size1> delays1(etl::begin(delay_times1), etl::end(delay_times1));
  delays1.reverse();

  // Fill the second delay list,
  const size_t size2 = sizeof(etl::array_size(delay_times2));
  etl::list<int, size2> delays2(etl::begin(delay_times2), etl::end(delay_times2));

  struct device *dev;
	
  dev = device_get_binding("GPIOA");
	if (dev == NULL) {
     return;
	}
	
	int ret = gpio_pin_configure(dev, led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
  
  while (true)
  {
    iterate(delays1, dev);
    iterate(delays2, dev);
  }
}
