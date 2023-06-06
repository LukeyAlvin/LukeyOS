/*
 * @Description: 用于启动应用程序
 * @Author: Alvin
 * @Date: 2023-05-24 20:25:35
 * @LastEditTime: 2023-06-06 10:50:50
 */
#include "lib_syscall.h"
#include <stdlib.h>

int main (int argc, char ** argv);


extern uint8_t __bss_start__[], __bss_end__[];

/**
 * @brief 应用的初始化，完成shell中应用的初始化工作和收尾工作
 * 初始化工作，比如就是对bss区清0
 * return xxx，要将xx返回给操作系统，这个值怎么返回的
 * 在main返回后，要调用exit终止进程
 */
void cstart (int argc, char ** argv) {
    // 清空bss区，注意这是必须的！！！
    // 像newlib库中有些代码就依赖于此，未清空时数据未知，导致调用sbrk时申请很大内存空间
    uint8_t * start = __bss_start__;
    while (start < __bss_end__) {
        *start++ = 0;
    }
    // 进入应用程序运行
    exit(main(argc, argv));
}