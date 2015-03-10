#include "ttc_scheduler.h"
#include "p18f6627.h"
#include "main.h"
#include "PES_ports.h"
#include "mytask.h"

//---------------------------------------------------
//Global variables
//---------------------------------------------------
// Count the number of ticks yet to be processed.
volatile uint32_t elapsed_ticks = 0;

//An array of tasks managed by the scheduler
task_type_t tasks[NUM_TASKS];

//Timer0 reload values
uint8_t reload_8h, reload_8l;

extern uint32_t led_period;

//----------------------------------------------------
//Local functions
//----------------------------------------------------
static void Disable_Interrupts();
static void Enable_Interrupts();
static void Manual_Reload_Timer0();

static void Disable_Interrupts() {
  INTCONbits.TMR0IE = 0; //Disable TIMER0 interrupt  
}

static void Enable_Interrupts() {
  INTCONbits.TMR0IE = 1; //Enable TIMER0 interrupt 
}

static void Manual_Reload_Timer0(void) {
  TMR0H = reload_8h;
  TMR0L = reload_8l;
}

void SCH_Init(uint32_t tick_interval_ms) {
  //Configure timer0 so that it generates interrupt 
  //every tick_interval_ms ms
  uint32_t inc;
  uint16_t reload_16;
  uint16_t prescaler;
	
	INTCONbits.TMR0IE = 0;  //Disable TIMER0 interrupt
	T0CONbits.TMR0ON = 0;   // Disable Timer0 
	
	T0CONbits.T08BIT = 0; //Timer0 is configured as a 16-bit timer/counter 
	T0CONbits.T0CS = 0;   //Choose Internal instruction cycle clock (CLKO) = Fosc/4
	T0CONbits.PSA = 0;    //Assign prescaler
	T0CONbits.T0PS0 = 1;
	T0CONbits.T0PS1 = 1; 
	T0CONbits.T0PS2 = 1; //Set prescaler = 1:256
	INTCONbits.TMR0IE = 1; //Enable timer0 interrupt
	
	prescaler = 256; //Set prescaler 256
	inc = ((uint32_t)tick_interval_ms * (CLK_FREQ/1000)) /  
	      ((uint16_t)OSC_PER_INST * (uint16_t)prescaler);
	      
	reload_16 = (uint16_t)(65536UL - inc);
	reload_8h = (uint8_t)(reload_16 / 256);
	reload_8l = (uint8_t)(reload_16 % 256);
	
	//Reload timer0
	TMR0H = reload_8h;
	TMR0L = reload_8l;
}

void SCH_Start(void) {
  T0CONbits.TMR0ON = 1; //Enable timer0 
}

#pragma interrupt Timer0_ISR
void Timer0_ISR(void) {
  if(INTCONbits.TMR0IE) {
    if (INTCONbits.TMR0IF) {
      INTCONbits.TMR0IF = 0; //Clear timer0 overflow flag
      Manual_Reload_Timer0();
      ++elapsed_ticks;          
    }
  }
}

//Code at interrupt vector
#pragma code highVector=0x808
void HighVector(void) {
  _asm goto Timer0_ISR _endasm
}
#pragma code


void SCH_Create_Task(void (*function_pointer)(void),
                     uint32_t period,
                     uint32_t delay) {
  static uint8_t index = 0;
  
  if (index < NUM_TASKS) {
    tasks[index].ptask = function_pointer;
    tasks[index].period = period;
    // Avoid underflow in the dispatcher.
    tasks[index].delay = delay + 1;
    ++index;    
  }                  
}

void SCH_Dispatch_Tasks(void) {
  uint8_t i = 0;
  
  Disable_Interrupts();
  while (elapsed_ticks > 0) { // TRUE only if the ISR ran.
    for (i = 0; i < NUM_TASKS; i++) {
      if (--tasks[i].delay == 0) {
        if (tasks[i].ptask == task_led) { //This led task
          tasks[i].delay = (uint32_t)led_period;
        } else {
          tasks[i].delay = tasks[i].period;
        }
        

        Enable_Interrupts();
        tasks[i].ptask(); // Execute the task!
        Disable_Interrupts();
      }
    }
    --elapsed_ticks;
  }
  Enable_Interrupts();
}

void SCH_Enter_Idle_Mode(void) {
  OSCCONbits.IDLEN = 1; 
  _asm sleep _endasm  
}

