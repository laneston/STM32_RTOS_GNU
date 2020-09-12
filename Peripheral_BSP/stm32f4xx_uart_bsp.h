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
#ifndef __STM32F4XX_UART_BSP_H
#define __STM32F4XX_UART_BSP_H

#include <stdio.h>
#include "stm32f4xx.h"

#ifdef __ARM_NONE_EABI
  #define PUTCHAR_PROTOTYPE int _write (int fd, char *pBuffer, int size)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __ARM_NONE_EABI */


/*USART buffer size*/
#define UART_RX_BUFFER_SIZE	1024
#define UART_TX_BUFFER_SIZE	1024
/**/
#define UART_HEAP_BUFFER_SIZE	1024*4

/*USART interrupt enable*/
#define UART_IT
#define	UART_IT_IDLE
#define UART_IT_RXNE
#define	UART_DMA_IT
#define	UART_DMA_TX
//#define UART_IT_TC
//#define UART_OverSampling

#define UART_BAUDRATE 115200
#define UART_WORDLENGTH USART_WordLength_8b
#define UART_STOPBITS USART_StopBits_1
#define UART_PARITY USART_Parity_No
#define UART_FLOWCONTROL USART_HardwareFlowControl_None


#define Wait_BlockTime_uart_idle	0
#define Wait_BlockTime_uart_dma     0
#define Wait_BlockTime_uart_tc	    20


/************************************************************** 
** This part is the data structure of serial data storage block
***************************************************************
*/
typedef struct
{
    u8 *rP;
    u8 *wP;
    u16	FLen;
    u16	ULen;
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

extern UARTBufferTypeDef	RxdBufferStructure;

extern void assert_failed(uint8_t* file, uint32_t line);

/*Confuguration of low level*/
extern u8 WirteToUartRxBufferFromRxBuffer0(UARTBufferTypeDef *p, u16 length);
extern u8 WirteToUartRxBufferFromRxBuffer1(UARTBufferTypeDef *p, u16 length);
extern void ClearRxBuffer0WirtePointer(UARTBufferTypeDef *p, u16 dmaITCounter);
extern void ClearRxBuffer1WirtePointer(UARTBufferTypeDef *p, u16 dmaITCounter);
extern u8 ReadHeapBufferToExtralBuffer(UARTBufferTypeDef *p, u8 *ExtralBuffer, u16 length);
extern void UartDmaStreamSend(u8 *buffer, u16 length);

/*Initialization of low level*/
extern void USART_Config(u32 boundrate, u16 WordLength, u16 StopBits, u16 Parity, u16 HardwareFlowControl);
extern void UART_Init(u32 boundrate, u16 WordLength, u16 StopBits, u16 Parity, u16 HardwareFlowControl);
extern void UART_Init_115200(void);
#endif/*__STM32F4XX_UART_BSP_H*/
