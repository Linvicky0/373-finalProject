#include "grideye.h"

void GridEye_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t data;

    // Wake up from sleep mode
    data = 0x00;  // Normal mode
    HAL_I2C_Mem_Write(hi2c, GRIDEYE_ADDR, POWER_CONTROL_REG,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    HAL_Delay(50);  // Wait for sensor to wake up

    // Set frame rate to 10 FPS
    data = 0x00;  // 10 FPS (default)
    HAL_I2C_Mem_Write(hi2c, GRIDEYE_ADDR, FRAMERATE_REG,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

void GridEye_ReadPixels(I2C_HandleTypeDef *hi2c, float temps[64]) {
    uint8_t raw_data[128];  // 64 pixels × 2 bytes each

    // Read all 64 pixels at once (burst read from 0x80)
    HAL_I2C_Mem_Read(hi2c, GRIDEYE_ADDR, PIXEL_BASE_REG,
                     I2C_MEMADD_SIZE_8BIT, raw_data, 128, HAL_MAX_DELAY);

    // Convert each pixel to degrees Celsius
    for (int i = 0; i < 64; i++) {
        int16_t raw = (raw_data[i*2 + 1] << 8) | raw_data[i*2];

        // Sign extend from 12-bit to 16-bit
        if (raw & 0x800) {
            raw |= 0xF000;  // Negative temperature
        }

        // Convert to celsius (LSB = 0.25°C)
        temps[i] = raw * 0.25f;
    }
}

float GridEye_ReadThermistor(I2C_HandleTypeDef *hi2c) {
    uint8_t raw_data[2];

    HAL_I2C_Mem_Read(hi2c, GRIDEYE_ADDR, THERMISTOR_REG,
                     I2C_MEMADD_SIZE_8BIT, raw_data, 2, HAL_MAX_DELAY);

    int16_t raw = (raw_data[1] << 8) | raw_data[0];

    // Sign extend from 12-bit
    if (raw & 0x800) {
        raw |= 0xF000;
    }

    return raw * 0.0625f;  // LSB = 0.0625°C
}
