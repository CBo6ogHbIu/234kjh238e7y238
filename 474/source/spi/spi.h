#ifndef SPI_H
#define SPI_H
#include <stdbool.h>

bool spi_initialize(void);
bool spi_start(void);
void spi_stop(void);

enum SPI_DEVICES_LIST {SDL_LED1, SDL_LED2, SDL_LED3, SDL_RELAY, SDL_DINP, SDL_KEYB, SDL_COUNT};
extern unsigned char spi_device_mask [SDL_COUNT];

#define SPI_LED1  spi_device_mask[SDL_LED1 ]
#define SPI_LED2  spi_device_mask[SDL_LED2 ]
#define SPI_LED3  spi_device_mask[SDL_LED3 ]
#define SPI_RELAY spi_device_mask[SDL_RELAY]
#define SPI_DINP  spi_device_mask[SDL_DINP ]
#define SPI_KEYB  spi_device_mask[SDL_KEYB ]

/**  распиновка диодов:
  0   1   2   3   4   5   6   7
  r0  g0  r1  g1  r2  g2  r3  g3
  r[0-3] красные диоды с соответствующим индексом
  g[0-3] зеленые диоды с соответствующим индексом 
  некоторые цвета: 0x55 - красный, 0x99 - вперемешку, 0xaa - зеленый, 0xff - одновременно красный и зеленый = оранжевый
  оранжевый видно плохо, лучше не использовать
*/
bool spi_send_bit(enum SPI_DEVICES_LIST device_index, unsigned char bit_index, bool value);

/**  отправить байт одному или нескольким получателям
  использование:
  spi_send_byte(SPI_LED1 | SPI_LED2 | SPI_DINP, 0x05);
 */
void spi_send_byte(unsigned char mask, unsigned char byte);

//  todo: обдумать перевод записи на mailbox-ы

/**  Для визуального тетисрования работоспособности диодов
*/
void spi_LED_test(void);
#endif
