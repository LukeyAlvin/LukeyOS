/*
 * @Description: ELF相关数据类型
 * @Author: Alvin
 * @Date: 2023-04-15 15:34:03
 * @LastEditTime: 2023-05-25 11:23:25
 */
#ifndef ELF_H_
#define ELF_H_

// ELF相关数据类型
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

#define EI_NIDENT 16
#define ELF_MAGIC 0x7F

#define ET_EXEC         2   // 可执行文件
#define ET_386          3   // 80386处理器

#define PT_LOAD         1


#pragma pack(1)

// 用于32位ELF文件的文件头结构体
typedef struct {
    unsigned char   e_ident[EI_NIDENT]; /* ELF文件标识 */
    Elf32_Half      e_type;             /* 文件类型 */
    Elf32_Half      e_machine;          /* 目标体系结构 */
    Elf32_Word      e_version;          /* ELF文件版本 */
    Elf32_Addr      e_entry;            /* 入口点地址 */
    Elf32_Off       e_phoff;            /* 程序头表在文件中的偏移量 */
    Elf32_Off       e_shoff;            /* 节表在文件中的偏移量 */
    Elf32_Word      e_flags;            /* 处理器特定标志 */
    Elf32_Half      e_ehsize;           /* ELF文件头的大小 */
    Elf32_Half      e_phentsize;        /* 程序头表项大小 */
    Elf32_Half      e_phnum;            /* 程序头表项数量 */
    Elf32_Half      e_shentsize;        /* 节表项大小 */
    Elf32_Half      e_shnum;            /* 节表项数量 */
    Elf32_Half      e_shstrndx;         /* 节名称字符串表在节表中的索引 */
} Elf32_Ehdr;

// 用于32位ELF文件的程序头表结构体
typedef struct {
    Elf32_Word  p_type;         /* 程序段类型 */
    Elf32_Off   p_offset;       /* 该段在文件中的偏移量 */
    Elf32_Addr  p_vaddr;        /* 该段在内存中的虚拟地址 */
    Elf32_Addr  p_paddr;        /* 该段在内存中的物理地址 */
    Elf32_Word  p_filesz;       /* 该段在文件中的大小 */
    Elf32_Word  p_memsz;        /* 该段在内存中的大小 */
    Elf32_Word  p_flags;        /* 该段的访问权限 */
    Elf32_Word  p_align;        /* 该段的对齐方式 */
} Elf32_Phdr;

#pragma pack()
#endif