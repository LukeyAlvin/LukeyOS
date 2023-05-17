/*
 * @Description: 进程管理
 * @Author: Alvin
 * @Date: 2023-04-19 19:55:43
 * @LastEditTime: 2023-05-17 15:49:11
 */
#include "core/task.h"
#include "cpu/irq.h"
#include "tools/klib.h"
#include "tools/log.h"
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "core/memory.h"
#include "cpu/mmu.h"

static task_manager_t task_manager; // 任务管理器
static uint32_t idle_task_stack[IDLE_STACK_SIZE];
/**
 * @brief 初始化TSS段
 *
 * @param task      任务控制块指针
 * @param entry     任务入口地址
 * @param flag      系统任务标志位
 * @param esp       任务堆栈指针
 * @return int      成功返回0，失败返回-1
 */
static int tss_init(task_t *task, int flag, uint32_t entry, uint32_t esp)
{
    // 为TSS分配GDT
    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0)
    {
        log_printf("alloc tss failed.\n");
        return -1;
    }

    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
                     SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);

    // tss段初始化
    kernel_memset(&task->tss, 0, sizeof(tss_t));

    int code_sel, data_sel;
    if (flag & TASK_FLAG_SYSTEM)    // 对系统工程进行初始化
    {
        code_sel = KERNEL_SELECTOR_CS;
        data_sel = KERNEL_SELECTOR_DS;
    }
    else
    {
        // 注意加了RP3,不然将产生段保护错误
        code_sel = task_manager.app_code_sel | SEG_RPL3;
        data_sel = task_manager.app_data_sel | SEG_RPL3;
    }

    task->tss.eip = entry;
    task->tss.esp = task->tss.esp0 = esp;
    task->tss.ss0 = KERNEL_SELECTOR_DS;
    task->tss.eip = entry;
    task->tss.eflags = EFLAGS_DEFAULT | EFLAGS_IF;
    task->tss.es = task->tss.ss = task->tss.ds = task->tss.fs = task->tss.gs = data_sel; // 全部采用同一数据段
    task->tss.cs = code_sel;
    task->tss.iomap = 0;

    // 页表初始化
    uint32_t page_dir = memory_create_uvm();
    if (page_dir == 0)
    {
        gdt_free_sel(tss_sel);
        return -1;
    }
    task->tss.cr3 = page_dir;

    task->tss_sel = tss_sel;
    return 0;
}

void task_switch_from_to(task_t *from, task_t *to)
{
    // 基于TSS 切换至 to->tss_sel 的任务
    switch_to_tss(to->tss_sel);
}

int task_init(task_t *task, const char *name, int flag, uint32_t entry, uint32_t esp)
{
    // 基于TSS 初始化
    int err = tss_init(task, flag, entry, esp);
    if (err < 0)
    {
        log_printf("init task failed.\n");
        return err;
    }
    // 任务字段初始化
    kernel_strncpy(task->name, name, TASK_NAME_SIZE);
    // 当前任务设置为创建态
    task->state = TASK_CREATED;
    task->sleep_ticks = 0;
    task->times_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->times_ticks;

    list_node_init(&task->all_node);
    list_node_init(&task->run_node);
    list_node_init(&task->wait_node);

    irq_state_t state = irq_enter_protection();

    // 将task任务插入到就绪队列中并设置为就绪态
    task_set_ready(task);
    // 将任务添加到任务队列（尾插法）
    list_insert_last(&task_manager.task_list, &task->all_node);

    irq_leave_protection(state);
    return 0;
}

/**
 * @brief 任务管理器初始化：将就绪队列和任务队列初始化为空，并将当前运行的任务指针设置为 NULL。
 */
void task_manager_init(void)
{
    // 数据段和代码段，使用DPL3，所有应用共用同一个
    // 为调试方便，暂时使用DPL0
    int sel = gdt_alloc_desc();
    segment_desc_set(sel, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL3 | SEG_S_NORMAL |
                         SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);
    task_manager.app_data_sel = sel;

    sel = gdt_alloc_desc();
    segment_desc_set(sel, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL3 | SEG_S_NORMAL |
                         SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);
    task_manager.app_code_sel = sel;

    // 各队列初始化
    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    list_init(&task_manager.sleep_list);

    // 对空闲进程进行初始化
    task_init(&task_manager.idle_task,
              "idle task",
              TASK_FLAG_SYSTEM,
              (uint32_t)idle_task_entry,
              (uint32_t)(idle_task_stack + IDLE_STACK_SIZE)); // 里面的值不必要写

    task_manager.curr_task = (task_t *)0;
}
/**
 * @brief 返回任务管理器中的第一个任务
 */
task_t *task_first_task(void)
{
    return &task_manager.first_task;
}

// 空闲进程初始化
static void idle_task_entry()
{
    for (;;)
    {
        // 调用低功耗指令，使得CPU处于低功耗状态
        hlt();
    }
}
// 初始化了一个名为 first_task 的内核任务，并将其设为当前运行的任务。它还通过写 TR 寄存器来切换到这个任务。
void task_first_init(void)
{
    // 获取入口地址
    void first_task_entry(void);
    uint32_t first_start = (uint32_t)first_task_entry;

    // 以下获得的是bin文件在内存中的物理地址
    extern uint8_t s_first_task[], e_first_task[];

    // 分配的空间比实际存储的空间要大一些，多余的用于放置栈
    uint32_t copy_size = (uint32_t)(e_first_task - s_first_task); // 待拷贝的区域
    uint32_t alloc_size = 10 * MEM_PAGE_SIZE;                     // 为待拷贝区域分配内存
    ASSERT(copy_size < alloc_size);

    // 初始化任务
    task_init(&task_manager.first_task, "first task", 0, first_start, 0);

    // 当前任务 指向 当前运行的第一个任务
    task_manager.curr_task = &task_manager.first_task;
    //  更新页表地址为自己的
    mmu_set_page_dir(task_manager.first_task.tss.cr3);
    // 分配一页内存供代码存放使用，然后将代码复制过去
    memory_alloc_page_for(first_start, alloc_size, PTE_P | PTE_W | PTE_U);

    kernel_memcpy((void *)first_start, (void *)s_first_task, copy_size);

    // 写TR寄存器，指示当前运行的第一个任务
    write_tr(task_manager.first_task.tss_sel);
}

