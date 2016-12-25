#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

void controller_init();
void controller_set_constant_p(int p);
void controller_set_constant_i(int i);
void controller_set_constant_d(int d);
void controller_set_setpoint_temperature(int temp);

#endif//_CONTROLLER_H_
