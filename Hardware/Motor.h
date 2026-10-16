#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
float GetSpeed(int n);
void Motor_Set(float a,float b);

extern float Motor1Speed,Motor2Speed;
void MotorPidSet(float Motor1SetSpeed,float Motor2SetSpeed);

#endif
