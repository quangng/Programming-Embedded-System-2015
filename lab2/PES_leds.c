#include "PES_leds.h"
#include "p18f6627.h"

void blink_led(unsigned char led) {
	switch (led) {
		case LED_RED:
			TRISBbits.TRISB6 = 0;
			LATBbits.LATB6 ^= 1;
		break;
		
		case LED_YELLOW:
			TRISCbits.TRISC1 = 0;
			LATCbits.LATC1 ^= 1;
		break;
		
		case LED_GREEN:
			TRISGbits.TRISG1 = 0;
			LATGbits.LATG1 ^= 1;
		break;
		
		default:
			LATGbits.LATG1 ^= 1;
		break;
	} 
}	