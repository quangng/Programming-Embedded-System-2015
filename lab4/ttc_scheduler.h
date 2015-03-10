#ifndef __TTC_SCHEDULER_H
#define __TTC_SCHEDULER_H

#include "main.h"

#define NUM_TASKS (3)

//Scheduler data structure
typedef struct {
  void  (*ptask)(void); // Pointer to the task function.
  uint32_t period;      // Period to execute with.
  uint32_t delay;       // Delay before first call.
} task_type_t;

void SCH_Init(uint32_t tick_interval_ms);
void SCH_Start(void);
void SCH_Create_Task(void (*function_pointer)(void),
                     uint32_t period,
                     uint32_t delay);
                     
void SCH_Dispatch_Tasks(void);
void SCH_Enter_Idle_Mode(void);

#endif
