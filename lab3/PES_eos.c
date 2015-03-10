#include "PES_eos.h"
#include "p18f6627.h"
#include "projdefs.h"
#include "main.h"
#include "PES_traffic_lights.h"

//Global data
volatile unsigned char reload_8h, reload_8l;

void PES_eos_init_timer0(unsigned int ms) {
	unsigned long inc;
	unsigned int reload_16;
	unsigned int prescaler;
	
	INTCONbits.TMR0IE = 0; //Disable TIMER0 interrupt
	T0CONbits.TMR0ON = 0; // Disable Timer0 while configuring
	
	T0CONbits.T08BIT = 0; //Timer0 is configured as a 16-bit timer/counter 
	T0CONbits.T0CS = 0; //Choose Internal instruction cycle clock (CLKO) = Fosc/4
	T0CONbits.PSA = 0; //Assign prescaler
	T0CONbits.T0PS0 = 1;
	T0CONbits.T0PS1 = 1; 
	T0CONbits.T0PS2 = 1; //Set prescaler = 1:256
	INTCONbits.TMR0IE = 1; //Enable timer0 interrupt
	
	prescaler = 256; //Set prescaler 256
	inc = ((unsigned long)ms * (CLOCK_FREQ/1000)) /  
	      ((unsigned int)OSC_PER_INST * (unsigned int)prescaler);
	      
	reload_16 = (unsigned int)(65536UL - inc);
	reload_8h = (unsigned char)(reload_16 / 256);
	reload_8l = (unsigned char)(reload_16 % 256);

	TMR0H = reload_8h;
	TMR0L = reload_8l;
	
	T0CONbits.TMR0ON = 1; // Enable Timer0
}

	
void PES_eos_go_to_idle(void) {
  OSCCONbits.IDLEN = 1; //Device enters an idle mode when a SLEEP instruction is executed
  _asm sleep _endasm  
}

//ISR for Timer0. Reload Timer0 again in the ISR
//because Timer0 does not support automatic reload
#pragma interrupt HighISR save=section(".tmpdata")
void HighISR(void) {
  if(INTCONbits.TMR0IE) {
	  if (INTCONbits.TMR0IF) {
  	  INTCONbits.TMR0IF = 0; //Clear timer0 overflow flag
  	  TMR0H = reload_8h;
      TMR0L = reload_8l;
      traffic_lights_update(); 
	  }	  
  } 
}

//Code at interrupt vector
#pragma code highVector=0x808
void HighVector(void) {
  _asm goto HighISR _endasm
}
#pragma code

