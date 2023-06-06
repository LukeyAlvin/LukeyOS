/*
 * @Description: 设备管理器及其接口
 * @Author: Alvin
 * @Date: 2023-05-29 10:13:48
 * @LastEditTime: 2023-05-30 20:57:47
 */
#ifndef DEV_H
#define DEV_H

#define DEV_NAME_SIZE 32

enum
{
    DEV_UNKNOWN = 0, // 未知类型
    DEV_TTY,         // TTY设备

};
struct _dev_desc_t;

/**
 * @brief 表示一个具体的设备实例
 * 通过将设备的状态信息和实例相关的数据存储在其中，可以针对具体的设备实例保存和管理状态信息
 */
typedef struct _device_t
{
    struct _dev_desc_t *desc; // 设备特性描述符
    int mode;                 // 操作模式
    int minor;                // 次设备号
    void *data;               // 设备参数
    int open_count;           // 打开次数
} device_t;
/**
 * @brief: 表示设备的特性描述符。它描述了一个设备的属性和操作函数
 * 将设备的公共属性和操作函数抽象出来，使得不同类型的设备可以共享相同的接口
 */
typedef struct _dev_desc_t
{
    char name[DEV_NAME_SIZE]; // 设备名称
    int major;                // 主设备号

    /* 这些函数指针需要通过这些顶层函数进行调用和管理 */

    int (*open)(device_t *dev); 
    int (*read)(device_t *dev, int addr, char *buf, int size);
    int (*write)(device_t *dev, int addr, char *buf, int size);
    int (*control)(device_t *dev, int cmd, int arg0, int arg1);
    void (*close)(device_t *dev);
} dev_desc_t;

/* 设备接口的顶层函数，用于操作设备和管理设备的生命周期 */
/* dev_open 函数应该返回一个设备实例的标识符 dev_id，以便唯一标识设备实例并进行后续的操作*/

int dev_open(int major, int minor, void *data);
int dev_read(int dev_id, int addr, char *buf, int size);
int dev_write(int dev_id, int addr, char *buf, int size);
int dev_control(int dev_id, int cmd, int arg0, int arg1);
void dev_close(int dev_id);

#endif