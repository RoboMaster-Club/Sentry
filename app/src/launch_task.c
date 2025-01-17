#include "launch_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "user_math.h"
#include "referee_system.h"

#define KP 0
#define KI 0
#define KF 0

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;

DJI_Motor_Handle_t *motors[3] = {NULL, NULL, NULL};
int motor_ids[3] = {1, 2, 3};

void Launch_Task_Init()
{
    // Init Launch Hardware
    for(int i = 0; i < 3; i++){
        Motor_Config_t config = {
            .can_bus = 1,
            .speed_controller_id = motor_ids[i],
            .offset = 0,
            .control_mode = VELOCITY_CONTROL,
            .motor_reversal = MOTOR_REVERSAL_NORMAL,
            .velocity_pid = {
                .kp = KP,
                .ki = KI,
                .kf = KF,
                .output_limit = M2006_MAX_CURRENT,
            }};
        motors[i] = DJI_Motor_Init(&config, M2006);       
    }
}

void Launch_Ctrl_Loop()
{
    // Control loop for launch
    if(g_robot_state.launch.IS_FLYWHEEL_ENABLED){
        DJI_Motor_Set_Velocity(motors[0], )
    }
}
