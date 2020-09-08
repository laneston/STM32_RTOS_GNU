/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.1
  * @date    09-Aug-2020
  * @brief   on-and off-chip memory allocation
  *          How to use it.
  *          1. initializate memory space.[void stSramInit(Heap_TypeDef *p, u32 headAddr, u32 heapSize)]
  *          2. Allocate memory.[void *stSramMalloc(Heap_TypeDef *p, u32 xWantedSize )]
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
  *
  * website: developerlab.cn
  *
  * Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
  
#ifndef __STM32F4XX_HEAP_BSP_H
#define __STM32F4XX_HEAP_BSP_H

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	
#define STM32F4XX_SRAM1_START			(u32)0x20000000
#define STM32F4XX_SRAM1_SIZE			(u32)0x20000
#define STM32F4XX_eSRAM_START			(u32)0x68000000
#define STM32F4XX_eSRAM_SIZE			(u32)0x80000
	
#define ALIGNMENT_SIZE						4
	
#pragma pack(4)
typedef struct
{
	void *heap_head;
	void *heap_current;
	void *heap_tail;
	
	u32 heap_size;
	u32 heap_residual;
}Heap_TypeDef;
#pragma pack()
	
extern Heap_TypeDef HeapStruct_SRAM1;
extern Heap_TypeDef HeapStruct_eSRAM;

extern void stSramInit(Heap_TypeDef *p, u32 heapHead, u32 heapSize);
extern void *stSramMalloc(Heap_TypeDef *p, u32 xWantedSize );

#ifdef __cplusplus
}
#endif
#endif /* __STM32F4XX_HEAP_BSP_H */

