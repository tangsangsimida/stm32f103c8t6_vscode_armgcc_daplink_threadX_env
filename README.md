# STM32F103C8T6 + ThreadX RTOS 开发环境

基于 STM32CubeMX 生成的 STM32F103C8T6 工程，集成 Eclipse ThreadX RTOS，使用 CMake + arm-none-eabi-gcc 工具链，适配 VS Code + DAPLink 调试。

## 硬件信息

| 项目 | 参数 |
|------|------|
| MCU | STM32F103C8Tx (Cortex-M3, 72MHz) |
| Flash / RAM | 64KB / 20KB |
| 封装 | LQFP48 |
| 时钟 | HSE 8MHz → PLL x9 = 72MHz SYSCLK |
| 调试接口 | SWD (PA13/SWDIO, PA14/SWCLK) |
| 时基 | TIM4 (SysTick 留给 ThreadX) |

## 快速开始

### 环境要求

- `arm-none-eabi-gcc` 工具链
- CMake 3.22+
- Ninja
- Python 3.6+ (代码格式化工具)
- clang-format 13+ (推荐 18+)

### 编译

```bash
# 配置
cmake --preset Debug
cmake --preset Release

# 编译
cmake --build build/Debug
cmake --build build/Release

# 清理
cmake --build build/Debug --target clean
```

输出文件: `build/Debug/stm32f103c8t6_vscode_armgcc_daplink_threadX_env.elf`

### 调试/烧录

VS Code 中按 `F5` 启动调试（需要 DAPLink 连接）。或使用任务:

- `Ctrl+Shift+B` — 编译、烧录并复位

### 代码格式化

```bash
python3 tools/python/format_code.py          # 格式化用户代码
python3 tools/python/format_code.py --check  # 检查模式 (CI)
python3 tools/python/format_code.py -v       # 详细输出
```

代码风格: Linux 内核规范 (Tab 缩进, 80 列宽)。

## 项目结构

```
├── CMakeLists.txt              # 根构建配置
├── CMakePresets.json           # CMake 预设 (Debug/Release)
├── .clang-format               # 代码格式化配置 (Linux 内核风格)
├── .vscode/                    # VS Code 调试/任务配置
│
├── Core/                       # CubeMX 生成代码 (不要手动修改)
│   ├── Inc/
│   └── Src/
├── Drivers/                    # STM32F1xx HAL 驱动和 CMSIS
├── lib/ThreadX/               # Eclipse ThreadX RTOS
│   ├── common/                # 平台无关内核 (185 个 .c 文件)
│   └── port/gnu/             # Cortex-M3/GNU 移植 (7 个 .S 文件)
├── cmake/                     # CMake 工具链和 CubeMX 配置
│
└── user/                      # 用户代码 (自动收集, 无需改 CMakeLists.txt)
    ├── Application/           # 业务逻辑线程
    │   ├── inc/
    │   └── src/
    ├── RTOS/                  # ThreadX 胶水代码
    │   └── src/
    │       ├── tx_application.c           # 线程编排层
    │       └── tx_initialize_low_level.c  # 底层初始化
    ├── Modules/               # 中间件应用代码
    └── Peripherals/           # 可复用硬件驱动
        └── src/
            └── rgb_led.c      # RGB LED 驱动 (PA6/PA7/PB0)
```

## 线程架构

每个线程是独立的 `.c` + `.h` 文件对，放在 `user/Application/` 中，线程之间零耦合。

`tx_application.c` 是薄编排层，只负责创建字节池并调用各线程的初始化函数。

### 添加新线程

1. 复制模板文件:
   ```bash
   cp user/Application/src/template_thread.c user/Application/src/your_thread.c
   cp user/Application/inc/template_thread.h user/Application/inc/your_thread.h
   ```

2. 全局替换 `template` → `your_thread`

3. 修改线程配置 (`THREAD_PRIORITY`, `THREAD_STACK_SIZE`)

4. 在 `thread_entry()` 中实现业务逻辑

5. 在 `tx_application.c` 中注册:
   ```c
   #include "your_thread.h"
   // 在 tx_application_define() 中:
   your_thread_init();
   ```

6. 重新编译 — 无需修改 CMakeLists.txt

### 线程间通信

线程之间通过 ThreadX 原语通信，不直接 `#include` 对方:

| 原语 | API | 用途 |
|------|-----|------|
| 信号量 | `tx_semaphore_create/get/put` | 同步、事件通知 |
| 互斥锁 | `tx_mutex_create/get/put` | 资源保护 |
| 消息队列 | `tx_queue_create/send/receive` | 数据传递 |
| 事件标志 | `tx_event_flags_create/set/get` | 多条件组合等待 |

## CubeMX 集成

`.ioc` 文件是 CubeMX 项目。重新生成代码时:

- `Core/` 目录被覆盖 (USER CODE 块内代码保留)
- `Drivers/` 被刷新
- **不会触及**: `lib/ThreadX/`, `user/`, `CMakeLists.txt`, `.vscode/`

重新生成后运行 `cmake --preset Debug` 即可，构建系统会自动修复中断处理函数冲突。

## 许可证

STM32 HAL 驱动和 CMSIS 由 STMicroelectronics 提供，遵循其许可协议。ThreadX 遵循 MIT 许可证。
