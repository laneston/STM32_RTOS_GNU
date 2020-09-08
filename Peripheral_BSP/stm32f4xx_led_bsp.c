/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.0
  * @date    09-May-2020
  * @brief   LED Initilization
	*          
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
  *
  * website:developerlab.cn
	*
	* Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
#include "main.h"

/**
  * @brief  DHT22 initilization
  * @param  None
  * @retval None
  */
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LED1, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LED1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIO_LED, &GPIO_InitStructure);
	
	LED1 = SET;/*set the pin level high*/
}


/**
  * @brief  Led conversion polarity 
  * @param  led: LED1, the NO. 1 LED
	*						   LED2, the NO. 2 LED
  *
  * @retval None
  */
void led_conversion(u8 led)
{
	u8 led_state;
	
	if(led == LED1)
	{
		led_state = GPIO_ReadOutputDataBit(GPIO_LED, GPIO_Pin_LED1);
		
		if(led_state)
			LED1 = RESET;
		else
			LED1 = SET;
	}
	else
	{
		
	}
}
