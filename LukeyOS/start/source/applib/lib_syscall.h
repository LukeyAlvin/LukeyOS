/*
 * @Description: 通常被用于应用程序和操作系统之间的系统调用的接口
 * @Author: Alvin
 * @Date: 2023-05-22 15:15:32
 * @LastEditTime: 2023-06-06 14:47:30
 */
#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include "core/syscall.h"
#include "os_cfg.h"

#include <sys/stat.h>

/**
 * 定义一个结构体，用于存放系统调用的参数列表
 */
typedef struct _syscall_args_t
{
    int id; // 系统调用号
    // 其它参数
    int arg0;
    int arg1;
    int arg2;
    int arg3;
} syscall_args_t;

int msleep (int ms);
int fork(void);
int getpid(void);
int yield (void);
int execve(const char *name, char * const *argv, char * const *env);
int print_msg(char * fmt, int arg);

int open(const char *name, int flags, ...);
int read(int file, char *ptr, int len);
int write(int file, char *ptr, int len);
int close(int file);
int lseek(int file, int ptr, int dir);

int isatty(int file);
int fstat(int file, struct stat *st);
void * sbrk(ptrdiff_t incr);

int dup(int file);

void _exit(int status);

int wait(int * status);
#endif // LIB_SYSCALL_H