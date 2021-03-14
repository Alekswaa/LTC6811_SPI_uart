#ifndef BMS_H_INCLUDED
#define BMS_H_INCLUDED

#include <stdint.h>


#define BATTERY_SERIES_STACKS   1
#define RDCVA       0x0004
#define RDCVB       0x0006
#define RDCVC       0x0008
#define RDCVD       0x000A
#define RDSTATA     0x0010
#define RDSTATB     0x0012
#define WRCFGA      0x0001

#define LTC6811_REG_SIZE    6

#define MAX_T_READY 10
#define MAX_T_WAKE  400

#define ADCV(MD,DCP,CH)     (0x260 | (MD<<7) | (DCP<<4) | (CH))

enum LTC6811_COMMAND_MD{
    MD_MODE_0 = 0,
    MD_MODE_1 = 1,
    MD_FAST = 1,
    MD_MODE_2 = 2,
    MD_NORMAL = 2,
    MD_MODE_3 = 3,
    MD_FILTERED = 3,
};

enum LTC6811_COMMAND_DCP{
    DCP_DISCHARGE_NOT_PERMITTED = 0,
    DCP_DISCHARGE_PERMITTED = 1,
};

enum LTC6811_COMMAND_CH{
    CH_ALL_CELLS = 0,
    CH_CELLS_1_7 = 1,
    CH_CELLS_2_8 = 2,
    CH_CELLS_3_9 = 3,
    CH_CELLS_4_10 = 4,
    CH_CELLS_5_11 = 5,
    CH_CELLS_6_12 = 6,
};

enum LTC6811_COMMAND_PUP{
    PUP_PULL_DOWN = 0,
    PUP_PULL_UP = 1,
};

enum LTC6811_COMMAND_ST{
    ST_SELF_TEST1 = 1,
    ST_SELF_TEST2 = 2,
};

enum LTC6811_COMMAND_CHG{
    CHG_ALL = 0,
    CHG_GPIO1 = 1,
    CHG_GPIO2 = 2,
    CHG_GPIO3 = 3,
    CHG_GPIO4 = 4,
    CHG_GPIO5 = 5,
    CHG_2ND_REFERENCE = 6,
};

enum LTC6811_COMMAND_CHST{
    CHST_SC_ITMP_VA_VD = 0,
    CHST_SC = 1,
    CHST_ITMP = 2,
    CHST_VA = 3,
    CHST_VD = 4,
};

void enable_cs();
void start_com();
int broadcast_read();
void get_cell_voltages();
void isoSpi_send(uint8_t*, int);
void isoSpi_receive(uint8_t*, int);
uint16_t PEC_calculate(unsigned char* , int);
void disable_cs();
void wakeup_sleep();
int broadcast_poll(unsigned int);
void start_cell_voltages_conversion();
void read_status_ltc(uint16_t*);
int broadcast_write(unsigned int, unsigned int, unsigned char*);
void turn_on_ref();


#endif