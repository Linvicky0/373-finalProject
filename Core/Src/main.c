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
#include "vl53l7cx_api.h"
#include <stdio.h>
//#include "network.h"
//#include "network_data.h"
//#include "network_data_params.h"
//#include "ai_platform.h"

#include "knowledge.h"
#include "NanoEdgeAI.h"
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef hlpuart1;

/* USER CODE BEGIN PV */
VL53L7CX_Configuration Dev;
VL53L7CX_ResultsData Results;
uint8_t isAlive, status, isReady, i;

// AI Network variables
//ai_handle network = AI_HANDLE_NULL;
//ai_buffer ai_input[AI_NETWORK_IN_NUM];
//ai_buffer ai_output[AI_NETWORK_OUT_NUM];
//ai_float input_data[AI_NETWORK_IN_1_SIZE];  // 128 floats (8x8x2)
//ai_float output_data[AI_NETWORK_OUT_1_SIZE]; // 8 floats (8 classes) // Activation buffer (memory for neural network)

//AI_ALIGNED(4) ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];
// Hand posture class names
//const char* class_names[] = {
//		"No Hand Detection",  // We need to figure out what each index means
//		"Open Hand",
//		"Class 2",
//		"Class 3",
//		"Class 4",
//		"Class 5",
//		"Class 6",
//		"Class 7"
//};

typedef enum {
    GESTURE_NONE = 0,
    GESTURE_ROCK,
    GESTURE_PAPER,
    GESTURE_SCISSORS
} RPS_Gesture;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_LPUART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
	 HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);  // Changed from huart3
	    return ch;
}

#define MAX_VALID_DIST_MM 10000U  /* sanity threshold for picking correct endian */

void fill_buffer(float sample_buffer[]) {
    uint8_t isReady = 0;

    // Wait for sensor data to be ready
    do {
        status = vl53l7cx_check_data_ready(&Dev, &isReady);
        HAL_Delay(10);  // Small delay to prevent busy-waiting
    } while(!isReady);

    // Get ranging data from sensor
    status = vl53l7cx_get_ranging_data(&Dev, &Results);

    if(status == 0) {
        // Fill buffer with distance data (64 zones)
        for(int i = 0; i < 64; i++) {
            sample_buffer[i] = (float)Results.distance_mm[i]/9.0;
        }

//        // If using 2 axes, add signal data
//        #if AXIS_NUMBER == 2
//        for(int i = 0; i < 64; i++) {
//            sample_buffer[64 + i] = (float)Results.signal_per_spad[i];
//        }
//        #endif
//
        // Debug: Print a few values to verify
        printf("Sample data: [0]=%.2f [10]=%.2f [20]=%.2f [30]=%.2f\r\n",
               sample_buffer[0], sample_buffer[10], sample_buffer[20], sample_buffer[30]);
    } else {
        printf("Error reading sensor: %d\r\n", status);
    }
}

//void prepare_network_input(VL53L7CX_ResultsData *results, ai_float *input) {
//    int i;
//
//    // Channel 0: Distance data (normalized)
//    for(i = 0; i < 64; i++) {
//        // Normalize distance: closer = higher value
//        // Invert so that close objects have high values
//        if(results->distance_mm[i] < 50) {
//            input[i] = 0.0f;  // Too close, probably noise
//        } else if(results->distance_mm[i] > 400) {
//            input[i] = 0.0f;  // Too far, no object
//        } else {
//            // Map 50-400mm to 1.0-0.0 (closer = higher value)
//            input[i] = 1.0f - ((ai_float)results->distance_mm[i] - 50.0f) / 350.0f;
//        }
//    }
//
//    // Channel 1: Use signal strength (better than just status)
//    for(i = 0; i < 64; i++) {
//        // Normalize signal per SPAD
//        if(results->target_status[i] == 5 || results->target_status[i] == 9) {
//            // Valid detection
//            input[64 + i] = (ai_float)results->signal_per_spad[i] / 10000.0f;
//        } else {
//            input[64 + i] = 0.0f;
//        }
//    }
//}

