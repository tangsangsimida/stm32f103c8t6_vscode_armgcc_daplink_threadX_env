/**
 * @file    tx_application.c
 * @brief   ThreadX应用入口 — 线程编排层
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 *
 * =============================================================================
 * [说明] 本文件只负责创建字节池和调用各线程的初始化函数。
 *        每个线程的实现位于独立的 Application/src/xxx_thread.c 文件中。
 *        添加新线程只需:
 *          1. 创建 Application/src/xxx_thread.c 和 Application/inc/xxx_thread.h
 *          2. 在下方 #include 并调用 xxx_thread_init()
 *          3. 重新编译, 无需修改 CMakeLists.txt
 *
 * [移植说明] 本文件与MCU无关, 更换MCU时无需修改。
 * =============================================================================
 */

#include "tx_api.h"

/* 线程头文件 — 添加新线程时在此 #include */
#include "main_thread.h"
/* #include "your_thread.h" */

/* 字节池配置 */
#define BYTE_POOL_SIZE  4096

static TX_BYTE_POOL byte_pool;
static UCHAR byte_pool_area[BYTE_POOL_SIZE] __attribute__((aligned(4)));

/**
 * @brief  ThreadX应用定义函数
 *
 * 由 tx_kernel_enter() 在 _tx_initialize_low_level() 完成后调用。
 * 在此创建字节池并调用各线程的初始化函数。
 *
 * 注意: 不要从本函数返回。ThreadX调度器在本函数完成后启动。
 */
void tx_application_define(VOID *first_free_memory)
{
	(void)first_free_memory;

	/* 创建字节池 */
	tx_byte_pool_create(&byte_pool, "app_byte_pool",
			    byte_pool_area, BYTE_POOL_SIZE);

	/* 注册所有线程 — 每个线程自行创建ThreadX对象 */
	main_thread_init();
	/* your_thread_init(); */
}
