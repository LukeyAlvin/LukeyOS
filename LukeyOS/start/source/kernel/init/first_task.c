/*
 * @Description: 内核初始化以及测试代码
 * @Author: Alvin
 * @Date: 2023-05-06 09:34:29
 * @LastEditTime: 2023-05-10 15:41:10
 */
#include "core/task.h"
#include "tools/log.h"

int first_task_main (void) {
    for (;;) {
        log_printf("first task.");
        sys_sleep(1000);
    }

    return 0;
} 