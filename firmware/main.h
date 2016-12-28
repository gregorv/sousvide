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

#ifndef BAUD
 #define BAUD 9600L
#endif
#ifndef F_CPU
 #error F_CPU not specified
#endif

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
