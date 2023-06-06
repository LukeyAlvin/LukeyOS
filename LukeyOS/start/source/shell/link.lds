/* 入口地址在  _start */
ENTRY(_start)
SECTIONS
{
    /* 为了方便调试，没有从0x80000000开始，以免与first_task冲突 */
	. = 0x81000000;
	.text : {
		*(*.text)
	}

	.rodata : {
		*(*.rodata)
	}

	.data : {
		*(*.data)
	}

	.bss : {
		/* BSS的起始地址 */
		__bss_start__ = .;
		*(*.bss)
		/* BSS的结束地址 */
    	__bss_end__ = . ;
	}
}
