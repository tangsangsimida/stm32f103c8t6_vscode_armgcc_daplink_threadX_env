#!/bin/bash
# ============================================================================
# CubeMX + ThreadX 兼容性修复脚本
#
# CubeMX重新生成代码后, stm32f1xx_it.c中的PendSV_Handler和SysTick_Handler
# 会变成强定义, 与ThreadX的强定义冲突导致多重定义错误。
#
# 本脚本删除这两个函数定义, 让ThreadX的版本通过链接器自动接管。
#
# 用法: ./tools/fix_cubemx_threadx.sh
# ============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
IT_FILE="$PROJECT_DIR/Core/Src/stm32f1xx_it.c"

if [ ! -f "$IT_FILE" ]; then
    echo "错误: 找不到 $IT_FILE"
    exit 1
fi

# 检查是否存在需要删除的函数
if grep -q "^void PendSV_Handler" "$IT_FILE" || grep -q "^void SysTick_Handler" "$IT_FILE"; then
    # 删除PendSV_Handler和SysTick_Handler函数定义(从void到闭合大括号)
    sed -i '/^void PendSV_Handler/,/^}/d' "$IT_FILE"
    sed -i '/^void SysTick_Handler/,/^}/d' "$IT_FILE"
    echo "已修复: 删除了PendSV_Handler和SysTick_Handler的强定义"
    echo "ThreadX的版本将在链接时自动接管"
else
    echo "无需修复: PendSV_Handler和SysTick_Handler已不存在于stm32f1xx_it.c"
fi
