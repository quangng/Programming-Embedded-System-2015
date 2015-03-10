#include "main.h"
#include "mytask.h"
#include "PES_ports.h"
#include "PES_leds.h"
#include "PES_adc.h"

//Buttons on the expansion board
#define BUTTON_BLUE_PRESSED 0
#define BUTTON_BLUE_UNPRESSED 1
#define BUTTON_BLUE PB1

//Global variable
static uint8_t button_blue_state = BUTTON_BLUE_UNPRESSED;

uint32_t led_period = 0;

void task_adc(void) {
  uint16_t temperature;
  
  temperature = PES_ADC_read();
  
  //Map adc reading from 10-bit ADC converter to the 
  //range of 20 - 200
  //period = (adc reading * 200 / 1024) + offset
  led_period = (uint32_t)(temperature * (uint32_t)200 / (uint32_t)1024) + 20;  
}

void task_button(void) {
  if (readPin(BUTTON_BLUE) == BUTTON_BLUE_PRESSED) {
    button_blue_state = BUTTON_BLUE_PRESSED;
  } else {
    button_blue_state = BUTTON_BLUE_UNPRESSED;
  } 
}

void task_led(void) {
  if (button_blue_state == BUTTON_BLUE_UNPRESSED) {
    blink_led(LED_RED);
  }
}