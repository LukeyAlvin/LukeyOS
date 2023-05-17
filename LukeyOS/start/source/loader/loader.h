/*
 * @Description: 二级加载部分，用于实现更为复杂的初始化、内核加载的工作。
 * @Author: Alvin
 * @Date: 2023-04-13 15:29:39
 * @LastEditTime: 2023-05-17 15:51:31
 */

#ifndef LOADER_H_
#define LOADER_H_

#include "common/types.h"
#include "common/boot_info.h"
#include "common/cpu_instr.h"
#include "common/elf.h"

// 保护模式入口函数，在start.asm中定义
void protect_mode_entry (void);

// 内存检测信息结构体
typedef struct SMAP_entry
{
    uint32_t BaseL;   // 内存区域的起始地址低32位
    uint32_t BaseH;   // 内存区域的起始地址高32位
    uint32_t LengthL; // 内存区域的长度低32位
    uint32_t LengthH; // 内存区域的长度高32位
    uint32_t Type;    // 内存区域的类型 Type = 1 可用 Type = 2 不可用
    uint32_t ACPI;    // 扩展信息
} __attribute__((packed)) SMAP_entry_t;
// 由于不同的编译器对结构体的对齐方式不同，这里使用了__attribute__((packed))来指示编译器按照紧凑型的方式对结构体进行排列，避免浪费空间

extern boot_info_t boot_info;
#endif