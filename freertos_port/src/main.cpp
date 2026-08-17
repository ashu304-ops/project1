extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

static void test_task(void *argument)
{
    (void)argument;

    for (;;)
    {
        /* Keep task alive. */
        __asm volatile ("nop");
    }
}

extern "C" int main(void)
{
    BaseType_t result;

    result = xTaskCreate(
        test_task,
        "TEST",
        256,
        nullptr,
        1,
        nullptr
    );

    if (result != pdPASS)
    {
        /* Task creation failed. */
        for (;;)
        {
            __asm volatile ("nop");
        }
    }

    vTaskStartScheduler();

    /* Scheduler should never return. */
    for (;;)
    {
        __asm volatile ("nop");
    }
}
