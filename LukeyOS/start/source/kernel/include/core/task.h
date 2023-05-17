/*
 * @Description: 任务实现
 * @Author: Alvin
 * @Date: 2023-04-19 04:55:57
 * @LastEditTime: 2023-05-16 20:36:51
 * @LastEditors: Please set LastEditors
 */
#ifndef TASK_H
#define TASK_H

#include "common/types.h"
#include "cpu/cpu.h"
#include "tools/list.h"

#define TASK_NAME_SIZE 32
#define TASK_TIME_SLICE_DEFAULT 100 // 时间片轮转时间
#define IDLE_STACK_SIZE 1024		// 空闲进程的空闲任务堆栈
#define TASK_FLAG_SYSTEM (1 << 0)	// 系统任务

/**
 * @brief 进程控制块（PCB）：用于跟踪进程状态的数据结构
 */
typedef struct _task_t
{
	tss_t tss;		  // 任务的TSS段描述符
	uint16_t tss_sel; // tss选择子

	// 这里利用之前定义的宏，通过run_node的地址获得结构体的地址
	list_node_t run_node;  // 用于插入就绪队列，插入 run_node 即为插入_task_t任务控制块
	list_node_t all_node;  // 用于插入任务队列，插入 all_node 即为插入_task_t任务控制块
	list_node_t wait_node; // 用于插入等待队列，插入 wait_node 即为插入_task_t任务控制块

	enum
	{
		TASK_CREATED, // 创建态
		TASK_RUNNING, // 运行态
		TASK_SLEEP,	  // 睡眠态
		TASK_READY,	  // 就绪态
		TASK_WAITTING // 阻塞态
	} state;

	char name[TASK_NAME_SIZE];

	// 任务运行时间的设置
	int slice_ticks; // 计数器 = times_ticks 每发生一次定时器中断就会-1
	int times_ticks; // 一个任务所能占用CPU的最大时间
	int sleep_ticks; // 延迟时间，是一个递减的计数器
} task_t;

/**
 * 初始化进程（创建进程）
 * @return {*}
 * @param {task_t}  *task
 * @param {char}    *name   任务的名称
 * @param {int} flag		系统任务标志位
 * @param {uint32_t} entry  入口地址
 * @param {uint32_t} esp    栈顶指针
 */
int task_init(task_t *task, const char *name, int flag,  uint32_t entry, uint32_t esp);
/**
 * 从任务 from 跳转到任务 to
 */
void task_switch_from_to(task_t *from, task_t *to);

// 任务管理器：管理所有的任务，包括当前运行的任务，就绪队列和任务队列
typedef struct _task_manager_t
{
	task_t *curr_task; // 当前运行的任务

	list_t ready_list; // 就绪队列
	list_t task_list;  // 所有已创建任务的队列
	list_t sleep_list; // 睡眠队列

	task_t first_task; // 内核任务
	task_t idle_task;  // 设置空闲进程

	int app_code_sel; // 任务代码段选择子
	int app_data_sel; // 应用任务的数据段选择子

} task_manager_t;

void task_manager_init(void);
void task_first_init(void);
task_t *task_first_task(void);

// 将task任务插入到就绪队列中并设置为就绪态
void task_set_ready(task_t *task);

// 将task任务从就绪态队列中移除
void task_set_block(task_t *task);

//  [已弃用！]进程自己主动放弃CPU，自觉移到队列尾部，从而给其它进程留出运行的机会
int sys_sched_yield(void);

/**
 * @description: 返回当前正在运行的进程
 * @return {*}
 */
task_t *task_current(void);

// 切换下一个欲运行的任务
void task_dispatch();

// 定时器切换任务
void task_time_tick();

/**
 * @description: 设置睡眠队列
 * @return {*}
 * @param {task_t} *task	要睡眠的任务
 * @param {uint32_t} ticks	任务睡眠的时间
 */
void task_set_sleep(task_t *task, uint32_t ticks);

/**
 * @description: 将任务从睡眠队列中移出
 * @return {*}
 * @param {task_t *} task	待移出的任务
 */
void task_set_wakeup(task_t *task);

// 延迟时间
void sys_sleep(uint32_t ms);
// 空闲进程初始化
static void idle_task_entry();
#endif