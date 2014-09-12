/*-----------------------------------------------------------------------------
 *      Имя:    			diagnostics.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	система диагностики
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
#include "Diagnostics.h"
#include <MK70F15.h>
#include "../system/system.h"
OS_TID diag_task_id = 0;

enum WORK_STATUSES{ WS_OK = 5000, WS_ERROR = 150, WS_INITIAL = 1000 }; 

uint32_t processing_status = WS_INITIAL;  //	общий статус диагностики
#define PIN_18_MASK 1UL << 18;						//	маска PORTB, пин 18


OS_TID diag_get_task_id(void){
	return diag_task_id;
}


__task void timer_1s_task(void){
  for(;;){
		//PTB->PTOR = PIN_18_MASK;		
		if( os_tmr_create(1000, st_diag_id) == NULL )	//	0x03e8 = 1000 = 1 секунда
      os_tsk_delete_self();
		else
			os_evt_wait_or(st_diag_id, 0xffff);
  }
}

bool diag_initialize(void){
	SIM->SCGC5 		|= SIM_SCGC5_PORTB_MASK;	//	питание
	PORTB->PCR[18] = PORT_PCR_MUX(0x01);		//	ввод/вывод
	PTB->PDDR 		 = PIN_18_MASK;						//	тип - выход
	return true;
}

bool diag_start(void){
	if(diag_task_id == 0)
		diag_task_id = os_tsk_create(timer_1s_task, PRIORITY_DIAG);
	return diag_task_id != 0;
}

bool diag_stop(void){
	return diag_task_id != 0 && os_tsk_delete(diag_task_id) == OS_R_OK;
}
