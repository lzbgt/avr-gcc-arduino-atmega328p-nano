#include <avr/io.h>
#include <string.h>
#include "uart.h"
#include <stdlib.h>
#include <stdarg.h>
#include <avr/delay.h>

typedef int (*fn)(void *arg, ...);
typedef struct CMD {
    const char *cmd;
    const uint8_t len;
    const uint8_t params;
    // 0x01: uint8_t; 0x02: uint16_t; 0x03: str ending with '\0'; 0x04: uint32_t; 0x05: undefined...
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
    va_end(argptr);
    return sbi((uint8_t *)((uint8_t)arg), bit);
}


void cbi_wrap(void *arg, ...)
{
    va_list argptr;
    va_start(argptr, arg);
    int bit = va_arg(argptr, int);
    va_end(argptr);
    return cbi((uint8_t *)((uint8_t)arg), bit);
}

int toggle_led(void *arg)
{
    (void)arg;
    DDRB |= _BV(PB5);
    PINB |= _BV(PB5);
    return PORTB & _BV(PB5);
}

int32_t str2i32(char *p)
{
    int len = strlen(p);
    int32_t r = 0;
    for (int i = len-1; i >=0; i--) {
        int j = i;
        int t = p[len - 1 - i] - '0';
        while (j-- > 0) {
            t = t * 10;
        }
        r += t;
    }
    return r;
}

int calc(void *arg, ...)
{
    char *lhs = (char *)arg;
    va_list argptr;
    va_start(argptr, arg);
    char *op = va_arg(argptr, int);
    char *rhs = va_arg(argptr, int);
    va_end(argptr);
    int32_t l = str2i32(lhs);
    int32_t r = str2i32(rhs);
    switch(*op) {
    case '+':
        return l + r;
    case '-':
        return l - r;
    case 'x':
    case '*':
        return l * r;
    case '/':
        return l / r;
    case '^':
        return l ^ r;
    default:
        return -1;
    }
}

int help(void*arg)
{
    (void)arg;
    printf("\n\rsbi addr bit\n\rcbi addr bit\n\rcalc lhs op rhs\n\r>");
    return 0;
}

static const CMD cmds[] = {
    {"cbi", 3, 2, "\x01\x01", cbi_wrap},
    {"sbi", 3, 2, "\x01\x01", sbi_wrap},
    {"led", 3, 0, "", toggle_led},
    {"calc", 4, 3, "\x03\x03\x03", calc},
    {"?", 1, 0, "", help},
    {"h", 1, 0, "", help}
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

uint16_t str2u16(char* str)
{
    uint16_t r = (str2byte(str) << 8)|str2byte(str+2);
    return r;
}

int parse(char * str)
{
    char *ptr = str;
    char *ptr2 = 0;
    int r = 0;
    for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        if(memcmp(str, cmds[i].cmd, cmds[i].len) == 0) {
            ptr = str + cmds[i].len;
            uint16_t *vals = (uint16_t*)malloc(cmds[i].params * 2);
            for (int j = 0; j < cmds[i].params; j++) {
                while (*ptr == ' ')
                    ptr++;
                ptr2 = ptr;
                while (*ptr != ' ')
                    ptr++;
                *ptr = 0; // replace ' ' with 0 to terminate possible string args
                ptr++;

                switch (cmds[i].types[j]) {
                case 0x01:
                    vals[j] = str2byte(ptr2);
                    break;
                case 0x02:
                    vals[j] = str2u16(ptr2);
                case 0x03:
                    vals[j] = ptr2;
                }
            }

            switch (cmds[i].params) {
            case 0:
                r = cmds[i].function((void*)0);
                break;
            case 1:
                r = cmds[i].function((void*)vals[0]);
                break;
            case 2:
                r = cmds[i].function((void*)vals[0], vals[1]);
                break;
            case 3:
                r = cmds[i].function((void*)vals[0], vals[1], vals[2]);
                break;
            case 4:
                r = cmds[i].function((void*)vals[0], vals[1], vals[2], vals[3]);
                break;

            default:
                break;
            }
            free(vals);
            break;
        }
    }
    return r;
}