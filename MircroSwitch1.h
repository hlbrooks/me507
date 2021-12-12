/** @file MircroSwitch1.h
 *  This task is the first of two micro limit switch tasks in the program.
 *  This switch is pressed when the fire extinguisher is fully compressed.
 *  This switch is designed to change the value of the shared state variable
 *  to 2 for the FSM within task_Extinguish. This will switch the direction
 *  of the motor's rotation, thus translating the motor back toward its reset
 *  position.
 * 
 *  @author Hunter Brooks & William Dorosk
 *  @date   20 Nov 2021 Created file
 */

void MicroSwitch1 (void* p_params);
