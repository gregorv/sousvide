#include "com.h"
#include "uart.h"
#include "controller.h"
#define BUF_MAX_LEN 36
#define MAX_ARGS 3
static int g_buf_cur_len;
static char g_buf[BUF_MAX_LEN];
extern fix16_t g_current_temperature;


void com_init()
{
	g_buf_cur_len = 0;
}

static unsigned char hex_to_uchar(const char* twochar)
{
	return (unsigned char)(((twochar[0] - com_zero) << 4)
		+ (twochar[1] - com_zero));
}
static fix16_t hex_to_fix16(const char* twochar)
{
	fix16_t f;
	unsigned char* p = (unsigned char*)&f;
	p[3] = hex_to_uchar(twochar);
	p[2] = hex_to_uchar(twochar + 2);
	p[1] = hex_to_uchar(twochar + 4);
	p[0] = hex_to_uchar(twochar + 6);
	return f;
}

static void put_uchar(unsigned char val)
{
	uart_putc(((val & 0xf0c)>>4) + com_zero);
	uart_putc((uint8_t)((val & 0x0f) + com_zero));
}

static void put_fix16(fix16_t val)
{
	unsigned char* p = (unsigned char*)&val;
	put_uchar(p[3]);
	put_uchar(p[2]);
	put_uchar(p[1]);
	put_uchar(p[0]);
}

static void com_fail_response(command_t cmd, error_code_t err)
{
	uart_puts_P("FAIL");
	put_uchar(cmd);
	// Error Code 15
	put_uchar(err);
	uart_puts_p(""); // TODO: Implement user friendly message
	uart_puts_P("\r\n");
}

static void com_response_v(command_t cmd, const fix16_t* val, uint8_t n)
{
	uart_puts_P("OKAY");
	put_uchar(cmd);
	for(uint8_t i = 0; i < n; ++i) {
		put_fix16(val[i]);
	}
	uart_puts_P("\r\n");
}

static void com_response_4(command_t cmd, fix16_t a, fix16_t b, fix16_t c, fix16_t d)
{
	uart_puts_P("OKAY");
	put_uchar(cmd);
	put_fix16(a);
	put_fix16(b);
	put_fix16(c);
	put_fix16(d);
	uart_puts_P("\r\n");
}

static void com_handle_command(char* cmd, uint8_t len)
{
	if(len < 2) {
		com_fail_response(COMMAND_NO_CMD, ERROR_NO_CMD);
		return;
	}
	command_t command = hex_to_uchar(cmd);
	if((len-2)%8 != 0) {
		com_fail_response(command, ERROR_ARG_SYNTAX);
		return;
	}
	uint8_t nargs = (uint8_t)((len-2) >> 3);
	if(nargs > MAX_ARGS) {
		com_fail_response(command, ERROR_TOO_MANY_ARGS);
		return;
	}
	fix16_t args[MAX_ARGS];
	for(uint8_t i=0; i<nargs; ++i) {
		args[i] = hex_to_fix16(cmd+2+(i<<3));
	}
	controller_state_t cstate = controller_get_state();
	if(command == COMMAND_GET_P) {
		com_response_v(command, &cstate.p, 1);
	} else if(command == COMMAND_GET_I) {
		com_response_v(command, &cstate.i, 1);
	} else if(command == COMMAND_GET_D) {
		com_response_v(command, &cstate.d, 1);
	} else if(command == COMMAND_GET_TS) {
		com_response_v(command, &cstate.setpoint, 1);
	} else if(command == COMMAND_GET_TI) {
		com_response_v(command, &g_current_temperature, 1);
	} else if(command == COMMAND_SET_P) {
		if(nargs != 1) {
			com_fail_response(command, ERROR_ARG_MISMATCH);
		} else {
			controller_set_constant_p(args[0]);
			com_response_v(command, args, 1);
		}
	} else if(command == COMMAND_SET_I) {
		if(nargs != 1) {
			com_fail_response(command, ERROR_ARG_MISMATCH);
		} else {
			controller_set_constant_i(args[0]);
			com_response_v(command, 0, 0);
		}
	} else if(command == COMMAND_SET_D) {
		if(nargs != 1) {
			com_fail_response(command, ERROR_ARG_MISMATCH);
		} else {
			controller_set_constant_d(args[0]);
			com_response_v(command, 0, 0);
		}
	} else if(command == COMMAND_SET_TS) {
		if(nargs != 1) {
			com_fail_response(command, ERROR_ARG_MISMATCH);
		} else {
			controller_set_setpoint_temperature(args[0]);
			com_response_v(command, 0, 0);
		}
	} else if(command == COMMAND_PID_DIAG) {
		com_response_4(command, cstate.integral, cstate.derivative,
		               fix16_from_int(cstate.output),
			       cstate.slow_temp);
	} else if(command == COMMAND_SOFT_RESET) {
		controller_reset(fix16_min);
		com_response_v(command, 0, 0);
	}
}

void com_handle()
{
	for(;;) {
		unsigned int data = uart_getc();
		if(data & UART_NO_DATA) {
			break;
		}
		g_buf[g_buf_cur_len++] = (char)(data & 0xFF);
		uint8_t cmdlen = 0;
		for(cmdlen = 0; cmdlen < g_buf_cur_len-1; ++cmdlen) {
			if(g_buf[cmdlen] == '\r' && g_buf[cmdlen+1] == '\n') {
				break;
			}
		}
		if(cmdlen < g_buf_cur_len-1) {
			com_handle_command(g_buf, cmdlen);
			int newlen = 0;
			for(int i = cmdlen + 2; i < g_buf_cur_len; ++i) {
				g_buf[i-cmdlen-2] = g_buf[i];
				newlen++;
			}
			g_buf_cur_len = newlen;
		}
		if(g_buf_cur_len == BUF_MAX_LEN) {
			// invalid command received!
			// shift buffer by one char
			for(int i=0; i < g_buf_cur_len-1; ++i) {
				g_buf[i] = g_buf[i+1];
			}
			g_buf_cur_len -= 1;
		}
		/*for(int i=0; i<g_buf_cur_len; ++i) {
			uart_putc(g_buf[i]);
		}
		uart_putc('\b');*/
	}
}
