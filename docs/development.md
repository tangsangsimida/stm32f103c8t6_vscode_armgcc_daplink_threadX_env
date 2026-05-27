# 开发环境配置

本文档记录本项目的主机环境、构建、烧录和格式化命令。README 仅保留最小入口信息。

## 支持的操作系统

| 操作系统 | 状态 | 说明 |
|----------|------|------|
| Linux (Ubuntu 22.04+) | 推荐 | 主要开发和测试环境 |
| macOS (12+) | 支持 | 通过 Homebrew 安装工具链 |
| Windows (10/11) | 支持 | 使用 Arm GNU Toolchain for Windows |

## 必需工具

| 工具 | 版本 | 用途 |
|------|------|------|
| `arm-none-eabi-gcc` | 13.x+ 推荐 | ARM 交叉编译、链接、调试工具 |
| CMake | 3.22+ | 生成 Ninja 构建系统 |
| Ninja | 1.10+ | 实际执行构建 |
| Python | 3.6+ | 运行项目工具脚本 |
| `clang-format` | 13+，推荐 18+ | 格式化用户代码 |
| OpenOCD | 当前发行版 | DAPLink 烧录和调试 |

Linux (Ubuntu/Debian) 示例：

```bash
sudo apt install gcc-arm-none-eabi cmake ninja-build python3 clang-format openocd
```

Linux (Arch Linux) 示例：

```bash
sudo pacman -S arm-none-eabi-gcc arm-none-eabi-newlib cmake ninja python clang openocd
```

Arch Linux 用户的详细配置说明见 [Arch Linux 环境配置指南](archlinux-setup.md)。

macOS 示例：

```bash
brew install cmake ninja python3 clang-format open-ocd
brew install --cask gcc-arm-embedded
```

Windows 需要安装 Arm GNU Toolchain for Windows，并确保 `arm-none-eabi-gcc` 在 `PATH` 中。CMake、Ninja、Python、LLVM/clang-format 和 OpenOCD 可通过官网安装包或 `winget` 安装。MSYS2 只是可选 shell/包管理环境，不是本项目要求的交叉编译工具链。

## 构建

```bash
cmake --preset Debug
cmake --preset Release

cmake --build build/Debug
cmake --build build/Release
cmake --build build/Debug --target clean
```

默认工具链文件为 `cmake/gcc-arm-none-eabi.cmake`。构建后会生成 ELF、HEX 和 BIN 文件。

## 烧录与调试

VS Code 中按 `F5` 启动调试，需要连接调试探针。SWD 连接为 PA13/SWDIO、PA14/SWCLK、GND 和 3.3V。

命令行烧录（WCH-Link 探针，需指定 VID/PID）：

```bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg \
  -c "cmsis_dap_vid_pid 0x1a86 0x8012" \
  -c "program build/Debug/stm32f103c8t6_vscode_armgcc_daplink_threadX_env.elf verify reset exit"
```

> 首次使用 WCH-Link 需要配置 udev 规则，详见 [Arch Linux 环境配置指南](archlinux-setup.md#udev-规则)。

## 代码格式化

项目使用 `.clang-format`，风格接近 Linux 内核规范：Tab 缩进、宽度 8、80 列限制。

```bash
python3 tools/python/format_code.py
python3 tools/python/format_code.py --check
python3 tools/python/format_code.py -v
```

## 提交前检查

运行本地检查脚本，确认格式、CMake 配置和 Debug 构建都通过：

```bash
tools/check.sh
```

该脚本不会烧录硬件；涉及外设行为的改动仍需在目标板上做 smoke test。
