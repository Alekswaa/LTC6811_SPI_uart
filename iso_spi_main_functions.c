#include <stdio.h>
#include <stdint.h>
#include "bms.h"
#include <atmel_start.h>
#include "function_def.h"
#include "PEC.h"


/**
 * @brief Write values to registers in the LTC6811
 * 
 * @param command, the command for the write see page 59 in datasheet
 * @param reg_values, values to write
 * @param num_bytes, number of bytes in the send register
 */
void write_to_ltc6811(uint16_t command, uint8_t* reg_values, int num_bytes)
{
	uint8_t pec_data[2];
	uint16_t reg_PEC = PEC_calculate(reg_values, num_bytes);
	pec_data[0]  = reg_PEC >> 8;
	pec_data[1]  = reg_PEC & 0x00FF;

	uint8_t* reg_values_p = malloc(num_bytes + 2);
	memcpy((void*)reg_values_p, (void*)reg_values, num_bytes);
	reg_values_p[num_bytes] = pec_data[0];
	reg_values_p[num_bytes+1] = pec_data[1];

	/* Send command and values to write to register*/
	bms_enable_cs();

	bms_send_command(command, true);
	isoSpi_send(reg_values_p, num_bytes+2);

	bms_disable_cs();

	/* ADD VERIFY PEC */

	/* disable chip select - active low */
	free(reg_values_p);
}

void read_data_from_ltc6811(uint16_t command, uint8_t* receive_values, int num_bytes, bool chip_select)
{

	uint8_t* receive_value_pec = malloc(num_bytes + 2);
	memset(receive_value_pec, 0, num_bytes + 2);

	/* Send command and values to write to register*/
	if(chip_select)
		bms_enable_cs();

	bms_send_command(command, false);
	isoSpi_receive(receive_value_pec, num_bytes+2);

	if(chip_select)
		bms_disable_cs();
	
	/* ADD VERIFY PEC */
	memcpy(receive_values, receive_value_pec, num_bytes);

	/* disable chip select - active low */
	free(receive_value_pec);
}

/**
 * \brief IsoSPI send command to LTC6811 through LTC6820
 * \param[in] command The 10 bit command that the LTC6811 should receive
 */
void bms_send_command(uint16_t command, bool chip_select)
{
	uint8_t message[4];
    uint16_t PEC;
    message[0] = command >> 8;
	message[1] = command & 0xFF;
    PEC = PEC_calculate(message, 2);
    message[2] = PEC >> 8;
    message[3] = PEC & 0xFF;
	if(chip_select)
		bms_enable_cs();
    isoSpi_send(message, 4);
	if(chip_select)
		bms_disable_cs();
}


/**
 * \brief Transmit and receive data through SPI1 (doesn't activte CS)
 * \param[in] transfer_data Pointer to uint8_t array containing the data to send
 * \param[in] size 	 Number of bytes to send and read. 
 * \param[out] receive_data  Pointer for output data using uint8_t data
 * @returns 0 if fine, -1 if wrong PEC
 */
void isoSpi_send(uint8_t *transfer_data, int size){
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_1, &io);
	io_write(io, transfer_data, size);
}

void isoSpi_receive(uint8_t *receive_data, int num_bytes){
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_1, &io);
	spi_m_sync_enable(&SPI_1);

	uint8_t* nothing = malloc(num_bytes);
	memset(nothing, 0xFF, num_bytes);

	struct spi_xfer spi_transmit_buffer;
	spi_transmit_buffer.size  = num_bytes;
	spi_transmit_buffer.rxbuf = receive_data;
	spi_transmit_buffer.txbuf = nothing;

	spi_m_sync_transfer(&SPI_1, &spi_transmit_buffer);
	
	free(nothing);
	
}

/**
 * \brief Transmit and receive data from LTC6811. PEC is verified
 * \param[in] data Pointer to char array containing PEC
 * \param[in] len  Number of bytes in the PEC
 * @returns the 2 PEC bytes
 */
uint16_t PEC_calculate(uint8_t *data , int len){
	
    unsigned int remainder, address;
    remainder = 16;/*PEC seed*/
    int i;
    for (i = 0; i < len; i++){
        address = ((remainder >> 7) ^ data[i]) & 0xff;/*calculate PEC table address*/
         remainder = (remainder << 8 ) ^ crc15Table[address];  //lookup table
		//remainder = (remainder << 8 ) ^ pec15Table[address]; //values calculated
    }
    return(remainder*2);/*The CRC15 has a 0 in the LSB so the final value must be multiplied by 2*/
}

int PEC_verify(unsigned char *data, unsigned int n, unsigned int PEC){

	if(PEC_calculate(data, n) == PEC){
		return 0;
	}
	return -1;
}	