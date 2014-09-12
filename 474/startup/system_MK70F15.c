/*
** ###################################################################
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    K70P256M150SF3RM, Rev. 2, Dec 2011
**     Version:             rev. 1.5, 2012-10-19
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright: 2012 Freescale, Inc. All Rights Reserved.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2011-08-24)
**         Initial version
**     - rev. 1.1 (2011-11-03)
**         Registers updated according to the new reference manual revision - Rev. 1, Oct 2011
**         Registers of the following modules have been updated - CAN, DDR, I2S, LCDC, MCG, MPU, NFC, RCM, RTC, SDHC, SIM, USBHS, WDOG
**     - rev. 1.2 (2012-01-04)
**         Registers updated according to the new reference manual revision - Rev. 2, Dec 2011
**         EWM - INTEN bit in EWM_CTRL register has been added.
**         PDB - register PDB_PO0EN renamed to PRB_POEN.
**         PMC - BGEN bit in PMC_REGSC register has been removed.
**         SIM - several changes in SCGC registers. Bit USBHS in SOPT2 register removed.
**         UART - new bits RXOFE in regiter CFIFO and RXOF in register SFIFO.
**         DRY - bit THYD in CR register renamed to THYS. Bit group KSL in LR register removed.
**         LCDC - bits GWLPM in LSR register, ERR_RES_EN, GW_ERR_RES_EN in LIER and ERR_RES, GW_ERR_RES in LISR removed.
**     - rev. 1.3 (2012-04-13)
**         Added new #define symbol MCU_MEM_MAP_VERSION_MINOR.
**         Added new #define symbols <peripheralType>_BASE_PTRS.
**     - rev. 1.4 (2012-09-17)
**         DRY module removed.
**     - rev. 1.5 (2012-10-19)
**         RTC - security related registers removed.
**         Interrupt Number Definitions updated - HardFault_IRQn has been added.
**
** ###################################################################
*/

/**
 * @file MK70F15
 * @version 1.5
 * @date 2012-10-19
 * @brief Device specific configuration file for MK70F15 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <MK70F15.h>
#define DISABLE_WDOG    1

/* Predefined clock setups
   0 ... Multipurpose Clock Generator (MCG) in PEE Engaged External (PEE) mode
         Reference clock source for MCG module is the external clock source 12 MHz
         Core clock = 150 MHz, BusClock = 75 MHz
*/

/*----------------------------------------------------------------------------
  Define clock source values
 *----------------------------------------------------------------------------*/
	#define CPU_XTAL0_CLK_HZ                0u 				 /* Value of the external crystal or oscillator clock frequency in Hz connected to System Oscillator 0 */
	#define CPU_XTAL1_CLK_HZ                12000000u  /* Value of the external crystal or oscillator clock frequency in Hz connected to System Oscillator 1 */
	#define CPU_XTAL32k_CLK_HZ              32768u     /* Value of the external 32k crystal or oscillator clock frequency in Hz */
	#define CPU_INT_SLOW_CLK_HZ             32768u     /* Value of the slow internal oscillator clock frequency in Hz  */
	#define CPU_INT_FAST_CLK_HZ             4000000u   /* Value of the fast internal oscillator clock frequency in Hz  */
	#define DEFAULT_SYSTEM_CLOCK            150000000u /* Default System clock value 150 MHz*/

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

