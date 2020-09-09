/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.1
  * @date    09-Aug-2020
  * @brief   FSMC Configuration & SRAM/NAND initialization
	*          Enable GPIO & configurate timming
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
	*
	* website: developerlab.cn
	*
	* Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
  
#ifndef __STM32F4XX_FSMC_BSP_H
#define __STM32F4XX_FSMC_BSP_H

#define FSMC_NWAIT          PDin(6)
#define NAND_WP             PAout(0)
#define NAND_PT             PFout(11)

extern void SRAM_Initilization(void);
extern void NAND_Initilization(void);
#endif/*__STM32F4XX_FSMC_BSP_H*/
