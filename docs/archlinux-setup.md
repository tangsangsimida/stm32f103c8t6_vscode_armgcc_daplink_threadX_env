# Arch Linux 开发环境配置指南

本文档记录在 Arch Linux 上搭建本项目开发环境的完整流程。

## 安装依赖

一条命令安装所有必需工具：

```bash
sudo pacman -S arm-none-eabi-gcc arm-none-eabi-newlib cmake ninja python clang openocd
```

各工具用途：

| 包名 | 提供的工具 | 用途 |
|------|-----------|------|
| `arm-none-eabi-gcc` | `arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, `arm-none-eabi-size` | ARM 交叉编译、链接 |
| `arm-none-eabi-newlib` | C 标准库 | 嵌入式 C 运行时 |
| `cmake` | `cmake` | 构建系统生成器 (需 3.22+) |
| `ninja` | `ninja` | 实际执行构建 |
| `python` | `python3` | 格式化脚本、CubeMX 兼容性修复脚本 |
| `clang` | `clang-format` | 代码格式化 (Linux 内核风格) |
| `openocd` | `openocd` | DAPLink 烧录与调试 |

## 验证安装

```bash
arm-none-eabi-gcc --version    # 应显示 13.x+
cmake --version                # 应显示 3.22+
ninja --version                # 应显示 1.10+
python3 --version              # 应显示 3.6+
clang-format --version         # 应显示 13+
openocd --version              # 应显示 0.12+
```

## 构建项目

```bash
cmake --preset Debug
cmake --build build/Debug
```

输出文件：

```text
build/Debug/stm32f103c8t6_vscode_armgcc_daplink_threadX_env.elf
build/Debug/stm32f103c8t6_vscode_armgcc_daplink_threadX_env.hex
build/Debug/stm32f103c8t6_vscode_armgcc_daplink_threadX_env.bin
```

构建结束会打印 RAM/FLASH 使用量。

## 代码格式化

项目使用 `.clang-format`，风格为 Linux 内核规范（Tab 缩进、宽度 8、80 列）。

```bash
python3 tools/python/format_code.py          # 格式化 user/ 下所有手写代码
python3 tools/python/format_code.py --check   # 仅检查，不修改
```

## 烧录与调试

### WCH-Link 探针配置

本项目使用 WCH-Link (VID:PID `1a86:8012`) 作为调试探针。OpenOCD 标准 `cmsis-dap.cfg` 默认不识别此设备，需要手动指定 VID/PID：

```bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg \
  -c "cmsis_dap_vid_pid 0x1a86 0x8012" \
  -c "program build/Debug/stm32f103c8t6_vscode_armgcc_daplink_threadX_env.elf verify reset exit"
```

### udev 规则

WCH-Link 默认需要 root 权限访问。添加 udev 规则使普通用户可直接烧录：

```bash
sudo bash -c 'echo "SUBSYSTEM==\"usb\", ATTR{idVendor}==\"1a86\", ATTR{idProduct}==\"8012\", MODE=\"0666\"" > /etc/udev/rules.d/99-wch-link.rules'
sudo udevadm control --reload-rules
sudo udevadm trigger
```

> Arch Linux 默认没有 `plugdev` 组，使用 `MODE="0666"` 即可让所有用户访问。

SWD 接线：PA13/SWDIO, PA14/SWCLK, GND, 3.3V。

### VS Code 调试

按 `F5` 启动调试（需安装 Cortex-Debug 扩展）。需在 `.vscode/launch.json` 中配置 WCH-Link 的 VID/PID。

## 提交前检查

```bash
tools/check.sh
```

该脚本依次执行格式检查、CMake 配置和 Debug 构建，不会烧录硬件。
