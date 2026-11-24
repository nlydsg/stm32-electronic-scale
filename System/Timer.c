#include "timer.h"
#include "HX711.h"
#include "MatrixKey.h"

// 定时器2相关变量（HX711）
volatile uint32_t hx711_ready = 0;
volatile int32_t latest_weight_ad = 0;
volatile uint8_t reading_in_progress = 0;

// 定时器3相关变量（DHT11）
volatile uint8_t dht11_trigger = 0;
volatile uint8_t dht11_reading_in_progress = 0;
volatile uint8_t dht11_read_complete = 0;

// 定时器2中断处理（HX711，100ms一次）
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        
        // 如果上一次读取已完成，启动新的读取
        if(!reading_in_progress)
        {
            reading_in_progress = 1;
            hx711_ready = 1;
        }
    }
}

// 定时器3中断处理（DHT11，2秒一次）
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        // 如果上一次读取已完成，启动新的DHT11读取
        if(!dht11_reading_in_progress)
        {
            dht11_reading_in_progress = 1;
            dht11_trigger = 1;
        }
    }
}

void Timer2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 定时器2配置，72MHz/7200=10kHz，计数1000次=100ms中断一次
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 使能定时器中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    // 设置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

void Timer3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // 定时器3配置，72MHz/7200=10kHz，计数20000次=2秒中断一次
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 使能定时器中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    // 设置中断优先级（比定时器2低）
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

uint8_t Timer2_GetHX711Ready(void)
{
    uint8_t ret = hx711_ready;
    if(ret)
    {
        hx711_ready = 0;
    }
    return ret;
}

void Timer2_SetReadingComplete(void)
{
    reading_in_progress = 0;
}

uint8_t Timer3_GetDHT11Trigger(void)
{
    uint8_t ret = dht11_trigger;
    if(ret)
    {
        dht11_trigger = 0;
    }
    return ret;
}

void Timer3_SetDHT11Complete(void)
{
    dht11_reading_in_progress = 0;
    dht11_read_complete = 1;
}

uint8_t Timer3_IsDHT11Reading(void)
{
    return dht11_reading_in_progress;
}
