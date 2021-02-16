/*
 *******************************************************************************
 * Filename:    Planner.c
 * Programmer:  Ivanov Semen
 * 
 * Created:     2015/11/17      (YYYY/MM/DD)
 * 
 * Description: 
 * *****************************************************************************
 */

/*******************************************************************************
 *                              Include Section
******************************************************************************/

#include    "Planner.h"

/*******************************************************************************
 *                              Global Variables
******************************************************************************/

/*******************************************************************************
 *                            Local Variables
******************************************************************************/
volatile uint8_t  array_tail = 0;
volatile static task_tag TaskArray[MAX_TASKS]; 

/*******************************************************************************
 *                             Local Function 
******************************************************************************/

/*******************************************************************************
 *                                Functions
******************************************************************************/

void planner_set_task (void (*taskFunc)(void), uint16_t taskDelay, uint16_t taskPeriod)
{
    uint8_t i;
  
    if(!taskFunc) return;
 
    for(i = 0; i < array_tail; i++) 
    {
        if(TaskArray[i].pFunc == taskFunc) 
        {
            __disable_irq ();
            TaskArray[i].delay = taskDelay;
            TaskArray[i].period = taskPeriod;
            TaskArray[i].run = 0; 
            __enable_irq ();
        }
    }
 
    if (array_tail < MAX_TASKS)
    { 
        __disable_irq ();
  
        TaskArray[array_tail].pFunc = taskFunc;
        TaskArray[array_tail].delay = taskDelay;
        TaskArray[array_tail].period = taskPeriod;
        TaskArray[array_tail].run = 0; 
 
        array_tail++;
        __enable_irq ();
    }
}

void planer_dispatch_task(void)
{
   uint8_t i;
   void (*function) (void);
    
   for (i=0; i<array_tail; i++)
   {
      if (TaskArray[i].run == 1)
      { 
         function = TaskArray[i].pFunc; 
         
        if(TaskArray[i].period == 0) 
        { 
            planner_delete_task(TaskArray[i].pFunc);
        }
        else
        {
           TaskArray[i].run = 0;
            if(!TaskArray[i].delay)
            {
               TaskArray[i].delay = TaskArray[i].period-1; 
           }
        }
         
       (*function)();
   }
 }
}

void planner_delete_task (void (*taskFunc)(void))
{
   uint8_t i;
   for (i=0; i<array_tail; i++)
   {
      if(TaskArray[i].pFunc == taskFunc)
      {
         __disable_irq ();
         if(i != (array_tail - 1))
         {
            TaskArray[i] = TaskArray[((uint8_t)array_tail)];
         }
         array_tail--;
         __enable_irq ();
      }
   }
}

void planner_tics_ISR (void)
{
   uint8_t i;
   for (i=0; i<array_tail; i++)
   {
      if (TaskArray[i].delay == 0)
      TaskArray[i].run = 1;
      else TaskArray[i].delay--;
   }
}

/*******************************************************************************
 *                              END OF FILE 
******************************************************************************/
