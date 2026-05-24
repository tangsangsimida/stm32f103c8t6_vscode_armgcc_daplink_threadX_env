/**
 * @file    app_config.h
 * @brief   应用层公共配置、检查宏和 ThreadX 辅助宏
 */

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#include "tx_api.h"
#include "main.h"
#include <stdint.h>

/**
 * @brief  ThreadX tick频率
 *
 * ThreadX tick 频率由 user/RTOS/src/tx_initialize_low_level.c 中的
 * SYSTICK_FREQ_HZ 决定。修改底层 tick 频率时同步更新此值。
 */
#define APP_TICKS_PER_SECOND 1000UL

/**
 * @brief  将毫秒转换为ThreadX tick, 使用整数除法向下取整
 *
 * @param  ms  毫秒数。
 *
 * @return 对应的ThreadX tick数量。小于1 tick的结果可能为0。
 */
#define APP_MS_TO_TICKS(ms) (((ULONG)(ms) * APP_TICKS_PER_SECOND) / 1000UL)

/**
 * @brief  将sleep间隔限制为至少1 tick, 避免线程忙循环
 *
 * @param  ticks  原始ThreadX tick数量。
 *
 * @return ticks非0时返回原值, ticks为0时返回1。
 */
#define APP_MIN_SLEEP_TICKS(ticks) ((ticks) ? (ticks) : 1UL)

/**
 * @brief  检查ThreadX API返回值, 失败时进入Error_Handler()
 *
 * @param  expr  返回UINT状态码的ThreadX API调用表达式。
 */
#define APP_TX_CHECK(expr)                \
	do {                              \
		if ((expr) != TX_SUCCESS) \
			Error_Handler();  \
	} while (0)

/**
 * @brief  检查HAL API返回值, 失败时进入Error_Handler()
 *
 * @param  expr  返回HAL_StatusTypeDef状态码的HAL API调用表达式。
 */
#define APP_HAL_CHECK(expr)              \
	do {                             \
		if ((expr) != HAL_OK)    \
			Error_Handler(); \
	} while (0)

/**
 * @brief  应用线程优先级范围约定
 *
 * ThreadX 优先级数字越小，调度优先级越高。
 * 0-4   : 系统保留或紧急实时任务
 * 5-9   : 实时控制类线程
 * 10-19 : 普通业务线程
 * 20+   : 后台或低优先级线程
 */
#define APP_PRIO_SYSTEM_MIN 0U
#define APP_PRIO_SYSTEM_MAX 4U
#define APP_PRIO_REALTIME_MIN 5U
#define APP_PRIO_REALTIME_MAX 9U
#define APP_PRIO_NORMAL_MIN 10U
#define APP_PRIO_NORMAL_MAX 19U
#define APP_PRIO_BACKGROUND_MIN 20U

#endif /* __APP_CONFIG_H */
