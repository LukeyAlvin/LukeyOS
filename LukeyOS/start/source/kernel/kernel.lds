/* 参考文档： https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html */
SECTIONS
{
	/* ============================1===从64KB 之前的区域设置为可读写=================================== */
	PROVIDE(kernel_base = 0x0);
	/* 定义内核的起始地址*/
	. = 0x00010000;

	/* ===============================从64KB 开始的只读区域.text和.rodata=================================== */
	PROVIDE(s_text = .);	
	.text : {
		/* EXCLUDE_FILE 依次放置内核的代码和只读数据，但不包括first_task进程的这些东西 */
		*(EXCLUDE_FILE(*first_task* *lib_syscall*) .text)	
	}
	.rodata : {
		*(EXCLUDE_FILE(*first_task* *lib_syscall*) .rodata)
	}
	PROVIDE(e_text = .);
	/* ===============================从64KB 开始的只读区域.text和.rodata=================================== */


 	/* ===============================.data和 .bss 位图 等设置为可读写=================================== */
	. = ALIGN(4096);
	PROVIDE(s_data = .);
	.data : {
		*(EXCLUDE_FILE(*first_task* *lib_syscall*) .data)
	}
	.bss : {
		*(EXCLUDE_FILE(*first_task* *lib_syscall*) .bss)
	}
	/*  内核数据结束地址 */
	e_data = .;
	/* 初始进程的配置：接紧着在低端1MB内存开始存储，但是运行时搬运到0x80000000处 */
	. = 0x80000000;

	/* 获取first_task开始地址 */
	PROVIDE(s_first_task = LOADADDR(.first_task));
    /* 下面放置first_task的text data rodata bss，起始运行地址是0x80000000 */
    /* 由于使用了AT(e_data)，指定了加载地址是放在kernel的bss后边 */
	.first_task : AT(e_data) {
		*first_task_entry*(.text .data. rodata .data)
		*first_task*(.text .data. rodata .data)
		*lib_syscall*(.text .data .bss)
	}
	/* 获取first_task结束地址 */
	e_first_task = LOADADDR(.first_task) + SIZEOF(.first_task);

	/* 1MB内存空间起始，在内核数据后面放物理页位图 */
	PROVIDE(mem_free_start = e_first_task);
	/* ===============================.data和 .bss位图 等设置为可读写=================================== */
}
