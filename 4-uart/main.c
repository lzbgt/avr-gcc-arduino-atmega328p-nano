

#include "uart.h"
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"


int main()
{
    uartint_init();

    sei();
    printf("hello hubstack.cn");

    printf("\r\n>");
    size_t n = 0;

    char line[256] = {0};
    char c;
    size_t pos = 0;
    while (1) {
        while(uartint_receive(&c))
            ;
        if (c == '\r') {
            uartint_transmitBurst("\n\r>", 3);
            // TODO: emmit line
            uint8_t r1, r2;
            line[pos] = 0;
            char s[256] = {0};
            strcpy(s, line);
            parse(s);
            memset(line, 0, 256);
            pos = 0;
        }
        else {
            line[pos++] = c;
            uartint_transmit(c);
        }
    }

    return 0;
}