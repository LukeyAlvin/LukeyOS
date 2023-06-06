/*
 * @Description: 终端显示部件 只支持VGA模式
 * @Author: Alvin
 * @Date: 2023-05-27 14:08:02
 * @LastEditTime: 2023-06-06 15:27:21
 */
#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_DISP_ADDR 0xb8000
#define CONSOLE_DISP_END (0xb8000 + 32 * 1024) // 显存的结束地址
#define CONSOLE_ROW_MAX 25                     // 行数
#define CONSOLE_COL_MAX 80                     // 最大列数

#define ASCII_ESC 0x1b   // ESC ascii码
#define ESC_PARAM_MAX 10 // 最多支持的ESC [ 参数数量

#include "common/types.h"
#include "dev/tty.h"
#include "ipc/mutex.h"

// 各种颜色
typedef enum _cclor_t
{
    COLOR_Black = 0,
    COLOR_Blue = 1,
    COLOR_Green = 2,
    COLOR_Cyan = 3,
    COLOR_Red = 4,
    COLOR_Magenta = 5,
    COLOR_Brown = 6,
    COLOR_Gray = 7,
    COLOR_Dark_Gray = 8,
    COLOR_Light_Blue = 9,
    COLOR_Light_Green = 10,
    COLOR_Light_Cyan = 11,
    COLOR_Light_Red = 12,
    COLOR_Light_Magenta = 13,
    COLOR_Yellow = 14,
    COLOR_White = 15
} cclor_t;

typedef union _disp_char_t
{
    struct
    {
        char ascii;
        char foreground : 4;
        char background : 3;
    };

    uint16_t v; // 一个字节显示ASIC码 一个字节显示属性
} disp_char_t;

typedef struct _console_t
{
    disp_char_t *disp_base; // 控制台基地址

    enum
    {
        CONSOLE_WRITE_NORMAL, // 普通模式
        CONSOLE_WRITE_ESC,    // ESC转义序列
        CONSOLE_WRITE_SQUARE, // ESC [接收状态
    } write_state;

    int old_cursor_col, old_cursor_row; // 保存的光标位置，ESC转义序列中使用

    int cursor_row;                 // 光标所在的行数
    int cursor_col;                 // 光标所在的列数
    int display_rows;               // 控制台的行数
    int display_cols;               // 控制台的列数
    cclor_t foreground, background; // 前后景色

    int esc_param[ESC_PARAM_MAX]; // ESC [ ;;参数数量
    int curr_param_index;

    mutex_t mutex; // 写互斥锁

} console_t;

// 初始化控制台
int console_init(int idx);
/**
 * @: 用于向指定的控制台输出数据
 * @param {int} dev 要输出数据的控制台对象的索引
 * @param {char} *data  要输出的数据的缓冲区地址
 * @param {int} size    要输出的数据的大小
 * @return {*}  输出的数据的长度
 */
int console_write(tty_t *tty);
void console_close(int dev);
void console_select(int idx);
#endif /* SRC_UI_TTY_WIDGET_H_ */
