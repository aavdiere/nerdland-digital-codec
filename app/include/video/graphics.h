#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include "common.h"

/* Font 20 pixels wide, 32 pixels high */
extern const uint8_t font_menlo[9120];

#define GL_OP_COPY 0
#define GL_OP_XOR 1
#define GL_OP_AND 2
#define GL_OP_OR 3

/* 16 + x as there are 2 bytes of settling region at the beginning of the line */
#define PIXEL(x, y) (uint8_t *)(0x22000000 \
    | (((uint32_t)y * 104 + ((uint32_t)(x + 16) >> 3)) << 5) \
    | (((uint32_t)x & 0b111) << 2) \
)

void glClear(void);
void glPoint(uint16_t x, uint16_t y, uint16_t gl_op);
void glLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t gl_op);
void glRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t gl_op);
void glSprite(const uint8_t *sprite, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t gl_op);
void glChar(const char ch, uint16_t x, uint16_t y, uint16_t gl_op);
void glText(const char *str, uint16_t x, uint16_t y, uint16_t gl_op);

#endif /* __GRAPHICS_H  */
