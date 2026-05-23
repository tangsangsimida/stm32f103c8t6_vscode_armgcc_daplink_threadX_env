/**
 * @file    main_thread.c
 * @brief   主应用线程 — RGB LED 颜色循环演示
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#include "main_thread.h"
#include "tx_api.h"
#include "init.h"
#include "main.h"
#include "rgb_led.h"

/* 线程配置 */
#define MAIN_THREAD_PRIORITY 5
#define MAIN_THREAD_STACK_SIZE 1024
#define MAIN_THREAD_PREEMPT_THRESH MAIN_THREAD_PRIORITY

/* 默认参数 */
static const main_thread_params_t default_params = {
	.color_cycle_ticks = 500,
};

/* ThreadX 对象(本文件私有) */
static TX_THREAD thread;
static UCHAR thread_stack[MAIN_THREAD_STACK_SIZE];

/* 线程入口函数 */
static void thread_entry(ULONG input);

/**
 * @brief  初始化并启动主应用线程
 *
 * @param  params  指向 main_thread_params_t 的指针, 传 NULL 使用默认参数。
 *                 调用者需保证 params 指向的内存在线程运行期间有效。
 *
 * 由 MODULE_INIT_DEFAULT() 自动注册(使用默认参数),
 * 也可被其他模块直接调用以传入自定义参数。
 */
void main_thread_init(const main_thread_params_t *params)
{
	UINT status;

	if (!params)
		params = &default_params;

	status = tx_thread_create(&thread, "main_thread", thread_entry,
				  (ULONG)params, thread_stack,
				  MAIN_THREAD_STACK_SIZE, MAIN_THREAD_PRIORITY,
				  MAIN_THREAD_PREEMPT_THRESH, TX_NO_TIME_SLICE,
				  TX_AUTO_START);
	if (status != TX_SUCCESS)
		Error_Handler();
}

static void thread_entry(ULONG input)
{
	const main_thread_params_t *params =
		(const main_thread_params_t *)input;

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
		tx_thread_sleep(params->color_cycle_ticks ?
					params->color_cycle_ticks :
					1);
	}
}

MODULE_INIT_DEFAULT(main_thread_init, default_params);
