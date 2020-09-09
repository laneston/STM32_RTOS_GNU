BIN = ./output/STM32_RTOS_GUN.bin
ELF = ./output/STM32_RTOS_GUN.elf

FLASH_LD = ./User/STM32F417IG_FLASH.ld

include FreeRTOS.mk
include STM32F4xx.mk

SRC += ./User/main.c
SRC += ./User/delay.c
SRC += ./RTOS_Task/*.c
SRC += ./Peripheral_BSP/*.c

INC += -I./Peripheral_BSP
INC += -I./RTOS_Task
INC += -I./User

DEF += -DSTM32F40_41xxx 
DEF += -DUSE_STDPERIPH_DRIVER
DEF += -D__ARM_NONE_EABI

CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard
CFLAGS += -mthumb
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -Os
CFLAGS += -Wall
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections

LFLAGS += -mcpu=cortex-m4
LFLAGS += -mfloat-abi=hard
LFLAGS += -mthumb
LFLAGS += -mfpu=fpv4-sp-d16
LFLAGS += -Wl,--gc-sections
LFLAGS += --specs=nosys.specs
LFLAGS += -Wl,-Map=STM32_RTOS_GUN.map

SRC_RAW = $(wildcard $(SRC))
OBJ = $(SRC_RAW:%.c=%.o)

STARTUP_RAW = $(wildcard $(STARTUP))
STARTUP_OBJ = $(STARTUP_RAW:%.s=%.o)

OUTPUT_OBJ = ./output/*.o

all:$(OBJ) $(STARTUP_OBJ) $(ELF) $(BIN)
	@echo $(ELF)
	@echo $(BIN)
	@echo

$(BIN):$(ELF)
	arm-none-eabi-objcopy -O binary -S $< $@
	@echo

$(ELF):
	arm-none-eabi-gcc -o $@ $(LFLAGS) $(OUTPUT_OBJ) -T$(FLASH_LD)
	mv *.map ./output
	@echo

%.o:%.s
	arm-none-eabi-gcc -c $(CFLAGS) $< -o $@
	-mv $@ ./output
	@echo

%.o:%.c
	arm-none-eabi-gcc -c $(CFLAGS) $(DEF) $(INC) $< -o $@
	-mv $@ ./output
	@echo

.PHONY: clean
clean :
	-rm ./output/*.o\
	    ./output/*.elf\
		./output/*.bin\
		./output/*.map
