#!/bin/bash
# ============================================================================
# CubeMX + ThreadX 兼容性修复脚本
#
# CubeMX重新生成代码后, stm32f1xx_it.c中的PendSV_Handler和SysTick_Handler
# 会变成强定义, 与ThreadX的强定义冲突导致多重定义错误。
#
# 本脚本删除这两个函数定义及其注释块, 让ThreadX的版本通过链接器自动接管。
#
# 用法: bash tools/fix_cubemx_threadx.sh
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
    python3 -c "
import re, sys

with open('$IT_FILE', 'r') as f:
    content = f.read()

# 删除PendSV_Handler: 从/**注释块到函数闭合大括号
content = re.sub(
    r'/\*\*\s*\n\s*\*\s*@brief\s+This function handles Pendable request.*?\n.*?^}',
    '',
    content,
    flags=re.DOTALL | re.MULTILINE
)

# 删除SysTick_Handler: 从/**注释块到函数闭合大括号
content = re.sub(
    r'/\*\*\s*\n\s*\*\s*@brief\s+This function handles System tick timer.*?\n.*?^}',
    '',
    content,
    flags=re.DOTALL | re.MULTILINE
)

# 清理连续空行
content = re.sub(r'\n{3,}', '\n\n', content)

with open('$IT_FILE', 'w') as f:
    f.write(content)
"
    echo "已修复: 删除了PendSV_Handler和SysTick_Handler的强定义"
    echo "ThreadX的版本将在链接时自动接管"
    exit 0
else
    echo "无需修复: PendSV_Handler和SysTick_Handler已不存在于stm32f1xx_it.c"
    exit 0
fi
