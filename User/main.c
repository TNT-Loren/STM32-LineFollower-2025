#include "stm32f10x.h" // Device header
#include "OLED.h"
#include "Delay.h"
#include "Timer.h"
#include "Key.h"
#include "Motor.h"
#include "PID.h"
#include "Encoder.h"
#include "PWM.h"
#include "Serial.h"
#include "JY60.h"
#include "HD.h"

//===============================变量
volatile u16 Keystart; // 按键标志位
int a;
int CNT = 1;

//=============================函数
void Key_Select(void);

//==================================main
int main()
{
	OLED_Init();
	Timer_Init();
	Key_Init();
	PID_Init();
	Encoder_Init();
	Motor_Init();
	Serial3_Init();
	HD_Init();
	//===============
	OLED_Printf(0, 0, OLED_8X16, "Hello,world");
	OLED_Update();
	Serial_JY60_Zero_Yaw();
	while (1)
	{
		// OLED_Printf(0, 0, OLED_8X16, "%d %d %d %d %d %d %d",HD[0],HD[1],HD[2],HD[3],HD[4],HD[5],HD[6]);
		// OLED_Printf(0, 16, OLED_8X16, "%x", HD_State);
		 //OLED_Printf(0,16,OLED_8X16,"%.2f",PidMotor1Speed.target_val);
		// OLED_Printf(0, 32, OLED_8X16, "%.2f   %.2f", PidMotor1Speed.actual_val, PidMotor2Speed.actual_val);
		// OLED_Printf(0, 48, OLED_8X16, "%.2f,%x, ", Yaw, HD_State);
		//OLED_Update();
		if (Keystart) 
		{
			Key_Loop();
			Keystart = 0;
		}  
		//====================================
		Key_Select();
		//Yaw_process();
		 //HD_Process();
		if(Keep_Ok==0)
		{
			HD_Process();
		}
		else if(Keep_Ok==1)
		{
			Serial_JY60_Zero_Yaw();
			Yaw_process();
			if(Yaw_ok)
			{
				Yaw_ok=0;
				Serial_JY60_Zero_Yaw();
				Keep_Ok = 0;
			}
		}
		
	}
}

//========================================TIM1 (1ms)
void TIM1_UP_IRQHandler(void)
{
	static u16 count1, count2;
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		if (++count1 == 20)
		{
			Keystart = 1;
			count1 = 0;
		}
		if (++count2 == 30) // 每30ms获取一次速度并进行PID运算
		{
			count2 = 0;
			HD_Read();
			Motor1Speed = GetSpeed(1); // OLED_Printf(0, 48, OLED_8X16, "speed=%.2f    %f.2f", Motor1Speed, Motor2Speed);// OLED_Update();
			Motor2Speed = GetSpeed(2);
			Motor_Set(PID_realize(&PidMotor1Speed, Motor1Speed), PID_realize(&PidMotor2Speed, Motor2Speed));
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
//=======================================Key
void Key_Select()
{
	switch (Key_Short)
	{
	case 1:
	{
		Key_Short = 0;
		a++;
	}
	break;
	case 2:
	{
		Key_Short = 0;
	}
	break;
	}

	switch (Key_Long)
	{
	case 1:
	{
		Key_Long = 0;
		a--;
	}
	break;
	case 2:
	{
		Key_Long = 0;
	}
	break;
	}
}
