
#include <errno.h>
#include <kernel.h>
#include <init.h>
#include <stdbool.h>

#include <drivers/i2c.h>
#include <display/lcd_i2c.h>
#include <sys/util.h>

#define LOG_LEVEL CONFIG_DISPLAY_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(LCD_HD44780);

#define SLEEP_IN_US(_x_)	((_x_) * 1000)

#define LCD_DISPLAY_ADDR		(0x27)


// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

struct command {
	u8_t control;
	u8_t data;
};

struct _data {
	struct device *i2c;
	u8_t input_set;
	u8_t display_switch;
	u8_t function;
};

struct _driver {
	u16_t	lcd_addr;
	u16_t	rgb_addr;
};

static inline void sleep(u32_t sleep_in_ms)
{
	k_busy_wait(SLEEP_IN_US(sleep_in_ms));
}


void lcd_send_cmd (struct device *port, char cmd)
{
	const struct _driver * const rom = (struct _driver *)
						port->config->config_info;
	struct _data *dev = port->driver_data;
 	
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	i2c_write (dev->i2c, (uint8_t *) data_t, 4, rom->lcd_addr);
}
void lcd_send_data (struct device *port, char data)
{
	const struct _driver * const rom = (struct _driver *)
						port->config->config_info;
	struct _data *dev = port->driver_data;
 	
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1
	i2c_write(dev->i2c, (uint8_t *) data_t, 4, rom->lcd_addr);
}
int lcd_init (struct device *port)
{ 	
	struct _data *dev = port->driver_data;
	
	LOG_DBG("initialize called");
	
	dev->input_set = 0U;
	dev->display_switch = 0U;
	dev->function = 0U;

	/*
	 * First set up the device driver...
	 * we need a pointer to the I2C device we are bound to
	 */
	dev->i2c = device_get_binding(
			CONFIG_LCD_HD44780_I2C_MASTER_DEV_NAME);
	if (!dev->i2c) {
		return -EPERM;
	}
	// 4 bit initialisation
	k_msleep(50);  // wait for >40ms
	lcd_send_cmd (port, 0x30);
	k_msleep(5);  // wait for >4.1ms
	lcd_send_cmd (port, 0x30);
	k_msleep(1);  // wait for >100us
	lcd_send_cmd (port, 0x30);
	k_msleep(10);
	lcd_send_cmd (port, 0x20);  // 4bit mode
	k_msleep(10);

  // dislay initialisation
	lcd_send_cmd (port, 0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	k_msleep(1);
	lcd_send_cmd (port, 0x09); //Display on/off control --> D=0,C=0, B=0  ---> display off
	k_msleep(2);
	lcd_send_cmd (port, 0x01);  // clear display
	k_msleep(3);
	lcd_send_cmd (port, 0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	k_msleep(2);
	lcd_send_cmd (port, 0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	k_msleep(2);


	lcd_clear(port);
	lcd_home(port);
	k_msleep(2);
	return 0;
}
void lcd_set_cursor(struct device *port, uint8_t row, uint8_t col)
{
	if(row == 1)
	{
		for (int i = 0; i < 40; i++)
		{
			lcd_send_cmd(port, 0x14);
		}
	}

	for (int i = 1; i < col; i++)
	{
		lcd_send_cmd(port, 0x14);
	}
}
void lcd_send_string (struct device *port, char *str)
{ 	
	while (*str) 
	{
		lcd_send_data (port, *str++);
	}
}
void lcd_clear(struct device *port)
{
	lcd_send_cmd(port, 0x01);
	lcd_home(port);
}
void lcd_puts(struct device *port ,const char *string)
{
	while (*string)
	{
		lcd_putch(port, *string++);
	} 
	
}
void lcd_putch(struct device *port ,char data)
{
	lcd_send_data(port, data);
}
void lcd_home(struct device *port)
{
	lcd_send_cmd(port, 0x02);
	k_msleep(2);
}


static const struct _driver lcd_config = {
	.lcd_addr = LCD_DISPLAY_ADDR,
};

static struct _data lcd_driver = {
	.i2c = NULL,
	.input_set = 0,
	.display_switch = 0,
	.function = 0,
};

DEVICE_AND_API_INIT(grove_lcd, LCD_NAME, lcd_init,
			&lcd_driver, &lcd_config,
			POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY,
			(void *)&lcd_driver);