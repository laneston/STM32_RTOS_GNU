/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.0
  * @date    09-May-2020
  * @brief   Task entrance.
	*          Create a major task.
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
	* website: www.developerlab.cn
	*Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
#ifndef __MAIN_TASK_H
#define __MAIN_TASK_H

/* Includes ------------------------------------------------------------------*/
#include "task.h"
/* Task Stack Size-------------------------------------------------------------*/
#define	Main_TASK_STACK_SIZE												 (configMINIMAL_STACK_SIZE*2)
#define UART1_Receive_Task_STACK_SIZE								 (configMINIMAL_STACK_SIZE*2)

/*Priority from low to high---------------------------------------------------*/
#define  Main_TASK_PRIORITY            		 					 (configMAX_PRIORITIES - 9)
#define	 UART1_Receive_Task_PRIORITY				         (configMAX_PRIORITIES - 2)

extern TaskHandle_t Main_Task_Handle;

extern void Main_Task(void);
#endif /* __MAIN_TASK_H */
