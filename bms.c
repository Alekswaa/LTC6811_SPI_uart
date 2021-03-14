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
/**
 * \brief Write Configuration register group
 */
void turn_on_ref(){
	//uint8_t cfg_refon[6] = {0xFC,0x270&0xFF,(0x697&0x0F)|((0x697&0xF00)>>8),(0x697&0xFF0)>>4,0x00,0x00};
	uint8_t cfg_refon[6] = {0xFF,0,0,0,0x00,0xFF};
	enable_cs();
	//delay_us(10);
    broadcast_write(WRCFGA, 6, cfg_refon);
	//delay_us(10);
	disable_cs();
    return;
}


void read_status_ltc(uint16_t* status){
	uint8_t stat1, stat2;
	wakeup_sleep();
	delay_ms(1);
	enable_cs();
	//broadcast_read(RDSTATA, 2, (uint8_t*)status);
	broadcast_read((uint8_t)0x01, 2, (uint8_t*)status);
	//broadcast_read(RDSTATA, 2, &stat1);
	//disable_cs();
	//delay_ms(1);
	//wakeup_sleep();
	//enable_cs();
	//broadcast_read(RDSTATB, 2, (uint8_t*)(status+1));
	disable_cs();
	//*status = stat1 << 8 | stat2;
	
	
	
		
		//wakeup_sleep();
		//delay_ms(1);
		//enable_cs();
		//broadcast_read(RDSTATA, 2, &stat1);
		////delay_us(10);
		////disable_cs();
		////delay_ms(1);
		////enable_cs();
		//broadcast_read(RDSTATB, 2, &stat2);
		////delay_us(10);
		//disable_cs();
		//*status = stat1<<8 | stat2;
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
 * \brief Transmit and receive data from LTC6811. PEC is verified
 * \param[in] command Command to send to IC
 * \param[in] size 	 Number of bytes to read. The two bytes for PEC is added in function
 * \param[out] data  Pointer for output data using uint8_t data
 * @returns 0 if fine, -1 if wrong PEC
 */
int broadcast_write( unsigned int command, unsigned int size, unsigned char *data)
{

	unsigned char command_message[4];
	unsigned char data_PEC[2];
    unsigned int PEC;
    unsigned int command_PEC;
    unsigned int slave;
    unsigned int everything_is_valid = true;

	command_message[0] = 0;
	command_message[1] = 1;
    command_PEC = PEC_calculate(command_message, 2);
    command_message[2] = command_PEC >> 8;
    command_message[3] = command_PEC;

	uint16_t PEC_data = PEC_calculate(data, size);
	uint8_t pec_data[2];
	pec_data[0]  = PEC_data>>8;
	pec_data[1] = PEC_data&0x00FF;

    isoSpi_send(&command_message, 4);
    isoSpi_send(data, size);
    isoSpi_send(pec_data, 2);

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


int16_t pec15Table[256];
int16_t CRC15_POLY = 0x4599;
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
         remainder = (remainder << 8 ) ^ crc15Table[address];
		//remainder = (remainder << 8 ) ^ pec15Table[address];
    }
    return(remainder*2);/*The CRC15 has a 0 in the LSB so the final value must be multiplied by 2*/
}


void init_PEC15_Table(){
	unsigned int remainder;
	for (int i = 0; i < 256; i++){
		remainder = i << 7;
		for (int bit = 8; bit > 0; --bit){
			if (remainder & 0x4000){
				remainder = ((remainder << 1));
				remainder = (remainder ^ CRC15_POLY);
			}
			else
			{
				remainder = ((remainder << 1));
			}
		}
		pec15Table[i] = remainder&0xFFFF;
	}
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
	spi_m_sync_set_data_order(&SPI_1,SPI_DATA_ORDER_MSB_1ST);
	spi_m_sync_set_char_size(&SPI_1,SPI_CHAR_SIZE_8); 
	//uint16_t delay_100_us = 100; 
	
	struct spi_xfer spi_transmit_buffer;
	io_write(io, transfer_data, size);
	// spi_transmit_buffer.size = size;
	//spi_transmit_buffer.rxbuf  = receive_data;
	//for(int i = 0; i < size; i++){
		// spi_transmit_buffer.txbuf = transfer_data;
		// spi_m_sync_transfer(&SPI_1, &spi_transmit_buffer);
	//}
	spi_m_sync_disable(&SPI_1);
	
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
