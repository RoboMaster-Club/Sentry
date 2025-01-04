#include "gimbal_task.h"

#include "dji_motor.h"
#include "imu_task.h"
#include "jetson_orin.h"
#include "pid.h"
#include "rate_limiter.h"
#include "robot.h"
#include "remote.h"
#include "user_math.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;
extern IMU_t g_imu;
extern Jetson_Orin_Data_t g_orin_data;

DJI_Motor_Handle_t* g_pitch_motor;
DJI_Motor_Handle_t* g_yaw_motor;
rate_limiter_t g_yaw_rate_limiter;
rate_limiter_t g_pitch_rate_limiter;

void Gimbal_Task_Init()
{
    // TODO: Work with gear ratios
    Motor_Config_t pitch_motor_config = {
        .can_bus = 2,
        .speed_controller_id = 0,
        .offset = PITCH_OFFSET,
        .control_mode = POSITION_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .angle_pid = {
            .kp = 1000.0f,
            .ki = 0.0f,
            .kd = 100.0f,
            .output_limit = M2006_MAX_CURRENT,
        }
    };
    g_pitch_motor = DJI_Motor_Init(&pitch_motor_config, M2006);

    Motor_Config_t yaw_motor_config = {
        .can_bus = 2,
        .speed_controller_id = 1,
        .offset = YAW_OFFSET,
        .control_mode = POSITION_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .angle_pid = {
            .kp = 1000.0f,
            .ki = 0.0f,
            .kd = 100.0f,
            .output_limit = M2006_MAX_CURRENT,
        }
    };
    g_pitch_motor = DJI_Motor_Init(&yaw_motor_config, M2006);

    rate_limiter_init(&g_yaw_rate_limiter, YAW_RATE_LIMIT);
    rate_limiter_init(&g_pitch_rate_limiter, PITCH_RATE_LIMIT);

}

void Gimbal_Ctrl_Loop()
{

    if (g_robot_state.launch.IS_AUTO_AIMING_ENABLED) {
        if (g_orin_data.receiving.auto_aiming.pitch || g_orin_data.receiving.auto_aiming.yaw) {
            // We can set these directly for now, they will be limited later in this function
            g_robot_state.gimbal.yaw_angle = (g_imu.deg.yaw - g_orin_data.receiving.auto_aiming.yaw) * DEG_TO_RAD;
            g_robot_state.gimbal.pitch_angle = (g_imu.deg.pitch - g_orin_data.receiving.auto_aiming.pitch) * DEG_TO_RAD;
        }
    }

    g_robot_state.gimbal.yaw_angle = fmod(g_robot_state.gimbal.yaw_angle, 2 * PI);
    __MAX_LIMIT(g_robot_state.gimbal.pitch_angle, PITCH_LOWER_LIMIT, PITCH_UPPER_LIMIT);

    g_robot_state.gimbal.yaw_angle = rate_limiter(&g_yaw_rate_limiter, g_robot_state.gimbal.yaw_angle);
    g_robot_state.gimbal.pitch_angle = rate_limiter(&g_pitch_rate_limiter, g_robot_state.gimbal.pitch_angle);

    DJI_Motor_Set_Angle(g_yaw_motor, g_robot_state.gimbal.yaw_angle);
    DJI_Motor_Set_Angle(g_pitch_motor, g_robot_state.gimbal.pitch_angle);
}
