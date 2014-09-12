#include "NAND_FLash.h"
#include "MK70F15.h"
#include "Driver_NAND_m.h"
extern NAND_DRV nand0_drv;
const NAND_DRV * const NAND_Membus_Driver = &nand0_drv;

#define CLE_SET()   PTD->PSOR |= 1ul << 8;
#define CLE_RESET() PTD->PCOR |= 1ul << 8;
#define ALE_SET()   PTD->PSOR |= 1ul << 9;
#define ALE_RESET() PTD->PCOR |= 1ul << 9;
#define RE_SET() 		PTD->PCOR |= 1ul << 10;
#define RE_RESET()  PTD->PSOR |= 1ul << 10;

#define WE_SET()   	PTC->PCOR |= 1ul << 11;
#define WE_RESET() 	PTC->PSOR |= 1ul << 11;
#define RB_SET()   	PTC->PSOR |= 1ul << 16;
#define RB_RESET() 	PTC->PCOR |= 1ul << 16;
#define CE_SET() 		PTC->PCOR |= 1ul << 17;
#define CE_RESET()  PTC->PSOR |= 1ul << 17;

void cmd_reset(){
	CE_SET();
	NFC->CMD2 = NFC_CMD2_BYTE1(0xFF);
	NFC->CMD2 |= NFC_CMD2_CODE(0x4002);	
	NFC->ISR |= NFC_ISR_DONEEN_MASK;
	NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;
	CE_RESET();
	return;
}

void cmd_read_id(){
	CE_SET();
	NFC->CMD2 = NFC_CMD2_BYTE1(0x90);
	NFC->CMD2 |= NFC_CMD2_CODE(0x4804);
	NFC->ISR 	|= NFC_ISR_DONEEN_MASK;
	NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;
	CE_RESET();
	return;
}

bool nand_start(void){
//  const ARM_NAND_DEVICE cfg = {
//    
//  };
	return true;
}

void NFC_IRQHandler(void){
	uint8_t b1, b2, b3, b4, b5;
	if(NFC->ISR & NFC_ISR_DONE_MASK){
		NFC->ISR |= NFC_ISR_DONECLR_MASK;
		b1 = NFC->SR1 & NFC_SR1_ID1_MASK;
		b2 = NFC->SR1 & NFC_SR1_ID2_MASK;
		b3 = NFC->SR1 & NFC_SR1_ID3_MASK;
		b4 = NFC->SR1 & NFC_SR1_ID4_MASK;
		b5 = NFC->SR2 & NFC_SR2_ID5_MASK;
	}
}

