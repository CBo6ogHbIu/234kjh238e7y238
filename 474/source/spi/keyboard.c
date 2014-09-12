/*-----------------------------------------------------------------------------
 *      Имя:    			keyboard.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	модуль клавиатуры
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
#include "keyboard.h"
#include <MK70F15.H>
#include "../system/system.h"
#include <rtl.h>

bool keyb_initialize(void)
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

bool keyb_get_bit( enum SPI_DEVICES_LIST device_index, unsigned char bit_index, bool value){
  if( bit_index > 8)
    return false;
  SPI0->CTAR[0] = SPI0->CTAR[0] & ~SPI_CTAR_CPHA_MASK;  //  изменение данных по спаду, захват по фронту
  return spi_send_bit(device_index, bit_index, value);
}

void keyb_get_byte(unsigned char mask, unsigned char byte){
  SPI0->CTAR[0] = SPI0->CTAR[0] & ~SPI_CTAR_CPHA_MASK;  //  изменение данных по спаду, захват по фронту
  spi_send_byte(mask, byte);
}

void keyb_test(void){
}
