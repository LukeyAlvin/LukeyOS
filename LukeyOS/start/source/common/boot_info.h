/*
 * @Description: 启动信息参数设置
 * @Author: Alvin
 * @Date: 2023-04-13 17:10:43
 * @LastEditTime: 2023-04-24 17:14:12
 */
#ifndef BOOT_INFO_H_
#define BOOT_INFO_H_

#include "types.h"

#define BOOT_RAM_REGION_MAX 10             // 限制系统中RAM区域的最大数量
#define SECTOR_SIZE 512                    // 磁盘每个扇区的大小
#define SYS_KERNEL_LOAD_ADDR (1024 * 1024) // 把内核代码加载到从1M开始的一段连续的内存地址中
/**
 * 启动信息参数
 */
// 我们可用的内存区域是一块一块的
// 定义这个结构体的目的是描述每一块可用内存的信息
typedef struct _boot_info_t
{   
    // ram_region_cfg 用于存储可用RAM区域的配置信息，限制大小为10
    struct
    {
        uint32_t start; // RAM区域的起始地址
        uint32_t size;  // RAM区域的大小
    } ram_region_cfg[BOOT_RAM_REGION_MAX];

    int ram_region_count; // 表示可用RAM区域的数量
} boot_info_t;
#endif