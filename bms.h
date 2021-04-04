#ifndef BMS_H_INCLUDED
#define BMS_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* BMS hardware setup parameters */
#define CELLS_PER_SLAVE 12
#define NUMBER_OF_SLAVES 1
#define BYTES_IN_VOLTAGE_REG 6
#define LTC6811_DAISYCHAIN_ALL_VOLTAGES (NUMBER_OF_SLAVES*CELLS_PER_SLAVE)


/********************************/

/*LTC6811 Commands. See page 59 of datasheet. Need to add all */
#define WRCFGA  0x0001
#define RDCFGA  0x0002 //bit at index 1 is constantly 2

#define RDCVA   0x0004
#define RDCVB   0x0006
#define RDCVC   0x0008
#define RDCVD   0x000A
#define PLADC	0x0714

#define RDAUXA  0x000C
#define RDSTATA 0x0010
#define RDSTATB 0x0012

#define REFON 1<<2
#define ADCOPT_ON  1
#define ADCOPT_OFF 0
#define ADCV 0x0260
#define SEVEN_kHz 2<<7
#define FOURTY_TWO_kHz 1<<7
#define DISCHARGE_NOT_PERMITTED 0<<4
#define ALL_CELLS 0


void bms_init_ltc6811();
void read_data_from_ltc6811(uint16_t, uint8_t*,int, bool);
void write_to_ltc6811(uint16_t, uint8_t*, int);
void bms_send_command(uint16_t, bool);
void bms_enable_cs();
void bms_disable_cs();

void bms_read_cell_voltages(uint16_t*);
void isoSpi_send(uint8_t*, int);
void isoSpi_receive(uint8_t*, int);
uint16_t PEC_calculate(unsigned char* , int);


#endif