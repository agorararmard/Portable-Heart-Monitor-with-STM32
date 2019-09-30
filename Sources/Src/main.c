
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "w25qxx.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#define __ON__ 1
#define __OFF__ 0 
#define CAP 9
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osMutexId myMutex01Handle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t __DISPLAY_COMMAND_OFF__ = 0xae;
uint8_t __DISPLAY_COMMAND_ON__ = 0xaf;
uint8_t __FILL_ZEROES__ = 0x00;

int power =__OFF__;
// CHAR ARRAY OLED DISPLAY BUFFER
// the size of display buffer is based on the longest sentence option
// WAITING READING = 15 CHARACTER OTHER MESSAGE IS RATE XXX/XXX = 12 CHARACTER
char displayBuffer[20];

int HeartRateSingleRead =0;
int HeartRateBuffer[20];
char* startingText = "HeartBeat is ON..";
int Mpointer = 4;


int stcount = 0;



void intTouint8(int x, uint8_t y[], int cap);
int uint8Toint(uint8_t y[], int cap);
int powr(int x,int y);
void revuint8(uint8_t x[], int cap);
void intToprint(int x, uint8_t y[], int cap);
void writeToMemory(uint8_t x[]);
void readFromMemory(uint8_t x[], int readAddr);
void updatePointer();
void loadPointer();
/**
// FONT TABLE TO DRAW CHARACTERS ON OLED

char font_table[][6] = { //sample font table 
	
	

{0x7e, 0x11, 0x11, 0x11, 0x7e,
0}, /* A \*\/

{0x7f, 0x49, 0x49, 0x49, 0x36,
0}, /* B *\/

{0x3e, 0x41, 0x41, 0x41, 0x22,
0}, /* C *\/
/* ENTER THE REST OF CHARACTERS HERE *\/
{0x7e, 0x42, 0x52, 0x42, 0x7e,
0}, /* 0 *\/
{0x88, 0x84, 0x82, 0xFF, 0x80,
0}, /* 1 *\/
{0xF1, 0x89, 0x89, 0x89, 0x8F,
0}, /* 2 *\/
{0x49, 0x49, 0x49, 0x49, 0x77,
0}, /* 3 *\/
{0x1F, 0x10, 0x10, 0x10, 0xFF,
0}, /* 4 *\/
{0x8F, 0x89, 0x89, 0x89, 0xF9,
0}, /* 5 *\/
{0xFF, 0x89, 0x89, 0x89, 0xF9,
0}, /* 6 *\/
{0x01, 0x01, 0x01, 0x01, 0xFF,
0}, /* 7 *\/
{0xFF, 0x89, 0x89, 0x89, 0xFF,
0}, /* 8 *\/
{0x11, 0x11, 0x11, 0x11, 0xFF,
0}, /* 9 *\/
}; /* END OF FONT TABLE *\/
**/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void powerOnOled();
void powerOffOled();
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

// INITIATE OLED DISPLAY POWER ON ROUTINE
void powerOnOled(){
	
	/*
// DISABLE IRQs TO CREATE ATOMIC INSTRUCTIONS
//	
//	__disable_irq();
//	
//// SET THE RESET PIN TO HIGH FOR NORMAL OPERATION
//		HAL_GPIO_WritePin(resetOled_GPIO_Port,resetOled_Pin,GPIO_PIN_SET);
//	
//// SET THE VDD TO 1 AS PER POWER ON ROUTINE
//	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
//	
//// SELECT SLAVE AND TRANSMIT DISPLAY OFF COMMAND
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
//		HAL_SPI_Transmit(&hspi1,&__DISPLAY_COMMAND_OFF__,1,HAL_MAX_DELAY);
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
//	
//// SET OPERATION MODE TO SHOW BUFFER CONTENT ON DISPLAY
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
//	
//// CLEAR SCREEN ROUTINE
//// MAINLY TRYING TO FILL THE BUFFER WITH ZEROES IN THE USAGE AREA
//// SO I HAVE 20 CHARS MAX. THE CHAR CONTAINS 6 BYTES IM WRITING PER BYTE
//// SO IN TOTAL I NEED TO WRITE 6*20 120 BYTES OF ZEROS

//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
//		for(int i = 0; i < 120; i++)
//			HAL_SPI_Transmit(&hspi1,&__FILL_ZEROES__,1,HAL_MAX_DELAY);
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
//	
//// SET VBAT = 1
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
//		
//// ENABLE IRQS TO ENABLE OS TIME DELAY TO WORK PROPERLY

//	__enable_irq();

//// WAIT FOR A 100ms AND ALLOW OTHER TASKS TO RUN IN THAT TIME
//osDelay(100);

//// CREATE ATOMIC INSTRUCTION AGAIN

//	__disable_irq();
//	
//// SELECT SLAVE AND SEND DISPLAY ON COMMAND
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
//		HAL_SPI_Transmit(&hspi1,&__DISPLAY_COMMAND_ON__,1,HAL_MAX_DELAY);
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
//		
//// RETURN CONTROL TO RTOS

//	__enable_irq();
//	
//// DONE ROUTINE
//	
*/



ssd1306_Init();
ssd1306_Fill( 0X00);

startingText = "HB ON..";
ssd1306_SetCursor(0,40);
ssd1306_WriteString(startingText,Font_11x18,White);
ssd1306_UpdateScreen();

}



