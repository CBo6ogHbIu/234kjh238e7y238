#include "Diagnostics.h"
#include "cmsis_os.h"
#include "MK70F15.h"
#include "timers.h"

osThreadId 	diag_thread_id;
enum WORK_STATUSES{ WS_OK = 5000, WS_ERROR = 150, WS_INITIAL = 1000 }; 

uint32_t processing_status = WS_INITIAL;  //	common diagnostics status
const uint32_t 		led_mask = 1UL << 18;		//	mask LED of port B, pin 18

bool check(bool function_result){
	processing_status = ( function_result == true ) ? WS_OK : WS_ERROR;
	return function_result;
}   

void blink (void const *argument) {
	for (;;) {
		osSignalWait(0x0001, osWaitForever);
		PTB->PTOR = led_mask;
	}
}

osThreadDef(blink, osPriorityNormal, 1, 0);

bool diag_initialize(void){
	SIM->SCGC5 		|= SIM_SCGC5_PORTB_MASK;	//	port B clocking
	PORTB->PCR[18] = PORT_PCR_MUX(0x01);		//	GPIO
	PTB->PDDR 		 = led_mask;							//	pin type - output
	return true;
}

bool diag_start(void){
	diag_thread_id  = osThreadCreate(osThread(blink), NULL);
	return (bool)(diag_thread_id != NULL && timers_add_subscribtion(&diag_thread_id, TIMER_1S));
}

bool diag_stop(void){
	timers_remove_subscription(&diag_thread_id);
	return (bool)(diag_thread_id != NULL && osThreadTerminate(diag_thread_id) == osOK);
}
