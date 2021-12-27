#include <avr/io.h>
#include <string.h>
#include "uart.h"
#include <stdlib.h>
#include <stdarg.h>

typedef int (*fn)(void *arg, ...);
typedef struct CMD {
    const char *cmd;
    const uint8_t len;
    const uint8_t params;
    const uint8_t *types;
    fn function;
} CMD;


int sbi(uint8_t *addr, uint8_t b)
{
    uint8_t a;

    a = *addr;
    a |= 1 << (b & 0x7);
    *addr = a;

    return 0;
}

int cbi(uint8_t *addr, uint8_t b)
{
    uint8_t a;

    a = *addr;
    a &= ~(1 << (b & 0x7));
    *addr = a;

    return 0;
}
void sbi_wrap(void *arg, ...)
{
    va_list argptr;
    va_start(argptr, arg);
    int bit = va_arg(argptr, int);
    return sbi((uint8_t *)((uint8_t)arg), bit);
}


void cbi_wrap(void *arg, ...)
{
    va_list argptr;
    va_start(argptr, arg);
    int bit = va_arg(argptr, int);
    return cbi((uint8_t *)((uint8_t)arg), bit);
}

int toggle_led(void *arg)
{
    (void)arg;
    DDRB |= _BV(PB5);
    PINB |= _BV(PB5);
    return PORTB & _BV(PB5);
}

static const CMD cmds[] = {
    {"cbi", 3, 2, "\x01\x01", cbi_wrap},
    {"sbi", 3, 2, "\x01\x01", sbi_wrap},
    {"led", 3, 0, "", toggle_led}
};


uint8_t hex2bin(char *str)
{
    uint8_t hex;
    if ((*str) >= 'a' && (*str) <= 'F') {
        hex = (*str - 'a' + 10);
    }
    else if(*str >= 'A' && *str <= 'F') {
        hex = (*str - 'A' + 10);
    }
    else if(*str >= '0' && *str <='9') {
        hex = *str - '0';
    }
    else {
        hex = 0;
    }
    return hex;
}

uint8_t str2byte(char* str)
{
    uint8_t H, L;
    H = hex2bin(str) * 16;
    L = hex2bin(str + 1);
    return H + L;
}

int parse(char * str)
{
    char *ptr = str;
    char *ptr2 = 0;
    int r = 0;
    for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        if(memcmp(str, cmds[i].cmd, cmds[i].len) == 0) {
            ptr = str + cmds[i].len;
            uint8_t *vals = malloc(cmds[i].params);
            for (int i = 0; i < cmds[i].params; i++) {
                while (*ptr == ' ')
                    ptr++;
                ptr2 = ptr;
                vals[i] = str2byte(ptr2);
                while (*ptr != ' ')
                    ptr++;
            }
            switch (cmds[i].params) {
            case 0:
                r = cmds[i].function((void*)0);
                break;
            case 1:
                r = cmds[i].function((uint8_t*)vals[0]);
                break;
            case 2:
                r = cmds[i].function((uint8_t*)vals[0], vals[1]);
                break;
            case 3:
                r = cmds[i].function((uint8_t*)vals[0], vals[1], vals[2]);
                break;

            default:
                break;
            }
            break;
        }
    }
    return r;
}