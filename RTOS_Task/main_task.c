/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.1
  * @date    09-May-2020
  * @brief   Task entrance.
	*          Create a major task.
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
	*
	* website: www.developerlab.cn
	*
	*Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
#include "main.h"

TaskHandle_t Main_Task_Handle = NULL;/*Main Task Handle*/

Heap_TypeDef HeapStruct_SRAM1;
EmbeverConfig_TypeDef EmbeverStruct;


/**
  * @brief  Peripheral parameter initialization.
  * @param  None
  * @retval None
  */
static void Device_Init(void)
{
	/*Initialization of serial port parameters*/
	UartParam_Init();
}


/**
  * @brief  main task
  * @param  None
  * @retval None
  */
void Main_Task(void)
{
	BaseType_t xReturn = pdPASS;
	
	/*Delay timer initialization*/
	DelayTimer_Init(TIM2_Period);
	
	/*Memory heap initialization*/
	stSramInit(&HeapStruct_SRAM1, STM32F4XX_SRAM1_START, STM32F4XX_SRAM1_SIZE);
	
	/*Peripheral parameters initialization.*/
	Device_Init();
	
	/*UART receive memory initialization. This function is required if receive is enabled*/
	UartRxBufferPointer_Init();
	
	/*UART initialization*/
	UART_Init(EmbeverStruct.uartdev.BaudRate, EmbeverStruct.uartdev.StopBits, EmbeverStruct.uartdev.Parity, EmbeverStruct.uartdev.HardwareFlowControl);
	
	DHT22_Init();
	
	taskENTER_CRITICAL();	
	
	xReturn = xTaskCreate((TaskFunction_t)DHT22_Task,
						(const char*)"DHT22_Task",
						(uint32_t)DHT22_Task_STACK_SIZE,
						(void*)NULL,
						(UBaseType_t)DHT22_Task_PRIORITY,
						(TaskHandle_t*)&DHT22_Task_Handle);
	if(pdPASS == xReturn){}
	else{
		printf("DHT22_Task ERROR\r\n");
	}
  
	taskEXIT_CRITICAL();
	
	
	vTaskDelete(Main_Task_Handle);
  for( ;; ){}
}
/*End of file*********************************************/
