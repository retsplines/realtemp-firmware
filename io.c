#include "defs.h"
#include <stdbool.h>
#include <avr/io.h>

/**
 * IO setup at boot or wake.
 */
void io_setup(void)
{
    // Set the LED pin as output
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