/**
 * @file    template_thread.c
 * @brief   线程模板 — 复制本文件并替换 "template" 为你的线程名
 *
 * 创建新线程步骤:
 *   1. 复制本文件 → your_thread.c, 复制头文件 → your_thread.h
 *   2. 全局替换 template → your_thread
 *   3. 修改 THREAD_PRIORITY 和 THREAD_STACK_SIZE
 *   4. 在头文件中定义你的参数结构体 xxx_params_t
 *   5. 在 thread_entry() 中实现业务逻辑
 *   6. 重新编译即可, 无需修改 tx_application.c
 *
 * 参数传递:
 *   - init 函数接收 const xxx_params_t *params
 *   - MODULE_INIT_DEFAULT() 自动注册时传入默认参数
 *   - 其他模块可构造 xxx_params_t 传入自定义参数
 *   - 传 NULL 则使用默认参数
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#include "template_thread.h"
#include "app_config.h"
#include "init.h"

/* 线程配置 */
#define THREAD_PRIORITY 10
#define THREAD_STACK_SIZE 512
#define THREAD_PREEMPT_THRESH THREAD_PRIORITY

/* 默认参数 */
static const template_thread_params_t default_params = {
	.interval_ticks = APP_MS_TO_TICKS(1000),
	.flags = 0,
	.user_data = NULL,
};

/* ThreadX 对象(本文件私有) */
static TX_THREAD thread;
static UCHAR thread_stack[THREAD_STACK_SIZE];

/* 线程入口函数 */
static void thread_entry(ULONG input);

/**
 * @brief  初始化并启动 template 线程
 *
 * @param  params  指向 template_thread_params_t 的指针, 传 NULL 使用默认参数。
 *                 调用者需保证 params 指向的内存在线程运行期间有效。
 *
 * 由 MODULE_INIT_DEFAULT() 自动注册(使用默认参数),
 * 也可被其他模块直接调用以传入自定义参数。
 */
void template_thread_init(const template_thread_params_t *params)
{
	if (!params)
		params = &default_params;

	APP_TX_CHECK(tx_thread_create(
		&thread, "template_thread", thread_entry, (ULONG)params,
		thread_stack, THREAD_STACK_SIZE, THREAD_PRIORITY,
		THREAD_PREEMPT_THRESH, TX_NO_TIME_SLICE, TX_AUTO_START));
}

/**
 * @brief  线程入口函数
 *
 * input 参数为参数结构体指针(由 tx_thread_create 的第3个参数传入),
 * 还原为参数结构体指针后即可访问所有参数。
 *
 * 重要: 循环中必须调用 tx_thread_sleep() 或某个阻塞式 ThreadX API
 * (tx_queue_receive, tx_semaphore_get 等)。永不阻塞的线程会饿死所有
 * 低优先级线程。
 */
static void thread_entry(ULONG input)
{
	const template_thread_params_t *params =
		(const template_thread_params_t *)input;

	while (1) {
		/* TODO: 在此添加业务逻辑, 通过 params-> 访问参数 */

		/* 必须调用阻塞API, 否则会饿死低优先级线程 */
		tx_thread_sleep(APP_MIN_SLEEP_TICKS(params->interval_ticks));
	}
}

/*
 * 自动注册 — 复制到 user/Application/src/ 后启用。
 * 示例文件位于 docs/examples/ 时不会参与固件构建。
 */
MODULE_INIT_DEFAULT(template_thread_init, default_params);
