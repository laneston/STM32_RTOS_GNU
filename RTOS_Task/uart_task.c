/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.0
  * @date    09-May-2020
  * @brief   Ethernet low level content initialization
	*          Enable GPIO, DMA on RMII interface
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

#define Wait_BlockTime_uart_idle	0
#define Wait_BlockTime_uart_dma   0
#define Wait_BlockTime_uart_tc	  20
	
static uint8_t *uartToEtherRxBuffer_HeadP = NULL;

UARTBufferTypeDef	RxdBufferStructure;

TaskHandle_t UART1_Receive_Task_Handle;


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
			{/*current memory target is 1, Memory 0 is full, We process data for memory 0*/
				WirteToUartRxBufferFromRxBuffer0(&RxdBufferStructure, (RxdBufferStructure.dmaCompleteCounter));
				RxdBufferStructure.RxBuffer0Structure.wP = RxBuffer0;
				RxdBufferStructure.RxBuffer0Structure.rP = RxBuffer0;
				RxdBufferStructure.RxBuffer0Structure.FLen = UART_RX_BUFFER_SIZE;
				RxdBufferStructure.RxBuffer0Structure.ULen = 0;
//				ClearRxBuffer0WirtePointer(&RxdBufferStructure, RxdBufferStructure.uartRecv_Counter);
			}
			else
			{/*current memory target is 0, Memory 1 is full, We process data for memory 1*/
				WirteToUartRxBufferFromRxBuffer1(&RxdBufferStructure, (RxdBufferStructure.dmaCompleteCounter));
				RxdBufferStructure.RxBuffer1Structure.wP = RxBuffer1;
				RxdBufferStructure.RxBuffer1Structure.rP = RxBuffer1;
				RxdBufferStructure.RxBuffer1Structure.FLen = UART_RX_BUFFER_SIZE;
				RxdBufferStructure.RxBuffer1Structure.ULen = 0;
//				ClearRxBuffer1WirtePointer(&RxdBufferStructure, RxdBufferStructure.uartRecv_Counter);
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
  * @brief  UartParam_Config
  * @param  None
  * @retval None
  */
void UartParam_Init(void)
{
	RxBuffer0 = stSramMalloc(&HeapStruct_SRAM1, UART_RX_BUFFER_SIZE);
	RxBuffer1 = stSramMalloc(&HeapStruct_SRAM1, UART_RX_BUFFER_SIZE);
	
	TxBuffer = stSramMalloc(&HeapStruct_SRAM1, UART_RX_BUFFER_SIZE);
	
	EmbeverStruct.uartdev.BaudRate = UART_BAUDRATE;
	EmbeverStruct.uartdev.StopBits = UART_STOPBITS;
	EmbeverStruct.uartdev.Parity = UART_PARITY;
	EmbeverStruct.uartdev.HardwareFlowControl = UART_FLOWCONTROL;
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
	uartToEtherRxBuffer_HeadP = stSramMalloc(&HeapStruct_SRAM1, UART_ETHER_BUFFER_SIZE);
	if(uartToEtherRxBuffer_HeadP == NULL){}
		
	RxdBufferStructure.readwriteLock = RESET;
	
	RxdBufferStructure.wirteP = uartToEtherRxBuffer_HeadP;
	RxdBufferStructure.readP = uartToEtherRxBuffer_HeadP;
	RxdBufferStructure.WirteResidualLength = UART_ETHER_BUFFER_SIZE;
	RxdBufferStructure.ReadResidualLength = UART_ETHER_BUFFER_SIZE;
	RxdBufferStructure.wirteableLength = UART_ETHER_BUFFER_SIZE;
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
@param	p:			UartToEtherBufferTypeDef
@param	length:	UartToEtherBufferTypeDef.uartRecv_Counter
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
				p->wirteP = uartToEtherRxBuffer_HeadP;
				
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
			
			p->WirteResidualLength = (uint16_t)(uartToEtherRxBuffer_HeadP + UART_ETHER_BUFFER_SIZE - p->wirteP);
			
			if(p->wirteP >=	p->readP)
			{
				p->wirteableLength = (uint16_t)((p->readP - uartToEtherRxBuffer_HeadP) + p->WirteResidualLength);
				p->readableLength = (uint16_t)(p->wirteP - p->readP);
			}
			else
			{
				p->wirteableLength = (uint16_t)(p->readP - p->wirteP);
				p->readableLength = (uint16_t)((p->wirteP-uartToEtherRxBuffer_HeadP) + p->ReadResidualLength);
			}
			
			if(p->RxBuffer0Structure.wP > (RxBuffer0 + UART_RX_BUFFER_SIZE - 1))	reval = RESET;
			
			reval = SET;
			}
	}
	else	reval = RESET;
	
	return (reval);
}

/***********************Extract data from memory heap***************************
@param	p:			UartToEtherBufferTypeDef
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
				p->wirteP = uartToEtherRxBuffer_HeadP;
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
			
			p->WirteResidualLength = (uint16_t)(uartToEtherRxBuffer_HeadP + UART_ETHER_BUFFER_SIZE - p->wirteP);
			
			if(p->wirteP >=	p->readP)
			{
				p->wirteableLength = (uint16_t)((p->readP - uartToEtherRxBuffer_HeadP) + p->WirteResidualLength);
				p->readableLength = (uint16_t)(p->wirteP - p->readP);
			}
			else
			{
				p->wirteableLength = (uint16_t)(p->readP - p->wirteP);
				p->readableLength = (uint16_t)((p->wirteP-uartToEtherRxBuffer_HeadP) + p->ReadResidualLength);
			}
			
			if(p->RxBuffer1Structure.wP > (RxBuffer1 + UART_RX_BUFFER_SIZE - 1))	reval = RESET;
			
			reval = SET;
		}
	}
	else	reval = RESET;
	return (reval);
}

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

u8 ReadUartRxBufferToEtherBuffer(UARTBufferTypeDef *p, u8 *EtherBuffer, u16 length)
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
				memcpy(EtherBuffer, p->readP, p->ReadResidualLength);
				p->readP = uartToEtherRxBuffer_HeadP;
				
				memcpy((EtherBuffer+p->ReadResidualLength),	p->readP, (length - p->ReadResidualLength));
				p->readP += (uint32_t)(length - p->ReadResidualLength);/*record the UartRxBuffer reading point*/
			}
			else
			{
				memcpy(EtherBuffer, p->readP, length);
				p->readP += (uint32_t)length;
			}
			
			p->ReadResidualLength = (uint16_t)(uartToEtherRxBuffer_HeadP + UART_ETHER_BUFFER_SIZE - p->readP);
			
			if(RxdBufferStructure.readwriteLock != RESET)
			{
				return RESET;
			}
			else
			{
				if(p->wirteP >=	p->readP)
				{
					p->wirteableLength = (uint16_t)((p->readP - uartToEtherRxBuffer_HeadP) + p->WirteResidualLength);
					p->readableLength = (uint16_t)(p->wirteP - p->readP);
				}
				else
				{
					p->wirteableLength =  (uint16_t)(p->readP - p->wirteP);
					p->readableLength = (uint16_t)((p->wirteP-uartToEtherRxBuffer_HeadP) + p->ReadResidualLength);
				}
				reval = SET;
			}
		}
	}
	else reval = RESET;
	
	if(RxdBufferStructure.readwriteLock != RESET)  reval = RESET;
	
	return reval;
}

