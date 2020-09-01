/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.0
  * @date    09-May-2020
  * @brief   Delay Timer Initilization
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
  * @brief  DelayTimer_Init
  * @param  TIM_Period: The frequency on the clock bus currently in use (TIM_Period MHz)
  * @retval None
  */
void DelayTimer_Init(u16 TIM_Period)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TIM_Period-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 1;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
}

/**
  * @brief  Delay_us
  * @param  time_us: Tick of us
  * @retval None
  */
void Delay_us(u32 time_us)
{
	/* Enable the TIM Counter */
	TIM_Cmd(TIM2, ENABLE);
	
	while(time_us--)
	{
		while(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == RESET);
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}
	
	TIM_Cmd(TIM2, DISABLE);
}

/**
  * @brief  Delay_ms
  * @param  time_ms: Tick of ms
  * @retval None
  */
void Delay_ms(u32 time_ms)
{
	while(time_ms--)
	{
		Delay_us(1000);
	}
}
