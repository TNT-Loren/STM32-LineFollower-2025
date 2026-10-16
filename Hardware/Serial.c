#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

float Roll, Pitch, Yaw;

// =============================== 全局变量 ===============================
uint8_t Serial3_TxPacket[4];            // 发送数据包（4 字节，格式：FF 01 02 03 04 FE）
uint8_t Serial3_RxPacket[9];            // 接收数据包（9 字节）
uint8_t Serial3_RxFlag;                 // 接收数据包标志位（1=收到数据包）

// =============================== 函数声明 ===============================
void Serial3_Init(void);
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String);
void Serial3_SendNumber(uint32_t Number, uint8_t Length);
void Serial3_SendPacket(void);
uint8_t Serial3_GetRxFlag(void);
void USART3_IRQHandler(void);
void Serial3_Printf(char *format, ...);
void canshu_get(void);
uint32_t Serial3_Pow(uint32_t X, uint32_t Y);  // ? 提前声明 Serial3_Pow

// =============================== USART3 初始化 ===============================
void Serial3_Init(void)
{
    /* 1. 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  // USART3 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   // GPIOB 时钟（PB10/TX, PB11/RX）

    /* 2. GPIO 初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PB10 (TX) → 复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB11 (RX) → 浮空输入（或上拉输入）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 或 GPIO_Mode_IPU（上拉输入）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 3. USART3 初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;              // 波特率 9600
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // 收发模式
    USART_InitStructure.USART_Parity = USART_Parity_No;     // 无奇偶校验
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 1 位停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8 位数据位
    USART_Init(USART3, &USART_InitStructure);

    /* 4. 中断配置（接收中断） */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  // 使能接收中断

    /* 5. NVIC 中断配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // 分组 2
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  // USART3 中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 抢占优先级 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         // 响应优先级 1
    NVIC_Init(&NVIC_InitStructure);

    /* 6. 使能 USART3 */
    USART_Cmd(USART3, ENABLE);
}

// =============================== 发送函数 ===============================
void Serial3_SendByte(uint8_t Byte)
{
    USART_SendData(USART3, Byte);  // 发送字节
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);  // 等待发送完成
}

void Serial3_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        Serial3_SendByte(Array[i]);  // 逐个发送字节
    }
}

void Serial3_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        Serial3_SendByte(String[i]);  // 逐个发送字符
    }
}

void Serial3_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        Serial3_SendByte(Number / Serial3_Pow(10, Length - i - 1) % 10 + '0');
    }
}

void Serial3_SendPacket(void)
{
    Serial3_SendByte(0xFF);          // 包头 FF
    Serial3_SendArray(Serial3_TxPacket, 4);  // 4 字节数据
    Serial3_SendByte(0xFE);          // 包尾 FE
}

// =============================== 接收函数 ===============================
uint8_t Serial3_GetRxFlag(void)
{
    if (Serial3_RxFlag == 1)
    {
        Serial3_RxFlag = 0;
        return 1;
    }
    return 0;
}

// =============================== 中断函数（USART3） ===============================
void USART3_IRQHandler(void)
{
    static uint8_t RxState = 0;
    static uint8_t pRxPacket = 0;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        uint8_t RxData = USART_ReceiveData(USART3);

        if (RxState == 0)
        {
            if (RxData == 0x55)  // 包头 0x55
            {
                RxState = 1;
                pRxPacket = 0;
            }
        }
        else if (RxState == 1)
        {
            if (RxData == 0x53)  // 包头 0x53
            {
                RxState = 2;
                pRxPacket = 0;
            }
        }
        else if (RxState == 2)
        {
            Serial3_RxPacket[pRxPacket] = RxData;
            pRxPacket++;
            if (pRxPacket >= 9)  // 收够 9 字节
            {
                RxState = 0;
                Serial3_RxFlag = 1;
                canshu_get();  // 你的数据处理函数
            }
        }

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

// =============================== 辅助函数 ===============================
uint32_t Serial3_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

void Serial3_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial3_SendString(String);
}

// =============================== 自定义数据处理函数 ===============================
void canshu_get(void)
{
    Roll = ((uint16_t)((uint16_t)Serial3_RxPacket[1] << 8 | (uint16_t)Serial3_RxPacket[0])) / 32768.0f * 180.0f;
    Pitch = ((uint16_t)((uint16_t)Serial3_RxPacket[3] << 8 | (uint16_t)Serial3_RxPacket[2])) / 32768.0f * 180.0f;
    Yaw = ((uint16_t)((uint16_t)Serial3_RxPacket[5] << 8 | (uint16_t)Serial3_RxPacket[4])) / 32768.0f * 180.0f;
    // 将 Yaw 转换到 -180 到 +180 度范围
    if (Yaw > 180)
    {
        Yaw -= 360;
    }
}

void Serial_JY60_Zero_Yaw(void)
{
    Serial3_SendByte(0xFF);
    Serial3_SendByte(0xAA);
    Serial3_SendByte(0x52);
}
