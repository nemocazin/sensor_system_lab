/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_ROWS 4
#define NUM_COLS 4
#define DEBOUNCE_TIME 5

#define ROW_PORT GPIOC
#define COL_PORT GPIOC

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

uint8_t RX_byte = 0;


uint8_t keyPressed[4][4];
uint32_t press_ack[4][4];

const char keyMap[4][4] = {
    {'0', '1', '2', '3'},
    {'4', '5', '6', '7'},
    {'8', '9', 'A', 'B'},
    {'C', 'D', 'E', 'F'}
};

uint8_t col = 0;
uint16_t row_pins [NUM_ROWS] = {GPIO_PIN_3,GPIO_PIN_2,GPIO_PIN_13,GPIO_PIN_12};

uint8_t baud_flag = 0;

//Different matrix
uint8_t matrix[5][2];

volatile uint8_t matrix_QM[5][2] = {
	  {32,16},	// y0 content {row}, address {column}
	  {64, 8},	// y1
	  {69, 4},	// y2
	  {72, 2},	// y3
	  {48, 1}
};


volatile uint8_t matrix_A[5][2] = {
	  {31,16},
	  {36, 8},
	  {68, 4},
	  {36, 2},
	  {31, 1}
};

volatile uint8_t matrix_B[5][2] = {
	  {127,16},
	  {73, 8},
	  {73, 4},
	  {85, 2},
	  {99, 1}
};
volatile uint8_t matrix_C[5][2] = {
	  {62,16},
	  {65, 8},
	  {65, 4},
	  {65, 2},
	  {34, 1}
};

volatile uint8_t matrix_D[5][2] = {
	  {127,16},
	  {65, 8},
	  {65, 4},
	  {65, 2},
	  {62, 1}
};
volatile uint8_t matrix_E[5][2] = {
	  {127,16},
	  {73, 8},
	  {73, 4},
	  {73, 2},
	  {9, 1}
};
volatile uint8_t matrix_F[5][2] = {
	  {127,16},
	  {72, 8},
	  {72, 4},
	  {72, 2},
	  {72, 1}
};
volatile uint8_t matrix_0[5][2] = {
	  {127,16},
	  {65, 8},
	  {65, 4},
	  {65, 2},
	  {127, 1}
};
volatile uint8_t matrix_1[5][2] = {
	  {0,16},
	  {33, 8},
	  {127, 4},
	  {1, 2},
	  {0, 1}
};
volatile uint8_t matrix_2[5][2] = {
	  {39,16},
	  {73, 8},
	  {73, 4},
	  {73, 2},
	  {49, 1}
};
volatile uint8_t matrix_3[5][2] = {
	  {65,16},
	  {73, 8},
	  {73, 4},
	  {73, 2},
	  {127, 1}
};
volatile uint8_t matrix_4[5][2] = {
	  {8,16},
	  {24, 8},
	  {40, 4},
	  {127, 2},
	  {8, 1}
};
volatile uint8_t matrix_5[5][2] = {
	  {99,16},
	  {81, 8},
	  {73, 4},
	  {73, 2},
	  {79, 1}
};
volatile uint8_t matrix_6[5][2] = {
	  {127,16},
	  {73, 8},
	  {73, 4},
	  {73, 2},
	  {15, 1}
};
volatile uint8_t matrix_7[5][2] = {
	  {67,16},
	  {68, 8},
	  {72, 4},
	  {80, 2},
	  {96, 1}
};
volatile uint8_t matrix_8[5][2] = {
	  {127,16},
	  {73, 8},
	  {73, 4},
	  {73, 2},
	  {127, 1}
};
volatile uint8_t matrix_9[5][2] = {
	  {121,16},
	  {73, 8},
	  {73, 4},
	  {73, 2},
	  {127, 1}
};
uint8_t column_index = 0;

uint8_t receive_flag = 0;
volatile uint8_t char_to_show = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM10_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM11_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);

		column_index++;
		if(column_index > 4) column_index = 0;
	}
}



void ShowLetter()
{
	if(char_to_show != '\n')
	{
		switch(char_to_show)
		{
			case '0':
				memcpy(matrix, (const void *)matrix_0,sizeof(matrix));
				break;
			case '1':
				memcpy(matrix, (const void *)matrix_1,sizeof(matrix));
				break;
			case '2':
				memcpy(matrix, (const void *)matrix_2,sizeof(matrix));
				break;
			case '3':
				memcpy(matrix, (const void *)matrix_3,sizeof(matrix));
				break;
			case '4':
				memcpy(matrix, (const void *)matrix_4,sizeof(matrix));
				break;
			case '5':
				memcpy(matrix, (const void *)matrix_5,sizeof(matrix));
				break;
			case '6':
				memcpy(matrix, (const void *)matrix_6,sizeof(matrix));
				break;
			case '7':
				memcpy(matrix, (const void *)matrix_7,sizeof(matrix));
				break;
			case '8':
				memcpy(matrix, (const void *)matrix_8,sizeof(matrix));
				break;
			case '9':
				memcpy(matrix, (const void *)matrix_9,sizeof(matrix));
				break;
			case 'A':
				memcpy(matrix, (const void *)matrix_A,sizeof(matrix));
				break;
			case 'B':
				memcpy(matrix, (const void *)matrix_B,sizeof(matrix));
				break;
			case 'C':
				memcpy(matrix, (const void *)matrix_C,sizeof(matrix));
				break;
			case 'D':
				memcpy(matrix, (const void *)matrix_D,sizeof(matrix));
				break;
			case 'E':
				memcpy(matrix, (const void *)matrix_E,sizeof(matrix));
				break;
			case 'F':
				memcpy(matrix, (const void *)matrix_F,sizeof(matrix));
				break;
			default:
				memcpy(matrix, (const void *)matrix_QM,sizeof(matrix));
				break;
		}
	}
}



