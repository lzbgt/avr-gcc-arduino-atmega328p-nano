/**
 * @file timer.c
 * @author Bruce.Lu (lzbgt@icloud.com)
 * @brief timer normal mode to toggle LED. NOTE: DON'T do this in production, it's NOT CPU efficient!
 * 
 * As the application note 'atmel-2505' states, timer can be configurated to toggle GPIO pin directly
 * 
 * ldi r16, (1<<COM2A1)|(1<<WGM21)|(1<<WGM20)
 * sts TCCR2A,r16 ; OC2A toggling on compare match/timer
 * ; Clock = system clock
 * ldi r16,32
 * sts OCR2A,r16 ; Set output compare value to 32
 * 
 * TOV = Fck/(Prescaler * (2^(res-1))) = Fck/p/255/2 => 30Hz, assuming the max prescaler is used (1024) and 16MHz system clock,
 * the longest piroid is about 33ms which is too short for LED blinking, thus we need to accumuate TOVs in ISR to devide futher.
 * 
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

uint8_t freq = F_CPU / 1024 / 256 / 2;  // 30

// NOTE: the best resolution is 30Hz, about 33ms, so ms can't be less than 33
uint16_t calc_scalar(uint16_t ms) {
    if (ms < 33) {
        ms = 33;
    }
    return ms * freq / 1000;
}

volatile uint16_t scaler_ref = 33;
volatile uint16_t scaler = 33;  //< holds up max period of 2184 seconds
                                //< uint8_t can only hold up for 8 seconds.
// Interrupt Service Routing
ISR(TIMER0_OVF_vect) {
    if (--scaler == 0) {
        scaler = scaler_ref;
        LED_TOGGLE;
    }
}

int main(void) {
    scaler_ref = calc_scalar(1000);
    scaler = scaler_ref;
    /* Timer clock = I/O clock / 1024 */
    TCCR0B = (1 << CS02) | (1 << CS00);
    /* Clear overflow flag */
    TIFR0 = 1 << TOV0;
    /* Enable Overflow Interrupt */
    TIMSK0 = 1 << TOIE0;

    // set PB5(LED) output mode
    DDRB |= _BV(DDB5);

    // enable power saving
    PRR = 0xFF & (~(_BV(PRTIM0) | (_BV(PRTWI))));
    set_sleep_mode(SLEEP_MODE_IDLE);
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