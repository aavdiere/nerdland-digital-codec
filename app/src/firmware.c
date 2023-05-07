#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/spi.h>

#include "core/system.h"
#include "video/graphics.h"
#include "video/vga.h"

#define LED_PORT (GPIOC)
#define LED_PIN (GPIO9)

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);

    /*
     * | Pin  | AF1      | AF2       | AF3 | AF4 |
     * | ---- | -------- | --------- | --- | --- |
     * | PA0  | TIM2_CH1 |           |     |     |
     * | PA1  | TIM2_CH2 |           |     |     |
     * | PA2  | TIM2_CH3 |           |     |     |
     * | PA3  | TIM2_CH4 |           |     |     |
     * | PA8  | TIM1_CH1 |           |     |     |
     * | PA9  | TIM1_CH2 | TIM1_BKIN |     |     |
     * | PA10 | TIM1_CH3 |           |     |     |
     * | PA11 | TIM1_CH4 | TIM1_BKIN |     |     |
     * | PA15 | TIM2_CH1 |           |     |     |
     */

    /* Configure HSYNC pin in alternative mode (TIM1_CH1) for HSYNC output */
    /* Decrease output slew rate to handle higher toggle rate */
    gpio_mode_setup(HSYNC_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, HSYNC_PIN);
    gpio_set_af(HSYNC_PORT, GPIO_AF1, HSYNC_PIN);
    gpio_set_output_options(HSYNC_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, HSYNC_PIN);

    /* Configure VSYNC pin in alternative mode (TIM2_CH1) for VSYNC output */
    /* Decrease output slew rate to handle higher toggle rate */
    gpio_mode_setup(VSYNC_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, VSYNC_PIN);
    gpio_set_af(VSYNC_PORT, GPIO_AF1, VSYNC_PIN);
    gpio_set_output_options(VSYNC_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, VSYNC_PIN);
}

int main(void) {
    system_setup();
    gpio_setup();
    vga_setup();

    glClear();

    glPoint(1, 1, GL_OP_OR);
    glPoint(3, 1, GL_OP_OR);
    glPoint(5, 1, GL_OP_OR);

    glPoint(1, 599, GL_OP_OR);
    glPoint(3, 599, GL_OP_OR);
    glPoint(5, 599, GL_OP_OR);

    uint64_t start_time = system_get_ticks();

    /* Infinte loop */
    for (;;) {
        // if (true){
        if (system_get_ticks() - start_time >= 500) {
            gpio_toggle(LED_PORT, LED_PIN);

            start_time = system_get_ticks();
        }
    }

    // Never return
    return 0;
}
