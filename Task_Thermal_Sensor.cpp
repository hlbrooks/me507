/** @file task_Thermal_Sensor.cpp
 *  This task continuously uses the thermal camera to scan for temperatures
 *  within the view of the lense when a fire is not being extinguished. 
 *  If a temperature above 140 degrees Fahrenheit is measured, an interrupt 
 *  is generated which raises the value of a share from 0 to 1, thus allowing
 *  the other tasks to take the appropriate actions.
 * 
 *  @author  Hunter Brooks & William Dorosk
 *  @date    20 Nov 2021 File Created
 *  @date    2  Dec 2021 Finalized file
 */

// To begin, #include the necessary libraries for this task
#include <Arduino.h>
#include <PrintStream.h>
#include <Wire.h>
#if (defined STM32L4xx || defined STM32F4xx)
    #include <STM32FreeRTOS.h>
#endif

#include "taskshare.h"               // Header for inter-task shared data
#include "taskqueue.h"               // Header for inter-task data queues
#include "shares.h"                  // Header for shares
#include <Adafruit_AMG88xx.h>        // Header for the methods provided by the thermal camera manufacturer
#include "task_Thermal_Sensor.h"     // Header for thermal camera task module

/// The number of RTOS ticks between runs of the thermal camera task
const TickType_t THERMAL_SENSOR_PERIOD = 100;

/// An object of class Adafruit_AMG88xx for the thermal camera that takes temperature measurements
Adafruit_AMG88xx amg;

/// Define the INT pin from the sensor board that will be used to integrate with the Nucleo
#define INT_PIN PC7 // The interrupt pin that fires when a temperature outside of the established bounds has been detected

// Any reading on any pixel above TEMP_INT_HIGH in degrees C, or under TEMP_INT_LOW in degrees C will trigger an interrupt
/// Specified temperature threshold, Triggers at any temperature above 140F
#define TEMP_INT_HIGH 60
/// Specified temperature threshold, Triggers at any temperature below 59F
#define TEMP_INT_LOW 15

// Code provided from thermal camera manufacturer
/// Variable that keeps track if interrupt was triggered or not
volatile bool intReceived = false;
/// Array of temperature data that is filled by thermal camera
uint8_t pixelInts[8];  

/** @brief   Interrupt subroutine function provided by thermal camera manufacturer
 *           that runs when interrupt is detected. This is intended to be short
 */
void AMG88xx_ISR() 
{
  intReceived = true;
}

/** @brief   This is the task function that controls the thermal camera which takes temperature measurements
 *  @details This task continuously uses the thermal camera to scan for temperatures
 *           within the view of the lense when a fire is not being extinguished. 
 *           If a temperature above 140 degrees Fahrenheit is measured, an interrupt 
 *           is generated which raises the value of a share from 0 to 1, thus allowing
 *           the other tasks to take the appropriate actions.
 *  @param   p_params A pointer to function parameters which we don't use.
 */

void task_Thermal_Sensor (void* p_params)
{
    (void)p_params;                             // Shuts up a compiler warning

    // The majority of what follows has been provided by the thermal camera manufacturer.
    pinMode(INT_PIN, INPUT);                    
    bool status;

    status = amg.begin();
    if (!status) 
        {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
        }

    amg.setInterruptLevels(TEMP_INT_HIGH, TEMP_INT_LOW);

    //set to absolue value mode
    amg.setInterruptMode(AMG88xx_ABSOLUTE_VALUE);

    //enable interrupts
    amg.enableInterrupt();

    //attach to our Interrupt Service Routine (ISR)
    attachInterrupt(digitalPinToInterrupt(INT_PIN), AMG88xx_ISR, FALLING);

    // Initialise the xLastWakeTime variable with the current time.
    // It will be used to run the task at precise intervals
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // If a fire is being extinguished, the thermal camera does not take temperature measurements
        // If a fire isn't being extinguished, the thermal camera takes temperature measurements and sets the value
        //     of the fire_detected share to one if a fire is detected
        if (fire_detected.get() == 1)  //share
        {
        }
        else 
        {
            if(intReceived)
            {
                amg.getInterrupt(pixelInts);
                fire_detected.put(1);         //share
                
                //clear the interrupt so we can get the next one!
                amg.clearInterrupt();
                intReceived = false;
             }
        }
    // This type of delay waits until it has been the given number of RTOS
        // ticks since the task previously began running. This prevents timing
        // inaccuracy due to not accounting for how long the task took to run
        vTaskDelayUntil (&xLastWakeTime, THERMAL_SENSOR_PERIOD);
    }
}
