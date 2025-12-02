#include "stepper.h"
#include "stdio.h"

int GANTRY_ZONE = 6; // move gantry to leftmost position or update this variable to its respective zone

void microDelay (uint16_t delay, TIM_HandleTypeDef* tim) // Note: may need to change thumb timer channel to channel 2 or 3
{
  __HAL_TIM_SET_COUNTER(tim, 0);
  while (__HAL_TIM_GET_COUNTER(tim) < delay);
}


void step (int steps, uint8_t direction, uint16_t delay, TIM_HandleTypeDef* tim)
{
  int x;
  if (direction == 0)
    HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);
  for(x=0; x<steps; x=x+1)
  {
    HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_SET);
    microDelay(delay, tim);
    HAL_GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);
    microDelay(delay, tim);
  }
}


void move_to_zone(int target_zone, TIM_HandleTypeDef* tim) { // target zone can be 0 to 7
	if (target_zone > GANTRY_ZONE) {
		for (int i = 0; i < target_zone - GANTRY_ZONE; i++) {
		 step(1900, 0, 500, tim); // move left
		 microDelay(100, tim);
		}
	}
	else if (target_zone < GANTRY_ZONE) {
		for (int i = 0; i < GANTRY_ZONE - target_zone; i++) {
			 step(1900, 1, 500, tim); // move right
			 microDelay(100, tim);
		}
	}
	printf("Gantry moving from zone %d to zone %d\r\n", GANTRY_ZONE, target_zone);
	GANTRY_ZONE = target_zone;
}
