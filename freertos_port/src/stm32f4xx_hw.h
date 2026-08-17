#ifndef STM32F4XX_HW_H
#define STM32F4XX_HW_H

#include <stdint.h>

void hw_init(void);
void hw_gpio_init(void);
void hw_led_toggle(void);
uint8_t hw_led_get_state(void);
void delay_ms(uint32_t ms);

#endif /* STM32F4XX_HW_H */