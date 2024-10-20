# RealTemp AC Temperature Sensor Firmware

This is the firmware for the RealTemp AC Temperature Sensor.

## Building

The firmware is built using `avr-gcc`.

## Flashing

RealTemp uses an ATtiny214, programmed via the UPDI interface. I recommend using a standard TTL serial cable with the appropriate modifications to program UPDI devices.

You can use the `program` Makefile target to invoke `avrdude`.
