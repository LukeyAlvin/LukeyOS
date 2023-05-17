/**
 * 自己动手写操作系统
 *
 * 二级引导，负责进行硬件检测，进入保护模式，然后加载内核，并跳转至内核运行
 *
 */
	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern loader_entry

_start:
	// 栈和段等沿用之前的设置,也可以重新设置
	// 这里简单起见，就不做任何设置了
	// 你可能会想，直接跳到loader_entry，但这样需要先知道loader_entry在哪儿
	// boot没有这个能力做到，所以直接用汇编，以便_start处于整个bin文件开头，这样boot直接跳到开头就可以
	jmp loader_entry


	// 16位代码，用于保护模式下运行
	.code32
	.text
	.global protect_mode_entry	// 设置为全局，可以在其他文件中调用
	.extern loader_kernel
protect_mode_entry:
	// 重置数据段寄存器
	mov $16, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	jmp $8, $loader_kernel
