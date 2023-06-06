/**
 * 自己动手写操作系统
 *
 * 二级引导，负责进行硬件检测，进入保护模式，然后加载内核，并跳转至内核运行
 *
 * 作者：李述铜
 * 联系邮箱: 527676163@qq.com
 */
#include "os_cfg.h"
    // 不必加.code32因默认就是32位
 	.text
 	.global _start
	.extern kernel_init
	
	# void start(boot_info_f *boot_info)
_start:
	// 取出 boot_info 参数
	push %ebp
	mov %esp, %ebp
	mov 0x8(%ebp), %eax
	# 等价于 mov 4(%exp), %eax

	// 传给 kernel_init
	# kernel_init(boot_info)
	push %eax
	call kernel_init

    // 重新加载GDT
	jmp $KERNEL_SELECTOR_CS, $gdt_reload

gdt_reload:
	mov $KERNEL_SELECTOR_DS, %ax		// 16为数据段选择子
	mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

	// 栈设置
	mov $(stack + KERNEL_STACK_SIZE), %esp

	// 栈和段等沿用之前的设置
	jmp init_main

	.bss
.comm stack, KERNEL_STACK_SIZE    // comm 声明未初始化的通用内存区域，以字节计

// 中断发生时，会自动切换到特权级0对应的栈中去执行
// 并且只保存ss,esp,cs,eip,flags寄存器
// 所以需要在中断中自行保存其它寄存器
   .text
.macro exception_handler name num with_error_code
	    .extern do_handler_\name
		.global exception_handler_\name
	exception_handler_\name:
		// 如果没有错误码，压入一个缺省值
		// 这样堆栈就和有错误码的情形一样了
		.if \with_error_code == 0
			push $0
		.endif

		// 压入异常号
		push $\num

		// 保存所有寄存器
		pushal
		push %ds
		push %es
		push %fs
		push %gs

		// 调用中断处理函数
		push %esp
		call do_handler_\name
		add $(1*4), %esp		// 丢掉esp

		// 恢复保存的寄存器
		pop %gs
		pop %fs
		pop %es
		pop %ds
		popal

		// 跳过压入的异常号和错误码
		add $(2*4), %esp
		iret
.endm
// 未知异常
exception_handler unknown, -1, 0
// 除零异常
exception_handler divider, 0, 0
exception_handler Debug, 1, 0
exception_handler NMI, 2, 0
exception_handler breakpoint, 3, 0
exception_handler overflow, 4, 0
exception_handler bound_range, 5, 0
exception_handler invalid_opcode, 6, 0
exception_handler device_unavailable, 7, 0
exception_handler double_fault, 8, 1
exception_handler invalid_tss, 10, 1
exception_handler segment_not_present, 11, 1
exception_handler stack_segment_fault, 12, 1
exception_handler general_protection, 13, 1
exception_handler page_fault, 14, 1
exception_handler fpu_error, 16, 0
exception_handler alignment_check, 17, 1
exception_handler machine_check, 18, 0
exception_handler smd_exception, 19, 0
exception_handler virtual_exception, 20, 0
// 硬件中断
exception_handler timer, 0x20, 0
exception_handler kbd, 0x21, 0


// eax, ecx, edx由调用者自动保存
// ebx, esi, edi, ebp需要由被调用者保存和恢复
// cs/ds/es/fs/gs/ss不用保存，因为都是相同的
// esp不用保存，只需要让esp保存在调用之前的状态
// eflags ?
	.text
	.global simple_switch
simple_switch:
	movl 4(%esp), %eax   // 取from->stack
	movl 8(%esp), %edx   // 取to->stack

	// 保存前一任务的状态
	push %ebp
	push %ebx
	push %esi
	push %edi

	// 切换栈
	mov %esp, (%eax)    // from->stack = esp
  	mov %edx, %esp      // esp = to->stack

	// 加载下一任务的栈
	pop %edi
	pop %esi
	pop %ebx
	pop %ebp
  	ret

// 保存当前进程的状态，然后使用特权级0的内核段寄存器，
// 调用内核的系统调用处理函数，最后恢复进程的状态并返回。
    .global exception_handler_syscall
    .extern do_handler_syscall
exception_handler_syscall:
	// 保存前一任务的状态
	// 使用pusha指令保存通用寄存器eax、ecx、edx、ebx、esp、ebp、esi和edi的值
	pusha
	// 使用push指令将段寄存器ds、es、fs、gs和标志寄存器eflags的值压入栈中，以保护它们的状态
	push %ds
	push %es
	push %fs
	push %gs
	pushf

	// 将内核段寄存器ds、es、fs和gs的值设置为内核数据段的选择子，以确保在系统调用处理程序中使用内核数据段
	mov $(KERNEL_SELECTOR_DS), %eax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs

    // 调用处理函数
    mov %esp, %eax
    push %eax
	call do_handler_syscall		// 使用call指令调用系统调用处理函数do_handler_syscall，以处理系统调用请求
	add $4, %esp				// 将栈顶指针恢复到正确的位置

    // 再切换回来
	popf
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	// 5个参数，加上5*4，不加会导致返回时ss取不出来，最后返回出现问题
    retf $(5*4)    // CS发生了改变，需要使用远跳转