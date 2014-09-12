/*-----------------------------------------------------------------------------
 *      Имя:    			rtc.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	модуль таймера реального времени (32 кГц)
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
 
#include "rtc.h"
#include <MK70F15.h>
#include <rtl.h>

void RTC_IRQHandler(void){
	if((RTC->SR & RTC_SR_TIF_MASK)== 0x01){					// ошибочное прерывание
		RTC->SR &= 0x07;  				//	сбросить TCE, иначе RTC_TSR не запишется
		RTC->TSR = 0x00000000;  	//	сбросить TIF
	}   
	else
		if((RTC->SR & RTC_SR_TOF_MASK) == 0x02){			// переполнение
			RTC->SR &= 0x07;  			//	сбросить TCE, иначе RTC_TSR не запишется
			RTC->TSR = 0x00000000;  //	сбросить TIF
		}
		else if((RTC->SR & RTC_SR_TAF_MASK) == 0x04){	// сигнал
		  RTC->TAR += 1;					//	новое значение для сигнала каждую секунду
		}   
		else{
															//	ничего
		}
}

void RTC_Seconds_IRQHandler(void){
	uint32_t rtcTime = RTC->TSR;
}

bool rtc_initialize(void){
	//	инициализация
  SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;						//	питание
  RTC->CR &= ~RTC_CR_SWR_MASK; 								//	пуск
  RTC->CR |= RTC_CR_OSCE_MASK;
	
	os_dly_wait( 1000 );												//	в даташите нет инфы о startup time для CM315-32.768KDZF-UT
																							//	для похожих резонаторов 838ms => с запасом выбрана 1 секунда
	
  RTC->TCR = RTC_TCR_CIR(0) | RTC_TCR_TCR(0);	//	компенсация времени
  RTC->TSR = 0x0F;														//	секунды
  RTC->TAR = 0xFFFFFFFF;											//	сигнальный регистр
	//	пуск
  RTC->IER= RTC_IER_TIIE_MASK | RTC_IER_TOIE_MASK | RTC_IER_TSIE_MASK;	//	прерывания

  NVIC_EnableIRQ(RTC_IRQn);																							//	NVIC
	NVIC_EnableIRQ(RTC_Seconds_IRQn);

  RTC->SR |= RTC_SR_TCE_MASK;																						//	счетчик	
	return true;
}

void rtc_stop(void){
	RTC->IER= 0x00;
  NVIC_DisableIRQ(RTC_IRQn);
	NVIC_DisableIRQ(RTC_Seconds_IRQn);	
	RTC->SR = 0x00;
}
