cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# ============================================================================
# [移植说明] 更换MCU时需要修改的内容:
#   1. CMAKE_PROJECT_NAME  - 项目名称
#   2. cmake/gcc-arm-none-eabi.cmake - 工具链文件中的TARGET_FLAGS(如-mcpu)
#   3. STM32F103XX_FLASH.ld - 链接脚本(内存区域大小和起始地址)
#   4. lib/ThreadX/port/ 目录 - 更换为对应ARM内核的ThreadX移植目录
# ============================================================================

# 编译器设置
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# [移植说明] 修改为你的项目名称
set(CMAKE_PROJECT_NAME stm32f103c8t6_vscode_armgcc_daplink_threadX_env)

# 生成compile_commands.json, 供clangd等工具使用
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# 项目设置
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# 启用C和ASM语言支持
# [移植说明] 如果MCU需要C++支持(如Cortex-M4/M7), 可在此添加CXX
enable_language(C ASM)

# 创建可执行目标
add_executable(${CMAKE_PROJECT_NAME})

# ---------------------------------------------------------------------------
# CubeMX生成的源文件(由CubeMX自动管理, 不要手动修改此子目录)
# ---------------------------------------------------------------------------
add_subdirectory(cmake/stm32cubemx)

# ---------------------------------------------------------------------------
# 中间件: ThreadX实时操作系统
#
# lib/ThreadX/ 包含:
#   common/   - 平台无关内核(185个.c文件)
#   port/gnu/ - Cortex-M3/GNU移植(7个.S汇编文件)
#
# 提供CMake目标:
#   threadx_kernel - OBJECT库, 内核源文件编译产物
#   threadx_port   - OBJECT库, 移植层汇编编译产物
#   threadx_core   - INTERFACE库, 聚合以上两者并导出头文件路径
#
# [移植说明] 切换ARM内核时, 修改 lib/ThreadX/CMakeLists.txt
#            中的 THREADX_PORT_DIR 变量。
# ---------------------------------------------------------------------------
add_subdirectory(lib/ThreadX)

# ---------------------------------------------------------------------------
# 用户应用代码
#
# 文件管理采用自动收集方式, 添加新文件无需修改CMakeLists.txt:
#   - 源文件(.c/.cpp): 放入 user/Application/src/ 或 user/Peripherals/src/
#   - 头文件(.h/.hpp): 放入 user/Application/inc/ 或 user/Peripherals/inc/
#   - 支持子目录(如 user/Peripherals/inc/spi/)
# ---------------------------------------------------------------------------
add_subdirectory(user)

# ThreadX头文件路径通过threadx_core INTERFACE依赖自动传递给用户代码

# 链接目录设置
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户自定义库搜索路径
)

# 添加源文件到可执行目标
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户自定义源文件
)

# 添加头文件搜索路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户自定义头文件路径
)

# 添加预处理宏定义
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户自定义宏定义
)

# 移除错误的libob.a依赖(使用C++文件时需要)
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# 链接库
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    threadx_kernel
    threadx_port
    user_objects
    # 在此添加用户自定义库
)

# ---------------------------------------------------------------------------
# 构建后处理: 生成.hex和.bin文件
# ---------------------------------------------------------------------------
set(HEX_FILE ${CMAKE_PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_PROJECT_NAME}.bin)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${HEX_FILE}
    COMMENT "Generating ${HEX_FILE}"
)
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${BIN_FILE}
    COMMENT "Generating ${BIN_FILE}"
)

set_directory_properties(PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${HEX_FILE};${BIN_FILE}"
)
