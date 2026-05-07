#ifndef HX711_H
#define HX711_H

#include "stm32l4xx_hal.h"

#define HX711_SCK_PORT   GPIOC
#define HX711_SCK_PIN    GPIO_PIN_0

#define HX711_DOUT_PORT  GPIOC
#define HX711_DOUT_PIN   GPIO_PIN_1

#define HX711_SCALE      217.209f
#define HX711_OFFSET     530986

void    HX711_Init(void);
int32_t HX711_ReadRaw(void);
float   HX711_ReadKg(void);

#endif
