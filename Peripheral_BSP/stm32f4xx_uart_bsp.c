/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.1
  * @date    09-Aug-2020
  * @brief   USART initialization
  *          Enable GPIO, DMA interface
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
  *
  * website: developerlab.cn
  *
  * Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
  
#include <string.h>
#include "main.h"



#ifdef	UART_IT

	#ifdef UART_IT_IDLE
	xSemaphoreHandle Semaphore_uart_idle = NULL;
	#endif

	#ifdef UART_IT_TC
	xSemaphoreHandle Semaphore_uart_tc = NULL;
	#endif

	#ifdef	UART_DMA_IT
	xSemaphoreHandle Semaphore_uart_dma = NULL;
	#endif

#endif



/* Transmit buffer size */
uint8_t *RxBuffer0;
uint8_t *RxBuffer1;

uint8_t *TxBuffer;

struct __FILE {
	int handle; 
}; 
FILE __stdout;
void _sys_exit(int x) {	x = x;}


/**
  * @brief  UART Pins initilization
  * @param  None
  * @retval None
  */
static void USART_Pins(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/**
  * @brief  Interupt initilization
  * @param  None
  * @retval None
  */
#ifdef	UART_IT
static void USART_NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	#ifdef	UART_IT_RXNE
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	#endif
	#ifdef	UART_IT_IDLE
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	#endif	
	#ifdef	UART_IT_TC
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	#endif

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=USART1_IRQnPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

	#ifdef	UART_DMA_IT
	static void DMA_UART_NVIC(void)
	{
		NVIC_InitTypeDef NVIC_InitStructure;

		DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
		DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=DMA2_Stream5_IRQnPriority;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	#endif
#endif	


/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
void USART_Config(u32 boundrate, u16 StopBits, u16 Parity, u16 HardwareFlowControl)
{		
  USART_InitTypeDef USART_InitStructure;
	
  
  /* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	
	
  USART_InitStructure.USART_BaudRate = boundrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = StopBits;
  USART_InitStructure.USART_Parity = Parity;
  USART_InitStructure.USART_HardwareFlowControl = HardwareFlowControl;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);	
	
	USART_Cmd(USART1, ENABLE);
}


/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
void USART_MDA_Config(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	
	
	/* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = UART_RX_BUFFER_SIZE;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*Single execution. automatically change to cirular mode when use the double buffer mode*/
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART1->DR));
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)RxBuffer0;
  DMA_Init(DMA2_Stream5, &DMA_InitStructure);
	
	DMA_DoubleBufferModeConfig(DMA2_Stream5, (uint32_t)RxBuffer1, DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA2_Stream5, ENABLE);

  /* Configure TX DMA */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_BufferSize = UART_TX_BUFFER_SIZE;
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)TxBuffer;
  DMA_Init(DMA2_Stream7,&DMA_InitStructure);
	
  /* Enable DMA USART RX Stream */
	DMA_Cmd(DMA2_Stream5,ENABLE);
  /* Enable DMA USART TX Stream */
//  DMA_Cmd(DMA2_Stream7,ENABLE);
	
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}


/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
void UART_Init_115200(void)
{
	USART_Pins();
	USART_Config(UART_BAUDRATE, UART_STOPBITS, UART_PARITY, UART_FLOWCONTROL);
}

void UART_Init(u32 boundrate, u16 StopBits, u16 Parity, u16 HardwareFlowControl)
{
	USART_Pins();
  /* USARTx configuration ----------------------------------------------------*/
  /* Enable the USART OverSampling by 8 */
	
#ifdef	UART_OverSampling
  USART_OverSampling8Cmd(USART1, ENABLE);
#endif
  /* USARTx configured as follows:
        - BaudRate = 5250000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 8
		     is: (USART APB Clock / 8) 
			 Example: 
			    - (USART3 APB1 Clock / 8) = (42 MHz / 8) = 5250000 baud
			    - (USART1 APB2 Clock / 8) = (84 MHz / 8) = 10500000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 16
		     is: (USART APB Clock / 16) 
			 Example: (USART3 APB1 Clock / 16) = (42 MHz / 16) = 2625000 baud
			 Example: (USART1 APB2 Clock / 16) = (84 MHz / 16) = 5250000 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */ 
	USART_Config(boundrate, StopBits, Parity, HardwareFlowControl);
	USART_MDA_Config();
#ifdef	UART_IT

	USART_NVIC();	

	#ifdef	UART_IT_IDLE
		if (Semaphore_uart_idle == NULL)
		{
			vSemaphoreCreateBinary(Semaphore_uart_idle);
			xSemaphoreTake( Semaphore_uart_idle, 0);
		}
	#endif
		
	#ifdef	UART_IT_TC
		if (Semaphore_uart_tc == NULL)
		{
			vSemaphoreCreateBinary(Semaphore_uart_tc);
			xSemaphoreTake( Semaphore_uart_tc, 0);
		}
	#endif
		
	#ifdef	UART_DMA_IT
		DMA_UART_NVIC();
		if (Semaphore_uart_dma == NULL)
		{
			vSemaphoreCreateBinary(Semaphore_uart_dma);
			xSemaphoreTake( Semaphore_uart_dma, 0);
		}
	#endif
		
#endif
}

#ifdef	UART_IT

	#ifdef	UART_IT_RXNE
	void UartRecv_RXNE_IRQ(void)
	{
		RxdBufferStructure.uartRecv_Counter++;
	}
	#endif

	#ifdef	UART_IT_IDLE
	void UartRecv_IDLE_IRQ(void)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

		/* Give the semaphore to wakeup LwIP task */
		xSemaphoreGiveFromISR( Semaphore_uart_idle, &xHigherPriorityTaskWoken );
		
		/* Switch tasks if necessary. */
		if( xHigherPriorityTaskWoken != pdFALSE )
		{
			portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
		}
	}
	#endif
	
	#ifdef	UART_IT_TC
	void UartTransmit_IRQ(void)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

		/* Give the semaphore to wakeup LwIP task */
		xSemaphoreGiveFromISR( Semaphore_uart_tc, &xHigherPriorityTaskWoken );
		
		/* Switch tasks if necessary. */
		if( xHigherPriorityTaskWoken != pdFALSE )
		{
			portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
		}
	}
	#endif

	#ifdef UART_DMA_IT
	void DmaComplete_IRQ(void)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		RxdBufferStructure.dmaReversalValue = !RxdBufferStructure.dmaReversalValue;
		RxdBufferStructure.dmaCompleteCounter = RxdBufferStructure.uartRecv_Counter;

		/* Give the semaphore to wakeup LwIP task */
		xSemaphoreGiveFromISR( Semaphore_uart_dma, &xHigherPriorityTaskWoken );
		
		/* Switch tasks if necessary. */
		if( xHigherPriorityTaskWoken != pdFALSE )
		{
			portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
		}
	}
	#endif
	
#endif


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
	USART1->SR;
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
