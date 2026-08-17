#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

/* ============================================================
 * Scheduler
 * ============================================================ */

#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1

#define configCPU_CLOCK_HZ                     ( ( unsigned long ) 168000000UL )
#define configTICK_RATE_HZ                     ( ( TickType_t ) 1000 )

#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                128
#define configMAX_TASK_NAME_LEN                 16

#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                1

/* ============================================================
 * Memory
 * ============================================================ */

#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1

#define configTOTAL_HEAP_SIZE                  ( ( size_t ) ( 16 * 1024 ) )

/* ============================================================
 * Hooks
 * ============================================================ */

#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0

#define configCHECK_FOR_STACK_OVERFLOW          0
#define configUSE_MALLOC_FAILED_HOOK            0

/* ============================================================
 * Synchronization
 * ============================================================ */

#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES            1
#define configUSE_QUEUE_SETS                    0

/* ============================================================
 * Cortex-M4F
 * ============================================================ */

#define configENABLE_FPU                        1
#define configENABLE_MPU                        0
#define configENABLE_TRUSTZONE                  0

/* ============================================================
 * Interrupt priorities
 * ============================================================ */

#define configPRIO_BITS                         4

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15

#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY \
    ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/*
 * Make FreeRTOS verify that the vector table contains:
 *
 *   SVC    -> vPortSVCHandler
 *   PendSV -> xPortPendSVHandler
 *
 * This is extremely useful for this QEMU bring-up.
 */
#define configCHECK_HANDLER_INSTALLATION         1

/* ============================================================
 * API inclusion
 * ============================================================ */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1

#endif
