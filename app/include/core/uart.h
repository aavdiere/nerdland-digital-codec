#ifndef __UART_H
#define __UART_H

#include "common.h"

void uart_setup(void);
void uart_write(const uint8_t *data, uint8_t size);

#endif /* __UART_H  */
