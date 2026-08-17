#ifndef QUEUE_HPP
#define QUEUE_HPP

extern "C" {
    #include "FreeRTOS.h"
    #include "queue.h"
}

template <typename T, size_t Length>
class MessageQueue {
public:
    MessageQueue() {
        handle = xQueueCreate(Length, sizeof(T));
    }

    ~MessageQueue() {
        if (handle != nullptr) {
            vQueueDelete(handle);
        }
    }

    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    bool send(const T& item, TickType_t timeout = portMAX_DELAY) {
        return xQueueSend(handle, &item, timeout) == pdTRUE;
    }

    bool receive(T& item, TickType_t timeout = portMAX_DELAY) {
        return xQueueReceive(handle, &item, timeout) == pdTRUE;
    }

    bool sendFromISR(const T& item, BaseType_t* higherPriorityTaskWoken) {
        return xQueueSendFromISR(handle, &item, higherPriorityTaskWoken) == pdTRUE;
    }

    bool receiveFromISR(T& item, BaseType_t* higherPriorityTaskWoken) {
        return xQueueReceiveFromISR(handle, &item, higherPriorityTaskWoken) == pdTRUE;
    }

private:
    QueueHandle_t handle;
};

#endif // QUEUE_HPP