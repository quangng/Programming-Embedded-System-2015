#include "PES_ports.h"
#include "p18f6627.h"

int readPin(unsigned char port_pin) {
	int value;
	
	if (port_pin == PB0) {
		TRISBbits.TRISB0 = 1;
		if (PORTBbits.RB0 & 0x1) {
			value = 1;
		} else {
			value = 0;
		}
			
	} 
	if (port_pin == PB1) {
		TRISBbits.TRISB1 = 1;
		if (PORTBbits.RB1 & 0x1) {
			value = 1;
		} else {
			value = 0;
		}
	}
	
	return value;
}

void writePin(unsigned char port_pin, unsigned char mode) {
  switch(port_pin) {
    case PB6: //red LED
      TRISBbits.TRISB6 = 0; //set port B pin 6 to output
      if (mode == HIGH)
        LATBbits.LATB6 = 1;  
      if (mode == LOW)
        LATBbits.LATB6 = 0;
    break;
    
    case PC1: //yellow LED
      TRISCbits.TRISC1 = 0; //set port C pin 1 to output
      if (mode == HIGH)
        LATCbits.LATC1 = 1;  
      if (mode == LOW)
        LATCbits.LATC1 = 0;    
    break;
    
    case PG1: //green LED
      TRISGbits.TRISG1 = 0; //set port G pin 1 to output
      if (mode == HIGH)
        LATGbits.LATG1 = 1;  
      if (mode == LOW)
        LATGbits.LATG1 = 0;    
    break;
  } //end switch case
}


