#ifndef DHT11_H
#define DHT11_H

#include "stm32l4xx_hal.h"

typedef struct {
    float temperature;
    float humidity;
} DHT11_Data;

HAL_StatusTypeDef DHT11_Read(DHT11_Data *data);

#endif
