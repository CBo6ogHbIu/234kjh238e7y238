/*-----------------------------------------------------------------------------
 *      Имя:    			lcd.c
 *			Автор: 				Бажаев А.Е.
 *      Назначение: 	контроллер LCD
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2014 НТК "Бреслер", ООО. Все права защищены
 *----------------------------------------------------------------------------*/

#include "lcd.h"
#include <MK70F15.H>
#include "../system/system.h"
#include <string.h>
#include <rtl.h>
#include <stdio.h>

#ifdef __UTF8
        //               А    Б     В      Г     Д     E     Ё     Ж     З     И     Й
char cg_rus_big[33] = {0x41, 0xa0, 0x42, 0xa1, 0xe0, 0x45, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
        //               К    Л     М      Н     О     П     Р     С     Т     У     Ф
                        0x4b, 0xa7, 0x4d, 0x48, 0x4f, 0xa8, 0x50, 0x43, 0x54, 0xa9, 0xaa,
        //               Х    Ц     Ч      Ш     Щ     Ъ     Ы     Ь     Э     Ю     Я
                        0x58, 0xe1, 0xab, 0xac, 0xe2, 0xad, 0xae, 0x62, 0xaf, 0xb0, 0xb1};

short utf8_big[33] = {0xd090, 0xd091, 0xd092, 0xd093, 0xd094, 0xd095, 0xd081, 0xd096, 0xd097, 0xd098, 0xd099,
                      0xd09a, 0xd09b, 0xd09c, 0xd09d, 0xd09e, 0xd09f, 0xd0a0, 0xd0a1, 0xd0a2, 0xd0a3, 0xd0a4,
                      0xd0a5, 0xd0a6, 0xd0a7, 0xd0a8, 0xd0a9, 0xd0aa, 0xd0ab, 0xd0ac, 0xd0ad, 0xd0ae, 0xd0af};

          //               а     б     в     г     д     е     ё     ж     з     и     й
char cg_rus_small[33] = { 0x61, 0xb2, 0xb3, 0xb4, 0xe3, 0x65, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9,
          //               к     л     м     н     о     п     р     с     т     у     ф
                          0xba, 0xbb, 0xbc, 0xbd, 0x6f, 0xbe, 0x70, 0x63, 0xbf, 0x79, 0xe4,
          //               х     ц     ч     ш     щ     ъ     ы     ь     э     ю     я
                          0x78, 0xe5, 0xc0, 0xc1, 0xe6, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7};

short utf8_small[33] = {0xd0b0, 0xd0b1, 0xd0b2, 0xd0b3, 0xd0b4, 0xd0b5, 0xd191, 0xd0b6, 0xd0b7, 0xd0b8, 0xd0b9,
                        0xd0ba, 0xd0bb, 0xd0bc, 0xd0bd, 0xd0be, 0xd0bf, 0xd180, 0xd181, 0xd182, 0xd183, 0xd184,
                        0xd185, 0xd186, 0xd187, 0xd188, 0xd189, 0xd18a, 0xd18b, 0xd18c, 0xd18d, 0xd18e, 0xd18f};
#endif

OS_TID lcd_tid = 0;

#define LCD_RES_ON()  PTE->PSOR |= 1 << 4
#define LCD_RES_OFF() PTE->PCOR |= 1 << 4
#define LCD_RES_SET(x) x ? LCD_RES_ON() : LCD_RES_OFF()

#define LCD_ENA_ON()  PTE->PSOR |= 1 << 5
#define LCD_ENA_OFF() PTE->PCOR |= 1 << 5

#define LCD_LED_ON()  PTE->PCOR |= 1 << 6
#define LCD_LED_OFF() PTE->PSOR |= 1 << 6

enum lcd_register{LCDR_INSTRUCTION, LCDR_DATA};

void lcd_write(enum lcd_register reg, char data){  //  >=300ns
  //  данные/команда
  if(reg == LCDR_DATA)
    LCD_RES_ON();
  if(reg == LCDR_INSTRUCTION)
    LCD_RES_OFF();

  LCD_ENA_ON();
  
  PTE->PSOR |= (0x0f & data);
  PTE->PCOR |= (0x0f & ~data);
  
  //__triple_nop();       //  tsp2(>=80ns)=>triple_nop() = ~100ns
  
  LCD_ENA_OFF();
}

void lcd_write_command(char data){
  lcd_write(LCDR_INSTRUCTION, data);
}

unsigned char get_russian_cg_code(short code){

  if( code >= utf8_big[0] && code <= utf8_big[5])         //  А-Е
    return cg_rus_big[code - utf8_big[0]];

  if( code >= utf8_big[7] && code <= utf8_big[32])        //  Ж-Я
    return cg_rus_big[7 + code - utf8_big[7]];

  if( code == utf8_big[6])                                //  Ё
    return cg_rus_big[6];

  if( code >= utf8_small[0] && code <= utf8_small[5] )    //  а - е
    return cg_rus_small[code - utf8_small[0]];

  if( code == utf8_small[6] )                             //  ё
    return cg_rus_small[6];

  if( code >= utf8_small[7] && code <= utf8_small[16] )   //  ж - п
    return cg_rus_small[7 + code - utf8_small[7]];  

  if( code >= utf8_small[17] && code <= utf8_small[32] )  //  р - я
    return cg_rus_small[17 + code - utf8_small[17]];

  return (unsigned char)code;
}

void lcd_cmd_function_set(void){
  lcd_write_command(0x02);
  lcd_write_command(0x02);
  lcd_write_command(0x0A);
  os_dly_wait(1);       //  38us=>минимальная задержка 1ms=>1ms
}
  
