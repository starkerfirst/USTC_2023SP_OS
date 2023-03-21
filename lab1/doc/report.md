## Lab1 report

PB20000328 杨博涵



### 实验原理

编写一个头部满足MultiBoot协议的OS内核，实现VGA、UART输出。qemu使用的是grub bootloader，支持MultiBoot启动，只要我们的内核头部满足MultiBoot协议就能被启动。

内核内容是在vga显存处写数据，同时在uart端口上写数据。

### 源代码说明

multibootHeader.s : 内核汇编代码

multibootHeader.ld : ld的script file，描述output file属性

Makefile : make文件

### 运行结果

内核没有指定存放地址，默认从0x0开始存代码。

0xB8000 ~ 0xB801B是本次实验使用到的VGA显存地址，内核中在这块地址上写了“PB20000328_ybh”字符串，同理uart是一个一个字节从串口输出到stdio。编译只需要在命令行使用make命令即可。

下图为结果截图

VGA：

![VGA](D:\file_from_desktop\USTC_2023SP_OS\lab1\VGA.png)

UART：![uart](D:\file_from_desktop\USTC_2023SP_OS\lab1\uart.png)

### 遇到的问题

第一次实验的耗时主要在搭建wsl2 ubuntu环境和了解gcc/ld/make等指令的用法。