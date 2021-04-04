#include <atmel_start.h>
#include "bms.h"
#include "function_def.h" 
#include <stdlib.h>
#include <string.h>

#define test


void write_ltc6811(uint16_t, uint8_t*, int);
void read_ltc6811(uint16_t, uint8_t*, int);


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	/* SPI configurations we might want to play with*/
	spi_m_sync_enable(&SPI_1);
	spi_m_sync_set_data_order(&SPI_1, SPI_DATA_ORDER_MSB_1ST);
	spi_m_sync_set_char_size( &SPI_1, SPI_CHAR_SIZE_8); 
	spi_m_sync_set_baudrate(  &SPI_1, 100000); //max frequency of the LTC6820 is 1MHz.... Should use like 900KHz because of reasons

	bool first_time = true;

	// uint8_t str[11] = "Hello world";
	//print_uart_ln(str, 11);

	//bms_init_ltc6811();
	//delay_us(100);
	//uint16_t* voltage_buffer = malloc(LTC6811_DAISYCHAIN_ALL_VOLTAGES*sizeof(uint16_t));
	//uint16_t* voltage_buffer = malloc(3*sizeof(uint16_t));
	uint16_t voltage_buffer[12]={0};
	uint8_t hc, lc;
	uint16_t delimiter = 0xAAAA;
	while(1)
	{
		wake_up_ltc();
		bms_read_cell_voltages(voltage_buffer);
		
		for(int i = 0; i < 12; i++){
			hc = voltage_buffer[i]>>8;
			lc = voltage_buffer[i]&0xFF;
			print_uart(&hc, 1);
			print_uart(&lc, 1);
		}
		print_uart((uint8_t*)&delimiter, 2);
	}
	return 0;
}


/**
 * \brief Print chars on ser port through uart
 * \param[in] string Pointer to char array
 * \param[in] len 	 Length of char array
 */
void print_uart(uint8_t* string, int len)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_1, &io);
	usart_sync_enable(&USART_1);

	io_write(io, string, len);
}
/**
 * \brief Print chars on ser port through uart. 
 * Adds newline at the end of print
 * \param[in] string Pointer to char array
 * \param[in] len 	 Length of char array
 */
void print_uart_ln(uint8_t* string, int len)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_1, &io);
	usart_sync_enable(&USART_1);
	print_uart(string, len);
	uint8_t newline[2] = "\r\n";
	io_write(io, newline, 2);
}





