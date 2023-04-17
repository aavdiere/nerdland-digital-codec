#include "common.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define LED_PORT (GPIOC)
#define LED_PIN (GPIO9)

#define CPU_FREQ (84000000)
#define SYSTICK_FREQ (1000)

volatile uint64_t ticks = 0;

void sys_tick_handler(void) {
    ticks++;
}

static uint64_t get_ticks(void) {
    return ticks;
}

static void rcc_setup(void) {
    rcc_clock_setup_pll(&rcc_hsi16_configs[RCC_CLOCK_VRANGE1_80MHZ]);
}

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void systick_setup(void) {
    systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
    systick_counter_enable();
    systick_interrupt_enable();
}

int main(void) {
    rcc_setup();
    gpio_setup();

    systick_setup();

    uint64_t start_time = get_ticks();

    /* Infinte loop */
    for (;;) {
        if (get_ticks() - start_time >= 1000) {
            gpio_toggle(LED_PORT, LED_PIN);
            start_time = get_ticks();
        }
    }

    // Never return
    return 0;
}
