/*
 * @Description: 简单的命令行解释器
 * @Author: Alvin
 * @Date: 2023-05-24 20:16:00
 * @LastEditTime: 2023-06-06 14:57:16
 */
#include "lib_syscall.h"
#include <stdio.h>
#include "main.h"
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

static cli_t cli;
static const char *promot = "LukeyOS@shell >>";

/**
 * 显示命令行提示符
 */
static void show_promot(void)
{
    printf("%s", cli.promot);
    fflush(stdout);
}

/**
 * help命令
 */
static int do_help(int argc, char **argv)
{
    const cli_cmd_t *start = cli.cmd_start;
    if (start < cli.cmd_end)
    {
        printf("%s, %s\n", start->name, start->useage);
    }
    return 0;
}

/**
 * 清屏命令
 */
static int do_clear(int argc, char **argv)
{
    printf("%s", ESC_CLEAR_SCREEN);
    printf("%s", ESC_MOVE_CURSOR(0, 0));
    return 0;
}

/**
 * 回显命令
 */
static int do_echo(int argc, char **argv)
{
    // 只有一个参数，需要先手动输入，再输出
    if (argc == 1)
    {
        char msg_buf[128];

        fgets(msg_buf, sizeof(msg_buf), stdin);
        msg_buf[sizeof(msg_buf) - 1] = '\0';
        puts(msg_buf);
        return 0;
    }

    // https://www.cnblogs.com/yinghao-liu/p/7123622.html
    // optind是下一个要处理的元素在argv中的索引
    // 当没有选项时，变为argv第一个不是选项元素的索引。
    int count = 1; // 缺省只打印一次
    int ch;
    while ((ch = getopt(argc, argv, "n:h")) != -1)
    {
        switch (ch)
        {
        case 'h':
            puts("echo echo any message");
            puts("Usage: echo [-n count] msg");
            optind = 1; // getopt需要多次调用，需要重置
            return 0;
        case 'n':
            count = atoi(optarg);
            break;
        case '?':
            if (optarg)
            {
                fprintf(stderr, "Unknown option: -%s\n", optarg);
            }
            optind = 1; // getopt需要多次调用，需要重置
            return -1;
        }
    }

    // 索引已经超过了最后一个参数的位置，意味着没有传入要发送的信息
    if (optind > argc - 1)
    {
        fprintf(stderr, "Message is empty \n");
        optind = 1; // getopt需要多次调用，需要重置
        return -1;
    }

    // 循环打印消息
    char *msg = argv[optind];
    for (int i = 0; i < count; i++)
    {
        puts(msg);
    }
    optind = 1; // getopt需要多次调用，需要重置
    return 0;
}

/**
 * 程序退出命令
 */
static int do_exit (int argc, char ** argv) {
    exit(0);
    return 0;
}

static const cli_cmd_t cmd_list[] =
    {
        {
            .name = "help",
            .useage = "help -- list support command",
            .do_func = do_help,
        },
        {
            .name = "clear",
            .useage = "clear -- clear screen",
            .do_func = do_clear,
        },
        {
            .name = "echo",
            .useage = "echo [-n count] msg --echo something",
            .do_func = do_echo,
        },
        {
            .name = "quit",
            .useage = "quit --quit from shell",
            .do_func = do_exit,
        },
};

static void cli_init(const char *promot, const cli_cmd_t *cmd_list, int cnt)
{
    cli.promot = promot;
    memset(cli.curr_input, 0, CLI_INPUT_SIZE);
    cli.cmd_start = cmd_list;
    cli.cmd_end = cmd_list + cnt;
}

static const cli_cmd_t *find_builtin(const char *name)
{
    for (const cli_cmd_t *cmd = cli.cmd_start; cmd < cli.cmd_end; cmd++)
    {
        if (strcmp(cmd->name, name) != 0)
        {
            continue;
        }
        return cmd;
    }
    return (const cli_cmd_t *)0;
}

static void *run_builtin(const cli_cmd_t *cmd, int argc, char **argv)
{
    int ret = cmd->do_func(argc, argv);
    if (ret < 0)
    {
        fprintf(stderr, ESC_COLOR_ERROR "Error: %d\n" ESC_COLOR_DEFAULT, ret);
    }
}
/**
 * 试图运行当前文件
 */
static void run_exec_file (const char * path, int argc, char ** argv) {
    int pid = fork();
    if (pid < 0){
        fprintf(stderr, "fork failed: %s", path);
    } else if (pid == 0) {
        // 以下供测试exit使用
        for (int i = 0; i < argc; i++) {
            msleep(1000);
            printf("arg %d = %s\n", i, argv[i]);
        }
        exit(-1);

        // 子进程
        // int err = execve(path, argv, (char * const *)0);
        // if (err < 0) {
        //     fprintf(stderr, "exec failed: %s", path);
        // }
        // exit(-1);
    } else {
		// 等待子进程执行完毕
        int status;
        int pid = wait(&status);
        fprintf(stderr, "cmd %s result: %d, pid = %d\n", path, status, pid);
    }
}
int main(int argc, char **argv)
{
    open(argv[0], 0); // 打开TTY设备
    dup(0);           // 标准输出
    dup(0);           // 标准错误输出

    printf("Hello from LukeyOS\n");
    printf("os version: %s\n", OS_VERSION);
    printf("This the %sth console\n", argv[0]);
    puts("author: alvin");
    puts("create data: 2023-5-31");

    cli_init(promot, cmd_list, sizeof(cmd_list) / sizeof(cmd_list[0]));
    for (;;)
    {
        show_promot();
        char *str = fgets(cli.curr_input, CLI_INPUT_SIZE, stdin);
        if (!str)
        {
            continue;
        }
        // 去除 curr_input 中的 \n \r“”
        char *cr = strchr(cli.curr_input, '\n');
        if (cr)
        {
            *cr = '\0';
        }
        cr = strchr(cli.curr_input, '\r');
        if (cr)
        {
            *cr = '\0';
        }
        // 将字符串按空格转换为多个字符串
        // 如： echo -n 100 message = 'echo' ,'-n' ,'100' ,'message'
        int argc = 0;
        char *argv[CLI_MAX_ARG_COUNT];
        memset(argv, 0, sizeof(argv));
        const char *space = " ";
        char *token = strtok(cli.curr_input, space);
        while (token)
        {
            argv[argc++] = token;
            token = strtok(NULL, space);
        }
        if (argc == 0)
        {
            continue;
        }

        // 解析命令
        // 内置命令（调用内核）
        const cli_cmd_t *cmd = find_builtin(argv[0]);
        if (cmd)
        {
            run_builtin(cmd, argc, argv);
            continue;
        }
        // 磁盘上的应用
        run_exec_file("", argc, argv);

        // 出现错误则打印
        fprintf(stderr, ESC_COLOR_ERROR "Unknow command: %s\n" ESC_COLOR_DEFAULT, cli.curr_input);
        
     
    }
}