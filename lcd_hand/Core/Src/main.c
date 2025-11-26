/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"
/* BSP LCD driver */
#include "Lcd/stm32_ili9488_lcd.h"

/* BSP TS driver */
#include "Lcd/stm32_ili9488_ts.h"
#include "game.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
extern LCD_DrvTypeDef *lcd_drv;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
void mainApp(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

struct GameInfo prevMatch = {NOGAME, NOCHOICE, NOCHOICE};
uint16_t xboxsize;

GameResult getGameResult(Gesture userChoice, Gesture robotChoice) {
    if (userChoice == robotChoice) {
       return TIED;
    }
    else if ((userChoice == ROCK && robotChoice == SCISSORS) || (userChoice == PAPER && robotChoice == ROCK) || (userChoice == SCISSORS && robotChoice == PAPER)) {
      return USER_WINS;
    }
    return ROBOT_WINS;
}

uint16_t* getBitMap(Gesture choice) {
    uint16_t* bitmap;
    switch (choice){
    case ROCK:
       bitmap = rockBitMap;
       break;
    case SCISSORS:
        bitmap = scissorBitMap;
        break;
    default:
        bitmap = paperBitMap;
    }
    return bitmap;
}

void displayText() {
	// font24 has 6 lines (0-5)
	  int resultTextLine = 5;

	if (prevMatch.res == NOGAME) {
		return;
	}
	BSP_LCD_ClearStringLine(resultTextLine);
	if (prevMatch.res == TIED) {
		BSP_LCD_DisplayStringAtLine(resultTextLine, (uint8_t*)"Stalemate");
	}
	else if (prevMatch.res == USER_WINS) {
		BSP_LCD_DisplayStringAtLine(resultTextLine, (uint8_t*)"You win!");
	}
	else if (prevMatch.res == ROBOT_WINS){
		BSP_LCD_DisplayStringAtLine(resultTextLine, (uint8_t*)"You lost");
	}
}
// Change graphics and result text to reflect current match info.
void displayLCD(struct GameInfo curMatch)
{

  uint16_t lcd_height = BSP_LCD_GetYSize();
  if (prevMatch.res != curMatch.res) {
	  prevMatch.res = curMatch.res;
	  displayText();
  }
  uint16_t* userBitmap = getBitMap(curMatch.userChoice);
  uint16_t* robotBitmap = getBitMap(curMatch.robotChoice);
  if (prevMatch.userChoice != curMatch.userChoice) {
	  prevMatch.userChoice = curMatch.userChoice;
	  BSP_LCD_DrawRGB16Image(0, lcd_height/2, 100, 100, (uint16_t*)userBitmap);
  }
  if (prevMatch.robotChoice != curMatch.robotChoice) {
	  prevMatch.robotChoice = curMatch.robotChoice;
	  BSP_LCD_DrawRGB16Image(BSP_LCD_GetXSize()-100, BSP_LCD_GetYSize()/2, 100, 100, (uint16_t*)robotBitmap);
  }
}


uint16_t XPT2046_ReadValue(uint8_t cmd) // command used to read x, y, or z value
{
	    uint8_t tx[3] = {cmd, 0x00, 0x00};  // Command + 2 dummy bytes
	    uint8_t rx[3] = {0};

	    // Set CS low to start communication
	    HAL_GPIO_WritePin(TS_CS_GPIO_Port, TS_CS_Pin, GPIO_PIN_RESET);

	    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 3, HAL_MAX_DELAY);

	    // Set CS high to end communication
	    HAL_GPIO_WritePin(TS_CS_GPIO_Port, TS_CS_Pin, GPIO_PIN_SET);

	    // Combine the two response bytes into a 12-bit value
	    uint16_t value = ((rx[1] << 8) | rx[2]) >> 3;  // 12-bit ADC
	    return value;
}


void HAL_GPIO_EXTI_Callback(uint16_t pin) {
	if (pin == TS_IRQ_Pin) {
		// disable interrupt be setting it high because reading would cause handler to trigger again
		 HAL_NVIC_DisableIRQ(EXTI0_IRQn); // NOTE: change this if we changed exti number for touch detection
		 while (HAL_GPIO_ReadPin(TS_IRQ_GPIO_Port, TS_IRQ_Pin) == GPIO_PIN_RESET);

		// get x, y points
		 uint16_t x_raw, y_raw;

		  x_raw = XPT2046_ReadValue(0xD1);  // X command (D1?)  1(___)00(00)
		  y_raw = XPT2046_ReadValue(0x91);  // Y command (91?)
		  uint16_t curTextcolor = BSP_LCD_GetTextColor();
		  if (curTextcolor == LCD_COLOR_MAGENTA) {
			  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		  }
		  else {
			  BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
		  }
         displayText();
         __HAL_GPIO_EXTI_CLEAR_IT(TS_IRQ_Pin);
         HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);

		 HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	}
}


