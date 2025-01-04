#ifndef LAUNCH_TASK_H
#define LAUNCH_TASK_H

#include "dji_motor.h"

#define FLY_WHEEL_VELOCITY (1000.0f * M3508_REDUCTION_RATIO)
#define FEED_HOLE_NUM 6.0f
#define FEED_ANGLE ((2.0f * PI) / FEED_HOLE_NUM)
#define LAUNCH_FREQUENCY 10.0f

void Launch_Task_Init(void);
void Launch_Ctrl_Loop(void);
void Calc_Feeding(void);

extern DJI_Motor_Handle_t *g_flywheel_left, *g_flywheel_right, *g_feed_motor;

#endif // LAUNCH_TASK_H
