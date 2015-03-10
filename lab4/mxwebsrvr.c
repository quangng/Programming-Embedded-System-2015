/*
	Abo Akademi University
	Programming Embedded Systems 2015
	Exercise 4: Cooperative scheduler for embedded systems
	Author: Vu Nguyen (71141) <quangngmetro@gmail.com>
*/
#include "projdefs.h"
#include "p18f6627.h"
#include "PES_ports.h"
#include "ttc_scheduler.h"
#include "mytask.h"
#include "PES_ports.h"
#include "PES_adc.h"

static void System_Init(void);

//Main application
void main(void) {
  //Initialize the system
  System_Init();
  
  //Initialize cooperative scheduler with tick interval of 5ms
  SCH_Init(5);
  
  //Initialize tasks
  PES_ADC_init();
  
  //Create tasks with name, period, and initial delay in ticks (NOT ms)
  SCH_Create_Task(task_adc, 10, 0);
  SCH_Create_Task(task_button, 20, 5);
  SCH_Create_Task(task_led, 40, 10);  
  
  
  //Start cooperating scheduler
  SCH_Start();
  
  while(1) {
    SCH_Enter_Idle_Mode();
    SCH_Dispatch_Tasks();
  }
}

static void System_Init(void) {
  //Disable watchdog timer
  WDTCONbits.SWDTEN = 0;  
}
