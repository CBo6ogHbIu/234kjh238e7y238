#ifndef RELAY_H
#define RELAY_H
#include <stdbool.h>
#include "spi.h"

bool relay_initialize(void);
bool relay_send_byte( unsigned char byte );
bool relay_send_bit( unsigned char bit_index, bool value );
void relay_test(void);

/**  вырубить все реле
 */
void relay_off(void);
#endif
