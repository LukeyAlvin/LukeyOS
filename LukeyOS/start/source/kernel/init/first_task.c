/*
 * @Description: 内核初始化以及测试代码
 * @Author: Alvin
 * @Date: 2023-05-06 09:34:29
 * @LastEditTime: 2023-06-06 15:05:56
 */
#include "applib/lib_syscall.h"
#include "dev/tty.h"

int first_task_main(void)
{
#if 0
    int count = 3;

    int pid = getpid();
    print_msg("first task id=%d", pid);
    
    pid = fork();
    if (pid < 0) {
        print_msg("create child proc failed.", 0);
    } else if (pid == 0) {
        print_msg("child: %d", count);

        char * argv[] = {"arg0", "arg1", "arg2", "arg3"};
        execve("/shell.elf", argv, (char **)0);
    } else {
        print_msg("child task id=%d", pid);
        print_msg("parent: %d", count);
    }

    pid = getpid();
#endif
    for (int i = 0; i < TTY_NR; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            print_msg("creat shell failed.", 0);
            break;
        }
        else if (pid == 0)
        {
            // 为每一个控制台（tty）创建一个shell
            char tty_num[5] = "tty:?";
            tty_num[4] = i + '0';
            char *argv[] = {tty_num, (char **)0};
            execve("/shell.elf", argv, (char **)0);
            print_msg("create shell proc failed", 0);
            while (1)
            {
                msleep(1000);
            }
        }
    }
    // 回收所有子进程
    while (1)
    {
        int status;
        wait(&status);
    }
    return 0;
}