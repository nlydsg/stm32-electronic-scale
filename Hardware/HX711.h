#ifndef __HX711_H
#define __HX711_H

#include "stm32f10x.h"

// HX711引脚定义
#define HX711_SCK_PORT    GPIOB
#define HX711_SCK_PIN     GPIO_Pin_12
#define HX711_DOUT_PORT   GPIOB
#define HX711_DOUT_PIN    GPIO_Pin_13

#define HX711_SCK_HIGH()  GPIO_SetBits(HX711_SCK_PORT, HX711_SCK_PIN)
#define HX711_SCK_LOW()   GPIO_ResetBits(HX711_SCK_PORT, HX711_SCK_PIN)
#define HX711_DOUT_READ() GPIO_ReadInputDataBit(HX711_DOUT_PORT, HX711_DOUT_PIN)

void HX711_Init(void);
uint32_t HX711_Read(void);
int32_t HX711_GetAverageValue(uint8_t times);

#endif