void lcd_cmd_display(bool display, bool cursor, bool blink){
  char cmd = 0x08;
  lcd_write_command(0x00);
  if(display)
    cmd |= 0x04;
  if(cursor)
    cmd |= 0x02;
  if(blink)
    cmd |= 0x01;
  lcd_write_command(cmd);
  os_dly_wait(1);       //  38us=>минимальная задержка 1ms=>1ms
}

void lcd_on(void){
  lcd_cmd_display(true, true, true);
}

void lcd_off(void){
  lcd_cmd_display(false, false, false);
}
  
void lcd_cmd_shift(void){
  lcd_write_command(0x01);
  lcd_write_command(0x0c);
  lcd_write_command(0x01);
  lcd_write_command(0x07);  
  os_dly_wait(1);       //  38us=>минимальная задержка 1ms=>1ms
}

void lcd_cmd_clear(void){
  lcd_write_command(0x00);
  lcd_write_command(0x01);
  os_dly_wait(7);       //  6.2ms=>7ms
}

void lcd_cmd_entry_mode(void){
  lcd_write_command(0x00);
  lcd_write_command(0x06);
  os_dly_wait(1);       //  38us=>минимальная задержка 1ms=>1ms
}

void lcd_cmd_return_home(void){
  lcd_write_command(0x00);
  lcd_write_command(0x02);
  os_dly_wait(1);       //  38us=>минимальная задержка 1ms=>1ms
}

void lcd_cmd_set_cgram_address(char address){
  //  старшие 4 бита
  lcd_write_command( (0x30 & address | 0x40) >> 4 );
  //  младшие 4 бита
  lcd_write_command( address );
  os_dly_wait(1);       //  38us=>минимальная задержка 1ms=>1ms
}

void lcd_cmd_set_ddram_address(char address){
  //  старшие 4 бита
  lcd_write_command( ( address | 0x80 ) >> 4 );
  //  старшие 4 бита
  lcd_write_command( address );
  os_dly_wait(1);       //  38us=>минимальная задержка 1ms=>1ms
}

bool lcd_outtextXY(unsigned char x, unsigned char y, const char *  data){
  unsigned char length = 0, i = 0, byte = 0, address = 0;
  short word = 0;
  bool utf8_rus = false;
  if(data == NULL || x > 19 || y > 3)
    return false;
  else
    length = strlen(data);
  
  length = length > 20 - x ? length = 20 - x : length;  //  обрезать строку, если нужно
  
  switch(y){
    case(0): address = x; break;
    case(1): address = 64 + x; break;
    case(2): address = 20 + x; break;
    case(3): address = 84 + x; break;
  };
  
  lcd_cmd_set_ddram_address(address);

  for(i = 0; i < length; i++){
    byte = data[i];
    utf8_rus = byte == 0xd0 || byte == 0xd1;
    if(utf8_rus){
      word = (uint16_t)(byte << 8) | (uint16_t)data[i + 1];
      byte = get_russian_cg_code(word);
      if(byte != data[i]) //  если произошла замена символа
        i++;
    }
    //  старшие 4 бита
    lcd_write(LCDR_DATA, byte >> 4);  
    //  младшие 4 бита
    lcd_write(LCDR_DATA, byte);
  }
  return true;
}


bool lcd_cg_symbolXY(unsigned char x, unsigned char y, unsigned char symbol){
  unsigned char address = 0;
  if(x > 19 || y > 3)
    return false;
  
  switch(y){
    case(0): address = x; break;
    case(1): address = 64 + x; break;
    case(2): address = 20 + x; break;
    case(3): address = 84 + x; break;
  };

  lcd_cmd_set_ddram_address(address);
  //  старшие 4 бита
  lcd_write(LCDR_DATA, symbol >> 4);  
  //  младшие 4 бита
  lcd_write(LCDR_DATA, symbol);
  return true;
}

bool lcd_send_buffer(const char *  buffer){
  if(buffer == NULL || strlen(buffer) != 80)
    return false;
  return (  
            lcd_outtextXY(0, 0, &buffer[0])   &&
            lcd_outtextXY(0, 1, &buffer[20])  &&
            lcd_outtextXY(0, 2, &buffer[40])  &&
            lcd_outtextXY(0, 3, &buffer[60])  
          );
}

void lcd_clear(void){
  char buffer[81]={0x00}, i = 0;
  for(i = 0; i < 80; i++)
    buffer[i]=0xff;
  lcd_send_buffer(buffer);
}

void lcd_test(void){

}

bool lcd_initialize(void)
{
  
  //  порт PTE, 0-6
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  //  GPIO, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7, LCD_RES, LCD_ENA, LCD_LED
  PORTE->PCR[0] = (uint32_t)((PORTE->PCR[0] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PORTE->PCR[1] = (uint32_t)((PORTE->PCR[1] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PORTE->PCR[2] = (uint32_t)((PORTE->PCR[2] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PORTE->PCR[3] = (uint32_t)((PORTE->PCR[3] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PORTE->PCR[4] = (uint32_t)((PORTE->PCR[4] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  PORTE->PCR[5] = (uint32_t)((PORTE->PCR[5] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  //  цепь управления подсветкой не оказывает на lcd никакого влияния, не использовать
  PORTE->PCR[6] = (uint32_t)((PORTE->PCR[6] & (uint32_t)~(uint32_t)( PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x05))) | (uint32_t)( PORT_PCR_MUX(0x01) ));
  //	выходы
  PTE->PDDR |= 0x7f;

  lcd_cmd_function_set();
  lcd_cmd_display(true, true, true);
  //lcd_cmd_shift();
  lcd_cmd_clear();
  lcd_cmd_entry_mode();
  
  lcd_cmd_return_home();

  lcd_clear();
  return true;
}
