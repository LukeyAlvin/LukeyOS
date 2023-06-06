/*
 * @Description: 测试fork函数
 * @Author: Alvin
 * @Date: 2023-05-22 20:17:56
 * @LastEditTime: 2023-05-22 20:18:11
 */
#include <stdio.h>

int main (void) {
    int count = 3;

    printf("Hello\n");

    int pid = fork();
    if (pid < 0) {
        printf("create child proc failed.\n");
    } else if (pid == 0) {
        printf("child: %d\n", count);
    } else {
        printf("child task id=%d\n", pid);
        printf("parent: %d\n", count);
    }

    return 0;
} 
