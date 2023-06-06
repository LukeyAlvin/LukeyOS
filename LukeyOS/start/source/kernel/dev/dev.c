/*
 * @Description: 设备管理器及其接口
 * @Author: Alvin
 * @Date: 2023-05-29 10:13:42
 * @LastEditTime: 2023-05-30 20:58:24
 */
#include "dev/dev.h"
#include "dev/tty.h"
#include "cpu/irq.h"
#include "tools/klib.h"

#define DEV_TABLE_SIZE 128 // 支持的设备数量

// 声明类型为 dev_desc_t 的 dev_tty_desc 的外部变量
// 事实上就是声明出一个 tty 设备描述信息
extern dev_desc_t dev_tty_desc;

// 用于储存不同设备描述信息的表：设备描述符表
static dev_desc_t *dev_desc_tbl[] = {
    &dev_tty_desc,
};
// 设备表
static device_t dev_tbl[DEV_TABLE_SIZE];

/**
 * @brief 打开设备
 * @param major 主要编号
 * @param minor 次要编号
 * @param data 数据指针（可选参数）
 * @return int 设备实例的标识符（用于后续设备操作），如果打开失败则返回 -1
 */
int dev_open(int major, int minor, void *data)
{
    irq_state_t state = irq_enter_protection();

    // 遍历设备表（dev_tbl）来查找设备，检查是否有相同的主要编号和次要编号的设备已经打开
    device_t *free_dev = (device_t *)0;
    for (int i = 0; i < sizeof(dev_tbl) / sizeof(dev_tbl[0]); i++)
    {
        device_t *dev = dev_tbl + i;
        if (dev->open_count == 0)
        {
            free_dev = dev;
        }
        // 如果找到已经打开的设备，增加其打开计数，并返回设备的标识符。
        else if ((dev->desc->major == major) && (dev->minor == minor))
        {
            dev->open_count++;
            irq_leave_protection(state);
            return i;
        }
    }
    // 查找设备类型描述符（dev_desc_tbl）来验证给定的主要编号是否被支持。
    // 遍历设备类型描述符表，寻找与给定主要编号匹配的描述符。
    dev_desc_t *desc = (dev_desc_t *)0;
    for (int i = 0; i < sizeof(dev_desc_tbl) / sizeof(dev_desc_tbl[0]); i++)
    {
        dev_desc_t *d = dev_desc_tbl[i];
        if (d->major == major)
        {
            desc = d;
            break;
        }
    }
    // 如果找到对应的设备类型描述符（desc）且存在空闲的设备项（free_dev）
    if (desc && free_dev)
    {
        free_dev->minor = minor;
        free_dev->data = data;
        free_dev->desc = desc;

        int err = desc->open(free_dev);
        if (err == 0)
        {
            free_dev->open_count = 1;
            irq_leave_protection(state);
            return free_dev - dev_tbl;
        }
    }
    irq_leave_protection(state);
    return -1;
}
int is_devid_bad(int dev_id)
{
    if ((dev_id < 0) || (dev_id >= sizeof(dev_tbl) / sizeof(dev_tbl[0])))
    {
        return 1;
    }

    if (dev_tbl[dev_id].desc == (dev_desc_t *)0)
    {
        return 1;
    }

    return 0;
}
/**
 * @brief 读取指定字节的数据
 */
int dev_read(int dev_id, int addr, char *buf, int size)
{
    if (is_devid_bad(dev_id))
    {
        return -1;
    }
    device_t *dev = dev_tbl + dev_id;
    return dev->desc->read(dev, addr, buf, size);
}

/**
 * @brief 写指定字节的数据
 */
int dev_write (int dev_id, int addr, char * buf, int size) {
    if (is_devid_bad(dev_id)) {
        return -1;
    }

    device_t * dev = dev_tbl + dev_id;
    return dev->desc->write(dev, addr, buf, size);
}

/**
 * @brief 发送控制命令
 */

int dev_control (int dev_id, int cmd, int arg0, int arg1) {
    if (is_devid_bad(dev_id)) {
        return -1;
    }

    device_t * dev = dev_tbl + dev_id;
    return dev->desc->control(dev, cmd, arg0, arg1);
}

/**
 * @brief 关闭设备
 */
void dev_close (int dev_id) {
    if (is_devid_bad(dev_id)) {
        return;
    }

    device_t * dev = dev_tbl + dev_id;
    
    irq_state_t state = irq_enter_protection();
    // 如果打开多次则循环关闭
    if (--dev->open_count == 0) {
        dev->desc->close(dev);
        kernel_memset(dev, 0, sizeof(device_t));
    }
    irq_leave_protection(state);
}