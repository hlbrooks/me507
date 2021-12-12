/** @file main.cpp
 *    This file contains the program for our final ME507 project which uses
 *    FreeRTOS to do multitasking. There are five total tasks that do the following:
 *    (1) - task_Rotation_Base - rotates the turntable while a fire has not been
 *                               detected by the thermal camera.  The turntable is 
 *                               rotated with a high torque motor.
 *    (2) - task_Thermal_Sensor - continuously uses thermal camera to scan for temperatures
 *                                within the view of the lense when a fire is not being 
 *                                extinguished. If a temperature above 140 degrees Fahrenheit
 *                                is measured, an interrupt is generated which raises the 
 *                                value of a share from 0 to 1, thus allowing the other tasks
 *                                to take the appropriate actions.
 *    (3) - task_Extinguisher -   when a fire is detected, this task actuates a motor that is 
 *                                press-fit to a lead screw which clamps down the lever of a 
 *                                fire extinguisher that is mounted to the assembly.  When the 
 *                                lever has been fully compressed, a micro limit switch is pressed
 *                                which switches the direction of the motor.  The motor rotates 
 *                                the lead screw thus translating the motor until a second mircro
 *                                limit switch is pressed.  At this point, the motor stops rotating,
 *                                and the assembly is reset, thus ready to extinguish another fire.
 *                                The motor which rotates the turntable resumes rotation.
 *    (4) - MicroSwitch1      -   this is the first of two micro limit switches in the assembly. This switch
 *                                is pressed when the fire extinguisher is fully compressed. This
 *                                switch is designed to change the value of the shared state variable
 *                                to 2 for the FSM within task_Extinguish. This will switch the direction
 *                                of the motor's rotation, thus translating the motor back toward its reset position
 *    (5) - MicroSwitch2      -   this is the second of two micro limit switches in the assembly. This switch
 *                                is pressed when the motor has fully translated back to the position it began
 *                                before the fire was extinguished. This switch is designed to change the value
 *                                of the shared state variable to 3 for the FSM within task_Extinguish. This will
 *                                halt the motor's rotation once it is back to its reset position
 * 
 *  @author Hunter Brooks & William Dorosk
 *  @date   20 Nov 2021 Created file
 *  @date   2  Dec 2021 Finalized file
 */

// To begin, #include the necessary libraries for this project
#include <Arduino.h>
#include <PrintStream.h>
#if (defined STM32L4xx || defined STM32F4xx)
    #include <STM32FreeRTOS.h>
#endif

#include "taskshare.h"               // Header for inter-task shared data
#include "taskqueue.h"               // Header for inter-task data queues
#include "shares.h"                  // Header for shares
#include "task_Rotation_Base.h"      // Header for turntable rotation task module
#include "task_Thermal_Sensor.h"     // Header for thermal camera task module
#include "task_Extinguisher.h"       // Header for extinguisher task module
#include "MircroSwitch1.h"           // Header for micro limit switch 1 task module
#include "MicroSwitch2.h"            // Header for micro limit switch 2 task module

/// Share which keeps track of whether a fire is being extinguished (1) or not (0)
Share<uint8_t> fire_detected ("fire_detected");

/// A share which holds the state of the FSM within task_Extinguish
Share<uint8_t> state_extinguish ("state_extinguish");

/// A share which keeps track of whether the fire has been extinguished and the values of the shares are being reset (1) or not (0)
Share<uint8_t> restart_program ("restart_program");

/** @brief   Arduino setup function which runs once at program startup.
 *  @details This function sets up a serial port for communication and creates
 *           the tasks which will be run.
 */
void setup () 
{
    // Start the serial port, wait a short time, then say hello. Use the
    // non-RTOS delay() function because the RTOS hasn't been started yet
    Serial.begin (115200);
    delay (5000);
    Serial << endl << endl << "Hello, I am FireBot" << endl;

    // Create a task which rotates the turntable while a fire has not been detected
    xTaskCreate (task_Rotation_Base,              // Task function
                 "Rotation",                      // Task name for debugging printouts
                 4096,                            // Stack size for this task
                 NULL,                            // Pointer to no parameters
                 1,                               // Priority
                 NULL);                           // Don't save task object pointer

    // Create a task which continuously scans for temperatures when a fire is not being extinguished
    xTaskCreate (task_Thermal_Sensor,             // Task function
                 "Thermal Sensor",                // Task name for debugging printouts
                 4096,                            // Stack size for this task
                 NULL,                            // Pointer to no parameters
                 2,                               // Priority
                 NULL);                           // Don't save task object pointer
    // Create a task which actuates a motor that compresses the lever of a fire extinguisher, thus extinguishing a fire
    xTaskCreate (task_Extinguisher,               // Task function
                 "Extinguisher",                  // Task name for debugging printouts
                 4096,                            // Stack size for this task
                 NULL,                            // Pointer to no parameters
                 3,                               // Priority
                 NULL);                           // Don't save task object pointer
                 
    // Create a task which switches the direction of the motor's rotation, thus translating the motor back toward its reset position
    xTaskCreate (MicroSwitch1,                    // Task function
                 "MicroSwitch1",                  // Task name for debugging printouts
                 4096,                            // Stack size for this task
                 NULL,                            // Pointer to no parameters
                 4,                               // Priority
                 NULL);                           // Don't save task object pointer

    // Create a task which halts the motor's rotation once it is back to its reset position
    xTaskCreate (MicroSwitch2,                    // Task function
                 "MicroSwitch2",                  // Task name for debugging printouts
                 4096,                            // Stack size for this task
                 NULL,                            // Pointer to no parameters
                 5,                               // Priority
                 NULL);                           // Don't save task object pointer
                 
    // If using an STM32, we need to call the scheduler startup function now;
    // if using an ESP32, it has already been called for us
    #if (defined STM32L4xx || defined STM32F4xx)
        vTaskStartScheduler ();
    #endif
}

/** @brief   Arduino's low-priority loop function, which we don't use.
 *  @details A non-RTOS Arduino program runs all of its continuously running
 *           code in this function after @c setup() has finished. When using
 *           FreeRTOS, @c loop() implements a low priority task on most
 *           microcontrollers and crashes on some others, so we'll not use it.
 */
void loop () 
{
}
