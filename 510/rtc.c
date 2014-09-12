#include "rtc.h"
#include "MK70F15.h"
#include <stdio.h>

void RTC_IRQHandler(void){
	if((RTC->SR & RTC_SR_TIF_MASK)== 0x01){
		// SRTC time invalid interrupt entered...
		RTC->SR &= 0x07;  				//clear TCE, or RTC_TSR can  not be written
		RTC->TSR = 0x00000000;  	//clear TIF
	}   
	else
		if((RTC->SR & RTC_SR_TOF_MASK) == 0x02){
			//SRTC time overflow interrupt entered...
			RTC->SR &= 0x07;  			//clear TCE, or RTC_TSR can  not be written
			RTC->TSR = 0x00000000;  //clear TOF
		}
		else if((RTC->SR & RTC_SR_TAF_MASK) == 0x04){
		  // SRTC alarm interrupt entered...
		  RTC->TAR += 1;// Write new alarm value, to generate an alarm every second add 1
		}   
		else{
			//No valid Flag was set!
		}
}

void RTC_Seconds_IRQHandler(void){
	uint32_t rtcTime = RTC->TSR;
	//printf("rtcTime = %d\n", rtcTime);
}
bool rtc_initialize(void){
	unsigned int i;
	
  //SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;	/*enable the clock to SRTC module register space*/
  /*Only VBAT_POR has an effect on the SRTC, RESET to the part does not, so you must manually reset the SRTC to make sure everything is in a known state*/
  /*clear the software reset bit*/
  RTC->CR  &= ~RTC_CR_SWR_MASK; 

  //RTC->CR |= RTC_CR_OSCE_MASK;							//	включение, уже есть в system_mk..

	//	todo: проверить необходимость, возможно уже прошло достаточно времени со старта системы
	//	Перетащить полностью в system_mk...., выделить в отдельную функцию
	//	в даташите нет инфы о startup time для CM315-32.768KDZF-UT
	//	для похожих резонаторов 838ms, поэтому выбрана 1 секунда
	for(i = 0; i < 50000000; i++);
	
  RTC->TCR = RTC_TCR_CIR(0) | RTC_TCR_TCR(0);	//	компенсация времени
  RTC->TSR = 0x0F;														//	секунды
  RTC->TAR = 0xFFFFFFFF;											//	сигнальный регистр

	return true;
}	
bool rtc_start(void){
  /*Enable Interrupts*/
  RTC->IER= RTC_IER_TIIE_MASK | RTC_IER_TOIE_MASK | RTC_IER_TSIE_MASK;

  /*Enable NVIC RTC interrupt*/
  NVIC_EnableIRQ(RTC_IRQn);
	NVIC_EnableIRQ(RTC_Seconds_IRQn);

  /*Enable the counter*/
  RTC->SR |= RTC_SR_TCE_MASK;
	return true;
}
void rtc_stop(void){
	RTC->IER= 0x00;
  NVIC_DisableIRQ(RTC_IRQn);
	NVIC_DisableIRQ(RTC_Seconds_IRQn);	
	RTC->SR = 0x00;
}
