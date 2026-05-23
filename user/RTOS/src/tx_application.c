/**
 * @file    tx_application.c
 * @brief   ThreadX应用入口 — 线程编排层
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 *
 * =============================================================================
 * [说明] 本文件只负责创建字节池并启动自动初始化。
 *        每个模块使用 MODULE_INIT(fn) 宏自动注册初始化函数,
 *        无需在本文件中手动 #include 或调用。
 *
 *        添加新线程步骤:
 *          1. 复制 template_thread.c/h → your_thread.c/h
 *          2. 全局替换 template → your_thread
 *          3. 在 your_thread.c 中使用 MODULE_INIT(your_thread_init);
 *          4. 重新编译即可, 无需修改本文件
 *
 * [移植说明] 本文件与MCU无关, 更换MCU时无需修改。
 * =============================================================================
 */

#include "tx_api.h"
#include "init.h"

/* 字节池配置 */
#define BYTE_POOL_SIZE 4096

static TX_BYTE_POOL byte_pool;
static UCHAR byte_pool_area[BYTE_POOL_SIZE] __attribute__((aligned(4)));

/**
 * @brief  ThreadX应用定义函数
 *
 * 由 tx_kernel_enter() 在 _tx_initialize_low_level() 完成后调用。
 * 在此创建字节池并自动调用所有 MODULE_INIT() 注册的初始化函数。
 *
 * 注意: 不要从本函数返回。ThreadX调度器在本函数完成后启动。
 */
void tx_application_define(VOID *first_free_memory)
{
	(void)first_free_memory;

	/* 创建字节池 */
	tx_byte_pool_create(&byte_pool, "app_byte_pool", byte_pool_area,
			    BYTE_POOL_SIZE);

	/* 自动调用所有 MODULE_INIT() 注册的初始化函数 */
	initcall_run();
}
