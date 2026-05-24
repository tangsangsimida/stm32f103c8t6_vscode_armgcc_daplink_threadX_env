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

## 注释规范

`user/` 中手写代码使用轻量 Doxygen 风格注释，目标是说明模块意图、接口契约和硬件/RTOS 约束，不重复描述显而易见的语句。

- 每个 `.c` / `.h` 文件顶部必须包含 `@file` 和 `@brief`。
- 头文件中的公开类型、宏和函数必须说明用途；函数至少写 `@brief`，有参数时逐一写 `@param`，有返回值时写 `@return`。
- `.c` 文件中的私有 helper 只有在行为、单位、硬件约束或算法不直观时才加注释；一旦写函数头注释，也必须补齐 `@param` / `@return`。
- ThreadX 相关注释应明确单位和生命周期，例如 tick 不是 ms、参数对象必须在线程生命周期内有效。
- 硬件驱动注释应写明外设通道、有效电平、CubeMX 配置依赖和是否会被重新生成覆盖。
- 避免逐行注释和空泛注释，例如“设置变量”“调用函数”；让函数名和局部变量名承担这类说明。

示例：

```c
/**
 * @brief  按基础颜色和整体亮度设置RGB LED
 * @param  color       颜色掩码, 可用RGB_COLOR_xxx宏或按位组合
 * @param  brightness  整体亮度, 0~RGB_BRIGHTNESS_MAX
 */
void rgb_led_set_color_brightness(rgb_color_t color,
				  rgb_brightness_t brightness);
```

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
