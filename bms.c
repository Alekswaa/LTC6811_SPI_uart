#include "bms.h"
#include <stdio.h>
#include <atmel_start.h>
#include <stdint.h>
#include "function_def.h"
#include "PEC.h"
#include "atmel_start_pins.h"


/**
 * \brief Command LTC6811 to start measuring ADCs
 */
void start_cell_voltages_conversion(){
    broadcast_poll(ADCV(MD_NORMAL, DCP_DISCHARGE_NOT_PERMITTED, CH_ALL_CELLS));
    return;
}

void read_status_ltc(uint16_t* status){
	wakeup_sleep();
	delay_ms(1);
	broadcast_read(RDSTATA, 2, (uint8_t*)status);
	broadcast_read(RDSTATB, 2, (uint8_t*)(status+1));
}


/**
 * \brief IsoSPI send command to LTC6811 through LTC6820
 * \param[in] command The command that the LTC6811 should receive
 */
int broadcast_poll(unsigned int command)
{
	unsigned char message[4];
    unsigned int PEC;
    message[0] = command >> 8;
	message[1] = command;
    PEC = PEC_calculate(message,2);
    message[2] = PEC >> 8;
    message[3] = PEC;

    isoSpi_send(&message[0], 4);

	return 0;
}

/**
 * \brief Transmit and receive data from LTC6811. PEC is verified
 * \param[in] command Command to send to IC
 * \param[in] size 	 Number of bytes to read. The two bytes for PEC is added in function
 * \param[out] data  Pointer for output data using uint8_t data
 * @returns 0 if fine, -1 if wrong PEC
 */
int broadcast_read( unsigned int command, unsigned int size, unsigned char *data)
{

	unsigned char command_message[4];
	unsigned char data_PEC[2];
    unsigned int PEC;
    unsigned int command_PEC;
    unsigned int slave;
    unsigned int everything_is_valid = true;

	command_message[0] = command >> 8;
	command_message[1] = command;
    command_PEC = PEC_calculate(command_message, 2);
    command_message[2] = command_PEC >> 8;
	command_message[3] = command_PEC;

    isoSpi_send(&command_message, 4);
    isoSpi_receive(data, size);
    isoSpi_receive(data_PEC, 2);
    PEC = data_PEC[0]<<8 | data_PEC[1];
    if(PEC_verify(&data[0], size, PEC) < 0) everything_is_valid = false;

    if(everything_is_valid == false){
		print_uart_ln((uint8_t*)"Nope1",5);
        return -1;
    }

    return 0;
}

/**
 * \brief Read voltages from LTC6811
 * \param[out] measured_voltages  Pointer for output data using uint16_t data
 */
void get_cell_voltages(uint16_t* measured_voltages){
    uint16_t voltages[3 * BATTERY_SERIES_STACKS];
    uint16_t stack, cell;

    /*
     * Collect cells 0-2 (group A) from each LTC6811
     */
    broadcast_read(RDCVA, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; cell++){
			measured_voltages[cell] = voltages[cell];
        }
    }

    /*
     * Collect cells 3-5 (group B) from each LTC6811
     */
    broadcast_read(RDCVB, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; cell++){
				measured_voltages[cell+3] = voltages[cell];
        }
    }

    /*
     * Collect cells 6-8 (group C) from each LTC6811
     */
    broadcast_read(RDCVC, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; cell++){
           	measured_voltages[cell+6] = voltages[cell];
        }
    }
    /*
     * Collect cells 9-11 (group D) from each LTC6811
     */
    broadcast_read(RDCVB, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; cell++){
            measured_voltages[cell+9] = voltages[cell];
        }
    }

    return;
}



/**
 * \brief Transmit and receive data from LTC6811. PEC is verified
 * \param[in] data Pointer to char array containing PEC
 * \param[in] len  Number of bytes in the PEC
 * @returns 0 if correct, -1 if wrong PEC
 */
unsigned int PEC_calculate(unsigned char *data , int len){
    unsigned int remainder,address;
    remainder = 16;/*PEC seed*/
    int i;
    for (i = 0; i < len; i++){
        address = ((remainder >> 7) ^ data[i]) & 0xff;/*calculate PEC table address*/
        remainder= (remainder << 8 ) ^ crc15Table[address];
    }
    return((remainder*2)&0xffff);/*The CRC15 has a 0 in the LSB so the final value must be multiplied by 2*/
}

int PEC_verify(unsigned char *data, unsigned int n, unsigned int PEC){

	if(PEC_calculate(data, n) == PEC){
		return 0;
	}
	return -1;
}

/**
 * \brief Wake LTC6811 up from sleep or idle
 */
void start_com(){
	gpio_set_pin_level(PC25, false);
	delay_us(1); 
	gpio_set_pin_level(PC25, true);
	delay_us(MAX_T_WAKE); 
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
	spi_m_sync_enable(&SPI_1);
	//uint16_t delay_100_us = 100; 
	
	struct spi_xfer spi_transmit_buffer;
	spi_transmit_buffer.size = size;
	//spi_transmit_buffer.rxbuf  = receive_data;

	
	
	//for(int i = 0; i < size; i++){
		spi_transmit_buffer.txbuf = transfer_data;
		spi_m_sync_transfer(&SPI_1, &spi_transmit_buffer);
	//}
	
}

void isoSpi_receive(uint8_t *receive_data, int size){
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_1, &io);
	spi_m_sync_enable(&SPI_1);
	//uint16_t delay_100_us = 100;
	
	struct spi_xfer spi_transmit_buffer;
	spi_transmit_buffer.size = size;
	spi_transmit_buffer.rxbuf  = receive_data;

	uint8_t nothing[10] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	
	//for(int i = 0; i < size; i++){
		spi_transmit_buffer.txbuf = &nothing;
		spi_m_sync_transfer(&SPI_1, &spi_transmit_buffer);
	//}
	
}

void enable_cs(){
	gpio_set_pin_level(PC25, false);
}

void disable_cs(){
	gpio_set_pin_level(PC25, true);
}

/**
 * @brief Wake up LTC6811 or LTC6820 from sleep or idle
 */
void wakeup_sleep()
{
	enable_cs();
	disable_cs();
	delay_us(MAX_T_WAKE);
}
