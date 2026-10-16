#include "stm32f10x.h" // Device header

/**
 * 函    数：PWM初始化
 * 参    数：无
 * 返 回 值：无
 */
void PWM_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  // 【修改】开启TIM4的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 【修改】开启GPIOB的时钟 (TIM4通道默认在GPIOB)

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // 【修改】使用PB6(TIM4_CH1)和PB7(TIM4_CH2)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // 【修改】初始化GPIOB的对应引脚

	/*配置时钟源*/
	TIM_InternalClockConfig(TIM4); // 【修改】选择TIM4为内部时钟

	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;	  // ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1; // PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure); // 【修改】配置TIM4的时基单元

	/*输出比较初始化*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); // 【修改】配置TIM4的输出比较通道1 (PB6)
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); // 【修改】配置TIM4的输出比较通道2 (PB7)
	// 如果需要，可以类似地初始化通道3 (PB8) 和通道4 (PB9)，使用 TIM_OC3Init 和 TIM_OC4Init

	/*TIM使能*/
	TIM_Cmd(TIM4, ENABLE); // 【修改】使能TIM4
}

/**
 * 函    数：PWM设置CCR1 (对应TIM4_CH1, PB6)
 * 参    数：Compare 要写入的CCR1的值
 * 返 回 值：无
 */
void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM4, Compare); // 【修改】设置TIM4的CCR1的值
}

/**
 * 函    数：PWM设置CCR2 (对应TIM4_CH2, PB7)
 * 参    数：Compare 要写入的CCR2的值
 * 返 回 值：无
 */
void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM4, Compare); // 【修改】设置TIM4的CCR2的值
}