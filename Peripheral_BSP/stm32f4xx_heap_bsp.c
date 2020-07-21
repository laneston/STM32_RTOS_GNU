 /** 
	* @version V1.0.0
  * @date    01-Jue-2020
  * @brief   Heap allocations
	*	@note    This memory allocation method is only suitable for single SRAM¡£
	*					 	The whole SRAM cannot be shared with other stacks
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
	*
	* Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
#include "main.h"


/*
*function: Initialize SRAM.
*param: p,				object of Heap_TypeDef Struct.
*param: heapHead,	the head address of SRAM.
*param: heapSize, the size of SRAM that can be use.
*/
void stSramInit(Heap_TypeDef *p, u32 headAddr, u32 heapSize)
{
	u32 offset = 0;
#ifdef HEAP_DEBUG
	UART_Init_115200();
#endif
	offset = (u32)(headAddr%ALIGNMENT_SIZE);
#ifdef HEAP_DEBUG
	printf("offset:%d\r\n",offset);
#endif
	/*alignment*/
	if(offset)
	{
		p->heap_head = (void *)(headAddr + ALIGNMENT_SIZE - offset);
	}
	else
	{
		p->heap_head = (void *)headAddr;
	}
#ifdef HEAP_DEBUG
	printf("heapHead:%p\r\n",p->heap_head);
#endif
	
	offset = (headAddr + heapSize)%ALIGNMENT_SIZE;
#ifdef HEAP_DEBUG
	printf("offset:%d\r\n",offset);
#endif
	if(offset)
	{
		p->heap_tail = (void *)(headAddr + heapSize - offset);
	}
	else
	{
		p->heap_tail = (void *)(headAddr + heapSize);
	}
#ifdef HEAP_DEBUG
	printf("heapTail:%p\r\n",p->heap_tail);
#endif	
	
	p->heap_size = ((u32)((u8 *)p->heap_tail - (u8 *)p->heap_head));
#ifdef HEAP_DEBUG
	printf("heap_size:%d\r\n",p->heap_size);
#endif
	
	p->heap_current = p->heap_head;
	p->heap_residual = p->heap_size;
#ifdef HEAP_DEBUG
	printf("heap_residual:%d\r\nheap_current:%p\r\n",p->heap_residual,p->heap_current);
#endif
}

/*
*function: Allocate memory.
*param: p,				    object of Heap_TypeDef Struct.
*param: xWantedSize,	the size you need.
*return: the pointer of the memory.
*/
void *stSramMalloc(Heap_TypeDef *p, u32 xWantedSize )
{
	void *pvReturn = NULL;
	u32 offset, current_offset;
	
	if(xWantedSize)
	{
		pvReturn = p->heap_current;	
	  offset = xWantedSize%ALIGNMENT_SIZE;
#ifdef HEAP_DEBUG
    printf("offset:%d\r\n",offset);
#endif		
		if(offset)
		{
			current_offset = xWantedSize + ALIGNMENT_SIZE - offset;
			p->heap_current = (void *)((u8 *)p->heap_current + current_offset);
		}
		else
		{
			p->heap_current = (void *)((u8 *)p->heap_current + xWantedSize);
		}
		p->heap_residual = (u32)((u8 *)p->heap_tail - (u8 *)p->heap_current);
#ifdef HEAP_DEBUG
	  printf("heap_residual:%d\r\nheap_current:%p\r\n",p->heap_residual,p->heap_current);
#endif
	}
	else
	{
		pvReturn = NULL;
	}	
	return pvReturn;
}
