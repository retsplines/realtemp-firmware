/**
 * Basic driver for the AT30TS75 temperature sensor.
 */

#include "defs.h"
#include "io.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "at30ts75.h"

#define SLAVE_ADDRESS ((0b1001000 << 1) & 0xfe)
#define TWI_SCL_FREQ 10000

// Define the addresses of a few registers
#define REG_TEMPERATURE 0x00
#define REG_CONFIGURATION 0x01
#define REG_TLOW 0x02
#define REG_THIGH 0x03

// Define bit positions & bitmasks for the configuration register
#define CONFIG_SHUTDOWN_bp 0
#define CONFIG_SHUTDOWN_bm (0b1 << CONFIG_SHUTDOWN_bp)
#define CONFIG_CMP_INT_MODE_bp 1
#define CONFIG_CMP_INT_MODE_bm (0b1 << CONFIG_CMP_INT_MODE_bp)
#define CONFIG_ALERT_POLARITY_bp 2
#define CONFIG_ALERT_POLARITY_bm (0b1 << CONFIG_ALERT_POLARITY_bp)
#define CONFIG_FAULT_QUEUE_bp 3
#define CONFIG_FAULT_QUEUE_bm (0b11 << CONFIG_FAULT_QUEUE_bp)
#define CONFIG_RESOLUTION_bp 5
#define CONFIG_RESOLUTION_bm (0b11 << CONFIG_RESOLUTION_bp)
#define CONFIG_ONE_SHOT_bp 7
#define CONFIG_ONE_SHOT_bm (0b1 << CONFIG_ONE_SHOT_bp)

// Define values for the resolution field
#define CONFIG_RESOLUTION_9BIT 0b00
#define CONFIG_RESOLUTION_10BIT 0b01
#define CONFIG_RESOLUTION_11BIT 0b10
#define CONFIG_RESOLUTION_12BIT 0b11

#define TWI0_BAUD(F_SCL) ((((float)F_CPU / (float)F_SCL)) - 10)

// Set up a base value for the configuration register
uint8_t config_register  =

    // 12-bit resolution
    (CONFIG_RESOLUTION_12BIT << CONFIG_RESOLUTION_bp) |

    // Shutdown mode
    CONFIG_SHUTDOWN_bm;

// Function prototypes
static void at30ts75_write(uint8_t* data, size_t len);
static void at30ts75_read(uint8_t* data, size_t len);

/**
 * Perform initialisation of the TWI and the AT30TS75.
 */
static void at30ts75_init(void)
{
    /*
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
    TWI0.MBAUD = (uint8_t)TWI0_BAUD(TWI_SCL_FREQ); 

    TWI0.MCTRLA = 1 << TWI_ENABLE_bp
        | 0 << TWI_QCEN_bp
        | 0 << TWI_RIEN_bp
        | 1 << TWI_SMEN_bp
        | TWI_TIMEOUT_DISABLED_gc
        | 0 << TWI_WIEN_bp;

    // Purge MADDR & MDATA
    TWI0.MCTRLB |= TWI_FLUSH_bm;

    // Bus state to IDLE
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

    // Clear both the read and write interrupt flags
    TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);

    // Wait for the bus to be idle
    while (!(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc));

    // Set the pointer register to the configuration register
    uint8_t write_bytes[2] = {REG_CONFIGURATION, config_register};
    at30ts75_write(write_bytes, 2);
}

static void at30ts75_deinit(void)
{
    // Disable TWI
    TWI0.MCTRLA = 0;
}

/**
 * Write bytes to the AT30TS75.
 */
static void at30ts75_write(uint8_t* data, size_t len)
{
    // Start the transaction
    TWI0.MADDR = SLAVE_ADDRESS;

    // Wait for the address to be sent
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    for (size_t i = 0; i < len; i++)
    {
        // Write the data
        TWI0.MDATA = data[i];

        // Wait for the data to be sent
        while (!(TWI0.MSTATUS & TWI_WIF_bm));
    }

    // Signal a stop condition
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

/**
 * Read bytes from the AT30TS75.
 */
static void at30ts75_read(uint8_t* data, size_t len)
{
    // Start the transaction
    TWI0.MADDR = SLAVE_ADDRESS | 1;

    // Read the requested number of bytes
    for (size_t i = 0; i < len; i++)
    {
        // Wait for the data to be received
        while (!(TWI0.MSTATUS & TWI_RIF_bm));

        // Acknowledge the data
        if (i < len - 1)
        {
            TWI0.MCTRLB &= ~TWI_ACKACT_NACK_gc;
        }

        // Read the data
        data[i] = TWI0.MDATA;
    }

    // Stop the transaction, and NACK the last byte
    TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

/**
 * Perform a temperature conversion, reading the value from the AT30TS75.
 * Returns the value in 1000th of degrees Celsius (millidegrees).
 */
int32_t at30ts75_convert(void)
{
    // Initialise
    at30ts75_init();

    // First, write the configuration register to set the one-shot bit
    uint8_t write_bytes[2] = {REG_CONFIGURATION, config_register | CONFIG_ONE_SHOT_bm};
    at30ts75_write(write_bytes, 2);

    // Wait for the conversion to complete (Tconv @ 12bit = 250-300ms)
    _delay_ms(350);
    
    // Switch to the temperature register
    write_bytes[0] = REG_TEMPERATURE;
    at30ts75_write(write_bytes, 1);

    // Read the temperature
    uint8_t temp[2] = {0};
    at30ts75_read(temp, 2);

    // Combine the two bytes into a single value
    // The temperature is a 12-bit value, with the most significant bit being the sign
    // The 4 least significant bits are not used.
    int32_t result = (((temp[0] & 0x7f) << 8) | temp[1]) >> 4;

    // If the temperature is negative, set the sign bit
    if (temp[0] & 0x80) {
        result *= -1;
    }

    // De-initialise
    at30ts75_deinit();

    return (int32_t)(result * 62.5);
}
