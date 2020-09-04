
#ifndef ZEPHYR_INCLUDE_DISPLAY_LCD_I2C_H_
#define ZEPHYR_INCLUDE_DISPLAY_LCD_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_NAME			"LCD"

void lcd_clear(struct device *port );
void lcd_puts(struct device *port ,const char *string);
void lcd_putch(struct device *port ,char pchar);
void lcd_home(struct device *port );
int lcd_init(struct device *port);
void lcd_send_cmd (struct device *port, char cmd);
void lcd_send_data (struct device *port, char data);
void lcd_send_string (struct device *port, char *str);
void lcd_set_cursor(struct device *port, uint8_t row, uint8_t col);

#ifdef __cplusplus
}
#endif

#endif 
