/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define stepper_dir_Pin GPIO_PIN_1
#define stepper_dir_GPIO_Port GPIOA
#define stepper_step_Pin GPIO_PIN_2
#define stepper_step_GPIO_Port GPIOA
#define TS_CS_Pin GPIO_PIN_4
#define TS_CS_GPIO_Port GPIOA
#define LCDTS_SCK_Pin GPIO_PIN_5
#define LCDTS_SCK_GPIO_Port GPIOA
#define LCDTS_MISO_Pin GPIO_PIN_6
#define LCDTS_MISO_GPIO_Port GPIOA
#define LCDTS_MOSI_Pin GPIO_PIN_7
#define LCDTS_MOSI_GPIO_Port GPIOA
#define TS_IRQ_Pin GPIO_PIN_0
#define TS_IRQ_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_12
#define LCD_RST_GPIO_Port GPIOF
#define LCD_RS_Pin GPIO_PIN_15
#define LCD_RS_GPIO_Port GPIOE
#define thumb_signal_Pin GPIO_PIN_10
#define thumb_signal_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_14
#define LCD_CS_GPIO_Port GPIOD
#define ring_pinky_signals_Pin GPIO_PIN_15
#define ring_pinky_signals_GPIO_Port GPIOD
#define grideye_int_Pin GPIO_PIN_7
#define grideye_int_GPIO_Port GPIOD
#define grideye_int_EXTI_IRQn EXTI9_5_IRQn
#define index_middle_signals_Pin GPIO_PIN_7
#define index_middle_signals_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
