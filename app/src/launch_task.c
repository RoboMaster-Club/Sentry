#include "launch_task.h"

#include "dji_motor.h"
#include "robot.h"
#include "remote.h"
#include "imu_task.h"
#include "user_math.h"
#include "referee_system.h"

#include <time.h>

extern Robot_State_t g_robot_state;
extern Remote_t g_remote;

DJI_Motor_Handle_t *leftFlywheel;
DJI_Motor_Handle_t *rightFlywheel;

clock_t lastHeatTick = 0;
clock_t currentTime = 0;
float barrelHeat = 0;
int maxHeat = 0;
int projectileHeat = 10;
float coolingPerTick = 0;
float timeSinceCheck = 0;
int botLvl = 1;

void Launch_Task_Init()
{
    // Init Launch Hardware
    Motor_Config_t flywheel_config = {
        .can_bus = 1,
        .speed_controller_id = 0,
        .offset = 0,
        .control_mode = VELOCITY_CONTROL,
        .motor_reversal = MOTOR_REVERSAL_NORMAL,
        .velocity_pid = {
            .kp = 500.0f,
            .ki = 0.0f,
            .kd = 0.0f,
            .output_limit = M3508_MAX_CURRENT
    
        }
    };

    flywheel_config.speed_controller_id = 1;
    leftFlywheel = DJI_Motor_Init(&flywheel_config, M3508);
    flywheel_config.speed_controller_id = 2;
    rightFlywheel = DJI_Motor_Init(&flywheel_config, M3508);

    time(&lastHeatTick);

}

void Launch_Ctrl_Loop()
{
    // Control loop for launch

    // heat is checked every 1/10 a second, one "tick"
    maxHeat = 170 + 30*botLvl;
    coolingPerTick = (32 + 8*botLvl) / 10;
    time(&currentTime);
    
    if (currentTime-lastHeatTick > .1f ){
        time(&lastHeatTick);

        if (barrelHeat >= coolingPerTick){
            barrelHeat - coolingPerTick;
        }
    }
    
}

void Launch_Speed_Test(){
    float speeds[20] = {}; 
    int vel = 100;
    for (int i = 0; i < 8; i++){
    DJI_Motor_Set_Velocity(leftFlywheel,vel);
    DJI_Motor_Set_Velocity(rightFlywheel,vel);
    while (DJI_Motor_Get_Velocity(leftFlywheel) < vel && DJI_Motor_Get_Velocity(rightFlywheel) < vel)
    {
        //wait
    }
    
    //fire();
    speeds[i] = Referee_Robot_State.Shooting_Speed;

    vel += 50;

    }
    printf(speeds);
}