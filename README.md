## 编译方式

本工程支持的编译方式有两种：IDE 编译器 keil_v5(MDK) 与交叉编译链 arm-none-eabi-

### keil_v5(MDK)

工程文件的目录路径： ../MDK/STM32_RTOS_GUN.uvprojx

值得注意的是，因为 keil_v5 封装的编译链和 arm-none-eabi- 是不一样的，所以底层的 printf 函数接口也是不一样的，这个下面会有相关介绍。

### arm-none-eabi-

这个工程也能使用交叉编译链 arm-none-eabi- 进行编辑，Makefile 的详细内容不在这里赘述，详情请看 <a href = "https://blog.csdn.net/weixin_39177986/article/details/108125580"> 交叉编译链下的Makefile(STM32F4xx) </a>。

值得注意的是，本工程为了使代码核心速率最大化，所以讲 STM32F4xx 的核心代码与 RTOS 放在 CCM RAM 中运行，所以链接文件也做了相关修改，详情请看 <a href = "https://blog.csdn.net/weixin_39177986/article/details/108455827">LD链接脚本解析-STM32F4xx</a>。

## arm-none-eabi- 编译环境

本工程的运行环境： Ubuntu 20.04 (我使用的是 windows 子系统)

**所需安装的程序**

1. make
2. arm-none-eabi-

**使用方式**

```
make
```

**清除目标文件**

```
make clean
```

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