// INITIATE OLED DISPLAY POWER OFF ROUTINE
void powerOffOled(){
// DISABLE IRQs TO CREATE ATOMIC INSTRUCTIONS
	
	__disable_irq();

// SELECT SLAVE AND TRANSMIT DISPLAY OFF COMMAND
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1,&__DISPLAY_COMMAND_OFF__,1,HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	
// RESET VBAT = 0
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
	
// ENABLE IRQS TO ENABLE OS TIME DELAY TO WORK PROPERLY

	__enable_irq();

// WAIT FOR A 100ms AND ALLOW OTHER TASKS TO RUN IN THAT TIME
osDelay(100);
	
// NO NEED TO CREATE ATOMIC STRUCTURE FOR THE FINAL COMMAND CLEAR VDD
	
// RESET THE VDD TO 0 AS PER POWER OFF ROUTINE
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
		
// RESET THE RESET PIN TO LOW FOR RESET MODE
		HAL_GPIO_WritePin(resetOled_GPIO_Port,resetOled_Pin,GPIO_PIN_RESET);
	
	
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityNormal, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, StartTask04, osPriorityIdle, 0, 128);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
	 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
 //HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
   W25qxx_Init();

  
	
	/* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, resetOled_Pin|GPIO_PIN_1|testLed_Pin|GPIO_PIN_6 
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : resetOled_Pin PB1 testLed_Pin PB4 
                           PB6 PB7 */
  GPIO_InitStruct.Pin = resetOled_Pin|GPIO_PIN_1|testLed_Pin|GPIO_PIN_4 
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LoPos_Pin */
  GPIO_InitStruct.Pin = LoPos_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LoPos_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LoNeg_Pin */
  GPIO_InitStruct.Pin = LoNeg_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LoNeg_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN 5 */

	int LoNegValue;
	 int LoPosValue;
	 int ADCValue;
  /* Infinite loop */
	
	
	
	for(;;)
  {
//		if(!stcount)
//		{
//			loadPointer();
//		}
		
		
//if(!stcount)
//{
//  loadPointer();
//	uint8_t toBeSent[5];
//	//readFromMemory(toBeSent,Mpointer-4);
//	//int tmpz = uint8Toint(toBeSent,4);
//  uint8_t toBeSent2[5];
//	intToprint(Mpointer,toBeSent2,4);
//	revuint8(toBeSent2,4);
//	for(int i =0; i< 4;i++)
//		toBeSent2[i]+='0';
//	toBeSent2[4] = 0;
//	HAL_UART_Transmit(&huart1,(uint8_t*)toBeSent2,5,100);
//	HAL_UART_Transmit(&huart1,(uint8_t*)toBeSent2,5,100);
//	HAL_UART_Transmit(&huart1,(uint8_t*)toBeSent2,5,100);
//	stcount++;
//}

			HAL_ADC_Start(&hadc1);
		LoNegValue = HAL_GPIO_ReadPin(LoNeg_GPIO_Port,LoNeg_Pin);
		LoPosValue = HAL_GPIO_ReadPin(LoPos_GPIO_Port, LoPos_Pin);
		
		
		
		if(LoNegValue == 0 && LoPosValue == 0){
			

			if(HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
			{
			
				
				ADCValue = HAL_ADC_GetValue(&hadc1);
				HeartRateSingleRead = (ADCValue * 330) / 4096;
				
				uint8_t x[4];
				intTouint8(HeartRateSingleRead, x, 4);
				writeToMemory(x);
//				ssd1306_SetCursor(0,20);
//			startingText = "reading!!!";
//			ssd1306_WriteString(startingText,Font_11x18,White);
//			ssd1306_UpdateScreen();
				
				osSignalSet(myTask02Handle,0x3);
	//			osSignalSet(myTask03Handle,0x7);
			} else {
	
//				ssd1306_SetCursor(0,20);
//			startingText = "msh3aref!!!";
//			ssd1306_WriteString(startingText,Font_11x18,White);
//			ssd1306_UpdateScreen();
			}
		}else {
			
			
//			ssd1306_SetCursor(0,0);
//			startingText = "invalid sig.";
//			ssd1306_WriteString(startingText,Font_11x18,White);
//			ssd1306_UpdateScreen();


		}
		
			HAL_ADC_Stop(&hadc1);
		  osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
		if(power == __ON__){
			stcount++;
		if(stcount> 10)
			HAL_GPIO_WritePin(testLed_GPIO_Port,testLed_Pin,GPIO_PIN_RESET);
		else
				HAL_GPIO_WritePin(testLed_GPIO_Port,testLed_Pin,GPIO_PIN_SET);
// MAINLY TRYING TO FILL THE BUFFER WITH ZEROES IN THE USAGE AREA
// SO I HAVE 20 CHARS MAX. THE CHAR CONTAINS 6 BYTES IM WRITING PER BYTE
// SO IN TOTAL I NEED TO WRITE 6*20 120 BYTES OF ZEROS
		
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
//			for(int i = 0; i < 120; i++)
//				//HAL_SPI_Transmit(&hspi1,(uint8_t *)font_table[0],6,HAL_MAX_DELAY);  // there is supposed to be a logic here that loops over a buffer and displays it
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
			
			osSignalWait(0x3,osWaitForever);
//			ssd1306_SetCursor(0,40);
//			startingText = "weselt!!!";
//			ssd1306_WriteString(startingText,Font_11x18,White);
//			ssd1306_UpdateScreen();
//			__disable_irq();
		int x = HeartRateSingleRead;
		uint8_t arr[100];
		intToprint(x, arr, CAP);
		revuint8(arr, CAP);
		char pri[100];
	//	char * startingText1 ="";
		
		ssd1306_SetCursor(0,20);
		for(int i =0;i < CAP; i++)
		{
			pri[i] = arr[i] + '0';
				ssd1306_WriteChar(pri[i], Font_11x18, White);

		}
			pri[CAP] = '\0';
	//	startingText1 = pri;
//ssd1306_WriteString(startingText1,Font_11x18,White);
ssd1306_UpdateScreen();
HAL_UART_Transmit(&huart1, (uint8_t*)pri, CAP+1,100);
//__enable_irq();
		}	
    osDelay(500);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
		updatePointer();
		if(power == __ON__)
		{
			
	//	updatePointer();
	//	osSignalWait(0x7,osWaitForever);
			
		
	
//		HAL_UART_Transmit(&huart1, (uint8_t *)sent,4,1000);	
	
//		ssd1306_SetCursor(0,20);
//		ssd1306_WriteChar(temp[0], Font_11x18, White);
//		ssd1306_WriteChar(temp[1], Font_11x18, White);
//		ssd1306_WriteChar(temp[2], Font_11x18, White);
//		ssd1306_UpdateScreen();
			

	
//		int x = HeartRateSingleRead;
//	
//		uint8_t arr[100];
//		
//		intToprint(x, arr, CAP);
//		revuint8(arr, CAP);
//		for(int i =0;i < CAP; i++)
//			arr[i] = arr[i] + '0';
//		
//			HAL_UART_Transmit(&huart1, (uint8_t *)&arr,CAP,1000);
		
//		updatePointer();
//			int z = Mpointer;
//		uint8_t arr2[10];
//		intToprint(z, arr2, CAP);
//		revuint8(arr2, CAP);
//		HAL_UART_Transmit(&huart1, (uint8_t *)&arr2,CAP,1000);

//		ssd1306_SetCursor(0,20);
//			startingText = "send uart";
//			ssd1306_WriteString(startingText,Font_11x18,White);
//			ssd1306_UpdateScreen();
			
			
			
			
			//********************************************** Testing That Memory Works **********************************// 
//		updatePointer();		
//		int tmpX = Mpointer;
//		loadPointer();
//		if(tmpX == Mpointer)
//		{
//				HAL_GPIO_WritePin(testLed_GPIO_Port,testLed_Pin,GPIO_PIN_SET);
//		}
//		int y = Mpointer;
//		uint8_t arr1[10];
//		intToprint(y, arr1, CAP);
//		revuint8(arr1, CAP);
//		char pri[11];
//		for(int i =0;i < CAP; i++)
//			pri[i] = arr1[i] + '0';
//			
//			pri[CAP] = '\0';
//		//startingText = pri;
//		HAL_UART_Transmit(&huart1, (uint8_t *)pri,CAP,1000);
//	//	ssd1306_SetCursor(0,0);
//	//ssd1306_WriteString(startingText,Font_11x18,White);
////	ssd1306_UpdateScreen();
//		
//	HAL_Delay(5000);
//	Mpointer = 7;
//	updatePointer();
//			int z = Mpointer;
//		uint8_t arr2[10];
//		intToprint(z, arr2, CAP);
//		revuint8(arr2, CAP);
//		char pri1[11];
//		for(int i =0;i < CAP; i++)
//			pri1[i] = arr2[i]+'0';
//		
//		pri1[CAP] = '\0';

//	//	startingText = pri1;
//	//	ssd1306_SetCursor(0,20);
////	ssd1306_WriteString(startingText,Font_11x18,White);
//	//ssd1306_UpdateScreen();
//		HAL_UART_Transmit(&huart1, (uint8_t *)pri1,CAP,1000);
//			
		}		
    osDelay(1000);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
		osSignalWait(0x1,osWaitForever);
		if(power == __ON__)
			powerOnOled();
		else
			powerOffOled();
		
    osDelay(3);
  }
  /* USER CODE END StartTask04 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

void intTouint8(int x, uint8_t y[], int cap)
{
	int cnt = 0;

	while(cap--)
	{
		y[cnt] = x%256;
		cnt++;
		x/=256;
	}
	
}

void intToprint(int x, uint8_t y[], int cap)
{
	int cnt = 0;

	while(cap--)
	{
		y[cnt] = x%10;
		cnt++;
		x/=10;
	}
	
}

int uint8Toint(uint8_t y[], int cap)
{
	int ret = 0;
	for(int i = 0; i < cap;i++)
	{
		ret = ret + powr(256,(i)) * y[i];
	}
	return ret;
}

int powr(int x,int y)
{
	int ret = 1;
	for(int i = 0; i < y;i++)
		ret*=x;
	return ret;
}

void revuint8(uint8_t x[], int cap)
{
	int i= 0;
	int j = cap-1;
	while (i<j)
	{
		x[i] = x[j] ^ x[i];
		x[j] = x[j] ^ x[i];
		x[i] = x[j] ^ x[i];
		i++;
		j--;
	}
	
}

void writeToMemory(uint8_t x[])
{
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)0x06,1,100);
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
//	HAL_Delay(1);
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)0x02,1,100);
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)Mpointer,3,100);
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)"1234",4,100);
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
//	Mpointer+=4;
//	HAL_Delay(1);
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)0x04,1,100);
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
//	HAL_Delay(1);
//	
	for(int i =0; i< 4;i++)
	{
				W25qxx_WriteByte((uint8_t) x[i],(uint32_t) Mpointer+i);
	}
	Mpointer+=4;
}


void readFromMemory(uint8_t x[], int readAddr)
{
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)0x03,1,100);
//	HAL_SPI_Transmit(&hspi2, (uint8_t*)readAddr,3,100);
//	HAL_SPI_Receive(&hspi2, (uint8_t*)x,4,100);
//	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
//	HAL_Delay(1);
	
	
	W25qxx_ReadBytes((uint8_t *)&x,(uint32_t) readAddr,4);
}


void updatePointer()
{
	uint8_t x[4];
	intTouint8(Mpointer, x, 4);
	Mpointer = 0;
	writeToMemory(x);
	Mpointer = uint8Toint(x,4);
}

void loadPointer()
{
	uint8_t x[4];
	readFromMemory(x, 0);
	Mpointer = uint8Toint(x,4);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
