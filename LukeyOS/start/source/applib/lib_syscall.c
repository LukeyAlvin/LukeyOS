#include "core/syscall.h"
#include "os_cfg.h"
#include "lib_syscall.h"
/**
 * 通用系统调用接口，用于执行系统调用
 * @param {syscall_args_t *} args 系统调用参数
 * @return {*} 返回执行结果
 */
static inline int sys_call(syscall_args_t *args)
{

    // 使用调用门的方式进行系统调用
    // 偏移量为0; SELECTOR_SYSCALL是一个常量，它的值为0x40，表示选择了内核的syscall段,
    // | 0是为了清除低三位，保持段选择器的格式正确。
    const unsigned long sys_gate_addr[] = {0, SELECTOR_SYSCALL | 0};
    int ret;

    // 采用调用门, 这里只支持5个参数
    // 用调用门的好处是会自动将参数复制到内核栈中，这样内核代码很好取参数
    // 而如果采用寄存器传递，取参比较困难，需要先压栈再取
    __asm__ __volatile__(
        "push %[arg3]\n\t"
        "push %[arg2]\n\t"
        "push %[arg1]\n\t"
        "push %[arg0]\n\t"
        "push %[id]\n\t"
        "lcalll *(%[gate])\n\n" // 调用系统调用门
        : "=a"(ret)             // 将返回值存入ret中
        // 将参数和系统调用门的地址传递给汇编语句
        : [arg3] "r"(args->arg3),
          [arg2] "r"(args->arg2),
          [arg1] "r"(args->arg1),
          [arg0] "r"(args->arg0),
          [id] "r"(args->id),
          [gate] "r"(sys_gate_addr));
    return ret; // 返回执行结果
}

/**
 * 休眠函数,调用系统的 sys_sleep函数
 * @param ms 休眠时间，单位为毫秒
 * @return 返回休眠的剩余时间，单位为毫秒
 */
int msleep(int ms)
{
    if (ms <= 0)
    {
        return 0;
    }

    syscall_args_t args;    // 创建一个系统调用参数结构体
    args.id = SYS_msleep;   // 设置系统调用号为SYS_msleep
    args.arg0 = ms;         // 设置休眠时间为第一个参数
    return sys_call(&args); // 调用sys_call函数，执行系统调用
}

/**
 * 获取当前进程的pid
 * @return {*}
 */
int getpid(void)
{

    syscall_args_t args;    // 创建一个系统调用参数结构体
    args.id = SYS_getpid;   // 设置系统调用号为SYS_msleep
    return sys_call(&args); // 调用sys_call函数，执行系统调用
}

// 临时使用的打印函数，只允许传递两个参数 后续使用文件系统
int print_msg(char *fmt, int arg)
{
    syscall_args_t args;
    args.id = SYS_printmsg;
    args.arg0 = (int)fmt;
    args.arg1 = arg;
    return sys_call(&args);
}
int fork()
{
    syscall_args_t args;
    args.id = SYS_fork;
    sys_call(&args);
}
/**
 * 用于执行指定的程序
 * @param {char} *name 要执行的程序的路径名。
 * @param {char} *  要执行的程序的命令行参数
 * @param {char} *  要执行的程序的环境变量
 * @return {*}
 */
int execve(const char *filename, char *const *argv, char *const *env)
{
    syscall_args_t args;
    args.id = SYS_execv;
    args.arg0 = (int)filename;
    args.arg1 = (int)argv;
    args.arg2 = (int)env;
    return sys_call(&args);
}

int yield(void)
{
    syscall_args_t args;
    args.id = SYS_yield;
    return sys_call(&args);
}
int open(const char *name, int flags, ...)
{
    // 不考虑支持太多参数
    syscall_args_t args;
    args.id = SYS_open;
    args.arg0 = (int)name;
    args.arg1 = (int)flags;
    return sys_call(&args);
}

int read(int file, char *ptr, int len)
{
    syscall_args_t args;
    args.id = SYS_read;
    args.arg0 = (int)file;
    args.arg1 = (int)ptr;
    args.arg2 = len;
    return sys_call(&args);
}

int write(int file, char *ptr, int len)
{
    syscall_args_t args;
    args.id = SYS_write;
    args.arg0 = (int)file;
    args.arg1 = (int)ptr;
    args.arg2 = len;
    return sys_call(&args);
}

int close(int file)
{
    syscall_args_t args;
    args.id = SYS_close;
    args.arg0 = (int)file;
    return sys_call(&args);
}

int lseek(int file, int ptr, int dir)
{
    syscall_args_t args;
    args.id = SYS_lseek;
    args.arg0 = (int)file;
    args.arg1 = (int)ptr;
    args.arg2 = dir;
    return sys_call(&args);
}

/**
 * 判断文件描述符与tty关联
 */
int isatty(int file)
{
    syscall_args_t args;
    args.id = SYS_isatty;
    args.arg0 = (int)file;
    return sys_call(&args);
}

/**
 * 获取文件的状态
 */
int fstat(int file, struct stat *st)
{
    syscall_args_t args;
    args.id = SYS_fstat;
    args.arg0 = (int)file;
    args.arg1 = (int)st;
    return sys_call(&args);
}

void *sbrk(ptrdiff_t incr)
{
    syscall_args_t args;
    args.id = SYS_sbrk;
    args.arg0 = (int)incr;
    return (void *)sys_call(&args);
}


int dup (int file) {
    syscall_args_t args;
    args.id = SYS_dup;
    args.arg0 = file;
    return sys_call(&args);
}

void _exit(int status)
{
    syscall_args_t args;
    args.id = SYS_exit;
    args.arg0 = (int)status;
    return (void *)sys_call(&args);
}

int wait(int * status)
{
    syscall_args_t args;
    args.id = SYS_wait;
    args.arg0 = (int)status;
    return sys_call(&args);
}
