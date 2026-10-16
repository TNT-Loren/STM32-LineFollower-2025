#include "stm32f10x.h"                  // Device header
#include "PID.h"

PID PidMotor1Speed;
PID PidMotor2Speed;
PID PidYaw;
PID PidHD;

void PID_Init(void)  //PID初始化，赋初始值
{

	//电机
	PidMotor1Speed.actual_val=0.0;
	PidMotor1Speed.target_val=0.00;
	PidMotor1Speed.err=0.0;
	PidMotor1Speed.err_last=0.0;
	PidMotor1Speed.err_sum=0.0;
	PidMotor1Speed.out=0;
	PidMotor1Speed.Kp=1;
	PidMotor1Speed.Ki=1;
	PidMotor1Speed.Kd=0;
	
	PidMotor2Speed.actual_val=0.0;
	PidMotor2Speed.target_val=0.00;
	PidMotor2Speed.err=0.0;
	PidMotor2Speed.err_last=0.0;
	PidMotor2Speed.err_sum=0.0;
	PidMotor2Speed.out=0;
	PidMotor2Speed.Kp=1;
	PidMotor2Speed.Ki=1;
	PidMotor2Speed.Kd=0;
	
	//JY60
	PidYaw.actual_val=0.0;
	PidYaw.target_val=90;
	PidYaw.err=0.0;
	PidYaw.err_last=0.0;
	PidYaw.err_sum=0.0;
	PidYaw.out=0;
	PidYaw.Kp=0.05;
	PidYaw.Ki=0;
	PidYaw.Kd=0.05;

	//HD
	PidHD.actual_val = 0.0;
	PidHD.target_val = 0;
	PidHD.err = 0.0;
	PidHD.err_last = 0.0;
	PidHD.err_sum = 0.0;
	PidHD.out = 0;
	PidHD.Kp = 0.8;
	PidHD.Ki = 0;
	PidHD.Kd = 0.3;
}

// PID控制函数 
float PID_realize(PID * pid,float actual_val)
{
	pid->actual_val = actual_val;
	pid->err = pid->target_val - pid->actual_val;
	pid->err_sum += pid->err;	
	pid->out = pid->Kp*pid->err + pid->Ki*pid->err_sum + pid->Kd*(pid->err - pid->err_last);
	pid->err_last = pid->err;							//为下一次做准备
	return pid->out;
}




