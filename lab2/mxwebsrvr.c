#include "projdefs.h"
#include "p18f6627.h"
#include "PES_delay.h"
#include "PES_leds.h"
#include "PES_ports.h"

void init(void);

void main(void)
{
	init();
	
	while(1){
		if (readPin(PB1) == 1) { 	//Push button is configured to be pullup; so if pressed, logic low should be read
									//If not pressed (released), logic high should be read	
			blink_led(LED_YELLOW);
			delay_ms(1000);
			blink_led(LED_YELLOW);
			delay_ms(1000);		
		}
	}
}

void init(void) {
	//Set SWDTEN bit, this will enable the watch dog timer
    WDTCON_SWDTEN = 0;		//Disable watch dog timer
    
    //Initlize timer0 for delay function
    initTimer0();
}
