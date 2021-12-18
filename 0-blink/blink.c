/**
 * @file blink.c
 * @author Bruce.Lu (lzbgt@icloud.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-19
 * 
 * @copyright hubstack.cn (c) 2021
 * 
 */
#include <avr/io.h>
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

int main(void) {
/**
 * @brief Avoid input tri-state(HI-z) reads undefined value issue
 * 
 * option1: explicity set pull-up mode, to enable PB1 normal HI
 * AVR pull-up resistor is between 20kΩ ~ 50kΩ; 
 * Pin HI voltage level is above 0.7*Vcc and LOW below 0.3*Vcc;
 * Assuming 30kΩ and Vcc=5V,
 * payload should be more than 7/3*30kΩ = 70kΩ (3.5V) or less than 3/7*30kΩ = 13Ω (1.5V),
 * Pin level is undefined when payload resistor is between 13Ω ~ 70 kΩ.
 */
#ifdef DEMO_OPT_PULL_UP
    // set PB1 input mode,
    DDRB &= ~_BV(DDB1);
    // set PB1 pull-up
    PORTB |= _BV(PB1);
#else
    /**
     * @brief Avoid input tri-state(HI-z) reads undefined issue
     * 
     * option2: set as output pin, and enable PB1 normal LOW
     */
    DDRB |= _BV(DDB1);
    PORTB &= ~_BV(PB1);
#endif

    // set PB5(LED) output mode
    DDRB |= _BV(DDB5);

    while (1) {
        // read PB1
        uint8_t v = PINB & _BV(PB1);

        // set LED on if PB1 reads HIGH
        if (v) {
            LED_ON;
        } else {
            // otherwise toggle LED
            LED_TOGGLE;
        }

        _delay_ms(500);
    }
}