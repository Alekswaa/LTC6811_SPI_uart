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
	//uint16_t measured_voltages[12]; //12 voltage measurements per 
	//uint8_t* newline = "\n";
	
	//while (1) {
		//wakeup_sleep();
		//enable_cs();
		//start_cell_voltages_conversion();
		//delay_ms(1);
		//get_cell_voltages(&measured_voltages);
		//for(int i = 0; i < 12; i++){
			//print_uart_ln(&measured_voltages[i], 2);
		//}
		//disable_cs();
		//delay_ms(100);
	//}
	uint16_t status_regs[2];	uint8_t data_ss = 0xFF;
while(1){	enable_cs();	//gpio_set_pin_level(PC25, false);	//read_status_ltc(&status_regs);
	delay_us(1);
	//gpio_set_pin_level(PC25, true);
	//for(int i = 0; i < 2; i++){
		//print_uart_ln((uint8_t*)&status_regs[i*2], 4);
	//}
	disable_cs();
	/*enable_cs();
	isoSpi_send(&data_ss, 1);
	disable_cs();*/
	delay_us(100);
}
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

	io_write(io, (uint8_t*) "\n", 1);
}





