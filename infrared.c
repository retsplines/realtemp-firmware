/**
 * The Midea protocol is mostly informed by esphome (components/remote_base/midea_protocol.cpp)
 * @link https://esphome.io/api/midea__protocol_8cpp_source
 */

#include "defs.h"
#include "io.h"
#include "infrared.h"
#include "util.h"
#include <stddef.h>
#include <avr/io.h>

#include <util/delay.h>

#define IR_CARRIER_FREQ 38000
#define IR_CARRIER_PERIOD ((uint16_t)(F_CPU / IR_CARRIER_FREQ))
#define IR_CARRIER_ON_CMP (IR_CARRIER_PERIOD / 2)

#define MIDEA_TICK_US 560
#define MIDEA_OFFSET_CS 5
#define MIDEA_OFFSET_FOLLOW_ME_TEMP 4
#define MIDEA_HEADER_MARK_US (8 * MIDEA_TICK_US)
#define MIDEA_HEADER_SPACE_US (8 * MIDEA_TICK_US)
#define MIDEA_BIT_MARK_US (MIDEA_TICK_US)
#define MIDEA_BIT_ONE_SPACE_US (3 * MIDEA_TICK_US)
#define MIDEA_BIT_ZERO_SPACE_US (MIDEA_TICK_US)
#define MIDEA_FOOTER_MARK_US (MIDEA_TICK_US)
#define MIDEA_FOOTER_SPACE_US (10 * MIDEA_TICK_US)

#define IR_MIDEA_PACKET_LEN 6
#define IR_MIDEA_TYPE_FOLLOW_ME_HEADER 0xA4

static uint8_t ir_midea_calc_checksum(uint8_t* data, size_t len)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < MIDEA_OFFSET_CS; i ++) {
        checksum -= reverse_bits(data[i]);
    }

    return reverse_bits(checksum);
}

static void delay_us(uint16_t us)
{
    _delay_loop_2(us * (F_CPU / 1000000) / 4);
}

static void ir_mark_space(uint16_t mark_us, uint16_t space_us)
{
    TCA0.SINGLE.CMP2 = IR_CARRIER_ON_CMP;
    delay_us(mark_us);
    TCA0.SINGLE.CMP2 = 0; 
    delay_us(space_us);
}

static void ir_send(uint8_t* data, size_t len)
{
    // Set up TCA0's WO2 to generate the 38 kHz carrier frequency
    TCA0.SINGLE.PER = IR_CARRIER_PERIOD;
    TCA0.SINGLE.CMP2 = 0; 
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; 

    // Do all of this twice, the second time inverted
    for (uint8_t invert = 0; invert < 2; invert ++) {

        // Send the header
        ir_mark_space(MIDEA_HEADER_MARK_US, MIDEA_HEADER_SPACE_US);

        // Send the data
        for (size_t i = 0; i < len; i ++) 
        {
            // Send each of the 8 bits
            for (uint8_t mask = 0x80; mask; mask >>= 1) 
            {
                bool bit = (data[i] & mask);
                
                if (invert) {
                    bit = !bit;
                }

                ir_mark_space(MIDEA_BIT_MARK_US, bit ? MIDEA_BIT_ONE_SPACE_US : MIDEA_BIT_ZERO_SPACE_US);
            }
        }

        // Send the footer
        ir_mark_space(MIDEA_FOOTER_MARK_US, MIDEA_FOOTER_SPACE_US);
    }

    // Turn off the waveform generator
    TCA0.SINGLE.CTRLA = 0;
}

void ir_midea_follow_me_temp(uint8_t temp)
{
    // Build the message
    // Since we're only ever sending the "Follow Me" message, we can hardcode the content mostly
    uint8_t data[IR_MIDEA_PACKET_LEN] = {

        // Byte 0
        // ---------------------
        // 0:4 - 10100 - Header
        // 5:7 - 100 - Message type
        IR_MIDEA_TYPE_FOLLOW_ME_HEADER, // 10100 100

        // Byte 1
        // ---------------------
        // 0 - 1 - Power on
        // 1 - 0 - Sleep off
        // 2 - 0 - (Unknown)
        // 3:5 - 00 - Fan speed (0 = Auto, 1 = Low, 2 = Medium, 3 = High)
        // 6:7 - 10 - Mode (0 = Cool, 1 = Dry, 2 = Auto, 3 = Heat, 4 = Fan)
        0x82, // 1 0 0 0 00 10

        // Byte 2
        // ---------------------
        // 0:2 - 01 - Not sure
        // 2:3 - 0 - Farenheit mode
        // 3:7 - 10010 - Desired temperature in ºC above the minimum (17ºC or 62ºF), so 25ºC here
        0x48, // 01 0 01000

        // Byte 3
        // ---------------------
        // 0:1 - 0 - Beeper (off)
        // 1:6 - 111111 - Number of half-hours until auto-off, 0b111111 = disable this feature
        // 6:7 - 1 - Unknown, always 1 
        0x7F, // 0 111111 1

        // Byte 4
        // ---------------------
        // 0:7 - 00011111 - (31 = 30ºC) 'Follow Me' temperature in ºC + 1
        // (This is the only byte we're interested in)
        // Allegedly the MSB disables the 'Follow Me' feature but we won't ever set it if we limit the temperature to 30ºC
        (temp + 1),

        // Byte 5 (Checksum)
        // ---------------------
        0x00
    };

    // Calculate the checksum
    data[MIDEA_OFFSET_CS] = ir_midea_calc_checksum(data, IR_MIDEA_PACKET_LEN - 1);
    
    ir_send(data, IR_MIDEA_PACKET_LEN);
}