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
#ifndef __STM32F4XX_LED_BSP__H
#define __STM32F4XX_LED_BSP__H

#define RCC_AHB1Periph_LED1 RCC_AHB1Periph_GPIOF
#define GPIO_Pin_LED1       GPIO_Pin_9
#define GPIO_LED            GPIOF

#define LED1                PFout(9)

/*n is SET or RESET, SET is turn off and RESET is turn on*/
#define LED1_Contorl(n)     {PFout(9) = n;}

extern void led_init(void);
extern void led_conversion(u8 led);
#endif/*__STM32F4XX_LED_BSP__H*/
