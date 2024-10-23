#include "defs.h"
#include <stdbool.h>
#include <avr/io.h>

/**
 * IO setup at boot or wake.
 */
void io_setup(void)
{
    PORTA.DIR = 
        // PA0 = UPDI (in)
        PIN1_bm | // PA1 = Debug (TX) (out)
        // PA2 = Debug (RX) (in)
        PIN3_bm | // PA3 = LED (out)
        PIN4_bm | // PA4 = (unused) (out)
        PIN5_bm | // PA5 = (unused) (out)
        PIN6_bm   // PA6 = (unused) (out)
        // PA7 = OPT_A (in)
    ;

    // PORTB.DIR = 
    //     PIN0_bm | // PB0 = TWI SCL (out)
    //     PIN1_bm | // PB1 = TWI SDA (out)
    //     PIN2_bm | // PB2 = IR LED (out)
    //     // PB3 = OPT_B (in)
    //     PIN4_bm | // PB4 = (unused) (out)
    //     PIN5_bm | // PB5 = (unused) (out)
    //     PIN6_bm | // PB6 = (unused) (out)
    //     PIN7_bm   // PB7 = (unused) (out)
    // ;

    // Set all outputs low
    // PORTA.OUT = 0;
    // PORTB.OUT = 0;

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