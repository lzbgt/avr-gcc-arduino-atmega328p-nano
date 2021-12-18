/**
 * @file timer.c
 * @author Bruce.Lu (lzbgt@icloud.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-19
 * 
 * @copyright hubstack.cn (c) 2021
 * 
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define LED_OFF PORTB &= ~_BV(PORTB5)
#define LED_ON PORTB |= _BV(PORTB5)

/*writing a logic one to a bit in the PINx Register, 
will result in a toggle in the corresponding bit in the Data Register
*/
#define LED_TOGGLE PINB |= _BV(PINB5)

// Interrupt Service Routing
// we use PCINT1(PB1) as Pin Change Interrupt, it belongs to vect0 (PINTC[0:7])
ISR(PCINT0_vect) {
    // remember input pin with pull-up is normal HI,
    // so we dectect PIN changed to LOW
    if (!(PINB & _BV(PB1))) {
        // PB1 reads LOW, LED on
        LED_ON;
    } else {
        // reads HI
        LED_OFF;
    }
}

int main(void) {
    // explicity set PB1 input pull-up mode, to enable PB1 normal HI
    // avoid input tri-state(HI-z) effect
    DDRB &= ~_BV(DDB1);
    PORTB |= _BV(PB1);

    // PCINT1(PB1) belongs to PCINT[0:7] and thus MASK bank 0 and ICR bank 0
    PCMSK0 |= _BV(PCINT1);
    PCICR |= _BV(PCIE0);

    // set PB5(LED) output mode
    DDRB |= _BV(DDB5);

    // enable ISR
    sei();

    while (1) {
        // enable power saving
        PRR = 0xFF;
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_bod_disable();
        sei();
        sleep_cpu();
        sleep_disable();
    }
}