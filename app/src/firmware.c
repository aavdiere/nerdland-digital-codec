#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>

#include "core/system.h"

#define LED_PORT (GPIOC)
#define LED_PIN (GPIO9)

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void timer_setup(void) {
    
}

int main(void) {
    system_setup();
    gpio_setup();

    uint64_t start_time = system_get_ticks();

    /* Infinte loop */
    for (;;) {
        if (system_get_ticks() - start_time >= 1000) {
            gpio_toggle(LED_PORT, LED_PIN);
            start_time = system_get_ticks();
        }
    }

    // Never return
    return 0;
}
