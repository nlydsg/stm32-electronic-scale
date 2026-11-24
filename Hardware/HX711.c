#include "HX711.h"
#include "Delay.h"

void HX711_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // SCK (PB12) 推挽输出
    GPIO_InitStructure.GPIO_Pin = HX711_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HX711_SCK_PORT, &GPIO_InitStructure);
    
    // DOUT (PB13) 浮空输入
    GPIO_InitStructure.GPIO_Pin = HX711_DOUT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HX711_DOUT_PORT, &GPIO_InitStructure);
    
    HX711_SCK_LOW();
}

uint32_t HX711_Read(void)
{
    uint32_t value = 0;
    uint8_t i;
    
    while(HX711_DOUT_READ());
    
    for(i = 0; i < 24; i++)
    {
        HX711_SCK_HIGH();
        Delay_us(1);
        value = value << 1;
        if(HX711_DOUT_READ())
        {
            value++;
        }
        HX711_SCK_LOW();
        Delay_us(1);
    }
    
    HX711_SCK_HIGH();
    Delay_us(1);
    HX711_SCK_LOW();
    Delay_us(1);
    
    if(value & 0x800000)
    {
        value |= 0xFF000000;
    }
    
    return value;
}

int32_t HX711_GetAverageValue(uint8_t times)
{
    int32_t sum = 0;
    uint8_t i;
    
    for(i = 0; i < times; i++)
    {
        sum += (int32_t)HX711_Read();
        Delay_ms(10);
    }
    
    return sum / times;
}
