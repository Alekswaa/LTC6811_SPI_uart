#ifndef FUNCTION_DEF_H
#define FUNCTION_DEF_H

#include <stdio.h>
#include <stdint.h>

#define print_uart(str_in)	print_uart_func(str_in, sizeof(str_in)/sizeof(uint8_t))

void print_uart_func(uint8_t*, int);


#endif