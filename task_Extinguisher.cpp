/** @file task_Extinguisher.cpp
 *  This task consists of an FSM which extinguishes a fire when one is detected.
 *  When a fire is detected, this task actuates a motor that is press-fit to a 
 *  lead screw which clamps down the lever of a fire extinguisher that is mounted
 *  to the assembly.  When the lever has been fully compressed, a micro limit 
 *  switch is pressed which switches the direction of the motor.  The motor rotates 
 *  the lead screw thus translating the motor until a second mircro limit switch 
 *  is pressed.  At this point, the motor stops rotating, and the assembly is reset,
 *  thus ready to extinguish another fire. The motor which rotates the turntable 
 *  resumes rotation.
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
#include "task_Rotation_Base.h"      // Header for extinguisher task module

/// The number of RTOS ticks between runs of the extinguisher task
const TickType_t Extinguisher_PERIOD = 100;

// Define the pins that will be used to integrate the  motor driver to the Nucleo

/// One of the two inputs that determines the direction
#define BIN1 PB5
/// One of the two inputs that determines the direction
#define BIN2 PA10  
/// PWM input that controls the speed 
#define PWMB PB3  
/// Allows the H-bridges to work when high (has a pulldown resistor so it must actively pulled high)  
#define STBY PB4    

/// This constant is used to allow motor configuration to line up with function names like "forward" within Motor class.  Value can be 1 or -1
const int offsetB = 1;

/// An object of class Motor for the motor that actuates the fire extinguisher
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

/** @brief   This is the task function that actuates the fire extinguisher to extinguish the detected fire
 *  @details This task consists of an FSM which extinguishes a fire when one is detected. When a fire is 
 *           detected, this task actuates a motor that is press-fit to a lead screw which clamps down the
 *           lever of a fire extinguisher that is mounted to the assembly.  When the lever has been fully
 *           compressed, a micro limit switch is pressed which switches the direction of the motor.  The 
 *           motor rotates the lead screw thus translating the motor until a second mircro limit switch 
 *           is pressed.  At this point, the motor stops rotating, and the assembly is reset, thus ready to
 *           extinguish another fire. The motor which rotates the turntable resumes rotation.
 *  @param   p_params A pointer to function parameters which we don't use.
 */

void task_Extinguisher (void* p_params)
{
    (void)p_params;                             // Shuts up a compiler warning
    
    // initialize each local variable value to zero
    uint8_t start_extinguish = 0;    // variable that determines if the motor has started moving toward extinguisher
    uint8_t start_unclamp = 0;       // variable that determines if the motor has reversed direction from extinguisher

    // Initialise the xLastWakeTime variable with the current time.
    // It will be used to run the task at precise intervals
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // What follows is an FSM for the extinguish operation.  The function of each state is as follows:
        //     (1) - Begin motor rotation toward extinguisher
        //     (2) - Reverse motor rotation direction after the extinguisher lever has been fully compressed
        //     (3) - Halt motor rotation, set restart_program share value to one that notifies turntable rotation task to resume
        //           rotation and reset the share values to zero
        //     (0) - Intermediate state where the task sits until another fire is detected          

        if (state_extinguish.get() == 1)  //share
        {
            if (start_extinguish == 0) 
            {
                motor2.drive(250);
                start_extinguish = 1; 
            }
            else
            {
            }
        }
        else if (state_extinguish.get() == 2)  //share
        {
            if (start_unclamp == 0)
            {
                motor2.drive(-250);
                start_unclamp = 1;
            }
            else
            {
            }
        }     
        else if (state_extinguish.get() == 3)  //share
        {
            motor2.drive(0);           
            start_extinguish = 0; //
            start_unclamp = 0;
            restart_program.put(1);  //share
            state_extinguish.put(0);  //share
            fire_detected.put(0);
        }
        else
        {
        }
        // This type of delay waits until it has been the given number of RTOS
        // ticks since the task previously began running. This prevents timing
        // inaccuracy due to not accounting for how long the task took to run
        vTaskDelayUntil (&xLastWakeTime, Extinguisher_PERIOD);
    }
}
