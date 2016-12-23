#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "wifi.h"

#define TRUE 1
#define FALSE 0

#define DIMMER_TRIAC_P PC5
#define DIMMER_TRIAC_PORT PORTC
#define DIMMER_TRIAC_DDR DDRC

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

uint8_t g_dimmerPhase = 0;
volatile uint8_t g_dimmerDuty = 10;

uint16_t flipShort(uint16_t x)
{
	return (
		((x&0x0001)? 0x8000 : 0) +
		((x&0x0002)? 0x4000 : 0) +
		((x&0x0004)? 0x2000 : 0) +
		((x&0x0008)? 0x1000 : 0) +
		((x&0x0010)? 0x0800 : 0) +
		((x&0x0020)? 0x0400 : 0) +
		((x&0x0040)? 0x0200 : 0) +
		((x&0x0080)? 0x0100 : 0) +
		((x&0x0100)? 0x0080 : 0) +
		((x&0x0200)? 0x0040 : 0) +
		((x&0x0400)? 0x0020 : 0) +
		((x&0x0800)? 0x0010 : 0) +
		((x&0x1000)? 0x0008 : 0) +
		((x&0x2000)? 0x0004 : 0) +
		((x&0x4000)? 0x0002 : 0) +
		((x&0x8000)? 0x0001 : 0)); 
}

void initDimmer()
{
	// Configure INT0
/*	EICRA &= (1 << ISC01) | (1 << ISC00); // INT0 rising edge
	EIMSK |= (1 << INT0); // Enable INT0 interrupt */

	// Configure AC
	ACSR |= (1 << ACIE); // enable AC interrupt
	ACSR |= (1 << ACIS1) | (1 << ACIS0); // rising edge

	TIMSK |= (1 << TOIE0); // enable timer interrupt
	DIMMER_TRIAC_DDR |= (1<<DIMMER_TRIAC_P);
}

void initSPI()
{
	SPCR |= (1<<SPE); // Enable SPI
	SPCR |= (1<<MSTR); // master
	SPCR |= (1<<SPI2X) | (0<<SPR1) | (1<<SPR0); // clk/8 -> 2.5MHz
	SPCR |= (1<<CPHA); // sample on falling edge
//	SPCR |= (1<<DORD); // LSB first
	DDRB |= (1<<PB5);
	AVR_SPI_SS_DDR |= (1<<AVR_SPI_SS);
	AVR_SPI_SS_PORT |= (1<<AVR_SPI_SS);
	TEMPERATURE_CS_DDR |= (1<<TEMPERATURE_CS);
	TEMPERATURE_CS_PORT |= (1<<TEMPERATURE_CS);
}

ISR(ANA_COMP_vect)
{
	g_dimmerPhase++;
	if(g_dimmerPhase < g_dimmerDuty) {
		// trigger TRIAC 
		DIMMER_TRIAC_PORT |= (1<<DIMMER_TRIAC_P);
		TCNT0 = 196; // 216 -> 10ms
		TCCR0 |= (1 << CS02) | (0 << CS01) | (1 << CS00); // clk/1024
	}
}

uint16_t temperatureReadStatus()
{
	TEMPERATURE_CS_PORT &= ~(1<<TEMPERATURE_CS);
	SPDR = 0;
	while((SPSR & (1<<SPIF)) == 0);
	uint8_t h = SPDR;
	SPDR = 0;
	while((SPSR & (1<<SPIF)) == 0);
	uint8_t l = SPDR;
	TEMPERATURE_CS_PORT |= (1<<TEMPERATURE_CS);
	return (((uint16_t)h) << 8) + l;
}

uint16_t temperatureAverage()
{
	uint16_t temp = TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	temp += TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	temp += TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	temp += TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	temp += TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	temp += TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	temp += TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	temp += TS_TEMPERATURE(temperatureReadStatus());
	_delay_ms(300);
	return temp >> 3;
}

ISR(TIMER0_OVF_vect)
{
	// stop timer
	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	// end trigger pulse
	DIMMER_TRIAC_PORT &= ~(1<<DIMMER_TRIAC_P);
}

int main()
{
	initDimmer();
	initSPI();
	wifi_setup();
	sei();

//	wifi_connect();

	float target_temperature = 60.0f;
	const float temperature_calibration = -2.0f;
	const float p = 5.0f;
	const float i = 0.01f;
	float i_cur = 0.0f;
	uint8_t wind_up = FALSE;
	const float dt = 1.2f;
	while(1)
	{
//		wifi_connect();
		float temperature = temperatureAverage()/4.0f + temperature_calibration;
		float error = target_temperature - temperature;
		if(!wind_up) {
			i_cur += dt*(error);
		}
		float power = p * error + i * i_cur;
		if(power > 255.0f) {
			g_dimmerDuty = 255;
			wind_up = TRUE;
		}
		else if(power < 0.0f) {
			wind_up = TRUE;
			g_dimmerDuty = 0;
		} else {
			wind_up = FALSE;
			g_dimmerDuty = power;
		}
		char str[50];
		sprintf(str, "TEiDw %.2f %.2f %.5f %i %i\r\n", temperature, error, i_cur, g_dimmerDuty, wind_up); 
		wifi_uart_send_string(str);
	}
	return 0;
}

ISR(USART_RXC_vect)
{
	wifi_uart_receive_byte(UDR);
}

void wifi_uart_send_byte(uint8_t b)
{
    while(!(UCSRA & (1<<UDRE)));
    UDR = b;
}

void wifi_uart_setup()
{
    DDRD |= (1<<PD1);
    UCSRB |= (1<<RXCIE); // enable TX interupt
    UCSRB |= (1<<TXEN) | (1<<RXEN);
    UCSRC |= (1<<USBS) | (1<<UCSZ1) | (1<<UCSZ0); // 8 bit, 2 stop, async
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X
	UCSRA |= (1 << U2X);
#else
	UCSRA &= ~(1 << U2X);
#endif
}

