/**
 * @file    tx_application.c
 * @brief   ThreadX应用入口和线程定义
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 *
 * =============================================================================
 * [移植说明] 本文件与MCU无关, 更换MCU时无需修改。
 *
 * 添加新线程、信号量、队列或事件标志时:
 *   1. 声明ThreadX对象及其栈/内存区域的静态变量
 *   2. 在tx_application_define()中创建对象
 *   3. 实现线程入口函数
 *
 * 线程优先级范围: 0(最高) 到 TX_MAX_PRIORITIES-1(最低)。
 * 默认TX_MAX_PRIORITIES = 32(定义在tx_port.h中)。
 * =============================================================================
 */

#include "tx_api.h"
#include "main.h"

/* =========================================================================
 * 线程配置
 *
 * MAIN_THREAD_STACK_SIZE      : 栈大小(字节)。如果线程调用链较深或使用大型
 *                               局部数组, 请增大此值。
 * MAIN_THREAD_PRIORITY        : 线程优先级(0 = 最高)。
 * MAIN_THREAD_PREEMPT_THRESH  : 抢占阈值。通常与优先级相同。
 *                               设置为低于优先级可创建优先级继承区域(配合互斥锁使用)。
 * ========================================================================= */
#define MAIN_THREAD_STACK_SIZE      1024
#define MAIN_THREAD_PRIORITY        5
#define MAIN_THREAD_PREEMPT_THRESH   5

/* =========================================================================
 * 字节池配置
 *
 * 字节池提供动态内存分配功能(类似malloc)。
 * 线程可通过tx_byte_allocate() / tx_byte_release()使用此池。
 *
 * BYTE_POOL_SIZE : 可用于动态分配的总字节数。
 *                   如果应用需要更多堆内存, 请增大此值。
 * ========================================================================= */
#define BYTE_POOL_SIZE              4096

/* ThreadX对象 */
static TX_THREAD main_thread;
static UCHAR main_thread_stack[MAIN_THREAD_STACK_SIZE];

static TX_BYTE_POOL byte_pool;
static UCHAR byte_pool_area[BYTE_POOL_SIZE] __attribute__((aligned(4)));

/* 前向声明 */
static void main_thread_entry(ULONG thread_input);

/**
 * @brief  ThreadX应用定义函数
 *
 * 由tx_kernel_enter()在_tx_initialize_low_level()完成后调用。
 * 在此创建所有ThreadX对象(线程、信号量、互斥锁、队列、事件标志、
 * 字节池、块池、定时器)。
 *
 * @param  first_free_memory  空闲RAM起始地址(由_tx_initialize_low_level提供)
 *
 * 注意: 不要从本函数返回。ThreadX调度器在本函数完成后启动, 不会返回到调用者。
 */
void tx_application_define(VOID *first_free_memory)
{
    (void)first_free_memory;

    /* 创建字节池用于动态内存分配 */
    tx_byte_pool_create(&byte_pool, "app_byte_pool", byte_pool_area, BYTE_POOL_SIZE);

    /* 创建主应用线程 */
    tx_thread_create(&main_thread,
                     "main_thread",          /* 线程名称(用于跟踪) */
                     main_thread_entry,      /* 线程入口函数 */
                     0,                      /* 线程输入参数 */
                     main_thread_stack,      /* 栈起始地址 */
                     MAIN_THREAD_STACK_SIZE, /* 栈大小(字节) */
                     MAIN_THREAD_PRIORITY,   /* 优先级 */
                     MAIN_THREAD_PREEMPT_THRESH, /* 抢占阈值 */
                     TX_NO_TIME_SLICE,       /* 不使用时间片 */
                     TX_AUTO_START);         /* 立即启动 */

    /*
     * 示例: 创建额外线程
     *
     * static TX_THREAD worker_thread;
     * static UCHAR worker_stack[512];
     *
     * tx_thread_create(&worker_thread, "worker", worker_entry, 0,
     *                  worker_stack, 512, 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
     *
     * 示例: 创建信号量
     *
     * static TX_SEM my_semaphore;
     * tx_semaphore_create(&my_semaphore, "my_sem", 0);
     *
     * 示例: 创建互斥锁
     *
     * static TX_MUTEX my_mutex;
     * tx_mutex_create(&my_mutex, "my_mutex", TX_NO_INHERIT);
     *
     * 示例: 创建队列
     *
     * static TX_QUEUE my_queue;
     * static ULONG queue_buf[16];
     * tx_queue_create(&my_queue, "my_queue", TX_1_ULONG, queue_buf, 16 * sizeof(ULONG));
     *
     * 示例: 创建事件标志
     *
     * static TX_EVENT_FLAGS_GROUP my_events;
     * tx_event_flags_create(&my_events, "my_events");
     */
}

/**
 * @brief  主应用线程入口函数
 *
 * 替代裸机while(1)循环。将应用逻辑添加到while(1)循环内。
 *
 * 重要: 循环中必须调用tx_thread_sleep()或某个阻塞式ThreadX API
 * (tx_queue_receive、tx_semaphore_get等)。永不阻塞的线程会饿死所有
 * 低优先级线程。
 *
 * @param  thread_input  输入参数(未使用, 始终为0)
 */
static void main_thread_entry(ULONG thread_input)
{
    (void)thread_input;

    /*
     * 初始化需要在ThreadX运行后才能工作的外设。
     * HAL_Init()和MX_GPIO_Init()已在main()中tx_kernel_enter()之前执行,
     * 因此只需在此添加延迟初始化的外设。
     */

    while (1)
    {
        /* 应用主循环 */

        /* 休眠1个节拍(1ms), 将CPU让给其他线程 */
        tx_thread_sleep(1);
    }
}
