# ============================================================================
# ThreadX RTOS CMake配置
#
# 本文件是ThreadX中间件的统一入口。根CMakeLists.txt通过
# add_subdirectory(lib/ThreadX)引用本文件。
#
# 提供三个CMake目标:
#   threadx_kernel : OBJECT库, 包含平台无关的ThreadX内核源文件
#   threadx_port   : OBJECT库, 包含当前ARM内核的移植汇编文件
#   threadx_core   : INTERFACE库, 聚合内核和移植层, 导出头文件路径
#
# [移植说明] 切换ARM内核或编译器时:
#   1. 修改 THREADX_PORT_DIR 变量指向对应的移植目录
#   2. Cortex-M3/M4/M7 使用同一 gnu 移植(tx_port.h通过预处理器区分)
#   3. IAR编译器: 使用 port/iar/
#   4. ARM Compiler 6: 使用 port/ac6/
# ============================================================================

# ---------------------------------------------------------------------------
# 移植目录配置
# ---------------------------------------------------------------------------
set(THREADX_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(THREADX_PORT_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/port/gnu)

# ---------------------------------------------------------------------------
# ThreadX内核目标 (平台无关, 所有ARM内核通用)
# ---------------------------------------------------------------------------
add_library(threadx_kernel OBJECT)

target_sources(threadx_kernel PRIVATE
    # 可覆盖的源文件(通过TX_SRC_OVERRIDES变量)
    ${THREADX_COMMON_DIR}/src/tx_thread_delete.c
    ${THREADX_COMMON_DIR}/src/tx_thread_reset.c

    # 内核源文件 - 块内存管理
    ${THREADX_COMMON_DIR}/src/tx_block_allocate.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_cleanup.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_create.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_delete.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_block_pool_prioritize.c
    ${THREADX_COMMON_DIR}/src/tx_block_release.c

    # 内核源文件 - 字节内存管理
    ${THREADX_COMMON_DIR}/src/tx_byte_allocate.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_cleanup.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_create.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_delete.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_prioritize.c
    ${THREADX_COMMON_DIR}/src/tx_byte_pool_search.c
    ${THREADX_COMMON_DIR}/src/tx_byte_release.c

    # 内核源文件 - 事件标志
    ${THREADX_COMMON_DIR}/src/tx_event_flags_cleanup.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_create.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_delete.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_get.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_set.c
    ${THREADX_COMMON_DIR}/src/tx_event_flags_set_notify.c

    # 内核源文件 - 初始化
    ${THREADX_COMMON_DIR}/src/tx_initialize_high_level.c
    ${THREADX_COMMON_DIR}/src/tx_initialize_kernel_enter.c
    ${THREADX_COMMON_DIR}/src/tx_initialize_kernel_setup.c

    # 内核源文件 - MISRA合规
    ${THREADX_COMMON_DIR}/src/tx_misra.c

    # 内核源文件 - 互斥锁
    ${THREADX_COMMON_DIR}/src/tx_mutex_cleanup.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_create.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_delete.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_get.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_prioritize.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_priority_change.c
    ${THREADX_COMMON_DIR}/src/tx_mutex_put.c

    # 内核源文件 - 消息队列
    ${THREADX_COMMON_DIR}/src/tx_queue_cleanup.c
    ${THREADX_COMMON_DIR}/src/tx_queue_create.c
    ${THREADX_COMMON_DIR}/src/tx_queue_delete.c
    ${THREADX_COMMON_DIR}/src/tx_queue_flush.c
    ${THREADX_COMMON_DIR}/src/tx_queue_front_send.c
    ${THREADX_COMMON_DIR}/src/tx_queue_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_queue_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_queue_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_queue_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_queue_prioritize.c
    ${THREADX_COMMON_DIR}/src/tx_queue_receive.c
    ${THREADX_COMMON_DIR}/src/tx_queue_send.c
    ${THREADX_COMMON_DIR}/src/tx_queue_send_notify.c

    # 内核源文件 - 信号量
    ${THREADX_COMMON_DIR}/src/tx_semaphore_ceiling_put.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_cleanup.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_create.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_delete.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_get.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_prioritize.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_put.c
    ${THREADX_COMMON_DIR}/src/tx_semaphore_put_notify.c

    # 内核源文件 - 线程管理
    ${THREADX_COMMON_DIR}/src/tx_thread_create.c
    ${THREADX_COMMON_DIR}/src/tx_thread_entry_exit_notify.c
    ${THREADX_COMMON_DIR}/src/tx_thread_identify.c
    ${THREADX_COMMON_DIR}/src/tx_thread_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_thread_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_thread_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_thread_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_thread_preemption_change.c
    ${THREADX_COMMON_DIR}/src/tx_thread_priority_change.c
    ${THREADX_COMMON_DIR}/src/tx_thread_relinquish.c
    ${THREADX_COMMON_DIR}/src/tx_thread_resume.c
    ${THREADX_COMMON_DIR}/src/tx_thread_shell_entry.c
    ${THREADX_COMMON_DIR}/src/tx_thread_sleep.c
    ${THREADX_COMMON_DIR}/src/tx_thread_stack_analyze.c
    ${THREADX_COMMON_DIR}/src/tx_thread_stack_error_handler.c
    ${THREADX_COMMON_DIR}/src/tx_thread_stack_error_notify.c
    ${THREADX_COMMON_DIR}/src/tx_thread_suspend.c
    ${THREADX_COMMON_DIR}/src/tx_thread_system_preempt_check.c
    ${THREADX_COMMON_DIR}/src/tx_thread_system_resume.c
    ${THREADX_COMMON_DIR}/src/tx_thread_system_suspend.c
    ${THREADX_COMMON_DIR}/src/tx_thread_terminate.c
    ${THREADX_COMMON_DIR}/src/tx_thread_time_slice.c
    ${THREADX_COMMON_DIR}/src/tx_thread_time_slice_change.c
    ${THREADX_COMMON_DIR}/src/tx_thread_timeout.c
    ${THREADX_COMMON_DIR}/src/tx_thread_wait_abort.c

    # 内核源文件 - 时间管理
    ${THREADX_COMMON_DIR}/src/tx_time_get.c
    ${THREADX_COMMON_DIR}/src/tx_time_set.c

    # 内核源文件 - 定时器
    ${THREADX_COMMON_DIR}/src/tx_timer_activate.c
    ${THREADX_COMMON_DIR}/src/tx_timer_change.c
    ${THREADX_COMMON_DIR}/src/tx_timer_create.c
    ${THREADX_COMMON_DIR}/src/tx_timer_deactivate.c
    ${THREADX_COMMON_DIR}/src/tx_timer_delete.c
    ${THREADX_COMMON_DIR}/src/tx_timer_expiration_process.c
    ${THREADX_COMMON_DIR}/src/tx_timer_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_timer_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_timer_performance_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_timer_performance_system_info_get.c
    ${THREADX_COMMON_DIR}/src/tx_timer_system_activate.c
    ${THREADX_COMMON_DIR}/src/tx_timer_system_deactivate.c
    ${THREADX_COMMON_DIR}/src/tx_timer_thread_entry.c

    # 内核源文件 - 跟踪
    ${THREADX_COMMON_DIR}/src/tx_trace_buffer_full_notify.c
    ${THREADX_COMMON_DIR}/src/tx_trace_disable.c
    ${THREADX_COMMON_DIR}/src/tx_trace_enable.c
    ${THREADX_COMMON_DIR}/src/tx_trace_event_filter.c
    ${THREADX_COMMON_DIR}/src/tx_trace_event_unfilter.c
    ${THREADX_COMMON_DIR}/src/tx_trace_initialize.c
    ${THREADX_COMMON_DIR}/src/tx_trace_interrupt_control.c
    ${THREADX_COMMON_DIR}/src/tx_trace_isr_enter_insert.c
    ${THREADX_COMMON_DIR}/src/tx_trace_isr_exit_insert.c
    ${THREADX_COMMON_DIR}/src/tx_trace_object_register.c
    ${THREADX_COMMON_DIR}/src/tx_trace_object_unregister.c
    ${THREADX_COMMON_DIR}/src/tx_trace_user_event_insert.c

    # 内核源文件 - 错误检查变体(txe_前缀)
    ${THREADX_COMMON_DIR}/src/txe_block_allocate.c
    ${THREADX_COMMON_DIR}/src/txe_block_pool_create.c
    ${THREADX_COMMON_DIR}/src/txe_block_pool_delete.c
    ${THREADX_COMMON_DIR}/src/txe_block_pool_info_get.c
    ${THREADX_COMMON_DIR}/src/txe_block_pool_prioritize.c
    ${THREADX_COMMON_DIR}/src/txe_block_release.c
    ${THREADX_COMMON_DIR}/src/txe_byte_allocate.c
    ${THREADX_COMMON_DIR}/src/txe_byte_pool_create.c
    ${THREADX_COMMON_DIR}/src/txe_byte_pool_delete.c
    ${THREADX_COMMON_DIR}/src/txe_byte_pool_info_get.c
    ${THREADX_COMMON_DIR}/src/txe_byte_pool_prioritize.c
    ${THREADX_COMMON_DIR}/src/txe_byte_release.c
    ${THREADX_COMMON_DIR}/src/txe_event_flags_create.c
    ${THREADX_COMMON_DIR}/src/txe_event_flags_delete.c
    ${THREADX_COMMON_DIR}/src/txe_event_flags_get.c
    ${THREADX_COMMON_DIR}/src/txe_event_flags_info_get.c
    ${THREADX_COMMON_DIR}/src/txe_event_flags_set.c
    ${THREADX_COMMON_DIR}/src/txe_event_flags_set_notify.c
    ${THREADX_COMMON_DIR}/src/txe_mutex_create.c
    ${THREADX_COMMON_DIR}/src/txe_mutex_delete.c
    ${THREADX_COMMON_DIR}/src/txe_mutex_get.c
    ${THREADX_COMMON_DIR}/src/txe_mutex_info_get.c
    ${THREADX_COMMON_DIR}/src/txe_mutex_prioritize.c
    ${THREADX_COMMON_DIR}/src/txe_mutex_put.c
    ${THREADX_COMMON_DIR}/src/txe_queue_create.c
    ${THREADX_COMMON_DIR}/src/txe_queue_delete.c
    ${THREADX_COMMON_DIR}/src/txe_queue_flush.c
    ${THREADX_COMMON_DIR}/src/txe_queue_front_send.c
    ${THREADX_COMMON_DIR}/src/txe_queue_info_get.c
    ${THREADX_COMMON_DIR}/src/txe_queue_prioritize.c
    ${THREADX_COMMON_DIR}/src/txe_queue_receive.c
    ${THREADX_COMMON_DIR}/src/txe_queue_send.c
    ${THREADX_COMMON_DIR}/src/txe_queue_send_notify.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_ceiling_put.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_create.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_delete.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_get.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_info_get.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_prioritize.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_put.c
    ${THREADX_COMMON_DIR}/src/txe_semaphore_put_notify.c
    ${THREADX_COMMON_DIR}/src/txe_thread_create.c
    ${THREADX_COMMON_DIR}/src/txe_thread_delete.c
    ${THREADX_COMMON_DIR}/src/txe_thread_entry_exit_notify.c
    ${THREADX_COMMON_DIR}/src/txe_thread_info_get.c
    ${THREADX_COMMON_DIR}/src/txe_thread_preemption_change.c
    ${THREADX_COMMON_DIR}/src/txe_thread_priority_change.c
    ${THREADX_COMMON_DIR}/src/txe_thread_relinquish.c
    ${THREADX_COMMON_DIR}/src/txe_thread_reset.c
    ${THREADX_COMMON_DIR}/src/txe_thread_resume.c
    ${THREADX_COMMON_DIR}/src/txe_thread_suspend.c
    ${THREADX_COMMON_DIR}/src/txe_thread_terminate.c
    ${THREADX_COMMON_DIR}/src/txe_thread_time_slice_change.c
    ${THREADX_COMMON_DIR}/src/txe_thread_wait_abort.c
    ${THREADX_COMMON_DIR}/src/txe_timer_activate.c
    ${THREADX_COMMON_DIR}/src/txe_timer_change.c
    ${THREADX_COMMON_DIR}/src/txe_timer_create.c
    ${THREADX_COMMON_DIR}/src/txe_timer_deactivate.c
    ${THREADX_COMMON_DIR}/src/txe_timer_delete.c
    ${THREADX_COMMON_DIR}/src/txe_timer_info_get.c
)

# SYSTEM关键字避免ThreadX头文件中的警告污染用户代码
# tx_api.h内部#include "tx_port.h", 因此内核也需要移植层头文件路径
target_include_directories(threadx_kernel SYSTEM PUBLIC
    ${THREADX_COMMON_DIR}/inc
    ${THREADX_PORT_DIR}/inc
)

# ---------------------------------------------------------------------------
# ThreadX移植层目标 (当前: Cortex-M3/GNU)
#
# [移植说明] 切换ARM内核或编译器时, 修改上方THREADX_PORT_DIR变量,
#            并将下方源文件列表替换为对应移植目录中的.S文件。
# ---------------------------------------------------------------------------
add_library(threadx_port OBJECT)

target_sources(threadx_port PRIVATE
    ${THREADX_PORT_DIR}/src/tx_thread_context_restore.S
    ${THREADX_PORT_DIR}/src/tx_thread_context_save.S
    ${THREADX_PORT_DIR}/src/tx_thread_interrupt_control.S
    ${THREADX_PORT_DIR}/src/tx_thread_schedule.S
    ${THREADX_PORT_DIR}/src/tx_thread_stack_build.S
    ${THREADX_PORT_DIR}/src/tx_thread_system_return.S
    ${THREADX_PORT_DIR}/src/tx_timer_interrupt.S
)

target_include_directories(threadx_port PUBLIC ${THREADX_PORT_DIR}/inc)
target_link_libraries(threadx_port PUBLIC threadx_kernel)

# ---------------------------------------------------------------------------
# 聚合接口库
#
# 用户代码只需链接此目标即可获得ThreadX全部头文件和编译产物。
# 用法: target_link_libraries(my_target PUBLIC threadx_core)
# ---------------------------------------------------------------------------
add_library(threadx_core INTERFACE)
target_link_libraries(threadx_core INTERFACE threadx_kernel threadx_port)
