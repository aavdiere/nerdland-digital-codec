#ifndef __VGA_H
#define __VGA_H

#include <libopencm3/stm32/rcc.h>

#include "common.h"

void vga_setup(void);

void hsync_setup(void);
void tim1_cc_isr(void);

void vsync_setup(void);
void tim2_isr(void);

/* Generic struct that hold all settings
 * related to a single color channel
 */
struct color_channel_t {
    enum rcc_periph_clken rcc_dma;
    uint32_t              dma;
    uint8_t               dma_channel;
    uint8_t               dma_stream;
    uint8_t               dma_irqn;

    enum rcc_periph_clken rcc_spi;
    uint32_t              spi;
    uint8_t               spi_irqn;
    uint32_t              spi_address;

    enum rcc_periph_clken rcc_gpio;
    uint32_t              gpio_port;
    uint16_t              gpio_pin;
    uint8_t               gpio_af;
    volatile uint8_t     *frame_buffer;
    uint16_t              frame_buffer_size;
};

void color_channel_setup(struct color_channel_t color_channel);
/* Green channel interrupt */
void dma1_channel5_isr(void);
/* Blue channel interrupt */
void dma1_channel3_isr(void);

void vidEmptyScreen(void);
void vidDemoScreen(void);

#endif /* __VGA_H  */
