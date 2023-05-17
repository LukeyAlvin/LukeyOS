/*
 * @Description: 内存管理
 * @Author: Alvin
 * @Date: 2023-04-22 20:04:11
 * @LastEditTime: 2023-05-06 10:53:23
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "tools/bitmap.h"
#include "common/boot_info.h"
#include "ipc/mutex.h"

// EBDA 是 BIOS 留给操作系统使用的一块内存区域，它通常包含了硬件信息、设备驱动程序等。
// 它的值为 0x00080000 ~ 0xFFFFFFFF，即 512KB 的位置。
#define MEM_EBDA_START 0x00080000
// 表示可用的物理内存区域的起始地址，即1024*1024（1MB），即系统中第一个可用的物理内存区域的起始地址。
#define MEM_EXT_START (1024 * 1024)
// 表示页的大小，它的值为 4096，即 4KB
#define MEM_PAGE_SIZE 4096 // 和页表大小一致
// 用户虚拟内存空间的起始虚拟地址
#define MEMORY_TASK_BASE 0x80000000
// 结束地址
#define MEM_EXT_END  (128*1024*1024 - 1)
/**
 * @brief 地址分配结构
 */
typedef struct _addr_alloc_t
{
    mutex_t mutex;   // 互斥信号量，用于保证在多线程环境下对地址分配的访问是同步的。
    bitmap_t bitmap; // 辅助分配用的位图，用于记录哪些地址已经被分配，哪些可以被分配。

    uint32_t page_size; // 页大小，表示每一页的大小。
    uint32_t start;     // 起始地址，表示分配的地址空间的起始地址。
    uint32_t size;      // 地址大小，表示分配的地址空间的总大小。
} addr_alloc_t;

/**
 * @brief 虚拟地址到物理地址之间的映射关系表
 */
typedef struct _memory_map_t
{
    void *vstart;  // 内存区域的虚拟起始地址。
    void *vend;    // 内存区域的虚拟结束地址。
    void *pstart;  // 内存区域的物理起始地址。
    uint32_t perm; // 内存区域的权限，例如读、写或执行等。
} memory_map_t;

void memory_init(boot_info_t *boot_info);
// 创建一个用户虚拟内存即页表
uint32_t memory_create_uvm();
/**
 * 为一个页目录分配一段连续的虚拟地址空间，并将这段虚拟地址空间与物理地址空间进行映射
 * @return {*}
 * @param {uint32_t} page_dir   页目录的地址
 * @param {uint32_t} vaddr      虚拟地址的起始地址
 * @param {uint32_t} size       需要分配的虚拟地址空间大小
 * @param {int} perm            映射的权限
 */
uint32_t memory_alloc_for_page_dir (uint32_t page_dir, uint32_t vaddr, uint32_t size, int perm);
/**
 * 为指定的虚拟地址空间分配多页内存
 * @return {*}
 * @param {uint32_t} addr   虚拟空间的起始地址
 * @param {uint32_t} size   虚拟空间的大小
 * @param {int} perm        权限
 */
int memory_alloc_page_for (uint32_t addr, uint32_t size, int perm);
#endif // MEMORY_H