/*------------------------------------------------------------------------------
 * Name:    SER.c
 * Purpose: Serial I/O functions for the 
 *          Freescale Kinetis TWR-K70F120M board
 * Note(s): 
 *------------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2012 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "MK70F15.H"
#include "SER.h"

#ifdef __DBG_ITM
volatile int32_t ITM_RxBuffer;
#endif


/*------------------------------------------------------------------------------
 * SER_Init:        Initialize Serial Interface
 *----------------------------------------------------------------------------*/

void SER_Init (void) {

#ifdef __DBG_ITM
  ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
#else
  /* Configure UART                                                           */
  /* Settings: 115200 baud @ 50MHz, 8 data bits, 1 stop bit, no flow control  */
  SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;   /* Enable UART 2 gate clocking        */
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;   /* Enable PORTE gate clocking         */ 

  PORTE->PCR[17] = (3UL <<  8);         /* Pin mux configured as ALT3         */
  PORTE->PCR[16] = (3UL <<  8);         /* Pin mux configured as ALT3         */

  UART2->BDH =  0;
  UART2->BDL = 27;                      /* SBR = 27                           */

  UART2->C1  =  0; 
  UART2->C4  =  4;                      /* BRFA = 4                           */

  UART2->C2  =  UART_C2_RE_MASK |       /* Receiver enable                    */
                UART_C2_TE_MASK;        /* Transmitter enable                 */
  UART2->S2  =  0;
  UART2->C3  =  0;

  UART2->RWFIFO = UART_RWFIFO_RXWATER(1);
  UART2->TWFIFO = UART_TWFIFO_TXWATER(0);
#endif
}


/*------------------------------------------------------------------------------
 * SER_Uninit:      Uninitialize Serial Interface
 *----------------------------------------------------------------------------*/

void SER_Uninit (void) {

#ifndef __DBG_ITM
  UART2->C2  = 0;
  UART2->C4  = 0;
  UART2->BDL = 4;

  PORTE->PCR[17] = 0;
  PORTE->PCR[16] = 0;

  SIM->SCGC4 &= ~SIM_SCGC4_UART2_MASK;
#endif
}


/*------------------------------------------------------------------------------
 * SER_PutChar:     Write a character to the Serial Port
 *----------------------------------------------------------------------------*/

int32_t SER_PutChar (int32_t ch) {

#ifdef __DBG_ITM
  int i;
  ITM_SendChar (ch & 0xFF);
  for (i = 10000; i; i--);
#else
  while (!(UART2->S1 & UART_S1_TDRE_MASK));
  UART2->D = (ch & 0xFF);
#endif
  return (ch);
}


/*------------------------------------------------------------------------------
 * SER_GetChar:     Read a character from the Serial Port (non-blocking)
 *----------------------------------------------------------------------------*/

int32_t SER_GetChar (void) {

#ifdef __DBG_ITM
  if (ITM_CheckChar())
    return (ITM_ReceiveChar());
#else
  if (UART2->S1 & UART_S1_RDRF_MASK) 
    return (UART2->D);
#endif
  return (-1);
}


/*------------------------------------------------------------------------------
 * End of file
 *----------------------------------------------------------------------------*/
