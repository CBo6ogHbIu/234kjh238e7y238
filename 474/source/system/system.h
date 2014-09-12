#ifndef SYSTEM_TASKS_H
#include <stdbool.h>
#define __double_nop() 			{__nop();__nop();}
#define __triple_nop() 			{__nop();__nop();__nop();}

enum SYSTEM_TASKS_IDS{st_none, st_diag_id, st_count};

enum MAIN_TASKS{ST_INIT = 0x0001, ST_RUN = 0x0002, ST_STOP = 0x0003};

struct SYSTEM_STATE_FLAGS{
	bool hard_fault_status;
	bool init_error_status;
	bool runtime_error_status;
};

enum PRIORITY_TABLE{
  //  todo: приоритеты прерываний вытащить в самый верх таблицы
	PRIORITY_ADC_PIT 		= 254,
	PRIORITY_ADC_IRQ  	= 253,
	PRIORITY_ADC_MATH 	= 252,
	PRIORITY_DIAG 			= 249,
  PRIORITY_SPI_IRQ    = 248,
  PRIORITY_RELAY      = 247,
  PRIORITY_LED        = 246,

  //
  PRIORITY_LCD        = 200,
	PRIORITY_DISPATCHER = 2,
	PRIORITY_MAIN_TASKS = 1
};

extern struct SYSTEM_STATE_FLAGS sf;
#define SYSTEM_TASKS_H
#endif
