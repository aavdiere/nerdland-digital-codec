#include "core/system.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

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

    /* Configure TIM2 Channel 2 */
    /* Output compare value */
    timer_set_oc_value(TIM2, TIM_OC1, V_SYNC_PULSE);
    /* Set PWM mode
     * - PWM1: high if counter < output compare
     * - PWM2: low if counter < output compare
     */
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
    /* Output timer channel on the pin with correct alternate function */
    timer_enable_oc_output(TIM2, TIM_OC1);

    /* Enable the counter */
    timer_enable_break_main_output(TIM2);
    timer_enable_counter(TIM2);
}

void timer_setup(void) {
    hsync_setup();
    vsync_setup();
}