//RPS_Gesture detect_rock_paper_scissors(VL53L7CX_ResultsData *results) {
//    int active_zones = 0;
//    int left_zones = 0, right_zones = 0, center_zones = 0;
//    int min_dist = 9999, max_dist = 0;
//    int total_dist = 0;
//
//    // Analyze 8x8 grid
//    for(int i = 0; i < 64; i++) {
//        int dist = results->distance_mm[i];
//
//        // Count zones with hand detected (50-300mm range)
//        if(dist > 50 && dist < 300 && results->target_status[i] == 5) {
//            active_zones++;
//            total_dist += dist;
//            if(dist < min_dist) min_dist = dist;
//            if(dist > max_dist) max_dist = dist;
//
//            // Track left/center/right distribution
//            int col = i % 8;
//            if(col < 3) left_zones++;
//            else if(col > 4) right_zones++;
//            else center_zones++;
//        }
//    }
//
//    // Need at least some zones active
//    if(active_zones < 5) return GESTURE_NONE;
//
//    int depth_variation = max_dist - min_dist;
//
//    // ROCK: Small, compact area (10-25 zones)
//    if(active_zones >= 10 && active_zones <= 25 && depth_variation < 60) {
//        return GESTURE_ROCK;
//    }
//
//    // PAPER: Large area (50+ zones), relatively flat
//    if(active_zones >= 50 && depth_variation < 80) {
//        return GESTURE_PAPER;
//    }
//
//    // SCISSORS: Split pattern - activity on left AND right, gap in center
//    if(left_zones >= 5 && right_zones >= 5 && center_zones < 8 && active_zones >= 15 && active_zones <= 35) {
//        return GESTURE_SCISSORS;
//    }
//
//    return GESTURE_NONE;
//}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

	  //ai_error err;
	  //ai_network_params params;

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
  MX_I2C1_Init();
  MX_LPUART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE BEGIN 2 */

  // Setup platform
  Dev.platform.address = 0x52;


  // Reset sensor
  HAL_GPIO_WritePin(LPn_C_GPIO_Port, LPn_C_Pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(LPn_C_GPIO_Port, LPn_C_Pin, GPIO_PIN_SET);
  HAL_Delay(100);

  printf("Starting...\r\n");

  // Check if sensor is alive

  vl53l7cx_is_alive(&Dev, &isAlive);
  printf("isAlive = %d\r\n", isAlive);

  if (isAlive) {
      printf("Initializing sensor...\r\n");
      vl53l7cx_init(&Dev);
      vl53l7cx_set_resolution(&Dev, VL53L7CX_RESOLUTION_8X8);
      vl53l7cx_set_ranging_frequency_hz(&Dev, 15);
      vl53l7cx_start_ranging(&Dev);
//
//      // Initialize AI Network
//      printf("Initializing AI network...\r\n");
//
//      // Create network
//      err = ai_network_create(&network, AI_NETWORK_DATA_CONFIG);
//      if (err.type != AI_ERROR_NONE) {
//          printf("Error creating network: %d\r\n", err.code);
//          Error_Handler();
//      }
//
//      // Initialize network with activation buffer
//      params.params = AI_NETWORK_DATA_WEIGHTS(ai_network_data_weights_get());
//      params.activations = AI_NETWORK_DATA_ACTIVATIONS(activations);
//
//      if (!ai_network_init(network, &params)) {
//          printf("Error initializing network!\r\n");
//          Error_Handler();
//      }
//
//      // Get input/output buffers from the network
//      ai_input[0] = *(ai_network_inputs_get(network, NULL));
//      ai_input[0].data = AI_HANDLE_PTR(input_data);
//
//      ai_output[0] = *(ai_network_outputs_get(network, NULL));
//      ai_output[0].data = AI_HANDLE_PTR(output_data);
//
//      printf("AI network ready!\r\n");
//      printf("Ready!\r\n");
      }

  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
//  while(1)
//  {
//      uint8_t isReady = 0;
//      ai_i32 batch;
//      int max_class, j;
//      float max_confidence;
//
//      status = vl53l7cx_check_data_ready(&Dev, &isReady);
//
//      if(isReady)
//      {
//          status = vl53l7cx_get_ranging_data(&Dev, &Results);
//
//          if(status == 0)
//          {
//
//        	  int active_zones = 0;
//			  for(int j = 0; j < 64; j++) {
//				  if(Results.distance_mm[j] < 400 && Results.distance_mm[j] > 50) {
//					  active_zones++;
//				  }
//			  }
//			  printf("Active zones: %d\r\n", active_zones);
//              // Prepare input data
//              prepare_network_input(&Results, input_data);
//
//              printf("Input sample: %.2f, %.2f, %.2f, %.2f\r\n",
//              input_data[0], input_data[1], input_data[2], input_data[3]);
//
//              // Run inference
//              batch = ai_network_run(network, ai_input, ai_output);
//
//              if (batch != 1) {
//                  printf("Error running inference!\r\n");
//              } else {
//                  // Print all class confidences
//                  printf("Confidences: ");
//                  for(j = 0; j < 8; j++) {
//                      printf("[%d]=%.2f ", j, output_data[j]);
//                  }
//                  printf("\r\n");
//
//                  // Find class with highest confidence
//                  max_class = 0;
//                  max_confidence = output_data[0];
//
//                  for(j = 1; j < 8; j++) {
//                      if(output_data[j] > max_confidence) {
//                          max_confidence = output_data[j];
//                          max_class = j;
//                      }
//                  }
//
//                  // Print result if confidence is high enough
//                  if(max_confidence > 0.5) {  // Lower threshold to 0.5
//                      printf(">>> Detected: %s (confidence: %.2f)\r\n",
//                             class_names[max_class], max_confidence);
//                  }
//              }
//          }
//      }
//      HAL_Delay(1000);



  /* USER CODE BEGIN WHILE */
	float input_user_buffer[DATA_INPUT_USER * AXIS_NUMBER];
	float output_class_buffer[CLASS_NUMBER];

  enum neai_state error_code = neai_classification_init(knowledge);
  if (error_code != NEAI_OK) {
	  printf("NanoEdgeAI init error: %d\r\n", error_code);
  }
  uint16_t id_class = 0;
  const char* class_names[] = {
         "Unknown",        // After testing, update this
         "Unknown1",       // Scissors?
         "Unknown2",    // Rock?
         "Unknown3",     // Paper?
         "No Sign"      // Class 4 confirmed
     };
  while(1)
  {
	  fill_buffer(input_user_buffer);
	  enum neai_state run_code;
	  run_code = neai_classification(input_user_buffer, output_class_buffer, &id_class);
	  if (run_code == NEAI_OK) {
		  printf("Detected: %s (ID: %d)\r\n", class_names[id_class], id_class);
	  }
	  HAL_Delay(500);
  }
//      uint8_t isReady = 0;
//      RPS_Gesture gesture;
//
//      // Check if sensor has new data
//      status = vl53l7cx_check_data_ready(&Dev, &isReady);
//
//      if(isReady)
//      {
//          // Get the ranging data - THIS IS THE IMPORTANT PART!
//          status = vl53l7cx_get_ranging_data(&Dev, &Results);
//
//          if(status == 0)
//          {
//              // Now Results is filled with data, detect gesture
//              gesture = detect_rock_paper_scissors(&Results);
//
//              switch(gesture) {
//                  case GESTURE_ROCK:
//                      printf("🪨 ROCK!\r\n");
//                      break;
//                  case GESTURE_PAPER:
//                      printf("📄 PAPER!\r\n");
//                      break;
//                  case GESTURE_SCISSORS:
//                      printf("✂️ SCISSORS!\r\n");
//                      break;
//                  default:
//                      // No gesture detected
//                      break;
//              }
//          }
//          else {
//              printf("Error reading sensor data: %d\r\n", status);
//          }
//      }
//
//      HAL_Delay(100);  // Small delay between readings
//  }
  /* USER CODE END WHILE */
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00000508;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_DISABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

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
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PWR_EN_C_Pin|LPn_C_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|GPIO_PIN_3|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PWR_EN_C_Pin LD3_Pin PB3 LPn_C_Pin
                           LD2_Pin */
  GPIO_InitStruct.Pin = PWR_EN_C_Pin|LD3_Pin|GPIO_PIN_3|LPn_C_Pin
                          |LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

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
