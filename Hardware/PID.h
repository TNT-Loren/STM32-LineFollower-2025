#ifndef __PID_H
#define __PID_H

typedef struct 
{
	float target_val;		//目标值
	float actual_val;		//实际值
	float out;       		//输出
	float err;				//当前偏差
	float err_last;			//上次偏差
	float err_sum;			//误差累计值
	float Kp,Ki,Kd;			//比例，积分，微分系数
	
} PID;

extern PID PidMotor1Speed;
extern PID PidMotor2Speed;
extern PID PidYaw;
extern PID PidHD;


void PID_Init(void);
float PID_realize(PID * pid,float actual_val);
#endif
