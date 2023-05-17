/*
 * @Description: 进程间通信：计数信号量
 * @Author: Alvin
 * @Date: 2023-04-21 16:44:49
 * @LastEditTime: 2023-04-21 17:23:59
 */
#ifndef OS_SEM_H
#define OS_SEM_H

#include "tools/list.h"
#include "cpu/irq.h"
#include "core/task.h"

/**
 * 进程同步用的计数信号量
 */
typedef struct _sem_t {
    int count;				// 信号量计数
    list_t wait_list;		// 等待的进程列表
}sem_t;
/**
 * @description: 计数信号量初始化
 * @return {*}
 * @param {sem_t *} sem      信号量结构体   
 * @param {int} init_count   计数器的值
 */
void sem_init (sem_t * sem, int init_count);


/**
 * @description: 进程申请信号量，无信号量则进入等待队列
 * @return {*}
 * @param {sem_t} *sem  待申请的信号量
 */
void sem_wait (sem_t * sem);


/**
 * @description: 系统释放信号量，有等待进程则移植就绪队列
 * @return {*}
 * @param {sem_t} *sem 待释放的信号量
 */
void sem_notify (sem_t * sem);

// 获取当前信号量的个数 
int sem_count(sem_t *sem);
#endif //OS_SEM_H
