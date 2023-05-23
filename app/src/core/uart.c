#include "core/uart.h"
#include "video/graphics.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

// variables that will be used to synchronize USB and UART
uint8_t heartbeat = 0x16;
uint8_t tx_data[64];

volatile uint8_t tx_done = 0;

volatile uint8_t rx_done = 0;
volatile char    rx_data = 'A';

void uart_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART2);
    rcc_periph_clock_enable(RCC_DMA1);

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO2);

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO3);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO3);

    /* Setup UART parameters. */
    // usart_set_baudrate(USART2, 115200);
    usart_set_baudrate(USART2, 19200);
    // usart_set_baudrate(USART2, 9600);
    // usart_set_baudrate(USART2, 2400);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_2);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    /* Set DMA interrupt priority */
    nvic_set_priority(NVIC_DMA1_CHANNEL7_IRQ, 16);
    nvic_enable_irq(NVIC_DMA1_CHANNEL7_IRQ);

    /* Set USART interrupt priority */
    nvic_set_priority(NVIC_USART2_IRQ, 0);
    nvic_enable_irq(NVIC_USART2_IRQ);

    /* DMA Transmit */
    for (uint16_t i = 0; i < sizeof(tx_data); i++) {
        tx_data[i] = heartbeat;
    }

    dma_channel_reset(DMA1, DMA_CHANNEL7);

    dma_set_channel_request(DMA1, DMA_CHANNEL7, 2);
    dma_set_read_from_memory(DMA1, DMA_CHANNEL7);
    dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL7);
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL7);
    dma_set_priority(DMA1, DMA_CHANNEL7, DMA_CCR_PL_MEDIUM);
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL7);

    dma_set_memory_address(DMA1, DMA_CHANNEL7, (uint32_t)tx_data);
    dma_set_memory_size(DMA1, DMA_CHANNEL7, DMA_CCR_MSIZE_8BIT);
    dma_set_number_of_data(DMA1, DMA_CHANNEL7, sizeof(tx_data));
    dma_set_peripheral_address(DMA1, DMA_CHANNEL7, (uint32_t)&USART2_TDR);
    dma_set_peripheral_size(DMA1, DMA_CHANNEL7, DMA_CCR_PSIZE_8BIT);

    /* Finally enable the USART. */
    usart_enable_tx_dma(USART2);
    usart_enable_rx_interrupt(USART2);

    usart_enable(USART2);
}

/* Transmit */
void dma1_channel7_isr(void) {
    dma_disable_channel(DMA1, DMA_CHANNEL7);

    dma_set_number_of_data(DMA1, DMA_CHANNEL7, sizeof(tx_data));

    for (uint16_t i = 0; i < sizeof(tx_data); i++) {
        tx_data[i] = heartbeat;
    }

    dma_clear_interrupt_flags(DMA1, DMA_CHANNEL7, DMA_TCIF);

    tx_done = 1;
}

void usart2_isr(void) {
    static uint8_t raw_data;
    static uint8_t prev_raw_data;
    static uint8_t heartbeat_counter = 0;

    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0)
        && ((USART_ISR(USART2) & USART_ISR_RXNE) != 0)) {
        prev_raw_data = raw_data;
        raw_data      = usart_recv(USART2);

        if (rx_done == 0) {
            if (raw_data == heartbeat) {
                heartbeat_counter++;
            } else if ((raw_data == prev_raw_data) && heartbeat_counter > 0) {
                rx_data           = (char)raw_data;
                rx_done           = 1;
                heartbeat_counter = 0;
            }
        } else {
            heartbeat_counter = 0;
        }

        // if (raw_data == heartbeat) {
        //     heartbeat_counter++;
        // } else {
        //     if (heartbeat_counter > 0) {
        //         rx_data = (char)raw_data;
        //         rx_done = 1;
        //         heartbeat_counter = 0;
        //     }
        // }
    }
}

void uart_write(const uint8_t data) {
    for (uint8_t i = sizeof(tx_data) - 8; i < sizeof(tx_data); tx_data[i++] = data)
        ;
    dma_enable_channel(DMA1, DMA_CHANNEL7);
    write_char_to_screen((const char)data, 0);
}
