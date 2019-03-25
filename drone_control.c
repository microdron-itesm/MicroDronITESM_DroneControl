#include <stdio.h>
#include "drone_control.h"
PID_CONFIG yawPIDConfig;
PID_CONFIG rollPIDConfig;
PID_CONFIG pitchPIDConfig;
PID_CONFIG heightPIDConfig;

const float k = 200;
bool usingManualThrust = false;
float currentManualThrust = 0;

void DRONE_CTRL_INITIALIZE(){
    yawPIDConfig.p = 0.05;
    yawPIDConfig.i = 0.0;
    yawPIDConfig.d = 0.0;
    yawPIDConfig.clampled = true;
    yawPIDConfig.min = -1.0;
    yawPIDConfig.max = 1.0;

    rollPIDConfig.p = 0.2;
    rollPIDConfig.i = 0.0;
    rollPIDConfig.d = 5.0;
    rollPIDConfig.clampled = true;
    rollPIDConfig.min = -1.0;
    rollPIDConfig.max = 1.0;

    pitchPIDConfig.p = 0.2;
    pitchPIDConfig.i = 0.0;
    pitchPIDConfig.d = 5.0;
    pitchPIDConfig.clampled = true;
    pitchPIDConfig.min = -1.0;
    pitchPIDConfig.max = 1.0;

    heightPIDConfig.p = 2.5;
    heightPIDConfig.i = 0.0;
    heightPIDConfig.d = 20.0;
    heightPIDConfig.clampled = true;
    heightPIDConfig.min = 0.0;
    heightPIDConfig.max = 1.0;

    YAW_PID_START(yawPIDConfig);
    ROLL_PID_START(rollPIDConfig);
    PITCH_PID_START(pitchPIDConfig);
    HEIGHT_PID_START(heightPIDConfig);

    YAW_PID_SETPOINT(0.0);
    ROLL_PID_SETPOINT(0.0);
    PITCH_PID_SETPOINT(0.0);
    HEIGHT_PID_SETPOINT(0.0);
}

void DRONE_CTRL_UPDATE(DRONE_POSE newPose){
    YAW_PID_UPDATE(newPose.yaw);
    ROLL_PID_UPDATE(newPose.roll);
    PITCH_PID_UPDATE(newPose.pitch);
    HEIGHT_PID_UPDATE(newPose.height);
}

void DRONE_CTRL_SET_TARGET_YAW(float yaw){
    YAW_PID_SETPOINT(yaw);
}

void DRONE_CTRL_SET_TARGET_ROLL(float roll){
    ROLL_PID_SETPOINT(roll);
}

void DRONE_CTRL_SET_TARGET_PITCH(float pitch){
    PITCH_PID_SETPOINT(pitch);
}

void DRONE_CTRL_SET_TARGET_HEIGHT(float height){
    HEIGHT_PID_SETPOINT(height);
}

void DRONE_CTRL_USE_MANUAL_THRUST(bool manualThrust){
    usingManualThrust = manualThrust;
}

void DRONE_CTRL_SET_MANUAL_THRUST(float thrust){
    currentManualThrust = thrust;
}

bool DRONE_CTRL_USING_MANUAL_THRUST(){
    return usingManualThrust;
}

DRONE_CTRL_MOTOR_OUTPUT DRONE_CTRL_GET_MOTOR_OUTPUT(){
    float yawRate = YAW_PID_GET_OUTPUT();
    float rollRate = ROLL_PID_GET_OUTPUT();
    float pitchRate = PITCH_PID_GET_OUTPUT();
    
    float thrust = usingManualThrust ? currentManualThrust : HEIGHT_PID_GET_OUTPUT();

    DRONE_CTRL_MOTOR_OUTPUT motorOutput;

    motorOutput.bottomLeft = yawRate * k - pitchRate * k - rollRate * k + thrust * k;
    motorOutput.bottomRight = (yawRate * k + pitchRate * k - rollRate * k - thrust * k) * -1;
    motorOutput.topLeft = (yawRate * k - pitchRate * k + rollRate * k - thrust * k) * -1;
    motorOutput.topRight = yawRate * k + pitchRate * k + rollRate * k + thrust * k;
    
    motorOutput.bottomLeft = motorOutput.bottomLeft >= 0 ? motorOutput.bottomLeft : 0;
    motorOutput.bottomRight = motorOutput.bottomRight >= 0 ? motorOutput.bottomRight : 0;
    motorOutput.topLeft = motorOutput.topLeft >= 0 ? motorOutput.topLeft : 0;
    motorOutput.topRight = motorOutput.topRight >= 0 ? motorOutput.topRight : 0;

    motorOutput.bottomLeft = motorOutput.bottomLeft > k ? k : motorOutput.bottomLeft;
    motorOutput.bottomRight = motorOutput.bottomRight > k ? k : motorOutput.bottomRight;
    motorOutput.topLeft = motorOutput.topLeft > k ? k : motorOutput.topLeft;
    motorOutput.topRight = motorOutput.topRight > k ? k : motorOutput.topRight;
    
    return motorOutput;
}


