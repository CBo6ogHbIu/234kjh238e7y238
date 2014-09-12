/*-----------------------------------------------------------------------------
 *      Имя:    			relay.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	управление реле
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
 
#include "relay.h"
#include <MK70F15.H>
#include "../system/system.h"
#include <rtl.h>

OS_TID relay_tid = 0, wdcl_tid = 0;
unsigned char relay_enabled = 0x00;

bool relay_send_byte( unsigned char byte ){
  if( wdcl_tid == 0 )
    return false;
  SPI0->CTAR[0] = SPI0->CTAR[0] |= SPI_CTAR_CPHA_MASK;  //  изменение данных по фронту, захват по спаду
  spi_send_byte(SPI_RELAY, byte);
  return true;
}

bool relay_send_bit( unsigned char bit_index, bool value ){
  if( bit_index > 8 || wdcl_tid == 0 )
    return false;
  SPI0->CTAR[0] = SPI0->CTAR[0] |= SPI_CTAR_CPHA_MASK;  //  изменение данных по фронту, захват по спаду
  return spi_send_bit(SDL_RELAY, bit_index, value);
}

void relay_test(void){
  unsigned char i = 0;

  if(wdcl_tid == 0)
    return;

  for(i = 0; i < 8; i++){
    relay_send_bit(i, true);
    os_dly_wait(1000);
    relay_send_bit(i, false);
    os_dly_wait(1000);
  }
}

__task void wdcl_run(void){
	while(1){
    PTB->PCOR |= 1UL << 17;
    os_dly_wait(20);
    PTB->PSOR |= 1UL << 17;
    os_dly_wait(10);
  }
  os_tsk_delete_self();
}

bool relay_initialize(void)
{
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
  //  wdcl
  PORTB->PCR[17] =  (PORTB->PCR[17] & (uint32_t)~PORT_PCR_MUX_MASK ) | PORT_PCR_MUX(0x01);
  PTB->PDDR |= 1UL << 17;		//	выход
  //  spi CS0
  PORTD->PCR[0] = (uint32_t)((PORTD->PCR[0] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05) )) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PTD->PDDR |= 1UL;
  return ( ( wdcl_tid = os_tsk_create(wdcl_run, PRIORITY_RELAY)) != 0 );
}
