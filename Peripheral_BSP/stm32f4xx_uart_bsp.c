/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.1
  * @date    09-Aug-2020
  * @brief   USART initialization
  *          Enable GPIO, DMA interface
	*
	* @note    UartRxBufferPointer_Init() -> UART_Init(u32 boundrate, u16 WordLength, u16 StopBits, u16 Parity, u16 HardwareFlowControl);
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


struct __FILE {
    int handle; 
}; 
FILE __stdout;
void _sys_exit(int x) {	x = x;}

/* Transmit buffer block pointer */
static uint8_t *RxBuffer0;
static uint8_t *RxBuffer1;
static uint8_t *TxBuffer;

/*Storage buffer block pointer*/
static uint8_t *heapBuffer_HeadP = NULL;

/*********************************
**  Serial interrupt semaphore
**********************************
*/
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


TaskHandle_t UART1_Receive_Task_Handle;

UARTBufferTypeDef	RxdBufferStructure;

/**
  * @brief  UART1_Receive_Task
  * @param  None
  * @retval None
  */
void UART1_Receive_Task(void)
{
	static u16 UARTWirteResidualLength = 0;
	while(1)
	{
		/*****************Serial port dma interrupt execution***********************/
		if(xSemaphoreTake(Semaphore_uart_dma, Wait_BlockTime_uart_dma) == pdTRUE)
		{
			/*View current queue usage*/
			if(DMA_GetCurrentMemoryTarget(DMA2_Stream5))
			{
				/*current memory target is 1, Memory 0 is full, We process data for memory 0*/
				WirteToUartRxBufferFromRxBuffer0(&RxdBufferStructure, (RxdBufferStructure.dmaCompleteCounter));
				RxdBufferStructure.RxBuffer0Structure.wP = RxBuffer0;
				RxdBufferStructure.RxBuffer0Structure.rP = RxBuffer0;
				RxdBufferStructure.RxBuffer0Structure.FLen = UART_RX_BUFFER_SIZE;
				RxdBufferStructure.RxBuffer0Structure.ULen = 0;
			}
			else
			{
				/*current memory target is 0, Memory 1 is full, We process data for memory 1*/
				WirteToUartRxBufferFromRxBuffer1(&RxdBufferStructure, (RxdBufferStructure.dmaCompleteCounter));
				RxdBufferStructure.RxBuffer1Structure.wP = RxBuffer1;
				RxdBufferStructure.RxBuffer1Structure.rP = RxBuffer1;
				RxdBufferStructure.RxBuffer1Structure.FLen = UART_RX_BUFFER_SIZE;
				RxdBufferStructure.RxBuffer1Structure.ULen = 0;
			}
		}
		
		/*****************Serial port idle interrupt execution***********************/
		if(xSemaphoreTake(Semaphore_uart_idle, Wait_BlockTime_uart_idle) == pdTRUE)
		{
			/*note: Perhaps interrupt occurred, but another memory has no data*/
			if(RxdBufferStructure.dmaReversalValue == SET)
			{
				UARTWirteResidualLength = RxdBufferStructure.uartRecv_Counter - 1 - RxdBufferStructure.dmaCompleteCounter;/*Subtract the number of last cached data*/
				WirteToUartRxBufferFromRxBuffer1(&RxdBufferStructure, UARTWirteResidualLength);
			}
			else
			{
				UARTWirteResidualLength = RxdBufferStructure.uartRecv_Counter - 1 - RxdBufferStructure.dmaCompleteCounter;
				WirteToUartRxBufferFromRxBuffer0(&RxdBufferStructure, UARTWirteResidualLength);
			}
			
			RxdBufferStructure.uartRecv_Counter = 0;
			RxdBufferStructure.dmaCompleteCounter = 0;
			UARTWirteResidualLength = 0;
		}
		
#ifdef	UART_IT_TC
		if(xSemaphoreTake(Semaphore_uart_tc, Wait_BlockTime_uart_tc) == pdTRUE)
		{
			
		}
#endif
		
		Delay(10);
	}
}



/**
  * @brief  uart sending function
	* @param  buffer: sending buffer
	* @param  length: sending length
  * @retva  none
  */
void UartDmaStreamSend(u8 *buffer, u16 length)
{
	DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
  /* Clear USART Transfer Complete Flags */
  USART_ClearFlag(USART1,USART_FLAG_TC);
	DMA_Cmd(DMA2_Stream7,DISABLE);
	memcpy(TxBuffer, buffer, length);
	DMA_SetCurrDataCounter(DMA2_Stream7, length);
	DMA_Cmd(DMA2_Stream7,ENABLE);
}


