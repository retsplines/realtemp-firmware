#include "defs.h"
#include "sleep.h"
#include "io.h"
#include "avr/io.h"
#include "avr/interrupt.h"

/**
 * Sleep for secs seconds.
 * If 0 is passed, the device will sleep indefinitely (I.e. until reset) 
 */
void sleep(uint16_t secs)
{
    // Set all pins to inputs
    io_sleep();

    SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | SLPCTRL_SEN_bm;

    // Sleeping forever?
    if (secs == 0)
    {
        // Disable the RTC & interrupts then sleep
        RTC.CTRLA = 0;
        cli();
        asm("sleep");
        return;
    }

    // Configure the 32k oscillator
    CLKCTRL.OSC32KCTRLA = CLKCTRL_ENABLE_bm | CLKCTRL_RUNSTDBY_bm;

    // Clock source 1.024kHz from internal 32.768kHz oscillator
    // This is the slowest clock source available
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;

    // Set the period to the number of seconds we want to sleep
    RTC.PER = secs;

    // Enable the overflow interrupt
    RTC.INTCTRL |= RTC_OVF_bm;

    // Div-1024 prescaling (so we're counting seconds), Enabled, Running in Standby
    RTC.CTRLA = RTC_PRESCALER_DIV1024_gc | RTC_RTCEN_bm | RTC_RUNSTDBY_bm;

    // Set the RTC to 0
    RTC.CNT = 0;

    sei();
    asm("sleep");

    // On wake, disable the RTC
    RTC.CTRLA = 0;

    // Re-initialise pins
    io_setup();
}

// RTC overflow interrupt
ISR(RTC_CNT_vect)
{
    // Disable the RTC
    RTC.CTRLA = 0;

    // Clear the interrupt flag
    RTC.INTFLAGS = RTC_OVF_bm;

    // Disable interrupts
    cli();
}
