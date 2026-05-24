/**
 * @file    rgb_led.c
 * @brief   RGB LED PWM驱动实现
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#include "rgb_led.h"
#include "app_config.h"
#include "tim.h"

#define RGB_RED_CHANNEL TIM_CHANNEL_1
#define RGB_GREEN_CHANNEL TIM_CHANNEL_2
#define RGB_BLUE_CHANNEL TIM_CHANNEL_3
#define RGB_ACTIVE_LOW 1

static rgb_brightness_t scale_to_timer(rgb_brightness_t brightness);

/**
 * @brief  启动TIM3三个PWM通道, 初始亮度为关闭
 *
 * @note   调用前必须已完成MX_TIM3_Init()。本函数会先写入关闭占空比,
 *         再启动TIM3_CH1/CH2/CH3, 避免PWM通道启动瞬间闪烁。
 */
void rgb_led_init(void)
{
	/* 先写入关闭占空比, 再启动PWM, 避免通道启动瞬间闪烁。 */
	rgb_led_off();
	APP_HAL_CHECK(HAL_TIM_PWM_Start(&htim3, RGB_RED_CHANNEL));
	APP_HAL_CHECK(HAL_TIM_PWM_Start(&htim3, RGB_GREEN_CHANNEL));
	APP_HAL_CHECK(HAL_TIM_PWM_Start(&htim3, RGB_BLUE_CHANNEL));
}

/**
 * @brief  以最大亮度显示离散RGB颜色
 *
 * @param  color  RGB颜色掩码, 可使用RGB_COLOR_xxx或按位组合。
 */
void rgb_led_set_color(rgb_color_t color)
{
	rgb_led_set_color_brightness(color, RGB_BRIGHTNESS_MAX);
}

/**
 * @brief  直接设置三个PWM通道的逻辑亮度
 *
 * @param  red    红色通道逻辑亮度, 0为熄灭, RGB_BRIGHTNESS_MAX为最亮。
 * @param  green  绿色通道逻辑亮度, 0为熄灭, RGB_BRIGHTNESS_MAX为最亮。
 * @param  blue   蓝色通道逻辑亮度, 0为熄灭, RGB_BRIGHTNESS_MAX为最亮。
 */
void rgb_led_set_rgb(rgb_brightness_t red, rgb_brightness_t green,
		     rgb_brightness_t blue)
{
	__HAL_TIM_SET_COMPARE(&htim3, RGB_RED_CHANNEL, scale_to_timer(red));
	__HAL_TIM_SET_COMPARE(&htim3, RGB_GREEN_CHANNEL, scale_to_timer(green));
	__HAL_TIM_SET_COMPARE(&htim3, RGB_BLUE_CHANNEL, scale_to_timer(blue));
}

/**
 * @brief  按颜色掩码和统一亮度设置多个通道
 *
 * @param  color       RGB颜色掩码, 未包含的通道保持熄灭。
 * @param  brightness  被点亮通道的逻辑亮度, 0为熄灭,
 *                     RGB_BRIGHTNESS_MAX为最亮。
 */
void rgb_led_set_color_brightness(rgb_color_t color,
				  rgb_brightness_t brightness)
{
	rgb_led_set_rgb((color & RGB_COLOR_RED) ? brightness : 0U,
			(color & RGB_COLOR_GREEN) ? brightness : 0U,
			(color & RGB_COLOR_BLUE) ? brightness : 0U);
}

/**
 * @brief  关闭所有颜色通道
 *
 * @note   对低电平点亮硬件, 本函数会将TIM比较值设置为Period。
 */
void rgb_led_off(void)
{
	rgb_led_set_rgb(0U, 0U, 0U);
}

/**
 * @brief  将逻辑亮度映射到TIM比较值
 *
 * 硬件为低电平点亮, 因此逻辑亮度越高, CCR值越低。
 *
 * @param  brightness  逻辑亮度, 0为熄灭, RGB_BRIGHTNESS_MAX为最亮。
 *
 * @return 写入TIM CCR寄存器的比较值。
 */
static rgb_brightness_t scale_to_timer(rgb_brightness_t brightness)
{
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim3);
	uint32_t compare = ((uint32_t)brightness * period) / RGB_BRIGHTNESS_MAX;

	if (RGB_ACTIVE_LOW)
		compare = period - compare;

	return (rgb_brightness_t)compare;
}
