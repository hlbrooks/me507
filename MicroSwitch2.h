/** @file MicroSwitch2.h
 *  This task is the first of two micro limit switch tasks in the program.
 *  This switch is pressed when the motor has fully translated back to the
 *  position it began before the fire was extinguished. This switch is 
 *  designed to change the value of the shared state variable to 3 for the
 *  FSM within task_Extinguish. This will halt the motor's rotation once 
 *  it is back to its reset position
 * 
 *  @author Hunter Brooks & William Dorosk
 *  @date   20 Nov 2021 Created file
 */

void MicroSwitch2 (void* p_params);