void move_hand(Gesture robotMove)
{
	//  CCR 50 = 0 degrees, CCR 150 = 90 degrees, CCR 250 = 180 degrees
  if (prevMatch.robotChoice != robotMove) {
	  if (robotMove == ROCK) { // thumb ccr to 250, other timers' CCR set to 50
			  while (htim4.Instance->CCR2 > 50 || htim4.Instance->CCR4 > 50 || htim1.Instance->CCR4 < 250) {
			  		if (htim4.Instance->CCR2 > 50) {
			  		 __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, htim4.Instance->CCR2-1);
			  		}
			  		if (htim4.Instance->CCR4 > 50) {
			  		 __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, htim4.Instance->CCR4-1);
			  		}
			  		if (htim1.Instance->CCR4 < 250) {
				  	 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, htim1.Instance->CCR4+1);
			  		}
			  		HAL_Delay(20);
			  }
		  }
		  else if (robotMove == PAPER) { // thumb ccr to 50, other timers' CCR set to 250
			  while (htim4.Instance->CCR2 < 250 || htim4.Instance->CCR4 < 250 || htim1.Instance->CCR4 > 50) {
				  if (htim4.Instance->CCR2 < 250) {
					  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, htim4.Instance->CCR2+1);
				  }
				  if (htim4.Instance->CCR4 < 250) {
					  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, htim4.Instance->CCR4+1);
				  }
				  if (htim1.Instance->CCR4 > 50) {
					 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, htim1.Instance->CCR4-1);
				  }
				  HAL_Delay(20);
			  }
	  }
	  else if (robotMove == SCISSORS) {
		  // thumb ccr => 250
		  // tim4ch2 represents index and middle fingers -> set CCR to 50
		  // tim4ch4 CCR set to 250
		  while (htim4.Instance->CCR2 < 250 || htim4.Instance->CCR4 > 50 || htim1.Instance->CCR4 < 250) {
		  		if (htim4.Instance->CCR2 < 250) {
		  			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, htim4.Instance->CCR2+1);
		  		}
		  		if (htim4.Instance->CCR4 > 50) {
		  			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, htim4.Instance->CCR4-1);
		  		}
		  	    if (htim1.Instance->CCR4 < 250) {
		  			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, htim1.Instance->CCR4+1);
		  		}
		  		HAL_Delay(20);
		  	}
	  }
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  // Start timers for hand: start off with hand position
   HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);  // index and middle fingers: CCR=250 is stretched, CCR=50 is curled
   HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // thumb finger: CCR=50 is stretched, CCR=250 is curled

  uint16_t lcd_width = BSP_LCD_GetXSize();
  uint16_t lcd_height = BSP_LCD_GetYSize();
  BSP_LCD_Init();
  BSP_TS_Init(lcd_width, lcd_height);
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  xboxsize = lcd_width / 6; // box size is 320/6 => 53

  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_FillRect(0, 0, xboxsize, xboxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_FillRect(xboxsize, 0, xboxsize, xboxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  BSP_LCD_FillRect(xboxsize * 2, 0, xboxsize, xboxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  BSP_LCD_FillRect(xboxsize * 3, 0, xboxsize, xboxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(xboxsize * 4, 0, xboxsize, xboxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
  BSP_LCD_FillRect(xboxsize * 5, 0, xboxsize, xboxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE); // set initial text color to white
  BSP_LCD_DrawRect(0, 0, xboxsize, xboxsize);

  while (htim4.Instance->CCR2 < 150 || htim4.Instance->CCR4 < 150 || htim1.Instance->CCR4 < 150) {
			  if (htim4.Instance->CCR2 < 150) {
				  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, htim4.Instance->CCR2+1);
			  }
			  if (htim4.Instance->CCR4 < 150) {
				  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, htim4.Instance->CCR4+1);
			  }
			  if (htim1.Instance->CCR4 < 150) {
				 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, htim1.Instance->CCR4+1);
			  }
			  HAL_Delay(20);
  }

  HAL_Delay(5000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 srand(HAL_GetTick());

    int userPick = rand() % 3;
    int robotPick = rand() % 3;
    Gesture userGesture = (Gesture)userPick;
    Gesture robotGesture = (Gesture)robotPick;
    GameResult res = getGameResult(userGesture, robotGesture);
    struct GameInfo matchInfo = {res, userGesture, robotGesture};
    move_hand(robotGesture);
    displayLCD(matchInfo);
    HAL_Delay(2000); // wait 2 seconds before starting another match
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 79;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 150;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 79;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 150;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 150;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TS_CS_GPIO_Port, TS_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TS_IRQ_Pin */
  GPIO_InitStruct.Pin = TS_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TS_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin TS_CS_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|TS_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : STLK_RX_Pin STLK_TX_Pin */
  GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : STLINK_TX_Pin STLINK_RX_Pin */
  GPIO_InitStruct.Pin = STLINK_TX_Pin|STLINK_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RS_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_RS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
