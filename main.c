/**
 * Realtemp Firmware.
 */

#include "defs.h"
#include "io.h"
#include "at30ts75.h"
#include "debug.h"
#include <stdlib.h>
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

    // Setup debug
    debug_init();

    // Setup the AT30TS75
    at30ts75_init();

    char message[32] = {0};

    while (1)
    {
        // Do a temperature conversion
        int32_t temp = at30ts75_convert();

        // debug it
        ltoa(temp, message, 10);
        debug(message);

        // Flash the led
        io_info_led(true);
        _delay_ms(100);
        io_info_led(false);
        _delay_ms(100);

        // Wait a while
        _delay_ms(1000);
    }

    return 0;
}
