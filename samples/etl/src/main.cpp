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

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_HAS_NODE(LED0_NODE)
#define LED0    DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN     DT_GPIO_PIN(LED0_NODE, gpios)
#if DT_PHA_HAS_CELL(LED0_NODE, gpios, flags)
#define FLAGS   DT_GPIO_FLAGS(LED0_NODE, gpios)
#endif
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0    ""
#define PIN     0
#endif

void iterate(const etl::ilist<int>& delays, struct device* dev)
{
    if (dev == NULL)
      return;
    etl::ilist<int>::const_iterator itr;

    // Iterate through the list.
    itr = delays.begin();

    while (itr != delays.end())
    {
	    gpio_pin_set(dev, PIN, 1);
	    k_msleep(100);
	    gpio_pin_set(dev, PIN, 0);
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
	
  dev = device_get_binding(LED0);
	if (dev == NULL) {
     return;
	}
	
	int ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}
  
  while (true)
  {
    iterate(delays1, dev);
    iterate(delays2, dev);
  }
}
