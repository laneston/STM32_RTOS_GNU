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
  
#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"


#define TIM2_Period  42 /*APB1 max frequent is 42MHz*/

extern void DelayTimer_Init(u16 TIM_Period);
extern void Delay_us(u32 time_us);
extern void Delay_ms(u32 time_ms);

#endif/*__DELAY_H*/

