/*-----------------------------------------------------------------------------
 *      Имя:    			log.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	журнал системных событий
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
 
#include "log.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
FILE * log_ = NULL;

bool log_open(void){
	return (log_ = fopen("system.log", "w+")) != NULL;
}


void log_close(void){
	if(log_ != NULL)
		fclose(log_);
}

/*Использование записи в лог:
	1 параметр - __FILE__ вызывающей функции
	2 параметр - __LINE__ вызывающей функции
	Реализовано в log.h в виде макроса #define log_write(...) log_write_ext(__FILE__, __LINE__, __VA_ARGS__);
3 параметр - ТОЛЬКО СИМВОЛЬНЫЙ.
*/
//TODO: конструкция вида printf(format, args) работать отказывается, разобраться
bool log_write_ext(const char * const file, int line, const char *const format, const char * const message){
  bool result = log_open();

#ifdef __DEBUG
  printf("%s, %s, %s, line %d, ", __DATE__, __TIME__, file, line);
	printf(format, message);
#else
	result = result && fprintf(log_, "%s, %s, %s, line %d, ", __DATE__, __TIME__, file, line)) > 0;
  result = result && message != NULL && fprintf(log_, format, message)) > 0;
#endif

  log_close();
	return result;
}
