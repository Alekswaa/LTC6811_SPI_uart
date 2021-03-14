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
	init_PEC15_Table();

	/* Turn on ref */
	uint8_t cfg_refon[6] = {0x04,0,0,0,0,0x04};


	/* Broadcast write start*/
	uint8_t command_message[4];
	uint8_t data_PEC[2];
    uint16_t PEC;
    uint16_t command_PEC;
    uint8_t slave;
	uint16_t PEC_data;
	uint8_t pec_data[2];
    bool everything_is_valid = true;
	while(1){
		/* enable chip select - active low */
		gpio_set_pin_level(PC25, false);

		everything_is_valid = true;
		command_message[0] = 0;
		command_message[1] = 1;
		command_PEC = PEC_calculate(command_message, 2);
		command_message[2] = (command_PEC >> 8)&0x00FF;
		command_message[3] = command_PEC&0x00FF;

		PEC_data = PEC_calculate(cfg_refon, 6);
		
		pec_data[0] = PEC_data>>8;
		pec_data[1] = PEC_data&0x00FF;

		isoSpi_send(command_message, 4);
		isoSpi_send(cfg_refon, 6);
		isoSpi_send(pec_data, 2);
		/*Broadcast write end*/

		/* disable chip select - active low */
		gpio_set_pin_level(PC25, true);

		delay_ms(2);
	}
	return 0;
}
// int main(void)
// {
// 	/* Initializes MCU, drivers and middleware */
// 	atmel_start_init();

// 	/* Replace with your application code */
// 	//uint16_t measured_voltages[12]; //12 voltage measurements per 
// 	//uint8_t* newline = "\n";
// 	//
// 	//while (1) {
// 		//wakeup_sleep();
// 		//enable_cs();
// 		//start_cell_voltages_conversion();
// 		//delay_ms(1);
// 		//get_cell_voltages(&measured_voltages);
// 		//for(int i = 0; i < 12; i++){
// 			//print_uart_ln(&measured_voltages[i], 2);
// 		//}
// 		//disable_cs();
// 		//delay_ms(3);
// 	//}
// 	uint16_t status_regs[2];
// 	uint8_t data_ss = 0xFF;
// 	wakeup_sleep();
// 	//turn_on_ref();

// 	while(1){
	
// 		// //gpio_set_pin_level(PC25, false);
// 		// read_status_ltc(&status_regs);
// 		// delay_us(10);
// 		// for(int i = 0; i < 2; i++){
// 		// 	print_uart_ln((uint8_t*)&status_regs[i*2], 4);
// 		// }

// 		wakeup_sleep();
// 		turn_on_ref();
// 		//uint8_t test[2] = {0xAA,0xAA};
// 		//isoSpi_send(&test[0],2);
// 		delay_ms(2);
		
// 	}
// }

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





