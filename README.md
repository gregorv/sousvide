# sousvide
An ATmega8 based Sous-Vide Temperature Controller

Sous-Vide is a cooking method for a very gentle preparation of meat and vegetables. The general idea is sealing the
food in a vacuum bed and then cooking it in a water bath with a maximum temperature of about 60°C to 70°C for an extended time.
Important cooking reactions (including sterilization) start at around 50°C, but unwanted processes (e.g. meat getting stringy)
increase above 70°C. Because of the lower temperatures the cooking time is elonged, but you get a very even and reproducible
result from the process.

## Overview
### Hardware
On the hardware side (schematics and layout KiCAD format) an ATmega8 regulates the temperature of the water bath with a TRIAC
controlled immersion heater. It is advised to use an acquarium water pump to better spread the heat in the bath. A DS18x2x type
Dallas 1Wire sensor is used for temperature monitoring. The UART can be used to communicate with a ESP8266 type WiFi module, but
is used in the prototype for serial communication and control.

### Firmware
A straight-forward, modularized fixed-point PID controller.

Uses the UART library from Peter Fleury, the 1wire library by Peter Dannegger and the libfixmath by Ben Brewer.

### PC Control Software
Qt5, QCustomPlot, nice.
