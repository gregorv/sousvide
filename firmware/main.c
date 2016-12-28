/*  Sousvide-Temperature Controller Firmware
    Copyright (C) 2016 Gregor Vollmer <git@dynamic-noise.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "main.h"

fix16_t g_current_temperature;


int tempsens_start_measure(void){
	if( W1_IN & 1<< W1_PIN ){
		w1_command( CONVERT_T, NULL );
		W1_OUT |= 1<< W1_PIN;
		W1_DDR |= 1<< W1_PIN; // parasite power on
	}else{
		uart_puts_P("Short Circuit!\r\n");
		return 0;
	}
	return 1;
}


fix16_t tempsens_get_temperature()
{
	unsigned char id[8], diff;
	for(diff = SEARCH_FIRST; diff != LAST_DEVICE;) {
		diff = w1_rom_search(diff, id);

		if(diff == PRESENCE_ERR) {
			uart_puts_P("\aNo Sensor found\r\n");
			break;
		}
		if(diff == DATA_ERR) {
			uart_puts_P("\aBus Error\r\n");
			break;
		}
		if(id[0] == 0x28 || id[0] == 0x10){	// temperature sensor
			w1_byte_wr(READ);			// read command
			unsigned int temp = w1_byte_rd();	// low byte
			temp |= (uint)w1_byte_rd() << 8;	// high byte
			if( id[0] == 0x10 )			// 9 -> 12 bit
				temp <<= 3;
			fix16_t fixtemp = fix16_from_int(temp) >> 4; // 0.5°C / 8 -> 1/16°C
			return fixtemp;
		}
	}
	uart_puts("\aNo Temperature Sensor\r\n");
	return fix16_min;
}


int main(void)
{
	init_timer();
	uart_init(UART_BAUD_SELECT(F_CPU, BAUD));
	controller_init();
	com_init();
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei();
	uint8_t first = 3;
	for(;;) {
		if(second == 1) {
			tempsens_start_measure();
		} else if(second == 2) {
			g_current_temperature = tempsens_get_temperature();
			if(g_current_temperature == fix16_min) {
				// EMERGENCY SHUTDOWN!!
				controller_disable_dimmer();
			} else {
				if(first) {
					controller_reset(g_current_temperature);
					--first;
				} else {
					controller_step_input_temperature(g_current_temperature);				}
			}
			second = 0;
		}
		com_handle();
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();
	}
	return 0;
}

