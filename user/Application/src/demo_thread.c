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

/* 线程配置: demo是普通业务线程, 不参与实时控制闭环。 */
#define DEMO_THREAD_PRIORITY APP_PRIO_NORMAL_MIN
#define DEMO_THREAD_STACK_SIZE 1024
#define DEMO_THREAD_PREEMPT_THRESH DEMO_THREAD_PRIORITY

/* 默认参数使用tick, 通过APP_MS_TO_TICKS()从便于阅读的毫秒值转换。 */
static const demo_thread_params_t default_params = {
	.color_step_ticks = APP_MS_TO_TICKS(120),
	.breath_frame_ticks = APP_MS_TO_TICKS(8),
};

/* ThreadX对象和栈为静态分配, 会直接占用SRAM。 */
static TX_THREAD thread;
static UCHAR thread_stack[DEMO_THREAD_STACK_SIZE];

static void thread_entry(ULONG input);
static void sleep_ticks(uint32_t ticks);
static void show_color(rgb_color_t color, uint32_t ticks);
static void show_color_brightness(rgb_color_t color,
				  rgb_brightness_t brightness,
				  uint32_t frame_ticks);
static rgb_brightness_t scale_brightness(uint8_t value, uint8_t max);
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

/**
 * @brief  线程睡眠封装, 防御0 tick导致的忙循环
 *
 * @param  ticks  需要睡眠的ThreadX tick数。传0时自动按1 tick处理。
 */
static void sleep_ticks(uint32_t ticks)
{
	tx_thread_sleep(APP_MIN_SLEEP_TICKS(ticks));
}

/**
 * @brief  显示一个离散颜色并保持指定tick
 *
 * @param  color  RGB颜色掩码, 可使用RGB_COLOR_xxx或按位组合。
 * @param  ticks  保持该颜色的ThreadX tick数。传0时自动按1 tick处理。
 */
static void show_color(rgb_color_t color, uint32_t ticks)
{
	rgb_led_set_color(color);
	sleep_ticks(ticks);
}

/**
 * @brief  使用硬件PWM显示指定颜色和亮度
 *
 * @param  color        RGB颜色掩码, 未包含的通道保持熄灭。
 * @param  brightness   整体亮度, 0为熄灭, RGB_BRIGHTNESS_MAX为最亮。
 * @param  frame_ticks  保持该亮度的ThreadX tick数。传0时自动按1 tick处理。
 */
static void show_color_brightness(rgb_color_t color,
				  rgb_brightness_t brightness,
				  uint32_t frame_ticks)
{
	rgb_led_set_color_brightness(color, brightness);
	sleep_ticks(frame_ticks);
}

/**
 * @brief  将小范围亮度等级映射到RGB驱动的16位亮度范围
 *
 * @param  value  当前亮度等级, 范围为0~max。
 * @param  max    最大亮度等级, 必须大于0。
 *
 * @return 映射后的16位逻辑亮度, 范围为0~RGB_BRIGHTNESS_MAX。
 */
static rgb_brightness_t scale_brightness(uint8_t value, uint8_t max)
{
	return (rgb_brightness_t)(((uint32_t)value * RGB_BRIGHTNESS_MAX) / max);
}

/**
 * @brief  离散彩虹色往返轮转
 *
 * @param  params  演示线程参数。使用color_step_ticks控制每个颜色
 *                 保持的时间。
 */
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

/**
 * @brief  基于TIM3硬件PWM的平滑呼吸效果
 *
 * @param  params  演示线程参数。使用breath_frame_ticks控制每个亮度
 *                 阶段保持的时间。
 */
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
				show_color_brightness(
					breath_colors[color],
					scale_brightness(brightness, 8U),
					params->breath_frame_ticks);

			for (brightness = 8U; brightness > 0U; brightness--)
				show_color_brightness(
					breath_colors[color],
					scale_brightness(
						(uint8_t)(brightness - 1U), 8U),
					params->breath_frame_ticks);
		}
	}

#undef BREATH_COLOR_COUNT
}

/**
 * @brief  快速亮灭交替的闪光点缀效果
 *
 * @param  params  演示线程参数。使用color_step_ticks派生闪光节奏。
 */
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

/**
 * @brief  高对比度彩色频闪段落
 *
 * @param  params  演示线程参数。使用color_step_ticks派生频闪节奏。
 */
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
