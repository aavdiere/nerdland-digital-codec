#include <string.h>

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
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

void clear_screen(void) {
    for (uint8_t i = 0; i < 22; i++) {
        glText("                        ", 8, 8 + i * 26, GL_OP_AND);
    }
    // uint32_t border = 4;
    // glRectangle(0 + border, border, 399 - border / 2, 599 - border, GL_OP_OR);
    // glRectangle(400 + border / 2, border, 799 - border, 599 - border, GL_OP_OR);

    glRectangle(0, 0, 799, 599, GL_OP_OR);
    glLine(400, 0, 400, 799, GL_OP_OR);
}

uint8_t px = 0;
uint8_t py = 0;

void write_char_to_screen(const char ch) {
    const struct font_config config = font_ibm_config;
    const uint8_t           *font   = font_ibm;

    if (ch == '\n') {
        px = 0;
        py++;
    } else {
        glChar(ch, config, font, 8 + config.char_width_px * px, 8 + config.sprite_height_px * py, GL_OP_OR);
        px++;
    }

    if (px >= 24) {
        px = 0;
        py++;
    }

    if (py >= 22) {
        py = 0;
        clear_screen();
    }
}

void write_text_to_screen(const char *str) {
    for (uint16_t i = 0; i < strlen(str); i++) {
        write_char_to_screen(str[i]);
    }
}

int main(void) {
    system_setup();
    gpio_setup();
    vga_setup();

    glClear();
    clear_screen();

    uint64_t start_time = system_get_ticks();

    /* Infinte loop */
    for (;;) {
        // if (true){
        if (system_get_ticks() - start_time >= 1000 / 5) {
            gpio_toggle(LED_PORT, LED_PIN);

            write_text_to_screen("Hello, World!\n");

            start_time = system_get_ticks();
        }
    }

    // Never return
    return 0;
}
