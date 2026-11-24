#ifndef __MATRIXKEY_H
#define __MATRIXKEY_H

#include "stm32f10x.h"

// 矩阵按键引脚定义
// 行线：PA7-PA4 (输出)
#define ROW1_PIN    GPIO_Pin_7   // 行1
#define ROW2_PIN    GPIO_Pin_6   // 行2
#define ROW3_PIN    GPIO_Pin_5   // 行3
#define ROW4_PIN    GPIO_Pin_4   // 行4

// 列线：PA3-PA0 (输入)
#define COL1_PIN    GPIO_Pin_3   // 列1
#define COL2_PIN    GPIO_Pin_2   // 列2
#define COL3_PIN    GPIO_Pin_1   // 列3
#define COL4_PIN    GPIO_Pin_0   // 列4

// 按键值定义
#define KEY_NONE    0
#define KEY_1       1
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_7       7
#define KEY_8       8
#define KEY_9       9
#define KEY_0       10   // 保持10不变
#define KEY_CLEAR   11
#define KEY_TARE    12
#define KEY_ZERO    13
#define KEY_PRICE1  14
#define KEY_PRICE2  15
#define KEY_PRICE3  16

void MatrixKey_Init(void);
uint8_t MatrixKey_Scan(void);

#endif
