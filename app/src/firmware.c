#include <string.h>
#include <stdio.h>

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

extern volatile uint8_t usb_done;
extern uint8_t pressed[6];

volatile uint8_t ghost_typer = 0;

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

    const char *hello_world[] = {
        "0\n",
        "1\n",
        "2\n",
        "3\n",
        "4\n",
        "5\n",
        "6\n",
        "7\n",
        "8\n",
        "9\n",
        "10\n",
        "11\n",
        "12\n",
        "13\n",
        "14\n",
        "15\n",
        "16\n",
        "17\n",
        "18\n",
        "19\n",
        "20\n",
        "21\n",
        "22\n",
        "23\n",
        "24\n",
        "25\n",
        "26\n",
        "27\n",
        "28\n",
        "29\n",
        "30\n",
        "31\n"
    };
    uint8_t     i           = 0;
    uint8_t     j           = 0;

    /* Infinte loop */
    for (;;) {
        if (rx_done == 1) {
            rx_done = 0;
            write_char_to_screen(rx_data, 1);
        }

        if (ghost_typer == 0) {
            if (usb_done == 1) {
                usb_done = 0;
                i = 0;
                for (uint8_t k = 0; k < sizeof(pressed); k++) {
                    if (pressed[k] > 0) {
                        i++;
                    }
                }
                tx_done = 1;
            }

            if (tx_done == 1) {
                if (i > 0) {
                    tx_done = 0;
                    uart_write(pressed[--i]);
                }
            }
        } else if (ghost_typer == 255) {
            i = 0;
            j = 0;
            ghost_typer = 1;
        } else {
            if (tx_done == 1) {
                tx_done = 0;
                if (i < strlen(hello_world[j])) {
                    uart_write(hello_world[j][i]);
                    i += 1;
                }
            }

            if (system_get_ticks() - start_time >= 1000) {
                i = 0;
                j = (j + 1) % 32;
                tx_done = 0;
                uart_write(hello_world[j][i]);
                i += 1;

                start_time = system_get_ticks();
            }
        }


        usb_process();
        // if (system_get_ticks() - usb_start_time >= 10) {

        //     usb_start_time = system_get_ticks();
        // }
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
