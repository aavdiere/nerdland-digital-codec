#ifndef __COMMON_H
#define __COMMON_H

#include <stdbool.h>
#include <stdint.h>

#include <libopencm3/stm32/gpio.h>

#define HSYNC_PORT (GPIOA)
#define HSYNC_PIN (GPIO8)

#define H_VISIBLE 800
#define H_FRONT_PORCH 40
#define H_SYNC_PULSE 128
#define H_BACK_PORCH 88
#define H_WHOLE_LINE (H_VISIBLE + H_FRONT_PORCH + H_SYNC_PULSE + H_BACK_PORCH)

#define VSYNC_PORT (GPIOA)
#define VSYNC_PIN (GPIO0)

#define V_VISIBLE 600
#define V_FRONT_PORCH 1
#define V_SYNC_PULSE 4
#define V_BACK_PORCH 23
#define V_WHOLE_LINE (V_VISIBLE + V_FRONT_PORCH + V_SYNC_PULSE + V_BACK_PORCH)

#define VGA_RED_PORT (GPIOC)
#define VGA_RED_PIN (GPIO12)

#define VGA_GREEN_PORT (GPIOC)
#define VGA_GREEN_PIN (GPIO3)

#define VGA_BLUE_PORT (GPIOA)
#define VGA_BLUE_PIN (GPIO7)

#endif /* __COMMON_H  */
