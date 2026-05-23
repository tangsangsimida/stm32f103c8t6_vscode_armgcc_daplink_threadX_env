/**
 * @file    rgb_led.h
 * @brief   RGB LED驱动头文件
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 *
 * 硬件连接:
 *   - PA6 : TIM3_CH1, RGB LED 红色通道
 *   - PA7 : TIM3_CH2, RGB LED 绿色通道
 *   - PB0 : TIM3_CH3, RGB LED 蓝色通道
 *
 * 通过TIM3 PWM控制亮度。当前CubeMX配置Period=65535。
 * 硬件为低电平点亮, 驱动内部会反向映射PWM占空比。
 */

#ifndef __RGB_LED_H
#define __RGB_LED_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * RGB LED颜色定义
 *
 * 每个通道独立控制, 通过按位或组合颜色。
 * 例如: RGB_COLOR_YELLOW = RGB_COLOR_RED | RGB_COLOR_GREEN
 * ========================================================================= */
#define RGB_COLOR_OFF 0x00
#define RGB_COLOR_RED 0x01
#define RGB_COLOR_GREEN 0x02
#define RGB_COLOR_BLUE 0x04
#define RGB_COLOR_YELLOW (RGB_COLOR_RED | RGB_COLOR_GREEN)
#define RGB_COLOR_CYAN (RGB_COLOR_GREEN | RGB_COLOR_BLUE)
#define RGB_COLOR_MAGENTA (RGB_COLOR_RED | RGB_COLOR_BLUE)
#define RGB_COLOR_WHITE (RGB_COLOR_RED | RGB_COLOR_GREEN | RGB_COLOR_BLUE)

/* 颜色类型 */
typedef uint8_t rgb_color_t;
typedef uint16_t rgb_brightness_t;

#define RGB_BRIGHTNESS_MAX 65535U

/**
 * @brief  初始化RGB LED PWM输出
 *
 * 由MX_TIM3_Init()完成PWM配置, 本函数启动三个PWM通道并确保LED熄灭。
 * 应在MX_TIM3_Init()之后调用。
 */
void rgb_led_init(void);

/**
 * @brief  设置RGB LED颜色
 * @param  color  颜色值, 可用RGB_COLOR_xxx宏或按位组合
 */
void rgb_led_set_color(rgb_color_t color);

/**
 * @brief  设置RGB三通道亮度
 * @param  red    红色亮度, 0~RGB_BRIGHTNESS_MAX
 * @param  green  绿色亮度, 0~RGB_BRIGHTNESS_MAX
 * @param  blue   蓝色亮度, 0~RGB_BRIGHTNESS_MAX
 */
void rgb_led_set_rgb(rgb_brightness_t red, rgb_brightness_t green,
		     rgb_brightness_t blue);

/**
 * @brief  按基础颜色和整体亮度设置RGB LED
 * @param  color       颜色掩码, 可用RGB_COLOR_xxx宏或按位组合
 * @param  brightness  整体亮度, 0~RGB_BRIGHTNESS_MAX
 */
void rgb_led_set_color_brightness(rgb_color_t color,
				  rgb_brightness_t brightness);

/**
 * @brief  关闭RGB LED(所有通道熄灭)
 */
void rgb_led_off(void);

#ifdef __cplusplus
}
#endif

#endif /* __RGB_LED_H */
