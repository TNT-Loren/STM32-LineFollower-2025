#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include "Motor.h"

#define BASE_SPEED  3

int Keep_Ok=0, Yaw_ok=0;
int HD[7];
char HD_State;
float HD_Out,HD_Out1,HD_Out2;

void HD_Init(void)
{
    /*开启时钟*/
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 注意：这里需要同时开启AFIO时钟
    /* 关键步骤：禁用JTAG调试功能，释放PB3、PB4、PA15引脚 */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    // 也可以使用 GPIO_Remap_SWJ_Disable 来禁用所有调试功能（JTAG+SWD），但这样会导致后续无法下载程序，不推荐。
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*GPIO初始化*/

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

void HD_Read(void)
{
    HD[0] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
    HD[1] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3);
    HD[2] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
    HD[3] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
    HD[4] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
    HD[5] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
    HD[6] = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
    HD_State = HD[0] | HD[1] <<1| HD[2] <<2| HD[3] <<3| HD[4] <<4| HD[5] <<5| HD[6] <<6;
    if (HD[6] == 1)
        Keep_Ok = 1;
    if (HD_State==0x20)
        Yaw_ok = 1;
}


void HD_Process(void)
{
    int i;
    HD_Read();
    switch(HD_State)
    {
        case 0x08:
            i = 0;
            break;
        case 0x04:
            i = 1;
            break;
        case 0x06:
            i = 2;
            break;
        case 0x02: //		0000 0010
            i = 3;
            break;
        case 0x03: //		0000 0011
            i = 4;
            break;
        case 0x01: //
            i = 5;
            break;

        case 0x10:             //       0001 0000
            i = -1; // 小车现在处于左转状态
            break;
        case 0x18: //			0001 1000
            i = -2;
            break;
        case 0x20: // 				0010 0000
            i = -3;
            break;
        case 0x30: //		0011 0000
            i = -4;
            break;
        case 0x40: // 				0100 0000
            i = -5;
            break;

        default:
            i = 0; // 默认前进
            break;
    }
    HD_Out = PID_realize(&PidHD,i);
    HD_Out1 = BASE_SPEED+HD_Out;
    if (HD_Out1 > 6)
        HD_Out1 = 6;
    if (HD_Out1 < 0)
        HD_Out1 = 0;
    HD_Out2 = BASE_SPEED - HD_Out;
    if (HD_Out2 > 6)
        HD_Out2 = 6;
    if (HD_Out2 < 0)
        HD_Out2 = 0;
    MotorPidSet(HD_Out1, HD_Out2);
}


