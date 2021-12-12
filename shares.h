/** @file shares.h
 *  This file contains extern declarations of shares which are used
 *  in our final project.
 * 
 *  @author Hunter Brooks & William Dorosk
 *  @date   2021-Nov-20 Original file
 *  @date   2  Dec 2021 Finalized file
 */

#ifndef _SHARES_H_
#define _SHARES_H_

// To begin, #include the necessary libraries
#include "task_Rotation_Base.h"
#include "task_Thermal_Sensor.h"
#include "task_Extinguisher.h"

/// Share which keeps track of whether a fire is being extinguished (1) or not (0)
extern Share<uint8_t> fire_detected;

/// A share which holds the state of the FSM within task_Extinguish
extern Share<uint8_t> state_extinguish;

/// A share which keeps track of whether the fire has been extinguished and the values of the shares are being reset (1) or not (0)
extern Share<uint8_t> restart_program;

#endif // _SHARES_H_
