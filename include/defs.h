/* Common definitions */

#ifndef H_DEFS
#define H_DEFS

// MCU
#define __AVR_ATtiny214__ 1

// CPU frequency in Hz
// Runs at 20 MHz with a prescaler of 5 by default
#define F_CPU 3333333UL

// Time between wake-ups in seconds
#define T_WAKE 10

// Transmit interval in seconds
#define T_TRANSMIT 20

// Transmit ºC delta threshold
#define TX_DELTA_C 1

#endif // H_DEFS
