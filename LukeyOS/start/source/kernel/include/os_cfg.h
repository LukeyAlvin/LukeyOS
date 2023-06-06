/*
 * @Description: os配置
 * @Author: Alvin
 * @Date: 2023-04-17 10:16:07
 * @LastEditTime: 2023-05-22 20:47:30
 */
#ifndef OS_OS_CFG_H
#define OS_OS_CFG_H

#define KERNEL_SELECTOR_CS (1 * 8)   // 内核代码段描述符
#define KERNEL_SELECTOR_DS (2 * 8)   // 内核数据段描述符
#define KERNEL_STACK_SIZE (8 * 1024) // 内核栈
#define SELECTOR_SYSCALL (3 * 8)     // 调用门的选择子

#define OS_TICK_MS 10      // 每毫秒的时钟数
#define OS_VERSION "1.0.0" // OS版本号
#define TASK_NR 128        // 支持的最大进程数

#endif // OS_OS_CFG_H
