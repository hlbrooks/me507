/** @file MicroSwitch1.cpp
 *  This task is the first of two micro limit switch tasks in the program.
 *  This switch is pressed when the fire extinguisher is fully compressed.
 *  This switch is designed to change the value of the shared state variable
 *  to 2 for the FSM within task_Extinguish. This will switch the direction
 *  of the motor's rotation, thus translating the motor back toward its reset
 *  position.
 * 
 *  @author  Hunter Brooks & William Dorosk
 *  @date    20 Nov 2021 File Created
 *  @date    2  Dec 2021 Finalized file
 */

// To begin, #include the necessary libraries for this task
#include <Arduino.h>
#include <PrintStream.h>
#if (defined STM32L4xx || defined STM32F4xx)
    #include <STM32FreeRTOS.h>
#endif

#include "taskshare.h"               // Header for inter-task shared data
#include "taskqueue.h"               // Header for inter-task data queues 
#include "shares.h"                  // Header for shares
#include "MircroSwitch1.h"           // Header for MicroSwitch1 task module

/// Define the input pin from the Nucleo that will integrate with the micro limit switch
//  This pin will be read whenever the extinguisher motor is rotating toward the extinguisher lever
#define IN1 PA9 // The input pin that reads as a digital zero when the switch is pressed

 /** @brief   This is the task function that controls the first micro limit switch
  *  @details This task is the first of two micro limit switch tasks in the program.
  *           This switch is pressed when the fire extinguisher is fully compressed.
  *           This switch is designed to change the value of the shared state variable
  *           to 2 for the FSM within task_Extinguish. This will switch the direction
  *           of the motor's rotation, thus translating the motor back toward its reset
  *           position.
  *  @param   p_params A pointer to function parameters which we don't use.
  */

void MicroSwitch1 (void* p_params)
{
    (void)p_params;                             // Shuts up a compiler warning

    /// The number of RTOS ticks between runs of the MicroSwitch1 task
    const TickType_t MICROSWITCH1_PERIOD = 100;
    
    // Initialise the xLastWakeTime variable with the current time.
    // It will be used to run the task at precise intervals

    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Set the pin to behave as an input pin tied to pullup resistor
    pinMode(IN1, INPUT_PULLUP);  

    for (;;)
    {
        // If the extinguisher motor is rotating toward the extinguisher lever, 
        //     the input pin tied to the micro limit switch will be read every
        //     time this task is run, else the task exits
        // If the input pin is read and returns a digital zero, the task_extinguisher
        //     state variable share is set to two
 
        if (state_extinguish.get() == 1)  //share
        {
            uint8_t current_value = digitalRead (IN1);
            if (current_value == 0)
            {
                current_value = 1;
                state_extinguish.put(2);  //share
            }
            else
            {
            }
        }
        else
        {
        }
        // This type of delay waits until it has been the given number of RTOS
        // ticks since the task previously began running. This prevents timing
        // inaccuracy due to not accounting for how long the task took to run
        vTaskDelayUntil (&xLastWakeTime, MICROSWITCH1_PERIOD);
    }
}
