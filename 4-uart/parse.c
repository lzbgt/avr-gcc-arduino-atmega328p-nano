#include <avr/io.h>
#include <string.h>
#include "uart.h"

typedef void (*fn)(uint8_t *addr, uint8_t val);
typedef struct CMD {
    const char *cmd;
    const uint8_t len;
    const uint8_t params;
    const uint8_t *types;
    fn function;
} CMD;

void sbi(uint8_t *addr, uint8_t b)
{
    uint8_t a;

    a = *addr;
    a |= 1 << (b & 0x7);
    *addr = a;
}


void cbi(uint8_t *addr, uint8_t b)
{
    uint8_t a;

    a = *addr;
    a &= ~(1 << (b & 0x7));
    *addr = a;
}

static const CMD cmds[] = {
    {"cbi", 3, 2, "\x01\x01", cbi},
    {"sbi", 3, 2, "\x01\x01", sbi}
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

void parse(char * str)
{
    char *ptr = str;
    char *ptr2 = 0;
    for (int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        if(memcmp(str, cmds[i].cmd, cmds[i].len) == 0) {
            ptr = str + cmds[i].len;
            uint8_t val1, val2;

            while(*ptr == ' ')
                ptr++;
            ptr2 = ptr;
            while(*ptr != ' ')
                ptr++;

            val1 = str2byte(ptr2);

            while(*ptr == ' ')
                ptr++;
            ptr2 = ptr;
            val2 = str2byte(ptr2);
            cmds[i].function((uint8_t*)val1, val2);
            break;
        }
    }
}