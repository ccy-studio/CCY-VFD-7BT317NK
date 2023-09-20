#ifndef __THREAD_HELPER_H
#define __THREAD_HELPER_H

#include <Ticker.h>
#include <constant.h>
#include "thread_tid_enum.h"

typedef void (*fun_thread_run)();

typedef struct thread_obj_struct {
    u8 task_id;
    u32 loop_ms = 255;
    u8 run_state = 0;
    Ticker ticker;
    fun_thread_run fun = NULL;
    struct thread_obj_struct* next = NULL;
    struct thread_obj_struct* pre = NULL;
} thread_obj;

/**
 * 创建一个模拟线程但是不启动
 * @param taskId 句柄ID
 * @param ms 循环调用延时时间ms
 * @param fun 回调函数
 */
thread_obj* thread_create(u8 taskId, u32 ms, fun_thread_run fun);

/**
 * 销毁一个模拟线程并释放内存空间
 */
void thread_destory(thread_obj* obj);
void thread_destory_byid(u8 id);

/**
 * 启动线程
 */
void thread_start(u8 id);

/**
 * 停止线程
 */
void thread_stop(u8 id);

#endif