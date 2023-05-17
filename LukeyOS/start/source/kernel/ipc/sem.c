/*
 * @Description: 进程间通信：计数信号量
 * @Author: Alvin
 * @Date: 2023-04-21 16:45:01
 * @LastEditTime: 2023-04-21 17:25:49
 */
#include "ipc/sem.h"

/**
 * 信号量初始化
 */
void sem_init(sem_t *sem, int init_count)
{
    sem->count = init_count;
    list_init(&sem->wait_list);
}
/**
 * @description: 进程申请信号量，无信号量则进入等待队列
 * @return {*}
 * @param {sem_t} *sem  待申请的信号量
 */
void sem_wait(sem_t *sem)
{
    // 该信号量可能会被多个进程同时访问，需要设置临界资源保护
    irq_state_t state = irq_enter_protection();
    // 首先判断信号量计数是否为 0
    if (sem->count > 0)
    {
        sem->count--; // 不为 0 则拿走一个信号量 继续执行进程
    }
    else // 为 0 则当前进程移出继续队列 进入等待队列的队尾进行等待
    {
        task_t *curr = task_current();
        task_set_block(curr);
        list_insert_last(&sem->wait_list, &curr->wait_node);
        task_dispatch();
    }
    irq_leave_protection(state);
}
/**
 * @description: 系统释放信号量，有等待进程则移植就绪队列
 * @return {*}
 * @param {sem_t} *sem 待释放的信号量
 */
void sem_notify(sem_t *sem)
{
    // 该信号量可能会被多个进程同时访问，需要设置临界资源保护
    irq_state_t state = irq_enter_protection();

    if (list_count(&sem->wait_list)) // 等待队列中有进程
    {
        // 将等待队列的第一个进程放入就绪队列中
        list_node_t *node = list_remove_first(&sem->wait_list);
        task_t *task = list_node_parent(node, task_t, wait_node);
        task_set_ready(task);
        task_dispatch();
    }
    else // 等待队列中无进程则可用信号量++
    {
        sem->count++;
    }
    irq_leave_protection(state);
}

int sem_count(sem_t *sem)
{
    irq_state_t state = irq_enter_protection();
    int count = sem->count;
    irq_leave_protection(state);
    return count;
}