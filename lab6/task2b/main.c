#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"

//LEDs on the Raspberry Pi's daughterboard
#define LED_RED 22
#define LED_YELLOW 21
#define LED_GREEN 17

//Buttons on the Raspberry Pi's daughterboard
#define BUTTON_MIDDLE 9 //This pin is configured as pull-up
#define BUTTON_MIDDLE_PRESSED 0
#define BUTTON_MIDDLE_RELEASED 1

//Global variables
unsigned int button_press_count = 0;

void task1(void *pParam) {
  unsigned int buttonState = BUTTON_MIDDLE_RELEASED;
  unsigned int buttonLastState = BUTTON_MIDDLE_RELEASED;
  
  while(1) {
    buttonState = ReadGpio(BUTTON_MIDDLE);
    if (buttonState != buttonLastState) {
      button_press_count++;
      buttonLastState = buttonState; 
      vTaskDelay(100);
    }
  }
}

void task2(void *pParam) {
  unsigned int i = 0;
  
  while(1) {
  
    for (i = 0; i < button_press_count; i++) {
      SetGpio(LED_RED, 1);
      vTaskDelay(500);
      SetGpio(LED_RED, 0);
      vTaskDelay(500);
    }
    
    vTaskDelay(1000);
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

	//SetGpioFunction(16, 1);			// Status LED (green led next to the PWR led)
  SetGpioDirection(LED_RED, GPIO_OUT);      //Set red LED pin (GPIO pin 16) an output pin
  SetGpioDirection(BUTTON_MIDDLE, GPIO_IN); //Set the middle button an input pin
  
	xTaskCreate(task1, "button_press_count", 128, NULL, 0, NULL); //task1 has higher priority than task2
	xTaskCreate(task2, "led", 128, NULL, 1, NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
