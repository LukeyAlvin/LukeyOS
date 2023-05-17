/*
 * @Description: 日志输出
 * @Author: Alvin
 * @Date: 2023-04-18 14:38:49
 * @LastEditTime: 2023-04-22 09:54:43
 */
#ifndef LOG_H
#define LOG_H

// 日志初始化（设计硬件相关）
void log_init (void);
// 自定义printf函数 fmt 为格式化指针 ...为可变参数
void log_printf(const char * fmt, ...);

#endif // LOG_H