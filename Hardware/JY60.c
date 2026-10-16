#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include "Serial.h"
#include "motor.h"

#define   Base_Speed  3

float JYOut=0.00f;
float JYOut1=0.00f;
float JYOut2 = 0.00f;


void Yaw_process(void)
{
    int16_t temp;
    int16_t temp_2;             // 存储 ​​当前 Yaw 角度的临时值
    temp = Yaw - PidYaw.target_val;
    if (temp > 180)
    {
        temp -= 360;
    }
    else if (temp < -180)
    {
        temp += 360;
    }
    // 检查是否在死区范围内
    if (temp > -3 && temp < 3) // 设死区为 ±3 度
    {
        temp_2 = PidYaw.target_val; // 目标附近视为达到目标，停止调整
    }
    else
    {
        temp_2 = Yaw;
    }
    JYOut = PID_realize(&PidYaw, temp_2);
    JYOut1 = Base_Speed + JYOut;
    JYOut2 = Base_Speed - JYOut;
    if (JYOut1 > 5)
         JYOut1 = 5;
    if (JYOut2 > 5)
         JYOut2 = 5;
    if (JYOut1 <0)
         JYOut1 = 0;
    if(JYOut2 <0)
         JYOut2 = 0;
    MotorPidSet(JYOut1, JYOut2);
}

