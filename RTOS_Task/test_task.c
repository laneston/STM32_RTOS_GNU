/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.1
  * @date    09-May-2020
  * @brief   TEST task
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
    

TaskHandle_t TEST_Task_Handle;

/**
  * @brief  DHT22_Task
  * @param  None
  * @retval None
  */
void TEST_Task(void)
{
//	float Humi;
//	float Temp;	
    
    u16 data_len;
    u8 *data_uart;
    
    data_uart = stSramMalloc(&HeapStruct_SRAM1, UART_RX_BUFFER_SIZE);
    
    while(1)
    {
        /* HDT22 Test*****************
        
        Humi = DHT22_GetHumi_F();
        printf("Humi:%.2f\r\n", Humi);
        Delay(3000);
        
        Temp = DHT22_GetTemp_F();
        printf("Temp:%.2f\r\n", Temp);
        Delay(3000);
        
        ******************************
        */
        
        
    /*
    LED1 = SET;
    Delay(500);
    LED1 = RESET;
    Delay(500);
    */
                
        if(RxdBufferStructure.readableLength)
        {
            data_len = RxdBufferStructure.readableLength;
            ReadHeapBufferToExtralBuffer(&RxdBufferStructure, data_uart, data_len);
            UartDmaStreamSend(data_uart, data_len);
        }
        
        Delay(500);
    }
}
