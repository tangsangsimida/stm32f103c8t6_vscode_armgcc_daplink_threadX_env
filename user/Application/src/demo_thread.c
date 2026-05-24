/**
 * @file    demo_thread.c
 * @brief   RGB LED 演示线程 — 连续色轮呼吸环境光
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
	.frame_ticks = APP_MS_TO_TICKS(12),
	.hue_step = 1U,
	.breath_step = 1U,
};

typedef struct {
	rgb_brightness_t red;
	rgb_brightness_t green;
	rgb_brightness_t blue;
} demo_rgb_t;

/* ThreadX对象和栈为静态分配, 会直接占用SRAM。 */
static TX_THREAD thread;
static UCHAR thread_stack[DEMO_THREAD_STACK_SIZE];

static void thread_entry(ULONG input);
static void sleep_ticks(uint32_t ticks);
static void show_rgb(demo_rgb_t color, uint32_t ticks);
static demo_rgb_t color_wheel(uint8_t hue, rgb_brightness_t brightness);
static rgb_brightness_t scale_brightness(uint16_t value, uint16_t max);
static rgb_brightness_t ease_brightness(uint8_t phase);
static rgb_brightness_t ambient_brightness(uint8_t phase);
static uint8_t normalized_step(uint8_t step);
static void run_ambient_loop(const demo_thread_params_t *params);

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

/**
 * @brief 线程入口函数，负责初始化RGB LED并执行环境光循环任务。
 *
 * @param input 传入的参数指针，需强制转换为 demo_thread_params_t 类型，包含线程运行所需的配置信息。
 *
 * @return 无返回值。
 */
static void thread_entry(ULONG input)
{
	/* 将输入参数转换为线程参数结构体指针 */
	const demo_thread_params_t *params =
		(const demo_thread_params_t *)input;

	/* 初始化RGB LED硬件 */
	rgb_led_init();

	/* 运行环境光检测与控制主循环 */
	run_ambient_loop(params);
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
 * @brief  设置三通道PWM亮度并保持指定tick
 *
 * @param  color  三通道逻辑亮度, 0为熄灭, RGB_BRIGHTNESS_MAX为最亮。
 * @param  ticks  保持该颜色的ThreadX tick数。传0时自动按1 tick处理。
 */
static void show_rgb(demo_rgb_t color, uint32_t ticks)
{
	rgb_led_set_rgb(color.red, color.green, color.blue);
	sleep_ticks(ticks);
}

/**
 * @brief  将色轮位置转换为RGB亮度
 *
 * @param  hue         色轮位置, 0~255, 自动从红到绿到蓝再回到红。
 * @param  brightness  整体亮度, 0为熄灭, RGB_BRIGHTNESS_MAX为最亮。
 *
 * @return 对应的RGB三通道逻辑亮度。
 */
static demo_rgb_t color_wheel(uint8_t hue, rgb_brightness_t brightness)
{
	demo_rgb_t color = { 0U, 0U, 0U };
	uint8_t offset;
	rgb_brightness_t rising;
	rgb_brightness_t falling;

	if (hue < 85U) {
		offset = hue;
		rising = scale_brightness(offset, 84U);
		falling = RGB_BRIGHTNESS_MAX - rising;
		color.red = falling;
		color.green = rising;
	} else if (hue < 170U) {
		offset = (uint8_t)(hue - 85U);
		rising = scale_brightness(offset, 84U);
		falling = RGB_BRIGHTNESS_MAX - rising;
		color.green = falling;
		color.blue = rising;
	} else {
		offset = (uint8_t)(hue - 170U);
		rising = scale_brightness(offset, 85U);
		falling = RGB_BRIGHTNESS_MAX - rising;
		color.blue = falling;
		color.red = rising;
	}

	color.red = (rgb_brightness_t)(((uint32_t)color.red * brightness) /
				       RGB_BRIGHTNESS_MAX);
	color.green = (rgb_brightness_t)(((uint32_t)color.green * brightness) /
					 RGB_BRIGHTNESS_MAX);
	color.blue = (rgb_brightness_t)(((uint32_t)color.blue * brightness) /
					RGB_BRIGHTNESS_MAX);

	return color;
}

/**
 * @brief  将小范围数值映射到RGB驱动的16位亮度范围
 *
 * @param  value  当前亮度等级, 范围为0~max。
 * @param  max    最大亮度等级, 必须大于0。
 *
 * @return 映射后的16位逻辑亮度, 范围为0~RGB_BRIGHTNESS_MAX。
 */
static rgb_brightness_t scale_brightness(uint16_t value, uint16_t max)
{
	return (rgb_brightness_t)(((uint32_t)value * RGB_BRIGHTNESS_MAX) / max);
}

/**
 * @brief  生成平滑的呼吸亮度曲线
 *
 * @param  phase  呼吸相位, 0~255。0和255附近较暗, 128附近最亮。
 *
 * @return 经过二次缓动后的16位逻辑亮度。
 */
static rgb_brightness_t ease_brightness(uint8_t phase)
{
	uint16_t level;
	uint32_t eased;

	if (phase < 128U)
		level = (uint16_t)phase * 2U;
	else
		level = (uint16_t)(255U - phase) * 2U;

	eased = (uint32_t)level * (uint32_t)level;

	return (rgb_brightness_t)((eased * RGB_BRIGHTNESS_MAX) / 65025UL);
}

/**
 * @brief  生成带最低亮度的环境光呼吸曲线
 *
 * @param  phase  呼吸相位, 0~255。
 *
 * @return 16位逻辑亮度。最低亮度不为0, 避免呼吸底部产生熄灭顿感。
 */
static rgb_brightness_t ambient_brightness(uint8_t phase)
{
	uint32_t min_brightness = RGB_BRIGHTNESS_MAX / 8U;
	uint32_t dynamic_range = RGB_BRIGHTNESS_MAX - min_brightness;

	return (rgb_brightness_t)(min_brightness +
				  ((uint32_t)ease_brightness(phase) *
				   dynamic_range) /
					  RGB_BRIGHTNESS_MAX);
}

/**
 * @brief  将步进值规范化为至少1
 *
 * @param  step  用户配置的步进值。
 *
 * @return step非0时返回原值, step为0时返回1。
 */
static uint8_t normalized_step(uint8_t step)
{
	return step ? step : 1U;
}

/**
 * @brief  连续环境光循环, 无模式切换边界
 *
 * @param  params  演示线程参数。frame_ticks控制刷新间隔, hue_step控制
 *                 色相流动速度, breath_step控制呼吸速度。
 */
static void run_ambient_loop(const demo_thread_params_t *params)
{
	uint8_t hue = 0U;
	uint8_t breath_phase = 0U;
	uint8_t hue_step = normalized_step(params->hue_step);
	uint8_t breath_step = normalized_step(params->breath_step);

	while (1) {
		show_rgb(color_wheel(hue, ambient_brightness(breath_phase)),
			 params->frame_ticks);
		hue = (uint8_t)(hue + hue_step);
		breath_phase = (uint8_t)(breath_phase + breath_step);
	}
}

MODULE_INIT_DEFAULT(demo_thread_init, default_params);
