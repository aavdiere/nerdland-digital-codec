#include "video/vga.h"

#include <libopencm3/stm32/timer.h>

#define raw_buffer (0x20000000)

// volatile uint8_t (*raw_buffer)[V_VISIBLE][(H_KEEPOUT + H_VISIBLE + H_KEEPOUT) / 8] =
//     (volatile uint8_t(*)[V_VISIBLE][(H_KEEPOUT + H_VISIBLE + H_KEEPOUT) / 8])(0x20000000);

static volatile uint8_t  vflag    = 0;
static volatile uint32_t vline[3] = {0, 0, 0};

void vga_setup(void) {
    buffer_setup();

    hsync_setup();
    vsync_setup();

    // color_channel_setup(vga_red);
    color_channel_setup(vga_green);
    // color_channel_setup(vga_blue);
}

void buffer_setup(void) {
    uint32_t i;

    for (i = 0; i < 1 << 16; i++) {
        *(uint8_t *)(raw_buffer + i) = 0x00;
    }
}

void hsync_setup(void) {
    /* Enable TIM1 clock */
    rcc_periph_clock_enable(RCC_TIM1);

    /* Configure TIM1 (for sync pulse)
     * Timer global mode:
     * - No divider
     * - Alignment edge
     * - Direction up
     */
    timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    /* Pixel clock should be 40MHz from the 80MHz system clock, set prescaler to / 2 */
    timer_set_prescaler(TIM1, 1);
    /* Maximum counter value of the timer, value AFTER which counter rolls over */
    /* Some manual tuning required */
    timer_set_period(TIM1, H_WHOLE_LINE - 1 + 2);

    /* Configure TIM1 Channel 1 */
    /* Output compare value */
    timer_set_oc_value(TIM1, TIM_OC1, H_SYNC_PULSE);
    /* Set PWM mode
     * - PWM1: high if counter < output compare
     * - PWM2: low if counter < output compare
     */
    timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);
    /* Output timer channel on the pin with correct alternate function */
    timer_enable_oc_output(TIM1, TIM_OC1);

    /* Configure TIM1 Channel 2 */
    /* Output compare value */
    /* Some manual tuning required */
    timer_set_oc_value(TIM1, TIM_OC2, H_SYNC_PULSE + H_BACK_PORCH - H_KEEPOUT - 16);
    /* Set PWM mode
     * - PWM1: high if counter < output compare
     * - PWM2: low if counter < output compare
     */
    timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM1);

    /* Set timer as master */
    timer_set_master_mode(TIM1, TIM_CR2_MMS_UPDATE);

    /* Enable the counter */
    timer_enable_break_main_output(TIM1);
    timer_enable_counter(TIM1);

    /* Setup interrupt
     * This timer will generate an interrupt that will be used to fire
     * the DMA request to start sending pixels through the SPI.
     */
    nvic_set_priority(NVIC_TIM1_CC_IRQ, 0);
    nvic_enable_irq(NVIC_TIM1_CC_IRQ);
    timer_enable_irq(TIM1, TIM_DIER_CC2IE);
}

/*
 * This is generated at the end of the horizontal back porch.
 * Check if we are in the drawable area, if so, start the DMA to output
 * a single line from the frame buffer through SPI.
 * After which, clear the interrupt flag for the corresponding channel.
 */
void tim1_cc_isr(void) {
    if (vflag == 1) {
        /* This needs to happen FAST, so raw register is required */
        // *(uint32_t *)((DMA2) + 0x08 + (0x14 * ((DMA_CHANNEL2) - 1))) |= DMA_CCR_EN;
        // *(uint32_t *)((DMA1) + 0x08 + (0x14 * ((DMA_CHANNEL5) - 1))) |= DMA_CCR_EN;
        // *(uint32_t *)((DMA1) + 0x08 + (0x14 * ((DMA_CHANNEL3) - 1))) |= DMA_CCR_EN;

        // DMA_CCR(vga_red.dma, vga_red.dma_channel) |= DMA_CCR_EN;
        // DMA_CCR(vga_green.dma, vga_green.dma_channel) |= DMA_CCR_EN;
        // DMA_CCR(vga_blue.dma, vga_blue.dma_channel) |= DMA_CCR_EN;

        // dma_enable_channel(vga_red.dma, vga_red.dma_channel);
        dma_enable_channel(vga_green.dma, vga_green.dma_channel);
        // dma_enable_channel(vga_blue.dma, vga_blue.dma_channel);
    }
    timer_clear_flag(TIM1, TIM_SR_CC2IF);
}

