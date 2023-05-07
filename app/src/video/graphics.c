#include "video/graphics.h"
#include "video/vga.h"

void glClear(void) {
    uint32_t i;

    for (i = 0; i < 1 << 16; i++) {
        *(uint8_t *)(0x20000000 + i) = 0x00;
    }
}

void glPoint(uint16_t x, uint16_t y, uint16_t gl_op) {
    /* Test for points outside of screen */
    if (x >= H_VISIBLE || y >= V_VISIBLE)
        return;

    switch (gl_op) {
        case GL_OP_COPY:
        case GL_OP_OR:
            *PIXEL(x, y) |= 1;
            break;

        case GL_OP_XOR:
            *PIXEL(x, y) ^= 1;
            break;

        case GL_OP_AND:
            *PIXEL(x, y) &= 1;
            break;

    }
}

void glLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t gl_op) {}

void glSprite(uint8_t **sprite, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t gl_op) {}
