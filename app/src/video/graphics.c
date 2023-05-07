#include "video/graphics.h"
#include "video/vga.h"

void glClear(void) {
    uint32_t i;

    for (i = 0; i < 1 << 16; i++) {
        *(uint8_t *)(0x20000000 + i) = 0x00;
    }
}

/* STM32 allows bit banding
 * Bit banding maps a region of memory to a specific bit in a byte
 * The following example shows how to map bit 2 of the byte located at SRAM1 address
 * 0x20000300 to the alias region:
 *      0x22006008 = 0x22000000 + (0x300*32) + (2*4)
 *
 * Writing to address 0x22006008 has the same effect as a read-modify-write operation on bit
 * 2 of the byte at SRAM1 address 0x20000300.
 *
 * Reading address 0x22006008 returns the value (0x01 or 0x00) of bit 2 of the byte at SRAM1
 * address 0x20000300 (0x01: bit set; 0x00: bit reset).
 */
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

/* Use Bresenham algorithm to draw a line */
void glLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t gl_op) {
    int16_t dx, dy;
    int16_t D;
    int16_t x, y;
    int16_t incx, incy;
    int16_t i;

    dx   = x1 - x0;
    incx = 1;
    if (dx < 0) {
        dx   = -dx;
        incx = -1;
    }

    dy   = y1 - y0;
    incy = 1;
    if (dy < 0) {
        dy   = -dy;
        incy = -1;
    }

    x = x0;
    y = y0;
    glPoint(x, y, gl_op);

    if (dx > dy) {
        D = 2 * dy - dx;

        for (i = 0; i < dx; i++) {
            if (D >= 0) {
                y += incy;
                D += 2 * (dy - dx);
            } else {
                D += 2 * dy;
            }
            x += incx;
            glPoint(x, y, gl_op);
        }
    } else {
        D = 2 * dx - dy;

        for (i = 0; i < dy; i++) {
            if (D >= 0) {
                x += incx;
                D += 2 * (dx - dy);
            } else {
                D += 2 * dx;
            }
            y += incy;
            glPoint(x, y, gl_op);
        }
    }
}

void glRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t gl_op) {
    glLine(x0, y0, x1, y0, gl_op);
    glLine(x0, y1, x1, y1, gl_op);
    glLine(x0, y0, x0, y1, gl_op);
    glLine(x1, y0, x1, y1, gl_op);
}

void glSprite(uint8_t *sprite, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t gl_op) {
    uint16_t dx, dy;

    for (dy = 0; dy < h; dy++) {
        for (dx = 0; dx < w; dx++) {
            if ((sprite[dy * (w >> 3)] >> dx) & 0x1) {
                glPoint(x + dx, y + dy, gl_op);
            }
        }
    }
}
