/**
 * @file    init.h
 * @brief   Linux内核风格自动初始化注册宏
 *
 * 原理: MODULE_INIT(fn) 将函数指针放入链接器 .initcall section,
 *       initcall_run() 遍历该 section 逐个调用, 实现自动注册。
 *
 * 用法:
 *   1. 在模块源文件中包含本头文件
 *   2. 在文件作用域(函数外)使用 MODULE_INIT(your_init_func);
 *   3. 无需在 tx_application.c 中手动 #include 或调用
 *
 * 本文件不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 */

#ifndef __INIT_H
#define __INIT_H

/**
 * @brief  自动注册无参初始化函数
 *
 * 放在文件作用域(函数外), 将 fn 的函数指针放入 .initcall section。
 * 链接后, initcall_run() 会自动调用 fn。
 */
#define MODULE_INIT(fn)                                       \
	static void __initcall_wrapper_##fn(void)             \
	{                                                     \
		fn();                                         \
	}                                                     \
	static void (*__initcall_##fn)(void)                  \
		__attribute__((used, section(".initcall"))) = \
			__initcall_wrapper_##fn

/**
 * @brief  自动注册带默认参数的初始化函数
 *
 * 用于 xxx_thread_init(const void *ctx) 签名的函数。
 * 自动生成 wrapper, 调用时传入 &default_params 作为默认参数。
 *
 * 用法:
 *   static const my_params_t default_params = { ... };
 *   void my_thread_init(const void *ctx) { ... }
 *   MODULE_INIT_DEFAULT(my_thread_init, default_params);
 *
 * @param  fn    初始化函数名, 签名须为 void fn(const void *ctx)
 * @param  def   默认参数变量名(须为 static 生命周期)
 */
#define MODULE_INIT_DEFAULT(fn, def)                          \
	static void __initcall_wrapper_##fn(void)             \
	{                                                     \
		fn(&(def));                                   \
	}                                                     \
	static void (*__initcall_##fn)(void)                  \
		__attribute__((used, section(".initcall"))) = \
			__initcall_wrapper_##fn

/**
 * @brief  initcall section 边界符号 (由链接器脚本 STM32F103XX_FLASH.ld 定义)
 */
extern void (*__initcall_start[])(void);
extern void (*__initcall_end[])(void);

/**
 * @brief  遍历并调用所有注册的初始化函数
 *
 * 在 tx_application_define() 中调用。
 */
static inline void initcall_run(void)
{
	void (**fn)(void);
	for (fn = __initcall_start; fn < __initcall_end; fn++)
		(*fn)();
}

#endif /* __INIT_H */
