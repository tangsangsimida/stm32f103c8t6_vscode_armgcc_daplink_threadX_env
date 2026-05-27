# STM32F103C8T6 + ThreadX RTOS 开发环境

基于 STM32CubeMX 生成的 STM32F103C8T6 工程，集成 Eclipse ThreadX RTOS，使用 CMake + `arm-none-eabi-gcc` 工具链，适配 VS Code + DAPLink 调试。

## 项目概览

| 项目 | 参数 |
|------|------|
| MCU | STM32F103C8Tx (Cortex-M3, 72MHz) |
| Flash / RAM | 64KB / 20KB |
| 时钟 | HSE 8MHz -> PLL x9 = 72MHz SYSCLK |
| 调试接口 | SWD (PA13/SWDIO, PA14/SWCLK) |
| 时基 | TIM4 用于 HAL，SysTick 留给 ThreadX |

## 快速开始

先安装 `arm-none-eabi-gcc`、CMake 3.22+、Ninja、Python 3 和 `clang-format`。完整安装说明见 [开发环境配置](docs/development.md)。

```bash
cmake --preset Debug
cmake --build build/Debug
```

输出文件位于：

```text
build/Debug/stm32f103c8t6_vscode_armgcc_daplink_threadX_env.elf
```

VS Code 中按 `F5` 启动 DAPLink 调试；命令行烧录和 OpenOCD 配置见 [开发环境配置](docs/development.md)。

## 常用命令

```bash
cmake --preset Debug
cmake --preset Release
cmake --build build/Debug
cmake --build build/Release
cmake --build build/Debug --target clean

python3 tools/python/format_code.py
python3 tools/python/format_code.py --check
tools/check.sh
```

## 目录入口

```text
Core/                 CubeMX 生成代码
Drivers/              STM32F1xx HAL 和 CMSIS
lib/ThreadX/          ThreadX 内核与 Cortex-M3 GNU 移植
cmake/                工具链和 CubeMX CMake 配置
user/                 手写应用、RTOS 胶水和外设驱动
tools/                项目脚本
docs/                 详细项目文档
```

## 文档

- [开发环境配置](docs/development.md)：支持的系统、工具链、构建、烧录和格式化命令。
- [项目结构与 CubeMX 集成](docs/project-structure.md)：目录职责、CubeMX 修改边界、内存占用和重新生成注意事项。
- [ThreadX 线程架构](docs/thread-architecture.md)：线程模板、自动注册、优先级约定、强类型参数传递和线程间通信。
- [Arch Linux 环境配置](docs/archlinux-setup.md)：Arch Linux 下的工具链安装、构建、烧录和 udev 规则。
- [示例代码](docs/examples/README.md)：不参与构建的线程模板和队列通信示例。

## 许可证

STM32 HAL 驱动和 CMSIS 由 STMicroelectronics 提供，遵循其许可协议。ThreadX 遵循 MIT 许可证。
