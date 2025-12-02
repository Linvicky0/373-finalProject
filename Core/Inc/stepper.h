/*
 * stepper.h

 */


#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define DIR_PIN GPIO_PIN_1
#define DIR_PORT GPIOA
#define STEP_PIN GPIO_PIN_2
#define STEP_PORT GPIOA

extern int GANTRY_ZONE;

void microDelay (uint16_t delay, TIM_HandleTypeDef* tim);

void step (int steps, uint8_t direction, uint16_t delay, TIM_HandleTypeDef* tim);

void move_to_zone(int target_zone, TIM_HandleTypeDef* tim); // target zone can be 0 to 7

#endif
