/**
 * @file    rgb_led.c
 * @brief   RGB LED驱动实现
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#include "rgb_led.h"

/* =========================================================================
 * RGB LED硬件引脚定义
 *
 * 与CubeMX中MX_GPIO_Init()的配置保持一致。
 * 修改引脚时只需更新此处, 无需修改其他文件。
 * ========================================================================= */
#define RGB_RED_PORT    GPIOA
#define RGB_RED_PIN     GPIO_PIN_6

#define RGB_GREEN_PORT  GPIOA
#define RGB_GREEN_PIN   GPIO_PIN_7

#define RGB_BLUE_PORT   GPIOB
#define RGB_BLUE_PIN    GPIO_PIN_0

/* 有效电平: 低电平点亮 */
#define RGB_ON          GPIO_PIN_RESET
#define RGB_OFF         GPIO_PIN_SET

void rgb_led_init(void)
{
    /* MX_GPIO_Init()已配置引脚为推挽输出, 此处确保初始状态为熄灭 */
    rgb_led_off();
}

void rgb_led_set_color(rgb_color_t color)
{
    HAL_GPIO_WritePin(RGB_RED_PORT,   RGB_RED_PIN,   (color & RGB_COLOR_RED)   ? RGB_ON : RGB_OFF);
    HAL_GPIO_WritePin(RGB_GREEN_PORT, RGB_GREEN_PIN, (color & RGB_COLOR_GREEN) ? RGB_ON : RGB_OFF);
    HAL_GPIO_WritePin(RGB_BLUE_PORT,  RGB_BLUE_PIN,  (color & RGB_COLOR_BLUE)  ? RGB_ON : RGB_OFF);
}

void rgb_led_off(void)
{
    HAL_GPIO_WritePin(RGB_RED_PORT,   RGB_RED_PIN,   RGB_OFF);
    HAL_GPIO_WritePin(RGB_GREEN_PORT, RGB_GREEN_PIN, RGB_OFF);
    HAL_GPIO_WritePin(RGB_BLUE_PORT,  RGB_BLUE_PIN,  RGB_OFF);
}

void rgb_led_set_channel(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state)
{
    HAL_GPIO_WritePin(port, pin, state);
}