/**
  * @brief  uart rx buffer pointer buffer init
  * @param  None
  * @retval None
  */
void UartRxBufferPointer_Init(void)
{	
	heapBuffer_HeadP = stSramMalloc(&HeapStruct_SRAM1, UART_HEAP_BUFFER_SIZE);
	if(heapBuffer_HeadP == NULL)
	{
#ifndef HEAP_DEBUG
		ASSERT();
#endif
	}	
	
	RxBuffer0 = stSramMalloc(&HeapStruct_SRAM1, UART_RX_BUFFER_SIZE);
	if(RxBuffer0 == NULL)
	{
#ifndef HEAP_DEBUG
		ASSERT();
#endif
	}
	RxBuffer1 = stSramMalloc(&HeapStruct_SRAM1, UART_RX_BUFFER_SIZE);
	if(RxBuffer1 == NULL)
	{
#ifndef HEAP_DEBUG
		ASSERT();
#endif
	}
	TxBuffer = stSramMalloc(&HeapStruct_SRAM1, UART_TX_BUFFER_SIZE);
	if(TxBuffer == NULL)
	{
#ifndef HEAP_DEBUG
		ASSERT();
#endif
	}
	
	RxdBufferStructure.readwriteLock = RESET;
	
	RxdBufferStructure.wirteP = heapBuffer_HeadP;
	RxdBufferStructure.readP = heapBuffer_HeadP;
	
	RxdBufferStructure.WirteResidualLength = UART_HEAP_BUFFER_SIZE;
	RxdBufferStructure.ReadResidualLength = UART_HEAP_BUFFER_SIZE;
	RxdBufferStructure.wirteableLength = UART_HEAP_BUFFER_SIZE;
	
	RxdBufferStructure.readableLength = 0;
	RxdBufferStructure.UsedLength = 0;
	
	RxdBufferStructure.RxBuffer0Structure.FLen = UART_RX_BUFFER_SIZE;
	RxdBufferStructure.RxBuffer0Structure.ULen = 0;
	RxdBufferStructure.RxBuffer0Structure.rP = RxBuffer0;
	RxdBufferStructure.RxBuffer0Structure.wP = RxBuffer0;
	
	RxdBufferStructure.RxBuffer1Structure.FLen = UART_RX_BUFFER_SIZE;
	RxdBufferStructure.RxBuffer1Structure.ULen = 0;
	RxdBufferStructure.RxBuffer1Structure.rP = RxBuffer1;
	RxdBufferStructure.RxBuffer1Structure.wP = RxBuffer1;
	
	RxdBufferStructure.uartRecv_Counter = 0;
	RxdBufferStructure.dmaCompleteCounter = 0;	
	RxdBufferStructure.dmaReversalValue = RESET;
}



