/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.5
  * @date    09-May-2020
  * @brief   Project entrance.
  *          Create the major task.
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
  *
  * website:developerlab.cn
  *
  * Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  argc:
  * @param  argv:App input parameters
  * @retval None
  */
int main(int argc, char *argv[])
{
  BaseType_t xReturn = pdPASS;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  xReturn = xTaskCreate((TaskFunction_t)Main_Task,
                        (const char *)"Main_Task",
                        (uint32_t)Main_TASK_STACK_SIZE,
                        (void *)NULL,
                        (UBaseType_t)Main_TASK_PRIORITY,
                        (TaskHandle_t *)&Main_Task_Handle);
  if (xReturn != pdPASS)
  {
    return -1;
  }
  else
  {
    /*Enable Task scheduler*/
    vTaskStartScheduler();
  }

  while (1)
  {
  }
}

/*End of file*********************************************/
