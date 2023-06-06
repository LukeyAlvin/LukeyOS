/*
 * @Description: 文件系统相关接口的实现
 * @Author: Alvin
 * @Date: 2023-05-25 10:14:40
 * @LastEditTime: 2023-06-05 15:09:38
 */
#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>


int sys_open(const char *name, int flags, ...);
int sys_read(int file, char *ptr, int len);
int sys_write(int file, char *ptr, int len);
int sys_lseek(int file, int ptr, int dir);
int sys_close(int file);


int sys_isatty(int file);
int sys_fstat(int file, struct stat *st);
int sys_dup(int file);

void fs_init(void);


#endif // FILE_H