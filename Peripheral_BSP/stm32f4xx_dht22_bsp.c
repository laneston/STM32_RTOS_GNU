/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.1
  * @date    09-Aug-2020
  * @brief   DHT22 Drive
  *          GPIO initialization
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
  *
  * website: developerlab.cn
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
void DHT22_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DHT22, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_DHT22;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIO_DHT22, &GPIO_InitStructure);
	
	GPIO_DHT22_OUT = SET;/*set the pin level high*/
}


/**
  * @brief  DHT22_Start
  * @param  None
  * @retval return SET if success, or RESET if fail
  */
static u8 DHT22_Start(void)
{
	u8 count;
	
	DHT22_OUT();
	
	GPIO_DHT22_OUT = RESET;
	Delay_us(550);
	GPIO_DHT22_OUT = SET;
	Delay_us(30);
	
  DHT22_IN();
	
	count = 100;
	while(!GPIO_DHT22_IN)
	{
		count--;
		Delay_us(1);
	}
	if(!count) return RESET;
	
	count = 100;
	while(GPIO_DHT22_IN)
	{
		count--;
		Delay_us(1);
	}
	if(!count) return RESET;
	
	return SET;
}



/**
  * @brief  DTH22_GetOnebit
  * @param  None
  * @retval return 0 or 1 according to current bit
  */
static u8 DTH22_GetOnebit(void)
{
	u8 revale = 0;
	u8 count;
	
	count = 100;
	/*wait for high level*/
	while((!GPIO_DHT22_IN) && count)
	{
		count--;
		Delay_us(1);
	}
	
	count = 40;
	while(GPIO_DHT22_IN && count)
	{
		count--;
		Delay_us(1);
	}
	
	if(count) revale = 0;
	else revale = 1;
	
	return revale;
}


/**
  * @brief  DHT22_GetOneByte
  * @param  None
  * @retval return one byte read
  */
static u8 DHT22_GetOneByte(void)
{
	u8 data = 0;
	u8 i;
	
	for(i=0; i<8; i++)
	{
		data <<= 1;
		data |= DTH22_GetOnebit();
	}
	return data;
}


/**
  * @brief  DHT22_GetOneFrame
  * @param  return the result pointer
  * @retval return SET if success, or RESET if fail
  */
u8 DHT22_GetOneFrame(u8 *result)
{
	u8 revale;
	u8 i;
	u8 data[5];
	
	if(DHT22_Start())
	{
		
	}
	else
	{
		return 0xff;
	}
	
	for(i=0; i<5; i++)
	{
		data[i] = DHT22_GetOneByte();
	}
	
	/*Data check*/
	revale = data[0] + data[1] + data[2] + data[3];
	
	if(revale == data[4])
	{
		for(i=0; i<4; i++)
		{
			*result = data[i];
			result++;
		}
		revale = SET;
	}
	else
	{
		revale = RESET;
	}
	
	return revale;
}

