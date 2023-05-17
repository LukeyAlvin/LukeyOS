/*
 * @Description: 与x86的体系结构相关的接口及参数
 * @Author: Alvin
 * @Date: 2023-04-17 10:16:07
 * @LastEditTime: 2023-05-17 15:50:05
 */
#ifndef CPU_H
#define CPU_H

#include "common/types.h"


#define GDT_TABLE_SIZE 256 // GDT表项数量

#define SEG_G (1 << 15)        // 设置段界限的单位，1-4KB，0-字节
#define SEG_D (1 << 14)        // 控制是否是32位、16位的代码或数据段
#define SEG_P_PRESENT (1 << 7) // 段是否存在

#define SEG_DPL0 (0 << 5) // 特权级0，最高特权级
#define SEG_DPL3 (3 << 5) // 特权级3，最低权限

#define SEG_S_SYSTEM (0 << 4) // 是否是系统段，如调用门或者中断
#define SEG_S_NORMAL (1 << 4) // 普通的代码段或数据段

#define SEG_TYPE_CODE (1 << 3) // 指定其为代码段
#define SEG_TYPE_DATA (0 << 3) // 数据段
#define SEG_TYPE_TSS (9 << 0)  // TSS 段

#define SEG_TYPE_RW (1 << 1) // 是否可写可读，不设置为只读

#define SEG_RPL0                (0 << 0)
#define SEG_RPL3                (3 << 0)

// TSS 中的EFlags寄存器
#define EFLAGS_DEFAULT (1 << 1)
#define EFLAGS_IF (1 << 9)


#pragma pack(1)

/**
 * GDT描述符
 */
typedef struct _segment_desc_t
{
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;
} segment_desc_t;

/**
 * tss描述符
 */
typedef struct _tss_t
{
    uint32_t pre_link;                                            // 前一个TSS的描述符地址，用于链表操作
    uint32_t esp0, ss0, esp1, ss1, esp2, ss2;                     // 任务的堆栈信息(不同特权级)
    uint32_t cr3;                                                 // 任务的页表基地址
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi; // 通用寄存器和指令指针
    uint32_t es, cs, ss, ds, fs, gs;                              // 段寄存器
    uint32_t ldt;                                                 // 局部描述符表的段选择子
    uint32_t iomap;                                               // I/O位图地址
}tss_t;

#pragma pack()

void cpu_init(void);
void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr);
// 寻找GDT中的空闲段
int gdt_alloc_desc (void);
// 切换至指定任务（使用 far_jump 实现）
void switch_to_tss (uint32_t tss_selector);
// 释放描述符
void gdt_free_sel(int tss_sel);
#endif
