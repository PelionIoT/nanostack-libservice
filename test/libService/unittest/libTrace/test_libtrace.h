/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TEST_LIB_TRACE_H
#define TEST_LIB_TRACE_H

#include "ns_trace.h"

class Test_LibTrace
{
public:
    Test_LibTrace();

    virtual ~Test_LibTrace();

    void test_libTrace_tracef();

    void test_libTrace_trace_ipv6_prefix();
};

#endif // TEST_LIB_TRACE_H
