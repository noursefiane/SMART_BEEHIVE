#include "dht11.h"

#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_PIN_9

static void DHT11_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

HAL_StatusTypeDef DHT11_Read(DHT11_Data *data)
{
    uint8_t bits[5] = {0};

    /* Send start signal: pull LOW for 18 ms, then release */
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(18);
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    delay_us(30);

    DHT11_SetInput();

    /* Wait for sensor response: LOW ~80 us then HIGH ~80 us */
    delay_us(40);
    if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET)
        return HAL_ERROR;
    delay_us(80);
    if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET)
        return HAL_ERROR;
    delay_us(80);

    /* Read 40 bits (5 bytes), MSB first */
    for(int i = 0; i < 5; i++) {
        for(int j = 7; j >= 0; j--) {
            while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET);
            delay_us(30);
            if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
                bits[i] |= (1 << j);
                while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET);
            }
        }
    }

    /* Checksum verification */
    if(bits[4] != ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF))
        return HAL_ERROR;

    data->humidity    = bits[0];
    data->temperature = bits[2];

    return HAL_OK;
}
