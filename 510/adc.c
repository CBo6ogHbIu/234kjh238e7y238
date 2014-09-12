#include "MK70F15.h"
#include "cmsis_os.h"
#include "priority_table.h"
#include "adc.h"
#include "timers.h"
#include "math/MeasAlgs.h"
int16_t adc_period[256]= {0x0000};
int16_t adc_sample[8]	= {0x0000};
Vectors res;
FreqF fr;
void mathDispatcher (void const *arg)
{
	for (;;) {
		osSignalWait(0x0001, osWaitForever);	/* wait for an event flag 0x0001 */
	}
}

osThreadId math_dispatcher_id;
osThreadDef (mathDispatcher, PRIORITY_ADC_MATH, 1, 2048 );

//	adc_busy falling edge interrupt
void PORTF_IRQHandler(void){
	char i;
	if((PORTF->ISFR & (1ul << 16)) != 0){
		PORTF->PCR[16] &= (uint32_t)~PORT_PCR_IRQC(0x0a);
		for(i = 0; i < 8; i++){
			ADC_CS_RD_ON();													//	read the word
			__triple_nop();
			adc_sample[i] = PTF->PDIR & 0x0000FFFF;	//	read v[i], suppose to be > 37ns (max datasheet value)
			ADC_CS_RD_OFF();												//	success
			__nop();			
		}
		
		//measNsignals((short*)&adc_sample, &fr, &res);
		PORTF->PCR[16] |= PORT_PCR_IRQC(0x0a);
	}
}

//	PIT0 interrupt, once 104us
void PIT0_IRQHandler(void){
	NVIC_DisableIRQ(PIT0_IRQn);
	PIT->CHANNEL[0].TCTRL = 0;
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
	ADC_RESET_ON();
	ADC_RESET_OFF();
	__nop();
	ADC_CONV_ON();
	ADC_CONV_OFF();
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;	
	NVIC_EnableIRQ(PIT0_IRQn);
}


bool adc_start(void){
	NVIC_ClearPendingIRQ(PIT0_IRQn);
	NVIC_SetPriority(PIT0_IRQn, PRIORITY_ADC_PIT);
	NVIC_EnableIRQ(PIT0_IRQn);	

	NVIC_ClearPendingIRQ(PORTF_IRQn);
	NVIC_SetPriority(PORTF_IRQn, PRIORITY_ADC_IRQ);
	NVIC_EnableIRQ(PORTF_IRQn);
	
	math_dispatcher_id =  osThreadCreate (osThread (mathDispatcher), NULL);
	return (bool)(math_dispatcher_id != NULL && timers_add_subscribtion(&math_dispatcher_id, TIMER_1MS));
}


bool adc_stop(void){
	NVIC_DisableIRQ(PIT0_IRQn);	
	NVIC_DisableIRQ(PORTF_IRQn);
	timers_remove_subscription(&math_dispatcher_id);
	return (bool)(math_dispatcher_id != NULL && osThreadTerminate(math_dispatcher_id) == osOK);
}


bool adc_initialize(void){
	char i = 0;
	//	oversampling, os0-os2 = 0x00;	
	const uint32_t OS0_MASK = 1 << 9;
	const uint32_t OS1_MASK = 1 << 10;
	const uint32_t OS2_MASK = 1 << 11;
	const uint32_t OS_SAMPLING_MASK = OS0_MASK | OS1_MASK | OS2_MASK;

	//	todo: убрать отсюда: переделать на проверку, к примеру, is_initialized в measNsignals(..)
	//	эти инициализации здесь - лишние и противоречат всем принципам модульности
	//	плюс сложность обработки ошибкок
	res = VectorsInit(0x01);
	fr = FreqFInit();		
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTF_MASK;
	PORTB->PCR[9]  |= PORT_PCR_MUX(0x01);		// 	OS0-OS2, GPIO
	PORTB->PCR[10] |= PORT_PCR_MUX(0x01);
	PORTB->PCR[11] |= PORT_PCR_MUX(0x01);
	PTB->PDDR |= OS_SAMPLING_MASK;	//	выходы
	PTB->PSOR |= OS0_MASK;					//	OS0 = 1
	PTB->PSOR |= OS1_MASK;					//	OS1 = 1
	PTB->PCOR |= OS2_MASK;					//	OS2 = 0

	//	ADC_BUSY, высокий уровень, вход
	PORTF->PCR[16] = PORT_PCR_IRQC(0x0a) | PORT_PCR_MUX(0x01);		// 	GPIO + прерывание по спаду(0x0a)
	PTF->PDDR &= (uint32_t)~(1UL << 16);		//	вход
	
	//	ADC_CONV, низкий уровень, выход
	PORTF->PCR[17] = PORT_PCR_MUX(0x01);		// 	ADC_CONV, GPIO
	PTF->PDDR |= 1UL << 17;									//	выход
	ADC_CONV_OFF();
	
	//	ADC_RES, высокий уровень, output
	PORTF->PCR[18] = PORT_PCR_MUX(0x01);		// 	ADC_RES, GPIO
	PTF->PDDR |= 1UL << 18;									//	выход

//	ADC_CS, низкий уровень, выход
	PORTF->PCR[19] 	|= 	PORT_PCR_MUX(0x01);	//	ADC_CS_RD, GPIO
	PTF->PDDR 		 	|= 	1UL << 19;					//	выход
	ADC_CS_RD_OFF();

	//	шина данных ацп
	for(i = 0; i < 16; i++){
		PORTF->PCR[i] = PORT_PCR_MUX(0x01);		// 	GPIO
		PTF->PDDR &= (uint32_t)~(1UL << i);		//	вход
	}
	//	PIT0, 104us таймер, генерирующий прерывания
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0x00UL;
	//PIT->CHANNEL[0].LDVAL = 0x00001E77;	//	104us
	//PIT->CHANNEL[0].LDVAL = 0x00001E83;	//	104.16us(<9600)	
	PIT->CHANNEL[0].LDVAL = 0x00001E84;	//	104.173us(>9600)
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
	return true;
}
