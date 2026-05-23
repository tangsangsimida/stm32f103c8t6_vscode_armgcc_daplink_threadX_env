/**
 * @file    queue_example.c
 * @brief   ThreadX 队列通信示例 — 生产者和消费者线程
 *
 * 本文件位于 docs/examples/ 时不参与构建。复制到 user/Application/src/
 * 后，可作为线程间通信的起点。
 */

#include "queue_example.h"
#include "app_config.h"
#include "init.h"

#define PRODUCER_THREAD_PRIORITY APP_PRIO_NORMAL_MIN
#define CONSUMER_THREAD_PRIORITY APP_PRIO_NORMAL_MIN
#define PRODUCER_THREAD_STACK_SIZE 512
#define CONSUMER_THREAD_STACK_SIZE 512
#define QUEUE_DEPTH 16

static TX_THREAD producer_thread;
static TX_THREAD consumer_thread;
static TX_QUEUE sample_queue;

static UCHAR producer_stack[PRODUCER_THREAD_STACK_SIZE];
static UCHAR consumer_stack[CONSUMER_THREAD_STACK_SIZE];
static ULONG queue_storage[QUEUE_DEPTH];

static void producer_entry(ULONG input);
static void consumer_entry(ULONG input);

void queue_example_init(void)
{
	APP_TX_CHECK(tx_queue_create(&sample_queue, "sample_queue", 1,
				     queue_storage, sizeof(queue_storage)));

	APP_TX_CHECK(tx_thread_create(
		&producer_thread, "queue_producer", producer_entry, 0,
		producer_stack, PRODUCER_THREAD_STACK_SIZE,
		PRODUCER_THREAD_PRIORITY, PRODUCER_THREAD_PRIORITY,
		TX_NO_TIME_SLICE, TX_AUTO_START));

	APP_TX_CHECK(tx_thread_create(
		&consumer_thread, "queue_consumer", consumer_entry, 0,
		consumer_stack, CONSUMER_THREAD_STACK_SIZE,
		CONSUMER_THREAD_PRIORITY, CONSUMER_THREAD_PRIORITY,
		TX_NO_TIME_SLICE, TX_AUTO_START));
}

static void producer_entry(ULONG input)
{
	ULONG value = 0;

	(void)input;

	while (1) {
		APP_TX_CHECK(
			tx_queue_send(&sample_queue, &value, TX_WAIT_FOREVER));
		value++;
		tx_thread_sleep(APP_MS_TO_TICKS(100));
	}
}

static void consumer_entry(ULONG input)
{
	ULONG value;

	(void)input;

	while (1) {
		APP_TX_CHECK(tx_queue_receive(&sample_queue, &value,
					      TX_WAIT_FOREVER));

		/* TODO: 处理 value，例如更新状态或触发外设动作。 */
	}
}

MODULE_INIT(queue_example_init);
