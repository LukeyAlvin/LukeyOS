/**
 * 一些字符串的处理函数
 */
#include "tools/klib.h"
#include "tools/log.h"
#include "common/cpu_instr.h"

/* 将 src 字符串复制到 dest 字符串中 */
void kernel_strcpy(char *dest, const char *src)
{
    if (!dest || !src)
    {
        return;
    }
    while (*dest && *src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';
}
/* 将 src 字符串中的 size 个字符复制到 dest 字符串中 */
void kernel_strncpy(char *dest, const char *src, int size)
{
    if (!dest || !src || !size)
    {
        return;
    }

    char *d = dest;
    const char *s = src;

    while ((size-- > 0) && (*s))
    {
        *d++ = *s++;
    }
    if (size == 0)
    {
        *(d - 1) = '\0';
    }
    else
    {
        *d = '\0';
    }
}
/* 计算字符串 str 的长度（不包括 null 字符） */
int kernel_strlen(const char *str)
{
    if (str == (const char *)0)
    {
        return 0;
    }

    const char *c = str;

    int len = 0;
    while (*c++)
    {
        len++;
    }

    return len;
}

/* 比较两个字符串 s1 和 s2 的前 size 个字符 , 如果某一字符串提前比较完成，也算相同*/
int kernel_strncmp(const char *s1, const char *s2, int size)
{
    if (!s1 || !s2)
    {
        return -1;
    }

    // 2023-3-18 这里size没有用到
    while (*s1 && *s2 && (*s1 == *s2) && size)
    {
        s1++;
        s2++;
        size--;
    }

    return !((*s1 == '\0') || (*s2 == '\0') || (*s1 == *s2));
}
/* 将 src 指向的内存块中的 size 个字节复制到 dest 指向的内存块中 */
void kernel_memcpy(void *dest, void *src, int size)
{
    if (!dest || !src || !size)
    {
        return;
    }

    uint8_t *s = (uint8_t *)src;
    uint8_t *d = (uint8_t *)dest;
    while (size--)
    {
        *d++ = *s++;
    }
}
/* 将 dest 指向的内存块中的前 size 个字节设置为值 v */
void kernel_memset(void *dest, uint8_t v, int size)
{
    if (!dest || !size)
    {
        return;
    }

    uint8_t *d = (uint8_t *)dest;
    while (size--)
    {
        *d++ = v;
    }
}
/* 比较 d1 和 d2 指向的内存块中的前 size 个字节 */
int kernel_memcmp(void *d1, void *d2, int size)
{
    if (!d1 || !d2)
    {
        return 1;
    }

    uint8_t *p_d1 = (uint8_t *)d1;
    uint8_t *p_d2 = (uint8_t *)d2;
    while (size--)
    {
        if (*p_d1++ != *p_d2++)
        {
            return 1;
        }
    }

    return 0;
}

/**
 * 格式化字符串
 * "Version:%s", "1.0.10"
 * 将可变参数列表 vl 中的内容根据 fmt 指定的格式进行格式化，并将结果存储在 buffer 中
 */
void kernel_vsprintf(char *buffer, const char *fmt, va_list vl)
{
    enum
    {
        NORMAL,
        READ_FMT
    } state = NORMAL;

    char *curr = buffer;
    char ch;

    /**首先，使用后缀自增运算符 ++ 将 fmt 指针向后移动一位，指向下一个字符。
     * 然后，将 fmt 指针所指向的字符赋值给变量 ch。由于 *fmt++ 的优先级比 = 高，因此这个表达式先执行了 *fmt，然后再执行 fmt++。
     * 如果 ch 的值为 0，也就是遇到了字符串的结束符，那么整个循环就会终止。否则，循环会继续执行，读取下一个字符。
     */
    while ((ch = *fmt++))
    {
        if (state == NORMAL)
        {
            if (ch == '%')
            {
                state = READ_FMT;
            }
            else
            {
                *curr++ = ch;
            }
        }
        else
        {
            if (ch == 's')
            {
                const char *str = va_arg(vl, char *);
                int len = kernel_strlen(str);
                while (len--)
                {
                    *curr++ = *str++;
                }
            }
            else if (ch == 'd')
            {
                int num = va_arg(vl, int);
                kernel_itoa(curr, num, 10);
                curr += kernel_strlen(curr);
            }
            else if (ch == 'x')
            {
                int num = va_arg(vl, int);
                kernel_itoa(curr, num, 16);
                curr += kernel_strlen(curr);
            }
            else if (ch == 'c')
            {
                char c = va_arg(vl, int);
                *curr++ = c;
            }
            state = NORMAL;
        }
    }
}
/* 将int 转换为 char*（ASCII） 其中 base表示进制数 */
void kernel_itoa(char * buf, int num, int base) {
    // 转换字符索引[-15, -14, ...-1, 0, 1, ...., 14, 15]
    static const char * num2ch = {"FEDCBA9876543210123456789ABCDEF"};
    char * p = buf;
    int old_num = num;

    // 仅支持部分进制
    if ((base != 2) && (base != 8) && (base != 10) && (base != 16)) {
        *p = '\0';
        return;
    }

    // 只支持十进制负数
    int signed_num = 0;
    if ((num < 0) && (base == 10)) {
        *p++ = '-';
        signed_num = 1;
    }

    if (signed_num) {
        do {
            char ch = num2ch[num % base + 15];
            *p++ = ch;
            num /= base;
        } while (num);
    } else {
        uint32_t u_num = (uint32_t)num;
        do {
            char ch = num2ch[u_num % base + 15];
            *p++ = ch;
            u_num /= base;
        } while (u_num);
    }
    *p-- = '\0';

    // 将转换结果逆序，生成最终的结果
    char * start = (!signed_num) ? buf : buf + 1;
    while (start < p) {
        char ch = *start;
        *start = *p;
        *p-- = ch;
        start++;
    }
}

void kernel_sprintf(char *buffer, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    
    kernel_vsprintf(buffer, fmt, vl);
    va_end(vl);
}
/**
 * @brief 计算字符串的数量
 */
int strings_count (char ** start) {
    int count = 0;

    if (start) {
        while (*start++) {
            count++;
        }
    }
    return count;
}


/**
 * @brief 从路径中解释文件名
 */
char * get_file_name (char * name) {
    char * s = name;

    // 定位到结束符
    while (*s != '\0') {
        s++;
    }

    // 反向搜索，直到找到反斜杆或者到文件开头
    while ((*s != '\\') && (*s != '/') && (s >= name)) {
        s--;
    }
    return s + 1;
}

void panic (const char * file, int line, const char * func, const char * cond) {
    log_printf("assert failed! %s", cond);
    log_printf("file: %s\nline %d\nfunc: %s\n", file, line, func);

    for (;;) {
        hlt();
    }
}
