#include "chassis_task.h"

#include "robot.h"
#include "remote.h"
#include "dji_motor.h"
#include "pid.h"
#include "omni_locomotion.h"

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;

#define CHASSIS_WHEEL_DIAMETER (0.05f)
#define CHASSIS_RADIUS (0.26f)
#define CHASSIS_MAX_SPEED (0.5f)
#define CHASSIS_MOUNTING_ANGLE (45.0f)

float chassis_rad;
DJI_Motor_Handle_t *motors[4];
uint8_t drive_esc_id_array[4] = {1, 2, 3, 4};
Motor_Reversal_t drive_motor_reversal_array[4] = {
    MOTOR_REVERSAL_NORMAL,
    MOTOR_REVERSAL_NORMAL,
    MOTOR_REVERSAL_NORMAL,
    MOTOR_REVERSAL_NORMAL};

omni_physical_constants_t physical_constants;
omni_chassis_state_t chassis_state;

void Chassis_Task_Init()
{
    // Init chassis hardware
    Motor_Config_t drive_motor_config = {
        .can_bus = 1,
        .control_mode = VELOCITY_CONTROL,
        .velocity_pid = {
            .kp = 500.0f,
            .kf = 100.0f,
            .output_limit = M2006_MAX_CURRENT,
            .integral_limit = 3000.0f,
        }};

    for (int i = 0; i < 4; i++)
    {
        // configure drive motor
        drive_motor_config.speed_controller_id = drive_esc_id_array[i];
        drive_motor_config.motor_reversal = drive_motor_reversal_array[i];
        motors[i] = DJI_Motor_Init(&drive_motor_config, M2006);
    }

    // Initialize the omni chassis state
    physical_constants = omni_init(
        CHASSIS_WHEEL_DIAMETER,
        CHASSIS_RADIUS,
        CHASSIS_MOUNTING_ANGLE,
        CHASSIS_MAX_SPEED);

    chassis_state.v_x = 0.0f;
    chassis_state.v_y = 0.0f;
    chassis_state.omega = 0.0f;
}

void Chassis_Ctrl_Loop()
{
    chassis_state.v_x = g_robot_state.input.vy; // x and y are swapped due to joytick orientation
    chassis_state.v_y = -g_robot_state.input.vx;
    chassis_state.omega = g_robot_state.input.vomega;

    // Control loop for the chassis
    omni_calculate_kinematics(&chassis_state, &physical_constants);
    omni_desaturate_wheel_speeds(&chassis_state, &physical_constants);
    omni_convert_to_rpm(&chassis_state);

    DJI_Motor_Set_Velocity(motors[0], chassis_state.phi_dot_1);
    DJI_Motor_Set_Velocity(motors[1], chassis_state.phi_dot_2);
    DJI_Motor_Set_Velocity(motors[2], chassis_state.phi_dot_3);
    DJI_Motor_Set_Velocity(motors[3], chassis_state.phi_dot_4);
}
