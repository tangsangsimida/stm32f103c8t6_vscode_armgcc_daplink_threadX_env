/**
 * @file    rgb_led.h
 * @brief   RGB LED驱动头文件
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 *
 * 硬件连接:
 *   - PA6 : RGB LED 红色通道
 *   - PA7 : RGB LED 绿色通道
 *   - PB0 : RGB LED 蓝色通道
 *
 * 有效电平: 低电平点亮(GPIO_PIN_RESET), 高电平熄灭(GPIO_PIN_SET)
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

/**
 * @brief  初始化RGB LED引脚
 *
 * 由MX_GPIO_Init()自动完成引脚配置, 本函数确保LED初始状态为熄灭。
 * 应在MX_GPIO_Init()之后调用。
 */
void rgb_led_init(void);

/**
 * @brief  设置RGB LED颜色
 * @param  color  颜色值, 可用RGB_COLOR_xxx宏或按位组合
 */
void rgb_led_set_color(rgb_color_t color);

/**
 * @brief  关闭RGB LED(所有通道熄灭)
 */
void rgb_led_off(void);

/**
 * @brief  设置单个通道状态
 * @param  pin_port  GPIO端口(如GPIOA)
 * @param  pin       GPIO引脚(如GPIO_PIN_6)
 * @param  state     GPIO_PIN_SET(熄灭) 或 GPIO_PIN_RESET(点亮)
 */
void rgb_led_set_channel(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);

#ifdef __cplusplus
}
#endif

#endif /* __RGB_LED_H */
