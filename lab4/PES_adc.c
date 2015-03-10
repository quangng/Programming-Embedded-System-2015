 #include "PES_adc.h"
#include "p18f6627.h"

/*
uint16_t PES_ADC_read(void) {
  uint16_t result;
   
  //Start ADC conversion by setting GO/NOT_DONE bit (ADCON0 register)
  ADCON0bits.GO_DONE = 1;
  
  //Wait for A/D conversion to complete by polling for GO/DONE bit to be cleared
  while(ADCON0bits.GO_DONE != 0);
  
  //Read A/D result register (ADRESH:ADRESL)
  result = (((uint16_t)ADRESH)<<8)|(ADRESL);
  
  return result;
}

*/

void PES_ADC_init(void) {
  //Configure AN2 as input port
  TRISAbits.TRISA2 = 1;
  
  //Select AN2
	ADCON0bits.CHS = 0b0010; 
	ADCON2bits.ACQT = 0b000; 
	ADCON2bits.ADCS = 0b111; 
	ADCON2bits.ADFM = 1; //Right justified

	ADCON0bits.ADON = 1; //Enable the ADC module
}

uint16_t PES_ADC_read(void) {
  uint16_t temperature;
  
  //Start ADC conversion
  ADCON0bits.GO_DONE = 1;
  
  //Wait until ADC conversion is done
  while(ADCON0bits.GO_DONE != 0);
  
  temperature = (((uint16_t)ADRESH)<<8)|(ADRESL);
	
  return temperature;
}