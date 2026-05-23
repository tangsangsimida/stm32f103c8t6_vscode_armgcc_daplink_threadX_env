/**
 * @file    demo_thread.h
 * @brief   RGB LED 演示线程接口
 */

#ifndef __DEMO_THREAD_H
#define __DEMO_THREAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  演示线程参数结构体
 *
 * 定义在头文件中, 外部模块可构造自定义参数传入 init 函数。
 */
typedef struct {
	uint32_t color_cycle_ticks; /* 颜色切换间隔(tick), 最小值 1 */
} demo_thread_params_t;

/**
 * @brief  初始化并启动 RGB LED 演示线程
 *
 * @param  params  指向 demo_thread_params_t 的指针, 传 NULL 使用默认参数。
 *                 调用者需保证 params 指向的内存在线程运行期间有效。
 *
 * 由 MODULE_INIT_DEFAULT() 自动注册(使用默认参数),
 * 也可被其他模块直接调用以传入自定义参数。
 */
void demo_thread_init(const demo_thread_params_t *params);

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_THREAD_H */
