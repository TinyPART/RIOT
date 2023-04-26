#include <stdint.h>
#include "unaligned.h"

const char input[] = "test input string";

uint32_t fletcher32(void *ctx)
{
    (void)ctx;
    uint32_t words = sizeof(input)/2;
    const uint16_t *data = (const uint16_t*)(void*)input;

    uint32_t sum1 = 0xffff, sum2 = 0xffff;

    while (words) {
        unsigned tlen = words > 359 ? 359 : words;
        words -= tlen;
        do {
            sum2 += sum1 += unaligned_get_u16(data++);
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return (sum2 << 16) | sum1;
}