// 将task任务插入到就绪队列中并设置为就绪态
void task_set_ready(task_t *task)
{
    // 不处理空闲进程
    if (task != &task_manager.idle_task)
    {
        // 首先将任务插入到就绪队列中（尾插法）
        list_insert_last(&task_manager.ready_list, &task->run_node);
        task->state = TASK_READY;
    }
}

// 将task任务从就绪态队列中移除
void task_set_block(task_t *task)
{
    // 不处理空闲进程
    if (task != &task_manager.idle_task)
    {
        list_remove(&task_manager.ready_list, &task->run_node);
        // 移除后状态不确定，因此不设置状态值
    }
}
/**
 * @description: 返回当前正在运行的进程
 * @return {*}
 */
task_t *task_current(void)
{
    return task_manager.curr_task;
}
/**
 * @description: [已弃用！]进程自己主动放弃CPU，自觉移到队列尾部，从而给其它进程留出运行的机会
 * @return {*}
 */
int sys_sched_yield(void)
{
    irq_state_t state = irq_enter_protection();

    // 首先判断就绪队列中是否有多个进程
    if (list_count(&task_manager.ready_list) > 1)
    {
        // 获取当前正在运行的任务
        task_t *curr_task = task_current();
        // 将当前任务从就绪队列中删除
        task_set_block(curr_task);
        // 将当前任务加入到就绪队列尾部
        task_set_ready(curr_task);
        // 将当前任务切换为下一个任务
        task_dispatch();
    }

    irq_leave_protection(state);
    return 0;
}
// 返回下一个即将运行的任务(本项目直接返回队头的进程)
static task_t *task_next_run()
{
    // 如果就绪队列中没有进程，则返回一个空闲进程
    if (list_count(&task_manager.ready_list) == 0)
    {
        return &task_manager.idle_task;
    }
    // 取出链表的第一个结点，即为队头的进程
    list_node_t *task_node = list_first(&task_manager.ready_list);
    // 根据当前结点的地址，获得整个任务控制块
    return list_node_parent(task_node, task_t, run_node);
}

// 切换下一个欲运行的任务
void task_dispatch()
{
    task_t *to = task_next_run();
    // 下一个任务和当前任务不同的时候才进行切换
    if (to != task_manager.curr_task)
    {
        task_t *from = task_manager.curr_task;
        task_manager.curr_task = to;

        to->state = TASK_RUNNING;
        task_switch_from_to(from, to);
    }
}
/**
 * @brief 进程调度器
 */
void task_time_tick()
{

    // 获取当前正在运行的任务
    task_t *curr_task = task_current();

    irq_state_t state = irq_enter_protection();
    // 判断当前任务的时间片计数器是否减到0
    if (--curr_task->slice_ticks == 0)
    {
        // 重新设置计数器
        curr_task->slice_ticks = curr_task->times_ticks;

        // 将当前任务从就绪队列中删除
        task_set_block(curr_task);
        // 将当前任务加入到就绪队列尾部
        task_set_ready(curr_task);
    }
    // 睡眠处理
    // 在每个时钟节拍中断发生时，扫描延时队列，发现延时时间到，则再将进程移回就绪队列的队尾
    list_node_t *curr = list_first(&task_manager.sleep_list);
    while (curr)
    {
        list_node_t *next = list_node_next(curr);
        task_t *task = list_node_parent(curr, task_t, run_node);
        if (--task->sleep_ticks == 0)
        {
            task_set_wakeup(task);
            task_set_ready(task);
        }
        curr = next;
    }
    // 由于有进程已经插入到就绪队列中，可能要马上运行，因此需要调用切换任务函数
    task_dispatch();
    irq_leave_protection(state);
}

/**
 * @description: 设置睡眠队列
 * @return {*}
 * @param {task_t} *task	要睡眠的任务
 * @param {uint32_t} ticks	任务睡眠的时间
 */
void task_set_sleep(task_t *task, uint32_t ticks)
{
    if (ticks <= 0)
        return;
    task->sleep_ticks = ticks;
    task->state = TASK_SLEEP;
    list_insert_last(&task_manager.sleep_list, &task->run_node);
}

/**
 * @description: 将任务从睡眠队列中移出
 * @return {*}
 * @param {task_t *} task	待移出的任务
 */
void task_set_wakeup(task_t *task)
{
    list_remove(&task_manager.sleep_list, &task->run_node);
}

// 延迟时间
void sys_sleep(uint32_t ms)
{
    // 至少延时1个tick
    if (ms < OS_TICK_MS)
    {
        ms = OS_TICK_MS;
    }
    irq_state_t state = irq_enter_protection();

    // 将当前进程从就绪队列中移出
    task_set_block(task_current());
    // 将给队列插入到睡眠队列中 将ms转换成时钟节拍的计数
    task_set_sleep(task_current(), (ms + (OS_TICK_MS - 1)) / OS_TICK_MS);
    // 切换进程
    task_dispatch();

    irq_leave_protection(state);
}