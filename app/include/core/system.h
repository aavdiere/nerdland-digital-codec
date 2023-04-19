#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "common.h"

#define CPU_FREQ      (80000000)
#define SYSTICK_FREQ  (1000)

void system_setup(void);
uint64_t system_get_ticks(void);

#endif /* __SYSTEM_H  */
