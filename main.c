/**
 * Realtemp Firmware.
 */

#include "defs.h"
#include "io.h"
#include "at30ts75.h"
#include "infrared.h"
#include "sleep.h"
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

    // Keep track of the last converted temperature
    int8_t last_temp_c = -1;
    uint8_t wakes_since_last_transmit = 0;

    // The main loop
    // We'll wake up every T_WAKE, take a temperature reading, and send it if either:
    //   - It's been T_TRANSMIT seconds since the last transmission
    //   - The temperature has changed by more than TX_DELTA_C since the last transmission
    while (1)
    {
        // Do a temperature conversion
        int32_t temp_mdeg = at30ts75_convert();

        // Convert to ºC
        int8_t temp_c = temp_mdeg / 1000;

        // If the temperature has changed by more than TX_DELTA_C, or it's been T_TRANSMIT seconds since the last transmission
        if (
            abs(temp_c - last_temp_c) >= TX_DELTA_C ||
            wakes_since_last_transmit >= (T_TRANSMIT / T_WAKE)
        ) {

            // Send a "Follow Me" message with the current temperature
            ir_midea_follow_me_temp(temp_c);

            // Blink the LED once for the transmission
            io_info_led(true);
            _delay_ms(30);
            io_info_led(false);
            _delay_ms(30);

            // Reset the number of wakes since the last transmit
            wakes_since_last_transmit = 0;
        }

        // Update the last temperature
        last_temp_c = temp_c;

        // Increment the number of wakes since the last transmit
        wakes_since_last_transmit ++;

        // Sleep
        sleep(T_WAKE);
    }

    return 0;
}
