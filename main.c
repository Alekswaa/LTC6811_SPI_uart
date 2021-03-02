#include <atmel_start.h>
#include "bms.h"


/*
	Implements printing function
*/
#include "function_def.h" 


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Replace with your application code */
	uint16_t measured_voltages[12]; //12 voltage measurements per 
	uint8_t* newline = "\n";
	
	while (1) {
		print_uart(newline);
		print_uart(10);
		print_uart(newline);
		start_cell_voltages_conversion();
		delay_ms(20);
		get_cell_voltages(&measured_voltages);
		for(int i = 0; i < 12; i++){
			print_uart(&measured_voltages[i]);
			print_uart(newline);
		}
		delay_ms(100);
	}
}


void print_uart_func(uint8_t* string, int len)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_1, &io);
	usart_sync_enable(&USART_1);

	io_write(io, string, len);
}





