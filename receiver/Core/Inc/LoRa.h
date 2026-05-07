#ifndef LORA_H
#define LORA_H

#include "stm32l4xx_hal.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

void LoRa_Init(void);
void LoRa_Receive(void);

#endif
