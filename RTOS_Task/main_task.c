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

	led_init();
	
	taskENTER_CRITICAL();
	
	xReturn = xTaskCreate((TaskFunction_t)TEST_Task,
						(const char*)"DHT22_Task",
						(uint32_t)TEST_Task_STACK_SIZE,
						(void*)NULL,
						(UBaseType_t)TEST_Task_PRIORITY,
						(TaskHandle_t*)&TEST_Task_Handle);
	if(pdPASS == xReturn){}
	else{}
  
	taskEXIT_CRITICAL();
	
	
	vTaskDelete(Main_Task_Handle);
  for( ;; ){}
}
/*End of file*********************************************/
