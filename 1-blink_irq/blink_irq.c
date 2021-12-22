/**
 * @file blink_irq.c
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
#include <avr/sleep.h>  ///< Power Redution
#include <util/delay.h>

#define LED_OFF PORTB &= ~_BV(PORTB5)
#define LED_ON PORTB |= _BV(PORTB5)

/**
 * @brief Toggle LED pin
 * 
 * writing a logic one to a bit in the PINx Register, 
 * will result in a toggle in the corresponding bit in the Data Register
 */
#define LED_TOGGLE PINB |= _BV(PINB5)

/**
 * @brief Interrupt Service Routing
 * 
 * we use PCINT1(PB1) as Pin Change Interrupt, it belongs to PCINT0_vect (PINTC0:7)
 * @return * Interrupt 
 */
ISR(PCINT0_vect) {
    //< remember PB1 configured as input pin with pull-up, which is normal HI,
    //< so we dectect PIN changed to LOW
    if (!(PINB & _BV(PB1))) {
        // PB1 reads LOW, LED on
        LED_ON;
    } else {
        // reads HI
        LED_OFF;
    }

    //< We should not use delay in ISR, but here this is used to test if ISR is stable or
    //< fluctuating which is indicated by LED flashing,
    //< because attaching PB1 to VCC should not generate Pin Change IRQ.
    //< If we leave PB1 as input in Tri-state, it may read undefined value when floating-connected,
    //< and ISR may fluctuating.
    _delay_ms(100);
}

int main(void) {
    // Explicity set PB1 input pull-up mode, to enable PB1 normal HI
    // avoid input tri-state(HI-z) effect
    DDRB &= ~_BV(DDB1);
    PORTB |= _BV(PB1);

    // PCINT1(PB1) belongs to PCINT0:7, PCMSK bank 0 and PCICR bank 0
    PCMSK0 |= _BV(PCINT1);
    PCICR |= _BV(PCIE0);

    // set PB5(LED) output mode
    DDRB |= _BV(DDB5);

    // enable power saving
    PRR = 0xFF;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_bod_disable();
    // enable ISR
    sei();

    while (1) {
        sleep_cpu();
        sleep_disable();
        sei();
    }
}