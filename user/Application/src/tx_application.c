#include "tx_api.h"
#include "main.h"

/*
 * ThreadX application entry point.
 *
 * Called by tx_kernel_enter() after _tx_initialize_low_level().
 * Create threads, semaphores, queues, etc. here.
 *
 * Available memory starts at first_free_memory and extends to end of RAM.
 * Do NOT return from this function — ThreadX scheduler takes over.
 */

/* Thread parameters */
#define MAIN_THREAD_STACK_SIZE      1024
#define MAIN_THREAD_PRIORITY        5
#define MAIN_THREAD_PREEMPT_THRESH   5

/* Byte pool for dynamic memory allocation */
#define BYTE_POOL_SIZE              4096

static TX_THREAD main_thread;
static UCHAR main_thread_stack[MAIN_THREAD_STACK_SIZE];

static TX_BYTE_POOL byte_pool;
static UCHAR byte_pool_area[BYTE_POOL_SIZE] __attribute__((aligned(4)));

/* Forward declaration */
static void main_thread_entry(ULONG thread_input);

/*
 * tx_application_define — called by ThreadX to create application objects.
 *
 * first_free_memory: start of unused RAM provided by _tx_initialize_low_level.
 */
void tx_application_define(VOID *first_free_memory)
{
    (void)first_free_memory;

    /* Create a byte pool for dynamic allocation */
    tx_byte_pool_create(&byte_pool, "app_byte_pool", byte_pool_area, BYTE_POOL_SIZE);

    /* Create the main application thread */
    tx_thread_create(&main_thread,
                     "main_thread",
                     main_thread_entry,
                     0,
                     main_thread_stack,
                     MAIN_THREAD_STACK_SIZE,
                     MAIN_THREAD_PRIORITY,
                     MAIN_THREAD_PREEMPT_THRESH,
                     TX_NO_TIME_SLICE,
                     TX_AUTO_START);
}

/*
 * Main application thread.
 *
 * This replaces the bare-metal while(1) loop in main().
 * Add your application logic here.
 */
static void main_thread_entry(ULONG thread_input)
{
    (void)thread_input;

    /* Initialize user peripherals after ThreadX is running */
    /* user_peripherals_init(); */

    while (1)
    {
        /* Application main loop */
        /* user_main(); */

        /* Sleep for 1 tick (1ms) to yield CPU */
        tx_thread_sleep(1);
    }
}
