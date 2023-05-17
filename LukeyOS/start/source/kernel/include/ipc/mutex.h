/*
 * @Description: 互斥信号量
 * @Author: Alvin
 * @Date: 2023-04-22 09:49:26
 * @LastEditTime: 2023-05-17 15:50:44
 */
#ifndef MUTEX_H
#define MUTEX_H

#include "core/task.h"
#include "tools/list.h"

/**
 * 进程同步用的计数信号量
 */
typedef struct _mutex_t {
    task_t * owner;
    int locked_count;
    list_t wait_list;
}mutex_t;
// 初始化
void mutex_init (mutex_t * mutex);
// 上锁
void mutex_lock (mutex_t * mutex);
// 解锁
void mutex_unlock (mutex_t * mutex);
 
#endif //MUTEX_H
