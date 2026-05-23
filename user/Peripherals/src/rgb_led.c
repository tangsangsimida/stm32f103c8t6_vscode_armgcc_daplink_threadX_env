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

void rgb_led_init(void)
{
	rgb_led_off();
	APP_HAL_CHECK(HAL_TIM_PWM_Start(&htim3, RGB_RED_CHANNEL));
	APP_HAL_CHECK(HAL_TIM_PWM_Start(&htim3, RGB_GREEN_CHANNEL));
	APP_HAL_CHECK(HAL_TIM_PWM_Start(&htim3, RGB_BLUE_CHANNEL));
}

void rgb_led_set_color(rgb_color_t color)
{
	rgb_led_set_color_brightness(color, RGB_BRIGHTNESS_MAX);
}

void rgb_led_set_rgb(rgb_brightness_t red, rgb_brightness_t green,
		     rgb_brightness_t blue)
{
	__HAL_TIM_SET_COMPARE(&htim3, RGB_RED_CHANNEL, scale_to_timer(red));
	__HAL_TIM_SET_COMPARE(&htim3, RGB_GREEN_CHANNEL, scale_to_timer(green));
	__HAL_TIM_SET_COMPARE(&htim3, RGB_BLUE_CHANNEL, scale_to_timer(blue));
}

void rgb_led_set_color_brightness(rgb_color_t color,
				  rgb_brightness_t brightness)
{
	rgb_led_set_rgb((color & RGB_COLOR_RED) ? brightness : 0U,
			(color & RGB_COLOR_GREEN) ? brightness : 0U,
			(color & RGB_COLOR_BLUE) ? brightness : 0U);
}

void rgb_led_off(void)
{
	rgb_led_set_rgb(0U, 0U, 0U);
}

static rgb_brightness_t scale_to_timer(rgb_brightness_t brightness)
{
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim3);
	uint32_t compare = ((uint32_t)brightness * period) / RGB_BRIGHTNESS_MAX;

	if (RGB_ACTIVE_LOW)
		compare = period - compare;

	return (rgb_brightness_t)compare;
}
