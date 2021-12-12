/** @file task_Rotation_Base.h
 *  This task rotates the turntable which holds the rest of the assembly while a fire has not been
 *  detected by the thermal camera.  The turntable is rotated with a high torque motor.  When a 
 *  fire is detected, this motor's rotation is halted while the fire is extinguished.  When 
 *  extinguished, the motor's rotation resumes.
 * 
 *  @author Hunter Brooks & William Dorosk
 *  @date   20 Nov 2021 Created file
 */

void task_Rotation_Base (void* p_params);
