#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

extern uint8_t Key_Short, Key_Long;

void Key_Init(void);
uint8_t Key_State(void);
void Key_Loop(void);

#endif