/***********************Extract data from memory heap***************************
@param	p: UartToEtherBufferTypeDef
@param	length:	UartToEtherBufferTypeDef.uartRecv_Counter
reuturn SET:  success
				RESET:The buffer is full,or data length is fault.
*/
u8 WirteToUartRxBufferFromRxBuffer0(UARTBufferTypeDef *p, u16 length)
{
	u8 reval = RESET;
	
	RxdBufferStructure.readwriteLock = SET;/*lock the buffer*/
	
	if(length)
	{
		/*The buffer is full,There's not enough space left*/
		if(length > p->wirteableLength)
		{
			return RESET;
		}
		else
		{
			if(length >= p->WirteResidualLength)
			{
				memcpy(p->wirteP,	p->RxBuffer0Structure.rP, p->WirteResidualLength);
				p->wirteP = heapBuffer_HeadP;
				
				p->RxBuffer0Structure.rP += (uint32_t)p->WirteResidualLength;
				p->RxBuffer0Structure.wP += (uint32_t)p->WirteResidualLength;/*record the RxBuffer0 wirting pointer*/
				
				memcpy(p->wirteP,	(p->RxBuffer0Structure.rP+p->WirteResidualLength), (length - p->WirteResidualLength));
				p->wirteP += (uint32_t)(length - p->WirteResidualLength);/*record the UartRxBuffer wirting point*/
				
				p->RxBuffer0Structure.rP += (uint32_t)(length - p->WirteResidualLength);
				p->RxBuffer0Structure.wP += (uint32_t)(length - p->WirteResidualLength);/*record the RxBuffer0 wirting pointer*/
			}
			else
			{
				memcpy(p->wirteP,	p->RxBuffer0Structure.rP, length);
				p->wirteP += (uint32_t)(length);/*record the UartRxBuffer wirting point*/
				
				p->RxBuffer0Structure.rP += (uint32_t)length;
				p->RxBuffer0Structure.wP += (uint32_t)length;/*record the RxBuffer0 wirting pointer*/
			}
			
			p->WirteResidualLength = (uint16_t)(heapBuffer_HeadP + UART_HEAP_BUFFER_SIZE - p->wirteP);
			
			if(p->wirteP >=	p->readP)
			{
				p->wirteableLength = (uint16_t)((p->readP - heapBuffer_HeadP) + p->WirteResidualLength);
				p->readableLength = (uint16_t)(p->wirteP - p->readP);
			}
			else
			{
				p->wirteableLength = (uint16_t)(p->readP - p->wirteP);
				p->readableLength = (uint16_t)((p->wirteP-heapBuffer_HeadP) + p->ReadResidualLength);
			}
			
			if(p->RxBuffer0Structure.wP > (RxBuffer0 + UART_RX_BUFFER_SIZE - 1))	reval = RESET;
			
			reval = SET;
			}
	}
	else	reval = RESET;
	
	return (reval);
}

/***********************Extract data from memory heap***************************
@param	p: UartToEtherBufferTypeDef
@param	length:	UartToEtherBufferTypeDef.uartRecv_Counter
reuturn SET:  success
				RESET:The buffer is full,or data length is fault.
*/
u8 WirteToUartRxBufferFromRxBuffer1(UARTBufferTypeDef *p, u16 length)
{
	u8 reval = RESET;
	
	RxdBufferStructure.readwriteLock = SET;/*lock the buffer*/
	
	if(length)
	{
		/*The buffer is full,There's not enough space left*/
		if(length > p->wirteableLength)
		{
			return RESET;
		}
		else
		{
			if(length >= p->WirteResidualLength)
			{
				memcpy(p->wirteP,	p->RxBuffer1Structure.rP, p->WirteResidualLength);
				p->wirteP = heapBuffer_HeadP;
				p->RxBuffer1Structure.rP += (uint32_t)p->WirteResidualLength;
				p->RxBuffer1Structure.wP += (uint32_t)p->WirteResidualLength;/*record the RxBuffer0 wirting pointer*/
				
				memcpy(p->wirteP,	(p->RxBuffer1Structure.rP+p->WirteResidualLength), (length - p->WirteResidualLength));
				p->wirteP += (uint32_t)(length - p->WirteResidualLength);/*record the UartRxBuffer wirting point*/
				p->RxBuffer1Structure.rP += (uint32_t)(length - p->WirteResidualLength);
				p->RxBuffer1Structure.wP += (uint32_t)(length - p->WirteResidualLength);/*record the RxBuffer0 wirting pointer*/
			}
			else
			{
				memcpy(p->wirteP,	p->RxBuffer1Structure.rP, length);
				p->wirteP += (uint32_t)(length);/*record the UartRxBuffer wirting point*/
				p->RxBuffer1Structure.rP += (uint32_t)length;
				p->RxBuffer1Structure.wP += (uint32_t)length;/*record the RxBuffer0 wirting pointer*/
			}
			
			p->WirteResidualLength = (uint16_t)(heapBuffer_HeadP + UART_HEAP_BUFFER_SIZE - p->wirteP);
			
			if(p->wirteP >=	p->readP)
			{
				p->wirteableLength = (uint16_t)((p->readP - heapBuffer_HeadP) + p->WirteResidualLength);
				p->readableLength = (uint16_t)(p->wirteP - p->readP);
			}
			else
			{
				p->wirteableLength = (uint16_t)(p->readP - p->wirteP);
				p->readableLength = (uint16_t)((p->wirteP-heapBuffer_HeadP) + p->ReadResidualLength);
			}
			
			if(p->RxBuffer1Structure.wP > (RxBuffer1 + UART_RX_BUFFER_SIZE - 1))	reval = RESET;
			
			reval = SET;
		}
	}
	else	reval = RESET;
	return (reval);
}


/**
  * @brief  initializate RxBuffer0
  * @param  None
  * @retval None
  */
