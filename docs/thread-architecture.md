# ThreadX 线程架构

本文档说明用户线程的组织方式、自动初始化注册机制、参数传递和线程间通信建议。

## 基本原则

每个线程使用独立的 `.c` + `.h` 文件对，放在 `user/Application/` 中。`tx_application.c` 只负责创建字节池并调用 `initcall_run()`，不直接包含或调用具体线程。

## 自动注册

线程模块通过 `MODULE_INIT()` 或 `MODULE_INIT_DEFAULT()` 把初始化函数注册到 `.initcall` section。链接脚本保留该 section，运行时由 `initcall_run()` 遍历调用。

无参初始化：

```c
void my_thread_init(void)
{
	/* create ThreadX objects */
}

MODULE_INIT(my_thread_init);
```

带默认参数初始化：

```c
static const my_thread_params_t default_params = {
	.interval_ms = 500,
};

void my_thread_init(const void *ctx)
{
	if (!ctx)
		ctx = &default_params;

	tx_thread_create(&thread, "my_thread", thread_entry, (ULONG)ctx,
			 thread_stack, THREAD_STACK_SIZE, THREAD_PRIORITY,
			 THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
}

MODULE_INIT_DEFAULT(my_thread_init, default_params);
```

## 添加新线程

1. 复制模板文件：

   ```bash
   cp user/Application/src/template_thread.c user/Application/src/your_thread.c
   cp user/Application/inc/template_thread.h user/Application/inc/your_thread.h
   ```

2. 全局替换 `template` 为 `your_thread`。
3. 修改 `THREAD_PRIORITY` 和 `THREAD_STACK_SIZE`。
4. 根据需要定义参数结构体和默认参数。
5. 在 `thread_entry()` 中实现业务逻辑。
6. 重新编译，无需修改 `tx_application.c`。

## 参数传递

`tx_thread_create()` 的入口参数类型为 `ULONG`。当前目标是 32 位 Cortex-M3，指针宽度与 `ULONG` 匹配，因此可以把静态参数结构体地址传给线程入口。外部传参时，调用者必须保证参数对象在线程生命周期内有效。

## 线程间通信

线程之间不应直接依赖对方的内部状态。优先使用 ThreadX 原语：

| 原语 | API | 用途 |
|------|-----|------|
| 信号量 | `tx_semaphore_create/get/put` | 同步、事件通知 |
| 互斥锁 | `tx_mutex_create/get/put` | 资源保护 |
| 消息队列 | `tx_queue_create/send/receive` | 数据传递 |
| 事件标志 | `tx_event_flags_create/set/get` | 多条件组合等待 |

线程循环中必须调用 `tx_thread_sleep()` 或阻塞式 ThreadX API，避免高优先级线程持续占用 CPU。
