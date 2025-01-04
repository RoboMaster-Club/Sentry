#include "launch_task.h"

#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "user_math.h"
#include "referee_system.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
DJI_Motor_Handle_t *g_flywheel_left, *g_flywheel_right, *g_feed_motor;

void Launch_Task_Init()
{
    // Init Launch Hardware
    Motor_Config_t flywheel_left_config = {
        .can_bus = 1,
        .speed_controller_id = 4,
        .offset = 0,
        .control_mode = VELOCITY_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_REVERSED,
        .velocity_pid =
            {
                .kp = 500.0f,
                .output_limit = M3508_MAX_CURRENT,
            },
    };

    Motor_Config_t flywheel_right_config = {
        .can_bus = 1,
        .speed_controller_id = 5,
        .offset = 0,
        .control_mode = VELOCITY_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .velocity_pid =
            {
                .kp = 500.0f,
                .output_limit = M3508_MAX_CURRENT,
            },
    };

    Motor_Config_t feed_speed_config = {
        .can_bus = 1,
        .speed_controller_id = 2,
        .offset = 0,
        .control_mode = VELOCITY_CONTROL | POSITION_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .velocity_pid =
            {
                .kp = 500.0f,
                .kd = 200.0f,
                .kf = 100.0f,
                .output_limit = M2006_MAX_CURRENT,
            },
        .angle_pid =
            {
                .kp = 450000.0f,
                .kd = 15000000.0f,
                .ki = 0.1f,
                .output_limit = M2006_MAX_CURRENT,
                .integral_limit = 1000.0f,
            }
    };

    g_flywheel_left = DJI_Motor_Init(&flywheel_left_config,M3508);
    g_flywheel_right = DJI_Motor_Init(&flywheel_right_config,M3508);
    g_feed_motor = DJI_Motor_Init(&feed_speed_config,M2006);
}

void Launch_Ctrl_Loop()
{
    // Control loop for launch
    if (g_robot_state.IS_SAFELY_STARTED) {
        if (g_robot_state.launch.IS_FLYWHEEL_ENABLED) {
            if (g_robot_state.launch.IS_FIRING_ENABLED) {
                // Setting flywheel velocity
                float flywheel_velo = FLY_WHEEL_VELOCITY;
                DJI_Motor_Set_Velocity(g_flywheel_left, flywheel_velo);
                DJI_Motor_Set_Velocity(g_flywheel_right, flywheel_velo);
                // Logic for feeding
                Calc_Feeding();
            }
        } 
        else {
            DJI_Motor_Disable(g_feed_motor);
            DJI_Motor_Disable(g_flywheel_left);
            DJI_Motor_Disable(g_flywheel_right);
        }
    }
    else {
        DJI_Motor_Disable(g_feed_motor);
        DJI_Motor_Disable(g_flywheel_left);
        DJI_Motor_Disable(g_flywheel_right);
    }
}

void Calc_Feeding() 
{
    switch (g_robot_state.launch.fire_mode) {
        case (SINGLE_FIRE):
            DJI_Motor_Set_Control_Mode(g_feed_motor, POSITION_CONTROL);
            if (!g_robot_state.launch.single_launch_finished_flag) {
                g_robot_state.launch.feed_angle = DJI_Motor_Get_Total_Angle(g_feed_motor) + FEED_ANGLE;
                g_robot_state.launch.single_launch_finished_flag = 1;
            }
            else {
                float angle = g_robot_state.launch.feed_angle;
                DJI_Motor_Set_Angle(g_feed_motor, angle);
            }
            break;
        case (BURST):
            break;
        case (FULL_AUTO): ;
            float feed_motor_velo = LAUNCH_FREQUENCY / FEED_HOLE_NUM * 60.0f; // TODO: might be wrong, copy pasted from swerve
            DJI_Motor_Set_Control_Mode(g_feed_motor, VELOCITY_CONTROL);
            DJI_Motor_Set_Velocity(g_feed_motor, feed_motor_velo);
            break;
        case (NO_FIRE_MODE_SELECTED):
            DJI_Motor_Set_Control_Mode(g_feed_motor, VELOCITY_CONTROL);
            DJI_Motor_Set_Velocity(g_feed_motor, 0);
            break;
        default:
            break;
    }
}
