#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#define SYSTICK_CTRL  (*(volatile uint32_t *)0xE000E010U)
#define SYSTICK_LOAD  (*(volatile uint32_t *)0xE000E014U)
#define SYSTICK_VAL   (*(volatile uint32_t *)0xE000E018U)

static volatile uint8_t g_led_state = 0;

/* Hardware initialization for QEMU emulation */
void hw_init(void) {
    /* Configure SysTick reload for 1ms tick rate at 168 MHz */
    SYSTICK_LOAD = (168000000U / 1000U) - 1U;
    SYSTICK_VAL  = 0U;
    /* Enable SysTick with CPU Clock (Bit 2), Interrupt (Bit 1), Enable (Bit 0) */
    SYSTICK_CTRL = 0x07U;
}

/* GPIO / LED Stubs for QEMU */
void hw_gpio_init(void) {
    g_led_state = 0;
    printf("[HW] GPIO Initialized for LED\n");
}

void hw_led_toggle(void) {
    g_led_state ^= 1;
    printf("[HW] LED Toggled -> State: %s\n", g_led_state ? "ON" : "OFF");
}

uint8_t hw_led_get_state(void) {
    return g_led_state;
}

/* Software delay function compatible with both scheduler and pre-scheduler boot */
void delay_ms(uint32_t ms) {
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    } else {
        /* Busy wait loop prior to scheduler start */
        for (volatile uint32_t i = 0; i < ms * 10000; i++) {
            __asm__ volatile ("nop");
        }
    }
}