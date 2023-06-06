/*
 * @Description: CPU设置
 * @Author: Alvin
 * @Date: 2023-04-17 10:16:07
 * @LastEditTime: 2023-05-22 15:12:10
 */
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "common/cpu_instr.h"
#include "cpu/irq.h"
#include "ipc/mutex.h"
#include "core/syscall.h"

static segment_desc_t gdt_table[GDT_TABLE_SIZE];
// 定义互斥锁结构
static mutex_t mutex;

/**
 * 设置段描述符
 */
void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr)
{
    segment_desc_t *desc = gdt_table + selector / sizeof(segment_desc_t);

    // 如果界限比较长，将长度单位换成4KB
    if (limit > 0xfffff)
    {
        attr |= 0x8000;
        limit /= 0x1000;
    }
    desc->limit15_0 = limit & 0xffff;
    desc->base15_0 = base & 0xffff;
    desc->base23_16 = (base >> 16) & 0xff;
    desc->attr = attr | (((limit >> 16) & 0xf) << 8);
    desc->base31_24 = (base >> 24) & 0xff;
}

/**
 * 初始化GDT
 */
void init_gdt(void)
{
    // 全部清空
    for (int i = 0; i < GDT_TABLE_SIZE; i++)
    {
        segment_desc_set(i * sizeof(segment_desc_t), 0, 0, 0);
    }
    // 使用平摊模型（只用了两个段：代码段和数据段）
    // 数据段
    segment_desc_set(KERNEL_SELECTOR_DS, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);

    // 只能用非一致代码段，以便通过调用门更改当前任务的CPL执行关键的资源访问操作
    segment_desc_set(KERNEL_SELECTOR_CS, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);

    // 调用门
    gate_desc_set((gate_desc_t *)(gdt_table + (SELECTOR_SYSCALL >> 3)),
            KERNEL_SELECTOR_CS,
            (uint32_t)exception_handler_syscall,    // 函数入口地址
            GATE_P_PRESENT | GATE_DPL3 | GATE_TYPE_SYSCALL | SYSCALL_PARAM_COUNT);
    
    // 加载gdt
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}
/**
 * @description: 寻找GDT中的空闲段，分配一个GDT推荐表符
 * @return {*}
 */
int gdt_alloc_desc(void)
{
    mutex_lock(&mutex);
    // 跳过第0项
    for (int i = 1; i < GDT_TABLE_SIZE; i++)
    {
        segment_desc_t *desc = gdt_table + i;
        if (desc->attr == 0)
        {
            mutex_unlock(&mutex);
            return i * sizeof(segment_desc_t);
        }
    }
    mutex_unlock(&mutex);
    return -1;
}

// 释放描述符
void gdt_free_sel (int sel) {
    mutex_lock(&mutex);
    gdt_table[sel / sizeof(segment_desc_t)].attr = 0;
    mutex_unlock(&mutex);
}

/**
 * 切换至TSS，即跳转实现任务切换
 */
void switch_to_tss(uint32_t tss_selector)
{
    far_jump(tss_selector, 0);
}

/**
 * CPU初始化
 */
void cpu_init(void)
{
    // 互斥锁初始化
    mutex_init(&mutex);
    init_gdt();
}
