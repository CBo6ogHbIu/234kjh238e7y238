#ifndef ADC_H
#include <stdint.h>
#include <stdbool.h>

#define ADC_RESET_ON()		PTF->PSOR |= 1UL << 18
#define ADC_RESET_OFF()		PTF->PCOR |= 1UL << 18
#define ADC_CONV_ON() 		PTF->PCOR |= 1UL << 17
#define ADC_CONV_OFF() 		PTF->PSOR |= 1UL << 17
#define ADC_CS_RD_ON() 		PTF->PCOR |= 1UL << 19
#define ADC_CS_RD_OFF()		PTF->PSOR |= 1UL << 19

bool adc_initialize(void);
void adc_uninitialize(void);
bool adc_start(void);
bool adc_stop(void);

#define ADC_H
#endif