/**
 * @brief Callback of UART
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// UART1
    if (huart->Instance == USART1)
    {
    	HAL_UART_Receive_IT(&huart1, &RX_byte, sizeof(RX_byte));
		receive_flag =1;
		char_to_show = RX_byte;

        HAL_UART_Transmit_DMA(&huart2, &RX_byte, sizeof(RX_byte));
    }

    // UART2
    if(huart->Instance == USART2)
    {
        HAL_UART_Receive_IT(&huart2, &RX_byte, 1);
        receive_flag =1;
        char_to_show = RX_byte;
	}
}



/**
 * @brief Send the byte via IR transmitter
 */
void UART_IR_Transmit(uint8_t tx_byte)
{
	// Enable baud rate interrupt timer
	HAL_TIM_Base_Start_IT(&htim3);

    // Send the start bit (0)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Enable PWM
    while (baud_flag == 0); // Wait for one bit period
    baud_flag = 0;

    // Send 8 data bits
	for(uint8_t bit_index = 0; bit_index <8 ; bit_index++)
	{
		// The Flag has been reset in the periodElapsedCallback of TIM10
		if(tx_byte &(0x01 << bit_index))
		{
			// We want to send a '1'
			// So we stop the PWM
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3); // UART reading 1
		}
		else
		{
			// We want to send a zero
			// So we have to enable the PWM
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // UART reading 0
		}
	    while (baud_flag == 0){} // Wait for one bit period
	    // Then we reset the flag
		baud_flag = 0;
	}

	// We need to send the STOP bit '1'
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3); // UART reading 1
	while (baud_flag == 0); // Wait for one bit period
	baud_flag = 0;

	HAL_TIM_Base_Stop_IT(&htim3);
}



/**
 * @brief Timers Callback
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// TIMER 10
    if(htim->Instance == TIM10)
    {
    	// Read row pins -
     	 for(uint8_t row = 0; row < 4; row++)
     	 {
     		 GPIO_PinState state = HAL_GPIO_ReadPin(ROW_PORT, row_pins[row]);
   			 if(state == GPIO_PIN_SET)
   			 {
				 keyPressed[row][col] = 0;
				 press_ack[row][col] = 0;
			 }
			 else // Key pressed
			 {
				 keyPressed[row][col]++;
			 }
       	 }
       	 col = (col + 1) % 4;
       	 HAL_GPIO_WritePin(COL_PORT, GPIO_PIN_8, (col== 0) ? GPIO_PIN_SET:GPIO_PIN_RESET);
       	 HAL_GPIO_WritePin(COL_PORT, GPIO_PIN_9, (col== 1) ? GPIO_PIN_SET:GPIO_PIN_RESET);
       	 HAL_GPIO_WritePin(COL_PORT, GPIO_PIN_10, (col== 2) ? GPIO_PIN_SET:GPIO_PIN_RESET);
       	 HAL_GPIO_WritePin(COL_PORT, GPIO_PIN_11, (col== 3) ? GPIO_PIN_SET:GPIO_PIN_RESET);
    }

    // TIMER 3 = Baudrate
	if (htim->Instance == TIM3) // 2400 bps bit-timing interrupt
	{
		baud_flag = 1;
    }

	// TIMER 11 = for MATRIX LED
    if(htim->Instance == TIM11)
    {
    	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)matrix[column_index], sizeof(matrix[column_index]));
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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM10_Init();
  MX_SPI1_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */

  //HAL_UART_Receive_IT(&huart2, &RX_byte, 1); // for debugging

  HAL_UART_Receive_IT(&huart1, &RX_byte, 1); // Start reception'


  if(HAL_TIM_Base_Start_IT(&htim10) != HAL_OK)
  {
   	  Error_Handler();
  }

  if(HAL_TIM_Base_Start_IT(&htim11) != HAL_OK)
  {
   	  Error_Handler();
  }

  memcpy(matrix, (const void *)matrix_0,sizeof(matrix));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(receive_flag)
	  {
		  ShowLetter();
		  receive_flag = 0;
	  }

	  for(uint8_t i=0; i<4; i++)
	  {
		  for(uint8_t j=0; j<4; j++)
		  {
			  if (keyPressed[i][j] > DEBOUNCE_TIME)
			  {
				  if (press_ack[i][j] == 0) // New valid keypress
				  {
					  press_ack[i][j] = 1;     // Mark key as processed
					  UART_IR_Transmit(keyMap[i][j]);

					  //for debugging
					  //HAL_UART_Transmit_DMA(&huart2, &keyMap[i][j], sizeof(keyMap[i][j]));
				}
			  }
		  }
	  }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2210;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1105;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 34999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 83;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 4999;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 83;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 99;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 2400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC2 PC3 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 PC10 PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

#ifdef  USE_FULL_ASSERT
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
