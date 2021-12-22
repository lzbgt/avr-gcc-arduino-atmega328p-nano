/*
 * GccApplication2.c
 *
 * Created: 2021/12/22 0:41:03
 * Author : lzbgt
 */

#define F_CPU 16000000UL
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

int8_t TIMER_0_init() {
    /* Enable TC1 */
    PRR &= ~(1 << PRTIM1);

    TCCR1A = (0 << COM1A1) | (0 << COM1A0)   /* Normal port operation, OCA disconnected */
             | (0 << COM1B1) | (0 << COM1B0) /* Normal port operation, OCB disconnected */
             | (0 << WGM11) | (0 << WGM10);  /* TC16 Mode 0 Normal */

    TCCR1B = (0 << WGM13) | (0 << WGM12)                /* TC16 Mode 0 Normal */
             | 0 << ICNC1                               /* Input Capture Noise Canceler: disabled */
             | 0 << ICES1                               /* Input Capture Edge Select: disabled */
             | (1 << CS12) | (0 << CS11) | (1 << CS10); /* IO clock divided by 1024 */

    // ICR1 = 0x0; /* Input capture value, used as top counter value in some modes: 0x0 */
    // GTCCR = 0 << TSM /* Timer/Counter Synchronization Mode: disabled */
    //		 | 0 << PSRASY /* Prescaler Reset Timer/Counter2: disabled */
    //		 | 0 << PSRSYNC; /* Prescaler Reset: disabled */

    OCR1A = 0xFF;
    OCR1B = 0x0F;
    TIFR1 = 1 << TOV1;
    TIMSK1 = 0 << OCIE1B   /* Output Compare B Match Interrupt Enable: disabled */
             | 0 << OCIE1A /* Output Compare A Match Interrupt Enable: disabled */
             | 0 << ICIE1  /* Input Capture Interrupt Enable: disabled */
             | 1 << TOIE1; /* Overflow Interrupt Enable: enabled */

    return 0;
}

const int freq_PER_1000s = (F_CPU * 1000 / 1024 / 2 / 256 / 256);
#define LED_TOG PINB |= _BV(PB5)
ISR(TIM1_OVF_vect) {
    LED_TOG;
}

int main(void) {
    DDRB |= _BV(PB5);

    TIMER_0_init();
    sei();

    while (1) {
        //LED_TOG;
        _delay_ms(500);
    }
}
