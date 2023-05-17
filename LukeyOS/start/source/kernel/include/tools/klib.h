/*
 * @Description: 工具函数
 * @Author: Alvin
 * @Date: 2023-04-18 15:14:56
 * @LastEditTime: 2023-04-22 21:55:37
 */

#ifndef KLIB_H_
#define KLIB_H_

#include "common/types.h"
#include "stdarg.h"


/* 将 src 字符串复制到 dest 字符串中 */
void kernel_strcpy(char *dest, const char *src);

/* 将 src 字符串中的 size 个字符复制到 dest 字符串中 */
void kernel_strncpy(char *dest, const char *src, int size);

/* 比较两个字符串 s1 和 s2 的前 size 个字符 */
int kernel_strncmp(const char *s1, const char *s2, int size);

/* 计算字符串 str 的长度（不包括 null 字符） */
int kernel_strlen(const char *str);

/* 将 src 指向的内存块中的 size 个字节复制到 dest 指向的内存块中 */
void kernel_memcpy(void *dest, void *src, int size);

/* 将 dest 指向的内存块中的前 size 个字节设置为值 v */
void kernel_memset(void *dest, uint8_t v, int size);

/* 比较 d1 和 d2 指向的内存块中的前 size 个字节 */
int kernel_memcmp(void *d1, void *d2, int size);

/* 格式化字符串 */
void kernel_vsprintf(char * buffer, const char * fmt, va_list args);

/* 将int 转换为 char*（ASCII） 其中 base表示进制数 */
void kernel_itoa(char * buffer, int num, int base);

void kernel_sprintf(char * buffer, const char * fmt, ...);

/**
 * 例如，如果 size 是 1000，bound 是 256，那么对 size 进行向上对齐后，
 * 它的值将变为 1280，即最接近 1000 且是 256 的倍数的数。
*/
// 向上对齐到页边界
static inline uint32_t up2 (uint32_t size, uint32_t bound) {
    return (size + bound - 1) & ~(bound - 1);
}
/**
 * 例如，如果 size 是 1000，bound 是 256，那么对 size 进行向下对齐后，
 * 它的值将变为 768，即最接近 1000 且是 256 的倍数的数。
*/
// 向下对齐到页边界
static inline uint32_t down2 (uint32_t size, uint32_t bound) {
    return size & ~(bound - 1);
}
// ASSERT 函数通常用于进行断言检查，如果断言失败，就会终止程序的执行。
#ifndef RELEASE
#define ASSERT(condition)    \
    if (!(condition)) panic(__FILE__, __LINE__, __func__, #condition)
void panic (const char * file, int line, const char * func, const char * cond);
#else
#define ASSERT(condition)    ((void)0)
#endif

#endif // KLIB_H
