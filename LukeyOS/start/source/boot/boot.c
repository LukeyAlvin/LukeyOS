/*
 * @Description: 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * boot扇区容量较小，仅512字节。由于dbr占用了不少字节，导致其没有多少空间放代码，
 * 所以功能只能最简化,并且要开启最大的优化-os
 * @Author: Alvin
 * @Date: 2023-04-11 20:38:18
 * @LastEditTime: 2023-05-17 15:47:23
 */
__asm__(".code16gcc");

#include "boot.h"

#define LOADER_START_ADDR 0x8000   // loader加载的地址 
/**
 * Boot的C入口函数
 * 只完成一项功能，即从磁盘找到loader文件然后加载到内存中，并跳转过去
 */
void boot_entry(void) {
    ((void (*)(void))LOADER_START_ADDR)();
} 