void vsync_setup(void) {
    /* Enable TIM2 clock */
    rcc_periph_clock_enable(RCC_TIM2);

    /* Set timer as slave */
    timer_slave_set_mode(TIM2, TIM_SMCR_SMS_ECM1);
    timer_slave_set_trigger(TIM2, TIM_SMCR_TS_ITR0);

    /* Configure TIM2 (for sync pulse)
     * Timer global mode:
     * - No divider
     * - Alignment edge
     * - Direction up
     */
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    /* No prescaler */
    timer_set_prescaler(TIM2, 0);
    /* Maximum counter value of the timer, value AFTER which counter rolls over */
    timer_set_period(TIM2, V_WHOLE_LINE - 1);

    /* Configure TIM2 Channel 1 */
    /* Output compare value */
    timer_set_oc_value(TIM2, TIM_OC1, V_SYNC_PULSE);
    /* Set PWM mode
     * - PWM1: high if counter < output compare
     * - PWM2: low if counter < output compare
     */
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
    /* Output timer channel on the pin with correct alternate function */
    timer_enable_oc_output(TIM2, TIM_OC1);

    /* Configure TIM2 Channel 2 */
    /* Output compare value */
    timer_set_oc_value(TIM2, TIM_OC2, V_SYNC_PULSE + V_BACK_PORCH);
    /* Set PWM mode
     * - PWM1: high if counter < output compare
     * - PWM2: low if counter < output compare
     */
    timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_PWM1);

    /* Enable the counter */
    timer_enable_break_main_output(TIM2);
    timer_enable_counter(TIM2);

    /* Setup interrupt
     * This interrupt will set a variable indicating that
     * the scanning is within a valid frame and the DMA can
     * start sending pixels to the screen.
     */
    nvic_set_priority(NVIC_TIM2_IRQ, 0);
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(TIM2, TIM_DIER_CC2IE);
}

/*
 * This is generated at the end of the vertical back porch (to signal new frame).
 * We can use this to signal that we are in the drawable area.
 * After which, clear the interrupt flag for the corresponding channel.
 */
void tim2_isr(void) {
    vflag = 1;
    timer_clear_flag(TIM2, TIM_SR_CC2IF);
}

void color_channel_setup(struct color_channel_t color_channel) {
    /* Enable DMA clock */
    rcc_periph_clock_enable(color_channel.rcc_dma);

    /* Set DMA interrupt priority */
    nvic_set_priority(color_channel.dma_irqn, 0);
    nvic_enable_irq(color_channel.dma_irqn);

    /* Enable GPIO clock */
    rcc_periph_clock_enable(color_channel.rcc_gpio);

    /* Setup GPIO for MOSI only */
    gpio_mode_setup(color_channel.gpio_port, GPIO_MODE_AF, GPIO_PUPD_NONE, color_channel.gpio_pin);
    gpio_set_af(color_channel.gpio_port, color_channel.gpio_af, color_channel.gpio_pin);
    gpio_set_output_options(
        color_channel.gpio_port, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, color_channel.gpio_pin);

    /* Enable SPI clock */
    rcc_periph_clock_enable(color_channel.rcc_spi);

    /* Setup SPI as master */
    spi_set_master_mode(color_channel.spi);
    /* Has to be set up as full duplex (even though only transmit is used) */
    spi_set_full_duplex_mode(color_channel.spi);
    /* Set as transmit only */
    spi_set_bidirectional_transmit_only_mode(color_channel.spi);
    /* Set 8 bit data size */
    spi_set_data_size(color_channel.spi, SPI_CR2_DS_8BIT);
    /* Clock polarity */
    spi_set_clock_polarity_0(color_channel.spi);
    /* Clock phase */
    spi_set_clock_phase_1(color_channel.spi);
    /* This has to be enabled, otherwise it doesn't work */
    spi_enable_software_slave_management(color_channel.spi);
    /* Set it high */
    spi_set_nss_high(color_channel.spi);
    /* Use 40MHz clock (div 2) */
    spi_set_baudrate_prescaler(color_channel.spi, SPI_CR1_BR_FPCLK_DIV_2);
    /* MSB first */
    spi_send_msb_first(color_channel.spi);
    /* Disable cyclic check */
    spi_disable_crc(color_channel.spi);

    /* Set DMA interrupt priority */
    nvic_set_priority(color_channel.spi_irqn, 0);
    nvic_enable_irq(color_channel.spi_irqn);

    /* Setup DMA */
    dma_channel_reset(color_channel.dma, color_channel.dma_channel);

    /* Disable channel as it will be enabled by the HSYNC/VSYNC pulses */
    dma_disable_channel(color_channel.dma, color_channel.dma_channel);

    /* Select specific DMA stream */
    dma_set_channel_request(color_channel.dma, color_channel.dma_channel, color_channel.dma_stream);
    /* From memory to peripheral */
    dma_set_read_from_memory(color_channel.dma, color_channel.dma_channel);
    /* Keep peripheral address the same */
    dma_disable_peripheral_increment_mode(color_channel.dma, color_channel.dma_channel);
    /* Increment the memory address */
    dma_enable_memory_increment_mode(color_channel.dma, color_channel.dma_channel);
    /* Medium priority */
    dma_set_priority(color_channel.dma, color_channel.dma_channel, DMA_CCR_PL_MEDIUM);
    /* Enable end of transfer interrupt, this will be used to disable the DMA */
    dma_enable_transfer_complete_interrupt(color_channel.dma, color_channel.dma_channel);

    /* Set initial address to beginning of frame */
    dma_set_memory_address(color_channel.dma, color_channel.dma_channel, (uint32_t)raw_buffer);
    /* Set size to be byte */
    dma_set_memory_size(color_channel.dma, color_channel.dma_channel, DMA_CCR_MSIZE_8BIT);
    dma_set_number_of_data(
        color_channel.dma, color_channel.dma_channel, (H_KEEPOUT + H_VISIBLE) / 8);
    /* Peripheral is SPI buffer */
    dma_set_peripheral_address(
        color_channel.dma, color_channel.dma_channel, color_channel.spi_address);
    /* Set size to be byte */
    dma_set_peripheral_size(color_channel.dma, color_channel.dma_channel, DMA_CCR_PSIZE_8BIT);

    /* Enable SPI TX DMA */
    spi_enable_tx_dma(color_channel.spi);
    /* Enable SPI */
    spi_enable(color_channel.spi);
}

