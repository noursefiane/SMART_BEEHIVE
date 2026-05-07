#include "lora.h"
#include <string.h>
#include <stdio.h>

static void sendAT(const char *cmd)
{
    HAL_UART_Transmit(&huart4, (uint8_t*)cmd, strlen(cmd), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    HAL_Delay(2000);
    uint8_t buf[64]; uint8_t byte; uint8_t i = 0;
    while(HAL_UART_Receive(&huart4, &byte, 1, 200) == HAL_OK && i < 63) buf[i++] = byte;
    if(i > 0) HAL_UART_Transmit(&huart2, buf, i, 1000);
}

void LoRa_Init(void)
{
    sendAT("AT\r\n");
    sendAT("AT+MODE=TEST\r\n");
    sendAT("AT+TEST=RFCFG,868,SF7,125,8,8,14,ON,OFF,OFF\r\n");
}

void LoRa_Send(float ext_temp, float ext_hum, float int_temp, float int_hum, float weight)
{
    char cmd[128];
    snprintf(cmd, sizeof(cmd),
             "AT+TEST=TXLRSTR,\"EXT:%.1fC,%.1f%% INT:%.1fC,%.1f%% W:%.2fkg\"\r\n",
             ext_temp, ext_hum, int_temp, int_hum, weight);
    HAL_UART_Transmit(&huart4, (uint8_t*)cmd, strlen(cmd), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 1000);
    HAL_Delay(2000);
}

void LoRa_Receive(void)
{
    uint8_t buf[64]; uint8_t byte; uint8_t i = 0;
    while(HAL_UART_Receive(&huart4, &byte, 1, 200) == HAL_OK && i < 63) buf[i++] = byte;
    if(i > 0) HAL_UART_Transmit(&huart2, buf, i, 1000);
}
