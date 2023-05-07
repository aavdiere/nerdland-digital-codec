#ifndef __VGA_H
#define __VGA_H

#include <libopencm3/cm3/nvic.h>

#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "common.h"

void vga_setup(void);

void hsync_setup(void);
void tim1_cc_isr(void);

void vsync_setup(void);
void tim2_isr(void);

/* Buffer */
extern volatile uint8_t (*raw_buffer)[V_VISIBLE][(H_KEEPOUT + H_VISIBLE + H_KEEPOUT) / 8];
#define buffer (*raw_buffer)

// extern volatile uint8_t buffer[V_VISIBLE][(H_KEEPOUT + H_VISIBLE + H_KEEPOUT) / 8]
//     __attribute__((aligned(4)));
extern volatile uint8_t *frame_buffer[V_VISIBLE];

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
    // volatile uint8_t     *frame_buffer;
    // uint16_t              frame_buffer_size;
};

static const struct color_channel_t vga_red = {
    .rcc_dma     = RCC_DMA2,
    .dma         = DMA2,
    .dma_channel = DMA_CHANNEL2,
    .dma_stream  = 3,
    .dma_irqn    = NVIC_DMA2_CHANNEL2_IRQ,
    .rcc_spi     = RCC_SPI3,
    .spi         = SPI3,
    .spi_irqn    = NVIC_SPI3_IRQ,
    .spi_address = (uint32_t)&SPI3_DR,
    .rcc_gpio    = RCC_GPIOC,
    .gpio_port   = VGA_RED_PORT,
    .gpio_pin    = VGA_RED_PIN,
    .gpio_af     = GPIO_AF6,
    // .frame_buffer      = &buffer[0][0],
    // .frame_buffer_size = (H_KEEPOUT + H_VISIBLE + H_KEEPOUT) / 8,
};

static const struct color_channel_t vga_green = {
    .rcc_dma     = RCC_DMA1,
    .dma         = DMA1,
    .dma_channel = DMA_CHANNEL5,
    .dma_stream  = 1,
    .dma_irqn    = NVIC_DMA1_CHANNEL5_IRQ,
    .rcc_spi     = RCC_SPI2,
    .spi         = SPI2,
    .spi_irqn    = NVIC_SPI2_IRQ,
    .spi_address = (uint32_t)&SPI2_DR,
    .rcc_gpio    = RCC_GPIOC,
    .gpio_port   = VGA_GREEN_PORT,
    .gpio_pin    = VGA_GREEN_PIN,
    .gpio_af     = GPIO_AF5,
    // .frame_buffer      = &buffer[0][0],
    // .frame_buffer_size = (H_KEEPOUT + H_VISIBLE + H_KEEPOUT) / 8,
};

static const struct color_channel_t vga_blue = {
    .rcc_dma     = RCC_DMA1,
    .dma         = DMA1,
    .dma_channel = DMA_CHANNEL3,
    .dma_stream  = 1,
    .dma_irqn    = NVIC_DMA1_CHANNEL3_IRQ,
    .rcc_spi     = RCC_SPI1,
    .spi         = SPI1,
    .spi_irqn    = NVIC_SPI1_IRQ,
    .spi_address = (uint32_t)&SPI1_DR,
    .rcc_gpio    = RCC_GPIOA,
    .gpio_port   = VGA_BLUE_PORT,
    .gpio_pin    = VGA_BLUE_PIN,
    .gpio_af     = GPIO_AF5,
    // .frame_buffer      = &buffer[0][0],
    // .frame_buffer_size = (H_KEEPOUT + H_VISIBLE + H_KEEPOUT) / 8,
};

void color_channel_setup(struct color_channel_t color_channel);
/* Red channel interrupt */
void dma2_channel2_isr(void);
/* Green channel interrupt */
void dma1_channel5_isr(void);
/* Blue channel interrupt */
void dma1_channel3_isr(void);

void buffer_setup(void);

#endif /* __VGA_H  */
