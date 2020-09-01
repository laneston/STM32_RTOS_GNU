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
#ifndef __STM32F4XX_DHT22_BSP_H
#define __STM32F4XX_DHT22_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
	

#define DHT22_IN()  {GPIOC->MODER &= ~(3<<(10*2)); GPIOC->MODER |= 0<<10*2; GPIOC->OTYPER |= 1<<10;}
#define DHT22_OUT() {GPIOC->MODER &= ~(3<<(10*2)); GPIOC->MODER |= 1<<10*2; GPIOC->OTYPER |= 0<<10;}



/**set the PC10 as the data BUS**/
#define RCC_AHB1Periph_DHT22  RCC_AHB1Periph_GPIOC	
#define	GPIO_DHT22						GPIOC
#define	GPIO_Pin_DHT22        GPIO_Pin_10

#define GPIO_DHT22_OUT        PCout(10)
#define GPIO_DHT22_IN         PCin(10)

extern void DHT22_Init(void);
extern u8 DHT22_GetOneFrame(u8 *result);
extern float DHT22_GetHumi_F(void);
extern float DHT22_GetTemp_F(void);
#ifdef __cplusplus
}
#endif
#endif /* __STM32F4XX_DHT22_BSP_H */
