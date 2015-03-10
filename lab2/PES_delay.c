#include "PES_delay.h"

void initTimer0(void) {
	/* Note: If Timer0 interrupt is set, the timer0 will not overflow */
	T0CONbits.TMR0ON = 1; // Enables Timer0 
	T0CONbits.T08BIT = 0; //Timer0 is configured as a 16-bit timer/counter 
	T0CONbits.T0CS = 0; //Choose Internal instruction cycle clock (CLKO) = Fosc/4
	T0CONbits.PSA = 1; //TImer0 prescaler is NOT assigned. Timer0 clock input bypasses prescaler
	INTCONbits.TMR0IE = 0; //Disable timer0 interrupt
}

void delay_ms(unsigned int ms) {
	unsigned int i = 0;
	INTCONbits.TMR0IF = 0;
	
	//The internal instruction cycle clock Fcyl = Fosc/4 = 40Mhz/4 = 10Mhz
	//16-bit Timer 0 overflow at 0xffff. The timer increments every 1/10Mhz = 0.1us, and we
	//need the timer to overflow every 1ms. The timer should be incremented 1ms/0.1us = 10000 times
	//before it overflows. So the starting value of the timer is 65535 - 10000 = 55535 (0xd8ef)
	for (i = 0; i < ms; i++) {
		TMR0H = 0xd8;
		TMR0L = 0xef;
		while(!INTCONbits.TMR0IF);
		INTCONbits.TMR0IF = 0;
	}
}