/**
 * Basic USART interface for debugging.
 */

#include "defs.h"
#include "io.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void debug_init(void)
{
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);

    // Enable transmitter
    USART0.CTRLB = USART_TXEN_bm;

    // Set the character size to 8
    USART0.CTRLC |= USART_CHSIZE_8BIT_gc;

    // USART0 will use PA1 for TX and PA2 for RX
    PORTMUX.CTRLB |= PORTMUX_USART0_ALTERNATE_gc;
}

void debug(const char* message)
{
    for (size_t i = 0; i < strlen(message); i ++) {
        while (!(USART0.STATUS & USART_DREIF_bm));
        USART0.TXDATAL = message[i];
    }
}
