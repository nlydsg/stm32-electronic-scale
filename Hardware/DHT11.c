#include "DHT11.h"
#include "Delay.h"

static uint8_t read_state = 0;
static uint8_t bit_count = 0;
static uint8_t byte_count = 0;
static uint8_t current_byte = 0;
static DHT11_Data temp_data;

void DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    
    DHT11_HIGH();
    read_state = 0;
}

uint8_t DHT11_IsBusy(void)
{
    return (read_state != 0);
}

void DHT11_TriggerRead(void)
{
    if(read_state == 0)
    {
        // 配置为输出模式
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
        
        // 发送起始信号
        DHT11_LOW();
        Delay_ms(18);
        DHT11_HIGH();
        Delay_us(30);
        
        // 切换为输入模式
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
        
        read_state = 1;
        bit_count = 0;
        byte_count = 0;
        current_byte = 0;
    }
}

uint8_t DHT11_ReadDataNonBlocking(DHT11_Data *data)
{
    static uint16_t timeout;
    
    if(read_state == 1)
    {
        // 等待DHT11响应
        timeout = 0;
        while(DHT11_READ() && timeout < 100)
        {
            Delay_us(10);
            timeout++;
        }
        if(timeout >= 100) { read_state = 0; return 0; }
        
        timeout = 0;
        while(!DHT11_READ() && timeout < 100)
        {
            Delay_us(10);
            timeout++;
        }
        if(timeout >= 100) { read_state = 0; return 0; }
        
        read_state = 2;
    }
    
    if(read_state == 2)
    {
        // 读取40位数据
        while(1)
        {
            // 等待高电平开始
            timeout = 0;
            while(!DHT11_READ() && timeout < 100)
            {
                Delay_us(10);
                timeout++;
            }
            if(timeout >= 100) { read_state = 0; return 0; }
            
            // 延时40us判断位值
            Delay_us(40);
            
            if(DHT11_READ())
            {
                current_byte |= (1 << (7 - bit_count));
            }
            
            bit_count++;
            
            // 等待低电平结束
            timeout = 0;
            while(DHT11_READ() && timeout < 100)
            {
                Delay_us(10);
                timeout++;
            }
            if(timeout >= 100) { read_state = 0; return 0; }
            
            if(bit_count >= 8)
            {
                // 保存当前字节
                switch(byte_count)
                {
                    case 0: temp_data.humidity_int = current_byte; break;
                    case 1: temp_data.humidity_dec = current_byte; break;
                    case 2: temp_data.temperature_int = current_byte; break;
                    case 3: temp_data.temperature_dec = current_byte; break;
                    case 4: temp_data.checksum = current_byte; break;
                }
                
                byte_count++;
                bit_count = 0;
                current_byte = 0;
                
                if(byte_count >= 5)
                {
                    // 校验
                    uint8_t sum = temp_data.humidity_int + temp_data.humidity_dec + 
                                  temp_data.temperature_int + temp_data.temperature_dec;
                    if(sum == temp_data.checksum)
                    {
                        *data = temp_data;
                        read_state = 0;
                        return 1;
                    }
                    read_state = 0;
                    return 0;
                }
            }
        }
    }
    
    return 0;
}
