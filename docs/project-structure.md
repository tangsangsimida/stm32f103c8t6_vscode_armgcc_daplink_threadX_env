# 项目结构与 CubeMX 集成

本文档说明目录职责、用户代码放置规则，以及 CubeMX 重新生成代码后的注意事项。

## 顶层目录

```text
CMakeLists.txt        根构建配置
CMakePresets.json     Debug/Release 构建预设
STM32F103XX_FLASH.ld   链接脚本
Core/                 CubeMX 生成的启动、GPIO、中断和 HAL MSP 代码
Drivers/              STM32F1xx HAL 驱动和 CMSIS
lib/ThreadX/          Eclipse ThreadX RTOS
cmake/                工具链和 CubeMX 生成的 CMake 配置
user/                 手写业务代码
tools/                维护脚本
docs/                 项目文档
docs/examples/        不参与构建的代码模板
```

## 用户代码组织

`user/CMakeLists.txt` 会递归收集 `user/` 下的源文件和头文件，添加新文件通常不需要修改 CMake。

```text
user/
├── Application/       业务逻辑线程
│   ├── inc/
│   └── src/
├── RTOS/              ThreadX 初始化和应用入口
│   ├── inc/
│   └── src/
├── Modules/           中间件应用代码
└── Peripherals/       可复用外设驱动
    ├── inc/
    └── src/
```

新增 `.c` / `.cpp` 文件放入对应层级的 `src/`，新增 `.h` / `.hpp` 文件放入 `inc/`。子目录会被递归处理。

## 修改边界

- `Core/`：CubeMX 生成区，只在 `USER CODE BEGIN/END` 块内保留手写代码。
- `Drivers/`：ST HAL 和 CMSIS，不直接手改。
- `cmake/stm32cubemx/`：CubeMX 生成的构建片段，重新生成后可被覆盖。
- `user/`：主要开发区，业务线程、RTOS 胶水、模块和外设驱动都放这里。
- `docs/examples/`：复制模板，不会被 CMake 自动编译。

## ThreadX 目录

`lib/ThreadX/common/` 保存平台无关内核源码，`lib/ThreadX/port/gnu/` 保存 Cortex-M3/GNU 移植层。切换 ARM 内核时，需要同步检查 ThreadX port、工具链参数和链接脚本。

## CubeMX 重新生成

`.ioc` 文件是 CubeMX 工程。重新生成后通常会刷新：

- `Core/`
- `Drivers/`
- `cmake/stm32cubemx/`

手写代码应优先放在 `user/`。如果必须修改 `Core/`，放在 CubeMX `USER CODE BEGIN/END` 块内。

CubeMX 可能重新写入 `PendSV_Handler` 和 `SysTick_Handler`，与 ThreadX 强定义冲突。运行 `cmake --preset Debug` 时，根 CMake 会调用 `tools/fix_cubemx_threadx.sh` 自动修复。

## 内存占用

STM32F103C8T6 只有 20KB RAM。每个 ThreadX 线程栈都是静态数组，会直接占用 RAM；空模板线程不应进入默认构建。构建结束时 CMake 会输出 RAM/FLASH 使用量，必要时结合 `build/Debug/*.map` 进一步定位占用来源。

新增线程时先保守设置栈大小，完成硬件验证后再根据实际调用深度调整。优先复用队列、事件标志等 RTOS 原语，不要为简单周期任务创建过多线程。
