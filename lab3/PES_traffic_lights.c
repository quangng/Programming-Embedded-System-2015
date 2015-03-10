#include "PES_ports.h"
#include "PES_traffic_lights.h"

//LEDs on extension shield
#define LED_RED PB6
#define LED_YELLOW PC1
#define LED_GREEN PG1

//Port pin button on extension shield
#define BUTTON_RED PB0
#define BUTTON_BLUE PB1

//Times in each of the four light states (in milliseconds)
#define RED_DURATION 5000
#define RED_AMBER_DURATION 500
#define AMBER_DURATION 500
#define GREEN_DURATION 5000
#define AMBER_BLINKING_DURATION 5000

//Define update interval of state machine in milliseconds
#define TRAFFIC_LIGHTS_UPDATE_INTERVAL 50 

//Private variables
static tl_state_t tlState;
static unsigned char button_state = 0;

//Time in a state
static unsigned int stateTime = 0;

//Implementation of functions
void traffic_lights_init(const tl_state_t START_STATE) {
    tlState = START_STATE;
}

/* Update traffic light states
 * This must be updated every 50 ms millisecond defined in PES_eos_init_timer0
 */
void traffic_lights_update(void) {
  unsigned char i = 0;
  unsigned int j = 0;
 
  switch(tlState) {
    case AMBER_BLINKING:
      writePin(LED_RED, LOW); //Turn red LED off
      writePin(LED_GREEN, LOW); //Turn green LED off
			//stateTime++;
			if(++stateTime == (AMBER_BLINKING_DURATION/TRAFFIC_LIGHTS_UPDATE_INTERVAL)) {
				printf("amber blinking time = %u\n", stateTime);
				tlState = GREEN;
				stateTime = 0;
			} else {
				writePin(LED_YELLOW, HIGH); //Turn yellow LED on
      	for (i = 0; i < 15; i++) {
        	for (j = 0; j < 1500; j++);
      	}
      	writePin(LED_YELLOW, LOW); //Turn yellow LED off
      	for (i = 0; i < 15; i++) {
        	for (j = 0; j < 1500; j++);
      	}
			}
    break;
      
		case GREEN: //stay in green state for 5 seconds
      writePin(LED_RED, LOW); //Turn red LED off
      writePin(LED_YELLOW, LOW); //Turn yellow LED off
			writePin(LED_GREEN, HIGH); //Turn green LED on 
			if(++stateTime == GREEN_DURATION/TRAFFIC_LIGHTS_UPDATE_INTERVAL) {
				tlState = AMBER;	
				stateTime = 0;
   		}
		break;
      
		case AMBER:
			writePin(LED_RED, LOW); //Turn red LED off
      writePin(LED_YELLOW, HIGH); //Turn yellow LED on
      writePin(LED_GREEN, LOW); //Turn green LED off
    	if(++stateTime == AMBER_DURATION/TRAFFIC_LIGHTS_UPDATE_INTERVAL) {
				tlState = RED;
				stateTime = 0;
			}
    break;
    
    case RED:
      writePin(LED_RED, HIGH); //Turn red LED on
      writePin(LED_YELLOW, LOW); //Turn yellow LED off
      writePin(LED_GREEN, LOW); //Turn green LED off   
			button_state = readPin(BUTTON_BLUE); 
    	if (button_state == 0) { //red button is pressed
				tlState = RED_AMBER;
				stateTime = 0;
			} else {
				if (++stateTime == RED_DURATION/TRAFFIC_LIGHTS_UPDATE_INTERVAL) {
					tlState = GREEN;
					stateTime = 0;
				}
			}
    break;
    
    case RED_AMBER:
      writePin(LED_RED, LOW); //Turn red LED off
      writePin(LED_YELLOW, HIGH); //Turn yellow LED on
      writePin(LED_GREEN, LOW); //Turn green LED off
    	if(++stateTime == RED_AMBER_DURATION/TRAFFIC_LIGHTS_UPDATE_INTERVAL) {
				tlState = GREEN;
				stateTime = 0;
			}
    break;  
  } //End switch case
}
