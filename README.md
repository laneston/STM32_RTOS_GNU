# STM32_RTOS_GUN
This is a project template about RTOS in STM32F4xx. Compilated by keil_v5 and GCC Cross compilation chain.



## 问题集锦

### 链接错误提示一

在进行编译的时候，我们会看到这样的警告：

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

可以引用静态库 libnosys.a 来解决，在编译标签 LFLAGS 添加如下参数：

```
--specs=nosys.specs
```
