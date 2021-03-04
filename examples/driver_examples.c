/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"


/**
 * Example of using SPI_1 to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_1[12] = "Hello World!";
static uint8_t receive_buffer_spi[12];

void SPI_1_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_1, &io);
	spi_m_sync_enable(&SPI_1);

	struct spi_xfer spi_transmit_buffer;
	spi_transmit_buffer.txbuf = example_SPI_1;
	spi_transmit_buffer.size = 12;
	spi_transmit_buffer.rxbuf  = receive_buffer_spi;
	
	gpio_set_pin_level(PC25, false); //set cs low. Needs to be done manually yes. 
	// GPIO_set(CS_PIN_test);

	spi_m_sync_transfer(&SPI_1, &spi_transmit_buffer);
	//io_write(io, example_SPI_1, 12);
	gpio_set_pin_level(PC25, true); //set cs high.
	// GPIO_clr(CS_PIN_test);
}

void delay_example(void)
{
	delay_ms(5000);
}

void USART_1_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_1, &io);
	usart_sync_enable(&USART_1);

	io_write(io, (uint8_t *)"Hello World!", 12);
}
