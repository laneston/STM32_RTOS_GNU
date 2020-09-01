#STM32F4xx_StdPeriph_Driver
SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/src/misc.c
SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c

SRC += ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

STARTUP = ./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/CMSIS/Device/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f40_41xxx.s

INC += -I./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/STM32F4xx_StdPeriph_Driver/inc
INC += -I./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/CMSIS/Device/ST/STM32F4xx/Include
INC += -I./STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/CMSIS/Include