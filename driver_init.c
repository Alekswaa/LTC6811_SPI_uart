/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <hal_init.h>
#include <hpl_pmc.h>
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hpl_spi_base.h>

struct spi_m_sync_descriptor SPI_0;

struct usart_sync_descriptor USART_1;

void SPI_0_PORT_init(void)
{
	gpio_set_pin_function(PC26, MUX_PC26C_SPI1_MISO);
	gpio_set_pin_function(PC27, MUX_PC27C_SPI1_MOSI);
	gpio_set_pin_function(PC24, MUX_PC24C_SPI1_SPCK);
	
	gpio_set_pin_function(PC28, GPIO_PIN_FUNCTION_OFF);// PIO_PC28C_SPI1_NPCS1
	gpio_set_pin_direction(PC28, GPIO_DIRECTION_OUT);
	
	gpio_set_pin_level(PC25,
	// <y> Initial level
	// <id> pad_initial_level
	// <false"> Low
	// <true"> High
	true);
	
	gpio_set_pin_level(PC28, true);

	
	gpio_set_pin_direction(PC25, GPIO_DIRECTION_OUT);
	//gpio_set_pin_function(PC25, MUX_PC25C_SPI1_NPCS0);  //NCS0, pin 133
	
	/* 1 */
	gpio_set_pin_function(PC25, GPIO_PIN_FUNCTION_OFF);
	// gpio_set_pin_function(PC25, PIN_PC25C_SPI1_NPCS0);  //NCS0, pin 133
	//gpio_set_pin_function(PC25, PIO_PC25C_SPI1_NPCS0);  //NCS0, pin 133
}

void SPI_0_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_SPI1);
}

void SPI_0_init(void)
{
	SPI_0_CLOCK_init();
	spi_m_sync_set_func_ptr(&SPI_0, _spi_get_spi_m_sync());
	spi_m_sync_init(&SPI_0, SPI1);
	SPI_0_PORT_init();
}

void delay_driver_init(void)
{
	delay_init(SysTick);
}

void USART_1_PORT_init(void)
{

	gpio_set_pin_function(PA5, MUX_PA5C_UART1_URXD1);

	gpio_set_pin_function(PA6, MUX_PA6C_UART1_UTXD1);
}

void USART_1_CLOCK_init(void)
{
	_pmc_enable_periph_clock(ID_UART1);
}

void USART_1_init(void)
{
	USART_1_CLOCK_init();
	usart_sync_init(&USART_1, UART1, _uart_get_usart_sync());
	USART_1_PORT_init();
}

void system_init(void)
{
	init_mcu();

	/* Disable Watchdog */
	hri_wdt_set_MR_WDDIS_bit(WDT);

	SPI_0_init();

	delay_driver_init();

	USART_1_init();
}
