/*
 * Copyright (c) 2014 ARM. All rights reserved.
 */
#include <stdio.h>
#include <inttypes.h>
#include "ip6string.h"

/**
 * Print binary IPv6 address to a string.
 * String must contain enough room for full address, 40 bytes exact.
 * IPv4 tunneling addresses are not covered.
 * \param addr IPv6 address.
 * \p buffer to write string to.
 */
void ip6tos(const void *ip6addr, char *p)
{
    uint_fast8_t zero_start = 255, zero_len = 1;
    const uint8_t *addr = ip6addr;
    uint_fast16_t part;

    /* Follow RFC 5952 - pre-scan for longest run of zeros */
    for (uint_fast8_t n = 0; n < 8; n++) {
        part = *addr++;
        part = (part << 8) | *addr++;
        if (part != 0) {
            continue;
        }

        /* We're at the start of a run of zeros - scan to non-zero (or end) */
        uint_fast8_t n0 = n;
        for (n = n0 + 1; n < 8; n++) {
            part = *addr++;
            part = (part << 8) | *addr++;
            if (part != 0) {
                break;
            }
        }

        /* Now n0->initial zero of run, n->after final zero in run. Is this the
         * longest run yet? If equal, we stick with the previous one - RFC 5952
         * S4.2.3. Note that zero_len being initialised to 1 stops us
         * shortening a 1-part run (S4.2.2.)
         */
        if (n - n0 > zero_len) {
            zero_start = n0;
            zero_len = n - n0;
        }

        /* Continue scan for initial zeros from part n+1 - we've already
         * consumed part n, and know it's non-zero. */
    }

    /* Now go back and print, jumping over any zero run */
    addr = ip6addr;
    for (uint_fast8_t n = 0; n < 8;) {
        if (n == zero_start) {
            if (n == 0) {
                *p++ = ':';
            }
            *p++ = ':';
            addr += 2 * zero_len;
            n += zero_len;
            continue;
        }

        part = *addr++;
        part = (part << 8) | *addr++;
        n++;

        p += sprintf(p, "%"PRIxFAST16, part);

        /* One iteration writes "part:" rather than ":part", and has the
         * explicit check for n == 8 below, to allow easy extension for
         * IPv4-in-IPv6-type addresses ("xxxx::xxxx:a.b.c.d"): we'd just
         * run the same loop for 6 parts, and output would then finish with the
         * required : or ::, ready for "a.b.c.d" to be tacked on.
         */
        if (n != 8) {
            *p++ = ':';
        }
    }
    *p++ = '\0';
}
