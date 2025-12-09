#include "grideye.h"
#include "stdio.h"


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
        temps[i] = (temps[i] * (9.0/5.0)) + 32;
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

int find_hottest_col(I2C_HandleTypeDef *hi2c) {

	float pixel_temps[64];
	float column_vals[8];
	for (int i = 0; i < 8; i++) {
		column_vals[i] = 0;
	}
	GridEye_ReadPixels(hi2c, pixel_temps);

	for (int i = 0; i < 8; i++) {
		 for (int j = 0; j < 8; j++) {
		 	 column_vals[i] += pixel_temps[i + (j*8)];
             printf("    %.2f", pixel_temps[(i*8)+j]);
		 }
         printf("\r\n");
	}
	int max_col_val = 0;
	int max_index = 0;
	for (int i = 0; i < 8; i++) {
        printf("  %.2f", column_vals[i]);
		if (column_vals[i] > max_col_val) {
			max_col_val = column_vals[i];
		 	 max_index = i;
		}
	}
	int max_val_in_col = 0;
	for (int i = 0; i < 8; i++) {
		if (pixel_temps[max_index + (8*i)] > max_val_in_col) {
			max_val_in_col = pixel_temps[max_index + (8*i)];
		}
	}
   printf("  Max Index: %d\r\n",  max_val_in_col > FTEMP_THRESHOLD ? max_index : -1);
   return max_val_in_col > FTEMP_THRESHOLD ? max_index : -1; // ignore reading if summed column value is less than the temperature threshold
}

