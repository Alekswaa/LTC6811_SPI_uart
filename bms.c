#include "bms.h"
#include <atmel_start.h>
#include "function_def.h"
#include <stdio.h>
#include <stdint.h>
#include "atmel_start_pins.h"


/**
 * @brief Write configurations to the LTC6811 chip
 * 
 */
void bms_init_ltc6811()
{
	/*Setting values to write to CFGA*/
	uint8_t reg_cfga[6] = {0};
	reg_cfga[0] = (REFON | ADCOPT_OFF);

	write_to_ltc6811(WRCFGA, reg_cfga, 6);

}

void bms_read_cell_voltages(uint16_t* voltage_buffer)
{
	/*Measure and read from ADC
	* 1. Send ADCV command with configuration
	* 2. Wait for delay according to frequency
	* 3. Disable cs, wait for a bit, enable cs
	* 4. Send read command for register
	* 5. enjoy all the munchy munchy voltages
	*/
	uint16_t voltages[12*NUMBER_OF_SLAVES] = {0};

	/*Initialize the ADC reading*/
	/*1. Configuration of command*/
	uint16_t start_adc_meas;
	start_adc_meas = ADCV | FOURTY_TWO_kHz | DISCHARGE_NOT_PERMITTED | ALL_CELLS;
	bms_send_command(start_adc_meas, true);
	delay_us(1100); //time for measurement to finish
	
	///* Tell IC to send voltages from reg A */
	read_data_from_ltc6811(RDCVA, (uint8_t*)voltages, 6, true);
	for(int i = 0; i < 3; i++)
		voltage_buffer[i] = voltages[i];
		
	read_data_from_ltc6811(RDCVB, (uint8_t*)voltages, 6, true);
	for(int i = 0; i < 3; i++)
		voltage_buffer[i+3] = voltages[i];

	read_data_from_ltc6811(RDCVC, (uint8_t*)voltages, 6, true);
	for(int i = 0; i < 3; i++)
		voltage_buffer[i+6] = voltages[i];
		
	read_data_from_ltc6811(RDCVD, (uint8_t*)voltages, 6, true);
	for(int i = 0; i < 3; i++)
		voltage_buffer[i+9] = voltages[i];
}

/**
 * @brief Wake the LTC6811 chip up from sleep mode
 */
void wake_up_ltc(){
	bms_enable_cs();
	bms_disable_cs();
	delay_us(400);
}

void bms_enable_cs(){
	gpio_set_pin_level(PC25, false);
	delay_us(10);
}

void bms_disable_cs(){
	gpio_set_pin_level(PC25, true);
}