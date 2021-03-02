#include <atmel_start.h>
#include "PEC.h"

#define BATTERY_SERIES_STACKS   1
#define RDCVA       0x0004
#define RDCVB       0x0006
#define RDCVC       0x0008
#define RDCVD       0x000A
#define LTC6811_REG_SIZE    6

#define MAX_T_READY 10
#define MAX_T_WAKE  400

void enable_cs();
void start_com();
int broadcast_read();
void get_cell_voltages();
void isoSpi_send(uint8_t*, int, uint8_t*);
unsigned int PEC_calculate(unsigned char* , int);
void disable_cs();
void print_all_the_shit(uint8_t*, int);
void wakeup_sleep();

//#define newline "\n",1


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Replace with your application code */
	uint16_t measured_voltages[BATTERY_SERIES_STACKS*12];
	uint8_t* newline = "\n";
	
	while (1) {
		//SPI_1_example();
		//USART_1_example();
		get_cell_voltages(&measured_voltages);
		print_all_the_shit(newline, 1);
		
		for(int i = 0; i < 12; i++){
			print_all_the_shit(&measured_voltages[i], 1);
			print_all_the_shit(newline, 1);
		}
		delay_ms(500);
		
		
	}
}



void get_cell_voltages(uint16_t* measured_voltages){
    uint16_t voltages[3 * BATTERY_SERIES_STACKS];
    uint16_t stack, cell;
	//uint16_t measured_voltages[12];
    /*
     * Collect cells 0-2 (group A) from each LTC6811
     */
    broadcast_read(RDCVA, LTC6811_REG_SIZE, (uint8_t*) voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; cell++){
            measured_voltages[stack*3 + cell] = voltages[cell];
        }
    }
	/*
    *
     * Collect cells 3-5 (group B) from each LTC6811
     *
    broadcast_read(RDCVB, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; cell++){
            measured_voltages[cell+3] = voltages[stack * 3 + cell];
        }
    }

    *
     * Collect cells 6-8 (group C) from each LTC6811
     *
    broadcast_read(RDCVC, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; stack++){
        for(cell = 0; cell < 3; cell++){
            measured_voltages[cell+6] = voltages[stack * 3 + cell];
        }
    }
    *
     * Collect cells 9-11 (group D) from each LTC6811
     *
    broadcast_read(RDCVB, LTC6811_REG_SIZE, (uint8_t*)voltages);

    for(stack = 0; stack < BATTERY_SERIES_STACKS; ++stack){
        for(cell = 0; cell < 3; ++cell){
             measured_voltages[cell+9] = voltages[stack * 3 + cell];
        }
    }
*/
    return;
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
    unsigned int everything_is_valid = true;

	//command_message[0] = command >> 8;
	//command_message[1] = command;
	command_message[0] = 0x00;
	command_message[1] = 0x04;
	
    command_PEC = PEC_calculate(command_message,2);
    command_message[2] = command_PEC >> 8;
	command_message[3] = command_PEC;

	wakeup_sleep();
	
	enable_cs();
	//send data
    isoSpi_send(&command_message, 4, data);

	//read data
	for(int slave = 0; slave < 1; slave++){
		isoSpi_send(NULL, size, &data[slave*size]);
		isoSpi_send(NULL, 2, data_PEC);
		PEC = data_PEC[0]<<8 | data_PEC[1];
		if(PEC_verify(&data[slave*size], size, PEC) < 0) 
			everything_is_valid = false;
	}
	disable_cs();
	if(!everything_is_valid)
		print_all_the_shit((uint8_t*)"nope hahah\n",11);

	
    return 0;
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

	//enable_cs();
	
	
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


void print_all_the_shit(uint8_t* string, int len)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_1, &io);
	usart_sync_enable(&USART_1);

	io_write(io, string, len);
}

void wakeup_sleep()
{
	enable_cs();
	delay_ms(MAX_T_WAKE); // Guarantees the LTC6804 will be in standby
	disable_cs();
}




