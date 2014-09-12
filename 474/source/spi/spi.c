/*-----------------------------------------------------------------------------
 *      Имя:    			spi.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	драйвер SPI. к нему подключаются модули реле, диодов, дискретных входов и клавиатуры
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/
#include "spi.h"
#include "relay.h"
#include "led.h"
#include <MK70F15.H>
#include "../system/system.h"
#include <rtl.h>

OS_TID spi_tid = 0;

unsigned char recievers = 0x00;

unsigned char spi_device_status[SDL_COUNT] = {0x00,     0x00,     0x00,     0x00,     0x00,     0x00    };
unsigned char spi_device_mask  [SDL_COUNT] = {0x01,     0x02,     0x04,     0x08,     0x10,     0x20    };
unsigned int  spi_port_mask    [SDL_COUNT] = {1u << 4,  1u << 18, 1u << 19, 1u << 0,  1u << 3,  1u << 2 };
GPIO_Type *   spi_ports        [SDL_COUNT] = {PTA,      PTB,      PTB,      PTD,      PTC,      PTC     };

//  управление собственно записью/чтением ведется здесь
//  после отправки данных выбирается получатель
void SPI0_IRQHandler(void){
  char i = 0, rcvd_data = 0x00;
  if(SPI_SR_TCF_MASK){              //  отправка

    for(i = 0; i < SDL_COUNT; i++)
      if( ( recievers & spi_device_mask[i] ) != 0x00 )
        spi_ports[i]->PSOR |= spi_port_mask[i];
    
    __triple_nop();                 //  выдержать паузу 100ns

    //  для маскированных диодов снять сигнал защелкивания
    for(i = 0; i < SDL_RELAY; i++)
      if( ( recievers & spi_device_mask[i] ) != 0x00 )
        spi_ports[i]->PCOR |= spi_port_mask[i];
    
    recievers = 0x00;               //  сброс маски отправки
    SPI0->SR |= SPI_SR_TCF_MASK;    //  сброс прерывания
  }

  if(SPI_SR_RFDF_MASK){             //  прием
    rcvd_data = SPI0->POPR;
    SPI0->SR |= SPI_SR_RFDF_MASK;   //  сброс прерывания
  }
}

/**  маскирование spi. 
  можно выбрать несколько линий, воздействие будет общим.
  битовая маска(байт):  
  xxxx  xxxx  keyb  dinp  relay led3  led2  led1
   0     0     0     0     0     0     0     0 */
void spi_set_mask(unsigned char mask, bool enable){
  unsigned char i = SDL_RELAY;
  recievers = enable ? recievers | mask : recievers & ~mask;
  
  //  выставить CS для устройств, которые не являются диодами
  if( (mask & 0x38) != 0)
    for(; i < SDL_COUNT; i++)
      if((recievers & ( 1 << i )) != 0x00)
        spi_ports[i]->PCOR |= spi_port_mask[i];
}

bool spi_send_bit(enum SPI_DEVICES_LIST device_index, unsigned char bit_index, bool value){
  unsigned char device_status = 0;
  if(device_index >= SDL_COUNT)
    return false;
  //  получатель
  spi_set_mask(spi_device_mask[device_index], true);
  //  получить текущее состояние
  device_status = spi_device_status[device_index];
  //  обработать бит
  device_status = value ? 
    device_status | (unsigned char) (1 << bit_index) : 
    device_status & (unsigned char)~(1 << bit_index);
  //  отправить данные
  SPI0->PUSHR = SPI_PUSHR_TXDATA(device_status);
  //  сохранить новое состояние
  spi_device_status[device_index] = device_status;
  
  return true;
}

void spi_send_byte(unsigned char mask, unsigned char byte){
  unsigned char i = 0;
  //  обозначить получателей
  spi_set_mask(mask, true);
  //  отправить данные
  SPI0->PUSHR = SPI_PUSHR_TXDATA(byte);

  //  сохранить состояние
  for(; i < SDL_COUNT; i++)
    if( ( mask & spi_device_mask[i] ) != 0x00 )
      spi_device_status[i] = byte;
}

