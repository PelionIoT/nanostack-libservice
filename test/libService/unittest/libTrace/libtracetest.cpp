/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
//CppUTest includes should be after your and system includes
#include "CppUTest/TestHarness.h"
#include "test_libtrace.h"

TEST_GROUP(LibTrace)
{
    Test_LibTrace *lib_trace;

    void setup() {
        lib_trace = new Test_LibTrace();
    }

    void teardown() {
        delete lib_trace;
    }
};

TEST(LibTrace, Create)
{
    CHECK(lib_trace != NULL);
}

TEST(LibTrace, test_libTrace_tracef)
{
    lib_trace->test_libTrace_tracef();
}

TEST(LibTrace, test_libTrace_trace_ipv6_prefix)
{
    lib_trace->test_libTrace_trace_ipv6_prefix();
}
