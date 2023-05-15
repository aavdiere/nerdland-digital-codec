#include "core/system.h"

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>

volatile uint64_t ticks = 0;

void sys_tick_handler(void) {
    ticks++;
}

static void rcc_setup(void) {
    /* 80MHz PLL from HSE */
    const struct rcc_clock_scale rcc_hse16_config = {
        .pllm           = 1,
        .plln           = 10,
        .pllp           = RCC_PLLCFGR_PLLP_DIV7,
        .pllq           = RCC_PLLCFGR_PLLQ_DIV2,
        .pllr           = RCC_PLLCFGR_PLLR_DIV2,
        .pll_source     = RCC_PLLCFGR_PLLSRC_HSE,
        .hpre           = RCC_CFGR_HPRE_NODIV,
        .ppre1          = RCC_CFGR_PPRE_NODIV,
        .ppre2          = RCC_CFGR_PPRE_NODIV,
        .voltage_scale  = PWR_SCALE1,
        .flash_config   = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_LATENCY_4WS,
        .ahb_frequency  = 80000000,
        .apb1_frequency = 80000000,
        .apb2_frequency = 80000000,
    };

    // rcc_clock_setup_pll(&rcc_hsi16_configs[RCC_CLOCK_VRANGE1_80MHZ]);
    rcc_clock_setup_pll(&rcc_hse16_config);

    rcc_set_clock48_source(RCC_CCIPR_CLK48SEL_PLLSAI1Q);
}

static void systick_setup(void) {
    systick_set_frequency(SYSTICK_FREQ, CPU_FREQ);
    systick_counter_enable();
    systick_interrupt_enable();
}

uint64_t system_get_ticks(void) {
    return ticks;
}

void system_setup(void) {
    rcc_setup();
    systick_setup();
}

uint32_t HAL_GetTick(void) {
    return ticks;
}
