#include "fixmath.h"
#include "controller.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>

controller_state_t cstate;

static uint8_t g_dimmerPhase = 0;
static volatile uint8_t g_dimmerDuty = 0;

fix16_t eepromP __attribute__ ((section (".eeprom")));
fix16_t eepromI __attribute__ ((section (".eeprom")));
fix16_t eepromD __attribute__ ((section (".eeprom")));
fix16_t eepromTS __attribute__ ((section (".eeprom")));

static inline fix16_t eeprom_read_fix16(const fix16_t* __p)
{
	return (fix16_t)eeprom_read_dword((uint32_t*)__p);
}

static inline void eeprom_write_fix16(const fix16_t* __p, fix16_t __value)
{
	eeprom_write_dword((uint32_t*)__p, (uint32_t)__value);
}

static inline void eeprom_update_fix16(const fix16_t* __p, fix16_t __value)
{
	eeprom_update_dword((uint32_t*)__p, (uint32_t)__value);
}

void controller_init()
{
	cstate.p = eeprom_read_fix16(&eepromP);
	cstate.i = eeprom_read_fix16(&eepromI);
	cstate.d = eeprom_read_fix16(&eepromD);
	cstate.setpoint = eeprom_read_fix16(&eepromTS);
	// 40°C is a compromise value. Neither too high nor too low on startup.
	controller_reset(fix16_from_int(40));
	
	// Configure Analog Comparator (AC)
	ACSR |= (1 << ACIE); // enable AC interrupt
	ACSR |= (1 << ACIS1) | (1 << ACIS0); // rising edge

	TIMSK |= (1 << TOIE0); // enable timer interrupt
	CONTROLLER_TRIAC_DDR |= (1<<CONTROLLER_TRIAC_PIN);
}

void controller_reset(fix16_t init_temp)
{
	cstate.integral = 0;
	cstate.derivative = 0;
	if(init_temp != fix16_min) {
		cstate.prev_temp = init_temp;
	}
	cstate.slow_temp = cstate.prev_temp;
	cstate.output = 0;
	cstate.windup = 0;
}

void controller_disable_dimmer()
{
	cstate.output = 0;
	g_dimmerDuty = 0;
}

void controller_set_constant_p(fix16_t p)
{
	cstate.p = p;
	eeprom_update_fix16(&eepromP, cstate.p);
}

void controller_set_constant_i(fix16_t i)
{
	cstate.i = i;
	eeprom_update_fix16(&eepromI, cstate.i);
}

void controller_set_constant_d(fix16_t d)
{
	cstate.d = d;
	eeprom_update_fix16(&eepromD, cstate.d);
}

fix16_t controller_get_constant_p()
{
	return cstate.p;
}

fix16_t controller_get_constant_i()
{
	return cstate.i;
}

fix16_t controller_get_constant_d()
{
	return cstate.d;
}

void controller_set_setpoint_temperature(fix16_t temp)
{
	cstate.setpoint = temp;
	eeprom_update_fix16(&eepromTS, cstate.setpoint);
}

fix16_t controller_get_setpoint_temperature()
{
	return cstate.setpoint;
}

void controller_step_input_temperature(fix16_t temp)
{
	fix16_t cur_deriv = fix16_ssub(temp, cstate.prev_temp);
	fix16_t error = fix16_ssub(cstate.setpoint, temp);
	// 0xb333 <-> 0.7
	cstate.derivative = fix16_sadd(fix16_smul(cstate.derivative, 0xb333),
	                               fix16_smul(cur_deriv, 0x10000-0xb333));
	if(cstate.windup == 0) {
		cstate.integral = fix16_sadd(cstate.integral, error);
	}
	// 0xffbe <-> 0.9
	cstate.slow_temp = fix16_sadd(fix16_smul(cstate.slow_temp, 0xe666),
	                              fix16_smul(temp, 0x10000-0xe666));
	cstate.prev_temp = temp;
	fix16_t output = fix16_smul(cstate.p, error) +
	                 fix16_smul(cstate.i, cstate.integral) +
		         fix16_smul(cstate.d, cstate.derivative);
	if(output > 0xff0000) {
		cstate.output = 0xff;
		cstate.windup = 1;
	} else if(output < 0x0) {
		cstate.output = 0x0;
		cstate.windup = 1;
	} else {
		cstate.output = (uint8_t)(fix16_to_int(output));
		cstate.windup = 0;
	}
	g_dimmerDuty = cstate.output;
}

controller_state_t controller_get_state()
{
	return cstate;
}


ISR(ANA_COMP_vect)
{
	g_dimmerPhase++;
	if(g_dimmerPhase < g_dimmerDuty) {
		// trigger TRIAC 
		CONTROLLER_TRIAC_PORT |= (1<<CONTROLLER_TRIAC_PIN);
		TCNT0 = 196; // 216 -> 10ms
		TCCR0 |= (1 << CS02) | (0 << CS01) | (1 << CS00); // clk/1024
	}
}

ISR(TIMER0_OVF_vect)
{
	// stop timer
	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	// end trigger pulse
	CONTROLLER_TRIAC_PORT &= ~(1<<CONTROLLER_TRIAC_PIN);
}

