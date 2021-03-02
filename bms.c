#include "bms.h"
#include <stdio.h>
#include <atmel_start.h>
#include <stdint.h>
#include "function_def.h"
#include "PEC.h"
#include "atmel_start_pins.h"



void start_cell_voltages_conversion(){
    broadcast_poll(ADCV(MD_NORMAL, DCP_DISCHARGE_NOT_PERMITTED, CH_ALL_CELLS));
    return;
}

int broadcast_poll(unsigned int command){ //send command to LTC6811

	unsigned char message[4];
    unsigned int PEC;
    message[0] = command >> 8;
	message[1] = command;
    PEC = PEC_calculate(message,2);
    message[2] = PEC >>8;
    message[3] = PEC;

	wakeup_sleep();
	enable_cs();
    isoSpi_send(&message[0], 4, NULL);
    disable_cs();

	return 0;
}

int broadcast_read(
        unsigned int command,
        unsigned int size,
        unsigned char *data)
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
	wakeup_sleep();
    enable_cs();
    isoSpi_send(&command_message[0], 4, NULL);
    isoSpi_send(NULL, size, data);
    isoSpi_send(NULL, 2, data_PEC);
    PEC = data_PEC[0]<<8 | data_PEC[1];
    if(PEC_verify(&data[slave*size], size, PEC) < 0) everything_is_valid = false;

    disable_cs();

    if(everything_is_valid == false){
		print_uart((uint8_t*)"Nope1\n");
        return -1;
    }

    return 0;
}

void get_cell_voltages(uint16_t* measured_voltages){
    uint16_t voltages[3 * BATTERY_SERIES_STACKS];
    uint16_t stack, cell;

    /*
     * Collect cells 0-2 (group A) from each LTC6811
     */
    broadcast_read(RDCVA, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; ++cell){
			measured_voltages[cell] = voltages[cell];
        }
    }

    /*
     * Collect cells 3-5 (group B) from each LTC6811
     */
    broadcast_read(RDCVB, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; ++cell){
				measured_voltages[cell+3] = voltages[cell];
        }
    }

    /*
     * Collect cells 6-8 (group C) from each LTC6811
     */
    broadcast_read(RDCVC, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; ++cell){
           	measured_voltages[cell+6] = voltages[cell];
        }
    }
    /*
     * Collect cells 9-11 (group D) from each LTC6811
     */
    broadcast_read(RDCVB, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; ++cell){
            measured_voltages[cell+9] = voltages[cell];
        }
    }

    return;
}




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


void start_com(){
	gpio_set_pin_level(PC25, false);
	delay_us(1); 
	gpio_set_pin_level(PC25, true);
	delay_us(MAX_T_WAKE); 
}

void isoSpi_send(uint8_t *transfer_data, int size, uint8_t *receive_data){
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);
	spi_m_sync_enable(&SPI_0);
	//uint16_t delay_100_us = 100; 
	
	struct spi_xfer spi_transmit_buffer;
	spi_transmit_buffer.size = size;
	spi_transmit_buffer.rxbuf  = receive_data;

	//enable_cs(); //do that shit manually before
	
	
	for(int i = 0; i < size; i++){
		spi_transmit_buffer.txbuf = &transfer_data[i];
		spi_m_sync_transfer(&SPI_0, &spi_transmit_buffer);
		//delay_us(delay_100_us);
	}
	
	//disable_cs();
}



void enable_cs(){
	gpio_set_pin_level(PC25, false);
}

void disable_cs(){
	gpio_set_pin_level(PC25, true);
}

void wakeup_sleep()
{
	enable_cs();
	disable_cs();
	delay_us(MAX_T_WAKE); // Guarantees the LTC6804 will be in standby
}
