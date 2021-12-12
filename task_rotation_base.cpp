/** @file task_Rotation_Base.cpp
 *  This task rotates the turntable which holds the rest of the assembly while a fire has not been
 *  detected by the thermal camera.  The turntable is rotated with a high torque motor.  When a 
 *  fire is detected, this motor's rotation is halted while the fire is extinguished.  When 
 *  extinguished, the motor's rotation resumes.
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
#include "SparkFun_TB6612.h"         // Header for the methods provided by the motor driver manufacturer
#include "task_Rotation_Base.h"      // Header for turntable rotation task module

/// The number of RTOS ticks between runs of the turntable rotation task
const TickType_t ROTATION_BASE_PERIOD = 100;

// Define the pins that will be used to integrate the  motor driver to the Nucleo

/// One of the two inputs that determines the direction
#define AIN1 PA8  
/// One of the two inputs that determines the direction  
#define AIN2 PB10  
/// PWM input that controls the speed
#define PWMA PA7   
/// Allows the H-bridges to work when high (has a pulldown resistor so it must actively pulled high) 
#define STBY PB4    

/// This constant is used to allow motor configuration to line up with function names like "forward" within Motor class.  Value can be 1 or -1
const int offsetA = 1;

/// An object of class Motor for the motor that rotates the turntable
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);

/** @brief   This is the task function that controlls the rotation of the motor that turns the turntable.
 *  @details This task rotates the turntable which holds the rest of the assembly while a fire has not been
 *           detected by the thermal camera.  The turntable is rotated with a high torque motor.  When a 
 *           fire is detected, this motor's rotation is halted while the fire is extinguished.  When 
 *           extinguished, the motor's rotation resumes.
 *  @param   p_params A pointer to function parameters which we don't use.
 */

void task_Rotation_Base (void* p_params)
{
    (void)p_params;          // Shuts up a compiler warning

    // initialize each share value to zero
    fire_detected.put(0);    
    state_extinguish.put(0); 
    restart_program.put(0);
    
    // Initialise the xLastWakeTime variable with the current time.
    // It will be used to run the task at precise intervals
    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Begin program with turntable rotating
    motor1.drive(250);

    for (;;)
    {
        // If a fire is not detected and if the program is not being restarted, the task will exit on each pass
        // If a fire is detected, the turntable motor will halt rotation and the share controlling the task_Extinguish
        //     FSM will be set to one
        // If the program is being restarted, the task will resume the motor rotation, and the shares will be reset to zero

        if (fire_detected.get() == 1) //share
        {
            if (state_extinguish.get() == 0) //share
            { 
                motor1.drive(0);
                state_extinguish.put(1);  //share
            }
            else 
            { 
            }
        }
        else
        {
        }
        if (restart_program.get() == 1)  //share
        {
            motor1.drive(250);
            restart_program.put(0);      //share
        }
        else
        {
        }
        // This type of delay waits until it has been the given number of RTOS
        // ticks since the task previously began running. This prevents timing
        // inaccuracy due to not accounting for how long the task took to run
        vTaskDelayUntil (&xLastWakeTime, ROTATION_BASE_PERIOD);
    }
}
