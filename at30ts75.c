/**
 * Basic driver for the AT30TS75 temperature sensor.
 */

#include "defs.h"
#include <avr/io.h>
#include "at30ts75.h"

#define SLAVE_ADDRESS (0b1001000 << 1)

/**
 * Perform initialisation of the TWI and the AT30TS75.
 */
void at30ts75_init(void)
{
    // Enable the TWI bus for master use
    TWI0.MCTRLA = TWI_ENABLE_bm;

    // Enable NACKs
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc;

    /**
     * Set the baud rate.
     * This is defined by:
     *
     *      baud = fClkPer / (2*fSCL) - (5 + ((fClkPer * tR) / 2))
     * 
     * Where:
     * 
     *    - fClkPer is the Peripheral Clock (same rate as main clock)
     *    - fSCL is the I2C clock frequency (target 10kHz here)
     *    - tR is the maximum rise/fall time of SDA/SCL (1000ns for <=100kHz SCL)
     */
    TWI0.MBAUD = 242;

    // TODO: Verify whether we need to set up PORTMUX to use an alternative TWI pin set
}

/**
 * Perform a temperature conversion, reading the value  
 */
int at30ts75_convert(void)
{
    return 0;
}