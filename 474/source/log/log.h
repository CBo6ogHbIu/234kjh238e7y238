#ifndef LOG_H
#define LOG_H
#include <stdbool.h>
bool log_write_ext(const char * const file, int line, const char * const format, const char * const message);
#define log_write(format, message) log_write_ext(__FILE__, __LINE__, format, message);

#endif
