# ThreadX 线程架构

本文档说明用户线程的组织方式、自动初始化注册机制、参数传递和线程间通信建议。

## 基本原则

每个线程使用独立的 `.c` + `.h` 文件对，放在 `user/Application/` 中。`tx_application.c` 只负责创建字节池并调用 `initcall_run()`，不直接包含或调用具体线程。

线程模板放在 `docs/examples/`，不会参与默认固件构建。复制到 `user/Application/` 后再按实际模块改名和启用。

## 命名规范

线程模块统一使用职责命名，不使用 `main_thread` 这类暗示主次关系的名字：

```text
xxx_thread.c/h
xxx_thread_init()
xxx_thread_params_t
XXX_THREAD_PRIORITY
XXX_THREAD_STACK_SIZE
```

例如当前 RGB LED 演示线程命名为 `demo_thread`，表示用途是演示，不代表它比其他线程更特殊。

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
	.interval_ticks = APP_MS_TO_TICKS(500),
};

void my_thread_init(const my_thread_params_t *params)
{
	if (!params)
		params = &default_params;

	APP_TX_CHECK(tx_thread_create(&thread, "my_thread", thread_entry,
				      (ULONG)params, thread_stack,
				      THREAD_STACK_SIZE, THREAD_PRIORITY,
				      THREAD_PRIORITY, TX_NO_TIME_SLICE,
				      TX_AUTO_START));
}

MODULE_INIT_DEFAULT(my_thread_init, default_params);
```

选择规则：无参初始化函数使用 `MODULE_INIT()`；需要默认参数的强类型初始化函数使用 `MODULE_INIT_DEFAULT()`。

## 添加新线程

1. 复制模板文件：

   ```bash
   cp docs/examples/template_thread.c user/Application/src/your_thread.c
   cp docs/examples/template_thread.h user/Application/inc/your_thread.h
   ```

2. 全局替换 `template` 为 `your_thread`。
3. 修改 `THREAD_PRIORITY` 和 `THREAD_STACK_SIZE`。
4. 根据需要定义参数结构体和默认参数。
5. 在 `thread_entry()` 中实现业务逻辑。
6. 重新编译，无需修改 `tx_application.c`。

## 优先级约定

ThreadX 优先级数字越小，调度优先级越高。本项目约定：

| 范围 | 用途 |
|------|------|
| 0-4 | 系统保留或紧急实时任务 |
| 5-9 | 实时控制类线程 |
| 10-19 | 普通业务线程 |
| 20+ | 后台或低优先级线程 |

公共宏定义在 `user/Application/inc/app_config.h`。优先级表达实时性要求，不表达线程主次关系。

## 参数传递

线程初始化函数应使用强类型参数，例如 `const my_thread_params_t *params`，不要在公共接口中暴露 `const void *`。参数结构体定义在头文件中，调用方可以类型安全地构造自定义参数。

`tx_thread_create()` 的入口参数类型为 `ULONG`。当前目标是 32 位 Cortex-M3，指针宽度与 `ULONG` 匹配，因此可以把参数结构体地址传给线程入口。外部传参时，调用者必须保证参数对象在线程生命周期内有效。

`tx_thread_sleep()` 接收的是 ThreadX tick，不是毫秒。当前底层配置为 1ms tick，但模板字段仍使用 `*_ticks` 命名，避免以后修改 tick 频率时产生误解。传入 sleep 的 tick 值至少为 1，防止线程进入无阻塞忙循环。

使用 `APP_MS_TO_TICKS(ms)` 从毫秒转换为 tick。该宏按整数除法向下取整；如果转换结果可能为 0，再通过 `APP_MIN_SLEEP_TICKS(ticks)` 防御 0 tick sleep。ThreadX API 初始化返回值使用 `APP_TX_CHECK()` 统一检查。

## 线程间通信

线程之间不应直接依赖对方的内部状态。优先使用 ThreadX 原语：

| 原语 | API | 用途 |
|------|-----|------|
| 信号量 | `tx_semaphore_create/get/put` | 同步、事件通知 |
| 互斥锁 | `tx_mutex_create/get/put` | 资源保护 |
| 消息队列 | `tx_queue_create/send/receive` | 数据传递 |
| 事件标志 | `tx_event_flags_create/set/get` | 多条件组合等待 |

线程循环中必须调用 `tx_thread_sleep()` 或阻塞式 ThreadX API，避免高优先级线程持续占用 CPU。

`docs/examples/queue_example.c/h` 提供了生产者线程和消费者线程通过 `TX_QUEUE` 传递 `ULONG` 消息的最小示例。实际模块中，队列对象通常由通信双方共同依赖的模块持有，而不是由某个线程私有持有。
