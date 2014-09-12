#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "stdio.h"
#include <stdbool.h>
#include <stdint.h>
bool fs_initialize(void);
void fs_uninitialize(void);
#endif
