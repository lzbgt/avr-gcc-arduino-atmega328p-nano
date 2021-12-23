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
#include <avr/sleep.h>

#define LED_TOG PINB |= _BV(PB5)


int8_t TIMER_0_init()
{
    TCCR1B |= (1 << WGM12); //CTC
    TCCR1B |= (1 << CS10) | (1 << CS12); //set the pre-scalar as 1024
    TIFR1 |= 1 << TOV1;
    OCR1A = 1562 * 10; 	   //1000ms delay
    TIMSK1 = 0 << OCIE1B   /* Output Compare B Match Interrupt Enable: disabled */
             | 1 << OCIE1A /* Output Compare A Match Interrupt Enable: disabled */
             | 0 << ICIE1  /* Input Capture Interrupt Enable: disabled */
             | 0 << TOIE1; /* Overflow Interrupt Enable: enabled */
}

ISR(TIMER1_COMPA_vect)
{
    LED_TOG;
}

int main(void)
{
    // // LED output mode
    DDRB |= _BV(PB5);

    TIMER_0_init();

    // enable power saving
    PRR = 0xFF & (~(_BV(PRTIM1) | (_BV(PRTWI))));
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_bod_disable();
    sei();
    while (1) {
        sleep_cpu();
        sleep_disable();
        sei();
    }
}
