/*
 * @Description: 定时器相关设置
 * @Author: Alvin
 * @Date: 2023-04-18 09:54:30
 * @LastEditTime: 2023-04-20 19:55:41
 */
#ifndef TIME_H_
#define TIME_H_
#include "common/types.h"
#include "os_cfg.h"
#include "cpu/irq.h"
#include "core/task.h"

#define PIT_OSC_FREQ                1193182				// 定时器时钟

// 定时器的寄存器和各项位配置
#define PIT_CHANNEL0_DATA_PORT       0x40       // 数据端口
#define PIT_COMMAND_MODE_PORT        0x43       // 命令模式端口

#define PIT_CHANNLE0                (0 << 6)
#define PIT_LOAD_LOHI               (3 << 4)
#define PIT_MODE3                   (3 << 1)

// 定时器初始化
void time_init (void);
// 定时器中断处理函数
void exception_handler_timer (void);

#endif