/*-----------------------------------------------------------------------------
 *      Имя:    			File_system.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	Инициализация файловой системы
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
 
#include "file_system.h"
#include <rtl.h>
#include <string.h>

bool fs_initialize(){
  U32 retv = 0;
	char count = 0;
  while ((retv = finit (NULL)) != 0 && count < 2) {	//	инициализация всех подключенных дисков
		if (retv == 1) {
			return false;																	//	nand память не подключена
		}
		else
			retv = fformat ("NTKB");
		count++;
	}
	if(retv == 0 && fanalyse("") > 50) 
    fdefrag ("");
	return retv == 0;
}

U64 fs_get_free_space (void) {
  return ffree("");
}

U64 fs_fwrite(char * file_name, char * buffer, U8 size, U64 count){
	FILE * f = NULL;
	if(file_name == NULL || strlen(file_name) == 0 || buffer == NULL || strlen(buffer) == 0 || size == 0 || size > ffree("") || count == 0)
		return 0;
	f = fopen(file_name, "w");
	if(f == NULL)
		return 0;
	return fwrite(buffer, size, count, f);
}

void fs_uninitialize(){
	funinit (NULL);  
}
