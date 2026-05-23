#include "tx_api.h"
#include "stm32f1xx_hal.h"

/* ThreadX timer interrupt handler (defined in tx_timer_interrupt.S) */
extern void _tx_timer_interrupt(void);

/*
 * ThreadX low-level initialization for STM32F103C8T6 (Cortex-M3).
 *
 * Called automatically by tx_kernel_enter() before tx_application_define().
 * Responsibilities:
 *   1. Provide first free RAM address to ThreadX for memory allocation
 *   2. Configure SysTick for 1ms ThreadX timer tick
 *   3. Set handler priorities (PendSV and SVC must be lowest)
 */

/* System clock frequency: 72MHz */
#define SYSTEM_CLOCK_HZ     72000000UL
#define SYSTICK_FREQ_HZ     1000UL          /* 1ms tick */
#define SYSTICK_RELOAD      (SYSTEM_CLOCK_HZ / SYSTICK_FREQ_HZ - 1)

/* Linker symbol: end of used RAM (start of free heap for ThreadX) */
extern ULONG end;

/* ThreadX globals set by this function */
extern ULONG _tx_thread_system_stack_ptr;
extern VOID *_tx_initialize_unused_memory;

void _tx_initialize_low_level(void)
{
    /* Disable interrupts during initialization */
    __disable_irq();

    /* Tell ThreadX where free memory begins */
    _tx_initialize_unused_memory = (VOID *)(((ULONG)&end) + 4);

    /* Save system stack pointer (top of RAM from vector table) */
    _tx_thread_system_stack_ptr = *(ULONG *)0x00000000;

    /* Enable DWT cycle counter for ThreadX performance monitoring */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* Configure SysTick for 1ms tick (ThreadX timer source) */
    SysTick->LOAD = SYSTICK_RELOAD;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;

    /*
     * Set handler priorities.
     * PendSV must be lowest (0xFF) — context switches happen here.
     * SVC must be lowest (0xFF) — thread-level service calls.
     * SysTick should be low but can preempt HAL TIM4.
     */
    SCB->SHP[7]  = 0xFF;        /* SVCall = lowest priority */
    SCB->SHP[10] = 0xFF;        /* PendSV = lowest priority */
    SCB->SHP[11] = 0x40;        /* SysTick priority */

    __enable_irq();
}

/*
 * SysTick_Handler — ThreadX timer tick.
 *
 * This overrides the empty handler in stm32f1xx_it.c.
 * The startup file's weak symbol is replaced by this strong definition.
 */
void SysTick_Handler(void)
{
    _tx_timer_interrupt();
}
