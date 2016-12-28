#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define UART_PORT PORTB
#define UART_PORT_PIN PB1
#define UART_PORT_DDR DDRB

#define TRUE 1
#define FALSE 0

#define TEMPERATURE_CS PC4
#define TEMPERATURE_CS_PORT PORTC
#define TEMPERATURE_CS_DDR DDRC

#define AVR_SPI_SS PB2
#define AVR_SPI_SS_PORT PORTB
#define AVR_SPI_SS_DDR DDRB

#define TS_CONNECTED(x) ((x & (1<<2)) == 0)
#define TS_TEMPERATURE(x) (x >> 3)

#define BAUD 9600UL
#include <util/setbaud.h>
/*#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#error Systematic Baudrate Error larger than 1% !
#endif*/

#define XTAL F_CPU

#define W1_PIN	PC1
#define W1_IN	PINC
#define W1_OUT	PORTC
#define W1_DDR	DDRC

#define uchar unsigned char
#define uint unsigned int
#define bit uchar
#define idata
#define code


#include "1wire.h"
#include "delay.h"
#include "timebase.h"

void control_init();
void control_send_str(const char* ch);
void control_send_byte(char b);
void control_receive_byte(char b);
void start_meas();
void read_meas();
#endif//MAIN_H
