#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"

//LEDs on the Raspberry Pi's daughterboard
#define LED_RED 22
#define LED_YELLOW 21
#define LED_GREEN 17

//Buttons on the Raspberry Pi's daughterboard
#define BUTTON_MIDDLE 9           //This pin is configured as pull-up
#define BUTTON_MIDDLE_PRESSED 0
#define BUTTON_MIDDLE_RELEASED 1

//Global variables
int button_flag_g = 0;


void task1(void *pParam) {
  while(1) {
    if (ReadGpio(BUTTON_MIDDLE) == BUTTON_MIDDLE_PRESSED) {
      button_flag_g = 1;
    } else {
      button_flag_g = 0;
    }   
  }
}

void task2(void *pParam) {
  while(1) {
    if (button_flag_g) { 
      //button flag is set, turn on led
      SetGpio(LED_RED, 1);
    } else {
      SetGpio(LED_RED, 0);
    }
  }
}
 
/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
void main(void) {

	DisableInterrupts();
	InitInterruptController();

  SetGpioDirection(LED_RED, GPIO_OUT);      //Set red LED pin (GPIO pin 16) an output pin
  SetGpioDirection(BUTTON_MIDDLE, GPIO_IN); //Set the middle button an input pin
  
	xTaskCreate(task1, "button", 128, NULL, 0, NULL);
	xTaskCreate(task2, "led", 128, NULL, 0, NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
