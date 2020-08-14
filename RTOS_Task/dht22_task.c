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
	u8 raw_data[4];
	u16 temp, humi;
	
	
	while(1)
	{
		if(DHT22_GetOneFrame(raw_data))
		{
			humi = (u16)(raw_data[0]<<8)|raw_data[1];
			temp = (u16)(raw_data[2]<<8)|raw_data[3];
			
			printf("Humi:%d, Temp:%d\r\n", humi,temp);
		}
		else
		{
			#ifdef HUMI_DEBUG
			printf("Temp-Humi GET ERROR\r\n");
			#endif
		}
		Delay(3000);
	}
}
