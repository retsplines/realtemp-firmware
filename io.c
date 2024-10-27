#include "defs.h"
#include <stdbool.h>
#include <avr/io.h>

/**
 * IO setup at boot or wake.
 */
void io_setup(void)
{
    // Set all outputs low
    PORTA.OUT = 0;
    PORTB.OUT = 0;

    PORTA.DIR = 
                  // PA0 = UPDI (in)
        PIN1_bm | // PA1 = Debug (TX) (out)
                  // PA2 = Debug (RX) (unused) (in)
        PIN3_bm   // PA3 = LED (out)
                  // PA4 = (unused) (in)
                  // PA5 = (unused) (in)
                  // PA6 = (unused) (in)
                  // PA7 = OPT_A (unused) (in)
    ;

    PORTB.DIR = 
        PIN0_bm | // PB0 = TWI SCL (out)
        PIN1_bm | // PB1 = TWI SDA (out)
        PIN2_bm   // PB2 = IR LED (out)
                  // PB3 = OPT_B (unused) (in)
                  // PB4 = (unused) (in)
                  // PB5 = (unused) (in)
                  // PB6 = (unused) (in)
                  // PB7 = (unused) (in)
    ;

    // Invert LED state
    PORTA.PIN3CTRL = PORT_INVEN_bm;
}

void io_sleep(void)
{
    // Set all pins to inputs
    PORTA.DIR = 0;
    PORTB.DIR = 0;

    // Disable all input buffers
    PORTA.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;

    // Disable BOD
    BOD.CTRLA = 0;

    // Disable WDT
    WDT.CTRLA = 0;

    // Disable ADC & DAC
    ADC0.CTRLA = 0;
    DAC0.CTRLA = 0;
}

/**
 * Control the INFO LED state. 
 */
void io_info_led(bool on)
{
    if (on)
    {
        PORTA.OUTSET = PIN3_bm;
        return;
    }
    
    PORTA.OUTCLR = PIN3_bm;
}