void ClearRxBuffer0WirtePointer(UARTBufferTypeDef *p, u16 dmaITCounter)
{
	if(	p->RxBuffer0Structure.wP >= (RxBuffer0 + UART_RX_BUFFER_SIZE - 1) )
	{
		p->dmaCompleteCounter = dmaITCounter;
		
		p->RxBuffer0Structure.wP = RxBuffer0;
		p->RxBuffer0Structure.rP = RxBuffer0;
		p->RxBuffer0Structure.FLen = UART_RX_BUFFER_SIZE;
		p->RxBuffer0Structure.ULen = 0;
	}
	else
	{
		printf(":%d\r\n",(p->RxBuffer0Structure.wP-RxBuffer0));
	}
}


/**
  * @brief  initializate RxBuffer1
  * @param  None
  * @retval None
  */
void ClearRxBuffer1WirtePointer(UARTBufferTypeDef *p, u16 dmaITCounter)
{
	if(	p->RxBuffer1Structure.wP >= (RxBuffer1 + UART_RX_BUFFER_SIZE - 1) )
	{
		p->dmaCompleteCounter = dmaITCounter;
		
		p->RxBuffer1Structure.wP = RxBuffer1;
		p->RxBuffer1Structure.rP = RxBuffer1;
		p->RxBuffer1Structure.FLen = UART_RX_BUFFER_SIZE;
		p->RxBuffer1Structure.ULen = 0;
	}
	else
	{
		printf(":%d\r\n",(p->RxBuffer1Structure.wP-RxBuffer1));
	}
}



/**
  * @brief  Read the data of heap buffer to extral buffer
  * @param  p, UARTBufferTypeDef
  * @param  ExtralBuffer, A memory area for exchanging data
  * @param  length£¬the length of exchanging data
  * @retval None
  */
u8 ReadHeapBufferToExtralBuffer(UARTBufferTypeDef *p, u8 *ExtralBuffer, u16 length)
{
	u8 reval = RESET;
	
	RxdBufferStructure.readwriteLock = RESET;/*unlock the buffer*/
	
	if(length)
	{
		/*Check whether there is data readable in buffer*/
		if(length > p->readableLength)
		{
			reval = RESET;
		}
		else
		{
			if(length > p->ReadResidualLength)
			{
				memcpy(ExtralBuffer, p->readP, p->ReadResidualLength);
				p->readP = heapBuffer_HeadP;
				
				memcpy((ExtralBuffer+p->ReadResidualLength),	p->readP, (length - p->ReadResidualLength));
				p->readP += (uint32_t)(length - p->ReadResidualLength);/*record the UartRxBuffer reading point*/
			}
			else
			{
				memcpy(ExtralBuffer, p->readP, length);
				p->readP += (uint32_t)length;
			}
			
			p->ReadResidualLength = (uint16_t)(heapBuffer_HeadP + UART_HEAP_BUFFER_SIZE - p->readP);
			
			if(RxdBufferStructure.readwriteLock != RESET)
			{
				return RESET;
			}
			else
			{
				if(p->wirteP >=	p->readP)
				{
					p->wirteableLength = (uint16_t)((p->readP - heapBuffer_HeadP) + p->WirteResidualLength);
					p->readableLength = (uint16_t)(p->wirteP - p->readP);
				}
				else
				{
					p->wirteableLength =  (uint16_t)(p->readP - p->wirteP);
					p->readableLength = (uint16_t)((p->wirteP-heapBuffer_HeadP) + p->ReadResidualLength);
				}
				reval = SET;
			}
		}
	}
	else reval = RESET;
	
	if(RxdBufferStructure.readwriteLock != RESET)  reval = RESET;
	
	return reval;
}


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
void USART_Config(u32 boundrate, u16 WordLength, u16 StopBits, u16 Parity, u16 HardwareFlowControl)
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
  USART_InitStructure.USART_WordLength = WordLength;
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
    USART_Config(UART_BAUDRATE, UART_WORDLENGTH, UART_STOPBITS, UART_PARITY, UART_FLOWCONTROL);
}

void UART_Init(u32 boundrate, u16 WordLength, u16 StopBits, u16 Parity, u16 HardwareFlowControl)
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
    USART_Config(boundrate, WordLength, StopBits, Parity, HardwareFlowControl);
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


/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* Infinite loop */
  while (1)
  {
  }
}
