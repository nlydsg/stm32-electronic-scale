#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "HX711.h"
#include "MatrixKey.h"
#include "timer.h"
#include "DHT11.h"
#include "LED.h"
#include <stdio.h>
#include <string.h>

// 全局变量
int32_t HX711_Offset = 0;
float Weight = 0.0;
float Price = 10.0;
float Total = 0.0;

// 预设单价（可修改）
float PRICE1 = 10.0;
float PRICE2 = 20.0;
float PRICE3 = 30.0;

// 重量读取缓冲
volatile int32_t raw_ad_value = 0;
volatile uint8_t new_data_ready = 0;

// DHT11数据
DHT11_Data dht_data;
volatile uint8_t dht_data_ready = 0;
uint8_t dht_read_fail_count = 0;

// 单价输入缓冲区
uint8_t price_input[5] = {0};
uint8_t price_input_index = 0;
uint8_t price_edit_mode = 0;

// 函数声明
void HX711_CalibrateZero(void);
void UpdateDisplay(void);
void ProcessKey(uint8_t key);
void InputPriceNumber(uint8_t num);
void ConfirmPriceInputTo(uint8_t price_index);
void ClearPriceInput(void);
void ReadHX711InMain(void);
void ReadDHT11InMain(void);

int main(void)
{
    OLED_Init();
    HX711_Init();
    MatrixKey_Init();
    Timer2_Init();
    Timer3_Init();
    DHT11_Init();
    LED_Init();
    
    BUZZ_ON();
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    
    OLED_ShowString(1, 1, "Calibrating...");
    HX711_CalibrateZero();
    Delay_ms(1000);
    OLED_Clear();
    
    BUZZ_OFF();
		LED1_OFF();
    
    while (1)
    {
        ReadHX711InMain();
        ReadDHT11InMain();
        
        if(new_data_ready)
        {
            new_data_ready = 0;
            Weight = (float)(raw_ad_value - HX711_Offset) / 200.43;
            if(Weight < 0) Weight = 0;
            Total = (Weight / 1000.0) * Price;
        }
        
        uint8_t key = MatrixKey_Scan();
        if(key != KEY_NONE)
        {
            ProcessKey(key);
        }
        
        UpdateDisplay();
        Delay_ms(10);
    }
}

void ReadHX711InMain(void)
{
    static uint8_t reading_state = 0;
    static uint32_t read_value = 0;
    static uint8_t bit_count = 0;
    
    if(Timer2_GetHX711Ready())
    {
        reading_state = 1;
        read_value = 0;
        bit_count = 0;
    }
    
    if(reading_state == 1)
    {
        if(HX711_DOUT_READ() == 0)
        {
            reading_state = 2;
        }
    }
    else if(reading_state == 2)
    {
        for(; bit_count < 24; bit_count++)
        {
            HX711_SCK_HIGH();
            Delay_us(1);
            read_value = read_value << 1;
            if(HX711_DOUT_READ()) read_value++;
            HX711_SCK_LOW();
            Delay_us(1);
        }
        reading_state = 3;
    }
    else if(reading_state == 3)
    {
        HX711_SCK_HIGH();
        Delay_us(1);
        HX711_SCK_LOW();
        Delay_us(1);
        
        if(read_value & 0x800000) read_value |= 0xFF000000;
        
        raw_ad_value = (int32_t)read_value;
        new_data_ready = 1;
        reading_state = 0;
        Timer2_SetReadingComplete();
    }
}

void ReadDHT11InMain(void)
{
    static uint8_t dht_reading = 0;
    
    if(Timer3_GetDHT11Trigger())
    {
        if(!DHT11_IsBusy())
        {
            DHT11_TriggerRead();
            dht_reading = 1;
        }
    }
    
    if(dht_reading)
    {
        if(DHT11_ReadDataNonBlocking(&dht_data))
        {
            dht_data_ready = 1;
            dht_read_fail_count = 0;
            dht_reading = 0;
            Timer3_SetDHT11Complete();
            BUZZ_OFF();
        }
        else if(!DHT11_IsBusy())
        {
            dht_read_fail_count++;
            if(dht_read_fail_count >= 3)
            {
                dht_data_ready = 0;
            }
            dht_reading = 0;
            Timer3_SetDHT11Complete();
        }
    }
}

