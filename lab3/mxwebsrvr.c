/*
	Programming Embedded Systems - 2015
	Student name: Vu Nguyen (71141)
	University of Turku
	
	Exercise 3: I/O and scheduling
	Implement a traffic light system as a multi-state machine. the system should work the following way:
	Initialize to amber blinking, after 5 seconds it should go to operation. Red(5s) -> Amber+Red(0.5s) 
	-> Green(5s) -> Amber(0.5s) -> Red. A button press will switch to green via Amber+Red
*/
#include "projdefs.h"
#include "p18f6627.h"
#include "PES_ports.h"
#include "PES_eos.h"

#include "PES_traffic_lights.h"

//Main application
void main(void) {	
	//Set up embedded operating system (50ms)
	PES_eos_init_timer0(50);
	
	//Set up traffic lights
	traffic_lights_init(AMBER_BLINKING);
	
	while(1){
		//Go to idle mode
		PES_eos_go_to_idle();
	}
}

