/*
 * @Description: 系统调用实现
 * @Author: Alvin
 * @Date: 2023-05-22 10:35:28
 * @LastEditTime: 2023-06-06 14:48:35
 */
#ifndef OS_SYSCALL_H
#define OS_SYSCALL_H

#define SYSCALL_PARAM_COUNT     5       	// 系统调用最大支持的参数
#define SYS_msleep 0
#define SYS_getpid 1
#define SYS_fork 2
#define SYS_execv 3
#define SYS_yield 4
#define SYS_exit 5
#define SYS_wait 6

#define SYS_open                50
#define SYS_read                51
#define SYS_write               52
#define SYS_close               53
#define SYS_lseek				54
#define SYS_isatty              55
#define SYS_sbrk                56
#define SYS_fstat               57
#define SYS_dup               	58

#define SYS_printmsg 100 



/**
 * 系统调用的栈信息
 * 用于在系统调用处理程序中保存系统调用的参数和状态，以便处理程序可以正确地处理系统调用请求
 */
typedef struct _syscall_frame_t {
	int eflags;
	int gs, fs, es, ds;
    // dummy就是esp 为了避免与后面的esp冲突
	int edi, esi, ebp, dummy, ebx, edx, ecx, eax;
	int eip, cs;
	int func_id, arg0, arg1, arg2, arg3;
	int esp, ss;
}syscall_frame_t;

void exception_handler_syscall (void);		// syscall处理

#endif //OS_SYSCALL_H
