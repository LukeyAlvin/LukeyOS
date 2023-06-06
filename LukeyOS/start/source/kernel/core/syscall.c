/*
 * @Description: 系统调用实现
 * @Author: Alvin
 * @Date: 2023-05-22 16:12:03
 * @LastEditTime: 2023-06-06 14:48:54
 */
#include "core/syscall.h"
#include "tools/klib.h"
#include "core/task.h"
#include "tools/log.h"
#include "core/memory.h"
#include "core/task.h"
#include "file/file.h"
// 系统调用处理函数类型
typedef int (*syscall_handler_t)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);

// 临时用函数 
void sys_printmsg(char* fmt, int arg)
{
    log_printf(fmt, arg);
}
// 系统调用表
static const syscall_handler_t sys_table[] = {
	[SYS_msleep] = (syscall_handler_t)sys_msleep,   // 表的 SYS_msleep[0]项 对应的是 sys_msleep函数
    [SYS_getpid] = (syscall_handler_t)sys_getpid, 
    [SYS_fork] = (syscall_handler_t)sys_fork,
    [SYS_execv] = (syscall_handler_t)sys_execv,
    [SYS_yield] = (syscall_handler_t)sys_yield,
    [SYS_exit] = (syscall_handler_t)sys_exit,
    [SYS_wait] = (syscall_handler_t)sys_wait,

	[SYS_open] = (syscall_handler_t)sys_open,
	[SYS_read] = (syscall_handler_t)sys_read,
	[SYS_write] = (syscall_handler_t)sys_write,
	[SYS_close] = (syscall_handler_t)sys_close,
	[SYS_lseek] = (syscall_handler_t)sys_lseek,

	[SYS_isatty] = (syscall_handler_t)sys_isatty,
	[SYS_sbrk] = (syscall_handler_t)sys_sbrk,
	[SYS_fstat] = (syscall_handler_t)sys_fstat,
    [SYS_dup] = (syscall_handler_t)sys_dup,

    [SYS_printmsg] = (syscall_handler_t)sys_printmsg,
	

};



/**
 * 处理系统调用请求，并调用相应的系统调用处理函数。
 * @param {syscall_frame_t *} frame 系统调用的栈信息
 * @return {*} 
 */
void do_handler_syscall (syscall_frame_t * frame) {

    if (frame->func_id < sizeof(sys_table) / sizeof(sys_table[0])) {
		// 查根据系统调用号查找系统调用表中的处理函数
		syscall_handler_t handler = sys_table[frame->func_id];

        // 将系统调用的参数传递给处理函数。如果找到了处理函数，则调用它并将返回值存储在ret变量中
		if (handler) {
			int ret = handler(frame->arg0, frame->arg1, frame->arg2, frame->arg3);
            // eax 可以用来存放函数的返回值
            frame->eax = ret;
            return;
		}
	}

	// 不支持的系统调用，打印出错信息
	task_t * task = task_current();
	log_printf("task: %s, Unknown syscall: %d", task->name,  frame->func_id);
    frame->eax = -1;
}
