#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void Timer2_Init(void);
uint8_t Timer2_GetHX711Ready(void);
void Timer2_SetReadingComplete(void);

// 添加定时器3相关函数
void Timer3_Init(void);
uint8_t Timer3_GetDHT11Trigger(void);
void Timer3_SetDHT11Complete(void);
uint8_t Timer3_IsDHT11Reading(void);

#endif
