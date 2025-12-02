/*
 * grideye.h
 *
 *  Created on: Nov 25, 2025
 *      Author: kchens
 */

#ifndef GRIDEYE_H
#define GRIDEYE_H

#include "stm32l4xx_hal.h"

#define GRIDEYE_ADDR (0x69 << 1)  // 0xD0

// Register addresses
#define POWER_CONTROL_REG    0x00
#define RESET_REG            0x01
#define FRAMERATE_REG        0x02
#define THERMISTOR_REG       0x0E
#define PIXEL_BASE_REG       0x80
#define FTEMP_THRESHOLD      77
// Function prototypes
void GridEye_Init(I2C_HandleTypeDef *hi2c);
void GridEye_ReadPixels(I2C_HandleTypeDef *hi2c, float temps[64]);
float GridEye_ReadThermistor(I2C_HandleTypeDef *hi2c);
int find_hottest_col(I2C_HandleTypeDef *hi2c);

#endif
