#ifndef THREAD_HPP
#define THREAD_HPP

extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
}

class Thread {
public:
    Thread(const char* name, uint16_t stackDepth, UBaseType_t priority) 
        : taskHandle(nullptr), taskName(name), stackSize(stackDepth), taskPriority(priority) {}

    virtual ~Thread() {
        if (taskHandle != nullptr) {
            vTaskDelete(taskHandle);
        }
    }

    // Delete copy constructor and assignment operator to prevent double-free of tasks
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    void start() {
        xTaskCreate(taskWrapper, taskName, stackSize, this, taskPriority, &taskHandle);
    }

    // Pure virtual function that every subclass must implement for its task loop
    virtual void run() = 0;

private:
    static void taskWrapper(void* pvParameters) {
        Thread* threadInstance = static_cast<Thread*>(pvParameters);
        threadInstance->run();
    }

    TaskHandle_t taskHandle;
    const char* taskName;
    uint16_t stackSize;
    UBaseType_t taskPriority;
};

#endif // THREAD_HPP