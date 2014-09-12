/*-----------------------------------------------------------------------------
 *      Имя:    			led.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	управление светодиодами
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
#include "led.h"
#include <MK70F15.H>
#include "../system/system.h"
#include <rtl.h>

bool led_initialize(void)
{
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK;
  
  //GPIO, LED1-LED4 */
  PORTA->PCR[4]  =  (PORTA->PCR[4]  & (uint32_t)~PORT_PCR_MUX_MASK ) | PORT_PCR_MUX(0x01);
  PORTB->PCR[18] =  (PORTB->PCR[18] & (uint32_t)~PORT_PCR_MUX_MASK ) | PORT_PCR_MUX(0x01);
  PORTB->PCR[19] =  (PORTB->PCR[19] & (uint32_t)~PORT_PCR_MUX_MASK ) | PORT_PCR_MUX(0x01);
  PORTA->PCR[19] =  (PORTA->PCR[19] & (uint32_t)~PORT_PCR_MUX_MASK ) | PORT_PCR_MUX(0x01);

  PTA->PDDR |= 1UL << 4 | 1UL << 19;		//	выходы
  PTB->PDDR |= 1UL << 18 | 1UL << 19;		//	выходы
  return true;
}

bool led_send_bit( enum SPI_DEVICES_LIST device_index, unsigned char bit_index, bool value){
  if( bit_index > 8)
    return false;
  SPI0->CTAR[0] = SPI0->CTAR[0] & ~SPI_CTAR_CPHA_MASK;  //  изменение данных по спаду, захват по фронту
  return spi_send_bit(device_index, bit_index, value);
}

void led_send_byte(unsigned char mask, unsigned char byte){
  SPI0->CTAR[0] = SPI0->CTAR[0] & ~SPI_CTAR_CPHA_MASK;  //  изменение данных по спаду, захват по фронту
  spi_send_byte(mask, byte);
}

void led_test(void){
  unsigned char i = 0;
  for(i = 0; i < 8; i++){
    led_send_bit(SDL_LED1, i, true);
    os_dly_wait(200);
    led_send_bit(SDL_LED1, i, false);
    os_dly_wait(200);
  }
  for(i = 0; i < 8; i++){
    led_send_bit(SDL_LED2, i, true);
    os_dly_wait(200);
    led_send_bit(SDL_LED2, i, false);
    os_dly_wait(200);  
  }
  for(i = 0; i < 8; i++){
    led_send_bit(SDL_LED3, i, true);
    os_dly_wait(200);
    led_send_bit(SDL_LED3, i, false);
    os_dly_wait(200);
  }
}
