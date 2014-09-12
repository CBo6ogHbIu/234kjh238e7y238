#include <stdbool.h>
#include "Diagnostics.h"
#include "nand/NAND_Flash.h"
//#include "FileSystem.h"
#include "adc.h"
#include "rtc.h"
#include "timers.h"

#include "cmsis_os.h"
#include "MK70F15.h"	//	todo: delete, just for debugging

//	todo: move to exceptions.cpp/h module
void HardFault_Handler(void){
	//	stop section	
	adc_stop();		
	diag_stop();
	timers_stop();
	timers_uninitialize();
	NVIC_SystemReset();
}

int main (void) {
	bool init_success = true;
	//	init section	
	init_success &= timers_initialize();
	init_success &= rtc_initialize();
	init_success &= diag_initialize();
	init_success &= adc_initialize();	
	init_success &= nand_initialize();
	//init_success &= fs_initialize();
	//	start section
	init_success &= timers_start();
	init_success &= diag_start();
	init_success &= adc_start();
	init_success &= nand_start();
  
	NVIC->STIR = NFC_IRQn;
	while(1){}
}
