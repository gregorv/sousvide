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
#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include "fixmath.h"

#define CONTROLLER_TRIAC_PIN  PC5
#define CONTROLLER_TRIAC_PORT PORTC
#define CONTROLLER_TRIAC_DDR  DDRC

typedef struct {
	fix16_t p;
	fix16_t i;
	fix16_t d;
	fix16_t setpoint;
	fix16_t integral;
	fix16_t derivative;
	fix16_t prev_temp;
	fix16_t slow_temp;
	uint8_t output;
	uint8_t windup;
} controller_state_t;

void controller_init();
void controller_reset(fix16_t init_temp);
void controller_disable_dimmer();
void controller_set_constant_p(fix16_t p);
void controller_set_constant_i(fix16_t i);
void controller_set_constant_d(fix16_t d);
fix16_t controller_get_constant_p();
fix16_t controller_get_constant_i();
fix16_t controller_get_constant_d();
void controller_set_setpoint_temperature(fix16_t temp);
fix16_t controller_get_setpoint_temperature();
void controller_step_input_temperature(fix16_t temp);
controller_state_t controller_get_state();
#endif//_CONTROLLER_H_
