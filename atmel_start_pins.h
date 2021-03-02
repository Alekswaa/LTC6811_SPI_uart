/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMV71 has 4 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3

#define PA5 GPIO(GPIO_PORTA, 5) //UART
#define PA6 GPIO(GPIO_PORTA, 6) //UART
#define PC24 GPIO(GPIO_PORTC, 24) //SCK
#define PC26 GPIO(GPIO_PORTC, 26) //MISO
#define PC27 GPIO(GPIO_PORTC, 27)

#define PC25 GPIO(GPIO_PORTC, 25) // Chip select SPI 6820
#define PC28 GPIO(GPIO_PORTC, 28) //Enable 6820



#endif // ATMEL_START_PINS_H_INCLUDED
