#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <util/delay.h>

volatile uint8_t flag = 0;

#define LED_OFF PORTB &= ~_BV(PORTB5)
#define LED_ON PORTB |= _BV(PORTB5)
#define LED_TOGGLE PINB |= _BV(PINB5)
#define INT_LOW !(PINB & _BV(PB1))

void toggle_led() {
    // on -> off
    if (PINB & _BV(PB5)) {
        LED_OFF;
    } else {
        // off -> on
        LED_ON;
    }
    //
}

ISR(PCINT0_vect) {
    if (INT_LOW) {
        LED_ON;
    } else {
        LED_OFF;
    }
}

#define MS_DELAY 300

//const int data[] PROGMEM = {1, 2, 3, 4};

int main(void) {
    // set PB1, PCINT1 input mode,
    DDRB &= ~_BV(DDB1);
    DDRB |= _BV(DDB5);

    // use Pin Change Interrupt Request 1: PCINT1 - Pin13/PB1/PCINT1 input mode only
    // PCINT1 belongs to control bank PCIE0 (0:7) and mask bank PCMSK0
    PCMSK0 |= _BV(PCINT1);
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
        //_delay_ms(MS_DELAY);
    }
}