#include "sht31.h"

HAL_StatusTypeDef SHT31_StartMeasure(I2C_HandleTypeDef *hi2c)
{
    /* High-repeatability single-shot measurement command */
    uint8_t cmd[2] = {0x24, 0x00};
    return HAL_I2C_Master_Transmit(hi2c, SHT31_ADDR, cmd, 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef SHT31_ReadData(I2C_HandleTypeDef *hi2c, SHT31_Data *data)
{
    uint8_t buffer[6];

    if(HAL_I2C_Master_Receive(hi2c, SHT31_ADDR, buffer, 6, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    /* buffer[0..1] = raw temperature, buffer[3..4] = raw humidity */
    uint16_t rawTemp = (buffer[0] << 8) | buffer[1];
    uint16_t rawHum  = (buffer[3] << 8) | buffer[4];

    /* Datasheet conversion formulas */
    data->temperature = -45 + 175 * ((float)rawTemp / 65535.0);
    data->humidity    = 100 * ((float)rawHum / 65535.0);

    return HAL_OK;
}
