/**
 * @file    template_thread.c
 * @brief   线程模板 — 复制本文件并替换 "template" 为你的线程名
 *
 * 创建新线程步骤:
 *   1. 复制本文件 → your_thread.c, 复制头文件 → your_thread.h
 *   2. 全局替换 template → your_thread
 *   3. 修改 THREAD_PRIORITY 和 THREAD_STACK_SIZE
 *   4. 在 thread_entry() 中实现业务逻辑
 *   5. 重新编译即可, 无需修改 tx_application.c
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#include "template_thread.h"
#include "tx_api.h"
#include "init.h"

/* 线程配置 */
#define THREAD_PRIORITY      10
#define THREAD_STACK_SIZE    512
#define THREAD_PREEMPT_THRESH THREAD_PRIORITY

/* ThreadX 对象(本文件私有) */
static TX_THREAD thread;
static UCHAR thread_stack[THREAD_STACK_SIZE];

/* 线程入口函数 */
static void thread_entry(ULONG input);

/**
 * @brief  初始化并启动 template 线程
 *
 * 通过 MODULE_INIT() 宏自动注册, 无需在 tx_application.c 中手动调用。
 */
void template_thread_init(void)
{
	tx_thread_create(&thread, "template_thread", thread_entry, 0,
			 thread_stack, THREAD_STACK_SIZE,
			 THREAD_PRIORITY, THREAD_PREEMPT_THRESH,
			 TX_NO_TIME_SLICE, TX_AUTO_START);
}

/**
 * @brief  线程入口函数
 *
 * 重要: 循环中必须调用 tx_thread_sleep() 或某个阻塞式 ThreadX API
 * (tx_queue_receive, tx_semaphore_get 等)。永不阻塞的线程会饿死所有
 * 低优先级线程。
 */
static void thread_entry(ULONG input)
{
	(void)input;

	while (1) {
		/* TODO: 在此添加业务逻辑 */

		/* 必须调用阻塞API, 否则会饿死低优先级线程 */
		tx_thread_sleep(1000);
	}
}

/* 自动注册 — 添加新线程时只需这一行, 无需修改 tx_application.c */
MODULE_INIT(template_thread_init);
