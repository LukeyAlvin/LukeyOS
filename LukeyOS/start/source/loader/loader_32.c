/**
 * loader 的主要作用是：
 * 二级引导，负责进行硬件检测，进入保护模式，开启分页机制，建立页表，然后加载内核，并跳转至内核运行
 */
#include "loader.h"

/**
 *从磁盘上读取指定扇区的数据到内存缓冲区
 *@param sector 要读取的扇区号
 *@param sector_count 要读取的扇区数量
 *@param buffer 存放读取数据的缓冲区指针
 */
static void read_disk(uint32_t sector, uint32_t sector_count, uint8_t *buffer)
{
    // 0x1F6 是磁盘控制器的寄存器之一，该寄存器的高四位用于指定驱动器号
    // 其中 0 表示主硬盘，1 表示从硬盘。低四位用于指定磁头号（Head
    outb(0x1f6, 0xE0);

    // 设置要读/写的扇区数量，需要设置两个字节，因此需要分别指定高字节和低字节。
    // 在 LBA48 模式下，扇区数量最大值为 0xFFFF。

    outb(0x1F2, (uint8_t)(sector_count >> 8)); // 高字节
    outb(0x1F3, (uint8_t)(sector >> 24));      // LBA参数的24~31位
    outb(0x1F4, (uint8_t)(0));                 // LBA参数的32~39位
    outb(0x1F5, (uint8_t)(0));                 // LBA参数的40~47位

    outb(0x1F2, (uint8_t)(sector_count)); // 低字节
    outb(0x1F3, (uint8_t)(sector));       // LBA参数的0~7位
    outb(0x1F4, (uint8_t)(sector >> 8));  // LBA参数的8~15位
    outb(0x1F5, (uint8_t)(sector >> 16)); // LBA参数的16~23位

    // 向命令寄存器发送读取扇区的命令
    // READ SECTORS EXT 是读取扇区的命令，其命令码是 0x24。
    outb(0x1F7, 0x24);
    uint16_t *data_buf = (uint16_t *)buffer;
    while (sector_count--)
    {
        // 表示磁盘控制器已经准备好读取下一个扇区的数据，程序会通过 I/O 端口将这个扇区的数据读取到 buffer 中。
        // 如果状态的值不为 0x8，则说明磁盘控制器还没有准备好读取数据，程序会一直等待直到磁盘控制器准备好读取数据为止。
        while ((inb(0x1F7) & 0x88) != 0x8)
        {
        }
        for (int i = 0; i < SECTOR_SIZE / 2; i++) // 一个扇区所占用的字节数（512字节）除以2，因为每次读取的是2个字节（16位）。
        {
            *data_buf++ = inw(0x1F0); // 每次从数据寄存器（0x1F0的16位）
        }
    }
}

/**
 * 解析elf文件，提取内容到相应的内存中
 * https://wiki.osdev.org/ELF
 * @param file_buffer 指向ELF文件缓冲区的指针
 * @return 返回ELF文件的入口点地址，即程序的起始地址
 */
static uint32_t reload_elf_file(uint8_t *file_buffer)
{
    // 读取的只是ELF文件，不像BIN那样可直接运行，需要从中加载出有效数据和代码

    // 从传入的file_buffer指针中读取ELF头信息（Elf32_Ehdr）
    Elf32_Ehdr *elf_hdr = (Elf32_Ehdr *)file_buffer;

    // 检查ELF文件头是否合法
    // ELF文件头以四个字节的ELF_MAGIC（0x7F,'E','L','F'）作为标志
    if ((elf_hdr->e_ident[0] != ELF_MAGIC) || (elf_hdr->e_ident[1] != 'E') || (elf_hdr->e_ident[2] != 'L') || (elf_hdr->e_ident[3] != 'F'))
    {
        // 如果不合法，返回0表示加载失败
        return 0;
    }
    // 遍历ELF文件中的程序头（program header）
    // 程序头告诉操作系统如何加载可执行文件
    // 通过e_phoff和e_phnum两个字段来定位和计算程序头的位置和数量
    for (int i = 0; i < elf_hdr->e_phnum; i++)
    {
        // 获取第i个程序头的指针（Elf32_Phdr）
        // 程序头在ELF文件中的偏移量为e_phoff
        // 加上i*sizeof(Elf32_Phdr)就能得到第i个程序头的指针
        Elf32_Phdr *phdr = (Elf32_Phdr *)(file_buffer + elf_hdr->e_phoff) + i;

        // 如果该程序头的类型不是PT_LOAD，跳过此程序头
        // PT_LOAD类型表示需要将对应的文件段加载到内存中
        if (phdr->p_type != PT_LOAD)
        {
            continue;
        }

        // 从ELF文件中读取对应的内容，将其拷贝到相应的物理地址（p_paddr）中
        // 这里是直接进行了物理地址的拷贝，因为此时还未启用分页机制
        uint8_t *src = file_buffer + phdr->p_offset; // 源地址，ELF文件中的偏移量为p_offset
        uint8_t *dest = (uint8_t *)phdr->p_paddr;    // 目标地址，程序头中的p_paddr字段
        // 拷贝p_filesz个字节的数据
        for (int j = 0; j < phdr->p_filesz; j++)
        {
            *dest++ = *src++; // 将源地址中的数据拷贝到目标地址中
        }

        // 如果p_memsz > p_filesz，则需要填充0以达到
        // 这是因为 .bss 表示未初始化的变量，并不会分配内存
        dest = (uint8_t *)phdr->p_paddr + phdr->p_filesz;
        for (int j = 0; j < phdr->p_memsz - phdr->p_filesz; j++)
        {
            *dest++ = 0;
        }
    }

    return elf_hdr->e_entry;
}

