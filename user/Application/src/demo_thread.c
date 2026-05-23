/**
 * @file    demo_thread.c
 * @brief   RGB LED 演示线程 — 彩虹、呼吸、闪烁和频闪效果
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
	.color_step_ticks = APP_MS_TO_TICKS(120),
	.breath_frame_ticks = APP_MS_TO_TICKS(8),
};

/* ThreadX 对象(本文件私有) */
static TX_THREAD thread;
static UCHAR thread_stack[DEMO_THREAD_STACK_SIZE];

/* 线程入口函数 */
static void thread_entry(ULONG input);
static void sleep_ticks(uint32_t ticks);
static void show_color(rgb_color_t color, uint32_t ticks);
static void show_color_pwm(rgb_color_t color, uint8_t brightness,
			   uint32_t frame_ticks);
static void run_rainbow_cycle(const demo_thread_params_t *params);
static void run_breath_cycle(const demo_thread_params_t *params);
static void run_sparkle_cycle(const demo_thread_params_t *params);
static void run_strobe_cycle(const demo_thread_params_t *params);

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

	while (1) {
		run_rainbow_cycle(params);
		run_breath_cycle(params);
		run_sparkle_cycle(params);
		run_strobe_cycle(params);
	}
}

static void sleep_ticks(uint32_t ticks)
{
	tx_thread_sleep(APP_MIN_SLEEP_TICKS(ticks));
}

static void show_color(rgb_color_t color, uint32_t ticks)
{
	rgb_led_set_color(color);
	sleep_ticks(ticks);
}

static void show_color_pwm(rgb_color_t color, uint8_t brightness,
			   uint32_t frame_ticks)
{
#define PWM_STEPS 8U
	uint32_t on_ticks;
	uint32_t off_ticks;
	uint32_t unit_ticks;

	if (brightness == 0U) {
		rgb_led_off();
		sleep_ticks(frame_ticks);
		return;
	}

	if (brightness >= PWM_STEPS) {
		show_color(color, frame_ticks);
		return;
	}

	unit_ticks = APP_MIN_SLEEP_TICKS(frame_ticks / PWM_STEPS);
	on_ticks = brightness * unit_ticks;
	off_ticks = (PWM_STEPS - brightness) * unit_ticks;

	rgb_led_set_color(color);
	sleep_ticks(on_ticks);
	rgb_led_off();
	sleep_ticks(off_ticks);
#undef PWM_STEPS
}

static void run_rainbow_cycle(const demo_thread_params_t *params)
{
	static const rgb_color_t color_table[] = {
		RGB_COLOR_RED,	   RGB_COLOR_YELLOW, RGB_COLOR_GREEN,
		RGB_COLOR_CYAN,	   RGB_COLOR_BLUE,   RGB_COLOR_MAGENTA,
		RGB_COLOR_WHITE,   RGB_COLOR_OFF,    RGB_COLOR_WHITE,
		RGB_COLOR_MAGENTA, RGB_COLOR_BLUE,   RGB_COLOR_CYAN,
		RGB_COLOR_GREEN,   RGB_COLOR_YELLOW,
	};
#define COLOR_TABLE_SIZE (sizeof(color_table) / sizeof(color_table[0]))
	uint32_t i;

	for (i = 0; i < COLOR_TABLE_SIZE; i++)
		show_color(color_table[i], params->color_step_ticks);

#undef COLOR_TABLE_SIZE
}

static void run_breath_cycle(const demo_thread_params_t *params)
{
	static const rgb_color_t breath_colors[] = {
		RGB_COLOR_BLUE,
		RGB_COLOR_CYAN,
		RGB_COLOR_MAGENTA,
	};
#define BREATH_COLOR_COUNT (sizeof(breath_colors) / sizeof(breath_colors[0]))
	uint32_t color;
	uint8_t brightness;
	uint8_t repeat;

	for (color = 0; color < BREATH_COLOR_COUNT; color++) {
		for (repeat = 0; repeat < 3U; repeat++) {
			for (brightness = 0; brightness <= 8U; brightness++)
				show_color_pwm(breath_colors[color], brightness,
					       params->breath_frame_ticks);

			for (brightness = 8U; brightness > 0U; brightness--)
				show_color_pwm(breath_colors[color],
					       (uint8_t)(brightness - 1U),
					       params->breath_frame_ticks);
		}
	}

#undef BREATH_COLOR_COUNT
}

static void run_sparkle_cycle(const demo_thread_params_t *params)
{
	static const rgb_color_t sparkle_table[] = {
		RGB_COLOR_WHITE,  RGB_COLOR_OFF,     RGB_COLOR_CYAN,
		RGB_COLOR_OFF,	  RGB_COLOR_MAGENTA, RGB_COLOR_OFF,
		RGB_COLOR_YELLOW, RGB_COLOR_OFF,     RGB_COLOR_BLUE,
		RGB_COLOR_OFF,	  RGB_COLOR_GREEN,   RGB_COLOR_OFF,
	};
#define SPARKLE_TABLE_SIZE (sizeof(sparkle_table) / sizeof(sparkle_table[0]))
	uint32_t i;
	uint32_t sparkle_ticks = params->color_step_ticks / 3U;

	for (i = 0; i < SPARKLE_TABLE_SIZE; i++)
		show_color(sparkle_table[i], sparkle_ticks);

#undef SPARKLE_TABLE_SIZE
}

static void run_strobe_cycle(const demo_thread_params_t *params)
{
	static const rgb_color_t strobe_colors[] = {
		RGB_COLOR_RED,	  RGB_COLOR_OFF,  RGB_COLOR_GREEN,
		RGB_COLOR_OFF,	  RGB_COLOR_BLUE, RGB_COLOR_OFF,
		RGB_COLOR_YELLOW, RGB_COLOR_CYAN, RGB_COLOR_MAGENTA,
		RGB_COLOR_WHITE,  RGB_COLOR_OFF,
	};
#define STROBE_COLOR_COUNT (sizeof(strobe_colors) / sizeof(strobe_colors[0]))
	uint32_t i;
	uint32_t strobe_ticks = params->color_step_ticks / 2U;

	for (i = 0; i < STROBE_COLOR_COUNT; i++)
		show_color(strobe_colors[i], strobe_ticks);

#undef STROBE_COLOR_COUNT
}

MODULE_INIT_DEFAULT(demo_thread_init, default_params);