bool nand_initialize(void){
	SIM->SCGC3 |= SIM_SCGC3_NFC_MASK;			                        //	питание NFC	
    SIM->CLKDIV4 = (uint32_t)(
                (SIM->CLKDIV4 & (uint32_t)~(uint32_t)( SIM_CLKDIV4_NFCDIV(0x11) | SIM_CLKDIV4_NFCFRAC(0x04))) | 
                (uint32_t)( SIM_CLKDIV4_NFCDIV(0x0E) | SIM_CLKDIV4_NFCFRAC(0x03))
                              );
	SIM->SOPT2 &= (uint32_t)~(uint32_t)(SIM_SOPT2_NFCSRC(0x03));	//	выбрать шину как источник
  
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTC_MASK;    // питание на порты C, D
	
	// CLE, ALE, RE
	PORTD->PCR[8] &= ~PORT_PCR_ISF_MASK;	//	CLE
	PORTD->PCR[9] &= ~PORT_PCR_ISF_MASK;	//	ALE
	PORTD->PCR[10]&= ~PORT_PCR_ISF_MASK;	//	RE
  
	PORTD->PCR[8] |= PORT_PCR_MUX(0x01);  //  GPIO
	PORTD->PCR[9] |= PORT_PCR_MUX(0x01);  //  GPIO
	PORTD->PCR[10]|= PORT_PCR_MUX(0x01);  //  GPIO
  
	PTD->PDDR |= 1UL << 8 | 1UL << 9 | 1UL << 10; // CLE, ALE, RE - выходы
  
	CLE_RESET();	                        //	CLE->0
	ALE_RESET();	                        //	ALE->0
	RE_RESET();		                        //	RE ->1
	
	// WE, RB, CE
	PORTC->PCR[11] &= ~PORT_PCR_ISF_MASK;	//	WE
	PORTC->PCR[16] &= ~PORT_PCR_ISF_MASK;	//	RB
	PORTC->PCR[17] &= ~PORT_PCR_ISF_MASK;	//	CE
  
	PORTC->PCR[11] |= PORT_PCR_MUX(0x01); //  GPIO
	PORTC->PCR[16] |= PORT_PCR_MUX(0x01); //  GPIO
	PORTC->PCR[17] |= PORT_PCR_MUX(0x01); //  GPIO
  
	PTC->PDDR |= 1UL << 11 | 1UL << 16 | 1UL << 17; // WE, RB, CE - выходы
	
	WE_RESET();		                        // WE->1
	RB_RESET();		                        // RB->0
	CE_RESET();		                        // CE->1
	
  //  NFC_D0 - NFC_D7
  PORTD->PCR[5]  &= PORT_PCR_MUX(0x01);
  PORTD->PCR[4]  &= PORT_PCR_MUX(0x01);
  PORTC->PCR[10] &= PORT_PCR_MUX(0x01);
  PORTC->PCR[9]  &= PORT_PCR_MUX(0x01);
  PORTC->PCR[8]  &= PORT_PCR_MUX(0x01);
  PORTC->PCR[7]  &= PORT_PCR_MUX(0x01);
  PORTC->PCR[6]  &= PORT_PCR_MUX(0x01);
  PORTC->PCR[5]  &= PORT_PCR_MUX(0x01);
  
  PTD->PDDR |= 1ul << 5 | 1ul << 4;                                               //  NFC_D0, NFC_D1
  PTC->PDDR |= 1ul << 10 | 1ul << 9 | 1ul << 8 | 1ul << 7 | 1ul << 6 | 1ul << 5;  //  NFC_D2 - NFC_D7
  
  /* NFC_CFG: STOPWERR=0,ECCAD=0,ECCSRAM=0,DMAREQ=0,ECCMODE=7,FAST=0,IDCNT=5,TIMEOUT=6,BITWIDTH=0,AIAD=1,AIBN=1,PAGECNT=1 */
  NFC->CFG = (uint32_t)(( NFC->CFG & (uint32_t)~(uint32_t)(
             NFC_CFG_STOPWERR_MASK |  //  останов при ошибке записи
             NFC_CFG_ECCAD(0x01FF) |  //  ecc статус в SRAM
             NFC_CFG_ECCSRAM_MASK  |  //  писать/нет ecc статус в SRAM
             NFC_CFG_DMAREQ_MASK   |  //  передавать/не передвать сектор после ecc done
             NFC_CFG_FAST_MASK     |  //  fast flash для edo
             NFC_CFG_IDCNT(0x02)   |  //  количество байт, читаемых read id
             NFC_CFG_TIMEOUT(0x19) |  //  выдержка в тактах NFC_CLK 
             NFC_CFG_BITWIDTH_MASK |  //  полоса пропускания 8/16 бит
             NFC_CFG_PAGECNT(0x0E) )) //  количество страниц
             | 
             (uint32_t)( NFC_CFG_ECCMODE(0x07) |
             NFC_CFG_IDCNT(0x05) |
             NFC_CFG_TIMEOUT(0x06) |
             NFC_CFG_AIAD_MASK |
             NFC_CFG_AIBN_MASK |
             NFC_CFG_PAGECNT(0x01)
            ));
  
  NFC->SWAP = (NFC_SWAP_ADDR1(0x07FF) | NFC_SWAP_ADDR2(0x07FF));              //  буфер обмена DMA
  NFC->RAI = (NFC_RAI_INC3(0x00) | NFC_RAI_INC2(0x00) | NFC_RAI_INC1(0x01));  //  инкремент адреса ряда
  NFC->SECSZ = NFC_SECSZ_SIZE(0x0420);                                        //  размер сектора 1056(1024 + 32)

  NFC->ISR = NFC_ISR_WERRCLR_MASK |
             NFC_ISR_DONECLR_MASK |
             NFC_ISR_IDLECLR_MASK |
             NFC_ISR_RESBN(0x00)  |
             NFC_ISR_ECCBN(0x00)  |
             NFC_ISR_DMABN(0x00);
  
  if(NAND_Membus_Driver == NULL)
    return false;
	return true;
}
