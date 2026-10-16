#ifndef __SERIAL3_H
#define __SERIAL3_H

#include "stm32f10x.h"
#include <stdio.h>

extern float Roll, Pitch, Yaw;

extern uint8_t Serial3_TxPacket[4];
extern uint8_t Serial3_RxPacket[9];
extern uint8_t Serial3_RxFlag;

void Serial3_Init(void);
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String);
void Serial3_SendNumber(uint32_t Number, uint8_t Length);
void Serial3_Printf(char *format, ...);

void Serial3_SendPacket(void);
uint8_t Serial3_GetRxFlag(void);
void Serial_JY60_Zero_Yaw(void);

#endif
