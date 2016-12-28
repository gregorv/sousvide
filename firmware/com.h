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
#ifndef _COM_H_
#define _COM_H_

#include "fixmath.h"

typedef unsigned char command_t;

void com_init(void);
void com_handle(void);

typedef enum {
	NO_ERROR = 0,
	ERROR_NO_CMD = 1,
	ERROR_ARG_SYNTAX = 2,
	ERROR_TOO_MANY_ARGS = 3,
	ERROR_ARG_MISMATCH = 4,
} error_code_t;

#define COMMAND_GET_P		0x00
#define COMMAND_GET_I		0x01
#define COMMAND_GET_D		0x02
#define COMMAND_GET_TS		0x03
#define COMMAND_GET_TI		0x04
#define COMMAND_SET_P		0x05
#define COMMAND_SET_I		0x06
#define COMMAND_SET_D		0x07
#define COMMAND_SET_TS		0x08
#define COMMAND_PID_DIAG	0x09
#define COMMAND_SOFT_RESET	0x0A
#define COMMAND_NO_CMD		0xFF

#endif//_COM_H_
