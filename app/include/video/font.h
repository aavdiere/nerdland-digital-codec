#ifndef __FONT_H
#define __FONT_H

#include "common.h"

struct font_config {
    uint8_t sprite_width_px;
    uint8_t sprite_height_px;
    uint8_t char_width_px;
};

/* Font 20 pixels wide, 32 pixels high */
extern const uint8_t            font_menlo[9120];
extern const struct font_config font_menlo_config;

extern const uint8_t            font_ibm[6080];
extern const struct font_config font_ibm_config;

#endif /* __FONT_H  */
