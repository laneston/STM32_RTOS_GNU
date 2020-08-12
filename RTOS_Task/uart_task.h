/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.0
  * @date    09-May-2020
  * @brief   
	*          
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
	* Website: developerlab.cn
	* Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
#ifndef __UART_TASK_H
#define __UART_TASK_H

#include "task.h"
#include "stm32f4xx.h"

#define UART_BAUDRATE 115200
#define UART_STOPBITS USART_StopBits_1
#define UART_PARITY USART_Parity_No
#define UART_FLOWCONTROL USART_HardwareFlowControl_None

typedef struct
{
	u8 *rP;
	u8 *wP;
	u16	  FLen;
	u16		ULen;
}RxBufferTypeDef;
	
typedef struct
{
	u8 *readP;
	u8 *wirteP;
	
	u8 readwriteLock;
	
	u16		WirteResidualLength;
	u16		ReadResidualLength;
	u16		wirteableLength;
	u16		readableLength;
	u16		FreeLength;
	u16		UsedLength;
	
	RxBufferTypeDef	RxBuffer0Structure;
	RxBufferTypeDef	RxBuffer1Structure;
	
	u16 	uartRecv_Counter;
	u16		dmaCompleteCounter;
	u8		dmaReversalValue;
	
	u32   send_counter;
	u32   receive_counter;
}UARTBufferTypeDef;


extern TaskHandle_t UART1_Receive_Task_Handle;
extern UARTBufferTypeDef	RxdBufferStructure;

extern void UartParam_Init(void);
extern void UART1_Receive_Task(void);
extern void UartRxBufferPointer_Init(void);
extern u8 WirteToUartRxBufferFromRxBuffer0(UARTBufferTypeDef *p, u16 length);
extern u8 WirteToUartRxBufferFromRxBuffer1(UARTBufferTypeDef *p, u16 length);
extern void ClearRxBuffer0WirtePointer(UARTBufferTypeDef *p, u16 dmaITCounter);
extern void ClearRxBuffer1WirtePointer(UARTBufferTypeDef *p, u16 dmaITCounter);
extern u8 ReadUartRxBufferToEtherBuffer(UARTBufferTypeDef *p, u8 *EtherBuffer, u16 length);
extern void UartDmaStreamSend(u8 *buffer, u16 length);

#endif /* __UART_TASK_H*/
