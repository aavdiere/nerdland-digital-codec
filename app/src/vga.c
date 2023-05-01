#include "vga.h"

#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/timer.h>

#include <libopencm3/cm3/nvic.h>

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
    timer_set_period(TIM1, H_WHOLE_LINE - 1);

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
    timer_set_oc_value(TIM1, TIM_OC2, H_SYNC_PULSE + H_BACK_PORCH);
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
}

/*
 * This is generated at the end of the horizontal back porch.
 * Check if we are in the drawable area, if so, start the DMA to output
 * a single line from the frame buffer through SPI.
 * After which, clear the interrupt flag for the corresponding channel.
 */
void tim1_cc_isr(void) {
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
}

/*
 * This is generated at the end of the vertical back porch (to signal new frame).
 * We can use this to signal that we are in the drawable area.
 * After which, clear the interrupt flag for the corresponding channel.
 */
void tim2_isr(void) {
    timer_clear_flag(TIM2, TIM_SR_CC2IF);
}

uint8_t fb[V_VISIBLE][H_VISIBLE / 8] __attribute__((aligned(4)));

void spi_green_setup(void) {
    /* Setup GPIO for MOSI for green channel (alternate function 5) */
    gpio_mode_setup(VGA_GREEN_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, VGA_GREEN_PIN);
    gpio_set_af(VGA_GREEN_PORT, GPIO_AF5, VGA_GREEN_PIN);
    gpio_set_output_options(VGA_GREEN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, VGA_GREEN_PIN);

    /* Setup DMA */
    /* Disable channel as it will be enabled by the HSYNC/VSYNC pulses */
    dma_disable_channel(DMA1, DMA_CHANNEL5);

    /* Peripheral is SPI buffer */
    dma_set_peripheral_address(DMA1, DMA_CHANNEL5, (uint32_t)&SPI2_DR);
    /* Set initial address to beginning of frame */
    dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)&fb[0][0]);
    /* Read 800 bits = 100 bytes from memory */
    dma_set_read_from_memory(DMA1, DMA_CHANNEL5);
    dma_set_number_of_data(DMA1, DMA_CHANNEL5, H_VISIBLE / 8);
    /* Keep peripheral address the same */
    dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL5);
    /* Increment the memory address */
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL5);
    /* Set size to be byte */
    dma_set_peripheral_size(DMA1, DMA_CHANNEL5, DMA_CCR_PSIZE_8BIT);
    dma_set_memory_size(DMA1, DMA_CHANNEL5, DMA_CCR_MSIZE_8BIT);
    /* Low priority */
    dma_set_priority(DMA1, DMA_CHANNEL5, DMA_CCR_PL_LOW);
    /* Memory to peripheral */
    dma_enable_mem2mem_mode(DMA1, DMA_CHANNEL5);

    /* Link DMA for SPI */
    spi_enable_tx_dma(SPI2);

    /* Setup SPI */
    spi_disable(SPI2);
    spi_init_master(SPI2,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_2,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_2,
                    SPI_CR1_MSBFIRST);

    /* Set as transmit only */
    spi_set_bidirectional_transmit_only_mode(SPI2);
    spi_set_unidirectional_mode(SPI2);

    /* Set 8 bit data size */
    spi_set_data_size(SPI2, SPI_CR2_DS_8BIT);

    /*
     * Set NSS management to software.
     *
     * Note:
     * Setting nss high is very important, even if we are controlling the GPIO
     * ourselves this bit needs to be at least set to 1, otherwise the spi
     * peripheral will not send any data out.
     */
    spi_enable_software_slave_management(SPI2);
    spi_set_nss_high(SPI2);

    spi_disable_crc(SPI2);

    /* Enable SPI */
    spi_enable(SPI2);

    /* Setup interupt functions at end of transmission */
    nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ);
    nvic_set_priority(NVIC_DMA1_CHANNEL5_IRQ, 0);

    /* Enable end of transfer interrupt */
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);
}

/*
 * This interrupt is generated at the end of every line.
 * It will increment the line number and set the corresponding line pointer
 * in the DMA register.
 */
void dma1_channel5_isr(void) {
    return;
}

void vga_setup(void) {
    spi_green_setup();
    hsync_setup();
    vsync_setup();
}
