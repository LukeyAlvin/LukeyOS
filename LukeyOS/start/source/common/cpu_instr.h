/**
 * 汇编指令的封装
 */
#ifndef CPU_INSTR_H_
#define CPU_INSTR_H_
#include "types.h"
// 关中断
static inline void cli(void)
{
    __asm__ __volatile__("cli");
}
// 开中断
static inline void sti(void)
{
    __asm__ __volatile__("sti");
}
// 读端口:输入指定端口号的函数，返回读取到的一个字节(byte)的值
static inline uint8_t inb(uint16_t port)
{
    uint8_t res;
    // int al, dx
    __asm__ __volatile__("inb %[p], %[v]"
                         : [v] "=a"(res)
                         : [p] "d"(port));
    return res;
}

// 读端口:输入指定端口号的函数，返回读取到的一个字(word)的值
static inline uint16_t inw(uint16_t port)
{
    uint16_t res;
    // int ax, dx
    __asm__ __volatile__("in %[p], %[v]"
                         : [v] "=a"(res)
                         : [p] "d"(port));
    return res;
}

// 写端口:输入指定端口号并写入一个字节的数据
static inline void outb(uint16_t port, uint8_t data)
{
    // outb al, dx
    __asm__ __volatile__("outb %[v], %[p]" ::[p] "d"(port), [v] "a"(data));
}
/**
 * 加载 lgdt 表的指令
 * start: 加载数据的起始地址
 * size : 加载数据的大小
 */
static inline void lgdt(uint32_t start, uint32_t size)
{
    struct
    {
        uint16_t limit;      // 段的长度
        uint16_t start15_0;  // 起始地址的底16位
        uint16_t start31_16; // 起始地址的高16位
    } gdt;
    // 初始化
    gdt.start31_16 = start >> 16;
    gdt.start15_0 = start & 0xFFFF;
    gdt.limit = size - 1;

    __asm__ __volatile__("lgdt %[g]" ::[g] "m"(gdt));
}
/**
 * 加载 lidt 表的指令
 * start: 加载数据的起始地址
 * size : 加载数据的大小
 */
static inline void lidt(uint32_t start, uint32_t size) {
	struct {
		uint16_t limit;
		uint16_t start15_0;
		uint16_t start31_16;
	} idt;

	idt.start31_16 = start >> 16;
	idt.start15_0 = start & 0xFFFF;
	idt.limit = size - 1;

	__asm__ __volatile__("lidt %0"::"m"(idt));
}
// 读取 CR0 指令,CR0 PE位控制保护模式使能位，PG位控制分页机制使能位
static inline uint32_t read_cr0()
{
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %[v]"
                         : [v] "=r"(cr0));
    return cr0;
}
// 写入 CR0 指令,CR0 PE位控制保护模式使能位，PG位控制分页机制使能位
static inline void write_cr0(uint32_t v)
{
    __asm__ __volatile__("mov %[v], %%cr0" ::[v] "r"(v));
}
// 读取 CR3 指令,CR3 在项目中用于存储页目录表的物理地址。
static inline uint32_t read_cr2() {
	uint32_t cr2;
	__asm__ __volatile__("mov %%cr2, %[v]":[v]"=r"(cr2));
	return cr2;
}

// 写入 CR3 指令,CR3 在项目中用于存储页目录表的物理地址。
static inline void write_cr3(uint32_t v) {
    __asm__ __volatile__("mov %[v], %%cr3"::[v]"r"(v));
}

// 读取 CR3 指令,CR3 在项目中用于存储页目录表的物理地址。
static inline uint32_t read_cr3() {
    uint32_t cr3;
    __asm__ __volatile__("mov %%cr3, %[v]":[v]"=r"(cr3));
    return cr3;
}
// 读取 CR4 指令,CR4 在项目中用于设置PSE，以便启用4M的页，而不是4KB
static inline uint32_t read_cr4() {
    uint32_t cr4;
    __asm__ __volatile__("mov %%cr4, %[v]":[v]"=r"(cr4));
    return cr4;
}
// 写入 CR4 指令,CR4 在项目中用于设置PSE，以便启用4M的页，而不是4KB
static inline void write_cr4(uint32_t v) {
    __asm__ __volatile__("mov %[v], %%cr4"::[v]"r"(v));
}

static inline void far_jump(uint32_t selector, uint32_t offset)
{
    uint32_t addr[] = {offset, selector};
    __asm__ __volatile__("ljmpl *(%[a])" ::[a] "r"(addr));
}

static inline void hlt(void) {
    __asm__ __volatile__("hlt");
}

/**
 * @description: 对TR寄存器进行写操作
 * @param {uint32_t} tss_selector tss选择子类型
 */
static inline void write_tr (uint32_t tss_selector) {
    __asm__ __volatile__("ltr %%ax"::"a"(tss_selector));
}
/**
 * @description: 读 eflags 寄存器
 * @return {*}
 */
static inline uint32_t read_eflags (void) {
    uint32_t eflags;

    __asm__ __volatile__("pushfl\n\tpopl %%eax":"=a"(eflags));
    return eflags;
}
/**
 * @description: 写 eflags 寄存器
 * @return {*}
 * @param {uint32_t} eflags
 */
static inline void write_eflags (uint32_t eflags) {
    __asm__ __volatile__("pushl %%eax\n\tpopfl"::"a"(eflags));
}

#endif