void UpdateDisplay(void)
{
    char buf[17];
    
    // 第1行：显示重量
    int32_t w_int = (int32_t)Weight;
    int32_t w_dec = (int32_t)(Weight * 10) % 10;
    sprintf(buf, "W:%4d.%1dg", w_int, w_dec);
    OLED_ShowString(1, 1, buf);
    
    // 第2行：显示温湿度
    if(dht_data_ready)
    {
				LED1_OFF();
        sprintf(buf, "T:%2dC H:%2d%%", dht_data.temperature_int, dht_data.humidity_int-100);
        OLED_ShowString(2, 1, buf);
    }
    else
    {
        OLED_ShowString(2, 1, "T:--C H:--%");
				LED1_ON();
    }
    
    // 第3行：显示单价/编辑
    if(price_edit_mode)
    {
        OLED_ShowString(3, 1, "P:    ");
        if(price_input_index > 0)
        {
            for(int i = 0; i < price_input_index; i++)
            {
                OLED_ShowNum(3, 3 + i, price_input[i], 1);
            }
            OLED_ShowString(3, 3 + price_input_index, "_");
        }
        else
        {
            OLED_ShowString(3, 3, "_");
        }
    }
    else
    {
        sprintf(buf, "P:%4.1fY", Price);
        OLED_ShowString(3, 1, buf);
    }
    
    // 第4行：显示总价和预设单价
    sprintf(buf, "T:%4.1fY", Total);
    OLED_ShowString(4, 1, buf);
}

void ProcessKey(uint8_t key)
{
    // 数字键 1-9
    if(key >= KEY_1 && key <= KEY_9)
    {
        uint8_t num = key;
        if(price_edit_mode)
        {
            InputPriceNumber(num);
        }
        else
        {
            Price = (float)num;
        }
    }
    // 数字键 0
    else if(key == KEY_0)
    {
        if(price_edit_mode)
        {
            InputPriceNumber(0);
        }
        else
        {
            Price = 0.0;
        }
    }
    // 清除键
    else if(key == KEY_CLEAR)
    {
        if(price_edit_mode)
        {
            ClearPriceInput();
        }
        else
        {
            price_edit_mode = 1;
            price_input_index = 0;
            memset(price_input, 0, sizeof(price_input));
        }
    }
    // 去皮/置零
    else if(key == KEY_TARE || key == KEY_ZERO)
    {
        int32_t sum = 0;
        for(int i = 0; i < 20; i++)
        {
            while(HX711_DOUT_READ());
            sum += (int32_t)HX711_Read();
            Delay_ms(10);
        }
        HX711_Offset = sum / 20;
    }
    // 单价1键
    else if(key == KEY_PRICE1)
    {
        if(price_edit_mode)
        {
            ConfirmPriceInputTo(1);
        }
        else
        {
            Price = PRICE1;
        }
    }
    // 单价2键
    else if(key == KEY_PRICE2)
    {
        if(price_edit_mode)
        {
            ConfirmPriceInputTo(2);
        }
        else
        {
            Price = PRICE2;
        }
    }
    // 单价3键
    else if(key == KEY_PRICE3)
    {
        if(price_edit_mode)
        {
            ConfirmPriceInputTo(3);
        }
        else
        {
            Price = PRICE3;
        }
    }
}

void InputPriceNumber(uint8_t num)
{
    if(price_input_index < 4)
    {
        price_input[price_input_index] = num;
        price_input_index++;
    }
}

void ConfirmPriceInputTo(uint8_t price_index)
{
    float new_price = 0;
    
    for(int i = 0; i < price_input_index; i++)
    {
        new_price = new_price * 10 + price_input[i];
    }
    
    if(new_price > 99.99) new_price = 99.99;
    
    if(price_input_index > 0)
    {
        if(price_index == 1)
        {
            PRICE1 = new_price;
        }
        else if(price_index == 2)
        {
            PRICE2 = new_price;
        }
        else if(price_index == 3)
        {
            PRICE3 = new_price;
        }
        
        Price = new_price;
    }
    
    price_edit_mode = 0;
    price_input_index = 0;
    memset(price_input, 0, sizeof(price_input));
}

void ClearPriceInput(void)
{
    price_input_index = 0;
    memset(price_input, 0, sizeof(price_input));
}

void HX711_CalibrateZero(void)
{
    int32_t sum = 0;
    for(int i = 0; i < 20; i++)
    {
        while(HX711_DOUT_READ());
        sum += (int32_t)HX711_Read();
        Delay_ms(50);
    }
    HX711_Offset = sum / 20;
}
