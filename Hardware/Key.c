#include "stm32f10x.h" // Device header

//长按键编号、短按键编号、按键持续时间计数器、长按键标志位
uint8_t Key_Short, Key_Long, Key_Time, Long_State = 1;

void Key_Init(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


}

uint8_t Key_State(void)
{
	uint8_t Key = 0;

	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10))
		Key = 1;
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
		Key = 2;


	return Key;
}

void Key_Loop(void)
{
	static uint8_t LastState, NowState;
	uint8_t i;
	LastState = NowState;
	NowState = Key_State();
	for (i = 1; i <= 2; i++)
	{
		if (LastState == 0 && NowState == i)
			Key_Time = 0;						 // 开始计时
		if (LastState == i && NowState == i)
		{
			if (++Key_Time >= 25 && Long_State)   //20ms*25 = 500ms
			{
				Key_Long = i;
				Long_State = 0;
			}
		}
		if (LastState == i && NowState == 0)
		{
			if (Long_State)
				Key_Short = i;
			else
				Long_State = 1;
		}
	}
}
