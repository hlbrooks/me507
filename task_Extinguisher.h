/** @file task_Extinguisher.h
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
 *  @author Hunter Brooks & William Dorosk
 *  @date   20 Nov 2021 Created file
 */

void task_Extinguisher (void* p_params);
