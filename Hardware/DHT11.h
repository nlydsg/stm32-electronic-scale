#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"

#define DHT11_PORT      GPIOB
#define DHT11_PIN       GPIO_Pin_14

#define DHT11_HIGH()    GPIO_SetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_LOW()     GPIO_ResetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_READ()    GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)

typedef struct {
    uint8_t humidity_int;
    uint8_t humidity_dec;
    uint8_t temperature_int;
    uint8_t temperature_dec;
    uint8_t checksum;
} DHT11_Data;

void DHT11_Init(void);
uint8_t DHT11_IsBusy(void);
void DHT11_TriggerRead(void);
uint8_t DHT11_ReadDataNonBlocking(DHT11_Data *data);

#endif
