/**
 * 中断处理
 */
#ifndef IRQ_H
#define IRQ_H

#include "cpu/cpu.h"
#include "common/cpu_instr.h"
#include "common/types.h"
#include "os_cfg.h"
#include "tools/log.h"

#define IDT_TABLE_NR 128 // IDT表项数量

#define GATE_TYPE_IDT (0xE << 8) // 中断32位门描述符
#define GATE_P_PRESENT (1 << 15) // 是否存在
#define GATE_DPL0 (0 << 13)		 // 特权级0，最高特权级
#define GATE_DPL3 (3 << 13)		 // 特权级3，最低权限
#pragma pack(1)
/*
 * 调用门描述符 IDT表 Interrupt Gate
 */
typedef struct _gate_desc_t
{
	uint16_t offset15_0;
	uint16_t selector;
	uint16_t attr;
	uint16_t offset31_16;
} gate_desc_t;
#pragma pack()

/**
 * 中断发生时相应的栈结构，暂时为无特权级发生的情况
 */
typedef struct _exception_frame_t
{
	// 结合压栈的过程，以及pusha指令的实际压入过程
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t num;
	uint32_t error_code;
	uint32_t eip, cs, eflags;
	int esp3, ss3;
} exception_frame_t;

// 中断号码
#define IRQ0_DE 0	// 除法错误，Divide Error,当除数为零或者在有符号数的情况下除法结果溢出时触发。
#define IRQ1_DB 1	// 调试异常，Debug Exceptions,当执行单步调试时触发。
#define IRQ2_NMI 2	// 非屏蔽中断，Non-maskable Interrupt,由硬件产生，用于一些紧急情况。
#define IRQ3_BP 3	// 断点异常，Breakpoint Exception,当执行int3指令时触发。
#define IRQ4_OF 4	// 溢出异常，Overflow Exception,当执行指令导致溢出时触发。
#define IRQ5_BR 5	// 边界异常，Bound Range Exceeded Exception,当执行检查数组边界指令时越界时触发。
#define IRQ6_UD 6	// 无效操作码异常，Invalid Opcode Exception,当执行无效指令时触发。
#define IRQ7_NM 7	// 设备不存在异常，Coprocessor Not Available Exception,当执行访问IO地址空间的指令时，访问的端口号不存在时触发。
#define IRQ8_DF 8	// 双重故障，Double Fault Exception,用于处理异常处理程序本身出现异常的情况。
#define IRQ10_TS 10 // 任务切换异常，Invalid TSS Exception,用于在多任务操作系统中进行任务切换时触发。
#define IRQ11_NP 11 // 段不存在异常，Segment Not Present Exception,当指向不存在的段时触发。
#define IRQ12_SS 12 // 堆栈段异常，Stack-Segment Fault Exception,当堆栈操作溢出时触发。
#define IRQ13_GP 13 // 一般保护异常， General Protection Exception,当违反保护模式下的某些规则时触发。
#define IRQ14_PF 14 // 页面错误，Page Fault Exception,当访问不存在或非法的页面时触发。
#define IRQ16_MF 16 // FPU浮点异常，Floating-Point Error,当FPU发生浮点异常时触发。
#define IRQ17_AC 17 // 对齐检查异常， Alignment Check Exception,当执行指令时发现内存访问地址不是对齐的情况下触发。
#define IRQ18_MC 18 // 机器检查异常， Machine-Check Exception,由硬件检测到硬件错误时触发。
#define IRQ19_XM 19 // SIMD浮点异常， Floating-Point Exception,当执行SSE指令时触发。
#define IRQ20_VE 20 // 虚拟化异常， Virtualization Exception,当执行虚拟化指令时出现错误时触发。

#define IRQ0_TIMER 0x20

// PIC控制器相关的寄存器及位配置
#define PIC0_ICW1 0x20
#define PIC0_ICW2 0x21
#define PIC0_ICW3 0x21
#define PIC0_ICW4 0x21
#define PIC0_OCW2 0x20
#define PIC0_IMR 0x21

#define PIC1_ICW1 0xa0
#define PIC1_ICW2 0xa1
#define PIC1_ICW3 0xa1
#define PIC1_ICW4 0xa1
#define PIC1_OCW2 0xa0
#define PIC1_IMR 0xa1

#define PIC_ICW1_ICW4 (1 << 0)	   // 1 - 需要初始化ICW4
#define PIC_ICW1_ALWAYS_1 (1 << 4) // 总为1的位
#define PIC_ICW4_8086 (1 << 0)	   // 8086工作模式`
#define PIC_OCW2_EOI (1 << 5)	   // 1 - 非特殊结束中断EOI命令

#define IRQ_PIC_START 0x20 // PIC中断起始号

#define ERR_PAGE_P (1 << 0)		// 表示页面错误（Page Fault）中的存在位错误（Present）标志，该标志表示页表项是否存在的状态码
#define ERR_PAGE_WR (1 << 1)	// 表示页面错误中的写入错误（Write）标志，该标志表示是否尝试在只读页面上进行写操作的状态码
#define ERR_PAGE_US (1 << 1)	// 表示页面错误中的用户态错误（User/Supervisor）标志，该标志表示是否尝试在非用户态（内核态）下访问用户态页面的状态码。

#define ERR_EXT (1 << 0)	// 表示外部中断错误的标志，该标志表示外部设备向CPU发送中断信号时可能出现的错误。
#define ERR_IDT (1 << 1)	// 表示中断描述符表错误的标志，该标志表示中断处理程序所在的中断描述符表中可能出现的错误

// 定义在汇编里 source/kernel/init/start.S
void exception_handler_unknown(void);
void exception_handler_divider(void);
void exception_handler_Debug(void);
void exception_handler_NMI(void);
void exception_handler_breakpoint(void);
void exception_handler_overflow(void);
void exception_handler_bound_range(void);
void exception_handler_invalid_opcode(void);
void exception_handler_device_unavailable(void);
void exception_handler_double_fault(void);
void exception_handler_invalid_tss(void);
void exception_handler_segment_not_present(void);
void exception_handler_stack_segment_fault(void);
void exception_handler_general_protection(void);
void exception_handler_page_fault(void);
void exception_handler_fpu_error(void);
void exception_handler_alignment_check(void);
void exception_handler_machine_check(void);
void exception_handler_smd_exception(void);
void exception_handler_virtual_exception(void);

void gate_desc_set(gate_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr);
// 函数指针类型定义，这个函数指针类型指向的是一个无返回值、无参数的函数。
typedef void (*irq_handler_t)(void);
int irq_install(int irq_num, irq_handler_t handler);

void irq_enable(int irq_num);
void irq_disable(int irq_num);
void irq_disable_global(void);
void irq_enable_global(void);

void irq_init(void);
void pic_send_eoi(int irq_num);

typedef uint32_t irq_state_t;
// 进入临界区保护状态 返回值为中断的开启状态
irq_state_t irq_enter_protection();

/**
 * @description: 退出临界区保护状态
 * @return {*}
 * @param {irq_state_t} state 传入进入临界区之前的中断状态，退出临界区仍未该状态
 */
void irq_leave_protection(irq_state_t state);
#endif