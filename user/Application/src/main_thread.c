/**
 * @file    main_thread.c
 * @brief   主应用线程 — RGB LED 颜色循环演示
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#include "main_thread.h"
#include "tx_api.h"
#include "init.h"
#include "rgb_led.h"

/* 线程配置 */
#define MAIN_THREAD_PRIORITY 5
#define MAIN_THREAD_STACK_SIZE 1024
#define MAIN_THREAD_PREEMPT_THRESH MAIN_THREAD_PRIORITY

/* ThreadX 对象(本文件私有) */
static TX_THREAD thread;
static UCHAR thread_stack[MAIN_THREAD_STACK_SIZE];

/* 线程入口函数 */
static void thread_entry(ULONG input);

void main_thread_init(void)
{
	tx_thread_create(&thread, "main_thread", thread_entry, 0, thread_stack,
			 MAIN_THREAD_STACK_SIZE, MAIN_THREAD_PRIORITY,
			 MAIN_THREAD_PREEMPT_THRESH, TX_NO_TIME_SLICE,
			 TX_AUTO_START);
}

static void thread_entry(ULONG input)
{
	(void)input;

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
		tx_thread_sleep(500);
	}
}

MODULE_INIT(main_thread_init);
