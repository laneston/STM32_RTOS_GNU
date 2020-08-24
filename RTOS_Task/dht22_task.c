/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.0
  * @date    09-May-2020
  * @brief   dht22 task
	*          
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
	* Website: developerlab.cn
	* Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
#include "main.h"
	

TaskHandle_t DHT22_Task_Handle;

/**
  * @brief  DHT22_Task
  * @param  None
  * @retval None
  */
void DHT22_Task(void)
{
	float Humi;
	float Temp;
	
	while(1)
	{
		Humi = DHT22_GetHumi_F();
		printf("Humi:%.2f\r\n", Humi);
		Delay(3000);
		
		Temp = DHT22_GetTemp_F();
		printf("Temp:%.2f\r\n", Temp);		
		Delay(3000);		
	}
}
