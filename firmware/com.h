#ifndef _COM_H_
#define _COM_H_

#include "fixmath.h"

typedef unsigned char command_t;

void com_init();
void com_handle();

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
