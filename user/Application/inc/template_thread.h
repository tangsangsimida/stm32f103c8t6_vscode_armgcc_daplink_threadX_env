/**
 * @file    template_thread.h
 * @brief   线程模板头文件 — 复制本文件并替换 "template" 为你的线程名
 */

#ifndef __TEMPLATE_THREAD_H
#define __TEMPLATE_THREAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  线程参数结构体 — 根据实际需求修改字段
 *
 * 定义在头文件中, 外部模块可构造自定义参数传入 init 函数。
 */
typedef struct {
	uint32_t interval_ms; /* 循环间隔(ms), 最小值 1 */
	uint32_t flags; /* 功能标志 */
	void *user_data; /* 附加数据指针(可指向任意类型) */
} template_thread_params_t;

/**
 * @brief  初始化并启动 template 线程
 *
 * @param  ctx  指向 template_thread_params_t 的指针, 传 NULL 使用默认参数。
 *              调用者需保证 ctx 指向的内存在线程运行期间有效。
 *
 * 由 MODULE_INIT_DEFAULT() 自动注册(使用默认参数),
 * 也可被其他模块直接调用以传入自定义参数。
 */
void template_thread_init(const void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __TEMPLATE_THREAD_H */
