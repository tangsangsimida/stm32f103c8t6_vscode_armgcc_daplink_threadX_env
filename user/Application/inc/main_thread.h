/**
 * @file    main_thread.h
 * @brief   主应用线程接口
 */

#ifndef __MAIN_THREAD_H
#define __MAIN_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  初始化并启动主应用线程
 *
 * 由 tx_application_define() 调用。线程在函数内部自动创建并启动。
 */
void main_thread_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_THREAD_H */