bool spi_initialize(void)
{
  //  питание и настройка портов
  
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
  //GPIO, MISO, MOSI, CLK
  PORTE->PCR[19] = (uint32_t)((PORTE->PCR[19] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x02) ));
  PORTE->PCR[18] = (uint32_t)((PORTE->PCR[18] & (uint32_t)~(uint32_t)(PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05) )) | (uint32_t)( PORT_PCR_MUX(0x02) ));
  PORTE->PCR[17] = (uint32_t)((PORTE->PCR[17] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05) )) | (uint32_t)( PORT_PCR_MUX(0x02) ));
  PTE->PDDR |= 1UL << 17 | 1UL << 18 | 1UL << 19;									//	выходы
  
  //GPIO, CS1-CS2(дискретные входы, клавиатура)
  PORTC->PCR[3] = (uint32_t)((PORTC->PCR[3] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05) )) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PORTC->PCR[2] = (uint32_t)((PORTC->PCR[2] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05) )) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PTC->PDDR |= 1UL << 2 | 1UL << 3;		//	выходы
  
  relay_initialize();
  led_initialize();

  //  настройка spi
  
  /* SIM_SCGC6: DSPI0=1 */
  SIM->SCGC6 |= SIM_SCGC6_DSPI0_MASK;                                   
  /* SPI0_MCR: MSTR=0,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=0,??=0,??=0,PCSIS=0,DOZE=0,MDIS=0,DIS_TXF=0,DIS_RXF=0,CLR_TXF=0,CLR_RXF=0,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
  SPI0->MCR = SPI_MCR_DCONF(0x00) |
             SPI_MCR_PCSIS(0x00) |
             SPI_MCR_SMPL_PT(0x00) |
             SPI_MCR_HALT_MASK;       
  /* SPI0_MCR: MSTR=0,CONT_SCKE=0,DCONF=0,FRZ=0,MTFE=0,PCSSE=0,ROOE=0,??=0,??=0,PCSIS=1,DOZE=0,MDIS=0,DIS_TXF=0,DIS_RXF=0,CLR_TXF=1,CLR_RXF=1,SMPL_PT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,HALT=1 */
  SPI0->MCR = SPI_MCR_DCONF(0x00) |
             SPI_MCR_MSTR_MASK |
             SPI_MCR_PCSIS(0x00) |
             //SPI_MCR_DIS_RXF_MASK | //  включить RX FIFO
             SPI_MCR_DIS_TXF_MASK | //  отключить TX FIFO
             SPI_MCR_CLR_RXF_MASK |
             SPI_MCR_CLR_TXF_MASK |
             SPI_MCR_SMPL_PT(0x00) |
             SPI_MCR_HALT_MASK;       
  /* SPI0_TCR: SPI_TCNT=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SPI0->TCR = SPI_TCR_SPI_TCNT(0x00);                                   
  /* SPI0_CTAR0: DBR=1,FMSZ=0x0F,CPOL=0,CPHA=0,LSBFE=0,PCSSCK=0,PASC=0,PDT=0,PBR=0,CSSCK=0,ASC=0,DT=0,BR=0 */
  SPI0->CTAR[0] = /*SPI_CTAR_DBR_MASK |*/
               SPI_CTAR_FMSZ(0x07) |
               SPI_CTAR_PCSSCK(0x00) |
               SPI_CTAR_PASC(0x00) |
               SPI_CTAR_PDT(0x00) |
               SPI_CTAR_PBR(0x02) |       // fsys/5
               SPI_CTAR_CSSCK(0x00) |
               SPI_CTAR_ASC(0x00) |
               SPI_CTAR_DT(0x00) |
               SPI_CTAR_BR(0x03);         //  fsys/8 => около 1.25 МГц. Потом попробовать поднять до 59МГц и использовать двойной бодрейт
               
  /* SPI0_CTAR1: DBR=1,FMSZ=0x0F,CPOL=0,CPHA=0,LSBFE=0,PCSSCK=0,PASC=0,PDT=0,PBR=0,CSSCK=0,ASC=0,DT=0,BR=0 */
  SPI0->CTAR[1] = /*SPI_CTAR_DBR_MASK |*/
               SPI_CTAR_FMSZ(0x0F) |
               SPI_CTAR_PCSSCK(0x00) |
               SPI_CTAR_PASC(0x00) |
               SPI_CTAR_PDT(0x00) |
               SPI_CTAR_PBR(0x00) |
               SPI_CTAR_CSSCK(0x00) |
               SPI_CTAR_ASC(0x00) |
               SPI_CTAR_DT(0x00) |
               SPI_CTAR_BR(0x00);
               
  /* SPI0_SR: TCF=1,TXRXS=1,??=0,EOQF=1,TFUF=1,??=0,TFFF=1,??=0,??=0,??=0,??=0,??=0,RFOF=1,??=0,RFDF=1,??=0,TXCTR=0,TXNXTPTR=0,RXCTR=0,POPNXTPTR=0 */
  SPI0->SR = SPI_SR_TCF_MASK |
            SPI_SR_TXRXS_MASK |
            SPI_SR_EOQF_MASK |
            SPI_SR_TFUF_MASK |
            SPI_SR_TFFF_MASK |
            SPI_SR_RFOF_MASK |
            SPI_SR_RFDF_MASK |
            SPI_SR_TXCTR(0x00) |
            SPI_SR_TXNXTPTR(0x00) |
            SPI_SR_RXCTR(0x00) |
            SPI_SR_POPNXTPTR(0x00);       
  /* SPI0_RSER: TCF_RE=0,??=0,??=0,EOQF_RE=0,TFUF_RE=0,??=0,TFFF_RE=0,TFFF_DIRS=0,??=0,??=0,??=0,??=0,RFOF_RE=0,??=0,RFDF_RE=0,RFDF_DIRS=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SPI0->RSER = SPI_RSER_TCF_RE_MASK | //  флаг передачи
               SPI_RSER_RFDF_RE_MASK; //  флаг приема
               
  /* SPI0_MCR: HALT=0 */
  SPI0->MCR &= (uint32_t)~(uint32_t)(SPI_MCR_HALT_MASK);               
  
  //  прерывания
	NVIC_ClearPendingIRQ(SPI0_IRQn);
	NVIC_SetPriority(SPI0_IRQn, PRIORITY_SPI_IRQ);
	NVIC_EnableIRQ(SPI0_IRQn);

  //  сброс подключенных устройств
  spi_send_byte  ( spi_device_mask[SDL_DINP] | spi_device_mask[SDL_KEYB], 0x00);
  os_dly_wait(10);
  led_send_byte  ( spi_device_mask[SDL_LED1]  | spi_device_mask[SDL_LED2] | spi_device_mask[SDL_LED3], 0x00);
  os_dly_wait(10);
  return relay_send_byte( 0x00 );
}