/* Red interrupt
 * This interrupt is generated at the end of every line.
 * It will increment the line number and set the corresponding line pointer
 * in the DMA register.
 */
void dma2_channel2_isr(void) {
    dma_disable_channel(DMA2, DMA_CHANNEL2);

    vline[0]++;

    if (vline[0] == V_VISIBLE) {
        vflag = vline[0] = 0;
        dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)raw_buffer);
    } else {
        dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)(raw_buffer + vline[0] * 104));
    }

    /* Number of data points needs to be reset as it is decremented
     * internally during each transmit.
     */
    dma_set_number_of_data(DMA2, DMA_CHANNEL2, (H_KEEPOUT + H_VISIBLE) / 8);

    dma_clear_interrupt_flags(DMA2, DMA_CHANNEL2, DMA_TCIF);
}

/* Green interrupt
 * This interrupt is generated at the end of every line.
 * It will increment the line number and set the corresponding line pointer
 * in the DMA register.
 */
void dma1_channel5_isr(void) {
    dma_disable_channel(DMA1, DMA_CHANNEL5);

    vline[1]++;

    if (vline[1] == V_VISIBLE) {
        vflag = vline[1] = 0;
        dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)raw_buffer);
    } else {
        dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)(raw_buffer + vline[1] * 104));
    }

    /* Number of data points needs to be reset as it is decremented
     * internally during each transmit.
     */
    dma_set_number_of_data(DMA1, DMA_CHANNEL5, (H_KEEPOUT + H_VISIBLE) / 8);

    dma_clear_interrupt_flags(DMA1, DMA_CHANNEL5, DMA_TCIF);
}

/* Blue interrupt
 * This interrupt is generated at the end of every line.
 * It will increment the line number and set the corresponding line pointer
 * in the DMA register.
 */
void dma1_channel3_isr(void) {
    dma_disable_channel(DMA1, DMA_CHANNEL3);

    vline[2]++;

    if (vline[2] == V_VISIBLE) {
        vflag = vline[2] = 0;
        dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)raw_buffer);
    } else {
        dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)(raw_buffer + vline[2] * 104));
    }

    /* Number of data points needs to be reset as it is decremented
     * internally during each transmit.
     */
    dma_set_number_of_data(DMA1, DMA_CHANNEL3, (H_KEEPOUT + H_VISIBLE) / 8);

    dma_clear_interrupt_flags(DMA1, DMA_CHANNEL3, DMA_TCIF);
}
