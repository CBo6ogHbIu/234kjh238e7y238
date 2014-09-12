#ifndef LCD_H
#define LCD_H
#include <stdbool.h>
bool lcd_initialize(void);
void lcd_test(void);
//  вывод строки
//  x[0..19] y[0..3]
bool lcd_outtextXY(unsigned char x, unsigned char y, const char *  data);
//  вывод одного символа(для спецсимволов)
bool lcd_outsymbolXY(unsigned char x, unsigned char y, unsigned char symbol);
//  вывод буфера(4 строки х 20 символов = 80 символов)
bool lcd_send_buffer(const char *  buffer);

void lcd_on(void);
void lcd_off(void);

/**  погасить дисплей
 */
void lcd_off(void);
#endif
