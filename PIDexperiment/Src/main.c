/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
unsigned char PWM1,PWM2,PWM3,PWM4,i,flag;
unsigned int ADC014,ADC023,AI1,AI2;
unsigned int ADC014B=0,ADC023B=0;
unsigned int timeout;
uint8_t aRxBuffer[10];
uint8_t* attention;
//uint8_t ADC014_H1,ADC014_H2,ADC014_L1,ADC014_L2,ADC023_H1,ADC023_H2,ADC023_L1,ADC023_L2;
uint8_t Calculate_flag;
unsigned long sparetime,sparetimemax,error140,error141,error230,error231;
int P=30,I=0,D=0,S=60,P14=900,I14=0,D14=8000,S14=50,P23=900,I23=0,D23=8000,S23=50,errorsum14=0,errorsum23=0;
void set_PWM1(uint16_t value)
{

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);   
}
void set_PWM2(uint16_t value)
{

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);   
}
void set_PWM3(uint16_t value)
{

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);   
}
void set_PWM4(uint16_t value)
{

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);   
}
void get_ADC014()
{    for(i=0;i<10;i++){
	  HAL_ADC_Start(&hadc1); 
    HAL_ADC_PollForConversion(&hadc1, 0xff);
    ADC014B+=HAL_ADC_GetValue(&hadc1) ; }
   ADC014=ADC014B/10;
		ADC014B=0;
}
void get_ADC023()
{    for(i=0;i<10;i++){
	  HAL_ADC_Start(&hadc2); 
    HAL_ADC_PollForConversion(&hadc2, 0xff);
    ADC023B+=HAL_ADC_GetValue(&hadc2) ; }
   ADC023=ADC023B/10;
		ADC023B=0;
}
void AT_command(){
	if(attention[0]!='R'){
	if(attention[0]=='P'&&attention[1]=='='){P=1000*(attention[3]-48)+100*(attention[4]-48)+10*(attention[5]-48)+attention[6]-48;
	if(attention[2]=='-'){P=0-P;}}
  if(attention[0]=='I'&&attention[1]=='='){I=1000*(attention[3]-48)+100*(attention[4]-48)+10*(attention[5]-48)+attention[6]-48;
	if(attention[2]=='-'){I=0-I;}}
	if(attention[0]=='D'&&attention[1]=='='){D=1000*(attention[3]-48)+100*(attention[4]-48)+10*(attention[5]-48)+attention[6]-48;
	if(attention[2]=='-'){D=0-D;}}
	if(attention[0]=='S'&&attention[1]=='='){S=1000*(attention[3]-48)+100*(attention[4]-48)+10*(attention[5]-48)+attention[6]-48;
	if(attention[2]=='-'){S=0-S;}}
	if(attention[0]=='1'&&attention[1]=='4'&&attention[2]=='='){P14=P;I14=I;D14=D;S14=S;}
	if(attention[0]=='2'&&attention[1]=='3'&&attention[2]=='='){P23=P;I23=I;D23=D;S23=S;}
	}}

	
int PID14(void){
	long result;
	result=P14*error141+I14*errorsum14+D14*(error141-error140);
	result/=10000;
	return (int)result;}
int PID23(void){
	long result;
	result=P23*error231+I23*errorsum23+D23*(error231-error230);
	result/=10000;
	return (int)result;}
void get_PWM(void){
	if((S14-PID14())<(2*S14)&&(S14-PID14())>=0){
	set_PWM1(S14-PID14());}
	else if((S14-PID14())>=(2*S14))set_PWM1(2*S14);
	else set_PWM1(0);
	if((S14+PID14())<(2*S14)&&(S14+PID14())>=0){
	set_PWM4(S14+PID14());}
	else if((S14+PID14())>=(2*S14))set_PWM4(2*S14);
	else set_PWM4(0);
	errorsum14+=error141;
	error140=error141;
	if((S23-PID23())<(2*S23)&&(S23-PID23())>=0){
	set_PWM2(S23-PID23());}
	else if((S23-PID23())>=(2*S23))set_PWM2(2*S23);
	else set_PWM2(0);
	if((S23+PID23())<(2*S23)&&(S23+PID23())>=0){
	set_PWM3(S23+PID23());}
	else if((S23+PID23())>=(2*S23))set_PWM3(2*S23);
	else set_PWM3(0);
	errorsum23+=error231;
	error230=error231;}
	
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define timemax 50000//UART NVIC RESET TIMEOUT SETTING
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_ADC_Start(&hadc1); 
    HAL_ADC_PollForConversion(&hadc1, 0xff);
    AI1=HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Start(&hadc2); 
    HAL_ADC_PollForConversion(&hadc2, 0xff);
    AI2=HAL_ADC_GetValue(&hadc2);
		HAL_UART_Receive_DMA(&huart1,(uint8_t *)aRxBuffer,10);
  while (1)
  {  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		AT_command();
		get_ADC014();
	  get_ADC023();
		error141=ADC014-AI1;
		error231=ADC023-AI2;
		get_PWM();
		HAL_UART_Receive_DMA(&huart1,(uint8_t *)aRxBuffer,10);
		//HAL_UART_Receive_IT(&huart1,(uint8_t*)&aRxBuffer,10);
		
		if(flag){HAL_UART_Transmit(&huart1,(uint8_t*)"receive:",8,0xFFFF);
				HAL_UART_Transmit(&huart1,(uint8_t*)&aRxBuffer,10,0xFFFF);
				HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n",2,0xFFFF);
					if(aRxBuffer[0]=='A'&&aRxBuffer[1]=='T'&&aRxBuffer[2]=='+'){
				attention=aRxBuffer+3;
						
					HAL_UART_Transmit(&huart1,(uint8_t*)"        OK\r\n",13,0xFFFF);}
					else {
						HAL_UART_Transmit(&huart1,(uint8_t*)"      error\r\n",13,0xFFFF);
						
						}
					flag=0;
					//HAL_UART_Receive_IT(&huart1,(uint8_t *)&aRxBuffer,10);	
  }
  /* USER CODE END 3 */
}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System 
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{      
	
        if(huart->Instance == USART1)
        { //static int timeout;
					//timeout =0;
					flag=1;		
				
					}
        //HAL_Delay(5); 
        //MX_USART1_UART_Init();
				//while(HAL_UART_Receive_IT(&huart1,(uint8_t*)&aRxBuffer,10)!=HAL_OK)
				//{ timeout++;
				//	if(timeout>timemax)break;
				//}
				}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){ 
//if(htim==(&htim4)){
	//get_ADC014();
	//get_ADC023();
	// HAL_UART_Receive_IT(&huart1,(uint8_t*)&aRxBuffer,10);
	// Calculate_flag=1;
//}
//}
	
	


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
