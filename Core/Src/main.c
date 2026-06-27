/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RC522.h"
#include "stdio.h"
#include "string.h"

#include "stm32f4xx_hal.h"
#include "liquidcrystal_i2c.h"
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

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* Definitions for RFID_Task */
osThreadId_t RFID_TaskHandle;
const osThreadAttr_t RFID_Task_attributes = {
  .name = "RFID_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LCD_Task */
osThreadId_t LCD_TaskHandle;
const osThreadAttr_t LCD_Task_attributes = {
  .name = "LCD_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Uart_Task */
osThreadId_t Uart_TaskHandle;
const osThreadAttr_t Uart_Task_attributes = {
  .name = "Uart_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Alarm_Task */
osThreadId_t Alarm_TaskHandle;
const osThreadAttr_t Alarm_Task_attributes = {
  .name = "Alarm_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for My_Queue */
osMessageQueueId_t My_QueueHandle;
const osMessageQueueAttr_t My_Queue_attributes = {
  .name = "My_Queue"
};
/* Definitions for My_Timer */
osTimerId_t My_TimerHandle;
const osTimerAttr_t My_Timer_attributes = {
  .name = "My_Timer"
};
/* Definitions for My_Mutex */
osMutexId_t My_MutexHandle;
const osMutexAttr_t My_Mutex_attributes = {
  .name = "My_Mutex"
};
/* Definitions for My_Semaphore */
osSemaphoreId_t My_SemaphoreHandle;
const osSemaphoreAttr_t My_Semaphore_attributes = {
  .name = "My_Semaphore"
};
/* USER CODE BEGIN PV */
uint8_t status;
uint8_t str[16];
uint8_t sNum[5];
uint32_t check = 0;

uint8_t card=0;

char *msg1 = "Reading from card\r\n";
char *msg2 = "Reading from tag\r\n";

char buffer2[100];
char buffer3[100];

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
void Start_RFID_Task(void *argument);
void Start_LCD_Task(void *argument);
void Start_Uart_Task(void *argument);
void Start_Alarm_Task(void *argument);
void My_Timer_Callback(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	MFRC522_Init();

	snprintf(buffer2,sizeof(buffer2),"first buffer2 : %d",2);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of My_Mutex */
  My_MutexHandle = osMutexNew(&My_Mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of My_Semaphore */
  My_SemaphoreHandle = osSemaphoreNew(1, 0, &My_Semaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of My_Timer */
  My_TimerHandle = osTimerNew(My_Timer_Callback, osTimerPeriodic, NULL, &My_Timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */

	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of My_Queue */
  My_QueueHandle = osMessageQueueNew (16, sizeof(uint32_t), &My_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of RFID_Task */
  RFID_TaskHandle = osThreadNew(Start_RFID_Task, NULL, &RFID_Task_attributes);

  /* creation of LCD_Task */
  LCD_TaskHandle = osThreadNew(Start_LCD_Task, NULL, &LCD_Task_attributes);

  /* creation of Uart_Task */
  Uart_TaskHandle = osThreadNew(Start_Uart_Task, NULL, &Uart_Task_attributes);

  /* creation of Alarm_Task */
  Alarm_TaskHandle = osThreadNew(Start_Alarm_Task, NULL, &Alarm_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  osTimerStart(My_TimerHandle, 10000);
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x1;
  sTime.Minutes = 0x2;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JUNE;
  sDate.Date = 0x22;
  sDate.Year = 0x26;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
void tapcard() {
	HD44780_Clear();
	HD44780_SetCursor(4, 0);
	HD44780_PrintStr("Tap Your");
	HD44780_SetCursor(5, 1);
	HD44780_PrintStr("Card !");
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_Start_RFID_Task */
/**
 * @brief  Function implementing the RFID_Task thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_Start_RFID_Task */
void Start_RFID_Task(void *argument)
{
  /* USER CODE BEGIN 5 */
	///char buffer[50];
	uint16_t c = 1;
	/* Infinite loop */
	for (;;) {
		if (c == 1) {
			tapcard();
		}


		status = MFRC522_Request(PICC_REQIDL, str);

		if (status == MI_OK) {
			c = 0;
			card++;

			status = MFRC522_Anticoll(sNum);
			osDelay(100);
			//HAL_UART_Transmit(&huart2, (uint8_t*) "Card Detected\r\n", 15,
			//HAL_MAX_DELAY);
			//sprintf(buffer, "UID: %d %d %d %d %d\r\n", sNum[0], sNum[1],
					//sNum[2], sNum[3], sNum[4]);

			//HAL_UART_Transmit(&huart2, (uint8_t*) buffer, strlen(buffer),
			//HAL_MAX_DELAY);
			if ((sNum[0] == 211) && (sNum[1] == 172) && (sNum[2] == 13)
					&& (sNum[3] == 228) && (sNum[4] == 150)) {
				//HAL_UART_Transmit(&huart2, (uint8_t*) msg1, strlen(msg1),HAL_MAX_DELAY);
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1);
				check = 1;
			} else if ((sNum[0] == 20) && (sNum[1] == 36) && (sNum[2] == 183)
					&& (sNum[3] == 167) && (sNum[4] == 32)) {
				//HAL_UART_Transmit(&huart2, (uint8_t*) msg2, strlen(msg2),HAL_MAX_DELAY);
				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
				check = 2;
			} else {
				//HAL_UART_Transmit(&huart2, (uint8_t*) "Else Part\r\n",strlen("Else Part\r\n"), HAL_MAX_DELAY);
				check = 3;
				card--;
				//continue;
			}
			//vTaskNotifyGiveFromISR(LCD_TaskHandle, NULL);
			xTaskNotifyGive(LCD_TaskHandle);
			//osDelay(500);
			osMessageQueuePut(My_QueueHandle, &check, 0, 0);
			//
			osSemaphoreRelease(My_SemaphoreHandle);



			//osDelay(200);
		}

		/*else
		 {
		 continue;
		 }*/

		osDelay(1000);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Start_LCD_Task */
/**
 * @brief Function implementing the LCD_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Start_LCD_Task */
void Start_LCD_Task(void *argument)
{
  /* USER CODE BEGIN Start_LCD_Task */

	//char msg[20];
	HD44780_Init(2);
	HD44780_Clear();
	HD44780_Backlight();

	//HD44780_Clear();
	/*HD44780_SetCursor(5, 0);
	HD44780_PrintStr("Welcome");
	HD44780_SetCursor(3, 1);
	HD44780_PrintStr("Employee 1");
	osDelay(1500);*/



	uint8_t x=0;
	uint8_t y=0;
	/* Infinite loop */
	for (;;) {

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		osMutexAcquire(My_MutexHandle, osWaitForever);


		//HAL_UART_Transmit(&huart2, (uint8_t*) "waiting\r\n",sizeof("waiting\r\n"), HAL_MAX_DELAY);

		osDelay(10);

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		snprintf(buffer2,sizeof(buffer2),"Date : %02d/%02d/%02d",sDate.Date,sDate.Month,sDate.Year);
		snprintf(buffer3,sizeof(buffer3),"Time : %02d/%02d/%02d",sTime.Hours,sTime.Minutes,sTime.Seconds);

		//HAL_UART_Transmit(&huart2, (uint8_t*) "LCD Task\r\n",sizeof("LCD Task\r\n"), HAL_MAX_DELAY);

		//sprintf(msg,"getcheck = %lu \r\n",getcheck);
		//HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

		if (check == 1) {
			//HAL_UART_Transmit(&huart2, (uint8_t*) "getcheck = 1\r\n",
					//sizeof("getcheck = 1\r\n"), HAL_MAX_DELAY);

			if (x == 1) {
				card--;
				HD44780_Clear();
				HD44780_SetCursor(3, 0);
				HD44780_PrintStr("Attendance");
				HD44780_SetCursor(1, 1);
				HD44780_PrintStr("already marked");
				osDelay(1500);
			}
			else{
				HD44780_Clear();
				HD44780_SetCursor(5, 0);
				HD44780_PrintStr("Welcome");
				HD44780_SetCursor(3, 1);
				HD44780_PrintStr("Employee 1");
				osDelay(1500);
				HD44780_Clear();
				HD44780_SetCursor(0, 0);
				HD44780_PrintStr(buffer3);
				HD44780_SetCursor(0, 1);
				HD44780_PrintStr(buffer2);

				osDelay(1500);
			}


			tapcard();


			x=1;

		}
		else if (check == 2) {

			if (y == 1) {
				card--;
				HD44780_Clear();
				HD44780_SetCursor(3, 0);
				HD44780_PrintStr("Attendance");
				HD44780_SetCursor(1, 1);
				HD44780_PrintStr("already marked");
				osDelay(1500);
			}
			else {

				HD44780_Clear();
				HD44780_SetCursor(5, 0);
				HD44780_PrintStr("Welcome");
				HD44780_SetCursor(3, 1);
				HD44780_PrintStr("Employee 2");
				osDelay(1500);
				HD44780_Clear();
				HD44780_SetCursor(0, 0);
				HD44780_PrintStr(buffer3);
				HD44780_SetCursor(0, 1);
				HD44780_PrintStr(buffer2);

				osDelay(1500);
			}
			tapcard();

			y=1;

		} else {
			HD44780_Clear();
			HD44780_SetCursor(5, 0);
			HD44780_PrintStr("Access");
			HD44780_SetCursor(4, 1);
			HD44780_PrintStr("Denied !");
			osDelay(2000);
			tapcard();
		}

		osDelay(100);

		osMutexRelease(My_MutexHandle);
		osDelay(100);

	}

  /* USER CODE END Start_LCD_Task */
}

/* USER CODE BEGIN Header_Start_Uart_Task */
/**
 * @brief Function implementing the Uart_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Start_Uart_Task */
void Start_Uart_Task(void *argument)
{
  /* USER CODE BEGIN Start_Uart_Task */
	//char msg[50];
	uint32_t getcheck=0;
	uint8_t m=0;
	uint8_t n=0;
	/* Infinite loop */

	for (;;) {
		osMutexAcquire(My_MutexHandle, osWaitForever);

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		snprintf(buffer2, sizeof(buffer2), "Date : %02d/%02d/%02d", sDate.Date,sDate.Month, sDate.Year);
		snprintf(buffer3,sizeof(buffer3),"Time : %02d/%02d/%02d",sTime.Hours,sTime.Minutes,sTime.Seconds);

		osMessageQueueGet(My_QueueHandle, &getcheck, NULL, osWaitForever);

		//HAL_UART_Transmit(&huart2, (uint8_t*) "Uart_task\r\n",sizeof("Uart_task\r\n"), HAL_MAX_DELAY);
		//sprintf(msg,"getcheck = %d \r\n",getcheck);

		//HAL_UART_Transmit(&huart2, (uint8_t*)msg,strlen(msg), HAL_MAX_DELAY);



		//sprintf(msg, "getcheck=%d n=%d\r\n", getcheck, n);
		//HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		if (getcheck == 1 && m==0) {

			HAL_UART_Transmit(&huart2, (uint8_t*)"Employee 1\r\n",sizeof("Employee 1\r\n"), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) buffer3, strlen(buffer3), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) "\r\n", sizeof("\r\n"),HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) buffer2, strlen(buffer2), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) "\r\n", sizeof("\r\n"),HAL_MAX_DELAY);
			m=1;

		} else if (getcheck == 2 && n==0) {

			HAL_UART_Transmit(&huart2, (uint8_t*)"Employee 2\r\n",sizeof("Employee 2\r\n"), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) buffer3, strlen(buffer3), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) "\r\n", sizeof("\r\n"),HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) buffer2, strlen(buffer2), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, (uint8_t*) "\r\n", sizeof("\r\n"),HAL_MAX_DELAY);
			n=1;
		}
		else if(getcheck == 3){
			HAL_UART_Transmit(&huart2, (uint8_t*)"Access Denied !\r\n",sizeof("Access Denied !\r\n"), HAL_MAX_DELAY);
		}
		else{
			HAL_UART_Transmit(&huart2, (uint8_t*)"Attendance already marked\r\n",sizeof("Attendance already marked\r\n"), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, (uint8_t*) "\r\n", sizeof("\r\n"),HAL_MAX_DELAY);

		osDelay(100);

		osMutexRelease(My_MutexHandle);
		osDelay(100);
	}
  /* USER CODE END Start_Uart_Task */
}

/* USER CODE BEGIN Header_Start_Alarm_Task */
/**
 * @brief Function implementing the Alarm_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Start_Alarm_Task */
void Start_Alarm_Task(void *argument)
{
  /* USER CODE BEGIN Start_Alarm_Task */

	//char ms[30];

	/* Infinite loop */
	for (;;) {
		//HAL_UART_Transmit(&huart2, (uint8_t*) "Alarm function\r\n", sizeof("Alarm function\r\n"), HAL_MAX_DELAY);

		osSemaphoreAcquire(My_SemaphoreHandle, osWaitForever);
		osDelay(10);

		//sprintf(ms,"Set Alarm , Check = %lu\r\n",check);
		//HAL_UART_Transmit(&huart2, (uint8_t*)ms, strlen(ms), HAL_MAX_DELAY);

		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

		if(check == 3 ){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);

			for(int i=0;i<6;i++){
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
				osDelay(500);
			}
		}
		else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 1);
			osDelay(1200);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, 0);
		}

		osDelay(100);
	}
  /* USER CODE END Start_Alarm_Task */
}

/* My_Timer_Callback function */
void My_Timer_Callback(void *argument)
{
  /* USER CODE BEGIN My_Timer_Callback */
	char cardno[30];
	HAL_UART_Transmit(&huart2, (uint8_t*) "System Running\r\n", sizeof("System Running\r\n"), HAL_MAX_DELAY);
	sprintf(cardno,"Cards Registered : %d\r\n",card);
	HAL_UART_Transmit(&huart2, (uint8_t*) cardno, strlen(cardno), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*) "\r\n", sizeof("\r\n"), HAL_MAX_DELAY);
  /* USER CODE END My_Timer_Callback */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
