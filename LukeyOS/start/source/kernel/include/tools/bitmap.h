/*
 * @Description: 位图数据结构
 * @Author: Alvin
 * @Date: 2023-04-22 11:26:28
 * @LastEditTime: 2023-04-22 20:07:43
 */
#ifndef BITMAP_H
#define BITMAP_H

#include "common/types.h"

/**
 * @brief 位图数据结构，实质上是一个数组
 */
typedef struct _bitmap_g {
    uint8_t * bits;             // 位图的每一项的bit数，例如每一项是8bit则表示每一项可以管理8个物理页
    int bit_count;              // 位图总的位数
}bitmap_t;

void bitmap_init (bitmap_t * bitmap, uint8_t * bits, int count, int init_bit);
int bitmap_byte_count (int bit_count);
/**
 * @brief 获取指定位的状态
 */
int bitmap_get_bit (bitmap_t * bitmap, int index);
/**
 * @brief 连续设置N个位
 */
void bitmap_set_bit (bitmap_t * bitmap, int index, int count, int bit);
/**
 * @brief 检查指定位是否置1
 */
int bitmap_is_set (bitmap_t * bitmap, int index);
/**
 * @brief 连续分配若干指定比特位，返回起始索引
 */
int bitmap_alloc_nbits (bitmap_t * bitmap, int bit, int count);
#endif // BITMAP_H