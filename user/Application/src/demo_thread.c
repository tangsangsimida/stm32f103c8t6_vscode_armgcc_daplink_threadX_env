/**
 * @file    demo_thread.c
 * @brief   RGB LED 演示线程 — 颜色循环演示
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#include "demo_thread.h"
#include "app_config.h"
#include "init.h"
#include "rgb_led.h"

/* 线程配置 */
#define DEMO_THREAD_PRIORITY APP_PRIO_NORMAL_MIN
#define DEMO_THREAD_STACK_SIZE 1024
#define DEMO_THREAD_PREEMPT_THRESH DEMO_THREAD_PRIORITY

/* 默认参数 */
static const demo_thread_params_t default_params = {
	.color_cycle_ticks = APP_MS_TO_TICKS(500),
};

/* ThreadX 对象(本文件私有) */
static TX_THREAD thread;
static UCHAR thread_stack[DEMO_THREAD_STACK_SIZE];

/* 线程入口函数 */
static void thread_entry(ULONG input);

/**
 * @brief  初始化并启动 RGB LED 演示线程
 *
 * @param  params  指向 demo_thread_params_t 的指针, 传 NULL 使用默认参数。
 *                 调用者需保证 params 指向的内存在线程运行期间有效。
 *
 * 由 MODULE_INIT_DEFAULT() 自动注册(使用默认参数),
 * 也可被其他模块直接调用以传入自定义参数。
 */
void demo_thread_init(const demo_thread_params_t *params)
{
	if (!params)
		params = &default_params;

	APP_TX_CHECK(tx_thread_create(
		&thread, "demo_thread", thread_entry, (ULONG)params,
		thread_stack, DEMO_THREAD_STACK_SIZE, DEMO_THREAD_PRIORITY,
		DEMO_THREAD_PREEMPT_THRESH, TX_NO_TIME_SLICE, TX_AUTO_START));
}

static void thread_entry(ULONG input)
{
	const demo_thread_params_t *params =
		(const demo_thread_params_t *)input;

	rgb_led_init();

	static const rgb_color_t color_table[] = {
		RGB_COLOR_RED,	  RGB_COLOR_GREEN, RGB_COLOR_BLUE,
		RGB_COLOR_YELLOW, RGB_COLOR_CYAN,  RGB_COLOR_MAGENTA,
		RGB_COLOR_WHITE,  RGB_COLOR_OFF,
	};
#define COLOR_TABLE_SIZE (sizeof(color_table) / sizeof(color_table[0]))

	uint32_t color_index = 0;

	while (1) {
		rgb_led_set_color(color_table[color_index]);
		color_index = (color_index + 1) % COLOR_TABLE_SIZE;
		tx_thread_sleep(APP_MIN_SLEEP_TICKS(params->color_cycle_ticks));
	}
}

MODULE_INIT_DEFAULT(demo_thread_init, default_params);
