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

void LoRa_Receive(void)
{
    uint8_t buf[128]; uint8_t byte; uint8_t i = 0;
    while(HAL_UART_Receive(&huart4, &byte, 1, 200) == HAL_OK && i < 127) buf[i++] = byte;
    if(i > 0)
    {
        /* Find the hex payload between the quotes in the +TEST: RX response */
        uint8_t *start = (uint8_t*)strchr((char*)buf, '"');
        if(start != NULL)
        {
            start++; /* Skip opening quote */
            uint8_t decoded[64]; uint8_t j = 0;
            while(*start && *start != '"' && j < 63)
            {
                /* Convert each pair of hex digits to one ASCII byte */
                uint8_t hi = *start++;
                uint8_t lo = *start++;
                hi = (hi >= 'A') ? hi - 'A' + 10 : hi - '0';
                lo = (lo >= 'A') ? lo - 'A' + 10 : lo - '0';
                decoded[j++] = (hi << 4) | lo;
            }
            decoded[j] = '\0';
            HAL_UART_Transmit(&huart2, decoded, j, 1000);
            HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
        }
    }
}
