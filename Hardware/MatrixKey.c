#include "MatrixKey.h"
#include "Delay.h"

// 初始化矩阵按键
void MatrixKey_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置行线为推挽输出 (PA7-PA4)
    GPIO_InitStructure.GPIO_Pin = ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置列线为下拉输入 (PA3-PA0)
    GPIO_InitStructure.GPIO_Pin = COL1_PIN | COL2_PIN | COL3_PIN | COL4_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // 下拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始化所有行线为低电平
    GPIO_ResetBits(GPIOA, ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN);
}

// 扫描按键，返回按键值
uint8_t MatrixKey_Scan(void)
{
    uint8_t row, col;
    uint16_t row_pins[4] = {ROW1_PIN, ROW2_PIN, ROW3_PIN, ROW4_PIN};
    uint16_t col_pins[4] = {COL1_PIN, COL2_PIN, COL3_PIN, COL4_PIN};
    
    // 按键映射表 - 根据你的实际布局
    uint8_t key_map[4][4] = {
        {KEY_PRICE1, KEY_1, KEY_4, KEY_7},      // 行1: 10, 01, 04, 07
        {KEY_PRICE2, KEY_2, KEY_5, KEY_8},      // 行2: 14, 02, 05, 08
        {KEY_PRICE3, KEY_3, KEY_6, KEY_9},      // 行3: 15, 03, 06, 09
        {KEY_ZERO, KEY_CLEAR, KEY_TARE, KEY_0}  // 行4: 16, 13, 12, 11
    };
    
    for(row = 0; row < 4; row++)
    {
        // 将当前行设置为高电平
        GPIO_SetBits(GPIOA, row_pins[row]);
        Delay_us(50);
        
        // 检测所有列
        for(col = 0; col < 4; col++)
        {
            if(GPIO_ReadInputDataBit(GPIOA, col_pins[col]) == Bit_SET)
            {
                Delay_ms(20);  // 消抖
                if(GPIO_ReadInputDataBit(GPIOA, col_pins[col]) == Bit_SET)
                {
                    // 等待按键释放
                    while(GPIO_ReadInputDataBit(GPIOA, col_pins[col]) == Bit_SET);
                    Delay_ms(10);
                    
                    GPIO_ResetBits(GPIOA, row_pins[row]);
                    return key_map[row][col];
                }
            }
        }
        
        GPIO_ResetBits(GPIOA, row_pins[row]);
    }
    
    return KEY_NONE;
}
