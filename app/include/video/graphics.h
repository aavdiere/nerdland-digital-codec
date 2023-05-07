#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include "common.h"

#define GL_OP_COPY 0
#define GL_OP_XOR 1
#define GL_OP_AND 2
#define GL_OP_OR 3

void glClear(void);
void glPoint(uint16_t x, uint16_t y, uint16_t gl_op);
void glLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t gl_op);
void glSprite(uint8_t **sprite, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t gl_op);

#endif /* __GRAPHICS_H  */