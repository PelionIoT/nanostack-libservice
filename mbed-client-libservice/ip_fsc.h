/*
 * Copyright (c) 2014 ARM. All rights reserved.
 */
#ifndef _NS_FSC_H
#define _NS_FSC_H

#include "ns_types.h"

#define NEXT_HEADER_TCP     0x06
#define NEXT_HEADER_UDP     0x11
#define NEXT_HEADER_ICMP6   0x3A

extern uint16_t ip_fcf_v(uint_fast8_t count, const ns_iovec_t vec[static count]);
extern uint16_t ipv6_fcf(const uint8_t src_address[static 16], const uint8_t dest_address[static 16],
                         uint16_t data_length, const uint8_t data_ptr[static data_length],  uint8_t next_protocol);

#endif
