#ifndef IPV6_TEST_VALUES_H
#define IPV6_TEST_VALUES_H

#include <stdint.h>

struct ip6_addresses_and_its_binary_form_t {
    char *addr;
    uint8_t bin[16];
} ipv6_test_values[] = {
    { "2001:db8::1:0:0:1",                      { 0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 }},
    { "2001:db8:aaaa:bbbb:cccc:dddd:eeee:1",    { 0x20, 0x01, 0xd, 0xb8, 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0x00, 0x01 }},
    { "2001:db8::1",                            { 0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }},
    { "2001:db8::2:1",                          { 0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1 }},
    { "2001:db8:aaaa:bbbb:cccc:dddd:0:1",       { 0x20, 0x01, 0xd, 0xb8, 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0, 0, 0x00, 0x01 }},
    { "2001:db8::aaaa:0:0:1",                   { 0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, 0xaa, 0xaa, 0, 0, 0, 0, 0, 1 }},
    { "2001:0:0:1::1",                          { 0x20, 0x01, 0, 0 , 0, 0 , 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 }},
    { "2001:0:0:1::",                           { 0x20, 0x01, 0, 0 , 0, 0 , 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }},
    { "2001:db8::",                             { 0x20, 0x01, 0xd, 0xb8 }},
    { "::aaaa:0:0:1",                           { 0, 0, 0, 0, 0, 0, 0, 0, 0xaa, 0xaa, 0, 0, 0, 0, 0, 1 }},
    { "::1",                                    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }},
    { "::",                                     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }},
    { "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF", { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}},
    {NULL, {0}}
};

#endif /* IPV6_TEST_VALUES_H */
