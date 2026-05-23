/**
 * @file    queue_example.h
 * @brief   ThreadX 队列通信示例接口
 */

#ifndef __QUEUE_EXAMPLE_H
#define __QUEUE_EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  创建示例队列、生产者线程和消费者线程
 */
void queue_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_EXAMPLE_H */
