#include "main.h"



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

void start_meas( void ){
  if( W1_IN & 1<< W1_PIN ){
    w1_command( CONVERT_T, NULL );
    W1_OUT |= 1<< W1_PIN;
    W1_DDR |= 1<< W1_PIN;			// parasite power on

  }else{
	  control_send_str("Short Circuit!\r\n");
  }
}


void read_meas( void )
{
  uchar id[8], diff;
  uchar s[30];
  uchar i;
  uint temp;

  for( diff = SEARCH_FIRST; diff != LAST_DEVICE; ){
    diff = w1_rom_search( diff, id );

    if( diff == PRESENCE_ERR ){
      control_send_str( "No Sensor found" );
      break;
    }
    if( diff == DATA_ERR ){
      control_send_str( "Bus Error" );
      break;
    }
    if( id[0] == 0x28 || id[0] == 0x10 ){	// temperature sensor
      control_send_str( "ID: " );
      for( i = 0; i < 8; i++ ){
	sprintf( s, "%02X ", id[i] );
	control_send_str( s );
      }
      w1_byte_wr( READ );			// read command
      temp = w1_byte_rd();			// low byte
      temp |= (uint)w1_byte_rd() << 8;		// high byte
      if( id[0] == 0x10 )			// 9 -> 12 bit
        temp <<= 3;
      sprintf( s, "  T: %04X = ", temp );	// hex value
      control_send_str( s );
      sprintf( s, "%4d.%01døC", temp >> 4, (temp << 12) / 6553 ); // 0.1øC
      control_send_str( s );
    }
  }
  control_send_str( "\r\n" );
}


int main()
{
	init_timer();
	control_init();
	sei();
	control_send_str("Hello.\r\n");
	for(;;) {
		start_meas();
		_delay_ms(500);
		read_meas();
		_delay_ms(500);
	}
//	wifi_connect();
/*
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
		control_send_str(str);
	}*/
	return 0;
}

ISR(USART_RXC_vect)
{
	control_receive_byte(UDR);
}

void control_init()
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

void control_send_str(const char* str)
{
	for(; *str != '\0'; str++) {
		control_send_byte(*str);
	}
}

void control_send_byte(char b)
{
    while(!(UCSRA & (1<<UDRE)));
    UDR = b;
}

void control_receive_byte(char b)
{
}

