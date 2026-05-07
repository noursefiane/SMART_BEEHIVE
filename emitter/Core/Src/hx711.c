#include "hx711.h"

static void delay_us_hx(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

void HX711_Init(void)
{
    /* SCK must be LOW in idle state */
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
}

int32_t HX711_ReadRaw(void)
{
    /* Wait for DOUT to go LOW (data ready), with 500 ms timeout */
    uint32_t timeout = HAL_GetTick();
    while (HAL_GPIO_ReadPin(HX711_DOUT_PORT, HX711_DOUT_PIN) == GPIO_PIN_SET)
    {
        if (HAL_GetTick() - timeout > 500)
            return 0;
    }

    int32_t value = 0;

    /* Clock in 24 bits, MSB first */
    for (int i = 0; i < 24; i++)
    {
        HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
        delay_us_hx(1);
        value = (value << 1) | HAL_GPIO_ReadPin(HX711_DOUT_PORT, HX711_DOUT_PIN);
        HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
        delay_us_hx(1);
    }

    /* 25th pulse: selects channel A with gain 128 for next conversion */
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
    delay_us_hx(1);
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
    delay_us_hx(1);

    /* Sign-extend from 24 to 32 bits */
    if (value & 0x800000)
        value |= 0xFF000000;

    return value;
}

float HX711_ReadKg(void)
{
    int32_t raw = HX711_ReadRaw();
    return (float)(raw - HX711_OFFSET) / HX711_SCALE / 1000.0f;
}
