/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "CppUTest/TestHarness.h"
#include "ip6string.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

TEST_GROUP(ip6tos)
{
    void setup() {
    }

    void teardown() {
    }
};

TEST(ip6tos, ip6tos_func)
{
    char addr_str[40] = {0};
    char str_len = 0;
    char *expected;

    uint8_t addr[] = { 0x20, 0x01, 0xd, 0xb8, 0,0,0,0,0,1,0,0,0,0,0,1 };
    expected = "2001:db8::1:0:0:1";
    str_len = ip6tos(addr, addr_str);
    CHECK(str_len == 17);
    STRCMP_EQUAL(expected, addr_str);

    str_len = 0;
    memset(addr, 0x88, 16);
    expected = "8888:8888:8888:8888:8888:8888:8888:8888";
    str_len = ip6tos(addr, addr_str);
    CHECK(str_len == 39);
    STRCMP_EQUAL(expected, addr_str);
}

TEST(ip6tos, ip6_prefix_tos_func)
{
    char prefix_str[45] = {0};
    char str_len = 0;
    char *expected;

    uint8_t prefix[] = { 0x14, 0x6e, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int prefix_len = 64;
    expected = "146e:a00::/64";
    str_len = ip6_prefix_tos(prefix, prefix_len, prefix_str);
    CHECK(str_len == 13);
    STRCMP_EQUAL(expected, prefix_str);

    memset(prefix_str, 0, 45);
    str_len = 0;
    expected = "::/0";
    str_len = ip6_prefix_tos(NULL, 0, prefix_str);
    CHECK(str_len == 4);
    STRCMP_EQUAL(expected, prefix_str);

    str_len = 0;
    uint8_t prefix_2[16];
    memset(prefix_2, 0x88, 16);
    expected = "8888:8888:8888:8888:8888:8888:8888:8888/128";
    str_len = ip6_prefix_tos(prefix_2, 128, prefix_str);
    CHECK(str_len == 43);
    STRCMP_EQUAL(expected, prefix_str);

    memset(prefix_str, 0, 45);
    str_len = ip6_prefix_tos(prefix, 130, prefix_str);
    CHECK(str_len == 0);
}
