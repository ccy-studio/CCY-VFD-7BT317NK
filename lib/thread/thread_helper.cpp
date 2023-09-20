#include "thread_helper.h"

thread_obj root;

thread_obj* find_by_id(u8 id) {
    thread_obj* to = &root;
    while (to->next != NULL) {
        if (to->task_id == id) {
            return to;
        }
        to = to->next;
    }
    return NULL;
}

void push_linked_list(thread_obj* obj) {
    thread_obj* next = &root;
    while (next->next != NULL) {
        next = next->next;
    }
    next->next = obj;
    obj->pre = next;
}

void remove_item(thread_obj* obj) {
    if (obj == NULL) {
        return;
    }
    obj->pre->next = obj->next;
    free(obj);
    obj = NULL;
}

thread_obj* thread_create(u8 taskId, u32 ms, fun_thread_run fun) {
    // 判断是否有重复
    if (find_by_id(taskId) != NULL) {
        return NULL;
    }
    thread_obj* obj = (thread_obj*)malloc(sizeof(thread_obj));
    if (obj == NULL) {
        Serial.println("ERROR:内存分配失败");
        return NULL;
    }
    obj->loop_ms = ms;
    obj->task_id = taskId;
    obj->next = NULL;
    obj->run_state = 0;
    obj->fun = fun;
    obj->ticker.attach_ms(obj->loop_ms, obj->fun);
    push_linked_list(obj);
    return obj;
}

void thread_destory(thread_obj* obj) {
    obj->run_state = 0;
    obj->ticker.detach();
    // 移除链表
    remove_item(obj);
}

void thread_destory_byid(u8 id) {
    thread_obj* obj = find_by_id(id);
    if (obj == NULL) {
        return;
    }
    thread_destory(obj);
}

void thread_start(u8 id) {
    thread_obj* obj = find_by_id(id);
    if (obj == NULL || obj->run_state == 1 || obj->ticker.active()) {
        return;
    }
    obj->run_state = 1;
    obj->ticker.attach_ms(obj->loop_ms, obj->fun);
}

void thread_stop(u8 id) {
    thread_obj* obj = find_by_id(id);
    if (obj == NULL || obj->run_state == 0 || !obj->ticker.active()) {
        return;
    }
    obj->run_state = 0;
    obj->ticker.detach();
}