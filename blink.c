#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <util/delay.h>

volatile uint8_t flag = 0;

void toggle_led() {
    // on -> off
    if (PORTB & _BV(PORTB5)) {
        PORTB &= ~_BV(PORTB5);
    } else {
        // off -> on
        PORTB |= _BV(PORTB5);
    }
}

ISR(PCINT0_vect) {
    // PINC reads high, flag get 1, otherwise flag get 0
    // if (PINC & _BV(PC1)) flag = 1 - flag;
    flag = 1 - flag;

    toggle_led();
}

#define MS_DELAY 300

//const int data[] PROGMEM = {1, 2, 3, 4};

int main(void) {
    DDRB = 0;   // all PB0 - PB7 defaults to input only
    PORTB = 1;  // all PB0 -PB7 initial HIGH
    // set LED PB5 output mode
    DDRB |= _BV(DDB5);

    // set PB1, PCINT1 input mode,
    // DDRB1 = 0 and PORTB1=1, so PB1 is pulled-up
    DDRB &= ~_BV(DDB1);

    // use Pin Change Interrupt Request 1: PCINT1 - Pin13/PB1/PCINT1 input mode only
    // PCINT1 belongs to control bank PCIE0 (0:7) and mask bank PCMSK0
    PCIFR = (1 << PCIF0);
    PCMSK0 = 0;
    PCMSK0 |= _BV(PCINT1);
    PCICR = 0;
    // enables pcint[0:7]
    PCICR |= _BV(PCIE0);

    // Turn interrupts on.
    sei();
    while (1) {
        // int x = pgm_read_byte(data);

        // // This turns interrupts off for the code inside it.  Probably
        // // not needed here but it's good to know about.
        // ATOMIC_BLOCK(ATOMIC_FORCEON) {

        // }

        // /*Wait 300 ms */
        _delay_ms(MS_DELAY);
    }
}