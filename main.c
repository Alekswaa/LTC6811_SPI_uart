#include <atmel_start.h>
#include "bms.h"
#include "function_def.h" 
#include <stdlib.h>
#include <string.h>

/*LTC6811 Commands. See page 59 of datasheet. Need to add all */
#define WRCFGA  0x0001
#define RDCFGA  0x0002 //bit at index 1 is constantly 2
#define RDCVA   0x0004
#define RDAUXA  0x000C
#define RDSTATA 0x0010
#define RDSTATB 0x0012


#define REFON 1 << 2;
void write_ltc6811(uint16_t, uint8_t*, int);
void read_ltc6811(uint16_t, uint8_t*, int);


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	init_PEC15_Table();
	/* SPI configurations we might want to play with*/
	spi_m_sync_enable(&SPI_1);
	spi_m_sync_set_data_order(&SPI_1,SPI_DATA_ORDER_MSB_1ST);
	spi_m_sync_set_char_size(&SPI_1,SPI_CHAR_SIZE_8); 
	spi_m_sync_set_baudrate(&SPI_1,100000); //max frequency of the LTC6820 is 1MHz.... Should use like 900KHz because of reasons


	/*Setting values to write to CFGA*/
	uint8_t reg_cfga[6] = {0};
	reg_cfga[0] = 4;
	reg_cfga[1] = 250;
	reg_cfga[2] = 250;
	reg_cfga[3] = 250;
	reg_cfga[4] = 250;
	reg_cfga[5] = 250;

	/*Buffer register to store the read configuration after we wrote to LTC6811*/
	uint8_t cfga_read[6]= {0}; // {0} means initialize all values of the array to 1
	/*Definition of a string and printing*/
	uint8_t  str[11] = "Hello world";
	print_uart_ln(str, 11);

	/*Write to the LTC6811*/
	write_ltc6811(WRCFGA, reg_cfga, 6);

	while(1){
		/*Need to play around with the delays inbetween the commands
		* Currently, they are just randomly set. Probably not even necessary*/
		
		/*Issue a read of config register A, 6 bytes should be read*/
		read_ltc6811(RDCFGA,  cfga_read, 6);

		/*Print the values received. this function adds a new line, 
		 *print_uart() does not add the newline char. */
		print_uart_ln(cfga_read,6); 

		/* Resets all the values in the array */
		memset(cfga_read, 0, 6);

		/*If the delay is too long, LTC6811 will go to sleep 
		 * and reset configurations. Should be about 2seconds 
		 * before it goes to sleep */
		delay_us(1500); 
	}
	return 0;
}
/**
 * @brief Write values to registers in the LTC6811
 * 
 * @param command is the command see page 59 in datasheet
 * @param reg_values, values to write
 * @param num_bytes, number of bytes in the send register
 */
void write_ltc6811(uint16_t command, uint8_t* reg_values, int num_bytes ){

	/* Broadcast write start*/
	uint8_t command_message[4];
	uint16_t command_PEC, reg_PEC;
	uint8_t pec_data[2];
	command_message[0] = command >> 8;
	command_message[1] = command && 0xFF;
	command_PEC = PEC_calculate(command_message, 2);
	command_message[2] = (command_PEC >> 8);
	command_message[3] = command_PEC & 0xFF;

	reg_PEC = PEC_calculate(reg_values, num_bytes);
	pec_data[0]  = reg_PEC >> 8;
	pec_data[1]  = reg_PEC & 0x00FF;

	uint8_t* reg_values_p = malloc(num_bytes+2);
	memcpy((void*)reg_values_p, (void*)reg_values, num_bytes);
	reg_values_p[num_bytes] = pec_data[0];
	reg_values_p[num_bytes+1] = pec_data[1];

	/* Send command and values to write to register*/
	gpio_set_pin_level(PC25, false);
	isoSpi_send(command_message, 4);
	isoSpi_send(reg_values_p, num_bytes+2);
	gpio_set_pin_level(PC25, true);

	/* disable chip select - active low */
	free(reg_values_p);
}

void read_ltc6811(uint16_t command, uint8_t* receive_values, int num_bytes ){

	/* Broadcast write start*/
	uint8_t command_message[4];
    uint16_t PEC;
	uint16_t command_PEC, reg_PEC;
	uint8_t pec_data[2];
	command_message[0] = command >> 8;
	command_message[1] = command & 0xFF;
	command_PEC = PEC_calculate(command_message, 2);
	command_message[2] = (command_PEC >> 8);
	command_message[3] = command_PEC & 0x00FF;

	uint8_t* receive_value_pec = malloc(num_bytes + 2);
	memset(receive_value_pec, 0, num_bytes + 2);

	/* Send command and values to write to register*/
	gpio_set_pin_level(PC25, false);
	isoSpi_send(command_message, 4);
	isoSpi_receive(receive_value_pec, num_bytes+2);
	gpio_set_pin_level(PC25, true);
	
	memcpy(receive_values, receive_value_pec, num_bytes);
	/* disable chip select - active low */
	free(receive_value_pec);
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
	uint8_t newline[1] = "\n";
	io_write(io, newline, 1);
}





