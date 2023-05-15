#include <string.h>

// TODO: Cleanup
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>

#include <libopencm3/cm3/nvic.h>

// for error handler interrupt
#include <libopencm3/cm3/cortex.h>

#include "core/system.h"
#include "core/uart.h"
#include "video/graphics.h"
#include "video/vga.h"

#include "usb/usb.h"

extern volatile uint8_t rx_done;
extern volatile char    rx_data;

extern volatile uint8_t tx_done;
extern uint8_t          tx_data[8];

int main(void) {
    system_setup();
    vga_setup();
    uart_setup();

    gpio_mode_setup(USB_VBUS_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, USB_VBUS_EN_PIN);
    gpio_set_output_options(USB_VBUS_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, USB_VBUS_EN_PIN);

    usb_init();

    glClear();
    clear_full_screen();

    uint64_t start_time     = system_get_ticks();
    uint64_t usb_start_time = 0;

    const char *hello_world = "Hello, World!\n";
    uint8_t     i           = 0;

    /* Infinte loop */
    for (;;) {
        if (rx_done == 1) {
            rx_done = 0;
            write_char_to_screen(rx_data, 1);
        }

        if (tx_done == 1) {
            tx_done = 0;
            if (i < strlen(hello_world)) {
                uart_write(hello_world[i++]);
            }
        }

        if (system_get_ticks() - start_time >= 1000) {
            i = 0;
            uart_write(hello_world[i++]);

            start_time = system_get_ticks();
        }

        if (system_get_ticks() - usb_start_time >= 1000) {
            usb_process();

            usb_start_time = system_get_ticks();
        }
    }

    // Never return
    return 0;
}

void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    cm_disable_interrupts();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
