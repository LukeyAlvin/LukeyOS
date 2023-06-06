/*
 * @Description: MMU与分布处理
 * @Author: Alvin
 * @Date: 2023-04-24 11:26:26
 * @LastEditTime: 2023-05-23 16:07:28
 */
#ifndef MMU_H
#define MMU_H

#include "common/types.h"
#include "common/cpu_instr.h"

#define PDE_CNT 1024 // 页目录的表项个数 1024个
#define PTE_CNT 1024 // 页表的表项个数 1024个
#define PTE_P (1 << 0)
#define PTE_W (1 << 1)
#define PTE_U (1 << 2)

#define PDE_P (1 << 0)
#define PDE_U (1 << 2)



#pragma pack(1)
/**
 * @brief Page-Directory Entry：32位的页目录项，它用于引用一个页表
 */
typedef union _pde_t
{
    uint32_t v;
    struct
    {
        uint32_t present : 1;       // 0 (P) 存在位；必须为1才能访问该页表
        uint32_t write_disable : 1; // 1 (R/W) 读写位；如果为0，则不允许对该条目控制的4MB区域进行写操作
        uint32_t user_mode_acc : 1; // 2 (U/S) 用户/超级用户位；如果为0，则不允许用户模式访问该条目控制的4MB区域
        uint32_t write_through : 1; // 3 (PWT) 页面级写穿位；间接决定了访问该条目引用的页表所使用的内存类型
        uint32_t cache_disable : 1; // 4 (PCD) 页面级缓存禁用位；间接决定了访问该条目引用的页表所使用的内存类型
        uint32_t accessed : 1;      // 5 (A) 访问位；指示该条目是否已被用于线性地址转换
        uint32_t : 1;               // 6 Ignored;
        uint32_t ps : 1;            // 7 (PS) 如果CR4.PSE = 1，则必须为0（否则，该条目映射4MB页；）；否则，忽略
        uint32_t : 4;               // 11:8 忽略位
        uint32_t phy_pt_addr : 20;  // 高20位page table物理地址
    };
} pde_t;

/**
 * @brief Page-Table Entry
 */
typedef union _pte_t
{
    uint32_t v; 
    struct
    {
        uint32_t present : 1;        // 0 (P) 存在位；必须为1才能映射4KB页
        uint32_t write_disable : 1;  // 1 (R/W) 读写位；如果为0，则不允许对该条目引用的4KB页进行写操作
        uint32_t user_mode_acc : 1;  // 2 (U/S) 用户/超级用户位；如果为0，则不允许用户模式访问该条目引用的4KB页
        uint32_t write_through : 1;  // 3 (PWT) 页级写穿位；间接决定了访问该条目引用的4KB页所使用的内存类型
        uint32_t cache_disable : 1;  // 4 (PCD) 页面级缓存禁用位；间接决定了访问该条目引用的4KB页所使用的内存类型
        uint32_t accessed : 1;       // 5 (A) 访问位；指示该条目引用的4KB页是否已被软件访问过
        uint32_t dirty : 1;          // 6 (D) 脏位；指示该条目引用的4KB页是否已被软件写入过
        uint32_t pat : 1;            // 7 PAT 页属性表索引位；如果支持PAT，则间接决定了访问该条目引用的4KB页所使用的内存类型；否则，保留（必须为0）
        uint32_t global : 1;         // 8 (G) 全局位；如果CR4.PGE = 1，则决定翻译是否是全局的；否则忽略
        uint32_t : 3;                // Ignored
        uint32_t phy_page_addr : 20; // 高20位物理地址
    };
} pte_t;

#pragma pack()

/**
 * @brief 返获取页目录项的索引：将虚拟地址的高 10 位作为页目录项的索引。
 */
static inline uint32_t pde_index(uint32_t vaddr)
{
    int index = (vaddr >> 22); // 只取高10位
    return index;
}

/**
 * @brief 获取页目录项中的物理地址
 */
static inline uint32_t pde_paddr(pde_t *pde)
{
    return pde->phy_pt_addr << 12;
}

/**
 * @brief 获取页表项的索引：将虚拟地址的中间 10 位作为页表项的索引。
 */
static inline int pte_index(uint32_t vaddr)
{
    return (vaddr >> 12) & 0x3FF; // 取中间10位
}

/**
 * @brief 获取页表项中的物理地址
 */
static inline uint32_t pte_paddr(pte_t *pte)
{
    return pte->phy_page_addr << 12;
}

/**
 * @brief 获取页表项中的权限位
 */
static inline uint32_t get_pte_perm(pte_t *pte)
{
    return (pte->v & 0x1FF); // 2023年2月19 同学发现有问题，改了下
}

/**
 * @brief 设置页目录表的物理地址
 * @param paddr 页目录表的物理地址
 */
static inline void mmu_set_page_dir(uint32_t paddr)
{
    // 将该物理地址写入控制寄存器CR3中，从而更新页目录表的物理地址
    write_cr3(paddr);
}



#endif // MMU_H