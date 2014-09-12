#ifndef LED_H
#define LED_H
#include <stdbool.h>
#include "spi.h"

bool led_initialize(void);
void led_test(void);


/**  распиновка диодов:
  0   1   2   3   4   5   6   7
  r0  g0  r1  g1  r2  g2  r3  g3
  r[0-3] красные диоды с соответствующим индексом
  g[0-3] зеленые диоды с соответствующим индексом 
  некоторые цвета: 0x55 - красный, 0x99 - вперемешку, 0xaa - зеленый, 0xff - одновременно красный и зеленый = оранжевый
  оранжевый видно плохо, лучше не использовать
*/
bool led_send_bit( enum SPI_DEVICES_LIST device_index, unsigned char bit_index, bool value);

/**  отправить байт одному или нескольким получателям
  использование:
  spi_send_byte(SPI_LED1 | SPI_LED2, 0x05);
 */
void led_send_byte(unsigned char mask, unsigned char byte);

/**  погасить все светодиоды
 */
void led_off(void);
#endif