void PEE_mode(void)
{

  /* SIM_SCGC6: RTC=1 */
  SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;                                   
  if ((RTC->CR & RTC_CR_OSCE_MASK) == 0u) { /* Only if the OSCILLATOR is not already enabled */
    /* RTC_CR: SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
    RTC->CR &= (uint32_t)~(uint32_t)(
               RTC_CR_SC2P_MASK |
               RTC_CR_SC4P_MASK |
               RTC_CR_SC8P_MASK |
               RTC_CR_SC16P_MASK
              );                                   
    /* RTC_CR: OSCE=1 */
    RTC->CR |= RTC_CR_OSCE_MASK;                                   
    /* RTC_CR: CLKO=0 */
    RTC->CR &= (uint32_t)~(uint32_t)(RTC_CR_CLKO_MASK);                                   
  }
  /* Disable the WDOG module */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* Key 1 */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,??=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = WDOG_STCTRLH_BYTESEL(0x00) |
                 WDOG_STCTRLH_WAITEN_MASK |
                 WDOG_STCTRLH_STOPEN_MASK |
                 WDOG_STCTRLH_ALLOWUPDATE_MASK |
                 WDOG_STCTRLH_CLKSRC_MASK |
                 0x0100U;       

  /* System clock initialization */
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=3,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                SIM_CLKDIV1_OUTDIV2(0x01) |
                SIM_CLKDIV1_OUTDIV3(0x03) |
                SIM_CLKDIV1_OUTDIV4(0x03); /* Set the system prescalers to safe value */
  /* SIM_SCGC5: PORTE=1,PORTD=1,PORTC=1,PORTA=1 */
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK |
               SIM_SCGC5_PORTD_MASK |
               SIM_SCGC5_PORTC_MASK |
               SIM_SCGC5_PORTA_MASK;   /* Enable clock gate for ports to enable pin routing */
  if ((PMC->REGSC & PMC_REGSC_ACKISO_MASK) != 0x0U) {
    /* PMC_REGSC: ACKISO=1 */
    PMC->REGSC |= PMC_REGSC_ACKISO_MASK; /* Release IO pads after wakeup from VLLS mode. */
  }
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=2,OUTDIV3=2,OUTDIV4=0x0E,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                SIM_CLKDIV1_OUTDIV2(0x02) |
                SIM_CLKDIV1_OUTDIV3(0x02) |
                SIM_CLKDIV1_OUTDIV4(0x0E); /* Update system prescalers */
  /* SIM_SOPT2: PLLFLLSEL=1 */
  SIM->SOPT2 = (uint32_t)((SIM->SOPT2 & (uint32_t)~(uint32_t)(
               SIM_SOPT2_PLLFLLSEL(0x02)
              )) | (uint32_t)(
               SIM_SOPT2_PLLFLLSEL(0x01)
              ));                      /* Select PLL 0 as a clock source for various peripherals */
  /* SIM_SOPT1: OSC32KSEL=0 */
  SIM->SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL_MASK); /* System oscillator drives 32 kHz clock for various peripherals */
  /* SIM_SCGC1: OSC1=1 */
  SIM->SCGC1 |= SIM_SCGC1_OSC1_MASK;                                   
  /* PORTE_PCR24: ISF=0,MUX=0 */
  PORTE->PCR[24] &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));                                   
  /* PORTE_PCR25: ISF=0,MUX=0 */
  PORTE->PCR[25] &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));                                   
  /* Switch to FBE Mode */
  /* MCG_C7: OSCSEL=1 */
  MCG->C7 |= MCG_C7_OSCSEL_MASK;                                   
  /* MCG_C10: LOCRE2=0,??=0,RANGE1=2,HGO1=0,EREFS1=1,??=0,??=0 */
  MCG->C10 = (MCG_C10_RANGE1(0x02) | MCG_C10_EREFS1_MASK);                                   
  /* MCG_C2: LOCRE0=0,??=0,RANGE0=0,HGO0=0,EREFS0=0,LP=0,IRCS=0 */
  MCG->C2 = MCG_C2_RANGE0(0x00);                                   
  /* OSC0_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0->CR = OSC_CR_ERCLKEN_MASK;                                   
  /* OSC1_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC1->CR = OSC_CR_ERCLKEN_MASK;                                   
  /* MCG_C1: CLKS=2,FRDIV=0,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x00) | MCG_C1_IRCLKEN_MASK);                                   
  /* MCG_C4: DMX32=0,DRST_DRS=0 */
  MCG->C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));                                   
  /* MCG_C5: PLLREFSEL0=1,PLLCLKEN0=0,PLLSTEN0=0,??=0,??=0,PRDIV0=0 */
  MCG->C5 = (MCG_C5_PLLREFSEL0_MASK | MCG_C5_PRDIV0(0x00));                                   
  /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=9 */
  MCG->C6 = MCG_C6_VDIV0(0x09);                                   
  /* MCG_C11: PLLREFSEL1=0,PLLCLKEN1=0,PLLSTEN1=0,PLLCS=0,??=0,PRDIV1=0 */
  MCG->C11 = MCG_C11_PRDIV1(0x00);                                   
  /* MCG_C12: LOLIE1=0,??=0,CME2=0,VDIV1=0 */
  MCG->C12 = MCG_C12_VDIV1(0x00);                                   
  while((MCG->S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((MCG->S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to PBE Mode */
  /* MCG_C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=9 */
  MCG->C6 = (MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0x09));                                   
  while((MCG->S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
  }
  while((MCG->S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until PLL locked */
  }
  /* Switch to PEE Mode */
  /* MCG_C1: CLKS=0,FRDIV=0,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG->C1 = (MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x00) | MCG_C1_IRCLKEN_MASK);                                   
  while((MCG->S & 0x0CU) != 0x0CU) {    /* Wait until output of the PLL is selected */
  }
}

void SystemInit (void) {
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    /* set CP10, CP11 Full Access */
#endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

#if (DISABLE_WDOG)
  /* Disable the WDOG module */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG->UNLOCK = (uint16_t)0xC520u;     /* Key 1 */
  /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
  WDOG->UNLOCK  = (uint16_t)0xD928u;    /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG->STCTRLH = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */

  /* System clock initialization */
	PEE_mode();
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {
  uint32_t MCGOUTClock;                                                        /* Variable to store output clock frequency of the MCG module */
  uint8_t Divider;

  if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) {
    /* Output of FLL or PLL is selected */
    if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) {
      /* FLL is selected */
      if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) {
        /* External reference clock is selected */
        if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
          MCGOUTClock = CPU_XTAL0_CLK_HZ;                                       /* System oscillator 0 drives MCG clock */
        } else { /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
          MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                    /* RTC 32 kHz oscillator drives MCG clock */
        } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
        Divider = (uint8_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
        MCGOUTClock = (MCGOUTClock / Divider);  /* Calculate the divided FLL reference clock */
        if ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) {
          MCGOUTClock /= 32u;                                                  /* If high range is enabled, additional 32 divider is active */
        } /* ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) */
      } else { /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
        MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                     /* The slow internal reference clock is selected */
      } /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
      /* Select correct multiplier to calculate the MCG output clock  */
      switch (MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) {
        case 0x0u:
          MCGOUTClock *= 640u;
          break;
        case 0x20u:
          MCGOUTClock *= 1280u;
          break;
        case 0x40u:
          MCGOUTClock *= 1920u;
          break;
        case 0x60u:
          MCGOUTClock *= 2560u;
          break;
        case 0x80u:
          MCGOUTClock *= 732u;
          break;
        case 0xA0u:
          MCGOUTClock *= 1464u;
          break;
        case 0xC0u:
          MCGOUTClock *= 2197u;
          break;
        case 0xE0u:
          MCGOUTClock *= 2929u;
          break;
        default:
          break;
      }
    } else { /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
      /* PLL is selected */
      if ((MCG->C11 & MCG_C11_PLLCS_MASK) != 0x0u) {
        /* PLL1 output is selected */
        if ((MCG->C11 & MCG_C11_PLLREFSEL1_MASK) != 0x0u) {
          /* OSC1 clock source used as an external reference clock */
          MCGOUTClock = CPU_XTAL1_CLK_HZ;
        } else { /* (!((MCG->C11 & MCG_C11_PLLREFSEL1_MASK) != 0x0u)) */
          /* OSC0 clock source used as an external reference clock */
          MCGOUTClock = CPU_XTAL0_CLK_HZ;
        } /* (!((MCG->C11 & MCG_C11_PLLREFSEL1_MASK) != 0x0u)) */
        Divider = (1u + (MCG->C11 & MCG_C11_PRDIV1_MASK));
        MCGOUTClock /= Divider;                                                  /* Calculate the PLL reference clock */
        Divider = ((MCG->C12 & MCG_C12_VDIV1_MASK) + 16u);
        MCGOUTClock = (MCGOUTClock * Divider) / 2u;                              /* Calculate the MCG output clock */
      } else { /* (!((MCG->C11 & MCG_C11_PLLCS_MASK) != 0x0u)) */
        /* PLL0 output is selected */
        if ((MCG->C5 & MCG_C5_PLLREFSEL0_MASK) != 0x0u) {
          /* OSC1 clock source used as an external reference clock */
          MCGOUTClock = CPU_XTAL1_CLK_HZ;
        } else { /* (!((MCG->C5 & MCG_C5_PLLREFSEL0_MASK) != 0x0u)) */
          /* OSC0 clock source used as an external reference clock */
          MCGOUTClock = CPU_XTAL0_CLK_HZ;
        } /* (!((MCG->C5 & MCG_C5_PLLREFSEL0_MASK) != 0x0u)) */
        Divider = (1u + (MCG->C5 & MCG_C5_PRDIV0_MASK));
        MCGOUTClock /= Divider;                                                  /* Calculate the PLL reference clock */
        Divider = ((MCG->C6 & MCG_C6_VDIV0_MASK) + 16u);
        MCGOUTClock = (MCGOUTClock * Divider) / 2u;                              /* Calculate the MCG output clock */
      } /* (!((MCG->C11 & MCG_C11_PLLCS_MASK) != 0x0u)) */
    } /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x40u) {
    /* Internal reference clock is selected */
    if ((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u) {
      MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                       /* Slow internal reference clock selected */
    } else { /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
      MCGOUTClock = CPU_INT_FAST_CLK_HZ / (1 << ((MCG->SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT));  /* Fast internal reference clock selected */
    } /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u) {
    /* External reference clock is selected */
    if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
      MCGOUTClock = CPU_XTAL0_CLK_HZ;                                           /* System oscillator drives MCG clock */
    } else { /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
      MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                        /* RTC 32 kHz oscillator drives MCG clock */
    } /* (!((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u)) */
  } else { /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
    /* Reserved value */
    return;
  } /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
  SystemCoreClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)));
}
