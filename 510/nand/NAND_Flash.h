#ifndef NAND_FLASH_H
#define NAND_FLASH_H
#include <stdbool.h>
bool nand_initialize(void);
bool nand_start(void);
void nand_unitialize(void);

#endif
