#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "lcd.h"
#include "dht11.h"
#include "lora.h"
#include "sht31.h"
#include "hx711.h"
#include <stdio.h>

SHT31_Data sht31;
DHT11_Data dht11;

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_UART4_Init();

  /* Enable DWT cycle counter for microsecond delays */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  rgb_lcd lcd;
  lcd_init(&hi2c1, &lcd);
  LoRa_Init();
  HX711_Init();

  while (1)
  {
    if(SHT31_StartMeasure(&hi2c1) == HAL_OK)
    {
      HAL_Delay(20);
      if(SHT31_ReadData(&hi2c1, &sht31) == HAL_OK)
      {
        char buffer[16];

        clearlcd();
        lcd_position(&hi2c1, 0, 0);
        lcd_print(&hi2c1, "External");
        sprintf(buffer, "T:%.1fC H:%.1f%%", sht31.temperature, sht31.humidity);
        lcd_position(&hi2c1, 0, 1);
        lcd_print(&hi2c1, buffer);
        HAL_Delay(3000);

        if(DHT11_Read(&dht11) == HAL_OK)
        {
          clearlcd();
          lcd_position(&hi2c1, 0, 0);
          lcd_print(&hi2c1, "Internal");
          sprintf(buffer, "T:%.1fC H:%.1f%%", dht11.temperature, dht11.humidity);
          lcd_position(&hi2c1, 0, 1);
          lcd_print(&hi2c1, buffer);
          HAL_Delay(3000);

          float weight = HX711_ReadKg();
          clearlcd();
          lcd_position(&hi2c1, 0, 0);
          lcd_print(&hi2c1, "Weight");
          sprintf(buffer, "%.3f kg", weight);
          lcd_position(&hi2c1, 0, 1);
          lcd_print(&hi2c1, buffer);
          HAL_Delay(3000);

          LoRa_Send(sht31.temperature, sht31.humidity,
                    dht11.temperature, dht11.humidity, weight);
        }
      }
    }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    Error_Handler();

  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = 1;
  RCC_OscInitStruct.PLL.PLLN            = 10;
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    Error_Handler();

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                   | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    Error_Handler();
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif
