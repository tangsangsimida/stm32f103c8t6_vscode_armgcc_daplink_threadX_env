/**
 * @file    tx_initialize_low_level.c
 * @brief   ThreadX底层硬件初始化
 *
 * 本文件属于ThreadX移植层, 不由CubeMX生成, CubeMX重新生成代码时不会覆盖。
 *
 * =============================================================================
 * [移植说明] 更换MCU或修改时钟配置时需要修改的内容:
 *
 *   1. SYSTEM_CLOCK_HZ  - 必须与实际的SYSCLK频率一致。
 *                          查看main.c中SystemClock_Config()获取实际值。
 *
 *   2. SYSTICK_FREQ_HZ  - ThreadX节拍频率, 默认1000(1ms)。通常无需修改,
 *                          除非需要不同的节拍周期。
 *
 *   3. 链接器符号"end" - 定义在STM32F103XX_FLASH.ld中。如果使用不同的
 *                          链接脚本, 请确保其在已用BSS末尾定义了符号
 *                         (如_ebss、__bss_end__)。
 *
 *   4. SCB->SHP[]索引  - 这些索引是Cortex-M3专用的(SHP[7]=SVCall,
 *                          SHP[10]=PendSV, SHP[11]=SysTick)。Cortex-M4/M7
 *                          的索引相同, 但移植到其他架构时请对照CMSIS头文件验证。
 *
 *   5. PendSV_Handler   - 定义在cortex_m3/gnu/src/tx_thread_schedule.S中。
 *                          更换ARM内核时, 对应移植目录的汇编文件会提供此处理函数。
 *                          stm32f1xx_it.c中的__weak桩函数会自动被覆盖。
 *
 *   6. SysTick_Handler  - 定义在本文件中。调用_tx_timer_interrupt()执行
 *                          ThreadX定时器节拍。必须以SYSTICK_FREQ_HZ定义的频率
 *                          从SysTick中断中调用。
 * =============================================================================
 */

#include "tx_api.h"
#include "stm32f1xx_hal.h"

/* ThreadX定时器中断处理函数, 定义在common移植汇编中 */
extern void _tx_timer_interrupt(void);

/*
 * 系统时钟和节拍配置
 *
 * [移植说明] SYSTEM_CLOCK_HZ必须与SystemClock_Config()中配置的实际SYSCLK频率一致。
 * 当前项目: HSE 8MHz * PLL x9 = 72MHz。修改时钟时需同步更新此值以保证节拍精度。
 */
#define SYSTEM_CLOCK_HZ     72000000UL
#define SYSTICK_FREQ_HZ     1000UL          /* 1ms节拍周期 */
#define SYSTICK_RELOAD      (SYSTEM_CLOCK_HZ / SYSTICK_FREQ_HZ - 1)

/*
 * 链接器符号: 已用RAM的末尾地址
 *
 * 标记所有静态分配数据(.data、.bss、.tbss)之后的第一个地址。
 * ThreadX据此确定空闲堆内存的起始位置。
 *
 * [移植说明] "end"定义在STM32F103XX_FLASH.ld中。如果链接脚本使用不同的符号名
 * (如_ebss、__bss_end__), 请在此处修改。
 */
extern ULONG end;

/* ThreadX内部全局变量, 由本函数设置 */
extern ULONG _tx_thread_system_stack_ptr;
extern VOID *_tx_initialize_unused_memory;

/**
 * @brief  ThreadX底层硬件初始化
 *
 * 由tx_kernel_enter()在tx_application_define()之前自动调用。
 * 职责:
 *   1. 告知ThreadX空闲RAM起始地址(用于线程栈、字节池)
 *   2. 保存系统栈指针(用于ISR处理)
 *   3. 使能DWT周期计数器(用于ThreadX性能跟踪)
 *   4. 配置SysTick作为ThreadX周期性定时器源
 *   5. 设置中断处理函数优先级(PendSV和SVCall必须最低)
 */
void _tx_initialize_low_level(void)
{
    __disable_irq();

    /* 1. 告知ThreadX空闲堆内存起始地址 */
    _tx_initialize_unused_memory = (VOID *)(((ULONG)&end) + 4);

    /* 2. 从向量表(地址0x00000000)保存系统栈指针 */
    _tx_thread_system_stack_ptr = *(ULONG *)0x00000000;

    /* 3. 使能DWT周期计数器, 用于ThreadX性能监控 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* 4. 配置SysTick产生周期性1ms中断(ThreadX定时器源) */
    SysTick->LOAD = SYSTICK_RELOAD;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;

    /*
     * 5. 设置中断处理函数优先级
     *
     * Cortex-M3 SHP寄存器布局(按字节寻址):
     *   SHP[0]  = MemManage优先级
     *   SHP[1]  = BusFault优先级
     *   SHP[2]  = UsageFault优先级
     *   SHP[7]  = SVCall优先级
     *   SHP[10] = PendSV优先级
     *   SHP[11] = SysTick优先级
     *
     * ThreadX要求:
     *   - PendSV = 最低优先级(0xFF): 上下文切换不能抢占ISR
     *   - SVCall = 最低优先级(0xFF): 线程级服务调用
     *   - SysTick = 0x40: 可被更高优先级的ISR抢占
     */
    SCB->SHP[7]  = 0xFF;        /* SVCall  = 最低优先级 */
    SCB->SHP[10] = 0xFF;        /* PendSV  = 最低优先级 */
    SCB->SHP[11] = 0x40;        /* SysTick = 中等优先级 */

    __enable_irq();
}

/**
 * @brief  ThreadX的SysTick中断处理函数
 *
 * 由SysTick硬件定时器每1ms调用一次。
 * 调用_tx_timer_interrupt()处理以下事务:
 *   - 线程时间片轮转
 *   - 线程睡眠超时
 *   - 定时器API回调
 *   - 所有基于时间的ThreadX服务
 *
 * 此函数覆盖stm32f1xx_it.c中的空SysTick_Handler。
 * 启动文件将SysTick_Handler定义为weak符号, 此处的强定义会自动优先使用。
 *
 * [移植说明] 不要在此处调用HAL_IncTick()。HAL时间基准使用TIM4
 * (见stm32f1xx_hal_timebase_tim.c), SysTick专属于ThreadX。
 */
void SysTick_Handler(void)
{
    _tx_timer_interrupt();
}
