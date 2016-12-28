#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define UART_PORT PORTB
#define UART_PORT_PIN PB1
#define UART_PORT_DDR DDRB

#define BAUD 9600UL

#define W1_PIN	PC1
#define W1_IN	PINC
#define W1_OUT	PORTC
#define W1_DDR	DDRC

/* Required by Peter Danneggers 1Wire code */
#define uchar unsigned char
#define uint unsigned int
#define bit uchar
#define idata
#define code
#define XTAL F_CPU

#include "fixmath.h"

#define UART_TX_BUFFER_SIZE 16
#define UART_RX_BUFFER_SIZE 48

extern fix16_t g_current_temperature;

#include "1wire.h"
#include "delay.h"
#include "timebase.h"
#include "controller.h"
#include "uart.h"
#include "com.h"


int tempsens_start_measure(void);
fix16_t tempsens_get_temperature(void);

#endif//MAIN_H
