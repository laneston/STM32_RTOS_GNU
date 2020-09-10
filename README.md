# STM32_RTOS_GUN
This is a project template about RTOS in STM32F4xx. Compilated by keil_v5 and GCC Cross compilation chain.

## 使用方式

本工程编译方式有两种：keil_v5 或者交叉编译链 arm-none-eabi-

## 链接文件解析

文件 STM32F417IG_FLASH.ld 是用来存放芯片内存信息，笔者想向大家说明的是，该文件与 Makefile 类似，同样为脚本文件，不参与代码编译。以下笔者将对这个文件进行一个简单的解析。

首先我们需要普及几个简单的知识点：

1. .text   段是用来存放程序执行代码的区；
2. .data   段通常是用来存放程序中已初始化的全局变量的一块内存区域，属于静态内存分配。
3. .bss    段通常是指用来存放程序中未初始化的全局变量的一块内存区域。属于静态内存分配。
4. .rodata 段通常是指用来存放程序中常量的一块内存区域。属于静态内存分配。

下面我们就开始正式进行分析：

### part 0

ENTRY(Reset_Handler)

第一条运行的指令被称为入口点entry point,可以使用ENTRY链接脚本命令设置entry point，参数是一个符号名。有几种方法可以设置entry point,链接器会按照如下的顺序来尝试各种方法，只要任何一种方法成功则会停止：

1. the ‘-e’ entry command-line option;
2. the ENTRY(symbol) command in a linker script;
3. the value of the symbol start, if defined;
4. the address of the first byte of the ‘.text’ section, if present;
5. The address 0

### part 1
_estack = 0x2001FFFF;

这段声明内存末尾地址。

### part 2

_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;

这段定义了堆和栈的最小空间大小。如果定义的数值不符合内存的规格，在编译时会产生链接错误。

### part 3

```
MEMORY
{
FLASH (rx)       : ORIGIN = 0x8000000, LENGTH = 1024K
RAM (xrw)        : ORIGIN = 0x20000000, LENGTH = 128K
CCMRAM (rw)      : ORIGIN = 0x10000000, LENGTH = 64K
}
```

这段定义了 FLASH RAM 和 CCMRAM 的大小信息，(xrw)表明了权限，r是读、w是写、x是执行。

### part 4

```
SECTIONS
{
    ...
    secname:{
        contents
    }
    ...
}
```

它是脚本文件中最重要的元素，不可缺省。它的作用就是用来描述输出文件的布局。secname 和 contents 是必须的，其他都是可选的参数。关于链接脚本更为详细解的析内容请看 <a href = "https://github.com/laneston/Note/blob/master/LdScript_Note.md"> LdScript_Note </a>。

## 注意事项

### printf函数重映射

在 GCC 中没有 Keil_v5 的 MicroLib 库，是调用标准库实现的 printf 函数，GCC 标准库的底层是使用 _write() 函数实现输出的。

```
#define PUTCHAR_PROTOTYPE int _write (int fd, char *pBuffer, int size)

PUTCHAR_PROTOTYPE
{
  u16 i;
  for(i=0; i<size; i++)
  {
	  USART_SendData(USART1, (uint8_t)pBuffer[i]);
	  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  }
  return i;
}
```

### CCMRAM

为了加速程勋的运行速率，本项目内核与系统运行在 CCM RAM 中，所以链接脚本做了相关修改：

```
CCMRAM (xrw)     : ORIGIN = 0x10000000, LENGTH = 64K

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >CCMRAM AT>FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >CCMRAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >CCMRAM
```

以上主要是将 RAM 映射改为 CCMRAM 。值得注意的是，当使用起始地址为 0x2000 0000 的 RAM ，或者使用起始地址为 0x6800 0000 的片外 SRAM 时，需要使用项目中的 Peripheral_BSP\stm32f4xx_heap_bsp.c 文件用于片内/外内存的定义。

### 链接错误提示

在进行编译的时候，出现这样的警告：

```
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-sbrkr.o): in function `_sbrk_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/sbrkr.c:51: undefined reference to `_sbrk'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-writer.o): in function `_write_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/writer.c:49: undefined reference to `_write'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-closer.o): in function `_close_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/closer.c:47: undefined reference to `_close'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-fstatr.o): in function `_fstat_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/fstatr.c:55: undefined reference to `_fstat'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-isattyr.o): in function `_isatty_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/isattyr.c:52: undefined reference to `_isatty'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-lseekr.o): in function `_lseek_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/lseekr.c:49: undefined reference to `_lseek'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-readr.o): in function `_read_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/readr.c:49: undefined reference to `_read'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-abort.o): in function `abort':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/stdlib/../../../../../../../../newlib/libc/stdlib/abort.c:59: undefined reference to `_exit'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-signalr.o): in function `_kill_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/signalr.c:53: undefined reference to `_kill'
/usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/bin/ld: /usr/lib/gcc/arm-none-eabi/9.2.1/../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(lib_a-signalr.o): in function `_getpid_r':
/build/newlib-CVVEyx/newlib-3.3.0/build/arm-none-eabi/thumb/v7e-m+fp/hard/newlib/libc/reent/../../../../../../../../newlib/libc/reent/signalr.c:83: undefined reference to `_getpid'
```

造成这个错误的原因是使用 printf ，scanf ，malloc 等函数，需要实现 `_read' `_lseek' `_isatty' `_fstat' `_write' `_sbrk' 函数。因为使用 GNU 编译的时候，printf 底层实现已经不是 fputc 了，而是__write：

可以引用静态库 libnosys.a 来解决，在 Makefile 的编译标签 LFLAGS 添加如下参数：

```
--specs=nosys.specs
```
