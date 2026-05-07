#ifndef SHT31_H
#define SHT31_H

#include "stm32l4xx_hal.h"

#define SHT31_ADDR (0x44 << 1)

typedef struct
{
    float temperature;
    float humidity;
} SHT31_Data;

HAL_StatusTypeDef SHT31_StartMeasure(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef SHT31_ReadData(I2C_HandleTypeDef *hi2c, SHT31_Data *data);

#endif
