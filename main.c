/**
 * Realtemp Firmware.
 */

#include "defs.h"
#include "io.h"
#include <avr/io.h>
#include <util/delay.h>

/**
 * The main routine.
 * 
 * @return int 
 */
int main(void)
{
    // Setup IO
    io_setup();

    while (1)
    {
        // Flash the LED
        io_info_led(true);
        _delay_ms(100);
        io_info_led(false);
        _delay_ms(100);
    }

    return 0;
}
