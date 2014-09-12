/*-----------------------------------------------------------------------------
 *      Имя:    			main.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	главный файл проекта
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/

#include <stdbool.h>
#include <rtl.h>
#include "system/system.h"
#include "diagnostics/diagnostics.h"
#include "file_system/file_system.h"
//#include "adc/adc.h"
#include "rtc/rtc.h"
#include "log/log.h"
#include <MK70F15.h>	//	todo: delete, just for debugging
#include "serial/SER.h"

#include "lcd/lcd.h"
#include "spi/led.h"
#include "spi/relay.h"

OS_TID dispatcher_task_id = 0;
bool is_low_power_mode = false;

//	TODO: move to exceptions.cpp/h module
void HardFault_Handler(void){
	//	stop section
  char message[32] = {0};
  sprintf(message, "Task %d", isr_tsk_get());
	log_write("Hard fault detected. System stop and reset. Task %s\n", message);
	isr_evt_set(ST_STOP, dispatcher_task_id);
	NVIC_SystemReset();
}


__task void init (void){
  //  TODO: вроде как нечитаемо. возможно, скрыть присваивание sf.init_error_status, растащить по модулям. плотнее обдумать структуру.
	sf.init_error_status = rtc_initialize();
  //sf.init_error_status &= diag_initialize();
	//sf.init_error_status &= adc_initialize();	
  sf.init_error_status &= fs_initialize();
  #ifdef __DEBUG
  SER_Init();
  #endif
  sf.init_error_status &= spi_initialize();
  sf.init_error_status &= lcd_initialize();
	log_write("System initialization: %s\n", sf.init_error_status ? "OK" : "FAIL");
	os_evt_set(ST_INIT, dispatcher_task_id);
	os_tsk_delete_self();
}


__task void run (void){
	log_write("Diagnostics module start: %s\n", diag_start() ? "OK" : "FAIL");
	//log_write("ADC module start: %s", adc_start ? "OK" : "FAIL");
	//diag_start();

  
  while(1){
    //  2 режима работы:
    if(is_low_power_mode){  // пониженного энергопотребления
      //  led_off();
      //  lcd_off();
      //  relay_off();
    }
    else{                   // и обычный
//          led_test();
//          lcd_test();
//          relay_test();
    }
  }
	os_evt_set(ST_RUN, dispatcher_task_id);
	os_tsk_delete_self();
}


__task void stop(void){
	sf.hard_fault_status 		= false;
	sf.init_error_status 		= false;
	sf.runtime_error_status = false;
	//adc_stop();
	diag_stop();
	os_evt_set(ST_STOP, dispatcher_task_id);
	os_tsk_delete_self();
}

__task void dispatcher (void){
	dispatcher_task_id = os_tsk_self();
	os_tsk_prio_self(PRIORITY_MAIN_TASKS);

	while(1){
		os_tsk_create(init, 		PRIORITY_MAIN_TASKS);
		os_evt_wait_or(ST_INIT, 0xffff);
		os_tsk_create(run, 			PRIORITY_MAIN_TASKS);
		os_evt_wait_or(ST_RUN, 	0xffff);
		os_tsk_create(stop, 		PRIORITY_MAIN_TASKS);
		os_evt_wait_or(ST_STOP, 0xffff);
	}
}


int main (void) {
  os_sys_init(dispatcher);
}
