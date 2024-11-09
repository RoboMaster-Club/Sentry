#include "gimbal_task.h"

#include "robot.h"
#include "remote.h"
#include "user_math.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;

DJI_Motor_Handle_t *g_gimbal_motors[2];

void Gimbal_Task_Init()
{
    for (int i = 0; i < 2; i++)
    {
        Motor_Config_t chassis_motor_config = {
        .can_bus = 2,
        .speed_controller_id = i+1,
        .offset = 0,
        .control_mode = POSITION,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .angle = 
            {
                .kp = 1000.0f,
                .ki = 0.0f,
                .kd = 100.0f,
                .output_limit = M2006_MAX_CURRENT,
            }};
        g_chassis_motors[i] = DJI_Motor_Init(&chassis_motor_config, M2006);
    }
}

void Gimbal_Ctrl_Loop()
{
    float yaw_velocity = g_remote.controller.right_stick.x;
    float pitch_velocity = g_remote.controller.right_stick.y;
    g_robot_state.gimbal.pitch_angle += pitch_velocity * 0.01f;
    g_robot_state.gimbal.yaw_angle += yaw_velocity * 0.01f;
}
