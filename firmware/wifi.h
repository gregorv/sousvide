#ifndef _WIFI_H_
#define _WIFI_H_

#include <stdint.h>
#include <avr/io.h>

#define WIFI_SSID "AndroidAP"
#define WIFI_PASSWORD "wmhw1743"
#define WIFI_PORT PORTB
#define WIFI_PORT_PIN PB1
#define WIFI_PORT_DDR DDRB

// Performs UART initialisation and ESP8266 powerup
void wifi_setup();
void wifi_connect();

// called for each received byte
void wifi_uart_receive_byte(uint8_t b);
void wifi_uart_send_string(char* str);

// Externaly implemented functions
void wifi_uart_setup();
void wifi_uart_send_byte(uint8_t b);

#endif//_WIFI_H_