static void die(int code)
{
    for (;;)
    {
    }
}

/**
 * @brief 开启分页机制
 * 将0-4M空间映射到0-4M和SYS_KERNEL_BASE_ADDR~+4MB空间
 * 0-4MB的映射主要用于保护loader自己还能正常工作
 * SYS_KERNEL_BASE_ADDR+4MB则用于为内核提供正确的虚拟地址空间
 */
void enable_page_mode(void)
{
#define PDE_P (1 << 0)   // 存在位；必须为1才能访问该页表
#define PDE_PS (1 << 7)  // 如果CR4.PSE = 1，则必须为0（否则，该条目映射4MB页；）；否则，忽略
#define PDE_W (1 << 1)   // 读写位；如果为0，则不允许对该条目控制的4MB区域进行写操作
#define CR4_PSE (1 << 4) // PSE标志，以便启用4M的页，而不是4KB。
#define CR0_PG (1 << 31) // PG位置为1，从而启用分页机制。

    // 使用4MB页块，这样构造页表就简单很多，只需要1个表即可。
    // 以下表为临时使用，用于帮助内核正常运行，在内核运行起来之后，将重新设置
    static uint32_t page_dir[1024] __attribute__((aligned(4096))) = {
        [0] = PDE_P | PDE_PS | PDE_W, // PDE_PS，开启4MB的页
    };

    // 设置CR4控制寄存器的PSE标志，以便启用4M的页，而不是4KB。
    uint32_t cr4 = read_cr4();
    write_cr4(cr4 | CR4_PSE);

    // 将页表的物理地址写入CR3控制寄存器。这个地址应该是page_dir数组在物理内存中的地址。
    write_cr3((uint32_t)page_dir);

    // 开启分页机制，将CR0控制寄存器中的PG位置为1，从而启用分页机制。
    write_cr0(read_cr0() | CR0_PG);
}
/**
 * 从磁盘上加载内核
 */
void loader_kernel()
{
    // 将内核代码从磁盘上加载到内存中
    // 将内核放在loader后，为预留空间，将内核放置于第100个扇区开始的地方
    // 内核大小预设为500个扇区大小（250KB）
    read_disk(100, 500, (uint8_t *)SYS_KERNEL_LOAD_ADDR);

    // 解析ELF文件，并通过调用的方式，进入到内核中去执行，同时传递boot参数
    // 临时将elf文件先读到SYS_KERNEL_LOAD_ADDR处，再进行解析
    uint32_t kernel_entry = reload_elf_file((uint8_t *)SYS_KERNEL_LOAD_ADDR);
    if (kernel_entry == 0) // 加载失败......
    {
        die(-1);
    }

    // 打开分页机制
    enable_page_mode();

    // 函数指针的定义语法如下：
    // <return_type> (*<pointer_name>)(<parameter_list>);
    // 将宏定义 SYS_KERNEL_LOAD_ADDR 强制转换为一个函数指针类型。
    // 整个语句的作用是将 boot_info 的地址作为参数传递给一个函数，并调用该函数
    // 函数的调用在 ./kernel/init/start.S
    // ((void (*)(boot_info_t *))SYS_KERNEL_LOAD_ADDR)(&boot_info);
    ((void (*)(boot_info_t *))kernel_entry)(&boot_info);
}