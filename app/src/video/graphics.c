#include "video/graphics.h"
#include "video/vga.h"

void glClear(void) {
    uint16_t x, y;

    for (y = 0; y < V_VISIBLE; y++) {
        for (x = 0; x < H_VISIBLE / 8; x++) {
            frame_buffer[y][x] = 0xFF;
        }
    }
}

void glPoint(uint16_t x, uint16_t y, uint16_t gl_op) {
    /* Test for points outside of screen */
    if (x >= H_VISIBLE || y >= V_VISIBLE)
        return;

    uint32_t address = (uint32_t)frame_buffer[y];

    uint32_t bit_band_base = 0x22000000;
    uint32_t byte_offset   = (address & 0x0003ffff) + (x >> 3);
    uint32_t bit_number    = x & 0x0F;

    uint8_t *bit_band = (uint8_t *)(bit_band_base + (byte_offset << 5) + (bit_number << 2));

    *bit_band |= 1;

    return;

    /*
     * Pixels are grouped by byte (8 bits)
     * Calculate x position in the framebuffer
     * Calculate offset position in byte
     */
    uint16_t dx     = x >> 3;
    uint16_t offset = x & 0x0F;

    /* Create mask for offset position */
    uint8_t mask = 1 << offset;

    switch (gl_op) {
        case GL_OP_COPY:
        case GL_OP_OR:
            frame_buffer[y][dx] |= mask;
            break;

        case GL_OP_XOR:
            frame_buffer[y][dx] ^= mask;
            break;

        case GL_OP_AND:
            frame_buffer[y][dx] &= mask;
            break;
    }
}

void glLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t gl_op) {}

void glSprite(uint8_t **sprite, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t gl_op) {}
