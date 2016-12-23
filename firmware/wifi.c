#include "wifi.h"
#include <util/delay.h>

void wifi_setup()
{
	wifi_uart_setup();
	WIFI_PORT_DDR |= (1<<WIFI_PORT_PIN);
	// pull reset low for a short time
	WIFI_PORT &= ~(1 << WIFI_PORT_PIN);
	_delay_ms(50);
	WIFI_PORT |= (1 << WIFI_PORT_PIN);
	_delay_ms(1000);
}

void wifi_connect()
{
	wifi_uart_send_string("AT+CWJAP=\"");
	wifi_uart_send_string(WIFI_SSID);
	wifi_uart_send_string("\",\"");
	wifi_uart_send_string(WIFI_PASSWORD);
	wifi_uart_send_string("\"\r\n");
	_delay_ms(1000);
}

void wifi_uart_send_string(char* str)
{
	for(; *str != '\0'; str++) {
		wifi_uart_send_byte(*str);
	}
}

void wifi_uart_receive_byte(uint8_t b)
{

}
